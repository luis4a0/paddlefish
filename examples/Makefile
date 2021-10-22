# Copyright (c) 2017-2020 Luis Peñaranda. All rights reserved.
#
# This file is part of paddlefish.
#
# Paddlefish is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Paddlefish is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with paddlefish.  If not, see <http://www.gnu.org/licenses/>.

# Do NOT use -ansi in CXXPARAMS when using valgrind. It is also better
# to use -O0 in OPTIMIZATION.

PDF_LIB=paddlefish
PDF_LIB_PATH=..
EXT_LIBS_DEFS=-DPADDLEFISH_USE_ZLIB
CXXPARAMS=-g ${EXT_LIBS_DEFS} -Wall -pedantic -std=c++11 #-ansi
OPTIMIZATION=-O0
EXT_LIBS=-lm -lz

EXAMPLES=basic indexed pattern empdfer

%: %.cpp
	g++ ${CXXPARAMS} ${OPTIMIZATION} $< -L${PDF_LIB_PATH} -l${PDF_LIB} ${EXT_LIBS} -o $@

all: ${PDF_LIB_PATH}/lib${PDF_LIB}.a ${EXAMPLES}

empdfer: empdfer.cpp
	g++ ${CXXPARAMS} ${OPTIMIZATION} $< -L${PDF_LIB_PATH} -l${PDF_LIB} ${EXT_LIBS} -ljpeg -o $@

clean:
	rm -f *.o ${EXAMPLES} ${LIBNAME}
