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

#include "color_profile.h"
#include "colorspace_properties.h"
#include "util.h"
#include <sstream>

namespace paddlefish {

ColorProfile::ColorProfile(ColorProfile::Type t,
                           double *white_point,
                           double *black_point,
                           double *gamma,
                           double *matrix)
{
  if (t != ColorProfile::Type::CALRGB && t != ColorProfile::Type::CALGRAY)
  {
    throw std::runtime_error("color profile must be CALRGB or CALGRAY");
  }

  if (ColorProfile::Type::CALRGB == t)
  {
    contents = "[ /CalRGB\n";
  }
  else
  {
    contents = "[ /CalGray\n";
  }
  contents += "<< /WhitePoint [ " +
    util::vector_to_string(white_point, 3) + " ]\n";
  if (black_point)
  {
    contents += "   /BlackPoint [ " +
      util::vector_to_string(black_point, 3) + " ]\n";
  }
  if (gamma)
  {
    contents += "   /Gamma [ " +
      util::vector_to_string(gamma, 3) + " ]\n";
  }
  if (matrix && ColorProfile::Type::CALRGB == t)
  {
    contents += "   /Matrix [";
    for (size_t i = 0; i < 3; ++i)
    {
      for (size_t j = 0; j < 3; ++j)
      {
        contents += " " + util::to_str(matrix[3 * i + j]);
      }
      contents += "\n            ";
    }
    contents += "]\n";
  }
  contents += ">>\n]";
}

ColorProfile::ColorProfile(ColorProfile::Type t,
                           unsigned base,
                           unsigned char **colors,
                           unsigned num_colors,
                           unsigned base_components)
{
  if (t != ColorProfile::Type::INDEXED)
  {
    throw std::runtime_error("color profile must be INDEXED");
  }

  contents = "[ /Indexed\n  ";
  switch (base)
  {
  case COLORSPACE_DEVICERGB:
    contents += "/DeviceRGB";
    break;
  case COLORSPACE_DEVICEGRAY:
    contents += "/DeviceGray";
    break;
  case COLORSPACE_DEVICECMYK:
    contents += "/DeviceCMYK";
    break;
  default:
    contents += util::to_str(base) + " 0 R";
    break;
  }
  contents += "\n  " + util::to_str(num_colors - 1) + "\n  (";
  std::string colors_string;
  std::stringstream colors_stream(colors_string);
  for (unsigned color = 0; color < num_colors; ++color)
  {
    for (unsigned component = 0; component < base_components; ++component)
    {
      unsigned char c = colors[color][component];
      colors_stream << c;
    }
  }
  contents += colors_stream.str() + ")\n]";
}

ColorProfile::ColorProfile(Type t, unsigned stream_id)
{
  if (t != ColorProfile::Type::ICC)
  {
    throw std::runtime_error("color profile must be ICC");
  }

  contents = "[ /ICCBased " + util::to_str(stream_id) + " 0 R ]";
}

ColorProfile::ColorProfile(Type t, std::string name, unsigned alt_cs_id, unsigned alt_f_id)
{
  if (t != ColorProfile::Type::SEPARATION)
  {
    throw std::runtime_error("color profile must be SEPARATION");
  }

  contents = "[ /Separation /" + util::format_name(name) + " " +
    util::color_profile_ref(alt_cs_id) + " " + util::to_str(alt_f_id) +
    " 0 R ]";
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
