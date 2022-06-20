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

#ifndef PADDLEFISH_CUSTOM_OBJECT_H
#define PADDLEFISH_CUSTOM_OBJECT_H

#include "pdf_object.h"
#include <string>
#include <ostream>
#include <memory>

namespace paddlefish {

class CustomObject;

typedef std::shared_ptr<CustomObject> CustomObjectPtr;

// A custom object is any object given as a text string.
class CustomObject: public PdfObject
{
public:

  CustomObject();

  CustomObject(const std::string& text, bool flate = false);

  ~CustomObject() {}

  bool uses_flate() const { return use_flate; }

  Type get_type() const { return Type::CUSTOM_OBJECT; }

  std::ostream& to_stream(std::ostream &os) const { return os << get_contents(); }

  const std::string get_contents() const { return contents; }

protected:

  // A string containing the object data, as it will be output to the PDF.
  std::string contents;

  // Use flate compression for object contents.
  bool use_flate;
};

} // namespace paddlefish

#endif // PADDLEFISH_CUSTOM_OBJECT_H

// vim: ts=2:sw=2:expandtab
