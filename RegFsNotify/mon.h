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

void Output_asdf(USHORT Color, LPTSTR format, ... );
void Output_qwer(USHORT Color, LPTSTR format, ... );
void StartFileMonitor(void);
void StartRegistryMonitor(void);

extern HANDLE  g_hStopEvent;
extern HANDLE  g_hFile;
extern HANDLE  g_hRegWatch[2];

// whitelisted filenames or paths
static LPTSTR asdf_szAllow[] = {       //asdf���� ȭ��Ʈ����Ʈ
	_T("Users\kosta\Desktop\asdf"),
	_T("Desktop\asdf"),
	_T("asdf"),
};

static LPTSTR qwer_szAllow[] = {      //qwer���� ȭ��Ʈ����Ʈ
	_T("Users\kosta\Desktop\qwer"),
	_T("Desktop\qwer"),
	_T("qwer"),
};

// return true if szFile is in the g_szAllow list

static BOOL asdfWhitelisted(LPTSTR szFile)              
{
	for(int i=0; i<sizeof(asdf_szAllow)/sizeof(LPTSTR); i++)
	{
		if (_tcsstr(szFile, asdf_szAllow[i]) != NULL) //ȭ��Ʈ ����Ʈ�� ��ġ�ϸ� ���� �־, NULL�� �ƴϴϱ� ��
			return TRUE;							// ��ġ���� ������, NULL�̴ϱ� ����.
	}
	return FALSE;
}

//IsWhitelisted --> qwer������ ��ȭ����
static BOOL qwerWhitelisted(LPTSTR szFile)                
{
	for(int i=0; i<sizeof(qwer_szAllow)/sizeof(LPTSTR); i++)
	{
		if (_tcsstr(szFile, qwer_szAllow[i]) != NULL) 
			return TRUE;
	}
	return FALSE;
}