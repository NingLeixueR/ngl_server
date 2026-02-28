cmake_minimum_required(VERSION 3.5)

PROJECT (libmycore)

SET(PUBLIC_LIST ${PROJECT_SOURCE_DIR})
FILE(GLOB_RECURSE PUBLIC_LIST_INCLUDE ${PUBLIC_LIST}/*.h ) 
FILE(GLOB_RECURSE PUBLIC_LIST_SRC ${PUBLIC_LIST}/*.cpp) 
FILE(GLOB_RECURSE PUBLIC_LIST_CSRC ${PUBLIC_LIST}/*.c) 
FILE(GLOB_RECURSE CORE_LIST_PBSRC ${PUBLIC_LIST}/*.cc) 
SOURCE_GROUP("Header Files" FILES ${PUBLIC_LIST_INCLUDE})
SOURCE_GROUP("Source Files" FILES ${PUBLIC_LIST_SRC})
SOURCE_GROUP("Source Files" FILES ${PUBLIC_LIST_CSRC})
SOURCE_GROUP("Pb Source Files" FILES ${CORE_LIST_PBSRC})

find_package(lua REQUIRED)
find_package(protobuf REQUIRED)
find_package(CURL REQUIRED)
find_package(libmysqlclient REQUIRED)
find_package(hiredis REQUIRED)
find_package(PostgreSQL REQUIRED)

INCLUDE_DIRECTORIES(${lua_INCLUDE_DIRS})   
INCLUDE_DIRECTORIES(${protobuf_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${libmysqlclient_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${hiredis_INCLUDE_DIRS})  
INCLUDE_DIRECTORIES(${PostgreSQL_INCLUDE_DIRS})


# 判断是否在 GitHub Actions 环境中
if(DEFINED ENV{GITHUB_ACTIONS} AND "$ENV{GITHUB_ACTIONS}" STREQUAL "true")
    message(STATUS "当前构建环境：GitHub Actions CI/CD")
    # 在这里添加 GHA 环境专属的配置（比如开启警告、禁用测试、设置编译选项等）
    set(IS_GITHUB_ACTIONS_BUILD OFF)
else()
    message(STATUS "当前构建环境：本地开发环境")
    set(IS_GITHUB_ACTIONS_BUILD ON)
endif()

# 后续可通过 IS_GITHUB_ACTIONS_BUILD 变量复用判断结果
if(IS_GITHUB_ACTIONS_BUILD)
    IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
		ADD_LIBRARY(libmycore STATIC ${PUBLIC_LIST_INCLUDE} ${PUBLIC_LIST_SRC} ${PUBLIC_LIST_CSRC} ${CORE_LIST_PBSRC})
		target_link_libraries(libmycore PRIVATE lua::lua)
		target_link_libraries(libmycore PRIVATE protobuf::protobuf)
		target_link_libraries(libmycore PRIVATE CURL::libcurl)
		target_link_libraries(libmycore PRIVATE libmysqlclient::libmysqlclient)
		target_link_libraries(libmycore PRIVATE hiredis::hiredis)
		target_link_libraries(libmycore PRIVATE libpq::libpq)
	ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
endif()






