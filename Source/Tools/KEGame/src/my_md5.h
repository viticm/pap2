/**
* @file     my_md5.h
* @brief    计算MD5值
*
* @details  字符串MD5值计算
* @author   xiaojian <xiaojian@kingsoft.com>
* @date     2008-08-26
* @version	1.0.0.1
*/

#pragma once

#include "md5.h"

/**
* @brief		    计算字符串MD5值
*
* @details 
* @param[in]        需要计算的字符串地址
* @param[out]       输出计算结果
* @param[in]        接受输入结果的内存大小
* @return[true]     成功
* @return[false]    失败
* @remarks	
*/
bool StringMD5(CHAR* pInStr, CHAR* pOutStr, UINT uSize)
{
    bool    bRet = false;
    //计算密码MD5值
    MD5_CTX md5PostString;
    UCHAR   ucFinish[32]    = {0};
    CHAR    szHex[10]       = {0};
    CHAR    szResult[33]    = {0};

    if (NULL == pInStr && uSize > 32)
    {
        return bRet;
    }

    md5PostString.MD5Update((unsigned char*)pInStr, strlen(pInStr));        
    md5PostString.MD5Final((unsigned char*)ucFinish);

    for (int i = 0; i < 16; i++)
    {
        sprintf(szHex, "%02x", ucFinish[i]);
        strcat( szResult, szHex );
    }

    strcpy(pOutStr, szResult);

    bRet = true;

    return bRet;
}