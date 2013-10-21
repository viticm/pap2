#include "stdafx.h"
#include "Tools.h"

int Tools::IsPathExist(char szFilePath[], int nSize)
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = Tools::Trim(szFilePath, nSize);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = _access(szFilePath, 0);
    if (nRetCode)
    {
        printf("ERROR:路径有误, 请输入正确的路径!\n");
        goto Exit0;
    }

    nResult = true;
Exit0:
    return nResult;
}

int Tools::Trim(char szSource[], int nSize)
{
    int     nResult             = false;
    int     nRetCode            = false;
    int     nLength             = 0;
    int     nFirstPos           = 0;
    int     nLastPos            = 0;
    char    szTempStr[MAX_PATH];

    szTempStr[0]                = '\0';

    KGLOG_PROCESS_ERROR(szSource);

    nLength = (int)strlen(szSource);
    KGLOG_PROCESS_ERROR(nLength <= MAX_PATH);

    nFirstPos = FindFirstNotChar(szSource, ' ');
    nLastPos  = FindLastNotChar(szSource, ' ');

    nRetCode = nLastPos - nFirstPos + 1;
    KGLOG_PROCESS_ERROR(nRetCode <= nLength);

    strncpy(szTempStr, szSource + nFirstPos, nRetCode);
    szTempStr[nRetCode] = '\0';
    szTempStr[sizeof(szTempStr) - 1] = '\0';

    strncpy(szSource, szTempStr, nSize);
    szSource[nSize - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

int Tools::FindFirstNotChar(const char cszSource[], char cKey)
{
    int nPos    = 0;
    int nSize   = 0;

    KGLOG_PROCESS_ERROR(cszSource);

    nSize = (int)strlen(cszSource);
    for (int i = 0; i < nSize; i++)
    {
        if (cKey != cszSource[i])
        {   
            nPos = i;
            break;
        }
    }

Exit0:
    return nPos;
}

int Tools::FindLastNotChar(const char cszSource[], char cKey)
{
    int nPos    = 0;
    int nSize   = 0;

    KGLOG_PROCESS_ERROR(cszSource);

    nSize = (int)strlen(cszSource);
    for (int i = nSize - 1; i >= 0; i--)
    {
        if (cKey != cszSource[i])
        {   
            nPos = i;
            break;
        }
    }

Exit0:
    return nPos;
}

int Tools::MaskMatch(const char cszSrc[], int nSrcLen, const char cszDst[], int nDstLen)
{
    int nResult = false;
    int nRetCode = false;
    char *pszSrc = NULL;
    char *pszDst = NULL;
    char *pRetCode = NULL;

    KGLOG_PROCESS_ERROR(cszSrc && cszSrc[0]);
    KGLOG_PROCESS_ERROR(cszDst && cszDst[0]);

    pszSrc = new char[nSrcLen + 1];
    KGLOG_PROCESS_ERROR(pszSrc);

    pszDst = new char[nDstLen + 1];
    KGLOG_PROCESS_ERROR(pszDst);

    strncpy(pszSrc, cszSrc, nSrcLen);
    pszSrc[nSrcLen] = '\0';

    strncpy(pszDst, cszDst, nDstLen);
    pszDst[nDstLen] = '\0';

    nRetCode = _strlwr_s(pszSrc, nSrcLen + 1);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = _strlwr_s(pszDst, nDstLen + 1);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    pRetCode = strstr(pszSrc, pszDst);
    KG_PROCESS_ERROR(pRetCode);

    nResult = true;
Exit0:
    KG_DELETE(pszDst);
    KG_DELETE(pszSrc);
    return nResult;
}

int Tools::SlashPath(char szPath[], int nSize)
{
    int nResult = false;
    int nRetCode = false;
    char *pszBacklash = NULL;
    int nLen = 0;

    KGLOG_PROCESS_ERROR(szPath && szPath[0]);
   
    pszBacklash = strchr(szPath, '/');
    KGLOG_PROCESS_ERROR(pszBacklash == NULL);

    nLen = (int)strlen(szPath);
    KGLOG_PROCESS_ERROR(nLen < nSize - 1);
    KGLOG_PROCESS_ERROR(nLen < MAX_PATH - 1);

    if (szPath[nLen - 1] != '\\')
    {
        szPath[nLen] = '\\';
        szPath[nLen + 1] = '\0';
    }

    szPath[nSize - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}