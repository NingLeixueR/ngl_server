# File overview: Defines CMake targets and build rules for node.

cmake_minimum_required(VERSION 3.15)

PROJECT (node)

SET(CORE_LIST ${PROJECT_SOURCE_DIR}/core)
FILE(GLOB_RECURSE CORE_LIST_INCLUDE ${CORE_LIST}/*.h ) 
FILE(GLOB_RECURSE CORE_LIST_SRC ${CORE_LIST}/*.cpp) 
FILE(GLOB_RECURSE CORE_LIST_CSRC ${CORE_LIST}/*.c) 
SOURCE_GROUP("Header Files" FILES ${CORE_LIST_INCLUDE})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_SRC})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_CSRC})

ADD_EXECUTABLE(node
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
)

target_link_libraries(node
	PRIVATE nglcore 
	PRIVATE ngl_build_options
)
target_include_directories(node PRIVATE ${CORE_LIST} ${PROJECT_SOURCE_DIR}/../libnglcore/core)

ngl_apply_msvc_defaults(node)