//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSG_MD5_String.cpp
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-7-29 17:53:17
//  Comment     :   Conver String to MD5 String
//
//////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include "md5.h"
#include "KFileMD5Maker.h"

#define MD5_BUFFER_SIZE 2048

int MakdFileMD5(TCHAR szDestMD5String[64], const TCHAR szFileName[])
{
    int nResult = false;
    md5_state_t md5_state;
    unsigned char MD5Value[16];
    int nSrcStringLen = 0;
    unsigned char byBuffer[MD5_BUFFER_SIZE];
    FILE *pFile = NULL;

    if (!szDestMD5String)
        goto Exit0;

    szDestMD5String[0] = _T('\0');

    if  (!szFileName)
       goto Exit0;

    pFile = _tfopen(szFileName, _T("rb"));
    if (!pFile)
        goto Exit0;

    md5_init(&md5_state);

    while (!feof(pFile))
    {
        size_t uRealSize = fread(byBuffer, 1, MD5_BUFFER_SIZE, pFile);
        if (uRealSize < MD5_BUFFER_SIZE)
        {
            if (ferror(pFile))
                goto Exit0;
        }
        if (uRealSize > 0)
        {
            md5_append(&md5_state, byBuffer, (int)uRealSize);
        }        
    }

    md5_finish(&md5_state, MD5Value);

    _sntprintf(
        szDestMD5String,
        64,
        _T("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"),
        MD5Value[ 0], MD5Value[ 1], MD5Value[ 2], MD5Value[ 3],
        MD5Value[ 4], MD5Value[ 5], MD5Value[ 6], MD5Value[ 7],
        MD5Value[ 8], MD5Value[ 9], MD5Value[10], MD5Value[11],
        MD5Value[12], MD5Value[13], MD5Value[14], MD5Value[15]
    );

    nResult = true;
Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    return nResult;
}
