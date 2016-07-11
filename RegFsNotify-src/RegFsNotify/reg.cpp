//HKEY_CLASSES_ROOT			= (0x80000000)
//HKEY_CURRENT_USER			= (0x80000001)
//HKEY_LOCAL_MACHINE		= (0x80000002)
//HKEY_USERS				= (0x80000003)
//HKEY_PERFORMANCE_DATA		= (0x80000004)
//HKEY_CURRENT_CONFIG		= (0x80000005)
//HKEY_DYN_DATA				= (0x80000006)


#include "mon.h"
#include <stdio.h>
///////////////////////////////////////////////////////

ULARGE_INTEGER g_tmStart;

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

///////////////////////////////////////////////////////

typedef struct REGMON { 
	HKEY   hMainKey;
	LPTSTR szSubkey;
} REGMON, *PREGMON;

typedef struct _KEY_NAME_INFORMATION {
	ULONG NameLength;
	WCHAR Name[4096];
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

typedef enum _KEY_INFORMATION_CLASS {
	KeyBasicInformation            = 0,
	KeyNodeInformation             = 1,
	KeyFullInformation             = 2,
	KeyNameInformation             = 3,
	KeyCachedInformation           = 4,
	KeyFlagsInformation            = 5,
	KeyVirtualizationInformation   = 6,
	KeyHandleTagsInformation       = 7,
	MaxKeyInfoClass                = 8 
} KEY_INFORMATION_CLASS;

typedef NTSTATUS (WINAPI *ZWQUERYKEY)(
	HANDLE, 
	KEY_INFORMATION_CLASS, 
	PVOID, 
	ULONG, 
	PULONG);

ZWQUERYKEY ZwQueryKey;

///////////////////////////////////////////////////////

//[4]
void GetKeyName(HKEY hKey, LPWSTR szName)
{
	//typedef struct _KEY_NAME_INFORMATION {
	//	ULONG NameLength;
	//	WCHAR Name[4096];
	//} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

	//TCHAR   achValue[MAX_VALUE_NAME];
	//DWORD    cchValue = MAX_VALUE_NAME;
	//DWORD    i=0, ret; 
	//memset(achValue, 0, sizeof(achValue));
	//ret = RegEnumValue(
	//	hKey,              // Handle to an open key
	//	i,                      // Index of value
	//	achValue,       // Value name 
	//	&cchValue,		// Buffer for value name
	//	NULL,            // Reserved
	//	NULL,            // Value type
	//	NULL,           // Value data
	//	NULL);          // Buffer for value data

	//if(ret == ERROR_SUCCESS){ 
	//	printf("(%d) Value Name: %s.\n", i+1, achValue); 
	//}

	KEY_NAME_INFORMATION info;
	DWORD dwLen;
	NTSTATUS n;

	memset(&info, 0, sizeof(info));

	//hkey�� KeyNameInformation�� info�� ����
	//������Ʈ�� Ű�� Ŭ����, ����Ű�� ���� ũ�⿡ ���� ������ ������ �޾ƿ� info�� ����
	if (ZwQueryKey != NULL) {
		n = ZwQueryKey(hKey, KeyNameInformation,
			&info, sizeof(info), &dwLen);

		_tprintf(_T("------------------------------------------------------------------------\n"));

		Output(FOREGROUND_RED, _T("[info.Name] %s \n"), info.Name);

		if (n == STATUS_SUCCESS &&
			info.NameLength > 0 &&
			info.NameLength < MAX_KEY_LENGTH) 
		{ 
			//szName�� info.Name�� ����
			wcscpy(szName, info.Name);

			Output(FOREGROUND_RED, _T("[szName] %s \n"), szName);

			//������ ���� \x00
			szName[info.NameLength-1] = L'\x00';
		}
	}
}

///////////////////////////////////////////////////////

//[3]
void GetRegistryChanges(HKEY hKey) 
{ 
	TCHAR    szKey[MAX_KEY_LENGTH]; 
	TCHAR    szName[MAX_KEY_LENGTH];

	TCHAR	achValue[MAX_VALUE_NAME];
	DWORD	cchValue=MAX_VALUE_NAME;

	DWORD    cbName;                  
	DWORD    cSubKeys=0; 
	DWORD    cValues;

	FILETIME ftWrite;      
	DWORD    i, j, ret; 
	HKEY     hNewKey;
	ULARGE_INTEGER tmWrite;


	//����Ű�� ������ ����
	ret = RegQueryInfoKey(
		hKey, //�����ϰ��� �ϴ� Ű�� �ڵ�                  
		NULL, NULL, NULL,               
		&cSubKeys,  //����Ű�� ����            
		NULL, NULL, 
		&cValues, //���� ����
		NULL, NULL, NULL, NULL);       

	//����Ű�� ������ŭ �ݺ�
	for (i=0; i<cSubKeys; i++) 
	{ 
		cbName = MAX_KEY_LENGTH;

		//����Ű ����
		ret = RegEnumKeyEx(
			hKey,
			i,
			szKey, //***����Ű���� ��� ����***
			&cbName, //szKey�� ����� ��� ����
			NULL, NULL, NULL, 
			&ftWrite); //���� ���� �ð�

		//������
		if (ret == ERROR_SUCCESS) 
		{

			tmWrite.HighPart = ftWrite.dwHighDateTime;
			tmWrite.LowPart  = ftWrite.dwLowDateTime;

			//�ð���
			if (tmWrite.QuadPart > g_tmStart.QuadPart)
			{
				//szName �ʱ�ȭ
				memset(szName, 0, sizeof(szName));

				GetKeyName(hKey, szName); // -> [4] GetKeyName()

				//�ι��� �̾� ���̱�
				_tcscat(szName, _T("\\"));
				_tcscat(szName, szKey);

				Output(FOREGROUND_GREEN, _T("[Subkey] %s \n"), szKey);

				if (!IsWhitelisted(szName)) { 
					Output(FOREGROUND_BLUE, _T("[REGISTRY] %s \n"), szName);

					Output(FOREGROUND_GREEN, _T("[cValues] %d \n"), cValues);

					if(cValues) //����Ű���� ���� �����ϸ�
					{
						for(j=0, ret=ERROR_SUCCESS; j<cValues; j++)
						{
							cchValue = MAX_VALUE_NAME; 
							achValue[0] = '\0';

							ret = RegEnumValue( //������ Ű�� ������ �ִ� ��� ���� �̸���
								hKey,             
								j,                
								achValue,       //���� �̸�
								&cchValue,		//achValue�� ũ��
								NULL, NULL, NULL, NULL);

							if(ret == ERROR_SUCCESS)
							{
								Output(FOREGROUND_RED, _T("[Value %d] %s \n"), j+1, achValue);
							}
							printf("[cValues Count] %d\n", cValues);
						}
					}
					else{ _tprintf(_T("Value Failed!! \n")); }
				}
			}


			//������Ʈ�� ����
			ret = RegOpenKeyEx(hKey, 
				szKey, 0, 
				KEY_ALL_ACCESS,
				//KEY_READ, 
				&hNewKey);


			if (ret == ERROR_SUCCESS) 
			{ 
				GetRegistryChanges(hNewKey);
				RegCloseKey(hNewKey);
			}
		}
	}
}

///////////////////////////////////////////////////////

void UpdateTime(void)
{
	SYSTEMTIME st;
	FILETIME   ft;

	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);

	g_tmStart.HighPart = ft.dwHighDateTime;
	g_tmStart.LowPart  = ft.dwLowDateTime;	
}

///////////////////////////////////////////////////////

//[2]
DWORD WatchKey(PREGMON p)
{
	HANDLE hEvent;
	HKEY   hKey;
	LONG   ret;

	//ó�� ��� �κ�
	Output(0, _T("Monitoring HKEY %x\\%s\n"), 
		p->hMainKey, p->szSubkey); //�Ʒ� StartRegistryMonitor���� ������ ����Ű, ����Ű ���

	//������Ʈ�� Key ����
	ret = RegOpenKeyEx(
		p->hMainKey, 
		p->szSubkey, 
		0,
		KEY_ALL_ACCESS,
		//KEY_READ | KEY_NOTIFY | KEY_ALL_ACCESS, 
		&hKey);

	//���� ó��
	if (ret != ERROR_SUCCESS)
	{
		return -1;
	}
	else
		printf("RegOpenKeyEx() is OK.\n");

	//�ý��ۿ� ���� ��ȣ�� ���� �̺�Ʈ�� ����
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	//���� ó��
	if (hEvent == NULL)
	{
		return -1;
	}

	//mon.h : HANDLE g_hStopEvent;
	//Ư�� �����尡 ������ ������ ���
	while(WaitForSingleObject(g_hStopEvent, 1) != WAIT_OBJECT_0)
	{
		UpdateTime();

		//���� �˸��� ���� �� �ֵ��� ��� -> ���߿� 
		ret = RegNotifyChangeKeyValue(hKey, 
			TRUE, 
			REG_CHANGE_FLAGS, 
			hEvent, 
			TRUE);

		//���� ó��
		if (ret != ERROR_SUCCESS)
		{
			_tprintf(_T("Registry Open Failed!! \n"));
			break;
		}

		//����ó��
		if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED)
		{
			break;
		}

		GetRegistryChanges(hKey); // -> [3] GetRegistryChanges()
	}

	//������ ���ᰡ �Ǹ� ���α׷��� �������� ���
	Output(0, _T("Closing HKEY %x\\%s\n"), 
		p->hMainKey, p->szSubkey);

	RegCloseKey(hKey);
	CloseHandle(hEvent);

	return 0;
}

///////////////////////////////////////////////////////

//[1]
void StartRegistryMonitor(void)
{
	HMODULE hNtdll = GetModuleHandle(_T("ntdll.dll"));
	ZwQueryKey = (ZWQUERYKEY)GetProcAddress(hNtdll, "NtQueryKey");

	//typedef struct REGMON { 
	//	HKEY   hMainKey;
	//	LPTSTR szSubkey;
	//} REGMON, *PREGMON;

	PREGMON p[2];

	p[0] = new REGMON;
	p[1] = new REGMON;

	//��� ������ ��� ���⼭ ����Ű, ����Ű ����
	p[0]->hMainKey = HKEY_LOCAL_MACHINE;
	p[0]->szSubkey = _T("SOFTWARE");

	//mon.h : HANDLE g_hRegWatch[2];
	g_hRegWatch[0] = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)WatchKey, p[0], 0, NULL); // -> [2] WatchKey()

	p[1]->hMainKey = HKEY_CURRENT_USER;
	p[1]->szSubkey = _T("SOFTWARE");

	g_hRegWatch[1] = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)WatchKey, p[1], 0, NULL); // -> [2] WatchKey()

}