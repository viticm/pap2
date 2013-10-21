////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_BishopProcessor3.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2005-09-22 14:52:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Protocol/GoddessProtocol.h"
#include "KRoleDBDataDef.h"
#include "KG_BishopProcessor3.h"

////////////////////////////////////////////////////////////////////////////////
int KG_BishopProcessor3::CheckRoleDataCRC(unsigned uBufferSize, void *pvRoleData)
{
    return false;
}

char *KG_BishopProcessor3::GetRoleNameFromRoleData(unsigned uBufferSize,  void *pvRoleData)
{
    return false;
}

int KG_BishopProcessor3::GetNormalRoleCountFromList(unsigned uBufferSize, void *pvRoleListData)
{
    TProcessData *pTProcessData = NULL;

    ASSERT(uBufferSize >= sizeof(TProcessData));
    pTProcessData = (TProcessData *)pvRoleListData;

    return pTProcessData->pDataBuffer[0];
}

int KG_BishopProcessor3::GetFreezedRoleCountFromList(unsigned uBufferSize, void *pvRoleListData)
{
    return false;
}

int KG_BishopProcessor3::GetNormalRoleNameFromList(
        unsigned uBufferSize, 
        void *pvRoleListData, 
        int nIndex, 
        char szRoleName[], 
        int nNameBufferSize
)
{
    int           nResult       = false;
    int           nRoleCount    = 0;
    TProcessData *pTProcessData = NULL;
    KBaseData    *pRoleBaseDate = NULL;

    ASSERT(uBufferSize >= sizeof(TProcessData));
    pTProcessData = (TProcessData *)pvRoleListData;
    nRoleCount = pTProcessData->pDataBuffer[0];
    KGLOG_PROCESS_ERROR(nRoleCount >= 0);
    KGLOG_PROCESS_ERROR(nIndex < nRoleCount);
    pRoleBaseDate = (KBaseData *)(pTProcessData->pDataBuffer + 1);
    ASSERT(nNameBufferSize > 0);
    pRoleBaseDate[nIndex].szRoleName[sizeof(pRoleBaseDate[nIndex].szRoleName) - 1] = '\0';
    strncpy(szRoleName, pRoleBaseDate[nIndex].szRoleName, nNameBufferSize);
    szRoleName[nNameBufferSize - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

int KG_BishopProcessor3::GetFreezedRoleNameFromList(
        unsigned uBufferSize, 
        void *pvRoleListData, 
        int nIndex, 
        char szRoleName[], 
        int nNameBufferSize
)
{
    // not implement in jx3 !
    return false;
}

int KG_BishopProcessor3::GetRoleNameFromPlayerCreateRoleParam(
    unsigned uParamSize, 
    void *pvCreateRoleParam,
    char szRoleName[],
    int nNameBufferSize
)
{
    int                         nResult             = false;
    KUSER_ROLE_GENERATOR_PARAM  *pRoleCreateParam   = NULL;

    KGLOG_PROCESS_ERROR(uParamSize == sizeof(KUSER_ROLE_GENERATOR_PARAM));

    pRoleCreateParam = (KUSER_ROLE_GENERATOR_PARAM *)pvCreateRoleParam;

    pRoleCreateParam->szRoleName[sizeof(pRoleCreateParam->szRoleName) - 1] = '\0';
    strncpy(szRoleName, pRoleCreateParam->szRoleName, nNameBufferSize);
    szRoleName[nNameBufferSize - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

int KG_BishopProcessor3::GetGoddessCreateRoleParamSize()
{
    return (int)sizeof(KUSER_ROLE_GENERATOR_PARAM);
}

int KG_BishopProcessor3::GetGoddessCreateRoleParam(
    unsigned uGoddessParamSize, void *pvGoddessParam, // output
    unsigned uPlayerParamSize, void *pvPlayerCreateRoleParam, // input
    const char cszAccountName[]
)
{
    // 接口已经废弃
    return false;
}


int KG_BishopProcessor3::OnSendRoleList(unsigned uBufferSize, void *pvRoleList)
{
    int nResult  = false;

    // do nothing !
    nResult = true;
//Exit0:
    return nResult;
}

int KG_BishopProcessor3::OnEnterGameRequest(unsigned uBufferSize, void *pvQueryPacket)
{
    int nResult  = false;

    // do nothing !
    nResult = true;
//Exit0:
    return nResult;
}

int KG_BishopProcessor3::GetRoleDataFlag(
    unsigned uRoleDataBufferSize, 
    void *pvRoleData,
    int  *pnUseRevive, 
    int  *pnRevivalID, 
    int  *pnEnterGameID 
)
{
    return false;
}

// Set cUseRevive, cFightMode, reset CRC. 
int KG_BishopProcessor3::SetRoleDataFlag(   
    unsigned uRoleDataBufferSize, 
    void *pvRoleData,
    char cUseRevive, 
    char cFightMode 
)
{
    return false;
}
