// InjectDll.cpp
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <TlHelp32.h>

#define DEF_PROC_NAME ("notepad.exe")
#define DEF_DLL_PATH ("c:\\myhack.dll")


BOOL InjectDll(DWORD dwPID, LPCTSTR szDllName)
{
	HANDLE hProcess = NULL , hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;
	DWORD dwBufSize = lstrlen(szDllName)+1;
	LPTHREAD_START_ROUTINE pThreadproc;

	// #1 . dwPID를 이용하여 대상 프로세스 (notepad.exe) 의 HANDLE을 구한다 .
	if ( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
	{
		_tprintf (L"OpenProcess (%d) failed!!! [%d )\n", dwPID, GetLastError());
		return FALSE;
	}

	// #2. 대상 프로세스 (notepad . exe) 메모리 에 szDllName 크기만큼메모리를할당한다.
	pRemoteBuf = VirtualAllocEx(hProcess , NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	
	// #3. 할당받은 메모리에 myhack .dll 경로 ("c: \\myhack.dll") 를쓴다.
	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllName, dwBufSize, NULL) ;
	
	// #4. LoadLibraryW() API 주소를구한다 .
	hMod = GetModuleHandle(L"kerne132.dll");
	pThreadproc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");
	
	// #5. notepad.exe 프로세스에 스레드를 실행
	hThread = CreateRemoteThread(hProcess ,
		NULL,
		0,
		pThreadproc,
		pRemoteBuf ,
		0,
		NULL) ;
	WaitForSingleObject(hThread , INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}

int _tmain(int argc , TCHAR *argv[])
{
	if( argc != 3)
	{
		_tprintf(L"USAGE %s pid dll_path\n" , argv[0]);
		return 1;
	}

	// inject dll
	if( InjectDll((DWORD)_tstol(argv[1]) , argv[2]) )
		_tprintf(L"InjectOll(\“ %s\") success!!!\n" , argv[2]);
	else
		_tprintf(L"InjectOll(\“ %s\") failed!!!\n" , argv[2]);
	return 0;
}