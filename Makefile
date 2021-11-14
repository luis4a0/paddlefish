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

LIB=paddlefish
LIBNAME=lib${LIB}.a

OBJECTS=cid_to_gid.o color_profile.o colorspace_properties.o command.o \
custom_object.o document.o file_stream.o flate.o font.o graphics_state.o \
image.o info.o ocg.o page.o resources_dict.o text.o text_state.o util.o

EXT_LIBS_DEFS=-DPADDLEFISH_USE_ZLIB
CXXPARAMS=-g ${EXT_LIBS_DEFS} -Wall -pedantic -std=c++11 #-ansi
OPTIMIZATION=-O0
EXT_LIBS=-lm -lz

%.o: %.cpp
	g++ ${CXXPARAMS} ${OPTIMIZATION} -c $< -o $@

${LIBNAME}: ${OBJECTS}
	ar rsc ${LIBNAME} ${OBJECTS}

all: ${LIBNAME}

install: ${LIBNAME}
	install -D ${LIBNAME} ${DESTDIR}

clean:
	rm -f *.o ${LIBNAME}
