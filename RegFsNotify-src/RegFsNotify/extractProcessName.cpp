#include "mon.h"

#include <stdio.h>
#include <string.h>

static TCHAR *list[10];
static int i;

void ListPrint(){
	Output(FOREGROUND_BLUE, _T("%s\n"),list[0]);
	Output(FOREGROUND_BLUE, _T("%s\n"),list[1]);
	Output(FOREGROUND_BLUE, _T("%s\n"),list[2]);
	Output(FOREGROUND_BLUE, _T("%s\n"),list[3]);
	Output(FOREGROUND_BLUE, _T("%s\n"),list[4]);


}

void ExtractProcess(TCHAR *p){
	
	list[i]=p;
	Output(FOREGROUND_BLUE, _T("-----------------[i=%d] extractProcessName--------------%s\n"), i,list[i]);
	i++;

	/*
	for(int j=0; j<10; j++){
		Output(FOREGROUND_BLUE, _T("--------index [%d] -  extractProcessName--------------[%s]\n"), j,list[j]);
	}
	*/
	
	
	
}


// File.cpp가 Reg.cpp보다 먼저 끝나기 때문에 의심 프로세스 추출에 NULL값이 자꾸 들어가는 것
// 그래서 file.cpp 와 reg.cpp가 동작이 끝날 때마다 extractProceeName 파일로 와서 
// 추출한 프로세스 이름을 쌓여진 배열과 비교해서 공통적인 이름을 뽑아낸다.

