#include "stdafx.h"
#include "KG_RoleCreator.h"
#include "Protocol/GoddessProtocol.h"
#include "Engine/KG_InterlockedVariable.h"
#include "Common/CRC32.h"
#include "KG_PublicFunction.h"

KG_RoleCreator::KG_RoleCreator()
{
	
}

KG_RoleCreator::~KG_RoleCreator()
{

}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleCreator::Init()
{
    int nRetCode = false;
    int nResult  = false;
    
    
    nResult = true;
//Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleCreator::UnInit()
{
    int nRetCode = false;
    int nResult  = false;

    nResult = true;
//Exit0: 
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleCreator::GetRoleData(
    KDB_ROLE_GENERATOR_PARAM *pParam, 
    unsigned uRoleDataBufferSize, 
    void *pvRoleData, 
    unsigned *puRoleDataSize,
    unsigned uRoleID
)
{
    int nResult  = false;
    int nRetCode = false;
    KRoleDBData *pRoleData = NULL;
    KBaseData   *pBaseData = NULL;
    BYTE        *pbyOffset = (BYTE *)pvRoleData;
    const size_t ROLE_INIT_DATA_SIZE = sizeof(KRoleDBData) + sizeof(KRoleDataBlock) + sizeof(KBaseData);
    
    KGLOG_PROCESS_ERROR(uRoleDataBufferSize >= ROLE_INIT_DATA_SIZE);
    
    pRoleData = (KRoleDBData *)pbyOffset;    
    pbyOffset += sizeof(KRoleDBData) + sizeof(KRoleDataBlock);

    // ---------------- Set Base Information ----------------->

    pBaseData = (KBaseData *)pbyOffset;
    pbyOffset += sizeof(KBaseData);

    memset(pBaseData, 0, sizeof(KBaseData));
    
	nRetCode = (int)strlen(pParam->UserParam.szRoleName);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && "Empty rolename");
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(pBaseData->szRoleName) && "Rolename too long");
	strncpy(pBaseData->szRoleName, pParam->UserParam.szRoleName, sizeof(pBaseData->szRoleName));
    pBaseData->szRoleName[sizeof(pBaseData->szRoleName) - 1] = '\0';
    
    nRetCode = (int)strlen(pParam->szAccountName);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && "Empty AccountName");
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(pBaseData->szAccount) && "AccountName too long");
	strncpy(pBaseData->szAccount, pParam->szAccountName,  sizeof(pBaseData->szAccount));
    pBaseData->szAccount[sizeof(pBaseData->szAccount) - 1] = '\0';

    KGLOG_PROCESS_ERROR(pParam->UserParam.cRoleType > rtInvalid && pParam->UserParam.cRoleType < rtTotal);

    pBaseData->dwPlayerID                   = (DWORD)uRoleID;
    pBaseData->cRoleType                    = pParam->UserParam.cRoleType;
    pBaseData->byLevel                      = 1;
    pBaseData->CurrentPos.dwMapID           = pParam->UserParam.dwMapID;
	pBaseData->CurrentPos.nMapCopyIndex     = pParam->UserParam.nMapCopyIndex;
    pBaseData->CurrentPos.nX                = pParam->nX;
    pBaseData->CurrentPos.nY                = pParam->nY;
    pBaseData->CurrentPos.nZ                = pParam->nZ;
    pBaseData->CurrentPos.byFaceDirection   = pParam->byDirection;

    memcpy(
        pBaseData->wRepresentId, 
        pParam->UserParam.wRepresentId, 
        sizeof(pBaseData->wRepresentId)
    );

    pRoleData->BlockList[0].byBlockType = rbtBaseData;
    pRoleData->BlockList[0].wDataLen    = (WORD)sizeof(KBaseData);
    pRoleData->BlockList[0].byReserved  = 0;

    ASSERT(pbyOffset - (BYTE *)pvRoleData == ROLE_INIT_DATA_SIZE);

    pRoleData->wBlockCount = 1;
    pRoleData->dwDataLen   = (DWORD)ROLE_INIT_DATA_SIZE;
    pRoleData->wVersion    = rdvCurrentRoleDataVersion;
    pRoleData->dwCRC       = CRC32(0, ROLE_DATA_CRC_POS(pRoleData), ROLE_DATA_CRC_SIZE(pRoleData));

    *puRoleDataSize        = (unsigned)ROLE_INIT_DATA_SIZE;

    KGLogPrintf(
        KGLOG_ERR, "[PlayerCreator] Created role %s, ServerID=%u",
        pBaseData->szRoleName, pBaseData->dwPlayerID
    );

    nResult = true;
Exit0:
	return nResult;
}
