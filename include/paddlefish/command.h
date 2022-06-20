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

#ifndef PADDLEFISH_COMMAND_H
#define PADDLEFISH_COMMAND_H

#include "pdf_object.h"
#include <ostream>
#include <string>
#include <memory>

#define NONSTROKING 0
#define STROKING 1

namespace paddlefish {

class Command;

typedef std::shared_ptr<Command> CommandPtr;

class Command: public PdfObject
{
  public:
    Command(const std::string&);

    ~Command() {}

    Type get_type() const { return Type::COMMAND; }

    std::ostream& to_stream(std::ostream& os) const { return os << get_contents(); }

    const std::string get_contents() const { return contents; }

  private:
    std::string contents;
};

} // namespace paddlefish

#endif // PADDLEFISH_COMMAND_H

// vim: ts=2:sw=2:expandtab
