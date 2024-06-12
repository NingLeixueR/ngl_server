cmake_minimum_required(VERSION 2.6)

PROJECT (IDL)

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


ADD_EXECUTABLE(IDL
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
	${CORE_PROTOBUF_LIST_CSRC}
)

#set_target_properties
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(IDL protobuf)
	target_link_libraries(IDL pthread)
	target_link_libraries(IDL uuid)
	target_link_libraries(IDL mysqlclient)
	target_link_libraries(IDL curl)
	target_link_libraries(IDL hiredis)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
	ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
	SET_TARGET_PROPERTIES(IDL PROPERTIES COMPILE_FLAGS "/EHa" )
	TARGET_LINK_LIBRARIES(IDL debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/libcurl.lib)
	TARGET_LINK_LIBRARIES(IDL optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/libcurl.lib)
	TARGET_LINK_LIBRARIES(IDL debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/libmysql.lib)
	TARGET_LINK_LIBRARIES(IDL optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/libmysql.lib)
	TARGET_LINK_LIBRARIES(IDL debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/hiredis.lib)
	TARGET_LINK_LIBRARIES(IDL optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/hiredis.lib)
	TARGET_LINK_LIBRARIES(IDL debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/libprotobuf.lib)
	TARGET_LINK_LIBRARIES(IDL optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/libprotobuf.lib)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")



