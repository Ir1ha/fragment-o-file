/*file fragment.h*/
#ifndef FRAGMENT_H
#define FRAGMENT_H

#include<stdio.h>
#include<Windows.h>
#include<conio.h>
#include<iostream>
#include <list>
#include<locale>
#include <string>
using namespace std;

DWORD SetFiles(int n, const char *name, WCHAR *DiskName, WCHAR *FileName);
void Search(const char *name);
void find_max_clus(WCHAR *DiskName);
int FindFreeBlock(HANDLE VolumeHandle, ULONG64 MinimumLcn, DWORD MinimumSize, ULONG64 *BeginLcn, ULONG64 *EndLcn);           
void GetClusters(DWORD Clusters, WCHAR *DiskName, WCHAR *FileName);

#endif FRAGMENT_H