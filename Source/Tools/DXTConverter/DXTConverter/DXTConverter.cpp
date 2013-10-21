#include "stdafx.h"
#include "DXTConverter.h"
#include <d3dx9.h>
#include <time.h>
#include "Tools.h"

KMutex g_DxtLock;

KGDXTConverterManager::KGDXTConverterManager()
{
    m_vctFileList.clear();
    m_vctMapName.clear();
    m_vctDXTConverterList.clear();
    m_mapThreadState.clear();
    m_vctCheckPaths.clear();
    m_vctFilterPaths.clear();
    m_vctFilterFiles.clear();
    m_vctConvertorFiles.clear();
    m_piErrorTextureFile = NULL;
    m_piFileterTextureFile = NULL;
    m_pConverterLogFile = NULL;
    m_nSuccessNum = 0;
    m_nFailedNum = 0;
    m_nFilterNum = 0;
    m_bAutoRun = false;
}

KGDXTConverterManager::~KGDXTConverterManager()
{
    UnInit();
}

int KGDXTConverterManager::Init(const char cszIniPath[], int nCommandFlag)
{
    int nResult = false;
    int nRetCode = false;
    int nOpenErrorTextureFile = false;
    int nOpenFilterTextureFile = false;
    int nLen = 0;
    struct tm *pCurrentTime = NULL;
    time_t TimeValue = 0;
    char szResultFile[MAX_PATH];
    char szDir[MAX_PATH];
    char szIniName[MAX_PATH];
    szResultFile[0] = '\0';
    szDir[0] = '\0';
    szIniName[0] = '\0';

    nRetCode = _access(TGA_TO_DDS_EXE, 0);
    KGLOG_PROCESS_ERROR(!nRetCode && "texconv.exe is not exists in current path!");

    strncpy(szIniName, INI_FILE, sizeof(szIniName));
    szIniName[sizeof(szIniName) - 1] = '\0';
    if (nCommandFlag)
    {
        KGLOG_PROCESS_ERROR(cszIniPath && cszIniPath[0]);
        strncpy(szIniName, cszIniPath, sizeof(szIniName));

        nRetCode = Tools::SlashPath(szIniName, sizeof(szIniName));
        KGLOG_PROCESS_ERROR(nRetCode && "IniPath: '\' or '/' failed!");

        nRetCode = _snprintf_s(
            szIniName,
            sizeof(szIniName) - 1,
            "%s%s",
            szIniName,
            INI_FILE
        );
        _ASSERTE(nRetCode <= sizeof(szIniName) - 1);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szIniName[sizeof(szIniName) - 1] = '\0';
    }
    
    nRetCode = _LoadConfig(szIniName);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadConfig() failed!");

    TimeValue = time(NULL);
    pCurrentTime = localtime(&TimeValue);
    KGLOG_PROCESS_ERROR(pCurrentTime);

    nRetCode = _snprintf_s(
        szDir,
        sizeof(szDir) - 1,        
        "%.4d_%.2d_%.2d_%.2d_%.2d_%.2d",
        pCurrentTime->tm_year + 1900,
        pCurrentTime->tm_mon + 1,
        pCurrentTime->tm_mday,
        pCurrentTime->tm_hour,
        pCurrentTime->tm_min,
        pCurrentTime->tm_sec
        );
    _ASSERTE(nRetCode <= sizeof(szDir) - 1);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    szDir[sizeof(szDir) - 1] = '\0';

    nRetCode = _mkdir(szDir);
    KGLOG_PROCESS_ERROR(!nRetCode && "_mkdir() error!");

    nRetCode = _snprintf_s(
        szResultFile,
        sizeof(szResultFile) - 1,
        "%s\\ErrorTexture.txt",
        szDir
        );
    _ASSERTE(nRetCode <= sizeof(szResultFile) - 1);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    szResultFile[sizeof(szResultFile) - 1] = '\0';

    m_piErrorTextureFile = g_OpenFile(szResultFile, false, true);
    KGLOG_PROCESS_ERROR(m_piErrorTextureFile && "open ErrorTextureFile failed!");

    nRetCode = _snprintf_s(
        szResultFile,
        sizeof(szResultFile) - 1,        
        "%s\\FilterTexture.txt",
        szDir
        );
    ASSERT(nRetCode <= sizeof(szResultFile) - 1);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    szResultFile[sizeof(szResultFile) - 1] = '\0';

    m_piFileterTextureFile = g_OpenFile(szResultFile, false, true);
    KGLOG_PROCESS_ERROR(m_piFileterTextureFile && "Open FilterTexture failed!");

    nRetCode = _snprintf_s(
        szResultFile,
        sizeof(szResultFile) - 1,        
        "%s\\ConverterLog.txt",
        szDir
    );
    ASSERT(nRetCode <= sizeof(szResultFile) - 1);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    szResultFile[sizeof(szResultFile) - 1] = '\0';

    errno_t err = freopen_s(&m_pConverterLogFile, szResultFile, "wb", stdout);
    KGLOG_PROCESS_ERROR(err == 0);
    KGLOG_PROCESS_ERROR(m_pConverterLogFile);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (m_piErrorTextureFile)
        {
            m_piErrorTextureFile->Close();
            m_piErrorTextureFile->Release();
            m_piErrorTextureFile = NULL;
        }
        if (m_piFileterTextureFile)
        {
            m_piFileterTextureFile->Close();
            m_piFileterTextureFile->Release();
            m_piFileterTextureFile = NULL;
        }
        if (m_pConverterLogFile)
        {
            fclose(m_pConverterLogFile);
            m_pConverterLogFile = NULL;
        }
    }
    return nResult;   
}

int KGDXTConverterManager::UnInit()
{
    int nRetCode = false;

    if (m_piErrorTextureFile)
    {
        m_piErrorTextureFile->Close();
        m_piErrorTextureFile->Release();
        m_piErrorTextureFile = NULL;
    }
    if (m_piFileterTextureFile)
    {
        m_piFileterTextureFile->Close();
        m_piFileterTextureFile->Release();
        m_piFileterTextureFile = NULL;
    }
    if (m_pConverterLogFile)
    {
        fclose(m_pConverterLogFile);
        m_pConverterLogFile = NULL;
    }
    m_vctFileList.clear();
    m_vctMapName.clear();
    m_vctCheckPaths.clear();
    m_vctFilterPaths.clear();
    m_vctFilterFiles.clear();
    m_vctConvertorFiles.clear();
    m_mapThreadState.clear();

    int nSize = (int)m_vctDXTConverterList.size();
    for (int i = 0; i < nSize; i++)
    {
        if (m_vctDXTConverterList[i])
        {
            nRetCode = m_vctDXTConverterList[i]->UnInit();
            if (!nRetCode)
            {
                KGLogPrintf(KGLOG_INFO, "Thread %d Uninit() failed!", i);
            }
            KG_DELETE(m_vctDXTConverterList[i]);
        }
    }
    m_vctDXTConverterList.clear();

    return true;
}

int KGDXTConverterManager::Run()
{
    int nResult = false;
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    int nTotalNum = 0; 
    int nThreadNum = 0;
    int nNumFiles = MIN_NUM_FILES_OF_ONE_THREAD;
    vector<string> vctThreadFileList;    
    list<string>::iterator it;

    KGLOG_PROCESS_ERROR(!m_vctDXTConverterList.size());
    KGLOG_PROCESS_ERROR(!m_mapThreadState.size());

    printf("Begin find all files...\n");
    KGLogPrintf(KGLOG_INFO, "Begin find all files...");

    KGLogPrintf(KGLOG_INFO, "CheckPaths are:");
    for (int i = 0; i < (int)m_vctCheckPaths.size(); i++)
    {
        KGLogPrintf(KGLOG_INFO, "%s", m_vctCheckPaths[i].c_str());

        nRetCode = _FindAllFile(m_vctCheckPaths[i].c_str());
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nTotalNum = (int)m_vctFileList.size();
    printf("Starting convertion, Total %d files...\n", nTotalNum);
    KGLogPrintf(KGLOG_INFO, "Starting convertion, Total %d files.", nTotalNum);

    nThreadNum = nTotalNum / nNumFiles;
    while (nThreadNum > MAX_NUM_THREAD)
    {
        nNumFiles *= 2;
        nThreadNum = nTotalNum / nNumFiles;
    }

    if ((nTotalNum % nNumFiles) != 0)
    {
        nThreadNum++;
    }
    
    it = m_vctFileList.begin();
    for (int i = 0; i < nThreadNum; i++)
    {
        vctThreadFileList.clear();
        for (int j = 0; j < nNumFiles; j++)
        {
            if (it == m_vctFileList.end())
                break;

            vctThreadFileList.push_back((*it));
            it++;
        }

        KGDXTConverter *pDXTConverter = new KGDXTConverter;
        _ASSERTE(pDXTConverter && "new KGDXTConverter failed!");
        if (!pDXTConverter)
        {
            break;
        }
        
        nRetCode = pDXTConverter->Init(this, i, vctThreadFileList);
        if (!nRetCode)
        {
            KG_DELETE(pDXTConverter);
            break;
        }
        
        m_vctDXTConverterList.push_back(pDXTConverter);
        m_mapThreadState.insert(make_pair(i, THREAD_CREATE));
    }

    while (true)
    {
        if (_GetExitFlag())
            break;

        KGThread_Sleep(1000);
    }

    printf("All convertion over!\n");
    KGLogPrintf(KGLOG_INFO, "All convertion over!");

    nResult = true;
Exit0:    
    if (!nResult)
    {
        printf("All convertion failed!");
        KGLogPrintf(KGLOG_INFO, "All convertion failed!");
    }
    KGLogPrintf(KGLOG_INFO, "TotalNum:%d, SuccessNum:%d, FailedNum:%d, FilterNum:%d", nTotalNum, m_nSuccessNum, m_nFailedNum, m_nFilterNum);
    if (nTotalNum != (m_nSuccessNum + m_nFailedNum + m_nFilterNum) || m_nFailedNum != 0)
    {
        MessageBox(NULL, "Converter failed! please check the logs!", NULL, MB_OK);
    }
    return nResult;    
}

int KGDXTConverterManager::SetExitFlag(int nThreadID, THREAD_STATE nFlag)
{
    int nResult = false;
    int nRetCode = false;    
    map<int, THREAD_STATE>::iterator it;

    m_ThreadStateLock.Lock();

    it = m_mapThreadState.find(nThreadID);
    _ASSERTE(it != m_mapThreadState.end());

    if (it != m_mapThreadState.end())
    {
        (*it).second = nFlag;
    }

    nResult = true;
//Exit0:
    m_ThreadStateLock.Unlock();
    return nResult;
}

int KGDXTConverterManager::SetConverterResult(int nSuccessNum, int nFailedNum, int nFilterNum)
{
    m_NumLock.Lock();
    
    m_nSuccessNum += nSuccessNum;
    m_nFailedNum += nFailedNum;
    m_nFilterNum += nFilterNum;

    m_NumLock.Unlock();
    return true;
}

int KGDXTConverterManager::_GetExitFlag()
{   
    int nResult = false;

   map<int, THREAD_STATE>::iterator it;
    for (it = m_mapThreadState.begin(); it != m_mapThreadState.end(); it++)
    {
        if ((*it).second != THREAD_EXIT)
        {
            KG_PROCESS_ERROR(false);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KGDXTConverterManager::_FindAllFile(const char cszPath[])
{
    int		nResult				= false;
    int		nRetCode			= false;
    bool	bIsCurrentPath		= false;
    bool	bIsPreviousPath		= false;

    int		nPathLength			= 0;
    char	szExcutePath[MAX_PATH];
    char	szBufferPath[MAX_PATH];
    char	szTempPath[MAX_PATH];
    HANDLE  hFile				= NULL;
    WIN32_FIND_DATA strFileDate = {0};

    szExcutePath[0] = '\0';
    szBufferPath[0] = '\0';
    szTempPath[0]	= '\0';

    KGLOG_PROCESS_ERROR(cszPath);

    strncpy(szBufferPath, cszPath, sizeof(szBufferPath));
    nPathLength = (int)strlen(szBufferPath);

    nRetCode = Tools::SlashPath(szBufferPath, sizeof(szBufferPath));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = _snprintf_s(szExcutePath, sizeof(szExcutePath) - 1, "%s*", szBufferPath);
    _ASSERTE(nRetCode <= sizeof(szExcutePath) -1 );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    szExcutePath[sizeof(szExcutePath) - 1] = 0;

    hFile = FindFirstFile(szExcutePath, &strFileDate);
    KGLOG_PROCESS_ERROR(INVALID_HANDLE_VALUE != hFile);

    do 
    {
        if (strFileDate.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)     //directory
        {
            bIsCurrentPath	= false;
            bIsPreviousPath	= false;

            nRetCode = _stricmp(strFileDate.cFileName, ".");
            if (nRetCode == 0)
            {
                bIsCurrentPath	= true;
            }

            nRetCode = _stricmp(strFileDate.cFileName, "..");
            if (nRetCode == 0)
            {
                bIsPreviousPath	= true;
            }

            if (!bIsCurrentPath && !bIsPreviousPath)
            {
                nRetCode = _stricmp(strFileDate.cFileName, ".svn");
                if (nRetCode != 0)
                {
                    nRetCode = _snprintf_s(szTempPath, sizeof(szTempPath) - 1, "%s%s", szBufferPath, strFileDate.cFileName);
                    _ASSERTE(nRetCode <= sizeof(szTempPath) - 1);
                    KGLOG_PROCESS_ERROR(nRetCode > 0);
                    szTempPath[sizeof(szTempPath) - 1] = '\0';

                    nRetCode = _FindAllFile(szTempPath);
                    KGLOG_PROCESS_ERROR(nRetCode);
                }
            }
        }
        else
        {
            nRetCode = Tools::MaskMatch(strFileDate.cFileName, (int)strlen(strFileDate.cFileName), ".dds", (int)strlen(".dds")) || 
                       Tools::MaskMatch(strFileDate.cFileName, (int)strlen(strFileDate.cFileName), ".tga", (int)strlen(".tga"));
            if (nRetCode)
            {
                nRetCode = _snprintf_s(szTempPath, sizeof(szTempPath) - 1, "%s%s", szBufferPath, strFileDate.cFileName);
                _ASSERTE(nRetCode <= sizeof(szTempPath) - 1);
                KGLOG_PROCESS_ERROR(nRetCode > 0);
                szTempPath[sizeof(szTempPath) - 1] = '\0';
                
                _ASSERTE(m_vctFileList.size() < m_vctFileList.max_size());

                m_vctFileList.push_back(string(szTempPath));
            }                               
        }

        nRetCode = FindNextFile(hFile, &strFileDate);
        if (!nRetCode)
        {
            DWORD dwError = 0;
            dwError = GetLastError();
            KG_PROCESS_SUCCESS(dwError == ERROR_NO_MORE_FILES);
        }
        
        KGLOG_PROCESS_ERROR(nRetCode);
    } while(nRetCode);

Exit1:
    nResult = true;
Exit0:
    if (hFile)
    {
        FindClose(hFile);
        hFile = NULL;
    }
    return nResult;
}

int KGDXTConverterManager::_LoadConfig(const char cszFileName[])
{
    int nResult = false;
    int nRetCode = false;
    int nMaxPathNum = 100;
    int nAutoRun = 0;
    int nLen = 0;
    int nMatchPathCount = 0;
    char szValue[MAX_PATH];
    char szKey[sizeof("path_100") + 1];
    char szFilterFileName[MAX_PATH];
    char szConvertorFileName[MAX_PATH];
    char szMaplistPath[MAX_PATH];
    szValue[0] = '\0';
    szKey[0] = '\0';
    szFilterFileName[0] = '\0';
    szConvertorFileName[0] = '\0';
    szMaplistPath[0] = '\0';

    IIniFile *piIniFile = NULL;

    KGLOG_PROCESS_ERROR(cszFileName && cszFileName[0]);

    piIniFile = g_OpenIniFile(cszFileName);
    KGLOG_PROCESS_ERROR(piIniFile);

    nRetCode = piIniFile->GetString("Base", "MaplistPath", "", szValue, sizeof(szValue));
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(szValue[0] && "MaplistPath is null!");
    strncpy(szMaplistPath, szValue, sizeof(szMaplistPath));
    szMaplistPath[sizeof(szMaplistPath) - 1] = '\0';

    nRetCode = _access(szMaplistPath, 0);
    KGLOG_PROCESS_ERROR(nRetCode == 0 && "MaplistPath is not exist!");

    nRetCode = _ReadMapList(szMaplistPath);
    KGLOG_PROCESS_ERROR(nRetCode && "_ReadMapList() failed!");

    nRetCode = piIniFile->GetInteger("Base", "AutoRun", 0, &nAutoRun);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    m_bAutoRun = (nAutoRun == 0 ? false : true);

    m_vctCheckPaths.clear();
    for (int i = 0; i < nMaxPathNum; i++)
    {
        nRetCode = _snprintf_s(
            szKey,
            sizeof(szKey) - 1,
            "path_%.2d",
            i + 1
        );
        _ASSERTE(nRetCode <= sizeof(szKey) - 1);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szKey[sizeof(szKey) - 1] = '\0';

        nRetCode = piIniFile->GetString("CheckPath", szKey, "", szValue, sizeof(szValue));
        if (!nRetCode)
        {
            if (i == 0)
                KGLOG_PROCESS_ERROR(false && "DXTConvertor.ini,[CheckPath] or path_01 is not exist!");    
            
            break;
        }

        KGLOG_PROCESS_ERROR((i + 1 < nMaxPathNum) && "CheckPath Num is over 100!");
        KGLOG_PROCESS_ERROR(szValue[0] != '\0' && "CheckPath can not be null!");
        szValue[sizeof(szValue) - 1] = '\0';

        nRetCode = Tools::SlashPath(szValue, sizeof(szValue));
        KGLOG_PROCESS_ERROR(nRetCode && "CheckPath: '\' or '/' failed!");

        nRetCode = _access(szValue, 0);
        if (nRetCode != 0)
        {
            KGLogPrintf(KGLOG_ERR, "CheckPath:%s is not exist!", szValue);
            KGLOG_PROCESS_ERROR(false);
        }

        m_vctCheckPaths.push_back(string(szValue));
    }   
    KGLOG_PROCESS_ERROR(m_vctCheckPaths.size() != 0 && "CheckPath is empty!");
    
    nRetCode = _AnalyseCheckPath(m_vctCheckPaths);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_vctFilterPaths.clear();
    for (int i = 0; i < nMaxPathNum; i++)
    {       
        nRetCode = _snprintf_s(
            szKey,
            sizeof(szKey) - 1,
            "path_%.2d",
            i + 1
        );
        _ASSERTE(nRetCode <= sizeof(szKey) - 1);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szKey[sizeof(szKey) - 1] = '\0';

        nRetCode = piIniFile->GetString("FilterPath", szKey, "", szValue, sizeof(szValue));
        if (!nRetCode)
        {
            if (i == 0)
                KGLOG_PROCESS_ERROR(false && "DXTConvertor.ini,[FilterPath] or path_01 is not exist!");    

            break;
        }

        KGLOG_PROCESS_ERROR((i + 1 < nMaxPathNum) && "FilterPath Num is over 100!");
        
        if (szValue[0] == '\0' && i == 0)
        {
            break;
        }

        KGLOG_PROCESS_ERROR(szValue[0] != '\0' && "FilterPath can not be null!");        
        szValue[sizeof(szValue) - 1] = '\0';

        nRetCode = Tools::SlashPath(szValue, sizeof(szValue));
        KGLOG_PROCESS_ERROR(nRetCode && "FilterPath: '\' or '/' failed!");

        if (szValue[1] == ':')
        {
            KGLogPrintf(KGLOG_ERR, "FilterPath: %s can not be fullpath!", szValue);
            KGLOG_PROCESS_ERROR(false);
        }

        nRetCode = _IsExistInCheckPath(szValue, "", &nMatchPathCount);
        if (!nRetCode)
        {
            KGLogPrintf(KGLOG_ERR, "FilterPath: %s failed in CheckPath!", szValue);
            KGLOG_PROCESS_ERROR(false);
        }

        m_vctFilterPaths.push_back(string(szValue));
    }

    nRetCode = piIniFile->GetString("FilterFile", "filepath", "", szFilterFileName, sizeof(szFilterFileName));
    KGLOG_PROCESS_ERROR(nRetCode && "DXTConvertor.ini,[FilterFile] or filepath is not exist");
    szFilterFileName[sizeof(szFilterFileName) - 1] = '\0';

    if (szFilterFileName[0])
    {
        nRetCode = _ReadFilterFiles(szFilterFileName);
        KGLOG_PROCESS_ERROR(nRetCode && "_ReadFilterFiles() failed!");
    }

    nRetCode = piIniFile->GetString("ConvertorFile", "filepath", "", szConvertorFileName, sizeof(szConvertorFileName));
    KGLOG_PROCESS_ERROR(nRetCode && "DXTConvertor.ini,[ConvertorFile] or filepath is not exist");
    szConvertorFileName[sizeof(szConvertorFileName) - 1] = '\0';

    if (szConvertorFileName[0])
    {
        nRetCode = _ReadConvertorFiles(szConvertorFileName);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    if (piIniFile)
    {
        piIniFile->Release();
        piIniFile = NULL;
    }
    return nResult;
}

int KGDXTConverterManager::_ReadFilterFiles(const char cszFileName[])
{
    int nResult = false;
    int nRetCode = false;
    int nHeight = -1;
    int nMatchPathCount = -1;
    char szFullPath[MAX_PATH];
    char *pszPoint = NULL;
    FILTER_FILE FilterFiles;
    ITabFile *piTableFile = NULL;
    szFullPath[0] = '\0';

    KGLOG_PROCESS_ERROR(cszFileName && cszFileName[0]);

    m_vctFilterFiles.clear();

    piTableFile = g_OpenTabFile(cszFileName);
    KGLOG_PROCESS_ERROR(piTableFile && "Open filter file failed!");

    nHeight = piTableFile->GetHeight();
    KGLOG_PROCESS_SUCCESS(nHeight == 1);
    KGLOG_PROCESS_ERROR(nHeight >= 2);

    for (int i = 2; i <= nHeight; i++)
    {
        ZeroMemory(&FilterFiles, sizeof(FILTER_FILE));

        nRetCode = piTableFile->GetString(i, "name", "", FilterFiles.szName, sizeof(FilterFiles.szName));
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(FilterFiles.szName[0] && "FilterFiles is empty!");

        nRetCode = Tools::Trim(FilterFiles.szName, sizeof(FilterFiles.szName));
        KGLOG_PROCESS_ERROR(nRetCode && "Tools::Trim() failed!");

        pszPoint = strrchr(FilterFiles.szName, '.');
        KGLOG_PROCESS_ERROR(pszPoint && "FilterFile do not have extname!");

        nRetCode = _stricmp(pszPoint, ".tga");
        KGLOG_PROCESS_ERROR(nRetCode == 0 && "FilterFile is not tga file!");

        nRetCode = piTableFile->GetString(i, "path", "", FilterFiles.szPath, sizeof(FilterFiles.szPath));
        KGLOG_PROCESS_ERROR(nRetCode);

        if (FilterFiles.szPath[0])
        {
            nRetCode = Tools::Trim(FilterFiles.szPath, sizeof(FilterFiles.szPath));
            KGLOG_PROCESS_ERROR(nRetCode && "Tools::Trim() failed!");

            nRetCode = Tools::SlashPath(FilterFiles.szPath, sizeof(FilterFiles.szPath));
            KGLOG_PROCESS_ERROR(nRetCode && "FilterFiles: '\' or '/' failed!");
            
            if (FilterFiles.szPath[1] == ':')
            {
                KGLogPrintf(KGLOG_ERR, "FilterFiles path: %s can not be fullpath!", FilterFiles.szPath);
                KGLOG_PROCESS_ERROR(false);
            }

            nMatchPathCount = -1;
            nRetCode = _IsExistInCheckPath(FilterFiles.szPath, FilterFiles.szName, &nMatchPathCount);
            if (!nRetCode)
            {
                KGLogPrintf(KGLOG_ERR, "FilterFile: %s\\%s failed in CheckPath!", FilterFiles.szPath, FilterFiles.szName);
                //自动构建，文件名和路径都是有可能不存在的，这里我们只记日志，同时不作为过滤的文件
                if (m_bAutoRun && (nMatchPathCount == 0))
                {
                    continue;
                }

                KGLOG_PROCESS_ERROR(false);                
            }
        }

        m_vctFilterFiles.push_back(FilterFiles);
    }

Exit1:
    nResult = true;
Exit0:
    if (piTableFile)
    {
        piTableFile->Release();
        piTableFile = NULL;
    }
    if (!nResult)
    {
        KGLogPrintf(KGLOG_ERR, "FilterFile:%s load failed!", cszFileName);
    }  
    return nResult;
}

int KGDXTConverterManager::_ReadConvertorFiles(const char cszFileName[])
{
    int nResult = false;
    int nRetCode = false;
    int nHeight = -1;
    int nMatchPathCount = -1;
    char szFullPath[MAX_PATH];
    char *pszPoint = NULL;
    CONVERTOR_FILE ConvertorFiles;
    ITabFile *piTableFile = NULL;
    szFullPath[0] = '\0';

    KGLOG_PROCESS_ERROR(cszFileName && cszFileName[0]);

    m_vctConvertorFiles.clear();

    piTableFile = g_OpenTabFile(cszFileName);
    KGLOG_PROCESS_ERROR(piTableFile && "Open convertor file failed!");

    nHeight = piTableFile->GetHeight();
    KGLOG_PROCESS_SUCCESS(nHeight == 1);
    KGLOG_PROCESS_ERROR(nHeight >= 2);

    for (int i = 2; i <= nHeight; i++)
    {
        ZeroMemory(&ConvertorFiles, sizeof(CONVERTOR_FILE));

        nRetCode = piTableFile->GetString(i, "name", "", ConvertorFiles.szName, sizeof(ConvertorFiles.szName));
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(ConvertorFiles.szName[0] && "ConvertorFiles name is empty!");

        nRetCode = Tools::Trim(ConvertorFiles.szName, sizeof(ConvertorFiles.szName));
        KGLOG_PROCESS_ERROR(nRetCode && "Tools::Trim() failed!");
        
        pszPoint = strrchr(ConvertorFiles.szName, '.');
        KGLOG_PROCESS_ERROR(pszPoint && "ConvertorFile do not have extname!");

        nRetCode = _stricmp(pszPoint, ".tga");
        KGLOG_PROCESS_ERROR(nRetCode == 0 && "ConvertorFile is not tga file!");

        nRetCode = piTableFile->GetString(i, "path", "", ConvertorFiles.szPath, sizeof(ConvertorFiles.szPath));
        KGLOG_PROCESS_ERROR(nRetCode);

        if (ConvertorFiles.szPath[0])
        {
            nRetCode = Tools::Trim(ConvertorFiles.szPath, sizeof(ConvertorFiles.szPath));
            KGLOG_PROCESS_ERROR(nRetCode && "Tools::Trim() failed!");
            
            nRetCode = Tools::SlashPath(ConvertorFiles.szPath, sizeof(ConvertorFiles.szPath));
            KGLOG_PROCESS_ERROR(nRetCode && "ConvertorFiles: '\' or '/' failed!");

            if (ConvertorFiles.szPath[1] == ':')
            {
                KGLogPrintf(KGLOG_ERR, "ConvertorFiles path: %s can not be fullpath!", ConvertorFiles.szPath);
                KGLOG_PROCESS_ERROR(false);
            }
            
            nMatchPathCount = -1;
            nRetCode = _IsExistInCheckPath(ConvertorFiles.szPath, ConvertorFiles.szName, &nMatchPathCount);
            if (!nRetCode)
            {
                KGLogPrintf(KGLOG_ERR, "ConvertorFile: %s\\%s failed in CheckPath!", ConvertorFiles.szPath, ConvertorFiles.szName);
                //自动构建，文件名和路径都是有可能不存在的，这里我们只记日志，同时不作为过滤的文件
                if (m_bAutoRun && (nMatchPathCount == 0))
                {
                    continue;
                }

                KGLOG_PROCESS_ERROR(false);
            }
        }

        nRetCode = piTableFile->GetInteger(i, "DXT", 0, &ConvertorFiles.nDXT);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR((ConvertorFiles.nDXT <= 5) && (ConvertorFiles.nDXT >= 1));

        m_vctConvertorFiles.push_back(ConvertorFiles);
    }

Exit1:
    nResult = true;
Exit0:
    if (piTableFile)
    {
        piTableFile->Release();
        piTableFile = NULL;
    }
    if (!nResult)
    {
        KGLogPrintf(KGLOG_ERR, "ConvertorFile:%s load failed!", cszFileName);
    }  
    return nResult;
}

int KGDXTConverterManager::_ReadMapList(const char cszFilePath[])
{
    int nResult = false; 
    int nRetCode = false;
    char szMapListFullPath[MAX_PATH];
    ITabFile *piTableFile = NULL;
    int nHeight = -1;
    char szName[128];
    szName[0] = '\0';
    szMapListFullPath[0] = '\0';

    KGLOG_PROCESS_ERROR(cszFilePath && cszFilePath[0]);

    m_vctMapName.clear();

    strncpy(szMapListFullPath, cszFilePath, sizeof(szMapListFullPath));
    szMapListFullPath[sizeof(szMapListFullPath) - 1] = '\0';
    
    nRetCode = Tools::SlashPath(szMapListFullPath, sizeof(szMapListFullPath));
    KGLOG_PROCESS_ERROR(nRetCode && "MaplistPath: '\' or '/' failed!");

    nRetCode = _snprintf_s(
        szMapListFullPath, 
        sizeof(szMapListFullPath) - 1, 
        "%sMapList.tab", 
        szMapListFullPath
    );
    _ASSERTE(nRetCode <= sizeof(szMapListFullPath) - 1);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    szMapListFullPath[sizeof(szMapListFullPath) - 1] = '\0';

    piTableFile = g_OpenTabFile(szMapListFullPath);
    KGLOG_PROCESS_ERROR(piTableFile && "Open MapList.tab failed!");

    nHeight = piTableFile->GetHeight();
    //KGLOG_PROCESS_SUCCESS(nHeight == 1);
    KGLOG_PROCESS_ERROR(nHeight >= 2);

    for (int i = 2; i <= nHeight; i++)
    {
        nRetCode = piTableFile->GetString(i, "Name", "", szName, sizeof(szName));
        KGLOG_PROCESS_ERROR(nRetCode == 1);
        KGLOG_PROCESS_ERROR(szName[0]);
    
        nRetCode = Tools::Trim(szName, sizeof(szName));
        KGLOG_PROCESS_ERROR(nRetCode && "Tools::Trim() failed!");

        m_vctMapName.push_back(string(szName));
    }

//Exit1:
    nResult = true;
Exit0:
    if (piTableFile)
    {
        piTableFile->Release();
        piTableFile = NULL;
    }
    if (!nResult)
    {
        KGLogPrintf(KGLOG_ERR, "MapList.tab:%s load failed!", szMapListFullPath);
    }    
    return nResult;
}

int KGDXTConverterManager::_IsExistInCheckPath(const char cszFilePath[], const char cszFileName[], int *pnMatchPathCount)
{
    int nResult = false;
    int nRetCode = false;
    int nSize = 0;
    int nLen = 0;
    int nMatchPathCount = 0;
    char *pszSlash = NULL;
    char *pszFirstPos = NULL;
    char *pszSubCheckPath = NULL;
    char szFullPath[MAX_PATH];
    char szCheckPath[MAX_PATH];
    char szFilePathFirstDir[MAX_PATH];
    char szTempFilePath[MAX_PATH];
    szFullPath[0] = '\0';
    szCheckPath[0] = '\0';
    szFilePathFirstDir[0] = '\0';   
    szTempFilePath[0] = '\0';

    KGLOG_PROCESS_ERROR(cszFilePath && cszFilePath[0]);
    KGLOG_PROCESS_ERROR(cszFileName);
    KGLOG_PROCESS_ERROR(m_vctCheckPaths.size());
    KGLOG_PROCESS_ERROR(pnMatchPathCount);
    
    strncpy(szTempFilePath, cszFilePath, sizeof(szTempFilePath));
    szTempFilePath[sizeof(szTempFilePath) - 1] = '\0';

    nRetCode = Tools::SlashPath(szTempFilePath, sizeof(szTempFilePath));
    KGLOG_PROCESS_ERROR(nRetCode);

    pszSlash = strchr(szTempFilePath, '\\');
    KGLOG_PROCESS_ERROR((pszSlash != &szTempFilePath[0]) && "First char cannot be '\\'");

    strncpy(szFilePathFirstDir, szTempFilePath, pszSlash - szTempFilePath);
    szFilePathFirstDir[pszSlash - szTempFilePath] = '\0';

    nRetCode = _strlwr_s(szFilePathFirstDir, (int)strlen(szFilePathFirstDir) +1);
    KGLOG_PROCESS_ERROR(nRetCode == 0);  

    nSize = (int)m_vctCheckPaths.size();
    for (int i = 0; i < nSize; i++)
    {
        strncpy(szCheckPath, m_vctCheckPaths[i].c_str(), sizeof(szCheckPath));
        szCheckPath[sizeof(szCheckPath) - 1] = '\0';

        nLen = (int)strlen(szCheckPath);        
        
        //CheckPath is root path(d:)
        if (nLen <= 3)
        {
            nRetCode = _snprintf_s(
                szFullPath,
                sizeof(szFullPath) - 1,
                "%s%s\\%s",
                szCheckPath,
                cszFilePath,
                cszFileName
            );
            _ASSERTE(nRetCode <= sizeof(szFullPath) - 1);
            KGLOG_PROCESS_ERROR(nRetCode > 0);
            szFullPath[sizeof(szFullPath) - 1] = '\0';

            nRetCode = _access(szFullPath, 0);
            if (nRetCode == 0)
            {
                nMatchPathCount++;
            }
            continue;
        }
        
        nRetCode = _strlwr_s(szCheckPath, (int)strlen(szCheckPath) + 1);
        KGLOG_PROCESS_ERROR(nRetCode == 0);
    
        szFullPath[0] = '\0';
        pszSubCheckPath = szCheckPath;
        while (true)
        {
            pszFirstPos = strstr(pszSubCheckPath, szFilePathFirstDir);
            if (!pszFirstPos) 
                break;

            pszSubCheckPath = pszFirstPos + (int)strlen(szFilePathFirstDir);

            if (pszFirstPos == &szCheckPath[0]) 
            {
                continue;
            }

            if (*(--pszFirstPos) != '\\')
            {
                continue;
            }

            ++pszFirstPos;

            nRetCode = Tools::MaskMatch(cszFilePath, (int)strlen(cszFilePath), pszFirstPos, (int)strlen(pszFirstPos));
            if (nRetCode)
            {
                *pszFirstPos = '\0';
                nRetCode = _snprintf_s(
                    szFullPath,
                    sizeof(szFullPath) - 1,
                    "%s%s\\%s",
                    szCheckPath,
                    cszFilePath,
                    cszFileName
                 );
                _ASSERTE(nRetCode <= sizeof(szFullPath) - 1);
                KGLOG_PROCESS_ERROR(nRetCode > 0);
                szFullPath[sizeof(szFullPath) - 1] = '\0';
                break;
            }
        }

        if (!szFullPath[0])
            continue;

        nRetCode = _access(szFullPath, 0);
        if (nRetCode == 0)
        {
            nMatchPathCount++;
        }
    }
    
    KGLOG_PROCESS_ERROR(nMatchPathCount == 1);

    nResult = true;
Exit0:
    if (pnMatchPathCount)
    {
        *pnMatchPathCount = nMatchPathCount;
    }
    
    if (!nResult)
    {
        if (nMatchPathCount != 1)
        {
            KGLogPrintf(KGLOG_ERR, "there are %d CheckPath match of %s\\%s", nMatchPathCount, cszFilePath, cszFileName);
        }
    }
    return nResult;
}

int KGDXTConverterManager::_AnalyseCheckPath(vector<string> &vctCheckPaths)
{
    int nResult = false;
    int nRetCode = false;
    int nSize = 0;
    map<string, int> mapMaxPath;
    map<string, int>::iterator it;
    string strMaxPath;

    KGLOG_PROCESS_ERROR(vctCheckPaths.size());
    KG_PROCESS_SUCCESS(vctCheckPaths.size() == 1);
    KGLOG_PROCESS_ERROR(vctCheckPaths.size() >= 2);

    nSize = (int)vctCheckPaths.size();
    for (int i = 0; i < nSize; i++)
    {
        strMaxPath = vctCheckPaths[i];
        for (int j = 0; j < nSize; j++)
        {
            if (i == j)
                continue;

            nRetCode = Tools::MaskMatch(
                strMaxPath.c_str(), (int)strMaxPath.length(), 
                vctCheckPaths[j].c_str(), (int)vctCheckPaths[j].length()
            );
            if (nRetCode)
                strMaxPath = vctCheckPaths[j];
        }

        mapMaxPath.insert(make_pair(strMaxPath, 0));
    }
    _ASSERTE(mapMaxPath.size());

    vctCheckPaths.clear();
    for (it = mapMaxPath.begin(); it != mapMaxPath.end(); it++)
    {
        vctCheckPaths.push_back((*it).first);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KGDXTConverter::KGDXTConverter()
{
    m_vctFileList.clear();
    m_vctMapName.clear();
    m_vctFilterPaths.clear();
    m_vctFilterFiles.clear();
    m_vctConvertorFiles.clear();
    m_piErrorTextureFile = NULL;
    m_piFilterTextureFile = NULL;
    m_pDXTConverterManager = NULL;
    m_nThreadID = -1;
    m_nCreateThreadFlag = false;
    m_strDXT = string("-dxt5");
}

KGDXTConverter::~KGDXTConverter()
{
    UnInit();
}

int KGDXTConverter::Init(
     KGDXTConverterManager *pDXTConverter, 
     int nThreadID,
     vector<string> vctFileList
)
{
    int nResult = false;
    int nRetCode = false;
    
    KGLOG_PROCESS_ERROR(pDXTConverter);

    m_pDXTConverterManager = pDXTConverter;
    m_nThreadID = nThreadID;
    m_vctFileList = vctFileList;

    nRetCode = m_pDXTConverterManager->GetMapName(m_vctMapName);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pDXTConverterManager->GetFilterPaths(m_vctFilterPaths);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pDXTConverterManager->GetFilterFiles(m_vctFilterFiles);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pDXTConverterManager->GetConvertorFiles(m_vctConvertorFiles);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_piErrorTextureFile = m_pDXTConverterManager->GetErrorTextureFile();
    KGLOG_PROCESS_ERROR(m_piErrorTextureFile);

    m_piFilterTextureFile = m_pDXTConverterManager->GetFilterTextureFile();
    KGLOG_PROCESS_ERROR(m_piFilterTextureFile);

    nRetCode = m_WorkThread.Create(_WorkThread, this);
    KGLOG_PROCESS_ERROR(nRetCode && "m_WorkThread.Create() error!");
    m_nCreateThreadFlag = true;    

    nResult = true;
Exit0:
    if (!nResult)
    {
        UnInit();
    }
    return nResult;
}

int KGDXTConverter::UnInit()
{
    int nResult = false;
    int nRetCode = false;

    m_piErrorTextureFile = NULL;
    m_piFilterTextureFile = NULL;
    m_vctFileList.clear();
    m_vctFilterPaths.clear();
    m_vctFilterFiles.clear();
    m_vctConvertorFiles.clear();
    m_vctMapName.clear();
    
    if (m_nCreateThreadFlag)
    {
        nRetCode = m_WorkThread.Destroy();
        KGLOG_PROCESS_ERROR(nRetCode && "KThread::Destroy() failed!");

        m_nCreateThreadFlag = false;
    }
    
    nResult = true;
Exit0:
    return nResult;
}

void KGDXTConverter::_WorkThread(void *pvArg)
{
    _ASSERTE(pvArg);

    KGDXTConverter *pThis = (KGDXTConverter *)pvArg;

    pThis->_ThreadFunction();
}

int KGDXTConverter::_ThreadFunction()
{
    int nResult = false;
    int nRetCode = false;
    int nLen = 0;
    HRESULT hr = E_FAIL;
    int nTotalNum = 0;
    int nSuccessNum = 0;
    int nFailedNum = 0;
    int nFilterNum = 0;
    char szDriver[MAX_PATH];
    char szPath[MAX_PATH];
    char szFileName[MAX_PATH];
    char szOutputPath[MAX_PATH];
    char szCommandLine[1024];
    char szExt[FILE_EXT_NAME_LEN];    
    STARTUPINFO StartInfo;
    ZeroMemory(&StartInfo, sizeof(StartInfo));
    StartInfo.cb = sizeof(StartInfo);

    PROCESS_INFORMATION ProcessInfo;  
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
    string strFile;    

    KGLOG_PROCESS_ERROR(m_vctFileList.size());

    nTotalNum = (int)m_vctFileList.size();

    for (int i = 0; i < nTotalNum; i++)
    {
        //printf("ThreadID:%d ========================[%d/%d]===================\n", m_nThreadID, i + 1, nTotalNum);
        //KGLogPrintf(KGLOG_INFO, "ThreadID:%d ========================[%d/%d]===================\n", m_nThreadID, i + 1, nTotalNum);

        strFile = m_vctFileList[i];

        //过滤路径:m_vctFilterPaths路径下的所有TGA文件，DDS文件不过滤，要做检查
        nRetCode = _CheckFilterPath(strFile.c_str());
        if (nRetCode)
        {
            nFilterNum++;
            continue;
        }

        //过滤贴图资源
        nRetCode = _CheckFilterFile(strFile.c_str());
        if (nRetCode)
        {
            nFilterNum++;
            continue;
        }

        nRetCode = _CheckTexture(strFile.c_str());
        if (!nRetCode)
        {
            nFilterNum++;
            continue;
        }

        nRetCode = _splitpath_s(strFile.c_str(),
            szDriver,
            sizeof(szDriver) - 1,
            szPath,
            sizeof(szPath) - 1,
            szFileName,
            sizeof(szFileName) - 1,
            szExt,
            sizeof(szExt) - 1
        );
        KGLOG_PROCESS_ERROR(nRetCode == 0);
        szDriver[sizeof(szDriver) - 1] = '\0';
        szPath[sizeof(szPath) - 1] = '\0';
        szFileName[sizeof(szFileName) - 1] = '\0';
        szExt[sizeof(szExt) - 1] = '\0';

        if (_stricmp(szExt, ".dds") == 0)
        {
            nFilterNum++;
            continue;
        }

        m_strDXT = "-dxt5";
        nRetCode = _ResetDXT(strFile.c_str());
        KGLOG_PROCESS_ERROR(nRetCode && "_ResetDXT() failed!");

        nLen = (int)strlen(szPath);
        if (szPath[nLen - 1] == '\\' || szPath[nLen - 1] == '/')
        {
            szPath[nLen - 1] = '\0';
        }

        nRetCode = _snprintf_s(szOutputPath, sizeof(szOutputPath) - 1, "\"%s%s\"", szDriver, szPath);
        _ASSERTE(nRetCode <= sizeof(szOutputPath) - 1);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szOutputPath[sizeof(szOutputPath) - 1] = '\0';

        nRetCode = _snprintf_s(
            szCommandLine,
            sizeof(szCommandLine) - 1,
            "%s -file \"%s\" %s -outdir %s",
            TGA_TO_DDS_EXE,
            strFile.c_str(),
            m_strDXT.c_str(),
            szOutputPath            
        );
        _ASSERTE(nRetCode <= sizeof(szCommandLine) - 1);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szCommandLine[sizeof(szCommandLine) - 1] = '\0';        

        g_DxtLock.Lock();

        nRetCode = CreateProcess(
            TGA_TO_DDS_EXE,
            szCommandLine,
            NULL, 
            NULL,
            FALSE,
            NORMAL_PRIORITY_CLASS,
            NULL,
            NULL, 
            &StartInfo, 
            &ProcessInfo
        );
        if (!nRetCode)
        {
            DWORD dwError = 0;
            dwError = GetLastError();
            KGLogPrintf(KGLOG_ERR, "ThreadID:%d,TGA:%s, ErrorCode: %d", m_nThreadID, strFile.c_str(), dwError);
            KGLOG_PROCESS_ERROR(false);
        }
               
        DWORD dwExitCode = -1;
        DWORD dwBegin = GetTickCount();
        while (true)
        {   
            nRetCode = GetExitCodeProcess(ProcessInfo.hProcess, &dwExitCode);
            KGLOG_PROCESS_ERROR(nRetCode);

            if (dwExitCode != STILL_ACTIVE)
                break;

            DWORD dwEnd = GetTickCount();
            if (dwEnd - dwBegin >= WAIT_TIME)
                break;

            KGThread_Sleep(10);
        }

        if (ProcessInfo.hThread)
        {
            CloseHandle(ProcessInfo.hThread);
            ProcessInfo.hThread = NULL;
        }
        if (ProcessInfo.hProcess)
        {
            CloseHandle(ProcessInfo.hProcess); 
            ProcessInfo.hProcess = NULL;
        }

        g_DxtLock.Unlock();         
       
        if (dwExitCode != 0)
        {
            nFailedNum++;
            KGLogPrintf(KGLOG_ERR, "ThreadID:%d,TGA:%s, ExitCode: %d", m_nThreadID, strFile.c_str(), dwExitCode);
            continue;
        }

        nRetCode = _tremove(strFile.c_str());
        if (nRetCode != 0)
        {
            DWORD dwError = 0;
            dwError = GetLastError();
            LPSTR lpBuffer;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_IGNORE_INSERTS |
                FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                dwError, 
                LANG_NEUTRAL,
                (LPTSTR) & lpBuffer,
                0,
                NULL
                );
            nFailedNum++;
            KGLogPrintf(KGLOG_ERR, "ThreadID:%d,remove file failed: %s, %s", m_nThreadID ,strFile.c_str(), lpBuffer);
        }
        else
        {
            nSuccessNum++;
            KGLogPrintf(KGLOG_INFO, "ThreadID:%d,convertion DDS success:%s, %s", m_nThreadID, strFile.c_str(), m_strDXT.c_str());
        }
        
    }

    KGLogPrintf(KGLOG_INFO, "ThreadID:%d,convertion over!", m_nThreadID);

    nResult = true;
Exit0:    
    if (ProcessInfo.hThread)
    {
        CloseHandle(ProcessInfo.hThread);
        ProcessInfo.hThread = NULL;
    }
    if (ProcessInfo.hProcess)
    {
        CloseHandle(ProcessInfo.hProcess); 
        ProcessInfo.hProcess = NULL;
    }
    
    KGLogPrintf(
        KGLOG_INFO, 
        "ThreadID:%d, TotalNum:%d, SuccessNum:%d, FailedNum:%d, FilterNum:%d", 
        m_nThreadID, nTotalNum, nSuccessNum, nFailedNum, nFilterNum
    );
    
    nRetCode = m_pDXTConverterManager->SetConverterResult(nSuccessNum, nFailedNum, nFilterNum);
    _ASSERTE(nRetCode);

    nRetCode = m_pDXTConverterManager->SetExitFlag(m_nThreadID, THREAD_EXIT);
    _ASSERTE(nRetCode);
    return nResult;
}

int KGDXTConverter::_IsMinimapFile(const char cszFilePath[])
{
    int nResult = false;
    int nRetCode = -1;
    int nLen = 0;
    int nSize = 0;
    int nUnlineCount = 0;
    int nMinimapFlag = false;
    char szMinimapPath[MAX_PATH];
    char szFileName[MAX_PATH];
    szMinimapPath[0] = '\0';
    szFileName[0] = '\0';

    KG_PROCESS_ERROR(cszFilePath && cszFilePath[0]);

    nSize = (int)m_vctMapName.size();
    for (int i = 0; i < nSize; i++)
    {
        nRetCode = _snprintf_s(
            szMinimapPath,
            sizeof(szMinimapPath) - 1,
            "\\data\\source\\maps\\%sminimap\\",
            m_vctMapName[i].c_str()
            );
        _ASSERTE(nRetCode <= sizeof(szMinimapPath) - 1);
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        nRetCode = Tools::MaskMatch(cszFilePath, (int)strlen(cszFilePath), szMinimapPath, (int)strlen(szMinimapPath));
        if (nRetCode)
        {
            nMinimapFlag = true;
            break;
        }
    }

    KG_PROCESS_ERROR(nMinimapFlag);

    nRetCode = _splitpath_s(cszFilePath,
        NULL,
        0,
        NULL,
        0,
        szFileName,
        sizeof(szFileName) - 1,
        NULL,
        0
        );
    KGLOG_PROCESS_ERROR(nRetCode == 0);
    szFileName[sizeof(szFileName) - 1] = '\0';

    nLen = (int)strlen(szFileName);
    for (int i = 0; i < nLen; i++)
    {
        if (szFileName[i] == '_')
        {
            nUnlineCount++;
        }
    }

    KG_PROCESS_ERROR(nUnlineCount == 1);

//Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KGDXTConverter::_IsLightMapFile(const char cszFilePath[])
{
    int nResult = false;
    int nRetCode = false;
    int nSize = 0;
    char szLightMapFile[MAX_PATH];
    szLightMapFile[0] = '\0';
    
    KG_PROCESS_ERROR(cszFilePath && cszFilePath[0]);

    nSize = (int)m_vctMapName.size();
    for (int i = 0; i < nSize; i++)
    {
        //data\source\maps\地图\地图.tga
        nRetCode = _snprintf_s(
            szLightMapFile,
            sizeof(szLightMapFile) - 1,
            "\\data\\source\\maps\\%s\\%s.tga",
            m_vctMapName[i].c_str(),
            m_vctMapName[i].c_str()
        );
        _ASSERTE(nRetCode <= sizeof(szLightMapFile) - 1);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szLightMapFile[sizeof(szLightMapFile) -1 ] = '\0';

        nRetCode = Tools::MaskMatch(cszFilePath, (int)strlen(cszFilePath), szLightMapFile, (int)strlen(szLightMapFile));
        KG_PROCESS_SUCCESS(nRetCode);

        //data\source\maps\地图\地图_TerrainMap.tga
        nRetCode = _snprintf_s(
            szLightMapFile,
            sizeof(szLightMapFile) - 1,
            "\\data\\source\\maps\\%s\\%s_TerrainMap.tga",
            m_vctMapName[i].c_str(),
            m_vctMapName[i].c_str()
        );
        _ASSERTE(nRetCode <= sizeof(szLightMapFile) - 1);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szLightMapFile[sizeof(szLightMapFile) -1 ] = '\0';

        nRetCode = Tools::MaskMatch(cszFilePath, (int)strlen(cszFilePath), szLightMapFile, (int)strlen(szLightMapFile));
        KG_PROCESS_SUCCESS(nRetCode);
    }
    KG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult =true;
Exit0:
    return nResult;
}

int KGDXTConverter::_CheckTexture(const char cszFilePath[])
{
    int nResult = true;
    int nRetCode = false;
    UINT uWidth = 0;
    UINT uHeight = 0;
    HRESULT hRetCode = E_FAIL;
    D3DXIMAGE_INFO Info;
    int nCheckError = false;
    int nWidthErrorFlag = false;
    int nHeightErrorFlag = false;
    int nWidthHeightRatioFlag = false;
    int nMinHeightAndWidthFlag = false;

    KGLOG_PROCESS_ERROR(cszFilePath && cszFilePath[0]);
    KGLOG_PROCESS_ERROR(m_piErrorTextureFile);

    ZeroMemory(&Info, sizeof(D3DXIMAGE_INFO));
    hRetCode = D3DXGetImageInfoFromFile(cszFilePath, &Info);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    assert(Info.Width && Info.Height);

    uWidth = Info.Width;
    uHeight = Info.Height;

    if (uWidth < 4 || uHeight < 4)
    {
        nCheckError = true;
        nMinHeightAndWidthFlag = true;
        nResult = false;
        KG_PROCESS_ERROR(false);
    }

    nRetCode = ((uWidth & (uWidth - 1)) == 0);
    if (!nRetCode)
    {
        nCheckError = true;
        nWidthErrorFlag = true;
        nResult = false;
        //KGLogPrintf(KGLOG_INFO, "%s", "贴图宽不是2的N次幂");
    }

    nRetCode = ((uHeight & (uHeight - 1)) == 0);
    if (!nRetCode)
    {
        nCheckError = true;
        nHeightErrorFlag = true;
        nResult = false;
        //KGLogPrintf(KGLOG_INFO, "%s", "贴图高不是2的N次幂");    
    }

    nRetCode = ((uWidth / uHeight) > 4) || 
               ((uHeight / uWidth) > 4) ||
               (((uWidth / uHeight) == 4) && ((uWidth % uHeight) != 0)) ||
               (((uHeight / uWidth) == 4) && ((uHeight % uWidth) != 0));
    if (nRetCode)
    {
        nCheckError = true;
        nWidthHeightRatioFlag = true;
        nResult = false;
        //KGLogPrintf(KGLOG_INFO, "%s", "贴图宽高比例大于4");    
    }

Exit0:
    if (!nResult)
    {
        if (nCheckError)
        {
            string strMsg = string(cszFilePath);
            if (nMinHeightAndWidthFlag)
            {
                strMsg += string("\t贴图的长或宽长度小于4");
            }
            else
            {
                if (nWidthErrorFlag)
                {
                    strMsg += string("\t贴图宽不是2的N次幂");
                }
                if (nHeightErrorFlag)
                {
                    strMsg += string("\t贴图高不是2的N次幂");
                }
                if (nWidthHeightRatioFlag)
                {
                    strMsg += string("\t贴图宽高比例大于4");
                }
            }
            strMsg += string("\n");
            m_piErrorTextureFile->Write(strMsg.c_str(), (unsigned long)strMsg.length());
        }
    }
    return nResult;
}


int KGDXTConverter::_CheckFilterPath(const char cszFilePath[])
{
    int nResult = false;
    int nRetCode = false;
    int nFilterPathFlag = false;
    int nSize = 0;
    char szExt[FILE_EXT_NAME_LEN];
    szExt[0] = '\0';

    KGLOG_PROCESS_ERROR(cszFilePath && cszFilePath[0]);
    KGLOG_PROCESS_ERROR(m_piFilterTextureFile);

    nSize = (int)m_vctFilterPaths.size();
    for (int i = 0; i < nSize; i++)
    {
        nRetCode = Tools::MaskMatch(cszFilePath, (int)strlen(cszFilePath), m_vctFilterPaths[i].c_str(), (int)m_vctFilterPaths[i].length());
        if (nRetCode)
        {
            nFilterPathFlag = true;
            break;
        }
    }
    
    KG_PROCESS_ERROR(nFilterPathFlag);

    nRetCode = _splitpath_s(
        cszFilePath,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0,
        szExt,
        sizeof(szExt) - 1
        );
    KGLOG_PROCESS_ERROR(nRetCode == 0);
    szExt[sizeof(szExt) - 1] = '\0';

    nRetCode = _stricmp(szExt, ".tga");
    KGLOG_PROCESS_ERROR((nRetCode == 0) && "filter path but is dds file!");

    nResult = true;
Exit0:
    if (nResult)
    {
        string strMsg = string(cszFilePath) + string("\n");

        m_piFilterTextureFile->Write(strMsg.c_str(), (unsigned long)strMsg.length());
    }
    return nResult;
}

int KGDXTConverter::_CheckFilterFile(const char cszFilePath[])
{
    int nResult = false;
    int nRetCode = false;
    int nLen = 0;
    int nSize = 0;
    char szFilterFile[MAX_PATH];
    char szPath[MAX_PATH];
    char szExt[FILE_EXT_NAME_LEN];
    szFilterFile[0] = '\0';
    szPath[0] = '\0';
    szExt[0] = '\0';

    KGLOG_PROCESS_ERROR(cszFilePath && cszFilePath[0]);
    KGLOG_PROCESS_ERROR(m_piFilterTextureFile);

    nRetCode = _splitpath_s(
        cszFilePath,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0,
        szExt,
        sizeof(szExt) - 1
        );
    KGLOG_PROCESS_ERROR(nRetCode == 0);
    szExt[sizeof(szExt) - 1] = '\0';

    nRetCode = _stricmp(szExt, ".dds");
    KG_PROCESS_ERROR(nRetCode != 0);

    nSize = (int)m_vctFilterFiles.size();
    for (int i = 0; i < nSize; i++)
    {
        _ASSERTE(m_vctFilterFiles[i].szName[0]);

        strncpy(szPath, m_vctFilterFiles[i].szPath, sizeof(szPath));
        szPath[sizeof(szPath) - 1] = '\0';
        
        nLen = (int)strlen(szPath);
        if (nLen != 0)
        {
            szPath[nLen - 1] = '\0';
        }

        nRetCode = _snprintf_s(
            szFilterFile,
            sizeof(szFilterFile) -1,
            "%s\\%s",
            szPath,
            m_vctFilterFiles[i].szName
        );
        _ASSERTE(nRetCode <= sizeof(szFilterFile) - 1);
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        nRetCode = Tools::MaskMatch(cszFilePath, (int)strlen(cszFilePath), szFilterFile, (int)strlen(szFilterFile));
        KG_PROCESS_SUCCESS(nRetCode);
    }
    
    //data\source\maps\*(地图名)\*(地图名).tga
    nRetCode = _IsLightMapFile(cszFilePath);
    KG_PROCESS_SUCCESS(nRetCode);

    //data\source\maps\*(地图名)minimap\*_*.tga
    nRetCode = _IsMinimapFile(cszFilePath);
    KG_PROCESS_SUCCESS(nRetCode);

    KG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    if (nResult)
    {
        string strMsg = string(cszFilePath) + string("\n");
        
        m_piFilterTextureFile->Write(strMsg.c_str(), (unsigned long)strMsg.length());
    }
    return nResult;
}

int KGDXTConverter::_ResetDXT(const char cszFilePath[])
{
    int nResult = false;
    int nRetCode = false;
    int nLen = 0;
    int nSize = 0;
    char szPath[MAX_PATH];
    char szConvertorFile[MAX_PATH];
    char szDXT[sizeof("-dxt5") + 1];
    szPath[0] = '\0';
    szConvertorFile[0] = '\0';
    szDXT[0] = '\0';

    KGLOG_PROCESS_ERROR(cszFilePath && cszFilePath[0]);

    nSize = (int)m_vctConvertorFiles.size();
    for (int i = 0; i < nSize; i++)
    {
        _ASSERTE(m_vctConvertorFiles[i].szName[0]);

        strncpy(szPath, m_vctConvertorFiles[i].szPath, sizeof(szPath));
        szPath[sizeof(szPath) - 1] = '\0';

        nLen = (int)strlen(szPath);
        if (nLen != 0)
        {
            szPath[nLen - 1] = '\0';            
        }

        nRetCode = _snprintf_s(
            szConvertorFile,
            sizeof(szConvertorFile) -1,
            "%s\\%s",
            szPath,
            m_vctConvertorFiles[i].szName
        );
        _ASSERTE(nRetCode <= sizeof(szConvertorFile) - 1);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szConvertorFile[sizeof(szConvertorFile) - 1] = '\0';

        nRetCode = Tools::MaskMatch(cszFilePath, (int)strlen(cszFilePath), szConvertorFile, (int)strlen(szConvertorFile));
        if (nRetCode)
        {
            nRetCode = _snprintf_s(
                szDXT,
                sizeof(szDXT) - 1,
                "-dxt%d",
                m_vctConvertorFiles[i].nDXT
            );
            _ASSERTE(nRetCode <= sizeof(szDXT) - 1);
            KGLOG_PROCESS_ERROR(nRetCode > 0);
            szDXT[sizeof(szDXT) - 1] = '\0';

            m_strDXT = string(szDXT);

            break;
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

