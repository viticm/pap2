#ifndef _KTHREAT_LIST_H_
#define _KTHREAT_LIST_H_

#include "Engine/KNode.h"
#include "Global.h"

//#define MAX_TARGET_COUNT	4

#define CHECK_THREAT_INTERVAL		((GAME_FPS) * 2)
#define THREAT_FADE_FRAME			((GAME_FPS) * 1)
#define KEEP_THREAT_RANGE			(CELL_LENGTH * REGION_GRID_WIDTH * 2)

class KCharacter;
class KSkill;
class KPlayer;

enum THREAT_TYPE
{
	thtInvalid = -1,

	thtMainThreat = 0,
	thtDirectThreat,
	thtIndirectThreat,
	thtDirectAssistThreat,
	thtIndirectAssistThreat,
	thtSpecialThreat,

	thtTotal
};

struct KThreatLiteNode : KNode
{
	DWORD			dwCharacterID;
	//KThreatLiteNode* pPairNode;
};


//客户端用仇恨表
class KThreatList
{
public:
	KThreatList(void);
	~KThreatList(void);

	BOOL Init(KCharacter* pSelf);
	BOOL UnInit(void);

	BOOL ModifyThreat(DWORD dwCharacterID);
	BOOL ModifyThreaten(DWORD dwCharacterID);

	BOOL ClearThreat(DWORD dwCharacterID);
	BOOL ClearThreaten(DWORD dwCharacterID);

	BOOL IsInThreatList(DWORD dwCharacterID);
	BOOL IsInThreatenList(DWORD dwCharacterID);
private:
	KList		m_ThreatList;
	KList		m_ThreatenList;
	KCharacter* m_pSelf;

	BOOL ClearThreat(KThreatLiteNode* pNode);
};

#endif	//_KTHREAT_LIST_H_