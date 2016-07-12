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

 HANDLE g_hFile;
 HANDLE g_hStopEvent;
 HANDLE g_hRegWatch[2];


USHORT GetConsoleTextAttribute(HANDLE hConsole)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return(csbi.wAttributes);
}

void Output(USHORT Color, LPTSTR format, ... )
{
	va_list args;
	int len;
	DWORD cb;
	LPTSTR buffer;

	va_start(args, format);
	len = _vsctprintf(format, args) + sizeof(TCHAR); 
	buffer = new TCHAR[len * sizeof(TCHAR)];

	if (!buffer) { 
		return;
	}

	_vstprintf_s(buffer, len, format, args);

	if (g_hFile != INVALID_HANDLE_VALUE) {
#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1];
		if (str) { 
			memset(str, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);
			WriteFile(g_hFile, str, strlen(str), &cb, NULL);
			delete[] str;
		}
#else 
	WriteFile(g_hFile, buffer, strlen(buffer), &cb, NULL);
#endif
	} 

	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (Color) 
	{ 
		SetConsoleTextAttribute(
			Handle, Color | FOREGROUND_INTENSITY);
	} 

	_tprintf(buffer);

	SetConsoleTextAttribute(Handle, 
		FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

   delete[] buffer;
}
 
BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch(fdwCtrlType) 
	{ 
    case CTRL_C_EVENT: 
		SetEvent(g_hStopEvent);
		return TRUE;
    case CTRL_CLOSE_EVENT: 
		SetEvent(g_hStopEvent);
		return TRUE; 
    case CTRL_BREAK_EVENT: 
		return FALSE; 
    case CTRL_LOGOFF_EVENT: 
		return FALSE; 
    case CTRL_SHUTDOWN_EVENT: 
		return FALSE; 
    default: 
		return FALSE; 
	} 
} 

void _tmain(int argc, TCHAR *argv[])
{
	g_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	g_hFile = CreateFile(_T("RegFsNotify.txt"), 
		GENERIC_WRITE, 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);

	HANDLE hThread[2];

	hThread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRegistryMonitor, NULL, 0, NULL);

	WaitForMultipleObjects(2, (const HANDLE*)&hThread, TRUE, INFINITE);
	TerminateThread(g_hRegWatch[0], 0);
	TerminateThread(g_hRegWatch[1], 0);


	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile);

	_tprintf(_T("Program terminating.\n"));
}