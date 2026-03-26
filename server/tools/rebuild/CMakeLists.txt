# File overview: Defines CMake targets and build rules for rebuild.

cmake_minimum_required(VERSION 3.16)

PROJECT (rebuild)

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
	# A single shared nglcore target would need to export a very large public/cpp
	# symbol surface for these tools on Windows, so shared mode keeps the original
	# direct-compilation path.
	list(APPEND CORE_LIST_SRC ${NGL_PUBLIC_CPP_SRC})
	list(APPEND CORE_LIST_CSRC ${NGL_PUBLIC_CPP_CSRC})
endif()

ADD_EXECUTABLE(rebuild
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
	$<$<BOOL:${NGLCORE_BUILD_SHARED}>:${CORE_PROTOBUF_LIST_CSRC}>
)

target_include_directories(rebuild PRIVATE ${CORE_LIST})
if(NGLCORE_BUILD_SHARED)
	target_link_libraries(rebuild PRIVATE ngl_build_options)
else()
	target_link_libraries(rebuild PRIVATE nglcore)
	set_target_properties(rebuild PROPERTIES UNITY_BUILD OFF)
endif()
ngl_apply_runtime_deps(rebuild)
if(NGLCORE_BUILD_SHARED)
	ngl_apply_unity_build(rebuild)
endif()
ngl_apply_msvc_defaults(rebuild)
