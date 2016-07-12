#include "windows.h"
#include "tlhelp32.h"
#include "tchar.h"

#include <stdio.h>

#define DEF_PROC_NAME	(L"ApplicationFrameHost.exe")
//#define DEF_PROC_NAME	(L"KakaoTalk.exe")
#define DEF_DLL_NAME	(L"kernel32.dll")		// ���� ����ϰ��� �ϴ� dll��

// Function : find process
// ���� ���ǵǾ��ִ� ���ϸ�(DEF_PROC_NAME)�� �ش�Ǵ� PID�� return �Ѵ�.
DWORD FindProcessID(LPCTSTR szProcessName)
{
	// ������ ���� �ʱ�ȭ
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
    hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, NULL );	// ���μ����� ���ǵ� DLL ���� ���ϱ�
	/*
		*** CreateToolhelp32Snapshot
		https://msdn.microsoft.com/ko-kr/library/aa446560.aspx
		���μ������� ���Ǵ� ���μ���, ��, ��� �� �������� �������� ���մϴ�.
		�ý��� ���������� �߻��� ù ��° ���μ����� ���� ������ �˻��ϱ� ���� Process32First�� ���ǰ�
		�� ���� Process32Next�� ���˴ϴ�.

		���μ����� �ε��� ���(DLL)�� ������ ���� �� �ִ�.
		�̷��� ���� hSnapShot �ڵ��� Module32First()/Module32Next() �Լ��� �Ѱ��ָ�
		MODULEENTRY32 ����ü�� �����´�.

		- TH32CS_SNAPALL : Includes all processes and threads in the system,
				plus the heaps and modules of the process specified in th32ProcessID.

		HANDLE
		WINAPI CreateToolhelp32Snapshot(
			DWORD dwFlags,			// The portions of the system to be included in the snapshot.
			DWORD th32ProcessID
			);
		*/

    // find process
	// ���� ������ �ִ� snapshot�� ���� �ý��ۿ��� ���μ������� ���ʷ� �˻�
    Process32First(hSnapShot, &pe);	// ù��° ���μ��� �˻�
    do
    {
		// �˻��� ���μ����� ���� �츮�� ���ϴ� ���μ����� �������� ��
        if(!_tcsicmp(szProcessName, (LPCTSTR)pe.szExeFile))
        {
            dwPID = pe.th32ProcessID;
            break;
        }
    }
    while(Process32Next(hSnapShot, &pe));	// ���� ���μ��� �˻�

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

		*** LUID (���� ���� �ĺ���)
		�ý��� �õ� �� �� � ü�迡 �ο��Ǵ� ������ �ĺ���.
		������ NT���� ���� ��ü�� �����ϴ� �ĺ��ڷ�, ����, ���͸�, ���� �� ��ü�� ���� ��ȣ�� ���� � ü�� ��ȣ�� �����Ѵ�.
		Ŭ���̾�Ʈ/���� �ý��ۿ��� Ŭ���̾�Ʈ�� LUID��,
		������ GUID(globally unique identifier)�� ���´�.
		*/
    HANDLE hToken;
    LUID luid;
	
	// ���� ���μ����� ��ū�� ��´�.
    if( !OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
    {
		// ���� ���μ����� ��ū�� ������ ���ϴ� ��쿡 ������ �߻���Ų��.
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
				���÷� ������ ��õ� privilege name �� ��Ÿ���� ���� �ý��ۿ� ���Ǵ� locally unique identifier(LUID)�� �˻��Ѵ�.
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
			// : �����Ǵ� ��� Ư���� ȣ���ڿ��� �����Ǿ� ������ �ʽ��ϴ�. 
			/******************
			�Ϲ����� ������ ��� Amdin �����̶� �ص� �ൿ�� ������ �ɸ��� �ȴ�. (UAC�� ����)
			���� ����� �����Ű�� ���ؼ��� �������� �������� ��������־�� �Ѵ�.
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
// dwPID : ��� ���μ����� pid
// szDllName : ����ϰ��� �ϴ� dll ���
/************* �� �Ʒ� �κ��� ���� �츮 ������Ʈ�� ���ʿ��� �κ��̱� ������ ���� *************/
BOOL EjectDll(DWORD dwPID, LPCTSTR szDllName)
{
	// �ʱⰪ ����
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
		�����忡�� ������ ���������� ȣ��Ʈ�� �˸��� �Լ��� ����ŵ�ϴ�.
		.NET Framework 4������ �� �Լ� �����Ͱ� �� �̻� ������ �ʽ��ϴ�.

		typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
			LPVOID lpThreadParameter	// ������ ������ �ڵ忡 ���� �������Դϴ�.
		);
		typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;
		*/

    // dwPID = ��� ���μ��� ID
    // TH32CS_SNAPMODULE �Ķ���͸� �̿��ؼ� ��� ���μ����� �ε��� DLL �̸��� ����
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
	
	// ��� ���μ��� �ڵ� ���ϱ� :
	// ���μ��� ID�� �̿��ؼ� ��� ���μ���(DEF_PROC_NAME)�� ���μ��� �ڵ��� ���Ѵ�.
	// �Ŀ� �� �ڵ��� �̿��Ͽ� CreateRemoteThread() API�� ȣ��
    if ( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
    {
        _tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
        return FALSE;
    }
	

	// FreeLibrary() API �ּ� ���ϱ�
	// ��� ���μ���(DEF_PROC_NAME) ���μ����� �Ͽ��� ������ FreeLibrary() API�� ȣ���ϵ��� �Ϸ���
	// FeeLibrary()�� �ּҸ� �˾ƾ� �Ѵ�.
	// �� ��, FreeLibrary �ּҴ� ��� �����ϴ�.
	// ���� ���� ���μ���(DllEjection.exe ���μ����� �ε��� Kernel32!FreeLibrary �ּҸ� �̿��Ѵ�.
    hModule = GetModuleHandle(L"kernel32.dll");
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
    
	
	// ��� ���μ����� �����带 �����Ų��.
	// pThreadProc �Ķ���ʹ� FreeLibrary() API�� �ּ��̸�,
	// me.modBaseAddr �Ķ���ʹ� �������ϱ� ���ϴ� DLL�� �ε� �ּ��̴�.
	// �� ������ �Լ��� FreeLibrary �Լ��� �����ϰ� ������ �Ķ���Ϳ� DLL �ε� �ּҸ� �Ѱ��ָ�,
	// �ᱹ ��� ���μ��������� FreeLibrary() API�� ���������� ȣ��ȴ�.
	// (CreateRemoteThread() API�� ���� �ǵ��� �ܺ� ���μ����� ������ �Լ��� �����Ű�� ���ε�,
	// �� ��쿡 ������ �Լ��� FreeLibrary()�� �ȴ�.
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
	// ���� ���ǵǾ��ִ� ���μ���(DEF_PROC_NAME)�� �ش�Ǵ� PID �����´�.
    dwPID = FindProcessID(DEF_PROC_NAME);
    if( dwPID == 0xFFFFFFFF )	// �˻��� ���μ����� �������� ���� ���
    {
        _tprintf(L"There is no <%s> process!\n", DEF_PROC_NAME);
        return 1;
    }

    _tprintf(L"PID of \"%s\" is %d\n", DEF_PROC_NAME, dwPID);	// ��� ���μ��� (DEF_PROC_NAME) PID ���

    // change privilege
	// SE_DEBUG_NAME https://support.microsoft.com/ko-kr/kb/131065
    if( !SetPrivilege(SE_DEBUG_NAME, TRUE) )
        return 1;

    // eject dll
    if( EjectDll(dwPID, DEF_DLL_NAME) )
        _tprintf(L"EjectDll(%d, \"%s\") success!!!\n", dwPID, DEF_DLL_NAME);
    else
        _tprintf(L"EjectDll(%d, \"%s\") failed!!!\n", dwPID, DEF_DLL_NAME);

	getchar();	// stop ��Ű�� ���� �κ�
    return 0;
}
