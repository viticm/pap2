#ifndef _K_CHARACTER_ACTION_LIST_
#define _K_CHARACTER_ACTION_LIST_

#include <map>
#include "Global.h"

#define MAX_FUNCTION_NAME_LEN 64

struct KCharacterAction 
{
	DWORD	dwID;
	BOOL	bIsLoop;
	DWORD	dwRange;
	DWORD	dwAnimationID;
	char	szScriptFuncName[MAX_FUNCTION_NAME_LEN];
};

class KCharacterActionList
{
public:
	BOOL Init();
	BOOL UnInit();

	KCharacterAction* GetAction(DWORD dwID);
	BOOL LoadLine(int nLine, ITabFile* piTabFile, KCharacterAction& DefaultCharacterAction, KCharacterAction& CharacterAction);

private:
	typedef std::map<DWORD, KCharacterAction> KACTION_LIST;
	KACTION_LIST m_mapActionList;
};

#endif //_K_CHARACTER_ACTION_LIST_
