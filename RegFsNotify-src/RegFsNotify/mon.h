#pragma once

#include <windows.h>
#include <tchar.h>

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
extern HANDLE  g_hFile, onlyFile;	// 파일 핸들 추가
extern HANDLE  g_hRegWatch[2];

static int flag;	// 경로에 해당되는지 구분을 위한 플래그 선언

// 임의의 경로 설정 : 해당 경로의 파일만 따로 뽑아냄
static LPTSTR g_szAllow[] = {
	_T("\\Users\Win7\Desktop\test"),
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