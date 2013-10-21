////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_CompareVersionNumber.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-10-21  20:15:33
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_COMPARE_VERSION_NUMBER_H_
#define _KG_COMPARE_VERSION_NUMBER_H_

// @brief  : Compare two version number.
// @result : false indicate either the version numbers with wrong format.
// @param pnRetResult : return the compare result. 
//                       0 indicate both equal.
//                      -1 indicate left less than right.
//                       1 indicate left greater than right.
int KG_CompareVersionNumber(const TCHAR cszLeft[], const TCHAR cszRight[], int *pnRetResult);

#endif  // _KG_COMPARE_VERSION_NUMBER_H_