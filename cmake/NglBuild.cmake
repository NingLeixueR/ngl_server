include_guard(GLOBAL)

function(ngl_apply_compiler_warnings target_name)
	if(NOT NGL_ENABLE_STRONG_WARNINGS)
		return()
	endif()

	if(MSVC)
		target_compile_options(${target_name} PRIVATE /W4)
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
		target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
	endif()
endfunction()

function(ngl_apply_sanitizers target_name)
	if(NOT NGL_ENABLE_SANITIZERS)
		return()
	endif()

	if(MSVC)
		message(WARNING "NGL_ENABLE_SANITIZERS is not supported for MSVC target ${target_name}")
		return()
	endif()

	if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
		message(WARNING "NGL_ENABLE_SANITIZERS is only supported for GNU and Clang toolchains")
		return()
	endif()

	target_compile_options(${target_name} PRIVATE
		-fsanitize=address,undefined
		-fno-omit-frame-pointer
		-fno-sanitize-recover=all
	)

	get_target_property(target_type ${target_name} TYPE)
	if(NOT target_type STREQUAL "STATIC_LIBRARY")
		target_link_options(${target_name} PRIVATE
			-fsanitize=address,undefined
			-fno-omit-frame-pointer
			-fno-sanitize-recover=all
		)
	endif()
endfunction()

function(ngl_apply_msvc_defaults target_name)
	if(MSVC)
		target_compile_definitions(${target_name} PRIVATE WIN32_LEAN_AND_MEAN)
		target_compile_options(${target_name} PRIVATE /EHa)
		# Prevent parallel compilation from contending on a shared PDB file (/Zi).
		target_compile_options(${target_name} PRIVATE /FS)
	endif()

	ngl_apply_compiler_warnings(${target_name})
	ngl_apply_sanitizers(${target_name})
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
