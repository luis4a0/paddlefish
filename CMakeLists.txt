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

add_library(paddlefish ${PADDLEFISH_LIB_TYPE} cid_to_gid.cpp color_profile.cpp
            colorspace_properties.cpp command.cpp custom_object.cpp
            document.cpp file_stream.cpp flate.cpp font.cpp graphics_state.cpp
            image.cpp info.cpp ocg.cpp page.cpp resources_dict.cpp text.cpp
            text_state.cpp util.cpp)

set_target_properties(paddlefish PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(paddlefish PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

set(HEADERS cid_to_gid.h;color_profile.h;colorspace_properties.h;command.h;
    custom_object.h;document.h;file_stream.h;flate.h;font.h;graphics_state.h;
    image.h;info.h;ocg.h;page.h;pdf_object.h;resources_dict.h;text.h;
    text_state.h;util.h)

if(PADDLEFISH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(PADDLEFISH_BUILD_EXAMPLES)

if(PADDLEFISH_USE_ZLIB)
    find_package(ZLIB REQUIRED)
    add_definitions(-DPADDLEFISH_USE_ZLIB)
    target_link_libraries(paddlefish z)
endif(PADDLEFISH_USE_ZLIB)

include(GNUInstallDirs)
install(FILES ${HEADERS} DESTINATION "include")
install(TARGETS paddlefish LIBRARY)

add_compile_options(-ansi)
add_compile_options(-Wall)
add_compile_options(-pedantic)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

target_link_libraries(paddlefish m)

