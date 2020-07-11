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

#include "resources_dict.h"
#include "util.h"
#include <sstream>

namespace paddlefish {

ResourcesDict::ResourcesDict(const ResourcesDict &resources):
fonts(resources.get_fonts()),
colorspaces(resources.get_colorspaces()),
patterns(resources.get_patterns()),
gstates(resources.get_graphics_states()),
shadings(resources.get_shadings()),
images(resources.get_images())
{}

template <class T>
std::ostream& ResourcesDict::write_resources(std::ostream &out_stream,
                                             const std::string &name,
                                             const std::string &prefix,
                                             const std::vector<T> &elements) const
{
  if (!elements.empty())
  {
    std::string pad1, pad2;
    pad1.append(17, ' ');
    pad2.append(22 + name.length(), ' ');

    out_stream << pad1 << '/' << name << " <<\n";
    for (size_t i = 0; i < elements.size(); ++i)
    {
      out_stream << pad2 << '/' << prefix << util::to_str(elements[i]) <<
        " " << util::to_str(elements[i]) << " 0 R\n";
    }
    out_stream << pad1 << ">>\n";
  }

  return out_stream;
}

std::ostream& ResourcesDict::to_stream(std::ostream &out_stream) const
{
  out_stream << "   /Resources <<\n";

  write_resources(out_stream, "Font",       "F",  fonts);
  write_resources(out_stream, "ColorSpace", "CS", colorspaces);
  write_resources(out_stream, "Pattern",    "Pt", patterns);
  write_resources(out_stream, "ExtGState",  "s",  gstates);
  write_resources(out_stream, "Shading",    "sh", shadings);
  write_resources(out_stream, "XObject",    "Im", images);

  out_stream << "   >>";

  return out_stream;
}

std::string ResourcesDict::to_string() const
{
  std::stringstream sstr;

  to_stream(sstr);

  return sstr.str();
}

void ResourcesDict::add_if_not_exists(unsigned elt, std::vector<unsigned> &vec)
{
  for (size_t i = 0; i < vec.size(); ++i)
    if (elt == vec[i])
      return;

  vec.push_back(elt);

  return;
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
