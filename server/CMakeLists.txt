# File overview: Defines CMake targets and build rules for server.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED NGL_IS_LOCAL_BUILD)
	set(NGL_IS_LOCAL_BUILD ON)
endif()

add_subdirectory(libnglcore)
add_subdirectory(node)

if(NGL_IS_LOCAL_BUILD AND CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_subdirectory(tools)
endif()

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()
