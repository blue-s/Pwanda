#pragma once

// 헤더파일 선언
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>


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
//extern TCHAR * roamingList[100];
//extern TCHAR * prefetchBuffer;

void ListPrint(void);
void compare(void);
void ExtractProcess(DWORD, TCHAR *);






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
