include_guard(GLOBAL)

function(ngl_collect_imported_target_include_dirs out_var)
	set(result)
	foreach(target_name IN LISTS ARGN)
		if(NOT TARGET ${target_name})
			continue()
		endif()

		foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
			get_target_property(imported_location ${target_name} IMPORTED_LOCATION_${config})
			if(imported_location AND EXISTS "${imported_location}")
				get_filename_component(lib_dir "${imported_location}" DIRECTORY)
				get_filename_component(package_dir "${lib_dir}" DIRECTORY)
				if(EXISTS "${package_dir}/include")
					list(APPEND result "${package_dir}/include")
				endif()
			endif()
		endforeach()

		get_target_property(interface_links ${target_name} INTERFACE_LINK_LIBRARIES)
		if(NOT interface_links)
			continue()
		endif()

		foreach(link_item IN LISTS interface_links)
			set(link_targets)
			if(TARGET "${link_item}")
				list(APPEND link_targets "${link_item}")
			endif()

			string(REGEX MATCHALL "CONAN_LIB::[A-Za-z0-9.+_-]+" extracted_targets "${link_item}")
			list(APPEND link_targets ${extracted_targets})
			list(REMOVE_DUPLICATES link_targets)

			foreach(link_target IN LISTS link_targets)
				if(NOT TARGET ${link_target})
					continue()
				endif()
				foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
					get_target_property(imported_location ${link_target} IMPORTED_LOCATION_${config})
					if(imported_location AND EXISTS "${imported_location}")
						get_filename_component(lib_dir "${imported_location}" DIRECTORY)
						get_filename_component(package_dir "${lib_dir}" DIRECTORY)
						if(EXISTS "${package_dir}/include")
							list(APPEND result "${package_dir}/include")
						endif()
					endif()
				endforeach()
			endforeach()
		endforeach()
	endforeach()

	list(REMOVE_DUPLICATES result)
	set(${out_var} "${result}" PARENT_SCOPE)
endfunction()

function(ngl_collect_openssl_include_dirs out_var)
	set(result)

	foreach(package_var
		openssl_PACKAGE_FOLDER_DEBUG
		openssl_PACKAGE_FOLDER_RELEASE
		openssl_PACKAGE_FOLDER_RELWITHDEBINFO
		openssl_PACKAGE_FOLDER_MINSIZEREL
	)
		if(DEFINED ${package_var} AND EXISTS "${${package_var}}/include")
			list(APPEND result "${${package_var}}/include")
		endif()
	endforeach()

	ngl_collect_imported_target_include_dirs(imported_result OpenSSL::SSL OpenSSL::Crypto)
	list(APPEND result ${imported_result})
	list(REMOVE_DUPLICATES result)
	set(${out_var} "${result}" PARENT_SCOPE)
endfunction()

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
	if(NOT TARGET OpenSSL::SSL)
		find_package(OpenSSL REQUIRED)
		foreach(package_var
			openssl_PACKAGE_FOLDER_DEBUG
			openssl_PACKAGE_FOLDER_RELEASE
			openssl_PACKAGE_FOLDER_RELWITHDEBINFO
			openssl_PACKAGE_FOLDER_MINSIZEREL
		)
			if(DEFINED ${package_var})
				set(${package_var} "${${package_var}}" PARENT_SCOPE)
			endif()
		endforeach()
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
		OpenSSL::SSL
		OpenSSL::Crypto
	)

	ngl_collect_openssl_include_dirs(openssl_include_dirs)
	message(STATUS "NGL OpenSSL include dirs for ${target_name}: ${openssl_include_dirs}")
	if(openssl_include_dirs)
		target_include_directories(${target_name} SYSTEM ${link_scope}
			${openssl_include_dirs}
		)
	endif()

	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		target_link_libraries(${target_name} ${link_scope} libuuid::libuuid)
	endif()
endfunction()
