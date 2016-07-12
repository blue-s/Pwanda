/*
# Copyright (C) 2010 Michael Ligh
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mon.h"

// maximum number of drives to monitor 
// 모니터링할 최대 드라이브 수 
#define MAX_DRIVES 24

// global variables for change notifications
// 변경 알림 
HANDLE  g_ChangeHandles[MAX_DRIVES]; // 변경된 핸들 ?
HANDLE  g_DirHandles[MAX_DRIVES]; // 핸들 디렉토리 ? 
LPTSTR  g_szDrives[MAX_DRIVES];// 드라이브 사이즈
DWORD   g_idx = 0;

void ProcessChange(int idx)
{
	BYTE buf[32 * 1024]; // 버퍼 수 
	DWORD cb = 0;
	PFILE_NOTIFY_INFORMATION pNotify;
	// PFILE_NOTIFY_INFORMATION: 한 개의 파일 변경에 대한 정보를 담을 수 있는 구조체
	int offset = 0;
	TCHAR szFile[MAX_PATH*2];

	memset(buf, 0, sizeof(buf));
	// 버퍼 초기화 

	// find out what type of change triggered the notification
	// 어떤 타입의 변경이 알림을 일으켰는지 찾아낸다
	if (ReadDirectoryChangesW(g_DirHandles[idx], buf, 
		sizeof(buf), TRUE, 
		FILE_CHANGE_FLAGS, &cb, NULL, NULL))
		// mon.h 에서 선언했던 파일 변화 플래그들 
		// ReadDirectoryChanges - 디렉토리에 대한 변경 발생 --> 이벤트와 경로를 큐에 저장 
	{
		// parse the array of file information structs
		// 파일 구조체 배열을 파싱 --> 파일 정보 구조체의 배열 구문을 분석 
		do {
			pNotify = (PFILE_NOTIFY_INFORMATION) &buf[offset];
			offset += pNotify->NextEntryOffset;

			memset(szFile, 0, sizeof(szFile));

			memcpy(szFile, pNotify->FileName, 
				pNotify->FileNameLength);
			// ? 

			// if the file is whitelisted, go to the next
			if (IsWhitelisted(szFile)) {
				continue;
			}

			//결과 버퍼에 저장된 구조체(FILE_NOTIFY_INFORMATION)에서 
			//Action에 대한 내용 확인하여 해당 액션에 따른 콘솔창 내용 출력
			/*
			switch (pNotify->Action) // --> pNotify 구조체 안에 Action이 있는거겠지 ?
			{
			case FILE_ACTION_ADDED:
				Output(FOREGROUND_GREEN, 
					_T("[ADDED] %s%s\n"), g_szDrives[idx], szFile);
				break;
			case FILE_ACTION_REMOVED: 
				Output(FOREGROUND_RED, 
					_T("[REMOVED] %s%s\n"), g_szDrives[idx], szFile);
				break;
			case FILE_ACTION_MODIFIED: 
				Output(0, _T("[MODIFIED] %s%s\n"), 
					g_szDrives[idx], szFile);
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				Output(0, _T("[RENAMED (OLD)] %s%s"), 
					g_szDrives[idx], szFile);
				break; 
			case FILE_ACTION_RENAMED_NEW_NAME:
				Output(0,_T("[RENAMED (NEW)] %s%s"), 
					g_szDrives[idx], szFile);
				break;
			default:
				Output(0,_T("[??] %s%s\n"), 
					g_szDrives[idx], szFile);
				break;
			};
			*/
		} while (pNotify->NextEntryOffset != 0);
	}
}

void StartFileMonitor(void)
{
	DWORD dwWaitStatus;
	BOOL  bOK = FALSE;
	TCHAR   pszList[1024]; // ? 프로세스 리스트 
	DWORD   ddType; // ?
	LPTSTR  pStart = NULL;
	HANDLE  hChange, hDir;



	Output(FOREGROUND_RED, _T("-----------------File.cpp--------------------[resultBuffer] %s\n"), resultBuffer);


	// get a list of logical drives
	memset(pszList, 0, sizeof(pszList));
	GetLogicalDriveStrings(sizeof(pszList), pszList);
	// GetLogicalDriveStrings : 컴퓨터 디스크의 드라이브를 문자열로 받아옴 

	// parse the list of null-terminated drive strings
	// null로 끝나는 드라이브 문자열 목록 파싱 ?
	pStart = pszList; // 시작 배열 오프셋을 저장
	while(_tcslen(pStart)) 
	{
		ddType = GetDriveType(pStart); //첫 번째 드라이버 종류를 받아옴 ?

		// only monitor local and removable (i.e. USB) drives
		//모니터링하는 드라이브 조건 지정 : 로컬 및 이동식 드라이브
		//드라이브의 종류가 하드디스크 또는 이동식 드라이버 또는 A:\\ 중 
		//하나라도 참이면 다음 단계를 수행
		if ((ddType == DRIVE_FIXED || ddType == DRIVE_REMOVABLE) && 
			_tcscmp(pStart, _T("A:\\")) != 0)
		{
			hChange = FindFirstChangeNotification(pStart,
									TRUE, /* watch subtree */
								    FILE_CHANGE_FLAGS);
			// 드디어 FindFirstChangeNotification이 나왔네

			if (hChange == INVALID_HANDLE_VALUE) // hChange 핸들 ?
				continue;

			// 지정 디렉토리의 핸들값을 취득한다(파일도 마찬가지)
			hDir = CreateFile(pStart, 
				FILE_LIST_DIRECTORY, 
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL, 
				OPEN_EXISTING, 
				FILE_FLAG_BACKUP_SEMANTICS, /* opens a directory */
				NULL);

			if (hDir == INVALID_HANDLE_VALUE) {
				FindCloseChangeNotification(hChange);
				// 핸들 닫기 ? 
				continue;
			}

			_tprintf(_T("Monitoring %s\n"), pStart);

			// save the handles and drive letter  
			//핸들 및 드라이브 문자를 저장
			g_szDrives[g_idx]      = _tcsdup(pStart);
			g_DirHandles[g_idx]    = hDir;
			g_ChangeHandles[g_idx] = hChange;
			g_idx++;
		}

		pStart += wcslen(pStart) + 1;
	}
 
	// wait for a notification to occur
	// WatiForSingleObject도 나왔네 
	// 대기하는 모든 객체가 신호 상태에 놓일 때까지 기다림 
	while(WaitForSingleObject(g_hStopEvent, 1) != WAIT_OBJECT_0) 
	{
		dwWaitStatus = WaitForMultipleObjects(
			g_idx, 
			g_ChangeHandles, 
			FALSE, INFINITE); 

		bOK = FALSE;

		// if the wait suceeded, for which handle did it succeed?
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

		// the wait failed or timed out
		if (!bOK) break;
	}
	_tprintf(_T("Got stop event...\n"));

	for(int i=0; i < g_idx; i++)
	{
		_tprintf(_T("Closing handle for %s\n"), g_szDrives[i]);
		CloseHandle(g_DirHandles[i]); // DirHandles도 Close
		FindCloseChangeNotification(g_ChangeHandles[i]);
	}

}
