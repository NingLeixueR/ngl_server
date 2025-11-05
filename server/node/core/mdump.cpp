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

// 1. 生成 Dump 文件的函数（复用之前的实现）
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

// 2. 全局异常处理函数
LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* pExcepInfo) 
{
    char szTime[_MAX_PATH];
    time_t timeNow = time(nullptr);
    strftime(szTime, _MAX_PATH, ".%Y%m%d-%H%M%S", localtime(&timeNow));

    char dumpPath[MAX_PATH];
    GetModuleFileNameA(NULL, dumpPath, MAX_PATH); // 获取 EXE 路径
    PathRemoveFileSpecA(dumpPath); // 去掉文件名，保留目录
    strcat_s(dumpPath, "\\"); // 拼接 Dump 文件名
    strcat_s(dumpPath, Dumper::m_excname.c_str());
    strcat_s(dumpPath, szTime);
    strcat_s(dumpPath, ".dmp"); // 拼接 Dump 文件名

    CreateMiniDump(pExcepInfo, dumpPath);

    Dumper::getDumperHandler()();

    return EXCEPTION_EXECUTE_HANDLER; // 允许系统默认处理（如弹窗）
}


DumperHandler Dumper::dumperHandler;
std::string Dumper::m_excname;

void cxerr()
{
	return;
}

Dumper::Dumper()
{
	dumperHandler = nullptr;
	Dumper::setDumperHandler(cxerr);
	::SetUnhandledExceptionFilter(ExceptionFilter);
}

#endif
