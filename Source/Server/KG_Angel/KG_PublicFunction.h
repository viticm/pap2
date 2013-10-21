////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_PublicFunction.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2005-10-24 15:31:11
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG_PUBLICFUNCTION_H_
#define _INCLUDE_KG_PUBLICFUNCTION_H_

#include "KRoleDBDataDef.h"


#define ROLE_BASE_DATA(pRoleDBData)                           \
    (KBaseData *)(                                            \
        (char *)(pRoleDBData) + sizeof(KRoleDBData) +                 \
        (pRoleDBData)->wBlockCount * sizeof(KRoleDataBlock)   \
    );
#define ROLE_DATA_MIN_SIZE                                    \
    (sizeof(KRoleDBData) + sizeof(KRoleDataBlock) + sizeof(KBaseData))

#define ROLE_DATA_CRC_POS(pRoleDBData) (void *)&((pRoleDBData)->wVersion)

#define ROLE_DATA_CRC_SIZE(pRoleDBData)                              \
    (                                                                \
        (unsigned)(pRoleDBData->dwDataLen)  -                        \
        ((char *)&((pRoleDBData)->wVersion) - (char *)(pRoleDBData)) \
    )
////////////////////////////////////////////////////////////////////////////////

int CheckRoleData();




#endif //_INCLUDE_KG_PUBLICFUNCTION_H_
