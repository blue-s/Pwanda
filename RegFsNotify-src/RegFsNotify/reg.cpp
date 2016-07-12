#include "mon.h"
#include <stdio.h>
#include <string.h>

ULARGE_INTEGER g_tmStart;

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

char *src="";

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

//[3]
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
	FILETIME ftLastWriteTime;		
	DWORD i, retCode;
	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	TCHAR  achData[MAX_VALUE_NAME];
	DWORD cchData = MAX_VALUE_NAME;

	//
	TCHAR path[MAX_VALUE_NAME] = _T("");
	TCHAR search[MAX_VALUE_NAME] = _T("OneDrive");
	TCHAR * result=_T("");
	
	TCHAR * FileName=_T("");


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
		&ftLastWriteTime);				

	if(cSubKeys)
	{
		printf("Subkey Names:\n");
		for(i=0; i<cSubKeys; i++) 
		{
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(
				hKey,					
				i,					
				achKey,					
				&cbName,				
				NULL,					
				NULL,					
				NULL,					
				&ftLastWriteTime		
				);
			if(retCode == ERROR_SUCCESS)  
			{ Output(FOREGROUND_GREEN, _T("[%d] %s\n"), i+1, achKey); }
		}
		printf("Number of subkeys: %d\n\n", cSubKeys);
	}
	else
		printf("RegEnumKeyEx(), there is no subkey.\n");

	if(cValues) 
	{
		for(i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
		{
			cchValue = MAX_VALUE_NAME; 
			achValue[0] = '\0';
			cchData = MAX_VALUE_NAME; 
			achData[0] = '\0';

			retCode = RegEnumValue(
				hKey,					
				i,                      
				achValue,	//값 이름 저장 배열 
				&cchValue,	//				
				NULL,					
				NULL,					
				(LPBYTE)achData,	// 포인터 				
				&cchData);					

			if(retCode == ERROR_SUCCESS)
			{ Output(FOREGROUND_RED, _T("[Value %d] %s = %s\n"), i+1, achValue, achData); 

				//1. 경로 값 넣어주기
				memcpy(path, achData, MAX_VALUE_NAME);
				Output(FOREGROUND_GREEN, _T("[Path] %s\n"), path);
				Output(FOREGROUND_BLUE, _T("[search] %s\n"), search);
				
				//2. strstr로 파싱하기 
				//Output(FOREGROUND_BLUE, _T("[search] %s\n"), (char *)search);

				//Output(FOREGROUND_GREEN, _T("[result] %s\n"), _tcsstr(path,search));

				//_tprintf(_T("---------------------> %s \n"), _tcsstr(path,search));

				
				if(result = _tcsstr(path,search)){

					Output(FOREGROUND_GREEN, _T("[result] %s\n"), result);

					FileName = result+9;

					_tprintf(_T("-------------------------------------------------------------------------> [FileName] %s \n"), FileName);

					//Output(FOREGROUND_BLUE, _T("[result] %s\n"), src);
					//_tprintf(_T("No search"));
					//Output(FOREGROUND_BLUE, _T("[Src] %s\n"), src);
					
				}
				
				

			}
			//achValue: Value 값 / achData: 해당 Value의 Data 값 


		} // end for

		printf("Number of values: %d\n", cValues);
	}
	else
		printf("No value under this key/subkey...\n");  

	_tprintf(_T("------------------------------------------------------------------------\n"));

}


//[2]
DWORD WatchKey(PREGMON p)
{
	HANDLE hEvent;
	HKEY   hKey;
	LONG   ret;

	//처음 출력 부분
	Output(0, _T("Monitoring HKEY %x\\%s\n"), 
		p->hMainKey, p->szSubkey); 

	//레지스트리 Key 오픈
	ret = RegOpenKeyEx(
		p->hMainKey, 
		p->szSubkey, 
		0,
		KEY_ALL_ACCESS,
		&hKey);

	if (ret != ERROR_SUCCESS)
	{
		return -1;
	}

	else
		printf("RegOpenKeyEx() is OK.\n");


	// --------------------end of RegOpenKeyEx 


	QueryKey(hKey); // -> [3] QueryKey()

	//return WatchKey(p);
	return 0;
}


//[1]
void StartRegistryMonitor(void)
{
	HMODULE hNtdll = GetModuleHandle(_T("ntdll.dll"));
	ZwQueryKey = (ZWQUERYKEY)GetProcAddress(hNtdll, "NtQueryKey");

	PREGMON p[1];

	p[0] = new REGMON;
	p[0]->hMainKey = HKEY_CURRENT_USER;
	p[0]->szSubkey = _T("SOFTWARE\\\Microsoft\\Windows\\CurrentVersion\\Run");

	g_hRegWatch[0] = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)WatchKey, p[0], 0, NULL); // -> [2] WatchKey


}