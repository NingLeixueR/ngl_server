# File overview: Defines CMake targets and build rules for node.

cmake_minimum_required(VERSION 3.16)

PROJECT (node)

SET(CORE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/core)
FILE(GLOB_RECURSE CORE_LIST_INCLUDE ${CORE_LIST}/*.h ) 
FILE(GLOB_RECURSE CORE_LIST_SRC ${CORE_LIST}/*.cpp) 
FILE(GLOB_RECURSE CORE_LIST_CSRC ${CORE_LIST}/*.c) 
SOURCE_GROUP("Header Files" FILES ${CORE_LIST_INCLUDE})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_SRC})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_CSRC})
ngl_configure_vs_unity_filters()

ADD_EXECUTABLE(node
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
)

target_link_libraries(node
	PRIVATE nglcore
)
target_include_directories(node PRIVATE ${CORE_LIST})

ngl_apply_runtime_deps(node PRIVATE)
ngl_apply_unity_build(node)
ngl_apply_msvc_defaults(node)
