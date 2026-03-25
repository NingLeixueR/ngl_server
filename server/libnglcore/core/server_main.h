// File overview: Declares interfaces for core.

#pragma once

#include "start_node.h"

#include <functional>
#include <string>

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

// Main entry used by the thin executable wrappers in the server targets.
NGL_EXPORT int ngl_main(int argc, char** argv);

// Return a crash-dump callback that forwards the captured content to mail.
NGL_EXPORT std::function<void()> dump_logic(const std::string& acontent);
