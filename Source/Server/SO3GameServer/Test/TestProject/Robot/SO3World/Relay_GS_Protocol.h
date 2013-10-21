/************************************************************************/
/* Relay和GS之间的协议定义					                            */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.09	Create												*/
/************************************************************************/
#ifndef _RELAY_GS_PROTOCOL_H_
#define _RELAY_GS_PROTOCOL_H_

#include "ProtocolBasic.h"
#include "SO3ProtocolBasic.h"

enum RELAY_GS_PROTOCOL
{
	relay_gs_connection_begin = 0,

	//GS连接上Relay后的初始化数据
	g2r_initialize_data,
	r2g_initialize_data,

	//地图管理相关协议
	r2g_dml_create_mapcopy,
	g2r_dml_reply_create_mapcopy,
	r2g_dml_delete_mapcopy,
	g2r_dml_reply_delete_mapcoy,

	//跨地图相关协议
	g2r_dml_search_mapcopy,
	r2g_dml_reply_search_mapcopy,
	g2r_dml_send_player_data,
	r2g_dml_send_player_data,
	g2r_dml_inform_player_guid,
	r2g_dml_inform_player_guid,
	r2g_dml_kick_player_out,

	//玩家登陆、登出
	r2g_login_request,
	g2r_reply_login_request,

	g2r_inform_player_login,
	r2g_reply_player_login,

	g2r_player_leave_gs,

	r2g_kick_player,

	//好友数据更新协议
	g2r_update_relation,
	r2g_update_relation,

	//帮派数据更新协议

    // ------------- 组队相关协议 --------------------------------

	//客户端发起邀请后，GS将这个邀请转发给Relay处理(邀请对象不一定是本GS的)
	g2r_invite_player_join_party_request,
	//Relay处理玩家邀请组队的请求，将该邀请转发到被邀请的玩家
	r2g_invite_player_join_party_request,
	//被邀请玩家接收到邀请后给出的应答，通过这个协议转发给Relay
	g2r_invite_player_join_party_respond,
	//Relay对组队邀请的回应,注如果是一切顺利的话不会发这个协议，而是直接收到队伍添加成员的协议
	r2g_invite_player_join_party_respond,
    //GS接收到玩家的入队申请后，将请求转发给Relay处理
    g2r_apply_join_party_request,
    //Relay对玩家组队申请的应答(只有在申请失败的情况下才会返回这个消息)
    r2g_apply_join_party_respond,
    //Relay对玩家的入队申请稍加处理后，转发到目标玩家所在GS
    r2g_apply_join_party_request,
    //队长对入队申请作出应答后，通过这个协议转发到Relay
    g2r_apply_join_party_respond,

    r2g_party_create_notify,
    r2g_party_add_member_notify,
    g2r_party_del_member_request,
    r2g_party_del_member_notify,
    g2r_party_change_leader_request,
    r2g_party_change_leader_notify,
    g2r_party_set_loot_mode_request,
	g2r_party_set_roll_quality_request,
    r2g_party_set_loot_mode_notify,
    g2r_party_disband_request,
    r2g_party_disband_notify,
    r2g_sync_party_member_online_flag,

    g2r_sync_party_member_max_lmr,
    g2r_sync_party_member_current_lmr,
    g2r_sync_party_member_misc,

    r2g_sync_party_member_max_lmr,
    r2g_sync_party_member_current_lmr,
    r2g_sync_party_member_misc,

    // ---------------- 聊天相关 --------------------------------------
    r2g_player_talk_error,

    g2r_player_talk_to_one,
    r2g_player_talk_to_one,
    g2r_player_talk_to_many,
    r2g_player_talk_to_many,

    // ----------------------------------------------------------------

    // ---------------- 好友相关 --------------------------------------
    g2r_player_add_relation,
    r2g_player_add_relation,

    g2r_player_del_relation,
    r2g_player_del_relation,

    g2r_player_update_relation,
    r2g_player_update_relation,
    // ----------------------------------------------------------------

	relay_gs_connection_end
};

#pragma	pack(1)
//GS连接上Relay后的初始化数据
struct G2R_INITIALIZE_DATA : INTERNAL_PROTOCOL_HEADER
{
};

struct R2G_INITIALIZE_DATA : INTERNAL_PROTOCOL_HEADER
{
	int		nGSIndex;
};

//地图管理相关协议结构
struct R2G_DML_CREATE_MAPCOPY : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwMapID;							//地图ID
	DWORD		dwMapCopyIndex;					//地图副本编号
	DWORD		dwPlayerID;						//申请创建地图的玩家的ID
};

struct G2R_DML_REPLY_CREATE_MAPCOPY : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwMapID;							//地图ID
	DWORD		dwMapCopyIndex;					//地图副本编号
	DWORD		dwPlayerID;						//申请创建地图的玩家的ID
	BOOL		bResult;							//加载结果，1 成功
};

struct R2G_DML_DELETE_MAPCOPY : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwMapID;							//地图ID
	DWORD		dwMapCopyIndex;					//地图副本编号
	BOOL		bDirectDelete;					//是否直接删除
};

struct G2R_DML_REPLY_DELETE_MAPCOY : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwMapID;							//地图ID
	DWORD		dwMapCopyIndex;					//地图副本编号
	BOOL		bResult;							//删除结果，1 成功
};

//跨地图相关协议结构
struct G2R_DML_SEARCH_MAPCOPY : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwPlayerID;
	DWORD		dwMapID;
	DWORD		dwMapCopyIndex;
};

struct R2G_DML_REPLY_SEARCH_MAPCOPY : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwPlayerID;
	BOOL		bResult;
	int			nDestConnIndex;
	DWORD		dwDestMapID;
	DWORD		dwDestMapCopyIndex;
};

struct G2R_DML_SEND_PLAYER_DATA : INTERNAL_PROTOCOL_HEADER
{
	DWORD			dwPlayerID;
	DWORD			dwDestMapID;
	DWORD			dwDestMapCopyIndex;

	char            szAccountName[_NAME_LEN];
	char            szRoleName[_NAME_LEN];  // 其实角色名在后面的数据里面当然有,但是这里副上便于写日志
	tagExtPointInfo	ExtPointInfo;			// 可用的附送点
	tagExtPointInfo	ChangePointInfo;		// 变化的附送点
	unsigned		ulLeftTime;				// 剩余时间,以秒为单位,包括点数和包时(包天，包月)
	unsigned        uRoleDataLength;        // 角色信息数据的长度
	char			RoleInfo[0];			// 角色数据
};

struct R2G_DML_SEND_PLAYER_DATA : INTERNAL_PROTOCOL_HEADER
{
	DWORD			dwPlayerID;
	DWORD			dwDestMapID;
	DWORD			dwDestMapCopyIndex;

	char            szAccountName[_NAME_LEN];
	char            szRoleName[_NAME_LEN];  // 其实角色名在后面的数据里面当然有,但是这里副上便于写日志
	tagExtPointInfo	ExtPointInfo;			// 可用的附送点
	tagExtPointInfo	ChangePointInfo;		// 变化的附送点
	unsigned long	ulLeftTime;				// 剩余时间,以秒为单位,包括点数和包时(包天，包月)
	unsigned        uRoleDataLength;        // 角色信息数据的长度
	char			RoleInfo[0];			// 角色数据
};

struct G2R_DML_INFORM_PLAYER_GUID : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwPlayerID;
	GUID		Guid;
	int			nGSAddr;
	int			nGSPort;
	BOOL		bResult;
};

struct R2G_DML_INFORM_PLAYER_GUID : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwPlayerID;
	GUID		Guid;
	int			nGSAddr;
	int			nGSPort;
	BOOL		bResult;
};

struct R2G_DML_KICK_PLAYER_OUT : INTERNAL_PROTOCOL_HEADER
{
	DWORD dwPlayerID;
};


//玩家登陆、登出
struct R2G_LOGIN_REQUEST : IDENTITY_HEADER
{
	char            szAccountName[_NAME_LEN];
	char            szRoleName[_NAME_LEN];  // 其实角色名在后面的数据里面当然有,但是这里副上便于写日志
    DWORD           dwPartyID;
	tagExtPointInfo	ExtPointInfo;			// 可用的附送点
	tagExtPointInfo	ChangePointInfo;		// 变化的附送点
	unsigned long   ulLeftTime;				// 剩余时间,以秒为单位,包括点数和包时(包天，包月)
	unsigned        uRoleDataLength;        // 角色信息数据的长度
	char			RoleInfo[0];			// 角色数据
};

struct G2R_REPLY_LOGIN_REQUEST : IDENTITY_HEADER
{
	DWORD		dwPlayerID;
	BOOL		bResult;
	GUID		Guid;
	int			nGSAddr;
	int			nGSPort;
};

struct G2R_INFORM_PLAYER_LOGIN : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwPlayerID;
};

struct R2G_REPLY_PLAYER_LOGIN : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwPlayerID;
	BYTE		byResult;
};

struct G2R_PLAYER_LEAVE_GS : INTERNAL_PROTOCOL_HEADER
{
    DWORD       dwPlayerID;
};

struct R2G_KICK_PLAYER : INTERNAL_PROTOCOL_HEADER
{
	DWORD       dwPlayerID;
};

struct G2R_UPDATE_RELATION : INTERNAL_PROTOCOL_HEADER
{
	__int64			nPlayerIndexA;	//玩家流水号
	__int64			nPlayerIndexB;	//玩家流水号
	int				nRelationType;	//相关枚举 RELATION_TYPE
	int				nIntimacy;		//好感度 
};

struct R2G_UPDATE_RELATION : INTERNAL_PROTOCOL_HEADER
{
	__int64			nPlayerIndexA;	//玩家流水号
	__int64			nPlayerIndexB;	//玩家流水号
	int				nRelationType;	//相关枚举 RELATION_TYPE
	int				nIntimacy;		//好感度 
};

// ------------------ 组队相关协议结构定义 ---------------------------

// 从GS转发过来的玩家组队邀请
struct G2R_INVITE_PLAYER_JOIN_PARTY_REQUEST : INTERNAL_PROTOCOL_HEADER
{
	DWORD dwInviteSrc;
	char  szInviteRole[_NAME_LEN];
};

// Relay将组队邀请转发到被邀请者所在的服务器
struct R2G_INVITE_PLAYER_JOIN_PARTY_REQUEST : INTERNAL_PROTOCOL_HEADER
{
	char  szInviteSrc[_NAME_LEN];
	DWORD dwInviteDst;
    GUID  guid;
};

// Relay发给邀请发出者的应答消息
struct R2G_INVITE_PLAYER_JOIN_PARTY_RESPOND : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwInviteSrc;
    char  szInvitedPlayer[_NAME_LEN];
    int   nRespondCode;
};

// 被邀请玩家的应答消息GS==>Relay
struct G2R_INVITE_PLAYER_JOIN_PARTY_RESPOND : INTERNAL_PROTOCOL_HEADER
{
    char  szInviteSrc[_NAME_LEN];
    DWORD dwInviteDst;
    BOOL  bAgreeFlag;
    GUID  guid;
};

// 转发玩家的入队申请到Relay
struct G2R_APPLY_JOIN_PARTY_REQUEST : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwApplySrc; // 发出入队申请的玩家
    char  szApplyDst[_NAME_LEN]; // 申请加入这个玩家的队伍
};

// Relay对入队申请稍加处理后，转发到目标玩家所在的服务器
struct R2G_APPLY_JOIN_PARTY_REQUEST : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwApplyDst;
    char  szApplySrc[_NAME_LEN];
    GUID  guid;
};

// 将队长对入队申请的应答转发到Relay
struct G2R_APPLY_JOIN_PARTY_RESPOND : INTERNAL_PROTOCOL_HEADER
{
    char  szApplySrc[_NAME_LEN];
    DWORD dwApplyDst;
    BOOL  bAgreeFlag;
    GUID  guid;
};

struct R2G_APPLY_JOIN_PARTY_RESPOND : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwApplySrc;
    char  szLeader[_NAME_LEN];
    int   nRespondCode;
};

// 队伍解散请求
struct G2R_PARTY_DISBAND_REQUEST : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwLeaderID;
    DWORD dwPartyID;
};

// 队伍解散消息广播
struct R2G_PARTY_DISBAND_NOTIFY : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPartyID;
};

// 队员上下线通知
struct R2G_SYNC_PARTY_MEMBER_ONLINE_FLAG : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwMemberID;
    BOOL  bOnlineFlag;
};


struct R2G_PARTY_CREATE_NOTIFY : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwLeaderID;
    int   nLootMode;
	int	  nRollQuality;
    char  szLeaderName[_NAME_LEN];
    int   nLeaderPortraitID; // 队长头像ID
    BOOL  bLeaderOnlineFlag;
};

// 队伍添加成员消息广播
struct R2G_PARTY_ADD_MEMBER_NOTIFY : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwMemberID;
    char  szMemberName[_NAME_LEN];
    int   nMemberPortraitID; // 头像ID
    BOOL  bMemberOnlineFlag;
};

// 队伍删除成员请求
struct G2R_PARTY_DEL_MEMBER_REQUEST : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwRequestSrc; // 请求发起者的ID
    DWORD dwMemberID;
};

// 队伍删除成员消息广播
struct R2G_PARTY_DEL_MEMBER_NOTIFY : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwMemberID; 
};

// 设置队伍队长请求
struct G2R_PARTY_CHANGE_LEADER_REQUEST : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwRequestSrc; // 请求发起者的ID
    DWORD dwNewLeaderID;
};

// 队伍队长变更通知
struct R2G_PARTY_CHANGE_LEADER_NOTIFY : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwNewLeaderID;
};

// 设置队伍物品分配方式
struct G2R_PARTY_SET_LOOT_MODE_REQUEST : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    DWORD dwRequestSrc;
    int   nLootMode;
};

struct G2R_PARTY_SET_ROLL_QUALITY_REQUEST : INTERNAL_PROTOCOL_HEADER
{
	DWORD dwPartyID;
	DWORD dwRequestSrc;
	int   nRollQuality;
};

// 物品分配模式变更通知
struct R2G_PARTY_SET_LOOT_MODE_NOTIFY : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPartyID;
    int   nLootMode;
	int   nRollQuality;
};


struct G2R_SYNC_PARTY_MEMBER_MAX_LMR : INTERNAL_PROTOCOL_HEADER
{
    DWORD 	dwPartyID;
    DWORD 	dwMemberID;
    int   	nMaxLife; 
    int   	nMaxMana; 
    int   	nMaxRage;
};

struct G2R_SYNC_PARTY_MEMBER_CURRENT_LMR : INTERNAL_PROTOCOL_HEADER
{
    DWORD 	dwPartyID;
    DWORD 	dwMemberID;
    BYTE   	byLifePercent; 
    BYTE   	byManaPercent; 
    BYTE   	byRagePercent; 
};

struct G2R_SYNC_PARTY_MEMBER_MISC : INTERNAL_PROTOCOL_HEADER
{
    DWORD 	dwPartyID;
    DWORD 	dwMemberID;
    DWORD   dwPortraitID;
    DWORD   dwMapID;
    DWORD   dwForceID;
    BYTE    byLevel;
};

struct R2G_SYNC_PARTY_MEMBER_MAX_LMR : INTERNAL_PROTOCOL_HEADER
{
    DWORD 	dwPartyID;
    DWORD 	dwMemberID;
    int   	nMaxLife; 
    int   	nMaxMana; 
    int   	nMaxRage;
};

struct R2G_SYNC_PARTY_MEMBER_CURRENT_LMR : INTERNAL_PROTOCOL_HEADER
{
    DWORD 	dwPartyID;
    DWORD 	dwMemberID;
    BYTE   	byLifePercent; 
    BYTE   	byManaPercent; 
    BYTE   	byRagePercent; 
};

struct R2G_SYNC_PARTY_MEMBER_MISC : INTERNAL_PROTOCOL_HEADER
{
    DWORD 	dwPartyID;
    DWORD 	dwMemberID;
    DWORD   dwPortraitID;
    DWORD   dwMapID;
    DWORD   dwForceID;
    BYTE    byLevel;
};


// -------------- 聊天相关协议结构定义 ------------------------
struct R2G_PLAYER_TALK_ERROR : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwReceiverID;
    int   nErrorCode;
};

struct G2R_PLAYER_TALK_TO_ONE : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwTalkerID;
    char  szTalkTo[_NAME_LEN];
    BYTE  byTalkData[0];        // 对话内容数据
};

struct R2G_PLAYER_TALK_TO_ONE : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwReceiverID;
    BYTE  byReplyFlag;          // 表明是否是对说话者的回应
    char  szName[_NAME_LEN];    // bReplyFlag = 1, 接受者的名字; bReplyFlag = 0, 说话人的名字;
    BYTE  byTalkData[0];        // 对话内容数据
};

struct G2R_PLAYER_TALK_TO_MANY : INTERNAL_PROTOCOL_HEADER
{
    BYTE  byTalkRange;
    DWORD dwTalkerID;
    BYTE  byTalkData[0];        // 对话内容数据
};

struct R2G_PLAYER_TALK_TO_MANY : INTERNAL_PROTOCOL_HEADER
{
    BYTE  byTalkRange;
    DWORD dwReceiverID;
    char  szTalker[_NAME_LEN];
    BYTE  byTalkData[0];        // 对话内容数据
};
// -------------------------------------------------------------

// -------------- 好友相关协议结构定义 -------------------------
struct G2R_PLAYER_ADD_RELATION : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPlayerID;
    char  szAlliedPlayerName[_NAME_LEN];
};

struct R2G_PLAYER_ADD_RELATION : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPlayerID;
    char  szAlliedPlayerName[_NAME_LEN];
    BYTE  byResult;
    DWORD dwAlliedPlayerID;
};

struct G2R_PLAYER_DEL_RELATION : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPlayerID;
    DWORD dwAlliedPlayerID;
};

struct R2G_PLAYER_DEL_RELATION : INTERNAL_PROTOCOL_HEADER
{
    DWORD dwPlayerID;
    DWORD dwAlliedPlayerID;
    BYTE  byResult;
};
// -------------------------------------------------------------

#pragma pack()

#endif	//_RELAY_GS_PROTOCOL_H