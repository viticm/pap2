////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_RoleProcessor3.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2005-09-22 14:51:45
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_RoleProcessor3.h"
#include "KG_PublicFunction.h"


const unsigned STAT_NAME_MAX_SIZE = 32;
const unsigned KG_ROLE_NAME_MIN_SIZE = 7; // length is 6

using namespace std;

#define PROCESSOR_FILE_NAME "RoleProcessorData.dat"

KG_RoleProcessor3::KG_RoleProcessor3() :
    m_piRoleInfo(NULL),
    m_ulRefCount(1)
{

}

KG_RoleProcessor3::~KG_RoleProcessor3()
{
    ASSERT(!m_piRoleInfo);
    KG_COM_RELEASE(m_piRoleInfo);
}

int KG_RoleProcessor3::Init(void *pvContent)
{
    int nResult  = false;
    int nRetCode = false;
    unsigned i = 0;
    char szName[STAT_NAME_MAX_SIZE];
    int  nOrder = 0;
    unsigned uCount = 0;
    FILE *fpReadFile = NULL;
    int nPlayerCreatorInitFlag = false;

    KG_USE_ARGUMENT(pvContent);

    m_RoleNameAndWorldStatMapLock.Lock(); 

    m_piRoleInfo = KG_CreateRoleInfo();
    KGLOG_PROCESS_ERROR(m_piRoleInfo);

    // init player creator
    nRetCode = m_RoleCreator.Init();
    KGLOG_PROCESS_ERROR(nRetCode && "PlayerCreator init error");
    nPlayerCreatorInitFlag = true;

    // load role stat
    fpReadFile = fopen(PROCESSOR_FILE_NAME, "rb");
    KG_PROCESS_SUCCESS(!fpReadFile); // file not exist

    nRetCode = (int)fread(&uCount, 1, sizeof(uCount), fpReadFile);
    KGLOG_PROCESS_ERROR(nRetCode == sizeof(uCount));

    m_RoleNameAndWorldStatMap.clear();

    for (i = 0; i < uCount; ++i)
    {
        nRetCode = (int)fread(szName, 1, sizeof(szName), fpReadFile);
        KGLOG_PROCESS_ERROR(nRetCode == sizeof(szName));
        szName[sizeof(szName) - 1] = '\0';

        nRetCode = (int)fread(&nOrder, 1, sizeof(nOrder), fpReadFile);
        KGLOG_PROCESS_ERROR(nRetCode == sizeof(nOrder));

        m_RoleNameAndWorldStatMap.insert(make_pair(szName, nOrder));
    }
    
Exit1:
    nResult = true;
Exit0:
    if (fpReadFile)
    {
        fclose(fpReadFile);
        fpReadFile = NULL;
    }
    if (!nResult)
    {
        if (nPlayerCreatorInitFlag)
        {
            m_RoleCreator.UnInit();
            nPlayerCreatorInitFlag = true;
        }
        KG_COM_RELEASE(m_piRoleInfo);
    }
    m_RoleNameAndWorldStatMapLock.Unlock(); 

    return nResult;
}

int KG_RoleProcessor3::UnInit(void *pvContent)
{
    int nResult  = false;
    int nRetCode = false;
    ROLE_NAME_AND_WORLD_STAT_MAP::iterator it;
    FILE *fpWirteFile = NULL;
    char szName[STAT_NAME_MAX_SIZE];
    int  nOrder = 0;
    unsigned uCount = 0;

    KG_USE_ARGUMENT(pvContent);

    m_RoleCreator.UnInit();
    KG_COM_RELEASE(m_piRoleInfo);

    m_RoleNameAndWorldStatMapLock.Lock();  

    fpWirteFile = fopen(PROCESSOR_FILE_NAME, "wb");
    KGLOG_PROCESS_ERROR(fpWirteFile);

    uCount = (unsigned)m_RoleNameAndWorldStatMap.size();
    nRetCode = (int)fwrite(&uCount, 1, sizeof(uCount), fpWirteFile);
    KGLOG_PROCESS_ERROR(nRetCode == sizeof(uCount)); 

    for (it = m_RoleNameAndWorldStatMap.begin(); it != m_RoleNameAndWorldStatMap.end(); ++it)
    {
        strncpy(szName, it->first.c_str(), sizeof(szName) - 1);
        szName[sizeof(szName) - 1] = '\0';
        nOrder = it->second;

        nRetCode = (int)fwrite(szName, 1, sizeof(szName), fpWirteFile);
        KGLOG_PROCESS_ERROR(nRetCode == sizeof(szName));

        nRetCode = (int)fwrite(&nOrder, 1, sizeof(nOrder), fpWirteFile);
        KGLOG_PROCESS_ERROR(nRetCode == sizeof(nOrder));
    }

//Exit1:
    nResult = true;
Exit0:
    if (fpWirteFile)
    {
        fclose(fpWirteFile);
        fpWirteFile = NULL;
    }

    m_RoleNameAndWorldStatMapLock.Unlock(); 

    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_RoleProcessor3::CreateRole(
    unsigned uParamBufferSize, void *pvCreateParam,
    unsigned *puRoleDataBufferSize, void *pvRoleData,
    unsigned *puNameSize, void **ppvName,
    unsigned *puAccountSize, void **ppvAccount,
    unsigned *puRoleListItemSize, void *pvRoleListItem,
    unsigned uRoleID
)
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uRoleNameSize = 0;
    unsigned uRoleDataBufferSize = 0;
    KRoleDBData *pRole = NULL;
    KBaseData   *pBaseData = NULL;

    KGLOG_PROCESS_ERROR(uParamBufferSize == sizeof(KDB_ROLE_GENERATOR_PARAM));
    KGLOG_PROCESS_ERROR(pvCreateParam);
    KGLOG_PROCESS_ERROR(puRoleDataBufferSize);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(puNameSize);
    KGLOG_PROCESS_ERROR(ppvName);
    KGLOG_PROCESS_ERROR(puAccountSize);
    KGLOG_PROCESS_ERROR(ppvAccount);
    KGLOG_PROCESS_ERROR(puRoleListItemSize);
    KGLOG_PROCESS_ERROR(pvRoleListItem);

    // -------------------- create a new role -------------------- 
    uRoleDataBufferSize = *puRoleDataBufferSize;
    nRetCode = m_RoleCreator.GetRoleData(
        (KDB_ROLE_GENERATOR_PARAM *)pvCreateParam, 
        uRoleDataBufferSize, 
        pvRoleData, 
        puRoleDataBufferSize,
        uRoleID
    );
    KGLOG_PROCESS_ERROR(nRetCode && "KG_RoleCreator::GetRoleData() failed");

    // -------------------- return role's information -------------------- 
    pRole  = (KRoleDBData *)pvRoleData;
    pBaseData = ROLE_BASE_DATA(pRole);

    *ppvName        = pBaseData->szRoleName;
    *puNameSize     = (unsigned)strlen(pBaseData->szRoleName) + 1;
    *ppvAccount     = pBaseData->szAccount;
    *puAccountSize  = (unsigned)strlen(pBaseData->szAccount) + 1;
    
    ASSERT(m_piRoleInfo);
    nRetCode = m_piRoleInfo->GetRoleListItem(
        *puRoleDataBufferSize, pRole, puRoleListItemSize, pvRoleListItem
    );
    KGLOG_PROCESS_ERROR(nRetCode);

//Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KG_RoleProcessor3::SetStatisticData(
    KG_STATISTIC_DATA_TYPE eDataType, IKG_Buffer *piStatistic
)
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uDataSize = 0;
    char *pchReadPos = NULL;
    unsigned i = 0;
    unsigned uNameLength = 0;
    char szName[STAT_NAME_MAX_SIZE];

    KGLOG_PROCESS_ERROR(piStatistic);

    KG_PROCESS_SUCCESS(eDataType != STATISTIC_MOST_LEVEL_NAME_LIST);

    pchReadPos = (char *)piStatistic->GetData();
    ASSERT(pchReadPos);
    uDataSize = piStatistic->GetSize();

    {
        m_RoleNameAndWorldStatMapLock.Lock();

        m_RoleNameAndWorldStatMap.clear();
        for (i = 0; i < uDataSize / STAT_NAME_MAX_SIZE; ++i)
        {
            strncpy(szName, pchReadPos, sizeof(szName) - 1);
            szName[sizeof(szName) - 1] = '\0';

            if (szName[0] != '\0')
            {
                m_RoleNameAndWorldStatMap.insert(make_pair(szName, i + 1));
            }
            pchReadPos += STAT_NAME_MAX_SIZE;
        }

        m_RoleNameAndWorldStatMapLock.Unlock();
    }


Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KG_RoleProcessor3::OnRoleLoadFromDatabase(
    unsigned uRoleDataSize, void *pvRoleData, int *pnContinueFlag
)
{
    int nResult  = false;
    int nRetCode = false;
    ROLE_NAME_AND_WORLD_STAT_MAP::iterator it;
    unsigned uCRC = 0;
    KRoleDBData *pRoleData = NULL;

    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(pnContinueFlag);

    *pnContinueFlag = true;

    KGLOG_PROCESS_ERROR(uRoleDataSize >= ROLE_DATA_MIN_SIZE);
    pRoleData = (KRoleDBData *)pvRoleData;

//Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KG_RoleProcessor3::OnRoleSaveToDatabase(
    unsigned uRoleDataSize, void *pvRoleData, int *pnContinueFlag
)
{
    int nResult  = false;

    KGLOG_PROCESS_ERROR(uRoleDataSize >= ROLE_DATA_MIN_SIZE);
    KGLOG_PROCESS_ERROR(pvRoleData);
    KGLOG_PROCESS_ERROR(pnContinueFlag);
    
    *pnContinueFlag = true;

//Exit1:
    nResult = true;
Exit0:
    return nResult;
}
