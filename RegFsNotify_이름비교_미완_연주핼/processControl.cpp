#include "mon.h"

// 원하는 프로세스의 PID를 가져옴
DWORD FindProcessID(LPCTSTR szProcessName)
{
	DWORD dwPID = 0xFFFFFFFF;
	HANDLE hSnapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe;

	// Get the snapshot of the system
	pe.dwSize = sizeof( PROCESSENTRY32 );
	hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, NULL );

	Process32First(hSnapShot, &pe);
	do
	{
		if(!_tcsicmp(szProcessName, (LPCTSTR)pe.szExeFile))
		{
			dwPID = pe.th32ProcessID;
			break;
		}
	}
	while(Process32Next(hSnapShot, &pe));

	CloseHandle(hSnapShot);
	printProcessInformation(pe);

	return dwPID;
}

// 프로세스의 정보 출력
void printProcessInformation(PROCESSENTRY32 pe)
{
	/*
	typedef struct tagPROCESSENTRY32 {
		DWORD     dwSize;				: The size of structure, in bytes.
		DWORD     cntUsage;				: This member is no longer used and is always set to zero.
		DWORD     th32ProcessID;		: The process identifier.
		ULONG_PTR th32DefaultHeapID;	: This member is no longer used and is always set to zero.
		DWORD     th32ModuleID;			: This member is no longer used and is always set to zero.
		DWORD     cntThreads;			: The number of execution threads started by the process.
		DWORD     th32ParentProcessID;	: The identifier of the process that created this process (its parent process).
		LONG      pcPriClassBase;		: The base priority of any threads created by this process.
		DWORD     dwFlags;				: This member is no longer used, and is always set to zero.
		TCHAR     szExeFile[MAX_PATH];	: The name of the executable file for the process.
	} PROCESSENTRY32, *PPROCESSENTRY32;
	*/

	printf("< Process Information >\n");
	
	printf("dwSize : %d\n", pe.dwSize);
	printf("cntUsage : %d\n", pe.cntUsage);
	printf("th32ProcessID : %d\n", pe.th32ProcessID);
	printf("th32DefaultHeapID : %Id\n", pe.th32DefaultHeapID);
	printf("th32ModuleID : %d\n", pe.th32ModuleID);
	printf("cntThreads : %d\n", pe.cntThreads);
	printf("th32ParentProcessID : %d\n", pe.th32ParentProcessID);
	printf("pcPriClassBase : %d\n", pe.pcPriClassBase);
	printf("dwFlags : %d\n", pe.dwFlags);
	printf("szExeFile : %S\n", pe.szExeFile);
	printf("\n");
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
BOOL KillProcess(TCHAR* TargetProcess){
	HANDLE hProcess = NULL;
	BOOL isKill = FALSE;
	PROCESSENTRY32 pe32;

	//매개변수로 Kill 하려는 프로세스의 이름을 가져옴
	HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0);

	if( hProcessSnap == INVALID_HANDLE_VALUE){
		_tprintf( _T("CreateToolhelp32Snapshot error! \n"));
		return isKill;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(hProcessSnap, &pe32)){
		_tprintf(_T("Process32First error ! \n"));
		CloseHandle(hProcessSnap);
		return isKill;
	}

	do {
		if(_tcscmp(pe32.szExeFile, TargetProcess) == 0)
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

		if(hProcess != NULL){
			// 프로세스 정보 출력
			printf("We are starting Kill.\n");
			printProcessInformation(pe32);

			// 핸들이 정상적이라면 kill
			TerminateProcess(hProcess, -1);
			isKill = TRUE;

			// kill 된 파일명 출력
			Output_File(pe32.szExeFile);
			CloseHandle(hProcess);
			break;
		}
	} while( Process32Next(hProcessSnap, &pe32) );

	CloseHandle(hProcessSnap);
	
	if(!isKill)
		_tprintf( _T("Kill process fail. Try again! \n"));

	return isKill;
}