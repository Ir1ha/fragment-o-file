#include<stdio.h>
#include<Windows.h>
#include<conio.h>
#include<iostream>
#include <list>
#include<locale>
#include <string>
using namespace std;

DWORD ClusterSize;

/*void Search(char let);
int GetClusters(const char* name, bool d);*/

void ProcessVolume(char VolumeName);
int FindFreeBlock(
	HANDLE VolumeHandle,
	ULONG64 MinimumLcn,          /* Cluster must be at or above this LCN. */
	DWORD MinimumSize,           /* Cluster must be at least this big. */
	ULONG64 *BeginLcn,           /* Result, LCN of begin of cluster. */
	ULONG64 *EndLcn);


/////////////////////////////////////////

/////////////////////////////////////////
ULONG64 MaxLcn;                 /* Highest possible LCN + 1. */
struct {
	ULONG64 Start;
	ULONG64 End;
} Excludes[3];

//HANDLE 
void    ProcessVolume(char VolumeName) {
	HANDLE VolumeHandle;
	STARTING_LCN_INPUT_BUFFER InBuffer;
	struct {
		ULONG64 StartingLcn;
		ULONG64 BitmapSize;
		BYTE Buffer[8];
	} Data;
	int Result;
	NTFS_VOLUME_DATA_BUFFER NtfsData;
	DWORD w;

	/* Initialize. */
	MaxLcn = 0;
	Excludes[0].Start = 0;
	Excludes[0].End = 0;
	Excludes[1].Start = 0;
	Excludes[1].End = 0;
	Excludes[2].Start = 0;
	Excludes[2].End = 0;

	/* Initialize random number generator. */

	char name[7];
	sprintf(name, "\\\\.\\%c:", VolumeName);
	/* Open the VolumeHandle. If error then leave. */
	VolumeHandle = CreateFile(L"\\\\.\\I:", GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (VolumeHandle == INVALID_HANDLE_VALUE) {
		printf("Error while opening volume \"%c\"", VolumeName);
		//return INVALID_HANDLE_VALUE;
	}

	/* If the volume is not mounted then leave. Unmounted volumes can be
	defragmented, but the system administrator probably has unmounted
	the volume because he wants it untouched. */
	if (DeviceIoControl(VolumeHandle, FSCTL_IS_VOLUME_MOUNTED, NULL, 0, NULL, 0, &w, NULL) == 0) {
		printf("Skipping volume '%c' because it is not mounted.", VolumeName);
		CloseHandle(VolumeHandle);
		//return INVALID_HANDLE_VALUE;
	}

	/* Determine the maximum LCN. A single call to FSCTL_GET_VOLUME_BITMAP
	is enough, we don't have to walk through the entire bitmap.
	It's a pity we have to do it in this roundabout manner, because
	there is no system call that reports the total number of clusters
	in a volume. GetDiskFreeSpace() does, but is limited to 2Gb volumes,
	GetDiskFreeSpaceEx() reports in bytes, not clusters, and
	FSCTL_GET_NTFS_VOLUME_DATA only works for NTFS volumes. */
	InBuffer.StartingLcn.QuadPart = MaxLcn;
	Result = DeviceIoControl(VolumeHandle, FSCTL_GET_VOLUME_BITMAP,
		&InBuffer, sizeof(InBuffer),
		&Data, sizeof(Data),
		&w, NULL);
	if (Result == 0) {
		Result = GetLastError();
		if (Result != ERROR_MORE_DATA) {
			printf("Cannot defragment volume: %c", VolumeName);
			CloseHandle(VolumeHandle);
			//return INVALID_HANDLE_VALUE;
		}
	}
	MaxLcn = Data.StartingLcn + Data.BitmapSize;

	/* Setup the list of clusters that cannot be used. The Master File
	Table cannot be moved and cannot be used by files. All this is
	only necessary for NTFS volumes. */
	Result = DeviceIoControl(VolumeHandle, FSCTL_GET_NTFS_VOLUME_DATA,
		NULL, 0, &NtfsData, sizeof(NtfsData), &w, NULL);
	if (Result != 0) {
		/* Note: NtfsData.TotalClusters.QuadPart should be exactly the same
		as the MaxLcn that was determined in the previous block. */
		Excludes[0].Start = NtfsData.MftStartLcn.QuadPart;
		Excludes[0].End = NtfsData.MftStartLcn.QuadPart +
			NtfsData.MftValidDataLength.QuadPart / NtfsData.BytesPerCluster;
		Excludes[1].Start = NtfsData.MftZoneStart.QuadPart;
		Excludes[1].End = NtfsData.MftZoneEnd.QuadPart;
		Excludes[2].Start = NtfsData.Mft2StartLcn.QuadPart;
		Excludes[2].End = NtfsData.Mft2StartLcn.QuadPart +
			NtfsData.MftValidDataLength.QuadPart / NtfsData.BytesPerCluster;

		/* Show debug info. */
	}
	//return VolumeHandle;
	/* Close the volume handle. */
	CloseHandle(VolumeHandle);

	/* Cleanup the FileList. */
}

void Search()
{
	system("cls");
	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD NumberOfFreeClusters;
	DWORD TotalNumberOfClusters;
	GetDiskFreeSpaceA("I:\\",
		&SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters);
	ClusterSize = SectorsPerCluster*BytesPerSector;
	printf("%d", ClusterSize);
}

void find_max_clus(){
	//setlocale(LC_ALL, "");
	HANDLE VolumeHandle;
	STARTING_LCN_INPUT_BUFFER InBuffer;
	struct {
		ULONG64 StartingLcn;
		ULONG64 BitmapSize;
		BYTE Buffer[8];
	} Data;
	int Result;
	NTFS_VOLUME_DATA_BUFFER NtfsData;
	DWORD w;

	/* Initialize. */
	MaxLcn = 0;
	Excludes[0].Start = 0;
	Excludes[0].End = 0;
	Excludes[1].Start = 0;
	Excludes[1].End = 0;
	Excludes[2].Start = 0;
	Excludes[2].End = 0;

	/* Open the VolumeHandle. If error then leave. */
	VolumeHandle = CreateFile(L"\\\\.\\I:", GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (VolumeHandle == INVALID_HANDLE_VALUE) {
		printf("Error while opening volume");
		//return INVALID_HANDLE_VALUE;
	}

	/* If the volume is not mounted then leave. Unmounted volumes can be
	defragmented, but the system administrator probably has unmounted
	the volume because he wants it untouched. */
	if (DeviceIoControl(VolumeHandle, FSCTL_IS_VOLUME_MOUNTED, NULL, 0, NULL, 0, &w, NULL) == 0) {
		printf("Skipping volume because it is not mounted");
		CloseHandle(VolumeHandle);
		//return INVALID_HANDLE_VALUE;
	}

	/* Determine the maximum LCN. A single call to FSCTL_GET_VOLUME_BITMAP
	is enough, we don't have to walk through the entire bitmap.
	It's a pity we have to do it in this roundabout manner, because
	there is no system call that reports the total number of clusters
	in a volume. GetDiskFreeSpace() does, but is limited to 2Gb volumes,
	GetDiskFreeSpaceEx() reports in bytes, not clusters, and
	FSCTL_GET_NTFS_VOLUME_DATA only works for NTFS volumes. */
	InBuffer.StartingLcn.QuadPart = MaxLcn;
	Result = DeviceIoControl(VolumeHandle, FSCTL_GET_VOLUME_BITMAP,
		&InBuffer, sizeof(InBuffer),
		&Data, sizeof(Data),
		&w, NULL);
	if (Result == 0) {
		Result = GetLastError();
		if (Result != ERROR_MORE_DATA) {
			printf("Cannot defragment volume");
			CloseHandle(VolumeHandle);
			//return INVALID_HANDLE_VALUE;
		}
	}
	MaxLcn = Data.StartingLcn + Data.BitmapSize;

	/* Close the volume handle. */
	CloseHandle(VolumeHandle);
}

int FindFreeBlock(
	HANDLE VolumeHandle,
	ULONG64 MinimumLcn,          /* Cluster must be at or above this LCN. */
	DWORD MinimumSize,           /* Cluster must be at least this big. */
	ULONG64 *BeginLcn,           /* Result, LCN of begin of cluster. */
	ULONG64 *EndLcn) {           /* Result, LCN of end of cluster. */
	STARTING_LCN_INPUT_BUFFER InBuffer;
	struct {
		ULONG64 StartingLcn;
		ULONG64 BitmapSize;
		BYTE Buffer[32768];           /* Most efficient if binary multiple. */
	} Data;
	ULONG64 Lcn;
	ULONG64 ClusterStart;
	int Index;
	int IndexMax;
	BYTE Mask;
	int InUse;
	int PrevInUse;
	int Result;
	DWORD w;

	/* Main loop to walk through the entire clustermap. */
	Lcn = MinimumLcn;
	ClusterStart = 0;
	PrevInUse = 1;
	do {

		/* Sanity check. */
		if ((MaxLcn > 0) && (Lcn >= MaxLcn)) return 0;

		/* Fetch a block of cluster data. */
		InBuffer.StartingLcn.QuadPart = Lcn;
		Result = DeviceIoControl(VolumeHandle, FSCTL_GET_VOLUME_BITMAP,
			&InBuffer, sizeof(InBuffer),
			&Data, sizeof(Data),
			&w, NULL);
		if (Result == 0) {
			Result = GetLastError();
			if (Result != ERROR_MORE_DATA) {
				return 0;
			}
		}

		/* Analyze the clusterdata. We resume where the previous block left
		off. If a cluster is found that matches the criteria then return
		it's LCN (Logical Cluster Number). */
		Lcn = Data.StartingLcn;
		Index = 0;
		Mask = 1;
		IndexMax = sizeof(Data.Buffer);
		if (Data.BitmapSize / 8 < IndexMax) IndexMax = (int)(Data.BitmapSize / 8);
		while (Index < IndexMax) {
			InUse = (Data.Buffer[Index] & Mask);
			if ((PrevInUse == 0) && (InUse != 0)) {
				if ((ClusterStart >= MinimumLcn) &&
					(Lcn - ClusterStart >= MinimumSize)) {
					*BeginLcn = ClusterStart;
					if (EndLcn != NULL) *EndLcn = Lcn;
					return 1;
				}
			}
			if ((PrevInUse != 0) && (InUse == 0)) ClusterStart = Lcn;
			PrevInUse = InUse;
			if (Mask == 128) {
				Mask = 1;
				Index = Index + 1;
			}
			else {
				Mask = Mask << 1;
			}
			Lcn = Lcn + 1;
		}

	} while ((Result == ERROR_MORE_DATA) &&
		(Lcn < Data.StartingLcn + Data.BitmapSize));

	if (PrevInUse == 0) {
		if ((ClusterStart >= MinimumLcn) &&
			(Lcn - ClusterStart >= MinimumSize)) {
			*BeginLcn = ClusterStart;
			if (EndLcn != NULL) *EndLcn = Lcn;
			return 1;
		}
	}
	return 0;
}

int GetClusters()
{
	MOVE_FILE_DATA MoveParams;

	ULONG  ClCount;
	LARGE_INTEGER FileSize;
	HANDLE  hFile;
	ULONG   OutSize;
	ULONG   Bytes;
	LARGE_INTEGER PrevVCN;
	STARTING_VCN_INPUT_BUFFER  InBuf;
	PRETRIEVAL_POINTERS_BUFFER OutBuf;

	hFile = CreateFile(L"I:\\try.jpg", FILE_READ_ATTRIBUTES,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
//	char namef[MAX_PATH];
//	sprintf(namef, "\\\\.\\%c:", name[0]);
	HANDLE hDisk = CreateFile(L"\\\\.\\I:", GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	MoveParams.FileHandle = hFile;

	if (hFile != INVALID_HANDLE_VALUE)
	{
		GetFileSizeEx(hFile, &FileSize);
		
		//22.08
		OutSize = (ULONG)sizeof(RETRIEVAL_POINTERS_BUFFER) + (FileSize.QuadPart / ClusterSize) * sizeof(OutBuf->Extents);
		OutBuf = (PRETRIEVAL_POINTERS_BUFFER)malloc(OutSize + 1);
		InBuf.StartingVcn.QuadPart = 0;
		if (DeviceIoControl(hFile, FSCTL_GET_RETRIEVAL_POINTERS, &InBuf, sizeof(InBuf), OutBuf, OutSize, &Bytes, NULL))
		{
			ClCount = (FileSize.QuadPart + ClusterSize - 1) / ClusterSize;
			
			//ProcessVolume(name[0]);
			ULONG64 BLcn = -1, ELcn = -1;
			FindFreeBlock(hDisk, 0, 32, &BLcn, &ELcn);
			MoveParams.ClusterCount = 0;
			MoveParams.StartingLcn.QuadPart = BLcn;
			PrevVCN = OutBuf->StartingVcn;
			size_t k = sizeof(MoveParams);
			MoveParams.StartingVcn.QuadPart = 0;
			for (ULONG r = 0; r < 2/*OutBuf->ExtentCount+1-1*/; r++)
			{
				DWORD br;
				//MoveParams.ClusterCount = OutBuf->Extents[r].NextVcn.QuadPart - PrevVCN.QuadPart;
				MoveParams.ClusterCount = 32;
				if (DeviceIoControl(hDisk, FSCTL_MOVE_FILE, &MoveParams, sizeof(MoveParams), NULL, 0, &br, NULL))
					printf("error %d\n", GetLastError());
				printf("error %d\n", GetLastError());
				//MoveParams.StartingLcn.QuadPart = MoveParams.StartingLcn.QuadPart + MoveParams.ClusterCount;
				MoveParams.StartingVcn.QuadPart = MoveParams.StartingVcn.QuadPart + MoveParams.ClusterCount;
				//OutBuf->ExtentCount++;
				//OutBuf->Extents[r].NextVcn.QuadPart = PrevVCN.QuadPart + MoveParams.ClusterCount;
				//PrevVCN = OutBuf->Extents[r].NextVcn;
				BLcn = -1, ELcn = -1;
				FindFreeBlock(hDisk, 0, 32, &BLcn, &ELcn);
				MoveParams.StartingLcn.QuadPart = BLcn;
				printf("ok");
			}

		}
		free(OutBuf);
		CloseHandle(hFile);
		CloseHandle(hDisk);
	}
	return 1;
}

void main()
{
	find_max_clus();
	Search();
	GetClusters();
	ULONG64 BLcn = -1, ELcn = -1;
	HANDLE hDisk = CreateFile(L"\\\\.\\I:", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (FindFreeBlock(hDisk, 0, 32, &BLcn, &ELcn)){
		printf("its ok");
	};
	CloseHandle(hDisk);
	
	printf("WWOW");
	return;
}
