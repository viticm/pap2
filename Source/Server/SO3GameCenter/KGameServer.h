/************************************************************************/
/* 与GameServer的连接协议处理		                                    */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.09	Create												*/
/************************************************************************/
#ifndef _KGAMESERVER_H_
#define _KGAMESERVER_H_

#include "Relay_GS_Protocol.h"
#include "SO3Result.h"
#include "KMentorDef.h"
#include "KMentorCenter.h"

class KRole;
struct KPARTY;
struct KMail;
struct KMapInfo;
struct KACCOUNT_LOGIN_INFO; 
enum SWITCH_MAP_RESULT_CODE;
class KGPQ;
class KTong;
struct KTeam;

struct KGS_INFO 
{
    int                 nConnIndex;
    IKG_SocketStream*   piSocket;
    int                 nWorldIndex;
    float               fImmediatePerformance;  // 短期性能指数
    float               fAveragePerformance;    // 长期性能指数
    size_t              uMemory;
    int                 nPlayerCount;
	int	                nConnectionCount;
    size_t              uNetworkFlux;
	time_t              nLastRecvPacketTime;
    BOOL                bQuiting;

    DWORD               dwRoleID;
    size_t              uRoleDataOffset;
    BYTE                byRoleDataBuffer[MAX_ROLE_DATA_SIZE];
};

class KGameServer
{
public:
	KGameServer();

	BOOL Init();
    void UnInit();

    void Activate();

    void PrepareToQuit() { m_bQuiting = true; }
    BOOL IsAnyGSConnected();

    KGS_INFO* GetGSInfoByConnectionIndex(int nConnIndex);

    int  GetRandomGS();
    int  GetBestGSConnectionForCreateMap(KMapInfo* pMapInfo);
    
    // 检查玩家登陆时,GS性能是否构成性能限制
    BOOL IsPlayerLoginPerformanceLimit(int nConnIndex);
    
    BOOL ProcessRoleLogin(
        KACCOUNT_LOGIN_INFO* pAccInfo, 
		DWORD dwRoleID, 
		KRoleBaseInfo* pBaseInfo, 
		DWORD dwQueueID
    );
    
    BOOL GetGameWorldProtocolVersion(
		int* pnGameWorldLowerProtocolVersion, 
		int* nGameWorldUpperProtocolVersion
	);

    size_t GetFluxTotal();

    BOOL SendGlobalSysMsg(const char cszMsg[]);

    BOOL LoadRoleData(int nConnIndex, DWORD dwPlayerID, BOOL bSucceed, BYTE* pbyRoleData, size_t uRoleDataLen);
   
private:
    void ProcessAcceptor();
    BOOL ProcessConnection(int nConnIndex, IKG_SocketStream* piSocket);

    BOOL CloseConnection(int nConnIndex);

    BOOL Send(int nConnIndex, IKG_Buffer* piBuffer);
	void Broadcast(IKG_Buffer* piBuffer, int nExceptConnIndex = 0);

    BOOL CallGSConnectScript(int nConnIndex);

private:

    KG_SocketAcceptor	m_SocketAcceptor;
    int                 m_nLastConnIndex;
    BOOL                m_bQuiting;

	BOOL                m_bGameWorldProtocolValidFlag;
    int                 m_nGameWorldLowerProtocolVersion;
    int                 m_nGameWorldUpperProtocolVersion;

	typedef void (KGameServer::*PROCESS_PROTOCOL_FUNC)(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	PROCESS_PROTOCOL_FUNC	m_ProcessProtocolFuns[s2r_protocol_end];
    
    //协议大小数组
    size_t m_uProtocolSize[s2r_protocol_end];

    KGS_INFO m_GSList[MAX_GS_COUNT];

    int      m_nConnectionTimeout;
    DWORD    m_dwNextLoginQueueID;

    struct KTALK_TONG_TRAVERSE 
    {
        BOOL operator () (KTongMember* pMember);

        IKG_Buffer*         piSendBuff;
        R2S_TALK_MESSAGE*   pDownMsg;
        KGameServer*        pGameServer;
    };

    struct KTONG_BROADCAST_MESSAGE_TRAVERSE
    {
        BOOL operator () (KTongMember* pMember);

        IKG_Buffer*                 piSendBuff;
        R2S_TONG_BROADCAST_MESSAGE* pMessage;
        KGameServer*                pGameServer;
        DWORD                       dwExceptID;
    };

private:
	void OnHandshakeRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnPingSignal(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnUpdatePerformance(BYTE* pbyData, size_t uDataLen, int nConnIndex);

	void OnPlayerLoginRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);

	void OnCreateMapRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnSearchMapRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

	void OnTransferPlayerRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnTransferPlayerRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);

	void OnConfirmPlayerLoginRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

	void OnPlayerLeaveGS(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // ----------------------- 组队相关 ------------------------------

	// 邀请某人组队
	void OnInvitePlayerJoinTeamRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    // 处理: 玩家收到组队邀请后的应答
    void OnInvitePlayerJoinTeamRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    // 处理玩家的入队申请
    void OnApplyJoinTeamRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    // 处理队长对入队申请的应答
    void OnApplyJoinTeamRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    // 队伍删除(通常是将xxx踢出队伍)成员的请求
    void OnTeamDelMemberRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    // 队伍更换队长
    void OnTeamChangeAuthorityRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnTeamSetLootModeRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnTeamSetRollQualityRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnTeamSetFormationLeader(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // 请求解散队伍
    void OnTeamDisbandRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnTeamSyncMemberMaxLMR(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnTeamSyncMemberCurrentLMR(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnTeamSyncMemberMisc(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnTeamSyncMemberPosition(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnTeamSetMarkRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnTeamLevelRaidRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnTeamChangeMemberGroupRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // ----------------------- 聊天相关 ------------------------------

    void OnTalkMessage(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnPlayerTalkError(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // ------------------------ 好友相关 -----------------------------

    void OnApplyFellowshipDataRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnUpdateFellowshipData(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnAddFellowshipRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnGetFellowshipNameRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnApplyFellowshipPlayerLevelAndForceID(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnAddFellowshipNotify(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // --------------------- 邮件相关 --------------------------------
    void OnSendMailRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnSendGlobalMailRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnGetMailListRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnQueryMailContent(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnAcquireMailMoneyRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnAcquireMailItemRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnSetMailRead(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnDeleteMail(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnReturnMail(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnGiveMailMoneyToPlayerRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnGiveMailItemToPlayerRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    // ----------------------------------------------------------------

	void OnSyncMidMapMark(BYTE* pbyData, size_t uDataLen, int nConnIndex);    
    
    void OnSaveScenePlayerList(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnSaveSceneData(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnPlayerEnterSceneNotify(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // ---------------------- 交通相关 --------------------------------
    void OnSyncRoadTrackInfo(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // ----------------------------------------------------------------
    
    // --------------------- 角色操作相关 -----------------------------
    void OnSaveRoleDataRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnLoadRoleDataRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnSyncRoleData(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnSaveRoleData(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnChangeRoleLevelRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnChangeRoleForceIDRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnSendGmMessage(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnSendGmChn(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnSendGMCommandGS(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnSendGMCommandGC(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnJoinBattleFieldQueueRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnLeaveBattleFieldQueueRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnAcceptJoinBattleField(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnGetBattleFieldList(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnLeaveBattleFieldRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnApplyCreatePQRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnApplyDeletePQRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnApplyChangePQValueRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    
    void OnAddCampScore(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnApplySetCampRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // --------------------- 帮会操作相关 -----------------------------
	void OnApplyTongRosterRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnApplyTongInfoRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex); 
	void OnApplyCreateTongRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnInvitePlayerJoinTongRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnInvitePlayerJoinTongRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnApplyKickOutTongMemberRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnModifyTongInfoRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnModifyTongSchemaRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnApplyQuitTongRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnChangeTongMemberGroupRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnChangeTongMasterRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnChangeTongMemberRemarkRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnGetTongDescriptionRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnSaveMoneyInTongRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnPayTongSalaryRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnGetTongSalaryRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnGetTongSalaryFailRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);
	void OnChangeTongCampRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // --------------------- 帮会仓库 -----------------------------
    void OnApplyRepertoryPageRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnApplyOpenTongRepertoryRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnTakeTongRepertoryItemToPosRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnTakeTongRepertoryItemRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnTakeTongRepertoryItemRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnPutTongRepertoryItemToPosRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnPutTongRepertoryItemRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnPutTongRepertoryItemRespond(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnUnlockTongRepertoryGrid(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnExchangeTongRepertoryItemPosRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnApplyStackTongRepertoryItemRequest(BYTE*pbyData, size_t uDataLen, int nConnIndex);
    void OnStackTongRepertoryItemRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // --------------------- 寄卖行相关 -----------------------------
    void OnAuctionLookupRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnAuctionBidRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnAuctionSellRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnAuctionCancelRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnRemoteLuaCall(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnAchievementGlobalAnnounceRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnDesignationGlobalAnnounceRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnUpdateStatDataRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnQueryStatIDRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnChangeExtPointRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnSetChargeFlagRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnMapCopyKeepPlayer(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnActivePresentCodeRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnApexProtocol(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    
    void OnReportFarmerPlayerRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    void OnGameCardSellRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnGameCardBuyRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnGameCardLookupRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnGameCardCancelRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // ------------------------- 师徒相关 -----------------------------
    void OnApplyMentorData(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnUpdateMaxApprenticeNum(BYTE* pbyData, size_t uDataLen, int nConnIndex);
    void OnAddMentorValueRequest(BYTE* pbyData, size_t uDataLen, int nConnIndex);

    // ----------------------------------------------------------------
    //AutoCode:-处理协议函数结束-
public:
	BOOL DoHandshakeRespond(int nConnIndex);
    BOOL DoQuitNotify(int nConnIndex);

	BOOL DoPlayerLoginRequest(
        int nConnIndex, const KACCOUNT_LOGIN_INFO& crAccInfo, DWORD dwRoleID, const KRoleBaseInfo& crBaseInfo
    );

	BOOL DoConfirmPlayerLoginRespond(int nConnIndex, DWORD dwPlayerID, BOOL bPermit);
	
	BOOL DoKickAccountNotify(DWORD dwPlayerID);

	BOOL DoCreateMapNotify(int nConnIndex, DWORD dwMapID, int nMapCopyIndex);

    BOOL DoFinishCreateMapNotify(int nConnIndex, DWORD dwPlayerID);

	BOOL DoDeleteMapNotify(int nConnIndex, DWORD dwMapID, int nMapCopyIndex);

	BOOL DoSearchMapRespond(
        int nConnIndex, DWORD dwPlayerID, SWITCH_MAP_RESULT_CODE eResult, BOOL bSwitchServer, 
        DWORD dwDestMapID, int nDestMapCopyIndex,
        int nX = 0, int nY = 0, int nZ = 0
    );

    BOOL DoTransferPlayerRespond(
        int nConnIndex, DWORD dwPlayerID, BOOL bSucceed, 
        DWORD dwAddress = 0, int nPort = 0, const GUID* pcGuid = NULL
    );
    
    // ------------------------ 组队相关 --------------------------------------

    BOOL DoSyncTeamInfo(int nConnectionIndex, DWORD dwTeamID, KTeam* pTeam);
    BOOL DoApplyJoinTeamRequest(KRole *pApplyDst, KRole *pApplySrc);
    BOOL DoTeamCreateNotify(DWORD dwTeamID);
    BOOL DoTeamChangeAuthorityNotify(DWORD dwTeamID, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID);
    BOOL DoTeamSetLootModeNotify(DWORD dwTeamID, int nLootMode, int nRollQuality);
    BOOL DoTeamAddMemberNotify(DWORD dwTeamID, int nGroupIndex, DWORD dwMemberID);
    BOOL DoTeamDelMemberNotify(DWORD dwTeamID, int nGroupIndex, DWORD dwMemberID);
    BOOL DoTeamDisbandNotify(DWORD dwTeamID);
    BOOL DoSyncTeamMemberOnlineFlag(DWORD dwTeamID, DWORD dwMemberID, BOOL bOnlineFlag);
    BOOL DoTeamMessageNotify(int nConnIndex, DWORD dwPlayerID, int nCode, const char* pszText = NULL);
    BOOL DoTeamSetFormationLeaderNotify(DWORD dwTeamID, int nGroupIndex, DWORD dwNewFormationLeader);

    BOOL DoTeamSetMarkRespond(DWORD dwTeamID, int nMarkType, DWORD dwTargetID);
    BOOL DoTeamCampChange(DWORD dwTeamID, KCAMP eCamp);
    BOOL DoTeamLevelUpRaidNotify(DWORD dwTeamID, int nGroupNum);
    BOOL DoTeamChangeMemberGroupNotify(
        DWORD dwTeamID, DWORD dwMemberID, int nOldGroupIndex, DWORD dwDstMemberID, int nNewGroupIndex
    );

    // ------------------------ 聊天相关 --------------------------------------

    BOOL DoPlayerTalkError(int nConnectionIndex, DWORD dwReceiverID, int nErrorCode);
    BOOL DoPlayerTalkWhisperSuccessRespond(int nConnectionIndex, DWORD dwTalkerID, DWORD dwReceiverID);

    // ------------------------ 好友相关 -----------------------------

    BOOL DoApplyFellowshipDataRespond(DWORD dwPlayerID, size_t uDataLength, BYTE byData[]);
    BOOL DoAddFellowshipRespond(DWORD dwPlayerID, DWORD dwAlliedPlayerID, const char szAlliedPlayerName[], int nType);
    BOOL DoGetFellowshipNameRespond(DWORD dwPlayerID, int nArrayLength, DWORD dwAlliedPlayerIDArray[]);
    BOOL DoSyncFellowshipMapID(DWORD dwPlayerID, DWORD dwMapID);
    BOOL DoSyncFellowshipPlayerLevel(DWORD dwPlayerID, int nLevel, BOOL bNotifyClient);
    BOOL DoSyncFellowshipPlayerForceID(DWORD dwPlayerID, DWORD dwForceID);
    BOOL DoAddFellowshipNotify(int nConnIndex, DWORD dwPlayerID, DWORD dwAlliedPlayerID, const char szAlliedPlayerName[], int nType);

    // --------------------- 邮件相关 --------------------------------
    BOOL DoSendMailRespond(int nRespondID, KRole* pRole, int nResult, KMail* pMail, size_t uMailLen, const char cszReceiver[]);
    BOOL DoGetMailListRespond(DWORD dwPlayerID, KMailListInfo MailList[], int nMailCount);
    BOOL DoSyncMailContent(
        int nConnIndex, DWORD dwPlayerID, DWORD dwMailID, BYTE byResult, KMailContent* pContent, size_t uContentDataLen
    );
    BOOL DoGiveMailMoneyToPlayer(
        int nConnIndex, DWORD dwPlayerID, DWORD dwMailID, BYTE byMailType, int nMoney, const char cszSender[]
    );
    BOOL DoGiveMailItemToPlayer(
        int nConnIndex, DWORD dwPlayerID, DWORD dwMailID, BYTE byMailType, BYTE byResult, 
        int nItemIndex, int nItemPrice, BYTE* pbyItemData, size_t uItemDataLen, const char cszSender[]
    );
    BOOL DoNewMailNotity(int nConnIndex, DWORD dwPlayerID, KMailListInfo* pMailListInfo);
    BOOL DoMailGeneralRespond(int nConnIndex, DWORD dwPlayerID, DWORD dwMailID, BYTE byResult);

    // ----------------------------------------------------------------
    BOOL DoMapCopyDoClearPlayerPrepare(int nConnIndex, DWORD dwMapID, int nMapCopyIndex, int nType, int nDelayTime);
    BOOL DoMapCopyDoClearPlayer(int nConnIndex, DWORD dwMapID, int nMapCopyIndex);
    BOOL DoSetMapCopyOwner(int nConnIndex, DWORD dwMapID, int nMapCopyIndex, DWORD dwOwnerID);
    
    // --------------------- 角色操作相关 -----------------------------
    BOOL DoSaveRoleDataRespond(int nConnIndex, DWORD dwPlayerID, BOOL bSucceed, int nUserValue);

    BOOL DoSyncRoleData(int nConndex, DWORD dwRoleID, BYTE* pbyData, size_t uOffset, size_t uDataLen);
    BOOL DoLoadRoleData(int nConndex, DWORD dwRoleID, BOOL bSucceed, size_t uRoleDataLen);

    // ---------------------   帮会相关   -----------------------------
    
    BOOL DoSyncTongMemberInfo(int nConnIndex, int dwPlayerID, DWORD dwMemberID, const KTongMember& crMember);
    BOOL DoTongDeleteMemberNotify(int nConnIndex, DWORD dwPlayerID, DWORD dwMemberID);

    BOOL DoSyncTongRepertoryPageRespond(int nConnIndex, int dwPlayerID, int nPageIndex, int nLastUpdateCounter);

    BOOL DoUpdateTongBaseInfo(int nConnIndex, DWORD dwPlayerID, KTong* pTong);
    BOOL DoUpdateTongDevelopmentInfo(int nConnIndex, DWORD dwPlayerID, KTong* pTong);
    BOOL DoUpdateTongTextInfo(int nConnIndex, DWORD dwPlayerID, int nDataType,  const char cszText[]);
    BOOL DoUpdateTongSchemaInfo(int nConnIndex, DWORD dwPlayerID, KTongSchemaData* pData);
    BOOL DoUpdateTongMemorabilia(int nConnIndex, DWORD dwPlayerID, KTong* pTong);

    BOOL DoUpdateTongClientDataVersion(int nConnIndex, DWORD dwPlayerID, KTONG_UPDATE_FLAG eFrameType, int nUpdateFrame);
    
    BOOL DoInvitePlayerJoinTongRequest(int nConnIndex, DWORD dwPlayerID, DWORD dwInviter, DWORD dwTongID, char szInviterName[], char szTongName[]);
    BOOL DoChangePlayerTongNotify(int nConnIndex, DWORD dwPlayerID, DWORD dwTongID, KCHANGE_TONG_REASON eReason, char szTongName[]);
    BOOL DoTongBroadcastMessage(KTong* pTong, int nCode, DWORD dwExceptID, BYTE* pbyData = NULL, size_t uDataLen = 0);
    BOOL DoGetTongDescriptionRepond(int nConnIndex, DWORD dwPlayerID, KTong* pTong);

    BOOL DoSyncTongOnlineMessage(int nConnIndex, DWORD dwPlayerID);
    BOOL DoTakeTongRepertoryItemToPosRespond(
        int nConnIndex, DWORD dwPlayerID, DWORD dwTongID,
        int nPageIndex, int nRepertoryPos, int nBoxIndex, int nBoxPos, 
        size_t uSize, BYTE* pbyData
    );
    BOOL DoTakeTongRepertoryItemRespond(
        int nConnIndex, DWORD dwPlayerID, DWORD dwTongID,
        int nPageIndex, int nRepertoryPos, size_t uSize, BYTE* pbyData
    );

    BOOL DoPutTongRepertoryItemRespond(
        DWORD dwPlayerID, int nConnIndex, DWORD dwTongID, 
        DWORD dwServerItemID,BYTE byBoxIndex, BYTE byBoxPos, BYTE byPageIndex, BYTE byRepertoryPos
    );
    
    BOOL DoApplyStackTongRepertoryItemRespond(
        int nConnIndex, DWORD dwPlayerID, DWORD dwTongID,
        BYTE byPageIndex, BYTE byPagePos, BYTE byBoxIndex, BYTE byBoxPos, DWORD dwItemID,
        BYTE byTabType, WORD wTabIndex, WORD wStackNum
    );

    BOOL DoApplyOpenTongRepertoryRespond(int nConnIndex, DWORD dwPlayerID, KTong* pTong);

    BOOL DoGetTongSalaryRespond(int nConnIndex, DWORD dwPlayerID, DWORD dwTongID, int nSilver);
    BOOL DoSyncTongHistoryRespond(DWORD dwPlayerID, int nType, DWORD dwStartIndex, int nSyncRecordCount, BYTE byBuffer[], size_t uBufferSize);

    // ---------------------   寄卖行相关   -----------------------------
    BOOL DoAuctionLookupRespond(DWORD dwPlayerID, BYTE byRespondID, BYTE byCode, KAUCTION_PAGE_HEAD* pPage);
    BOOL DoAuctionBidRespond(DWORD dwPlayerID, BYTE byCode, int nBidPrice);
    BOOL DoAuctionSellRespond(
        DWORD dwPlayerID, BYTE byCode, int nStartPrice, int nBuyItNowPrice, 
        int nCustodyCharges, size_t uItemDataLen, BYTE* pbyItemData
    );
    BOOL DoAuctionCancelRespond(DWORD dwPlayerID, BYTE byCode);
    BOOL DoAuctionMessageNotify(DWORD dwPlayerID, BYTE byCode, const char* pszSaleName, int nPrice);

    // -----------------------------------------------------------------

    BOOL DoGmCommand(int nConnIndex, DWORD dwPlayerID, const char cszGmName[], const char cszGmCommand[]);

    BOOL DoJoinBattleFieldQueueRespond(int nConnIndex, DWORD dwPlayerID, DWORD dwMapID, int nErrorCode);
    BOOL DoLeaveBattleFieldQueueRespond(int nConnIndex, DWORD dwPlayerID, DWORD dwMapID, int nErrorCode);

    BOOL DoBattleFieldNotify(
        int nConnIndex, DWORD dwPlayerID, time_t nAvgQueueTime, time_t nPassTime,
        DWORD dwMapID, int nCopyIndex, BATTLE_FIELD_NOTIFY_TYPE eType
    );
    BOOL DoSyncBattleFieldList(int nConnIndex, DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex);

    BOOL DoAcceptJoinBattleFieldRespond(int nConnIndex, DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex);
    BOOL DoLeaveBattleFieldRespond(int nConnIndex, DWORD dwPlayerID);

    BOOL DoSyncPQ(int nConnIndex, const KGPQ& crPQ);
    BOOL DoBroadcastSyncPQ(const KGPQ& crPQ);
    BOOL DoBroadcastDeletePQ(DWORD dwPQID);
    BOOL DoBroadcastPQValueChange(DWORD dwPQID, int nIndex, int nValue);
    BOOL DoBroadcastPQFinish(DWORD dwPQID);

    BOOL DoSyncCampInfo(int nConnIndex);
    BOOL DoApplySetCampRespond(KRole* pRole, CAMP_RESULT_CODE eResult);

    BOOL DoRemoteLuaCall(int nConnIndex, const char cszFunction[], BYTE* pbyData, size_t uDataLen);

    BOOL DoAchievementAnnounceRespond(DWORD dwPlayerID, int nAchievementID);

    BOOL DoDesignationGlobalAnnounceRespond(DWORD dwPlayerID, int nPrefix, int nPostfix);

    BOOL DoSyncGlobalSystemValue(int nConnIndex);
    BOOL DoQueryStatIDRespond(int nConnIndex, const char szName[], int nID);
    BOOL DoChangeExtPointRespond(int nConnIndex, DWORD dwPlayerID, BOOL bSucceed);
    BOOL DoSetChargeFlagRespond(
        int nConnIndex, DWORD dwPlayerID, int nChargeFlag, const tagExtPointInfo& rExtPointInfo,
        DWORD dwEndTimeOfFee
    );

    BOOL DoSyncZoneChargeFlag(int nConnIndex, int nZoneChargeFlag);
    BOOL DoActivePresentCodeRespond(
        int nConnIndex, DWORD dwPlayerID, BOOL bSucceed, const char* pszPresentCode, DWORD dwPresentType
    );

    // ------------ 交通相关 ----------------------------------------->>
    BOOL DoSyncRoadTrackInfo(int nConnIndex, int nRoadNodeID, KCAMP eCamp, BOOL bEnable);

    // ------------ 反外挂相关 -----------------------------------------
    BOOL DoApexProtocol(DWORD dwRoleID, BYTE* pbyData, size_t nSize);

    // ------------ 反打钱 -----------------------------------------
    BOOL DoSetFarmerLimitFlag(int nConnIndex, DWORD dwPlayerID, BOOL bLimitFlag);
    BOOL DoFreeOnlineFarmerRole(int nConnIndex, DWORD dwRoleID);

    // ------------ 月卡寄售 -----------------------------------------
    BOOL DoGameCardSellRespond(DWORD dwPlayerID, int nCode, int nCoin);
    BOOL DoGameCardBuyRespond(DWORD dwPlayerID, int nCode, int nBuyPrice);
    BOOL DoGameCardLookupRespond(DWORD dwPlayerID, int nTotalCount, int nCount, KGAME_CARD* pCardList);
    BOOL DoGameCardCancelRespond(DWORD dwPlayerID, BYTE byCode);

    // ------------ 师徒相关 -----------------------------------------
    BOOL DoSyncMentorData(int nConnIndex, DWORD dwMentorID, DWORD dwApprenticeID);
    BOOL DoDeleteMentorRecord(uint64_t uMKey);
    BOOL DoUpdateMentorRecord(DWORD dwMentor, DWORD dwApprentice, const KMentorInfo& crMentorInfo);
    BOOL BroadcastSeekMentorYell(KRole* pRole);
    BOOL BroadcastSeekApprenticeYell(KRole* pRole);

    // -----------------------------------------------------------------
    //AutoCode:-发送协议函数结束-
};
#endif	//_KGAMESERVER_H_
