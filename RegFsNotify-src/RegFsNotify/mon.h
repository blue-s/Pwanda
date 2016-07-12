/*
 * 변경 사항 :
 * [1] 파일만 정보를 확인할 수 있도록 수정
 * [2] 현재 사용자가 Admin인지, 현재 프로세스가 어떠한 권한으로 실행되었는지 확인 : checkState.cpp
 * [3] 권한 상승 : setUser.cpp
 */

#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdio.h>	// [1] 테스트를 위한 추가

// [2] 테스트를 위한 추가
#include <locale.h>	
#include <shlobj.h>					// for IsUserAnAdmin() function
#pragma comment(lib, "shell32.lib")	// for IsUserAnAdmin() function
void checkState();			// 점검 함수 추가

// [3] 테스트를 위한 추가
#include "tlhelp32.h"
#define DEF_PROC_NAME	(L"explorer.exe")
#define DEF_DLL_NAME	(L"myhack.dll")
int setUser();				// 사용자 권한 함수 추가


#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

#define FILE_CHANGE_FLAGS FILE_NOTIFY_CHANGE_FILE_NAME |\
					 FILE_NOTIFY_CHANGE_DIR_NAME |\
					 FILE_NOTIFY_CHANGE_ATTRIBUTES |\
					 FILE_NOTIFY_CHANGE_SIZE |\
					 FILE_NOTIFY_CHANGE_CREATION |\
					 FILE_NOTIFY_CHANGE_SECURITY

#define REG_CHANGE_FLAGS REG_NOTIFY_CHANGE_NAME |\
					 REG_NOTIFY_CHANGE_LAST_SET

void Output(USHORT Color, LPTSTR format, ... );
void StartFileMonitor(void);
void StartRegistryMonitor(void);

extern HANDLE  g_hStopEvent;
extern HANDLE  g_hFile;
extern HANDLE  g_hRegWatch[2];

static LPTSTR g_szAllow[] = {
	_T("WINDOWS\\system32\\config\\"),
	_T("\\ntuser.dat.LOG"),
	_T("UsrClass.dat.LOG"),
	_T("RegFsNotify.txt"),
	_T("_restore"),
	_T("CatRoot2"),
	_T("\\Microsoft\\Cryptography\\RNG"),
	_T("\\Microsoft\\WBEM"),
};

static BOOL IsWhitelisted(LPTSTR szFile)
{
	for(int i=0; i<sizeof(g_szAllow)/sizeof(LPTSTR); i++)
	{
		if (_tcsstr(szFile, g_szAllow[i]) != NULL) 
			return TRUE;
	}
	return FALSE;
}