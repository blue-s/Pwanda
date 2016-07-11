#include "mon.h"

// maximum number of drives to monitor 
#define MAX_DRIVES 24

// global variables for change notifications
HANDLE  g_ChangeHandles[MAX_DRIVES];
HANDLE  g_DirHandles[MAX_DRIVES];
LPTSTR  g_szDrives[MAX_DRIVES];
DWORD   g_idx = 0;


//asdf�� ���� ���μ��� ��ȭ����.
void ProcessChange(int idx)
{
	BYTE buf[32 * 1024];
	DWORD cb = 0;
	PFILE_NOTIFY_INFORMATION pNotify;
	int offset = 0;
	TCHAR szFile[MAX_PATH*2];

	memset(buf, 0, sizeof(buf));

	// find out what type of change triggered the notification
	if (ReadDirectoryChangesW(g_DirHandles[idx], buf, 
		sizeof(buf), TRUE, 
		FILE_CHANGE_FLAGS, &cb, NULL, NULL))
	{
		// parse the array of file information structs
	do {
			pNotify = (PFILE_NOTIFY_INFORMATION) &buf[offset];
			offset += pNotify->NextEntryOffset;

			memset(szFile, 0, sizeof(szFile));

			memcpy(szFile, pNotify->FileName, 
				pNotify->FileNameLength);

		/*	
			if (!(asdfWhitelisted(szFile))) {    
				continue;
			}
		*/  //���� ���� ��쿡
			//��Ͽ� ������ ���, ������ ��¾��ϰ� ���� ������ �Ѿ.
			// ��ȯ���� ���� ���� ȭ��Ʈ����Ʈ�� ��ġ�� �Ѵٴ� ��.
			// ȭ��Ʈ ����Ʈ�� ��ġ�ϸ�, ���ε� �տ� "!" �־ �������� ��. �׷��� ���,
			// ��ġ���� ������, ����� ����.
		/*	
			if (asdfWhitelisted(szFile)) {    
				continue;
			}
		*/  //���� ���� ��쿡
			//ȭ��Ʈ ����Ʈ�� �ִ°��� �ǳʶٰ� �������� ����Ѵٴ� ����.
			// ���� �� �κп� switch �κ��� ������ �ͼ� ����� ��Ű��, ������ �κ��� continue�� �ٲ����.
		
			if (asdfWhitelisted(szFile)) {    
					switch (pNotify->Action)
				{
					case FILE_ACTION_ADDED:
						Output_asdf(FOREGROUND_GREEN, 
							_T("[ADDED] %s%s\n"), g_szDrives[idx], szFile);
					break;
					case FILE_ACTION_REMOVED: 
						Output_asdf(FOREGROUND_RED, 
							_T("[REMOVED] %s%s\n"), g_szDrives[idx], szFile);
						break;
					case FILE_ACTION_MODIFIED: 
						Output_asdf(0, _T("[MODIFIED] %s%s\n"), 
							g_szDrives[idx], szFile);
						break;
					case FILE_ACTION_RENAMED_OLD_NAME:
						Output_asdf(0, _T("[RENAMED (OLD)] %s%s\n"), 
							g_szDrives[idx], szFile);
						break; 
					case FILE_ACTION_RENAMED_NEW_NAME:
						Output_asdf(0,_T("[RENAMED (NEW)] %s%s\n"), 
							g_szDrives[idx], szFile);
						break;
					default:
						Output_asdf(0,_T("[??] %s%s\n"), 
							g_szDrives[idx], szFile);
						break;
				}; //end of switch
			}else if(qwerWhitelisted(szFile)){
					switch (pNotify->Action)
				{
					case FILE_ACTION_ADDED:
						Output_qwer(FOREGROUND_GREEN, 
							_T("[ADDED] %s%s\n"), g_szDrives[idx], szFile);
					break;
					case FILE_ACTION_REMOVED: 
						Output_qwer(FOREGROUND_RED, 
							_T("[REMOVED] %s%s\n"), g_szDrives[idx], szFile);
						break;
					case FILE_ACTION_MODIFIED: 
						Output_qwer(0, _T("[MODIFIED] %s%s\n"), 
							g_szDrives[idx], szFile);
						break;
					case FILE_ACTION_RENAMED_OLD_NAME:
						Output_qwer(0, _T("[RENAMED (OLD)] %s%s\n"), 
							g_szDrives[idx], szFile);
						break; 
					case FILE_ACTION_RENAMED_NEW_NAME:
						Output_qwer(0,_T("[RENAMED (NEW)] %s%s\n"), 
							g_szDrives[idx], szFile);
						break;
					default:
						Output_qwer(0,_T("[??] %s%s\n"), 
							g_szDrives[idx], szFile);
						break;
				}; //end of switch
			}else{
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

	// get a list of logical drives
	memset(pszList, 0, sizeof(pszList));
	GetLogicalDriveStrings(sizeof(pszList), pszList);

	// parse the list of null-terminated drive strings
	pStart = pszList;
	while(_tcslen(pStart)) 
	{
		ddType = GetDriveType(pStart);

		// only monitor local and removable (i.e. USB) drives
		if ((ddType == DRIVE_FIXED || ddType == DRIVE_REMOVABLE) && 
			_tcscmp(pStart, _T("A:\\")) != 0)
		{
			hChange = FindFirstChangeNotification(pStart,
									TRUE, /* watch subtree */
								    FILE_CHANGE_FLAGS);

			if (hChange == INVALID_HANDLE_VALUE)
				continue;

			hDir = CreateFile(pStart, 
				FILE_LIST_DIRECTORY, 
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL, 
				OPEN_EXISTING, 
				FILE_FLAG_BACKUP_SEMANTICS, /* opens a directory */
				NULL);

			if (hDir == INVALID_HANDLE_VALUE) {
				FindCloseChangeNotification(hChange);
				continue;
			}

			_tprintf(_T("Monitoring %s\n"), pStart);

			// save the handles and drive letter 
			g_szDrives[g_idx]      = _tcsdup(pStart);
			g_DirHandles[g_idx]    = hDir;
			g_ChangeHandles[g_idx] = hChange;
			g_idx++;
		}

		pStart += wcslen(pStart) + 1;
	}
 
	// wait for a notification to occur
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
		CloseHandle(g_DirHandles[i]);
		FindCloseChangeNotification(g_ChangeHandles[i]);
	}
}
