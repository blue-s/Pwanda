#pragma once

#include <windows.h>
#include <tchar.h>

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
// NTSTATUS: 커널API 함수 호출 후 반환 (확실하지 않음)
// 성공 상태 리턴 

#define FILE_CHANGE_FLAGS FILE_NOTIFY_CHANGE_FILE_NAME |\
					 FILE_NOTIFY_CHANGE_DIR_NAME |\
					 FILE_NOTIFY_CHANGE_ATTRIBUTES |\
					 FILE_NOTIFY_CHANGE_SIZE |\
					 FILE_NOTIFY_CHANGE_CREATION |\
					 FILE_NOTIFY_CHANGE_SECURITY
// * 위 value들은 모두 '알림'을 위한 플래그들 
// 1. 파일 이름 변경 | 파일이 생성 | 삭제 
// 2. 디렉터리 생성 | 삭제 
// 3. 디렉터리의 파일 속성 변경
// 4. 디렉터리에 있는 파일 크기 변경
// 5. 디렉터리에 있는 파일 생성 시간 변경 Q: 생성 시간이 어떻게 변경되니? 조작밖에는. 
// 6. 디렉터리의 보안 디스크립터가 변경 
// 같은 파일 변화를 FILE_CHANGE_FLAGS로 대표해서 사용하겠다

#define REG_CHANGE_FLAGS REG_NOTIFY_CHANGE_NAME |\
					 REG_NOTIFY_CHANGE_LAST_SET
// * REGISTRY도 마찬가지로 파일 처럼 변화 플래그를 define해줘서 '알림'을 제공한다 
// 1. 서브키가 추가 | 삭제
// 2. 값(value) 추가 | 삭제 | 수정 

void Output(USHORT Color, LPTSTR format, ... );
// RegFsNotify.cpp에 위치 
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
// whitelisted : 허용된 목록 -> 이 문자열 배열을 어디다 가져가서 쓰는지 알아야겠지 
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
	// g_szAllow 배열 크기만큼 돌리는데 
	for(int i=0; i<sizeof(g_szAllow)/sizeof(LPTSTR); i++)
	{
		if (_tcsstr(szFile, g_szAllow[i]) != NULL) 
			// _tcsstr(검색 대상, 검색어)/
			return TRUE;
	}
	return FALSE;
}
// szFile에 g_szAllow 배열 값들과 동일한 단어가 있는지 체크하여 있으면 TRUE를 반환 
// 없으면 FALSE를 반환 
