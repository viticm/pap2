#ifndef _KNPC_TEAM_
#define _KNPC_TEAM_


#include "KNpcOrderList.h"

#define MAX_NPC_TEAM_COUNT 16

class KCharacter;
class KNpc;

enum NPC_TEAM_MODE
{
	ntmNormal = 0,
	ntmKing,

	ntmTotal
};

struct KPatrolTarget
{
	int nX;
	int nY;
};

class KNpcTeam
{
public:
	KNpcTeam();
	~KNpcTeam();

	void Activate();

	BOOL Init(DWORD dwOrder);
	void UnInit();

	void SetOrder(DWORD dwOrder);
	int	 GetOrder();
	BOOL SetLeader(DWORD dwLeader);
	BOOL SetLeader();				//选择任意一个没有死的Npc作为Leader;
	BOOL SetKing(DWORD dwKing);

	BOOL AddMember(KNpc* pNewMember, int nIndex);
	BOOL RemoveMember(int nIndex);
	KNpc* GetMember(int nIndex);
	BOOL GetMemberPostion(int nIndex, int* pnDistance, int* pnAngle);
	int GetMemberCount() { return m_nMemberCount; }

#ifdef _SERVER
	// 得到一个队友的目标加到自己的仇恨列表里,是谁无所谓
	KCharacter* GetMemberTarget();	
	// 检查Npc的工作是否都完成了
	BOOL CheckNpcWorkDone();

	// 事件
	BOOL OnMemberDeath(int nIndex);
	BOOL OnMemberRevive(int nIndex, OUT int& nX, OUT int& nY, OUT int& nZ);
#endif //_SERVER

	KNpc* m_pLeader;			//小队的领导，所有的成员坐标以领导的坐标做偏移，死亡后被其它成员替换
	KNpc* m_pKing;				//小队的King，用来判断一些特殊的队伍，比如死亡后全队消失

private:
	KNpc* m_pMember[MAX_NPC_TEAM_COUNT];
	int m_nMemberCount;
	const KORDER* m_pOrder;
	int m_nOrder;
};

#endif //_KNPC_TEAM_
