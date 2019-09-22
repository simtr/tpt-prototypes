# CMakeList.txt : CMake project for tpt-prototype, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

# Add source to this project's executable.
add_executable (tpt "tpt-prototype.cpp" "tpt-prototype.h")
target_include_directories(tpt PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(tpt ${SDL2_LIBRARIES})   

# TODO: Add tests and install targets if needed.
