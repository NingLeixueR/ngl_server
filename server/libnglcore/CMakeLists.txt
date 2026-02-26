cmake_minimum_required(VERSION 3.5)

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


SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

ADD_DEFINITIONS(-DNGL_EXPORT)

# ============ 加载 Conan 配置 ============
if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()
# =========================================
# 查找依赖
find_package(lua REQUIRED CONFIG)
find_package(protobuf REQUIRED CONFIG)
find_package(CURL REQUIRED CONFIG)
find_package(libmysqlclient REQUIRED CONFIG)   # 注意：这个包可能不存在！
find_package(hiredis REQUIRED CONFIG)
find_package(PostgreSQL REQUIRED CONFIG)


ADD_LIBRARY(nglcore SHARED 
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
	${CORE_PROTOBUF_LIST_CSRC}
)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")	
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
	ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
	SET_TARGET_PROPERTIES(nglcore PROPERTIES COMPILE_FLAGS "/EHa" )
	
	target_link_libraries(nglcore 
		lua::lua 
		protobuf::libprotobuf
		CURL::libcurl 
		libmysqlclient::libmysqlclient 
		hiredis::hiredis 
		libpq::libpq
	)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")





