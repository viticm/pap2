////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KGU_SelfUpdater.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-11 10:21:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KGU_SelfUpdater.h"
#include "KGU_Public.h"

////////////////////////////////////////////////////////////////////////////////
struct _LANG_CODEPAGE   // Used for get gameupdater version number.
{
    WORD wLanguage;
    WORD wCodepage;
};

////////////////////////////////////////////////////////////////////////////////
KGU_SelfUpdater::KGU_SelfUpdater(void)
{
}

KGU_SelfUpdater::~KGU_SelfUpdater(void)
{
}

int KGU_SelfUpdater::Init(
    const TCHAR cszApplicationName[],
    const TCHAR cszApplicationPath[],
    const TCHAR cszTempFilePath[],
    void *pvContext
)
{
    int nResult  = false;
    int nRetCode = false;
    tstring strDownloadURL;

    KGLOG_PROCESS_ERROR(cszApplicationName);
    KGLOG_PROCESS_ERROR(cszApplicationName[0]);
    KGLOG_PROCESS_ERROR(cszApplicationPath);
    KGLOG_PROCESS_ERROR(cszApplicationPath[0]);
    KGLOG_PROCESS_ERROR(cszTempFilePath);
    KGLOG_PROCESS_ERROR(cszTempFilePath[0]);

    KG_USE_ARGUMENT(pvContext);

    m_strApplicationName = cszApplicationName;
    m_strApplicationPath = cszApplicationPath;
    m_strTempFilePath    = cszTempFilePath;

    nRetCode = _LoadDownloadURL(&strDownloadURL);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadDownloadURL()");

    m_strSelfUpdateURLPrefix = strDownloadURL + _T("updaterV2/");

    nResult = true;
Exit0:
    return nResult;
}

int KGU_SelfUpdater::UnInit(void *pvContext)
{
    KG_USE_ARGUMENT(pvContext);
    return true;
}

int KGU_SelfUpdater::CheckNeedUpdate(int *pnRetNeedUpdateFlag)
{
    int nResult  = false;
    int nRetCode = false;
    tstring strUpdateInfoFileURL;
    tstring strSelfVersionNumber;
    tstring strLastVersionNumber;
    int nVersionComapreResult = 0;

    KGLOG_PROCESS_ERROR(pnRetNeedUpdateFlag);

    strUpdateInfoFileURL    = m_strSelfUpdateURLPrefix + _T("updaterversion.txt");
    m_strUpdateInfoFileName = m_strTempFilePath        + _T("updaterversion.txt");

    nRetCode = KGU_DownloadHttpFile(strUpdateInfoFileURL.c_str(), m_strUpdateInfoFileName.c_str());
    KGLOG_PROCESS_ERROR(nRetCode && "KGU_DownloadHttpFile()");

    nRetCode = _GetLastVersionNumber(&strLastVersionNumber);
    KGLOG_PROCESS_ERROR(nRetCode && "_GetLastVersionNumber()");

    nRetCode = _GetSelfVersionNumber(&strSelfVersionNumber);
    KGLOG_PROCESS_ERROR(nRetCode && "_GetSelfVersionNumber()");

    nRetCode = KGU_CompareVersionNumber(
        strSelfVersionNumber.c_str(), strLastVersionNumber.c_str(), &nVersionComapreResult
    );
    KGLOG_PROCESS_ERROR(nRetCode && "KGU_CompareVersionNumber()");

    if (nVersionComapreResult < 0)  // Only if self version is less than last version, need update.
        *pnRetNeedUpdateFlag = true;
    else
        *pnRetNeedUpdateFlag = false;

    nResult = true;
Exit0:
    return nResult;
}

int KGU_SelfUpdater::DownloadFiles()
{
    int nResult  = false;
    int nRetCode = false;

    nRetCode = _GetUpdateFileList();
    KGLOG_PROCESS_ERROR(nRetCode && "_GetUpdateFileList()");

    nRetCode = _DownloadFileList();
    KGLOG_PROCESS_ERROR(nRetCode && "_DownloadFileList()");
    
    nResult = true;
Exit0:
    if (!nResult)
        m_UpdateFileList.clear();
    return nResult;
}

int KGU_SelfUpdater::UpdateFiles()
{
    int nResult  = false;
    int nRetCode = false;
    UPDATE_FILE_LIST::iterator it;
    tstring strTargetFileName;
    tstring strTempFileName;
    tstring strNewFileName;

    nRetCode = m_UpdateFileList.empty();
    ASSERT(nRetCode == false);  // make sure call DownloadFiles() before calling UpdateFiles().

    // Move all file to temp file.
    for (it = m_UpdateFileList.begin(); it != m_UpdateFileList.end(); ++it)
    {
        strTargetFileName = m_strApplicationPath + *it;
        strTempFileName   = m_strTempFilePath    + *it + _T(".tmp");

        nRetCode = SetFileAttributes(strTempFileName.c_str(), FILE_ATTRIBUTE_NORMAL);
        // KGLOG_PROCESS_ERROR(nRetCode);    // Temp file may not exist.

        nRetCode = _MoveFile(strTargetFileName.c_str(), strTempFileName.c_str());
        // KGLOG_PROCESS_ERROR(nRetCode);    // The target file may not exist if this file is added newly.
    }

    for (it = m_UpdateFileList.begin(); it != m_UpdateFileList.end(); ++it)
    {
        strTargetFileName = m_strApplicationPath + *it;
        strNewFileName    = m_strTempFilePath    + *it;
 
        nRetCode = _MoveFile(strNewFileName.c_str(), strTargetFileName.c_str());
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KGU_SelfUpdater::_LoadDownloadURL(tstring *pstrRetURL)
{
    int nResult  = false;
    int nRetCode = false;
    tstring strConfigFileName;
    tstring strVersionType;
    TCHAR szBuffer[1024];

    ASSERT(pstrRetURL);

    strConfigFileName = m_strApplicationPath + _T("gameupdater.ini");

    nRetCode = GetPrivateProfileString(
        _T("server_1"), _T("URL"), NULL, szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]),
        strConfigFileName.c_str()
    );
    if (nRetCode > 0)
    {
        *pstrRetURL = szBuffer;
        KG_PROCESS_SUCCESS(true);
    }

    // If can't get URL information, then joint the ip with version type string.
    KGLogPrintf(KGLOG_DEBUG, "Can't load [server_1]URL from gameupdater.ini.");

    nRetCode = GetPrivateProfileString(
        _T("server_1"), _T("ip"), NULL, szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]), 
        strConfigFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && "Can't load [server_1]ip from gameupdater.ini.");
    
    *pstrRetURL = szBuffer;

    nRetCode = _LoadVersionType(&strVersionType);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadVersionName()");
    
    *pstrRetURL += strVersionType + _T('/');

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KGU_SelfUpdater::_LoadVersionType(tstring *pstrRetVersionType)
{
    int nResult  = false;
    int nRetCode = false;
    FILE *fpVersionFile = NULL;
    tstring strVersionFileName;
    tstring strVersionValue;
    tstring strVersionExValue;
    TCHAR szBuffer[1024];
    TCHAR *pchRetCode = NULL;
    const TCHAR VERSION_PREFIX[]    = _T("Sword3.VersionLineName=");
    const TCHAR VERSION_EX_PREFIX[] = _T("Sword3.versionex=");

    ASSERT(pstrRetVersionType);

    strVersionFileName = m_strApplicationPath + _T("version.cfg");
    
    fpVersionFile = _tfopen(strVersionFileName.c_str(), _T("r"));
    KGLOG_PROCESS_ERROR(fpVersionFile);

    while (true)
    {
        pchRetCode = _fgetts(szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]), fpVersionFile);
        if (pchRetCode == NULL)
        {
            nRetCode = ferror(fpVersionFile);
            if (nRetCode == 0)
                break;

            KGLogPrintf(KGLOG_DEBUG, "Fail to read file, Error code: %d.", nRetCode);
            KGLOG_PROCESS_ERROR(false);
        }

        nRetCode = _tcsncicmp(szBuffer, VERSION_PREFIX, _tcsclen(VERSION_PREFIX));
        if (nRetCode == 0)
        {
            strVersionValue = szBuffer + _tcsclen(VERSION_PREFIX);
            continue;
        }

        nRetCode = _tcsncicmp(szBuffer, VERSION_EX_PREFIX, _tcsclen(VERSION_EX_PREFIX));
        if (nRetCode == 0)
        {
            strVersionExValue = szBuffer + _tcsclen(VERSION_EX_PREFIX);
            continue;
        }
    }

    // Trim the string
    for (int i = (int)strVersionValue.length() - 1; i >= 0; i--)
    {

        if (
            (strVersionValue[i] == _T(' '))  ||
            (strVersionValue[i] == _T('\n')) ||
            (strVersionValue[i] == _T('\r')) ||
            (strVersionValue[i] == _T('\t'))
        )
            strVersionValue.erase(i);
        else
            break;        
    }
        
    for (int i = (int)strVersionExValue.length() - 1; i >= 0; i--)
    {

        if (
            (strVersionExValue[i] == _T(' '))  ||
            (strVersionExValue[i] == _T('\n')) ||
            (strVersionExValue[i] == _T('\r')) ||
            (strVersionExValue[i] == _T('\t'))
        )
            strVersionExValue.erase(i);
        else
            break;        
    }   

    nRetCode = strVersionValue.empty();
    KGLOG_PROCESS_ERROR(nRetCode == false);

    *pstrRetVersionType = strVersionValue;

    nRetCode = strVersionExValue.empty();
    if (nRetCode == false)
    {
        *pstrRetVersionType += _T('_') + strVersionExValue;
    }
    
    nResult = true;
Exit0:
    if (fpVersionFile)
    {
        fclose(fpVersionFile);
        fpVersionFile = NULL;
    }
    return nResult;
}

int KGU_SelfUpdater::_GetLastVersionNumber(tstring *pstrRetVersionNumber)
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szBuffer[1024];

    ASSERT(pstrRetVersionNumber);

    nRetCode = GetPrivateProfileString(
        _T("version"), _T("LatestVersion"), NULL, szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]),
        m_strUpdateInfoFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    *pstrRetVersionNumber = szBuffer;

    nResult = true;
Exit0:
    return nResult;
}

int KGU_SelfUpdater::_GetSelfVersionNumber(tstring *pstrRetVersionNumber)
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwVersionInfoSize = 0;
    unsigned char *pbyVersionInfo = NULL;
    _LANG_CODEPAGE *pLangCodepage = NULL;
    DWORD dwTranslation  = 0;
    DWORD dwBufferLength = 0;
    TCHAR szSubBlock[128];
    TCHAR *pszFileVersion = NULL;

    ASSERT(pstrRetVersionNumber);

    dwVersionInfoSize = GetFileVersionInfoSize(m_strApplicationName.c_str(), NULL);
    KGLOG_PROCESS_ERROR(dwVersionInfoSize);

    pbyVersionInfo = new(std::nothrow) unsigned char[dwVersionInfoSize];
    KGLOG_PROCESS_ERROR(pbyVersionInfo);

    nRetCode = GetFileVersionInfo(m_strApplicationName.c_str(), 0, dwVersionInfoSize, pbyVersionInfo);
    KGLOG_PROCESS_ERROR(nRetCode && "GetFileVersionInfo()");

    nRetCode = VerQueryValue(
        pbyVersionInfo, _T("\\VarFileInfo\\Translation"), 
        (LPVOID *)&pLangCodepage, (PUINT)&dwBufferLength
    );
    KGLOG_PROCESS_ERROR(nRetCode && "VerQueryValue(Translation)");

    nRetCode = _sntprintf(
        szSubBlock, sizeof(szSubBlock) / sizeof(szSubBlock[0]), 
        _T("\\StringFileInfo\\%04x%04x\\FileVersion"),
        pLangCodepage->wLanguage, pLangCodepage->wCodepage
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(szSubBlock) / sizeof(szSubBlock[0]));

    nRetCode = VerQueryValue(
        pbyVersionInfo, szSubBlock, (LPVOID *)&pszFileVersion, (PUINT)&dwBufferLength
    );
    KGLOG_PROCESS_ERROR(nRetCode && "VerQueryValue(FileInfo)");

    *pstrRetVersionNumber = pszFileVersion;

    nResult = true;
Exit0:
    if (!nResult)
        KGLogPrintf(KGLOG_DEBUG, "Failed to get self version number, Error code: %d.", GetLastError());
    KG_DELETE_ARRAY(pbyVersionInfo);
    return nResult;
}

int KGU_SelfUpdater::_GetUpdateFileList()
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uIndex = 0;
    TCHAR szKeyName[16];
    TCHAR szFileName[MAX_PATH];

    m_UpdateFileList.clear();

    uIndex = 1;
    while (true)
    {
        nRetCode = _sntprintf(
            szKeyName, sizeof(szKeyName) / sizeof(szKeyName[0]), _T("file_%d"), uIndex
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < sizeof(szKeyName) / sizeof(szKeyName[0]));

        nRetCode = GetPrivateProfileString(
            _T("filelist"), szKeyName, NULL, szFileName, sizeof(szFileName) / sizeof(szFileName[0]),
            m_strUpdateInfoFileName.c_str()
        );
        if (nRetCode <= 0)
            break;      // Indicate no more file.

        m_UpdateFileList.push_back(szFileName);

        uIndex++;
    }

    nRetCode = m_UpdateFileList.empty();
    KGLOG_PROCESS_ERROR(!nRetCode && "Update file list is empty.");

    nResult = true;
Exit0:
    if (!nResult)
        m_UpdateFileList.clear();
    return nResult;
}

int KGU_SelfUpdater::_DownloadFileList()
{
    int nResult  = false;
    int nRetCode = false;
    tstring strFileURL;
    tstring strFileName;
    UPDATE_FILE_LIST::iterator it;

    for (it = m_UpdateFileList.begin(); it != m_UpdateFileList.end(); ++it)
    {
        strFileURL  = m_strSelfUpdateURLPrefix + *it;
        strFileName = m_strTempFilePath        + *it;

        nRetCode = KGU_DownloadHttpFile(strFileURL.c_str(), strFileName.c_str());
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    
    nResult = true;
Exit0:
    return nResult;
}

int KGU_SelfUpdater::_MoveFile(const TCHAR cszSourceFileName[], const TCHAR cszTargetFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    const unsigned TRY_TIME_COUNT = 3;

    ASSERT(cszSourceFileName);
    ASSERT(cszSourceFileName[0]);
    ASSERT(cszTargetFileName);
    ASSERT(cszTargetFileName[0]);

    for (unsigned i = 0; i < TRY_TIME_COUNT; i++)
    {
        nRetCode = MoveFileEx(cszSourceFileName, cszTargetFileName, MOVEFILE_REPLACE_EXISTING);
        KG_PROCESS_SUCCESS(nRetCode);

        // Log the error information, and try again.
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to move file `%s` to `%s`, Error code : %d",
            cszSourceFileName, cszTargetFileName, GetLastError()
        );
        Sleep(500);
    }

    KG_PROCESS_ERROR(false);    // Here indicate failed to move.

Exit1:
    nResult = true;
Exit0:
    return nResult;
}