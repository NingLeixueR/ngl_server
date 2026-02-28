cmake_minimum_required(VERSION 3.5)

add_subdirectory(libnglcore)

# 判断是否在 GitHub Actions 环境中
if(DEFINED ENV{GITHUB_ACTIONS} AND "$ENV{GITHUB_ACTIONS}" STREQUAL "true")
    message(STATUS "当前构建环境：GitHub Actions CI/CD")
    # 在这里添加 GHA 环境专属的配置（比如开启警告、禁用测试、设置编译选项等）
    set(IS_GITHUB_ACTIONS_BUILD OFF)
else()
    message(STATUS "当前构建环境：本地开发环境")
    set(IS_GITHUB_ACTIONS_BUILD ON)
endif()

if(IS_GITHUB_ACTIONS_BUILD)
    IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
		add_subdirectory(rebuild)
		add_subdirectory(makeproto)
		add_subdirectory(idl)
	ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
endif()

add_subdirectory(node)

