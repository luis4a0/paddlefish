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

#ifndef PADDLEFISH_PADDLEFISH_VERSION_H
#define PADDLEFISH_PADDLEFISH_VERSION_H

#include <string>

namespace paddlefish {

int version_major();
int version_minor();
int version_patchlevel();
std::string version();

} // namespace paddlefish

#endif // PADDLEFISH_PADDLEFISH_VERSION_H

// vim: ts=2:sw=2:expandtab
