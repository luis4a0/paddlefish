// Copyright (c) 2017-2023 Luis Peñaranda. All rights reserved.
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
#include <vector>

namespace paddlefish {

Font::Font(const std::string &name):
  type(Type::STANDARD_TYPE_1),
  base_font(name) {}

Font::Font(Type font_type, const TrueTypeFontPtr font, unsigned embed)
:
  type(font_type),
  base_font(font->get_font_name()),
  base_font_tag(generate_tag()),
  ttf(font),
  first_char(ttf->get_first_char()),
  last_char(ttf->get_last_char()),
  scale(ttf->get_scale()),
  widths_ref(0),
  font_descriptor_ref(0),
  font_file_ref(0),
  embedding(embed)
{
PADDLEFISH_ONLY_TRUETYPE(
  if (font_type != Font::Type::TRUE_TYPE && type != Font::Type::CID_TYPE_2)
    throw std::runtime_error("The font type must be TRUE_TYPE or CID_TYPE_2");

  widths = (long*)malloc((last_char - first_char + 1) * sizeof(long));

  font_bbox = (double*)malloc(4 * sizeof(double));
  ttf->get_bbox(font_bbox);

  font_bbox[0] *= scale;
  font_bbox[1] *= scale;
  font_bbox[2] *= scale;
  font_bbox[3] *= scale;

  double line_gap;
  ttf->get_vmetrics(&ascent, &descent, &line_gap);
  ascent *= scale;
  descent *= scale;
  line_gap *= scale;
  leading = ascent - descent + line_gap;

  flags = ttf->get_flags();
  italic_angle = ttf->get_italic_angle();
  cap_height = ttf->get_cap_height();
  stem_v = ttf->get_stemv();
)
}

Font::~Font()
{
#ifdef PADDLEFISH_USE_STB_TRUETYPE
if (type == paddlefish::Font::Type::TRUE_TYPE)
{
  free(widths);
  free(font_bbox);
}
#endif
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

const std::string Font::get_widths()
{
PADDLEFISH_ONLY_TRUETYPE(
  // Construct an array of integers specifying the widths of all the glyphs in
  // the range [first_char, last_char].
  std::string widths_str("[ ");

  ttf->get_widths(first_char, last_char, widths);

  for (auto i = 0; i <= last_char - first_char; ++i)
    widths[i] = (long)nearbyint((long double)widths[i] * scale);

  switch (type)
  {
    case Type::TRUE_TYPE:
      widths_str += util::vector_to_string(widths, last_char - first_char + 1);
      break;

    case Type::CID_TYPE_2:
    {
      // Use brackets because we declare variables here.
      std::vector<wchar_t> from;
      std::vector<wchar_t> to;
      std::vector<bool> repeating;
      from.push_back(first_char);
      repeating.push_back(widths[0] == widths[1]);

      // Construct ranges of repeating or non-repeating characters.
      for (wchar_t g = first_char + 1; g < last_char; ++g)
      {
        if (repeating[repeating.size() - 1])
        {
          while (widths[g - first_char] == widths[g - 1 - first_char] &&
                 g <= last_char)
          {
            ++g;
          }
        }
        else
        {
          while (widths[g - first_char] != widths[g - 1 - first_char] &&
                 g <= last_char)
          {
            ++g;
          }

          // Since we found two equal characters, we need to process again the
          // last one.
          --g;
        }

        repeating.push_back(widths[g - first_char] == widths[g + 1 - first_char]);

        to.push_back(g - 1);

        from.push_back(g);

      }
      to.push_back(last_char);

      size_t rs = repeating.size();

      // Construct the string from the ranges.
      for (size_t i = 0; i < rs; ++i)
      {
        if (repeating[i])
        {
          // Avoid repeated zeroes.
          if (widths[from[i] - first_char] != 0)
          {
            widths_str +=
              util::to_str(from[i]) + " " +
              util::to_str(to[i]) + " " +
              util::to_str(widths[from[i] - first_char]) + " ";
          }
        }
        else
        {
          // Do this only if there is something significant to add. In
          // particular, avoid empty brackets at the end of the array.
          if (from[i] <= to[i]) {
            // Don't avoid zeroes in this case, since it would result in a
            // longer array.
            widths_str += util::to_str(from[i]) + " [ ";
            for (wchar_t j = from[i]; j <= to[i]; ++j) {
              widths_str += util::to_str(widths[j - first_char]) + " ";
            }
            widths_str += "] ";
          }
        }
      }

      break;
    }

    default:
      break;
  }

  widths_str += ']';

  return util::split_string(widths_str, 80);
)
}

const std::string Font::get_font_descriptor() const
{
PADDLEFISH_ONLY_TRUETYPE(
  std::string fd = ("<< /Type /FontDescriptor\n   /FontName /");

  fd += util::format_name(get_name()) +
    "\n   /Flags " + util::to_str(flags) +
    "\n   /FontBBox [ " + util::vector_to_string(font_bbox, 4) +
    " ]\n   /ItalicAngle " + util::to_str(italic_angle) +
    "\n   /Ascent " + util::to_str(ascent) +
    "\n   /Descent " + util::to_str(descent) +
    "\n   /Leading " + util::to_str(leading) +
    "\n   /CapHeight " + util::to_str(cap_height) +
    "\n   /StemV " + util::to_str(stem_v) + "\n";

  if (embedding != NOT_EMBEDDED &&
      (type == Type::TRUE_TYPE || type == Type::CID_TYPE_2))
  {
    fd += "   /FontFile2 " + util::to_str(font_file_ref) + " 0 R\n";
  }

  fd += ">>";

  return fd;
)
}

const std::string Font::get_file_name() const
{
  return ttf->get_file_name();
}

const std::string Font::get_font_program() const
{
PADDLEFISH_ONLY_TRUETYPE(
  if (embedding != NOT_EMBEDDED)
    return std::string(reinterpret_cast<const char*>(ttf->get_font_data()));
  else
    return std::string();
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
