/*
 * ���� ������Ʈ�� ������ Ȯ���� �� �ִ� �ڵ�
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

		*** pIsAdmin : ���� Admin�� �ش��ϴ��� ����
		*/
{
    HANDLE hToken = NULL;
		// ��ū : http://www.terms.co.kr/token.htm
    BOOL bResult = FALSE;
    DWORD dwSize = 0;
 
    // ���� ���μ����� ��ū�� ��´�.
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
 
    // ���ѻ�� ���¿� ���� ������ ��´�.
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
         
        // ������ �׷��� SID ���� �����Ѵ�.
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
             
            // ����� ��ū�� �ڵ��� ��´�.
            GetTokenInformation(hToken, TokenLinkedToken, (void *)&hUnfilteredToken, sizeof(HANDLE), &dwSize);
 
            // ������ ��ū�� �������� SID�� �����ϰ� �ִ��� ���θ� Ȯ���Ѵ�.
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
    BOOL bAdmin = FALSE;				// Admin������ Ȯ���ϴ� �÷���
    TCHAR szUser[0xFF] = {0};			// User Name (��: kosta)�� �����ϱ� ���� ����
    DWORD dwUser = _countof(szUser);	// User Name�� ����� ����
		/*
		*** _countof
		�������� �Ҵ�� �迭�� ��� ���� ����Ѵ�.

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
	��Ÿ�� ��Ķ�� �����ϰų� �����ɴϴ�.
	TCHAR.H ��ƾ���� �����Ǿ� �ִ�.
	- LC_ALL : ���� ��Ͽ� ��� ���ְ� �����ȴ�.

	// Setlocale functions
	#define _tsetlocale _wsetlocale
	wchar_t *_wsetlocale(
		int category,			// ��Ķ�� ������ �޴� �����̴�.
		const wchar_t *locale	// ��Ķ �������̴�.
	);

	*** loclae : ������� ���, ���� �� �ƴ϶� ����� �������̽�����
			����ڰ� ��ȣ�ϤѤ� ������ ������ �Ű������� �����̴�.
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
            _tprintf(_T("�⺻ ������̰ų�, UAC ����� OFF �� ������.\n"));
            break;
        case TokenElevationTypeFull:
            _tprintf(_T("���ѻ�µǾ���\n"));
            break;
        case TokenElevationTypeLimited:
            _tprintf(_T("���͵� ��ū�� ���� ���ѵ� �������� ����Ǿ���.\n"));
            break;
        default:
            break;
        }
    }
 
    return 0;
}