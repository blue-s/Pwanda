/*
* http://sosal.kr/
* made by so_Sal
*/


/*
* 아래 프로그램 예제 소스에서 쓰이는 함수들은
* LINK_ 에 모두 정리되어 있습니다.
* Process Name을 사용자로부터 입력받아
* 프로세스를 강제-종료하는 프로그램입니다.
*/

#include<stdio.h>
#include<locale.h>
#include<string.h>
#include<stdlib.h>
#include<tchar.h>
#include<windows.h>
#include<tlhelp32.h>

void ListProcessInfo(void);
void KillProcess(TCHAR* TargetProcess);

int _tmain(int argc, TCHAR* argv[]){
	TCHAR TargetProcess[BUFSIZ];	// 종료를 원하는 프로세스의 이름을 입력받을 버퍼
	/*	Buffered I/O macros		#define BUFSIZ  512	*/

	while(1){
		// 프로세스 목록을 출력
		ListProcessInfo();		

		_tprintf( _T("Input TargetProcess name (End :: EXIT) :: "));

		_tscanf( _T("%s"), TargetProcess);
		_tprintf(_T("input :: %s"), TargetProcess);

		// 종료
		if(_tcscmp(TargetProcess, _T("EXIT")) == 0)
			exit(EXIT_SUCCESS);

		// 프로세스 Kill
		// 매개변수 : ProcessName
		KillProcess(TargetProcess);
	}
	return 0;
}

// 프로세스 목록 출력 함수
void ListProcessInfo(void){
	PROCESSENTRY32 pe32;		// 프로세스 정보 저장 구조체
	pe32.dwSize = sizeof(PROCESSENTRY32);	// 초기화

	// 현재 프로세스의 상태를 스냅샷
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	if(hProcessSnap == INVALID_HANDLE_VALUE){
		_tprintf( _T("CreateToolhelp32Snapshot error \n"));
		exit(EXIT_FAILURE);
	}
	
	// Process32First 함수로부터 System Process의 정보를 받아옴
	if( !Process32First(hProcessSnap, &pe32)){
		_tprintf( _T("Process32First error ! \n"));
		CloseHandle(hProcessSnap);
		return;
	}
	_tprintf(_T("\t[Process name] \t[PID]\t[PPID]\t[ThreadID] \n"));

	// Process32Next로 얻어온 프로세스 정보들을 모두 출력
	do{
		_tprintf(_T("%25s %8d %8d %8d \n"),
			pe32.szExeFile,pe32.th32ProcessID, pe32.th32ParentProcessID,pe32.cntThreads);
	} while(Process32Next(hProcessSnap,&pe32));
}

// 프로세스를 Kill
// 프로세스를 찾기 전까지 ListProcessInfo()와 거의 같음
void KillProcess(TCHAR* TargetProcess){
	PROCESSENTRY32 pe32;

	//매개변수로 Kill 하려는 프로세스의 이름을 가져옴
	HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0);

	if( hProcessSnap == INVALID_HANDLE_VALUE){
		_tprintf( _T("CreateToolhelp32Snapshot error! \n"));
		return;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(hProcessSnap, &pe32)){
		_tprintf(_T("Process32First error ! \n"));
		CloseHandle(hProcessSnap);
		return;
	}
	HANDLE hProcess = NULL;
	BOOL isKill = FALSE;

	do {
		if(_tcscmp(pe32.szExeFile, TargetProcess) == 0)
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

		if(hProcess != NULL){
			//핸들이 정상적이라면 kill
			TerminateProcess(hProcess, -1);
				/*
				*** TerminateProcess
				Terminates the specified process and all of its threads.

				WINBASEAPI
				BOOL
				WINAPI
				TerminateProcess(
					__in HANDLE hProcess,
					__in UINT uExitCode
					);
				*/

			isKill = TRUE;
			CloseHandle(hProcess);
			break;
		}
	} while( Process32Next(hProcessSnap, &pe32) );

	CloseHandle(hProcessSnap);
	
	if(!isKill)
		_tprintf( _T("Kill process fail. Try again! \n"));
}