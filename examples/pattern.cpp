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
#include <sstream>
#include "../document.h"

int main()
{
  paddlefish::DocumentPtr d(new paddlefish::Document());

  paddlefish::Info& info = d->get_info();
  info.title = "Tiling pattern test";
  info.author = "Luis";

  paddlefish::PagePtr p(new paddlefish::Page());

  p->set_mediabox(0, 0, INCHES(6), INCHES(2));

  // We will define an image in device gray colorspace.
  unsigned char image_bytes[] = { 127, 0, 127, 0, 127, 0, 127, 0 };

  // And a soft mask with pixel opacities.
  unsigned char soft_mask[]{ 255, 0, 255, 0, 127, 63, 31, 15 };

  // We add the image as document resource. This means that the image will
  // not be drawn in the document if we don't instruct PDF to do that.
  unsigned image_id = d->add_image_resource(image_bytes,
                                            soft_mask,
                                            8,
                                            1,
                                            4, 2,
                                            INCHES(2), INCHES(1),
                                            COLORSPACE_DEVICEGRAY,
                                            false);

  // Create a resource dictionary and add the image to it.
  paddlefish::ResourcesDictPtr resources(new paddlefish::ResourcesDict());
  resources->add_image(image_id);

  // Construct a tiling, starting by its contents.
  std::string tiling_str("q\n4 0 0 2 0 0 cm\n/Im");
  tiling_str += std::to_string(image_id) + " Do\nQ";

  // Create a pattern to be used for stroking or non-stroking operations.
  double matrix[] = { 1, 0, 0, 1, 0, 0 };
  double bbox[] = { 0, 0, 4, 2 };
  unsigned id_pattern = d->add_tiling_pattern(tiling_str,
                                              resources,
                                              bbox,
                                              4,
                                              2,
                                              matrix,
                                              false);

  // Set the colorspace for non-stroking operations to DeviceRGB.
  p->set_colorspace(NONSTROKING, COLORSPACE_DEVICERGB);

  // Set the color to blue. We use three components because the current
  // colorspace uses them.
  p->set_color(NONSTROKING, 0, 0, 1);

  // Add a font to the document.
  unsigned id_helvetica = d->add_standard_type1_font("Helvetica-Bold");

  // Write something.
  p->add_text(id_helvetica, 48, INCHES(.5), INCHES(.5), "OVERLAP");

  // Set the pattern as the color for non-stroking operations on the current
  // page.
  p->set_pattern(NONSTROKING, id_pattern);

  // Add the text to the page, using the non-stroking parameters defined
  // above.
  p->add_text(id_helvetica, 96, INCHES(1), INCHES(.5), "Ñandú");

  d->push_back_page(p);

  std::ofstream f("pattern.pdf", std::ios_base::out | std::ios_base::binary);
  d->to_stream(f);
  f.close();

  return 0;
}
