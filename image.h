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

#ifndef PADDLEFISH_IMAGE_H
#define PADDLEFISH_IMAGE_H

#include "colorspace_properties.h"
#include "pdf_object.h"
#include <ostream>
#include <cmath>
#include <string>
#include <cstdint>
#include <memory>

namespace paddlefish {

class Image;

typedef std::shared_ptr<Image> ImagePtr;

// This class represents an image. For the moment, we only support JPEG.
class Image: public PdfObject
{
    public:
        enum class Type:std::uint8_t{
          JPEG,
          RAW,
          IMAGE_MASK,
          OTHER
        };

        // This constructor needs a JPEG file. It does not process the
        // contents, it only pastes the file contents in the output.
        Image(unsigned x_size,
              unsigned y_size,
              double x_pos,
              double y_pos,
              double print_width,
              double print_height,
              const std::string &file,
              unsigned cs = COLORSPACE_DEVICERGB,
              bool flate = true);

        // This constructor accepts the bytes of the image in raw format.
        // Each pixel must be defined using 8 bits per channel, in RGBA
        // order.
        Image(const unsigned char *raw_bytes,
              bool uses_soft_mask,
              unsigned bits_per_component,
              unsigned number_of_channels,
              unsigned image_width,
              unsigned image_height,
              double x_pos,
              double y_pos,
              double width,
              double height,
              unsigned cs = COLORSPACE_DEVICERGB,
              bool flate = true);

        // Same as the constructor above, but the size, position and rotation
        // is specified by a matrix.
        Image(const unsigned char *raw_bytes,
              bool uses_soft_mask,
              unsigned bits_per_component,
              unsigned number_of_channels,
              unsigned image_width,
              unsigned image_height,
              double *matrix23,
              unsigned cs = COLORSPACE_DEVICERGB,
              bool flate = true);

        // Constructor for an image mask. Each line in *mask_bytes must
        // use a number of bytes which is multiple of 8. If the width is
        // not multiple of eight, then some bits must be added to the row.
        Image(const unsigned char *mask_bytes,
              unsigned image_width,
              unsigned image_height,
              double *matrix23,
              unsigned *decode_array,
              bool flate = true);

        ~Image();

        std::ostream& to_stream(std::ostream &o) const { return o << get_contents(); }

        const std::string get_contents() const;

        // Write to stream the object representing the image. It adds a
        // reference to the next object in the file, containing stream
        // length. Returns the length of the stream, needed to write the
        // next object.
        unsigned write_image(std::ostream&,unsigned)const;
        // Write the file contents to stream. Return the number of written
        // bytes.
        unsigned write_image_stream(std::ostream&)const;
        // Set the object number of the image in the document.
        void set_object_number(unsigned);
        // Get the object number.
        unsigned get_object_number()const { return image_object_number; }
        // Get a string containing the filters needed to read the stream.
        std::string get_image_filters()const;
        // Get image data.
        unsigned get_bits_per_component()const { return bpc; }
        unsigned get_number_of_channels()const { return channels; }
        double get_x_position() const { return matrix[4]; }
        double get_y_position() const { return matrix[5]; }
        unsigned get_image_width()const { return image_size[0]; }
        unsigned get_image_height()const { return image_size[1]; }
        double get_print_width()const { return matrix[0]; }
        double get_print_height()const { return matrix[3]; }
        unsigned get_colorspace()const { return colorspace; }
        std::string get_colorspace_string()const;
        unsigned* get_decode()const { return decode; }
        Image::Type get_image_type()const { return image_type; }
        PdfObject::Type get_type() const { return PdfObject::Type::IMAGE; }
        bool uses_flate()const { return use_flate; }
        bool has_soft_mask()const { return use_soft_mask; }
    private:
        void fill_bytes(const unsigned char *source);
        unsigned write_file_contents(std::ostream&)const;
        unsigned write_bytes(std::ostream&)const;
    private:
        std::shared_ptr<const unsigned char> bytes;
        unsigned bytes_size;
        unsigned bpc;
        unsigned channels;
        Type image_type;
        unsigned image_object_number;
        unsigned image_size[2];
        unsigned stride;
        unsigned raw_size;
        double *matrix;
        std::string filename;
        unsigned colorspace;
        unsigned *decode;
        bool use_flate;
        bool use_soft_mask;
};

} // namespace paddlefish

#endif // PADDLEFISH_IMAGE_H

// vim: ts=2:sw=2:expandtab
