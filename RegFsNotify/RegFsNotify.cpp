#include "mon.h"

HANDLE g_hFile_Roaming;   //g_hFile_asdf-> g_hFile_Roaming
HANDLE g_hFile_Prefetch;   //g_hFile_qwer-> g_hFile_Prefetch
HANDLE g_hStopEvent;
HANDLE g_hRegWatch[2];

USHORT GetConsoleTextAttribute(HANDLE hConsole)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	return(csbi.wAttributes);
}



void Output_Roaming(USHORT Color, LPTSTR format, ... )  //asdf.txt를 출력하기 위한 함수 output
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

	if (g_hFile_Roaming != INVALID_HANDLE_VALUE) {
#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1];
		if (str) { 
			memset(str, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);
			WriteFile(g_hFile_Roaming, str, strlen(str), &cb, NULL);
			delete[] str;
		}
#else 
		WriteFile(g_hFile_Roaming, buffer, strlen(buffer), &cb, NULL);
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

void Output_Prefetch(USHORT Color, LPTSTR format, ... )  //qwer.txt를 출력하기 위한 함수 output
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

	if (g_hFile_Prefetch != INVALID_HANDLE_VALUE) {
#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1];
		if (str) { 
			memset(str, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);
			WriteFile(g_hFile_Prefetch, str, strlen(str), &cb, NULL);
			delete[] str;
		}
#else 
		WriteFile(g_hFile_Prefetch, buffer, strlen(buffer), &cb, NULL);
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

//프로그램 종료 시 사용될 부분이라 신경안써도 됨.
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

	g_hFile_Prefetch = CreateFile(_T("Prefetch.txt"),    //qwer 파일을 만드는 부분
		GENERIC_WRITE, 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);

	g_hFile_Roaming = CreateFile(_T("Roaming.txt"),     // asdf 파일을 만드는 부분
		GENERIC_WRITE, 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);

	HANDLE hThread;

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);

	WaitForMultipleObjects(1, (const HANDLE*)&hThread, TRUE, INFINITE);  //레지스트리 부분의 스레드 사용안함.

	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile_Roaming);
	CloseHandle(g_hFile_Prefetch);
	_tprintf(_T("Program terminating.\n"));
}