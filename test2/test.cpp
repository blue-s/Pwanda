#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>

char* list[100];
int num = 0;

TCHAR* tList[100];

void ListPrint(void){
	for(int i=0; i<num; i++){
		printf("%S\n", tList[i]);
	}
}

int _tmain(int argc, TCHAR *argv[]){
	list[0] = "hihi";
	list[1] = "hihiho";
	
	TCHAR* buffer1 = _T("hihi");
	TCHAR* buffer2 = _T("hello");

	tList[0] = buffer1;
	tList[1] = buffer2;

	num = 2;
	ListPrint();

	return 0;
}

