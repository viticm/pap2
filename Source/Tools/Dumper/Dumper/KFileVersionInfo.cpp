///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KFileVersionInfo.H
// Creator  : chengzhaohui
// Date     : 
// Comment  :
//
///////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "KFileVersionInfo.h"

KFileVersionInfo::KFileVersionInfo(const TCHAR szFilePath[])
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
   
    unsigned nLength = 0;
    LANGANDCODEPAGE *pTranslate = NULL;

    ASSERT(szFilePath && szFilePath[0]);

    m_pvVersionInfoData = NULL;
    m_szLanguageCode[0] = _T('\0');

    int nInfoSize = GetFileVersionInfoSize(szFilePath, NULL);
    if (nInfoSize == 0)
        goto Exit0;

    m_pvVersionInfoData = (void *)new unsigned char[nInfoSize];
    if (!m_pvVersionInfoData)
        goto Exit0;
    nRetCode = GetFileVersionInfo(szFilePath, 0, nInfoSize, m_pvVersionInfoData);
    if (!nRetCode)
        goto Exit0;

    nRetCode = VerQueryValue(m_pvVersionInfoData, TEXT("\\VarFileInfo\\Translation"), (void **)&(pTranslate), &nLength);
    if (!nRetCode)
        goto Exit0;

    _sntprintf(m_szLanguageCode, sizeof(m_szLanguageCode) / sizeof(TCHAR),
            _T("%04x%04x"), 
            pTranslate->wLanguage, pTranslate->wCodePage
        );

    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        if (m_pvVersionInfoData)
        {
            delete[] m_pvVersionInfoData;
            m_pvVersionInfoData = NULL;
        }
    }
    return;
}

KFileVersionInfo::~KFileVersionInfo(void)
{
    if (m_pvVersionInfoData)
    {
        delete[] m_pvVersionInfoData;
        m_pvVersionInfoData = NULL;
    }
}

TCHAR *KFileVersionInfo::ProductVersion()
{
    int nRetCode = FALSE;
    TCHAR *pszResult = _T("");

    void *pvVersionInfo = NULL;
    unsigned nSize = 0;
    TCHAR szSubBlock[64];

    if (!m_pvVersionInfoData)
        goto Exit0;

    _sntprintf(szSubBlock, sizeof(szSubBlock) / sizeof(TCHAR),
        TEXT("\\StringFileInfo\\%s\\ProductVersion"),
        m_szLanguageCode);
    
    nRetCode = VerQueryValue(m_pvVersionInfoData, szSubBlock, &pvVersionInfo, &nSize);
    if (!nRetCode)
        goto Exit0;

    pszResult = (TCHAR *)pvVersionInfo;
Exit0:
    return pszResult;
}

TCHAR *KFileVersionInfo::FileVersion()
{
    int nRetCode = FALSE;
    TCHAR *pszResult = _T("");

    void *pvVersionInfo = NULL;
    unsigned nSize = 0;
    TCHAR szSubBlock[64];

    if (!m_pvVersionInfoData)
        goto Exit0;

    _sntprintf(szSubBlock, sizeof(szSubBlock) / sizeof(TCHAR),
        TEXT("\\StringFileInfo\\%s\\FileVersion"),
        m_szLanguageCode);

    nRetCode = VerQueryValue(m_pvVersionInfoData, szSubBlock, &pvVersionInfo, &nSize);
    if (!nRetCode)
        goto Exit0;

    pszResult = (TCHAR *)pvVersionInfo;
Exit0:
    return pszResult;
}

TCHAR *KFileVersionInfo::OriginalFilename()
{
    int nRetCode = FALSE;
    TCHAR *pszResult = _T("");

    void *pvVersionInfo = NULL;
    unsigned nSize = 0;
    TCHAR szSubBlock[64];

    if (!m_pvVersionInfoData)
        goto Exit0;

    _sntprintf(szSubBlock, sizeof(szSubBlock) / sizeof(TCHAR),
        TEXT("\\StringFileInfo\\%s\\OriginalFilename"),
        m_szLanguageCode);

    nRetCode = VerQueryValue(m_pvVersionInfoData, szSubBlock, &pvVersionInfo, &nSize);
    if (!nRetCode)
        goto Exit0;

    pszResult = (TCHAR *)pvVersionInfo;
Exit0:
    return pszResult;
}
