cmake_minimum_required(VERSION 3.9)
project(paddlefish_examples LANGUAGES CXX)

set(EXAMPLES basic blank indexed pattern)

foreach(EXAMPLE IN LISTS EXAMPLES)
    add_executable(${EXAMPLE} ${EXAMPLE}.cpp)
    target_link_libraries(${EXAMPLE} paddlefish)
endforeach()
