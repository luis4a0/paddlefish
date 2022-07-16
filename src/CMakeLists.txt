add_library(paddlefish ${PADDLEFISH_LIB_TYPE} cid_to_gid.cpp color_profile.cpp
            colorspace_properties.cpp command.cpp custom_object.cpp
            document.cpp file_stream.cpp flate.cpp font.cpp graphics_state.cpp
            image.cpp info.cpp ocg.cpp page.cpp resources_dict.cpp text.cpp
            text_state.cpp util.cpp version.cpp)

set_target_properties(paddlefish PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(paddlefish PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

if(PADDLEFISH_USE_ZLIB)
    find_package(ZLIB REQUIRED)
    add_definitions(-DPADDLEFISH_USE_ZLIB)
    target_include_directories(paddlefish PRIVATE ${ZLIB_INCLUDE_DIR})
    target_link_libraries(paddlefish ${ZLIB_LIBRARY_RELEASE})
endif(PADDLEFISH_USE_ZLIB)

if (NOT MSVC)
    target_link_libraries(paddlefish m)
endif (NOT MSVC)
