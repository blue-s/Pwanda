#include "mon.h"

TCHAR * search(){
	TCHAR * result;
	// 
	// runList �޾ƿ� 
	// Ž�� �˰��� 
	return result;
	// ���⼭ ����Ʈ�� kill�� ������ 
}

void sorting(int flag){
	// ���� ���� ������ 
	// abc ���� 
	// ����� roamingList prefetchList�� �ҷ��� // flag �־���� �� 
}

int compare(){
	
	// sorting�� fileName�� ������ �갡 ���� ���ߵ��� 
	// �߰� ©�� �� �� ���� return int flag 
	// search �� sorting ����� �κ� 
	// ���ڿ� �� 

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
	
	// Char�� ��ȯ
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
		TCHAR * result = search(); // �̰� result ����غ��� // ų�� ���� �� 
		break;
	default:
		printf("Error!\n");
		break;
	}

	ListPrint();
	//compare();
}