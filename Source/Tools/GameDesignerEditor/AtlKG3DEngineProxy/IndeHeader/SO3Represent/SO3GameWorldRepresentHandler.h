#ifndef SO3_GAME_WORLD_REPRESENT_HANDLER_H
#define SO3_GAME_WORLD_REPRESENT_HANDLER_H

#include "SO3World/Global.h"
#include "SO3World/KTarget.h"

enum
{
	REPRESENT_SKILL_EFFECT_RESULT_BEGIN,

    REPRESENT_SKILL_EFFECT_RESULT_BLOCK_BEGIN = REPRESENT_SKILL_EFFECT_RESULT_BEGIN,
	REPRESENT_SKILL_EFFECT_RESULT_PHYSICS_BLOCK = REPRESENT_SKILL_EFFECT_RESULT_BLOCK_BEGIN,
	REPRESENT_SKILL_EFFECT_RESULT_SOLAR_MAGIC_BLOCK,
	REPRESENT_SKILL_EFFECT_RESULT_NEUTRAL_MAGIC_BLOCK,
	REPRESENT_SKILL_EFFECT_RESULT_LUNAR_MAGIC_BLOCK,
	REPRESENT_SKILL_EFFECT_RESULT_POISON_BLOCK,
    REPRESENT_SKILL_EFFECT_RESULT_BLOCK_END,

    REPRESENT_SKILL_EFFECT_RESULT_MISS = REPRESENT_SKILL_EFFECT_RESULT_BLOCK_END,
	REPRESENT_SKILL_EFFECT_RESULT_HIT,
    REPRESENT_SKILL_EFFECT_RESULT_DODGE,
    REPRESENT_SKILL_EFFECT_RESULT_PARRY,
	REPRESENT_SKILL_EFFECT_RESULT_CRITICAL_STRIKE,

    REPRESENT_SKILL_EFFECT_RESULT_DAMAGE_BEGIN,
	REPRESENT_SKILL_EFFECT_RESULT_PHYSICS_DAMAGE = REPRESENT_SKILL_EFFECT_RESULT_DAMAGE_BEGIN,
	REPRESENT_SKILL_EFFECT_RESULT_SOLAR_MAGIC_DAMAGE,
	REPRESENT_SKILL_EFFECT_RESULT_NEUTRAL_MAGIC_DAMAGE,
	REPRESENT_SKILL_EFFECT_RESULT_LUNAR_MAGIC_DAMAGE,
	REPRESENT_SKILL_EFFECT_RESULT_POISON_DAMAGE,
    REPRESENT_SKILL_EFFECT_RESULT_DAMAGE_END,

    REPRESENT_SKILL_EFFECT_RESULT_SPECIAL_BEGIN = REPRESENT_SKILL_EFFECT_RESULT_DAMAGE_END,
    REPRESENT_SKILL_EFFECT_RESULT_REFLECT_DAMAGE = REPRESENT_SKILL_EFFECT_RESULT_SPECIAL_BEGIN,
	REPRESENT_SKILL_EFFECT_RESULT_THERAPY,
	REPRESENT_SKILL_EFFECT_RESULT_STEAL_LIFE,
    REPRESENT_SKILL_EFFECT_RESULT_ABSORB_DAMAGE,      
	REPRESENT_SKILL_EFFECT_RESULT_SHIELD_DAMAGE,
    REPRESENT_SKILL_EFFECT_RESULT_SPECIAL_END,

	REPRESENT_SKILL_EFFECT_RESULT_END = REPRESENT_SKILL_EFFECT_RESULT_SPECIAL_END,
};

enum REPRESENT_SFX
{
    REPRESENT_SFX_PLAYER_LEVEL_UP,
    REPRESENT_SFX_SKILL_LEVEL_UP,
    REPRESENT_SFX_LEARN_PROFESSION,
    REPRESENT_SFX_FORGET_PROFESSION,
    REPRESENT_SFX_LEARN_RECIPE,
    REPRESENT_SFX_PROFESSION_LEVEL,
    REPRESENT_SFX_PROFESSION_MAX_LEVEL,
    REPRESENT_SFX_REPUTATION_LEVEL_UP,
    REPRESENT_SFX_REPUTATION_LEVEL_DOWN,
    REPRESENT_SFX_FINISH_QUEST,
    REPRESENT_SFX_FINISH_ASSIST_QUEST,

    REPRESENT_SFX_COUNT,
};

class KScene;
class KPlayer;
class KNpc;
class KDoodad;
class KCharacter;
class KSkill;

struct KRepresentSkillEffectResult
{
	DWORD dwSkillID;
    DWORD dwSkillLevel;
	DWORD dwBulletID;

    KCaster	Caster;
    KTargetData Target;

    bool    bImmediately;
    bool	bHitTarget;
    bool	bSkillDodge;
    bool	bCriticalStrike;
    bool    bParry;
    bool    bInsight;

    bool    aSkillBlockFlag[serTotal];
	bool	aSkillResultFlag[serTotal];
	int		aSkillResultValue[serTotal];
};

struct KChainDisplayInfo
{
    DWORD dwSkillID;
    DWORD dwSkillLevel;
    DWORD dwSrcCharacterID;
    DWORD dwDstCharacterID;
};

class IKSO3GameWorldRepresentHandler
{
public:
	virtual~IKSO3GameWorldRepresentHandler();

	//场景相关事件接口
	virtual BOOL OnNewScene(KScene* pScene, int nX, int nY, int nZ, BOOL bEditor) = 0;
	virtual void OnDeleteScene(DWORD dwID) = 0;

    virtual void OnResetFrameData(int nGameLoop, DWORD dwStartTime) = 0;

    //Player和Npc共用事件接口
    virtual void OnAddCharacter(KCharacter* pCharacter) = 0;
    virtual void OnRemoveCharacter(KCharacter* pCharacter, BOOL bKilled) = 0;

    virtual void OnUpdateCharacterFrameData(KCharacter* pCharacter) = 0;
    virtual void OnCharacterDisplayData(KCharacter* pCharacter) = 0;
    virtual void OnCharacterMountHorse(KCharacter* pCharacter, BOOL bMount) = 0;
    virtual void OnCharacterTurnTo(KCharacter* pCharacter, BOOL bTurnImmediately) = 0;

    virtual void OnCharacterPrepareCastSkill(KCharacter* pCharacter, DWORD dwSkillID, DWORD dwSkillLevel) = 0;
    virtual void OnCharacterCastSkill(KCharacter* pCharacter, KTargetData* pTarget, KSkill* pSkill, DWORD dwBulletID, BOOL bPlayAni) = 0;
    virtual void OnCharacterSkillBuffData(KCharacter* pCharacter) = 0;
    virtual void OnCharacterSkillEffectResult(KCharacter* pTarget, KRepresentSkillEffectResult* pResult) = 0;
    virtual void OnCharacterSkillChainDisplay(int nCount, KChainDisplayInfo* pDisplayList) = 0;

    virtual void OnCharacterPlayAnimation(KCharacter* pCharacter, DWORD dwID) = 0;
    virtual void OnCharacterPlayAnimationEnd(KCharacter* pCharacter, DWORD dwID) = 0;

    virtual void OnCharacterLevelUp(KCharacter* pCharacter, REPRESENT_SFX nSFX) = 0;
    virtual void OnCharacterSound(KCharacter* pCharacter, DWORD dwSoundID) = 0;

    virtual void OnCharacterOpenDoodad(KCharacter* pCharacter, KDoodad* pDoodad) = 0;
    virtual void OnCharacterBeginPickDoodad(KCharacter* pCharacter, KDoodad* pDoodad) = 0;
    virtual void OnCharacterEndPickDoodad(KCharacter* pCharacter, KDoodad* pDoodad) = 0;
    virtual void OnCharacterBeginCraftAnimation(KCharacter* pCharacter, int nCraftID) = 0;
    virtual void OnCharacterEndCraftAnimation(KCharacter* pCharacter, int nCraftID) = 0;

    virtual void OnCharacterUpdateTitleMark(KCharacter* pCharacter) = 0;
    virtual void OnCharacterFollowTarget(KCharacter* pCharacter, KCharacter* pTarget) = 0;

    virtual void OnCharacterUpdateRelation(KCharacter* pCharacter) = 0;
    virtual void OnCharacterUpdateAllRelation() = 0;

    //Doodad相关事件接口
	virtual void OnAddDoodad(KDoodad* pDoodad) = 0;
	virtual void OnRemoveDoodad(KDoodad* pDoodad) = 0;

	virtual void OnDoodadIdle(KDoodad* pDoodad) = 0;
	virtual void OnDoodadAfterHit(KDoodad* pDoodad) = 0;
	virtual void OnDoodadBeHit(KDoodad* pDoodad) = 0;
	virtual void OnDoodadLootList(KDoodad* pDoodad) = 0;
    virtual void OnUpdateDoodadQuestEffect() = 0;

    virtual void OnFinishQuest(KPlayer* pPlayer, DWORD dwQuestID, BOOL bAssist) = 0;
    virtual void OnReputationLevelUp(KPlayer* pPlayer, BOOL bLevelUp) = 0;
};

//-------------------------------------------------------------------------------------------------------
inline IKSO3GameWorldRepresentHandler::~IKSO3GameWorldRepresentHandler()
{
}


#endif // SO3_GAME_WORLD_REPRESENT_HANDLER_H
