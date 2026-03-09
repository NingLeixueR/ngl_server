cmake_minimum_required(VERSION 3.15)

function(ngl_apply_msvc_defaults target_name)
	if(MSVC)
		target_compile_definitions(${target_name} PRIVATE WIN32_LEAN_AND_MEAN)
		target_compile_options(${target_name} PRIVATE /EHa)
	endif()
endfunction()

add_subdirectory(libnglcore)

# 判断是否在 GitHub Actions 环境中
if(DEFINED ENV{GITHUB_ACTIONS} AND "$ENV{GITHUB_ACTIONS}" STREQUAL "true")
    message(STATUS "当前构建环境：GitHub Actions CI/CD")
    set(IS_LOCAL_BUILD OFF)
else()
    message(STATUS "当前构建环境：本地开发环境")
    set(IS_LOCAL_BUILD ON)
endif()

if(IS_LOCAL_BUILD)
    IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
		add_subdirectory(rebuild)
		add_subdirectory(makeproto)
		add_subdirectory(idl)
	ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
endif()

add_subdirectory(node)

