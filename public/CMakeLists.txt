# File overview: Defines CMake targets and build rules for public.

cmake_minimum_required(VERSION 3.15)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_subdirectory(cpp)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")