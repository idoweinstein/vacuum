cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Include /src directory
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add gtest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/34ad51b3dc4f922d8ab622491dd44fc2c39afee9.zip
)

# For Windows: Prevent overriding the parent project's compiler / linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(INPUT_FILES
    inputs/input_sanity.txt
    inputs/input_nohouse.txt
    inputs/input_missvalue.txt
    inputs/input_missparam.txt
    inputs/input_invparam.txt
    inputs/input_invchar.txt
    inputs/input_dupdock.txt
    inputs/input_trappeddirt.txt
    inputs/input_maze.txt
    inputs/input_minbattery.txt
    inputs/input_distantdirt.txt
    inputs/input_allchars.txt
    inputs/input_nodock.txt
    inputs/input_filledline.txt
    inputs/input_filledcol.txt
)

file(
    COPY ${INPUT_FILES}
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/inputs/
)

# Create tests executables
add_executable(
    path_tree_test
    path_tree_test.cc
)
target_link_libraries(path_tree_test
    vacuum_cleaner
    GTest::gtest_main
)

add_executable(
    house_test
    house_test.cc
)
target_link_libraries(house_test
    vacuum_cleaner
    GTest::gtest_main
)

add_executable(
    battery_test
    battery_test.cc
)

target_link_libraries(battery_test
    vacuum_cleaner
    GTest::gtest_main
)

add_executable(
    deserializer_test
    deserializer_test.cc
)
target_link_libraries(deserializer_test
    vacuum_cleaner
    GTest::gtest_main
)

add_executable(
    algorithm_test
    algorithm_test.cc
)
target_link_libraries(algorithm_test
    vacuum_cleaner
    GTest::gtest_main
    GTest::gmock_main
)

add_executable(
    simulator_test
    simulator_test.cc
)
target_link_libraries(simulator_test
    vacuum_cleaner
    GTest::gtest_main
)

# Add unit test targets
add_test(
    NAME path_tree_test
    COMMAND path_tree_test
)

add_test(
    NAME house_test
    COMMAND house_test
)

add_test(
    NAME battery_test
    COMMAND battery_test
)

add_test(
    NAME deserializer_test
    COMMAND deserializer_test
)

add_test(
    NAME algorithm_test
    COMMAND algorithm_test
)

add_test(
    NAME simulator_test
    COMMAND simulator_test
)
