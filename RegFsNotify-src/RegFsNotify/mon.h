#pragma once //�ش� �ߺ� ����- �����Ϸ����� ��� ������ �ѹ��� ����ǵ��� �˸�.

#include <windows.h>  //win API �Լ����� ����
#include <tchar.h>  //MBCS�� WBCS�� ���� ���� - �ƽ�Ű�ڵ�� �����ڵ带 ���ÿ� �����Ѵٴ� ���� ��ġ.
#include <stdio.h>

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) // �������� ����


//���� ��ȭ �÷��� ����

// �����̸� ����, ���ϻ���, ���� �˸�
//�ٷ��͸� ����, ���� �˸�
//������ ���͸��� ���� �Ӽ� ����˸�
//������ ���͸��� �ִ� ���� ũ�� ���� �˸�
//������ ���͸��� �ִ� ������ �����ð��� ����� ��� �˸�
//������ ���͸��� ���� ��ũ���Ͱ� ����� ��� �˸�.

#define FILE_CHANGE_FLAGS FILE_NOTIFY_CHANGE_FILE_NAME |\
					 FILE_NOTIFY_CHANGE_DIR_NAME |\
					 FILE_NOTIFY_CHANGE_ATTRIBUTES |\
					 FILE_NOTIFY_CHANGE_SIZE |\
					 FILE_NOTIFY_CHANGE_CREATION |\
					 FILE_NOTIFY_CHANGE_SECURITY


//������Ʈ�� ��ȣ �÷��� ����
#define REG_CHANGE_FLAGS REG_NOTIFY_CHANGE_NAME |\
					 REG_NOTIFY_CHANGE_LAST_SET

//RegFsNotify.cpp��ġ
void Output(USHORT Color, LPTSTR format, ... );
void StartFileMonitor(void);  //file.cpp��ġ
void StartRegistryMonitor(void);//reg.cpp��ġ


//extern �ܺ� ����  --> �ڵ� ����
extern HANDLE  g_hStopEvent;  
extern HANDLE  g_hFile;
extern HANDLE  g_hRegWatch[2];

// whitelisted filenames or paths
// ȭ��Ʈ����Ʈ �����̸� �Ǵ� ��� : ��ȭ ������ �������� ����, ��η� ����
//_T:
// ȭ��Ʈ ����Ʈ�� �� ����Ʈ �ȿ� �ִ� ��Ģ(�����ϴٰ� �Ǹ�Ȱ�) �̿ܿ� ��� ���� �����ϴ� ��.
// �� ����Ʈ�� ����Ʈ�� �ִ°��� ����(�������̶�� �Ǹ�Ȱ�.)�ϰ� �������� ��� ����ϴ°�.

static LPTSTR g_szAllow[] = {
		
	//_T("WINDOWS\\system32\\config\\"),
	//_T("\\ntuser.dat.LOG"),
	//_T("UsrClass.dat.LOG"),
	//_T("RegFsNotify.txt"),
	//_T("_restore"),
	//_T("CatRoot2"),
	//_T("\\Microsoft\\Cryptography\\RNG"),
	//_T("\\Microsoft\\WBEM"),
	_T("Users\kosta\Desktop\asdf"),
	_T("Desktop\asdf"),
	_T("asdf"),
	
};

// return true if szFile is in the g_szAllow list
//ȭ��Ʈ����Ʈ�� ��ϵ� ���� �Ǵ� ���--
//TRUE�� ��ȯ

//�Լ� _tcsstr(�˻����, �˻���)=wcsstr()
//��ȯ : ������ ��� �ּҰ��� ��ȯ/ �˻��� �κк��� ������ ���ڿ� ��ȯ

//szFile���� g_szAllow(ȭ��Ʈ����Ʈ)�� �˻��� ��� ��ȯ���� NULL�� �ƴҰ��
//szFile�� ȭ��Ʈ����Ʈ�� �ܾ�� ������ �ܾ �����ϴ� ������ �Ǵ��Ͽ� True�� ��ȯ
//�˻� ����� ������, false��ȯ

static BOOL IsWhitelisted(LPTSTR szFile)
{
	for(int i=0; i<sizeof(g_szAllow)/sizeof(LPTSTR); i++)
	{
		if (_tcsstr(szFile, g_szAllow[i]) != NULL) 
			return TRUE;
	}
	return FALSE;
}