// Copyright (c) 2022 Luis Peñaranda. All rights reserved.
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

namespace paddlefish {

struct TrueTypeFont
{
  TrueTypeFont(std::filesystem::path& file, int offset = 0)
  {
  PADDLEFISH_ONLY_TRUETYPE(
    auto file_size = std::filesystem::file_size(file);
    font_data =
      (const unsigned char*)malloc(file_size * sizeof(const unsigned char));

    FILE *source;
    PADDLEFISH_FOPEN(source, file.generic_string().c_str(), "rb");
    fread((void*)font_data, 1, file_size, source);

    if (!stbtt_InitFont(&font_info, font_data, 0))
      throw std::runtime_error("Error initializing TrueType font");
  )
  }

  ~TrueTypeFont()
  {
  PADDLEFISH_ONLY_TRUETYPE(
    free((void*)font_data);
  )
  }

  std::string get_font_name() const
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

  wchar_t get_first_char() const
  {
  PADDLEFISH_ONLY_TRUETYPE(
    // TODO
    return 0;
  )
  }

  wchar_t get_last_char() const
  {
  PADDLEFISH_ONLY_TRUETYPE(
    // TODO
    return 0;
  )
  }

  stbtt_fontinfo font_info;
  const unsigned char* font_data;
};

} // namespace paddlefish

#endif // PADDLEFISH_TRUETYPE_H

// vim: ts=2:sw=2:expandtab
