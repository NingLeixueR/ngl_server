cmake_minimum_required(VERSION 3.15)

project(libmycore)

set(PUBLIC_LIST ${PROJECT_SOURCE_DIR})
file(GLOB_RECURSE PUBLIC_LIST_INCLUDE CONFIGURE_DEPENDS ${PUBLIC_LIST}/*.h)
file(GLOB_RECURSE PUBLIC_LIST_SRC CONFIGURE_DEPENDS ${PUBLIC_LIST}/*.cpp)
file(GLOB_RECURSE PUBLIC_LIST_CSRC CONFIGURE_DEPENDS ${PUBLIC_LIST}/*.c)
file(GLOB_RECURSE CORE_LIST_PBSRC CONFIGURE_DEPENDS ${PUBLIC_LIST}/*.cc)

source_group("Header Files" FILES ${PUBLIC_LIST_INCLUDE})
source_group("Source Files" FILES ${PUBLIC_LIST_SRC} ${PUBLIC_LIST_CSRC})
source_group("Pb Source Files" FILES ${CORE_LIST_PBSRC})

find_package(lua REQUIRED)
find_package(protobuf REQUIRED)
find_package(CURL REQUIRED)
find_package(libmysqlclient REQUIRED)
find_package(hiredis REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(RapidJSON REQUIRED)

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
		lua::lua
		protobuf::protobuf
		CURL::libcurl
		libmysqlclient::libmysqlclient
		hiredis::hiredis
		libpq::libpq
		rapidjson
	)

	target_include_directories(libmycore PRIVATE
		${lua_INCLUDE_DIRS}
		${protobuf_INCLUDE_DIRS}
		${CURL_INCLUDE_DIRS}
		${libmysqlclient_INCLUDE_DIRS}
		${hiredis_INCLUDE_DIRS}
		${PostgreSQL_INCLUDE_DIRS}
		${RapidJSON_INCLUDE_DIRS}
	)
endif()
