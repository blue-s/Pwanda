///*
// * System �������� ��½�Ű�� ����
// */
//
//#include <stdio.h>
//#include <windows.h>
//#include <tchar.h>
//#include <locale.h>
//#include <shlobj.h>
//
//#include <tlhelp32.h>
//#include <iostream>	
//#include <string>
//#include "psapi.h"
//
//
//// ******************** test1
////DWORD GetProcessByFileName(char* name){
////    DWORD process_id_array[1024];
////    DWORD bytes_returned;
////    DWORD num_processes;
////    HANDLE hProcess;
////
////    char image_name[256];
////    char buffer[256];
////	int i;
////    DWORD exitcode;
////    
////	EnumProcesses(process_id_array, 256*sizeof(DWORD), &bytes_returned);
////    num_processes = (bytes_returned/sizeof(DWORD));
////    
////	for (i = 0; i < num_processes; i++) {
////        hProcess=OpenProcess(PROCESS_ALL_ACCESS,TRUE,process_id_array[i]);
////        if(GetModuleBaseName(hProcess,0,(LPTSTR)image_name,256)){
////            if(!stricmp((char*)image_name,name)){
////                CloseHandle(hProcess);
////                return process_id_array[i];
////            }
////        }
////        CloseHandle(hProcess);
////    }
////    return 0;
////}
////
////
////void __cdecl main(int argc, char *argv[])
////{
////	DWORD dwPID;
////	dwPID = GetProcessByFileName("calc.exe");
////	printf("%lu", (unsigned long)dwPID);
////	return;
////}
//
//
//
//// ******************** test2
// //// Process ID�� Process Handle�� �����Ѵ�.
// //DWORD dwPid = GetPIDFromShortFileName("explorer.exe");
// //HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ  , FALSE, dwPid);
// //
// //// Process Handle�� NULL�̸�, Open Failed ����
// //if (hProcess == NULL)
// //{
// // return;
// //}
// //
// //// ���μ����� ������ SID�� ã�´�.
// //// ���� ���α׷��� Local System �������� �α����Ͽ����Ƿ�, HKEY_CURRENT_USER�� ������Ʈ����
// //// Open�� ��� App�� �ٸ� ������Ʈ�� Ű�� ������ �ȴ�.
// //// �׷��Ƿ�, Process Token����, SID�� Ȯ���ؼ� ������Ʈ���� ���� ã�� ���� �Ѵ�.
// //
// //HANDLE hTProcess;
// //DWORD dwSize;
// //
// //// Process �ڵ�� ��ū�� ��´�. (Vista�̹Ƿ�, �ɼ� ������ ������ ����... QUERY ���Ѹ����� ����)
// //if (OpenProcessToken(hProcess, TOKEN_QUERY, &hTProcess) == FALSE)
// //{
// // CloseHandle(hProcess);
// // return;
// //}
// //
// //// NULL ������ ��ū ������ ��û�Ͽ�,  ��ū ������ ����� ������ ũ�⸦ �޾ƿ´�.
// //GetTokenInformation(hTProcess, TokenUser, NULL, 0, &dwSize);
// //
// //TOKEN_USER *sidUser = (TOKEN_USER *)malloc(dwSize);
// //if (GetTokenInformation(hTProcess, TokenUser, sidUser, dwSize, &dwSize) == 0)
// //{
// //
// // free(sidUser);
// // CloseHandle(hTProcess);
// // CloseHandle(hProcess);
// //
// // return;
// //}
// //
// //// SID�� ���ڿ��� �ƴϰ� ����ü���̴�.
// //// ������ SID�� �����Ѵ�.
// //LPTSTR pszSid;
// //ConvertSidToStringSid(sidUser[0].User.Sid, &pszSid);
// //free(sidUser);
// //CloseHandle(hTProcess);
// //
// // 
// //CString strUserSID = pszSid;
// //LocalFree(pszSid);
// //
// //// ������Ʈ�� ����
// //strUserSID += L"\\Software\\microsoft\\windows\\currentversion\\run";
// //if( RegOpenKeyEx( HKEY_USERS, strUserSID, 0, KEY_READ, &hTestKey) == ERROR_SUCCESS )
// //{
// //       //���⼭ ���� �׳� ���� �ϴ����� ������ ������ ��
// //}