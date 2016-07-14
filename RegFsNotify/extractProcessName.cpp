#include "mon.h"

TCHAR * search(){
	TCHAR * result;
	// 
	// runList 받아옴 
	// 탐색 알고리즘 
	return result;
	// 여기서 리저트를 kill로 보내줌 
}

void sorting(int flag){
	// 값이 들어올 때마다 
	// abc 정렬 
	// 여기로 roamingList prefetchList를 불러옴 // flag 넣어줘야 함 
}

int compare(){
	
	// sorting에 fileName이 들어오면 얘가 어디로 들어가야될지 
	// 중간 짤라서 왼 오 결정 return int flag 
	// search 와 sorting 공통된 부분 
	// 문자열 비교 

	return 0;
}



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

void ExtractProcess(DWORD flag, TCHAR *fileName){
	TCHAR buffer[100];
	_tcsncpy(buffer, fileName, 100);
	
	// Char로 변환
	// flag : (1) Roaming, (2) Prefetch, (3) Run, (-1) Nothing
	switch(flag)
	{
	case 1:
		Output_Console(FOREGROUND_BLUE, _T("-----------------File> roaming -------------->>>>>>> %s\n"), fileName);
		roamingList[roamingNum] = new TCHAR [100];
		_tcsncpy(roamingList[roamingNum], buffer, 100);
		roamingNum += 1;
		sorting(1);
		break;
	case 2:
		Output_Console(FOREGROUND_BLUE, _T("-----------------File> prefetch -------------->>>>>>> %s\n"), fileName);
		prefetchList[prefetchNum] = new TCHAR [100];
		_tcsncpy(prefetchList[prefetchNum], buffer, 100);
		prefetchNum += 1;
		sorting(2);
		break;
	case 3:
		Output_Console(FOREGROUND_BLUE, _T("-----------------Registry> Run -------------->>>>>>> %s\n"), fileName);
		runList[runNum] = new TCHAR [100];
		_tcsncpy(runList[runNum], buffer, 100);
		runNum += 1;
		TCHAR * result = search(); // 이거 result 출력해보기 // 킬로 보낼 값 
		break;
	default:
		printf("Error!\n");
		break;
	}

	ListPrint();
	//compare();
}