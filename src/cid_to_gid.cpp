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

#include <paddlefish/cid_to_gid.h>

namespace paddlefish {
namespace cid_to_gid{

#define MAP_ADD(_c) map.push_back((unsigned char)_c)

#define MAP_ADD_MANY(_n, _c) map+=std::string(_n, _c)

#define MAP_ADD_RANGE(_f, _l) \
  for(unsigned char i=(unsigned char)_f;i<=(unsigned char)_l;++i)\
  {MAP_ADD(i);MAP_ADD(0x00);}

std::string default_map()
{
  std::string map;

  // The size of the mapping is 0x20000 characters.
  map.reserve(131072);

  MAP_ADD_MANY(11, 0x00);
  MAP_ADD(0xe6); MAP_ADD(0x00);
  MAP_ADD(0xe7); MAP_ADD_MANY(3, 0x00);
  MAP_ADD(0x08); MAP_ADD_MANY(3, 0x00);
  MAP_ADD_RANGE(0x04, 0x07); MAP_ADD_MANY(22, 0x00);
  MAP_ADD_RANGE(0xe1, 0xe3);
  MAP_ADD(0xdf); MAP_ADD(0x00);
  MAP_ADD(0xe0); MAP_ADD(0x00);
  MAP_ADD_RANGE(0xe4, 0xe5);
  MAP_ADD(0x03); MAP_ADD(0x00);
  MAP_ADD_RANGE(0x09, 0x66); MAP_ADD_MANY(66, 0x00);
  MAP_ADD(0x03); MAP_ADD(0x00);
  MAP_ADD_RANGE(0x80, 0xde); MAP_ADD_MANY(164, 0x00);
  MAP_ADD(0x71); MAP_ADD(0x00);
  MAP_ADD(0x7d); MAP_ADD_MANY(25, 0x00);
  MAP_ADD(0x6f); MAP_ADD(0x00);
  MAP_ADD(0x7b); MAP_ADD_MANY(45, 0x00);
  MAP_ADD(0x7e); MAP_ADD_MANY(51, 0x00);
  MAP_ADD(0x68); MAP_ADD_MANY(615, 0x00);
  MAP_ADD(0x6d); MAP_ADD_MANY(43, 0x00);
  MAP_ADD(0x79); MAP_ADD_MANY(14957, 0x00);
  MAP_ADD_RANGE(0x77, 0x78); MAP_ADD_MANY(6, 0x00);
  MAP_ADD_RANGE(0x72, 0x73);
  MAP_ADD(0x67); MAP_ADD_MANY(3, 0x00);
  MAP_ADD_RANGE(0x74, 0x75);
  MAP_ADD(0x69); MAP_ADD_MANY(3, 0x00);
  MAP_ADD_RANGE(0x6b, 0x6c);
  MAP_ADD(0x76); MAP_ADD_MANY(7, 0x00);
  MAP_ADD(0x6a); MAP_ADD_MANY(19, 0x00);
  MAP_ADD(0x6e); MAP_ADD_MANY(17, 0x00);
  MAP_ADD(0x70); MAP_ADD(0x00);
  MAP_ADD(0x7c); MAP_ADD_MANY(463, 0x00);
  MAP_ADD(0x7a); MAP_ADD_MANY(493, 0x00);
  MAP_ADD(0x96); MAP_ADD_MANY(113612, 0x00);

  return map;
}

#undef MAP_ADD
#undef MAP_ADD_MANY
#undef MAP_ADD_RANGE

std::string create_map(const TrueTypeFont& font)
{
PADDLEFISH_ONLY_TRUETYPE(
  // TODO
  return std::string();
)
}

} // namespace cid_to_gid
} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
