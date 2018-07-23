# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Test")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(COGL_BUILD_EXAMPLES CACHE BOOL OFF)

add_subdirectory("deps")

# Include sub-projects.
add_subdirectory ("Test")