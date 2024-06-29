cmake_minimum_required(VERSION 3.0.0)
project(VacuumCleaner)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_executable(myrobot src/logger.cc src/robot.cc src/pathtree.cc src/locationmanager.cc src/navigationsystem.cc src/robotlogger.cc src/robotdeserializer.cc src/main.cc)
