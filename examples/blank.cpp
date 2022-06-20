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

#include <paddlefish/document.h>

#include <fstream>
#include <iostream>

int main()
{
  paddlefish::DocumentPtr d(new paddlefish::Document());

  paddlefish::Info& info = d->get_info();
  info.title = "Blank PDF";
  info.author = "Luis";

  paddlefish::PagePtr p(new paddlefish::Page());

  p->set_mediabox(0, 0, MILIMETERS(210), MILIMETERS(297));

  d->push_back_page(p);

  std::ofstream f("blank.pdf",std::ios_base::out|std::ios_base::binary);
  d->to_stream(f);
  f.close();

  return 0;
}
