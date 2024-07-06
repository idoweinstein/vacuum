cmake_minimum_required(VERSION 3.0.0)
project(VacuumCleaner)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

add_subdirectory(src)

option(BUILD_TESTS "Build Tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
