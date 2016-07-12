#include "windows.h"
#include "tlhelp32.h"
#include "tchar.h"

#include <stdio.h>

#define DEF_PROC_NAME	(L"ApplicationFrameHost.exe")
//#define DEF_PROC_NAME	(L"KakaoTalk.exe")
#define DEF_DLL_NAME	(L"kernel32.dll")


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

// change privilege
// lpszPrivilege : SE_DEBUG_NAME
// bEnablePrivilege : TRUE
// 권한 변경시키기
BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) 
{
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;

	// 현재 프로세스의 토큰을 얻는다.
	if( !OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
	{
		_tprintf(L"OpenProcessToken error: %u\n", GetLastError());
		return FALSE;
	}

	// LUID를 얻어온다.
	// NULL : 로컬 컴퓨터를 지칭
	// lpszPrivilege = SE_DEBUG_NAME
	//		- Required to debug and adjust the memory of a process owned by another account.
	//		#define SE_DEBUG_NAME                     TEXT("SeDebugPrivilege")
	if( !LookupPrivilegeValue(NULL, lpszPrivilege, &luid) ) 
	{
		_tprintf(L"LookupPrivilegeValue error: %u\n", GetLastError() ); 
		return FALSE; 
	}

	// TOKEN_PRIVILEGES의 멤버 정의
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if( bEnablePrivilege ){
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		/*
		*** SE_PRIVILEGE_ENABLED
		The function enables the privilege.

		#define SE_PRIVILEGE_ENABLED            (0x00000002L)
		*/
	}

	// Enable the privilege or disable all privileges.
	// 권한 가능 여부
	if( !AdjustTokenPrivileges(hToken, FALSE, &tp, 
		sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL) )
		/*
		*** AdjustTokenPrivileges
		Enabling or disabling privileges
		in an access token requires TOKEN_ADJUST_PRIVILEGES access.

		WINAPI AdjustTokenPrivileges (
		__in      HANDLE TokenHandle,
				// A handle to the access token that contains the privileges to be modified.
				// The handle must have TOKEN_ADJUST_PRIVILEGES access to the token.
		__in      BOOL DisableAllPrivileges,
				// Specifies whether the function disables all of the token's privileges.
				// FALSE : the function modifies privileges based
				//		on the information pointed to by the NewState parameter.
		__in_opt  PTOKEN_PRIVILEGES NewState,
				// A pointer to a TOKEN_PRIVILEGES structure
				// that specifies an array of privileges and their attributes.
		__in      DWORD BufferLength,
				// Specifies the size, in bytes,
				// of the buffer pointed to by the PreviousState parameter.
		__out_bcount_part_opt(BufferLength, *ReturnLength) PTOKEN_PRIVILEGES PreviousState,
				// A pointer to a buffer that the function fills
				// with a TOKEN_PRIVILEGES structure
				// that contains the previous state of any privileges
				// that the function modifies.
		__out_opt PDWORD ReturnLength
				// A pointer to a variable that receives the required size, in bytes,
				// of the buffer pointed to by the PreviousState parameter.
				// NULL : PreviousState is NULL.
		);
		*/
	{ 
		_tprintf(L"AdjustTokenPrivileges error: %u\n", GetLastError() ); 
		return FALSE; 
	} 

	// 권한 사용 가능 여부 설정
	if( GetLastError() == ERROR_NOT_ALL_ASSIGNED )
		// Not all privileges or groups referenced are assigned to the caller.
		// 1300 0x0514 참조되는 모든 특권이 호출자에게 지정되어 있지는 않습니다. 
	{
		_tprintf(L"The token does not have the specified privilege. \n");
		return FALSE;
	} 

	return TRUE;
}


BOOL setUser(DWORD dwPID, LPCTSTR szDllName)
{
	BOOL bMore = FALSE, bFound = FALSE;
	HANDLE hSnapshot;
	HMODULE hModule = NULL;
	MODULEENTRY32 me = { sizeof(me) };
	LPTHREAD_START_ROUTINE pThreadProc;

	HANDLE hToken, hProcess;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	// PID와 관련된 스냅샷 받아옴
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

	bMore = Module32First(hSnapshot, &me);
	for( ; bMore ; bMore = Module32Next(hSnapshot, &me) )
	{		
		if( !_tcsicmp((LPCTSTR)me.szModule, szDllName) || !_tcsicmp((LPCTSTR)me.szExePath, szDllName) )
		{
			bFound = TRUE;
			break;
		}
	}

	CloseHandle(hSnapshot);

	// 대상의 프로세스 핸들을 가져옴
	if ( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
    {
        _tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
        return FALSE;
    }
	
	// 현재 프로세스의 토큰을 얻는다.
	if( !OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
	{
		_tprintf(L"OpenProcessToken error: %u\n", GetLastError());
		return FALSE;
	}

	// LUID를 얻어온다.
	// SE_DEBUG_NAME : NT Defined Privileges
	//		https://support.microsoft.com/en-us/kb/132958
	if( !LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid) ) 
	{
		_tprintf(L"LookupPrivilegeValue error: %u\n", GetLastError() ); 
		getchar();	// 고정용
		return FALSE; 
	}

	// TOKEN_PRIVILEGES의 멤버 정의
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// 권한 가능 여부
	if( !AdjustTokenPrivileges(hToken, FALSE, &tp, 
		sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL) )
	{ 
		_tprintf(L"AdjustTokenPrivileges error: %u\n", GetLastError() ); 
		getchar();	// 고정용
		return FALSE; 
	} 

	// 권한 사용 가능 여부 설정
	if( GetLastError() == ERROR_NOT_ALL_ASSIGNED )
	{
		_tprintf(L"The token does not have the specified privilege. \n");
		getchar();	// 고정용
		return FALSE;
	} 
	return TRUE;
}


// main Function
int _tmain(int argc, TCHAR *argv[])
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

	if( !SetPrivilege(SE_DEBUG_NAME, TRUE) )
		return 1;

	// setUser
	if( !setUser(dwPID, DEF_DLL_NAME)){
		printf("EjectDll Fail!!\n");
		return 1;
	}


	getchar();	// stop 시키기 위한 부분
	return 0;
}
