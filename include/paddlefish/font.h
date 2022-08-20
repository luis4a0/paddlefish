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

#ifndef PADDLEFISH_FONT_H
#define PADDLEFISH_FONT_H

#include "pdf_object.h"
#include <string>
#include <memory>

namespace paddlefish {

class Font;

typedef std::shared_ptr<Font> FontPtr;

#define NOT_EMBEDDED  0
#define EMBEDDED_FONT 1
#define SUBSET        2

class Font: public PdfObject
{
public:
  enum class Type:std::uint8_t
  {
    STANDARD_TYPE_1,
    TRUE_TYPE,
    CID_TYPE_2
  };
  
  // Constructor for a standard Type 1 font. Only the font name must be given.
  Font(const std::string &name);

  // Generate the random six capitals tag for the font. This is used only
  // when the font is subset.
  const std::string generate_tag();

  // Set the object number of the widths array.
  void set_widths_ref(unsigned ref) { widths_ref = ref; }

  // Set the object number of the font descriptor.
  void set_font_descriptor_ref(unsigned ref) { font_descriptor_ref = ref; }

  // Set the object number containing the stream with the font program.
  void set_font_file_ref(unsigned ref) { font_file_ref = ref; }

  // Set the object number of the descendant font.
  void set_descendant_ref(unsigned ref) { descendant_ref = ref; }

  // Set the object number of the CID to GID map.
  void set_map_ref(unsigned ref) { map_ref = ref; }

  // Get a string containing the font object to write on the PDF. The
  // references to widths and font descriptor must be set.
  const std::string get_contents() const;

  // Write the contents to a stream, same preconditions as get_contents().
  std::ostream& to_stream(std::ostream &o) const { return o << get_contents(); }

  Font::Type get_font_type() const { return type; }

  PdfObject::Type get_type() const { return PdfObject::Type::FONT; }

  unsigned get_embedding() const { return embedding; }

  void set_name(const std::string &new_name) { base_font = new_name; }
  std::string get_name() const;

private:
  Font::Type type;
  std::string base_font;
  std::string base_font_tag;

  // Some data needed for the font dictionary.
  wchar_t first_char, last_char;
  unsigned widths_ref, font_descriptor_ref, font_file_ref, descendant_ref, map_ref;
  unsigned embedding = NOT_EMBEDDED;
  [[maybe_unused]] unsigned flags;
  [[maybe_unused]] double scale;
  [[maybe_unused]] double ppEm;

  // Some data needed for the font descriptor dictionary.
  [[maybe_unused]] double *font_bbox;
  [[maybe_unused]] double italic_angle, ascent, descent, cap_height, stem_v;
};

} // namespace paddlefish

#endif // PADDLEFISH_FONT_H

// vim: ts=2:sw=2:expandtab
