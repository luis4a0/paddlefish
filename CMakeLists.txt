# Copyright (c) 2022 Luis Peñaranda. All rights reserved.
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

cmake_minimum_required(VERSION 3.9)

project(paddlefish
        DESCRIPTION "PDF creation library"
        VERSION 0.9.9
        LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(PADDLEFISH_BUILD_EXAMPLES "Build examples" OFF)
option(PADDLEFISH_USE_ZLIB "Use zlib" ON)
option(PADDLEFISH_USE_STB_TRUETYPE "Use STB TrueType library" ON)

if(BUILD_SHARED_LIBS)
    set(PADDLEFISH_LIB_TYPE "SHARED")
else(BUILD_SHARED_LIBS)
    set(PADDLEFISH_LIB_TYPE "STATIC")
endif(BUILD_SHARED_LIBS)

if(WIN32)
    add_definitions(-DPADDLEFISH_WINDOWS)
elseif(APPLE)
    add_definitions(-DPADDLEFISH_MACOS)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions(-DPADDLEFISH_LINUX)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    add_definitions(-DPADDLEFISH_FREEBSD)
else()
    message(WARNING "Unknown platform '${CMAKE_SYSTEM_NAME}'")
endif()

if(NOT MSVC)
    add_compile_options(-ansi)
    add_compile_options(-Wall)
    add_compile_options(-pedantic)
endif(NOT MSVC)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    add_compile_options(-Wno-attributes)
endif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")

add_compile_options(-DPADDLEFISH_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_compile_options(-DPADDLEFISH_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_compile_options(-DPADDLEFISH_VERSION_PATCH=${PROJECT_VERSION_PATCH})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include_directories(include ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)

if(PADDLEFISH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(PADDLEFISH_BUILD_EXAMPLES)

include(GNUInstallDirs)
install(DIRECTORY include DESTINATION .)
install(TARGETS paddlefish LIBRARY)
install(DIRECTORY resources DESTINATION share/paddlefish)
