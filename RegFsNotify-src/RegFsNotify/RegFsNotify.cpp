
#include "mon.h"

// �ڵ��� ID ������ �ϴ� �����̰� OS���� ������ ��Ź���� �� ���������� �䱸�� �ڿ�?�� ���°� �ƴ϶�
// �ڵ��� ��ȯ���ִ� ���̴� 
// �ڵ��� ���� �������ε� ���ڿ����� ������ �����̴�
// �ڵ��� ũ�� ������ ��Ÿ���� ���� �ƴ� �ܼ��� ������ ���� ǥ���̴� 
// ���� ������ �ڵ鳢���� ����� �ߺ��� ���� ������ �ʴ´� 
// �ڵ��� �ü���� �߱��Ѵ� (����ڴ� �߱޵� ���� ����ϱ⸸ �Ѵ�)
 
 HANDLE g_hFile;
 HANDLE g_hStopEvent; // ������ ���� �̺�Ʈ �ڵ� 
 HANDLE g_hRegWatch[2]; // g_hRegWatch �� ������ ���� 

 // �ܼ��ؽ�Ʈ�� �������� �κ� ? /
USHORT GetConsoleTextAttribute(HANDLE hConsole) // hConsole�� ��� �Դ��� ã�� 
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
	// hConsole �ܼ� �ڵ��� ��ũ�� ���ۿ� ��Ƽ� 
    return(csbi.wAttributes);
	// ���� �ε����� �����Ͽ� Ư�� �������� ����Ѵ� 
}

// ��� : ���� ����� �ܼ� ��ũ���� ������ �����Ͽ� ��� 
void Output(USHORT Color, LPTSTR format, ... )
{
	va_list args;
	//char * va_list : �� ������ �����ּҸ� ����Ű�� ������
	int len; // ���� 
	DWORD cb; // 
	LPTSTR buffer; // 
	

	va_start(args, format);
	//va_start : va_list�� ������� �����Ϳ��� ���� ���� �� ù��° ������ �ּҸ� ��������
	//va_start(va_list�� ���� ������, ������ ���� �μ�)
	len = _vsctprintf(format, args) + sizeof(TCHAR); 
	// _vsctprintf: ���յ� ���ڿ��� ���� Ȯ��, ���ڿ��� ���̰� ũ�� �������� �޸� Ȯ���Ͽ� printf
	// ���� �޸� ���� 
	buffer = new TCHAR[len * sizeof(TCHAR)];

	if (!buffer) { 
		return;
	}

	_vstprintf_s(buffer, len, format, args);
	// INVALID_HANDLE_VALUE: �ڵ��� �ʱ�ȭ, ���� -1�� ����
	if (g_hFile != INVALID_HANDLE_VALUE) {
	// �ڵ��� �ʱ�ȭ �ȵ��� ���

#ifdef _UNICODE
		LPSTR str = new CHAR[len + 1];
		if (str) { 
			memset(str, 0, len + 1);
			// 0���� ���ڿ��� �� ä�� 
			WideCharToMultiByte(CP_ACP, 0, 
				buffer, -1, str, len, NULL, NULL);
			//WideCharToMultiByte : �����ڵ� -> ANSI
			WriteFile(g_hFile, str, strlen(str), &cb, NULL);
			//WriteFile: Ư�� ���� Ȥ�� ����� ��ġ�� �����͸� ����
			delete[] str;
		}
#else 
	WriteFile(g_hFile, buffer, strlen(buffer), &cb, NULL);
#endif
	} 

	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	//GetStdHandle - ǥ�� ����� �� ���� �ڵ��� ���� 
	//�ֿܼ��� stdout, stdin, stderr �� �ڵ��� �������� �Լ�

	//GetStdHandle ����
	//STD_INPUT_HANDLE : ǥ�� �Է� �ڵ�
	//STD_OUTPUT_HANDLE : ǥ�� ��� �ڵ�
	//STD_ERROR_HANDLE : ǥ�� ���� �ڵ�

	if (Color) 
	{ 
		SetConsoleTextAttribute(
			Handle, Color | FOREGROUND_INTENSITY);
		// FOREGROUND_INTENSITY: ������ 
	} 

	_tprintf(buffer);

	SetConsoleTextAttribute(Handle, 
		FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

   delete[] buffer;
}

//�ý��� ���� �� �߻��ϴ� �ñ׳�
BOOL CtrlHandler(DWORD fdwCtrlType) // --> fwdCtrlType ��� �������� ���� ?
{ 
	switch(fdwCtrlType) 
	{ 
    case CTRL_C_EVENT: 
		SetEvent(g_hStopEvent); // �̺�Ʈ�� ����, Non Signal ���¸� Signal ���·� ��ȯ 
		return TRUE;
	//�ֿܼ��� [��Ʈ�� + c] ������ �� �߻��ϴ� �ñ׳�

    case CTRL_CLOSE_EVENT: 
		SetEvent(g_hStopEvent);
		return TRUE; 
	//�ֿܼ��� �ݱ� | Close | ���� from �۾������� 

    case CTRL_BREAK_EVENT: 
		return FALSE; 
	//�ֿܼ��� Ctrl+Break

    case CTRL_LOGOFF_EVENT: 
		return FALSE; 

	//����ڰ� �α׿����� �õ��� �� �ý����� ��� �ܼ� ���α׷��� ��ε� ĳ���õǴ� �ñ׳� 
    
	case CTRL_SHUTDOWN_EVENT: 
		return FALSE; 
    //����ڰ� �ý����� �˴ٿ��ϰ� �Ǹ� ��� �ܼ� ���α׷��� ��ε� ĳ���õǴ� �ñ׳�

	default: 
		return FALSE; 
	} 
} 


void _tmain(int argc, TCHAR *argv[])
{
	g_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// HANDLE ���������� ���⼭ �������ִ±��� 
	// CreateEvent : �̸� ���� �̺�Ʈ ��ü�� ���� 
	// �̺�Ʈ�� �߻����� ���� ���·� �����Ǹ� 
	// ������ �̺�Ʈ �ڵ��� �ڽ� ���μ����� ��ӵ��� �ʰ� �ڵ����� �̺�Ʈ�� �����Ǵ� ��ü
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	g_hFile = CreateFile(_T("RegFsNotify.txt"), 
		GENERIC_WRITE, // ���� 
		FILE_SHARE_READ, 0, 
		CREATE_ALWAYS, 0, NULL);
	// CreateFile: ������ �����ϴ� �� �� �ƴ϶�, ����/���͸��� �� �� ���� 

	HANDLE hThread[2];

	hThread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFileMonitor, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartRegistryMonitor, NULL, 0, NULL);
	
	// CreateThread: ������ ����
	// �Ѱ��� �Ű������� NULL, ������ ������� ��� CPU�� ���� �����층, �������� ID ���� NULL 

	WaitForMultipleObjects(2, (const HANDLE*)&hThread, TRUE, INFINITE);
	// ��ٸ���� �� ��� ��ü�� ��ȣ ���¿� ���� ������ ��ٸ���. ��� �ð��� ���� ���� �ִ�
	// g_hRegWatch �ڵ��� �ڵ� 0���� ���� 
	TerminateThread(g_hRegWatch[0], 0);
	TerminateThread(g_hRegWatch[1], 0);

	CloseHandle(g_hStopEvent);
	CloseHandle(g_hFile);
	// g_hStopEvent, g_hFile �ڵ��� ���� 

	_tprintf(_T("Program terminating.\n"));
}