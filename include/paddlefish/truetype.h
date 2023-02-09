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

#ifndef PADDLEFISH_TRUETYPE_H
#define PADDLEFISH_TRUETYPE_H

#ifdef PADDLEFISH_USE_STB_TRUETYPE
#include <stb_truetype.h>
#define PADDLEFISH_ONLY_TRUETYPE(code) code
#else
#include <stdexcept>
typedef void* stbtt_fontinfo;
#define PADDLEFISH_ONLY_TRUETYPE(code) \
throw std::runtime_error("TrueType fonts are not supported");
#endif

#ifdef PADDLEFISH_WINDOWS
#define PADDLEFISH_FOPEN(source, file, mode) \
    fopen_s(&source, file, "rb");
#else
#define PADDLEFISH_FOPEN(source, file, mode) \
    source = fopen(file, "rb");
#endif

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <memory>

namespace paddlefish {

class TrueTypeFont;
typedef typename std::shared_ptr<TrueTypeFont> TrueTypeFontPtr;

class TrueTypeFont
{
  public:
  TrueTypeFont(std::filesystem::path& file, int index = 0);
  ~TrueTypeFont();

  std::string get_file_name() const;
  const unsigned char*  get_font_data() const;
  std::string get_font_name() const;
  wchar_t get_first_char() const;
  wchar_t get_last_char() const;
  uint16_t get_glyph_index(uint16_t c) const;
  float get_scale() const;
  void get_bbox(double* font_bbox) const;
  void get_vmetrics(double* ascent, double* descent, double* line_gap) const;
  void get_widths(wchar_t first_char, wchar_t last_char, long* widths) const;
  int get_flags() const;
  int get_italic_angle() const;
  int get_cap_height() const;
  int get_stemv() const;

  private:
  std::filesystem::path& file_path;
  const unsigned char* font_data;
  [[ maybe_unused ]] stbtt_fontinfo font_info;
};
} // namespace paddlefish

#endif // PADDLEFISH_TRUETYPE_H

// vim: ts=2:sw=2:expandtab
