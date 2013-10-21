#include "stdafx.h"
#include "KCharacterActionList.h"

#define CHARACTER_ACTION_LIST_TAB_FILE "CharacterAction.tab"

#define CHARACTER_ACTION_ID					"ID"
#define CHARACTER_ACTION_ISLOOP				"IsLoop"
#define CHARACTER_ACTION_RANGE				"Range"
#define CHARACTER_ACTION_ANIMATION_ID		"AnimationID"
#define CHARACTER_ACTION_SCRIPT_FUNC_NAME	"ScriptFuncName"

BOOL KCharacterActionList::Init()
{
    BOOL bResult        = false;
	BOOL bRetCode		= false;
	int nIndex			= 0;
	int nHeight			= 0;
	ITabFile *pTabFile	= NULL;
	KCharacterAction DefaultCharacterAction;
	KCharacterAction CharacterAction;
	char szFileName[MAX_PATH];

	memset(&DefaultCharacterAction, 0, sizeof(KCharacterAction));

	snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, CHARACTER_ACTION_LIST_TAB_FILE);
    szFileName[sizeof(szFileName) - 1] = '\0';

	pTabFile = g_OpenTabFile(szFileName);
	KGLOG_PROCESS_ERROR(pTabFile);

	nHeight = pTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight > 2);

	bRetCode = LoadLine(2, pTabFile, DefaultCharacterAction, DefaultCharacterAction);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (nIndex = 3; nIndex <= nHeight; nIndex++)
	{
		bRetCode = LoadLine(nIndex, pTabFile, DefaultCharacterAction, CharacterAction);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(CharacterAction.dwID > 0);

		m_mapActionList[CharacterAction.dwID] = CharacterAction;
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(pTabFile);
	return bResult;
}

BOOL KCharacterActionList::UnInit()
{
	m_mapActionList.clear();

	return true;
}

KCharacterAction* KCharacterActionList::GetAction(DWORD dwID)
{
	KACTION_LIST::iterator iter;
	KCharacterAction* pResult = NULL;

	iter = m_mapActionList.find(dwID);
	if (iter != m_mapActionList.end())
	{
		pResult = &(iter->second);
	}
Exit0:
	return pResult;
}

BOOL KCharacterActionList::LoadLine(
    int nLine,  ITabFile* piTabFile, KCharacterAction& DefaultCharacterAction, KCharacterAction& CharacterAction
)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	KGLOG_PROCESS_ERROR(piTabFile);

	bRetCode = piTabFile->GetInteger(nLine, CHARACTER_ACTION_ID, DefaultCharacterAction.dwID, (int*)&CharacterAction.dwID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, CHARACTER_ACTION_ISLOOP, DefaultCharacterAction.bIsLoop, (int*)&CharacterAction.bIsLoop);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, CHARACTER_ACTION_RANGE, DefaultCharacterAction.dwRange, (int*)&CharacterAction.dwRange);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nLine, CHARACTER_ACTION_ANIMATION_ID, DefaultCharacterAction.dwAnimationID, (int*)&CharacterAction.dwAnimationID
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nLine, CHARACTER_ACTION_SCRIPT_FUNC_NAME, DefaultCharacterAction.szScriptFuncName, 
        CharacterAction.szScriptFuncName, MAX_FUNCTION_NAME_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}
