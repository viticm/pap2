#ifndef _KMAIL_CLIENT_H_
#define _KMAIL_CLIENT_H_

#ifdef _CLIENT

#include "Luna.h"
#include "KMailDef.h"

struct KMailInfo
{
    KMailInfo(KMailListInfo* pMailListNode);

    int GetLeftSecond(int nMailSurvivalTime);

    DWORD       dwMailID;
    char        szSenderName[ROLE_NAME_LEN];
    char        szTitle[MAIL_TITLE_LEN];
    time_t      nSendTime;
    time_t      nRecvTime;
    bool        bReadFlag;
    bool        bMoneyFlag;
    bool        bItemFlag;
    size_t      uTextLen;
    BYTE        byType;
    BYTE        bGotContentFlag;
    int         nMoney;
    BYTE        byText[MAX_MAIL_TEXT_LEN];
    DWORD       dwItemID[KMAIL_MAX_ITEM_COUNT];
    int         nItemPrice[KMAIL_MAX_ITEM_COUNT];

	DECLARE_LUA_CLASS(KMailInfo);

	DECLARE_LUA_STRUCT_DWORD(MailID, dwMailID);
	DECLARE_LUA_STRUCT_STRING(SenderName, ROLE_NAME_LEN, szSenderName);
	DECLARE_LUA_STRUCT_STRING(Title, MAIL_TITLE_LEN, szTitle);
	DECLARE_LUA_STRUCT_INTEGER(Money, nMoney);
	DECLARE_LUA_STRUCT_BOOL(ReadFlag, bReadFlag);
	DECLARE_LUA_STRUCT_BOOL(MoneyFlag, bMoneyFlag);
	DECLARE_LUA_STRUCT_BOOL(ItemFlag, bItemFlag);
	DECLARE_LUA_STRUCT_BOOL(GotContentFlag, bGotContentFlag);

	int LuaGetSendTime(Lua_State* L);
	int LuaGetRecvTime(Lua_State* L);
	int LuaRead(Lua_State* L);
	int LuaGetLeftTime(Lua_State* L);
	int LuaGetType(Lua_State* L);
	int LuaRequestContent(Lua_State* L);
	int LuaGetText(Lua_State* L);
	int LuaGetItem(Lua_State* L);
	int LuaTakeMoney(Lua_State* L);
	int LuaTakeItem(Lua_State* L);
};

struct KMailListInfo;

class KMailClient
{
public:
    KMailClient();
    ~KMailClient();

    void Clear();
    void Reset();
    BOOL AddMailList(int nCount, KMailListInfo* pMailList);
    void CountMail();
    BOOL UpdateMailContent(DWORD dwMailID, KMailContent* pContent, int* pnMailType);
    BOOL ApplyMessageMailContent();

private:
    typedef std::map<DWORD, KMailInfo*> KMailTable;
    KMailTable      m_MailTable;

public:
    int             m_nTotalCount;
    int             m_nUnreadCount;
    int             m_nSysCount;
    int             m_nSysUnreadCount;

    BOOL            m_bGotMessageMail;
    BOOL            m_bMailListApplying;

public:
    int LuaSendMail(Lua_State* L);
    int LuaDeleteMail(Lua_State* L);
    int LuaCountMail(Lua_State* L);
    int LuaApplyMailList(Lua_State* L);
    int LuaGetMailList(Lua_State* L);
    int LuaReturnMail(Lua_State* L);
    int LuaGetGmMessage(Lua_State* L);
    int LuaGetPlayerMessage(Lua_State* L);
	int LuaGetMailInfo(Lua_State* L);

    DECLARE_LUA_CLASS(KMailClient);
};

#endif // _CLIENT

#endif // _KMAIL_CLIENT_H_

