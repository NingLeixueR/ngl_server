cmake_minimum_required(VERSION 3.5)

PROJECT (ngl)

set(CMAKE_CXX_STANDARD 20)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 强制要求C++20，不兼容则报错
set(CMAKE_CXX_EXTENSIONS OFF)        # 关闭微软C++扩展，启用【纯标准C++20】
# 开启标准C++预处理器，支持__VA_OPT__
add_compile_options(/Zc:preprocessor)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

#IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
#set(CMAKE_CXX_FLAGS "${COLLECT_GCC} -std=c++20 -march=native -msse4.1 -O3 -lrt -L/usr/lib64/mysql/ -L/usr/lib/x86_64-linux-gnu -I/usr/include/postgresql -lcurl -L/usr/lib64/ -I/usr/local/include/google/protobuf/ -L/usr/local/lib/ -L`pwd`/../../../lib -I/usr/include/mysql -I/usr/include/curl -I/usr/include/x86_64-linux-gnu/curl -L/usr/lib/x86_64-linux-gnu/ -I/usr/include -I/usr/include/protobuf -L/usr/local/lib/ -I/usr/local/include -lmysqlclient")
#ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

INCLUDE_DIRECTORIES(/)
INCLUDE_DIRECTORIES(public/cpp)
INCLUDE_DIRECTORIES(public/cpp/third_party)
INCLUDE_DIRECTORIES(public/cpp/third_party/asio/include)
INCLUDE_DIRECTORIES(public/cpp/third_party/asio/include/asio)
INCLUDE_DIRECTORIES(public/cpp/third_party/base64)
INCLUDE_DIRECTORIES(public/cpp/third_party/hmac_sha1)
INCLUDE_DIRECTORIES(public/cpp/third_party/json)
INCLUDE_DIRECTORIES(public/cpp/third_party/kcp)
INCLUDE_DIRECTORIES(public/cpp/third_party/sha1)
INCLUDE_DIRECTORIES(public/cpp/third_party/tinyxml2)
INCLUDE_DIRECTORIES(public/cpp/third_party/utf8cpp)
INCLUDE_DIRECTORIES(public/cpp/third_party/utf8cpp/utf8)
INCLUDE_DIRECTORIES(public/cpp/third_party/varint)

ADD_DEFINITIONS(-DASIO_STANDALONE)
#ADD_DEFINITIONS(-DASIO_NO_DEPRECATED)
ADD_DEFINITIONS(-D_WEBSOCKETPP_CPP11_TYPE_TRAITS_)
ADD_DEFINITIONS(-DUSE_BOOST_INTERPROCESS)


IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)
	ADD_DEFINITIONS(-DBOOST_THREAD_BUILD_DLL)
	ADD_DEFINITIONS(-DBUILDING_LIBCURL)
	ADD_DEFINITIONS(-DHTTP_ONLY)
	ADD_DEFINITIONS(-DNOMINMAX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")	
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
	ADD_DEFINITIONS(-DBOOST_HAS_PTHREADS)
	ADD_DEFINITIONS(-DBOOST_SP_USE_PTHREADS)
	ADD_DEFINITIONS(-DBOOST_AC_USE_PTHREADS)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/bin/Debug)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_subdirectory(public)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

add_subdirectory(server)