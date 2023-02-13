// Copyright (c) 2023 Luis Peñaranda. All rights reserved.
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

#include <paddlefish/paddlefish.h>

#include <fstream>
#include <iostream>

int main()
{
  paddlefish::DocumentPtr d(new paddlefish::Document());

  paddlefish::Info& info = d->get_info();
  info.title = "TrueType font example";
  info.author = "Luis";

  paddlefish::PagePtr p(new paddlefish::Page());
  p->set_mediabox(0, 0, MILIMETERS(40), MILIMETERS(25));

  std::filesystem::path font_file("../resources/FiraSans-Regular.ttf");

  // Create a pointer to the font.
  paddlefish::TrueTypeFontPtr ttf_ptr(new paddlefish::TrueTypeFont(font_file));

  // Add the font to the document as a TTF.
  unsigned id_ttf1 = d->add_truetype_font(ttf_ptr, true);

  // Add as a Type 2 CID font.
  auto [id_ttf2, _] = d->add_type2_cid_font(ttf_ptr, true);

  p->add_text(id_ttf1, 16, MILIMETERS(5), MILIMETERS(5), "Paddlefish");
  p->add_text(id_ttf2, 16, MILIMETERS(5), MILIMETERS(15), "PDF!");

  d->push_back_page(p);

  try
  {
    std::ofstream f("ttf.pdf",std::ios_base::out|std::ios_base::binary);
    d->to_stream(f);
    f.close();
  }
  catch (std::runtime_error &e)
  {
    std::cout << "Error generating PDF: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
