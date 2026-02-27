cmake_minimum_required(VERSION 3.5)

add_subdirectory(libnglcore)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_subdirectory(rebuild)
	add_subdirectory(makeproto)
	add_subdirectory(idl)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

add_subdirectory(node)

