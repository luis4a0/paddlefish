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

#include <paddlefish/ocg.h>

namespace paddlefish {

std::string Ocg::format_name(const std::string &f_name)
{
  std::string ret_name;

  size_t f_name_length = f_name.length();

  ret_name.reserve(f_name_length);

  for (size_t i = 0; i < f_name_length; ++i)
  {
    char current = f_name[i];
    if (('a' <= current && 'z' >= current) ||
      ('A' <= current && 'Z' >= current) ||
      ('0' <= current && '9' >= current))
    {
      ret_name += current;
    }
  }

  return ret_name;
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
