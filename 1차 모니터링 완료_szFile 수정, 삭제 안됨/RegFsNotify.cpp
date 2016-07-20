#include "mon.h"

HANDLE g_hStopEvent;
HANDLE g_hFile;		// 파일 다루는 핸들
HANDLE g_hRegWatch[2];

USHORT GetConsoleTextAttribute(HANDLE hConsole)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	return(csbi.wAttributes);
}

// 콘솔로 출력 함수
void Output_Console(USHORT Color, LPTSTR format, ... )
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

// 텍스트 파일로 출력 함수
void Output_File(LPTSTR format, ...)
{
	va_list args;
	int len;
	DWORD cb;
	LPTSTR buffer;

	va_start(args, format);
	len = _vsctprintf(format, args) + sizeof(TCHAR); 
	buffer = new TCHAR[len * sizeof(TCHAR)];

	if (!buffer){	return; }

	_vstprintf_s(buffer, len, format, args);

	if (g_hFile != INVALID_HANDLE_VALUE) {
#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1];
		if (str) { 
			memset(str, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, str, len, NULL, NULL);
			WriteFile(g_hFile, str, strlen(str), &cb, NULL);
			delete[] str;
		}
#else 
		WriteFile(g_hFile_Prefetch, buffer, strlen(buffer), &cb, NULL);
#endif
	}

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
	checkState();	// [2] 테스트 함수

	// Privilege Setting
	if(!SetPrivilege(SE_DEBUG_NAME, TRUE)){
		Output_Console(FOREGROUND_RED, _T("*** SetPrivilege Fail!!\n\n"));
	}

	g_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	// 파일 생성 : 현재 우리는 파일 출력을 할 필요가 없기 때문에 완전한 결과값 하나만 출력하도록 함
	g_hFile = CreateFile(_T("RegFsNotify.txt"), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	HANDLE g_hTestFile = CreateFile(_T("C:\\test.txt"), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	CloseHandle(g_hTestFile);

	// 쓰레드 생성 및 활동
	HANDLE hThread[2];

	hThread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRegistryMonitor, NULL, 0, NULL);
	
	WaitForMultipleObjects(2, (const HANDLE*)&hThread, TRUE, INFINITE); 

	// 열려 있는 핸들 종료
	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile);

	_tprintf(_T("Program terminating.\n"));
}