# File overview: Defines CMake targets and build rules for tests.

find_package(GTest REQUIRED)
include(GoogleTest)

if(TARGET GTest::gtest_main)
	set(NGL_GTEST_MAIN_TARGET GTest::gtest_main)
elseif(TARGET gtest::gtest_main)
	set(NGL_GTEST_MAIN_TARGET gtest::gtest_main)
else()
	message(FATAL_ERROR "Unable to find a gtest_main target from GTest package")
endif()

file(GLOB_RECURSE NGL_TEST_CORE_PROTOBUF_CSRC CONFIGURE_DEPENDS
	${CMAKE_SOURCE_DIR}/public/*.cc
)
list(SORT NGL_TEST_CORE_PROTOBUF_CSRC)
source_group("Protobuf Source Files" FILES ${NGL_TEST_CORE_PROTOBUF_CSRC})

file(GLOB NGL_TEST_CASE_SRCS CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*_gtest.cpp
)
list(SORT NGL_TEST_CASE_SRCS)
source_group("Test Files" FILES ${NGL_TEST_CASE_SRCS})
ngl_configure_vs_unity_filters()

if(NOT NGL_TEST_CASE_SRCS)
	message(FATAL_ERROR "No *_gtest.cpp files found under server/tests")
endif()

set(NGL_TEST_SUPPORT_SRCS
	${CMAKE_SOURCE_DIR}/server/libnglcore/core/server_main.cpp
	${CMAKE_SOURCE_DIR}/server/libnglcore/core/startup_support.cpp
	${NGL_PUBLIC_CPP_SRC}
	${NGL_PUBLIC_CPP_CSRC}
)

if(NGLCORE_BUILD_SHARED)
	add_executable(ngl_test
		${NGL_TEST_CASE_SRCS}
		${NGL_TEST_SUPPORT_SRCS}
		${NGL_TEST_CORE_PROTOBUF_CSRC}
	)
else()
	add_executable(ngl_test
		${NGL_TEST_CASE_SRCS}
	)
endif()

target_include_directories(ngl_test PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../libnglcore/core
)

if(NGLCORE_BUILD_SHARED)
	target_compile_definitions(ngl_test PRIVATE NGLCORE_EXPORTS)
	target_link_libraries(ngl_test PRIVATE
		ngl_build_options
		${NGL_GTEST_MAIN_TARGET}
	)
else()
	target_link_libraries(ngl_test PRIVATE
		nglcore
		${NGL_GTEST_MAIN_TARGET}
	)
	set_target_properties(ngl_test PROPERTIES UNITY_BUILD OFF)
endif()
ngl_apply_runtime_deps(ngl_test PRIVATE)
if(NGLCORE_BUILD_SHARED)
	ngl_apply_unity_build(ngl_test)
endif()
ngl_apply_msvc_defaults(ngl_test)

gtest_discover_tests(ngl_test
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DISCOVERY_TIMEOUT 30
	PROPERTIES LABELS "unit"
)
