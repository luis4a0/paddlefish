// Copyright (c) 2017-2022 Luis Peñaranda. All rights reserved.
//
// This file is part of paddlefish.
//
// Paddlefish is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Paddlefish is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with paddlefish.  If not, see <http://www.gnu.org/licenses/>.

#include <paddlefish/document.h>
#include <paddlefish/flate.h>
#include <paddlefish/custom_object.h>
#include <paddlefish/file_stream.h>
#include <paddlefish/font.h>
#include <paddlefish/color_profile.h>
#include <paddlefish/cid_to_gid.h>

#include <fstream>
#include <iomanip>
#include <sstream>

namespace paddlefish {

Document::Document():
  next_object_number(1),
  first_body_object_number(6),
  total_body_objects(0)
{
  colorspace_properties.emplace(
    COLORSPACE_DEVICERGB,
    ColorspaceProperties(ColorspaceProperties::Type::DEVICE,
                         3,
                         std::string("/DeviceRGB")));
  colorspace_properties.emplace(
    COLORSPACE_DEVICEGRAY,
    ColorspaceProperties(ColorspaceProperties::Type::DEVICE,
                         1,
                         std::string("/DeviceGray")));
  colorspace_properties.emplace(
    COLORSPACE_DEVICECMYK,
    ColorspaceProperties(ColorspaceProperties::Type::DEVICE,
                         4,
                         std::string("/DeviceCMYK")));
  colorspace_properties.emplace(
    COLORSPACE_PATTERN,
    ColorspaceProperties(ColorspaceProperties::Type::PATTERN,
                         0,
                         std::string("/Pattern")));

  ocgs = std::vector<OcgPtr>();
}

void Document::push_back_page(const PagePtr& page_ptr)
{
  page_ptr->set_document(this);

  pages.push_back(page_ptr);

  page_ptr->set_number((unsigned int)pages.size());

  return;
}

unsigned Document::add_standard_type1_font(const std::string& font_name)
{
  // Search in the standard font list if this font was already added.
  // If yes, return its identifier.
  for (unsigned i = 0; i < (unsigned)body_objects.size(); ++i)
  {
    if (body_objects[i]->get_type() == PdfObject::Type::FONT &&
        std::dynamic_pointer_cast<Font>(body_objects[i])->get_font_type() ==
        Font::Type::STANDARD_TYPE_1 &&
        std::dynamic_pointer_cast<Font>(body_objects[i])->get_name() ==
        font_name)
    {
      return i;
    }
  }

  // If the font wasn't added to the document, add it.
  body_objects.push_back(FontPtr(new Font(font_name)));
  ++total_body_objects;

  return first_body_object_number + total_body_objects - 1;
}

unsigned Document::add_truetype_font(const TrueTypeFont* font, bool embed)
{
PADDLEFISH_ONLY_TRUETYPE(
  FontPtr f(new Font(Font::Type::TRUE_TYPE, font,
                     (embed ? EMBEDDED_FONT : NOT_EMBEDDED)));

  unsigned widths_id = add_custom_object(f->get_widths());
  f->set_widths_ref(widths_id);

  if (f->get_embedding() != NOT_EMBEDDED)
  {
    std::string filename = f->get_file_name();

    // First we try to get the file name. If we manage to do it, then
    // we directly set to deflate the file into the output PDF.
    if (filename != "")
    {
      f->set_font_file_ref(add_custom_stream_from_file(filename, "", embed));
    }
    else
    {
      // Otherwise, we try to read the font program and copy it.
      std::string font_program = f->get_font_program();
      if (!font_program.empty())
      {
        f->set_font_file_ref(add_custom_stream(font_program, ""));
      }
    }
  }

  unsigned fd_id = add_custom_object(f->get_font_descriptor());
  f->set_font_descriptor_ref(fd_id);

  body_objects.push_back(f);
  ++total_body_objects;

  return first_body_object_number + total_body_objects - 1;
)
}

std::pair<unsigned, std::string> Document::add_type2_cid_font
  (const TrueTypeFont& font, bool embed, const std::string &name)
{
PADDLEFISH_ONLY_TRUETYPE(
  // TODO
  return std::make_pair(0, std::string());
)
}

unsigned Document::add_type0_font(std::pair<unsigned, std::string> t2_font)
{
  std::string object("<< /Type /Font\n   /Subtype /Type0\n   /BaseFont /");

  object += util::format_name(t2_font.second) +
    "\n   /Encoding /Identity-H\n   /DescendantFonts [" +
    util::to_str(t2_font.first) + " 0 R]\n>>";

  return add_custom_object(object);
}

unsigned Document::add_icc_color_profile(const std::string& filename,
                                         unsigned channels,
                                         bool use_flate)
{
  // Add the stream containing the file.
  std::string extra_header("/N " + util::to_str(channels));
  unsigned stream_id = add_custom_stream_from_file(filename, extra_header, use_flate);

  // Put the object in the vector.
  body_objects.push_back(ColorProfilePtr(new ColorProfile(ColorProfile::Type::ICC, stream_id)));

  // We need two actual objects to specify an ICC-based color profile.
  ++total_body_objects;

  // Set the properties and return the object number.
  ColorspaceProperties::Type cs_type = ColorspaceProperties::Type::ICC_BASED;
  unsigned object_number = first_body_object_number + total_body_objects - 1;
  std::string cs_string = util::to_str(object_number) + " 0 R";
  colorspace_properties.emplace(object_number,
                                ColorspaceProperties(cs_type,
                                                     channels,
                                                     cs_string));

  return object_number;
}

unsigned Document::add_icc_color_profile(const char *buffer,
                                         unsigned length,
                                         unsigned channels,
                                         bool use_flate)
{
  // Add the stream containing the file.
  std::string extra_header("/N " + util::to_str(channels));
  unsigned stream_id = add_custom_stream(buffer,
                                         length,
                                         extra_header.c_str(),
                                         (unsigned int)extra_header.length(),
                                         use_flate);

  // Put the object in the vector.
  body_objects.push_back(ColorProfilePtr(new ColorProfile(ColorProfile::Type::ICC, stream_id)));

  // We need two actual objects to specify an ICC-based color profile.
  ++total_body_objects;

  // Set the properties and return the object number.
  ColorspaceProperties::Type cs_type = ColorspaceProperties::Type::ICC_BASED;
  unsigned object_number = first_body_object_number + total_body_objects - 1;
  std::string cs_string = util::to_str(object_number) + " 0 R";
  colorspace_properties.emplace(object_number,
                                ColorspaceProperties(cs_type,
                                                     channels,
                                                     cs_string));

  return object_number;
}

unsigned Document::add_calrgb_color_profile(double *white_point,
                                            double *black_point,
                                            double *gamma,
                                            double *matrix)
{
  body_objects.push_back(ColorProfilePtr(new ColorProfile(ColorProfile::Type::CALRGB,
                                         white_point,
                                         black_point,
                                         gamma,
                                         matrix)));

  ++total_body_objects;

  // Set the properties and return the object number.
  ColorspaceProperties::Type cs_type = ColorspaceProperties::Type::CALRGB;
  unsigned object_number = first_body_object_number + total_body_objects - 1;
  std::string cs_string = util::to_str(object_number) + " 0 R";
  colorspace_properties.emplace(object_number,
                                ColorspaceProperties(cs_type, 3, cs_string));

  return object_number;
}

unsigned Document::add_calgray_color_profile(double *white_point,
                                            double *black_point,
                                            double *gamma)
{
  body_objects.push_back(
    ColorProfilePtr(new ColorProfile(ColorProfile::Type::CALGRAY,
                                     white_point,
                                     black_point,
                                     gamma)));

  ++total_body_objects;

  // Set the properties and return the object number.
  ColorspaceProperties::Type cs_type = ColorspaceProperties::Type::CALGRAY;
  unsigned object_number = first_body_object_number + total_body_objects - 1;
  std::string cs_string = util::to_str(object_number) + " 0 R";
  colorspace_properties.emplace(object_number,
                                ColorspaceProperties(cs_type, 1, cs_string));

  return object_number;
}

unsigned Document::add_indexed_color_profile(unsigned base,
                                             unsigned char **colors,
                                             unsigned num_colors,
                                             unsigned base_channels)
{
  body_objects.push_back(ColorProfilePtr(new ColorProfile(ColorProfile::Type::INDEXED,
                                         base,
                                         colors,
                                         num_colors,
                                         base_channels)));

  ++total_body_objects;

  // Set the properties and return the object number.
  ColorspaceProperties::Type cs_type = ColorspaceProperties::Type::INDEXED;
  unsigned object_number = first_body_object_number + total_body_objects - 1;
  std::string cs_string = util::to_str(object_number) + " 0 R";
  colorspace_properties.emplace(object_number,
                                ColorspaceProperties(cs_type, 1, cs_string));

  return object_number;
}

unsigned Document::add_separation_color_space(std::string name,
                                              unsigned alternate_cs,
                                              unsigned alternate_function)
{
  body_objects.push_back(ColorProfilePtr(new ColorProfile(ColorProfile::Type::SEPARATION,
                                         name,
                                         alternate_cs,
                                         alternate_function)));

  ++total_body_objects;

  // Set the properties and return the object number.
  ColorspaceProperties::Type cs_type = ColorspaceProperties::Type::SEPARATION;
  unsigned object_number = first_body_object_number + total_body_objects - 1;
  std::string cs_string = util::to_str(object_number) + " 0 R";
  colorspace_properties.emplace(object_number,
                                ColorspaceProperties(cs_type, 1, cs_string));

  return object_number;
}

std::ostream& Document::to_stream(std::ostream &out_stream)
{
  // We start the objects at number 6.
  next_object_number = 6;

  // Compute the start stream position. This is useful to compute
  // the byte offset for the PDF trailer.
  start_stream_position = out_stream.tellp();

  // Save space to write the body objects in the document before the pages.
  first_body_object_number = next_object_number;
  next_object_number += total_body_objects;

  // Assign numbers to the objects in the pages of the document.
  for (auto const &p: pages)
  {
    next_object_number = p->set_object_number(next_object_number);
    add_ocg_list(p->get_ocgs());
  }

  // Write to the stream the four components of the document.
  write_header(out_stream);
  write_objects(out_stream);
  write_xref(out_stream);
  write_trailer(out_stream);

  return out_stream;
}

std::ostream& Document::write_header(std::ostream &out_stream)
{
  // Header. The second line is a comment containing four characters with
  // code greater than 127, for the eventual reader to realize that the file
  // is not text.
  out_stream << "%PDF-1.4\n%" << 
    (char)128 << (char)128 << (char)128 << (char)128 << '\n';

  // Comments.
  for (auto const &c: document_comments)
  {
    out_stream << "% " << c << '\n';
  }

  // Object 1 is the document catalog. Before writing the bytes
  // corresponding to the object 1, we compute its offset to the
  // beginning of the file. We compute offsets for each output object.
  object_offsets.push_back(out_stream.tellp());
  out_stream << "1 0 obj\n<< /Type /Catalog\n   /Outlines 2 0 R\n";
  out_stream << "   /Pages 3 0 R\n   /PageLabels 4 0 R\n";

  // Print OCG information on the catalog if needed.
  if (0 < ocgs.size())
  {
    out_stream << "   /OCProperties << /OCGs [ ";
    for (size_t iocg = 0; iocg < ocgs.size(); ++iocg)
    {
      out_stream << ocgs[iocg]->object_number << " 0 R ";
    }
    out_stream << "]\n                    /D << /Name (Default)\n" <<
      "                          /Order";
    unsigned nesting_level, last_nesting_level = 0;
    for (size_t iocg = 0; iocg < ocgs.size(); ++iocg)
    {
      nesting_level = ocgs[iocg]->nesting_level;
      // If the nesting level of the current layer is the same as the last
      // one, append the layer to the array. If it is bigger, create a new
      // array at the end and add the current layer. If it is smaller, then
      // close the last array and add a new element.
      if (nesting_level > last_nesting_level)
      {
        // If the nesting level is bigger than the last used level, then
        // it is strictly one higher. Append an array containing the new
        // group to the end of the proper array.
        out_stream << " [" ;
      }
      else
      {
        if (nesting_level < last_nesting_level)
        {
          // If the nesting level is smaller, then close the corresponding
          // amount of brackets.
          for (; nesting_level < last_nesting_level; --last_nesting_level)
          {
            out_stream << " ]";
          }
        }
      }
      last_nesting_level = nesting_level;
      out_stream << (" " + std::to_string(ocgs[iocg]->object_number) + " 0 R");
    }
    for (; last_nesting_level > 0; --last_nesting_level)
      out_stream << " ]";
    out_stream << "\n                          /BaseState /ON\n" <<
      "                       >>\n                 >>\n";
  }

  out_stream << ">>\nendobj\n";

  // Object 2 is the document outline.
  object_offsets.push_back(out_stream.tellp());
  out_stream << "2 0 obj\n<< /Type /Outlines\n   /Count 0\n>>\nendobj\n";

  // Object 3 is the page dictionary.
  object_offsets.push_back(out_stream.tellp());
  out_stream << "3 0 obj\n<< /Type /Pages\n   /Kids [\n";

  for (auto const &p: pages)
  {
    out_stream << "           " << p->get_object_number() << " 0 R\n";
  }

  // Add custom references.
  for (size_t i = 0; i < custom_object_page_numbers.size(); ++i)
  {
    out_stream << "           " << custom_object_page_numbers[i] << " 0 R\n";
  }
  out_stream << "         ]\n   /Count " <<
    pages.size() + custom_object_page_numbers.size() << "\n>>\nendobj\n";

  // Write page labels in object number 4.
  // TODO: avoid writing all of them, in order to save space.
  object_offsets.push_back(out_stream.tellp());
  out_stream << "4 0 obj\n<< /Nums [\n";
  for (size_t i = 0; i < pages.size(); ++i)
  {
    out_stream << "           " << i << " << ";
    if ("" == pages[i]->get_label())
    {
      out_stream << "/S /D";
    }
    else
    {
      out_stream << "/P (" << pages[i]->get_label() << ")";
    }
    out_stream << " >>\n";
  }
  out_stream << "         ]\n>>\nendobj\n";

  // Write document info in object number 5.
  object_offsets.push_back(out_stream.tellp());
  out_stream << "5 0 obj\n<<\n";
  if (!document_information.title.empty())
    out_stream << "   /Title (" << document_information.title << ")\n";
  if (!document_information.author.empty())
    out_stream << "   /Author ("<< document_information.author << ")\n";
  if (!document_information.subject.empty())
    out_stream << "   /Subject (" << document_information.subject << ")\n";
  if (!document_information.keywords.empty())
    out_stream << "   /Keywords (" << document_information.keywords << ")\n";
  if (!document_information.creator.empty())
    out_stream << "   /Creator (" << document_information.creator << ")\n";
  if (!document_information.producer.empty())
    out_stream << "   /Producer (" << document_information.producer << ")\n";
  if (!document_information.creation_date.empty())
    out_stream << "   /CreationDate (" << document_information.creation_date << ")\n";
  if (!document_information.mod_date.empty())
    out_stream << "   /ModDate (" << document_information.mod_date << ")\n";
  out_stream << ">>\nendobj\n";

  return out_stream;
}

std::ostream& Document::write_objects(std::ostream &out_stream)
{
  // We keep track of the next object number to be written.
  unsigned written_body_objects = 0;

  for(size_t i = 0; i < body_objects.size(); ++i)
  {
    unsigned body_object_number = first_body_object_number + written_body_objects;

    // Set the object number if needed.
    if (body_objects[i]->get_type() == PdfObject::Type::FILE_STREAM)
    {
      std::dynamic_pointer_cast<FileStream>(body_objects[i])->set_object_number(body_object_number);
    }

    // Store the object offset before writing it.
    object_offsets.push_back(out_stream.tellp());

    // Write the object object.
    out_stream << body_object_number << " 0 obj\n";

    body_objects[i]->to_stream(out_stream);
    out_stream << "\nendobj\n";

    if (body_objects[i]->get_type() == PdfObject::Type::FILE_STREAM)
    {
      // Store the object offset before writing the length object.
      object_offsets.push_back(out_stream.tellp());

      // Write the length object.
      std::dynamic_pointer_cast<FileStream>(body_objects[i])->length_to_stream(out_stream);

      // We have written two objects here.
      written_body_objects += 2;
    }
    else
    {
      ++written_body_objects;
    }
  }

  // Write the pages.
  for (auto const &p: pages)
  {
    // We write the page to the stream and remember the offset of
    // each output object.
    p->to_stream(out_stream, std::back_inserter(object_offsets));
  }

  return out_stream;
}

std::ostream& Document::write_xref(std::ostream &out_stream)
{
  // Before writing the cross-reference, we compute its byte offset.
  xref_stream_position = out_stream.tellp();
  xref_stream_position -= start_stream_position;

  // The cross-reference table. Its lines should be exactly 20 characters,
  // so we need to pad a space at the end of the line.
  // Note that this space is also necessary in Windows, since we always
  // produce Unix files. However, it may be cleaner to check at runtime the
  // size of the newline and add the space if it uses only one byte.
  out_stream << "xref\n0 " << next_object_number << "\n0000000000 65535 f \n";

  // We use here the object offsets computed so far.
  for (size_t i = 0; i < object_offsets.size(); ++i)
  {
    // iomanip magic!
    out_stream << std::setfill('0') << std::setw(10) << object_offsets[i] <<
      " 00000 n \n";
  }

  return out_stream;
}

std::ostream& Document::write_trailer(std::ostream &out_stream)
{
  out_stream << "trailer\n<< /Size " << next_object_number << '\n';
  // The root catalog is at the time being object number 1.
  out_stream << "   /Root 1 0 R\n   /Info 5 0 R";
  out_stream << "\n>>\nstartxref\n" << xref_stream_position;
  out_stream << "\n%%EOF" << std::flush;
  return out_stream;
}

void Document::add_comment(const std::string &comment)
{
  document_comments.push_back(comment);

  return;
}

unsigned Document::add_custom_object(const std::string &object_contents)
{
  body_objects.push_back(CustomObjectPtr(new CustomObject(object_contents)));

  ++total_body_objects;

  return first_body_object_number + total_body_objects - 1;
}

unsigned Document::add_image_resource(const unsigned char *bytes,
                                      const unsigned char *soft_mask,
                                      unsigned bpc,
                                      unsigned channels,
                                      unsigned image_width,
                                      unsigned image_height,
                                      unsigned width,
                                      unsigned height,
                                      unsigned colorspace,
                                      bool flate)
{
  // If the image has soft mask, then add first the mask.
  unsigned soft_mask_id = soft_mask ? add_image_resource(soft_mask,
                                                         NULL,
                                                         8,
                                                         1,
                                                         image_width,
                                                         image_height,
                                                         width,
                                                         height,
                                                         COLORSPACE_DEVICEGRAY,
                                                         flate)
                                    : 0;

  // Encode the image contents in a string.
  std::string image_contents;
  unsigned contents_size = image_width * image_height * channels * (bpc/8);
  for (size_t i = 0; i < contents_size; ++i)
  {
    image_contents += bytes[i];
  }

  // Set the flate value and deflate the image if needed.
  bool use_flate;
#ifdef PADDLEFISH_USE_ZLIB
  use_flate = flate;
  if (use_flate)
  {
    image_contents = flate::deflate_string(image_contents);
  }
#else
  use_flate = false;
#endif

  // Create the contents of the object.
  std::string object_contents("<< /Type /XObject\n   /Subtype /Image");
  object_contents += "\n   /Width " + util::to_str(image_width) +
    "\n   /Height " + util::to_str(image_height) +
    "\n   /BitsPerComponent " + util::to_str(bpc) +
    "\n   /ColorSpace " + colorspace_properties[colorspace].colorspace_string;
  if (soft_mask)
  {
    object_contents += "\n   /SMask " + util::to_str(soft_mask_id) + " 0 R";
  }
  if(use_flate)
  {
    object_contents += "\n   /Filter [ /FlateDecode ]";
  }
  object_contents += "\n   /Length " + util::to_str(image_contents.size());
  object_contents += "\n>>\nstream\n" + image_contents + "\nendstream";

  // Add the image object to the document.
  unsigned object_number = add_custom_object(object_contents);

  // Store the colorspace of the image in the map.
  image_colorspaces.emplace(object_number, colorspace);

  return object_number;
}

unsigned Document::add_custom_stream(const std::string &strm,
                                     const std::string &extra_header,
                                     bool flate)
{
  return add_custom_stream(strm.c_str(),
                           (unsigned int)strm.length(),
                           extra_header.c_str(),
                           (unsigned int)extra_header.length(),
                           flate);
}

unsigned Document::add_custom_stream(const char *buffer,
                                     unsigned length,
                                     const char *extra_header,
                                     unsigned extra_headerLength,
                                     bool flate)
{
  bool use_flate;

#ifdef PADDLEFISH_USE_ZLIB
  use_flate = flate;
#else
  use_flate = false;
#endif

  std::string stream_contents =
#ifdef PADDLEFISH_USE_ZLIB
    use_flate ?
    flate::deflate_buffer(buffer, length) :
#endif
    std::string(buffer, length);

  std::string object_contents("<< /Length " +
    util::to_str(stream_contents.size()) + "\n" +
    (use_flate ? "   /Filter [ /FlateDecode ]\n" : "") +
    (extra_headerLength ? 
     std::string(extra_header, extra_headerLength) + "\n" :
     "") +
    ">>\nstream\n" + stream_contents + "\nendstream");

  body_objects.push_back(CustomObjectPtr(new CustomObject(object_contents)));

  ++total_body_objects;

  return first_body_object_number + total_body_objects - 1;
}

unsigned Document::add_custom_stream_from_file(const std::string &file_name,
                                               const std::string &extra_header = "",
                                               bool flate)
{
  body_objects.push_back(FileStreamPtr(new FileStream(file_name, extra_header, flate)));

  // The FileStream comprises two objects (stream and length).
  total_body_objects += 2;

  // It is -2 here because we want to refer the first of the two written
  // objects.
  return first_body_object_number + total_body_objects - 2;
}

void Document::add_custom_object_page_number(unsigned custom_page)
{
  custom_object_page_numbers.push_back(custom_page);
  return;
}

// We don't store the image size because we plan to let the user specify
// various pattern parameters.
unsigned Document::add_texture_pattern(unsigned image_id,
                                       double x_size,
                                       double y_size,
                                       double x_step,
                                       double y_step,
                                       double *matrix,
                                       bool flate)
{
  std::string sx(util::to_str(x_size));
  std::string sy(util::to_str(y_size));
  std::string step_x(util::to_str(x_step));
  std::string step_y(util::to_str(y_step));
  std::string stream("q\n" + sx + " 0 0 " + sy + " 0 0 cm\n/Im" +
    util::to_str(image_id) + " Do\nQ");
  std::string header("   /Type /Pattern\n\
   /PatternType 1\n\
   /PaintType 1\n\
   /TilingType 1\n\
   /BBox [0 0 " + sx + " " + sy + "]\n\
   /XStep " + step_x + "\n\
   /YStep " + step_y + "\n" +
    (matrix ? "   " + util::matrix23_to_string(matrix) + "\n" : "") +
    "   /Resources << /XObject << /Im" +
    util::to_str(image_id) + " " + util::to_str(image_id) + " 0 R >> >>");

  return add_custom_stream(stream, header, flate);
}

unsigned Document::add_tiling_pattern(const std::string content,
                                      const ResourcesDictPtr& resources,
                                      double *bbox,
                                      double x_step,
                                      double y_step,
                                      double *matrix,
                                      bool flate)
{
  std::string header("   /Type /Pattern\n   /PatternType 1\n");
  header += "   /PaintType 1\n   /TilingType 2\n   /BBox [ " +
    util::vector_to_string(bbox, 4) + " ]\n   /XStep " +
    util::to_str(x_step) + "\n   /YStep " + util::to_str(y_step) + "\n" +
    resources->to_string() + "\n   /Matrix [ " +
    util::matrix23_contents_to_string(matrix) + " ]\n";

  return add_custom_stream(content, header, flate);
}

unsigned Document::add_interpolation_function(unsigned d,
                                              double *c0,
                                              double *c1,
                                              double n,
                                              double t0,
                                              double t1)
{
  std::string function("<< /FunctionType 2\n\
   /Domain [ " + util::to_str(t0) + ' ' + util::to_str(t1) + " ]\n\
   /C0 [ " + util::vector_to_string(c0, d) + " ]\n\
   /C1 [ " + util::vector_to_string(c1, d) + " ]\n\
   /N " + util::to_str(n) + "\n>>");

  return add_custom_object(function);
}

unsigned Document::add_stitching_function(unsigned k,
                                          unsigned *functions,
                                          double *bounds,
                                          double *encode,
                                          double d0,
                                          double d1)
{
  std::string function("<< /FunctionType 3\n\
   /Domain [ " + util::to_str(d0) + ' ' + util::to_str(d1) + " ]\n\
   /Functions [ ");

  for (unsigned i = 0; i < k; ++i)
  {
    function += util::to_str(functions[i]) + " 0 R ";
  }

  function += "]\n\
   /Bounds [ " + util::vector_to_string(bounds, k - 1) + " ]\n\
   /Encode [ " + util::vector_to_string(encode, 2 * k) + " ]\n>>";

  return add_custom_object(function);
}

unsigned Document::add_shading(unsigned type,
                               unsigned colorspace_id,
                               double *coords,
                               double t0,
                               double t1,
                               unsigned function_id,
                               bool extend_start,
                               bool extend_end)
{
  std::string shading("<< /ShadingType " + util::to_str(type) + "\n\
   /ColorSpace " +
  colorspace_properties[colorspace_id].colorspace_string + "\n\
   /Function " + util::to_str(function_id) + " 0 R\n\
   /Coords [ " + util::vector_to_string(coords, 3 == type ? 6 : 4) + " ]\n\
   /Domain [ " + util::to_str(t0) + ' ' + util::to_str(t1) + " ]\n\
   /Extend [ " + (extend_start ? "true " : "false ") +
  (extend_end ? "true" : "false") + " ]\n>>");

  return add_custom_object(shading);
}

unsigned Document::add_shading_pattern(unsigned shading_id, double *matrix)
{
  return add_custom_object("<< /Type /Pattern\n\
   /PatternType 2\n   " +
    util::matrix23_to_string(matrix) +
    "\n   /Shading " + util::to_str(shading_id) + " 0 R\n>>");
}

unsigned Document::add_shading_soft_mask(unsigned sh_pattern_id,
                                         double *coords,
                                         double *matrix)
{
  std::string sh_id = util::to_str(sh_pattern_id);

  std::string gstream("/a" + sh_id + " gs /sh" + sh_id + " sh");

  std::string gparameters("   /Type /XObject\n\
   /Subtype /Form\n\
   /FormType 1\n\
   /BBox [ " + util::vector_to_string(coords, 4) + " ]\n   " +
    util::matrix23_to_string(matrix) +
    "\n   /Resources << /ExtGState << /a" + sh_id + " << /ca 1 /CA 1 >> >>\n\
                 /Shading << /sh" + sh_id + " " + sh_id + " 0 R >>\n\
   >>\n\
   /Group << /Type /Group\n\
             /S /Transparency\n\
             /I true\n\
             /CS /DeviceGray\n\
   >>");

  unsigned g = add_custom_stream(gstream, gparameters, false);

  std::string smask("<< /Type /Mask /S /Luminosity /G " +
    util::to_str(g) + " 0 R >>");

  std::string extgs("<< /Type /ExtGState\n\
   /SMask " + smask + "\n\
   /ca 1\n\
   /CA 1\n\
   /AIS false\n>>");

  return add_custom_object(extgs);
}

GraphicsStatePtr Document::add_graphics_state()
{
  body_objects.push_back(GraphicsStatePtr(new GraphicsState()));

  ++total_body_objects;

  GraphicsStatePtr rGS = std::dynamic_pointer_cast<GraphicsState>(body_objects.back());

  rGS->set_object_number(first_body_object_number + total_body_objects - 1);

  return rGS;
}

const ColorspaceProperties* Document::get_colorspace_properties(unsigned object_id)const
{
  typedef typename colorspace_map::const_iterator map_iterator;
  map_iterator properties = colorspace_properties.find(object_id);
  return (properties == colorspace_properties.end() ?
          NULL :
          &(properties->second));
}

const unsigned Document::get_image_colorspace(unsigned image_id)const
{
  typedef typename image_resource_map::const_iterator map_iterator;
  map_iterator image_colorspace = image_colorspaces.find(image_id);
  return (image_colorspace == image_colorspaces.end() ?
          COLORSPACE_DEVICERGB :
          image_colorspace->second);
}

void Document::add_ocg_list(const std::vector<OcgPtr> &ocg_vector)
{
  ocgs.reserve(ocgs.size() + ocg_vector.size());

  ocgs.insert(ocgs.end(), ocg_vector.begin(), ocg_vector.end());

  return;
}

} // namespace paddlefish

std::ostream& operator<<(std::ostream& out_stream, paddlefish::DocumentPtr doc)
{
  return doc->to_stream(out_stream);
}

// vim:ts=2:sw=2:expandtab
