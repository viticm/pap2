/************************************************************************/
/* Copyright : Kingsoft 2008										    */
/* Author	 : Chen Jie												    */
/* History	 :															*/
/*		2008.08.07	Create												*/
/* Comment	 :															*/
/************************************************************************/
#ifndef _KBUFF_MANAGER_H_
#define _KBUFF_MANAGER_H_

#include <map>
#include "KLRUCacheMap.h"
#include "../../Source/Common/SO3World/Src/KBuff.h"


#define MAX_BUFF_REPRESENT_ID       (1 << 8)
#define INVALID_BUFF_REPRESENT_ID   0
#define MAX_BUFF_DECAY_TYPE         16
#define MAX_DECAY_COUNT             3

enum KBUFF_TYPE
{
    btpInvalid = 0,

    btpStun,
    btpDaze,
    btpHalt,
    btpFear,

    btpTotal
};

enum KBUFF_PERSIST_SHIELD_TYPE
{
    pmsInvalid  = 0,

    pmsNone,

    pmsGlobalMana,
    pmsPhysicsMana,
    pmsSolarMana,
    pmsNeutralMana,
    pmsLunarMana,
    pmsPoisonMana,

    pmsGlobalAbsorb,
    pmsPhysicsAbsorb,
    pmsSolarAbsorb,
    pmsNeutralAbsorb,
    pmsLunarAbsorb,
    pmsPoisonAbsorb,

    pmsTotal
};

enum KBUFF_PERSIST_ON_FIGHT
{
    pofInvalid  = 0,

    pofAny,
    pofFight,
    pofNotFight,

    pofTotal
};

enum KBUFF_PERSIST_ON_HORSE
{
    pohInvalid  = 0,

    pohAny,
    pohHorse,
    pohNotHorse,

    pohTotal
};

enum KBUFF_PERSIST_ON_TERRAIN
{
    potInvalid  = 0,

    potAny,
    potLand,
    potWater,

    potTotal
};

struct KBUFF_DECAY_INFO
{
    int nDecayFrame;
    int nImmunityFrame;
};

class KBuffManager
{
public:
    BOOL        Init();
    BOOL        UnInit();

    KBuff*      GetBuff_RAW(DWORD dwID, int nLevel);
    KBuff*      GetBuff(const KBUFF_RECIPE_KEY& crBuffRecipeKey);

    BOOL        GetBuffDecayInfo(DWORD dwDecayType, int* pnRetDecayFrame, int* pnRetImmunityFrame);

    KBUFF_RECIPE_INFO*  GetBuffRecipeInfo(DWORD dwRecipeKey);

private:
    BOOL        LoadBuffInfo(const char* szTabFile);
    BOOL        LoadBuffRecipeInfo(const char* szTabFile);
    BOOL        LoadBuffRecipeLine(ITabFile* piTabFile, int nIndex, DWORD* pdwRecipeKey, KBUFF_RECIPE_INFO* pValue, const KBUFF_RECIPE_INFO& crDefault);

    BOOL        LoadBuffDecayInfo(const char* cszTabFile);

    BOOL        CheckBuffExclude();
    void        CleanUp();

private:
    typedef std::map<unsigned long long, KBuff>     KBUFF_INFO_MAP;
    typedef std::map<DWORD, KBUFF_RECIPE_INFO>      KBUFF_RECIPE_INFO_MAP;
    typedef KLRUCahceMap<KBUFF_RECIPE_KEY, KBuff, KBuffFreeAttribEraseFunc2<KBUFF_RECIPE_KEY, KBuff> > KBUFF_RECIPE_CACHE;

    KBUFF_INFO_MAP          m_BuffInfoMap; 
    KBUFF_RECIPE_INFO_MAP   m_BuffRecipInfoMap;
    KBUFF_RECIPE_CACHE      m_BuffRecipeCache;

    KBUFF_DECAY_INFO        m_BuffDecayTable[MAX_BUFF_DECAY_TYPE];

    DWORD                   m_dwBuffSNGenerator;

};

#endif	//_KBUFF_MANAGER_H_
