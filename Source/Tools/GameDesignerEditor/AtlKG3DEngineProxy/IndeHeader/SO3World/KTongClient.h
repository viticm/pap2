#ifndef _KTONGCLIENT_H_ 
#define _KTONGCLIENT_H_ 

#if defined(_CLIENT)
#include "KTongDef.h"

class KTongClient
{
public:
    KTongClient();

    BOOL Init();
    void UnInit();

    void Reset();

    BOOL UpdateBaseInfo(BYTE* pbyData, size_t uDataLen);
    BOOL UpdateDevelopmentInfo(BYTE* pbyData, size_t uDataLen);
    BOOL UpdateAnnouncement(BYTE* pbyData, size_t uDataLen);
    BOOL UpdateOnlineMessage(BYTE* pbyData, size_t uDataLen);
    BOOL UpdateIntroduction(BYTE* pbyData, size_t uDataLen);
    BOOL UpdateRules(BYTE* pbyData, size_t uDataLen);
    BOOL UpdateSchemaInfo(BYTE* pbyData, size_t uDataLen);

    BOOL UpdateMemberInfo(const KTongMemberInfo& crTongMemerInfo);
    BOOL DeleteMember(DWORD dwMemberID);

    int  GetMemberCount() { return (int)m_Roster.size(); }
    BOOL GetMemberList(DWORD* pdwMember, int nMaxCount);

    BOOL TryGetTongName(DWORD dwTongID, char* pszRetBuffer, size_t uBufferSize);
    BOOL ProcessGetTongDescrioptionRespond(DWORD dwTongID, char szTongName[]);

public:
    int     m_nLastRosterUpdateFrame;
    int     m_nLastInfoUpdateFrame;

private:
    typedef KMemory::KAllocator<std::pair<DWORD, KTongMemberInfo> > KCLIENT_ROSTER_ALLOCTOR;
    typedef std::map<DWORD, KTongMemberInfo, less<DWORD>, KCLIENT_ROSTER_ALLOCTOR> KCLIENT_ROSTER;
    KCLIENT_ROSTER  m_Roster;                           // 成员名单缓存

    KTongSchemaData m_Schema;

    DWORD           m_dwMaster;                         // 帮主
    int             m_nFund;                            // 资金
    int             m_nLevel;
    int             m_nDevelopmentPoint;                // 发展点
    int             m_nMaxMemberCount;                  // 成员上限
    KCAMP           m_eCamp;                            // 阵营

    char            m_szTongName[_NAME_LEN];            // 帮会名称
    char            m_szAnnouncement[128];              // 公告
    char            m_szIntroduction[128];              // 介绍
    char            m_szRules[1024];                    // 帮规

    KTONG_STATE     m_eState;
    time_t          m_nStateTimer;

    BYTE            m_byTechTree[MAX_TONG_TECHTREE_NODE_NUM];

    int             m_nOperationIndex[totTotal];

    struct KTONG_DESCRIPTION
    {
        char szTongName[_NAME_LEN];
    };
    typedef KMemory::KAllocator<std::pair<DWORD, KTONG_DESCRIPTION> > KTONG_DESCRIPTION_ALLOCTOR;
    typedef std::map<DWORD, KTONG_DESCRIPTION, less<DWORD>, KTONG_DESCRIPTION_ALLOCTOR> KTONG_DESCRIPTION_TABLE;
    KTONG_DESCRIPTION_TABLE m_TongTable;

    struct FlushTongNameTraverser
    {
        DWORD dwTongID;
        char* pszTongName;
        BOOL operator () (DWORD dwID, KPlayer* pPlayer);
    };

    std::set<DWORD> m_DealingTongDescription;

public:
    char            m_szOnlineMessage[128];             // 上线提示

private:
    KTongMemberInfo*    GetMember(DWORD dwMemberID);
    BOOL                LoadOperationConfig();
    BOOL                CanBaseOperate(int nGroupID, KTONG_OPERATION_TYPE eOperationType);
    BOOL                CanAdvanceOperate(int nGroup, int nTargetGroup, KTONG_OPERATION_TYPE eOperationType);

private:
    int LuaGetGroupInfo(Lua_State* L);
    int LuaGetMemberList(Lua_State* L);
	int LuaGetMemberInfo(Lua_State* L);
    int LuaGetDefaultGroupID(Lua_State* L);
	int LuaGetMasterGroupID(Lua_State* L);
    int LuaGetStateTimer(Lua_State* L);

    int LuaCanBaseOperate(Lua_State* L);
    int LuaCanAdvanceOperate(Lua_State* L);
    int LuaCanBaseGrant(Lua_State* L);
    int LuaCanAdvanceGrant(Lua_State* L);
    int LuaCheckBaseOperationGroup(Lua_State* L);
    int LuaCheckAdvanceOperationGroup(Lua_State* L);

    int LuaApplyTongInfo(Lua_State* L);
    int LuaApplyTongRoster(Lua_State* L);

    int LuaApplyModifyTongName(Lua_State* L);
    int LuaApplyModifyAnnouncement(Lua_State* L);
    int LuaApplyModifyOnlineMessage(Lua_State* L);
    int LuaApplyModifyIntroduction(Lua_State* L);
    int LuaApplyModifyRules(Lua_State* L);

    int LuaInvitePlayerJoinTong(Lua_State* L);      
    int LuaRespondInviteJoinTong(Lua_State* L);
    int LuaApplyKickOutMember(Lua_State* L);
    int LuaModifyGroupName(Lua_State* L);
    int LuaModifyBaseOperationMask(Lua_State* L);
    int LuaModifyAdvanceOperationMask(Lua_State* L);
    int LuaModifyGroupWage(Lua_State* L);
    int LuaQuit(Lua_State* L);
    int LuaChangeMaster(Lua_State* L);
    int LuaChangeMemberGroup(Lua_State* L);
    int LuaChangeMemberRemark(Lua_State* L);
	int LuaGetMemberCount(Lua_State* L);
public:
    DECLARE_LUA_CLASS(KTongClient);

    DECLARE_LUA_DWORD(Master);
    DECLARE_LUA_INTEGER(Fund);
    DECLARE_LUA_INTEGER(Level);
	DECLARE_LUA_ENUM(Camp);
    DECLARE_LUA_INTEGER(DevelopmentPoint);
    DECLARE_LUA_INTEGER(MaxMemberCount);

    DECLARE_LUA_STRING(TongName, sizeof(m_szTongName));
    DECLARE_LUA_STRING(Announcement, sizeof(m_szAnnouncement));
    DECLARE_LUA_STRING(OnlineMessage, sizeof(m_szOnlineMessage));
    DECLARE_LUA_STRING(Introduction, sizeof(m_szIntroduction));
    DECLARE_LUA_STRING(Rules, sizeof(m_szRules));

    DECLARE_LUA_ENUM(State);
};
#endif	// _CLIENT
#endif  // _KTONGCLIENT_H_ 

