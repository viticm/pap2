////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_I18N.cpp
//  Version     : 1.0
//  Creator     : tongxuehu
//  Create Date : 11/24/2009
//  Comment     : 
//  
//  Copyright (C) 2009 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "KG_I18N.h"

int ConvertToUTF8(
    unsigned int uCodePage, 
    const char szSrc[], 
    int nSrcSize, 
    char szDst[], 
    int nDstSize, 
    int *pnRetLength
)
{
    int nResult  = false;
    int nRetCode = false;

    wchar_t szUnicode[1024];
    wchar_t *pwszBuff = NULL;
    int nUnicodeLength = 0;
    int nUTFLength = 0;

    KGLOG_PROCESS_ERROR(szSrc);
    KGLOG_PROCESS_ERROR(nSrcSize > 0);
    KGLOG_PROCESS_ERROR(szDst);
    KGLOG_PROCESS_ERROR(nDstSize > 0);
    KGLOG_PROCESS_ERROR(pnRetLength);

    nUnicodeLength = MultiByteToWideChar(uCodePage, 0, szSrc, nSrcSize, NULL, 0);
    KGLOG_PROCESS_ERROR(nUnicodeLength > 0);
    if (nUnicodeLength <= _countof(szUnicode))
    {        
        nUnicodeLength = MultiByteToWideChar(uCodePage, 0, szSrc, nSrcSize, szUnicode, _countof(szUnicode));
        KGLOG_PROCESS_ERROR(nUnicodeLength > 0);

        nUTFLength = WideCharToMultiByte(CP_UTF8, 0, szUnicode, nUnicodeLength, szDst, nDstSize, NULL, NULL);
        KGLOG_PROCESS_ERROR(nUTFLength > 0);
    }
    else
    {
        pwszBuff = new wchar_t[nUnicodeLength];
        KGLOG_PROCESS_ERROR(pwszBuff);

        nUnicodeLength = MultiByteToWideChar(uCodePage, 0, szSrc, nSrcSize, pwszBuff, nUnicodeLength);
        KGLOG_PROCESS_ERROR(nUnicodeLength > 0);

        nUTFLength = WideCharToMultiByte(CP_UTF8, 0, pwszBuff, nUnicodeLength, szDst, nDstSize, NULL, NULL);
        KGLOG_PROCESS_ERROR(nUTFLength > 0);
    }

    // Exit1:
    *pnRetLength = nUTFLength;
    nResult = true;
Exit0:
    SAFE_DELETE_ARRAY(pwszBuff);
    return nResult;
}


static unsigned int gs_uUICodePage = CODE_PAGE_GBK;

int SetCodePage(unsigned int uCodePage)
{
    gs_uUICodePage = uCodePage;
    return true;
}

unsigned int GetCodePage()
{
    return gs_uUICodePage;
}
