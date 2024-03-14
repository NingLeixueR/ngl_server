cmake_minimum_required(VERSION 2.8)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_subdirectory(cpp)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

