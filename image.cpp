// Copyright (c) 2017-2020 Luis Peñaranda. All rights reserved.
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

#include "image.h"
#include <fstream>
#include "flate.h"
#include "util.h"
#include "colorspace_properties.h"
#include <cstring>

namespace paddlefish {

Image::Image(unsigned x_size,
             unsigned y_size,
             double x_pos,
             double y_pos,
             double print_width,
             double print_height,
             const std::string &file,
             unsigned cs,
             bool flate):
    bytes(NULL),
    bytes_size(0),
    bpc(8),
    image_type(Image::Type::JPEG),
    filename(file),
    colorspace(cs),
    decode(NULL)
{
#ifdef PADDLEFISH_USE_ZLIB
  use_flate = flate;
#else
  use_flate = false;
#endif

  use_soft_mask = false;

  image_size[0] = x_size;
  image_size[1] = y_size;
  stride = (unsigned)ceil((double)(image_size[0] * channels*bpc) / 8.0);
  raw_size = stride * y_size;

  matrix = (double*)malloc(6 * sizeof(double));
  matrix[0] = print_width;
  matrix[1] = matrix[2] = 0.;
  matrix[3] = print_height;
  matrix[4] = x_pos;
  matrix[5] = y_pos;
}

Image::Image(const unsigned char *raw_bytes,
             bool uses_soft_mask,
             unsigned bits_per_component,
             unsigned number_of_channels,
             unsigned image_width,
             unsigned image_height,
             double x_pos,
             double y_pos,
             double print_width,
             double print_height,
             unsigned cs,
             bool flate):
  bpc(bits_per_component),
  channels(number_of_channels),
  image_type(Image::Type::RAW),
  filename(""),
  colorspace(cs),
  decode(NULL),
  use_soft_mask(uses_soft_mask)
{
#ifdef PADDLEFISH_USE_ZLIB
  use_flate = flate;
#else
  use_flate = false;
#endif
  image_size[0] = image_width;
  image_size[1] = image_height;
  stride = (unsigned)ceil((double)(image_size[0] * channels*bpc) / 8.0);
  raw_size = stride * image_height;

  fill_bytes(raw_bytes);

  matrix = (double*)malloc(6*sizeof(double));
  matrix[0] = print_width;
  matrix[1] = matrix[2] = 0.;
  matrix[3] = print_height;
  matrix[4] = x_pos;
  matrix[5] = y_pos;
}

Image::Image(const unsigned char *raw_bytes,
             bool uses_soft_mask,
             unsigned bits_per_component,
             unsigned number_of_channels,
             unsigned image_width,
             unsigned image_height,
             double *matrix23,
             unsigned cs,
             bool flate) :
  bpc(bits_per_component),
  channels(number_of_channels),
  image_type(Image::Type::RAW),
  matrix(matrix23),
  filename(""),
  colorspace(cs),
  decode(NULL),
  use_soft_mask(uses_soft_mask)
{
#ifdef PADDLEFISH_USE_ZLIB
  use_flate = flate;
#else
  use_flate = false;
#endif

  image_size[0] = image_width;
  image_size[1] = image_height;
  stride = (unsigned)ceil((double)(image_size[0] * channels*bpc) / 8.0);
  raw_size = stride * image_height;

  fill_bytes(raw_bytes);
}

Image::Image(const unsigned char *mask_bytes,
             unsigned image_width,
             unsigned image_height,
             double *matrix23,
             unsigned *decode_array,
             bool flate) :
bpc(1),
channels(1),
image_type(Image::Type::IMAGE_MASK),
matrix(matrix23),
filename(""),
colorspace(0), // The colorspace is unused in this kind of image.
decode(decode_array),
use_soft_mask(false)
{
#ifdef PADDLEFISH_USE_ZLIB
  use_flate = flate;
#else
  use_flate = false;
#endif

  image_size[0] = image_width;
  image_size[1] = image_height;
  stride = (unsigned)ceil((double)(image_size[0] * channels*bpc) / 8.0);
  raw_size = stride * image_height;

  fill_bytes(mask_bytes);
}

Image::~Image()
{
  /*if (NULL != bytes)
  {
    free((void*)bytes);
    bytes = NULL;
  }*/
}

const std::string Image::get_contents() const
{
  return std::string ("q\n" + util::matrix23_contents_to_string(matrix) +
    " cm\n/Im" + util::to_str(get_object_number()) + " Do\nQ\n");
}

unsigned Image::write_image(std::ostream &o, unsigned obj_number)const
{
  o << obj_number <<
    " 0 obj\n<< /Type /XObject\n   /Subtype /Image\n" <<
    "   /Name /Im" << get_object_number() <<
    "\n   /Filter " << get_image_filters() <<
    "\n   /Width " << get_image_width() <<
    "\n   /Height " << get_image_height() <<
    "\n   /BitsPerComponent " << get_bits_per_component();
  if(image_type == Image::Type::IMAGE_MASK)
  {
    o << "\n   /ImageMask true"
      "\n   /Decode [ " << util::vector_to_string(decode, 2) << " ]";
  }
  else
  {
    o << "\n   /ColorSpace " << get_colorspace_string();
  }
  // If the image has soft mask, then the soft mask is the next object, after
  // the stream specifying the length.
  if(has_soft_mask())
  {
    o << "\n   /SMask " << (obj_number + 2) << " 0 R";
  }
  o << "\n   /Length " << (obj_number + 1) << " 0 R\n>>\nstream\n";
  unsigned written = write_image_stream(o);
  o << "\nendstream\nendobj\n";
  return written;
}

// Fills the bytes array with the contents of the image, compressing
// the stream if needed. This function must be called after setting
// raw_size.
void Image::fill_bytes(const unsigned char *source)
{
#ifdef PADDLEFISH_USE_ZLIB
  if (use_flate)
  {
    unsigned bound = flate::deflate_bound(raw_size);
    const unsigned char *bytes_raw_ptr = (const unsigned char*)malloc(bound * sizeof(const unsigned char));
    bytes_size = flate::deflate_buffer_to_buffer(reinterpret_cast<const char*>(source),
                                                 raw_size,
                                                 const_cast<char*>(reinterpret_cast<const char*>(bytes_raw_ptr)),
                                                 bound);
    bytes = std::shared_ptr<const unsigned char>(bytes_raw_ptr);
  }
  else
#endif
  {
    const unsigned char *bytes_raw_ptr = (const unsigned char*)malloc(raw_size * sizeof(const unsigned char));
    memcpy(const_cast<unsigned char*>(bytes_raw_ptr), source, raw_size);
    bytes_size = raw_size;
    bytes = std::shared_ptr<const unsigned char>(bytes_raw_ptr);
  }

  return;
}

unsigned Image::write_file_contents(std::ostream &o)const
{
#ifdef PADDLEFISH_USE_ZLIB
    if(use_flate)
    {
      unsigned start_pos = o.tellp();
      flate::deflate_file_to_stream(o, filename);
      return (unsigned)o.tellp() - start_pos;
    }
    else
#endif
    {
        std::ifstream f(filename,std::ios_base::in|std::ios_base::binary);
        unsigned start,end;
        start=o.tellp();
        o << f.rdbuf();
        end=o.tellp();
        f.close();
        return end-start;
    }
}

unsigned Image::write_bytes(std::ostream &o)const
{
  unsigned start,end;

  start = o.tellp();

  o.write(reinterpret_cast<const char*>(bytes.get()), bytes_size);
  /*std::string raw_image;
  for (size_t i = 0; i < raw_size; ++i)
    raw_image += bytes[i];
  o << raw_image;*/

  end = o.tellp();

  return end-start;
}

unsigned Image::write_image_stream(std::ostream &o)const
{
  unsigned written_bytes;

  switch (image_type)
  {
    case Image::Type::JPEG:
      written_bytes = write_file_contents(o);
      break;
    default:
      written_bytes = write_bytes(o);
      break;
  }

  return written_bytes;
}

void Image::set_object_number(unsigned n){
        image_object_number=n;
        return;
}

std::string Image::get_image_filters()const
{
  std::string filters("[");
  switch (image_type) {
    case Image::Type::JPEG:
#ifdef PADDLEFISH_USE_ZLIB
      if (use_flate)
        filters += " /FlateDecode";
#endif
      filters += " /DCTDecode";
      break;
    case Image::Type::RAW:
    case Image::Type::IMAGE_MASK:
#ifdef PADDLEFISH_USE_ZLIB
      if (use_flate)
        filters += " /FlateDecode";
#endif
      break;
    default:
      filters="ERROR";
      break;
  }
  filters += " ]";
  return filters;
}

std::string Image::get_colorspace_string()const
{
  switch (colorspace)
  {
  case COLORSPACE_DEVICERGB:
    return "/DeviceRGB";
    break;
  case COLORSPACE_DEVICEGRAY:
    return "/DeviceGray";
    break;
  default:
    return util::to_str(colorspace) + " 0 R";
    break;
  }
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
