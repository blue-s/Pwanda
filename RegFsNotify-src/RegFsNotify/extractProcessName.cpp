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


// File.cpp�� Reg.cpp���� ���� ������ ������ �ǽ� ���μ��� ���⿡ NULL���� �ڲ� ���� ��
// �׷��� file.cpp �� reg.cpp�� ������ ���� ������ extractProceeName ���Ϸ� �ͼ� 
// ������ ���μ��� �̸��� �׿��� �迭�� ���ؼ� �������� �̸��� �̾Ƴ���.

