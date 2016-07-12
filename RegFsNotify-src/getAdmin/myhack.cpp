// myhack.cpp
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

#pragma comment(lib , "urlmon.lib")

#define DEF_NAVER_ADDR (L"http://www.naver.com/index.html")
#define DEF_INDEX_PATH (L"c:\\index.html")

HMODULE g_hMod = NULL;

DWORD WINAPI ThreadProc(LPVOID lParam)
{
	URLDownloadToFile (NULL, DEF_NAVER_ADDR, DEF_INDEX_PATH, 0, NULL);
	TCHAR szPath[_MAX_PATH] = {0,};

	if( !GetModuleFileName( g_hMod, szPath, MAX_PATH))
		return FALSE;

	TCHAR *p = _tcsrchr(szPath, '\\');
	if(!p)
		return FALSE;

	_tcscpy_s(p+1, _MAX_PATH, DEF_INDEX_PATH);

	URLDownloadToFile (NULL, DEF_NAVER_ADDR, szPath, 0, NULL);

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	HANDLE hThread = NULL;
	g_hMod = (HMODULE)hinstDll;

	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugString(L"myhack.dll Injection!!!");
		hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
		CloseHandle(hThread);
		break;
	}
	return TRUE;
}