# File overview: Defines CMake targets and build rules for libnglcore.

cmake_minimum_required(VERSION 3.16)

PROJECT (nglcore)

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
list(APPEND CORE_LIST_SRC ${NGL_PUBLIC_CPP_SRC})
list(APPEND CORE_LIST_CSRC ${NGL_PUBLIC_CPP_CSRC})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NGLCORE_BUILD_SHARED)
	set(NGLCORE_LIBRARY_KIND SHARED)
else()
	set(NGLCORE_LIBRARY_KIND STATIC)
endif()

ADD_LIBRARY(nglcore ${NGLCORE_LIBRARY_KIND}
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
	${CORE_PROTOBUF_LIST_CSRC}
)
message(STATUS "Building nglcore as a ${NGLCORE_LIBRARY_KIND} library")

if(NGLCORE_BUILD_SHARED)
	target_compile_definitions(nglcore PRIVATE NGLCORE_EXPORTS)
else()
	target_compile_definitions(nglcore PUBLIC NGLCORE_STATIC)
endif()

target_include_directories(nglcore PUBLIC
	${CORE_LIST}
)
target_link_libraries(nglcore PUBLIC ngl_build_options)

ngl_apply_runtime_deps(nglcore PRIVATE)
ngl_apply_unity_build(nglcore)
ngl_apply_msvc_defaults(nglcore)
