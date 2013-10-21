////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_RoleInfo3.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2005-09-22 14:51:53
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGPublic.h"
#include "KG_RoleInfo3.h"
#include "KRoleDBDataDef.h"
#include "KG_PublicFunction.h"
#include "Common/CRC32.h"

using namespace std;

const unsigned KG_NAME_MAX_SIZE      = 32;
const unsigned KG_ACCOUNT_MAX_ROLE_COUNT = 3;

////////////////////////////////////////////////////////////////////////////
// @brief : Get a string's size in a buffer, limited scan range.
// @param uBufferSize : [in] string buffer max size.
// @param pszStr      : [in] point to a NULL terminal string.
// @param puSize      : [out] string size, include terminal NULL, 
//                            if string's size > uBufferSize,
//                            it will be set to uBufferSize
// @return : true if successful, false if string length > uMaxSize.
static inline unsigned _KG_GetStringSize(
    unsigned uBufferSize, const char *pszStr, unsigned *puSize
)
{
    int nResult  = false;
    const char *pcchBegin = NULL;
    const char *pcchEnd   = NULL;
    
    ASSERT(pszStr);
    ASSERT(puSize);

    pcchBegin = pszStr;
    pcchEnd   = pcchBegin + uBufferSize;

    while (pszStr != pcchEnd)
    {
        KG_PROCESS_SUCCESS(*pszStr == '\0'); 
        ++pszStr;
    }
    KG_PROCESS_ERROR(false);

Exit1:
    *puSize = (unsigned)(pszStr - pcchBegin) + 1;
    nResult = true;
Exit0:
    if (!nResult)
    {
        *puSize = uBufferSize;
    }
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
KG_RoleInfo3::KG_RoleInfo3(void) :
    m_ulRefCount(1)
{
}

KG_RoleInfo3::~KG_RoleInfo3(void)
{
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::CheckRoleData(
    unsigned uBufferSize, 
    void *pvRoleData,
    KG_ROLE_DATA_ERROR_PROCESS *pErrorProcess
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL;
    unsigned uRoleCRC = 0;

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(pErrorProcess);
    
    pRoleData = (KRoleDBData *)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRoleData);

    KGLOG_PROCESS_ERROR(pRoleData->wBlockCount >= 1);
    KGLOG_PROCESS_ERROR(pRoleData->BlockList[0].byBlockType == rbtBaseData);
    uRoleCRC = CRC32(
        0, ROLE_DATA_CRC_POS(pRoleData), ROLE_DATA_CRC_SIZE(pRoleData)
    );
    KGLOG_PROCESS_ERROR(uRoleCRC == pRoleData->dwCRC);

//Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::GetRoleName(
    unsigned uBufferSize, void *pvRoleData, 
    unsigned *puRoleNameSize, void **ppvRoleName
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL;

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(puRoleNameSize);
    KGLOG_PROCESS_ERROR(ppvRoleName);

    pRoleData = (KRoleDBData *)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRoleData);

    *ppvRoleName = pBaseData->szRoleName;
    nRetCode = _KG_GetStringSize(
        sizeof(pBaseData->szRoleName), 
        pBaseData->szRoleName, 
        puRoleNameSize
    );
    KGLOG_PROCESS_ERROR(nRetCode);
//Exit1:
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::GetAccountName(
    unsigned uBufferSize, void *pvRoleData, 
    unsigned *puAccountNameSize, void **ppvAccountName 
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL;

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(puAccountNameSize);
    KGLOG_PROCESS_ERROR(ppvAccountName);

    pRoleData = (KRoleDBData *)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRoleData);

	*ppvAccountName = &(pBaseData->szAccount[0]);
    nRetCode = _KG_GetStringSize(
        sizeof(pBaseData->szAccount),
        pBaseData->szAccount, 
        puAccountNameSize
    );
    KGLOG_PROCESS_ERROR(nRetCode);
//Exit1:
    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::GetRoleID(
    unsigned uBufferSize,
    void *pvRoleData,
    unsigned *puRetRoleIDSize, 
    void **ppvRetRoleID
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL; 

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(puRetRoleIDSize);
    KGLOG_PROCESS_ERROR(ppvRetRoleID);

    pRoleData = (KRoleDBData *)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRoleData);
    
    *ppvRetRoleID = (void *)&(pBaseData->dwPlayerID);
    *puRetRoleIDSize = sizeof(pBaseData->dwPlayerID);

    nResult = true;
Exit0:
    return nResult;

}
////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::GetRoleListItem(
    unsigned uBufferSize, void *pvRoleData, 
    unsigned *puRoleListItemSize, void *pvRoleListItem
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL;

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(puRoleListItemSize);
    KGLOG_PROCESS_ERROR(*puRoleListItemSize >= sizeof(KBaseData));
    KGLOG_PROCESS_ERROR(pvRoleListItem);
    
    pRoleData = (KRoleDBData *)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRoleData);

    memcpy(pvRoleListItem, pBaseData, sizeof(KBaseData));
    *puRoleListItemSize = sizeof(KBaseData);
//Exit1:
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
unsigned KG_RoleInfo3::GetRoleListItemMaxSize()
{
    return (unsigned)sizeof(KBaseData);
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::GetRoleList(
    unsigned uBufferSize, void *pvRoleListItems,
    unsigned *puRoleListBufferSize, void *pvRoleList        
)
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uItemCout = 0;
    char     cRoleCout = 0;
    unsigned uItemsSize = 0;
    unsigned char *pbyBuffer = (unsigned char *)pvRoleListItems;
    unsigned i = 0;
    unsigned uRoleListSize = 0;

    KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(unsigned));
    KGLOG_PROCESS_ERROR(pvRoleListItems);
    KGLOG_PROCESS_ERROR(puRoleListBufferSize);
    KGLOG_PROCESS_ERROR(pvRoleList);

    uItemCout  = *(unsigned *)pbyBuffer;
    pbyBuffer += sizeof(unsigned);
    uBufferSize -= sizeof(unsigned);
    if (uItemCout > KG_ACCOUNT_MAX_ROLE_COUNT)
        uItemCout = KG_ACCOUNT_MAX_ROLE_COUNT;

    uRoleListSize = 1;  // 1 byte for role count
    KGLOG_PROCESS_ERROR(uRoleListSize <= *puRoleListBufferSize);
    for (i = 0; i < uItemCout; ++i)
    {
        uItemsSize = *(unsigned *)pbyBuffer;
        pbyBuffer += sizeof(unsigned);
        uBufferSize -= sizeof(unsigned);
        if (
            (uItemsSize != sizeof(KBaseData)) ||
            (uItemsSize > uBufferSize)
        )
        {
            KGLogPrintf(
                KGLOG_ERR, "[KG_Angel] role list item size error: %u", uItemsSize
            );
            if (uItemsSize > uBufferSize)
                break;
        }
        else
        {
            KGLOG_PROCESS_ERROR(uRoleListSize <= *puRoleListBufferSize);
            memcpy((char *)pvRoleList + uRoleListSize, pbyBuffer, uItemsSize);
            uRoleListSize += uItemsSize;
            ++cRoleCout;
        }

        pbyBuffer += uItemsSize;
        uBufferSize -= uItemsSize;
    }
    
    *(char *)pvRoleList = cRoleCout;
    *puRoleListBufferSize = uRoleListSize;
//Exit1:
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
// @brief : Get max buffer size for account info
unsigned KG_RoleInfo3::GetRoleListMaxSize()
{
    return (unsigned)(1 + sizeof(KBaseData) * KG_ACCOUNT_MAX_ROLE_COUNT);
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::GetRoleDataSize(
    unsigned uBufferSize, void *pvRoleData, unsigned *puRoleDataSize
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(puRoleDataSize);

    pRoleData = (KRoleDBData *)pvRoleData;
    *puRoleDataSize  = pRoleData->dwDataLen;
//Exit1:
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::CopyRoleName(
    unsigned uBufferSize,   void *pvRoleData, 
    unsigned uRoleNameBufferSize, char *pszRoleName
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL;

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(uRoleNameBufferSize > 0);
    KGLOG_PROCESS_ERROR(pszRoleName);

    pRoleData = (KRoleDBData *)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRoleData);
    strncpy(
        pszRoleName, pBaseData->szRoleName, uRoleNameBufferSize - 1
    );
    pszRoleName[uRoleNameBufferSize - 1] = '\0';

//Exit1:
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::CopyAccountName(
    unsigned uBufferSize, void *pvRoleData, 
    unsigned uAccountNameBufferSize, char *pszAccountName
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL;

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(uAccountNameBufferSize > 0);
    KGLOG_PROCESS_ERROR(pszAccountName);

    pRoleData = (KRoleDBData*)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRoleData);
    strncpy(
        pszAccountName, 
        pBaseData->szAccount, 
        uAccountNameBufferSize - 1
    );
    pszAccountName[uAccountNameBufferSize - 1] = '\0';

//Exit1:
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::SetRoleName(
    unsigned uBufferSize, void *pvRoleData, const char cszNewName[]
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL;

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(cszNewName);

    pRoleData = (KRoleDBData *)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRoleData);

    nRetCode = (int)strlen(cszNewName);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(pBaseData->szRoleName));
    memcpy(pBaseData->szRoleName, cszNewName, nRetCode + 1);
    
    pRoleData->dwCRC = CRC32(
        0, ROLE_DATA_CRC_POS(pRoleData), ROLE_DATA_CRC_SIZE(pRoleData)
    );

//Exit1:
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::SetAccountName(
    unsigned uBufferSize, void *pvRoleData, const char cszNewName[]
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL;

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(cszNewName);

    pRoleData = (KRoleDBData *)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRoleData);

    nRetCode = (int)strlen(cszNewName);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(pBaseData->szAccount));
    memcpy(pBaseData->szAccount, cszNewName, nRetCode + 1);

    pRoleData->dwCRC = CRC32(
        0, ROLE_DATA_CRC_POS(pRoleData), ROLE_DATA_CRC_SIZE(pRoleData)
    );

//Exit1:
    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::SetRoleID(unsigned uBufferSize, void *pvRoleData, unsigned uRoleID)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL;

    KGLOG_PROCESS_ERROR(uBufferSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);

    pRoleData = (KRoleDBData *)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRoleData);

    pBaseData->dwPlayerID = uRoleID;

    pRoleData->dwCRC = CRC32(
        0, ROLE_DATA_CRC_POS(pRoleData), ROLE_DATA_CRC_SIZE(pRoleData)
    );


    nResult = true;
Exit0:
    return nResult;

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int KG_RoleInfo3::ModifyRoleData(
    unsigned uOrgRoleDataSize, void *pvOrgRoleData,
    unsigned uNewRoleDataBufferSize, 
    unsigned *puNewRoleDataSize, void *pvNewRoleData,
    int *pnModifiedFlag
)
{
    int          nResult            = false;
    int          nRetCode           = false;

    *pnModifiedFlag = false;

    nResult = true;
//Exit0:
    return nResult;
}
