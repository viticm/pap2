////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_RoleInfo.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-26  11:05:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_ROLE_INFO_H_
#define _KG_ROLE_INFO_H_

#include "KRoleDBDataDef.h"
#include "SO3GlobalDef.h"
#include "KItemDef.h"
#include "KSkillDef.h"

class KG_RoleInfo
{
public:
    KG_RoleInfo();
    ~KG_RoleInfo();

    int Init(
        const unsigned char cbyBaseInfo[],   unsigned uBaseInfoSize,
        const unsigned char cbyExtendInfo[], unsigned uExtendInfoSize, void *pvContext
    );
    int UnInit(void *pvContext);

    int GetLevel(int *pnRetLevel) const;

    int GetForceID(int *pnRetForceID) const;

    int GetCampID(int *pnRetCampID) const;

    int IsHaveSkill(int nSkillID) const;
    int GetSkillLevel(int nSkillID, int *pnRetSkillLevel) const;

    int GetTotalMoney(int *pnRetTotalMoney) const;

    int GetItemInfo(int nBoxID, int nPosID, int *pnRetEmptyFlag, int *pnRetTypeID, int *pnRetItemID) const;

    int GetUsedTrain(int *pnRetUsedTrain) const;

    int GetForceReputeLevel(int nForceID, int *pnRetReputeLevel) const;
    int GetForceExtRepute(int nForceID, int *pnRetExtRepute) const;

    int GetAchievementRecord(int *pnRetAchievementRecord) const;

private:
    int _ParseBaseInfo(const unsigned char cbyBaseInfo[], unsigned uBaseInfoSize);
    int _ParseExtendInfo(const unsigned char cbyExtendInfo[], unsigned uExtendInfoSize);
    int _ParseSkillInfo(const unsigned char cbyInfo[], unsigned uInfoSize);
    int _ParseItemInfo(const unsigned char cbyInfo[], unsigned uInfoSize);
    int _ParseStateInfo(const unsigned char cbyInfo[], unsigned uInfoSize);
    int _ParseForceInfo(const unsigned char cbyInfo[], unsigned uInfoSize);
    int _ParseAchievementInfo(const unsigned char cbyInfo[], unsigned uInfoSize);

private:
    struct _KG_SKILL_INFO
    {
        int   nLevel;
        int   nExp;
    };
    typedef std::map<int, _KG_SKILL_INFO>   _KG_SKILL_INFO_MAP; // map the skill ID with skill info.

    enum _KG_ITEM_TYPE
    {
        ITEM_EMPTY = 0,
        ITEM_COMMON_ITEM,
        ITEM_CUSTOM_EQUI,
    };

    struct _KG_ITEM_INFO
    {
        _KG_ITEM_TYPE eItemType;
        union
        {
            KCOMMON_ITEM_DATA   CommonItem;
            KCUSTOM_EQUI_DATA   CustomEqui;
        }Data;
    };
    static const unsigned _KG_ITEM_BOX_COUNT    = 12;
    static const unsigned _KG_ITEM_BOX_SIZE     = 32;
    
private:
    KRoleBaseInfo               m_BaseInfo;    
    _KG_SKILL_INFO_MAP          m_SkillInfoMap;
    int                         m_nTotalMoney;
    _KG_ITEM_INFO               m_ItemInfoArray[_KG_ITEM_BOX_COUNT][_KG_ITEM_BOX_SIZE];
    KROLE_STATE_INFO            m_StateInfo;
    KFORCE_DB_DATA::FORECE_INFO m_ForceInfoArray[MAX_FORCE_COUNT];
    int                         m_nAchievementRecord;
};

#endif  // _KG_ROLE_INFO_H_
