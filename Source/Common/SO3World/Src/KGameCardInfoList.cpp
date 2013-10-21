#include "stdafx.h"
#include "KGameCardInfoList.h"

KGameCardInfoList::KGameCardInfoList()
{
}

KGameCardInfoList::~KGameCardInfoList()
{
}

BOOL KGameCardInfoList::Init()
{
    BOOL        bResult                 = false;
    int         nRetCode                = false;
    ITabFile*   piTabFile               = NULL;
    int         nLine                   = 0;
    int         nHeight                 = 0;
    char        szFileName[MAX_PATH];

    nRetCode = snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, GAME_CARD_INFO_TAB);
    szFileName[sizeof(szFileName) - 1] = '\0';
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szFileName));

    piTabFile = g_OpenTabFile(szFileName);
    KGLOG_PROCESS_ERROR(piTabFile);

    nHeight = piTabFile->GetHeight(); 
    KGLOG_PROCESS_ERROR(nHeight > 0);

    for (nLine = 2; nLine <= nHeight; nLine++)
    {
        int                     nID;
        int                     nType;
        KGAME_CARD_INFO         Info;
        KINFO_LIST::iterator    it;

        nRetCode = piTabFile->GetInteger(nLine, "ID", 0, &nID);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(nLine, "Coin", 0, &Info.nCoin);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(nLine, "GameTime", 0, &Info.nGameTime);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(nLine, "Type", 0, &nType);
        KGLOG_PROCESS_ERROR(nRetCode);
        Info.byType = (BYTE)nType;

        it = m_InfoList.find(nID);
        KGLOG_PROCESS_ERROR(it == m_InfoList.end());

        m_InfoList.insert(std::make_pair(nID, Info));
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_InfoList.clear();
        KGLogPrintf(KGLOG_DEBUG, "Load %s failed at line %d.", GAME_CARD_INFO_TAB, nLine);
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

void KGameCardInfoList::UnInit()
{
    m_InfoList.clear();
}

KGAME_CARD_INFO* KGameCardInfoList::GetInfo(int nID)
{
    KGAME_CARD_INFO*        pResult = NULL;
    KINFO_LIST::iterator    it;

    it = m_InfoList.find(nID);
    if (it != m_InfoList.end())
    {
        pResult = &(it->second);
    }

    return pResult;
}

#ifdef _CLIENT
int KGameCardInfoList::LuaGetInfoList(Lua_State* L)
{
    int nCount = 0;

    Lua_NewTable(L);

    for (KINFO_LIST::iterator it = m_InfoList.begin(); it != m_InfoList.end(); ++it)
    {
        Lua_PushNumber(L, ++nCount);
        Lua_NewTable(L);

        Lua_PushString(L, "ID");
        Lua_PushNumber(L, it->first);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "Coin");
        Lua_PushNumber(L, it->second.nCoin);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "GameTime");
        Lua_PushNumber(L, it->second.nGameTime);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "Type");
        Lua_PushNumber(L, (int)(it->second.byType));
        Lua_SetTable(L, -3);

        Lua_SetTable(L, -3);
    }

    return 1;
}
#endif // _CLIENT
