#pragma once

// 헤더파일 선언
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <tlhelp32.h>

// [2] 테스트를 위한 추가
#include <locale.h>	
#include <shlobj.h>					// for IsUserAnAdmin() function
#pragma comment(lib, "shell32.lib")	// for IsUserAnAdmin() function
void checkState();			// 점검 함수 추가

// [3] 테스트를 위한 추가
#include "tlhelp32.h"
#define DEF_PROC_NAME	(L"RegFsNotify.exe")	//(L"svchost.exe")
#define DEF_DLL_NAME	(L"myhack.dll")
int setUser();				// 사용자 권한 함수 추가

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#define MAX_VALUE_NAME 16383
#define FILE_CHANGE_FLAGS FILE_NOTIFY_CHANGE_FILE_NAME |\
	FILE_NOTIFY_CHANGE_DIR_NAME |\
	FILE_NOTIFY_CHANGE_ATTRIBUTES |\
	FILE_NOTIFY_CHANGE_SIZE |\
	FILE_NOTIFY_CHANGE_CREATION |\
	FILE_NOTIFY_CHANGE_SECURITY

#define REG_CHANGE_FLAGS REG_NOTIFY_CHANGE_NAME |\
	REG_NOTIFY_CHANGE_LAST_SET


// RegFsNotify.cpp
extern HANDLE  g_hStopEvent;	// 이벤트 다루는 핸들
extern HANDLE  g_hFile;			// 파일 다루는 핸들
extern HANDLE  g_hRegWatch[2];

void Output_Console(USHORT Color, LPTSTR format, ... );
void Output_File(LPTSTR format, ...);
void StartFileMonitor(void);
void StartRegistryMonitor(void);

// reg.cpp
extern TCHAR * resultBuffer;

// extractProcessName.cpp
static int roamingNum = 0;
static int prefetchNum = 0;
static int runNum = 0;
static TCHAR* roamingList[100];
static TCHAR* prefetchList[100];
static TCHAR* runList[100];

void ListPrint(void);
BOOL compare(void);
void ExtractProcess(DWORD, TCHAR *);


// setUser.cpp
BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);	// setPrivilege


// processControl.cpp
DWORD FindProcessID(LPCTSTR szProcessName);			// 원하는 프로세스의 PID 가져옴
void printProcessInformation(PROCESSENTRY32 pe);	// 프로세스 정보 출력
void ListProcessInfo(void);							// 프로세스 목록 출력
BOOL KillProcess(TCHAR* TargetProcess);				// 프로세스 죽이기





// ******************* 화이트리스트 *************************

// 화이트 리스트 목록
static LPTSTR Roaming_szAllow[] = { 
	_T("Roaming\\"),
	_T("AppData\Roaming\\"),

};
static LPTSTR Prefetch_szAllow[] = {  
	_T("Windows\Prefetch\\"),
	_T("Prefetch\\"),
};

// 화이트리스트에 속하는지 여부 확인 함수
// return 1 : Roaming
// return 2 : Prefetch
// return -1 : Nothing
static int Whitelisted(LPTSTR szFile)              
{
	for(int i=0; i<sizeof(Roaming_szAllow)/sizeof(LPTSTR); i++)
	{
		if (_tcsstr(szFile, Roaming_szAllow[i]) != NULL)
			return 1;
	}

	for(int i=0; i<sizeof(Prefetch_szAllow)/sizeof(LPTSTR); i++)
	{
		if (_tcsstr(szFile, Prefetch_szAllow[i]) != NULL) 
			return 2;
	}
	return -1;
}
