// File overview: Implements logic for core.

#if WIN32
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <dbghelp.h>
#include <dbghelp.h>
#include <shlwapi.h>
#include <iostream>
#include <tchar.h>
#include <format>
#include <string>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "shlwapi.lib")

#include "dumper.h"

std::function<void()> Dumper::m_callback;
std::string Dumper::m_excname;

// 1. Dump file function( before )
void CreateMiniDump(EXCEPTION_POINTERS* pExcepInfo, const TCHAR* dumpPath) {
    HANDLE hFile = CreateFile(dumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    MINIDUMP_EXCEPTION_INFORMATION excepInfo;
    excepInfo.ThreadId = GetCurrentThreadId();
    excepInfo.ExceptionPointers = pExcepInfo;
    excepInfo.ClientPointers = FALSE;

    MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        (MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithUnloadedModules),
        &excepInfo,
        NULL,
        NULL
    );

    CloseHandle(hFile);
}

// 2. Handler
LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* pExcepInfo) 
{
    char szTime[_MAX_PATH];
    time_t timeNow = time(nullptr);
    strftime(szTime, _MAX_PATH, ".%Y%m%d-%H%M%S", localtime(&timeNow));

    char dumpPath[MAX_PATH];
    GetModuleFileNameA(NULL, dumpPath, MAX_PATH); // Get EXE path
    PathRemoveFileSpecA(dumpPath); // File, directory
    strcat_s(dumpPath, "\\"); // Dump file
    strcat_s(dumpPath, Dumper::m_excname.c_str());
    strcat_s(dumpPath, szTime);
    strcat_s(dumpPath, ".dmp"); // Dump file

    CreateMiniDump(pExcepInfo, dumpPath);

    Dumper::getDumperHandler()();

    return EXCEPTION_EXECUTE_HANDLER; // Defaulthandle( )
}

void cxerr()
{
	return;
}

Dumper::Dumper()
{
	::SetUnhandledExceptionFilter(ExceptionFilter);
}

#endif