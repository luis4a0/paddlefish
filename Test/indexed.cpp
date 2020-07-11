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

#include <iostream>
#include <fstream>
#include <string>
#include "../document.h"

int main()
{
  paddlefish::DocumentPtr d(new paddlefish::Document());

  paddlefish::Info& info = d->get_info();
  info.title = "Indexed colorspace test";
  info.author = "Luis";

  // Define the base color space.
  std::string icc_file("../resources/sRGB.icc");
  unsigned id_srgb = d->add_icc_color_profile(icc_file, 3);

  // Define the indexed colors. Since the RGB base colorspace has three
  // channels, each row must have three components. We add four colors
  // to the index.
  unsigned char **colors = (unsigned char**)malloc(4 * sizeof(unsigned char*));
  for(size_t i=0; i<4; ++i){
    colors[i] = (unsigned char*)malloc(3 * sizeof(unsigned char));
    colors[i][0] = colors[i][1] = colors[i][2] = 0;
  }
  // The colors array will be { black, red, green, blue}.
  colors[1][0] = 200;
  colors[2][1] = 127;
  colors[3][2] = 255;

  // Add the indexed color profile containing the four defined colors, based
  // on the sRGB profile defined above.
  unsigned id_indexed = d->add_indexed_color_profile(id_srgb,
                                                     (unsigned char**)colors,
                                                     4,
                                                     3);

  paddlefish::PagePtr p(new paddlefish::Page());

  p->set_mediabox(0, 0, INCHES(3), INCHES(2));

  unsigned char image_bytes[] = { 0, 1, 2, 2, 0, 3, 3, 1 };

  // Add the image as 8 bpc, 1 channel, 4x2 raw image. Size will be 2x1 inches
  // and the position (.5,.5) inches from the left-bottom corner of the page.
  double matrix23[] = {INCHES(2), 0, 0, INCHES(1), INCHES(.5), INCHES(.5) };
  p->add_image_bytes(image_bytes,
                     NULL,
                     8,
                     1,
                     4, 2,
                     matrix23,
                     id_indexed);

  d->push_back_page(p);

  std::ofstream f("indexed.pdf", std::ios_base::out | std::ios_base::binary);
  d->to_stream(f);
  f.close();

  for(size_t i=0; i<4; ++i)
    free(colors[i]);
  free(colors);

  return 0;
}
