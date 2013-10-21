//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGDXTConverter.h    
//  Version     :   1.0
//  Creater     :   DengWenfeng
//  Date        :   2009-2-25 19:55
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KGDXTConverter_H__
#define _KGDXTConverter_H__
#include "stdafx.h"
#include <atlstr.h>
#include <d3dx9.h>
#include <direct.h>
#include <vector>
#include <string>
#include <map>
#include <list>
using namespace std;

const int MIN_NUM_FILES_OF_ONE_THREAD = 500;
const int MAX_NUM_THREAD = 16;
const int FILE_EXT_NAME_LEN = 30;
const char INI_FILE[] = {"DXTConvertor.ini"};
const char TGA_TO_DDS_EXE[] = {"texconv.exe"};
const DWORD WAIT_TIME = 30 * 60 * 1000;

typedef enum _THREAD_STATE{
    THREAD_NONE,
    THREAD_CREATE,
    THREAD_EXIT
} THREAD_STATE;

typedef struct _FILTER_FILE 
{
    char szName[MAX_PATH];
    char szPath[MAX_PATH];
} FILTER_FILE;

typedef struct _CONVERTOR_FILE  
{
    char szName[MAX_PATH];
    char szPath[MAX_PATH];
    int nDXT;
} CONVERTOR_FILE;

class KGDXTConverter;

class KGDXTConverterManager 
{
public:
    KGDXTConverterManager();
    ~KGDXTConverterManager();
public:
    int Init(const char cszIniPath[], int nCommandFlag);
    int Run();
    int UnInit();

    int GetFilterPaths(vector<string> &vctFilterPaths)
    { 
        vctFilterPaths =  m_vctFilterPaths; 
        return true; 
    }
    int GetFilterFiles(vector<FILTER_FILE> &vctFilterFiles)
    {
        vctFilterFiles = m_vctFilterFiles;
        return true; 
    }
    int GetConvertorFiles(vector<CONVERTOR_FILE> &vctConvertorFiles)
    {
        vctConvertorFiles = m_vctConvertorFiles;
        return true;
    }
    int GetMapName(vector<string> &vctMapName)
    {
        vctMapName = m_vctMapName;
        return true;
    }

    IFile* GetErrorTextureFile(){ return m_piErrorTextureFile; }
    IFile* GetFilterTextureFile(){ return m_piFileterTextureFile; }
    int SetExitFlag(int nThreadID, THREAD_STATE nFlag);
    int SetConverterResult(int nSuccess, int nFailed, int nFilter);
private: 
    int _GetExitFlag();   
    int _FindAllFile(const char cszFilePath[]);   
    int _ReadMapList(const char cszFilePath[]);
    int _ReadFilterFiles(const char cszFileName[]);
    int _ReadConvertorFiles(const char cszFileName[]);
    int _LoadConfig(const char cszFileName[]);
    //FilePath in CheckPath or FilePath + FileName in CheckPath
    int _IsExistInCheckPath(const char cszFilePath[], const char cszFileName[], int *pnMatchPathCount);
    //filter same and child CheckPath
    int _AnalyseCheckPath(vector<string> &vctCheckPaths);
private:
    list<string> m_vctFileList; 
    vector<string> m_vctMapName;
    IFile *m_piErrorTextureFile;
    IFile *m_piFileterTextureFile;
    vector<KGDXTConverter *> m_vctDXTConverterList;
    map<int, THREAD_STATE> m_mapThreadState;
    KMutex m_ThreadStateLock;
    KMutex m_DXTConverterListLock;
    KMutex m_NumLock;
    vector<string> m_vctCheckPaths;
    vector<string> m_vctFilterPaths;
    vector<FILTER_FILE> m_vctFilterFiles;
    vector<CONVERTOR_FILE> m_vctConvertorFiles;
    int m_nSuccessNum;
    int m_nFailedNum;
    int m_nFilterNum;
    bool m_bAutoRun;
    FILE *m_pConverterLogFile;
};

class KGDXTConverter 
{
public:
    KGDXTConverter();
    ~KGDXTConverter();
public:
    int Init(
        KGDXTConverterManager *pDXTConverter, 
        int nThreadID,
        vector<string> vctFileList
    );
    int UnInit();
private:
    static void _WorkThread(void *pvArg);
    int _ThreadFunction(void);
    int _CheckTexture(const char cszFilePath[]);
    int _CheckFilterPath(const char cszFilePath[]);
    int _CheckFilterFile(const char cszFilePath[]);
    int _ResetDXT(const char cszFilePath[]);
    int _IsLightMapFile(const char cszFilePath[]);
    int _IsMinimapFile(const char cszFilePath[]);  
private:
    vector<string> m_vctFileList;
    vector<string> m_vctMapName;
    vector<string> m_vctFilterPaths;
    vector<FILTER_FILE> m_vctFilterFiles;
    vector<CONVERTOR_FILE> m_vctConvertorFiles;
    IFile *m_piErrorTextureFile;
    IFile *m_piFilterTextureFile;
    KThread m_WorkThread;
    KGDXTConverterManager *m_pDXTConverterManager;
    int m_nThreadID;
    int m_nCreateThreadFlag;
    string m_strDXT;
};

#endif //_KGDXTConverter_H__