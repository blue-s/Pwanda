#include "mon.h"
#include <stdio.h>
#include <string.h>

#define MAX_DRIVES 24
#define MAX_VALUE_NAME 16383

// 변수 선언
HANDLE  g_ChangeHandles[MAX_DRIVES];
HANDLE  g_DirHandles[MAX_DRIVES];
LPTSTR  g_szDrives[MAX_DRIVES];
DWORD   g_idx = 0;


TCHAR * Rname;					//확장자 판단하기 위해 원본 파일명을 복사해 담아두는 변수
TCHAR * file_name;
TCHAR * roaming_file_name;		//로밍부분 파싱해서 얻은 파일 명
TCHAR * prefetch_file_name;		//프리패치에서 파싱해서 얻은 파일 명

TCHAR * findRoam =_T("Roaming");
TCHAR * findPre =_T("Prefetch");
const TCHAR * checkPoint = _T(".");
const TCHAR * checkText = _T(".txt");


void ProcessChange(int idx)
{
	BYTE buf[MAX_VALUE_NAME];
	DWORD cb = 0;
	PFILE_NOTIFY_INFORMATION pNotify;
	int offset = 0;
	TCHAR szFile[MAX_PATH*2];
	int flag = -1;		// 화이트 리스트에 속하는지 여부 확인 플래그

	memset(buf, 0, sizeof(buf));

	if (ReadDirectoryChangesW(g_DirHandles[idx], buf, sizeof(buf), TRUE, FILE_CHANGE_FLAGS, &cb, NULL, NULL))
	{
		do {
			pNotify = (PFILE_NOTIFY_INFORMATION) &buf[offset];
			offset += pNotify->NextEntryOffset;

			memset(szFile, 0, sizeof(szFile));
			memcpy(szFile, pNotify->FileName, pNotify->FileNameLength);

			// 플래그에 따른 파일명 추출 및 비교
			flag = Whitelisted(szFile);
			if(flag == 1)
			{
				switch (pNotify->Action)
				{
				case FILE_ACTION_ADDED:
					roaming_file_name = _tcsstr(szFile, findRoam)+8;

					//원래 파일명 보존을 위해 변수에 복사
					Rname = _tcsstr(roaming_file_name, checkPoint); //(abc.txt, .)

					//확장자만 추출
					_tprintf(_T("+++++++++++++++++++ %s ++++++++++++++++++++[File Name] \n"), Rname);

					//확장자가 txt가 맞다면 배열에 넣었었던 파일명을 출력 
					if(_tcsicmp(Rname, checkText) == 0)
					{
						Output_Console(FOREGROUND_RED, _T("ROAMING [%d] ------> %s \n"), 0, roaming_file_name);
						roamingBuffer = roaming_file_name;

						Output_Console(FOREGROUND_BLUE, _T("roamingBuffer [%d] ------> %s \n"), 0, roamingBuffer);

						ExtractProcess(2, roamingBuffer);
						_tprintf(_T("-------------------------------[SUCCESS]--------------------------------- \n"));
					}
					else
					{
						_tprintf(_T("--------------------------------[NOT]----------------------------------- \n"));
					}
					Output_Console(FOREGROUND_GREEN, _T("[ADDED] %s%s \n"), g_szDrives[idx], szFile);


					break;
				default:
					break;
				}; 
			}
			else if(flag == 2)
			{
				switch (pNotify->Action)
				{
				case FILE_ACTION_ADDED:
					prefetch_file_name = _tcsstr(szFile, findPre)+9;
					Output_Console(FOREGROUND_BLUE, _T("PREFETCH [%d] ------> %s \n"), 0, prefetch_file_name);
					prefetchBuffer = prefetch_file_name;
					ExtractProcess(3, prefetchBuffer);
					break;
				default:
					break;
				}; 
			}
			else {
				continue;
			}
		} while (pNotify->NextEntryOffset != 0);
	}
}    

void StartFileMonitor(void)
{
	DWORD dwWaitStatus;
	BOOL  bOK = FALSE;
	TCHAR   pszList[1024];
	DWORD   ddType;
	LPTSTR  pStart = NULL;
	HANDLE  hChange, hDir;

	memset(pszList, 0, sizeof(pszList));
	GetLogicalDriveStrings(sizeof(pszList), pszList);

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
