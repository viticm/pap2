#ifndef _KLEVEL_UP_LIST_H_
#define _KLEVEL_UP_LIST_H_
#include <map>
#include "Global.h"


struct KLevelUpData
{
    int		nExperience;
	int		nStrength;
	int		nAgility;
	int		nVigor;
	int		nSpirit;
    int     nSpunk;
	int		nMaxLife;
	int		nMaxMana;
    int     nMaxRage;
    int     nMaxStamina;
    int     nMaxQi;
    int     nWalkSpeed;
    int     nRunSpeed;
    int     nSwimSpeed;
    int     nJumpSpeedXY;
    int     nJumpSpeedZ;
    int     nHeight;
    int     nLifeReplenish;
    int     nLifeReplenishPercent;
    int     nManaReplenish;
    int     nManaReplenishPercent;
    int     nRageReplenish;
    int     nRageReplenishPercent;
    int     nStaminaReplenish;
    int     nPhysicsAttackHit;
    int     nSolarMagicHit;
    int     nNeutralMagicHit;
    int     nLunarMagicHit;
    int     nPoisonMagicHit;
    int     nPhysicsCriticalStrike;
    int     nSolarCriticalStrike;
    int     nNeutralCriticalStrike;
    int     nLunarCriticalStrike;
    int     nPoisonCriticalStrike;
	//双倍经验相关信息
	int		nExpPerMinute;
	int		nExpPerEnergy;
	int		nMaxDoubleExp;
    //空手时拳头的攻击速度,与新手技能有关
    int     nNoneWeaponAttackSpeedBase; 
    int     nMaxPhysicsDefence;
};

class KLevelUpList
{
public:
	KLevelUpList(void);
	~KLevelUpList(void);

	BOOL Init(void);
	BOOL UnInit(void);

	KLevelUpData* GetLevelUpData(int nType, int nLevel);

private:

    BOOL LoadLevelData(char szFileName[], int nType);
    BOOL LoadLine(ITabFile* piTabFile, int nLine, KLevelUpData& rLevelData, const KLevelUpData& crDefault);

    typedef std::map<LONG, KLevelUpData> KLEVEL_DATA_LIST;
    KLEVEL_DATA_LIST m_LevelData;
};

#endif	//_KLEVEL_UP_LIST_H_