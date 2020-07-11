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

#include "file_stream.h"
#include "flate.h"
#include "util.h"
#include <fstream>
#include <sstream>

namespace paddlefish {

FileStream::FileStream(const std::string& filename,
                       const std::string& extra_header,
                       bool flate):
  filename(filename), object_number(0), stream_length(0)
{
#ifdef PADDLEFISH_USE_ZLIB
  use_flate = flate;
#else
  use_flate = false;
#endif

  header = "<< ";

  if (use_flate)
  {
    header += "/Filter [/FlateDecode]\n   ";
  }

  if (extra_header != std::string())
  {
    header += extra_header + "\n   ";
  }
}

std::ostream& FileStream::to_stream(std::ostream& os) const
{
  // We complete the header here. We couldn't do it before because the object
  // number was not computed at construction time.
  os << header << "/Length " << util::to_str(object_number + 1) << " 0 R\n>>"
    << "\nstream\n";
  
  unsigned lStreamStart = os.tellp();

#ifdef PADDLEFISH_USE_ZLIB
  if (use_flate)
  {
    flate::deflate_file_to_stream(os, filename);
  }
  else
#endif
  {
    // Open the input file.
    std::ifstream lFile(filename, std::ios_base::in | std::ios_base::binary);

    // If the file doesn't exist, just return the ostream as it is.
    if (lFile.good())
    {
      os << lFile.rdbuf();

      lFile.close();
    }
  }

  stream_length = (unsigned)os.tellp() - lStreamStart;

  os << "\nendstream";

  return os;
}

const std::string FileStream::get_contents() const
{
  std::stringstream ss;

  to_stream(ss);

  return ss.str();
}

std::ostream& FileStream::length_to_stream(std::ostream& os) const
{
  os << (object_number + 1) << " 0 obj\n   " << stream_length << "\nendobj\n";

  return os;
}

} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
