/*
# Copyright (C) 2010 Michael Ligh
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mon.h" // 사용자가 정의한 헤더 파일 선언


//핸들 정의
 HANDLE g_hFile;
 HANDLE g_hStopEvent;
 HANDLE g_hRegWatch[2];

//USHORT 부호 없는 정수를 저장하는 데이터 타입
//매개 변수로 받아온 hConsole을 콘솔 스크린 버퍼에 담고
//csbi.wAttrigutes로 생상 인덱스를 설정 -> 특정 색상으로 텍스트 출력
USHORT GetConsoleTextAttribute(HANDLE hConsole)
{
	//콘솔 스크린 버퍼 선언
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	//콘솔 스크린 정보 담기, 콘솔 위치 반환
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return(csbi.wAttributes);
}

// 출력 함수: 감지 결과를 콘솔 스크린 색깔을 적용하여 출력
void Output(USHORT Color, LPTSTR format, ... )
{
	va_list args;
	int len;
	DWORD cb;
	LPTSTR buffer;

	//char*va_list : 각 인자의 시작주소를 가리키는 포인터
	//va_start: va_list로 만들어진 포인터에게 가변 인자 중 첫번째 인자의주소를 가르쳐줌
	//va_start(va_list로 만든 포인터, 마지막 고정인수)
	va_start(args, format);

	//vsctprintf:조합될 문자열의 길이 확인, 문자열의 길이가 코면 동적으로 메모리 확봐여 printf
	len = _vsctprintf(format, args) + sizeof(TCHAR); 

	//동적 메모리 생성-버퍼
	buffer = new TCHAR[len * sizeof(TCHAR)];

	if (!buffer) { 
		return;
	}

	_vstprintf_s(buffer, len, format, args);

	//#define INVALID_GANDLE_VALUE((HANDLE)(LONG_PTR)-1)
	//INVALID_HANDLE_VALUE:핸들의 초기화, 보통 -1로 정의

	if (g_hFile != INVALID_HANDLE_VALUE) { //핸들이 초기화 안됐을 경우
#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1]; //동적할당
		if (str) { 

			//memset : 메모리를 지장한 문자로 채움, 버퍼를 똑같은 문자로 채움
			//str를 0으로 len+1개를 채움
			memset(str, 0, len + 1);
			// WideCharToMultiByte 유니코드 -> ANSI
			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);
			//WriteFile: 특수 파일 혹은 입출력 장치에 데이터를 쓴다.
			WriteFile(g_hFile, str, strlen(str), &cb, NULL);
			delete[] str;
		}
#else 
	WriteFile(g_hFile, buffer, strlen(buffer), &cb, NULL);
#endif
	} 

	//GetStdHandle 함수란?

		//표준 입출력 및 오류 핸들을 리턴
		//콘솔에서 stdout, stdin, stderr의 핸들으 가져오는 함수
	//GetStdHandle인자
		//STD_INPUT_HANDLE: 표준 입력 핸들
		//STD_OUPUT_HANDLE : 표준 출력 핸들
		//STD_ERROR_HANDLE : 표준 오류 핸들

	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (Color) 
	{ 
		//SetConsoleTextAttribute : 콘솔창 글자에 색 넣기
		//SetConsoleTextAttribute (출력한 콘솔의 핸들, 색상)
		SetConsoleTextAttribute(
			Handle, Color | FOREGROUND_INTENSITY);
	} 

	_tprintf(buffer);

	SetConsoleTextAttribute(Handle, 
		FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

   delete[] buffer;
}

//시스템 종료 시 발생하는 시그널
BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch(fdwCtrlType) 
	{ 
		//콘솔에서[컨트롤+C]를 눌렀을 때 발생하는 시그널
    case CTRL_C_EVENT:
		//SetEvent: 이벤트를 전송, Non Signal 상태를 Signal상태로 변환
		SetEvent(g_hStopEvent);
		return TRUE;

	//[닫기]버튼을 누르거나 콘솔 시스템 메뉴에서[close]메뉴를 선택할때,
	//또는 작업관리자로부터[종료]버튼을 눌렀을 때 발생하는 시그널
    case CTRL_CLOSE_EVENT: 
		SetEvent(g_hStopEvent);
		return TRUE;

			//콘솔에서[컨트롤+브레이크]눌렀을때 발생하는 시그널
    case CTRL_BREAK_EVENT: 
		return FALSE; 

		//사용자가 로그오프를 시도할 때 시스템의 모든 콘솔 프로그램에 브로드 캐스팅되는 시그널
    case CTRL_LOGOFF_EVENT: 
		return FALSE; 

		//사용자가 시스템을 셧다운하게 되면 시스템의 모든 콘솔 프로그램에 브로드캐스팅 되는 시그널
    case CTRL_SHUTDOWN_EVENT: 
		return FALSE; 
    default: 
		return FALSE; 
	} 
} 

void _tmain(int argc, TCHAR *argv[])
{

	//CrreateEvent: 이름이 정해진 또는 이름이 없는 이벤트 객체를 열거나 생성하는 함수
	// 생성된 이벤트 핸들은 자식 프로세스에 상속되지 않고 자동으로 이벤트가 해제되는 이벤트 객체며,
	//이벤트가 발생하지 않은 상태로 생성되며, 이름없는 이벤트 객체를 생성.
	g_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	//SetConsoleCtrlHandler: 위에서 정의한 CtrlHandler 대한 처리
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	
	//CreateFile :파일을 생성하는 것 뿐만 아니라, 파일/디렉터리를 열 수도 있다.
	g_hFile = CreateFile(_T("RegFsNotify.txt"), 
		GENERIC_WRITE, 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);

	//스레드 생성 반환 변수 선언
	HANDLE hThread[2];

	//CreateThread : 스레드 생성, 호출한 프로세스의 가상주소 공간에 쓰레드를 생성해서 실행
	//기본 보안 특성을 사용하고, 기본크기(0), 실행되는 함수명은[StartFileMonitor/ StartRegistryMonitor]이고
	//넘겨줄 매개변수는 NULL, 생성된 스레드는 즉시 CPU에 의해 스케줄링, 스레드의 ID 값은 NULL
	hThread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	//hThread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRegistryMonitor, NULL, 0, NULL);


	//WaitForMultipleObjects : 기다리기로 한 모든 객체가 신호 상태에 놓일 때까지 기다린다. 대기 시간을 정할 수도 있다.
	//기다리고자 하는 객체 핸들의 배열 크기는 2, 이름은 hThread
	//모든 배열에 있는 모든 객체 핸들이 시그널 상태가 되어야 반환
	//모든 핸들이 시그널 상태가 될 때까지 기다림
	WaitForMultipleObjects(1, (const HANDLE*)&hThread, TRUE, INFINITE);
	
	//TerminateThread : 스레드를 종료(정리)
	//g_hRegWatch 핸들을 코드 0 종료
	TerminateThread(g_hRegWatch[0], 0);
	//TerminateThread(g_hRegWatch[1], 0);

	//g_hStopEvent, g_hFile 핸들을 닫는다.
	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile);

	_tprintf(_T("Program terminating.\n"));
}