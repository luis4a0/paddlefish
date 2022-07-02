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

#include <paddlefish/text_state.h>
#include <paddlefish/util.h>

namespace paddlefish {

// Set the default text state parameters. All values are set to the defaults
// stated in the PDF standard 1.4, except those of font and font_size, which
// do not have defaults.
TextState::TextState():
  char_space(0.),
  word_space(0.),
  scale(100.),
  leading(0.),
  font_id(NO_FONT),
  font_size(0.),
  render_mode(TEXT_FILL),
  rise(0.) {}

TextState::TextState(double cs, double ws, double s, double l, unsigned fi, double fs, unsigned rm, double r):
  char_space(cs),
  word_space(ws),
  scale(s),
  leading(l),
  font_id(fi),
  font_size(fs),
  render_mode(rm),
  rise(r) {}

TextState::~TextState() {}

std::string TextState::to_string() const
{
  std::string s;

  s += util::to_str(char_space) + " Tc\n" +
    util::to_str(word_space) + " Tw\n" +
    util::to_str(scale) + " Tz\n" +
    util::to_str(leading) + " TL\n";

  if (font_id != NO_FONT)
  {
    s += "/F" + util::to_str(font_id) + ' ' +
      util::to_str(font_size) + " Tf\n";
  }

  s += util::to_str(render_mode) + " Tr\n"+
    util::to_str(rise) + " Ts\n";

  return s;
}

std::string TextState::to_string(TextStatePtr &current_ts) const
{
  std::string s;

  //if (!current_ts || current_ts->char_space != char_space)
    s += util::to_str(char_space) + " Tc\n";

  //if (!current_ts || current_ts->word_space != word_space)
    s += util::to_str(word_space) + " Tw\n";

  //if (!current_ts || current_ts->scale != scale)
    s += util::to_str(scale) + " Tz\n";

  //if (!current_ts || current_ts->leading != leading)
    s += util::to_str(leading) + " TL\n";

  // Always output the font.
  if (font_id != NO_FONT)
  {
    s += "/F" + util::to_str(font_id) + ' ' +
      util::to_str(font_size) + " Tf\n";
  }

  //if (!current_ts || current_ts->render_mode != render_mode)
    s += util::to_str(render_mode) + " Tr\n";

  //if (!current_ts || current_ts->rise != rise)
    s += util::to_str(rise) + " Ts\n";

  return s;
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
