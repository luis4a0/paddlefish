// Copyright (c) 2017-2023 Luis Peñaranda. All rights reserved.
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

#ifndef PADDLEFISH_CID_TO_GID_H
#define PADDLEFISH_CID_TO_GID_H

#include <string>

#include "truetype.h"

namespace paddlefish {
namespace cid_to_gid{

// The default map is valid for many fonts. But unfortunately, not for all.
std::string default_map();

std::string create_map(TrueTypeFontPtr font);

} // namespace cid_to_gid
} // namespace paddlefish

#endif // PADDLEFISH_CID_TO_GID_H

// vim: ts=2:sw=2:expandtab
