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

#ifndef PADDLEFISH_TEXT_STATE_H
#define PADDLEFISH_TEXT_STATE_H

#include <ostream>
#include <vector>
#include <string>
#include <memory>

namespace paddlefish {

struct TextState;

typedef std::shared_ptr<TextState> TextStatePtr;

// Text rendering modes (Tr operator).
#define TEXT_FILL             0 // Fill text (default if Tr is not specified).
#define TEXT_STROKE           1 // Stroke text.
#define TEXT_FILL_STROKE      2 // Fill, then stroke text.
#define TEXT_INVISIBLE        3 // Do nothing with text.
#define TEXT_FILL_CLIP        4 // Fill and add path for clipping.
#define TEXT_STROKE_CLIP      5 // Stroke and add path for clipping.
#define TEXT_FILL_STROKE_CLIP 6 // Fill, then stroke and add path for clipping.
#define TEXT_CLIP             7 // Add text to path for clipping.

// When there is no font, we use NO_FONT.
#define NO_FONT 0

// As stated in the PDF standard, the text state comprises those graphics
// state parameters that only affect text. For more information, see section
// 5.2 of the PDF standard version 1.4, page 300.
struct TextState
{
  double char_space;
  double word_space;
  double scale;
  double leading;
  unsigned font_id;
  double font_size;
  unsigned render_mode;
  double rise;

  TextState();
  TextState(double cs, double ws, double s, double l, unsigned fi, double fs, unsigned rm, double r);
  ~TextState();

  // Returns a string containing the PDF commands needed to specify this
  // text state on the page. In case the current text state is specified,
  // it only returns the string containing the commands needed to change
  // the text state, not all of them.
  std::string to_string()const;
  std::string to_string(TextStatePtr &current_ts)const;
};

} // namespace paddlefish

#endif // PADDLEFISH_TEXT_STATE_H

// vim: ts=2:sw=2:expandtab
