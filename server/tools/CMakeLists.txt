# File overview: Groups local development tools under a single entry point.

cmake_minimum_required(VERSION 3.16)

add_subdirectory(rebuild)
add_subdirectory(makeproto)
add_subdirectory(idl)

if(TARGET rebuild)
	set_target_properties(rebuild PROPERTIES FOLDER "tools")
endif()

if(TARGET makeproto)
	set_target_properties(makeproto PROPERTIES FOLDER "tools")
endif()

if(TARGET IDL)
	set_target_properties(IDL PROPERTIES FOLDER "tools")
endif()
