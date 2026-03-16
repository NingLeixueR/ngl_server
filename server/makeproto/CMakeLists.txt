# File overview: Defines CMake targets and build rules for makeproto.

cmake_minimum_required(VERSION 3.15)

PROJECT (makeproto)

########################################################################################core
SET(CORE_LIST ${PROJECT_SOURCE_DIR}/core)
FILE(GLOB_RECURSE CORE_LIST_INCLUDE CONFIGURE_DEPENDS ${CORE_LIST}/*.h )
FILE(GLOB_RECURSE CORE_LIST_SRC CONFIGURE_DEPENDS ${CORE_LIST}/*.cpp)
FILE(GLOB_RECURSE CORE_LIST_CSRC CONFIGURE_DEPENDS ${CORE_LIST}/*.c)
FILE(GLOB_RECURSE CORE_PROTOBUF_LIST_CSRC CONFIGURE_DEPENDS ${CORE_LIST}/../../../public/*.cc)
SOURCE_GROUP("Header Files" FILES ${CORE_LIST_INCLUDE})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_SRC})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_CSRC})
SOURCE_GROUP("Protobuf Source Files" FILES ${CORE_PROTOBUF_LIST_CSRC})

if(NOT NGL_USE_UB_CPP)
	list(FILTER CORE_LIST_SRC EXCLUDE REGEX "ub_.*\\.cpp$")
	list(APPEND CORE_LIST_SRC ${NGL_PUBLIC_CPP_SRC})
	list(APPEND CORE_LIST_CSRC ${NGL_PUBLIC_CPP_CSRC})
endif()

ADD_EXECUTABLE(makeproto
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
	${CORE_PROTOBUF_LIST_CSRC}
)

target_include_directories(makeproto PRIVATE ${CORE_LIST})
target_link_libraries(makeproto PRIVATE ngl_build_options)
ngl_apply_runtime_deps(makeproto)
ngl_apply_msvc_defaults(makeproto)