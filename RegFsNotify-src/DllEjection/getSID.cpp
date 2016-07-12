/* main 내부에 있는 소스만 가져옴 */
/*

#include <sddl.h>

	// [4] 권한 상승 ********************************
	printf("권한 상승 시작\n");
	// 해당 프로세스의 PID를 통해 핸들을 얻어옴
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);

	// Process Handle이 NULL이면, Open Failed 에러
	if (hProcess == NULL)
	{
		return 1;
	}

	// 프로세서의 실행자 SID를 찾는다.
	// 서비스 프로그램은 Local System 계정으로 로그인하였으므로, HKEY_CURRENT_USER로 레지스트리를
	// Open할 경우 App와 다른 레지스트리 키로 접근이 된다.
	// 그러므로, Process Token에서, SID를 확보해서 레지스트리를 직접 찾아 가야 한다.

	HANDLE hTProcess;
	DWORD dwSize;

	// Process 핸들로 토큰을 얻는다.
	if (OpenProcessToken(hProcess, TOKEN_QUERY, &hTProcess) == FALSE)
	{
		CloseHandle(hProcess);
		return 1;
	}

	// NULL 값으로 토큰 정보를 요청하여, 토큰 정보가 저장될 버퍼의 크기를 받아온다.
	GetTokenInformation(hTProcess, TokenUser, NULL, 0, &dwSize);

	TOKEN_USER *sidUser = (TOKEN_USER *)malloc(dwSize);
	if (GetTokenInformation(hTProcess, TokenUser, sidUser, dwSize, &dwSize) == 0)
	{
		free(sidUser);
		CloseHandle(hTProcess);
		CloseHandle(hProcess);
		return 1;
	}

	// SID는 문자열이 아니고 구조체형이다.
	// 문자형 SID로 변경한다.
	LPTSTR pszSid;
	ConvertSidToStringSid(sidUser[0].User.Sid, &pszSid);
		
		// *** ConvertSidToStringSid
		// The ConvertSidToStringSid function converts a security identifier (SID) to a string format suitable for display, storage, or transmission.
		// To convert the string-format SID back to a valid, functional SID, call the ConvertStringSidToSid function.

		// BOOL ConvertSidToStringSid(
		// 	_In_  PSID   Sid,
		// 	_Out_ LPTSTR *StringSid
		// );
		
	printf("%S\n", pszSid);

	free(sidUser);
	CloseHandle(hTProcess);
	LocalFree(pszSid);
	*/