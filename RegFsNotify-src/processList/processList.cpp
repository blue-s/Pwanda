/*
* http://sosal.kr/
* made by so_Sal
*/


/*
* �Ʒ� ���α׷� ���� �ҽ����� ���̴� �Լ�����
* LINK_ �� ��� �����Ǿ� �ֽ��ϴ�.
* Process Name�� ����ڷκ��� �Է¹޾�
* ���μ����� ����-�����ϴ� ���α׷��Դϴ�.
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
	TCHAR TargetProcess[BUFSIZ];	// ���Ḧ ���ϴ� ���μ����� �̸��� �Է¹��� ����
	/*	Buffered I/O macros		#define BUFSIZ  512	*/

	while(1){
		// ���μ��� ����� ���
		ListProcessInfo();		

		_tprintf( _T("Input TargetProcess name (End :: EXIT) :: "));

		_tscanf( _T("%s"), TargetProcess);
		_tprintf(_T("input :: %s"), TargetProcess);

		// ����
		if(_tcscmp(TargetProcess, _T("EXIT")) == 0)
			exit(EXIT_SUCCESS);

		// ���μ��� Kill
		// �Ű����� : ProcessName
		KillProcess(TargetProcess);
	}
	return 0;
}

// ���μ��� ��� ��� �Լ�
void ListProcessInfo(void){
	PROCESSENTRY32 pe32;		// ���μ��� ���� ���� ����ü
	pe32.dwSize = sizeof(PROCESSENTRY32);	// �ʱ�ȭ

	// ���� ���μ����� ���¸� ������
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	if(hProcessSnap == INVALID_HANDLE_VALUE){
		_tprintf( _T("CreateToolhelp32Snapshot error \n"));
		exit(EXIT_FAILURE);
	}
	
	// Process32First �Լ��κ��� System Process�� ������ �޾ƿ�
	if( !Process32First(hProcessSnap, &pe32)){
		_tprintf( _T("Process32First error ! \n"));
		CloseHandle(hProcessSnap);
		return;
	}
	_tprintf(_T("\t[Process name] \t[PID]\t[PPID]\t[ThreadID] \n"));

	// Process32Next�� ���� ���μ��� �������� ��� ���
	do{
		_tprintf(_T("%25s %8d %8d %8d \n"),
			pe32.szExeFile,pe32.th32ProcessID, pe32.th32ParentProcessID,pe32.cntThreads);
	} while(Process32Next(hProcessSnap,&pe32));
}

// ���μ����� Kill
// ���μ����� ã�� ������ ListProcessInfo()�� ���� ����
void KillProcess(TCHAR* TargetProcess){
	PROCESSENTRY32 pe32;

	//�Ű������� Kill �Ϸ��� ���μ����� �̸��� ������
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
			//�ڵ��� �������̶�� kill
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