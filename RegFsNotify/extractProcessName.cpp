#include "mon.h"

#include <stdio.h>
#include <string.h>

static TCHAR *list[1000];
static TCHAR *roaming[1000];
static TCHAR *prefetch[1000];
static TCHAR *RPresult[100];  //�ι��ϰ�,������ġ�� ����� ������ ���⿡ ����
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


//�ι�,������ġ ��
void compare(void){  //�ι��� �����Ͱ� ���� ���⶧���� �ι��� �������� ������ġ�� ���� ����
	for(int r=0; r<j; r++){  //�ι��� ������ ����
		printf("%d : %S\n", r, roaming[r]);
		for(int p=0; p<k; p++){ //������ġ
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



// File.cpp�� Reg.cpp���� ���� ������ ������ �ǽ� ���μ��� ���⿡ NULL���� �ڲ� ���� ��
// �׷��� file.cpp �� reg.cpp�� ������ ���� ������ extractProceeName ���Ϸ� �ͼ� 
// ������ ���μ��� �̸��� �׿��� �迭�� ���ؼ� �������� �̸��� �̾Ƴ���.

