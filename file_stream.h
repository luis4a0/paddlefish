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

#ifndef PADDLEFISH_FILE_STREAM_H
#define PADDLEFISH_FILE_STREAM_H

#include "pdf_object.h"
#include <string>
#include <ostream>
#include <memory>

namespace paddlefish {

class FileStream;

typedef std::shared_ptr<FileStream> FileStreamPtr;

class FileStream: public PdfObject
{
public:

  FileStream(const std::string& filename,
             const std::string& extra_header = std::string(),
             bool flate = false);

  ~FileStream() {}

  void set_object_number(unsigned n) { object_number = n; }

  unsigned get_object_number() { return object_number; }

  bool uses_flate() const { return use_flate; }

  Type get_type() const { return Type::FILE_STREAM; }

  std::ostream& to_stream(std::ostream& os) const;

  const std::string get_contents() const;

  std::ostream& length_to_stream(std::ostream& os) const;

protected:
  // The file which will be copied to the stream.
  std::string filename;

  // A string containing the object data, as it will be output to the PDF.
  std::string header;

  // Use flate compression for object contents.
  bool use_flate;

  unsigned object_number;

  // The length of the stream object. I did this mutable to compute when
  // writing to a stream, which is a const function.
  mutable unsigned stream_length;
};

} // namespace paddlefish

#endif // PADDLEFISH_FILE_STREAM_H

// vim: ts=2:sw=2:expandtab
