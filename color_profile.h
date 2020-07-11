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

#ifndef PADDLEFISH_COLOR_PROFILE_H
#define PADDLEFISH_COLOR_PROFILE_H

#include "custom_object.h"
#include <cstdint>
#include <memory>

namespace paddlefish {

class ColorProfile;

typedef std::shared_ptr<ColorProfile> ColorProfilePtr;

class ColorProfile: public CustomObject
{
public:

  enum class Type:std::uint8_t
  {
    ICC,
    CALRGB,
    CALGRAY,
    INDEXED,
    SEPARATION
  };

  // Constructor for CalRGB and CalGray color profiles. For the latter,
  // the last parameter is ignored if present.
  ColorProfile(ColorProfile::Type t,
               double *white_point,
               double *black_point,
               double *gamma,
               double *matrix = NULL);

  // Constructor for indexed color profile.
  ColorProfile(ColorProfile::Type t,
               unsigned base,
               unsigned char **colors,
               unsigned num_colors,
               unsigned base_components);

  // Constructor for ICC color profiles. The contents of the ICC file must be
  // already pasted in the output document. One must specify the object number
  // of this stream.
  ColorProfile(ColorProfile::Type t, unsigned stream_id);

  // Constructor for separation color spaces.
  ColorProfile(ColorProfile::Type t, std::string name, unsigned alt_cs_id, unsigned alt_f_id);

  PdfObject::Type get_type() const { return PdfObject::Type::COLOR_PROFILE; }
};

} // namespace paddlefish

#endif // PADDLEFISH_COLOR_PROFILE_H

// vim: ts=2:sw=2:expandtab
