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

#include <paddlefish/version.h>

namespace paddlefish {

int version_major()
{
  return PADDLEFISH_VERSION_MAJOR;
}

int version_minor()
{
  return PADDLEFISH_VERSION_MINOR;
}

int version_patchlevel()
{
  return PADDLEFISH_VERSION_MINOR;
}

std::string version()
{
  return std::string{std::to_string(version_major()) + '.' +
                     std::to_string(version_minor()) + '.' +
                     std::to_string(version_patchlevel())};
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
