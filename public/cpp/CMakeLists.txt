# File overview: Defines CMake targets and build rules for cpp.

cmake_minimum_required(VERSION 3.16)

project(libmycore)

set(PUBLIC_LIST ${PROJECT_SOURCE_DIR})
file(GLOB_RECURSE PUBLIC_LIST_INCLUDE CONFIGURE_DEPENDS ${PUBLIC_LIST}/*.h)
file(GLOB_RECURSE PUBLIC_LIST_SRC CONFIGURE_DEPENDS ${PUBLIC_LIST}/*.cpp)
file(GLOB_RECURSE PUBLIC_LIST_CSRC CONFIGURE_DEPENDS ${PUBLIC_LIST}/*.c)
file(GLOB_RECURSE CORE_LIST_PBSRC CONFIGURE_DEPENDS ${PUBLIC_LIST}/*.cc)

source_group("Header Files" FILES ${PUBLIC_LIST_INCLUDE})
source_group("Source Files" FILES ${PUBLIC_LIST_SRC} ${PUBLIC_LIST_CSRC})
source_group("Pb Source Files" FILES ${CORE_LIST_PBSRC})
ngl_configure_vs_unity_filters()

if(NOT DEFINED NGL_IS_LOCAL_BUILD)
	set(NGL_IS_LOCAL_BUILD ON)
endif()

if(NGL_IS_LOCAL_BUILD AND CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_library(libmycore STATIC
		${PUBLIC_LIST_INCLUDE}
		${PUBLIC_LIST_SRC}
		${PUBLIC_LIST_CSRC}
		${CORE_LIST_PBSRC}
	)

	target_link_libraries(libmycore PRIVATE
		ngl_build_options
	)
	ngl_apply_runtime_deps(libmycore PRIVATE)
	ngl_apply_unity_build(libmycore)
	ngl_apply_msvc_defaults(libmycore)
endif()
