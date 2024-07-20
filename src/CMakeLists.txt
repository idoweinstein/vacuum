cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Create a library to from source files
add_library(vacuum_cleaner
    logger.cc
    simulator.cc
    path_tree.cc
    house.cc
    algorithm.cc
    deserializer.cc
    enum_operators.cc
)

target_include_directories(vacuum_cleaner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add an executable for the main program
add_executable(myrobot main.cc)
set_target_properties(myrobot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
target_link_libraries(myrobot vacuum_cleaner)
