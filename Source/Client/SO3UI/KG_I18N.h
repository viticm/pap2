////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_I18N.h
//  Version     : 1.0
//  Creator     : tongxuehu
//  Create Date : 11/24/2009
//  Comment     : i18n = internationalization
//  
//  Copyright (C) 2009 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#ifndef KG_I18N_H
#define KG_I18N_H

int ConvertToUTF8(
    unsigned int uCodePage, 
    const char szSrc[], 
    int nSrcSize, 
    char szDst[], 
    int nDstSize, 
    int *pnRetLength
);

int SetCodePage(unsigned int uCodePage);
unsigned int GetCodePage();

const unsigned int CODE_PAGE_GBK = 0;

// const char FILE_PREFIX_UTF8[] = {(char)0xEF, (char)0xBB, (char)0xBF};

#endif // KG_I18N_H
