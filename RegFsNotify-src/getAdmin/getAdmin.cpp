///*
// * System 권한으로 상승시키기 위함
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
// //// Process ID를 Process Handle로 변경한다.
// //DWORD dwPid = GetPIDFromShortFileName("explorer.exe");
// //HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ  , FALSE, dwPid);
// //
// //// Process Handle이 NULL이면, Open Failed 에러
// //if (hProcess == NULL)
// //{
// // return;
// //}
// //
// //// 프로세서의 실행자 SID를 찾는다.
// //// 서비스 프로그램은 Local System 계정으로 로그인하였으므로, HKEY_CURRENT_USER로 레지스트리를
// //// Open할 경우 App와 다른 레지스트리 키로 접근이 된다.
// //// 그러므로, Process Token에서, SID를 확보해서 레지스트리를 직접 찾아 가야 한다.
// //
// //HANDLE hTProcess;
// //DWORD dwSize;
// //
// //// Process 핸들로 토큰을 얻는다. (Vista이므로, 될수 있으면 권한은 낮게... QUERY 권한만으로 접근)
// //if (OpenProcessToken(hProcess, TOKEN_QUERY, &hTProcess) == FALSE)
// //{
// // CloseHandle(hProcess);
// // return;
// //}
// //
// //// NULL 값으로 토큰 정보를 요청하여,  토큰 정보가 저장될 버퍼의 크기를 받아온다.
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
// //// SID는 문자열이 아니고 구조체형이다.
// //// 문자형 SID로 변경한다.
// //LPTSTR pszSid;
// //ConvertSidToStringSid(sidUser[0].User.Sid, &pszSid);
// //free(sidUser);
// //CloseHandle(hTProcess);
// //
// // 
// //CString strUserSID = pszSid;
// //LocalFree(pszSid);
// //
// //// 레지스트리 오픈
// //strUserSID += L"\\Software\\microsoft\\windows\\currentversion\\run";
// //if( RegOpenKeyEx( HKEY_USERS, strUserSID, 0, KEY_READ, &hTestKey) == ERROR_SUCCESS )
// //{
// //       //여기서 부턴 그냥 원래 하던데로 레지값 읽으면 됨
// //}