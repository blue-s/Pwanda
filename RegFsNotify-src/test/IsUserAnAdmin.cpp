/*
 * 현재 프로젝트의 권한을 확인할 수 있는 코드
 */

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include <shlobj.h>                  // for IsUserAnAdmin() function.
#pragma comment(lib, "shell32.lib")  // for IsUserAnAdmin() function.
 
BOOL GetProcessElevation(TOKEN_ELEVATION_TYPE *pElevationType, BOOL *pIsAdmin)
		/*
		*** TOKEN_ELEVATION_TYPE
		The TOKEN_ELEVATION_TYPE enumeration indicates
		the elevation type of token being queried
		by the GetTokenInformation function.

		typedef enum _TOKEN_ELEVATION_TYPE {
			TokenElevationTypeDefault = 1,	// The token does not have a linked token.
			TokenElevationTypeFull,			// The token is an elevated token.
			TokenElevationTypeLimited,		// The token is a limited token.
		} TOKEN_ELEVATION_TYPE, *PTOKEN_ELEVATION_TYPE;

		*** pIsAdmin : 현재 Admin에 해당하는지 여부
		*/
{
    HANDLE hToken = NULL;
		// 토큰 : http://www.terms.co.kr/token.htm
    BOOL bResult = FALSE;
    DWORD dwSize = 0;
 
    // 현재 프로세스의 토큰을 얻는다.
    if ( !OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) )
        return FALSE;
	/*
	*** OpenProcessToken
	opens the access token associated with a process.
	If the function succeeds, the return value is nonzero.

	BOOL WINAPI OpenProcessToken(
		_In_  HANDLE  ProcessHandle,	// The process must have the PROCESS_QUERY_INFORMATION access permission.
		_In_  DWORD   DesiredAccess,	// Specifies an access mask that specifies the requested types of access to the access token.
		_Out_ PHANDLE TokenHandle		// A pointer to a handle that identifies the newly opened access token when the function returns.
	);

	*** GetCurrentProcess
	Retrieves a pseudo handle for the current process.
	The return value is a pseudo handle to the current process.

	HANDLE WINAPI GetCurrentProcess(void);
	*/
 
    // 권한상승 형태에 대한 정보를 얻는다.
    if ( GetTokenInformation(hToken, TokenElevationType, pElevationType, sizeof(TOKEN_ELEVATION_TYPE), &dwSize) )
			/*
			*** GetTokenInformation
			The GetTokenInformation function retrieves a specified type of information about an access token.
			The calling process must have appropriate access rights to obtain the information.
			
			BOOL WINAPI GetTokenInformation(
				_In_      HANDLE                  TokenHandle,
				_In_      TOKEN_INFORMATION_CLASS TokenInformationClass,
				_Out_opt_ LPVOID                  TokenInformation,
				_In_      DWORD                   TokenInformationLength,
				_Out_     PDWORD                  ReturnLength
			);
			*/
    {
        BYTE adminSID[SECURITY_MAX_SID_SIZE];
        dwSize = sizeof(adminSID);
         
        // 관리자 그룹의 SID 값을 생성한다.
        CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, &dwSize);
		/*
		*** CreateWellKnownSid
		The CreateWellKnownSid function creates a SID for predefined aliases.

		BOOL WINAPI CreateWellKnownSid(
			_In_      WELL_KNOWN_SID_TYPE WellKnownSidType,	// Member of the WELL_KNOWN_SID_TYPE enumeration that specifies what the SID will identify.
			_In_opt_  PSID                DomainSid,	// A pointer to a SID that identifies the domain to use when creating the SID.
														// Pass NULL to use the local computer.
			_Out_opt_ PSID                pSid,		// A pointer to memory where CreateWellKnownSid will store the new SID.
			_Inout_   DWORD               *cbSid	// A pointer to a DWORD that contains the number of bytes available at pSid.
													// The CreateWellKnownSid function stores the number of bytes actually used at this location.
		);
		*/

        if ( *pElevationType == TokenElevationTypeLimited )
        {
            HANDLE hUnfilteredToken = NULL;
             
            // 연결된 토큰의 핸들을 얻는다.
            GetTokenInformation(hToken, TokenLinkedToken, (void *)&hUnfilteredToken, sizeof(HANDLE), &dwSize);
 
            // 원래의 토큰이 관리자의 SID를 포함하고 있는지 여부를 확인한다.
            if ( CheckTokenMembership(hUnfilteredToken, &adminSID, pIsAdmin) )
                bResult = TRUE;
			/*
			*** CheckTokenMembership
			The CheckTokenMembership function determines
			whether a specified security identifier (SID)
			is enabled in an access token.
			If you want to determine group membership
			for app container tokens,
			you need to use the CheckTokenMembershipEx function.

			BOOL WINAPI CheckTokenMembership(
				_In_opt_ HANDLE TokenHandle,
				_In_     PSID   SidToCheck,
				_Out_    PBOOL  IsMember
			);
			*/
             
            CloseHandle(hUnfilteredToken);
        }
        else
        {
            *pIsAdmin = IsUserAnAdmin();
			/*
			*** IsUserAnAdmin
			BOOL IsUserAnAdmin(void);
			*/
            bResult = TRUE;
        }
    }
 
    CloseHandle(hToken);
    return bResult;
}
 
int _tmain(int argc, TCHAR *argv[])
{
    TOKEN_ELEVATION_TYPE t;
		/*
		// Token elevation values describe the relative strength of a given token.
		// A full token is a token with all groups and privileges to which the principal
		// is authorized.  A limited token is one with some groups or privileges removed.
		//

		typedef enum _TOKEN_ELEVATION_TYPE {
			TokenElevationTypeDefault = 1,
			TokenElevationTypeFull,
			TokenElevationTypeLimited,
		} TOKEN_ELEVATION_TYPE, *PTOKEN_ELEVATION_TYPE;
		*/
    BOOL bAdmin = FALSE;				// Admin인지를 확인하는 플래그
    TCHAR szUser[0xFF] = {0};			// User Name (예: kosta)를 저장하기 위한 변수
    DWORD dwUser = _countof(szUser);	// User Name의 사이즈를 리턴
		/*
		*** _countof
		정적으로 할당된 배열의 요소 수를 계산한다.

		#define _countof(_Array) (sizeof(*__countof_helper(_Array)) + 0)
		*/

    GetUserName(szUser, &dwUser);
		/*
		*** GetUserName
		Retrieves the name of the user associated with the current thread.
		If the function succeeds,
		the return value is a nonzero value, and the variable pointed to by lpnSize contains the number of TCHARs copied to the buffer specified by lpBuffer,
		including the terminating null character.

		WINADVAPI
		BOOL WINAPI GetUserNameW (
			__out_ecount_part_opt(*pcbBuffer, *pcbBuffer) LPWSTR lpBuffer,
			__inout LPDWORD pcbBuffer
			);
		*/
 
    _tsetlocale(LC_ALL, _T("korean"));
	/*
	*** _testlocale
	https://msdn.microsoft.com/ko-kr/library/x99tb11d.aspx
	런타임 로캘을 설정하거나 가져옵니다.
	TCHAR.H 루틴으로 설정되어 있다.
	- LC_ALL : 다음 목록에 모든 범주가 나열된다.

	// Setlocale functions
	#define _tsetlocale _wsetlocale
	wchar_t *_wsetlocale(
		int category,			// 로캘의 영향을 받는 범주이다.
		const wchar_t *locale	// 로캘 지정자이다.
	);

	*** loclae : 사용자의 언어, 국가 뿐 아니라 사용자 인터페이스에서
			사용자가 선호하ㅡㄴ 사항을 지정한 매개변수의 모임이다.
			https://ko.wikipedia.org/wiki/%EB%A1%9C%EC%BC%80%EC%9D%BC
	*/
 
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
            _tprintf(_T("권한상승되었음\n"));
            break;
        case TokenElevationTypeLimited:
            _tprintf(_T("필터된 토큰에 의해 제한된 권한으로 수행되었음.\n"));
            break;
        default:
            break;
        }
    }
 
    return 0;
}