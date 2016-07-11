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

	//hkey의 KeyNameInformation을 info에 저장
	//레지스트리 키의 클래스, 하위키의 수와 크기에 대한 정보를 제공를 받아와 info에 저장
	if (ZwQueryKey != NULL) {
		n = ZwQueryKey(hKey, KeyNameInformation,
			&info, sizeof(info), &dwLen);

		_tprintf(_T("------------------------------------------------------------------------\n"));

		Output(FOREGROUND_RED, _T("[info.Name] %s \n"), info.Name);

		if (n == STATUS_SUCCESS &&
			info.NameLength > 0 &&
			info.NameLength < MAX_KEY_LENGTH) 
		{ 
			//szName를 info.Name에 복사
			wcscpy(szName, info.Name);

			Output(FOREGROUND_RED, _T("[szName] %s \n"), szName);

			//마지막 값은 \x00
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


	//서브키의 개수를 얻음
	ret = RegQueryInfoKey(
		hKey, //조사하고자 하는 키의 핸들                  
		NULL, NULL, NULL,               
		&cSubKeys,  //서브키의 개수            
		NULL, NULL, 
		&cValues, //값의 개수
		NULL, NULL, NULL, NULL);       

	//서브키의 개수만큼 반복
	for (i=0; i<cSubKeys; i++) 
	{ 
		cbName = MAX_KEY_LENGTH;

		//서브키 열거
		ret = RegEnumKeyEx(
			hKey,
			i,
			szKey, //***서브키명을 담는 변수***
			&cbName, //szKey의 사이즈를 담는 변수
			NULL, NULL, NULL, 
			&ftWrite); //최종 기입 시간

		//성공시
		if (ret == ERROR_SUCCESS) 
		{

			tmWrite.HighPart = ftWrite.dwHighDateTime;
			tmWrite.LowPart  = ftWrite.dwLowDateTime;

			//시간비교
			if (tmWrite.QuadPart > g_tmStart.QuadPart)
			{
				//szName 초기화
				memset(szName, 0, sizeof(szName));

				GetKeyName(hKey, szName); // -> [4] GetKeyName()

				//두문자 이어 붙이기
				_tcscat(szName, _T("\\"));
				_tcscat(szName, szKey);

				Output(FOREGROUND_GREEN, _T("[Subkey] %s \n"), szKey);

				if (!IsWhitelisted(szName)) { 
					Output(FOREGROUND_BLUE, _T("[REGISTRY] %s \n"), szName);

					Output(FOREGROUND_GREEN, _T("[cValues] %d \n"), cValues);

					if(cValues) //서브키내에 값이 존재하면
					{
						for(j=0, ret=ERROR_SUCCESS; j<cValues; j++)
						{
							cchValue = MAX_VALUE_NAME; 
							achValue[0] = '\0';

							ret = RegEnumValue( //지정한 키가 가지고 있는 모든 값의 이름들
								hKey,             
								j,                
								achValue,       //값의 이름
								&cchValue,		//achValue의 크기
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


			//레지스트리 오픈
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

	//처음 출력 부분
	Output(0, _T("Monitoring HKEY %x\\%s\n"), 
		p->hMainKey, p->szSubkey); //아래 StartRegistryMonitor에서 설정한 메인키, 서브키 출력

	//레지스트리 Key 오픈
	ret = RegOpenKeyEx(
		p->hMainKey, 
		p->szSubkey, 
		0,
		KEY_ALL_ACCESS,
		//KEY_READ | KEY_NOTIFY | KEY_ALL_ACCESS, 
		&hKey);

	//에러 처리
	if (ret != ERROR_SUCCESS)
	{
		return -1;
	}
	else
		printf("RegOpenKeyEx() is OK.\n");

	//시스템에 의해 신호를 얻을 이벤트를 생성
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	//에러 처리
	if (hEvent == NULL)
	{
		return -1;
	}

	//mon.h : HANDLE g_hStopEvent;
	//특정 스레드가 종료할 때까지 대기
	while(WaitForSingleObject(g_hStopEvent, 1) != WAIT_OBJECT_0)
	{
		UpdateTime();

		//변경 알림을 받을 수 있도록 등록 -> 나중에 
		ret = RegNotifyChangeKeyValue(hKey, 
			TRUE, 
			REG_CHANGE_FLAGS, 
			hEvent, 
			TRUE);

		//에러 처리
		if (ret != ERROR_SUCCESS)
		{
			_tprintf(_T("Registry Open Failed!! \n"));
			break;
		}

		//에러처리
		if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED)
		{
			break;
		}

		GetRegistryChanges(hKey); // -> [3] GetRegistryChanges()
	}

	//스레드 종료가 되면 프로그램이 끝났음을 출력
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

	//경로 지정할 경우 여기서 메인키, 서브키 설정
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