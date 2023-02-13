cmake_minimum_required(VERSION 3.9)
project(paddlefish_examples LANGUAGES CXX)

if (PADDLEFISH_USE_STB_TRUETYPE)
    add_definitions(-DPADDLEFISH_USE_STB_TRUETYPE)
endif (PADDLEFISH_USE_STB_TRUETYPE)

set(EXAMPLES basic blank indexed pattern ttf)

foreach(EXAMPLE IN LISTS EXAMPLES)
    add_executable(${EXAMPLE} ${EXAMPLE}.cpp)
    target_link_libraries(${EXAMPLE} paddlefish)

    if (PADDLEFISH_USE_STB_TRUETYPE)
        target_include_directories(${EXAMPLE} PRIVATE ${CMAKE_SOURCE_DIR}/3rd-party/stb)
    endif (PADDLEFISH_USE_STB_TRUETYPE)

    if(NOT WIN32)
        set_target_properties(${EXAMPLE}
                              PROPERTIES
                              INSTALL_RPATH
                              "${CMAKE_INSTALL_PREFIX}/lib")
    endif(NOT WIN32)
endforeach()

install(TARGETS ${EXAMPLES} DESTINATION share/paddlefish/examples)
