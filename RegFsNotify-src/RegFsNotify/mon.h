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