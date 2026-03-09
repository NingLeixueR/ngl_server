cmake_minimum_required(VERSION 3.15)

PROJECT (rebuild)

INCLUDE_DIRECTORIES(./core)
########################################################################################core
SET(CORE_LIST ${PROJECT_SOURCE_DIR}/core)
FILE(GLOB_RECURSE CORE_LIST_INCLUDE ${CORE_LIST}/*.h ) 
FILE(GLOB_RECURSE CORE_LIST_SRC ${CORE_LIST}/*.cpp) 
FILE(GLOB_RECURSE CORE_LIST_CSRC ${CORE_LIST}/*.c) 
FILE(GLOB_RECURSE CORE_PROTOBUF_LIST_CSRC ${CORE_LIST}/../../../public/*.cc) 
SOURCE_GROUP("Header Files" FILES ${CORE_LIST_INCLUDE})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_SRC})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_CSRC})
SOURCE_GROUP("Protobuf Source Files" FILES ${CORE_PROTOBUF_LIST_CSRC})

find_package(lua REQUIRED)
find_package(protobuf REQUIRED)
find_package(CURL REQUIRED)
find_package(libmysqlclient REQUIRED)
find_package(hiredis REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(RapidJSON REQUIRED)

INCLUDE_DIRECTORIES(${lua_INCLUDE_DIRS})   
INCLUDE_DIRECTORIES(${protobuf_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${libmysqlclient_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${hiredis_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${PostgreSQL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${RapidJSON_INCLUDE_DIRS})

ADD_EXECUTABLE(rebuild
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
	${CORE_PROTOBUF_LIST_CSRC}
)

target_link_libraries(rebuild PRIVATE lua::lua)
target_link_libraries(rebuild PRIVATE protobuf::protobuf)
target_link_libraries(rebuild PRIVATE CURL::libcurl)
target_link_libraries(rebuild PRIVATE libmysqlclient::libmysqlclient)
target_link_libraries(rebuild PRIVATE hiredis::hiredis)
target_link_libraries(rebuild PRIVATE libpq::libpq)

ngl_apply_msvc_defaults(rebuild)

