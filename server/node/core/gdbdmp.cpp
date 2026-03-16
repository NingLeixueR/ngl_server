// File overview: Implements logic for core.

#ifndef WIN32
#include "dumper.h"

/**
* Related
*/
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define SYSTEM_WIN // Win
#else
#define SYSTEM_LINUX // Linux
#endif

#ifdef SYSTEM_WIN
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4819)
#endif // SYSTEM_WIN

#ifndef NDEBUG
#ifndef _DEBUG
#define _DEBUG
#endif
#endif


#include <stdlib.h>



#ifdef SYSTEM_WIN
#include <winsock2.h>
#endif

// Translated comment.
typedef char char_x;
typedef char byte_x;
typedef unsigned char ubyte_x;

// Translated comment.
typedef bool bool_x;

// Translated comment.
typedef short short_x;
typedef unsigned short ushort_x;

// Translated comment.
typedef int int_x;
typedef unsigned int uint_x;

// Translated comment.
typedef long long long_x;
typedef unsigned long long ulong_x;

// Translated comment.
typedef float float_x;
typedef double double_x;

// "BL"
typedef size_t size_x;
typedef long_x time_x;

// Sockettype
#ifdef SYSTEM_WIN
typedef SOCKET socket_x;
#else
typedef int_x socket_x;
#endif

#ifndef WIN32
typedef short WORD;
typedef long DWORD;
typedef long long GUID;
typedef char BYTE;
typedef unsigned int UINT;
typedef int INT;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef bool BOOL;
typedef void VOID;
#define CONST const
#define TRUE true
#define FALSE false
#endif

#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sighandler(int sig);

static bool_x isDumping = false;
std::function<void()> Dumper::m_callback;
std::string Dumper::m_excname;


Dumper::Dumper() {
	struct rlimit rlim;
	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &rlim);

	signal(SIGSEGV, &sighandler);
	signal(SIGABRT, &sighandler);
	signal(SIGFPE, &sighandler);
	signal(SIGPIPE, &sighandler);
}

void sighandler(int sig)
{
	if (isDumping)
	{
		// Crushing again, exiting...
		exit(0);
		return;
	}

	Dumper::m_excname += ".%e.%p.core";
	setenv("core_pattern", Dumper::m_excname.c_str(), 1);

	isDumping = true;

	signal(SIGSEGV, SIG_DFL);
	signal(SIGABRT, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGPIPE, SIG_DFL);

	// On handle
	auto dumperHandler = Dumper::getDumperHandler();
	if (dumperHandler != nullptr) {
		dumperHandler();
	}

	raise(sig);
}
#endif