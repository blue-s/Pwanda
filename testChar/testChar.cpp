#include <stdio.h>
#include <windows.h>
#include <tchar.h>

static TCHAR *list[1000];

int main(){
	list[0] = new TCHAR [4];
	for(int i=0; i<3; i++){
		list[0][i] = 'a';
	}

	printf("%S\n", list[0]);
}