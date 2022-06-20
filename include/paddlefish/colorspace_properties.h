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

#ifndef PADDLEFISH_COLORSPACE_PROPERTIES_H
#define PADDLEFISH_COLORSPACE_PROPERTIES_H

#include <string>

namespace paddlefish {

#define COLORSPACE_DEVICERGB 0
#define COLORSPACE_DEVICEGRAY 1
#define COLORSPACE_DEVICECMYK 2
#define COLORSPACE_PATTERN 3

struct ColorspaceProperties
{
  enum class Type:std::uint8_t
  {
    DEVICE,
    ICC_BASED,
    CALRGB,
    CALGRAY,
    INDEXED,
    PATTERN,
    SEPARATION,
    OTHER
  };

  ColorspaceProperties(Type, unsigned, const std::string&);

  ColorspaceProperties();

  ~ColorspaceProperties();

  Type        colorspace_type;
  unsigned    colorspace_channels;
  std::string colorspace_string;
};

} // namespace paddlefish

#endif // PADDLEFISH_COLORSPACE_PROPERTIES_H

// vim: ts=2:sw=2:expandtab
