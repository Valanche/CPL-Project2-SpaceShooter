cmake_minimum_required(VERSION 3.20)
project(CPL_Project2_SpaceShooter)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(include)
include_directories(lib)

add_executable(SpaceShooter SpaceShooter.c)
add_executable(sample sample.c)
