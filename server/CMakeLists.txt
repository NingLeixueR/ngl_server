cmake_minimum_required(VERSION 3.15)

function(ngl_apply_msvc_defaults target_name)
	if(MSVC)
		target_compile_definitions(${target_name} PRIVATE WIN32_LEAN_AND_MEAN)
		target_compile_options(${target_name} PRIVATE /EHa)
		# Prevent parallel compilation from contending on a shared PDB file (/Zi).
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
	set(link_scope PRIVATE)
	if(ARGC GREATER 1)
		set(link_scope ${ARGV1})
	endif()

	ngl_find_runtime_deps()

	target_link_libraries(${target_name} ${link_scope}
		lua::lua
		protobuf::protobuf
		CURL::libcurl
		libmysqlclient::libmysqlclient
		hiredis::hiredis
		libpq::libpq
		rapidjson
	)

	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		target_link_libraries(${target_name} ${link_scope} libuuid::libuuid)
	endif()
endfunction()

if(NOT DEFINED NGL_IS_LOCAL_BUILD)
	set(NGL_IS_LOCAL_BUILD ON)
endif()

add_subdirectory(libnglcore)

if(NGL_IS_LOCAL_BUILD AND CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_subdirectory(rebuild)
	add_subdirectory(makeproto)
	add_subdirectory(idl)
endif()

add_subdirectory(node)

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()
