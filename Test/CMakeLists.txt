﻿# CMakeList.txt : CMake project for Test, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (Test "Test.cpp" "Test.h")

# TODO: Add tests and install targets if needed.
target_link_libraries(Test cogl)
