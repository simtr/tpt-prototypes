# CMakeList.txt : CMake project for tpt-prototype, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

SET(GLEW_STATIC false CACHE BOOL "Glew static library")

find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Add source to this project's executable.
add_definitions(-DGLEW_STATIC=${GLEW_STATIC})
add_executable (tpt "tpt-prototype.cpp" "tpt-prototype.h")
target_include_directories(tpt PRIVATE ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(tpt ${CMAKE_THREAD_LIBS_INIT} ${GLEW_LIBRARIES} ${OPENGL_gl_LIBRARY} ${SDL2_LIBRARIES})   