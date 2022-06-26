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

// The icc files used in this test are in the icc-profiles-free Ubuntu
// package, https://packages.ubuntu.com/focal/icc-profiles-free .

#include <paddlefish/paddlefish.h>

#include <fstream>
#include <iostream>
#include <sstream>

int main()
{
  paddlefish::DocumentPtr d(new paddlefish::Document());

  paddlefish::Info& info = d->get_info();
  info.title = "Image in different color spaces";
  info.author = "Luis";

  paddlefish::PagePtr p(new paddlefish::Page());

  // Add an ICC color profile from a file.
  unsigned id_srgb = d->add_icc_color_profile("../resources/sRGB.icc", 3);

  // Add Adobe RGB.
  unsigned id_adobergb = d->add_icc_color_profile("../resources/compatibleWithAdobeRGB1998.icc", 3);

  // Add a CIE-based color profile specifying some parameters.
  double white_point[] = { .9505,1.,1.0890 };
  double matrix[] = { .4497,.2446,.0252,
    .3163,.6720,.1412,
    .1845,.0833,.9227 };
  unsigned id_calrgb = d->add_calrgb_color_profile(white_point, NULL, NULL, matrix);

  // Add an image using the sRGB profile.
  std::string color_img_file = "../resources/orgonite.jpg";
  p->add_jpeg_image(color_img_file,
                    1280, 1280,
                    INCHES(.5), INCHES(6), INCHES(3), INCHES(3),
                    id_srgb);

  // Add the same image using the CalRGB profile.
  p->add_jpeg_image(color_img_file,
                    1280, 1280,
                    INCHES(4.5), INCHES(6), INCHES(3), INCHES(3),
                    id_calrgb);

  // And using the default profile.
  p->add_jpeg_image(color_img_file,
                    1280, 1280,
                    INCHES(.5), INCHES(1.5), INCHES(3), INCHES(3),
                    id_adobergb);

  // And using the default profile.
  p->add_jpeg_image(color_img_file,
                    1280, 1280,
                    INCHES(4.5), INCHES(1.5), INCHES(3), INCHES(3),
                    COLORSPACE_DEVICERGB);

  // Write the text.
  unsigned id_courier = d->add_standard_type1_font("Courier");
  p->add_text(id_courier, 16, INCHES(.5), INCHES(5.7), "sRGB (from file)");
  p->add_text(id_courier, 16, INCHES(4.5), INCHES(5.7), "CIE-based RGB");
  p->add_text(id_courier, 16, INCHES(.5), INCHES(1.2), "Adobe RGB (from file)");
  p->add_text(id_courier, 16, INCHES(4.5), INCHES(1.2), "default DeviceRGB");

  d->push_back_page(p);

  // A new page for grayscale images.
  paddlefish::PagePtr p2(new paddlefish::Page());

  // Add a gray ICC-based color profile specifying white point.
  unsigned id_calgray = d->add_calgray_color_profile(white_point, NULL, NULL);

  // And using the new profile.
  std::string gray_img_file = "../resources/orgonite_gray.jpg";
  p2->add_jpeg_image(gray_img_file,
                     1280, 1280,
                     INCHES(.5), INCHES(6), INCHES(3), INCHES(3),
                     id_calgray);

  // And using the default profile.
  p2->add_jpeg_image(gray_img_file,
                     1280, 1280,
                     INCHES(4.5), INCHES(6), INCHES(3), INCHES(3),
                     COLORSPACE_DEVICEGRAY);

  // Write down reference for the images.
  p2->add_text(id_courier, 16, INCHES(.5), INCHES(5.7), "CIE-based gray");
  p2->add_text(id_courier, 16, INCHES(4.5), INCHES(5.7), "default DeviceGray");

  d->push_back_page(p2);

  std::ofstream f("basic.pdf",std::ios_base::out|std::ios_base::binary);
  d->to_stream(f);
  f.close();

  return 0;
}
