#ifndef _KPATROL_PATH_H_
#define _KPATROL_PATH_H_

#include <vector>
#include "Global.h"

struct KPatrolNode
{
	int nPatrolPointX;
	int nPatrolPointY;
	int nRestFrame;
	int nFaceTo;
	int nAnimationID;
	int nAniLoop;
	int bIsRun;
	char szScriptName[MAX_PATH];
};

class KPatrolPath
{
public:
	KPatrolPath(void);
	~KPatrolPath(void);

	BOOL Init(const char* szFileName);
	BOOL UnInit();

	BOOL GetPoint(int nPointIndex, OUT KPatrolNode& PatrolNode);
	int GetPointCount(void);

	BOOL Save();
	BOOL AddPoint(const KPatrolNode& rPatrol);

	char    m_szTabFileName[MAX_PATH];
    int     m_nWalkSpeed;
    int     m_nOrderID;

private:
	typedef std::vector<KPatrolNode> KPATROL_PATH;
	KPATROL_PATH    m_vecPatrolPath;
	KPatrolNode     m_DefaultPatrolNode;

	BOOL LoadFromTemplate(ITabFile* pTabFile, int nIndex, OUT KPatrolNode& rPatrolNode);
	BOOL SaveToTemplate(ITabFile* pTabFile, int nIndex, IN KPatrolNode& rPatrolNode);
};
#endif	//_KPATROL_PATH_H_
