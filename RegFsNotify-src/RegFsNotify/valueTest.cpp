#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

typedef struct REGMON { 
	HKEY   hMainKey; // ���� : ���� ������Ʈ�� �ڵ� 
	LPTSTR szSubkey; // ���� : ����Ű �̸� 
}   REGMON, *PREGMON; // �̰� PREGMON �̶�� �ϰڴ� 


#define MAX_PATH 255


void main(){

	DWORD i;
	LONG Result=ERROR_SUCCESS;
	LPTSTR str[MAX_PATH];
	LPDWORD Size;

	PREGMON p;
	p = new REGMON;
	p ->hMainKey = HKEY_CURRENT_USER;
	p ->szSubkey = _T("SOFTWARE\Microsoft\Windows\CurrentVersion\Run");

	
	if( RegOpenKeyEx( HKEY_CURRENT_USER, p->szSubkey, 0, KEY_ALL_ACCESS, &p->hMainKey) == ERROR_SUCCESS )
{
   for(i = 0; Result == ERROR_SUCCESS; i++ )
   {
	   Result = RegEnumValue( p->hMainKey, i, str[i], Size, NULL, NULL, NULL, NULL );
   
      if( Result == ERROR_SUCCESS ) 
      {
         DWORD dwType;
         DWORD dwSize = MAX_PATH;
         char szBuffer[MAX_PATH];
    
		 RegQueryValueEx(p->hMainKey, str[i], 0, &dwType, (LPBYTE)szBuffer, &dwSize);

         // str �������� ���� ���̸��� ����
         // szBuffer �������� ���� �������Ͱ� ����
      }
   
      Size =(LPDWORD)MAX_PATH;
   }
}

	RegCloseKey(p->hMainKey);


}
