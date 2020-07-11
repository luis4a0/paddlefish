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

#ifndef PADDLEFISH_PDF_OBJECT_H
#define PADDLEFISH_PDF_OBJECT_H

#include <ostream>
#include <memory>

namespace paddlefish {

struct PdfObject
{
  enum class Type:std::uint8_t
  {
    COMMAND,
    TEXT,
    IMAGE,
    FILE_STREAM,
    COLOR_PROFILE,
    FONT,
    CUSTOM_OBJECT,
    GRAPHICS_STATE,
    UNKNOWN
  };

  virtual Type get_type() const { return Type::UNKNOWN; }

  virtual std::ostream& to_stream(std::ostream &o) const = 0;

  virtual const std::string get_contents() const = 0;
};

} // namespace paddlefish

#endif // PADDLEFISH_PDF_OBJECT_H

// vim: ts=2:sw=2:expandtab
