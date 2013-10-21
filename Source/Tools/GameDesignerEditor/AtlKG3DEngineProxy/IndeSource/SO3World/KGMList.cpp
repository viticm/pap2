#include "stdafx.h"
#include "KGMList.h"

BOOL KGMList::Load()
{
    BOOL        bResult     = false;
    int         nRetCode    = false;
	ITabFile*	piTabFile   = NULL;
    int         nTabHeight  = 0;
	char		szFileName[MAX_PATH];

	snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, "gm.tab");
    szFileName[sizeof(szFileName) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFileName);
	KGLOG_PROCESS_ERROR(piTabFile);

    nTabHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight >= 1);

    for (int i = 1; i <= nTabHeight; i++)
    {
        char szAccount[_NAME_LEN];

        nRetCode = piTabFile->GetString(i, 1, "", szAccount, sizeof(szAccount));
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        m_AccountTable.insert(szAccount);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KGMList::CheckPermission(const char cszAccount[])
{
    KGM_TABLE::iterator it = m_AccountTable.find(cszAccount);
    if (it == m_AccountTable.end())
    {
        return false;
    }
    return true;
}
