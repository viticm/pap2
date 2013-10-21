//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSG_MD5_String.cpp
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-7-29 17:53:17
//  Comment     :   Conver String to MD5 String
//
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "md5.h"
#include "KSG_MD5_String.h"

int KG_EDStringToMD5String(char szDestMD5String[64], const char cszSrcString[])
{
    int nResult = false;
    md5_state_t md5_state;
    unsigned char MD5Value[16];
    int nSrcStringLen = 0;

    if (!szDestMD5String)
        goto Exit0;

    szDestMD5String[0] = '\0';

    if  (!cszSrcString)
       goto Exit0;

    nSrcStringLen = (int)strlen(cszSrcString);

    if (!nSrcStringLen)
        goto Exit0;

    md5_init(&md5_state);

    md5_append(&md5_state, (unsigned char *)cszSrcString, nSrcStringLen);

    md5_finish(&md5_state, MD5Value);


    sprintf(
        szDestMD5String,
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
        MD5Value[ 0], MD5Value[ 1], MD5Value[ 2], MD5Value[ 3],
        MD5Value[ 4], MD5Value[ 5], MD5Value[ 6], MD5Value[ 7],
        MD5Value[ 8], MD5Value[ 9], MD5Value[10], MD5Value[11],
        MD5Value[12], MD5Value[13], MD5Value[14], MD5Value[15]
    );

    nResult = true;
Exit0:
    return nResult;
}
