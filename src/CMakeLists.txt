include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(algorithm)

# Create a library to from source files
add_library(vacuum_cleaner
    simulator/simulator.cc
    simulator/house.cc
    simulator/deserializer.cc
    simulator/enum_operators.cc
    simulator/AlgorithmRegistrar.cpp
    input_handler.cc
    output_handler.cc
    task.cc
    task_queue.cc
)

target_include_directories(vacuum_cleaner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(vacuum_cleaner PUBLIC)
target_link_options(vacuum_cleaner PUBLIC)

# Add an executable for the main program
add_executable(myrobot main.cc)

set_target_properties(myrobot PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_link_libraries(myrobot vacuum_cleaner)
