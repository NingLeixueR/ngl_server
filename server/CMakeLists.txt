# File overview: Defines CMake targets and build rules for server.

cmake_minimum_required(VERSION 3.15)

if(NOT DEFINED NGL_IS_LOCAL_BUILD)
	set(NGL_IS_LOCAL_BUILD ON)
endif()

add_subdirectory(libnglcore)

if(NGL_IS_LOCAL_BUILD AND CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_subdirectory(rebuild)
	add_subdirectory(makeproto)
	add_subdirectory(idl)
endif()

add_subdirectory(node)

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()