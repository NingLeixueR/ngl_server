cmake_minimum_required(VERSION 2.6)

PROJECT (makeproto)

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)
ADD_DEFINITIONS(-DBOOST_HAS_WINTHREADS)
ADD_DEFINITIONS(-DBOOST_THREAD_BUILD_DLL)

ADD_DEFINITIONS(-D WIN32_LEAN_AND_MEAN)

ADD_DEFINITIONS(-DSERVER_GATEWAY)

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

ADD_EXECUTABLE(makeproto
${CORE_LIST_INCLUDE}
${CORE_LIST_SRC}
${CORE_LIST_CSRC}
${CORE_PROTOBUF_LIST_CSRC}
)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
target_link_libraries(makeproto boost)
target_link_libraries(makeproto protobuf)
target_link_libraries(makeproto pthread)
target_link_libraries(makeproto boost)
target_link_libraries(makeproto hiredis)
target_link_libraries(makeproto cryptopp)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
SET_TARGET_PROPERTIES(node PROPERTIES COMPILE_FLAGS "/EHa" )

TARGET_LINK_LIBRARIES(makeproto debug ${PROJECT_SOURCE_DIR}/../../lib/Debug/libboost.lib)
TARGET_LINK_LIBRARIES(makeproto optimized ${PROJECT_SOURCE_DIR}/../../lib/Release/libboost.lib)
TARGET_LINK_LIBRARIES(makeproto debug ${PROJECT_SOURCE_DIR}/../../lib/Debug/libcurl.lib)
TARGET_LINK_LIBRARIES(makeproto optimized ${PROJECT_SOURCE_DIR}/../../lib/Release/libcurl.lib)
TARGET_LINK_LIBRARIES(makeproto debug ${PROJECT_SOURCE_DIR}/../../lib/Debug/libmysql.lib)
TARGET_LINK_LIBRARIES(makeproto optimized ${PROJECT_SOURCE_DIR}/../../lib/Release/libmysql.lib)
TARGET_LINK_LIBRARIES(makeproto debug ${PROJECT_SOURCE_DIR}/../../lib/Debug/cryptlib.lib)
TARGET_LINK_LIBRARIES(makeproto optimized ${PROJECT_SOURCE_DIR}/../../lib/Release/cryptlib.lib)
TARGET_LINK_LIBRARIES(makeproto debug ${PROJECT_SOURCE_DIR}/../../lib/Debug/hiredis.lib)
TARGET_LINK_LIBRARIES(makeproto optimized ${PROJECT_SOURCE_DIR}/../../lib/Release/hiredis.lib)
TARGET_LINK_LIBRARIES(makeproto debug ${PROJECT_SOURCE_DIR}/../../lib/Debug/libprotobufd.lib)
TARGET_LINK_LIBRARIES(makeproto optimized ${PROJECT_SOURCE_DIR}/../../lib/Release/libprotobufd.lib)
TARGET_LINK_LIBRARIES(makeproto debug ${PROJECT_SOURCE_DIR}/../../lib/Debug/libprotocd.lib)
TARGET_LINK_LIBRARIES(makeproto optimized ${PROJECT_SOURCE_DIR}/../../lib/Release/libprotocd.lib)

ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")



