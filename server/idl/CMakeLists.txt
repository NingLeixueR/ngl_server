cmake_minimum_required(VERSION 2.6)

PROJECT (IDL)

INCLUDE_DIRECTORIES(./idl)
########################################################################################core
SET(CORE_LIST ${PROJECT_SOURCE_DIR}/idl)
FILE(GLOB_RECURSE CORE_LIST_INCLUDE ${CORE_LIST}/*.h ) 
FILE(GLOB_RECURSE CORE_LIST_SRC ${CORE_LIST}/*.cpp) 
FILE(GLOB_RECURSE CORE_LIST_CSRC ${CORE_LIST}/*.c) 
SOURCE_GROUP("Header Files" FILES ${CORE_LIST_INCLUDE})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_SRC})
SOURCE_GROUP("Source Files" FILES ${CORE_LIST_CSRC})

ADD_EXECUTABLE(IDL
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
)

TARGET_LINK_LIBRARIES(IDL debug ${PROJECT_SOURCE_DIR}/../../lib/winlib/libboost.lib)
TARGET_LINK_LIBRARIES(IDL optimized ${PROJECT_SOURCE_DIR}/../../lib/winlib/libboost.lib)



