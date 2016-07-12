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

	if( !LookupPrivilegeValue(NULL,		// lookup privilege on local system
		lpszPrivilege,  // privilege to lookup 
		&luid) )        // receives LUID of privilege
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

	// Enable the privilege or disable all privileges.
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
