#include "mon.h"



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

