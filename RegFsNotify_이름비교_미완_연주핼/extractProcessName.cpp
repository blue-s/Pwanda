#include "mon.h"

#include <stdio.h>
#include <string.h>

static TCHAR * run;
static TCHAR * roaming;
static TCHAR * prefetch;
static TCHAR * result;  //�ι��ϰ�,������ġ�� ����� ������ ���⿡ ����


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

void ExtractProcess(DWORD flag, TCHAR *fileName){
	// flag : (1) Roaming, (2) Prefetch, (3) Run

	switch(flag)
	{
	case 1:
		Output_Console(FOREGROUND_BLUE, _T("-----------------File> roaming -------------->>>>>>> %s\n"), fileName);
		break;
	case 2:
		Output_Console(FOREGROUND_BLUE, _T("-----------------File> prefetch -------------->>>>>>> %s\n"), fileName);
		break;
	case 3:
		Output_Console(FOREGROUND_BLUE, _T("-----------------Registry> Run -------------->>>>>>> %s\n"), fileName);
		break;
	default:
		printf("Error!\n");
		break;
	}

	//compare();
	//ProNamePrint();
}



// File.cpp�� Reg.cpp���� ���� ������ ������ �ǽ� ���μ��� ���⿡ NULL���� �ڲ� ���� ��
// �׷��� file.cpp �� reg.cpp�� ������ ���� ������ extractProceeName ���Ϸ� �ͼ� 
// ������ ���μ��� �̸��� �׿��� �迭�� ���ؼ� �������� �̸��� �̾Ƴ���.

