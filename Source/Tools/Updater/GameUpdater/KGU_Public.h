////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KGU_Public.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-5-18 17:02:07
//  Comment     : Public define and functions.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGU_PUBLIC_H_
#define _KGU_PUBLIC_H_

#include <tchar.h>

////////////////////////////////////////////////////////////////////////////////
// @brief : Download HTTP file by WinINet.
int KGU_DownloadHttpFile(const TCHAR cszURL[], const TCHAR cszFileName[]);

int KGU_GetRunningProcessCount(const TCHAR cszProcessName[], unsigned *puRetCount);

////////////////////////////////////////////////////////////////////////////////
// @brief  : Compare two version number.
// @result : false indicate either the version numbers with wrong format.
// @param pnRetResult : return the compare result. 
//                       0 indicate both equal.
//                      -1 indicate left less than right.
//                       1 indicate left greater than right.
int KGU_CompareVersionNumber(const TCHAR cszLeft[], const TCHAR cszRight[], int *pnRetResult);

#endif