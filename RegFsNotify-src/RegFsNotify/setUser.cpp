#include "mon.h"


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

	printf("< 프로세스 정보 출력 >\n");
	
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

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) 
{
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;

	if( !OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
	{
		_tprintf(L"OpenProcessToken error: %u\n", GetLastError());
		return FALSE;
	}

	if( !LookupPrivilegeValue(NULL, lpszPrivilege, &luid) )
	{
		_tprintf(L"LookupPrivilegeValue error: %u\n", GetLastError() ); 
		return FALSE; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if( bEnablePrivilege )
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	if( !AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), 
			(PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL) )
	{ 
		_tprintf(L"AdjustTokenPrivileges error: %u\n", GetLastError() ); 
		return FALSE; 
	} 

	if( GetLastError() == ERROR_NOT_ALL_ASSIGNED )
	{
		_tprintf(L"The token does not have the specified privilege. \n");
		return FALSE;
	} 

	return TRUE;
}

// main Function
int setUser()
{
	DWORD dwPID = 0xFFFFFFFF;

	// Privilege Setting
	if( !SetPrivilege(SE_DEBUG_NAME, TRUE) )
        return 1;

	// find process
	dwPID = FindProcessID(DEF_PROC_NAME);
	if( dwPID == 0xFFFFFFFF )
	{
		_tprintf(L"There is no <%s> process!\n", DEF_PROC_NAME);
		return 1;
	}
	_tprintf(L"PID of \"%s\" is %d\n", DEF_PROC_NAME, dwPID);

	return 0;
}
