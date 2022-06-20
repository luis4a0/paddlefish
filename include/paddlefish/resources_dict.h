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

#ifndef PADDLEFISH_RESOURCES_DICT_H
#define PADDLEFISH_RESOURCES_DICT_H

#include <vector>
#include <ostream>
#include <memory>

namespace paddlefish {

class ResourcesDict;

typedef std::shared_ptr<ResourcesDict> ResourcesDictPtr;

// This class represents a resources dictionary. A resource dictionary can
// be used by a page or by a tiling pattern.
class ResourcesDict
{
  public:
  ResourcesDict() {}
  ResourcesDict(const ResourcesDict &resources);
  ~ResourcesDict() {}

  void add_font(unsigned font_id) { add_if_not_exists(font_id, fonts); }

  void add_colorspace(unsigned cs_id) // Add if not device colorspace.
  { if (0 != cs_id) add_if_not_exists(cs_id, colorspaces); }

  void add_pattern(unsigned pat_id) { add_if_not_exists(pat_id, patterns); }

  void add_graphics_state(unsigned gs_id) { add_if_not_exists(gs_id, gstates); }

  void add_shading(unsigned sh_id) { add_if_not_exists(sh_id, shadings); }

  void add_image(unsigned image_id) { add_if_not_exists(image_id, images); }

  const std::vector<unsigned> &get_fonts() const { return fonts; }

  const std::vector<unsigned> &get_colorspaces() const { return colorspaces; }

  const std::vector<unsigned> &get_patterns() const { return patterns; }

  const std::vector<unsigned> &get_graphics_states() const { return gstates; }

  const std::vector<unsigned> &get_shadings() const { return shadings; }

  const std::vector<unsigned> &get_images() const { return images; }

  std::ostream& to_stream(std::ostream &out_stream) const;

  std::string to_string() const;

  private:

  void add_if_not_exists(unsigned elt, std::vector<unsigned> &vec);
    
  template <class T>
  std::ostream& write_resources(std::ostream &out_stream,
                                const std::string &name,
                                const std::string &prefix,
                                const std::vector<T> &elements) const;

  // The objects referenced by commands on this resources dictionary.
  std::vector<unsigned> fonts;
  std::vector<unsigned> colorspaces;
  std::vector<unsigned> patterns;
  std::vector<unsigned> gstates;
  std::vector<unsigned> shadings;
  std::vector<unsigned> images;

}; // class ResourcesDict

} // namespace paddlefish

#endif // PADDLEFISH_RESOURCES_DICT_H

// vim: ts=2:sw=2:expandtab
