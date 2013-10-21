#ifndef _KGAME_CARD_INFO_LIST_H_
#define _KGAME_CARD_INFO_LIST_H_

#include <map>
#include "Luna.h"

#define GAME_CARD_INFO_TAB "GameCardInfo.tab"

struct KGAME_CARD_INFO 
{
    int     nCoin;
    int     nGameTime;
    BYTE    byType;
};

class KGameCardInfoList
{
public:
    KGameCardInfoList();
    ~KGameCardInfoList();

    BOOL Init();
    void UnInit();

    KGAME_CARD_INFO* GetInfo(int nID);

#ifdef _CLIENT
    int LuaGetInfoList(Lua_State* L);
#endif // _CLIENT

private:
    typedef std::map<int, KGAME_CARD_INFO> KINFO_LIST;
    KINFO_LIST m_InfoList;
};
#endif
