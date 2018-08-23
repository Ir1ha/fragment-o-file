#include "fragment.h"

void main()
{
	setlocale(LC_ALL, "");
	char name[256] = "I:\\try.JPG";
	int n = 8;
	/*printf("Hello!\nWrite the full path to your file\nFor example: I:\\try.jpg");
	scanf("%s", name);
	printf("\nWrite the number of fragments\n");
	scanf("%d", &n);*/
	WCHAR DiskName[256] = L"";
	WCHAR FileName[256] = L"";
	DWORD ClustNumber;
	ClustNumber = SetFiles(n, name, DiskName, FileName);

	find_max_clus(DiskName);
	Search(name);
	GetClusters(ClustNumber, DiskName, FileName);
	/*ULONG64 BLcn = -1, ELcn = -1;
	HANDLE hDisk = CreateFile(L"\\\\.\\I:", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (FindFreeBlock(hDisk, 0, 32, &BLcn, &ELcn)){
		printf("its ok");
	};
	CloseHandle(hDisk);*/

	printf("WWOW");
	return;
}