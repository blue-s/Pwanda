#include "mon.h"
#include <stdio.h>
#include <string.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

TCHAR * resultBuffer;
ULARGE_INTEGER g_tmStart;

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

//[4] 레지스트리 변화를 탐지하고 키 값과 데이터를 가져온다 
void QueryKey(HKEY hKey){

	TCHAR    achKey[MAX_KEY_LENGTH];	
	DWORD    cbName;					
	TCHAR    achClass[MAX_PATH] = _T("");	
	DWORD    cchClassName = MAX_PATH;	
	DWORD    cSubKeys=0;				
	DWORD    cbMaxSubKey;				
	DWORD    cchMaxClass;		
	DWORD    cValues;					
	DWORD    cchMaxValue;			
	DWORD    cbMaxValueData;		
	DWORD    cbSecurityDescriptor;		

	FILETIME ftWrite;
	ULARGE_INTEGER tmWrite;

	DWORD	i, retCode;
	HKEY	hNewKey;
	TCHAR	achValue[MAX_VALUE_NAME];
	DWORD	cchValue = MAX_VALUE_NAME;

	TCHAR	achData[MAX_VALUE_NAME];
	DWORD	cchData = MAX_VALUE_NAME;

	TCHAR	path[MAX_VALUE_NAME] = _T("");
	TCHAR	search[MAX_VALUE_NAME] = _T("Roaming"); 
	TCHAR*	result = _T("");

	LPSTR lpresult = "";

	// 레지스트리가 가진 값, 데이터, 하위키 이름과 개수 등을 가져오는 함수 
	retCode = RegQueryInfoKey(
		hKey,						
		achClass,					
		&cchClassName,			
		NULL,					
		&cSubKeys,					
		&cbMaxSubKey,			
		&cchMaxClass,				
		&cValues,						
		&cchMaxValue,				
		&cbMaxValueData,				
		&cbSecurityDescriptor,			
		&ftWrite
		);				

	if(retCode == ERROR_SUCCESS)  
	{ 
		tmWrite.HighPart = ftWrite.dwHighDateTime;
		tmWrite.LowPart  = ftWrite.dwLowDateTime;

		// 가장 최근 쓰기 시간이 start보다 크면 변경된 것 
		if (tmWrite.QuadPart > g_tmStart.QuadPart)
		{
			// 지정된 레지스트리 키의 값들(Values) 수만큼
			for(i=0; i<cValues; i++) 
			{
				cchValue = MAX_VALUE_NAME;	// Values 최대 크기 
				achValue[0] = '\0';			// 값 초기화 
				cchData = MAX_VALUE_NAME;	// Data 최대 크기 
				achData[0] = '\0';			// 데이터 초기화 

				// 지정된 레지스트리 키가 가지는 값 열거하는 함수 
				retCode = RegEnumValue(
					hKey,					
					i,                      
					achValue, 
					&cchValue,					
					NULL,					
					NULL,					
					(LPBYTE)achData,	 				
					&cchData
					);					

				// 레지스트리 키의 Value 와 Data 값 출력 
				if(retCode == ERROR_SUCCESS)
				{ 
					// Output_Console(FOREGROUND_RED, _T("[Value %d] %s = %s\n"), i+1, achValue, achData); 

					// achData에 랜섬웨어 프로세스 경로가 나와있으므로 path에 복사 
					memcpy(path, achData, MAX_VALUE_NAME);	

					// path 문자열에서 search 문자열이 있는지 검색 
					if(result = _tcsstr(path, search)){
						resultBuffer = result + 8;
						ExtractProcess(3, resultBuffer);
					}
				}
			}
		} // end for

		retCode = RegOpenKeyEx( hKey, achKey, 0, KEY_ALL_ACCESS, &hNewKey);

		if(retCode == ERROR_SUCCESS) 
		{ 
			RegCloseKey(hNewKey);
		}
	}
}

//[3] 시간 업데이트
void UpdateTime(void)
{
	SYSTEMTIME st;
	FILETIME   ft;

	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);

	g_tmStart.HighPart = ft.dwHighDateTime;
	g_tmStart.LowPart  = ft.dwLowDateTime;
}

//[2] 스레드 실행
DWORD WatchKey(PREGMON p)
{
	HANDLE hEvent;
	HKEY   hKey;
	LONG   ret;

	Output_Console(0, _T("Monitoring HKEY %x\\%s\n"), p->hMainKey, p->szSubkey); 

	// 레지스트리 키 생성 (현재 내가 지정한 경로의 핸들을 가져온다)
	ret = RegOpenKeyEx(
		p->hMainKey, 
		p->szSubkey, 
		0,
		KEY_ALL_ACCESS,
		&hKey
		);

	if (ret != ERROR_SUCCESS)	return -1;

	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (hEvent == NULL)			return -1;

	// WAIT_OBJECT_0: 성공 
	// 스레드 종료가 되지 않으면 
	while(WaitForSingleObject(g_hStopEvent, 1) != WAIT_OBJECT_0)
	{
		// 파일 변경 시간을 시스템 시간으로 변경
		UpdateTime();	

		ret = RegNotifyChangeKeyValue(hKey, 
			TRUE, 
			REG_CHANGE_FLAGS, 
			hEvent, 
			TRUE
			);

		// 변경 실패한 경우 
		if (ret != ERROR_SUCCESS)
		{
			_tprintf(_T("Registry Open Failed!! \n"));
			break;
		}

		// 변경을 무한정 기다려야 하면 빠져나온다 
		if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED)	break;

		QueryKey(hKey); // [4] 키 값과 Data를 받음
	}

	RegCloseKey(hKey);
	CloseHandle(hEvent);
	return 0;
}

//[1] 레지스트리 모니터 시작 
void StartRegistryMonitor(void)
{
	HMODULE hNtdll = GetModuleHandle(_T("ntdll.dll"));
	ZwQueryKey = (ZWQUERYKEY)GetProcAddress(hNtdll, "NtQueryKey");

	PREGMON p[1];

	p[0] = new REGMON;
	p[0]->hMainKey = HKEY_CURRENT_USER;
	p[0]->szSubkey = _T("SOFTWARE\\\Microsoft\\Windows\\CurrentVersion\\Run");
	// Run(자동 실행 등록) 경로 설정 

	g_hRegWatch[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WatchKey, p[0], 0, NULL);
}