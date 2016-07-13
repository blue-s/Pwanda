#include "mon.h"

HANDLE g_hFile;
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

void Output(USHORT Color, LPTSTR format, ... )
{
	va_list args;
	//char * va_list : 각 인자의 시작주소를 가리키는 포인터
	int len; // 길이 
	DWORD cb; // 
	LPTSTR buffer; // 
	

	va_start(args, format);
	//va_start : va_list로 만들어진 포인터에게 가변 인자 중 첫번째 인자의 주소를 가르쳐줌
	//va_start(va_list로 만든 포인터, 마지막 고정 인수)
	len = _vsctprintf(format, args) + sizeof(TCHAR); 
	// _vsctprintf: 조합될 문자열의 길이 확인, 문자열의 길이가 크면 동적으로 메모리 확보하여 printf
	// 동적 메모리 생성 
	buffer = new TCHAR[len * sizeof(TCHAR)];

	if (!buffer) { 
		return;
	}

	_vstprintf_s(buffer, len, format, args);
	// INVALID_HANDLE_VALUE: 핸들의 초기화, 보통 -1로 정의
	if (g_hFile != INVALID_HANDLE_VALUE) {
	// 핸들이 초기화 안됐을 경우

#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1];
		if (str) { 
			memset(str, 0, len + 1);
			// 0으로 문자열을 다 채움 
			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);
			//WideCharToMultiByte : 유니코드 -> ANSI
			WriteFile(g_hFile, str, strlen(str), &cb, NULL);
			//WriteFile: 특수 파일 혹은 입출력 장치에 데이터를 쓴다
			delete[] str;
		}
#else 
	WriteFile(g_hFile, buffer, strlen(buffer), &cb, NULL);
#endif
	} 

	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	//GetStdHandle - 표준 입출력 및 오류 핸들을 리턴 
	//콘솔에서 stdout, stdin, stderr 의 핸들을 가져오는 함수

	//GetStdHandle 인자
	//STD_INPUT_HANDLE : 표준 입력 핸들
	//STD_OUTPUT_HANDLE : 표준 출력 핸들
	//STD_ERROR_HANDLE : 표준 오류 핸들

	if (Color) 
	{ 
		SetConsoleTextAttribute(
			Handle, Color | FOREGROUND_INTENSITY);
		// FOREGROUND_INTENSITY: 밝은색 
	} 

	_tprintf(buffer);

	SetConsoleTextAttribute(Handle, 
		FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

   delete[] buffer;
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

	g_hFile = CreateFile(_T("RegFsNotify.txt"), 
		GENERIC_WRITE, // 쓰기 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);

	g_hFile_Prefetch = CreateFile(_T("Prefetch.txt"),    //qwer 파일을 만드는 부분
		GENERIC_WRITE, 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);

	g_hFile_Roaming = CreateFile(_T("Roaming.txt"),     // asdf 파일을 만드는 부분
		GENERIC_WRITE, 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);

	HANDLE hThread[2];

	hThread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRegistryMonitor, NULL, 0, NULL);
	
	WaitForMultipleObjects(1, (const HANDLE*)&hThread, TRUE, INFINITE);  //레지스트리 부분의 스레드 사용안함.

	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile_Roaming);
	CloseHandle(g_hFile_Prefetch);
	CloseHandle(g_hFile);

	_tprintf(_T("Program terminating.\n"));
}