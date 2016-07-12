/* main ���ο� �ִ� �ҽ��� ������ */
/*

#include <sddl.h>

	// [4] ���� ��� ********************************
	printf("���� ��� ����\n");
	// �ش� ���μ����� PID�� ���� �ڵ��� ����
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);

	// Process Handle�� NULL�̸�, Open Failed ����
	if (hProcess == NULL)
	{
		return 1;
	}

	// ���μ����� ������ SID�� ã�´�.
	// ���� ���α׷��� Local System �������� �α����Ͽ����Ƿ�, HKEY_CURRENT_USER�� ������Ʈ����
	// Open�� ��� App�� �ٸ� ������Ʈ�� Ű�� ������ �ȴ�.
	// �׷��Ƿ�, Process Token����, SID�� Ȯ���ؼ� ������Ʈ���� ���� ã�� ���� �Ѵ�.

	HANDLE hTProcess;
	DWORD dwSize;

	// Process �ڵ�� ��ū�� ��´�.
	if (OpenProcessToken(hProcess, TOKEN_QUERY, &hTProcess) == FALSE)
	{
		CloseHandle(hProcess);
		return 1;
	}

	// NULL ������ ��ū ������ ��û�Ͽ�, ��ū ������ ����� ������ ũ�⸦ �޾ƿ´�.
	GetTokenInformation(hTProcess, TokenUser, NULL, 0, &dwSize);

	TOKEN_USER *sidUser = (TOKEN_USER *)malloc(dwSize);
	if (GetTokenInformation(hTProcess, TokenUser, sidUser, dwSize, &dwSize) == 0)
	{
		free(sidUser);
		CloseHandle(hTProcess);
		CloseHandle(hProcess);
		return 1;
	}

	// SID�� ���ڿ��� �ƴϰ� ����ü���̴�.
	// ������ SID�� �����Ѵ�.
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