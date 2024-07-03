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
)

file(
    COPY ${INPUT_FILES}
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/inputs/
)

# Create tests executables
add_executable(
    pathtree_test
    pathtree_test.cc
)
target_link_libraries(pathtree_test
    vacuum_cleaner
    GTest::gtest_main
)

add_executable(
    locationmanager_test
    locationmanager_test.cc
)
target_link_libraries(locationmanager_test
    vacuum_cleaner
    GTest::gtest_main
)

add_executable(
    batterycontroller_test
    batterycontroller_test.cc
)

target_link_libraries(batterycontroller_test
    vacuum_cleaner
    GTest::gtest_main
)

add_executable(
    robotdeserializer_test
    robotdeserializer_test.cc
)
target_link_libraries(robotdeserializer_test
    vacuum_cleaner
    GTest::gtest_main
)

add_executable(
    navigationsystem_test
    navigationsystem_test.cc
)
target_link_libraries(navigationsystem_test
    vacuum_cleaner
    GTest::gtest_main
    GTest::gmock_main
)

# Add unit test targets
add_test(
    NAME pathtree_test
    COMMAND pathtree_test
)

add_test(
    NAME locationmanager_test
    COMMAND locationmanager_test
)

add_test(
    NAME batterycontroller_test
    COMMAND batterycontroller_test
)

add_test(
    NAME robotdeserializer_test
    COMMAND robotdeserializer_test
)

add_test(
    NAME navigationsystem_test
    COMMAND navigationsystem_test
)