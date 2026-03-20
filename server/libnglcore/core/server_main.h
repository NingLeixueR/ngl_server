// File overview: Declares interfaces for core.

#pragma once

// Export symbols from the core library when building on Windows.
#ifdef NGLCORE_STATIC
#	define NGL_EXPORT
#elif defined(_WIN32)
#	ifdef NGLCORE_EXPORTS
#		define NGL_EXPORT __declspec(dllexport)
#	else
#		define NGL_EXPORT __declspec(dllimport)
#	endif
#else
#	define NGL_EXPORT
#endif

enum class startup_error : int
{
	ok = 0,
	invalid_args = 2,
	invalid_node_type = 3,
	config_not_found = 4,
	config_load_failed = 5,
	tab_server_missing = 6,
	net_config_missing = 7,
	init_server_failed = 8,
	node_start_failed = 9,
};

// Main entry used by the thin executable wrappers in the server targets.
NGL_EXPORT int ngl_main(int argc, char** argv);
