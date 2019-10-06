# CMakeList.txt : CMake project for tpt-prototype, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(GLEW_STATIC false CACHE BOOL "Glew static library")

find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(SHADER_FILES
	shaders/ns_advect_compute.glsl
	shaders/ns_forces_compute.glsl
	shaders/ns_bounds_compute.glsl
	shaders/ns_jacobi_compute.glsl
	shaders/ns_apply_pressure_compute.glsl
	shaders/ns_apply_vorticity_compute.glsl
	shaders/ns_vorticity_compute.glsl
	shaders/basic_fragment.glsl
	shaders/basic_vertex.glsl
	shaders/velocity_pressure_fragment.glsl)
set(SHADER_FILE_HEADERS)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/glsl.cmake "set(source \"\")

file(STRINGS \${INFILE} lines)
foreach (line \${lines})
    set(source \"\${source}    \${line}\\n\")
endforeach()
        
file(
    WRITE \${OUTFILE}
    \"char *\${VARNAME} = R\\\"shader_src(\\n\"
    \"\${source}\"
    \"\)shader_src\\\";\\n\"
)")
foreach (file ${SHADER_FILES})
        set(filename ${file})
        get_filename_component(name ${filename} NAME_WE)
        
		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}_shader_source.cpp
			COMMAND ${CMAKE_COMMAND} 
			-D VARNAME:STRING=${name}_shader_source
			-D INFILE:STRING=${CMAKE_CURRENT_SOURCE_DIR}/${file}
			-D OUTFILE:STRING=${CMAKE_CURRENT_BINARY_DIR}/${name}_shader_source.cpp
			-P ${CMAKE_CURRENT_BINARY_DIR}/glsl.cmake 
			DEPENDS ${file}
			)

		add_custom_target(${name}_shader_source DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${name}_shader_source.cpp)

		#set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${name}_shader_source.cpp PROPERTIES GENERATED 1)
		#add_dependencies(tpt)
		#list(APPEND SHADER_FILE_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${name}_shader_source.cpp)
		list(APPEND SHADER_FILE_HEADERS ${name}_shader_source)
		#add_dependencies(${CMAKE_CURRENT_BINARY_DIR}/${name}_shader_source.cpp)
endforeach()

# Add source to this project's executable.
add_definitions(-DGLEW_STATIC=${GLEW_STATIC})
add_executable (tpt "tpt-prototype.cpp" "tpt-prototype.h" ${SHADER_FILE_HEADERS})
target_include_directories(tpt PRIVATE ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(tpt ${CMAKE_THREAD_LIBS_INIT} ${GLEW_LIBRARIES} ${OPENGL_gl_LIBRARY} ${SDL2_LIBRARIES})   