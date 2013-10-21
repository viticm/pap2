#include "stdafx.h"
#include "KGCrashReporter.h"

KGCrashReporter::KGCrashReporter()
{

}

KGCrashReporter::~KGCrashReporter()
{

}

int KGCrashReporter::UpdateCrashReport(const TCHAR cszFile[], const TCHAR cszKey[])
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    long lFileSize = 0;
    TCHAR *pBuffer = NULL;
    TCHAR *pFreeBuffer = NULL;
    TCHAR szValue[MAX_CONTENT];
    TCHAR szOneLine[MAX_CONTENT];
    tstring strOneLine;
    string strInput;
    szValue[0] = _T('\0');
    szOneLine[0] = _T('\0');

    KG_PROCESS_ERROR(cszFile && cszFile[0]);
    KG_PROCESS_ERROR(cszKey && cszKey[0]);

    FILE *pFile = NULL;

    pFile = _tfopen(cszFile, _T("rb"));
    KG_PROCESS_ERROR(pFile);

    nRetCode = fseek(pFile, 0, SEEK_END);
    KG_PROCESS_ERROR(!nRetCode);

    lFileSize = ftell(pFile);
    
    nRetCode = fseek(pFile, 0 ,SEEK_SET);
    KG_PROCESS_ERROR(!nRetCode);

    pFreeBuffer = (TCHAR *)malloc(lFileSize * sizeof(TCHAR));
    KG_PROCESS_ERROR(pFreeBuffer);
    
    ZeroMemory(pFreeBuffer, lFileSize);

    nRetCode = (int)fread((TCHAR *)pFreeBuffer, sizeof(TCHAR), lFileSize, pFile);
    KG_PROCESS_ERROR(nRetCode > 0);
    
    pBuffer = pFreeBuffer;

    fclose(pFile);
    pFile = NULL;

    pFile = _tfopen(cszFile, _T("wb"));
    KG_PROCESS_ERROR(pFile);

    while (*pBuffer != _T('\0'))
    {
        szOneLine[0] = _T('\0');
        TCHAR *pszOneLine = szOneLine;
        while (*pBuffer != _T('\n') && *pBuffer != _T('\0'))
        {
            *pszOneLine = *pBuffer;
            pszOneLine++;
            pBuffer++;
        }
        *pszOneLine = _T('\0'); 
        strOneLine = tstring(szOneLine);

        nRetCode = _tcsicmp(strOneLine.c_str(), _T("<Feedback Content=\"\" />"));
        if (nRetCode == 0)
        {
            nRetCode = _sntprintf(
                szValue,
                sizeof(szValue) / sizeof(TCHAR) - 1,
                _T("<Feedback Content=\"%s\" />"),
                cszKey
            );        
            KG_PROCESS_ERROR(nRetCode > 0);
            szValue[sizeof(szValue) / sizeof(TCHAR) - 1] = _T('\0');
            strOneLine = tstring(szValue);
        }

        strOneLine += tstring(_T("\r\n"));
        strInput = TStrToUTF8(strOneLine);
        nRetCode = (int)fwrite(strInput.c_str(), sizeof(TCHAR), strInput.length(), pFile);
        KG_PROCESS_ERROR(nRetCode == strInput.length());
        
        fflush(pFile);

        if (*pBuffer == _T('\0'))
            break;
        
        pBuffer++;
    }
        
    nResult = TRUE;
Exit0:
    if (pFreeBuffer)
    {
        free(pFreeBuffer);
        pFreeBuffer = NULL;
    }
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    return nResult;
}

string KGCrashReporter::TStrToUTF8(const tstring& src)
{
#if defined(UNICODE) || defined(_UNICODE)
    int len = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, NULL, 0, NULL, NULL);
    char *tmp = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, tmp, len, NULL, NULL);
    string result(tmp);
    delete[] tmp;
    return result;
#else
    int len = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
    wchar_t *tmp = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, tmp, len);
    len = WideCharToMultiByte(CP_UTF8, 0, tmp, -1, NULL, 0, NULL, NULL);
    char *dst = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, tmp, -1, dst, len, NULL, NULL);
    string result(dst);
    delete[] tmp;
    delete[] dst;
    return result;
#endif
}