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

#ifndef PADDLEFISH_PAGE_H
#define PADDLEFISH_PAGE_H

#include "pdf_object.h"
#include "command.h"
#include "text.h"
#include "text_state.h"
#include "image.h"
#include "graphics_state.h"
#include "ocg.h"
#include "resources_dict.h"
#include <ostream>
#include <vector>
#include <memory>

namespace paddlefish {

class Document;

typedef std::shared_ptr<Document> DocumentPtr;

class Page;

typedef std::shared_ptr<Page> PagePtr;

// These definitions convert the argument from inches, milimeters or
// centimeters to user space points (1 point=1/72 inches). We also define
// a identity conversion to points for completeness.
#define INCHES(x)       ((x)*72.)
#define MILIMETERS(x)   ((x)*(72./25.4))
#define CENTIMETERS(x)  ((x)*(72./2.54))
#define POINTS(x)       (x)

class Page{
    public:
        Page();
        ~Page() {}
        Page& operator=(const Page&);

        // This function adds a JPEG image to the page. We need to specify
        // the size of the image because this function does not depend on
        // libjpeg.
        void add_jpeg_image(const std::string &filename,
                            unsigned jpeg_width,
                            unsigned jpeg_height,
                            double x_pos,
                            double y_pos,
                            double width,
                            double weight,
                            unsigned cs = COLORSPACE_DEVICERGB);

        // Analogously to the previous function, this one adds a JPEG image to
        // the page. The position and size of the image are defined by the
        // 2x3 matrix.
        void add_jpeg_image(const std::string &filename,
                            unsigned jpeg_width,
                            unsigned jpeg_height,
                            double *matrix23,
                            unsigned cs = COLORSPACE_DEVICERGB);

        // Adds an image given its bytes, size and position.
        void add_image_bytes(const unsigned char *bytes,
                             const unsigned char *soft_mask, // can be NULL
                             unsigned bpp,
                             unsigned channels,
                             unsigned image_width,
                             unsigned image_height,
                             double x_pos,
                             double y_pos,
                             double width,
                             double height,
                             unsigned cs = COLORSPACE_DEVICERGB,
                             bool flate = true);

        // Adds an image given its bytes and matrix.
        void add_image_bytes(const unsigned char *bytes,
                             const unsigned char *soft_mask, // can be NULL
                             unsigned bpp,
                             unsigned channels,
                             unsigned image_width,
                             unsigned image_height,
                             double *matrix23,
                             unsigned cs = COLORSPACE_DEVICERGB,
                             bool flate = true);

        // Adds an image mask to the document.
        void add_image_mask(const unsigned char *bytes,
                            unsigned image_width,
                            unsigned image_height,
                            double *matrix23,
                            unsigned *decode,
                            bool flate = true);

        // Adds a PDF command to the page.
        void add_command(const std::string &command);

        // Set and get the text state for all the text on the page.
        void set_text_state(const TextStatePtr &new_ts);
        TextStatePtr& get_text_state() { return text_state; }

        // Write some text on the page, but first modifying the current text
        // state with the specified font and size. The distance is given from
        // the bottom-left corner of the page. T can be std::string or
        // std::vector<std::string>.
        template <class T>
        void add_text(unsigned font_id,
                      double size,
                      double pos_x,
                      double pos_y,
                      const T &chars,
                      bool map = false);

        // Write text using the current page text state. T can be std::string
        // or std::vector<std::string>.
        template <class T>
        void add_text(unsigned pos_x,
                      unsigned pos_y,
                      const T &chars,
                      bool map = false);

        // Write text using the current page state and the given text matrix.
        // T can be std::string or std::vector<std::string>.
        template <class T>
        void add_text(double *matrix23, const T &chars, bool map = false);

        // This function sends the page to a stream. Its second parameter
        // is a back insert iterator to a vector storing the offsets to the
        // beginning of the stream.
        std::ostream& to_stream(
                std::ostream &out_stream,
                std::back_insert_iterator<std::vector<unsigned> > inserter);

        // Get and set the page number and label.
        unsigned get_number() const { return page_number; }
        void set_number(unsigned number) { page_number = number; }
        std::string get_label() const { return page_label; }
        void set_label(const std::string &label) { page_label = label; }

        // Sets the media box of the page.
        void set_mediabox(float x_start,
                          float y_start,
                          float x_end,
                          float y_end);

        // Sets the object number assigned to this page and to all the
        // objects contained in it and returns the next free number.
        unsigned set_object_number(unsigned id);

        // Gets the object number assigned to this page.
        unsigned get_object_number()const { return object_number; }

        // Set the document to which this page belongs to.
        void set_document(Document *father);
        void set_document(DocumentPtr father);

        // Add a custom string to the resources dictionary.
        void add_custom_resource(const std::string &aResource);

        // Set the colorspace for stroking or non-stroking operations.
        void set_colorspace(unsigned stroking, unsigned cs_id);

        // Set color for stroking or non-stroking operations. There exist
        // versions of the same function for 1, 3 and 4 channels.
        void set_color(unsigned stroking, double component);
        void set_color(unsigned stroking, double c1, double c2, double c3);
        void set_color(unsigned stroking, double c1, double c2, double c3, double c4);

        // Set a pattern as color for stroking and non-stroking operations.
        void set_pattern(unsigned stroking, unsigned pattern_id);

        // Load a graphics state.
        void set_graphics_state(unsigned gs_id);
        void set_graphics_state(GraphicsStatePtr &gstate);

        // Paint area defined by shading pattern.
        void draw_shading_pattern(unsigned sh_pat_id);

        // Functions related to marked content. The user is responsible for
        // maintaining the marked content balanced.
        void start_marked_content(const std::string &name);
        void end_marked_content();
        const std::vector<OcgPtr>& get_ocgs() const { return ocgs; }

        // Return the resources dictionary of this page.
        ResourcesDictPtr get_resources() const { return rdict; }

        // Add a [image|colorspace|pattern|graphics state|shading|font]
        // to the resources used on this page if it was not added before.
        void add_image_resource(unsigned image_id)
          { rdict->add_image(image_id); }
        void add_colorspace(unsigned cs_id)
          { rdict->add_colorspace(cs_id); }
        void add_pattern(unsigned pattern_id)
          { rdict->add_pattern(pattern_id); }
        void add_graphics_state(unsigned a_gs_id)
          { rdict->add_graphics_state(a_gs_id); }
        void add_shading(unsigned shading_id)
          { rdict->add_shading(shading_id); }
        void add_font(unsigned font_id)
          { rdict->add_font(font_id); }

    private:
        // Given the object number of a colorspace used on this page,
        // update the booleans containing the image types used.
        void set_image_color_information(unsigned cs_id);

        // Gets the image color information from all the images local to
        // this page.
        void gather_image_color_information();

        // These functions help in writing the page resources.
        std::ostream& write_colorspace_resources(std::ostream &out_stream) const;
        template <class T>
        std::ostream& write_resources(std::ostream &out_stream,
                                      const std::string &name,
                                      const std::string &prefix,
                                      const std::vector<T> &elements) const;

        // The page number. TODO: write it on the PDF.
        unsigned page_number;

        // The page label. TODO: write it on the PDF.
        std::string page_label;

        // The media box can be regarded as the size of the page.
        // Usually, it is [0,0,size_x,size_y].
        float mediabox[4];

        // The object number of the current page. It is set when writing
        // the page to a stream.
        unsigned object_number;

        // All page objects in an unique container. We need to store them
        // sequentially, because they can be stacked and, if we store them
        // in different containers, we lose that stacking information.
        std::vector<std::shared_ptr<PdfObject > > page_objects;
        bool has_images_color;
        bool has_images_gray;
        bool has_images_indexed;
        bool has_commands;
        bool has_text;
        unsigned images_count;

        // The resources dictionary contains the font, colorspace, pattern,
        // graphic state, shading and image resources of the page.
        ResourcesDictPtr rdict = ResourcesDictPtr(new ResourcesDict());

        // The extra strings to add to the resource dictionary.
        std::vector<std::string> custom_page_resources;

        // The page text state.
        TextStatePtr text_state = TextStatePtr(new TextState());

        // Marked content. They are different to the rest of the page
        // objects, because they need to be declared also at global level.
        std::vector<OcgPtr> ocgs;
        unsigned marked_content_nesting_level;

        // A pointer to the document where this page was inserted. Note that
        // not setting this pointer will make to_stream() output invalid.
        // We cannot use here a shared pointer, such as DocumentPtr, because
        // that would create cyclic dependences.
        Document *document_ptr;
};

template <class T>
void Page::add_text(unsigned font_id,
                    double size,
                    double pos_x,
                    double pos_y,
                    const T &chars,
                    bool map) {
  TextStatePtr new_ts(get_text_state());
  new_ts->font_id = font_id;
  new_ts->font_size = size;
  set_text_state(new_ts);
  page_objects.push_back(TextPtr(new Text(pos_x, pos_y, chars, map)));
  has_text = true;
  return;
}

template <class T>
void Page::add_text(unsigned pos_x, unsigned pos_y, const T &chars, bool map)
{
  page_objects.push_back(TextPtr(new Text(pos_x, pos_y, chars, map)));
  has_text = true;
  return;
}

template <class T>
void Page::add_text(double *matrix23, const T &chars, bool map)
{
  page_objects.push_back(TextPtr(new Text(matrix23, chars, map)));
  has_text = true;
  return;
}

} // namespace paddlefish

#endif // PADDLEFISH_PAGE_H

// vim: ts=2:sw=2:expandtab
