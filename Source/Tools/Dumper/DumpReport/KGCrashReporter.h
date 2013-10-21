//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGCrashReporter.h    
//  Version     :   1.0
//  Creater     :   DengWenfeng
//  Date        :   2009-3-2 20:37
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KGCrashReporter_H__
#define _KGCrashReporter_H__
const int MAX_CONTENT = 1024;

class KGCrashReporter 
{
public:
    KGCrashReporter();
    ~KGCrashReporter();
public:
    int UpdateCrashReport(const TCHAR cszFile[], const TCHAR cszKey[]);
    string TStrToUTF8(const tstring& src);
private:
    
};

#endif //_KGCrashReporter_H__