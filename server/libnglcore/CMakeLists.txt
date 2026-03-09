cmake_minimum_required(VERSION 3.15)

PROJECT (nglcore)

INCLUDE_DIRECTORIES(./core)
SET(CORE_LIST ${PROJECT_SOURCE_DIR}/core)
FILE(GLOB_RECURSE CORE_LIST_INCLUDE ${CORE_LIST}/*.h ) 
FILE(GLOB_RECURSE CORE_LIST_SRC ${CORE_LIST}/*.cpp) 
FILE(GLOB_RECURSE CORE_LIST_CSRC ${CORE_LIST}/*.c) 
FILE(GLOB_RECURSE CORE_PROTOBUF_LIST_CSRC ${CORE_LIST}/../../../public/*.cc) 
SOURCE_GROUP("Header Files" FILES ${CORE_LIST_INCLUDE})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_SRC})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_CSRC})
SOURCE_GROUP("Protobuf Source Files" FILES ${CORE_PROTOBUF_LIST_CSRC})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(lua REQUIRED)
find_package(protobuf REQUIRED)
find_package(CURL REQUIRED)
find_package(libmysqlclient REQUIRED)
find_package(hiredis REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(RapidJSON REQUIRED)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	find_package(libuuid REQUIRED)
	INCLUDE_DIRECTORIES(${libuuid_INCLUDE_DIRS})
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")


INCLUDE_DIRECTORIES(${lua_INCLUDE_DIRS})   
INCLUDE_DIRECTORIES(${protobuf_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${libmysqlclient_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${hiredis_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${PostgreSQL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${RapidJSON_INCLUDE_DIRS})

ADD_LIBRARY(nglcore SHARED 
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
	${CORE_PROTOBUF_LIST_CSRC}
)
target_compile_definitions(nglcore PRIVATE NGL_EXPORT)

# 链接
target_link_libraries(nglcore PRIVATE lua::lua)
target_link_libraries(nglcore PRIVATE protobuf::protobuf)
target_link_libraries(nglcore PRIVATE CURL::libcurl)
target_link_libraries(nglcore PRIVATE libmysqlclient::libmysqlclient)
target_link_libraries(nglcore PRIVATE hiredis::hiredis)
target_link_libraries(nglcore PRIVATE libpq::libpq)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(nglcore PRIVATE libuuid::libuuid)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

ngl_apply_msvc_defaults(nglcore)





