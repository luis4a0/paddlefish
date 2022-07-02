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

#include <paddlefish/text.h>
#include <paddlefish/util.h>

namespace paddlefish {

#define COPY_TEXT_MATRIX(_m) \
  { for (size_t i = 0; i < 6; ++i) { text_matrix[i] = (_m)[i]; } }

Text::Text(double x_pos, double y_pos, const std::string &c, bool map)
{
  lines.push_back(c);

  if (map)
  {
   add_zeroes();
  }

  text_matrix[0] = text_matrix[3] = 1.;
  text_matrix[1] = text_matrix[2] = 0.;
  text_matrix[4] = x_pos;
  text_matrix[5] = y_pos;
}

Text::Text(double x_pos, double y_pos, const std::vector<std::string> &text, bool map):
  lines(text)
{
  if (map)
  {
    add_zeroes();
  }

  text_matrix[0] = text_matrix[3] = 1.;
  text_matrix[1] = text_matrix[2] = 0.;
  text_matrix[4] = x_pos;
  text_matrix[5] = y_pos;
}

Text::Text(double *matrix23, const std::string &c, bool map)
{
  COPY_TEXT_MATRIX(matrix23)

  lines.push_back(c);

  if (map)
  {
    add_zeroes();
  }
}

Text::Text(double *matrix23, const std::vector<std::string> &text, bool map):
  lines(text)
{
  COPY_TEXT_MATRIX(matrix23)

  if (map)
  {
    add_zeroes();
  }
}

const std::string Text::get_contents() const
{
  // Start text block.
  std::string contents = "BT\n";

  // Write positioning. If the upper text matrix is identity, only
  // specify text position.
  if (text_matrix[0] == 1. && text_matrix[1] == 0. &&
      text_matrix[2] == 0. && text_matrix[3] == 1.)
  {
    contents += util::to_str(text_matrix[4]) + ' ' +
      util::to_str(text_matrix[5]) + " Td\n";
  }
  else
  {
    contents += util::vector_to_string(text_matrix, 6) + " Tm\n";
  }

  // Write the text lines.
  for (size_t i = 0; i < lines.size(); ++i)
  {
    contents += '(' + util::escape_string(lines[i]) + ") Tj\n";
    if (i < lines.size() - 1)
    {
      contents += "T*\n";
    }
  }

  // End text block.
  contents += "ET\n";

  return contents;
}

void Text::add_zeroes()
{
  for (size_t i = 0; i < lines.size(); ++i)
  {
    std::string line;
    for (size_t c = 0; c < lines[i].size(); ++c)
    {
      line += '\000';
      line += lines[i][c];
    }
    lines[i] = line;
  }

  return;
}

#undef COPY_TEXT_MATRIX

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
