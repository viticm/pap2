/************************************************************************/
/* 游戏服务器和Relay的连接	                                            */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.04	Create												*/
/************************************************************************/
#ifndef _KRELAY_CLIENT_H_
#define _KRELAY_CLIENT_H_

#if defined(_SERVER)

#include "Common/KG_Socket.h"
#include "Relay_GS_Protocol.h"
#include <list>

class IRecorderFactory;
class KPlayer;
class KScene;

class KRelayClient
{
public:
	KRelayClient();

	BOOL Init(IRecorderFactory* piFactory);
	void UnInit();

    BOOL Breathe();

    int  GetWorldIndex() { return m_nWorldIndex; }

    BOOL SaveRoleData(KPlayer* pPlayer);

    BOOL SaveAllPlayersForExit();

protected:

    IKG_SocketStream*	m_piSocketStream;

	typedef void (KRelayClient::*PROCESS_PROTOCOL_FUNC)(BYTE* pbyData, size_t uDataLen);
	PROCESS_PROTOCOL_FUNC	m_ProcessProtocolFuns[r2s_protocol_end];

    //协议大小数组
    size_t m_uProtocolSize[r2s_protocol_end];

    BOOL Send(IKG_Buffer* piBuffer);

private:
	int    m_nPingCycle;
    time_t m_nLastSendPacketTime;
    int    m_nWorldIndex;

    DWORD  m_dwSyncRoleID;
    BYTE*  m_pbySyncRoleBuffer;
    size_t m_uSyncRoleOffset;

    BYTE*  m_pbySaveRoleBuffer;

public:
	BOOL DoHandshakeRequest();

	BOOL DoPingSignal();

    BOOL DoUpdatePerformance();

	BOOL DoPlayerLoginRespond(DWORD dwPlayerID, BOOL bPermit, GUID Guid, DWORD dwPacketIdentity);

	BOOL DoConfirmPlayerLoginRequest(DWORD dwPlayerID);

    BOOL DoPlayerLeaveGS(DWORD dwPlayerID);

	BOOL DoCreateMapRespond(DWORD dwMapID, int nMapCopyIndex, BOOL bSucceed);

	BOOL DoSearchMapRequest(DWORD dwPlayerID, DWORD dwMapID, int nMapCopyIndex, int nX, int nY, int nZ);

	BOOL DoTransferPlayerRequest(KPlayer* pPlayer);
	BOOL DoTransferPlayerRespond(DWORD dwPlayerID, BOOL bSucceed, GUID Guid);

    // --------------------- 组队相关 --------------------------------------

    // 向Relay转发邀请组队的请求
    BOOL DoInviteJoinTeamRequest(DWORD dwInviteSrc, const char cszInviteDst[]);
    // 向Relay转发玩家对组队邀请的应答
    BOOL DoInviteJoinTeamRespond(const char cszInviteSrc[], DWORD dwInviteDst, BOOL bAgreeFlag);
    // 向Relay转发玩家的入队申请
    BOOL DoApplyJoinTeamRequest(DWORD dwPlayerID, const char cszApplyDst[]);
    // 向Relay转发队长对入队申请的应答
    BOOL DoApplyJoinTeamRespond(DWORD dwLeaderID, const char cszApplySrc[], BOOL bAgreeFlag);
    // 向Relay发请求: 解散队伍
    BOOL DoTeamDisbandRequest(DWORD dwLeaderID, DWORD dwPartyID);
    // 向Relay发请求: 将玩家从队伍删除(通常是将xxx踢出队伍)
    BOOL DoTeamDelMemberRequest(DWORD dwPartyID, DWORD dwRequestSrc, DWORD dwPlayerID);
    // 向Relay发请求: 设置队长
    BOOL DoTeamChangeAuthorityRequest(DWORD dwPartyID, DWORD dwRequestSrc, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID);  

    BOOL DoTeamSetLootModeRequest(DWORD dwPartyID, DWORD dwRequestSrc, int nLootMode);
	BOOL DoTeamSetRollQualityRequest(DWORD dwPartyID, DWORD dwRequestSrc, int nRollQuality);
    
    BOOL DoTeamSetFormationLeader(DWORD dwPartyID, DWORD dwRequestSrc, DWORD dwNewFormationLeaderID, int nGroupIndex);

    BOOL DoTeamSyncMemberMaxLMR(KPlayer* pPlayer);
    BOOL DoTeamSyncMemberCurrentLMR(KPlayer* pPlayer);
    BOOL DoPartySyncMemberMisc(KPlayer* pPlayer);
    BOOL DoSyncTeamMemberPosition(KPlayer* pPlayer);

    BOOL DoTeamSetMarkRequest(DWORD dwPartyID, DWORD dwPlayerID, int nMarkType, DWORD dwTargetID);

    BOOL DoTeamLevelUpRequest(DWORD dwTeamID, DWORD dwRequestSrc);

    BOOL DoTeamChangeMemberGroupRequest(DWORD dwTeamID, DWORD dwMemberID, int nDstGroupIndex, DWORD dwRequestSrc);

    // --------------------- 聊天相关 --------------------------------------

    BOOL DoTalkMessage(
        int nMsgType, 
        DWORD dwTalkerID, const char cszTalker[],
        DWORD dwReceiverID, const char cszReceiver[],
        size_t uDataLen, const BYTE*  pbyTalkData
    );

    BOOL DoPlayerTalkError(DWORD dwPlayerID, int nErrorCode);

    // --------------------- 好友相关 --------------------------------------

    BOOL DoApplyFellowshipDataRequest(DWORD dwPlayerID);
    BOOL DoUpdateFellowshipData(DWORD dwPlayerID, size_t uDataSize, BYTE byData[]);
    BOOL DoPlayerAddFellowshipRequest(DWORD dwPlayerID, char szAlliedPlayerName[], int nType);
    BOOL DoGetFellowshipName(DWORD dwPlayerID, int nArrayLength, DWORD dwAlliedPlayerIDArray[]);
    BOOL DoApplyFellowshipPlayerLevelAndForceID(DWORD dwPlayerID);
    BOOL DoAddFellowshipNotify(DWORD dwPlayerID, DWORD dwAlliedPlayerID, char szAlliedPlayerName[], int nType);

    // --------------------- 邮件系统 ---------------------------------------
    BOOL DoSendMailRequest(
        int nRequestID, const char cszSrcName[], const char cszDstName[], const char cszTitle[], int nDelayTime,
        BOOL bSystemMail, int nMoney, const char cszText[], BYTE* pbyItemData, size_t uItemDataLen
    );
    BOOL DoSendGlobalMailRequest(
        const char sczSenderName[], const char cszTitle[], int nLifeTime,
        int nMoney, const char cszText[], BYTE* pbyItemData, size_t uItemDataLen
    );
    BOOL DoGetMailListRequest(DWORD dwPlayerID);
    BOOL DoGetMailContent(DWORD dwPlayer, DWORD dwMailID);
    BOOL DoGetMailAttachment(DWORD dwPlayer, DWORD dwMailID, BOOL bGetMoney, BOOL bGetText, BOOL bGetItem);
    BOOL DoUpdateMailData(DWORD dwPlayerID, DWORD dwMailID, BOOL bMoneyFlag, BOOL bTextFlag, BOOL bItemFlag);
    BOOL DoSetMailRead(DWORD dwSrcID, DWORD dwMailID);
    BOOL DoDeleteMail(DWORD dwSrcID, DWORD dwMailID);
    BOOL DoReturnMail(DWORD dwSrcID, DWORD dwMailID);
    // ----------------------------------------------------------------------

	BOOL DoSyncMidMapMark(KPlayer* pSrc, int nMap, int nX, int nY, int nType, const char* pszComment);

    BOOL DoBroadcastWorldMessage(size_t uDataLen, const BYTE* pbyData);
    
    BOOL DoSaveScenePlayerList(KScene* pScene);
    BOOL DoSaveSceneData(KScene* pScene);
    BOOL DoResetMapCopyRequest(DWORD dwPlayerID, DWORD dwMapID, int nMapCopyIndex);
    BOOL DoPlayerEnterSceneNotify(DWORD dwPlayerID, DWORD dwMapID, int nMapCopyIndex);
    
    // ----------------------- 交通系统 -------------------------------------
    BOOL DoSyncRoadTrackForce(int nNodeID, DWORD dwForceID);
    
    //-----------------------------------------------------------------------

    // ----------------------- 角色相关操作 -------------------------------------    
    BOOL DoLoadRoleDataRequest(DWORD dwRoleID);

    BOOL DoChangeRoleLevelRequest(KPlayer* pPlayer);
    BOOL DoChangeRoleForceIDRequest(KPlayer* pPlayer);

    BOOL DoSendGmMessage(const char cszGmName[], const char cszMessage[]);

    BOOL DoSendGMCommandGS(const char cszRolename[], const char cszCommand[]);
    BOOL DoSendGMCommandGC(const char cszCommand[]);

    BOOL DoSyncRoleData(DWORD dwID, BYTE* pbyData, size_t uOffset, size_t uDataLen);
    BOOL DoSaveRoleData(KPlayer* pPlayer, size_t uRoleDataLen);

    //-----------------------------------------------------------------------

    // ----------------------- 战场排队相关操作 --------------------------------
    BOOL DoJoinBattleFieldQueueRequest(DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex, DWORD dwGroupID, BOOL bTeamJoin, DWORD dwJoinValue);
    BOOL DoLeaveBattleFieldQueueRequest(DWORD dwPlayerID, DWORD dwMapID);

    BOOL DoAcceptJoinBattleField(DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex);
    BOOL DoGetBattleFieldList(DWORD dwPlayerID, DWORD dwMapID);

    BOOL DoLeaveBattleFieldRequest(DWORD dwPlayerID);
    // -------------------------------------------------------------------------

    BOOL DoApplyCreatePQ(DWORD dwPQTemplateID);
    BOOL DoApplyDeletePQ(DWORD dwPQID);
    BOOL DoApplyChangePQValue(DWORD dwPQID, int nIndex, int nAddValue);

    BOOL DoAddCampScore(int nAddCampScore);
    BOOL DoApplySetCampRequest(DWORD dwPlayerID, KCAMP eNewCamp);

    // ----------------------- 帮会相关 --------------------------------
    BOOL DoApplyTongRosterRequest(DWORD dwPlayerID, int nLastUpdateFrame, DWORD dwMemberList[], int nMemberCount);
    BOOL DoApplyTongInfoRequest(DWORD dwPlayerID, int nLastUpdateFrame);
    BOOL DoApplyCreateTongRequest(DWORD dwPlayerID, const char cszTongName[], int nTemplateIndex);
    BOOL DoInvitePlayerJoinTongRequest(DWORD dwPlayerID, const char cszNewMemberName[]);
    BOOL DoInvitePlayerJoinTongRespond(DWORD dwPlayerID, DWORD dwInviterID, DWORD dwTongID, BOOL bAccept);
    BOOL DoApplyKickOutTongMemberRequest(DWORD dwPlayerID, DWORD dwMemberID);
    BOOL DoModifyTongInfoRequest(DWORD dwPlayerID, BYTE byInfoType, BYTE* pbyData, size_t uDataLen);
    BOOL DoModifyTongScheamRequest(DWORD dwPlayerID, BYTE byGroupIndex, BYTE byModifyType, BYTE* pbyData, size_t uDataLen);
    BOOL DoApplyQuiteTongRequest(DWORD dwPlayerID);

    BOOL DoChangeTongMemberGroupRequest(DWORD dwPlayerID, DWORD dwTargetMemberID, int nTargetGroup);
    BOOL DoChangeTongMasterRequest(DWORD dwPlayerID, DWORD dwNewMasterID);
    BOOL DoChangeTongMemberRemarkRequest(DWORD dwPlayerID, DWORD dwTargetMemberID, char szRemark[]);
    BOOL DoGetTongDescriptionRequest(DWORD dwPlayerID, DWORD dwTongID);
    BOOL DoChangeTongCampRequest(DWORD dwPlayerID, KCAMP eCamp);

    // ----------------------- 寄卖行相关 --------------------------------
    BOOL DoAuctionLookupRequest(BYTE byRequestID, DWORD dwPlayerID, KAUCTION_LOOKUP_PARAM* pParam);
    BOOL DoAuctionBidRequest(DWORD dwBidderID, DWORD dwSaleID, DWORD dwCRC, int nPrice);
    BOOL DoAuctionSellRequest(
        DWORD dwSellerID, char* pszSaleName, int nAucGenre, int nAucSub, int nRequireLevel, int nQuality, 
        int nStartPrice, int nBuyItNowPrice, int nCustodyCharges, int nLeftTime, size_t uItemDataLen, BYTE* pbyItemData
    );
    BOOL DoAuctionCancelRequest(DWORD dwPlayerID, DWORD dwSaleID);

    BOOL DoRemoteLuaCall(const char cszFunction[], BYTE* pbyParam, size_t uParamLen);

    BOOL DoAchievementGlobalAnnounceRequest(DWORD dwPlayerID, int nAchievementID);

    //AutoCode:-发送协议函数结束-
private:
	void OnHandshakeRespond(BYTE* pbyData, size_t uDataLen);

	void OnCreateMapNotify(BYTE* pbyData, size_t uDataLen);
    
    void OnFinishCreateMapNotify(BYTE* pbyData, size_t uDataLen);

	void OnDeleteMapNotify(BYTE* pbyData, size_t uDataLen);

	void OnSearchMapRespond(BYTE* pbyData, size_t uDataLen);

	void OnTransferPlayerRequest(BYTE* pbyData, size_t uDataLen);
	void OnTransferPlayerRespond(BYTE* pbyData, size_t uDataLen);

	void OnPlayerLoginRequest(BYTE* pbyData, size_t uDataLen);

	void OnConfirmPlayerLoginRespond(BYTE* pbyData, size_t uDataLen);

	void OnKickAccountNotify(BYTE* pbyData, size_t uDataLen);

    // ------------------ 组队相关 ----------------------------------------

    // Relay转发过来的玩家组队邀请
    void OnInvitePlayerJoinTeamRequest(BYTE* pbyData, size_t uDataLen);
    // Relay处理玩家的入队申请后，转发到这里
    void OnApplyJoinTeamRequest(BYTE* pbyData, size_t uDataLen);
    // Relay通知: 创建队伍
    void OnTeamCreateNotify(BYTE* pbyData, size_t uDataLen);
    // Relay通知: 将玩家加入到队伍
    void OnTeamAddMemberNotify(BYTE* pbyData, size_t uDataLen);
    // Relay通知: 将玩家从队伍删除
    void OnTeamDelMemberNotify(BYTE* pbyData, size_t uDataLen);
    // Relay通知: 队伍解散
    void OnTeamDisbandNotify(BYTE* pbyData, size_t uDataLen);
    // Relay通知: 设置队长
    void OnTeamChangeAuthorityNotify(BYTE* pbyData, size_t uDataLen);  
    // 队伍物品分配模式变更
    void OnTeamSetLootModeNotify(BYTE* pbyData, size_t uDataLen);
    // 上下线通知
    void OnSyncTeamMemberOnlineFlag(BYTE* pbyData, size_t uDataLen);
    // 队伍操作响应码
    void OnPartyMessageNotify(BYTE* pbyData, size_t uDataLen);
    void OnSetFormationLeaderNotify(BYTE* pbyData, size_t uDataLen);

    void OnTeamSyncMemberMaxLMR(BYTE* pbyData, size_t uDataLen);
    void OnTeamSyncMemberCurrentLMR(BYTE* pbyData, size_t uDataLen);
    void OnTeamSyncMemberMisc(BYTE* pbyData, size_t uDataLen);
    void OnTeamSyncMemberPosition(BYTE* pbyData, size_t uDataLen);

    void OnTeamSetMarkRespond(BYTE* pbyData, size_t uDataLen);

    void OnTeamCampChange(BYTE* pbyData, size_t uDataLen);

    void OnTeamLevelUpRaidNotify(BYTE* pbyData, size_t uDataLen);

    void OnTeamChangeMemberGroupNotify(BYTE* pbyData, size_t uDataLen);

    // ------------------ 聊天相关 ------------------------------------------

    void OnPlayerTalkError(BYTE* pbyData, size_t uDataLen);
    void OnPlayerTalkWhisperSuccessRespond(BYTE* pbyData, size_t uDataLen);
    void OnTalkMessage(BYTE* pbyData, size_t uDataLen);
    
    // ------------------ 好友相关 ------------------------------------------
    void OnApplyFellowshipDataRespond(BYTE* pbyData, size_t uDataLen);
    void OnAddFellowshipRespond(BYTE* pbyData, size_t uDataLen);
    void OnGetFellowshipNameRespond(BYTE* pbyData, size_t uDataLen);
    void OnSyncFellowshipMapID(BYTE* pbyData, size_t uDataLen);
    void OnSyncFellowshipPlayerLevel(BYTE* pbyData, size_t uDataLen);
    void OnSyncFellowshipPlayerForceID(BYTE* pbyData, size_t uDataLen);
    void OnAddFellowshipNotify(BYTE* pbyData, size_t uDataLen);

    // -------------------- 邮件系统　---------------------------------------
    void OnSendMailRespond(BYTE* pbyData, size_t uDataLen);
    void OnGetMailListRespond(BYTE* pbyData, size_t uDataLen);
    void OnGetMailContent(BYTE* pbyData, size_t uDataLen);
    void OnGetMailAttachment(BYTE* pbyData, size_t uDataLen);
    void OnNewMailNotify(BYTE* pbyData, size_t uDataLen);
    void OnMailGeneralRespond(BYTE* pbyData, size_t uDataLen);
    // ----------------------------------------------------------------------

	void OnSyncMidMapMark(BYTE* pbyData, size_t uDataLen);
    void OnMapCopyDoClearPlayerPrepare(BYTE* pbyData, size_t uDataLen);
    void OnMapCopyDoClearPlayer(BYTE* pbyData, size_t uDataLen);
    void OnSetMapCopyOwner(BYTE* pbyData, size_t uDataLen);
    // --------------------- 交通相关 ---------------------------------------
    void OnSyncRoadTrackForce(BYTE* pbyData, size_t uDataLen);

    // ----------------------------------------------------------------------

    // ----------------------- 角色相关操作 -------------------------------------
    void OnSaveRoleDataRespond(BYTE* pbyData, size_t uDataLen);

    void OnGmCommand(BYTE* pbyData, size_t uDataLen);

    void OnBroadcastSyncPQ(BYTE* pbyData, size_t uDataLen);
    void OnBroadcastDeletePQ(BYTE* pbyData, size_t uDataLen);
    void OnBroadcastPQValueChange(BYTE* pbyData, size_t uDataLen);
    void OnBroadcastPQFinish(BYTE* pbyData, size_t uDataLen);

    void OnSyncRoleData(BYTE* pbyData, size_t uDataLen);
    void OnLoadRoleData(BYTE* pbyData, size_t uDataLen);

    //-----------------------------------------------------------------------

    // ----------------------- 战场排队相关操作 -------------------------------
    void OnJoinBattleFieldQueueRespond(BYTE* pbyData, size_t uDataLen);
    void OnLeaveBattleFieldQueueRespond(BYTE* pbyData, size_t uDataLen);

    void OnBattleFieldNotify(BYTE* pbyData, size_t uDataLen);

    void OnSyncBattleFieldList(BYTE* pbyData, size_t uDataLen);

    void OnAcceptJoinBattleFieldRespond(BYTE* pbyData, size_t uDataLen);
    void OnLeaveBattleFieldRespond(BYTE* pbyData, size_t uDataLen);
    
    void OnSyncCampInfo(BYTE* pbyData, size_t uDataLen);
    void OnApplySetCampRespond(BYTE* pbyData, size_t uDataLen);

    // ----------------------- 帮会相关 -------------------------------
    void OnSyncTongMemberInfo(BYTE* pbyData, size_t uDataLen);
    void OnDeleteTongMemberNotify(BYTE* pbyData, size_t uDataLen);
    void OnApplyTongInfoRespond(BYTE* pbyData, size_t uDataLen);
    void OnUpdateTongClientDataVersion(BYTE* pbyData, size_t uDataLen);
    void OnInvitePlayerJoinTongRequest(BYTE* pbyData, size_t uDataLen);
    void OnChangePlayerTongNotify(BYTE* pbyData, size_t uDataLen);
    void OnTongEventNotify(BYTE* pbyData, size_t uDataLen);
    void OnBroadcastTongMessage(BYTE* pbyData, size_t uDataLen);
    void OnGetTongDescriptionRespond(BYTE* pbyData, size_t uDataLen);
    void OnSyncTongOnlineMessage(BYTE* pbyData, size_t uDataLen);

    // ----------------------- 寄卖行相关 -------------------------------
    void OnAuctionLookupRespond(BYTE* pbyData, size_t uDataLen);
    void OnAuctionBidRespond(BYTE* pbyData, size_t uDataLen);
    void OnAuctionCancelRespond(BYTE* pbyData, size_t uDataLen);
    void OnAuctionMessageNotify(BYTE* pbyData, size_t uDataLen);
    
    void OnRemoteLuaCall(BYTE* pbyData, size_t uDataLen);

    void OnAchievementGlobalAnnounceRespond(BYTE* pbyData, size_t uDataLen);

    void OnSyncGlobalSystemValue(BYTE* pbyData, size_t uDataLen);
    // ------------------------------------------------------------------------
    //AutoCode:-处理协议函数结束-
};

extern KRelayClient g_RelayClient;

#endif	//_SERVER
#endif	//_KRELAY_CLIENT_H_
