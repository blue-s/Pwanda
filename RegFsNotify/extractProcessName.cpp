#include "mon.h"

#include <stdio.h>
#include <string.h>

static TCHAR *list[1000];
static TCHAR *roaming[1000];
static TCHAR *prefetch[1000];

static int i, j, k;

void ListPrint(){
	Output(FOREGROUND_BLUE, _T("%s\n"),list[0]);
	Output(FOREGROUND_BLUE, _T("%s\n"),list[1]);
	Output(FOREGROUND_BLUE, _T("%s\n"),list[2]);
	Output(FOREGROUND_BLUE, _T("%s\n"),list[3]);
	Output(FOREGROUND_BLUE, _T("%s\n"),list[4]);
	Output(FOREGROUND_BLUE, _T("%s\n"),list[5]);
	Output(FOREGROUND_BLUE, _T("%s\n"),list[6]);
}

void ExtractProcess(DWORD flag, TCHAR *p){
	
	if(flag == 1)
	{
		list[i]=p;
		Output(FOREGROUND_BLUE, _T("-----------------[i=%d] reg -------------->>>>>>> %s\n"), i,list[i]);
		i++;
	}
	else if(flag == 2)
	{
		roaming[j]=p;
		Output(FOREGROUND_BLUE, _T("-----------------[j=%d] roaming -------------->>>>>>> %s\n"), j,roaming[j]);
		j++;
	}
	else
	{
		prefetch[k]=p;
		Output(FOREGROUND_BLUE, _T("-----------------[k=%d] prefetch -------------->>>>>>> %s\n"), k,prefetch[k]);
		k++;
	}




	/*
	for(int j=0; j<10; j++){
		Output(FOREGROUND_BLUE, _T("--------index [%d] -  extractProcessName--------------[%s]\n"), j,list[j]);
	}
	*/

}


// File.cpp가 Reg.cpp보다 먼저 끝나기 때문에 의심 프로세스 추출에 NULL값이 자꾸 들어가는 것
// 그래서 file.cpp 와 reg.cpp가 동작이 끝날 때마다 extractProceeName 파일로 와서 
// 추출한 프로세스 이름을 쌓여진 배열과 비교해서 공통적인 이름을 뽑아낸다.

