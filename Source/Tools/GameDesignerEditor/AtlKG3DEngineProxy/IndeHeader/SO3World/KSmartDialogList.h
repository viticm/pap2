#ifndef _KSMART_DIALOG_LIST_
#define _KSMART_DIALOG_LIST_

#include "Global.h"
#include <map>

#define MAX_NPC_SENTENCE_LEN 512
#define MAX_DIALOG_PER_ACTION 3

class KNpc;
class KTarget;

struct KNpcDialog
{
	int nIndex;
	NPC_TALK_ACTION_TYPE eNpcTalkAction;
	
	char szDialogText[MAX_DIALOG_PER_ACTION][MAX_NPC_SENTENCE_LEN];
	CHARACTER_TALK_TYPE eDialogType[MAX_DIALOG_PER_ACTION];
	int nDialogRate[MAX_DIALOG_PER_ACTION];
};

class KSmartDialogList
{
public:
	BOOL Init();
	BOOL UnInit();

	const char* GetDialogText(DWORD dwDialogID, NPC_TALK_ACTION_TYPE eAction, DWORD dwIndex);

#ifdef _SERVER
	BOOL FireEvent(KNpc* pNpc, NPC_TALK_ACTION_TYPE eAction, KTarget& rTarget);
#endif //_SERVER

private:
	typedef std::map<int64_t, KNpcDialog> KSMART_DIALOG_LIST;
	KSMART_DIALOG_LIST m_mapSmartDialogList;
};

#endif //_KSMART_DIALOG_LIST_
