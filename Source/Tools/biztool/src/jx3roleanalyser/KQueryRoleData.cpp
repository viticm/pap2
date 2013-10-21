#include "stdafx.h"
#include "KQueryRoleData.h"

KQueryRoleData::KQueryRoleData(void)
{
    //memset(&m_roleDataAll, 0, sizeof(m_roleDataAll));
    memset(&m_roleDataAll.m_BaseData, 0, sizeof(m_roleDataAll.m_BaseData));

    m_roleDataAll.m_pSkillData          = NULL;
    m_roleDataAll.m_pSkillRecipeList    = NULL;
    m_roleDataAll.m_pItemData           = NULL;

    memset(&m_roleDataAll.m_QuestData, 0, sizeof(m_roleDataAll.m_QuestData));

    m_roleDataAll.m_pProfessionData     = NULL;
    memset(m_roleDataAll.m_byRecipeState, 0, sizeof(m_roleDataAll.m_byRecipeState));
    memset(&m_roleDataAll.m_StateInfo, 0, sizeof(m_roleDataAll.m_StateInfo));

    m_roleDataAll.m_pBuffDBData           = NULL;
    m_roleDataAll.m_pForceDBData          = NULL;

    memset(&m_roleDataAll.m_UserPreferences, 0, sizeof(m_roleDataAll.m_UserPreferences));
    memset(&m_roleDataAll.m_BookList, 0, sizeof(m_roleDataAll.m_BookList));

    m_roleDataAll.m_pCooldownListDBData   = NULL;
    m_roleDataAll.m_pRoleRoadInfo         = NULL;
    m_roleDataAll.m_pPQList               = NULL;
    m_roleDataAll.m_bHeroFlag             = false;

    memset(&m_roleDataAll.m_DesignationData, 0, sizeof(m_roleDataAll.m_DesignationData));

    m_roleDataAll.m_QuestRand.m_RandCache.Init();
    m_roleDataAll.m_VenationRand.m_RandCache.Init();

    memset(&m_roleDataAll.m_MontorData, false, sizeof(m_roleDataAll.m_MontorData));
    memset(m_roleDataAll.m_DataBlockExist, false, sizeof(m_roleDataAll.m_DataBlockExist));
}

KQueryRoleData::~KQueryRoleData(void)
{
    if (m_roleDataAll.m_pSkillData)
    {
        delete[] m_roleDataAll.m_pSkillData;
        m_roleDataAll.m_pSkillData = NULL;
    }

    if (m_roleDataAll.m_pSkillRecipeList)
    {
        delete[]  m_roleDataAll.m_pSkillRecipeList;
        m_roleDataAll.m_pSkillRecipeList = NULL;
    }

    if (m_roleDataAll.m_pItemData)
    {
        delete[] m_roleDataAll.m_pItemData;
        m_roleDataAll.m_pItemData = NULL;
    }

    if (m_roleDataAll.m_pProfessionData)
    {
        delete[] m_roleDataAll.m_pProfessionData;
        m_roleDataAll.m_pProfessionData = NULL;
    }

    if (m_roleDataAll.m_pPQList)
    {
        delete[] m_roleDataAll.m_pPQList;
        m_roleDataAll.m_pPQList = NULL;
    }
    //if (m_roleDataAll.m_pRecipeData)
    //{
    //    delete[] m_roleDataAll.m_pRecipeData;
    //    m_roleDataAll.m_pRecipeData = NULL;
    //}

    if (m_roleDataAll.m_pBuffDBData)
    {
        delete[] m_roleDataAll.m_pBuffDBData;
        m_roleDataAll.m_pBuffDBData = NULL;
    }

    if (m_roleDataAll.m_pForceDBData)
    {
        delete[] m_roleDataAll.m_pForceDBData;
        m_roleDataAll.m_pForceDBData = NULL;
    }

    if (m_roleDataAll.m_pCooldownListDBData)
    {
        delete[] m_roleDataAll.m_pCooldownListDBData;
        m_roleDataAll.m_pCooldownListDBData = NULL;
    }

    if (m_roleDataAll.m_pRoleRoadInfo)
    {
        delete[] m_roleDataAll.m_pRoleRoadInfo;
        m_roleDataAll.m_pRoleRoadInfo = NULL;
    }

    m_roleDataAll.m_VenationRand.m_RandCache.UnInit();
    m_roleDataAll.m_QuestRand.m_RandCache.UnInit();
}

int KQueryRoleData::ParseRoleData(unsigned char* pbyData, size_t uDataLen)
{
    int                 nResult         = false;
    int                 nRetCode        = false;
    size_t              uLeftSize       = uDataLen;
    unsigned char*      pbyOffset       = pbyData;
    KRoleDataHeader*    pGlobalHeader   = NULL;

    KG_PROCESS_ERROR(pbyOffset);

    KG_PROCESS_ERROR(uLeftSize >= sizeof(KRoleDataHeader));
    pGlobalHeader = (KRoleDataHeader*)pbyOffset;

    uLeftSize -= sizeof(KRoleDataHeader);
    pbyOffset += sizeof(KRoleDataHeader);

    KG_PROCESS_ERROR(pGlobalHeader->dwVer == 0);
    KG_PROCESS_ERROR(pGlobalHeader->dwLen == uLeftSize);

    //dwCRC = CRC32(0, pbyOffset, (unsigned long)uLeftSize);
    //KG_PROCESS_ERROR(dwCRC == pGlobalHeader->dwCRC);

    while (uLeftSize > 0)
    {
        KRoleBlockHeader* pBlock = NULL;

        KG_PROCESS_ERROR(uLeftSize >= sizeof(KRoleBlockHeader));
        pBlock = (KRoleBlockHeader*)pbyOffset;

        uLeftSize -= sizeof(KRoleBlockHeader);
        pbyOffset += sizeof(KRoleBlockHeader);

        KG_PROCESS_ERROR(uLeftSize >= pBlock->dwLen);

        switch (pBlock->nType)
        { 
        case rbtSkillList:
            m_roleDataAll.m_DataBlockExist[rbtSkillList] = true;
            nRetCode = ParseSkillList(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtSkillRecipeList:
            m_roleDataAll.m_DataBlockExist[rbtSkillRecipeList] = true;
            nRetCode = ParseSkillRecipeList(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtItemList:
            m_roleDataAll.m_DataBlockExist[rbtItemList] = true;
            nRetCode = ParseItemList(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtQuestList:
            m_roleDataAll.m_DataBlockExist[rbtQuestList] = true;
            nRetCode = ParseQuestList(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtProfessionList:
            m_roleDataAll.m_DataBlockExist[rbtProfessionList] = true;
            nRetCode = ParseProfessionList(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtRecipeList:
            m_roleDataAll.m_DataBlockExist[rbtRecipeList] = true;
            if (pBlock->dwVer == RECIPE_LIST_VERSION_V0)
            {
                nRetCode = ParseRecipeListV0(pbyOffset, pBlock->dwLen);
                ASSERT(nRetCode);
            }
            else if (pBlock->dwVer == CURRENT_RECIPE_LIST_VERSION)
            {
                nRetCode = ParseRecipeListV1(pbyOffset, pBlock->dwLen);
                ASSERT(nRetCode);
            }
            else
            {
                ASSERT(false);
            }
            break;

        case rbtStateInfo:
            m_roleDataAll.m_DataBlockExist[rbtStateInfo] = true;
            nRetCode = ParseStateInfo(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtBuffList:
            m_roleDataAll.m_DataBlockExist[rbtBuffList] = true;
            nRetCode = ParseBuffList(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtRepute:
            m_roleDataAll.m_DataBlockExist[rbtRepute] = true;
            nRetCode = ParseRepute(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtUserPreferences:
            m_roleDataAll.m_DataBlockExist[rbtUserPreferences] = true;
            nRetCode = ParseUserPreferences(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtBookState:
            m_roleDataAll.m_DataBlockExist[rbtBookState] = true;
            nRetCode = ParseBookState(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtCoolDownTimer:
            m_roleDataAll.m_DataBlockExist[rbtCoolDownTimer] = true;
            nRetCode = ParseCoolDownTimer(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtRoadOpenList:
            m_roleDataAll.m_DataBlockExist[rbtRoadOpenList] = true;
            nRetCode = ParseRoadOpenList(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtCustomData:
            m_roleDataAll.m_DataBlockExist[rbtCustomData] = true;
            nRetCode = ParseCustomData(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtVisitedMap:
            m_roleDataAll.m_DataBlockExist[rbtVisitedMap] = true;
            nRetCode = ParseVisitedMap(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtPQList:
            m_roleDataAll.m_DataBlockExist[rbtPQList] = true;
            nRetCode = ParsePQList(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;

        case rbtHeroData:
            m_roleDataAll.m_DataBlockExist[rbtHeroData] = true;
            nRetCode = ParseHeroData(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;
        case rbtAchievementData:
            m_roleDataAll.m_DataBlockExist[rbtAchievementData] = true;
            nRetCode = ParseAchievementData(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;
        case rbtDesignationData:
            m_roleDataAll.m_DataBlockExist[rbtDesignationData] = true;
            nRetCode = ParseDesignationData(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;
        case rbtRandData:
            m_roleDataAll.m_DataBlockExist[rbtRandData] = true;
            nRetCode = ParseRandData(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;
        case rbtAntiFarmerData:
            m_roleDataAll.m_DataBlockExist[rbtAntiFarmerData] = true;
            //nRetCode = ParseAntiFarmer(pbyOffset, pBlock->dwLen);
            //ASSERT(nRetCode);
            break;
        case rbtMentorData:
            m_roleDataAll.m_DataBlockExist[rbtMentorData] = true;
            nRetCode = PaserMentorData(pbyOffset, pBlock->dwLen);
            ASSERT(nRetCode);
            break;
        default:
            ASSERT(true); //todo:这里的true是临时的！
        }

        uLeftSize -= pBlock->dwLen;
        pbyOffset += pBlock->dwLen;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KQueryRoleData::ParseRoleBaseData(unsigned char* pbyData, size_t uDataLen)
{
    int             nResult   = false;
    KRoleBaseInfo*  pBaseData = (KRoleBaseInfo*)pbyData;

    KG_PROCESS_ERROR(pBaseData);
    KG_PROCESS_ERROR(uDataLen == sizeof(KRoleBaseInfo));

    m_roleDataAll.m_BaseData.nVersion       = pBaseData->nVersion;
    m_roleDataAll.m_BaseData.byLevel        = pBaseData->byLevel;
    m_roleDataAll.m_BaseData.byForceID      = pBaseData->byForceID;
    m_roleDataAll.m_BaseData.cRoleType      = pBaseData->cRoleType;
    m_roleDataAll.m_BaseData.CurrentPos     = pBaseData->CurrentPos;
    m_roleDataAll.m_BaseData.LastEntry      = pBaseData->LastEntry;
    m_roleDataAll.m_BaseData.nLastLoginTime = pBaseData->nLastLoginTime;
    m_roleDataAll.m_BaseData.nLastSaveTime  = pBaseData->nLastSaveTime;
    m_roleDataAll.m_BaseData.nTotalGameTime = pBaseData->nTotalGameTime;
    m_roleDataAll.m_BaseData.byCamp         = pBaseData->byCamp;
    m_roleDataAll.m_BaseData.byForceID      = pBaseData->byForceID;
    m_roleDataAll.m_BaseData.nCreateTime    = pBaseData->nCreateTime;
    
    memcpy(m_roleDataAll.m_BaseData.wRepresentId, pBaseData->wRepresentId, sizeof(m_roleDataAll.m_BaseData.wRepresentId));
    memcpy(m_roleDataAll.m_BaseData.byReserved, pBaseData->byReserved, sizeof(m_roleDataAll.m_BaseData.byReserved));//new+

    nResult = true;
Exit0:
    return nResult;
}

int KQueryRoleData::ParseSkillList( unsigned char* pbyData, size_t uDataLen )
{
    int              nResult     = false;
//    int              nRetCode    = false;
    size_t           uLeftSize   = uDataLen;
    unsigned char*   pbyOffset   = pbyData;
    KDB_SKILL_DATA*  pSkillData  = NULL;
    
    KG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_SKILL_DATA));

    pSkillData = (KDB_SKILL_DATA*)pbyOffset;
    uLeftSize -= sizeof(KDB_SKILL_DATA);
    pbyOffset += sizeof(KDB_SKILL_DATA);

    m_roleDataAll.m_pSkillData = (KDB_SKILL_DATA*)new char[sizeof(KDB_SKILL_DATA) + sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM) * pSkillData->nSkillCount];
    KG_PROCESS_ERROR(m_roleDataAll.m_pSkillData);

    m_roleDataAll.m_pSkillData->dwMountKungfuID    = pSkillData->dwMountKungfuID;
    m_roleDataAll.m_pSkillData->dwMountKungfuLevel = pSkillData->dwMountKungfuLevel;
    m_roleDataAll.m_pSkillData->nSkillCount        = pSkillData->nSkillCount;

    KG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM) * pSkillData->nSkillCount);

    for (int i = 0; i < pSkillData->nSkillCount; i++)
    {
        KDB_SKILL_DATA::KDB_SKILL_ITEM*  pSkillItem        = &pSkillData->SkillItems[i];
        KG_PROCESS_ERROR(pSkillItem);

        m_roleDataAll.m_pSkillData->SkillItems[i].wSkillID   = pSkillItem->wSkillID;
        m_roleDataAll.m_pSkillData->SkillItems[i].byMaxLevel = pSkillItem->byMaxLevel;
        m_roleDataAll.m_pSkillData->SkillItems[i].dwExp      = pSkillItem->dwExp;

        uLeftSize -= sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM);
        pbyOffset += sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM);
    }

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_roleDataAll.m_pSkillData;
        m_roleDataAll.m_pSkillData = NULL;
    }
    return nResult;
}

int KQueryRoleData::ParseSkillRecipeList( unsigned char* pbyData, size_t uDataLen )
{
    int                               nResult          = false;
//    int                               nRetCode         = false;
    size_t                            uLeftSize        = uDataLen;
    unsigned char*                    pbyOffset        = pbyData;
    KG_SKILL_RECIPE_LIST_DB_DATA_V2*  pSkillRecipeData = NULL;

    KG_PROCESS_ERROR(uDataLen >= sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2));

    pSkillRecipeData = (KG_SKILL_RECIPE_LIST_DB_DATA_V2*)pbyOffset;
    uLeftSize       -= sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2);
    pbyOffset       += sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2);

    KG_PROCESS_ERROR(pSkillRecipeData->wVersion >= KG_SKILL_RECIPE_DB_LOAD_LOWEST_VER);
    KG_PROCESS_ERROR(pSkillRecipeData->wVersion <= KG_SKILL_RECIPE_DB_CURRENT_VER);

    m_roleDataAll.m_pSkillRecipeList = (KG_SKILL_RECIPE_LIST_DB_DATA_V2*)new char[sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2) + sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2::_RecipeNode)* pSkillRecipeData->wCount];
    KG_PROCESS_ERROR(m_roleDataAll.m_pSkillRecipeList);

    m_roleDataAll.m_pSkillRecipeList->wCount = pSkillRecipeData->wCount;

    for (int i = 0; i < pSkillRecipeData->wCount; i++)
    {
        m_roleDataAll.m_pSkillRecipeList->RecipeArray[i].byActive     = pSkillRecipeData->RecipeArray[i].byActive;
        m_roleDataAll.m_pSkillRecipeList->RecipeArray[i].dwRecipeKey  = pSkillRecipeData->RecipeArray[i].dwRecipeKey;

        uLeftSize -= sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2::_RecipeNode);
        pbyOffset += sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2::_RecipeNode);
    }

    KG_PROCESS_ERROR(uLeftSize == 0);

//Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_roleDataAll.m_pSkillRecipeList;
        m_roleDataAll.m_pSkillRecipeList = NULL;
    }
    return nResult;
}

int KQueryRoleData::ParseItemList(unsigned char* pbyData, size_t uDataLen)
{
    int        nResult   = false;
    int        nRetCode  = false;

    size_t          uLeftSize = uDataLen;
    unsigned char*  pbyOffset = pbyData;
    ItemData*       pItemData = NULL;

    KG_PROCESS_ERROR(uDataLen >= sizeof(ItemData));

    pItemData = (ItemData*)pbyOffset;
    uLeftSize -= sizeof(ItemData);
    pbyOffset += sizeof(ItemData);

    nRetCode = sizeof(ItemData) + sizeof(ItemData::KITEM_DB_HEADEREX) * pItemData->m_wItemCount;
    m_roleDataAll.m_pItemData = (ItemData* )new char[nRetCode];
    KG_PROCESS_ERROR(m_roleDataAll.m_pItemData);

    m_roleDataAll.m_pItemData->m_dwMoney                  = pItemData->m_dwMoney;
    m_roleDataAll.m_pItemData->m_wEnabledBankPackageCount = pItemData->m_wEnabledBankPackageCount;
    m_roleDataAll.m_pItemData->m_wItemCount               = pItemData->m_wItemCount;

    for (int i = 0; i < pItemData->m_wItemCount; i++)
    {
        ItemData::KITEM_DB_HEADEREX* pItemDBDeaderex = (ItemData::KITEM_DB_HEADEREX*)pbyOffset;

        m_roleDataAll.m_pItemData->ItemArray[i].byBox         = pItemDBDeaderex->byBox;
        m_roleDataAll.m_pItemData->ItemArray[i].byPos         = pItemDBDeaderex->byPos;
        m_roleDataAll.m_pItemData->ItemArray[i].byDataLen     = pItemDBDeaderex->byDataLen;

        if (pItemDBDeaderex->byDataLen == sizeof(KCOMMON_ITEM_DATA))
        {
            m_roleDataAll.m_pItemData->ItemArray[i].commonEquipData = pItemDBDeaderex->commonEquipData;
        }
        else if(pItemDBDeaderex->byDataLen == sizeof(KCUSTOM_EQUI_DATA))
        {
            m_roleDataAll.m_pItemData->ItemArray[i].customEquipData = pItemDBDeaderex->customEquipData;
        }
        //else if(pItemDBDeaderex->byDataLen == sizeof(KTRAIN_EQUIP_DATA))
        //{
        //    // 逻辑暂时未处理
        //    //m_roleDataAll.m_pItemData->ItemArray[i].trainEquipData = pItemDBDeaderex->trainEquipData;
        //}
        else
        {
            KG_PROCESS_ERROR(false);
        }


        int nTempLength = pItemDBDeaderex->byDataLen + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned char);

        uLeftSize -= nTempLength;
        pbyOffset += nTempLength;
    }

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_roleDataAll.m_pItemData;
        m_roleDataAll.m_pItemData = NULL;
    }
    return nResult;
}

int KQueryRoleData::ParseQuestList(unsigned char* pbyData, size_t uDataLen)
{
    int         nResult          = false;
//    int        nRetCode   = false;

    size_t          uLeftSize        = uDataLen;
    unsigned char*  pbyOffset        = pbyData;
    int             nDwordBitCount   = sizeof(unsigned long) * CHAR_BIT;

    //KG_PROCESS_ERROR(uDataLen >= sizeof(unsigned long) * MAX_QUEST_COUNT / 32);

    for (int nQuestIndex = 0; nQuestIndex < MAX_QUEST_COUNT / nDwordBitCount; nQuestIndex++)
    {
        unsigned long dwQuestState = 0;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(unsigned long));
        dwQuestState = *(unsigned long*)pbyOffset;
        uLeftSize -= sizeof(unsigned long);
        pbyOffset += sizeof(unsigned long);

        for (int nIndex = 0; nIndex < nDwordBitCount; nIndex++)
        {
            if (dwQuestState & (0x01 << nIndex))
            {
                m_roleDataAll.m_QuestData.m_dwQuestState[nQuestIndex * nDwordBitCount + nIndex] = qsFinished;
            }
        }
    }

    m_roleDataAll.m_QuestData.m_AcceptQuestCount = *(unsigned char*)pbyOffset;
    uLeftSize -= sizeof(unsigned char);
    pbyOffset += sizeof(unsigned char);

    KG_PROCESS_ERROR(m_roleDataAll.m_QuestData.m_AcceptQuestCount <= MAX_ACCEPT_QUEST_COUNT);

    for (int i = 0; i < m_roleDataAll.m_QuestData.m_AcceptQuestCount; i++)
    {
        KDB_ACCEPT_QUEST*  pDBQuest= NULL;

        pDBQuest   = (KDB_ACCEPT_QUEST*)pbyOffset;
        uLeftSize -= sizeof(KDB_ACCEPT_QUEST);
        pbyOffset += sizeof(KDB_ACCEPT_QUEST);

        m_roleDataAll.m_QuestData.m_AcceptQuest[i].wQuestID     = pDBQuest->wQuestID;
        m_roleDataAll.m_QuestData.m_AcceptQuest[i].byFailed     = pDBQuest->byFailed;
        m_roleDataAll.m_QuestData.m_AcceptQuest[i].nLimitTime   = pDBQuest->nLimitTime;

        for (int j = 0; j < QUEST_PARAM_COUNT; j++)
        {
            m_roleDataAll.m_QuestData.m_AcceptQuest[i].byKillNpcCount[j] = pDBQuest->byKillNpcCount[j];
        }

        for (int j = 0; j < QUEST_PARAM_COUNT * 2; j++)
        {
            m_roleDataAll.m_QuestData.m_AcceptQuest[i].nQuestValue[j]    = pDBQuest->nQuestValue[j];
        } 
    }

    m_roleDataAll.m_QuestData.m_AssistQuestCount = *(unsigned char*)pbyOffset;
    uLeftSize -= sizeof(unsigned char);
    pbyOffset += sizeof(unsigned char);

    m_roleDataAll.m_QuestData.m_AssistDailyCount = *(unsigned char*)pbyOffset;
    uLeftSize -= sizeof(unsigned char);
    pbyOffset += sizeof(unsigned char);

    KG_PROCESS_ERROR( m_roleDataAll.m_QuestData.m_AssistQuestCount <= MAX_ASSIST_QUEST_COUNT);

    for (int i = 0; i < m_roleDataAll.m_QuestData.m_AssistQuestCount; i++)
    {
        KDB_ASSIST_QUEST* pDBQuest = NULL;

        KG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_ASSIST_QUEST));

        pDBQuest = (KDB_ASSIST_QUEST*)pbyOffset;
        uLeftSize -= sizeof(KDB_ASSIST_QUEST);
        pbyOffset += sizeof(KDB_ASSIST_QUEST);

        m_roleDataAll.m_QuestData.m_AssistQuest[i].dwPlayerID = pDBQuest->dwPlayerID;
        m_roleDataAll.m_QuestData.m_AssistQuest[i].wQuestID   = pDBQuest->wQuestID;
    }

    m_roleDataAll.m_QuestData.m_DailyQuestCount = *(unsigned char*)pbyOffset;
    uLeftSize -= sizeof(unsigned char);
    pbyOffset += sizeof(unsigned char);

    KG_PROCESS_ERROR( m_roleDataAll.m_QuestData.m_DailyQuestCount <= MAX_ACCEPT_QUEST_COUNT);

    for (int i = 0; i < m_roleDataAll.m_QuestData.m_DailyQuestCount; i++)
    {
        KDB_DAILY_QUEST* pDailyQuest = NULL;

        KG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_DAILY_QUEST));

        pDailyQuest = (KDB_DAILY_QUEST*)pbyOffset;
        uLeftSize -= sizeof(KDB_DAILY_QUEST);
        pbyOffset += sizeof(KDB_DAILY_QUEST);

        m_roleDataAll.m_QuestData.m_DailyQuest[i].wQuestID        = pDailyQuest->wQuestID;
        m_roleDataAll.m_QuestData.m_DailyQuest[i].nNextAcceptTime = pDailyQuest->nNextAcceptTime;
    }

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    return nResult;
}

int KQueryRoleData::ParseProfessionList( unsigned char* pbyData, size_t uDataLen)
{
    int        nResult          = false;
//    int        nRetCode         = false;

    size_t          uLeftSize        = uDataLen;
    unsigned char*  pbyOffset        = pbyData;
    unsigned char   byCount          = 0;

    KG_PROCESS_ERROR(uDataLen >= sizeof(KProfessionDBData));

    byCount    = *(unsigned char*)pbyOffset;
    uLeftSize -= sizeof(KProfessionDBData);
    pbyOffset += sizeof(KProfessionDBData);

    m_roleDataAll.m_pProfessionData = (KProfessionDBData* )new char[sizeof(KProfessionDBData) + sizeof(KProfessionItem) * byCount];
    KG_PROCESS_ERROR(m_roleDataAll.m_pProfessionData);

    m_roleDataAll.m_pProfessionData->byCount = byCount;

    for (int i = 0; i < byCount; i++)
    {
        KProfessionItem* pProfessionDBData = (KProfessionItem*)pbyOffset;

        m_roleDataAll.m_pProfessionData->Profession[i].m_byBranchID     = pProfessionDBData->m_byBranchID;
        m_roleDataAll.m_pProfessionData->Profession[i].m_byCurrentLevel = pProfessionDBData->m_byCurrentLevel;
        m_roleDataAll.m_pProfessionData->Profession[i].m_byMaxLevel     = pProfessionDBData->m_byMaxLevel;
        m_roleDataAll.m_pProfessionData->Profession[i].m_byProfessionID = pProfessionDBData->m_byProfessionID;
        m_roleDataAll.m_pProfessionData->Profession[i].m_wProficiency   = pProfessionDBData->m_wProficiency;

        uLeftSize -= sizeof(KProfessionItem);
        pbyOffset += sizeof(KProfessionItem);

    }

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_roleDataAll.m_pProfessionData;
        m_roleDataAll.m_pProfessionData = NULL;
    }
    return nResult;
}

int KQueryRoleData::ParseRecipeListV0(unsigned char* pbyData, size_t uDataLen)
{
    int             nResult          = false;
    //    int             nRetCode         = false;

    size_t              uLeftSize        = uDataLen;
    unsigned char*      pbyOffset        = pbyData;
    int                 nMaxRecipeIndex  = (OLD_MAX_RECIPE_ID + CHAR_BIT - 1) / CHAR_BIT;
    KRecipeDBItem_V0*   pRecipeDBItem    = NULL;
    KRecipeDBData_V0*   pRecipeDBData    = (KRecipeDBData_V0*)pbyData;
    int                 bRecipeLearned   = false;

    KG_PROCESS_ERROR(uDataLen == sizeof(KRecipeDBData_V0) + pRecipeDBData->byCount * sizeof(KRecipeDBItem_V0));

    KG_PROCESS_ERROR(pRecipeDBData->byCount <= MAX_CRAFT_COUNT);

    for (int i = 0; i < pRecipeDBData->byCount; i++)
    {
        pRecipeDBItem = &(pRecipeDBData->RecipeDBItem[i]);

        KG_PROCESS_ERROR(pRecipeDBItem->byCraftID >= 1);
        KG_PROCESS_ERROR(pRecipeDBItem->byCraftID <= MAX_CRAFT_COUNT);

        for (int j = 0; j < nMaxRecipeIndex; j++)
        {
            unsigned char byBitData = pRecipeDBItem->byRecipeData[j];

            for (int nSubIndex = 0; nSubIndex < CHAR_BIT; ++nSubIndex)
            {
                int nRecipeIndex = j * CHAR_BIT + nSubIndex;
                if (nRecipeIndex < OLD_MAX_RECIPE_ID)
                {
                    bRecipeLearned = (byBitData & (1 << (CHAR_BIT - nSubIndex - 1))) ? true : false;
                    m_roleDataAll.m_byRecipeState[pRecipeDBItem->byCraftID - 1][nRecipeIndex] = (unsigned char)bRecipeLearned;
                }
            }
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KQueryRoleData::ParseRecipeListV1(unsigned char* pbyData, size_t uDataLen)
{
    int             nResult          = false;
    //    int             nRetCode         = false;

    size_t          uLeftSize        = uDataLen;
    unsigned char*  pbyOffset        = pbyData;
    int             nMaxRecipeIndex  = (MAX_RECIPE_ID + CHAR_BIT - 1) / CHAR_BIT;
    KRecipeDBItem*  pRecipeDBItem    = NULL;
    int             bRecipeLearned   = false;
    KRecipeDBData*  pRecipeDBData    = (KRecipeDBData*)pbyData;

    KG_PROCESS_ERROR(uDataLen == sizeof(KRecipeDBData) + pRecipeDBData->byCount * sizeof(KRecipeDBItem));

    KG_PROCESS_ERROR(pRecipeDBData->byCount <= MAX_CRAFT_COUNT);

    for (int i = 0; i < pRecipeDBData->byCount; i++)
    {
        pRecipeDBItem = &(pRecipeDBData->RecipeDBItem[i]);

        KGLOG_PROCESS_ERROR(pRecipeDBItem->byCraftID >= 1);
        KGLOG_PROCESS_ERROR(pRecipeDBItem->byCraftID <= MAX_CRAFT_COUNT);

        for (int j = 0; j < nMaxRecipeIndex; j++)
        {
            unsigned char byBitData = pRecipeDBItem->byRecipeData[j];

            for (int nSubIndex = 0; nSubIndex < CHAR_BIT; ++nSubIndex)
            {
                int nRecipeIndex = j * CHAR_BIT + nSubIndex;
                if (nRecipeIndex < MAX_RECIPE_ID)
                {
                    bRecipeLearned = (byBitData & (1 << (CHAR_BIT - nSubIndex - 1))) ? true : false;
                    m_roleDataAll.m_byRecipeState[pRecipeDBItem->byCraftID - 1][nRecipeIndex] = (unsigned char)bRecipeLearned;
                }
            }
        }

    }

    //KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    return nResult;
}

int KQueryRoleData::ParseStateInfo(unsigned char* pbyData, size_t uDataLen)
{
    int               nResult        = false;
//    int               nRetCode       = false;

    size_t            uLeftSize      = uDataLen;
    unsigned char*    pbyOffset      = pbyData;
    KROLE_STATE_INFO* pRoleStateInfo = NULL;

    KG_PROCESS_ERROR(uLeftSize >= sizeof(KROLE_STATE_INFO));
    pRoleStateInfo = (KROLE_STATE_INFO*)pbyOffset;
    uLeftSize -= sizeof(KROLE_STATE_INFO);
    pbyOffset += sizeof(KROLE_STATE_INFO);

    //m_roleDataAll.m_StateInfo.byMoveState              = (CHARACTER_MOVE_STATE)pRoleStateInfo->byMoveState;
    m_roleDataAll.m_StateInfo.byMoveState              = pRoleStateInfo->byMoveState;
    m_roleDataAll.m_StateInfo.wCurrentTrack            = pRoleStateInfo->wCurrentTrack;
    m_roleDataAll.m_StateInfo.nMoveFrameCounter        = pRoleStateInfo->nMoveFrameCounter;
    m_roleDataAll.m_StateInfo.wFromFlyNode             = pRoleStateInfo->wFromFlyNode;
    m_roleDataAll.m_StateInfo.wTargetCityID            = pRoleStateInfo->wTargetCityID;
    m_roleDataAll.m_StateInfo.nExperience              = pRoleStateInfo->nExperience;

    m_roleDataAll.m_StateInfo.nCurrentLife             = pRoleStateInfo->nCurrentLife;
    m_roleDataAll.m_StateInfo.nCurrentMana             = pRoleStateInfo->nCurrentMana;
    m_roleDataAll.m_StateInfo.nCurrentStamina          = pRoleStateInfo->nCurrentStamina;
    m_roleDataAll.m_StateInfo.nCurrentThew             = pRoleStateInfo->nCurrentThew;

    m_roleDataAll.m_StateInfo.nCurrentTrainValue       = pRoleStateInfo->nCurrentTrainValue;
    m_roleDataAll.m_StateInfo.nUsedTrainValue          = pRoleStateInfo->nUsedTrainValue;
    m_roleDataAll.m_StateInfo.nAddTrainTimeInToday     = pRoleStateInfo->nAddTrainTimeInToday;

    m_roleDataAll.m_StateInfo.byPKState                = pRoleStateInfo->byPKState;
    m_roleDataAll.m_StateInfo.wCloseSlayLeftTime       = pRoleStateInfo->wCloseSlayLeftTime;

    m_roleDataAll.m_StateInfo.wLeftReviveFrame         = pRoleStateInfo->wLeftReviveFrame;
    m_roleDataAll.m_StateInfo.nLastSituReviveTime	   = pRoleStateInfo->nLastSituReviveTime;
    m_roleDataAll.m_StateInfo.bySituReviveCount	       = pRoleStateInfo->bySituReviveCount;
    m_roleDataAll.m_StateInfo.dwKillerID			   = pRoleStateInfo->dwKillerID;  

    m_roleDataAll.m_StateInfo.wCurrentKillPoint        = pRoleStateInfo->wCurrentKillPoint;
    m_roleDataAll.m_StateInfo.nReserved0               = pRoleStateInfo->nReserved0;
    m_roleDataAll.m_StateInfo.nCurrentPrestige         = pRoleStateInfo->nCurrentPrestige;
    m_roleDataAll.m_StateInfo.wReserved                = pRoleStateInfo->wReserved;
    m_roleDataAll.m_StateInfo.nBanTime                 = pRoleStateInfo->nBanTime;
    m_roleDataAll.m_StateInfo.nContribution            = pRoleStateInfo->nContribution;
    m_roleDataAll.m_StateInfo.nMaxLevel                = pRoleStateInfo->nMaxLevel;
    m_roleDataAll.m_StateInfo.bHideHat                 = pRoleStateInfo->bHideHat;
    //m_roleDataAll.m_StateInfo.byMaxApprenticeNum       = pRoleStateInfo->byMaxApprenticeNum;

    memcpy(m_roleDataAll.m_StateInfo.byReserved, pRoleStateInfo->byReserved, sizeof(m_roleDataAll.m_StateInfo.byReserved));
    //memcpy(m_roleDataAll.m_StateInfo.nReserved, pRoleStateInfo->nReserved, sizeof(m_roleDataAll.m_StateInfo.nReserved));//new+

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    return nResult;
}

int KQueryRoleData::ParseBuffList( unsigned char* pbyData, size_t uDataLen )
{
    int             nResult      = false;
//    int             nRetCode     = false;
    size_t          uLeftSize    = uDataLen;
    unsigned char*  pbyOffset    = pbyData;
    KBUFF_DB_DATA*  pBuffDBData  = NULL;

    KG_PROCESS_ERROR(uLeftSize >= sizeof(KBUFF_DB_DATA));
    pBuffDBData = (KBUFF_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(KBUFF_DB_DATA);
    pbyOffset += sizeof(KBUFF_DB_DATA);

    m_roleDataAll.m_pBuffDBData = (KBUFF_DB_DATA* )new char[sizeof(KBUFF_DB_DATA) + sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM) * pBuffDBData->byCount];
    KG_PROCESS_ERROR(m_roleDataAll.m_pBuffDBData);

    m_roleDataAll.m_pBuffDBData->byCount = pBuffDBData->byCount;

    for (int i = 0; i < m_roleDataAll.m_pBuffDBData->byCount; i++)
    {
        KG_PROCESS_ERROR(uLeftSize >= sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM));
        uLeftSize -= sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM);
        pbyOffset += sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM);

        m_roleDataAll.m_pBuffDBData->Items[i].byStackNum      = pBuffDBData->Items[i].byStackNum;
        m_roleDataAll.m_pBuffDBData->Items[i].dwSkillSrcID    = pBuffDBData->Items[i].dwSkillSrcID;
        m_roleDataAll.m_pBuffDBData->Items[i].nLeftFrame      = pBuffDBData->Items[i].nLeftFrame;
        m_roleDataAll.m_pBuffDBData->Items[i].bySkillSrcLevel = pBuffDBData->Items[i].bySkillSrcLevel;
        m_roleDataAll.m_pBuffDBData->Items[i].RecipeKey       = pBuffDBData->Items[i].RecipeKey;
        m_roleDataAll.m_pBuffDBData->Items[i].nCustomValue    = pBuffDBData->Items[i].nCustomValue;//new+
    }

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_roleDataAll.m_pBuffDBData;
        m_roleDataAll.m_pBuffDBData = NULL;
    }
    return nResult;
}

int KQueryRoleData::ParseRepute( unsigned char* pbyData, size_t uDataLen )
{
    int              nResult      = false;
//    int              nRetCode     = false;
    size_t           uLeftSize    = uDataLen;
    unsigned char*   pbyOffset    = pbyData;
    KFORCE_DB_DATA*  pForceDBData = NULL;

    KG_PROCESS_ERROR(uLeftSize >= sizeof(KFORCE_DB_DATA));
    pForceDBData = (KFORCE_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(KFORCE_DB_DATA);
    pbyOffset += sizeof(KFORCE_DB_DATA);

    m_roleDataAll.m_pForceDBData = (KFORCE_DB_DATA* )new char[sizeof(KFORCE_DB_DATA) + sizeof(KFORCE_DB_DATA::FORECE_INFO) * pForceDBData->wCount];
    KG_PROCESS_ERROR(m_roleDataAll.m_pForceDBData);

    m_roleDataAll.m_pForceDBData->wCount    = pForceDBData->wCount;

    for (int i = 0; i < m_roleDataAll.m_pForceDBData->wCount; i++)
    {
        KG_PROCESS_ERROR(uLeftSize >= sizeof(KFORCE_DB_DATA::FORECE_INFO));
        uLeftSize -= sizeof(KFORCE_DB_DATA::FORECE_INFO);
        pbyOffset += sizeof(KFORCE_DB_DATA::FORECE_INFO);

        m_roleDataAll.m_pForceDBData->ForceInfo[i].byFightFlag = pForceDBData->ForceInfo[i].byFightFlag;
        m_roleDataAll.m_pForceDBData->ForceInfo[i].byForce     = pForceDBData->ForceInfo[i].byForce;
        m_roleDataAll.m_pForceDBData->ForceInfo[i].byLevel     = pForceDBData->ForceInfo[i].byLevel;
        m_roleDataAll.m_pForceDBData->ForceInfo[i].byShow      = pForceDBData->ForceInfo[i].byShow;
        m_roleDataAll.m_pForceDBData->ForceInfo[i].shRepute    = pForceDBData->ForceInfo[i].shRepute;
    }

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_roleDataAll.m_pForceDBData;
        m_roleDataAll.m_pForceDBData = NULL;
    }
    return nResult;
}

int KQueryRoleData::ParseUserPreferences( unsigned char* pbyData, size_t uDataLen )
{
    int     nResult   = false;
//    int     nRetCode  = false;

    size_t           uLeftSize = uDataLen;
    unsigned char*   pbyOffset = pbyData;

    KG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    m_roleDataAll.m_UserPreferences.m_nSize = *(int *)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    if (m_roleDataAll.m_UserPreferences.m_nSize > MAX_USER_PREFERENCES_LEN)
    {
        memcpy(m_roleDataAll.m_UserPreferences.m_byUserPreferences, pbyOffset, MAX_USER_PREFERENCES_LEN);
    }
    else
    {
        memcpy(m_roleDataAll.m_UserPreferences.m_byUserPreferences, pbyOffset, m_roleDataAll.m_UserPreferences.m_nSize);
    }
    uLeftSize -= m_roleDataAll.m_UserPreferences.m_nSize;
    pbyOffset += m_roleDataAll.m_UserPreferences.m_nSize;

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    return nResult;
}

int KQueryRoleData::ParseBookState( unsigned char* pbyData, size_t uDataLen )
{
    int     nResult   = false;
//    int     nRetCode  = false;

    size_t           uLeftSize = uDataLen;
    unsigned char*   pbyOffset = pbyData;

    KG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    m_roleDataAll.m_BookList.m_nSize = *(int*)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    if (m_roleDataAll.m_BookList.m_nSize > MAX_READ_BOOK_ID)
        memcpy(m_roleDataAll.m_BookList.m_byBookList, pbyOffset, MAX_READ_BOOK_ID);
    else
        memcpy(m_roleDataAll.m_BookList.m_byBookList, pbyOffset, m_roleDataAll.m_BookList.m_nSize);

    uLeftSize -= m_roleDataAll.m_BookList.m_nSize;
    pbyOffset += m_roleDataAll.m_BookList.m_nSize;

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    return nResult;
}

int KQueryRoleData::ParseCoolDownTimer( unsigned char* pbyData, size_t uDataLen )
{
    int                     nResult             = false;
//    int                     nRetCode            = false;

    size_t                  uLeftSize           = uDataLen;
    unsigned char*          pbyOffset           = pbyData;
    COOLDOWN_LIST_DB_DATA*  pCoolDownListDBData = NULL;

    KG_PROCESS_ERROR(uLeftSize >= sizeof(COOLDOWN_LIST_DB_DATA));
    pCoolDownListDBData = (COOLDOWN_LIST_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(COOLDOWN_LIST_DB_DATA);
    pbyOffset += sizeof(COOLDOWN_LIST_DB_DATA);

    m_roleDataAll.m_pCooldownListDBData = (COOLDOWN_LIST_DB_DATA* )new char[sizeof(COOLDOWN_LIST_DB_DATA) + sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA) * pCoolDownListDBData->wCount];
    KG_PROCESS_ERROR(m_roleDataAll.m_pCooldownListDBData);

    m_roleDataAll.m_pCooldownListDBData->wCount = pCoolDownListDBData->wCount;

    for (int i = 0; i < m_roleDataAll.m_pCooldownListDBData->wCount; i++)
    {
        m_roleDataAll.m_pCooldownListDBData->CoolDown[i].wTimerID  = pCoolDownListDBData->CoolDown[i].wTimerID;
        m_roleDataAll.m_pCooldownListDBData->CoolDown[i].nInterval = pCoolDownListDBData->CoolDown[i].nInterval;
        m_roleDataAll.m_pCooldownListDBData->CoolDown[i].nTime     = pCoolDownListDBData->CoolDown[i].nTime;

        uLeftSize -= sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA);
        pbyOffset += sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA);
    }

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_roleDataAll.m_pCooldownListDBData;
        m_roleDataAll.m_pCooldownListDBData = NULL;
    }
    return nResult;
}

int KQueryRoleData::ParseRoadOpenList( unsigned char* pbyData, size_t uDataLen )
{
    int                 nResult     = false;
//    int                 nRetCode    = false;

    size_t              uLeftSize   = uDataLen;
    unsigned char*      pbyOffset   = pbyData;
    KROLE_ROAD_INFO*    pLoadRoad   = NULL;

    KG_PROCESS_ERROR(uLeftSize >= sizeof(KROLE_ROAD_INFO));
    pLoadRoad = (KROLE_ROAD_INFO*)pbyOffset;
    pbyOffset += sizeof(KROLE_ROAD_INFO);
    uLeftSize -= sizeof(KROLE_ROAD_INFO);

    m_roleDataAll.m_pRoleRoadInfo = (KROLE_ROAD_INFO*)new char[sizeof(KROLE_ROAD_INFO) + sizeof(int) * pLoadRoad->nOpenNodeCount];
    KG_PROCESS_ERROR(m_roleDataAll.m_pRoleRoadInfo);

    m_roleDataAll.m_pRoleRoadInfo->nOpenNodeCount = pLoadRoad->nOpenNodeCount;

    for (int i = 0; i < pLoadRoad->nOpenNodeCount; i++)
    {
        m_roleDataAll.m_pRoleRoadInfo->OpenNodeList[i] = pLoadRoad->OpenNodeList[i];
        pbyOffset += sizeof(int);
        uLeftSize -= sizeof(int);
    }

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_roleDataAll.m_pRoleRoadInfo;
        m_roleDataAll.m_pRoleRoadInfo = NULL;
    }
    return nResult;
}

int KQueryRoleData::ParseCustomData(unsigned char* pbyData, size_t uDataLen)
{
    int                 nResult     = false;
//    int                 nRetCode    = false;

    size_t              uLeftSize   = uDataLen;
    unsigned char*      pbyOffset   = pbyData;

    //Todo:啥都不想写，先写个空壳子放这里吧

    nResult = true;
//Exit0:
    return nResult;
}

int KQueryRoleData::ParseVisitedMap(unsigned char* pbyData, size_t uDataLen)
{
    int                 nResult     = false;
//    int                 nRetCode    = false;

    size_t              uLeftSize   = uDataLen;
    unsigned char*      pbyOffset   = pbyData;

    //Todo:啥都不想写，先写个空壳子放这里吧

    nResult = true;
//Exit0:
    return nResult;
}

int KQueryRoleData::ParsePQList( unsigned char* pbyData, size_t uDataLen )
{
    int            nResult   = false;
//    int            nRetCode  = false;
    size_t         uLeftSize = uDataLen;
    unsigned char*          pbyOffset = pbyData;
    KGPQ_DB_LIST*  pDBList   = NULL;

    KG_PROCESS_ERROR(uDataLen >= sizeof(KGPQ_DB_LIST));
    pDBList = (KGPQ_DB_LIST*)pbyOffset;
    pbyOffset += sizeof(KGPQ_DB_LIST);
    uLeftSize -= sizeof(KGPQ_DB_LIST);

    m_roleDataAll.m_pPQList = (KGPQ_DB_LIST*)new char[sizeof(KGPQ_DB_LIST) + sizeof(unsigned long) * pDBList->wCount];
    KG_PROCESS_ERROR(m_roleDataAll.m_pPQList);

    m_roleDataAll.m_pPQList->wCount = pDBList->wCount;

    for (int i = 0; i < pDBList->wCount; i++)
    {
        m_roleDataAll.m_pPQList->dwPQID[i] = pDBList->dwPQID[i];
        pbyOffset += sizeof(unsigned long);
        uLeftSize -= sizeof(unsigned long);
    }

    KG_PROCESS_ERROR(uLeftSize == 0);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete[] m_roleDataAll.m_pPQList;
        m_roleDataAll.m_pPQList = NULL;
    }
    return nResult;
}

int KQueryRoleData::ParseHeroData(unsigned char* pbyData, size_t uDataLen)
{
    int            nResult   = false;
    int            nRetCode  = false;
    KHERO_DATA*    pHeroData = NULL;

    KG_PROCESS_ERROR(uDataLen >= sizeof(KHERO_DATA));
    pHeroData = (KHERO_DATA*)pbyData;

    KG_PROCESS_ERROR(pHeroData->byDataLen <= MAX_MAP_ID_DATA_SIZE);

    m_roleDataAll.m_bHeroFlag = pHeroData->byHeroFlag;

    nRetCode = m_roleDataAll.m_HeroMapCopyOpenFlag.Load(pHeroData->byData, pHeroData->byDataLen);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KQueryRoleData::ParseAchievementData(unsigned char* pbyData, size_t uDataLen)
{
    int                 nResult                 = false;
    int                 bRetCode                = false;
    size_t              uLeftSize               = uDataLen;
    unsigned char*      pbyOffset               = pbyData;
    size_t              uCountSize              = 0;

    KG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    m_roleDataAll.m_AchievementData.m_nPoint = *(int*)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    m_roleDataAll.m_AchievementData.m_nRecord = *(int*)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KG_PROCESS_ERROR(uLeftSize >= sizeof(size_t));
    uCountSize = *(size_t*)pbyOffset;
    uLeftSize -= sizeof(size_t);
    pbyOffset += sizeof(size_t);

    for (size_t i = 0; i < uCountSize; ++i)
    {
        int nID     = 0;
        int nCount  = 0;

        KG_PROCESS_ERROR(uLeftSize >= sizeof(int) * 2);

        nID = *(int*)pbyOffset;
        uLeftSize -= sizeof(int);
        pbyOffset += sizeof(int);

        nCount = *(int*)pbyOffset;
        uLeftSize -= sizeof(int);
        pbyOffset += sizeof(int);

        m_roleDataAll.m_AchievementData.m_CounterTable[nID] = nCount;
    }

    bRetCode = m_roleDataAll.m_AchievementData.m_BitSet.Load(pbyOffset, uLeftSize);
    KG_PROCESS_ERROR(bRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KQueryRoleData::ParseDesignationData(unsigned char* pbyData, size_t uDataLen)
{
    int            bResult             = false;
    int            bRetCode            = false;
    unsigned char* pbyOffset           = pbyData;
    size_t         uLeftSize           = uDataLen;

    KG_PROCESS_ERROR(uLeftSize >= sizeof(unsigned char));
    m_roleDataAll.m_DesignationData.m_nCurrentPrefix = (int)*(unsigned char*)pbyOffset;
    pbyOffset += sizeof(unsigned char);
    uLeftSize -= sizeof(unsigned char);

    KG_PROCESS_ERROR(uLeftSize >= sizeof(unsigned char));
    m_roleDataAll.m_DesignationData.m_nCurrentPostfix = (int)*(unsigned char*)pbyOffset;
    pbyOffset += sizeof(unsigned char);
    uLeftSize -= sizeof(unsigned char);

    KG_PROCESS_ERROR(uLeftSize >= sizeof(unsigned char));
    m_roleDataAll.m_DesignationData.m_nGenerationIndex = (int)*(unsigned char*)pbyOffset;
    pbyOffset += sizeof(unsigned char);
    uLeftSize -= sizeof(unsigned char);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    m_roleDataAll.m_DesignationData.m_nNameInForceIndex = *(int*)pbyOffset;
    pbyOffset += sizeof(int);
    uLeftSize -= sizeof(int);

    KG_PROCESS_ERROR(uLeftSize >= sizeof(bool));
    m_roleDataAll.m_DesignationData.m_bDisplayFlag = *(bool*)pbyOffset;
    pbyOffset += sizeof(bool);
    uLeftSize -= sizeof(bool);

    KG_PROCESS_ERROR(uLeftSize >= DESIGNATIONFIX_DATA_SIZE);
    bRetCode = m_roleDataAll.m_DesignationData.m_AcquiredPrefix.Load(pbyOffset, DESIGNATIONFIX_DATA_SIZE);
    KG_PROCESS_ERROR(bRetCode);
    pbyOffset += DESIGNATIONFIX_DATA_SIZE;
    uLeftSize -= DESIGNATIONFIX_DATA_SIZE;

    KG_PROCESS_ERROR(uLeftSize == DESIGNATIONFIX_DATA_SIZE);
    bRetCode = m_roleDataAll.m_DesignationData.m_AcquiredPostfix.Load(pbyOffset, DESIGNATIONFIX_DATA_SIZE);
    KG_PROCESS_ERROR(bRetCode);
    pbyOffset += DESIGNATIONFIX_DATA_SIZE;
    uLeftSize -= DESIGNATIONFIX_DATA_SIZE;

    bResult = true;
Exit0:
    return bResult;
}

int KQueryRoleData::ParseRandData(unsigned char* pbyData, size_t uDataLen)
{
    int                bResult   = false;
    unsigned char*     pbyOffset = pbyData;
    unsigned char*     pTail     = pbyData + uDataLen;
    PROBABILITY_TYPE*  peType    = NULL;

    while (pbyOffset < pTail)
    {
        peType = (PROBABILITY_TYPE*)pbyOffset;
        pbyOffset += sizeof(PROBABILITY_TYPE);

        switch (*peType)
        {
        case eptQuestDrop:
            pbyOffset = m_roleDataAll.m_QuestRand.Load(pbyOffset, pTail - pbyOffset);
            KGLOG_PROCESS_ERROR(pbyOffset);
            break;
        case eptVenation:
            pbyOffset = m_roleDataAll.m_VenationRand.Load(pbyOffset, pTail - pbyOffset);
            KGLOG_PROCESS_ERROR(pbyOffset);
            break;
        default:
            assert(false);
        }
    }
    KGLOG_PROCESS_ERROR(pbyOffset == pTail);

    bResult = true;
Exit0:
    return bResult;
}

//int KQueryRoleData::ParseAntiFarmer(unsigned char* pbyData, size_t uDataLen)
//{
//    int                             nResult             = false;
//    unsigned char*                  pbyOffset           = pbyData;
//    size_t                          uLeftSize           = uDataLen;
//    WORD*                           pwOnlineRecordCount = NULL;
//    WORD*                           pwLastNpcIndex      = 0;
//    int                             nOnlineRecordCount  = 0;
//    KCHECK_ONLINE_TIME_INTERVAL*    pOnlineRecordData   = NULL;
//
//    assert(pbyOffset);
//
//    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(WORD));
//    pwOnlineRecordCount = (WORD*)pbyOffset;
//    pbyOffset += sizeof(WORD);
//    uLeftSize -= sizeof(WORD);
//
//    nOnlineRecordCount = *pwOnlineRecordCount;
//    KGLOG_PROCESS_ERROR(nOnlineRecordCount >= 0);
//
//    while (nOnlineRecordCount--)
//    {
//        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KCHECK_ONLINE_TIME_INTERVAL));
//
//        pOnlineRecordData = (KCHECK_ONLINE_TIME_INTERVAL*)pbyOffset;
//        pbyOffset += sizeof(KCHECK_ONLINE_TIME_INTERVAL);
//        uLeftSize -= sizeof(KCHECK_ONLINE_TIME_INTERVAL);
//
//        KGLOG_PROCESS_ERROR(pOnlineRecordData->nStartTime <= pOnlineRecordData->nEndTime);
//        m_roleDataAll.m_AntiFarmer.m_OnlineTimeList.push_back(*pOnlineRecordData);
//    }
//
//    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(m_roleDataAll.m_AntiFarmer.m_dwKilledNpcTemplateID));
//    memcpy(m_roleDataAll.m_AntiFarmer.m_dwKilledNpcTemplateID, pbyOffset, sizeof(m_roleDataAll.m_AntiFarmer.m_dwKilledNpcTemplateID));
//    pbyOffset += sizeof(m_roleDataAll.m_AntiFarmer.m_dwKilledNpcTemplateID);
//    uLeftSize -= sizeof(m_roleDataAll.m_AntiFarmer.m_dwKilledNpcTemplateID);
//
//    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(WORD));
//    pwLastNpcIndex = (WORD*)pbyOffset;
//    pbyOffset += sizeof(WORD);
//    uLeftSize -= sizeof(WORD);
//
//    m_roleDataAll.m_AntiFarmer.m_nLastNpcIndex = *pwLastNpcIndex;
//
//    nResult = true;
//Exit0:
//    return nResult;
//}

int KQueryRoleData::PaserMentorData(unsigned char* pbyData, size_t uDataLen)
{
    BOOL                    bResult     = false;
    KMENTOR_DATA_ON_PLAYER* pMentorData = NULL; 

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KMENTOR_DATA_ON_PLAYER));
    pMentorData = (KMENTOR_DATA_ON_PLAYER*)pbyData;

    m_roleDataAll.m_MontorData.byMaxApprenticeCount  = pMentorData->byMaxApprenticeCount;
    m_roleDataAll.m_MontorData.nAcquiredMentorValue  = pMentorData->nAcquiredMentorValue;
    m_roleDataAll.m_MontorData.nLastEvokeMentorTime  = pMentorData->nLastEvokeMentorTime;
    m_roleDataAll.m_MontorData.byEvokeMentorCount    = pMentorData->byEvokeMentorCount;

    bResult = true;
Exit0:
    return bResult;
}
