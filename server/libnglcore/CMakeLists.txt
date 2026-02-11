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

ADD_LIBRARY(nglcore SHARED 
	${CORE_LIST_INCLUDE}
	${CORE_LIST_SRC}
	${CORE_LIST_CSRC}
	${CORE_PROTOBUF_LIST_CSRC}
)

#set_target_properties
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	
	# 2. 设置 SO 的生成路径（关键：ARCHIVE_OUTPUT_DIRECTORY 对 Linux SO 生效）
	# 方式1：生成到项目根目录的 lib 文件夹（绝对路径，推荐）
	set_target_properties(nglcore PROPERTIES
		# SO 生成路径：your_project/lib
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
		# 可选：按编译类型分目录（如 lib/Debug、lib/Release）
		# LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib/Debug"
		# LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib/Release"
	)

	target_link_libraries(nglcore PRIVATE
            protobuf 
            pthread
            uuid
            mysqlclient
            curl
            hiredis
            lua
            pq
    )
	#target_link_libraries(nglcore intl)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")


IF (CMAKE_SYSTEM_NAME MATCHES "Windows")	
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
	ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
	SET_TARGET_PROPERTIES(nglcore PROPERTIES COMPILE_FLAGS "/EHa" )
	TARGET_LINK_LIBRARIES(nglcore debug ${PROJECT_SOURCE_DIR}/../../third_party/lib/libcurl.lib)
	TARGET_LINK_LIBRARIES(nglcore optimized ${PROJECT_SOURCE_DIR}/../../third_party/lib/libcurl.lib)
	TARGET_LINK_LIBRARIES(nglcore debug ${PROJECT_SOURCE_DIR}/../../third_party/lib/libmysql.lib)
	TARGET_LINK_LIBRARIES(nglcore optimized ${PROJECT_SOURCE_DIR}/../../third_party/lib/libmysql.lib)
	TARGET_LINK_LIBRARIES(nglcore debug ${PROJECT_SOURCE_DIR}/../../third_party/lib/hiredis.lib)
	TARGET_LINK_LIBRARIES(nglcore optimized ${PROJECT_SOURCE_DIR}/../../third_party/lib/hiredis.lib)
	TARGET_LINK_LIBRARIES(nglcore debug ${PROJECT_SOURCE_DIR}/../../third_party/lib/libprotobuf.lib)
	TARGET_LINK_LIBRARIES(nglcore optimized ${PROJECT_SOURCE_DIR}/../../third_party/lib/libprotobuf.lib)
	TARGET_LINK_LIBRARIES(nglcore debug ${PROJECT_SOURCE_DIR}/../../third_party/lib/lua54.lib)
	TARGET_LINK_LIBRARIES(nglcore optimized ${PROJECT_SOURCE_DIR}/../../third_party/lib/lua54.lib)
	TARGET_LINK_LIBRARIES(nglcore debug ${PROJECT_SOURCE_DIR}/../../third_party/lib/luacjosn.lib)
	TARGET_LINK_LIBRARIES(nglcore optimized ${PROJECT_SOURCE_DIR}/../../third_party/lib/luacjosn.lib)
	TARGET_LINK_LIBRARIES(nglcore debug ${PROJECT_SOURCE_DIR}/../../third_party/lib/libpq.lib)
	TARGET_LINK_LIBRARIES(nglcore optimized ${PROJECT_SOURCE_DIR}/../../third_party/lib/libpq.lib)
	#TARGET_LINK_LIBRARIES(nglcore debug ${PROJECT_SOURCE_DIR}/../../third_party/lib/cryptlib.lib)
	#TARGET_LINK_LIBRARIES(nglcore optimized ${PROJECT_SOURCE_DIR}/../../third_party/lib/cryptlib.lib)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")





