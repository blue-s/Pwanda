#include "windows.h"
#include "tlhelp32.h"
#include "tchar.h"

#include <stdio.h>

#define DEF_PROC_NAME	(L"ApplicationFrameHost.exe")
//#define DEF_PROC_NAME	(L"KakaoTalk.exe")
#define DEF_DLL_NAME	(L"kernel32.dll")		// 현재 사용하고자 하는 dll명

// Function : find process
// 현재 정의되어있는 파일명(DEF_PROC_NAME)에 해당되는 PID를 return 한다.
DWORD FindProcessID(LPCTSTR szProcessName)
{
	// 각각의 변수 초기화
    DWORD dwPID = 0xFFFFFFFF;
    HANDLE hSnapShot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe;
		/*
		*** PROCESSENTRY32
		Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.

		typedef struct tagPROCESSENTRY32
		{
			DWORD   dwSize;		
			DWORD   cntUsage;				// this module
			DWORD   th32ProcessID;          // owing process
			ULONG_PTR th32DefaultHeapID;	// global usage count on the module
			DWORD   th32ModuleID;           // associated exe, module usage count in th32ProcessID's context
			DWORD   cntThreads;
			DWORD   th32ParentProcessID;    // this process's parent process
			LONG    pcPriClassBase;         // Base priority of process's threads
			DWORD   dwFlags;
			CHAR    szExeFile[MAX_PATH];    // Path
		} PROCESSENTRY32;
		*/

    // Get the snapshot of the system
    pe.dwSize = sizeof( PROCESSENTRY32 );
    hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, NULL );	// 프로세스에 정의된 DLL 정보 구하기
	/*
		*** CreateToolhelp32Snapshot
		https://msdn.microsoft.com/ko-kr/library/aa446560.aspx
		프로세스에서 사용되는 프로세스, 힙, 모듈 및 스레드의 스냅샷을 취합니다.
		시스템 스냅샷에서 발생된 첫 번째 프로세스에 대한 정보를 검색하기 위해 Process32First가 사용되고
		그 다음 Process32Next가 사용됩니다.

		프로세스에 로딩된 모듈(DLL)의 정보를 얻을 수 있다.
		이렇게 구한 hSnapShot 핸들을 Module32First()/Module32Next() 함수에 넘겨주면
		MODULEENTRY32 구조체를 가져온다.

		- TH32CS_SNAPALL : Includes all processes and threads in the system,
				plus the heaps and modules of the process specified in th32ProcessID.

		HANDLE
		WINAPI CreateToolhelp32Snapshot(
			DWORD dwFlags,			// The portions of the system to be included in the snapshot.
			DWORD th32ProcessID
			);
		*/

    // find process
	// 현재 가지고 있는 snapshot을 통해 시스템에서 프로세스들을 차례로 검색
    Process32First(hSnapShot, &pe);	// 첫번째 프로세스 검색
    do
    {
		// 검색한 프로세스와 현재 우리가 원하는 프로세스가 동일한지 비교
        if(!_tcsicmp(szProcessName, (LPCTSTR)pe.szExeFile))
        {
            dwPID = pe.th32ProcessID;
            break;
        }
    }
    while(Process32Next(hSnapShot, &pe));	// 다음 프로세스 검색

    CloseHandle(hSnapShot);

    return dwPID;
}

// change privilege
BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) 
{
    TOKEN_PRIVILEGES tp;
		/*
		*** TOKEN_PRIVILEGES
		The TOKEN_PRIVILEGES structure contains information about a set of privileges for an access token.

		typedef struct _TOKEN_PRIVILEGES {
			DWORD PrivilegeCount;
			LUID_AND_ATTRIBUTES Privileges[ANYSIZE_ARRAY];
		} TOKEN_PRIVILEGES, *PTOKEN_PRIVILEGES;

		*** LUID_AND_ATTRIBUTES
		The LUID_AND_ATTRIBUTES structure represents a locally unique identifier (LUID) and its attributes.

		typedef struct _LUID_AND_ATTRIBUTES {
			LUID  Luid;
			DWORD Attributes;
		} LUID_AND_ATTRIBUTES, *PLUID_AND_ATTRIBUTES;

		*** LUID (로컬 단일 식별자)
		시스템 시동 시 그 운영 체계에 부여되는 유일한 식별자.
		윈도즈 NT에서 보안 객체를 지시하는 식별자로, 파일, 디렉터리, 서비스 등 객체에 고유 번호와 같은 운영 체계 번호를 제공한다.
		클라이언트/서버 시스템에서 클라이언트는 LUID를,
		서버는 GUID(globally unique identifier)를 갖는다.
		*/
    HANDLE hToken;
    LUID luid;
	
	// 현재 프로세스의 토큰을 얻는다.
    if( !OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
    {
		// 현재 프로세스의 토큰을 얻어오지 못하는 경우에 에러를 발생시킨다.
        _tprintf(L"OpenProcessToken error: %u\n", GetLastError());
        return FALSE;
    }
	

    if( !LookupPrivilegeValue(NULL,           // lookup privilege on local system
                              lpszPrivilege,  // privilege to lookup 
                              &luid) )        // receives LUID of privilege
				/*
				*** LookupPrivilegeValue
				The LookupPrivilegeValue function retrieves the locally unique identifier (LUID)
				used on a specified system to locally represent the specified privilege name.
				로컬로 지정된 명시된 privilege name 을 나타내기 위해 시스템에 사용되는 locally unique identifier(LUID)를 검색한다.
				*/
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
    if( !AdjustTokenPrivileges(hToken, FALSE, &tp, 
                               sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL) )
    { 
        _tprintf(L"AdjustTokenPrivileges error: %u\n", GetLastError() ); 
        return FALSE; 
    } 

    if( GetLastError() == ERROR_NOT_ALL_ASSIGNED )
			// #define ERROR_NOT_ALL_ASSIGNED           1300L
			// : 참조되는 모든 특권이 호출자에게 지정되어 있지는 않습니다. 
			/******************
			일반적인 실행의 경우 Amdin 계정이라 해도 행동에 제약이 걸리게 된다. (UAC에 의해)
			따라서 제대로 실행시키기 위해서는 관리자의 권한으로 실행시켜주어야 한다.
			*******************/
    {
		printf("%d\n", ERROR_NOT_ALL_ASSIGNED);
        _tprintf(L"The token does not have the specified privilege. \n");
        return FALSE;
    } 

	printf("SetPrivilege Success!\n");
    return TRUE;
}

// eject dll
// dwPID : 대상 프로세스의 pid
// szDllName : 사용하고자 하는 dll 사용
/************* 이 아래 부분은 현재 우리 프로젝트에 불필요한 부분이기 때문에 생략 *************/
BOOL EjectDll(DWORD dwPID, LPCTSTR szDllName)
{
	// 초기값 정의
    BOOL bMore = FALSE, bFound = FALSE;
    HANDLE hSnapshot, hProcess, hThread;
    HMODULE hModule = NULL;
    MODULEENTRY32 me = { sizeof(me) };
		/*
		*** MODULEENTRY32
		Describes an entry from a list of the modules belonging to the specified process.

		typedef struct tagMODULEENTRY32 {
			DWORD   dwSize;
			DWORD   th32ModuleID;
			DWORD   th32ProcessID;
			DWORD   GlblcntUsage;
			DWORD   ProccntUsage;
			BYTE    *modBaseAddr;
			DWORD   modBaseSize;
			HMODULE hModule;
			TCHAR   szModule[MAX_MODULE_NAME32 + 1];
			TCHAR   szExePath[MAX_PATH];
		} MODULEENTRY32, *PMODULEENTRY32;
		*/
    LPTHREAD_START_ROUTINE pThreadProc;
		/*
		*** LPTHREAD_START_ROUTINE
		스레드에서 실행을 시작했음을 호스트에 알리는 함수를 가리킵니다.
		.NET Framework 4에서는 이 함수 포인터가 더 이상 사용되지 않습니다.

		typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
			LPVOID lpThreadParameter	// 실행을 시작한 코드에 대한 포인터입니다.
		);
		typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;
		*/

    // dwPID = 대상 프로세스 ID
    // TH32CS_SNAPMODULE 파라미터를 이용해서 대상 프로세스에 로딩된 DLL 이름을 얻음
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
		/*
		- TH32CS_SNAPMODULE : 
		Includes all modules of the process specified in th32ProcessID in the snapshot.
		To enumerate the modules, see Module32First.
		If the function fails with ERROR_BAD_LENGTH,
		retry the function until it succeeds.
		*/

	printf("%d\n", dwPID);
	printf("%d\n", hSnapshot);

    bMore = Module32First(hSnapshot, &me);
    for( ; bMore ; bMore = Module32Next(hSnapshot, &me) )
    {		
        if( !_tcsicmp((LPCTSTR)me.szModule, szDllName) || !_tcsicmp((LPCTSTR)me.szExePath, szDllName) )
        {
            bFound = TRUE;
            break;
        }
    }

    if( !bFound )
    {
        CloseHandle(hSnapshot);
        return FALSE;
    }
	
	// 대상 프로세스 핸들 구하기 :
	// 프로세스 ID를 이용해서 대상 프로세스(DEF_PROC_NAME)의 프로세스 핸들을 구한다.
	// 후에 이 핸들을 이용하여 CreateRemoteThread() API를 호출
    if ( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
    {
        _tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
        return FALSE;
    }
	

	// FreeLibrary() API 주소 구하기
	// 대상 프로세스(DEF_PROC_NAME) 프로세스로 하여금 스스로 FreeLibrary() API를 호출하도록 하려면
	// FeeLibrary()의 주소를 알아야 한다.
	// 이 때, FreeLibrary 주소는 모두 동일하다.
	// 따라서 현재 프로세스(DllEjection.exe 프로세스에 로딩된 Kernel32!FreeLibrary 주소를 이용한다.
    hModule = GetModuleHandle(L"kernel32.dll");
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
    
	
	// 대상 프로세스에 스레드를 실행시킨다.
	// pThreadProc 파라미터는 FreeLibrary() API의 주소이며,
	// me.modBaseAddr 파라미터는 이젝션하길 원하는 DLL의 로딩 주소이다.
	// 즉 스레드 함수로 FreeLibrary 함수를 지정하고 스레드 파라미터에 DLL 로딩 주소를 넘겨주면,
	// 결국 대상 프로세스에서는 FreeLibrary() API가 성공적으로 호출된다.
	// (CreateRemoteThread() API의 원래 의도는 외부 프로세스에 스레드 함수를 실행시키는 것인데,
	// 이 경우에 스레드 함수는 FreeLibrary()가 된다.
	hThread = CreateRemoteThread(hProcess, NULL, 0, 
                                 pThreadProc, me.modBaseAddr, 
                                 0, NULL);
    WaitForSingleObject(hThread, INFINITE);	

    CloseHandle(hThread);
    CloseHandle(hProcess);
    CloseHandle(hSnapshot);

    return TRUE;
}

int _tmain(int argc, TCHAR* argv[])
{
    DWORD dwPID = 0xFFFFFFFF;
 
    // find process
	// 현재 정의되어있는 프로세스(DEF_PROC_NAME)에 해당되는 PID 가져온다.
    dwPID = FindProcessID(DEF_PROC_NAME);
    if( dwPID == 0xFFFFFFFF )	// 검색한 프로세스가 열려있지 않은 경우
    {
        _tprintf(L"There is no <%s> process!\n", DEF_PROC_NAME);
        return 1;
    }

    _tprintf(L"PID of \"%s\" is %d\n", DEF_PROC_NAME, dwPID);	// 대상 프로세스 (DEF_PROC_NAME) PID 출력

    // change privilege
	// SE_DEBUG_NAME https://support.microsoft.com/ko-kr/kb/131065
    if( !SetPrivilege(SE_DEBUG_NAME, TRUE) )
        return 1;

    // eject dll
    if( EjectDll(dwPID, DEF_DLL_NAME) )
        _tprintf(L"EjectDll(%d, \"%s\") success!!!\n", dwPID, DEF_DLL_NAME);
    else
        _tprintf(L"EjectDll(%d, \"%s\") failed!!!\n", dwPID, DEF_DLL_NAME);

	getchar();	// stop 시키기 위한 부분
    return 0;
}
