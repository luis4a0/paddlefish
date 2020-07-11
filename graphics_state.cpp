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

#include "graphics_state.h"
#include "util.h"

namespace paddlefish {

GraphicsState::GraphicsState(unsigned type, double a) :
  object_number(0)
  {
    switch (type)
    {
    case GS_STROKING_ALPHA:
      stroking_alpha = a;
      has_stroking_alpha = true;
      has_nonstroking_alpha = false;
      break;
    case GS_NONSTROKING_ALPHA:
      nonstroking_alpha = a;
      has_stroking_alpha = false;
      has_nonstroking_alpha = true;
      break;
    default:
      stroking_alpha = a;
      nonstroking_alpha = a;
      has_stroking_alpha = true;
      has_nonstroking_alpha = true;
      break;
    }
  }

const std::string GraphicsState::get_contents() const
{
  std::string rContents = std::string("<< /Type /ExtGState\n");
 
  if (has_stroking_alpha)
  {
    rContents += "   /CA " + util::to_str(stroking_alpha) + "\n";
  }

  if (has_nonstroking_alpha)
  {
    rContents += "   /ca " + util::to_str(nonstroking_alpha) + "\n";
  }

  rContents += "   /AIS false\n>> ";

  return rContents;
}

std::string GraphicsState::to_string()const
{
  return std::string("/s" + std::to_string(object_number) + " gs\n");
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
