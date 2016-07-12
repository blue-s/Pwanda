#include "mon.h"
#include <stdio.h>

#define MAX_DRIVES 24
#define MAX_VALUE_NAME 16383

HANDLE  g_ChangeHandles[MAX_DRIVES];
HANDLE  g_DirHandles[MAX_DRIVES];
LPTSTR  g_szDrives[MAX_DRIVES];
DWORD   g_idx = 0;

//변경한 부분
TCHAR * roaming_file_name=_T(""); //로밍부분 파싱해서 얻은 파일 명
TCHAR * prefetch_file_name=_T(""); //프리패치에서 파싱해서 얻은 파일 명

TCHAR s1[MAX_VALUE_NAME]=_T("Roaming");
TCHAR s2[MAX_VALUE_NAME]=_T("Prefetch");

DWORD Ridx=0; //로밍부분 인덱스 초기화
DWORD Pidx=0; //프리패치 인덱스 초기화

void parsing(DWORD flag, TCHAR * filename);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ProcessChange(int idx)
{
	BYTE buf[32 * 1024];
	DWORD cb = 0;
	PFILE_NOTIFY_INFORMATION pNotify;
	int offset = 0;
	TCHAR szFile[MAX_PATH*2];
	memset(buf, 0, sizeof(buf));

	if (ReadDirectoryChangesW(g_DirHandles[idx], buf, 
		sizeof(buf), TRUE, 
		FILE_CHANGE_FLAGS, &cb, NULL, NULL))
	{

		do {

			pNotify = (PFILE_NOTIFY_INFORMATION) &buf[offset];
			offset += pNotify->NextEntryOffset;

			memset(szFile, 0, sizeof(szFile));

			memcpy(szFile, pNotify->FileName, 
				pNotify->FileNameLength);

			_tprintf(_T("--------- %s --------- \n"), szFile);
			_tprintf(_T("---------------------------------------------------- \n"));

			//	szfile = C:\Windows\Prefetch\dfsfasfafsfas.txt
			//	s1 = Prefetch
			//	prefetch_file_name -> Prefetch\dfsfasfafsfas.txt
			//	prefetch_file_name + 9
			//	dfsfasfafsfas.txt
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			
			if (RoamingWhitelisted(szFile)) { 
				
				roaming_file_name = _tcsstr(szFile, s1);
				Output_Roaming(FOREGROUND_BLUE, _T("[ROAMING] ------> %s \n"), roaming_file_name+8);


				switch (pNotify->Action)
				{
				case FILE_ACTION_ADDED:
					Output_Roaming(FOREGROUND_GREEN, _T("[ADDED] %s%s \n"), g_szDrives[idx], szFile);
					break;
				case FILE_ACTION_REMOVED: 
					Output_Roaming(FOREGROUND_RED, _T("\n [REMOVED] %s%s \n"), g_szDrives[idx], szFile);
					break;
				case FILE_ACTION_MODIFIED: 
					Output_Roaming(0, _T("[MODIFIED] %s%s \n"),g_szDrives[idx], szFile);
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					Output_Roaming(0, _T("\n[RENAMED (OLD)] %s%s \n"), g_szDrives[idx], szFile);
					break; 
				case FILE_ACTION_RENAMED_NEW_NAME:
					Output_Roaming(0, _T("\n[RENAMED (NEW)] %s%s \n"), g_szDrives[idx], szFile);
					break;
				default:
					Output_Roaming(0,_T("[??] %s%s \n"), 
						g_szDrives[idx], szFile);
					break;
				}; 
			}else if(PrefetchWhitelisted(szFile)){
			
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				//파일경로를 (char)로 변환시킴

				prefetch_file_name = _tcsstr(szFile, s2);
				Output_Prefetch(FOREGROUND_BLUE, _T("[PRFETCH] ------> %s \n"), prefetch_file_name+9);

				switch (pNotify->Action)
				{
				case FILE_ACTION_ADDED:
					Output_Prefetch(FOREGROUND_GREEN, _T("\n[ADDED] %s%s\n"), g_szDrives[idx], szFile);
					break;
				case FILE_ACTION_REMOVED: 
					Output_Prefetch(FOREGROUND_RED, _T("\n[REMOVED] %s%s\n"), g_szDrives[idx], szFile);
					break;
				case FILE_ACTION_MODIFIED: 
					Output_Prefetch(0, _T("[MODIFIED] %s%s \n"), g_szDrives[idx], szFile);
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					Output_Prefetch(0, _T("[RENAMED (OLD)] %s%s \n"), g_szDrives[idx], szFile);
					break; 
				case FILE_ACTION_RENAMED_NEW_NAME:
					Output_Prefetch(0,_T("\n[RENAMED (NEW)] %s%s \n"), g_szDrives[idx], szFile);
					break;
				default:
					Output_Prefetch(0,_T("[??] %s%s \n"), g_szDrives[idx], szFile);
					break;
				}; 
			}else{
				continue;
			}	
		} while (pNotify->NextEntryOffset != 0);
	}
}    

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StartFileMonitor(void)
{
	DWORD dwWaitStatus;
	BOOL  bOK = FALSE;
	TCHAR   pszList[1024];
	DWORD   ddType;
	LPTSTR  pStart = NULL;
	HANDLE  hChange, hDir;

	// get a list of logical drives
	memset(pszList, 0, sizeof(pszList));
	GetLogicalDriveStrings(sizeof(pszList), pszList);

	// parse the list of null-terminated drive strings
	pStart = pszList;
	while(_tcslen(pStart)) 
	{
		ddType = GetDriveType(pStart);

		if ((ddType == DRIVE_FIXED || ddType == DRIVE_REMOVABLE) && 
			_tcscmp(pStart, _T("A:\\")) != 0)
		{
			hChange = FindFirstChangeNotification(pStart,
				TRUE,
				FILE_CHANGE_FLAGS);

			if (hChange == INVALID_HANDLE_VALUE)
				continue;

			hDir = CreateFile(pStart, 
				FILE_LIST_DIRECTORY, 
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL, 
				OPEN_EXISTING, 
				FILE_FLAG_BACKUP_SEMANTICS,
				NULL);

			if (hDir == INVALID_HANDLE_VALUE) {
				FindCloseChangeNotification(hChange);
				continue;
			}

			_tprintf(_T("Monitoring %s\n"), pStart);

			g_szDrives[g_idx]      = _tcsdup(pStart);
			g_DirHandles[g_idx]    = hDir;
			g_ChangeHandles[g_idx] = hChange;
			g_idx++;
		}

		pStart += wcslen(pStart) + 1;
	}

	while(WaitForSingleObject(g_hStopEvent, 1) != WAIT_OBJECT_0) 
	{
		dwWaitStatus = WaitForMultipleObjects(
			g_idx, 
			g_ChangeHandles, 
			FALSE, INFINITE); 

		bOK = FALSE;

		for(int i=0; i < g_idx; i++)
		{
			if (dwWaitStatus == WAIT_OBJECT_0 + i) 
			{
				bOK = TRUE;
				ProcessChange(i);
				if (!FindNextChangeNotification(g_ChangeHandles[i])) 
				{
					SetEvent(g_hStopEvent);
					break;
				}
				break;
			}
		}

		if (!bOK) break;
	}
	_tprintf(_T("Got stop event...\n"));

	for(int i=0; i < g_idx; i++)
	{
		_tprintf(_T("Closing handle for %s\n"), g_szDrives[i]);
		CloseHandle(g_DirHandles[i]);
		FindCloseChangeNotification(g_ChangeHandles[i]);
	}
}
