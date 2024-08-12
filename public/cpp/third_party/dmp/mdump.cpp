#if WIN32
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <assert.h>

#include "dbghelp.h"

#include <iostream>
#include <tchar.h>

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);

static LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );

#include "threadtools.h"
#include "dumper.h"
#include "time.h"

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
	::SetUnhandledExceptionFilter( TopLevelFilter );
}

std::shared_mutex m_mutex;
bool isDumping = false;
LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	{/** 锁作用域 */
		monopoly_shared_lock(m_mutex);
		if (isDumping) {
			::MessageBox(nullptr, "Already crushing...", "MiniDump", MB_OK);
			return EXCEPTION_EXECUTE_HANDLER;
		}
		isDumping = true;
	}/** 锁作用域 */
	
//	EXCEPTION_EXECUTE_HANDLER = 1
//	EXCEPTION_CONTINUE_EXECUTION = -1
//	这两个返回值都应该由调用UnhandledExceptionFilter后返回。
//	EXCEPTION_EXECUTE_HANDLER表示进程结束
//	EXCEPTION_CONTINUE_EXECUTION表示处理异常之后继续执行
//	EXCEPTION_CONTINUE_SEARCH = 0 进行系统通常的异常处理（错误消息对话框）

	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HWND hParent = nullptr;						// find a better value for your app

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = nullptr;
	char szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName(nullptr, szDbgHelpPath, _MAX_PATH ))
	{
		char *pSlash = _tcsrchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			_tcscpy( pSlash+1, "DBGHELP.DLL" );
			hDll = ::LoadLibrary( szDbgHelpPath );
		}
	}

	if (hDll== nullptr)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	LPCTSTR szResult = nullptr;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			char szDumpPath[_MAX_PATH] = {0};
			char szScratch [_MAX_PATH];

			// work out a good place for the dump file
			//if (!GetTempPath( _MAX_PATH, szDumpPath ))
			//if (!GetModuleFileName(nullptr, szDumpPath, _MAX_PATH ))
			//	_tcscpy( szDumpPath, "c:\\temp\\" );

			// get the time now
			char szTime[_MAX_PATH];
			time_t timeNow = time(nullptr);
			strftime(szTime, _MAX_PATH, ".%Y%m%d-%H%M%S", localtime(&timeNow));

			_tcscat( szDumpPath, Dumper::m_excname.c_str());
			_tcscat( szDumpPath, szTime );
			_tcscat( szDumpPath, ".dmp" );

			// ask the user if they want to save a dump file
	//		if (::MessageBox( nullptr, "Something bad happened in your program, would you like to save a diagnostic file?", m_szAppName, MB_YESNO )==IDYES)
			{
				// create the file
				HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
											FILE_ATTRIBUTE_NORMAL, nullptr);

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
#ifndef NDEBUG // Debug mode
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &ExInfo, nullptr, nullptr);
#else
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &ExInfo, nullptr, nullptr);//MiniDumpNormal, &ExInfo, nullptr, nullptr );
#endif // NDEBUG
					if (bOK)
					{
						sprintf( szScratch, "Saved dump file to '%s'", szDumpPath );
						szResult = szScratch;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						sprintf( szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
					sprintf( szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
	}

	// 调用上层处理器
	DumperHandler dumperHandler = Dumper::getDumperHandler();
	if (dumperHandler) {
		(*dumperHandler)();
	}

	if (szResult)
		::MessageBox(nullptr, szResult, "MiniDump", MB_OK );

	return retval;
}

#endif
