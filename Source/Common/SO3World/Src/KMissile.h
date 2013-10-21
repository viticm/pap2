/************************************************************************/
/* 子弹			                                                        */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.31	Create												*/
/************************************************************************/
#ifndef _KMissile_H_
#define _KMissile_H_

#include "Global.h"
#include "KSceneObject.h"
#include "KCharacter.h"

class KSkill;
class KScene;
class KRegion;
struct KCell;

class KMissile : public KSceneObject
{
//子弹属性
public:
	int			m_nFrames;		//帧数

	int			m_nDamageRadius;		//伤害半径
	int			m_nStartAngle;
	int			m_nEndAngle;

//其他属性
public:
	KCharacter*	m_pLauncher;			//发射者指针
	DWORD		m_dwLauncherID;			//发射者ID
	DWORD		m_dwDice;				//发射者的掷骰子数值（1－100）
	int			m_nAttackRating;		//发射者在发射子弹时的命中率
										//可能和命中目标时发射者的命中率不同，所以记录在子弹上

	KSkill*		m_pSkill;				//发出子弹的技能
	KSkill*		m_pSubSkill;			//子弹带的子技能
	int			m_nFlyEventInterval;	//飞行中调用子技能的周期，周期为零表示无效
	int			m_nFlyEventFrame;		//下次调用子技能的帧数，基准是子弹自身的存在帧数
	BOOL		m_bCollideEvent;		//是否需要在碰撞时调用子技能
	BOOL		m_bVanishEvent;			//是否需要在消亡时调用子技能

	SCENE_OBJ_RELATION_TYPE	m_AllowRelation;	//子弹允许作用的关系
public:
	KMissile(void);
	~KMissile(void);

	BOOL Init(void);
	BOOL UnInit(void);

	BOOL Activate(void);

	//跟踪弹根据目标校正方向
	//void AdjustDirection(void);

	//子弹碰撞事件，由子弹调用
	BOOL OnCollided(KMissile* pMissile);
private:
	BOOL EnumDamageRange();
	BOOL EnumObject(KRegion *pRegion);
	BOOL InDamageRange(KSceneObject *pSceneObject);

	//子弹消亡事件
	void OnVanish(void);
	//子技能事件
	BOOL OnEvent(void);
};
#endif	//_KMissile_H_
