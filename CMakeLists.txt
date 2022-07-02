cmake_minimum_required(VERSION 3.9)

project(paddlefish
        DESCRIPTION "PDF creation library"
        VERSION 0.9.9
        LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(PADDLEFISH_BUILD_EXAMPLES "Build examples" OFF)
option(PADDLEFISH_USE_ZLIB "Use zlib" ON)

if(BUILD_SHARED_LIBS)
    set(PADDLEFISH_LIB_TYPE "SHARED")
else(BUILD_SHARED_LIBS)
    set(PADDLEFISH_LIB_TYPE "STATIC")
endif(BUILD_SHARED_LIBS)

if(WINDOWS)
    add_definitions(-DPADDLEFISH_WINDOWS)
elseif(APPLE)
    add_definitions(-DPADDLEFISH_MACOS)
elseif(LINUX)
    add_definitions(-DPADDLEFISH_LINUX)
else()
    message(WARNING "Unknown platform")
endif()

if(NOT MSVC)
add_compile_options(-ansi)
add_compile_options(-Wall)
add_compile_options(-pedantic)
endif (NOT MSVC)

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

if (NOT MSVC)
include(GNUInstallDirs)
install(DIRECTORY include DESTINATION .)
install(TARGETS paddlefish LIBRARY)
endif (NOT MSVC)
