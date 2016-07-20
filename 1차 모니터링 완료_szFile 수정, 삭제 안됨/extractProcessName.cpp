#include "mon.h"
// 수정 중
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
	printf("\n");
}

void PathListPrint(void){
	printf("*** Roaming Path List ***\n");
	for(int i=0; i<roamingNum; i++){
		printf("%S\n", roamingPath[i]);
	}

	printf("*** Prefetch Path List ***\n");
	for(int i=0; i<prefetchNum; i++){
		printf("%S\n", prefetchPath[i]);
	}
	printf("\n");
}

int compareFile(TCHAR* list[], int size, TCHAR* fileName)
{ 
	for(int i=size-1; i>=0; i--)
	{
		if(_tcscmp(list[i], fileName) == 0){
			printf("List[%d] %S\n", i, list[i]);
			return i;
		}
	}
	return -1;
}

void ExtractProcess(DWORD flag, TCHAR *fileName, TCHAR* szFileBuffer){
	int idxRoam, idxPre, idxRun;
	int compare = 0;
	TCHAR buffer[100];
	TCHAR buffer_path[MAX_PATH] = _T("C:\\");

	_tcsncpy(buffer, fileName, 100);
	_tcscat(buffer_path, szFileBuffer);
	
	// Char로 변환
	// flag : (1) Roaming, (2) Prefetch, (3) Run, (-1) Nothing
	switch(flag)
	{
	case 1:
		Output_Console(FOREGROUND_BLUE, _T("-----------------File> roaming -------------->>>>>>> %s\n"), fileName);
		roamingList[roamingNum] = new TCHAR [100];
		_tcsncpy(roamingList[roamingNum], buffer, 100);
		roamingPath[roamingNum] = new TCHAR [MAX_PATH*2];
		_tcsncpy(roamingPath[roamingNum], buffer_path, MAX_PATH*2);
		roamingNum += 1;
		break;
	case 2:
		Output_Console(FOREGROUND_BLUE, _T("-----------------File> prefetch -------------->>>>>>> %s\n"), fileName);
		prefetchList[prefetchNum] = new TCHAR [100];
		_tcsncpy(prefetchList[prefetchNum], buffer, 100);
		prefetchPath[prefetchNum] = new TCHAR [MAX_PATH*2];
		_tcsncpy(prefetchPath[prefetchNum], buffer_path, MAX_PATH*2);
		prefetchNum += 1;
		break;
	case 3:
		Output_Console(FOREGROUND_BLUE, _T("-----------------Registry> Run -------------->>>>>>> %s\n"), fileName);
		runList[runNum] = new TCHAR [100];
		_tcsncpy(runList[runNum], buffer, 100);
		runNum += 1;
		break;
	default:
		printf("Error!\n");
		break;
	}

	ListPrint();
	PathListPrint();
	// ListProcessInfo();
	
	
	if(idxRoam=compareFile(roamingList, roamingNum, fileName) != -1)	compare++;
	if(idxPre=compareFile(prefetchList, prefetchNum, fileName) != -1)	compare++;
	if(idxRun=compareFile(runList, runNum, fileName) != -1)				compare++;
	
	//idxRoam--;
	//idxPre--;
	
	printf("idxRoam = %d, idxPre = %d, idxRun = %d\n", idxRoam, idxPre, idxRun);

	if(compare > 1)
	{
		if(KillProcess(fileName)){
			Output_Console(FOREGROUND_GREEN, _T(">>>>>>> KILL SUCCESS!!!\n"));
		}

		// 복제 파일 삭제
		if(idxRoam != -1)
		{
			printf("Roaming Path[%d] : %S\n", idxRoam, roamingPath[idxRoam]);

			if(DeleteFile(roamingPath[idxRoam]))
				Output_Console(FOREGROUND_BLUE, _T("*** Roaming DELETE SUCCESS !!! ***\n\n"));
			else
				Output_Console(FOREGROUND_RED, _T("*** Roaming DELETE FIAL !!! ***\n\n"));
		}
		
		if(idxPre != -1)
		{
			printf("Prefetch Path[%d] : %S\n", idxPre, prefetchPath[idxPre]);

			if(DeleteFile(prefetchPath[idxPre]))
				Output_Console(FOREGROUND_BLUE, _T("*** Prefetch DELETE SUCCESS!!! ***\n\n"));
			else
				Output_Console(FOREGROUND_RED, _T("*** Prefetch DELETE FAIL !!! ***\n\n"));
		}

		if(idxRun != -1)
		{
			regDel(achValue);
		}
		return;
	}
	Output_Console(FOREGROUND_GREEN, _T(">>>>>>> Safe Process\n\n"));
}