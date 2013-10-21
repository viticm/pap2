#ifndef _GAMESERVERSIMULATOR_H
#define _GAMESERVERSIMULATOR_H
#include "ProtocolBasic.h"
#include "Common/KG_Memory.h"
#include "Common/KG_Socket.h"
#include "KRoleDBDataDef.h"
#include "Relay_GS_Protocol.h"
#include "KSimulateSocket.h"
#include "SO3Result.h"


#define  GAME_WORLD_CURRENT_VERSION 61
#define  GAME_WORLD_LOWEST_VERSION  61

struct MapData
{
	DWORD dwMapID;
	int nMapCopy;
};

class KGameServerSimulator : public KSimulateSocket
{
public:
	KGameServerSimulator(void);
	~KGameServerSimulator(void);
	bool Init();
	bool DoPingSignal();
	void ClearReceiveData();
    bool DoInitializeData(int nGameWorldLowerVersion, int nGameWorldUpperVersion);
	bool DoCreateMapRespond(DWORD dwMapID, __int64 nMapCopyIndex, bool bResult);
	bool DoPlayerLoginRespond(DWORD dwPlayerID, bool bPermit);
	bool DoSearchMapRequset(DWORD dwPlayerID, DWORD dwMapID, __int64 nMapCopyIndex, int nPosX, int nPosY, int nPosZ);
	bool DoTranferPlayerRequest(KROLE_LIST_INFO& RoleBaseInfo);
	bool DoTranferPlayerRequest(DWORD dwPlayerID, const char szAccount[], const char szRoleName[], DWORD dwMapID, __int64 nMapCopyID);
	bool DoTranferPlayerRespond(DWORD	dwPlayerID, bool bSucceed);
	bool OnCreateMapRequest(IKG_Buffer* piRecePacket);
	bool DoConfirmPlayerLoginRequest(DWORD dwPlayerID);
	bool DoPlayerLeaveGS(DWORD dwPlayerID);

	bool DoInvitePlayerJoinPartyRequest(DWORD dwInviteSrc, const char szInviteRole[]);
	bool DoInvitePlayerJoinPartyRespond(const char  szInviteSrc[], DWORD dwInviteDst, bool  bAgreeFlag, GUID  guid);
	bool DoApplyJoinPartyRequest(DWORD dwApplySrc, const char  szApplyDst[]);
	bool DoApplyJoinPartyRespond(const char  szApplySrc[], DWORD dwApplyDst, bool  bAgreeFlag, GUID  guid);
	bool DoPartyDelMemberRequest(DWORD dwPartyID, DWORD dwRequestSrc, DWORD dwMemberID);
	bool DoPartyChangeAuthorityRequest(DWORD dwRequestSrc, BYTE byType, DWORD dwTargetID);
	bool DoPartyDisbandRequest(DWORD dwLeaderID, DWORD dwPartyID);
	bool DoPartySyncMemberMaxLMR(DWORD dwPartyID, DWORD dwMemberID, int nMaxLife, int nMaxMana);
	bool DoPartySyncMemberCurrentLMR(DWORD dwPartyID, DWORD dwMemberID, BYTE byLifePercent, BYTE byManaPercent, BYTE byRagePercent);
	bool DoPartySyncMemberMisc(DWORD dwPartyID,DWORD dwMemberID, DWORD dwPortraitID, DWORD dwForceID, BYTE byLevel, BYTE byRoleType);
	bool DoPartySyncMemberPosition(DWORD dwPartyID, DWORD dwMemberID, DWORD dwMapID, INT64 nMapCopyIndex, int nPosX, int nPosY);
    bool DoPartySetLootModeRequest(DWORD dwPartyID, DWORD dwRequestSrc, int   nLootMode);
    bool DoPartySetRollQualityRequest(DWORD dwPartyID, DWORD dwRequestSrc, int nRollQuality);
    bool DoPartySetFormationLeader(DWORD dwPartyID, DWORD dwRequestSrc, DWORD dwNewFormationLeader,  int   nGourpIndex);


	bool DoApplyFellowshipDataRequest(DWORD dwPlayerID);
	bool DoUpdateFellowshipData(DWORD dwPlayerID, BYTE    byData[], size_t uDataLen);
	bool DoAddFellowshipRequest(DWORD dwPlayerID, const char szAlliedPlayerName[]);
	bool DoGetFellowshipNameRequest(DWORD   dwPlayerID, BYTE byCount, DWORD   dwAlliedPlayerIDArray[]);
    bool DoApplyFellowshipPlayerLevel(DWORD dwPlayerID);

	bool CheckApplyFellowshipDataRespond( DWORD   dwPlayerID, BYTE  byData[], unsigned int uLength);
	bool CheckAddFellowshipRespond(DWORD dwPlayerID, DWORD dwAlliedPlayerID);
	bool CheckGetFellowshipNameRespond(DWORD dwPlayerID, int nArrayLength, DWORD dwPalyerIDArray[]);
    bool CheckSyncFellowshipMapID(DWORD dwPlayerID, DWORD dwMapID);
    bool CheckSyncFellowshipPlayerLevel(DWORD dwPlayerID, int nLevel);

	bool CheckInvitePlayerJoinPartyRequest(const char szInviteSrc[], DWORD dwInviteDst, GUID&  guid);
	bool CheckApplyJoinPartyRequest(DWORD dwApplyDst, const char  szApplySrc[], GUID&  guid);
	bool CheckPartyCreateNotify(DWORD* dwPartyID, DWORD dwLeaderID, char  szLeaderName[]);
	bool CheckPartyAddMemberNotify(DWORD dwPartyID, DWORD dwMemberID);
	bool CheckPartyChangeAuthorityNotify(DWORD dwPartyID, BYTE byType, DWORD dwNewID);
	bool CheckPartyDelMemberNotify(DWORD dwPartyID, DWORD dwMemberID);
	bool CheckPartyDisbandNotify(DWORD dwPartyID);
	bool CheckSyncPartyMemberOnlineFlag(DWORD dwPartyID, DWORD dwMemberID, bool  bOnlineFlag);
	bool CheckPartySyncMemberMaxLMR(DWORD dwPartyID, DWORD dwMemberID, int nMaxLife, int nMaxMana);
	bool CheckPartySyncMemberCurrentLMR(DWORD dwPartyID, DWORD dwMemberID, BYTE byLifePercent, BYTE byManaPercent, BYTE byRagePercent);
	bool CheckPartySyncMemberMisc(DWORD dwPartyID,DWORD dwMemberID, DWORD dwPortraitID, DWORD dwForceID, BYTE byLevel, BYTE byRoleType);
	bool CheckPartySyncMemberPosition(DWORD dwPartyID, DWORD dwMemberID, DWORD dwMapID, INT64 nMapCopyIndex, int nPosX, int nPosY);
	bool CheckPartyMessageNotify(DWORD dwPlayerID, int nCode);
    bool CheckPartySetLootModeRequest(DWORD dwPartyID, int nLootMode);
    bool CheckPartySetRollQualityRequest(DWORD dwPartyID, int nRollQuality);
    bool CheckPartySetFormationLeader(DWORD dwPartyID, DWORD dwNewFormationLeader);

	bool CheckDeleteMapNotify(DWORD dwMapID, __int64 nMapCopyIndex);

	bool CheckPlayerLoginRequset(DWORD dwPlayerID, DWORD dwMapID, __int64  nMapCopyIndex);
	bool CheckCreateMapNotify(DWORD dwMapID, __int64  nMapCopyIndex);
    bool CheckCreateMapNotify(DWORD dwMapID, __int64* nMapCopyIndex);
    bool CheckSearchMapRespond(DWORD dwPlayerID, SWITCH_MAP_RESULT_CODE nResultCode, bool bSwitchServer, DWORD dwMapID, __int64 nMapCopyIndex);
    bool CheckTransferPlayerRequest(DWORD dwPlayerID, DWORD dwMapID, __int64 nMapCopyIndex);
	bool CheckTransferPlayerRespond(DWORD dwPlayerID, bool bSucceed);
	bool CheckConfirmPlayerLoginRespond(DWORD dwPlayerID, bool  bPermit);

    bool DoSaveSceneRequest(DWORD dwMapID, __int64 nMapCopyIndex, char* pSaveData, size_t uLength);
    bool DoGetMapDataResquest(DWORD dwMapID, __int64 nMapCopyIndex);
    bool CheckApplyCustmonSceneDataRespond(DWORD dwMapID, __int64 nMapCopyIndex, char* pSaveData, size_t uLength);
    bool DoResetMapCopyRequest(DWORD dwPlayerID, DWORD dwMapID, __int64  nMapCopyIndex);
    bool CheckRefreshMapCopyPrepare(DWORD dwMapID, __int64 nMapCopyIndex);

    bool DoPlayerEnterSceneNotify(DWORD dwPlayerID, DWORD dwMapID, __int64 nMapCopyIndex);
    bool CheckSetMapOwer(DWORD dwPlayerID, DWORD dwMapID, __int64 nMapCopyIndex);


    bool DoSaveRoleData(DWORD dwPlayerID, int nUserValue, KRoleBaseInfo& BaseInfo, size_t uRoleDataLen);
	bool DoSyncRoleData(DWORD   dwRoleID, size_t  uOffset, BYTE byData[], size_t uLength);
    bool DoLoadRoleData(DWORD dwPlayerID);
    bool CheckSaveRoleDataRespond(DWORD dwPlayerID, bool bSucceed, int nUserValue);
    bool CheckLoadRoleDataRespond(DWORD dwPlayerID, bool bSucceed, BYTE* pbyRoleData, unsigned uRoleDataLen);


    bool CheckGmCommand(DWORD dwPlayerID, const char cszGmName[], const char cszGmCommand[]);
    bool DoUpdatePerformance(int nIdleFrame, size_t uMemory, int nConnectionCount);

    bool DoRefreshMapCopyRequest(DWORD dwMapID, int64_t  nMapCopyIndex);
    bool DoApplyCustomSceneDataRequest(DWORD dwMapID, int64_t  nMapCopyIndex);
    
    void CloseConnect();	
    
    bool OnInitializeInfo();

    bool CheckKickOut(DWORD dwPlayerID);

    bool CheckGlobalSysMsg(BYTE  byMsgType, DWORD dwTalkerID, const char  szTalker[], 
        DWORD dwReceiverID, const char  szReceiver[], BYTE  byTalkData[]);

	bool DoHandshakeRequest();
	bool CheckHandshakeRespond();

	bool DoJoinBattleFieldQueueRequest(DWORD dwMapID, int64_t nCopyIndex, DWORD dwGroupID, bool bTeamJoin, DWORD dwRoleID);
	bool CheckJoinBattleFieldQueueRespond(DWORD dwPlayerID, DWORD dwMapID, int nErrorCode);

	bool DoChangeRoleLevelRequest(DWORD dwPlayerID, int nLevel);

	bool DoChangeRoleForceIDRequest(DWORD dwPlayerID, BYTE  byForceID);
	bool CheckSyncFellowshipPlayerForceID(DWORD dwPlayerID, BYTE  byForceID);

	bool CheckBattleFieldNotify();

	bool CheckBattleFieldNotify(R2S_BATTLE_FIELD_NOTIFY& BattleFifldNotify);

	bool DoAcceptJoinBattleField(DWORD   dwMapID, int64_t nCopyIndex, DWORD   dwRoleID);

	bool CheckAcceptJoinBattleFieldRespond(DWORD  dwMapID, int64_t nCopyIndex, DWORD   dwRoleID);

	bool DoLeaveBattleFieldQueueRequest(DWORD dwMapID, DWORD dwRoleID);
	bool CheckLeaveBattleFieldQueueRespond(    DWORD   dwPlayerID, DWORD   dwMapID, int     nErrorCode);

	bool DoGetBattleFieldList(DWORD dwPlayerID, DWORD dwMapID);
	bool CheckSyncBattleFieldList(DWORD   dwPlayerID, DWORD   dwMapID, int64_t nCopyIndex);

	bool CheckSyncPQ(R2S_SYNC_PQ& PQInfo);
	bool CheckDeletePQ(DWORD dwPQID);

	bool CheckMapCopyDoClearPlayerPrepare(DWORD dwMapID, int64_t  nMapCopyIndex, int      nType, int      nDelayTime);
	bool CheckMapCopyDoClearPlayer(DWORD dwMapID, int64_t  nMapCopyIndex);
	bool DoAuctionSellRequest(S2R_AUCTION_SELL_REQUEST& AuctionSellRequest);
	bool DoAuctionLookupRequest(S2R_AUCTION_LOOKUP_REQUEST& AuctionLookUpRequest);
	bool DoAuctioBidRequest(DWORD dwBidderID, DWORD dwSaleID, int nPrice, DWORD dwCRC);
	bool DoAuctionCancelRequest(DWORD dwPlayerID, DWORD dwSaleID);
	
	bool CheckAuctionMessageNotify( DWORD dwPlayerID, BYTE byCode, char szSaleName[], int nPrice);
	bool CheckAuctionCancelRespond( DWORD dwPlayerID, BYTE byCode);
	bool CheckAuctionBidRespond(DWORD dwPlayerID, BYTE byCode, int nBidPrice);
	bool CheckAuctionLookUpRespond(R2S_AUCTION_LOOKUP_RESPOND& AuctionLookUpRespond);

	bool DoSyncMentorData(DWORD dwMentorID, DWORD dwApprenticeID);
	int GetProtocolIndex(IKG_Buffer* pBuffer);

	std::vector<MapData> m_CreatedMapCopy;
};

#endif //_GAMESERVERSIMULATOR_H