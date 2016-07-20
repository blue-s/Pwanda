#include "mon.h"

BOOL regDel(TCHAR * reg){

	HKEY	hKey;
	LONG	ret;

	ret = RegOpenKeyEx(
		HKEY_CURRENT_USER, 
		_T("SOFTWARE\\\Microsoft\\Windows\\CurrentVersion\\Run"), 
		0,
		KEY_ALL_ACCESS,
		&hKey
		);

	if (ret != ERROR_SUCCESS)
	{
		_tprintf(_T("Registry Open Failed!! \n"));
		return FALSE;
	}
	

	ret = RegDeleteValue(hKey, reg); //삭제할 레지스트리명

	if (ret != ERROR_SUCCESS)
	{
		Output_Console(FOREGROUND_RED, _T("*** Registry DELETE FAIL !!! ***\n\n"));
		return FALSE;
	}
	else
		Output_Console(FOREGROUND_BLUE, _T("*** Registry DELETE SUCCESS !!! ***\n\n"));

	RegCloseKey(hKey);
	printf("Registry Close !! \n");
	return TRUE;
}