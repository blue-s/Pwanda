#include "mon.h"

#include <stdio.h>
#include <string.h>

static TCHAR *list[1000];
static TCHAR *roaming[1000];
static TCHAR *prefetch[1000];
static TCHAR *RPresult[100];  //로밍하고,프리패치비교 결과가 같을때 여기에 저장
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


//로밍,프리패치 비교
void compare(void){  //로밍이 데이터가 별로 없기때문에 로밍을 기준으로 프리패치를 비교할 예정
	for(int r=0; r<j; r++){  //로밍이 데이터 기준
		printf("%d : %S\n", r, roaming[r]);
		for(int p=0; p<k; p++){ //프리패치
			//printf("%d : %S, %d : %S\n",r, roaming[r], p, prefetch[p]);
			//printf("%d\n", _tcsicmp(roaming[r], prefetch[p]));
		
			if(_tcsicmp(roaming[r],prefetch[p])==0){

				RPresult[r]=roaming[r];
				Output(0, _T("####################### result ####################### %s\n"), RPresult[r]);
			}
		}
	}
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

}



// File.cpp가 Reg.cpp보다 먼저 끝나기 때문에 의심 프로세스 추출에 NULL값이 자꾸 들어가는 것
// 그래서 file.cpp 와 reg.cpp가 동작이 끝날 때마다 extractProceeName 파일로 와서 
// 추출한 프로세스 이름을 쌓여진 배열과 비교해서 공통적인 이름을 뽑아낸다.

