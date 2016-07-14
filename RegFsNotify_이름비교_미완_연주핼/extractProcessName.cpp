#include "mon.h"

//static TCHAR * run;
//static TCHAR * roaming;
//static TCHAR * prefetch;
//static TCHAR * result;  //�ι��ϰ�,������ġ�� ����� ������ ���⿡ ����


void ListPrint(void){
	printf("*** Roaming List ***\n");
	for(int i=0; i<roamingNum; i++){
		printf("%S\n", roamingList[i]);
	}

	printf("*** Prefetch List ***\n");
	for(int i=0; i<prefetchNum; i++){
		printf("%S\n", prefetchList[i]);
	}

	printf("*** Run List ***\n");
	for(int i=0; i<runNum; i++){
		printf("%S\n", runList[i]);
	}
}

void compare(void){  //�ι��� �����Ͱ� ���� ���⶧���� �ι��� �������� ������ġ�� ���� ����

	//DWORD res1, res2;

	//if(run!=NULL || roaming!=NULL || prefetch!=NULL){
	//	if(roaming!=NULL){
	//		res1 = _tcsicmp(run, roaming); //res == 0 ������
	//		if(res1 == 0 && prefetch!=NULL){
	//			res2 = _tcsicmp(prefetch, roaming); //res == 0 ������
	//			if(res2 == 0){
	//				printf("RESULT: %s\n", roaming);
	//			
	//			}
	//		}
	//	}

	//	else if(run!=NULL){
	//		res1 = _tcsicmp(run, roaming); //res == 0 ������
	//		if(res1 == 0 && prefetch!=NULL){
	//			res2 = _tcsicmp(prefetch, roaming); //res == 0 ������
	//			if(res2 == 0){
	//				printf("RESULT: %s\n", roaming);
	//			
	//			}
	//		}
	//	}
	//	else if(roaming!=NULL){
	//		res1 = _tcsicmp(run, roaming); //res == 0 ������
	//		if(res1 == 0 && prefetch!=NULL){
	//			res2 = _tcsicmp(prefetch, roaming); //res == 0 ������
	//			if(res2 == 0){
	//				printf("RESULT: %s\n", roaming);
	//			
	//			}
	//		}
	//	}
	//	else {
	//		printf("error");
	//	}
	//}
}

void ExtractProcess(DWORD flag, TCHAR *fileName){
	TCHAR buffer[100];
	_tcsncpy(buffer, fileName, 100);
	
	// Char�� ��ȯ
	// flag : (1) Roaming, (2) Prefetch, (3) Run, (-1) Nothing
	switch(flag)
	{
	case 1:
		Output_Console(FOREGROUND_BLUE, _T("-----------------File> roaming -------------->>>>>>> %s\n"), fileName);
		roamingList[roamingNum] = new TCHAR [100];
		_tcsncpy(roamingList[roamingNum], buffer, 100);
		roamingNum += 1;
		break;
	case 2:
		Output_Console(FOREGROUND_BLUE, _T("-----------------File> prefetch -------------->>>>>>> %s\n"), fileName);
		prefetchList[prefetchNum] = new TCHAR [100];
		_tcsncpy(prefetchList[prefetchNum], buffer, 100);
		prefetchNum += 1;
		break;
	case 3:
		Output_Console(FOREGROUND_BLUE, _T("-----------------Registry> Run -------------->>>>>>> %s\n"), fileName);
		runList[runNum] = new TCHAR [100];
		_tcsncpy(prefetchList[runNum], buffer, 100);
		runNum += 1;
		break;
	default:
		printf("Error!\n");
		break;
	}

	ListPrint();
	//compare();
}