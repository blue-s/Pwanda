#include "mon.h"

 HANDLE g_hFile;
 HANDLE g_hStopEvent;
 HANDLE g_hRegWatch[2];

USHORT GetConsoleTextAttribute(HANDLE hConsole)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return(csbi.wAttributes);
}

void Output(USHORT Color, LPTSTR format, ... )
{
	va_list args;
	int len;
	DWORD cb;
	LPTSTR buffer;

	va_start(args, format);
	
	len = _vsctprintf(format, args) + sizeof(TCHAR);
	buffer = new TCHAR[len * sizeof(TCHAR)];

	if (!buffer) { 
		return;
	}

	_vstprintf_s(buffer, len, format, args);

	if (g_hFile != INVALID_HANDLE_VALUE) {
#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1];
		if (str) { 
			memset(str, 0, len + 1);

			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);

			WriteFile(g_hFile, str, strlen(str), &cb, NULL);

			delete[] str;
		}
#else 
	WriteFile(g_hFile, buffer, strlen(buffer), &cb, NULL);
#endif
	} 

	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (Color) 
	{ 
		SetConsoleTextAttribute(
			Handle, Color | FOREGROUND_INTENSITY);
	} 

	_tprintf(buffer);

	SetConsoleTextAttribute(Handle, 
		FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

   delete[] buffer;
}
 
BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch(fdwCtrlType) 
	{ 
    case CTRL_C_EVENT:
		SetEvent(g_hStopEvent);
		return TRUE;
    case CTRL_CLOSE_EVENT:
		SetEvent(g_hStopEvent);
		return TRUE; 

    case CTRL_BREAK_EVENT:
		return FALSE; 
    case CTRL_LOGOFF_EVENT:
		return FALSE; 
    case CTRL_SHUTDOWN_EVENT:
		return FALSE; 
    default: 
		return FALSE; 
	} 
} 

void _tmain(int argc, TCHAR *argv[])
{
	g_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	g_hFile = CreateFile(_T("RegFsNotify.txt"), 
		GENERIC_WRITE,	
		FILE_SHARE_READ, 0,
		CREATE_ALWAYS, 0, NULL);

	// 파일만 출력하도록 변경
	//HANDLE hThread[2];

	//hThread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	//hThread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRegistryMonitor, NULL, 0, NULL);
	//
	//WaitForMultipleObjects(2, (const HANDLE*)&hThread, TRUE, INFINITE);		// WinBash.h
	//
	//TerminateThread(g_hRegWatch[0], 0);
	//TerminateThread(g_hRegWatch[1], 0);
	//
	//CloseHandle(g_hStopEvent);
	//CloseHandle(g_hFile);

	HANDLE hThread;
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	WaitForMultipleObjects(1, (const HANDLE*)&hThread, TRUE, INFINITE);
	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile);

	_tprintf(_T("Program terminating.\n"));
}