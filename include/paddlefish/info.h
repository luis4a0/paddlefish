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

#ifndef PADDLEFISH_INFO_H
#define PADDLEFISH_INFO_H

#include <string>

namespace paddlefish {

class Info{
  public:
  // Dates in PDF are defined in section 3.8.2 of the PDF standard,
  // version 1.4. They are a string of the form D:YYYYMMDDHHmmSSOHH'mm'.
  typedef std::string date;

  std::string title,author,subject,keywords,creator,producer;
  date creation_date,mod_date;

  Info();
  ~Info();

  private:
  std::string two_characters(int);

  public:
  date current_date();
};

} // namespace paddlefish

#endif // PADDLEFISH_INFO_H

// vim: ts=2:sw=2:expandtab
