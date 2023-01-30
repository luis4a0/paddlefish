// Copyright (c) 2022-2023 Luis Peñaranda. All rights reserved.
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

#include <paddlefish/truetype.h>

// The following only expands to the implementation of the functions of the
// STB TrueType library.
#ifdef PADDLEFISH_USE_STB_TRUETYPE
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#endif

paddlefish::TrueTypeFont::TrueTypeFont(std::filesystem::path& file, int index) :
file_path(file)
{
PADDLEFISH_ONLY_TRUETYPE(
  auto file_size = std::filesystem::file_size(file);
  font_data =
    (const unsigned char*)malloc(file_size * sizeof(const unsigned char));

  FILE *source;
  auto fd = PADDLEFISH_FOPEN(source, file.generic_string().c_str(), "rb");
  fread((void*)font_data, 1, file_size, source);
  fclose(fd);

  if (!stbtt_InitFont(&font_info,
                      font_data,
                      stbtt_GetFontOffsetForIndex(font_data, index)))
    throw std::runtime_error("Error initializing TrueType font");
)
}

paddlefish::TrueTypeFont::~TrueTypeFont()
{
  free((void*)font_data);
}

std::string paddlefish::TrueTypeFont::get_file_name() const
{
  return file_path.generic_string();
}

const unsigned char* paddlefish::TrueTypeFont::get_font_data() const
{
  return font_data;
}

std::string paddlefish::TrueTypeFont::get_font_name() const
{
PADDLEFISH_ONLY_TRUETYPE(
  int length;
  int platformID = 1;
  int encodingID = 0;
  int languageID = 0;
  int nameID = 0x006;
  const char* ttf_name =
    stbtt_GetFontNameString(&font_info, &length, platformID, encodingID,
                            languageID, nameID);

  return std::string(ttf_name, length);
)
}

wchar_t paddlefish::TrueTypeFont::get_first_char() const
{
PADDLEFISH_ONLY_TRUETYPE(
  for (auto i = 0; i < font_info.numGlyphs; ++i)
      if (!stbtt_IsGlyphEmpty(&font_info, i))
          return i;

  throw std::runtime_error("Font contains no glyphs");
)
}

wchar_t paddlefish::TrueTypeFont::get_last_char() const
{
PADDLEFISH_ONLY_TRUETYPE(
  for (auto i = font_info.numGlyphs; i > 0; --i)
      if (!stbtt_IsGlyphEmpty(&font_info, i))
          return i;

  throw std::runtime_error("Font contains no glyphs");
)
}

float paddlefish::TrueTypeFont::get_scale() const
{
PADDLEFISH_ONLY_TRUETYPE(
  return stbtt_ScaleForMappingEmToPixels(&font_info, 1000.f);
)
}

void paddlefish::TrueTypeFont::get_bbox(double* font_bbox) const
{
PADDLEFISH_ONLY_TRUETYPE(
  int x0 = 0;
  int y0 = 0;
  int x1 = 0;
  int y1 = 0;

  stbtt_GetFontBoundingBox(&font_info, &x0, &y0, &x1, &y1);

  font_bbox[0] = (double)x0;
  font_bbox[1] = (double)y0;
  font_bbox[2] = (double)x1;
  font_bbox[3] = (double)y1;
)
}

void paddlefish::TrueTypeFont::get_vmetrics(double* ascent,
                                            double* descent,
                                            double* line_gap) const
{
PADDLEFISH_ONLY_TRUETYPE(
  int ascent_int;
  int descent_int;
  int line_gap_int;

  stbtt_GetFontVMetrics(&font_info, &ascent_int, &descent_int, &line_gap_int);

  *ascent = (double)ascent_int;
  *descent = (double)descent_int;
  *line_gap = (double)line_gap_int;
)
}

void paddlefish::TrueTypeFont::get_widths(wchar_t first_char,
                                          wchar_t last_char,
                                          long* widths) const
{
PADDLEFISH_ONLY_TRUETYPE(

  for (wchar_t g = first_char; g <= last_char; ++g)
  {
    if (stbtt_IsGlyphEmpty(&font_info, g))
      widths[g - first_char] = 0;
    else
    {
      int advance_width;
      stbtt_GetCodepointHMetrics(&font_info, g, &advance_width, NULL);
      widths[g - first_char] = (long)advance_width;
    }
  }
)
}

int paddlefish::TrueTypeFont::get_flags() const
{
  return 4; // TODO
}

int paddlefish::TrueTypeFont::get_italic_angle() const
{
  return -12; // TODO
}

int paddlefish::TrueTypeFont::get_cap_height() const
{
PADDLEFISH_ONLY_TRUETYPE(
  int cap_e_height;
  int ret;

  ret =
    stbtt_GetCodepointBox(&font_info, 'E', NULL, NULL, NULL, &cap_e_height) ||
    stbtt_GetCodepointBox(&font_info, 'F', NULL, NULL, NULL, &cap_e_height) ||
    stbtt_GetCodepointBox(&font_info, 'I', NULL, NULL, NULL, &cap_e_height);

  if (ret)
    return cap_e_height;

  return 0;
)
}

int paddlefish::TrueTypeFont::get_stemv() const
{
  /* We need some heuristic for the StemV. We can use PDFlib-lite values, see
  https://fossies.org/dox/PDFlib-Lite-7.0.5p3/ft__font_8h.html for reference:
     FNT_STEMV_MIN   50
     FNT_STEMV_LIGHT   71
     FNT_STEMV_NORMAL   109
     FNT_STEMV_MEDIUM   125
     FNT_STEMV_SEMIBOLD   135
     FNT_STEMV_BOLD   165
     FNT_STEMV_EXTRABOLD   201
     FNT_STEMV_BLACK   241 */
  return 109; // TODO: get the heuristic based on the font name.
}
