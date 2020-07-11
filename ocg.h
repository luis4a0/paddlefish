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

#ifndef PADDLEFISH_OCG_H
#define PADDLEFISH_OCG_H

#include <string>
#include <memory>

namespace paddlefish {

struct Ocg;

typedef std::shared_ptr<Ocg> OcgPtr;

struct Ocg
{
  std::string name;
  std::string internal_name;
  unsigned object_number;
  unsigned nesting_level;

  Ocg(const std::string& ocg_name, unsigned obj_number, unsigned level):
    name(ocg_name),
    internal_name(format_name(ocg_name)),
    object_number(obj_number),
    nesting_level(level) {}

  std::string format_name(const std::string& f_name);
};

} // namespace paddlefish

#endif // PADDLEFISH_OCG_H

// vim: ts=2:sw=2:expandtab
