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

#include "mon.h" // ����ڰ� ������ ��� ���� ����


//�ڵ� ����
 HANDLE g_hFile;
 HANDLE g_hStopEvent;
 HANDLE g_hRegWatch[2];

//USHORT ��ȣ ���� ������ �����ϴ� ������ Ÿ��
//�Ű� ������ �޾ƿ� hConsole�� �ܼ� ��ũ�� ���ۿ� ���
//csbi.wAttrigutes�� ���� �ε����� ���� -> Ư�� �������� �ؽ�Ʈ ���
USHORT GetConsoleTextAttribute(HANDLE hConsole)
{
	//�ܼ� ��ũ�� ���� ����
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	//�ܼ� ��ũ�� ���� ���, �ܼ� ��ġ ��ȯ
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return(csbi.wAttributes);
}

// ��� �Լ�: ���� ����� �ܼ� ��ũ�� ������ �����Ͽ� ���
void Output(USHORT Color, LPTSTR format, ... )
{
	va_list args;
	int len;
	DWORD cb;
	LPTSTR buffer;

	//char*va_list : �� ������ �����ּҸ� ����Ű�� ������
	//va_start: va_list�� ������� �����Ϳ��� ���� ���� �� ù��° �������ּҸ� ��������
	//va_start(va_list�� ���� ������, ������ �����μ�)
	va_start(args, format);

	//vsctprintf:���յ� ���ڿ��� ���� Ȯ��, ���ڿ��� ���̰� �ڸ� �������� �޸� Ȯ���� printf
	len = _vsctprintf(format, args) + sizeof(TCHAR); 

	//���� �޸� ����-����
	buffer = new TCHAR[len * sizeof(TCHAR)];

	if (!buffer) { 
		return;
	}

	_vstprintf_s(buffer, len, format, args);

	//#define INVALID_GANDLE_VALUE((HANDLE)(LONG_PTR)-1)
	//INVALID_HANDLE_VALUE:�ڵ��� �ʱ�ȭ, ���� -1�� ����

	if (g_hFile != INVALID_HANDLE_VALUE) { //�ڵ��� �ʱ�ȭ �ȵ��� ���
#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1]; //�����Ҵ�
		if (str) { 

			//memset : �޸𸮸� ������ ���ڷ� ä��, ���۸� �Ȱ��� ���ڷ� ä��
			//str�� 0���� len+1���� ä��
			memset(str, 0, len + 1);
			// WideCharToMultiByte �����ڵ� -> ANSI
			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);
			//WriteFile: Ư�� ���� Ȥ�� ����� ��ġ�� �����͸� ����.
			WriteFile(g_hFile, str, strlen(str), &cb, NULL);
			delete[] str;
		}
#else 
	WriteFile(g_hFile, buffer, strlen(buffer), &cb, NULL);
#endif
	} 

	//GetStdHandle �Լ���?

		//ǥ�� ����� �� ���� �ڵ��� ����
		//�ֿܼ��� stdout, stdin, stderr�� �ڵ��� �������� �Լ�
	//GetStdHandle����
		//STD_INPUT_HANDLE: ǥ�� �Է� �ڵ�
		//STD_OUPUT_HANDLE : ǥ�� ��� �ڵ�
		//STD_ERROR_HANDLE : ǥ�� ���� �ڵ�

	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (Color) 
	{ 
		//SetConsoleTextAttribute : �ܼ�â ���ڿ� �� �ֱ�
		//SetConsoleTextAttribute (����� �ܼ��� �ڵ�, ����)
		SetConsoleTextAttribute(
			Handle, Color | FOREGROUND_INTENSITY);
	} 

	_tprintf(buffer);

	SetConsoleTextAttribute(Handle, 
		FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

   delete[] buffer;
}

//�ý��� ���� �� �߻��ϴ� �ñ׳�
BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch(fdwCtrlType) 
	{ 
		//�ֿܼ���[��Ʈ��+C]�� ������ �� �߻��ϴ� �ñ׳�
    case CTRL_C_EVENT:
		//SetEvent: �̺�Ʈ�� ����, Non Signal ���¸� Signal���·� ��ȯ
		SetEvent(g_hStopEvent);
		return TRUE;

	//[�ݱ�]��ư�� �����ų� �ܼ� �ý��� �޴�����[close]�޴��� �����Ҷ�,
	//�Ǵ� �۾������ڷκ���[����]��ư�� ������ �� �߻��ϴ� �ñ׳�
    case CTRL_CLOSE_EVENT: 
		SetEvent(g_hStopEvent);
		return TRUE;

			//�ֿܼ���[��Ʈ��+�극��ũ]�������� �߻��ϴ� �ñ׳�
    case CTRL_BREAK_EVENT: 
		return FALSE; 

		//����ڰ� �α׿����� �õ��� �� �ý����� ��� �ܼ� ���α׷��� ��ε� ĳ���õǴ� �ñ׳�
    case CTRL_LOGOFF_EVENT: 
		return FALSE; 

		//����ڰ� �ý����� �˴ٿ��ϰ� �Ǹ� �ý����� ��� �ܼ� ���α׷��� ��ε�ĳ���� �Ǵ� �ñ׳�
    case CTRL_SHUTDOWN_EVENT: 
		return FALSE; 
    default: 
		return FALSE; 
	} 
} 

void _tmain(int argc, TCHAR *argv[])
{

	//CrreateEvent: �̸��� ������ �Ǵ� �̸��� ���� �̺�Ʈ ��ü�� ���ų� �����ϴ� �Լ�
	// ������ �̺�Ʈ �ڵ��� �ڽ� ���μ����� ��ӵ��� �ʰ� �ڵ����� �̺�Ʈ�� �����Ǵ� �̺�Ʈ ��ü��,
	//�̺�Ʈ�� �߻����� ���� ���·� �����Ǹ�, �̸����� �̺�Ʈ ��ü�� ����.
	g_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	//SetConsoleCtrlHandler: ������ ������ CtrlHandler ���� ó��
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	
	//CreateFile :������ �����ϴ� �� �Ӹ� �ƴ϶�, ����/���͸��� �� ���� �ִ�.
	g_hFile = CreateFile(_T("RegFsNotify.txt"), 
		GENERIC_WRITE, 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);

	//������ ���� ��ȯ ���� ����
	HANDLE hThread[2];

	//CreateThread : ������ ����, ȣ���� ���μ����� �����ּ� ������ �����带 �����ؼ� ����
	//�⺻ ���� Ư���� ����ϰ�, �⺻ũ��(0), ����Ǵ� �Լ�����[StartFileMonitor/ StartRegistryMonitor]�̰�
	//�Ѱ��� �Ű������� NULL, ������ ������� ��� CPU�� ���� �����ٸ�, �������� ID ���� NULL
	hThread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	//hThread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRegistryMonitor, NULL, 0, NULL);


	//WaitForMultipleObjects : ��ٸ���� �� ��� ��ü�� ��ȣ ���¿� ���� ������ ��ٸ���. ��� �ð��� ���� ���� �ִ�.
	//��ٸ����� �ϴ� ��ü �ڵ��� �迭 ũ��� 2, �̸��� hThread
	//��� �迭�� �ִ� ��� ��ü �ڵ��� �ñ׳� ���°� �Ǿ�� ��ȯ
	//��� �ڵ��� �ñ׳� ���°� �� ������ ��ٸ�
	WaitForMultipleObjects(1, (const HANDLE*)&hThread, TRUE, INFINITE);
	
	//TerminateThread : �����带 ����(����)
	//g_hRegWatch �ڵ��� �ڵ� 0 ����
	TerminateThread(g_hRegWatch[0], 0);
	//TerminateThread(g_hRegWatch[1], 0);

	//g_hStopEvent, g_hFile �ڵ��� �ݴ´�.
	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile);

	_tprintf(_T("Program terminating.\n"));
}