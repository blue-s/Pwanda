#include "mon.h"

 HANDLE g_hFile_asdf;   //asdf�� ���� ��� ��� ������ ����� �ڵ� ����
 HANDLE g_hFile_qwer;   //qwer�� ���� ��� ��� ������ ����� �ڵ� ����
 HANDLE g_hStopEvent;
 HANDLE g_hRegWatch[2];

USHORT GetConsoleTextAttribute(HANDLE hConsole)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return(csbi.wAttributes);
}

void Output_asdf(USHORT Color, LPTSTR format, ... )  //asdf.txt�� ����ϱ� ���� �Լ� output
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

	if (g_hFile_asdf != INVALID_HANDLE_VALUE) {
#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1];
		if (str) { 
			memset(str, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);
			WriteFile(g_hFile_asdf, str, strlen(str), &cb, NULL);
			delete[] str;
		}
#else 
	WriteFile(g_hFile_asdf, buffer, strlen(buffer), &cb, NULL);
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
 
void Output_qwer(USHORT Color, LPTSTR format, ... )  //qwer.txt�� ����ϱ� ���� �Լ� output
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

	if (g_hFile_qwer != INVALID_HANDLE_VALUE) {
#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1];
		if (str) { 
			memset(str, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);
			WriteFile(g_hFile_qwer, str, strlen(str), &cb, NULL);
			delete[] str;
		}
#else 
	WriteFile(g_hFile_qwer, buffer, strlen(buffer), &cb, NULL);
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

//���α׷� ���� �� ���� �κ��̶� �Ű�Ƚᵵ ��.
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

	g_hFile_qwer = CreateFile(_T("qwer.txt"),    //qwer ������ ����� �κ�
		GENERIC_WRITE, 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);
	
	g_hFile_asdf = CreateFile(_T("asdf.txt"),     // asdf ������ ����� �κ�
		GENERIC_WRITE, 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);

	HANDLE hThread;

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	
	WaitForMultipleObjects(1, (const HANDLE*)&hThread, TRUE, INFINITE);  //������Ʈ�� �κ��� ������ ������.

	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile_asdf);
	CloseHandle(g_hFile_qwer);
	_tprintf(_T("Program terminating.\n"));
}