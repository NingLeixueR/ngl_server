cmake_minimum_required(VERSION 3.15)

function(ngl_apply_msvc_defaults target_name)
	if(MSVC)
		target_compile_definitions(${target_name} PRIVATE WIN32_LEAN_AND_MEAN)
		target_compile_options(${target_name} PRIVATE /EHa)
		# Prevent parallel compilation from contending on a shared PDB file (/Zi)
		target_compile_options(${target_name} PRIVATE /FS)
	endif()
endfunction()

function(ngl_find_runtime_deps)
	if(NOT TARGET lua::lua)
		find_package(lua REQUIRED)
	endif()
	if(NOT TARGET protobuf::protobuf)
		find_package(protobuf REQUIRED)
	endif()
	if(NOT TARGET CURL::libcurl)
		find_package(CURL REQUIRED)
	endif()
	if(NOT TARGET libmysqlclient::libmysqlclient)
		find_package(libmysqlclient REQUIRED)
	endif()
	if(NOT TARGET hiredis::hiredis)
		find_package(hiredis REQUIRED)
	endif()
	if(NOT TARGET libpq::libpq)
		find_package(PostgreSQL REQUIRED)
	endif()
	if(NOT TARGET rapidjson)
		find_package(RapidJSON REQUIRED)
	endif()
	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		if(NOT TARGET libuuid::libuuid)
			find_package(libuuid REQUIRED)
		endif()
	endif()
endfunction()

function(ngl_apply_runtime_deps target_name)
	ngl_find_runtime_deps()

	target_link_libraries(${target_name} PRIVATE
		lua::lua
		protobuf::protobuf
		CURL::libcurl
		libmysqlclient::libmysqlclient
		hiredis::hiredis
		libpq::libpq
		rapidjson
	)

	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		target_link_libraries(${target_name} PRIVATE libuuid::libuuid)
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

