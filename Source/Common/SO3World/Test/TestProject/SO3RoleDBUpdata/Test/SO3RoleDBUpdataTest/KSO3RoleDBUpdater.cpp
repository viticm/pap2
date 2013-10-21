///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KSO3RoleDBUpdater.cpp
// Creator  :   liuzhibiao
// Date     :   2009-6-16
// Comment  :   v0.1
//              2009-6-8	Create0.1
//              2009-6-16	修改实现方式
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSO3RoleDBUpdater.h"
#include "KSO3RoleDBTools.h"

int KSO3RoleDBUpdater::_FillConnectConfig(const char cszConnectConfigFileName[])
{
    int         nResult     = false;
    int         nRetCode    = false;
    IIniFile*   piIniFile   = NULL;

    ASSERT(cszConnectConfigFileName);
    
    piIniFile = g_OpenIniFile(cszConnectConfigFileName);
    KGLOG_PROCESS_ERROR(piIniFile);

    nRetCode = piIniFile->GetString(
        "MySQL", 
        "IP",
        "",
        m_RoleDBConfig.szConnectToIP,
        sizeof(m_RoleDBConfig.szConnectToIP)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(
        "MySQL", 
        "Port", 
        3306, 
        &m_RoleDBConfig.nPort
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "MySQL", 
        "Account", 
        "", 
        m_RoleDBConfig.szDBAcc,
        sizeof(m_RoleDBConfig.szDBAcc)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "MySQL",
        "Password", 
        "", 
        m_RoleDBConfig.szDBPsw, 
        sizeof(m_RoleDBConfig.szDBPsw)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "MySQL",
        "Database", 
        "", 
        m_RoleDBConfig.szDBName, 
        sizeof(m_RoleDBConfig.szDBName)
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_RoleDBConfig.szDBName[0] != '\0');

    nRetCode = piIniFile->GetInteger(
        "DBInfo", 
        "MailInfoMaxSize", 
        MAIL_BOX_INFO_MAX_SIZE, 
        &m_nMailInfoMaxSize
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "MySQL_NEW", 
        "IP",
        "",
        m_NewRoleDBConfig.szConnectToIP,
        sizeof(m_NewRoleDBConfig.szConnectToIP)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(
        "MySQL_NEW", 
        "Port", 
        3306, 
        &m_NewRoleDBConfig.nPort
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "MySQL_NEW", 
        "Account", 
        "", 
        m_NewRoleDBConfig.szDBAcc,
        sizeof(m_NewRoleDBConfig.szDBAcc)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "MySQL_NEW",
        "Password", 
        "", 
        m_NewRoleDBConfig.szDBPsw, 
        sizeof(m_NewRoleDBConfig.szDBPsw)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "MySQL_NEW",
        "Database", 
        "", 
        m_NewRoleDBConfig.szDBName, 
        sizeof(m_NewRoleDBConfig.szDBName)
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_NewRoleDBConfig.szDBName[0] != '\0');

    nRetCode = piIniFile->GetString(
        "MySQL_NEW", 
        "IP",
        "",
        m_CompareRoleDBConfig.szConnectToIP,
        sizeof(m_CompareRoleDBConfig.szConnectToIP)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetInteger(
        "MySQL_NEW", 
        "Port", 
        3306, 
        &m_CompareRoleDBConfig.nPort
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "MySQL_NEW", 
        "Account", 
        "", 
        m_CompareRoleDBConfig.szDBAcc,
        sizeof(m_CompareRoleDBConfig.szDBAcc)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "MySQL_NEW",
        "Password", 
        "", 
        m_CompareRoleDBConfig.szDBPsw, 
        sizeof(m_CompareRoleDBConfig.szDBPsw)
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString(
        "MySQL_NEW",
        "Database", 
        "", 
        m_CompareRoleDBConfig.szDBName, 
        sizeof(m_CompareRoleDBConfig.szDBName)
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_NewRoleDBConfig.szDBName[0] != '\0');

    nRetCode = memcmp(m_NewRoleDBConfig.szDBName, m_RoleDBConfig.szDBName, sizeof(m_NewRoleDBConfig.szDBName));
    if (nRetCode != 0)
    {
        goto Exit1;
    }
    nRetCode = memcmp(m_NewRoleDBConfig.szConnectToIP, m_RoleDBConfig.szConnectToIP, sizeof(m_RoleDBConfig.szConnectToIP));
    if (nRetCode != 0)
    {
        goto Exit1;
    }
    if (m_NewRoleDBConfig.nPort != m_RoleDBConfig.nPort)
    {
        goto Exit1;
    }

    m_nUpdateToLocal = true;
Exit1:
    nResult = true;
Exit0:
    KG_COM_RELEASE(piIniFile);
    return nResult;
}

int KSO3RoleDBUpdater::Init()
{
    int nResult     = false;
    int nRetCode    = false;
    
    int nSO3RoleDBToolFlag          = false;
    int nMailInfoFlag               = false;
    int nRoleAllInfoFlag            = false;
    int nNewSO3RoleDBToolFlag       = false;
    int nCompareSO3RoleDBToolFlag   = false;

    m_nUpdateToLocal        = false;    
    m_uMailInfoSize         = 0;
    m_pSO3RoleDBTool        = NULL;
    m_pNewSO3RoleDBTool     = NULL;
    m_pCompareSoleDBTool    = NULL;

    nRetCode = _FillConnectConfig(SO3_ROLEDB_UPDATER_CONFIG_FILE);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_piMailBoxInfoPackage = KG_MemoryCreateBuffer(m_nMailInfoMaxSize);
    KGLOG_PROCESS_ERROR(m_piMailBoxInfoPackage);
    nMailInfoFlag = true;
    
    m_SO3RoleAllInfo.m_uPackageUseSize = 0;
    m_SO3RoleAllInfo.m_piPackage = KG_MemoryCreateBuffer(MAX_ROLE_DATA_SIZE);
    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_piPackage);
    nRoleAllInfoFlag = true;

    m_SO3RoleAllInfo.m_bHadUpdateBaseInfo   = false;
    m_SO3RoleAllInfo.m_bHadUpdateExtInfo    = false;
    m_SO3RoleAllInfo.m_bHadExtInfo          = false;
    m_SO3RoleAllInfo.m_pBuffDBData          = NULL;
    m_SO3RoleAllInfo.m_pCooldownListDBData  = NULL;
    m_SO3RoleAllInfo.m_pItemData            = NULL;
    m_SO3RoleAllInfo.m_pPQList              = NULL;
    m_SO3RoleAllInfo.m_pForceDBData         = NULL;
    m_SO3RoleAllInfo.m_pRoleRoadInfo        = NULL;
    m_SO3RoleAllInfo.m_pSkillData           = NULL;
    m_SO3RoleAllInfo.m_pSkillRecipeList     = NULL;
    m_SO3RoleAllInfo.m_pProfessionData      = NULL;

    m_pSO3RoleDBTool = new KSO3RoleDBTools;
    KGLOG_PROCESS_ERROR(m_pSO3RoleDBTool);

    nRetCode = m_pSO3RoleDBTool->Init(this, m_RoleDBConfig);
    KGLOG_PROCESS_ERROR(nRetCode);
    nSO3RoleDBToolFlag = true;

    m_pNewSO3RoleDBTool = new KSO3RoleDBTools;
    KGLOG_PROCESS_ERROR(m_pNewSO3RoleDBTool);

    nRetCode = m_pNewSO3RoleDBTool->Init(this, m_NewRoleDBConfig);
    KGLOG_PROCESS_ERROR(nRetCode);
    nNewSO3RoleDBToolFlag = true;

    //if (!m_nUpdateToLocal)
    //{
    //    if (m_pNewSO3RoleDBTool->m_nDBState == dbs_Exist)
    //    {
    //        KGLogPrintf(KGLOG_ERR, "The New Connect MYSQL DB %s had already Existed ! Please Modify the SO3RoleDBUpdater.ini!", m_NewRoleDBConfig.szDBName);
    //        KGLOG_PROCESS_ERROR(false);
    //    }
    //}
    
    m_pCompareSoleDBTool = new KSO3RoleDBTools;
    KGLOG_PROCESS_ERROR(m_pSO3RoleDBTool);

    nRetCode = m_pCompareSoleDBTool->Init(this, m_CompareRoleDBConfig);
    KGLOG_PROCESS_ERROR(nRetCode);
    nCompareSO3RoleDBToolFlag = true;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (nCompareSO3RoleDBToolFlag)
        {
            m_pCompareSoleDBTool->UnInit();
            nCompareSO3RoleDBToolFlag = false;
        }
        KG_DELETE(m_pCompareSoleDBTool);
        if (nNewSO3RoleDBToolFlag)
        {
            m_pNewSO3RoleDBTool->UnInit();
            nNewSO3RoleDBToolFlag = false;
        }

        KG_DELETE(m_pNewSO3RoleDBTool);

        if (nSO3RoleDBToolFlag)
        {
            m_pSO3RoleDBTool->UnInit();
            nSO3RoleDBToolFlag = false;
        }
        
        KG_DELETE(m_pSO3RoleDBTool);

        if (nRoleAllInfoFlag)
        {
            KG_COM_RELEASE(m_SO3RoleAllInfo.m_piPackage);
        }
        
        if (nMailInfoFlag)
        {
            KG_COM_RELEASE(m_piMailBoxInfoPackage);
        }
    }
    return nResult;
}

int KSO3RoleDBUpdater::Run()
{
    int nResult     = false;
    int nRetCode    = false;

    KGLOG_PROCESS_ERROR(m_pNewSO3RoleDBTool);

    nRetCode = m_pNewSO3RoleDBTool->CheckRoleTable();//创建角色数据表
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pSO3RoleDBTool->LoadAndUpdateTable(ROLE_TABLE_NAME, KG_LAST_MODIFY_FIELD_NAME); // role表
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pSO3RoleDBTool->LoadAndUpdateTable(DELETE_ROLE_TABLE_NAME, KG_DELETE_TIME_FIELD_NAME); // deleterole表
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pSO3RoleDBTool->LoadAndUpdateTable(RESTORE_ROLE_TABLE_NAME, KG_DELETE_TIME_FIELD_NAME); // restorerole表
    KGLOG_PROCESS_ERROR(nRetCode);

    //nRetCode = m_pSO3RoleDBTool->LoadMailBoxTable();// 信箱
    //KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pCompareSoleDBTool->LoadAndCompareTable(ROLE_TABLE_NAME, KG_LAST_MODIFY_FIELD_NAME);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pCompareSoleDBTool->LoadAndCompareTable(DELETE_ROLE_TABLE_NAME, KG_DELETE_TIME_FIELD_NAME);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pCompareSoleDBTool->LoadAndCompareTable(RESTORE_ROLE_TABLE_NAME, KG_DELETE_TIME_FIELD_NAME);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    nResult = true;
    return nResult;
}

void KSO3RoleDBUpdater::UnInit()
{
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pBuffDBData);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pCooldownListDBData);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pItemData);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pPQList);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pForceDBData);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pRoleRoadInfo);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pSkillData);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pSkillRecipeList);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pProfessionData);
    KG_COM_RELEASE(m_SO3RoleAllInfo.m_piPackage);

    m_pCompareSoleDBTool->UnInit();
    m_pNewSO3RoleDBTool->UnInit();
    m_pSO3RoleDBTool->UnInit();
    
    KG_DELETE(m_pCompareSoleDBTool);
    KG_DELETE(m_pNewSO3RoleDBTool);
    KG_DELETE(m_pSO3RoleDBTool);
    KG_COM_RELEASE(m_piMailBoxInfoPackage);
}

int KSO3RoleDBUpdater::UpdateExtInfo(BYTE* pbyData, size_t uDataLen)
{
    int           nResult           = false;
    int           nRetCode          = false;

    size_t              uLeftSize         = uDataLen;
    BYTE*               pbyOffset         = pbyData;
    KRoleDataHeader*    pGlobalHeader     = NULL;
    KRoleDBData*        pSaveRoleDBData   = NULL;
    size_t              uSaveLeftSize     = 0;
    BYTE*               pbySaveOffset     = 0;
    size_t              uSaveRoleDataLen  = 0;
    DWORD               dwCRC             = 0;

    bool            bNotHadBlock = false;
    int            nBlockIndex = 0;
    KRoleDataBlock MaxRoleDataBlock[rbtTotal];
    BYTE           byTempData[MAX_ROLE_DATA_SIZE];
    size_t         uTempDataSaveLeftSize     = MAX_ROLE_DATA_SIZE;
    BYTE*          pbyTempDataSaveOffset     = byTempData;

    memset(MaxRoleDataBlock, 0, sizeof(MaxRoleDataBlock));

    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_piPackage);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KRoleDataHeader));
    pGlobalHeader = (KRoleDataHeader*)pbyOffset;

    uLeftSize -= sizeof(KRoleDataHeader);
    pbyOffset += sizeof(KRoleDataHeader);

    KGLOG_PROCESS_ERROR(pGlobalHeader->dwVer == 0);
    KGLOG_PROCESS_ERROR(pGlobalHeader->dwLen == uLeftSize);

    dwCRC = CRC32(0, pbyOffset, (DWORD)uLeftSize);
    KGLOG_PROCESS_ERROR(dwCRC == pGlobalHeader->dwCRC);

    pSaveRoleDBData = (KRoleDBData*)m_SO3RoleAllInfo.m_piPackage->GetData();
    uSaveLeftSize   = m_SO3RoleAllInfo.m_piPackage->GetSize();
    pbySaveOffset   = (BYTE*)pSaveRoleDBData;

    uSaveLeftSize -= sizeof(KRoleDBData);
    pbySaveOffset += sizeof(KRoleDBData);

    uSaveRoleDataLen += sizeof(KRoleDBData);

    KRoleDataBlock* pSaveRoleDataBlock  =  &MaxRoleDataBlock[nBlockIndex];
    
    while (uLeftSize > 0)
    {
        KRoleBlockHeader* pBlock = NULL;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KRoleBlockHeader));

        pBlock                      = (KRoleBlockHeader*)pbyOffset;
        BYTE*  pbyTempOffset        =  pbyOffset;

        uLeftSize -= sizeof(KRoleBlockHeader);
        pbyOffset += sizeof(KRoleBlockHeader);
        
        KGLOG_PROCESS_ERROR(uLeftSize >= pBlock->dwLen);

        size_t          uTempSaveLen = 0;

        switch (pBlock->nType)
        {
        case rbtSkillList:
            nRetCode = ConverSkillList(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtSkillList;
            memcpy(pbyTempDataSaveOffset, pbyOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtSkillRecipeList:
            nRetCode = ConverSkillRecipeList(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtSkillRecipeList;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtItemList:
            nRetCode = ConverItemList(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtItemList;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtQuestList:
            nRetCode = ConverQuestList(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtQuestList;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtProfessionList:
            nRetCode = ConverProfessionList(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtProfessionList;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtRecipeList:
            nRetCode = ConverRecipeList(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtRecipeList;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtStateInfo:
            nRetCode = ConverStateInfo(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtStateInfo;
            nRetCode = SaveStateInfo(&uTempSaveLen, pbyTempDataSaveOffset, uSaveLeftSize);
            _ASSERT(nRetCode);
            pSaveRoleDataBlock->wDataLen    =   (WORD)uTempSaveLen;
            uSaveRoleDataLen                +=  uTempSaveLen;
            //
            break;

        case rbtBuffList:
            nRetCode = ConverBuffList(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtBuffList;
            nRetCode = SaveBuffList(&uTempSaveLen, pbyTempDataSaveOffset, uSaveLeftSize);
            _ASSERT(nRetCode);
            pSaveRoleDataBlock->wDataLen    =   (WORD)uTempSaveLen;
            uSaveRoleDataLen                +=  uTempSaveLen;
            //
            break;

        case rbtRepute:
            nRetCode = ConverRepute(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtRepute;
            nRetCode = SaveReputeList(&uTempSaveLen, pbyTempDataSaveOffset, uSaveLeftSize);
            _ASSERT(nRetCode);
            pSaveRoleDataBlock->wDataLen    =   (WORD)uTempSaveLen;
            uSaveRoleDataLen                +=  uTempSaveLen;
            //
            break;

        case rbtUserPreferences:
            nRetCode = ConverUserPreferences(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtUserPreferences;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtBookState:
            nRetCode = ConverBookState(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtBookState;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtCoolDownTimer:
            nRetCode = ConverCoolDownTimer(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtCoolDownTimer;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtRoadOpenList:
            nRetCode = ConverRoadOpenList(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtRoadOpenList;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtCustomData:
            nRetCode = ConverCustomData(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtCustomData;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtVisitedMap:
            nRetCode = ConverVisitedMap(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtVisitedMap;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        case rbtPQList:
            nRetCode = ConverPQList(pbyOffset, pBlock->dwLen);
            _ASSERT(nRetCode);
            //
            pSaveRoleDataBlock->byBlockType = rbtPQList;
            memcpy(pbyTempDataSaveOffset, pbyTempOffset, pBlock->dwLen);
            pSaveRoleDataBlock->wDataLen    =   (WORD)pBlock->dwLen;
            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
            //
            break;

        default:
            bNotHadBlock = true;
            ASSERT(true); //todo:这里的true是临时的！
            //ASSERT(false); //todo:这里的true是临时的！
        }

        if (bNotHadBlock)
        {
            uLeftSize -= pBlock->dwLen;
            pbyOffset += pBlock->dwLen;
            bNotHadBlock = false;
        }
        else
        {
            nBlockIndex++;

            uLeftSize -= pBlock->dwLen;
            pbyOffset += pBlock->dwLen;

            uTempDataSaveLeftSize -= pSaveRoleDataBlock->wDataLen;
            pbyTempDataSaveOffset += pSaveRoleDataBlock->wDataLen;

            //pSaveRoleDataBlock += sizeof(KRoleDataBlock);
            pSaveRoleDataBlock = &MaxRoleDataBlock[nBlockIndex];
        }
    }

    BYTE* pbyCopyHeader = NULL;

    uSaveRoleDataLen += sizeof(KRoleDataBlock) * nBlockIndex;
    uSaveRoleDataLen += MAX_ROLE_DATA_SIZE - uTempDataSaveLeftSize;
    pbyCopyHeader = (BYTE*)pSaveRoleDBData;
    memcpy(pbyCopyHeader + sizeof(KRoleDBData), MaxRoleDataBlock, sizeof(KRoleDataBlock) * nBlockIndex);
    memcpy(pbyCopyHeader + sizeof(KRoleDBData) + sizeof(KRoleDataBlock) * nBlockIndex, byTempData, MAX_ROLE_DATA_SIZE - uTempDataSaveLeftSize);

    pSaveRoleDBData->wVersion           = 11;//modify
    pSaveRoleDBData->wBlockCount        = (WORD)nBlockIndex;
    pSaveRoleDBData->dwDataLen          = (DWORD)uSaveRoleDataLen;
    pSaveRoleDBData->dwCRC = 0;
    pSaveRoleDBData->dwCRC              = CRC32(0, pSaveRoleDBData, pSaveRoleDBData->dwDataLen);
    m_SO3RoleAllInfo.m_uPackageUseSize  = uSaveRoleDataLen;

    //Exit1:
    nResult = true;
Exit0:
    return nResult;
}

//int KSO3RoleDBUpdater::UpdateExtInfo(BYTE* pbyData, size_t uDataLen)
//{
//    int           nResult           = false;
//    int           nRetCode          = false;
//
//    size_t        uLeftSize         = uDataLen;
//    BYTE*         pbyOffset         = pbyData;
//    KRoleDBData*  pRoleDBData       = NULL;
//    KRoleDBData*  pSaveRoleDBData   = NULL;
//    size_t        uSaveLeftSize     = 0;
//    BYTE*         pbySaveOffset     = 0;
//    size_t        uSaveRoleDataLen  = 0;
//
//    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_piPackage);
//
//    pRoleDBData     =   (KRoleDBData*)pbyOffset;
//    uLeftSize       -=  sizeof(KRoleDBData);
//    pbyOffset       +=  sizeof(KRoleDBData);
//
//    //if (pRoleDBData->wVersion == rdvCurrentRoleDataVersion)
//    //{
//    //    m_SO3RoleAllInfo.m_bHadUpdateExtInfo = true;
//    //    memcpy(m_SO3RoleAllInfo.m_piPackage->GetData(), pbyData, uDataLen);
//    //    m_SO3RoleAllInfo.m_uPackageUseSize = uDataLen;
//    //    goto Exit1;
//    //}
//    
//    KGLOG_PROCESS_ERROR(pRoleDBData->dwDataLen == uDataLen);
//    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KRoleDataBlock) * pRoleDBData->wBlockCount);
//
//    pSaveRoleDBData = (KRoleDBData*)m_SO3RoleAllInfo.m_piPackage->GetData();
//    uSaveLeftSize   = m_SO3RoleAllInfo.m_piPackage->GetSize();
//    pbySaveOffset   = (BYTE*)pSaveRoleDBData;
//
//    uSaveLeftSize -= sizeof(KRoleDBData);
//    pbySaveOffset += sizeof(KRoleDBData);
//
//    uSaveRoleDataLen += sizeof(KRoleDBData);
//
//    //pSaveRoleDBData->wVersion       = rdvCurrentRoleDataVersion;
//    pSaveRoleDBData->wVersion       = 11;//modify
//    pSaveRoleDBData->wBlockCount    = pRoleDBData->wBlockCount;
//
//    uLeftSize -= sizeof(KRoleDataBlock) * pRoleDBData->wBlockCount;
//    pbyOffset += sizeof(KRoleDataBlock) * pRoleDBData->wBlockCount;
//
//    uSaveLeftSize -= sizeof(KRoleDataBlock) * pRoleDBData->wBlockCount;
//    pbySaveOffset += sizeof(KRoleDataBlock) * pRoleDBData->wBlockCount;
//
//    uSaveRoleDataLen += sizeof(KRoleDataBlock) * pRoleDBData->wBlockCount;;
//
//    for (int nBlockIndex = 0; nBlockIndex < pRoleDBData->wBlockCount; nBlockIndex++)
//    {
//        KRoleDataBlock* pRoleDataBlock  = &pRoleDBData->BlockList[nBlockIndex];
//        size_t          Temp            = pRoleDataBlock->wDataLen;
//        KGLOG_PROCESS_ERROR(uLeftSize >= Temp);
//
//        KRoleDataBlock* pSaveRoleDataBlock  = &pSaveRoleDBData->BlockList[nBlockIndex];
//        BYTE*           pbyTempOffset       =  pbyOffset;
//        size_t          uTempSaveLen = 0;
//
//        switch (pRoleDataBlock->byBlockType)
//        {
//        case rbtSkillList:
//            nRetCode = ConverSkillList(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtSkillList;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtSkillRecipeList:
//            nRetCode = ConverSkillRecipeList(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtSkillRecipeList;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtItemList:
//            nRetCode = ConverItemList(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtItemList;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtQuestList:
//            nRetCode = ConverQuestList(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtQuestList;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtProfessionList:
//            nRetCode = ConverProfessionList(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtProfessionList;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtRecipeList:
//            nRetCode = ConverRecipeList(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtRecipeList;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtStateInfo:
//            nRetCode = ConverStateInfo(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtStateInfo;
//            nRetCode = SaveStateInfo(&uTempSaveLen, pbySaveOffset, uSaveLeftSize);
//            _ASSERT(nRetCode);
//            pSaveRoleDataBlock->wDataLen    =   (WORD)uTempSaveLen;
//            uSaveRoleDataLen                +=  uTempSaveLen;
//            //
//            break;
//
//        case rbtBuffList:
//            nRetCode = ConverBuffList(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtBuffList;
//            nRetCode = SaveBuffList(&uTempSaveLen, pbySaveOffset, uSaveLeftSize);
//            _ASSERT(nRetCode);
//            pSaveRoleDataBlock->wDataLen    =   (WORD)uTempSaveLen;
//            uSaveRoleDataLen                +=  uTempSaveLen;
//            //
//            break;
//
//        case rbtRepute:
//            nRetCode = ConverRepute(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtRepute;
//            nRetCode = SaveReputeList(&uTempSaveLen, pbySaveOffset, uSaveLeftSize);
//            _ASSERT(nRetCode);
//            pSaveRoleDataBlock->wDataLen    =   (WORD)uTempSaveLen;
//            uSaveRoleDataLen                +=  uTempSaveLen;
//            //
//            break;
//
//        case rbtUserPreferences:
//            nRetCode = ConverUserPreferences(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtUserPreferences;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtBookState:
//            nRetCode = ConverBookState(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtBookState;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtCoolDownTimer:
//            nRetCode = ConverCoolDownTimer(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtCoolDownTimer;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtRoadOpenList:
//            nRetCode = ConverRoadOpenList(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtRoadOpenList;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtCustomData:
//            nRetCode = ConverCustomData(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtCustomData;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtVisitedMap:
//            nRetCode = ConverVisitedMap(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtVisitedMap;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        case rbtPQList:
//            nRetCode = ConverPQList(pbyOffset, pRoleDataBlock->wDataLen);
//            _ASSERT(nRetCode);
//            //
//            pSaveRoleDataBlock->byBlockType = rbtPQList;
//            memcpy(pbySaveOffset, pbyTempOffset, pRoleDataBlock->wDataLen);
//            pSaveRoleDataBlock->wDataLen    =   pRoleDataBlock->wDataLen;
//            uSaveRoleDataLen                +=  pSaveRoleDataBlock->wDataLen;
//            //
//            break;
//
//        default:
//            ASSERT(true); //todo:这里的true是临时的！
//            //ASSERT(false); //todo:这里的true是临时的！
//        }
//
//        uLeftSize -= pRoleDataBlock->wDataLen;
//        pbyOffset += pRoleDataBlock->wDataLen;
//
//        uSaveLeftSize -= pSaveRoleDataBlock->wDataLen;
//        pbySaveOffset += pSaveRoleDataBlock->wDataLen;
//    }
//
//    pSaveRoleDBData->dwDataLen          = (DWORD)uSaveRoleDataLen;
//    pSaveRoleDBData->dwCRC              = 0;
//    pSaveRoleDBData->dwCRC              = CRC32(0, pSaveRoleDBData, pSaveRoleDBData->dwDataLen);
//    m_SO3RoleAllInfo.m_uPackageUseSize  = uSaveRoleDataLen;
//
////Exit1:
//    nResult = true;
//Exit0:
//    return nResult;
//}

int KSO3RoleDBUpdater::ConverSkillList(BYTE* pbyData, size_t uDataLen)
{
    int              nResult     = false;
//    int              nRetCode    = false;

    size_t           uLeftSize   = uDataLen;
    BYTE*            pbyOffset   = pbyData;
    KDB_SKILL_DATA*  pSkillData  = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_SKILL_DATA));

    pSkillData = (KDB_SKILL_DATA*)pbyOffset;
    uLeftSize -= sizeof(KDB_SKILL_DATA);
    pbyOffset += sizeof(KDB_SKILL_DATA);

    m_SO3RoleAllInfo.m_pSkillData = (KDB_SKILL_DATA*)new char[sizeof(KDB_SKILL_DATA) + sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM) * pSkillData->nSkillCount];
    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_pSkillData);

    m_SO3RoleAllInfo.m_pSkillData->dwMountKungfuID    = pSkillData->dwMountKungfuID;
    m_SO3RoleAllInfo.m_pSkillData->dwMountKungfuLevel = pSkillData->dwMountKungfuLevel;
    m_SO3RoleAllInfo.m_pSkillData->nSkillCount        = pSkillData->nSkillCount;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM) * pSkillData->nSkillCount);

    for (int i = 0; i < pSkillData->nSkillCount; i++)
    {
        KDB_SKILL_DATA::KDB_SKILL_ITEM*  pSkillItem  = &pSkillData->SkillItems[i];
        KGLOG_PROCESS_ERROR(pSkillItem);

        m_SO3RoleAllInfo.m_pSkillData->SkillItems[i].wSkillID   = pSkillItem->wSkillID;
        m_SO3RoleAllInfo.m_pSkillData->SkillItems[i].byMaxLevel = pSkillItem->byMaxLevel;
        m_SO3RoleAllInfo.m_pSkillData->SkillItems[i].dwExp      = pSkillItem->dwExp;

        uLeftSize -= sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM);
        pbyOffset += sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM);
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_SO3RoleAllInfo.m_pSkillData;
        m_SO3RoleAllInfo.m_pSkillData = NULL;
    }
    return nResult;
}

int KSO3RoleDBUpdater::ConverSkillRecipeList(BYTE* pbyData, size_t uDataLen)
{
    int                               nResult          = false;
//    int                               nRetCode         = false;

    size_t                            uLeftSize        = uDataLen;
    BYTE*                             pbyOffset        = pbyData;
    KG_SKILL_RECIPE_LIST_DB_DATA_V2*  pSkillRecipeData = NULL;

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2));

    pSkillRecipeData = (KG_SKILL_RECIPE_LIST_DB_DATA_V2*)pbyOffset;
    uLeftSize       -= sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2);
    pbyOffset       += sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2);

    KGLOG_PROCESS_ERROR(pSkillRecipeData->wVersion >= KG_SKILL_RECIPE_DB_LOAD_LOWEST_VER);
    KGLOG_PROCESS_ERROR(pSkillRecipeData->wVersion <= KG_SKILL_RECIPE_DB_CURRENT_VER);

    m_SO3RoleAllInfo.m_pSkillRecipeList = (KG_SKILL_RECIPE_LIST_DB_DATA_V2*)new char[sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2) + sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2::_RecipeNode)* pSkillRecipeData->wCount];
    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_pSkillRecipeList);

    m_SO3RoleAllInfo.m_pSkillRecipeList->wCount = pSkillRecipeData->wCount;

    for (int i = 0; i < pSkillRecipeData->wCount; i++)
    {
        m_SO3RoleAllInfo.m_pSkillRecipeList->RecipeArray[i].byActive     = pSkillRecipeData->RecipeArray[i].byActive;
        m_SO3RoleAllInfo.m_pSkillRecipeList->RecipeArray[i].dwRecipeKey  = pSkillRecipeData->RecipeArray[i].dwRecipeKey;

        uLeftSize -= sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2::_RecipeNode);
        pbyOffset += sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2::_RecipeNode);
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    //Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_SO3RoleAllInfo.m_pSkillRecipeList;
        m_SO3RoleAllInfo.m_pSkillRecipeList = NULL;
    }
    return nResult;
}

int KSO3RoleDBUpdater::ConverItemList(BYTE* pbyData, size_t uDataLen)
{
    int        nResult   = false;
//    int        nRetCode  = false;

    size_t     uLeftSize = uDataLen;
    BYTE*      pbyOffset = pbyData;
    ItemData*  pItemData = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(ItemData));

    pItemData = (ItemData*)pbyOffset;
    uLeftSize -= sizeof(ItemData);
    pbyOffset += sizeof(ItemData);

    m_SO3RoleAllInfo.m_pItemData = (ItemData* )new char[sizeof(ItemData) + sizeof(ItemData::KITEM_DB_HEADEREX) * pItemData->m_wItemCount];
    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_pItemData);

    m_SO3RoleAllInfo.m_pItemData->m_dwMoney                  = pItemData->m_dwMoney;
    m_SO3RoleAllInfo.m_pItemData->m_wEnabledBankPackageCount = pItemData->m_wEnabledBankPackageCount;
    m_SO3RoleAllInfo.m_pItemData->m_wItemCount               = pItemData->m_wItemCount;

    for (int i = 0; i < pItemData->m_wItemCount; i++)
    {
        ItemData::KITEM_DB_HEADEREX* pItemDBHeaderex = (ItemData::KITEM_DB_HEADEREX*)pbyOffset;

        m_SO3RoleAllInfo.m_pItemData->ItemArray[i].byBox         = pItemDBHeaderex->byBox;
        if (pItemDBHeaderex->byBox == ibEquip)
        {
            //if (pItemDBHeaderex->byPos == eitArrow)
            //{
            //    KGLogPrintf(KGLOG_INFO, "The eitArrow had Item?");
            //}
            if (pItemDBHeaderex->byPos >= eitArrow)
            {
                m_SO3RoleAllInfo.m_pItemData->ItemArray[i].byPos         = pItemDBHeaderex->byPos + 1;
                pItemDBHeaderex->byPos = pItemDBHeaderex->byPos + 1;
            }
            else
            {
                m_SO3RoleAllInfo.m_pItemData->ItemArray[i].byPos         = pItemDBHeaderex->byPos;
            }

        }
        //if (pItemDBHeaderex->byBox > ibBankPackage5)
        //{
        //    KGLogPrintf(KGLOG_INFO, "had box  > ibBankPackage5 !");
        //}

        m_SO3RoleAllInfo.m_pItemData->ItemArray[i].byDataLen     = pItemDBHeaderex->byDataLen;

        if (pItemDBHeaderex->byDataLen == sizeof(KCOMMON_ITEM_DATA))
        {
            m_SO3RoleAllInfo.m_pItemData->ItemArray[i].commonEquipData = pItemDBHeaderex->commonEquipData;
        }
        else if(pItemDBHeaderex->byDataLen == sizeof(KCUSTOM_EQUI_DATA))
        {
            m_SO3RoleAllInfo.m_pItemData->ItemArray[i].customEquipData = pItemDBHeaderex->customEquipData;
        }
        else if(pItemDBHeaderex->byDataLen == sizeof(KTRAIN_EQUIP_DATA))
        {
            // 逻辑暂时未处理
            m_SO3RoleAllInfo.m_pItemData->ItemArray[i].trainEquipData = pItemDBHeaderex->trainEquipData;
        }
        else
        {
            KGLOG_PROCESS_ERROR(false);
        }


        int nTempLength = pItemDBHeaderex->byDataLen + sizeof(BYTE) + sizeof(BYTE) + sizeof(BYTE);

        uLeftSize -= nTempLength;
        pbyOffset += nTempLength;
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_SO3RoleAllInfo.m_pItemData;
        m_SO3RoleAllInfo.m_pItemData = NULL;
    }
    return nResult;
}

int KSO3RoleDBUpdater::ConverQuestList(BYTE* pbyData, size_t uDataLen)
{
    int        nResult    = false;
//    int        nRetCode   = false;

    size_t     uLeftSize  = uDataLen;
    BYTE*      pbyOffset  = pbyData;

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(DWORD) * MAX_QUEST_COUNT / 32);

    for (int i = 0; i < MAX_QUEST_COUNT / 32; i++)
    {
        DWORD dwQuestState = 0;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(DWORD));

        dwQuestState  = *(DWORD*)pbyOffset;
        uLeftSize    -= sizeof(DWORD);
        pbyOffset    += sizeof(DWORD);

        for (int nIndex = 0; nIndex < 32; nIndex++)
        {
            m_SO3RoleAllInfo.m_QuestData.m_dwQuestState[i * 32 + nIndex] = (BYTE)dwQuestState & 0x01;

            dwQuestState >>= 1;
        }
    }

    m_SO3RoleAllInfo.m_QuestData.m_AcceptQuestCount = *(BYTE*)pbyOffset;
    uLeftSize -= sizeof(BYTE);
    pbyOffset += sizeof(BYTE);

    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_QuestData.m_AcceptQuestCount <= MAX_ACCEPT_QUEST_COUNT);

    for (int i = 0; i < m_SO3RoleAllInfo.m_QuestData.m_AcceptQuestCount; i++)
    {
        KDB_ACCEPT_QUEST*  pDBQuest= NULL;

        pDBQuest   = (KDB_ACCEPT_QUEST*)pbyOffset;
        uLeftSize -= sizeof(KDB_ACCEPT_QUEST);
        pbyOffset += sizeof(KDB_ACCEPT_QUEST);

        m_SO3RoleAllInfo.m_QuestData.m_AcceptQuest[i].wQuestID     = pDBQuest->wQuestID;
        m_SO3RoleAllInfo.m_QuestData.m_AcceptQuest[i].byFailed     = pDBQuest->byFailed;
        m_SO3RoleAllInfo.m_QuestData.m_AcceptQuest[i].nLimitTime   = pDBQuest->nLimitTime;

        for (int j = 0; j < QUEST_PARAM_COUNT; j++)
        {
            m_SO3RoleAllInfo.m_QuestData.m_AcceptQuest[i].byKillNpcCount[j] = pDBQuest->byKillNpcCount[j];
        }

        for (int j = 0; j < QUEST_PARAM_COUNT * 2; j++)
        {
            m_SO3RoleAllInfo.m_QuestData.m_AcceptQuest[i].nQuestValue[j]    = pDBQuest->nQuestValue[j];
        } 
    }

    m_SO3RoleAllInfo.m_QuestData.m_AssistQuestCount = *(BYTE*)pbyOffset;
    uLeftSize -= sizeof(BYTE);
    pbyOffset += sizeof(BYTE);

    m_SO3RoleAllInfo.m_QuestData.m_AssistDailyCount = *(BYTE*)pbyOffset;
    uLeftSize -= sizeof(BYTE);
    pbyOffset += sizeof(BYTE);

    KGLOG_PROCESS_ERROR( m_SO3RoleAllInfo.m_QuestData.m_AssistQuestCount <= MAX_ASSIST_QUEST_COUNT);

    for (int i = 0; i < m_SO3RoleAllInfo.m_QuestData.m_AssistQuestCount; i++)
    {
        KDB_ASSIST_QUEST* pDBQuest = NULL;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_ASSIST_QUEST));

        pDBQuest = (KDB_ASSIST_QUEST*)pbyOffset;
        uLeftSize -= sizeof(KDB_ASSIST_QUEST);
        pbyOffset += sizeof(KDB_ASSIST_QUEST);

        m_SO3RoleAllInfo.m_QuestData.m_AssistQuest[i].dwPlayerID = pDBQuest->dwPlayerID;
        m_SO3RoleAllInfo.m_QuestData.m_AssistQuest[i].wQuestID   = pDBQuest->wQuestID;
    }

    m_SO3RoleAllInfo.m_QuestData.m_DailyQuestCount = *(BYTE*)pbyOffset;
    uLeftSize -= sizeof(BYTE);
    pbyOffset += sizeof(BYTE);

    KGLOG_PROCESS_ERROR( m_SO3RoleAllInfo.m_QuestData.m_DailyQuestCount <= MAX_ACCEPT_QUEST_COUNT);

    for (int i = 0; i < m_SO3RoleAllInfo.m_QuestData.m_DailyQuestCount; i++)
    {
        KDB_DAILY_QUEST* pDailyQuest = NULL;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_DAILY_QUEST));

        pDailyQuest = (KDB_DAILY_QUEST*)pbyOffset;
        uLeftSize -= sizeof(KDB_DAILY_QUEST);
        pbyOffset += sizeof(KDB_DAILY_QUEST);

        m_SO3RoleAllInfo.m_QuestData.m_DailyQuest[i].wQuestID        = pDailyQuest->wQuestID;
        m_SO3RoleAllInfo.m_QuestData.m_DailyQuest[i].nNextAcceptTime = pDailyQuest->nNextAcceptTime;
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBUpdater::ConverProfessionList(BYTE* pbyData, size_t uDataLen)
{
    int        nResult          = false;
//    int        nRetCode         = false;

    size_t     uLeftSize        = uDataLen;
    BYTE*      pbyOffset        = pbyData;
    BYTE       byCount          = 0;

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KProfessionDBData));

    byCount    = *(BYTE*)pbyOffset;
    uLeftSize -= sizeof(KProfessionDBData);
    pbyOffset += sizeof(KProfessionDBData);

    m_SO3RoleAllInfo.m_pProfessionData = (KProfessionDBData* )new char[sizeof(KProfessionDBData) + sizeof(KProfessionItem) * byCount];
    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_pProfessionData);

    m_SO3RoleAllInfo.m_pProfessionData->byCount = byCount;

    for (int i = 0; i < byCount; i++)
    {
        KProfessionItem* pProfessionDBData = (KProfessionItem*)pbyOffset;

        m_SO3RoleAllInfo.m_pProfessionData->Profession[i].m_byBranchID     = pProfessionDBData->m_byBranchID;
        m_SO3RoleAllInfo.m_pProfessionData->Profession[i].m_byCurrentLevel = pProfessionDBData->m_byCurrentLevel;
        m_SO3RoleAllInfo.m_pProfessionData->Profession[i].m_byMaxLevel     = pProfessionDBData->m_byMaxLevel;
        m_SO3RoleAllInfo.m_pProfessionData->Profession[i].m_byProfessionID = pProfessionDBData->m_byProfessionID;
        m_SO3RoleAllInfo.m_pProfessionData->Profession[i].m_wProficiency   = pProfessionDBData->m_wProficiency;

        uLeftSize -= sizeof(KProfessionItem);
        pbyOffset += sizeof(KProfessionItem);

    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_SO3RoleAllInfo.m_pProfessionData;
        m_SO3RoleAllInfo.m_pProfessionData = NULL;
    }
    return nResult;
}

int KSO3RoleDBUpdater::ConverRecipeList(BYTE* pbyData, size_t uDataLen)
{
    int             nResult          = false;
//    int             nRetCode         = false;

    size_t          uLeftSize        = uDataLen;
    BYTE*           pbyOffset        = pbyData;
    BYTE            byCount          = 0;
    BOOL            bRecipeLearned   = false;
    KRecipeDBData*  pRecipeDBData    = (KRecipeDBData*)pbyData;

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KRecipeDBData));

    byCount    = *(BYTE*)pbyOffset;
    uLeftSize -= sizeof(KRecipeDBData);
    pbyOffset += sizeof(KRecipeDBData);

    for (int i = 0; i < byCount; i++)
    {
        KRecipeDBItem* pRecipeDBItem = &(pRecipeDBData->RecipeDBItem[i]);

        for (int j = 0; j < sizeof(pRecipeDBItem->byRecipeData) / sizeof(pRecipeDBItem->byRecipeData[0]); j++)
        {
            BYTE byBitData = pRecipeDBItem->byRecipeData[j];

            for (int nSubIndex = 0; nSubIndex < CHAR_BIT; ++nSubIndex)
            {
                int nRecipeIndex = j * CHAR_BIT + nSubIndex;
                if (nRecipeIndex < MAX_RECIPE_ID)
                {
                    bRecipeLearned = (byBitData & (1 << (CHAR_BIT - nSubIndex - 1))) ? true : false;
                    m_SO3RoleAllInfo.m_byRecipeState[pRecipeDBItem->byCraftID - 1][nRecipeIndex] = (BYTE)bRecipeLearned;
                }
            }
        }
        uLeftSize -= sizeof(KRecipeDBItem);
        pbyOffset += sizeof(KRecipeDBItem);
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBUpdater::ConverStateInfo(BYTE* pbyData, size_t uDataLen)
{
    int                     nResult        = false;

    size_t                  uLeftSize      = uDataLen;
    BYTE*                   pbyOffset      = pbyData;
    KROLE_STATE_INFO*   pRoleStateInfo = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KROLE_STATE_INFO));

    pRoleStateInfo = (KROLE_STATE_INFO*)pbyOffset;
    uLeftSize -= sizeof(KROLE_STATE_INFO);
    pbyOffset += sizeof(KROLE_STATE_INFO);

    m_SO3RoleAllInfo.m_StateInfo.byMoveState              = pRoleStateInfo->byMoveState;
    m_SO3RoleAllInfo.m_StateInfo.wCurrentTrack            = pRoleStateInfo->wCurrentTrack;
    m_SO3RoleAllInfo.m_StateInfo.nMoveFrameCounter        = pRoleStateInfo->nMoveFrameCounter;
    m_SO3RoleAllInfo.m_StateInfo.wFromFlyNode             = pRoleStateInfo->wFromFlyNode;
    m_SO3RoleAllInfo.m_StateInfo.wTargetCityID            = pRoleStateInfo->wTargetCityID;

    m_SO3RoleAllInfo.m_StateInfo.nExperience              = pRoleStateInfo->nExperience;

    m_SO3RoleAllInfo.m_StateInfo.nCurrentLife             = pRoleStateInfo->nCurrentLife;
    m_SO3RoleAllInfo.m_StateInfo.nCurrentMana             = pRoleStateInfo->nCurrentMana;
    m_SO3RoleAllInfo.m_StateInfo.nCurrentStamina          = pRoleStateInfo->nCurrentStamina;
    m_SO3RoleAllInfo.m_StateInfo.nCurrentThew             = pRoleStateInfo->nCurrentThew;

    m_SO3RoleAllInfo.m_StateInfo.nAddTrainTimeInToday     = pRoleStateInfo->nAddTrainTimeInToday;
    m_SO3RoleAllInfo.m_StateInfo.nCurrentTrainValue       = pRoleStateInfo->nCurrentTrainValue;
    m_SO3RoleAllInfo.m_StateInfo.nUsedTrainValue          = pRoleStateInfo->nUsedTrainValue;
    m_SO3RoleAllInfo.m_StateInfo.wReserved                = pRoleStateInfo->wReserved;  //++

    m_SO3RoleAllInfo.m_StateInfo.byPKState                = pRoleStateInfo->byPKState;
    m_SO3RoleAllInfo.m_StateInfo.wCloseSlayLeftTime       = pRoleStateInfo->wCloseSlayLeftTime;

    m_SO3RoleAllInfo.m_StateInfo.wLeftReviveFrame         = pRoleStateInfo->wLeftReviveFrame;
    m_SO3RoleAllInfo.m_StateInfo.nLastSituReviveTime	  = pRoleStateInfo->nLastSituReviveTime;
    m_SO3RoleAllInfo.m_StateInfo.bySituReviveCount	      = pRoleStateInfo->bySituReviveCount;
    m_SO3RoleAllInfo.m_StateInfo.dwKillerID			      = pRoleStateInfo->dwKillerID;  

    m_SO3RoleAllInfo.m_StateInfo.wCurrentKillPoint        = pRoleStateInfo->wCurrentKillPoint;
    m_SO3RoleAllInfo.m_StateInfo.nLastKillPointReduceTime = pRoleStateInfo->nLastKillPointReduceTime;

    //m_SO3RoleAllInfo.m_StateInfo.nCurrentPrestige       = 0;//pRoleStateInfo->nCurrentPrestige; //++
    //m_SO3RoleAllInfo.m_StateInfo.nReserved1             = 0;//pRoleStateInfo->nReserved1;//++
    //m_SO3RoleAllInfo.m_StateInfo.nReserved2             = 0;//pRoleStateInfo->nReserved2;//++
    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KSO3RoleDBUpdater::SaveStateInfo(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                bResult         = false;
//    BOOL                bRetCode        = false;

    size_t              uLeftSize       = uBufferSize;
    BYTE*               pbyOffset       = pbyBuffer;
    KROLE_STATE_INFO_OLD*   pRoleStateInfo  = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KROLE_STATE_INFO_OLD));
    pRoleStateInfo = (KROLE_STATE_INFO_OLD*)pbyOffset;
    uLeftSize -= sizeof(KROLE_STATE_INFO_OLD);
    pbyOffset += sizeof(KROLE_STATE_INFO_OLD);

    ASSERT(cmsTotal < UCHAR_MAX);

    pRoleStateInfo->byMoveState                 = m_SO3RoleAllInfo.m_StateInfo.byMoveState;
    pRoleStateInfo->wCurrentTrack               = m_SO3RoleAllInfo.m_StateInfo.wCurrentTrack;
    pRoleStateInfo->nMoveFrameCounter           = m_SO3RoleAllInfo.m_StateInfo.nMoveFrameCounter;

    pRoleStateInfo->wFromFlyNode                = m_SO3RoleAllInfo.m_StateInfo.wFromFlyNode;
    pRoleStateInfo->wTargetCityID               = m_SO3RoleAllInfo.m_StateInfo.wTargetCityID;

    pRoleStateInfo->nExperience                 = m_SO3RoleAllInfo.m_StateInfo.nExperience ;

    pRoleStateInfo->nCurrentLife                = m_SO3RoleAllInfo.m_StateInfo.nCurrentLife;
    pRoleStateInfo->nCurrentMana                = m_SO3RoleAllInfo.m_StateInfo.nCurrentMana;
    pRoleStateInfo->nCurrentStamina             = m_SO3RoleAllInfo.m_StateInfo.nCurrentStamina;
    pRoleStateInfo->nCurrentThew                = m_SO3RoleAllInfo.m_StateInfo.nCurrentThew;

    pRoleStateInfo->nUsedTrainValue             = m_SO3RoleAllInfo.m_StateInfo.nUsedTrainValue;

    pRoleStateInfo->nCurrentTrainValue          = m_SO3RoleAllInfo.m_StateInfo.nCurrentTrainValue;
    pRoleStateInfo->nAddTrainTimeInToday        = m_SO3RoleAllInfo.m_StateInfo.nAddTrainTimeInToday;

    pRoleStateInfo->byPKState                   = m_SO3RoleAllInfo.m_StateInfo.byPKState;
    pRoleStateInfo->wCloseSlayLeftTime          = m_SO3RoleAllInfo.m_StateInfo.wCloseSlayLeftTime;
    pRoleStateInfo->wReserved                   = m_SO3RoleAllInfo.m_StateInfo.wReserved;

    pRoleStateInfo->wLeftReviveFrame            = m_SO3RoleAllInfo.m_StateInfo.wLeftReviveFrame;

    pRoleStateInfo->nLastSituReviveTime         = m_SO3RoleAllInfo.m_StateInfo.nLastSituReviveTime;
    pRoleStateInfo->bySituReviveCount	        = m_SO3RoleAllInfo.m_StateInfo.bySituReviveCount;
    pRoleStateInfo->dwKillerID			        = m_SO3RoleAllInfo.m_StateInfo.dwKillerID;

    pRoleStateInfo->wCurrentKillPoint           = m_SO3RoleAllInfo.m_StateInfo.wCurrentKillPoint;
    pRoleStateInfo->nLastKillPointReduceTime    = m_SO3RoleAllInfo.m_StateInfo.nLastKillPointReduceTime;

    //pRoleStateInfo->nCurrentPrestige            = m_SO3RoleAllInfo.m_StateInfo.nCurrentPrestige;
    //pRoleStateInfo->nReserved1                  = m_SO3RoleAllInfo.m_StateInfo.nReserved1;
    //pRoleStateInfo->nReserved2                  = m_SO3RoleAllInfo.m_StateInfo.nReserved2;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

int KSO3RoleDBUpdater::ConverBuffList(BYTE* pbyData, size_t uDataLen)
{
    int             nResult      = false;
//    int             nRetCode     = false;

    size_t          uLeftSize    = uDataLen;
    BYTE*           pbyOffset    = pbyData;
    KBUFF_DB_DATA*  pBuffDBData  = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KBUFF_DB_DATA));
    pBuffDBData = (KBUFF_DB_DATA*)pbyOffset;
    BYTE count = pBuffDBData->byCount;

    uLeftSize -= sizeof(KBUFF_DB_DATA);
    pbyOffset += sizeof(KBUFF_DB_DATA);

    m_SO3RoleAllInfo.m_pBuffDBData = (KBUFF_DB_DATA_OLD* )new char[sizeof(KBUFF_DB_DATA_OLD) + sizeof(KBUFF_DB_DATA_OLD::KBUFF_DB_ITEM) * pBuffDBData->byCount];
    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_pBuffDBData);

    m_SO3RoleAllInfo.m_pBuffDBData->byCount = count;

    for (int i = 0; i < count; i++)
    {
        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM));
        uLeftSize -= sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM);
        pbyOffset += sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM);

        m_SO3RoleAllInfo.m_pBuffDBData->Items[i].byStackNum      = pBuffDBData->Items[i].byStackNum;
        m_SO3RoleAllInfo.m_pBuffDBData->Items[i].dwSkillSrcID    = pBuffDBData->Items[i].dwSkillSrcID;
        m_SO3RoleAllInfo.m_pBuffDBData->Items[i].nLeftFrame      = pBuffDBData->Items[i].nLeftFrame;
        m_SO3RoleAllInfo.m_pBuffDBData->Items[i].bySkillSrcLevel = pBuffDBData->Items[i].bySkillSrcLevel;
        m_SO3RoleAllInfo.m_pBuffDBData->Items[i].RecipeKey       = pBuffDBData->Items[i].RecipeKey;
//        m_SO3RoleAllInfo.m_pBuffDBData->Items[i].nCustomValue    = 0;//++
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_SO3RoleAllInfo.m_pBuffDBData;
        m_SO3RoleAllInfo.m_pBuffDBData = NULL;
    }
    return nResult;
}

BOOL KSO3RoleDBUpdater::SaveBuffList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                bResult     = false;
    size_t              uLeftSize   = uBufferSize;
    BYTE*               pbyOffset   = pbyBuffer;
    KBUFF_DB_DATA_OLD*  pBuffDBData = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KBUFF_DB_DATA_OLD));
    pBuffDBData = (KBUFF_DB_DATA_OLD*)pbyOffset;

    uLeftSize -= sizeof(KBUFF_DB_DATA_OLD);
    pbyOffset += sizeof(KBUFF_DB_DATA_OLD);

    pBuffDBData->byCount = m_SO3RoleAllInfo.m_pBuffDBData->byCount;

    for (int i = 0; i < m_SO3RoleAllInfo.m_pBuffDBData->byCount; i++)
    {
        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KBUFF_DB_DATA_OLD::KBUFF_DB_ITEM));
        uLeftSize -= sizeof(KBUFF_DB_DATA_OLD::KBUFF_DB_ITEM);
        pbyOffset += sizeof(KBUFF_DB_DATA_OLD::KBUFF_DB_ITEM);

        pBuffDBData->Items[i].byStackNum        =   m_SO3RoleAllInfo.m_pBuffDBData->Items[i].byStackNum;
        pBuffDBData->Items[i].dwSkillSrcID      =   m_SO3RoleAllInfo.m_pBuffDBData->Items[i].dwSkillSrcID;
        pBuffDBData->Items[i].nLeftFrame        =   m_SO3RoleAllInfo.m_pBuffDBData->Items[i].nLeftFrame;
        pBuffDBData->Items[i].bySkillSrcLevel   =   m_SO3RoleAllInfo.m_pBuffDBData->Items[i].bySkillSrcLevel;
        pBuffDBData->Items[i].RecipeKey         =   m_SO3RoleAllInfo.m_pBuffDBData->Items[i].RecipeKey;
//        pBuffDBData->Items[i].nCustomValue      =   m_SO3RoleAllInfo.m_pBuffDBData->Items[i].nCustomValue;
    }

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

int KSO3RoleDBUpdater::ConverRepute(BYTE* pbyData, size_t uDataLen)
{
    int              nResult      = false;
//    int              nRetCode     = false;

    size_t           uLeftSize    = uDataLen;
    BYTE*            pbyOffset    = pbyData;

    KFORCE_DB_DATA*  pForceDBData = NULL;
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KFORCE_DB_DATA));
    pForceDBData = (KFORCE_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(KFORCE_DB_DATA);
    pbyOffset += sizeof(KFORCE_DB_DATA);

    m_SO3RoleAllInfo.m_pForceDBData = (KFORCE_DB_DATA_OLD* )new char[sizeof(KFORCE_DB_DATA_OLD) + sizeof(KFORCE_DB_DATA_OLD::FORECE_INFO) * pForceDBData->wCount];
    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_pForceDBData);

    m_SO3RoleAllInfo.m_pForceDBData->wCount    = pForceDBData->wCount;

    for (int i = 0; i < m_SO3RoleAllInfo.m_pForceDBData->wCount; i++)
    {
        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KFORCE_DB_DATA::FORECE_INFO));
        uLeftSize -= sizeof(KFORCE_DB_DATA::FORECE_INFO);
        pbyOffset += sizeof(KFORCE_DB_DATA::FORECE_INFO);

        m_SO3RoleAllInfo.m_pForceDBData->ForceInfo[i].byFightFlag = pForceDBData->ForceInfo[i].byFightFlag;
        m_SO3RoleAllInfo.m_pForceDBData->ForceInfo[i].byForce     = pForceDBData->ForceInfo[i].byForce;
        m_SO3RoleAllInfo.m_pForceDBData->ForceInfo[i].byLevel     = pForceDBData->ForceInfo[i].byLevel;
        m_SO3RoleAllInfo.m_pForceDBData->ForceInfo[i].byShow      = pForceDBData->ForceInfo[i].byShow;
        m_SO3RoleAllInfo.m_pForceDBData->ForceInfo[i].shRepute    = pForceDBData->ForceInfo[i].shRepute;
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_SO3RoleAllInfo.m_pForceDBData;
        m_SO3RoleAllInfo.m_pForceDBData = NULL;
    }
    return nResult;
}

BOOL KSO3RoleDBUpdater::SaveReputeList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                bResult         = false;

    size_t              uLeftSize       = uBufferSize;
    BYTE*               pbyOffset       = pbyBuffer;
    KFORCE_DB_DATA_OLD* pForceDBData    = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KFORCE_DB_DATA_OLD));
    pForceDBData = (KFORCE_DB_DATA_OLD*)pbyOffset;
    uLeftSize -= sizeof(KFORCE_DB_DATA_OLD);
    pbyOffset += sizeof(KFORCE_DB_DATA_OLD);

    pForceDBData->wCount = m_SO3RoleAllInfo.m_pForceDBData->wCount;

    for (int i = 0; i < m_SO3RoleAllInfo.m_pForceDBData->wCount; i++)
    {
        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KFORCE_DB_DATA_OLD::FORECE_INFO));
        uLeftSize -= sizeof(KFORCE_DB_DATA_OLD::FORECE_INFO);
        pbyOffset += sizeof(KFORCE_DB_DATA_OLD::FORECE_INFO);

        pForceDBData->ForceInfo[i].byFightFlag  = m_SO3RoleAllInfo.m_pForceDBData->ForceInfo[i].byFightFlag;
        pForceDBData->ForceInfo[i].byForce      = m_SO3RoleAllInfo.m_pForceDBData->ForceInfo[i].byForce;
        pForceDBData->ForceInfo[i].byLevel      = m_SO3RoleAllInfo.m_pForceDBData->ForceInfo[i].byLevel;
        pForceDBData->ForceInfo[i].byShow       = m_SO3RoleAllInfo.m_pForceDBData->ForceInfo[i].byShow;
        pForceDBData->ForceInfo[i].shRepute     = m_SO3RoleAllInfo.m_pForceDBData->ForceInfo[i].shRepute;
    }

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

int KSO3RoleDBUpdater::ConverUserPreferences(BYTE* pbyData, size_t uDataLen)
{
    int     nResult   = false;
//    int     nRetCode  = false;
    size_t  uLeftSize = uDataLen;
    BYTE*   pbyOffset = pbyData;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));

    m_SO3RoleAllInfo.m_UserPreferences.m_nSize = *(int *)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    if (m_SO3RoleAllInfo.m_UserPreferences.m_nSize > MAX_USER_PREFERENCES_LEN)
    {
        memcpy(m_SO3RoleAllInfo.m_UserPreferences.m_byUserPreferences, pbyOffset, MAX_USER_PREFERENCES_LEN);
    }
    else
    {
        memcpy(m_SO3RoleAllInfo.m_UserPreferences.m_byUserPreferences, pbyOffset, m_SO3RoleAllInfo.m_UserPreferences.m_nSize);
    }
    uLeftSize -= m_SO3RoleAllInfo.m_UserPreferences.m_nSize;
    pbyOffset += m_SO3RoleAllInfo.m_UserPreferences.m_nSize;

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBUpdater::ConverBookState(BYTE* pbyData, size_t uDataLen)
{
    int     nResult   = false;
//    int     nRetCode  = false;

    size_t  uLeftSize = uDataLen;
    BYTE*   pbyOffset = pbyData;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));

    m_SO3RoleAllInfo.m_BookList.m_nSize = *(int*)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    if (m_SO3RoleAllInfo.m_BookList.m_nSize > MAX_READ_BOOK_ID)
    {
        memcpy(m_SO3RoleAllInfo.m_BookList.m_byBookList, pbyOffset, MAX_READ_BOOK_ID);
    }
    else
    {
        memcpy(m_SO3RoleAllInfo.m_BookList.m_byBookList, pbyOffset, m_SO3RoleAllInfo.m_BookList.m_nSize);
    }

    uLeftSize -= m_SO3RoleAllInfo.m_BookList.m_nSize;
    pbyOffset += m_SO3RoleAllInfo.m_BookList.m_nSize;

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBUpdater::ConverCoolDownTimer(BYTE* pbyData, size_t uDataLen)
{
    int                     nResult             = false;
//    int                     nRetCode            = false;

    size_t                  uLeftSize           = uDataLen;
    BYTE*                   pbyOffset           = pbyData;
    COOLDOWN_LIST_DB_DATA*  pCoolDownListDBData = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(COOLDOWN_LIST_DB_DATA));

    pCoolDownListDBData = (COOLDOWN_LIST_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(COOLDOWN_LIST_DB_DATA);
    pbyOffset += sizeof(COOLDOWN_LIST_DB_DATA);

    m_SO3RoleAllInfo.m_pCooldownListDBData = (COOLDOWN_LIST_DB_DATA* )new char[sizeof(COOLDOWN_LIST_DB_DATA) + sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA) * pCoolDownListDBData->wCount];
    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_pCooldownListDBData);

    m_SO3RoleAllInfo.m_pCooldownListDBData->wCount = pCoolDownListDBData->wCount;

    for (int i = 0; i < m_SO3RoleAllInfo.m_pCooldownListDBData->wCount; i++)
    {
        m_SO3RoleAllInfo.m_pCooldownListDBData->CoolDown[i].wTimerID  = pCoolDownListDBData->CoolDown[i].wTimerID;
        m_SO3RoleAllInfo.m_pCooldownListDBData->CoolDown[i].nInterval = pCoolDownListDBData->CoolDown[i].nInterval;
        m_SO3RoleAllInfo.m_pCooldownListDBData->CoolDown[i].nTime     = pCoolDownListDBData->CoolDown[i].nTime;

        uLeftSize -= sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA);
        pbyOffset += sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA);
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_SO3RoleAllInfo.m_pCooldownListDBData;
        m_SO3RoleAllInfo.m_pCooldownListDBData = NULL;
    }
    return nResult;
}

int KSO3RoleDBUpdater::ConverRoadOpenList(BYTE* pbyData, size_t uDataLen)
{
    int                 nResult     = false;
//    int                 nRetCode    = false;

    size_t              uLeftSize   = uDataLen;
    BYTE*               pbyOffset   = pbyData;
    KROLE_ROAD_INFO*    pLoadRoad   = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KROLE_ROAD_INFO));

    pLoadRoad = (KROLE_ROAD_INFO*)pbyOffset;
    pbyOffset += sizeof(KROLE_ROAD_INFO);
    uLeftSize -= sizeof(KROLE_ROAD_INFO);

    m_SO3RoleAllInfo.m_pRoleRoadInfo = (KROLE_ROAD_INFO*)new char[sizeof(KROLE_ROAD_INFO) + sizeof(int) * pLoadRoad->nOpenNodeCount];
    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_pRoleRoadInfo);

    m_SO3RoleAllInfo.m_pRoleRoadInfo->nOpenNodeCount = pLoadRoad->nOpenNodeCount;

    for (int i = 0; i < pLoadRoad->nOpenNodeCount; i++)
    {
        m_SO3RoleAllInfo.m_pRoleRoadInfo->OpenNodeList[i] = pLoadRoad->OpenNodeList[i];
        pbyOffset += sizeof(int);
        uLeftSize -= sizeof(int);
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_SO3RoleAllInfo.m_pRoleRoadInfo;
        m_SO3RoleAllInfo.m_pRoleRoadInfo = NULL;
    }
    return nResult;
}

int KSO3RoleDBUpdater::ConverCustomData(BYTE* pbyData, size_t uDataLen)
{
    int                 nResult     = false;
//    int                 nRetCode    = false;
//    size_t              uLeftSize   = uDataLen;
//    BYTE*               pbyOffset   = pbyData;

    //Todo:啥都不想写，先写个空壳子放这里吧

    nResult = true;
    //Exit0:
    return nResult;
}

int KSO3RoleDBUpdater::ConverVisitedMap(BYTE* pbyData, size_t uDataLen)
{
    int                 nResult     = false;
//    int                 nRetCode    = false;
//    size_t              uLeftSize   = uDataLen;
//    BYTE*               pbyOffset   = pbyData;

    //Todo:啥都不想写，先写个空壳子放这里吧

    nResult = true;
    //Exit0:
    return nResult;
}

int KSO3RoleDBUpdater::ConverPQList(BYTE* pbyData, size_t uDataLen)
{
    int            nResult   = false;
//    int            nRetCode  = false;

    size_t         uLeftSize = uDataLen;
    BYTE*          pbyOffset = pbyData;
    KGPQ_DB_LIST*  pDBList   = NULL;

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KGPQ_DB_LIST));

    pDBList = (KGPQ_DB_LIST*)pbyOffset;
    pbyOffset += sizeof(KGPQ_DB_LIST);
    uLeftSize -= sizeof(KGPQ_DB_LIST);

    m_SO3RoleAllInfo.m_pPQList = (KGPQ_DB_LIST*)new char[sizeof(KGPQ_DB_LIST) + sizeof(DWORD) * pDBList->wCount];
    KGLOG_PROCESS_ERROR(m_SO3RoleAllInfo.m_pPQList);

    m_SO3RoleAllInfo.m_pPQList->wCount = pDBList->wCount;

    for (int i = 0; i < pDBList->wCount; i++)
    {
        m_SO3RoleAllInfo.m_pPQList->dwPQID[i] = pDBList->dwPQID[i];
        pbyOffset += sizeof(DWORD);
        uLeftSize -= sizeof(DWORD);
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_SO3RoleAllInfo.m_pPQList;
        m_SO3RoleAllInfo.m_pPQList = NULL;
    }
    return nResult;
}

int KSO3RoleDBUpdater::UpdateBaseInfo(KRoleBaseInfo* pBaseData)
{
    int         nResult                 = false;
//    int         nRetCode                = false;

    KGLOG_PROCESS_ERROR(pBaseData);
    
    if (m_SO3RoleAllInfo.m_bHadUpdateBaseInfo)
    {
        KBaseInfo_Old* pNew = (KBaseInfo_Old*)pBaseData;

        m_SO3RoleAllInfo.m_BaseInfo.cRoleType      = pNew->cRoleType;
        m_SO3RoleAllInfo.m_BaseInfo.CurrentPos     = pNew->CurrentPos;
        m_SO3RoleAllInfo.m_BaseInfo.LastEntry      = pNew->LastEntry;
        m_SO3RoleAllInfo.m_BaseInfo.byLevel        = pNew->byLevel;
        //m_SO3RoleAllInfo.m_BaseInfo.byCamp         = pNew->byCamp;
        m_SO3RoleAllInfo.m_BaseInfo.byForceID      = pNew->byForceID; 
        memcpy(m_SO3RoleAllInfo.m_BaseInfo.wRepresentId, pNew->wRepresentId, sizeof(m_SO3RoleAllInfo.m_BaseInfo.wRepresentId));
        m_SO3RoleAllInfo.m_BaseInfo.nLastSaveTime  = pNew->nLastSaveTime;
        m_SO3RoleAllInfo.m_BaseInfo.nLastLoginTime = pNew->nLastLoginTime;
        m_SO3RoleAllInfo.m_BaseInfo.nTotalGameTime = pNew->nTotalGameTime;
        m_SO3RoleAllInfo.m_BaseInfo.nBanTime       = pNew->nBanTime;
    }
    else
    {
        m_SO3RoleAllInfo.m_BaseInfo.cRoleType      = pBaseData->cRoleType;
        m_SO3RoleAllInfo.m_BaseInfo.CurrentPos.byFaceDirection  = pBaseData->CurrentPos.byFaceDirection;
        m_SO3RoleAllInfo.m_BaseInfo.CurrentPos.dwMapID          = pBaseData->CurrentPos.dwMapID;
        m_SO3RoleAllInfo.m_BaseInfo.CurrentPos.nMapCopyIndex    = (int64_t)pBaseData->CurrentPos.nMapCopyIndex;
        m_SO3RoleAllInfo.m_BaseInfo.CurrentPos.nX               = pBaseData->CurrentPos.nX;
        m_SO3RoleAllInfo.m_BaseInfo.CurrentPos.nY               = pBaseData->CurrentPos.nY;
        m_SO3RoleAllInfo.m_BaseInfo.CurrentPos.nZ               = pBaseData->CurrentPos.nZ;

        m_SO3RoleAllInfo.m_BaseInfo.LastEntry.byFaceDirection   = pBaseData->LastEntry.byFaceDirection;
        m_SO3RoleAllInfo.m_BaseInfo.LastEntry.dwMapID           = pBaseData->LastEntry.dwMapID;
        m_SO3RoleAllInfo.m_BaseInfo.LastEntry.nMapCopyIndex     = (int64_t)pBaseData->LastEntry.nMapCopyIndex;
        m_SO3RoleAllInfo.m_BaseInfo.LastEntry.nX                = pBaseData->LastEntry.nX;
        m_SO3RoleAllInfo.m_BaseInfo.LastEntry.nY                = pBaseData->LastEntry.nY;
        m_SO3RoleAllInfo.m_BaseInfo.LastEntry.nZ                = pBaseData->LastEntry.nZ;
        m_SO3RoleAllInfo.m_BaseInfo.byLevel        = pBaseData->byLevel;
        //m_SO3RoleAllInfo.m_BaseInfo.byCamp         = cNeutral;//++
        m_SO3RoleAllInfo.m_BaseInfo.byForceID      = pBaseData->byForceID; 
        memcpy(m_SO3RoleAllInfo.m_BaseInfo.wRepresentId, pBaseData->wRepresentId, sizeof(m_SO3RoleAllInfo.m_BaseInfo.wRepresentId));
        m_SO3RoleAllInfo.m_BaseInfo.nLastSaveTime  = pBaseData->nLastSaveTime;
        m_SO3RoleAllInfo.m_BaseInfo.nLastLoginTime = pBaseData->nLastLoginTime;
        m_SO3RoleAllInfo.m_BaseInfo.nTotalGameTime = pBaseData->nTotalGameTime;
        //m_SO3RoleAllInfo.m_BaseInfo.nBanTime       = pBaseData->nBanTime;
    }

    nResult = true;
Exit0:
    return nResult;

}

int KSO3RoleDBUpdater::ConverMailBoxInfo(unsigned long ulMailBoxTableID, BYTE* pbyData, unsigned long ulDataLen)
{
    BOOL               bResult   = false;
    BYTE*              pbyOffset = pbyData;
    unsigned long      uLeftLen  = ulDataLen;
    MAILBOX_DB_EX*     pHead     = NULL;
    MAIL_DATA_OLD*     pSrcMail  = NULL;
    MAIL_DATA*         pDstMail  = NULL;
    size_t             uMailLen  = 0;
    size_t             uSaveMailLen = 0;
    MAILBOX_DB_EX*     pSaveHead = NULL;
    BYTE*              pbySaveOffset = NULL;
    unsigned long      uSaveLeftLen  = 0;

    ASSERT(pbyData);
    ASSERT(ulDataLen > 0);

    KGLOG_PROCESS_ERROR(uLeftLen >= sizeof(MAILBOX_DB_EX));
    pHead = (MAILBOX_DB_EX*)pbyOffset;

    KGLOG_PROCESS_ERROR(m_piMailBoxInfoPackage);
    pSaveHead = (MAILBOX_DB_EX*)m_piMailBoxInfoPackage->GetData();
    memset(pSaveHead, 0, m_piMailBoxInfoPackage->GetSize());
    pbySaveOffset   = (BYTE*)pSaveHead;
    m_uMailInfoSize = 0;
    //KGLOG_PROCESS_ERROR(pHead->wVersion == MAILSYS_VERSION);

    pSaveHead->dwNextMailID = pHead->dwNextMailID;
    pSaveHead->wVersion     = pHead->wVersion;

    pbyOffset += sizeof(MAILBOX_DB_EX);
    uLeftLen  -= sizeof(MAILBOX_DB_EX);

    pbySaveOffset += sizeof(MAILBOX_DB_EX);
    uSaveLeftLen += sizeof(MAILBOX_DB_EX);

    while (uLeftLen > 0)
    {
        pSrcMail = (MAIL_DATA_OLD*)pbyOffset;
        pDstMail = (MAIL_DATA*)pbySaveOffset;

        uMailLen = sizeof(MAIL_DATA_OLD) + pSrcMail->Content.wTextSize + pSrcMail->Content.wItemSize;
        uSaveMailLen = sizeof(MAIL_DATA) + pSrcMail->Content.wTextSize + pSrcMail->Content.wItemSize;

        memset(pbySaveOffset, 0, uSaveMailLen);

        pDstMail->byFlags           = pSrcMail->byFlags;
        pDstMail->Content.nMoney    = pSrcMail->Content.nMoney;
        pDstMail->Content.wItemSize = pSrcMail->Content.wItemSize;
        pDstMail->Content.wTextSize = pSrcMail->Content.wTextSize;

        BYTE* pDstbyTextAndItemData = pDstMail->Content.byTextAndItemData;
        BYTE* pSrcbyTextAndItemData = pSrcMail->Content.byTextAndItemData;
        WORD wSizeTextAndItemData   = pSrcMail->Content.wItemSize + pSrcMail->Content.wTextSize;

        //对wTextSize处理,待确认,这里先做转换,免得kg_processerror狂出
        if (pDstMail->Content.wTextSize == 0)
        {
            pDstMail->Content.wTextSize = 1;
            uSaveMailLen += 1;
            pDstbyTextAndItemData[0] = '\0';
            pDstbyTextAndItemData += sizeof(char);
        }

        memcpy(pDstbyTextAndItemData, pSrcbyTextAndItemData, wSizeTextAndItemData);
        pDstMail->dwMailID              = pSrcMail->dwMailID;
        pDstMail->nGetAttachmentLock    = pSrcMail->nGetAttachmentLock;
        pDstMail->nReceiveTime          = pSrcMail->nReceiveTime;
        strncpy(pDstMail->szSenderName, pSrcMail->szSenderName, sizeof(pDstMail->szSenderName));
        pDstMail->szSenderName[sizeof(pDstMail->szSenderName) -1] = '\0';
        strncpy(pDstMail->szTitle, pSrcMail->szTitle, sizeof(pDstMail->szTitle));
        pDstMail->szTitle[sizeof(pDstMail->szTitle) -1] = '\0';

        KGLOG_PROCESS_ERROR(uLeftLen >= uMailLen);

        pDstMail = NULL;

        pbyOffset += (unsigned long)uMailLen;
        uLeftLen  -= (unsigned long)uMailLen;

        pbySaveOffset   += (unsigned long)uSaveMailLen;
        uSaveLeftLen    += (unsigned long)uSaveMailLen;
    }

    m_uMailInfoSize = uSaveLeftLen;

    bResult = true;
Exit0:
    return bResult;
}

int KSO3RoleDBUpdater::ResetRoleAllInfo()
{
    int nResult = false;
    //int nRetCode = false;

    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pBuffDBData);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pCooldownListDBData);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pItemData);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pPQList);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pForceDBData);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pRoleRoadInfo);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pSkillData);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pSkillRecipeList);
    KG_DELETE_ARRAY(m_SO3RoleAllInfo.m_pProfessionData);
    m_SO3RoleAllInfo.m_uPackageUseSize = 0;
    m_SO3RoleAllInfo.m_bHadUpdateBaseInfo = false;
    m_SO3RoleAllInfo.m_bHadUpdateExtInfo = false;
    m_SO3RoleAllInfo.m_bHadExtInfo = false;

    nResult = true;
    return nResult;
}

int KSO3RoleDBUpdater::ResetMailBoxInfo()
{
    int nResult = false;
    //int nRetCode = false;

    m_uMailInfoSize = 0;

    nResult = true;
    return nResult;
}

int KSO3RoleDBUpdater::GetMailBoxInfoSize(unsigned& refuSize)
{
    int nResult = false;
    //int nRetCode = false;

    KGLOG_PROCESS_ERROR(m_piMailBoxInfoPackage);

    refuSize = (unsigned int)m_uMailInfoSize;
    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBUpdater::InsertToNewDBRole(char szTableName[], char szTimeFileName[])
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(szTableName);
    KGLOG_PROCESS_ERROR(szTimeFileName);

    if (m_nUpdateToLocal)
    {
        nRetCode = m_pNewSO3RoleDBTool->UpdateRoleTable(szTableName);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = m_pNewSO3RoleDBTool->InsertRoleDateToNewDB(szTableName, szTimeFileName);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    
    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBUpdater::InsertToNewDBMail(DWORD dwID)
{
    int nResult     = false;
    int nRetCode    = false;

    KGLOG_PROCESS_ERROR(m_pNewSO3RoleDBTool);

    if (m_nUpdateToLocal)
    {
        nRetCode = m_pNewSO3RoleDBTool->UpdateMailBoxTable(dwID);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = m_pNewSO3RoleDBTool->InsertMailBoxDateToNewDB(dwID);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    
    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBUpdater::GetRoleAllInfoToCompare(BYTE* byBaseInfoData, unsigned long ulBaseInfoSize, BYTE* byExtInfoData, unsigned long ulExtInfoSize, char szTableName[], char szTimeFileName[], DWORD dwID)
{
    int nResult     = false;
    int nRetCode    = false;

    nRetCode = m_pNewSO3RoleDBTool->GetRoleAllInfoToCompare(byBaseInfoData, ulBaseInfoSize, byExtInfoData, ulExtInfoSize, szTableName, szTimeFileName, dwID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult; 
}
