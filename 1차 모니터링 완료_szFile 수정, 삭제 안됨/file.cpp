#include "mon.h"

// 변수 선언
HANDLE  g_ChangeHandles[MAX_DRIVES];
HANDLE  g_DirHandles[MAX_DRIVES];
LPTSTR  g_szDrives[MAX_DRIVES];
DWORD   g_idx = 0;

TCHAR * file_name;				// 파일명
TCHAR * buffer_file_name;		// 파일명 저장하는 임시 변수
TCHAR * buffer_extension;		// 확장자 저장하는 임시 변수	

const TCHAR * checkRoam =_T("Roaming");
const TCHAR * findPre =_T("Prefetch");
const TCHAR * checkPoint = _T(".");
const TCHAR * checkEXE = _T(".exe");
const TCHAR * checkHASH = _T("-");


void ProcessChange(int idx)
{
	BYTE buf[MAX_VALUE_NAME];
	TCHAR szFile[MAX_PATH*2];
	PFILE_NOTIFY_INFORMATION pNotify;
	DWORD cb = 0;
	int offset = 0;
	int flag = -1;		// 화이트 리스트에 속하는지 여부 확인 플래그

	memset(buf, 0, sizeof(buf));

	if (ReadDirectoryChangesW(g_DirHandles[idx], buf, sizeof(buf), TRUE, FILE_CHANGE_FLAGS, &cb, NULL, NULL))
	{
		do {
			pNotify = (PFILE_NOTIFY_INFORMATION) &buf[offset];
			offset += pNotify->NextEntryOffset;

			memset(szFile, 0, sizeof(szFile));
			memcpy(szFile, pNotify->FileName, pNotify->FileNameLength);
			
			// szFile 복사
			_tcsncpy(szFileBuffer, szFile, 100);

			// 플래그에 따른 파일명 추출 및 비교
			// 1: Roaming, 2: Prefetch, -1: Nothing
			flag = Whitelisted(szFile);

			// 화이트리스트에 속하지 않는 경우
			if(flag < 0){	continue; }			
			
			// 화이트리스트에 속하는경우
			switch (pNotify->Action)
			{
			case FILE_ACTION_ADDED:
				// Roaming에 해당하는 경우, 파일 명만 추출
				if(flag == 1)
				{
					buffer_file_name = _tcsstr(szFile, checkRoam) + 8;
					buffer_extension = _tcsstr(buffer_file_name, checkPoint);

					//printf("[파일 확장자: %S] ", buffer_extension);

					//확장자가 실행파일이 아닌 경우 
					if(_tcsicmp(buffer_extension, checkEXE) != 0)
					{
						//printf("실행 파일이 아닙니다.\n\n", buffer_extension);
						continue;
					}
				}
				else if(flag == 2)
				{
					buffer_file_name = _tcsstr(szFile, findPre) + 9;
					_tcstok(buffer_file_name, checkHASH);
				}

				ExtractProcess(flag, buffer_file_name, szFileBuffer);					
				Output_Console(FOREGROUND_GREEN, _T("[ADDED] %s%s \n"), g_szDrives[idx], szFile);
				_tprintf(_T("-------------------------------[SUCCESS]--------------------------------- \n\n"));
				break;

			default:
				break;
			}

			// 모든 활동이 끝난 뒤 flag 리셋
			flag = -1;
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

		if ((ddType == DRIVE_FIXED || ddType == DRIVE_REMOVABLE) && _tcscmp(pStart, _T("A:\\")) != 0)
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
		dwWaitStatus = WaitForMultipleObjects( g_idx, g_ChangeHandles, FALSE, INFINITE); 

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
