#include "mon.h"
#include <stdio.h>
#include <string.h>

#define MAX_DRIVES 24
#define MAX_VALUE_NAME 16383

// ���� ����
HANDLE  g_ChangeHandles[MAX_DRIVES];
HANDLE  g_DirHandles[MAX_DRIVES];
LPTSTR  g_szDrives[MAX_DRIVES];
DWORD   g_idx = 0;


TCHAR * Rname;					//Ȯ���� �Ǵ��ϱ� ���� ���� ���ϸ��� ������ ��Ƶδ� ����
TCHAR * file_name;
TCHAR * roaming_file_name;		//�ιֺκ� �Ľ��ؼ� ���� ���� ��
TCHAR * prefetch_file_name;		//������ġ���� �Ľ��ؼ� ���� ���� ��

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
	int flag = -1;		// ȭ��Ʈ ����Ʈ�� ���ϴ��� ���� Ȯ�� �÷���

	memset(buf, 0, sizeof(buf));

	if (ReadDirectoryChangesW(g_DirHandles[idx], buf, sizeof(buf), TRUE, FILE_CHANGE_FLAGS, &cb, NULL, NULL))
	{
		do {
			pNotify = (PFILE_NOTIFY_INFORMATION) &buf[offset];
			offset += pNotify->NextEntryOffset;

			memset(szFile, 0, sizeof(szFile));
			memcpy(szFile, pNotify->FileName, pNotify->FileNameLength);

			// �÷��׿� ���� ���ϸ� ���� �� ��
			flag = Whitelisted(szFile);
			if(flag == 1)
			{
				switch (pNotify->Action)
				{
				case FILE_ACTION_ADDED:
					roaming_file_name = _tcsstr(szFile, findRoam)+8;

					//���� ���ϸ� ������ ���� ������ ����
					Rname = _tcsstr(roaming_file_name, checkPoint); //(abc.txt, .)

					//Ȯ���ڸ� ����
					_tprintf(_T("+++++++++++++++++++ %s ++++++++++++++++++++[File Name] \n"), Rname);

					//Ȯ���ڰ� txt�� �´ٸ� �迭�� �־����� ���ϸ��� ��� 
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
