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

#include <paddlefish/colorspace_properties.h>
#include <paddlefish/document.h>
#include <paddlefish/page.h>
#include <paddlefish/util.h>

#include <memory>

namespace paddlefish {

Page::Page()
{
  // The page number will be zero until the page being added to a document.
  page_number = 0;

  // The label will be empty, but it will be used if set.
  page_label = std::string();

  // By default, we set a page size of US Letter.
  mediabox[0] = mediabox[1] = 0;
  mediabox[2] = 612;
  mediabox[3] = 792;

  has_images_color = has_images_gray = has_images_indexed = false;
  has_text = has_commands = false;
  images_count = 0;

  marked_content_nesting_level = 0;

  document_ptr = nullptr;
}

void Page::add_jpeg_image(const std::string &filename,
                          unsigned jpeg_width,
                          unsigned jpeg_height,
                          double x_pos,
                          double y_pos,
                          double width,
                          double height,
                          unsigned cs)
{
  page_objects.push_back(ImagePtr(new Image(jpeg_width,
                                            jpeg_height,
                                            x_pos,
                                            y_pos,
                                            width,
                                            height,
                                            filename,
                                            cs)));

  ++images_count;

  return;
}

void Page::add_jpeg_image(const std::string &filename,
                          unsigned jpeg_width,
                          unsigned jpeg_height,
                          double *matrix23,
                          unsigned cs)
{
  page_objects.push_back(ImagePtr(new Image(jpeg_width,
                                            jpeg_height,
                                            matrix23,
                                            filename,
                                            cs)));

  ++images_count;

  return;
}

void Page::add_image_bytes(const unsigned char *bytes,
                           const unsigned char *soft_mask,
                           unsigned bpp,
                           unsigned channels,
                           unsigned image_width,
                           unsigned image_height,
                           double x_pos,
                           double y_pos,
                           double width,
                           double height,
                           unsigned cs,
                           bool flate)
{
  page_objects.push_back(ImagePtr(new Image(bytes,
                                            (soft_mask != NULL),
                                            bpp,
                                            channels,
                                            image_width,
                                            image_height,
                                            x_pos,
                                            y_pos,
                                            width,
                                            height,
                                            cs,
                                            flate)));

  ++images_count;

  if (soft_mask != NULL)
  {
    page_objects.push_back(ImagePtr(new Image(soft_mask,
                                              false,
                                              bpp,
                                              1,
                                              image_width,
                                              image_height,
                                              x_pos,
                                              y_pos,
                                              width,
                                              height,
                                              COLORSPACE_DEVICEGRAY,
                                              flate)));

    ++images_count;
  }

  return;
}

void Page::add_image_bytes(const unsigned char *bytes,
                           const unsigned char *soft_mask, // can be NULL
                           unsigned bpp,
                           unsigned channels,
                           unsigned image_width,
                           unsigned image_height,
                           double *matrix23,
                           unsigned cs,
                           bool flate)
{
  page_objects.push_back(ImagePtr(new Image(bytes,
                                            (soft_mask != NULL),
                                            bpp,
                                            channels,
                                            image_width,
                                            image_height,
                                            matrix23,
                                            cs,
                                            flate)));

  ++images_count;

  if (soft_mask != NULL)
  {
    page_objects.push_back(ImagePtr(new Image(soft_mask,
                                              false,
                                              bpp,
                                              1,
                                              image_width,
                                              image_height,
                                              matrix23,
                                              COLORSPACE_DEVICEGRAY,
                                              flate)));

    ++images_count;
  }

  return;
}

void Page::add_image_mask(const unsigned char *bytes,
                          unsigned image_width,
                          unsigned image_height,
                          double *matrix23,
                          unsigned *decode,
                          bool flate)
{
  page_objects.push_back(ImagePtr(new Image(bytes,
                                            image_width,
                                            image_height,
                                            matrix23,
                                            decode,
                                            flate)));

  ++images_count;

  return;
}

void Page::add_command(const std::string &command)
{
  page_objects.push_back(CommandPtr(new Command(command)));
  has_commands = true;

  return;
}

void Page::set_text_state(const TextStatePtr &new_ts)
{
  add_command(new_ts->to_string(text_state));
  text_state = new_ts;
  if (0 != new_ts->font_id)
  {
    add_font(new_ts->font_id);
  }

  return;
}

std::ostream& Page::to_stream(
        std::ostream &out_stream,
        std::back_insert_iterator<std::vector<unsigned> > inserter)
{
  // We first gather the colorspace information on all the figures on
  // this page.
  gather_image_color_information();

  // Insert first the current offset in the documents object offset
  // table, passed as a parameter to this function.
  *inserter++ = out_stream.tellp();

  // Write the page to the stream.
  out_stream << object_number << " 0 obj\n<< /Type /Page\n";

  // For the time being, the page dictionary is object number 3.
  out_stream << "   /Parent 3 0 R\n";
  out_stream << "   /MediaBox [ " << mediabox[0] << ' ' << mediabox[1] <<
      ' ' << mediabox[2] << ' ' << mediabox[3] << " ]\n";
  if (has_commands ||
      has_text ||
      images_count > 0 ||
      rdict->get_images().empty() ||
      !ocgs.empty())
  {
    out_stream << "   /Contents " << object_number + 1 << " 0 R\n";
    out_stream << "   /Resources\n   << /ProcSet [ ";
    if (has_commands)
      out_stream << "/PDF ";
    if (has_text)
      out_stream << "/Text ";
    if (has_images_color)
      out_stream << "/ImageC ";
    if (has_images_gray)
      out_stream << "/ImageB ";
    if (has_images_indexed)
      out_stream << "/ImageI ";
    out_stream << "]\n";

    if (!ocgs.empty())
    {
      out_stream << "      /Properties <<\n";
      for (size_t oi = 0; oi < ocgs.size(); ++oi)
      {
        out_stream << "                     /" << ocgs[oi]->internal_name <<
          ' ' << ocgs[oi]->object_number << " 0 R\n";
      }
      out_stream << "                  >>\n";
    }

    //
    // Write references to all the elements used in this page.
    //

    // Write references to all image objects in the page.
    if (images_count > 0)
    {
      out_stream << "      /XObject <<\n";
      size_t image_number = object_number + 3;
      for (size_t i = 0; i < page_objects.size(); ++i)
      {
        if (page_objects[i]->get_type() == PdfObject::Type::IMAGE)
        {
          ImagePtr im=std::dynamic_pointer_cast<Image>(page_objects[i]);
          im->set_object_number(image_number);
          out_stream << "                  /Im" << im->get_object_number() <<
              ' ' << im->get_object_number() << " 0 R\n";
          // If the image has soft mask, we don't need to declare the soft
          // mask here. We only assign an object number to the soft mask,
          // in order to write it down later.
          image_number += 2;
          if (im->has_soft_mask())
          {
            ++i;
            ImagePtr mask = std::dynamic_pointer_cast<Image>(page_objects[i]);
            mask->set_object_number(image_number);
            image_number += 2;
          }
        }
      }
      out_stream << "      >>\n";
    }

    write_colorspace_resources(out_stream);

    write_resources(out_stream, "Font", "F", rdict->get_fonts());

    write_resources(out_stream, "Pattern", "Pt", rdict->get_patterns());

    write_resources(out_stream, "ExtGState", "s", rdict->get_graphics_states());

    write_resources(out_stream, "Shading", "sh", rdict->get_shadings());

    for (size_t i = 0; i < custom_page_resources.size(); ++i)
    {
      out_stream << "      " << custom_page_resources[i] << "\n";
    }

    out_stream << "   >>\n";

    //
    //
    //

    // Display page contents, that is, the references here and
    // the objects later.
    out_stream << ">>\nendobj\n";
    *inserter++ = out_stream.tellp();
    out_stream << (object_number+1) << " 0 obj\n<< /Length " <<
      (object_number+2) << " 0 R >>\nstream\n";

    // Page contents: commands and text.
    unsigned stream_start = out_stream.tellp();
    if (!page_objects.empty())
    {
      for (size_t i = 0; i < page_objects.size(); ++i)
      {
        page_objects[i]->to_stream(out_stream);
        // If the image has soft mask, then the soft mask must not
        // be printed in the image; it is only referenced when
        // declaring the image XObject on the page resources.
        if (page_objects[i]->get_type() == PdfObject::Type::IMAGE &&
            std::dynamic_pointer_cast<Image>(page_objects[i])->has_soft_mask())
        {
          ++i;
        }
      }
    }

    unsigned stream_end = out_stream.tellp();
    out_stream << "endstream\nendobj\n";
    // Write now the stream length object.
    *inserter++ = out_stream.tellp();
    out_stream << (object_number+2) << " 0 obj\n   " <<
      stream_end-stream_start << "\nendobj\n";
    // Write the referenced images, the first object number
    // is object_number+3.
    size_t image_number = object_number + 3;
    for(size_t ii = 0; ii < page_objects.size(); ++ii)
    {
      if(page_objects[ii]->get_type() == PdfObject::Type::IMAGE)
      {
        *inserter++ = out_stream.tellp();
        ImagePtr im = std::dynamic_pointer_cast<Image>(page_objects[ii]);
        unsigned written = im->write_image(out_stream, image_number);
        *inserter++ = out_stream.tellp();
        out_stream << (image_number + 1) << " 0 obj\n   " << written <<
          "\nendobj\n";
        image_number += 2;
      }
    }

    // Write the OCG objects.
    for (size_t oi = 0; oi < ocgs.size(); ++oi)
    {
      *inserter++ = out_stream.tellp();
      out_stream << ocgs[oi]->object_number << " 0 obj\n<< /Name (" <<
        util::escape_string(ocgs[oi]->name) <<
        ")\n   /Type /OCG\n>>\nendobj\n";
    }
  }
  else
  {
    out_stream << ">>\nendobj\n";
  }

  return out_stream;
}

void Page::set_mediabox(float x_start,
                        float y_start,
                        float x_end,
                        float y_end)
{
  mediabox[0] = x_start;
  mediabox[1] = y_start;
  mediabox[2] = x_end;
  mediabox[3] = y_end;

  return;
}

unsigned Page::set_object_number(unsigned id)
{
  // Assign n to the page object and consecutive numbers to the objects
  // on the page.
  object_number = id;

  // We need three object numbers here: one for declaring the page, one for
  // describing page contents and one for specifying the length of that
  // stream. If the page happen to be empty, we use those three objects
  // anyway.
  unsigned ret = object_number + 3;

  // We reserve two object numbers per page image and one per OCG.
  ret += (2 * images_count);

  // Before returning, we set the object number of the OCG's.
  for (unsigned iocg = 0; iocg < ocgs.size(); ++iocg)
  {
    ocgs[iocg]->object_number = ret + iocg;
  }

  // Finally, we reserve object numbers for the OCG's of this page.
  ret += ocgs.size();

  return ret;
}

void Page::set_document(Document *father)
{
  document_ptr = father;
}

void Page::set_document(DocumentPtr father)
{
  document_ptr = father.get();
}

void Page::add_custom_resource(const std::string &aResource)
{
  custom_page_resources.push_back(aResource);

  return;
}

// TODO: add automatically the colorspace "/CSn" in the page resources.
void Page::set_colorspace(unsigned stroking, unsigned cs_id)
{
  std::string command;
  switch (cs_id)
  {
    case COLORSPACE_DEVICERGB:
      command = "/DeviceRGB";
      break;
    case COLORSPACE_DEVICEGRAY:
      command = "/DeviceGray";
      break;
    case COLORSPACE_DEVICECMYK:
      command = "/DeviceCMYK";
      break;
    case COLORSPACE_PATTERN:
      command = "/Pattern";
      break;
    default:
      command = "/CS" + util::to_str(cs_id);
      add_colorspace(cs_id);
      break;
  }

  command += (stroking ? " CS\n" : " cs\n");

  add_command(command);

  return;
}

void Page::set_color(unsigned stroking, double component)
{
  add_command(util::to_str(component) + (stroking?" SCN":" scn") + '\n');

  return;
}

void Page::set_color(unsigned stroking, double c1, double c2, double c3)
{
  add_command(util::to_str(c1) + ' ' + util::to_str(c2) + ' ' +
    util::to_str(c3) + (stroking ? " SCN" : " scn") + '\n');

  return;
}

void Page::set_color(unsigned stroking, double c1, double c2, double c3, double c4)
{
  add_command(util::to_str(c1) + ' ' + util::to_str(c2) + ' ' +
    util::to_str(c3) + ' ' + util::to_str(c4) +
    (stroking ? " SCN" : " scn") + '\n');

  return;
}

void Page::set_pattern(unsigned stroking, unsigned pattern_id)
{
  std::string command("/Pattern ");
  command += (stroking ? "CS" : "cs");
  command += "\n/Pt" + util::to_str(pattern_id) + " scn\n";

  rdict->add_pattern(pattern_id);

  add_command(command);

  return;
}

void Page::set_graphics_state(unsigned gs_id)
{
  add_graphics_state(gs_id);

  add_command("/s" + util::to_str(gs_id) + " gs\n");

  return;
}

void Page::set_graphics_state(GraphicsStatePtr &gstate)
{
  unsigned obj_number = gstate->get_object_number();

  add_graphics_state(obj_number);

  add_command("/s" + util::to_str(obj_number) + " gs\n");

  return;
}

void Page::draw_shading_pattern(unsigned sh_pat_id)
{
  add_shading(sh_pat_id);

  add_command("/sh" + util::to_str(sh_pat_id) + " sh\n");
}

void Page::start_marked_content(const std::string &name)
{
  ++marked_content_nesting_level;

  // We add the OCG to the page resources only if an OCG with the same
  // name was not yet added to this page.
  bool ocg_exists = false;

  for (size_t i = 0; i < ocgs.size(); ++i)
  {
    if (ocgs[i]->name == name)
    {
      ocg_exists = true;
      add_command("/OC /" + ocgs[i]->internal_name + " BDC\n");
      break;
    }
  }

  if (!ocg_exists)
  {
    // When marking an OCG, we add the Ocg object to the page without
    // object number, which will be computed later.
    OcgPtr ocg(new Ocg(name, 0, marked_content_nesting_level));

    ocgs.push_back(ocg);

    add_command("/OC /" + ocg->internal_name + " BDC\n");
  }

  return;
}

void Page::end_marked_content()
{
  --marked_content_nesting_level;

  add_command("EMC\n");

  return;
}

void Page::set_image_color_information(unsigned cs_id){
  const ColorspaceProperties *properties =
    document_ptr->get_colorspace_properties(cs_id);

  switch (properties->colorspace_type)
  {
    case ColorspaceProperties::Type::DEVICE:
      if (cs_id == COLORSPACE_DEVICEGRAY)
        has_images_gray = true;
      else
        has_images_color = true;
      break;

    case ColorspaceProperties::Type::ICC_BASED:
      // ICC-based colorspaces are never indexed. If it has one channel,
      // we say it is a gray colorspace. Otherwise, it is a color one.
      if (properties->colorspace_channels == 1)
        has_images_gray = true;
      else
        has_images_color = true;
      break;

    case ColorspaceProperties::Type::CALRGB:
      has_images_color = true;
      break;

    case ColorspaceProperties::Type::CALGRAY:
      has_images_gray = true;
      break;

    case ColorspaceProperties::Type::INDEXED:
      has_images_indexed = true;
      break;

    default:
      break;
  }

  return;
}

void Page::gather_image_color_information()
{
  for (std::vector<std::shared_ptr<PdfObject> >::const_iterator i = page_objects.begin();
    i != page_objects.end();
    ++i)
  {
    if ((*i)->get_type() == PdfObject::Type::IMAGE)
    {
      set_image_color_information(std::dynamic_pointer_cast<Image>(*i)->get_colorspace());
    }
  }

  const auto &image_resources = rdict->get_images();

  for (size_t i = 0; i < image_resources.size(); ++i)
  {
    set_image_color_information(document_ptr->get_image_colorspace(i));
  }

  return;
}

std::ostream& Page::write_colorspace_resources(std::ostream &out_stream) const
{
  const auto &colorspaces = rdict->get_colorspaces();

  if (!colorspaces.empty())
  {
    std::string pad1, pad2;
    pad1.append(6, ' ');
    pad2.append(21, ' ');

    out_stream << pad1 << "/ColorSpace <<\n";

    for (size_t i = 0; i < colorspaces.size(); ++i)
    {
      const ColorspaceProperties *properties =
        document_ptr->get_colorspace_properties(colorspaces[i]);
      if (properties->colorspace_type != ColorspaceProperties::Type::DEVICE)
      {
        out_stream << pad2 << "/CS" << util::to_str(colorspaces[i]) <<
          " " << util::to_str(colorspaces[i]) << " 0 R\n";
      }
    }
    out_stream << pad1 << ">>\n";
  }

  return out_stream;
}

template <class T>
std::ostream& Page::write_resources(std::ostream &out_stream,
                                    const std::string &name,
                                    const std::string &prefix,
                                    const std::vector<T> &elements) const
{
  if (!elements.empty())
  {
    std::string pad1, pad2;
    pad1.append(6, ' ');
    pad2.append(11 + name.length(), ' ');

    out_stream << pad1 << '/' << name << " <<\n";
    for (size_t i = 0; i < elements.size(); ++i)
    {
      out_stream << pad2 << '/' << prefix << util::to_str(elements[i]) <<
        " " << util::to_str(elements[i]) << " 0 R\n";
    }
    out_stream << pad1 << ">>\n";
  }

  return out_stream;
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
