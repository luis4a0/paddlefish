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

#ifndef PADDLEFISH_GRAPHICS_STATE_H
#define PADDLEFISH_GRAPHICS_STATE_H

#include "pdf_object.h"
#include <ostream>
#include <vector>
#include <string>
#include <memory>

#define GS_STROKING_ALPHA     0
#define GS_NONSTROKING_ALPHA  1

namespace paddlefish {

class GraphicsState;

typedef std::shared_ptr<GraphicsState> GraphicsStatePtr;

class GraphicsState: public PdfObject
{
  public:
  GraphicsState() :
    object_number(0),
    has_stroking_alpha(true), has_nonstroking_alpha(true),
    stroking_alpha(1.0), nonstroking_alpha(1.0) {}

  GraphicsState(double sa, double nsa) :
    object_number(0),
    has_stroking_alpha(true), has_nonstroking_alpha(true),
    stroking_alpha(sa), nonstroking_alpha(nsa) {}

  GraphicsState(unsigned type, double a);

  ~GraphicsState() {}

  // Get or set the object number of the graphics state in the document.
  unsigned get_object_number()const { return object_number; }
  void set_object_number(unsigned n) { object_number = n; }

  Type get_type() const { return Type::GRAPHICS_STATE; }

  // Write to stream the object representing the graphics state.
  // The second argument is the object number.
  std::ostream& to_stream(std::ostream &o) const { return o<<get_contents(); }

  // Get the contents of the object in a string.
  const std::string get_contents() const;

  // Returns a string containing the PDF commands needed to specify this
  // graphics state on the page.
  std::string to_string()const;

  private:
  unsigned object_number;

  public:
  bool has_stroking_alpha, has_nonstroking_alpha;
  double stroking_alpha, nonstroking_alpha;
};

} // namespace paddlefish

#endif // PADDLEFISH_GRAPHICS_STATE_H

// vim: ts=2:sw=2:expandtab
