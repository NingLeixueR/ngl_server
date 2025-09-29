cmake_minimum_required(VERSION 2.6)

PROJECT (libnglcore)

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

ADD_DEFINITIONS(-DNGL_EXPORT)

ADD_LIBRARY(nglcore SHARED 
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
	${CORE_PROTOBUF_LIST_CSRC}
)

#set_target_properties
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(nglcore protobuf)
	target_link_libraries(nglcore pthread)
	target_link_libraries(nglcore uuid)
	target_link_libraries(nglcore mysqlclient)
	target_link_libraries(nglcore curl)
	target_link_libraries(nglcore hiredis)
	target_link_libraries(nglcore lua)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
	ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
	SET_TARGET_PROPERTIES(libnglcore PROPERTIES COMPILE_FLAGS "/EHa" )
	TARGET_LINK_LIBRARIES(libnglcore debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/libcurl.lib)
	TARGET_LINK_LIBRARIES(libnglcore optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/libcurl.lib)
	TARGET_LINK_LIBRARIES(libnglcore debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/libmysql.lib)
	TARGET_LINK_LIBRARIES(libnglcore optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/libmysql.lib)
	TARGET_LINK_LIBRARIES(libnglcore debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/hiredis.lib)
	TARGET_LINK_LIBRARIES(libnglcore optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/hiredis.lib)
	TARGET_LINK_LIBRARIES(libnglcore debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/libprotobuf.lib)
	TARGET_LINK_LIBRARIES(libnglcore optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/libprotobuf.lib)
	TARGET_LINK_LIBRARIES(libnglcore debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/lua54.lib)
	TARGET_LINK_LIBRARIES(libnglcore optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/lua54.lib)
	TARGET_LINK_LIBRARIES(libnglcore debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/luacjosn.lib)
	TARGET_LINK_LIBRARIES(libnglcore optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/luacjosn.lib)
	#TARGET_LINK_LIBRARIES(libnglcore debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/cryptlib.lib)
	#TARGET_LINK_LIBRARIES(libnglcore optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/cryptlib.lib)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")





