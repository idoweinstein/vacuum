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

# Create tests executables
add_executable(
    pathtree_test
    pathtree_test.cc
)
target_link_libraries(pathtree_test
    vacuum_cleaner
    GTest::gtest_main
)

# Add unit test targets
add_test(
    NAME pathtree_test
    COMMAND pathtree_test
)