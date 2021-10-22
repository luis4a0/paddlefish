// Copyright (c) 2021 Luis Peñaranda. All rights reserved.
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

#include <cstring>
#include <iostream>
#include <fstream>
#include <jerror.h>
#include <jpeglib.h>
#include <sstream>
#include <string>

#include "../document.h"

int main(int argc, char *argv[])
{
  std::string input_file, output_file;
  double img_x_mm = -1., img_y_mm = -1.;

  for (auto i = 1; i < argc; ++i)
  {
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
    {
      std::cerr <<
        argv[0] << "embeds a jpeg file on a single-page PDF document.\n"
        "usage: " << argv[0] << "options\nwhere options are zero or more of:\n"
        "-i, --input input\tinput image name\n"
        "-o, --output output\toutput image name\n"
        "-x, --size-x mm\thorizontal size of the image in milimeters\n"
        "-y, --size-y mm\tvertical size of the image in milimeters\n"
        "-h, --help\tshow this message\n";

      return -2;
    }

    if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input"))
    {
      input_file = std::string(argv[i + 1]);
      ++i;
    }

    if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output"))
    {
      output_file = std::string(argv[i + 1]);
      ++i;
    }

    if (!strcmp(argv[i], "-x") || !strcmp(argv[i], "--size-x"))
    {
      img_x_mm = atof(argv[i + 1]);
      ++i;
    }

    if (!strcmp(argv[i], "-y") || !strcmp(argv[i], "--size-y"))
    {
      img_y_mm = atof(argv[i + 1]);
      ++i;
    }
  }

  if (input_file.empty() || output_file.empty())
  {
    std::cerr << "Not enough arguments, use \"" << argv[0] << " --help\"." << std::endl;

    return -3;
  }

  paddlefish::DocumentPtr d(new paddlefish::Document());

  paddlefish::PagePtr p(new paddlefish::Page());

  // TODO: read these parameters from the arguments.
  double page_x_mm = 210.;
  double page_y_mm = 297.;

  // Compute image size using libjpeg. we need libjpeg only for this, because
  // the image stream will be copied as-is by Paddlefish.
  jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr err;

  jpeg_create_decompress(&cinfo);
  cinfo.err = jpeg_std_error(&err);

  FILE* infile;
  if ((infile = fopen(input_file.c_str(), "rb")) == NULL)
  {
    std::cerr << "can't open input file \"" << input_file << "\"" << std::endl;

    return -1;
  }

  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, 0);
  fclose(infile);

  // Done with libjpeg.

  // Compute the image size based on the resolution in the jpg.
  if (img_x_mm == -1. && img_y_mm == -1.)
  {
    // Assume density is specified in DPI. Convert it to dots per mm.
    double x_density_dpmm = (double)cinfo.X_density / 25.4;
    double y_density_dpmm = (double)cinfo.Y_density / 25.4;

    img_x_mm = cinfo.image_width / x_density_dpmm;
    img_y_mm = cinfo.image_height / y_density_dpmm;
  }
  // Compute the missing dimensions to maintain aspect ratio in case one size
  // was not specified.
  else if (img_x_mm == -1.)
    img_x_mm = (double)cinfo.image_width * img_y_mm / cinfo.image_height;
  else
    img_y_mm = (double)cinfo.image_height * img_x_mm / cinfo.image_width;

  // Compute the margins needed to center the image on page.
  double margin_x_mm = (page_x_mm - img_x_mm) / 2.;
  double margin_y_mm = (page_y_mm - img_y_mm) / 2.;

  // Set the page size.
  p->set_mediabox(0, 0, MILIMETERS(page_x_mm), MILIMETERS(page_y_mm));

  // Add the jpeg image to the page.
  p->add_jpeg_image(input_file,
                    cinfo.image_width, cinfo.image_height,
                    MILIMETERS(margin_x_mm), MILIMETERS(margin_y_mm), MILIMETERS(img_x_mm), MILIMETERS(img_y_mm),
                    COLORSPACE_DEVICERGB);

  d->push_back_page(p);

  std::ofstream f(output_file, std::ios_base::out|std::ios_base::binary);
  d->to_stream(f);
  f.close();

  return 0;
}
