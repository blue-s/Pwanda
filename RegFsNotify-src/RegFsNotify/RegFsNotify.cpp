#include "mon.h"

 HANDLE g_hFile;
 HANDLE g_hStopEvent;
 HANDLE g_hRegWatch[2];

 // [2]를 위한 함수 추가
BOOL GetProcessElevation(TOKEN_ELEVATION_TYPE *pElevationType, BOOL *pIsAdmin)
{
	HANDLE hToken = NULL;
    BOOL bResult = FALSE;
    DWORD dwSize = 0;
 
    // 현재 프로세스의 토큰을 얻는다.
    if ( !OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) )
        return FALSE;
 
    // 권한상승 형태에 대한 정보를 얻는다.
    if ( GetTokenInformation(hToken, TokenElevationType, pElevationType, sizeof(TOKEN_ELEVATION_TYPE), &dwSize) )
    {
        BYTE adminSID[SECURITY_MAX_SID_SIZE];
        dwSize = sizeof(adminSID);
         
        // 관리자 그룹의 SID 값을 생성한다.
        CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, &dwSize);
 
        if ( *pElevationType == TokenElevationTypeLimited )
        {
            HANDLE hUnfilteredToken = NULL;
             
            // 연결된 토큰의 핸들을 얻는다.
            GetTokenInformation(hToken, TokenLinkedToken, (void *)&hUnfilteredToken, sizeof(HANDLE), &dwSize);
 
            // 원래의 토큰이 관리자의 SID를 포함하고 있는지 여부를 확인한다.
            if ( CheckTokenMembership(hUnfilteredToken, &adminSID, pIsAdmin) )
                bResult = TRUE;
             
            CloseHandle(hUnfilteredToken);
        }
        else
        {
            *pIsAdmin = IsUserAnAdmin();
            bResult = TRUE;
        }
    }
 
    CloseHandle(hToken);
    return bResult;
}

USHORT GetConsoleTextAttribute(HANDLE hConsole)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return(csbi.wAttributes);
}

void Output(USHORT Color, LPTSTR format, ... )
{
	va_list args;
	int len;
	DWORD cb;
	LPTSTR buffer;

	va_start(args, format);
	
	len = _vsctprintf(format, args) + sizeof(TCHAR);
	buffer = new TCHAR[len * sizeof(TCHAR)];

	if (!buffer) { 
		return;
	}

	_vstprintf_s(buffer, len, format, args);

	if (g_hFile != INVALID_HANDLE_VALUE) {
#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1];
		if (str) { 
			memset(str, 0, len + 1);

			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);

			WriteFile(g_hFile, str, strlen(str), &cb, NULL);

			delete[] str;
		}
#else 
	WriteFile(g_hFile, buffer, strlen(buffer), &cb, NULL);
#endif
	} 

	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (Color) 
	{ 
		SetConsoleTextAttribute(
			Handle, Color | FOREGROUND_INTENSITY);
	} 

	_tprintf(buffer);

	SetConsoleTextAttribute(Handle, 
		FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

   delete[] buffer;
}
 
BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch(fdwCtrlType) 
	{ 
    case CTRL_C_EVENT:
		SetEvent(g_hStopEvent);
		return TRUE;
    case CTRL_CLOSE_EVENT:
		SetEvent(g_hStopEvent);
		return TRUE; 

    case CTRL_BREAK_EVENT:
		return FALSE; 
    case CTRL_LOGOFF_EVENT:
		return FALSE; 
    case CTRL_SHUTDOWN_EVENT:
		return FALSE; 
    default: 
		return FALSE; 
	} 
} 

void _tmain(int argc, TCHAR *argv[])
{
	g_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	g_hFile = CreateFile(_T("RegFsNotify.txt"), 
		GENERIC_WRITE,	
		FILE_SHARE_READ, 0,
		CREATE_ALWAYS, 0, NULL);

	// 파일만 출력하도록 변경
	//HANDLE hThread[2];

	//hThread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	//hThread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRegistryMonitor, NULL, 0, NULL);
	//
	//WaitForMultipleObjects(2, (const HANDLE*)&hThread, TRUE, INFINITE);
	//
	//TerminateThread(g_hRegWatch[0], 0);
	//TerminateThread(g_hRegWatch[1], 0);
	//
	//CloseHandle(g_hStopEvent);
	//CloseHandle(g_hFile);

	HANDLE hThread;
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	
	
	// [2]를 위한 코드 추가 ***************
	TOKEN_ELEVATION_TYPE t;
    BOOL bAdmin = FALSE;
    TCHAR szUser[0xFF] = {0};
    DWORD dwUser = _countof(szUser);
    GetUserName(szUser, &dwUser);
 
    _tsetlocale(LC_ALL, _T("korean"));
 
    if ( GetProcessElevation(&t, &bAdmin) )
    {
        if ( bAdmin )
            _tprintf(_T("%s is admin, "), szUser);
        else
            _tprintf(_T("%s is not admin, "), szUser);
 
        switch (t)
        {
        case TokenElevationTypeDefault:
            _tprintf(_T("기본 사용자이거나, UAC 기능이 OFF 된 상태임.\n"));
            break;
        case TokenElevationTypeFull:
            _tprintf(_T("권한상승되었음\n"));	// 우리가 원하는 부분
            break;
        case TokenElevationTypeLimited:
            _tprintf(_T("필터된 토큰에 의해 제한된 권한으로 수행되었음.\n"));
            break;
        default:
            break;
        }
    }
 
    getchar();
	// **************************************
	
	
	WaitForMultipleObjects(1, (const HANDLE*)&hThread, TRUE, INFINITE);
	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile);


	_tprintf(_T("Program terminating.\n"));
}