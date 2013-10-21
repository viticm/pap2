//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KDBBase.cpp
//  Version     :   1.0
//  Creater     :   Xia Yong
//  Date        :   2009-06-25 
//  Comment     :   ÐòÁÐÉú³ÉÆ÷
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KIDSEQUENCE_H_
#define _KIDSEQUENCE_H_

#include "KSUnknown.h"
#include "mysql.h"

struct IKIDSequence : public IUnknown 
{
    virtual BOOL GenerateID(const char cszSequenceName[], int nPrefetch, uint64_t* puRetID) = 0;
};

IKIDSequence* CreateIDSequence(
    const char cszDBAddr[], const char cszDBName[], const char cszUserName[], const char cszPassword[]
);

#endif
