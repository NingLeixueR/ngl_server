# File overview: Defines CMake targets and build rules for .

cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0091)
	cmake_policy(SET CMP0091 NEW)
endif()

PROJECT (ngl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Require C++20 and fail if the compiler is incompatible.
# Set(CMAKE_CXX_EXTENSIONS OFF) # close C++, [ C++20]
option(NGL_ENABLE_STRONG_WARNINGS "Enable stronger compiler warnings for project targets" ON)
option(NGL_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer on supported toolchains" OFF)
option(NGL_ENABLE_NATIVE_UNITY_BUILD "Enable CMake native unity build for large targets" OFF)
option(NGLCORE_BUILD_SHARED "Build nglcore as a shared library instead of a static library" OFF)
set(NGL_UNITY_BUILD_BATCH_SIZE 100 CACHE STRING "Maximum number of non-protobuf source files per native unity batch")

option(BUILD_TESTING "Build and register tests" ON)
option(NGL_TEST_REGISTER_PERF "Register performance-oriented GoogleTest suites with CTest" OFF)
if(BUILD_TESTING)
	enable_testing()
endif()

if(DEFINED ENV{GITHUB_ACTIONS} AND "$ENV{GITHUB_ACTIONS}" STREQUAL "true")
	set(NGL_IS_LOCAL_BUILD OFF)
	message(STATUS "Build environment: GitHub Actions CI")
else()
	set(NGL_IS_LOCAL_BUILD ON)
	message(STATUS "Build environment: Local development")
endif()

include(${CMAKE_SOURCE_DIR}/cmake/NglBuild.cmake)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(behaviortree_cpp REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# Keep predefined targets (ALL_BUILD / ZERO_CHECK / INSTALL) at solution top level.
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")

add_library(ngl_build_options INTERFACE)
if(TARGET Boost::headers)
target_link_libraries(ngl_build_options INTERFACE Boost::headers)
elseif(TARGET Boost::boost)
	target_link_libraries(ngl_build_options INTERFACE Boost::boost)
else()
	message(FATAL_ERROR "Unable to find a Boost headers target")
endif()
if(TARGET Boost::program_options)
	target_link_libraries(ngl_build_options INTERFACE Boost::program_options)
else()
	message(FATAL_ERROR "Unable to find Boost::program_options target")
endif()
target_link_libraries(ngl_build_options INTERFACE
	BT::behaviortree_cpp
)
target_include_directories(ngl_build_options INTERFACE
	${CMAKE_SOURCE_DIR}/public/cpp
)
target_include_directories(ngl_build_options SYSTEM INTERFACE
	${CMAKE_SOURCE_DIR}/public/cpp/third_party
	${CMAKE_SOURCE_DIR}/public/cpp/third_party/kcp
	${CMAKE_SOURCE_DIR}/public/cpp/third_party/utf8cpp
	${CMAKE_SOURCE_DIR}/public/cpp/third_party/tinyxml2
)
target_compile_definitions(ngl_build_options INTERFACE
	_WEBSOCKETPP_CPP11_TYPE_TRAITS_
	USE_BOOST_INTERPROCESS
	$<$<PLATFORM_ID:Windows>:BOOST_ALL_NO_LIB;BOOST_ALL_STATIC_LINK;BOOST_PROGRAM_OPTIONS_STATIC_LINK;BOOST_THREAD_BUILD_DLL;BOOST_SYSTEM_USE_UTF8;BUILDING_LIBCURL;HTTP_ONLY;NOMINMAX;_WIN32_WINNT=0x0601;WINVER=0x0601>
	$<$<PLATFORM_ID:Linux>:BOOST_HAS_PTHREADS;BOOST_SP_USE_PTHREADS;BOOST_AC_USE_PTHREADS>
)
target_compile_options(ngl_build_options INTERFACE
	$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor;/bigobj;/utf-8>
)

set(OUTPUT_BASE_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/$<CONFIG>)

set(NGL_PUBLIC_CPP_DIR ${CMAKE_SOURCE_DIR}/public/cpp)
file(GLOB_RECURSE NGL_PUBLIC_CPP_SRC CONFIGURE_DEPENDS ${NGL_PUBLIC_CPP_DIR}/*.cpp)
file(GLOB_RECURSE NGL_PUBLIC_CPP_CSRC CONFIGURE_DEPENDS ${NGL_PUBLIC_CPP_DIR}/*.c)

add_subdirectory(server)
