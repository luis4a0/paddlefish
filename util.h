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

#ifndef PADDLEFISH_UTIL_H
#define PADDLEFISH_UTIL_H

#include <string>

namespace paddlefish {
namespace util{

// Returns a string containing the argument, formatted. This can be done
// using std::to_string(), but sometimes it produces lots of zeros after
//the period.
template <class T> std::string to_str(T n) { return std::to_string(n); }

// Specialization of the above function for floats and doubles.
template <> std::string to_str(float n);
template <> std::string to_str(double n);
template <> std::string to_str(long double n);

// Returns a vector as a space-separated string.
template <class T>
std::string vector_to_string(T *vec, size_t vec_size)
{
  std::string vec_str;
  if (vec)
  {
    vec_str += to_str(vec[0]);
    for (size_t i = 1; i < vec_size; ++i)
      vec_str += " " + to_str(vec[i]);
  }
  return vec_str;
}

// Returns the contents of a 2x3 matrix as a space-separated string
// containing the matrix elements.
std::string matrix23_contents_to_string(double *matrix);

// Convert a 2x3 matrix to a string in the output. If the argument is NULL,
// then it returns the default matrix. Precondition: *matrix is NULL or it
// has six elements.
std::string matrix23_to_string(double *matrix);

// Splits a string in many lines if possible, by replacing some spaces by
// carriage returns.
std::string split_string(const std::string &s, unsigned split_columns);

// Change the spaces in the input string to #20. This is required when
// specifying names in the PDF, which cannot contain spaces.
std::string format_name(const std::string &s);

// Escape a given string for using inside a text block.
std::string escape_string(const std::string &s);

// Given the object number of a color profile, returns the name of the color
// profile if it is device-based, or a reference number otherwise. Pattern
// color spaces are not handled separately.
std::string color_profile_ref(unsigned cs_id);

} // namespace util
} // namespace paddlefish

#endif // PADDLEFISH_UTIL_H

// vim: ts=2:sw=2:expandtab
