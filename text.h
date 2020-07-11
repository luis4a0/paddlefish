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

#ifndef PADDLEFISH_TEXT_H
#define PADDLEFISH_TEXT_H

#include "pdf_object.h"
#include <ostream>
#include <vector>
#include <string>
#include <memory>

namespace paddlefish {

class Text;

typedef std::shared_ptr<Text> TextPtr;

// An object of class Text represents a text block.
class Text: public PdfObject
{
  public:
    // A line of text using the current text state.
    Text(double x_pos, double y_pos, const std::string &c, bool map = false);

    // Multiple lines of text using the current text state.
    Text(double x_pos, double y_pos, const std::vector<std::string> &text, bool map = false);

    // A line of text using the current text state and the given text matrix.
    Text(double *matrix23, const std::string &c, bool map = false);

    // Writes multiple lines of text using the current text state and the
    // given text matrix.
    Text(double *matrix23, const std::vector<std::string> &text, bool map = false);

    ~Text() {}

    Type get_type() const { return Type::TEXT; }

    std::ostream& to_stream(std::ostream &o) const { return o << get_contents(); }

    const std::string get_contents() const;

    // Add a \000 (character 0x00 in octal) before each character in the
    // text. This is needed to use the default CID to GID map we use for
    // Type 2 CID fonts.
    void add_zeroes();

  private:
    // The first two columns of the 3x3 text matrix. The third column is
    // assumed to be [0, 0, 1], so text_matrix has dimensions 2x3.
    double text_matrix[6];
    // The lines of text.
    std::vector<std::string> lines;
};

} // namespace paddlefish

#endif // PADDLEFISH_TEXT_H

// vim: ts=2:sw=2:expandtab
