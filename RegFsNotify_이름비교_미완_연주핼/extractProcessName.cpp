#include "mon.h"

//static TCHAR * run;
//static TCHAR * roaming;
//static TCHAR * prefetch;
//static TCHAR * result;  //로밍하고,프리패치비교 결과가 같을때 여기에 저장


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

void compare(void){  //로밍이 데이터가 별로 없기때문에 로밍을 기준으로 프리패치를 비교할 예정

	//DWORD res1, res2;

	//if(run!=NULL || roaming!=NULL || prefetch!=NULL){
	//	if(roaming!=NULL){
	//		res1 = _tcsicmp(run, roaming); //res == 0 같은것
	//		if(res1 == 0 && prefetch!=NULL){
	//			res2 = _tcsicmp(prefetch, roaming); //res == 0 같은것
	//			if(res2 == 0){
	//				printf("RESULT: %s\n", roaming);
	//			
	//			}
	//		}
	//	}

	//	else if(run!=NULL){
	//		res1 = _tcsicmp(run, roaming); //res == 0 같은것
	//		if(res1 == 0 && prefetch!=NULL){
	//			res2 = _tcsicmp(prefetch, roaming); //res == 0 같은것
	//			if(res2 == 0){
	//				printf("RESULT: %s\n", roaming);
	//			
	//			}
	//		}
	//	}
	//	else if(roaming!=NULL){
	//		res1 = _tcsicmp(run, roaming); //res == 0 같은것
	//		if(res1 == 0 && prefetch!=NULL){
	//			res2 = _tcsicmp(prefetch, roaming); //res == 0 같은것
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
	TCHAR buffer_filename[BUFSIZ];
	_tcsncpy(buffer_filename, fileName, BUFSIZ);
	
	// Char로 변환
	// flag : (1) Roaming, (2) Prefetch, (3) Run, (-1) Nothing
	switch(flag)
	{
	case 1:
		Output_Console(FOREGROUND_BLUE, _T("-----------------File> roaming -------------->>>>>>> %s\n"), fileName);
		roamingList[roamingNum] = new TCHAR [BUFSIZ];
		_tcsncpy(roamingList[roamingNum], buffer_filename, BUFSIZ);
		roamingNum += 1;
		break;
	case 2:
		Output_Console(FOREGROUND_BLUE, _T("-----------------File> prefetch -------------->>>>>>> %s\n"), fileName);
		prefetchList[prefetchNum] = new TCHAR [BUFSIZ];
		_tcsncpy(prefetchList[prefetchNum], buffer_filename, BUFSIZ);
		prefetchNum += 1;
		break;
	case 3:
		Output_Console(FOREGROUND_BLUE, _T("-----------------Registry> Run -------------->>>>>>> %s\n"), fileName);
		runList[runNum] = new TCHAR [BUFSIZ];
		_tcsncpy(runList[runNum], buffer_filename, BUFSIZ);
		runNum += 1;
		break;
	default:
		printf("Error!\n");
		break;
	}


	ListPrint();
	ListProcessInfo();

	if(KillProcess(fileName))
			Output_Console(FOREGROUND_RED, _T("*** KILL SUCCESS!!!\n\n"));

	//compare();
}