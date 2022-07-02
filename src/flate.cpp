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

#include <paddlefish/flate.h>

#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace paddlefish {
namespace flate{

#ifdef PADDLEFISH_USE_ZLIB

#include <zlib.h>

#define CHUNK 16384

std::string deflate_string(const std::string& str)
{
  return deflate_buffer(str.c_str(), str.size());
}

// Based on http://panthema.net/2007/0328-ZLibString.html
// See license at http://www.boost.org/LICENSE_1_0.txt
std::string deflate_buffer(const char *buffer, unsigned length)
{
  z_stream z_str;
  memset(&z_str, 0, sizeof(z_str));

  deflateInit(&z_str, Z_BEST_COMPRESSION);
  z_str.next_in = (Bytef*)buffer;
  z_str.avail_in = length;

  int ret_value;

  // We allocate for output the input size.
  // TODO: if this size doesn't work, then inform it so the
  // user writes the data without using flate method.
  char *out = (char*)malloc(length * sizeof(char));

  std::string retstr;
  retstr.reserve(length);

  do {
    z_str.next_out = reinterpret_cast<Bytef*>(out);
    z_str.avail_out = sizeof(out);
    ret_value = deflate(&z_str, Z_FINISH);
    if (retstr.size()<z_str.total_out)
      retstr.append(out, z_str.total_out - retstr.size());
  } while (Z_OK == ret_value);

  deflateEnd(&z_str);

  return retstr;
}

// Based on http://panthema.net/2007/0328-ZLibString.html
// See license at http://www.boost.org/LICENSE_1_0.txt
std::string inflate_string(const std::string& deflated,
                           unsigned times_out_alloc){
    z_stream z_str;

    memset(&z_str,0,sizeof(z_str));
    inflateInit(&z_str);
    z_str.next_in=(Bytef*)deflated.c_str();
    z_str.avail_in=deflated.size();

    int ret_value;

    // We allocate for output times_out_alloc times the input size.
    // TODO: if this size doesn't work, rerun with a bigger parameter
    char *out=(char*)malloc(times_out_alloc*deflated.size()*sizeof(char));

    std::string retstr;
    retstr.reserve(deflated.size());

    do{
        z_str.next_out=reinterpret_cast<Bytef*>(out);
        z_str.avail_out=sizeof(out);
        ret_value=inflate(&z_str,0);
        if(retstr.size()<z_str.total_out)
            retstr.append(out,z_str.total_out-retstr.size());
    }while(Z_OK==ret_value);

    inflateEnd(&z_str);

    return retstr;
}

// Based on zpipe.c (the example coming with zlib sources). We handle files
// using FILE* (C-like) and not using a ifstream (C++-like) because the
// basic_istream.write() is not overloaded for char*, as the operators << and
// >> are.
std::ostream& deflate_file_to_stream(std::ostream& out_stream,
                                     const std::string& filename)
{
  FILE *source = fopen(filename.c_str(), "rb");

  if (NULL == source)
  {
    throw std::runtime_error("error opening file \"" + filename + "\"");
  }

  int ret, flush;
  unsigned have;
  z_stream strm;
  unsigned char in[CHUNK];
  unsigned char out[CHUNK];

  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  ret = deflateInit(&strm, Z_BEST_COMPRESSION);
  if (ret != Z_OK)
  {
    throw std::runtime_error("error deflating file \"" + filename + "\"");
  }

  do {
    strm.avail_in = fread(in, 1, CHUNK, source);
    if (ferror(source))
    {
      (void)deflateEnd(&strm);
      fclose(source);
      throw std::runtime_error("Z_ERRNO deflating file \"" + filename + "\"");
    }
    flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
    strm.next_in = in;

    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;
      ret = deflate(&strm, flush);
      //assert(ret != Z_STREAM_ERROR);
      have = CHUNK - strm.avail_out;

      out_stream.write(reinterpret_cast<char*>(out), have);
      // TODO: check that the write was successful.

    } while (strm.avail_out == 0);
    //assert(strm.avail_in == 0);

  } while (flush != Z_FINISH);
  //assert(ret == Z_STREAM_END);

  (void)deflateEnd(&strm);

  fclose(source);

  return out_stream;
}

std::ostream& deflate_buffer_to_stream(std::ostream& out_stream,
                                       char* buffer,
                                       unsigned length)
{
  z_stream z_str;
  memset(&z_str, 0, sizeof(z_str));
  deflateInit(&z_str, Z_BEST_COMPRESSION);
  z_str.next_in = (Bytef*)buffer;
  z_str.avail_in = length;
  int ret_value;

  // We allocate for output the input size.
  // TODO: if this size doesn't work, then inform it so the
  // user writes the data without using flate method.
  char *out = (char*)malloc(length * sizeof(char));

  unsigned total_written = 0;

  do {
    z_str.next_out = reinterpret_cast<Bytef*>(out);
    z_str.avail_out = sizeof(out);
    ret_value = deflate(&z_str, Z_FINISH);

    if (total_written<z_str.total_out)
    {
      out_stream.write(out, z_str.total_out - total_written);
      total_written = z_str.total_out;
    }
  } while (Z_OK == ret_value);

  deflateEnd(&z_str);

  return out_stream;
}

unsigned deflate_bound(unsigned aSize)
{
  return compressBound((uLong)aSize);
}

unsigned deflate_buffer_to_buffer(const char *src,
                                  unsigned src_length,
                                  char *dst,
                                  unsigned dst_length)
{
  compress2(reinterpret_cast<Bytef*>(dst),
            (uLongf*)&dst_length,
            reinterpret_cast<const Bytef*>(src),
            src_length,
            Z_BEST_COMPRESSION);

  return dst_length;
}

#undef CHUNK

#endif // PADDLEFISH_USE_ZLIB

} // namespace flate
} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
