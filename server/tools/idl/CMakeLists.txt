# File overview: Defines CMake targets and build rules for idl.

cmake_minimum_required(VERSION 3.16)

PROJECT (IDL)

########################################################################################core
SET(CORE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/core)
FILE(GLOB_RECURSE CORE_LIST_INCLUDE CONFIGURE_DEPENDS ${CORE_LIST}/*.h )
FILE(GLOB_RECURSE CORE_LIST_SRC CONFIGURE_DEPENDS ${CORE_LIST}/*.cpp)
FILE(GLOB_RECURSE CORE_LIST_CSRC CONFIGURE_DEPENDS ${CORE_LIST}/*.c)
FILE(GLOB_RECURSE CORE_PROTOBUF_LIST_CSRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/public/*.cc)
SOURCE_GROUP("Header Files" FILES ${CORE_LIST_INCLUDE})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_SRC})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_CSRC})
SOURCE_GROUP("Protobuf Source Files" FILES ${CORE_PROTOBUF_LIST_CSRC})
ngl_configure_vs_unity_filters()

list(FILTER CORE_LIST_SRC EXCLUDE REGEX "ub_.*\\.cpp$")

if(NGLCORE_BUILD_SHARED)
	# Shared nglcore mode keeps the tool-local public/cpp build to avoid exporting
	# the entire helper surface from a single Windows DLL.
	list(APPEND CORE_LIST_SRC ${NGL_PUBLIC_CPP_SRC})
	list(APPEND CORE_LIST_CSRC ${NGL_PUBLIC_CPP_CSRC})
endif()

ADD_EXECUTABLE(IDL
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
	$<$<BOOL:${NGLCORE_BUILD_SHARED}>:${CORE_PROTOBUF_LIST_CSRC}>
)

target_include_directories(IDL PRIVATE ${CORE_LIST})
if(NGLCORE_BUILD_SHARED)
	target_link_libraries(IDL PRIVATE ngl_build_options)
else()
	target_link_libraries(IDL PRIVATE nglcore)
	set_target_properties(IDL PROPERTIES UNITY_BUILD OFF)
endif()
ngl_apply_runtime_deps(IDL)
if(NGLCORE_BUILD_SHARED)
	ngl_apply_unity_build(IDL)
endif()
ngl_apply_msvc_defaults(IDL)
