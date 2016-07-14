#include "mon.h"

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

void checkState(){
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
}