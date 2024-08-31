cmake_minimum_required(VERSION 3.0.0)
project(VacuumCleaner)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# set the output directory for all binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

add_custom_target(run
	DEPENDS algorithms myrobot
	COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/myrobot -algo_path=${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -house_path=${CMAKE_BINARY_DIR}/../examples)

option(BUILD_TESTS "Build Tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
