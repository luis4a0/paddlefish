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

#ifndef PADDLEFISH_DOCUMENT_H
#define PADDLEFISH_DOCUMENT_H

#include "page.h"
#include "info.h"
#include "graphics_state.h"
#include "util.h"
#include "colorspace_properties.h"
#include "ocg.h"
#include "resources_dict.h"
#include "pdf_object.h"
#include <ostream>
#include <vector>
#include <unordered_map>

namespace paddlefish {

class Document;

typedef std::shared_ptr<Document> DocumentPtr;

// This class represents a PDF document. It contains a vector of pages,
// functions to add pages and iterate over them and a function to send the
// document to a stream. A document is created by constructing an object of
// type Document and adding pages to it.
class Document{
        typedef typename std::unordered_map<unsigned, ColorspaceProperties>
                                                        colorspace_map;
        typedef typename std::unordered_map<unsigned, unsigned>
                                                        image_resource_map;
    public:
        Document();
        ~Document() {}
        Document& operator=(const Document&);

        // Append an already created page to the end of this document.
        void push_back_page(const PagePtr& page_ptr);

        // Function to add to the document a standard font. Its only
        // parameter is the font name, which is one of:
        // - Times-[Roman|Bold|Italic|BoldItalic],
        // - Helvetica(-[Bold|Oblique|BoldOblique])?,
        // - Courier(-[Bold|Oblique|BoldOblique])?,
        // - Symbol, or
        // - ZapfDingbats.
        // It returns an identifier for the font, which must be used
        // when calling the text writing functions.
        unsigned add_standard_type1_font(const std::string& font_name);

        // Add a Type 0 font with the CID Type 2 font given as argument as
        // descendant.
        unsigned add_type0_font(std::pair<unsigned, std::string> t2_font);

        // Add an ICC color profile read from a file or from a string to the
        // document, with a given number of channels. Return an identifier.
        unsigned add_icc_color_profile(const std::string& filename,
                                       unsigned channels,
                                       bool use_flate = true);
        unsigned add_icc_color_profile(const char *buffer,
                                       unsigned length,
                                       unsigned channels,
                                       bool use_flate = true);

        // Add a CalRGB CIE-based color profile. Parameters are:
        // - white point: array of three doubles
        // - black point: array of three doubles or NULL
        // - gamma: array of three doubles or NULL
        // - matrix: 3x3 matrix of doubles contaning the values
        //   [X_A Y_A Z_A]
        //   [X_B Y_B Z_B]
        //   [X_C Y_C Z_C]
        unsigned add_calrgb_color_profile(double *white_point,
                                          double *black_point,
                                          double *gamma,
                                          double *matrix);

        // Add a CalGray CIE-based color profile. The parameters are the
        // same as add_icc_color_profile, except that there is no matrix.

        unsigned add_calgray_color_profile(double *white_point,
                                           double *black_point,
                                           double *gamma);

        // Add an indexed color profile. The first argument is the identifier
        // of the colorspace on which it is based (that is, on which colors
        // on this colorspace are defined). The second argument is a
        // two-dimensional array containing the indexed colors. Each row of
        // this array contains one color, and the number of components of
        // each color must match the number of components of the base
        // colorspace (which is the fourth argument). The third argument
        // specifies the number of colors (should be at most 256).
        unsigned add_indexed_color_profile(unsigned base,
                                           unsigned char **colors,
                                           unsigned num_colors,
                                           unsigned base_channels);

        // Add a separation color space.
        unsigned add_separation_color_space(std::string name,
                                            unsigned alternate_cs,
                                            unsigned alternate_function);

        // Write the entire document to a stream.
        std::ostream& to_stream(std::ostream &out_stream);

        // To get the object representing document information.
        Info& get_info() { return document_information; }

        // To add a comment to the file
        void add_comment(const std::string &comment);

        // To add custom objects to the document. They will appear in the
        // xref table, but it is responsibility of the user to keep the
        // document consistent. Assuming that all global objects (like
        // fonts and color profiles) are already added, the return value
        // is the object number of this object.
        unsigned add_custom_object(const std::string &object_contents);

        // Add an image to the document. However, it is not plotted b
        // default, it must be referenced by the user. This is useful for
        // patterns. Argument list is analogous to Page::add_image_bytes(),
        // except that there is no position on the page.
        unsigned add_image_resource(const unsigned char *bytes,
                                    const unsigned char *soft_mask,
                                    unsigned bpc,
                                    unsigned channels,
                                    unsigned image_width,
                                    unsigned image_height,
                                    unsigned width,
                                    unsigned height,
                                    unsigned colorspace = COLORSPACE_DEVICERGB,
                                    bool flate = true);

        // Add a custom object containing a stream. The return value is
        // analog to add_custom_object(). The second parameter specifies
        // if the stream must be deflated. The third argument lets the
        // user add some content to the stream header, which by default
        // only contains the stream length.
        unsigned add_custom_stream(const std::string &strm,
                                   const std::string &extra_header = std::string(),
                                   bool flate = true);
        unsigned add_custom_stream(const char *buffer,
                                   unsigned buffer_length,
                                   const char *extra_header,
                                   unsigned extra_headerLength,
                                   bool flate);

        unsigned add_custom_stream_from_file(const std::string &file_name,
                                             const std::string &extra_header,
                                             bool flate = true);

        // If a custom object is a page, it must be entered here to
        // refer to it in the page dictionaries.
        void add_custom_object_page_number(unsigned custom_page);

        // Add a tiling pattern containing the image given as argument, along
        // with its size in pixels. Return the object number of the added
        //pattern.
        unsigned add_texture_pattern(unsigned image_id,
                                     double x_size,
                                     double y_size,
                                     double x_step,
                                     double y_step,
                                     double *matrix,
                                     bool flate = true);

        // Add a tiling pattern containing the pattern in content.
        unsigned add_tiling_pattern(const std::string content,
                                    const ResourcesDictPtr& resources,
                                    double *bbox,
                                    double x_step,
                                    double y_step,
                                    double *matrix,
                                    bool flate = true);

        // Add an exponential interpolation function (type 2, defined in
        // section 3.9.2 on page 113 of the PDf standard 1.4) whose domain
        // and output are d-dimensional. The output value will be
        // interpolated, for a parameter t, as y = c0 + t^n * (c1 - c0).
        // The parameter t ranges in [t0,t1].
        unsigned add_interpolation_function(unsigned d,
                                            double *c0,
                                            double *c1,
                                            double n,
                                            double t0 = 0.0,
                                            double t1 = 1.0);

        // Add a stitching function (type 3, defined in section 3.9.3 on page
        // 113 of the PDF standard 1.4) of k input functions. The resulting
        // function will be defined in the domain [d0,d1]. d0, d1 and
        // bounds need to satisfy that
        // d0 < bounds[0] < ... < bounds[k-2] < d1.
        // d0, bounds and d1 specify the interval on which each subfunction
        // applies. The original domain of the subfunction j will be mapped to
        // [encode[2*j],encode[2*j+1], thus encode will have 2*k elements.
        unsigned add_stitching_function(unsigned k,
                                        unsigned *functions,
                                        double *bounds,
                                        double *encode,
                                        double d0 = 0.0,
                                        double d1 = 1.0);

        // Add a shading to the document. Supported types are 2 (axial) and
        // 3 (radial).
        // TODO: add somewhere definitions for using a name instead of a
        // number to specify the shading type.
        unsigned add_shading(unsigned type,
                             unsigned colorspace_id,
                             double *coords,
                             double t0,
                             double t1,
                             unsigned function_id,
                             bool extend_start = false,
                             bool extend_end = false);

        // Add a shading pattern to the document.
        unsigned add_shading_pattern(unsigned shading_id,
                                     double *matrix = NULL);

        // Add a shading soft mask to the document. Returns the object id of
        // the external graphics state representing it. shading_pattern_id
        // must be defined in the DeviceGray colorspace.
        unsigned add_shading_soft_mask(unsigned sh_pattern_id,
                                       double *coords,
                                       double *matrix = NULL);

        GraphicsStatePtr add_graphics_state();

        // Get the colorspace properties of a given colorspace object.
        const ColorspaceProperties* get_colorspace_properties(unsigned object_id) const;

        // Get the colorspace of a given global image.
        const unsigned get_image_colorspace(unsigned image_id) const;

        void add_ocg_list(const std::vector<OcgPtr> &ocg_vector);

    private:
        // Functions to write the four parts of the document.
        std::ostream& write_header(std::ostream &out_stream);
        std::ostream& write_objects(std::ostream &out_stream);
        std::ostream& write_xref(std::ostream &out_stream);
        std::ostream& write_trailer(std::ostream &out_stream);

    private:
        // Each object has an offset in bytes from the start of the file.
        // We will compute them as long as we output the document and its
        // objects. This is not wrong because these offsets are only used
        // in the cross-reference tables and we will need only one at the
        // end of the document.
        std::vector<unsigned> object_offsets;
        // Usually zero, the position where the stream starts to be written.
        unsigned long start_stream_position;
        // The offset of the cross-reference table, which is used in the
        // document trailer.
        unsigned long xref_stream_position;
        // This variable always contain the number of the next object to be
        // written to the stream.
        unsigned next_object_number;
        // This variable stores the object number of the first font object
        // written to the document. We will use it to determine the object
        // numbers we will reference in the document each time we write
        // something. For simplicity, we will write down all the font
        // objects consecutively.
        unsigned first_body_object_number;
        // The objects used in the document body before starting to
        // describe the pages.
        std::vector<std::shared_ptr<PdfObject> > body_objects;
        // The number of actual objects that all body objects need in order
        // to be written in the file.
        unsigned total_body_objects;
        // The pages of the document.
        std::vector<PagePtr> pages;
        // Document information.
        Info document_information;
        // Comments in the header of the document
        std::vector<std::string> document_comments;
        // The object numbers of the custom pages;
        std::vector<unsigned> custom_object_page_numbers;
        // The information on colorspaces, we identify them with the
        // object number returned when we add them.
        colorspace_map colorspace_properties;
        // The map <image, colorspace> stores to which colorspace each
        // global image belongs.
        image_resource_map image_colorspaces;
        // The optional content groups (OCG's) used in the document.
        std::vector<OcgPtr> ocgs;
};

} // namespace paddlefish

std::ostream& operator<<(std::ostream& out_stream, paddlefish::DocumentPtr doc);

#endif // PADDLEFISH_DOCUMENT_H

// vim: ts=2:sw=2:expandtab
