#ifndef	_KAI_BASE_
#define _KAI_BASE_

#include "Global.h"
#include "SO3Result.h"
#include "KPathFinder.h"

class KSkill;
class KCharacter;
class KNpc;
class KPlayer;
class KTarget;
class KNpcTeam;
class KPatrolPath;
class KAIParamTemplate;

#define AI_IMMEDIATELY_CAST_EXP		(INT_MAX)   //技能的期望值达到时立即释放

//跟随相关
#define AI_FOLLOW_RANGE				2048		//超出后停止跟随
#define AI_FOLLOW_RUN				512			//超出后使用奔跑，否则走路
#define AI_FOLLOW_KEEPAWAY			128			//跟随时保持的距离

#define DEFAULT_IDLE_FRAME          (1024 * 1024)
#define DEFAULT_ESCAPE_FRAME        -1
#define DEFAULT_ESCAPE_RANGE        -1

#define CALL_HEAL_LIMIT             0.7
#define CALL_HEAL_MAX_RATE          1024
#define CALL_HEAL_RATE              1000
#define CALL_HEAL_RANGE             2048
#define CALL_HEAL_INTERVAL          (5 * GAME_FPS)

#define MAX_SKILL_RATE              1024

#define MAX_SELECT_TARGET_COUNT     8   //技能选择释放的时候会考虑8个单位

//#define ADD_RATE_PER_TARGET_IN_AOE 128 //每个仇恨目标增加的释放AOE的几率

#define AI_TRACE(__STRING__)	


class KAIBase
{
public:
	KAIBase(void);
	~KAIBase(void);

	virtual BOOL Init(KCharacter* pSelf);
	virtual BOOL UnInit(void);

    KNpcTeam* GetNpcTeam();

private:
	BOOL SetAIState(KAI_STATE eNewState);
	AI_TYPE GetAIType(void);
	void SetOriginPoint(int nX, int nY, int nZ, int nDir);
	BOOL GetOriginPoint(int* pnRetX, int* pnRetY, int* pnRetZ, int* pnRetDir);
	BOOL SetAIInterval(int nAIInterval);
	BOOL SetAIInterval(double fRate);
	KAIParamTemplate* GetAIParam();
	BOOL LoadParamFromTemplate();

#if defined(_SERVER)
	BOOL SetNpcTeam(KNpcTeam* pNpcTeam);
	BOOL SetPatrolPath(int nPatrolPathID, int nNextPointIndex = 0);
#endif //_SERVER

protected:
	virtual void Activate(void);

	// -------------- AI 状态函数 ------------------------------
	virtual void OnIdle();
	virtual void OnWander();
	virtual void OnPartol();
	virtual void OnAlert();
	virtual void OnFollow();
	virtual void OnPursuit();
	virtual void OnKeepAway();
	virtual void OnEscape();
	virtual void OnReturn();
	virtual void OnWait();

public:
	// -------------- AI 事件函数 ------------------------------
	virtual BOOL OnTryMove(int nRelativeZ, BOOL bIsWater);
	virtual void OnAttacked(KCharacter* pAttacker, int nDamage);
	virtual void OnClearThreat(KCharacter* pCharater);
	virtual void OnPathResult(BOOL bSucceed);

public:
	// ------------- 状态切换函数 -----------------------------
	void DoIdle(int nIdleFrame = DEFAULT_IDLE_FRAME);
	void DoWander();
	void DoPatrol();
	void DoAlert();
	void DoFollow();
	void DoPursuit();
	void DoKeepAway();
	void DoEscape();
	void DoReturn();
	void DoWait();

#ifdef _SERVER
	// ------------- 技能选择评估函数 -----------------------------
protected:
	virtual void InitSkillCD();
	virtual void CheckAttackTarget();

	virtual int GetSkillExp_PeriodTarget(DWORD dwSkillIndex, KTarget* pTarget);
	virtual int GetSkillExp_PeriodSelf(DWORD dwSkillIndex, KTarget* pTarget);
	virtual int GetSkillExp_PeriodFriend(DWORD dwSkillIndex, KTarget* pTarget);
	virtual int GetSkillExp_PassiveRespond(DWORD dwSkillIndex, KTarget* pTarget);
	virtual int GetSkillExp_ChannelingBreak(DWORD dwSkillIndex, KTarget* pTarget);
	virtual int GetSkillExp_TargetCount(DWORD dwSkillIndex, KTarget* pTarget);
	virtual int GetSkillExp_SelfHP(DWORD dwSkillIndex, KTarget* pTarget);

public:
	int GetSkillExpectation(AI_SKILL_TYPE eSkillType, DWORD dwSkillIndex, KTarget* pTarget);
#endif //_SERVER

public:
	// AI相关参数设定
	union PARAMS
	{
		int	m_nParams[AI_PARAM_COUNT];
		struct _DEFAULT_PARAMS
		{
			int		nSearchRange;
			int		nWanderMaxRange;
			int		nWanderRange;
			int		nWanderDuration;
			int		nFollowAttackRange;
		}DefaultParams;
		struct _CRITTER_PARAMS
		{
			int		nSearchRange;
			int		nWanderMaxRange;
			int		nWanderRange;
			int		nWanderDuration;
			int		nFollowAttackRange;
		}CritterParams;
	}m_Params;

	// ------------- 一些常用工具函数 -----------------------------

protected:
	void GetAdjustPoint(int nSrcX, int nSrcY, int nDistance, int nDir, OUT int& nDstX, OUT int& nDstY); //获取一个点的极坐标偏移点
	void Follow();
	void ClearTarget(void);
#if defined(_SERVER)
	void SaveReturnPoint(void);	
	void Wander(BOOL bForceBack = false);
	void Escape(void);
	void Patrol(void);

	void Alert(KCharacter* pTarget);
	BOOL CallHelp(int nRange);
	BOOL CallHeal(int nRange);
	void TurnToFight(void);
	void TurnToIdle(void);

	void CheckAIEvent(void);
	void FireAIEvent(AI_EVENT_CONDITION eAIEvent);
	BOOL CheckReturn(void);
	void CheckCallHeal(void);
	BOOL CheckWanderRange(int nOriginDistance2, int nWanderRange);
	BOOL CheckAttacked(void);
	BOOL CheckTargetInAlertRange(void);
    BOOL CheckTargetInAttackRange(void);    // 搜索目标,如果搜到,则转入追击状态

	//for fight
    void KeepAttackRange(KCharacter* pTarget, KSkill* pSkill);
    void KeepAttackRangeAStar(KCharacter* pTarget, KSkill* pSkill); // A*寻路临时代码
	void KeepWanderRange(int nRange);
#endif	//_SERVER

public:
	union{
		struct _IDLE_DATA
		{
			int nIdleFrameCount;		//Idle时间计数器
		} m_IdleData;

		struct _WANDER_DATA
		{
			int nWanderFrame;           //闲逛超时
		} m_WanderData;

		struct _PARTOL_DATA
		{
			int nDummy;
		} m_PartolData;

		struct _ALERT_DATA
		{
			int nAlartFrame;			//警戒超时攻击计数器
		} m_AlertData;

		struct _FOLLOW_DATA
		{
			DWORD dwFollowTargetID;		//当前正在跟随的目标
		} m_FollowData;

		struct _PURSUIT_DATA
		{
			int nTurnToFightFrame;		//进入战斗的计数器
			int nNpcSkillCommonCD;		//Npc技能公共CD
			int nCallHealFrame;			//呼叫治疗的计数器
			DWORD dwCallByNpcID;		//呼叫者的ID;
			DWORD dwKeepDisDivisor;		//Npc和目标保持距离的除数,初始为1,每次技能释放时如果距离不够,都会加1,成功就重置1
		} m_PursuitData;

		struct _KEEPAWAY_DATA
		{
			int nDummy;
		} m_KeepAwayData;

		struct _ESCAPE_DATA
		{
			int nEscapeIdleFrame;		//每次逃跑中的停顿时间
			int nEscapeFrame;			//逃跑的时间
			int nEscapeRange;			//逃跑的范围
		} m_EscapeData;

		struct _RETURN_DATA
		{	
			int nReturnFrame;			//返回超时的计数器
		} m_ReturnData;

		struct _WAIT_DATA
		{
			int nTimeOutFrame;			// Wait超时时间
		} m_WaitData;
	};

public:
	KAI_STATE		m_eAIState;
	int				m_nAIEventTimer[MAX_AI_EVENT];
protected:
	AI_TYPE				m_eAIType;
	KAIParamTemplate*	m_pAIParam;
	KCharacter*			m_pSelf;

	int				m_nAIInterval;			//AI的执行间隔
	int				m_nNextActiveFrame;		//AI帧数控制

	int				m_nOriginX;				//NPC的原始坐标，从编辑器获取
	int				m_nOriginY;
	int             m_nOriginZ;
	int				m_nOriginDirection;		//NPC原始方向

	int				m_nReturnX;				//NPC进入战斗时保存的坐标
	int				m_nReturnY;
	int				m_nReturnZ;

	int				m_nNextPartolIndex;		//下一个寻路点	

	int				m_nNextIdleActionFrame;	//Npc可能在空闲的时候不断尝试说话,这个是计数器

    int             m_nNextAStarFrame;

	// ------------- AI可变参数 ------------------------------
	KAI_STATE		m_eAIMainState;

#if defined(_SERVER)
	KNpcTeam*		m_pNpcTeam;
	KPatrolPath*	m_pPatrolPath;

    KPathFinder     m_PathFinder;
#endif //_SERVER

	friend class KAIControl;
	friend class KAIEvent;
};

#endif //_KAI_BASE_
