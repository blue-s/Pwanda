#include "mon.h"

#include <stdio.h>
#include <string.h>

static TCHAR * run;
static TCHAR * roaming;
static TCHAR * prefetch;
static TCHAR * result;  //�ι��ϰ�,������ġ�� ����� ������ ���⿡ ����
/*
void ProNamePrint(){
	Output(FOREGROUND_BLUE, _T("[Run] %s\n"), run);
	Output(FOREGROUND_BLUE, _T("[Roaming] %s\n"),roaming);
	Output(FOREGROUND_BLUE, _T("[Prefetch] %s\n"), prefetch);
}
*/

// Run���� ������ ���� �ιְ� ������ġ���� �� 


void compare(void){  //�ι��� �����Ͱ� ���� ���⶧���� �ι��� �������� ������ġ�� ���� ����

	DWORD res1, res2;

	if(run!=NULL || roaming!=NULL || prefetch!=NULL){
		if(roaming!=NULL){
			res1 = _tcsicmp(run, roaming); //res == 0 ������
			if(res1 == 0 && prefetch!=NULL){
				res2 = _tcsicmp(prefetch, roaming); //res == 0 ������
				if(res2 == 0){
					printf("RESULT: %s\n", roaming);
				
				}
			}
		}

		else if(run!=NULL){
			res1 = _tcsicmp(run, roaming); //res == 0 ������
			if(res1 == 0 && prefetch!=NULL){
				res2 = _tcsicmp(prefetch, roaming); //res == 0 ������
				if(res2 == 0){
					printf("RESULT: %s\n", roaming);
				
				}
			}
		}
		else if(roaming!=NULL){
			res1 = _tcsicmp(run, roaming); //res == 0 ������
			if(res1 == 0 && prefetch!=NULL){
				res2 = _tcsicmp(prefetch, roaming); //res == 0 ������
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



// File.cpp�� Reg.cpp���� ���� ������ ������ �ǽ� ���μ��� ���⿡ NULL���� �ڲ� ���� ��
// �׷��� file.cpp �� reg.cpp�� ������ ���� ������ extractProceeName ���Ϸ� �ͼ� 
// ������ ���μ��� �̸��� �׿��� �迭�� ���ؼ� �������� �̸��� �̾Ƴ���.

