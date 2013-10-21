#ifndef _KBUFFLISTNEW_H_
#define _KBUFFLISTNEW_H_

#include <list>
#include <vector>
#include "KBuff.h"
#include "KSkill.h"

#ifndef MAX_LIST_COUNT
#define MAX_LIST_COUNT 256
#endif

struct KBUFF_LIST_NODE
{
    BOOL                    bDeleteFlag;
    int                     nIndex;
    int                     nStackNum;
    int                     nNextActiveFrame;
    int                     nLeftActiveCount;
    KBuffRecipePointer      BuffRecipePointer;

#ifdef _SERVER
    KSkillRecipePointer     SkillRecipePointer;
    DWORD                   dwSkillSrcID;
    int                     nSkillSrcLevel;
    int                     nCustomValue;           // 这个字段本身对Buff是无用的，主要是提供给策划保存一些数据用。

    int                     nPhysicsDamage;         // 外功伤害点数
    int                     nSolarDamage;           // 阳性内功伤害
    int                     nNeutralDamage;         // 中性内功伤害
    int                     nLunarDamage;           // 阴性内功伤害
    int                     nPoisonDamage;          // 毒性伤害点数
    int                     nTherapy;               // 治疗能力
    int                     nStealLifePercent;
    int                     nStealManaPercent;
    int                     nCriticalStrike;

    int                     nBaseThreatCoefficient;
    int                     nDamageThreatCoefficient;
    int                     nTherapyThreatCoefficient;
#endif

#ifdef _SERVER
    DECLARE_LUA_CLASS(KBUFF_LIST_NODE);
    DECLARE_LUA_STRUCT_INTEGER(Index, nIndex);
    DECLARE_LUA_STRUCT_INTEGER(StackNum, nStackNum);
    DECLARE_LUA_STRUCT_INTEGER(NextActiveFrame, nNextActiveFrame);
    DECLARE_LUA_STRUCT_INTEGER(LeftActiveCount, nLeftActiveCount);
    DECLARE_LUA_STRUCT_DWORD(SkillSrcID, dwSkillSrcID);
    DECLARE_LUA_STRUCT_INTEGER(CustomValue, nCustomValue);
    int getID();
    int getLevel();

    int LuaGetIntervalFrame(Lua_State* L);
    int LuaGetEndTime(Lua_State* L);
#endif
};

#pragma pack(1)
struct KBUFF_TRANSFER_DATA
{
    int                 nIndex;
    KBUFF_RECIPE_KEY    RecipeKey;
    BYTE                byStackNum;
    int                 nEndFrame;
};

struct KBUFF_DB_DATA
{
    BYTE byCount;
    struct KBUFF_DB_ITEM 
    {
        KBUFF_RECIPE_KEY    RecipeKey;
        DWORD               dwSkillSrcID;
        BYTE                bySkillSrcLevel;
        BYTE                byStackNum;
        int                 nLeftFrame;
        int                 nCustomValue;
    } Items[0];
};
#pragma pack()

class KBuffList
{
public:
    KBuffList(void);
    ~KBuffList(void);

    BOOL Init(KCharacter *pOwner);
    void UnInit();

#ifdef _SERVER
    BOOL Load(BYTE* pbyData, size_t uDataLen);
    BOOL Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
#endif

    void Activate();

    SKILL_RESULT_CODE   CanAddBuff(DWORD dwSkillSrcID, KBuff* pBuffInfo);
    SKILL_RESULT_CODE   CallBuff(DWORD dwCasterID, int nCasterLevel, KBUFF_RECIPE_KEY BuffRecipeKey, int nCount, int nFristActiveFrame);
#ifdef _CLIENT
    BOOL                UpdateBuff(KBUFF_LIST_NODE &rBuffNode);
#endif
    KBUFF_LIST_NODE*    GetBuff(DWORD dwBuffID, int nBuffLevel);
    BOOL GetBuffMostStackNum(DWORD dwBuffID, int* pnRetBuffLevel, int* pnRetStackNum);
#ifdef _SERVER
    BOOL ConsumeBuff(DWORD dwCasterID, DWORD dwBuffID);
    BOOL SetBuffNextActiveFrame(int nIndex, int nNextActiveFrame);
    BOOL SetBuffLeftActiveCount(int nIndex, int nLeftActiveCount);
#endif

private:
    SKILL_RESULT_CODE   AddBuff(KBUFF_LIST_NODE &rBuffNode);
    BOOL                DelBuff(KBUFF_LIST_NODE &rBuffNode);
    BOOL                DelGroupBuff(KBUFF_LIST_NODE &rBuffNode);
    BOOL                CleanBuff();

public:
#ifdef _SERVER
    BOOL DelSingleBuff(DWORD dwID, int nLevel);
    BOOL DelSingleGroupBuff(DWORD dwID, int nLevel);

    BOOL CancelBuff(int nIndex);

    BOOL DetachSingleBuff(int nDetachType, int nIntensity);
    BOOL DetachMultiBuff(int nDetachType, int nIntensity);

    BOOL DelSingleBuffByFunctionType(int nFunctionType, int nIntensity);
    BOOL DelMultiBuffByFunctionType(int nFunctionType, int nIntensity);

    BOOL CleanBuffByType(int nBuffType);
    BOOL CleanBuffByCaster(DWORD dwCaster);

    BOOL CleanBuffByCanCancel(BOOL bCanCancel);
#endif

    BOOL GetTransferData(BYTE byBuffer[], size_t uBufferSize, size_t* puUsedSize);

#ifdef _CLIENT
    BOOL  UpdateBuffData(BYTE byBuffer[], DWORD dwBufferSize);
#endif
    void CleanBuffByPersist();

private:
    BOOL CheckBuffActive();

    void DelRemovedBuff();
    BOOL CallRemoveBuffScript(KBUFF_LIST_NODE &rBuffNode);

#ifdef _SERVER
    BOOL ProcessUniqueBuff(KBUFF_LIST_NODE &rBuffInfo);
#endif

    void UpdateRepresentState();

    BOOL CheckImmunity(int nFunctionType);

private:
    KCharacter*     m_pOwner;
    DWORD           m_dwRepresentState;
#ifdef _SERVER
    int             m_nIndexAlloc;
#endif

    typedef std::list<KBUFF_LIST_NODE> KBUFF_LIST;
    KBUFF_LIST      m_BuffList;

public:
#ifdef _CLIENT
    int _LuaGetBuffList(Lua_State* L);
#endif
};


#endif //_KBUFFLISTNEW_H_
