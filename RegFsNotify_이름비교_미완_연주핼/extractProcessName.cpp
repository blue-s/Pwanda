#include "mon.h"

#include <stdio.h>
#include <string.h>

static TCHAR * run;
static TCHAR * roaming;
static TCHAR * prefetch;
static TCHAR * result;  //로밍하고,프리패치비교 결과가 같을때 여기에 저장
/*
void ProNamePrint(){
	Output(FOREGROUND_BLUE, _T("[Run] %s\n"), run);
	Output(FOREGROUND_BLUE, _T("[Roaming] %s\n"),roaming);
	Output(FOREGROUND_BLUE, _T("[Prefetch] %s\n"), prefetch);
}
*/

// Run에서 가져온 값을 로밍과 프리패치에서 비교 


void compare(void){  //로밍이 데이터가 별로 없기때문에 로밍을 기준으로 프리패치를 비교할 예정

	DWORD res1, res2;

	if(run!=NULL || roaming!=NULL || prefetch!=NULL){
		if(roaming!=NULL){
			res1 = _tcsicmp(run, roaming); //res == 0 같은것
			if(res1 == 0 && prefetch!=NULL){
				res2 = _tcsicmp(prefetch, roaming); //res == 0 같은것
				if(res2 == 0){
					printf("RESULT: %s\n", roaming);
				
				}
			}
		}

		else if(run!=NULL){
			res1 = _tcsicmp(run, roaming); //res == 0 같은것
			if(res1 == 0 && prefetch!=NULL){
				res2 = _tcsicmp(prefetch, roaming); //res == 0 같은것
				if(res2 == 0){
					printf("RESULT: %s\n", roaming);
				
				}
			}
		}
		else if(roaming!=NULL){
			res1 = _tcsicmp(run, roaming); //res == 0 같은것
			if(res1 == 0 && prefetch!=NULL){
				res2 = _tcsicmp(prefetch, roaming); //res == 0 같은것
				if(res2 == 0){
					printf("RESULT: %s\n", roaming);
				
				}
			}
		}
		else {
			printf("error");
		}

	}


}

void ExtractProcess(DWORD flag, TCHAR *p){

	if(flag == 1)
	{
		run=p;
		Output(FOREGROUND_BLUE, _T("-----------------Registry> Run -------------->>>>>>> %s\n"), run);
	}
	else if(flag == 2)
	{
		roaming=p;
		Output(FOREGROUND_BLUE, _T("-----------------File> roaming -------------->>>>>>> %s\n"), roaming);
	}
	else if(flag == 3)
	{
		prefetch=p;
		Output(FOREGROUND_BLUE, _T("-----------------File> prefetch -------------->>>>>>> %s\n"), prefetch);
	}
	else
		printf("Error!!");

	//compare();
	//ProNamePrint();
}



// File.cpp가 Reg.cpp보다 먼저 끝나기 때문에 의심 프로세스 추출에 NULL값이 자꾸 들어가는 것
// 그래서 file.cpp 와 reg.cpp가 동작이 끝날 때마다 extractProceeName 파일로 와서 
// 추출한 프로세스 이름을 쌓여진 배열과 비교해서 공통적인 이름을 뽑아낸다.

