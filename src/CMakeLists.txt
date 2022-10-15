set(PADDLEFISH_SOURCES cid_to_gid.cpp color_profile.cpp
            colorspace_properties.cpp command.cpp custom_object.cpp
            document.cpp file_stream.cpp flate.cpp font.cpp graphics_state.cpp
            image.cpp info.cpp ocg.cpp page.cpp resources_dict.cpp text.cpp
            text_state.cpp util.cpp version.cpp)

if (PADDLEFISH_USE_STB_TRUETYPE)
    set (PADDLEFISH_SOURCES ${PADDLEFISH_SOURCES} truetype.cpp)
endif (PADDLEFISH_USE_STB_TRUETYPE)

add_library(paddlefish ${PADDLEFISH_LIB_TYPE} ${PADDLEFISH_SOURCES})

set_target_properties(paddlefish PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(paddlefish PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

if(PADDLEFISH_USE_ZLIB)
    find_package(ZLIB REQUIRED)
    add_definitions(-DPADDLEFISH_USE_ZLIB)
    target_include_directories(paddlefish PRIVATE ${ZLIB_INCLUDE_DIR})
    target_link_libraries(paddlefish ${ZLIB_LIBRARY_RELEASE})
endif(PADDLEFISH_USE_ZLIB)

if (PADDLEFISH_USE_STB_TRUETYPE)
    add_definitions(-DPADDLEFISH_USE_STB_TRUETYPE)
    target_include_directories(paddlefish PRIVATE ${CMAKE_SOURCE_DIR}/3rd-party/stb)
endif (PADDLEFISH_USE_STB_TRUETYPE)

if (NOT MSVC)
    target_link_libraries(paddlefish m)
endif (NOT MSVC)
