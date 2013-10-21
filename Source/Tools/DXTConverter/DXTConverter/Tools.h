//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   Tools.h    
//  Version     :   1.0
//  Creater     :   DengWenfeng
//  Date        :   2009-4-3 14:36
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _Tools_H__
#define _Tools_H__

namespace Tools 
{
    int IsPathExist(char szFilePath[], int nSize);
    int Trim(char szSource[], int nSize);
    int FindFirstNotChar(const char cszSource[], char cKey);
    int FindLastNotChar(const char cszSource[], char cKey);
    int MaskMatch(const char cszSrc[], int nSrcLen, const char cszDst[], int nDstLen);
    int SlashPath(char szPath[], int nSize);
}

#endif //_Tools_H__