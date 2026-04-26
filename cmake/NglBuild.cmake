# File overview: Provides shared CMake helpers for NglBuild.

include_guard(GLOBAL)

function(ngl_add_mi target_name)
	if(NOT NGL_ENABLE_MIMALLOC)
		return()
	endif()

	if(NOT DEFINED NGL_MI_SRC)
		message(FATAL_ERROR "NGL_MI_SRC is not configured")
	endif()

	if(NOT DEFINED NGL_MI_LIB OR NOT TARGET ${NGL_MI_LIB})
		message(FATAL_ERROR "NGL_MI_LIB is not configured")
	endif()

	set_source_files_properties(${NGL_MI_SRC} PROPERTIES
		SKIP_UNITY_BUILD_INCLUSION ON
	)
	target_sources(${target_name} PRIVATE ${NGL_MI_SRC})
	target_link_libraries(${target_name} PRIVATE ${NGL_MI_LIB})
endfunction()

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
		if(CMAKE_GENERATOR MATCHES "Visual Studio")
			set_property(TARGET ${target_name} PROPERTY VS_GLOBAL_VcpkgEnabled false)
			if(NOT NGL_ENABLE_NATIVE_UNITY_BUILD)
				# Non-unity Visual Studio builds can batch many large translation units into
				# one cl invocation, which triggers C1060 on this codebase. Compile one file
				# at a time so CI can catch include issues without exhausting compiler heap.
				set_property(TARGET ${target_name} PROPERTY VS_NO_COMPILE_BATCHING ON)
			endif()
		endif()
		target_compile_definitions(${target_name} PRIVATE WIN32_LEAN_AND_MEAN)
		target_compile_options(${target_name} PRIVATE /EHa)
		# Prevent parallel compilation from contending on a shared PDB file (/Zi).
		target_compile_options(${target_name} PRIVATE /FS)
	endif()

	ngl_apply_compiler_warnings(${target_name})
	ngl_apply_sanitizers(${target_name})
endfunction()

function(ngl_assign_unity_groups target_name group_prefix batch_size)
	if(${batch_size} LESS 1)
		message(FATAL_ERROR "NGL_UNITY_BUILD_BATCH_SIZE must be >= 1")
	endif()

	set(source_index 0)
	foreach(source_file IN LISTS ARGN)
		math(EXPR group_index "${source_index} / ${batch_size}")
		set_source_files_properties(${source_file} PROPERTIES
			UNITY_GROUP "${target_name}_${group_prefix}_${group_index}"
		)
		math(EXPR source_index "${source_index} + 1")
	endforeach()
endfunction()

function(ngl_apply_unity_build target_name)
	if(NOT NGL_ENABLE_NATIVE_UNITY_BUILD)
		return()
	endif()

	get_target_property(target_sources ${target_name} SOURCES)
	if(NOT target_sources)
		return()
	endif()

	set(c_sources)
	set(cxx_sources)
	set(protobuf_cc_sources)

	foreach(source_file IN LISTS target_sources)
		if(source_file MATCHES "^\\$<")
			continue()
		endif()

		get_filename_component(source_ext "${source_file}" LAST_EXT)
		string(TOLOWER "${source_ext}" source_ext)

		if(source_ext STREQUAL ".c")
			list(APPEND c_sources "${source_file}")
		elseif(source_ext STREQUAL ".cc")
			if(source_file MATCHES [[(^|[\\/]).*\.pb\.cc$]])
				list(APPEND protobuf_cc_sources "${source_file}")
			else()
				list(APPEND cxx_sources "${source_file}")
			endif()
		elseif(source_ext STREQUAL ".cpp" OR source_ext STREQUAL ".cxx")
			list(APPEND cxx_sources "${source_file}")
		endif()
	endforeach()

	set_target_properties(${target_name} PROPERTIES
		UNITY_BUILD ON
		UNITY_BUILD_MODE GROUP
	)

	if(cxx_sources)
		ngl_assign_unity_groups(${target_name} cxx ${NGL_UNITY_BUILD_BATCH_SIZE} ${cxx_sources})
	endif()

	if(c_sources)
		ngl_assign_unity_groups(${target_name} c ${NGL_UNITY_BUILD_BATCH_SIZE} ${c_sources})
	endif()

	if(protobuf_cc_sources)
		# Generated protobuf sources declare file-local static data that collides when
		# multiple .pb.cc files share one unity translation unit, so keep each .pb.cc
		# in its own generated unity source.
		ngl_assign_unity_groups(${target_name} protobuf_cc 1 ${protobuf_cc_sources})
	endif()
endfunction()

function(ngl_configure_vs_unity_filters)
	if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
		return()
	endif()

	# CMake places generated unity translation units under:
	#   <build-dir>/.../CMakeFiles/<target>.dir/Unity/...
	# Put them into a dedicated Visual Studio filter so they do not visually mix
	# with hand-written source files in Solution Explorer.
	source_group("Generated Files/Unity" REGULAR_EXPRESSION [[[\\/]CMakeFiles[\\/].*[\\/]Unity[\\/].*]])
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
