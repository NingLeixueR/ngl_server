# File overview: Defines CMake targets and build rules for idl.

cmake_minimum_required(VERSION 3.16)

PROJECT (IDL)

########################################################################################core
SET(CORE_LIST ${PROJECT_SOURCE_DIR}/core)
FILE(GLOB_RECURSE CORE_LIST_INCLUDE CONFIGURE_DEPENDS ${CORE_LIST}/*.h )
FILE(GLOB_RECURSE CORE_LIST_SRC CONFIGURE_DEPENDS ${CORE_LIST}/*.cpp)
FILE(GLOB_RECURSE CORE_LIST_CSRC CONFIGURE_DEPENDS ${CORE_LIST}/*.c)
SOURCE_GROUP("Header Files" FILES ${CORE_LIST_INCLUDE})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_SRC})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_CSRC})
ngl_configure_vs_unity_filters()

list(FILTER CORE_LIST_SRC EXCLUDE REGEX "ub_.*\\.cpp$")

ADD_EXECUTABLE(IDL
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
)

target_include_directories(IDL PRIVATE ${CORE_LIST})
target_link_libraries(IDL PRIVATE nglcore)
ngl_apply_runtime_deps(IDL)
ngl_apply_unity_build(IDL)
ngl_apply_msvc_defaults(IDL)
