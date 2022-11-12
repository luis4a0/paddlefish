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

#include <paddlefish/font.h>
#include <paddlefish/truetype.h>
#include <paddlefish/util.h>

#include <cmath>

namespace paddlefish {

Font::Font(const std::string &name):
  type(Type::STANDARD_TYPE_1),
  base_font(name) {}

Font::Font(Type font_type, const TrueTypeFont* font, unsigned embed)
:
  type(font_type),
  base_font(font->get_font_name()),
  base_font_tag(generate_tag()),
  first_char(font->get_first_char()),
  last_char(font->get_last_char()),
  widths_ref(0),
  font_descriptor_ref(0),
  font_file_ref(0),
  embedding(embed)
{
PADDLEFISH_ONLY_TRUETYPE(
  // TODO
)
}

const std::string Font::generate_tag()
{
  std::string tag;
  for (int i = 0; i < 6; ++i)
  {
    tag += 'A' + rand() % 26;
  }
  return tag;
}

const std::string Font::get_widths() const
{
PADDLEFISH_ONLY_TRUETYPE(
  // TODO
  return std::string();
)
}

const std::string Font::get_font_descriptor() const
{
PADDLEFISH_ONLY_TRUETYPE(
  // TODO
  return std::string();
)
}

const std::string Font::get_file_name() const
{
PADDLEFISH_ONLY_TRUETYPE(
  // TODO
  return std::string();
)
}

const std::string Font::get_font_program() const
{
PADDLEFISH_ONLY_TRUETYPE(
  // TODO
  return std::string();
)
}

void Font::compute_font_bbox()
{
PADDLEFISH_ONLY_TRUETYPE(
  // TODO
  font_bbox = (double*)malloc(4 * sizeof(double));

  return;
)
}

const std::string Font::get_contents() const
{
  std::string contents = std::string("<< /Type /Font\n   /BaseFont /");

  // Not sure whether spaces in the name must be converted to #20 or removed.
  // For multiple master fonts (PDF standard 1.4, page 320), spaces must be
  // converted to underscores.
  contents += util::format_name(get_name()) + "\n";

  switch(type)
  {
    case Type::STANDARD_TYPE_1:
      contents += "   /Subtype /Type1\n   /Encoding /WinAnsiEncoding\n";
      break;

    case Type::TRUE_TYPE:
      contents += ("   /Subtype /TrueType\n   /FirstChar " +
        util::to_str(first_char) + "\n   /LastChar " +
        util::to_str(last_char) + "\n   /Widths " +
        util::to_str(widths_ref) + " 0 R\n   /FontDescriptor " +
        util::to_str(font_descriptor_ref) +
        " 0 R\n   /Encoding /WinAnsiEncoding\n");
      break;

    case Type::CID_TYPE_2:
      contents += ("   /Subtype /CIDFontType2\n   /W " +
        util::to_str(widths_ref) + " 0 R\n   /FontDescriptor " +
        util::to_str(font_descriptor_ref) +
        " 0 R\n   /CIDSystemInfo << /Registry (Adobe)\n" +
        "                     /Ordering (Identity)\n" +
        "                     /Supplement 0 >>\n   /CIDToGIDMap " + 
        (map_ref ? util::to_str(map_ref) + " 0 R\n" : "/Identity\n"));
      break;

    default:
      break;
  }
  contents += ">>";

  return contents;
}

std::string Font::get_name() const
{
  if (embedding == SUBSET)
    return base_font_tag + "-" + base_font;
  else
    return base_font;
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
