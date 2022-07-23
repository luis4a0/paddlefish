cmake_minimum_required(VERSION 3.9)
project(paddlefish_examples LANGUAGES CXX)

set(EXAMPLES basic blank indexed pattern)

foreach(EXAMPLE IN LISTS EXAMPLES)
    add_executable(${EXAMPLE} ${EXAMPLE}.cpp)
    target_link_libraries(${EXAMPLE} paddlefish)
    if(NOT WIN32)
        set_target_properties(${EXAMPLE}
                              PROPERTIES
                              INSTALL_RPATH
                              "${CMAKE_INSTALL_PREFIX}/lib"
    endif(NOT WIN32)
)

endforeach()

install(TARGETS ${EXAMPLES} DESTINATION share/paddlefish/examples)
