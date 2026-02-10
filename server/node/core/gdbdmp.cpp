#ifndef WIN32
#include "dumper.h"

/**
* 平台相关
*/
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define SYSTEM_WIN // win 平台
#else
#define SYSTEM_LINUX // linux 平台
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

// 单字节
typedef char char_x;
typedef char byte_x;
typedef unsigned char ubyte_x;

// 单字节 - 布尔型
typedef bool bool_x;

// 两字节
typedef short short_x;
typedef unsigned short ushort_x;

// 四字节
typedef int int_x;
typedef unsigned int uint_x;

// 八字节
typedef long long long_x;
typedef unsigned long long ulong_x;

// 浮点
typedef float float_x;
typedef double double_x;

// 为"BL"统一风格
typedef size_t size_x;
typedef long_x time_x;

// socket类型
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

	// 调用上层处理器
	auto dumperHandler = Dumper::getDumperHandler();
	if (dumperHandler != nullptr) {
		dumperHandler();
	}

	raise(sig);
}
#endif