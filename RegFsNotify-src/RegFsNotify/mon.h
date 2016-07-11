#pragma once //해더 중복 방지- 컴파일러에게 헤더 파일이 한번만 빌드되도록 알림.

#include <windows.h>  //win API 함수들이 존재
#include <tchar.h>  //MBCS와 WBCS를 동시 지원 - 아스키코드와 유니코드를 동시에 지원한다는 말과 일치.
#include <stdio.h>

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) // 성공상태 리턴


//파일 변화 플래그 선언

// 파일이름 변경, 파일생성, 삭제 알림
//다렉터리 생성, 삭제 알림
//지정된 디렉터리의 파일 속성 변경알림
//지정된 디렉터리에 있는 파일 크기 변경 알림
//지정된 디렉터리에 있는 파일의 생성시간이 변경된 경우 알림
//지정된 디렉터리의 보안 디스크립터가 변경된 경우 알림.

#define FILE_CHANGE_FLAGS FILE_NOTIFY_CHANGE_FILE_NAME |\
					 FILE_NOTIFY_CHANGE_DIR_NAME |\
					 FILE_NOTIFY_CHANGE_ATTRIBUTES |\
					 FILE_NOTIFY_CHANGE_SIZE |\
					 FILE_NOTIFY_CHANGE_CREATION |\
					 FILE_NOTIFY_CHANGE_SECURITY


//레지스트리 변호 플래그 선언
#define REG_CHANGE_FLAGS REG_NOTIFY_CHANGE_NAME |\
					 REG_NOTIFY_CHANGE_LAST_SET

//RegFsNotify.cpp위치
void Output(USHORT Color, LPTSTR format, ... );
void StartFileMonitor(void);  //file.cpp위치
void StartRegistryMonitor(void);//reg.cpp위치


//extern 외부 변수  --> 핸들 정의
extern HANDLE  g_hStopEvent;  
extern HANDLE  g_hFile;
extern HANDLE  g_hRegWatch[2];

// whitelisted filenames or paths
// 화이트리스트 파일이름 또는 경로 : 변화 감지를 하지않은 파일, 경로로 추정
//_T:
// 화이트 리스트는 그 리스트 안에 있는 규칙(안전하다고 판명된것) 이외에 모든 것을 차단하는 것.
// 블랙 리스트는 리스트에 있는것을 제한(악의적이라고 판명된것.)하고 나머지는 모두 허용하는것.

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
//화이트리스트에 등록된 파일 또는 경로--
//TRUE를 반환

//함수 _tcsstr(검색대상, 검색어)=wcsstr()
//반환 : 성공한 경우 주소값을 반환/ 검색된 부분부터 끝까지 문자열 반환

//szFile에서 g_szAllow(화이트리스트)를 검색한 결과 반환값이 NULL이 아닐경우
//szFile에 화이트리스트의 단어와 동일한 단어가 존재하는 것으로 판단하여 True를 반환
//검색 결과가 없으면, false반환

static BOOL IsWhitelisted(LPTSTR szFile)
{
	for(int i=0; i<sizeof(g_szAllow)/sizeof(LPTSTR); i++)
	{
		if (_tcsstr(szFile, g_szAllow[i]) != NULL) 
			return TRUE;
	}
	return FALSE;
}