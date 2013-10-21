#include "stdafx.h"
#include "KCommon.h"
#include "KQueryRoleData.h"
#include "KFellowShipData.h"
#include "KMailBoxInfoData.h"
#include "KAuctionData.h"
#include "KTongData.h"
#include "KStatDataAnalyser.h"
#include "KPQDBTableData.h"
#include "KMentorTableData.h"

PARSEHANDLE ParseRoleBaseData(unsigned char* pbyData, size_t uDataLen)
{
    int nResult  = false;
    int nRetCode = false;

    KQueryRoleData* pQueryRoleData = new KQueryRoleData;
    KG_PROCESS_ERROR(pQueryRoleData);

    nRetCode = pQueryRoleData->ParseRoleBaseData(pbyData, uDataLen);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete pQueryRoleData;
        pQueryRoleData = NULL;
    }
    return (PARSEHANDLE)pQueryRoleData;
}

PARSEHANDLE ParseRoleExtData(unsigned char* pbyData, size_t uDataLen)
{
    int nResult  = false;
    int nRetCode = false;

    KQueryRoleData* pQueryRoleData = new KQueryRoleData;
    KG_PROCESS_ERROR(pQueryRoleData);

    nRetCode = pQueryRoleData->ParseRoleData(pbyData, uDataLen);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete pQueryRoleData;
        pQueryRoleData = NULL;
    }
    return (PARSEHANDLE)pQueryRoleData;
}

void EndRoleParse(PARSEHANDLE& hHandle)
{
    if (hHandle)
    {
        delete (KQueryRoleData *)hHandle;
        hHandle = NULL;
    }
}

int GetRoleType(PARSEHANDLE hParser, char& cRoleType)
{
    int             nResult         = false;
    KQueryRoleData* pQueryRoleData  = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    cRoleType = pQueryRoleData->m_roleDataAll.m_BaseData.cRoleType;

    nResult = true;
Exit0:
    return nResult;
}

int GetRolePosition(PARSEHANDLE hParser, unsigned long& dwMapID, int& nMapCopyIndex, int& nX, int& nY, int& nZ, unsigned char& byFaceDirection)
{
    int             nResult         = false;
    KQueryRoleData* pQueryRoleData  = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    dwMapID         = pQueryRoleData->m_roleDataAll.m_BaseData.CurrentPos.dwMapID;
    nMapCopyIndex   = pQueryRoleData->m_roleDataAll.m_BaseData.CurrentPos.nMapCopyIndex;
    nX              = pQueryRoleData->m_roleDataAll.m_BaseData.CurrentPos.nX;
    nY              = pQueryRoleData->m_roleDataAll.m_BaseData.CurrentPos.nY;
    nZ              = pQueryRoleData->m_roleDataAll.m_BaseData.CurrentPos.nZ;
    byFaceDirection = pQueryRoleData->m_roleDataAll.m_BaseData.CurrentPos.byFaceDirection;

    nResult = true;
Exit0:
    return nResult;
}

int GetLastEntry(PARSEHANDLE hParser, unsigned long& dwMapID, int& nMapCopyIndex, int& nX, int& nY, int& nZ, unsigned char& byFaceDirection)
{
    int             nResult         = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    dwMapID         = pQueryRoleData->m_roleDataAll.m_BaseData.LastEntry.dwMapID;
    nMapCopyIndex   = pQueryRoleData->m_roleDataAll.m_BaseData.LastEntry.nMapCopyIndex;
    nX              = pQueryRoleData->m_roleDataAll.m_BaseData.LastEntry.nX;
    nY              = pQueryRoleData->m_roleDataAll.m_BaseData.LastEntry.nY;
    nZ              = pQueryRoleData->m_roleDataAll.m_BaseData.LastEntry.nZ;
    byFaceDirection = pQueryRoleData->m_roleDataAll.m_BaseData.LastEntry.byFaceDirection;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleLevel(PARSEHANDLE hParser, int& nRoleLevel)
{
    int             nResult         = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nRoleLevel = pQueryRoleData->m_roleDataAll.m_BaseData.byLevel;

    nResult = true;
Exit0:
    return nResult;
}

int GetRepresentID(PARSEHANDLE hParser, unsigned short RepresentIDArray[], int& nSize)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;
    
    KG_PROCESS_ERROR(pQueryRoleData);

    nSize = sizeof(pQueryRoleData->m_roleDataAll.m_BaseData.wRepresentId) / sizeof(unsigned short);
    memcpy(RepresentIDArray, pQueryRoleData->m_roleDataAll.m_BaseData.wRepresentId, nSize);

    nResult = true;
Exit0:
    return nResult;
}

int GetLastSaveTime(PARSEHANDLE hParser, time_t& nLastSaveTime)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nLastSaveTime = pQueryRoleData->m_roleDataAll.m_BaseData.nLastSaveTime;

    nResult = true;
Exit0:
    return nResult;
}

int GetLastLoginTime(PARSEHANDLE hParser, time_t& nLastLoginTime)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nLastLoginTime = pQueryRoleData->m_roleDataAll.m_BaseData.nLastLoginTime;

    nResult = true;
Exit0:
    return nResult;
}

int GetTotalGameTime(PARSEHANDLE hParser, time_t& nTotalGameTime)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nTotalGameTime = pQueryRoleData->m_roleDataAll.m_BaseData.nTotalGameTime;

    nResult = true;
Exit0:
    return nResult;
}

int GetCamp(PARSEHANDLE hParser, int& nCamp)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCamp = (int)pQueryRoleData->m_roleDataAll.m_BaseData.byCamp;

    nResult = true;
Exit0:
    return nResult;
}

int GetForceID(PARSEHANDLE hParser, unsigned long& dwForceID)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    dwForceID = (unsigned long)pQueryRoleData->m_roleDataAll.m_BaseData.byForceID;

    nResult = true;
Exit0:
    return nResult;
}

int GetCreateTime(PARSEHANDLE hParser, time_t& nCreateTime)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCreateTime = (unsigned long)pQueryRoleData->m_roleDataAll.m_BaseData.nCreateTime;

    nResult = true;
Exit0:
    return nResult;
}

int IsDataBlockExist(PARSEHANDLE hParser, int nDataBlockIndex, bool& bIsDataBlockExist)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(nDataBlockIndex >= 0 || nDataBlockIndex < sizeof(pQueryRoleData->m_roleDataAll.m_DataBlockExist));

    bIsDataBlockExist = pQueryRoleData->m_roleDataAll.m_DataBlockExist[nDataBlockIndex];

    nResult = true;
Exit0:
    return nResult;
}

int GetMountKungfuID(PARSEHANDLE hParser, unsigned long& dwMountKungfuID)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    dwMountKungfuID = pQueryRoleData->m_roleDataAll.m_pSkillData->dwMountKungfuID;

    nResult = true;
Exit0:
    return nResult;
}

int GetMountKungfuLevel(PARSEHANDLE hParser, unsigned long& dwMountKungfuLevel)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pSkillData);

    dwMountKungfuLevel = pQueryRoleData->m_roleDataAll.m_pSkillData->dwMountKungfuLevel;

    nResult = true;
Exit0:
    return nResult;
}

int GetSkillCount(PARSEHANDLE hParser, int& nSkillCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pSkillData);

    nSkillCount = pQueryRoleData->m_roleDataAll.m_pSkillData->nSkillCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetSkillItem(PARSEHANDLE hParser, unsigned long dwSkillIndex, unsigned short& wSkillID, unsigned char& byMaxLevel, unsigned long& dwExp)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    //KG_PROCESS_ERROR(dwSkillIndex <= MAX_PLAYER_SKILL_COUNT);
    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(dwSkillIndex >= 0 && dwSkillIndex < (unsigned long)pQueryRoleData->m_roleDataAll.m_pSkillData->nSkillCount);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pSkillData);

    wSkillID   = pQueryRoleData->m_roleDataAll.m_pSkillData->SkillItems[dwSkillIndex].wSkillID;
    byMaxLevel = pQueryRoleData->m_roleDataAll.m_pSkillData->SkillItems[dwSkillIndex].byMaxLevel;
    dwExp      = pQueryRoleData->m_roleDataAll.m_pSkillData->SkillItems[dwSkillIndex].dwExp;

    nResult = true;
Exit0:
    return nResult;
}

int GetSkillRecipeCount(PARSEHANDLE hParser, int& nkillRecipeCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pSkillRecipeList);

    nkillRecipeCount = pQueryRoleData->m_roleDataAll.m_pSkillRecipeList->wCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetSkillRecipe(PARSEHANDLE hParser, unsigned long dwSkillRecipeIndex, unsigned long& dwRecipeKey, unsigned char& byActive)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    //KG_PROCESS_ERROR(dwSkillRecipeIndex <= MAX_PLAYER_SKILL_COUNT);
    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pSkillRecipeList);
    KG_PROCESS_ERROR(dwSkillRecipeIndex >=0 && dwSkillRecipeIndex < pQueryRoleData->m_roleDataAll.m_pSkillRecipeList->wCount);

    dwRecipeKey = pQueryRoleData->m_roleDataAll.m_pSkillRecipeList->RecipeArray[dwSkillRecipeIndex].dwRecipeKey;
    byActive    = pQueryRoleData->m_roleDataAll.m_pSkillRecipeList->RecipeArray[dwSkillRecipeIndex].byActive;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleMoney(PARSEHANDLE hParser, unsigned long& dwMoney)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pItemData);

    dwMoney = pQueryRoleData->m_roleDataAll.m_pItemData->m_dwMoney;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleEnabledBankPackageCount(PARSEHANDLE hParser, unsigned short& wEnabledBankPackageCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pItemData);

    wEnabledBankPackageCount = pQueryRoleData->m_roleDataAll.m_pItemData->m_wEnabledBankPackageCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleItemCount(PARSEHANDLE hParser, unsigned short& wItemCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pItemData);

    wItemCount = pQueryRoleData->m_roleDataAll.m_pItemData->m_wItemCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleItemInfo(
                    PARSEHANDLE hParser, unsigned long dwItemIndex, unsigned char& byBox, 
                    unsigned char& byPos, unsigned char& byVersion, unsigned char& byTabType, 
                    unsigned short& wTabIndex, unsigned char& byBind, unsigned short& wDurability, time_t& nGenTime,
                    int& nIsEqui, unsigned long& dwRandSeed, unsigned short& wLeftEnchantTime, unsigned char& byColorID,
                    unsigned short& wPermanentEnchant, unsigned short& wTemporaryEnchant,
                    unsigned short& wMount1, unsigned short& wMount2, unsigned short& wMount3, unsigned short& wMount4
)
{
    int nResult                    = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pItemData);
    KG_PROCESS_ERROR(dwItemIndex >= 0 && dwItemIndex <= pQueryRoleData->m_roleDataAll.m_pItemData->m_wItemCount);

    byBox       = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].byBox;
    byPos       = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].byPos;

    if (pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].byDataLen == sizeof(KCOMMON_ITEM_DATA))
    {
        nIsEqui     = false;
        byVersion   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].commonEquipData.byVersion;
        byTabType   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].commonEquipData.byTabType;
        wTabIndex   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].commonEquipData.wTabIndex;
        byBind      = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].commonEquipData.byBind;
        wDurability = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].commonEquipData.wDurability;
        nGenTime    = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].commonEquipData.nGenTime;

    }
    else if(pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].byDataLen == sizeof(KCUSTOM_EQUI_DATA))
    {
        nIsEqui     = true;
        byVersion   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.byVersion;
        byTabType   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.byTabType;
        wTabIndex   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.wTabIndex;
        byBind      = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.byBind;
        wDurability = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.wDurability;
        nGenTime    = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.nGenTime;

        dwRandSeed        = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.dwRandSeed;
        wLeftEnchantTime  = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.wLeftEnchantTime;
        byColorID         = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.byColorID;
        wPermanentEnchant = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.wEnchant[eiPermanentEnchant];
        wTemporaryEnchant = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.wEnchant[eiTemporaryEnchant];
        wMount1           = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.wEnchant[eiMount1];
        wMount2           = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.wEnchant[eiMount2];
        wMount3           = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.wEnchant[eiMount3];
        wMount4           = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[dwItemIndex].customEquipData.wEnchant[eiMount4];
    }
    else
    {
        KG_PROCESS_ERROR(false);
    }

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleBagItemCount(PARSEHANDLE hParser, unsigned char ucBoxIndex, unsigned short& wItemCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;
    unsigned short  usAllItemCount = 0;
    unsigned char   ucBox = 0;
    unsigned short  ucBagItemCount = 0;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pItemData);

    usAllItemCount = pQueryRoleData->m_roleDataAll.m_pItemData->m_wItemCount;

    for (unsigned short i = 0; i < usAllItemCount; i++)
    {
        ucBox  = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[i].byBox;
        if(ucBox == ucBoxIndex)
        {
            ucBagItemCount++;
        }
    }
    
    wItemCount = ucBagItemCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleItemInfoInBag(PARSEHANDLE hParser, unsigned char ucBagIndex, unsigned char ucPos, bool bNext,
         int& nHadFind, unsigned char& byVersion, unsigned char& byTabType, 
         unsigned short& wTabIndex, unsigned char& byBind, unsigned short& wDurability, time_t& nGenTime,
         int& nIsEqui, unsigned long& dwRandSeed, unsigned short& wLeftEnchantTime, unsigned char& byColorID,
         unsigned short& wPermanentEnchant, unsigned short& wTemporaryEnchant,
         unsigned short& wMount1, unsigned short& wMount2, unsigned short& wMount3, unsigned short& wMount4
)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;
    unsigned char   ucBag          = 0;
    unsigned char   ucBagPos       = 0;
    unsigned short  ucBagItemCount = 0;
    unsigned short  usAllItemCount = 0;
    int             nHadFound      = false;
    int             nIndex         = 0;
    bool            bNextRing      = bNext;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pItemData);

    usAllItemCount = pQueryRoleData->m_roleDataAll.m_pItemData->m_wItemCount;

    for (unsigned short i = 0; i < usAllItemCount; i++)
    {
        ucBag       = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[i].byBox;
        ucBagPos    = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[i].byPos;

        if ((ucBag == ucBagIndex) && (ucPos == ucBagPos))
        {
            if (ucPos == 5)// 戒指
            {
                if (bNextRing = true)
                {
                    bNextRing = false;
                    continue;
                }
            }

            nHadFound = true;
            nIndex    = i;
            break;
        }
    }

    if (nHadFound)
    {
        if (pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].byDataLen == sizeof(KCOMMON_ITEM_DATA))
        {
            nIsEqui     = false;
            byVersion   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].commonEquipData.byVersion;
            byTabType   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].commonEquipData.byTabType;
            wTabIndex   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].commonEquipData.wTabIndex;
            byBind      = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].commonEquipData.byBind;
            wDurability = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].commonEquipData.wDurability;
            nGenTime    = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].commonEquipData.nGenTime;

        }
        else if(pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].byDataLen == sizeof(KCUSTOM_EQUI_DATA))
        {
            nIsEqui     = true;
            byVersion   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.byVersion;
            byTabType   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.byTabType;
            wTabIndex   = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.wTabIndex;
            byBind      = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.byBind;
            wDurability = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.wDurability;
            nGenTime    = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.nGenTime;

            dwRandSeed        = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.dwRandSeed;
            wLeftEnchantTime  = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.wLeftEnchantTime;
            byColorID         = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.byColorID;
            wPermanentEnchant = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.wEnchant[eiPermanentEnchant];
            wTemporaryEnchant = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.wEnchant[eiTemporaryEnchant];
            wMount1           = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.wEnchant[eiMount1];
            wMount2           = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.wEnchant[eiMount2];
            wMount3           = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.wEnchant[eiMount3];
            wMount4           = pQueryRoleData->m_roleDataAll.m_pItemData->ItemArray[nIndex].customEquipData.wEnchant[eiMount4];
        }
        else
        {
            KG_PROCESS_ERROR(false);
        }
    }

    nResult = true;
Exit0:
    nHadFind = nHadFound;
    return nResult;
}

int GetGameQuestCount(PARSEHANDLE hParser, int& nCount)
{
    int             nResult         = false;
    KQueryRoleData* pQueryRoleData  = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCount = sizeof(pQueryRoleData->m_roleDataAll.m_QuestData.m_dwQuestState)/sizeof(unsigned char);

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleQuestState(PARSEHANDLE hParser, unsigned long dwQuestIndex, unsigned char& dwQuestState)
{
    int             nResult         = false;
    KQueryRoleData* pQueryRoleData  = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    dwQuestState = pQueryRoleData->m_roleDataAll.m_QuestData.m_dwQuestState[dwQuestIndex];

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleAcceptQuestCount(PARSEHANDLE hParser, unsigned char& dwAcceptQuestCount)
{
    int             nResult         = false;
    KQueryRoleData* pQueryRoleData  = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    dwAcceptQuestCount = pQueryRoleData->m_roleDataAll.m_QuestData.m_AcceptQuestCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleQuestInfo(PARSEHANDLE hParser, unsigned long dwQuestIndex, unsigned short& wQuestID, 
                         unsigned char& byFailed, time_t& nLimitTime, 
                         unsigned char KillNpcCountArray[], int& nKillNPCMaxKind, 
                         int QuestValueArray[], int& nnQuestValueMaxKind
)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(dwQuestIndex < MAX_ACCEPT_QUEST_COUNT);

    wQuestID         = pQueryRoleData->m_roleDataAll.m_QuestData.m_AcceptQuest[dwQuestIndex].wQuestID;
    byFailed         = pQueryRoleData->m_roleDataAll.m_QuestData.m_AcceptQuest[dwQuestIndex].byFailed;
    nLimitTime       = pQueryRoleData->m_roleDataAll.m_QuestData.m_AcceptQuest[dwQuestIndex].nLimitTime;

    memcpy(KillNpcCountArray, pQueryRoleData->m_roleDataAll.m_QuestData.m_AcceptQuest[dwQuestIndex].byKillNpcCount, sizeof(pQueryRoleData->m_roleDataAll.m_QuestData.m_AcceptQuest[dwQuestIndex].byKillNpcCount));
    nKillNPCMaxKind    = sizeof(pQueryRoleData->m_roleDataAll.m_QuestData.m_AcceptQuest[dwQuestIndex].byKillNpcCount);

    memcpy(QuestValueArray, pQueryRoleData->m_roleDataAll.m_QuestData.m_AcceptQuest[dwQuestIndex].nQuestValue, sizeof(pQueryRoleData->m_roleDataAll.m_QuestData.m_AcceptQuest[dwQuestIndex].nQuestValue));
    nnQuestValueMaxKind = sizeof(pQueryRoleData->m_roleDataAll.m_QuestData.m_AcceptQuest[dwQuestIndex].nQuestValue);

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleAssistQuestCount(PARSEHANDLE hParser, unsigned char& dwAssistQuestCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    dwAssistQuestCount = pQueryRoleData->m_roleDataAll.m_QuestData.m_AssistQuestCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleAssistDailyCount(PARSEHANDLE hParser, unsigned char& dwAssistDailyCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    dwAssistDailyCount = pQueryRoleData->m_roleDataAll.m_QuestData.m_AssistDailyCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleAssistQuestInfo(PARSEHANDLE hParser, unsigned long dwAssistQuestIndex, unsigned long& dwPlayerID, unsigned short& wQuestID)
{
    int             nResult         = false;
    KQueryRoleData* pQueryRoleData  = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(dwAssistQuestIndex < MAX_ASSIST_QUEST_COUNT);

    dwPlayerID = pQueryRoleData->m_roleDataAll.m_QuestData.m_AssistQuest[dwAssistQuestIndex].dwPlayerID;
    wQuestID  = pQueryRoleData->m_roleDataAll.m_QuestData.m_AssistQuest[dwAssistQuestIndex].wQuestID;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleDailyQuestCount(PARSEHANDLE hParser, unsigned char& byDailyQuestCount)
{
    int             nResult         = false;
    KQueryRoleData* pQueryRoleData  = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    byDailyQuestCount = pQueryRoleData->m_roleDataAll.m_QuestData.m_DailyQuestCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleDailyQuestInfo(PARSEHANDLE hParser, unsigned long dwQuestIndex, unsigned short& wQuestID, int& nNextAcceptTime)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(dwQuestIndex < MAX_ACCEPT_QUEST_COUNT);

    wQuestID        = pQueryRoleData->m_roleDataAll.m_QuestData.m_DailyQuest[dwQuestIndex].wQuestID;
    nNextAcceptTime = (int)pQueryRoleData->m_roleDataAll.m_QuestData.m_DailyQuest[dwQuestIndex].nNextAcceptTime;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleProfessionCount(PARSEHANDLE hParser, unsigned char& byCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pProfessionData);

    byCount = pQueryRoleData->m_roleDataAll.m_pProfessionData->byCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleProfessionInfo(PARSEHANDLE hParser, unsigned long dwProfessionIndex, unsigned char& byProfessionID, unsigned char& byMaxLevel, unsigned char& byCurrentLevel, unsigned short& wProficiency, unsigned char& byBranchID)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pProfessionData);
    KG_PROCESS_ERROR(dwProfessionIndex >= 0 && dwProfessionIndex < pQueryRoleData->m_roleDataAll.m_pProfessionData->byCount);

    byProfessionID = pQueryRoleData->m_roleDataAll.m_pProfessionData->Profession[dwProfessionIndex].m_byProfessionID;
    byMaxLevel     = pQueryRoleData->m_roleDataAll.m_pProfessionData->Profession[dwProfessionIndex].m_byMaxLevel;
    byCurrentLevel = pQueryRoleData->m_roleDataAll.m_pProfessionData->Profession[dwProfessionIndex].m_byCurrentLevel;
    wProficiency   = pQueryRoleData->m_roleDataAll.m_pProfessionData->Profession[dwProfessionIndex].m_wProficiency;
    byBranchID     = pQueryRoleData->m_roleDataAll.m_pProfessionData->Profession[dwProfessionIndex].m_byBranchID;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleRecipeListOfCraft(PARSEHANDLE hParser, unsigned long dwCraftID, unsigned long dwRecipeIndex, int& bIsLearned)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(dwCraftID > 0 && dwCraftID <= MAX_CRAFT_COUNT);
    KG_PROCESS_ERROR(dwRecipeIndex >= 0 && dwRecipeIndex < MAX_RECIPE_ID);

    bIsLearned = pQueryRoleData->m_roleDataAll.m_byRecipeState[dwCraftID - 1][dwRecipeIndex];

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleMoveState(PARSEHANDLE hParser, unsigned char& byMoveState)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    byMoveState = pQueryRoleData->m_roleDataAll.m_StateInfo.byMoveState;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleCurrentTrack(PARSEHANDLE hParser, unsigned short& wCurrentTrack)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    wCurrentTrack = pQueryRoleData->m_roleDataAll.m_StateInfo.wCurrentTrack;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleMoveFrameCounter(PARSEHANDLE hParser, int& nMoveFrameCounter)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nMoveFrameCounter = pQueryRoleData->m_roleDataAll.m_StateInfo.nMoveFrameCounter;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleFromFlyNode(PARSEHANDLE hParser, unsigned short& wFromFlyNode)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    wFromFlyNode = pQueryRoleData->m_roleDataAll.m_StateInfo.wFromFlyNode;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleTargetCityID(PARSEHANDLE hParser, unsigned short& wTargetCityID)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    wTargetCityID = pQueryRoleData->m_roleDataAll.m_StateInfo.wTargetCityID;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleExperience(PARSEHANDLE hParser, int& nExperience)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nExperience = pQueryRoleData->m_roleDataAll.m_StateInfo.nExperience;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleCurrentLife(PARSEHANDLE hParser, int& nCurrentLife)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCurrentLife = pQueryRoleData->m_roleDataAll.m_StateInfo.nCurrentLife;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleCurrentMana(PARSEHANDLE hParser, int& nCurrentMana)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCurrentMana = pQueryRoleData->m_roleDataAll.m_StateInfo.nCurrentMana;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleCurrentStamina(PARSEHANDLE hParser, int& nCurrentStamina)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCurrentStamina = pQueryRoleData->m_roleDataAll.m_StateInfo.nCurrentStamina;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleCurrentThew(PARSEHANDLE hParser, int& nCurrentThew)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCurrentThew = pQueryRoleData->m_roleDataAll.m_StateInfo.nCurrentThew;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleCurrentTrainValue(PARSEHANDLE hParser, int& nCurrentTrainValue)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCurrentTrainValue = pQueryRoleData->m_roleDataAll.m_StateInfo.nCurrentTrainValue;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleUsedTrainValue(PARSEHANDLE hParser, int& nUsedTrainValue)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nUsedTrainValue = pQueryRoleData->m_roleDataAll.m_StateInfo.nUsedTrainValue;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleAddTrainTimeInToday(PARSEHANDLE hParser, int& nAddTrainTimeInToday)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nAddTrainTimeInToday = pQueryRoleData->m_roleDataAll.m_StateInfo.nAddTrainTimeInToday;

    nResult = true;
Exit0:
    return nResult;
}

int GetRolePKState(PARSEHANDLE hParser, unsigned char& byPKState)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    byPKState = pQueryRoleData->m_roleDataAll.m_StateInfo.byPKState;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleCloseSlayLeftTime(PARSEHANDLE hParser, unsigned short& wCloseSlayLeftTime)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    wCloseSlayLeftTime = pQueryRoleData->m_roleDataAll.m_StateInfo.wCloseSlayLeftTime;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleLeftReviveFrame(PARSEHANDLE hParser, unsigned short& wLeftReviveFrame)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    wLeftReviveFrame = pQueryRoleData->m_roleDataAll.m_StateInfo.wLeftReviveFrame;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleLastSituReviveTime(PARSEHANDLE hParser, time_t& nLastSituReviveTime)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nLastSituReviveTime = pQueryRoleData->m_roleDataAll.m_StateInfo.nLastSituReviveTime;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleSituReviveCount(PARSEHANDLE hParser, unsigned char& bySituReviveCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    bySituReviveCount = pQueryRoleData->m_roleDataAll.m_StateInfo.bySituReviveCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleKillerID(PARSEHANDLE hParser, unsigned long& dwKillerID)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    dwKillerID = pQueryRoleData->m_roleDataAll.m_StateInfo.dwKillerID;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleCurrentKillPoint(PARSEHANDLE hParser, unsigned short& wCurrentKillPoint)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    wCurrentKillPoint = pQueryRoleData->m_roleDataAll.m_StateInfo.wCurrentKillPoint;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleCurrentPrestige(PARSEHANDLE hParser, int& nCurrentPrestige)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCurrentPrestige = pQueryRoleData->m_roleDataAll.m_StateInfo.nCurrentPrestige;

    nResult = true;
Exit0:
    return nResult;
}

int GetContribution(PARSEHANDLE hParser, int& nContribution)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nContribution = pQueryRoleData->m_roleDataAll.m_StateInfo.nContribution;

    nResult = true;
Exit0:
    return nResult;
}

int GetMaxLevel(PARSEHANDLE hParser, int& nMaxLevel)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nMaxLevel = pQueryRoleData->m_roleDataAll.m_StateInfo.nMaxLevel;

    nResult = true;
Exit0:
    return nResult;
}

int GetBanTime(PARSEHANDLE hParser, time_t& nBanTime)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nBanTime = pQueryRoleData->m_roleDataAll.m_StateInfo.nBanTime;

    nResult = true;
Exit0:
    return nResult;
}

int GetHideHat(PARSEHANDLE hParser, int& nHideHat)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nHideHat = pQueryRoleData->m_roleDataAll.m_StateInfo.bHideHat;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleBuffCount(PARSEHANDLE hParser, unsigned char& byCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pBuffDBData);

    byCount = pQueryRoleData->m_roleDataAll.m_pBuffDBData->byCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleBuffInfo(
                    PARSEHANDLE hParser, unsigned long BuffIndex, 
                    unsigned long& dwID, int& nLevel, unsigned long& dwRecipeKey, 
                    unsigned long& dwSkillSrcID, unsigned char& bySkillSrcLevel, 
                    unsigned char& byStackNum, int& nLeftFrame
)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pBuffDBData);
    KG_PROCESS_ERROR(BuffIndex >= 0 && BuffIndex <pQueryRoleData->m_roleDataAll.m_pBuffDBData->byCount);

    dwID            = pQueryRoleData->m_roleDataAll.m_pBuffDBData->Items[BuffIndex].RecipeKey.dwID;
    nLevel          = pQueryRoleData->m_roleDataAll.m_pBuffDBData->Items[BuffIndex].RecipeKey.nLevel;
    dwRecipeKey     = pQueryRoleData->m_roleDataAll.m_pBuffDBData->Items[BuffIndex].RecipeKey.dwRecipeKey[0];//todo:这里这样用有点不太好，再改正
    dwSkillSrcID    = pQueryRoleData->m_roleDataAll.m_pBuffDBData->Items[BuffIndex].dwSkillSrcID;
    bySkillSrcLevel = pQueryRoleData->m_roleDataAll.m_pBuffDBData->Items[BuffIndex].bySkillSrcLevel;
    byStackNum      = pQueryRoleData->m_roleDataAll.m_pBuffDBData->Items[BuffIndex].byStackNum;
    nLeftFrame      = pQueryRoleData->m_roleDataAll.m_pBuffDBData->Items[BuffIndex].nLeftFrame;

    nResult = true;
Exit0:
    return true;
}

int GetRoleReputeCount(PARSEHANDLE hParser, unsigned short& usCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pForceDBData);

    usCount = pQueryRoleData->m_roleDataAll.m_pForceDBData->wCount;
    nResult = true;
Exit0:
    return true;
}

int GetRoleReputeInfo(PARSEHANDLE hParser, int nIndex, unsigned char& ucFightFlag, unsigned char& ucForce, unsigned char& ucLevel, unsigned char& ucShow, short& shRepute)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;
    
    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pForceDBData);
    KG_PROCESS_ERROR(nIndex >= 0 && nIndex < pQueryRoleData->m_roleDataAll.m_pForceDBData->wCount);
    
    ucFightFlag = pQueryRoleData->m_roleDataAll.m_pForceDBData->ForceInfo[nIndex].byFightFlag;
    ucForce     = pQueryRoleData->m_roleDataAll.m_pForceDBData->ForceInfo[nIndex].byForce;
    ucLevel     = pQueryRoleData->m_roleDataAll.m_pForceDBData->ForceInfo[nIndex].byLevel;
    ucShow      = pQueryRoleData->m_roleDataAll.m_pForceDBData->ForceInfo[nIndex].byShow;
    shRepute    = pQueryRoleData->m_roleDataAll.m_pForceDBData->ForceInfo[nIndex].shRepute;

    nResult = true;
Exit0:
    return true;
}

int GetRolePQCount(PARSEHANDLE hParser, unsigned short& wCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pPQList);

    wCount = pQueryRoleData->m_roleDataAll.m_pPQList->wCount;

    nResult = true;
Exit0:
    return true;
}

int GetRolePQInfo(PARSEHANDLE hParser, unsigned long PQIndex, unsigned long& dwID)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(pQueryRoleData->m_roleDataAll.m_pPQList);

    dwID = pQueryRoleData->m_roleDataAll.m_pPQList->dwPQID[PQIndex];

    nResult = true;
Exit0:
    return true;
}

int GetRoleHeroFlag(PARSEHANDLE hParser, int& bHeroFlag)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    bHeroFlag = pQueryRoleData->m_roleDataAll.m_bHeroFlag;

    nResult = true;
Exit0:
    return true;
}

int GetRoleHeroMapCopyOpenFlag(PARSEHANDLE hParser, unsigned long dwMapID, int& bOpenFlag)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(dwMapID > 0 && dwMapID <= MAX_MAP_ID);

    pQueryRoleData->m_roleDataAll.m_HeroMapCopyOpenFlag.GetBit(dwMapID - 1, &bOpenFlag);

    nResult = true;
Exit0:
    return nResult;
}


int GetRoleAchievementPoint(PARSEHANDLE hParser, int& nPoint)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nPoint = pQueryRoleData->m_roleDataAll.m_AchievementData.m_nPoint;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleAchievementRecord(PARSEHANDLE hParser, int& nRecord)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nRecord = pQueryRoleData->m_roleDataAll.m_AchievementData.m_nRecord;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleAchievementCounterCount(PARSEHANDLE hParser, int nID, int& nCount)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;
    KCOUNTER_TABLE::iterator    it;

    KG_PROCESS_ERROR(pQueryRoleData);

    it = pQueryRoleData->m_roleDataAll.m_AchievementData.m_CounterTable.find(nID);
    KG_PROCESS_ERROR(it != pQueryRoleData->m_roleDataAll.m_AchievementData.m_CounterTable.end());

    nCount = it->second;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleAchievementAchieveStateFormID(PARSEHANDLE hParser, int nID, int& nIsAchieve)
{
    int             nResult        = false;
    int             nRetCode       = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KGLOG_PROCESS_ERROR(nID >= 0 && nID < MAX_ACHIEVEMENT_ID);

    nRetCode = pQueryRoleData->m_roleDataAll.m_AchievementData.m_BitSet.GetBit(nID, &nIsAchieve);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleAchievementCount(PARSEHANDLE hParser, int& nCount)
{
    int             nResult        = false;
    int             nRetCode       = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCount = (int)pQueryRoleData->m_roleDataAll.m_AchievementData.m_CounterTable.size();
    
    nResult = true;
Exit0:
    return nResult;  
}

int GetRoleAchievementInfo(PARSEHANDLE hParser, unsigned long ulIndex, int& nID, int& nCount, int& nIsAchieve)
{
    int             nResult        = false;
    int             nRetCode       = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;
    KCOUNTER_TABLE::iterator it;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(ulIndex >=0 && ulIndex < pQueryRoleData->m_roleDataAll.m_AchievementData.m_CounterTable.size());


    for (it = pQueryRoleData->m_roleDataAll.m_AchievementData.m_CounterTable.begin(); it != pQueryRoleData->m_roleDataAll.m_AchievementData.m_CounterTable.end(); ++it)
    {
        if (ulIndex<=0)
        {
            break;
        }
        else
        {
            ulIndex--;
        }
    }

    if (it != pQueryRoleData->m_roleDataAll.m_AchievementData.m_CounterTable.end())
    {
        int* pnTempID = (int*)&it->first;
        int* pnTempCount = &it->second;
        nID  = *pnTempID;
        nCount = *pnTempCount;

        nRetCode = pQueryRoleData->m_roleDataAll.m_AchievementData.m_BitSet.GetBit(nID, &nIsAchieve);
        KG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;  
}

int GetRoleDesignationCurrentPrefix(PARSEHANDLE hParser, int& nCurrentPrefix)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCurrentPrefix = pQueryRoleData->m_roleDataAll.m_DesignationData.m_nCurrentPrefix;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleDesignationCurrentPostfix(PARSEHANDLE hParser, int& nCurrentPostfix)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nCurrentPostfix = pQueryRoleData->m_roleDataAll.m_DesignationData.m_nCurrentPostfix;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleDesignationGenerationIndex(PARSEHANDLE hParser, int& nGenerationIndex)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nGenerationIndex = pQueryRoleData->m_roleDataAll.m_DesignationData.m_nGenerationIndex;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleDesignationNameInForceIndex(PARSEHANDLE hParser, int& nNameInForceIndex)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nNameInForceIndex = pQueryRoleData->m_roleDataAll.m_DesignationData.m_nNameInForceIndex;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleDesignationDisplayFlag(PARSEHANDLE hParser, int& nDisplayFlag)
{
    int             nResult        = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    nDisplayFlag = pQueryRoleData->m_roleDataAll.m_DesignationData.m_bDisplayFlag;

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleDesignationIsPrefixAcquiredFormPrefix(PARSEHANDLE hParser, int nPrefix, int& nIsPrefixAcquired)
{
    int             nResult        = false;
    int             nRetCode       = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(nPrefix >= 0 && nPrefix <= MAX_DESIGNATION_FIX_ID);

    nRetCode = pQueryRoleData->m_roleDataAll.m_DesignationData.m_AcquiredPrefix.GetBit(nPrefix, &nIsPrefixAcquired);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;    
}

int GetRoleDesignationIsPostfixAcquiredFormPostfix(PARSEHANDLE hParser, int nPostfix, int& nIsPostfixAcquired)
{
    int             nResult        = false;
    int             nRetCode       = false;
    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(nPostfix >= 0 && nPostfix <= MAX_DESIGNATION_FIX_ID);

    nRetCode = pQueryRoleData->m_roleDataAll.m_DesignationData.m_AcquiredPrefix.GetBit(nPostfix, &nIsPostfixAcquired);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleQuestDropInfo(PARSEHANDLE hParser, unsigned long dwID, int nItemPosIndex, int& nHadFind, unsigned long& dwFrustration, int& nCount)
{
    int             nResult        = false;
    int             nRetCode       = false;

    KQueryRoleData*                                           pQueryRoleData = (KQueryRoleData*)hParser;
    KProbability<uint64_t, MILLION_NUM, 128>::KRandData*      pRandData      = NULL;
    uint64_t                                                  ukey           = 0;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(dwID > 0);
    KG_PROCESS_ERROR(nItemPosIndex >= 0 && nItemPosIndex < QUEST_END_ITEM_COUNT);

    ukey = MAKE_INT64(dwID, nItemPosIndex);

    pRandData = pQueryRoleData->m_roleDataAll.m_QuestRand.m_RandCache.Find(ukey);
    if (pRandData)
    {
        nHadFind = true;
        dwFrustration   = pRandData->dwFrustration;
        nCount          = pRandData->nCount;
    }
    else
    {
        nHadFind = false;
    }

    nResult = true;
Exit0:
    return nResult;
}

int GetVenationInfo(PARSEHANDLE hParser, unsigned long dwSkillID, unsigned long dwSkillLevel, int& nHadFind, unsigned long& dwFrustration, int& nCount )
{
    int             nResult        = false;
    int             nRetCode       = false;

    KQueryRoleData*                                pQueryRoleData = (KQueryRoleData*)hParser;
    KProbability<uint64_t, 1024, 32>::KRandData*   pRandData      = NULL;

    uint64_t        ukey           = 0;

    KG_PROCESS_ERROR(pQueryRoleData);
    KG_PROCESS_ERROR(dwSkillID > 0);
    KG_PROCESS_ERROR(dwSkillLevel >= 0);

    ukey = MAKE_INT64(dwSkillID, dwSkillLevel);

    pRandData = pQueryRoleData->m_roleDataAll.m_VenationRand.m_RandCache.Find(ukey);
    if (pRandData)
    {
        nHadFind = true;
        dwFrustration   = pRandData->dwFrustration;
        nCount          = pRandData->nCount;
    }
    else
    {
        nHadFind = false;
    }

    nResult = true;
Exit0:
    return nResult;
}

int GetRoleMentorBaseInfo(PARSEHANDLE hParser, unsigned char& ucMaxApprenticeCount, int& nAcquiredMentorValue,
                                  time_t& nLastEvokeMentorTime, unsigned char& ucEvokeMentorCount
)
{
    int             nResult        = false;
    int             nRetCode       = false;

    KQueryRoleData* pQueryRoleData = (KQueryRoleData*)hParser;

    KG_PROCESS_ERROR(pQueryRoleData);

    ucMaxApprenticeCount = pQueryRoleData->m_roleDataAll.m_MontorData.byMaxApprenticeCount;
    nAcquiredMentorValue = pQueryRoleData->m_roleDataAll.m_MontorData.nAcquiredMentorValue;
    nLastEvokeMentorTime = pQueryRoleData->m_roleDataAll.m_MontorData.nLastEvokeMentorTime;
    ucEvokeMentorCount   = pQueryRoleData->m_roleDataAll.m_MontorData.byEvokeMentorCount;

    nResult = true;
Exit0:
    return nResult;
}


PARSEHANDLE ParseMailBoxInfoData(unsigned char* pbyData, size_t uDataLen)
{
    int nResult  = false;
    int nRetCode = false;

    KMailBoxInfoData* pMailBoxInfoData = new KMailBoxInfoData;
    KG_PROCESS_ERROR(pMailBoxInfoData);

    nRetCode = pMailBoxInfoData->PaserMailBoxInfo(pbyData, uDataLen);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete pMailBoxInfoData;
        pMailBoxInfoData = NULL;
    }
    return (PARSEHANDLE)pMailBoxInfoData;
}

int GetMailBaseInfo(PARSEHANDLE hParser, unsigned long& ulNextMailID, 
                            unsigned long& ulLastGlobalMailIndex,
                            unsigned short& usVersion
)
{
    int               nResult          = false;
    KMailBoxInfoData* pMailBoxInfoData = (KMailBoxInfoData*)hParser;

    KG_PROCESS_ERROR(pMailBoxInfoData);

    ulNextMailID            = pMailBoxInfoData->m_ulNextMailID;
    ulLastGlobalMailIndex   = pMailBoxInfoData->m_ulLastGlobalMailIndex;
    usVersion               = pMailBoxInfoData->m_usVersion;

    nResult = true;
Exit0:
    return nResult;
}

int GetMailCount(PARSEHANDLE hParser, int& nMailCount)
{
    int               nResult          = false;
    KMailBoxInfoData* pMailBoxInfoData = (KMailBoxInfoData*)hParser;

    KG_PROCESS_ERROR(pMailBoxInfoData);

    nMailCount = (int)pMailBoxInfoData->m_MailList.size();

    nResult = true;
Exit0:
    return nResult;
}

int GetMailInfo(PARSEHANDLE hParser, unsigned long dwMailIndex,     
                unsigned long& dwMailID, unsigned char& ucType,
                bool&   bRead,
                char szSenderName[], int nInSenderNameSize, 
                unsigned long& ulSenderID,
                char szTitle[], int nInTitleSize,
                char szText[], int nInTextSize,
                time_t& nSendTime,
                time_t& nReceiveTime,
                time_t& nLockTime, 
                int&    nMoney,
                int&    nItemCount
)
{
    int               nResult            = false;
    KMailBoxInfoData* pMailBoxInfoData   = (KMailBoxInfoData*)hParser;

    int               nSrcSenderNameSize = sizeof(pMailBoxInfoData->m_MailList[dwMailIndex]->szSenderName);
    int               nSrcTitleSize      = sizeof(pMailBoxInfoData->m_MailList[dwMailIndex]->szTitle);

    KG_PROCESS_ERROR(pMailBoxInfoData);
    KG_PROCESS_ERROR(nInSenderNameSize >= nSrcSenderNameSize);
    KG_PROCESS_ERROR(nInTitleSize >= nSrcTitleSize);
    KG_PROCESS_ERROR(dwMailIndex < pMailBoxInfoData->m_MailList.size());

    dwMailID = pMailBoxInfoData->m_MailList[dwMailIndex]->ulMailID;
    ucType   = pMailBoxInfoData->m_MailList[dwMailIndex]->ucType;
    bRead    = pMailBoxInfoData->m_MailList[dwMailIndex]->bRead;
    
    strncpy(szSenderName, pMailBoxInfoData->m_MailList[dwMailIndex]->szSenderName, nInSenderNameSize);
    szSenderName[nInSenderNameSize - 1] = '\0';
    strncpy(szTitle, pMailBoxInfoData->m_MailList[dwMailIndex]->szTitle, nInTitleSize);
    szTitle[nInTitleSize - 1] = '\0';
    strncpy(szText, pMailBoxInfoData->m_MailList[dwMailIndex]->szText, nInTextSize);
    szText[nInTextSize - 1] = '\0';

    ulSenderID      = pMailBoxInfoData->m_MailList[dwMailIndex]->dwSenderID;
    nSendTime       = pMailBoxInfoData->m_MailList[dwMailIndex]->nSendTime;
    nReceiveTime    = pMailBoxInfoData->m_MailList[dwMailIndex]->nRecvTime;
    nLockTime       = pMailBoxInfoData->m_MailList[dwMailIndex]->nLockTime;
    nMoney          = pMailBoxInfoData->m_MailList[dwMailIndex]->nMoney;
    nItemCount      = pMailBoxInfoData->m_MailList[dwMailIndex]->nItemCount;

    nResult = true;
Exit0:
    return nResult;
}

int GetInMailItemInfo(PARSEHANDLE hParser, unsigned long dwMailIndex,
                  int nItemIndex, bool& bAcquired, int& nPrice,
                  unsigned char& byVersion, unsigned char& byTabType, 
                  unsigned short& wTabIndex, unsigned char& byBind,
                  unsigned short& wDurability, time_t& nGenTime, 
                  int& nIsEquip, unsigned long& dwRandSeed, 
                  unsigned short& wLeftEnchantTime, unsigned char& byColorID,
                  unsigned short& wPermanentEnchant, unsigned short& wTemporaryEnchant,
                  unsigned short& wMount1, unsigned short& wMount2, unsigned short& wMount3, unsigned short& wMount4
)
{
    int                nResult          = false;
    KMailBoxInfoData*  pMailBoxInfoData = (KMailBoxInfoData*)hParser;
    KCOMMON_ITEM_DATA* pItemData        = NULL;
    KCUSTOM_EQUI_DATA* pEquiData        = NULL;
    unsigned char*     pbyData          = NULL;

    KG_PROCESS_ERROR(pMailBoxInfoData);
    KG_PROCESS_ERROR(dwMailIndex >=0 && dwMailIndex < pMailBoxInfoData->m_MailList.size());
    KG_PROCESS_ERROR(nItemIndex >= 0 && nItemIndex < pMailBoxInfoData->m_MailList[dwMailIndex]->nItemCount);

    bAcquired   = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].bAcquired;
    nPrice      = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].nPrice;
    nIsEquip    = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].nIsEquip;

    if (nIsEquip)
    {
        byVersion   = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.byVersion;
        byTabType   = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.byTabType;
        wTabIndex   = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.wTabIndex;
        byBind      = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.byBind;
        wDurability = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.wDurability;
        nGenTime    = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.nGenTime;

        dwRandSeed          = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.dwRandSeed;
        wLeftEnchantTime    = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.wLeftEnchantTime;
        byColorID           = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.byColorID;
        wPermanentEnchant   = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.wEnchant[eiPermanentEnchant];
        wTemporaryEnchant   = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.wEnchant[eiTemporaryEnchant];
        wMount1             = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.wEnchant[eiMount1];
        wMount2             = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.wEnchant[eiMount2];
        wMount3             = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.wEnchant[eiMount3];
        wMount4             = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].customEquipData.wEnchant[eiMount4];
    }
    else
    {
        byVersion   = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].commonEquipData.byVersion;
        byTabType   = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].commonEquipData.byTabType;
        wTabIndex   = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].commonEquipData.wTabIndex;
        byBind      = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].commonEquipData.byBind;
        wDurability = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].commonEquipData.wDurability;
        nGenTime    = pMailBoxInfoData->m_MailList[dwMailIndex]->MailItem[nItemIndex].commonEquipData.nGenTime;
    }

    nResult = true;
Exit0:
    return nResult;
}

PARSEHANDLE ParseFellowShipData(unsigned char* pbyData, size_t uDataLen)
{
    int nResult  = false;
    int nRetCode = false;

    KFellowShipData* pFellowShipData = new KFellowShipData;
    KG_PROCESS_ERROR(pFellowShipData);

    nRetCode = pFellowShipData->PaserFellowShipData(pbyData, uDataLen);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete pFellowShipData;
        pFellowShipData = NULL;
    }
    return (PARSEHANDLE)pFellowShipData;
}

int GetFellowShipCount(PARSEHANDLE hParser, int& nFellowShipCount)
{
    int              nResult         = false;
    KFellowShipData* pFellowShipData = (KFellowShipData*)hParser;

    KG_PROCESS_ERROR(pFellowShipData);

    nFellowShipCount = (int)pFellowShipData->m_FellowshipArray.size();

    nResult = true;
Exit0:
    return nResult;
}

int GetFellowshipInfo(PARSEHANDLE hParser, int nFellowShipIndex, unsigned long& dwAlliedPlayerID, 
                      int& nAttraction, int& nGroupID, int& nMarried, 
                      int& nBrother, int& nWhisperDailyCount, int& nDuelDailyCount, 
                      char szRemark[], int nRemarkSize
)
{
    int              nResult            = false;   
    KFellowShipData* pFellowShipData    = (KFellowShipData*)hParser;
    int              nSrcRemarkSize     = sizeof(pFellowShipData->m_FellowshipArray[nFellowShipIndex]->szRemark);

    KG_PROCESS_ERROR(pFellowShipData);
    KG_PROCESS_ERROR(nRemarkSize >= nSrcRemarkSize);
    KG_PROCESS_ERROR(nFellowShipIndex < (int)pFellowShipData->m_FellowshipArray.size());

    dwAlliedPlayerID    = pFellowShipData->m_FellowshipArray[nFellowShipIndex]->dwAlliedPlayerID;
    nAttraction         = pFellowShipData->m_FellowshipArray[nFellowShipIndex]->nAttraction;
    nGroupID            = pFellowShipData->m_FellowshipArray[nFellowShipIndex]->Flags.dwGroupID;
    nMarried            = pFellowShipData->m_FellowshipArray[nFellowShipIndex]->Flags.bMarried;
    nBrother            = pFellowShipData->m_FellowshipArray[nFellowShipIndex]->Flags.bBrother;
    nWhisperDailyCount  = pFellowShipData->m_FellowshipArray[nFellowShipIndex]->Flags.nWhisperDailyCount;
    nDuelDailyCount     = pFellowShipData->m_FellowshipArray[nFellowShipIndex]->Flags.nDuelDailyCount;

    strncpy(szRemark, pFellowShipData->m_FellowshipArray[nFellowShipIndex]->szRemark, nRemarkSize);
    szRemark[nRemarkSize- 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

int GetFoeCount(PARSEHANDLE hParser, int& nFoeCount)
{
    int              nResult         = false;
    KFellowShipData* pFellowShipData = (KFellowShipData*)hParser;

    KG_PROCESS_ERROR(pFellowShipData);
    
    nFoeCount = (int)pFellowShipData->m_FoeArray.size();

    nResult = true;
Exit0:
    return nResult;
}

int GetFoeID(PARSEHANDLE hParser, int nFoeIndex, unsigned long& dwAlliedPlayerID)
{
    int              nResult         = false;
    KFellowShipData* pFellowShipData = (KFellowShipData*)hParser;

    KG_PROCESS_ERROR(pFellowShipData);
    KG_PROCESS_ERROR(nFoeIndex < (int)pFellowShipData->m_FoeArray.size());

    dwAlliedPlayerID = pFellowShipData->m_FoeArray[nFoeIndex]->dwAlliedPlayerID;

    nResult = true;
Exit0:
    return nResult;
}

int GetBlackListCount(PARSEHANDLE hParser, int& nBlackListCount)
{
    int              nResult         = false;
    KFellowShipData* pFellowShipData = (KFellowShipData*)hParser;

    KG_PROCESS_ERROR(pFellowShipData);

    nBlackListCount = (int)pFellowShipData->m_BlackArray.size();

    nResult = true;
Exit0:
    return nResult;
}

int GetBlackListID(PARSEHANDLE hParser, int nBlackListIndex, unsigned long& dwAlliedPlayerID)
{
    int              nResult         = false;
    KFellowShipData* pFellowShipData = (KFellowShipData*)hParser;

    KG_PROCESS_ERROR(pFellowShipData);
    KG_PROCESS_ERROR(nBlackListIndex < (int)pFellowShipData->m_BlackArray.size());

    dwAlliedPlayerID = pFellowShipData->m_BlackArray[nBlackListIndex]->dwAlliedPlayerID;

    nResult = true;
Exit0:
    return nResult;
}

PARSEHANDLE PaserAuctionItemData(unsigned char* pbyData, size_t uDataLen)
{
    int nResult  = false;
    int nRetCode = false;

    KAuctionData* pAuctionData = new KAuctionData;
    KG_PROCESS_ERROR(pAuctionData);

    nRetCode = pAuctionData->PaserAuctionItemData(pbyData, uDataLen);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete pAuctionData;
        pAuctionData = NULL;
    }
    return (PARSEHANDLE)pAuctionData;
}

int GetAuctionItemInfo(
                       PARSEHANDLE hParser, unsigned char& byVersion, unsigned char& byTabType, 
                       unsigned short& wTabIndex, unsigned char& byBind, unsigned short& wDurability, 
                       time_t& nGenTime, int& nIsEqui, unsigned long& dwRandSeed, 
                       unsigned long& dwLeftEnchantTime, unsigned char& byColorID,
                       unsigned short& wPermanentEnchant, unsigned short& wTemporaryEnchant,
                       unsigned short& wMount1, unsigned short& wMount2, unsigned short& wMount3, unsigned short& wMount4
)
{
    int             nResult         = false;
    KAuctionData*   pAuctionData    = (KAuctionData*)hParser;

    KG_PROCESS_ERROR(pAuctionData);

    nIsEqui = pAuctionData->m_IsEqui;
    
    byVersion   = pAuctionData->m_ItemData.byVersion;
    byTabType   = pAuctionData->m_ItemData.byTabType;
    wTabIndex   = pAuctionData->m_ItemData.wTabIndex;
    byBind      = pAuctionData->m_ItemData.byBind;
    wDurability = pAuctionData->m_ItemData.wDurability;
    nGenTime    = pAuctionData->m_ItemData.nGenTime;

    if (pAuctionData->m_IsEqui)
    {
        dwRandSeed          = pAuctionData->m_ItemData.dwRandSeed;
        dwLeftEnchantTime   = pAuctionData->m_ItemData.dwRandSeed;
        byColorID           = pAuctionData->m_ItemData.byColorID;
        wPermanentEnchant   = pAuctionData->m_ItemData.wEnchant[eiPermanentEnchant];
        wTemporaryEnchant   = pAuctionData->m_ItemData.wEnchant[eiTemporaryEnchant];
        wMount1             = pAuctionData->m_ItemData.wEnchant[eiMount1];
        wMount2             = pAuctionData->m_ItemData.wEnchant[eiMount2];
        wMount3             = pAuctionData->m_ItemData.wEnchant[eiMount3];
        wMount4             = pAuctionData->m_ItemData.wEnchant[eiMount4];
    }

    nResult = true;
Exit0:
    return nResult;
}

PARSEHANDLE PaserTongData(unsigned char* pbyData, size_t uDataLen)
{
    int nResult  = false;
    int nRetCode = false;

    KTongData* pTongData = new KTongData;
    KG_PROCESS_ERROR(pTongData);

    nRetCode = pTongData->PaserTongDataInfo(pbyData, uDataLen);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete pTongData;
        pTongData = NULL;
    }
    return (PARSEHANDLE)pTongData;
}

int GetTongBaseInfo(PARSEHANDLE hParser, char szName[], int nNameSize, 
                    unsigned long& dwMaster, int& nFund, int& nLevel, 
                    int& nDevelopmentPoint, unsigned short& wMaxMemberCount, 
                    unsigned char& byTongState, time_t& nStateTimer, unsigned char& byCamp
                    )
{
    int        nResult = false;
    KTongData* pTongData = (KTongData*)hParser;

    KG_PROCESS_ERROR(pTongData);
    KG_PROCESS_ERROR(nNameSize >= sizeof(pTongData->m_TongBaseInfo.szName));

    strncpy(szName, pTongData->m_TongBaseInfo.szName, nNameSize);
    szName[nNameSize - 1] = '\0';

    dwMaster            = pTongData->m_TongBaseInfo.dwMaster;
    nFund               = pTongData->m_TongBaseInfo.nFund;
    nLevel              = pTongData->m_TongBaseInfo.nLevel;
    nDevelopmentPoint   = pTongData->m_TongBaseInfo.nDevelopmentPoint;
    wMaxMemberCount     = pTongData->m_TongBaseInfo.wMaxMemberCount;
    byTongState         = pTongData->m_TongBaseInfo.byTongState;
    nStateTimer         = pTongData->m_TongBaseInfo.nStateTimer;
    byCamp              = pTongData->m_TongBaseInfo.byCamp;

    nResult = true;
Exit0:
    return nResult;
}

int GetTongExInfo(PARSEHANDLE hParser, char szAnnouncement[], int nAnnouncementSize, 
                  char szOnlineMessage[], int nOnlineMessage, 
                  char szIntroduction[], int nIntroduction, 
                  char szRules[], int nRules 
                  )
{
    int        nResult      = false;
    KTongData* pTongData    = (KTongData*)hParser;

    KG_PROCESS_ERROR(pTongData);
    KG_PROCESS_ERROR(nAnnouncementSize >= sizeof(pTongData->m_szAnnouncement));
    KG_PROCESS_ERROR(nOnlineMessage >= sizeof(pTongData->m_szOnlineMessage));
    KG_PROCESS_ERROR(nIntroduction >= sizeof(pTongData->m_szIntroduction));
    KG_PROCESS_ERROR(nRules >= sizeof(pTongData->m_szRules));

    strncpy(szAnnouncement, pTongData->m_szAnnouncement, nAnnouncementSize);
    szAnnouncement[nAnnouncementSize - 1] = '\0';

    strncpy(szOnlineMessage, pTongData->m_szOnlineMessage, nOnlineMessage);
    szAnnouncement[nOnlineMessage - 1] = '\0';

    strncpy(szIntroduction, pTongData->m_szIntroduction, nIntroduction);
    szAnnouncement[nIntroduction - 1] = '\0';

    strncpy(szRules, pTongData->m_szRules, nRules);
    szAnnouncement[nRules - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

int GetTongMemberCount(PARSEHANDLE hParser, int& nCount)
{
    int        nResult      = false;
    KTongData* pTongData    = (KTongData*)hParser;

    KG_PROCESS_ERROR(pTongData);

    nCount = (int)pTongData->m_Memberlist.size();

    nResult = true;
Exit0:
    return nResult;
}

int GetTongMemberInfo(PARSEHANDLE hParser, int nMemberIndex, unsigned long& dwID, 
                      int& nGroupID, time_t& nJoinTime, time_t& nLastOfflineTime,
                      char szRemark[], int nRemarkSize, int& nSalary 
)
{
    int        nResult      = false;
    KTongData* pTongData    = (KTongData*)hParser;

    KG_PROCESS_ERROR(pTongData);
    KG_PROCESS_ERROR(nMemberIndex < (int)pTongData->m_Memberlist.size());
    KG_PROCESS_ERROR(nRemarkSize >= sizeof(pTongData->m_Memberlist[nMemberIndex]->szRemark));

    dwID             = pTongData->m_Memberlist[nMemberIndex]->dwID;
    nGroupID         = pTongData->m_Memberlist[nMemberIndex]->nGroupID;
    nJoinTime        = pTongData->m_Memberlist[nMemberIndex]->nJoinTime;
    nLastOfflineTime = pTongData->m_Memberlist[nMemberIndex]->nLastOfflineTime;
    nSalary          = pTongData->m_Memberlist[nMemberIndex]->nSalary;

    strncpy(szRemark, pTongData->m_Memberlist[nMemberIndex]->szRemark, nRemarkSize);
    szRemark[nRemarkSize - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

int GetTongMemorabiliaCount(PARSEHANDLE hParser, int& nCount)
{
    int        nResult      = false;
    KTongData* pTongData    = (KTongData*)hParser;

    KG_PROCESS_ERROR(pTongData);

    nCount = (int)pTongData->m_Memorabilia.size();

    nResult = true;
Exit0:
    return nResult;
}

int GetTongMemorabiliaInfo(PARSEHANDLE hParser, int nIndex, unsigned long& ulID, time_t& nTime, char szDescription[], int InputnDescriptionSize)
{
    int        nResult      = false;
    KTongData* pTongData    = (KTongData*)hParser;

    KG_PROCESS_ERROR(pTongData);
    KG_PROCESS_ERROR(nIndex >=0 && nIndex < (int)pTongData->m_Memorabilia.size());
    KG_PROCESS_ERROR(InputnDescriptionSize >= TONG_MEMORABILIA_RECORD_LEN);

    ulID  = pTongData->m_Memorabilia[nIndex].dwID;
    nTime = pTongData->m_Memorabilia[nIndex].nTime;

    strncpy(szDescription, pTongData->m_Memorabilia[nIndex].szDescription, sizeof(pTongData->m_Memorabilia[nIndex].szDescription));
    szDescription[InputnDescriptionSize - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}


PARSEHANDLE PaserStatData(unsigned char* pbyData, size_t uDataLen)
{
    int nResult  = false;
    int nRetCode = false;

    KStatDataAnalyser* pStatData = new KStatDataAnalyser;
    KG_PROCESS_ERROR(pStatData);

    nRetCode = pStatData->PaserStatData(pbyData, uDataLen);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete pStatData;
        pStatData = NULL;
    }
    return (PARSEHANDLE)pStatData;
}

int GetStatDataMaxUsedID(PARSEHANDLE hParser, int& nMaxUsedID)
{
    int                 nResult     = false;
    KStatDataAnalyser*  pStatData   = (KStatDataAnalyser*)hParser;

    KG_PROCESS_ERROR(pStatData);

    nMaxUsedID = pStatData->m_nMaxUsedID;

    nResult = true;
Exit0:
    return nResult;
}

int GetStatDataValueFromID(PARSEHANDLE hParser, int nID, long long& nValue)
{
    int                 nResult     = false;
    KStatDataAnalyser*  pStatData   = (KStatDataAnalyser*)hParser;

    KG_PROCESS_ERROR(pStatData);
    KG_PROCESS_ERROR(nID > 0 && nID < MAX_STAT_DATA_COUNT);

    nValue = pStatData->m_StatValues[nID - 1];

    nResult = true;
Exit0:
    return nResult;
}

PARSEHANDLE PaserPQTableData(unsigned char* pbyData, size_t uDataLen)
{
    int nResult  = false;
    int nRetCode = false;

    KPQDBTableData* pPQTableData = new KPQDBTableData;
    KG_PROCESS_ERROR(pPQTableData);

    nRetCode = pPQTableData->PaserPQDBTableData(pbyData, uDataLen);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete pPQTableData;
        pPQTableData = NULL;
    }
    return (PARSEHANDLE)pPQTableData;
}

int GetPQTabeDataInfo(PARSEHANDLE hParser, unsigned char& byVersion, unsigned long& dwPQID, 
                      unsigned long& dwPQTemplateID, time_t& nBeginTime, 
                      int nPQValue[], int nPQValueSize,  int& nNeedPQValueSize,
                      unsigned char& bFinished, unsigned char& byOnwerType, 
                      unsigned long& dwOnwerID1, unsigned long& dwOnwerID2, unsigned long& dwOnwerID3
)
{
    int                 nResult         = false;
    KPQDBTableData*     pPQTableData    = (KPQDBTableData*)hParser;

    KG_PROCESS_ERROR(pPQTableData);
    nNeedPQValueSize = sizeof(pPQTableData->m_PQ_DB_Data.nPQValue);
    KG_PROCESS_ERROR(nPQValueSize >= PQ_PARAM_DB_COUNT);

    byVersion           = pPQTableData->m_PQ_DB_Data.byVersion;
    dwPQID              = pPQTableData->m_PQ_DB_Data.dwPQID;
    dwPQTemplateID      = pPQTableData->m_PQ_DB_Data.dwPQTemplateID;
    nBeginTime          = pPQTableData->m_PQ_DB_Data.nBeginTime;
    memcpy(nPQValue, pPQTableData->m_PQ_DB_Data.nPQValue, sizeof(pPQTableData->m_PQ_DB_Data.nPQValue));
    bFinished           = pPQTableData->m_PQ_DB_Data.bFinished;
    byOnwerType         = pPQTableData->m_PQ_DB_Data.byOnwerType;
    dwOnwerID1          = pPQTableData->m_PQ_DB_Data.dwOnwerID1;
    dwOnwerID2          = pPQTableData->m_PQ_DB_Data.dwOnwerID2;
    dwOnwerID3          = pPQTableData->m_PQ_DB_Data.dwOnwerID3;
   
    nResult = true;
Exit0:
    return nResult;
}

PARSEHANDLE PaserMentorTableData(unsigned char* pbyData, size_t uDataLen)
{
    int nResult  = false;
    int nRetCode = false;

    KMentorTableData* pMentorTableData = new KMentorTableData;
    KG_PROCESS_ERROR(pMentorTableData);

    nRetCode = pMentorTableData->PaserMentorInfo(pbyData, uDataLen);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        delete pMentorTableData;
        pMentorTableData = NULL;
    }
    return (PARSEHANDLE)pMentorTableData;
}

int GetMentorInfo(PARSEHANDLE hParser, time_t& refnCreateTime, 
                          int& refnMentorValue, time_t& refnEndTime,
                          unsigned char& refucState, unsigned char& byOriginalALevel
)
{
    int                 nResult             = false;
    KMentorTableData*   pMentorTableData    = (KMentorTableData*)hParser;

    KG_PROCESS_ERROR(pMentorTableData);
    
    refnCreateTime      = pMentorTableData->m_KMentorTableData.nCreateTime;
    refnMentorValue     = pMentorTableData->m_KMentorTableData.nMentorValue;
    refnEndTime         = pMentorTableData->m_KMentorTableData.nEndTime;
    refucState          = pMentorTableData->m_KMentorTableData.byState;
    byOriginalALevel    = pMentorTableData->m_KMentorTableData.byOriginalALevel;

    nResult = true;
Exit0:
    return nResult;
}

void EndMailBoxParse(PARSEHANDLE& hHandle)
{
    if (hHandle)
    {
        delete (KMailBoxInfoData *)hHandle;
        hHandle = NULL;
    }
}

void EndFellowshipParse(PARSEHANDLE& hHandle)
{
    if (hHandle)
    {
        delete (KFellowShipData *)hHandle;
        hHandle = NULL;
    }
}

void EndAuctionParse(PARSEHANDLE& hHandle)
{
    if (hHandle)
    {
        delete (KAuctionData *)hHandle;
        hHandle = NULL;
    }
}

void EndTongParse(PARSEHANDLE& hHandle)
{
    if (hHandle)
    {
        delete (KTongData *)hHandle;
        hHandle = NULL;
    }
}

void EndStatDataParse(PARSEHANDLE& hHandle)
{
    if (hHandle)
    {
        delete (KStatDataAnalyser*)hHandle;
        hHandle = NULL;
    }
}

void EndPQDataParse(PARSEHANDLE& hHandle)
{
    if (hHandle)
    {
        delete (KPQDBTableData *)hHandle;
        hHandle = NULL;
    }
}

void EndMentorPares(PARSEHANDLE& hHandle)
{
    if (hHandle)
    {
        delete (KMentorTableData *)hHandle;
        hHandle = NULL;
    }
}
