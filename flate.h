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

#ifndef PADDLEFISH_FLATE_H
#define PADDLEFISH_FLATE_H

#include <string>
#include <ostream>

namespace paddlefish {
namespace flate{

#ifdef PADDLEFISH_USE_ZLIB

// Deflate a string with zlib.
std::string deflate_string(const std::string& str);
std::string deflate_buffer(const char *buffer, unsigned length);

// Inflate a string with zlib or, if it's not present, with puff. The
// second parameter is the number of times the input size is allocated
// to write the output.
std::string inflate_string(const std::string&,unsigned=10);

// Deflate the file and write it to the output stream. This can be done by
// saving the file in a string, then using deflate_string and sending the
// string to the ostream. But that's slow, this function only uses a small
// buffer as intermediate data structure and is thus faster.
std::ostream& deflate_file_to_stream(std::ostream& out_stream,
                                     const std::string& filename);

// Deflate a buffer and send it to the output stream. Same considerations
// as for the above function.
std::ostream& deflate_buffer_to_stream(std::ostream& out_stream,
                                       char *buffer,
                                       unsigned length);

// Maximum bound for the bytes needed to deflate an input of a given size.
unsigned deflate_bound(unsigned size);

// Deflate a given buffer. The destination buffer must have allocated
// dstLenght bytes. Returns the actual number of bytes written in dst.
unsigned deflate_buffer_to_buffer(const char *src,
                                  unsigned src_length,
                                  char *dst,
                                  unsigned dst_length);

#endif // PADDLEFISH_USE_ZLIB

} // namespace flate
} // namespace paddlefish

#endif // PADDLEFISH_FLATE_H

// vim: ts=2:sw=2:expandtab
