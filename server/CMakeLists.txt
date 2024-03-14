cmake_minimum_required(VERSION 2.8)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_subdirectory(rebuild)
	add_subdirectory(makeproto)
	add_subdirectory(idl)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

add_subdirectory(node)

