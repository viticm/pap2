////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_RoleInfo.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-26  11:08:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "KG_RoleInfo.h"

///////////////////////////////////////////////////////////////////////////////
const unsigned KG_ROLE_BASE_INFO_HEADER_VERSION         = 0;
const unsigned KG_ROLE_EXTEND_INFO_HEADER_VERSION       = 0;

///////////////////////////////////////////////////////////////////////////////
KG_RoleInfo::KG_RoleInfo()
{
}

KG_RoleInfo::~KG_RoleInfo()
{
}

int KG_RoleInfo::Init(
    const unsigned char cbyBaseInfo[],   unsigned uBaseInfoSize,
    const unsigned char cbyExtendInfo[], unsigned uExtendInfoSize, void *pvContext
)
{
	int nResult  = false;
	int nRetCode = false;

    KGLOG_PROCESS_ERROR(cbyBaseInfo);
    KGLOG_PROCESS_ERROR(uBaseInfoSize);
    // KGLOG_PROCESS_ERROR(cbyExtendInfo);      // Extend information could be empty.
    // KGLOG_PROCESS_ERROR(uExtendInfoSize);

    KG_USE_ARGUMENT(pvContext);

    nRetCode = _ParseBaseInfo(cbyBaseInfo, uBaseInfoSize);
    KGLOG_PROCESS_ERROR(nRetCode && "_ParseBaseInfo()");

    nRetCode = _ParseExtendInfo(cbyExtendInfo, uExtendInfoSize);
    KGLOG_PROCESS_ERROR(nRetCode && "_ParseExtendInfo()");

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::UnInit(void *pvContext)
{
    KG_USE_ARGUMENT(pvContext);

    return true;
}

int KG_RoleInfo::GetLevel(int *pnRetLevel) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(pnRetLevel);

    *pnRetLevel = m_BaseInfo.byLevel;

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::GetForceID(int *pnRetForceID) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(pnRetForceID);

    *pnRetForceID = m_BaseInfo.byForceID;

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::GetCampID(int *pnRetCampID) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(pnRetCampID);

    *pnRetCampID = m_BaseInfo.byCamp;

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::IsHaveSkill(int nSkillID) const
{
	int nResult  = false;
    _KG_SKILL_INFO_MAP::const_iterator cit;

    cit = m_SkillInfoMap.find(nSkillID);
    KG_PROCESS_ERROR(cit != m_SkillInfoMap.end());

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::GetSkillLevel(int nSkillID, int *pnRetSkillLevel) const
{
    int nResult  = false;
    _KG_SKILL_INFO_MAP::const_iterator cit;

    KGLOG_PROCESS_ERROR(pnRetSkillLevel);

    cit = m_SkillInfoMap.find(nSkillID);
    KGLOG_PROCESS_ERROR(cit != m_SkillInfoMap.end());

    *pnRetSkillLevel = cit->second.nLevel;

    nResult = true;
Exit0:
    return nResult;
}

int KG_RoleInfo::GetTotalMoney(int *pnRetTotalMoney) const
{
    int nResult  = false;

    KGLOG_PROCESS_ERROR(pnRetTotalMoney);

    *pnRetTotalMoney = m_nTotalMoney;

    nResult = true;
Exit0:
    return nResult;
}

int KG_RoleInfo::GetItemInfo(int nBoxID, int nPosID, int *pnRetEmptyFlag, int *pnRetTypeID, int *pnRetItemID) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(nBoxID >= 0);
    KGLOG_PROCESS_ERROR(nBoxID < _KG_ITEM_BOX_COUNT);
    KGLOG_PROCESS_ERROR(nPosID >= 0);
    KGLOG_PROCESS_ERROR(nPosID < _KG_ITEM_BOX_SIZE);
    KGLOG_PROCESS_ERROR(pnRetEmptyFlag);
    KGLOG_PROCESS_ERROR(pnRetTypeID);
    KGLOG_PROCESS_ERROR(pnRetItemID);

    switch (m_ItemInfoArray[nBoxID][nPosID].eItemType)
    {
    case ITEM_EMPTY:
        *pnRetEmptyFlag = true;
        break;
    case ITEM_COMMON_ITEM:
        *pnRetEmptyFlag = false;
        *pnRetTypeID = m_ItemInfoArray[nBoxID][nPosID].Data.CommonItem.byTabType;
        *pnRetItemID = m_ItemInfoArray[nBoxID][nPosID].Data.CommonItem.wTabIndex;
        break;
    case ITEM_CUSTOM_EQUI:
        *pnRetEmptyFlag = false;
        *pnRetTypeID = m_ItemInfoArray[nBoxID][nPosID].Data.CustomEqui.byTabType;
        *pnRetItemID = m_ItemInfoArray[nBoxID][nPosID].Data.CustomEqui.wTabIndex;
        break;
    default:
        KGLogPrintf(KGLOG_DEBUG, "Unknown item type: %d", m_ItemInfoArray[nBoxID][nPosID].eItemType);
        KGLOG_PROCESS_ERROR(false);
        break;        
    }

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::GetUsedTrain(int *pnRetUsedTrain) const
{
    int nResult  = false;

    KGLOG_PROCESS_ERROR(pnRetUsedTrain);

    *pnRetUsedTrain = m_StateInfo.nUsedTrainValue;

    nResult = true;
Exit0:
    return nResult;
}

int KG_RoleInfo::GetForceReputeLevel(int nForceID, int *pnRetReputeLevel) const
{
    int nResult  = false;

    KGLOG_PROCESS_ERROR(nForceID >= 0);
    KGLOG_PROCESS_ERROR(nForceID < sizeof(m_ForceInfoArray) / sizeof(m_ForceInfoArray[0]));
    KGLOG_PROCESS_ERROR(pnRetReputeLevel);

    *pnRetReputeLevel = m_ForceInfoArray[nForceID].byLevel;

    nResult = true;
Exit0:
    return nResult;
}

int KG_RoleInfo::GetForceExtRepute(int nForceID, int *pnRetReputation) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(nForceID >= 0);
    KGLOG_PROCESS_ERROR(nForceID < sizeof(m_ForceInfoArray) / sizeof(m_ForceInfoArray[0]));
    KGLOG_PROCESS_ERROR(pnRetReputation);

    *pnRetReputation = m_ForceInfoArray[nForceID].shRepute;

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::GetAchievementRecord(int *pnRetAchievementRecord) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(pnRetAchievementRecord);

    *pnRetAchievementRecord = m_nAchievementRecord;

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::_ParseBaseInfo(const unsigned char cbyBaseInfo[], unsigned uBaseInfoSize)
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(cbyBaseInfo);
    KGLOG_PROCESS_ERROR(uBaseInfoSize);

    KGLOG_PROCESS_ERROR(uBaseInfoSize == sizeof(KRoleBaseInfo));
    m_BaseInfo = *(KRoleBaseInfo *)cbyBaseInfo;
    KGLOG_PROCESS_ERROR(m_BaseInfo.nVersion == KG_ROLE_BASE_INFO_HEADER_VERSION);

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::_ParseExtendInfo(const unsigned char cbyExtendInfo[], unsigned uExtendInfoSize)
{
	int nResult  = false;
	int nRetCode = false;
    const unsigned char *pcbyReadPos = NULL;
    unsigned uLeftSize = 0;
    unsigned uCRC = 0;
    const KRoleDataHeader *pcRoleDataHeader = NULL;
    const KRoleBlockHeader *pcRoleBlockHeader = NULL;

    // KGLOG_PROCESS_ERROR(cbyExtendInfo);      // Extend information could be empty.
    // KGLOG_PROCESS_ERROR(uExtendInfoSize);

    // Set initial value for extend info.
    m_SkillInfoMap.clear();
    m_nTotalMoney = 0;
    memset(&m_ItemInfoArray, 0, sizeof(m_ItemInfoArray));
    memset(&m_StateInfo, 0, sizeof(m_StateInfo));
    memset(&m_ForceInfoArray, 0, sizeof(m_ForceInfoArray));
    m_nAchievementRecord = 0;

    KG_PROCESS_SUCCESS(cbyExtendInfo == NULL);  // No extend info.

    pcbyReadPos = cbyExtendInfo;
    uLeftSize   = uExtendInfoSize;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KRoleDataHeader));
    pcRoleDataHeader = (const KRoleDataHeader *)pcbyReadPos;

    pcbyReadPos += sizeof(KRoleDataHeader);
    uLeftSize   -= sizeof(KRoleDataHeader);

    KGLOG_PROCESS_ERROR(pcRoleDataHeader->dwVer == KG_ROLE_EXTEND_INFO_HEADER_VERSION);
    KGLOG_PROCESS_ERROR(pcRoleDataHeader->dwLen == uLeftSize);
    uCRC = CRC32(0, pcbyReadPos, uLeftSize);
    KGLOG_PROCESS_ERROR(pcRoleDataHeader->dwCRC == uCRC);

    while (true)
    {
        if (uLeftSize == 0)
            break;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KRoleBlockHeader));
        pcRoleBlockHeader = (const KRoleBlockHeader *)pcbyReadPos;

        pcbyReadPos += sizeof(KRoleBlockHeader);
        uLeftSize   -= sizeof(KRoleBlockHeader);

        KGLOG_PROCESS_ERROR(pcRoleBlockHeader->dwLen <= uLeftSize);
        switch (pcRoleBlockHeader->nType)
        {
        case rbtSkillList:
            KGLOG_PROCESS_ERROR(pcRoleBlockHeader->dwVer == 0);
            nRetCode = _ParseSkillInfo(pcbyReadPos, pcRoleBlockHeader->dwLen);
            KGLOG_PROCESS_ERROR(nRetCode && "_ParseSkillInfo()");
            break;
        case rbtItemList:
            KGLOG_PROCESS_ERROR(pcRoleBlockHeader->dwVer == 0);
            nRetCode = _ParseItemInfo(pcbyReadPos, pcRoleBlockHeader->dwLen);
            KGLOG_PROCESS_ERROR(nRetCode && "_ParseItemInfo()");
            break;
        case rbtStateInfo:
            KGLOG_PROCESS_ERROR(pcRoleBlockHeader->dwVer == 0);
            nRetCode = _ParseStateInfo(pcbyReadPos, pcRoleBlockHeader->dwLen);
            KGLOG_PROCESS_ERROR(nRetCode && "_ParseStateInfo()");
            break;
        case rbtRepute:
            KGLOG_PROCESS_ERROR(pcRoleBlockHeader->dwVer == 0);
            nRetCode = _ParseForceInfo(pcbyReadPos, pcRoleBlockHeader->dwLen);
            KGLOG_PROCESS_ERROR(nRetCode && "_ParseForceInfo()");
            break;
        case rbtAchievementData:
            KGLOG_PROCESS_ERROR(pcRoleBlockHeader->dwVer == 0);
            nRetCode = _ParseAchievementInfo(pcbyReadPos, pcRoleBlockHeader->dwLen);
            KGLOG_PROCESS_ERROR(nRetCode && "_ParseAchievementInfo()");
            break;
        default:    // Ignore the block not need parsed.
            break;
        }

        pcbyReadPos += pcRoleBlockHeader->dwLen;
        uLeftSize   -= pcRoleBlockHeader->dwLen;        
    }

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::_ParseSkillInfo(const unsigned char cbyInfo[], unsigned uInfoSize)
{
	int nResult  = false;
    const KDB_SKILL_DATA *pcSkillData = NULL;
    unsigned uSkillDataSize = 0;
    int nSkillID = 0;
    _KG_SKILL_INFO SkillInfo;
    pair<_KG_SKILL_INFO_MAP::iterator, bool> InsertResult;

    ASSERT(cbyInfo);
    ASSERT(uInfoSize);

    KGLOG_PROCESS_ERROR(uInfoSize >= sizeof(KDB_SKILL_DATA));
    pcSkillData = (const KDB_SKILL_DATA *)cbyInfo;

    uSkillDataSize = sizeof(KDB_SKILL_DATA) + sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM) * pcSkillData->nSkillCount;
    KGLOG_PROCESS_ERROR(uSkillDataSize == uInfoSize);

    m_SkillInfoMap.clear();
    for (int i = 0; i < pcSkillData->nSkillCount; i++)
    {
        nSkillID = pcSkillData->SkillItems[i].wSkillID;

        SkillInfo.nLevel = pcSkillData->SkillItems[i].byMaxLevel;
        SkillInfo.nExp   = pcSkillData->SkillItems[i].dwExp;

        InsertResult = m_SkillInfoMap.insert(make_pair(nSkillID, SkillInfo));
        KGLOG_PROCESS_ERROR(InsertResult.second);     
    }

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::_ParseItemInfo(const unsigned char cbyInfo[], unsigned uInfoSize)
{
	int nResult  = false;
    const unsigned char *pcbyReadPos = NULL;
    const KITEM_DB_HEADER *pcItemDBHeader = NULL;
    unsigned uLeftSize = 0;
    int nEnabledBankPackageCount = 0;   // No use.
    int nItemCount = 0;
    int nBoxID = 0;
    int nPosID = 0;

    ASSERT(cbyInfo);
    ASSERT(uInfoSize);

    pcbyReadPos = cbyInfo;
    uLeftSize   = uInfoSize;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(DWORD));
    m_nTotalMoney = *(const DWORD *)pcbyReadPos;
    pcbyReadPos += sizeof(DWORD);
    uLeftSize   -= sizeof(DWORD);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(WORD));
    nEnabledBankPackageCount = *(const WORD *)pcbyReadPos;
    pcbyReadPos += sizeof(WORD);
    uLeftSize   -= sizeof(WORD);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(WORD));
    nItemCount = *(const WORD *)pcbyReadPos;
    pcbyReadPos += sizeof(WORD);
    uLeftSize   -= sizeof(WORD);

    for (int i = 0; i < nItemCount; i++)
    {
        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KITEM_DB_HEADER));
        pcItemDBHeader = (const KITEM_DB_HEADER *)pcbyReadPos;
        pcbyReadPos += sizeof(KITEM_DB_HEADER);
        uLeftSize   -= sizeof(KITEM_DB_HEADER);

        nBoxID = pcItemDBHeader->byBox;
        KGLOG_PROCESS_ERROR(nBoxID >= 0);
        KGLOG_PROCESS_ERROR(nBoxID < _KG_ITEM_BOX_COUNT);

        nPosID = pcItemDBHeader->byPos;
        KGLOG_PROCESS_ERROR(nPosID >= 0);
        KGLOG_PROCESS_ERROR(nPosID < _KG_ITEM_BOX_SIZE);
        
        KGLOG_PROCESS_ERROR(uLeftSize >= pcItemDBHeader->byDataLen);
        KGLOG_PROCESS_ERROR(m_ItemInfoArray[nBoxID][nPosID].eItemType == ITEM_EMPTY);   // No repeat item.
        if (pcItemDBHeader->byDataLen == sizeof(KCOMMON_ITEM_DATA))
        {
            m_ItemInfoArray[nBoxID][nPosID].eItemType = ITEM_COMMON_ITEM;
            m_ItemInfoArray[nBoxID][nPosID].Data.CommonItem = *(KCOMMON_ITEM_DATA *)pcbyReadPos;
            
        }
        else if (pcItemDBHeader->byDataLen == sizeof(KCUSTOM_EQUI_DATA))
        {
            m_ItemInfoArray[nBoxID][nPosID].eItemType = ITEM_CUSTOM_EQUI;
            m_ItemInfoArray[nBoxID][nPosID].Data.CustomEqui = *(KCUSTOM_EQUI_DATA *)pcbyReadPos;
        }
        else
        {
            KGLogPrintf(KGLOG_DEBUG, "Unknown item data length: %d", pcItemDBHeader->byDataLen);
            KGLOG_PROCESS_ERROR(false);
        }

        pcbyReadPos += pcItemDBHeader->byDataLen;
        uLeftSize   -= pcItemDBHeader->byDataLen;
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::_ParseStateInfo(const unsigned char cbyInfo[], unsigned uInfoSize)
{
	int nResult  = false;

    ASSERT(cbyInfo);
    ASSERT(uInfoSize);

    KGLOG_PROCESS_ERROR(uInfoSize == sizeof(KROLE_STATE_INFO));
    m_StateInfo = *(KROLE_STATE_INFO *)cbyInfo;

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::_ParseForceInfo(const unsigned char cbyInfo[], unsigned uInfoSize)
{
	int nResult  = false;
    const KFORCE_DB_DATA *pcForceData = NULL;
    unsigned uForceDataSize = 0;
    int nForceID = 0;

    ASSERT(cbyInfo);
    ASSERT(uInfoSize);

    KGLOG_PROCESS_ERROR(uInfoSize >= sizeof(KFORCE_DB_DATA));
    pcForceData = (const KFORCE_DB_DATA *)cbyInfo;
    
    uForceDataSize = sizeof(KFORCE_DB_DATA) + sizeof(KFORCE_DB_DATA::FORECE_INFO) * pcForceData->wCount;
    KGLOG_PROCESS_ERROR(uForceDataSize == uInfoSize);

    for (int i = 0; i < pcForceData->wCount; i++)
    {
        nForceID = pcForceData->ForceInfo[i].byForce;

        KGLOG_PROCESS_ERROR(nForceID >= 0);
        KGLOG_PROCESS_ERROR(nForceID < sizeof(m_ForceInfoArray) / sizeof(m_ForceInfoArray[0]));
        m_ForceInfoArray[nForceID] = pcForceData->ForceInfo[i];        
    }

	nResult = true;
Exit0:
	return nResult;
}

int KG_RoleInfo::_ParseAchievementInfo(const unsigned char cbyInfo[], unsigned uInfoSize)
{
	int nResult  = false;
    const unsigned char *pcbyReadPos = NULL;
    unsigned uLeftSize = 0;
    int nAchievementPoint = 0;  // Not used.
    
    ASSERT(cbyInfo);
    ASSERT(uInfoSize);

    pcbyReadPos = cbyInfo;
    uLeftSize = uInfoSize;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    nAchievementPoint = *(int *)pcbyReadPos;
    pcbyReadPos += sizeof(int);
    uLeftSize   -= sizeof(int);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    m_nAchievementRecord = *(int *)pcbyReadPos;
    pcbyReadPos += sizeof(int);
    uLeftSize   -= sizeof(int);

    // The data left not used now, so ignore the parsing.
    // ...
        
	nResult = true;
Exit0:
	return nResult;
}
