
#include "mon.h"

// 핸들은 ID 역할을 하는 개념이고 OS에게 무엇을 부탁했을 때 직접적으로 요구한 자원?이 오는게 아니라
// 핸들을 반환해주는 것이다 
// 핸들은 보통 정수값인데 문자열보다 빠르기 때문이다
// 핸들은 크고 작음을 나타내는 것이 아닌 단순한 구분을 위한 표시이다 
// 같은 종류의 핸들끼리는 절대로 중복된 값을 가지지 않는다 
// 핸들은 운영체제가 발급한다 (사용자는 발급된 것을 사용하기만 한다)
 
 HANDLE g_hFile;
 HANDLE g_hStopEvent; // 스레드 종료 이벤트 핸들 
 HANDLE g_hRegWatch[2]; // g_hRegWatch 로 스레드 생성 

 // 콘솔텍스트를 가져오는 부분 ? /
USHORT GetConsoleTextAttribute(HANDLE hConsole) // hConsole이 어디서 왔는지 찾기 
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
	// hConsole 콘솔 핸들을 스크린 버퍼에 담아서 
    return(csbi.wAttributes);
	// 색상 인덱스를 설정하여 특정 색상으로 출력한다 
}

// 출력 : 감지 결과를 콘솔 스크린에 색깔을 적용하여 출력 
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

//시스템 종료 시 발생하는 시그널
BOOL CtrlHandler(DWORD fdwCtrlType) // --> fwdCtrlType 어디서 가져오는 거임 ?
{ 
	switch(fdwCtrlType) 
	{ 
    case CTRL_C_EVENT: 
		SetEvent(g_hStopEvent); // 이벤트를 전송, Non Signal 상태를 Signal 상태로 변환 
		return TRUE;
	//콘솔에서 [컨트롤 + c] 눌렀을 때 발생하는 시그널

    case CTRL_CLOSE_EVENT: 
		SetEvent(g_hStopEvent);
		return TRUE; 
	//콘솔에서 닫기 | Close | 종료 from 작업관리자 

    case CTRL_BREAK_EVENT: 
		return FALSE; 
	//콘솔에서 Ctrl+Break

    case CTRL_LOGOFF_EVENT: 
		return FALSE; 

	//사용자가 로그오프를 시도할 때 시스템의 모든 콘솔 프로그램에 브로드 캐스팅되는 시그널 
    
	case CTRL_SHUTDOWN_EVENT: 
		return FALSE; 
    //사용자가 시스템을 셧다운하게 되면 모든 콘솔 프로그램에 브로드 캐스팅되는 시그널

	default: 
		return FALSE; 
	} 
} 


void _tmain(int argc, TCHAR *argv[])
{
	g_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// HANDLE 전역변수를 여기서 정의해주는구만 
	// CreateEvent : 이름 없는 이벤트 객체를 생성 
	// 이벤트가 발생하지 않은 상태로 생성되며 
	// 생성된 이벤트 핸들은 자식 프로세스에 상속되지 않고 자동으로 이벤트가 해제되는 객체
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	g_hFile = CreateFile(_T("RegFsNotify.txt"), 
		GENERIC_WRITE, // 쓰기 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);
	// CreateFile: 파일을 생성하는 것 뿐 아니라, 파일/디렉터리를 열 수 있음 

	HANDLE hThread[2];

	hThread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRegistryMonitor, NULL, 0, NULL);
	
	// CreateThread: 스레드 생성
	// 넘겨줄 매개변수는 NULL, 생성된 스레드는 즉시 CPU에 의해 스케쥴링, 스레드의 ID 값은 NULL 

	WaitForMultipleObjects(2, (const HANDLE*)&hThread, TRUE, INFINITE);
	// 기다리기로 한 모든 객체가 신호 상태에 놓일 때까지 기다린다. 대기 시간을 정할 수도 있다
	// g_hRegWatch 핸들을 코드 0으로 종료 
	TerminateThread(g_hRegWatch[0], 0);
	TerminateThread(g_hRegWatch[1], 0);

	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile);
	// g_hStopEvent, g_hFile 핸들을 닫음 

	_tprintf(_T("Program terminating.\n"));
}