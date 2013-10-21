#include "stdafx.h"
#include "KCoolDownList.h"

#define COOL_DOWN_TAB_FILE  "CoolDownList.tab"

KCoolDownList::KCoolDownList()
{

}

BOOL KCoolDownList::Init()
{
    BOOL        bResult                 = false;
    int         nRetCode                = false;
    char        szFileName[MAX_PATH];
    ITabFile   *piTabFile               = NULL;
    int         nWidth                  = 0;
    int         nItemCount              = 0;

    nRetCode = snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, COOL_DOWN_TAB_FILE);
    szFileName[sizeof(szFileName) - 1] = '\0';
    KGLOG_PROCESS_ERROR(nRetCode > 0);

	piTabFile = g_OpenTabFile(szFileName);
	KGLOG_PROCESS_ERROR(piTabFile);

    // Tab 文件共两列，ID, Duration, 当然，可以加额外的第三列作注释
    nWidth = piTabFile->GetWidth();
    KGLOG_PROCESS_ERROR(nWidth >= 2);

    // 第一行是名称(ID, Duration)
    nItemCount = piTabFile->GetHeight() - 1; 
    KGLOG_PROCESS_ERROR(nItemCount >= 0);

    for (int i = 0; i < nItemCount; ++i)
    {
		DWORD dwID      = 0;
        float fDuration = 0.0f;

		nRetCode = piTabFile->GetInteger(2 + i, "ID", 0, (int*)&dwID);
		KGLOG_PROCESS_ERROR(nRetCode && "ID");
        KGLOG_PROCESS_ERROR(dwID > 0);

        // 策划填写数值时填秒数即可，这里转换为游戏帧数
		nRetCode = piTabFile->GetFloat(2 + i, "Duration", 0, &fDuration);
		KGLOG_PROCESS_ERROR(nRetCode && "Duration");
        KGLOG_PROCESS_ERROR(fDuration > 0);

        m_CoolDownTable[dwID] = (int)(fDuration * GAME_FPS);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_CoolDownTable.clear();
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

void KCoolDownList::UnInit()
{
    m_CoolDownTable.clear();
}

int KCoolDownList::GetCoolDownValue(DWORD dwCoolDownID)
{
    int                      nResult = 0;
    KCOOL_DOWN_MAP::iterator it;

    it = m_CoolDownTable.find(dwCoolDownID);
    KGLOG_PROCESS_ERROR(it != m_CoolDownTable.end());

    nResult = it->second;

Exit0:
    return nResult;
}