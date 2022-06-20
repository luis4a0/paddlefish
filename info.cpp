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

#include <paddlefish/info.h>
#include <paddlefish/util.h>

#include <ctime>

namespace paddlefish {

Info::Info():producer("Paddlefish"),creation_date(current_date()) {}

Info::~Info() {}

// x must be positive or zero.
std::string Info::two_characters(int x)
{
  return (x < 10 ? "0" : "") + util::to_str(x);
}

Info::date Info::current_date()
{
  std::string current("D:");

  // Return the time always in UTC.
  std::time_t t = std::time(0);
  std::tm* now = std::gmtime(&t);

  return current +
         util::to_str(now->tm_year + 1900) +
         two_characters(now->tm_mon + 1) +
         two_characters(now->tm_mday) +
         two_characters(now->tm_hour) +
         two_characters(now->tm_min) +
         two_characters(now->tm_sec) +
         "Z";
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
