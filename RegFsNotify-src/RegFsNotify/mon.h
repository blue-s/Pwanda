#pragma once

#include <windows.h>
#include <tchar.h>

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
// NTSTATUS: Ŀ��API �Լ� ȣ�� �� ��ȯ (Ȯ������ ����)
// ���� ���� ���� 

#define FILE_CHANGE_FLAGS FILE_NOTIFY_CHANGE_FILE_NAME |\
					 FILE_NOTIFY_CHANGE_DIR_NAME |\
					 FILE_NOTIFY_CHANGE_ATTRIBUTES |\
					 FILE_NOTIFY_CHANGE_SIZE |\
					 FILE_NOTIFY_CHANGE_CREATION |\
					 FILE_NOTIFY_CHANGE_SECURITY
// * �� value���� ��� '�˸�'�� ���� �÷��׵� 
// 1. ���� �̸� ���� | ������ ���� | ���� 
// 2. ���͸� ���� | ���� 
// 3. ���͸��� ���� �Ӽ� ����
// 4. ���͸��� �ִ� ���� ũ�� ����
// 5. ���͸��� �ִ� ���� ���� �ð� ���� Q: ���� �ð��� ��� ����Ǵ�? ���۹ۿ���. 
// 6. ���͸��� ���� ��ũ���Ͱ� ���� 
// ���� ���� ��ȭ�� FILE_CHANGE_FLAGS�� ��ǥ�ؼ� ����ϰڴ�

#define REG_CHANGE_FLAGS REG_NOTIFY_CHANGE_NAME |\
					 REG_NOTIFY_CHANGE_LAST_SET
// * REGISTRY�� ���������� ���� ó�� ��ȭ �÷��׸� define���༭ '�˸�'�� �����Ѵ� 
// 1. ����Ű�� �߰� | ����
// 2. ��(value) �߰� | ���� | ���� 

void Output(USHORT Color, LPTSTR format, ... );
// RegFsNotify.cpp�� ��ġ 
void StartFileMonitor(void);
void StartRegistryMonitor(void);
void ExtractProcess(TCHAR *);
void ListPrint(void);

extern HANDLE  g_hStopEvent;
extern HANDLE  g_hFile;
extern HANDLE  g_hRegWatch[2];
extern TCHAR * resultBuffer;


// whitelisted filenames or paths
// LPTSTR = LPSTR = char * 
// whitelisted : ���� ��� -> �� ���ڿ� �迭�� ���� �������� ������ �˾ƾ߰��� 
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

// return true if szFile is in the g_szAllow list/
static BOOL IsWhitelisted(LPTSTR szFile)
{
	// g_szAllow �迭 ũ�⸸ŭ �����µ� 
	for(int i=0; i<sizeof(g_szAllow)/sizeof(LPTSTR); i++)
	{
		if (_tcsstr(szFile, g_szAllow[i]) != NULL) 
			// _tcsstr(�˻� ���, �˻���)/
			return TRUE;
	}
	return FALSE;
}
// szFile�� g_szAllow �迭 ����� ������ �ܾ �ִ��� üũ�Ͽ� ������ TRUE�� ��ȯ 
// ������ FALSE�� ��ȯ 
