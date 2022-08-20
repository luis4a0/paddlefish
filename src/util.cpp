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

#include <paddlefish/colorspace_properties.h>
#include <paddlefish/util.h>

#include <iterator>

namespace paddlefish {
namespace util{

template <class F>
std::string fp_to_str(F n)
{
  std::string n_str(std::to_string(n));

  // Find the period.
  size_t period = n_str.size() + 1;
  for (size_t i = 0; i < n_str.size(); ++i)
  {
    if (n_str[i] == '.')
    {
      period = i;
      break;
    }
  }

  // Search for the last non-zero digit after the period.
  size_t lsd = period;
  for (size_t i = period + 1; i < n_str.size(); ++i)
  {
    if (n_str[i] != '0')
    {
      lsd = i;
    }
  }

  if (lsd == period)
    return n_str.substr(0, lsd);
  else
    return n_str.substr(0, lsd + 1);
}

template <> std::string to_str(float n) { return fp_to_str(n); }
template <> std::string to_str(double n) { return fp_to_str(n); }
template <> std::string to_str(long double n) { return fp_to_str(n); }

std::string matrix23_contents_to_string(double *matrix)
{
  if (matrix)
  {
    return vector_to_string(matrix, 6);
  }
  else
  {
    return std::string("1 0 0 1 0 0");
  }
}

std::string matrix23_to_string(double *matrix)
{
  return std::string("/Matrix [ " + matrix23_contents_to_string(matrix) + " ]");
}

std::string split_string(const std::string &s, unsigned split_columns)
{
  std::string r(s);

  unsigned column = 0;
  int last_space = -1;

  for (size_t i = 0; i < s.length(); ++i)
  {
    switch (r[i])
    {
      case ' ':
        last_space = (unsigned)i;
        break;
      case '\n':
        last_space = -1;
        column = 0;
        break;
      default:
        break;
    }

    ++column;

    if (column + 1 > split_columns && last_space != -1)
    {
      r[last_space] = '\n';
      column = (unsigned)i - last_space;
      last_space = -1;
    }
  }

  return r;
}

std::string format_name(const std::string &s)
{
  std::string r;

  r.reserve(s.length());

  for (size_t i = 0; i < s.length(); ++i)
  {
    switch (s[i])
    {
      case ' ':
        r += "#20";
        break;
      case '(':
        r += "#28";
        break;
      case ')':
        r+= "#29";
        break;
      default:
        r += s[i];
        break;
    }
  }

  return r;
}

std::string escape_string(const std::string &s)
{
  std::string r;

  size_t length = s.length();

  r.reserve(length);

  for (size_t i = 0; i < length; ++i)
  {
    switch (s[i])
    {
    case '(':
      r += "\\(";
      break;
    case ')':
      r += "\\)";
      break;
    case '\\':
      // If we find a backslash, we check if it escapes the next character.
      // The escaped characters checked for here come from page 30 of the
      // PDF standard, version 1.4.
      // TODO: check that the backslash is not prefixing an octal number.
      if (length > i+1  && s[i+1] != 'n' && s[i+1] != 'r' && s[i+1] != 't' &&
          s[i+1] != 'b' && s[i+1] != 'f' && s[i+1] != '(' && s[i+1] != ')' &&
          s[i+1] != '\\')
        r += "\\\\";
      else
        if (length > i+1)
          r += s[++i];
      break;
    default:
      r += s[i];
      break;
    }
  }

  return r;
}

std::string color_profile_ref(unsigned cs_id)
{
  switch (cs_id)
  {
  case COLORSPACE_DEVICERGB:
    return "/DeviceRGB";
    break;

  case COLORSPACE_DEVICEGRAY:
    return "/DeviceGray";
    break;

  case COLORSPACE_DEVICECMYK:
    return "/DeviceCMYK";
    break;

  default:
    return util::to_str(cs_id) + " 0 R";
    break;
  }
}

} // namespace util
} // namespace paddlefish

// vim: ts=2:sw=2:expandtab
