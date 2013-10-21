/************************************************************************/
/* 客户端与游戏服务器的连接												*/
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* Modify	 : Spe														*/
/* Remark	 : 不要随意修改分隔区块的注释，在自动生成代码的宏中需要使用 */
/* History	 :															*/
/*		2005.01.26	Create												*/
/************************************************************************/
#ifndef _KPLAYER_CLIENT_H_
#define _KPLAYER_CLIENT_H_

#if defined(_CLIENT)
#include <list>
#include <vector>
#include <map>
#include "Common/KG_Socket.h"
#include "IClient.h"
#include "GS_Client_Protocol.h"
#include "KIDCache.h"

#define	PACKET_MAX_SIZE			1024
#define PACKET_MAX_COUNT		512

class KSO3World;
class KCharacter;
class KPlayer;
class KNpc;
class KTarget;

class KPlayerClient
{
public:
	KPlayerClient(void);
	~KPlayerClient(void);

	BOOL Init(void);
	void UnInit(void);

	BOOL Connect(const char* pszIPAddr, int nPort);
	BOOL Disconnect();

	int  Breathe(void);

    int GetServerLoop(){ return m_nGSGameLoop; }
    int GetPingValue() { return (int)m_dwPingEchoTime; }

private:
    BOOL ProcessPackage(BYTE* pbyData, size_t uDataLen);
	BOOL Send(void* pvData, size_t nSize);

private:
    IKG_SocketStream*   m_piSocketStream;

	typedef void (KPlayerClient::*PROCESS_PROTOCOL_FUNC)(BYTE* pbyData, size_t nSize, int nFrame);
	PROCESS_PROTOCOL_FUNC	m_ProcessProtocolFuns[gs_client_connection_end];

	int m_nProtocolSize[gs_client_connection_end];

	// 最后的同步到的服务端帧数
	int		m_nGSGameLoop;
	// 累计帧数差
	float	m_fDeltaLoop;
	// 数据包缓冲
	struct KBufferPacket
	{
		int							m_nGameLoop;
		size_t						m_uSize;
		EXTERNAL_PROTOCOL_HEADER*	m_pHeader;
	};
	// 数据包缓冲队列
    std::list<IKG_Buffer*>          m_BufferPacketList;

	// 玩家显示数据的Cache
	KIDCache			m_PlayerDisplayDataCache;
	// Npc显示数据的Cache
	KIDCache			m_NpcDisplayDataCache;

    DWORD  m_dwNextPingTime;
    DWORD  m_dwPingEchoTime;

public:
    BOOL DoHandshakeRequest(DWORD dwRoleID, const GUID& crGuid);
	// 客户端场景加载完毕,请求进去场景
	BOOL DoApplyEnterScene();
	// 玩家登出
	BOOL DoPlayerLogout();
    // ping
    BOOL DoPingSignal();
    
    BOOL DoMoveCtrl(KPlayer* pPlayer, BOOL bMove, BOOL bWalk, int nDirection, int nTurn);
    // 申请同步校正
    BOOL DoApplyMoveAdjust();
    // 角色站立
	BOOL DoCharacterStand(KPlayer* pPlayer, int nCurrentState);
	// 角色跳跃
	BOOL DoCharacterJump(KPlayer* pPlayer, int nCurrentState, BOOL bStandJump, int nDirectionXY);
	// 角色坐下
	BOOL DoCharacterSit(KPlayer* pPlayer, int nCurrentState);
	// 角色朝向与俯仰
	BOOL DoSyncDirection(KPlayer* pPlayer);

    BOOL DoStopCurrentAction();
	// 角色放技能
	BOOL DoCharacterSkill(DWORD dwSkillID, DWORD dwSkillLevel, int nFaceDirection, KTarget* pTarget);
	// 角色交互动作
	BOOL DoCharacterAction(DWORD dwCharacterID, DWORD dwAction);

	// 请求玩家显示数据
	BOOL DoApplyPlayerDisplayData(KPlayer* pPlayer, DWORD dwCheckSum);

    BOOL DoMountKungfu(DWORD dwID, DWORD dwLevel);
    BOOL DoCancelBuff(int nIndex);

    BOOL DoSetSheathState(BOOL bSheathFlag);

    BOOL DoSetHideHatFlag(BOOL bHideHat);

    // 请求玩家Buff list数据，通常是在同步范围内的队友
    BOOL DoApplyPlayerBuffList(DWORD dwPlayerID);

    // -------------------- party about ------------------->

	// 邀请xxx组队
    BOOL DoInviteJoinTeamRequest(const char cszInvitePlayer[]);
    // 对组队邀请作出应答
    BOOL DoInviteJoinTeamRespond(const char cszInviteSrc[], BOOL bAgreeFlag);
    // 申请加入xxx的队伍
    BOOL DoApplyJoinTeamRequest(const char cszApplyDst[]);
    // 对入队申请作出应答
    BOOL DoApplyJoinTeamRespond(const char cszApplySrc[], BOOL bAgreeFlag);
    // 请求离开队伍
    BOOL DoLeaveTeamRequest();
    // 将xxx踢出队伍
    BOOL DoKickoutTeamMember(const char cszRoleName[]);
    // 请求移交队长
    BOOL DoTeamChangeAuthorityRequest(KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID);
    
    BOOL DoTeamSetLootMode(int nLootMode, KPlayer* pPlayer);
	BOOL DoTeamSetRollQuality(int nRollQuality, KPlayer* pPlayer);
    BOOL DoTeamSetFormationLeader(DWORD dwNewFormationLeaderID, int nGroupIndex, KPlayer* pPlayer);

    // 发送小地图上的标记给队友
    BOOL DoTeamMemberNotifySignpost(int nX, int nY);

    BOOL DoTeamSetMarkRequest(int nMarkType, DWORD dwTargetID);

    BOOL DoTeamLevelUpRaidRequest();

    BOOL DoTeamChangeMemberGroupRequest(DWORD dwMemberID, int nGroupIndex);

	// 接受任务
	BOOL DoAcceptQuest(KTarget* pTarget, DWORD dwQuestID);
	// 完成任务
	BOOL DoFinishQuest(DWORD dwQuestID, KTarget& rTarget, int nPresentChoice1, int nPresentChoice2);
	// 取消任务
	BOOL DoCancelQuest(int nQuestIndex);
	// 共享任务
	BOOL DoShareQuest(int nQuestIndex);
	// 接受护送任务
	BOOL DoAcceptEscortQuest(DWORD dwStartPlayerID, DWORD dwQuestID);

	BOOL DoGmCommand(const char cszCommand[]);

    // --------------------- shop about --------------------->
    BOOL DoOpenShopRequest(DWORD dwShopID);
    BOOL DoQueryShopItems(DWORD dwShopID, DWORD dwPageIndex);
    BOOL DoShopBuyRequest(DWORD dwShopID, DWORD dwPageIndex, DWORD dwPosIndex, DWORD dwItemID, int nCount, int nCost);
    BOOL DoShopSellRequest(DWORD dwShopID, DWORD dwItemID, int nCost, DWORD dwBoxIndex, DWORD dwPosIndex);
    BOOL DoShopRepairRequest(DWORD dwShopID, DWORD dwItemID, int nCost, DWORD dwBoxIndex, DWORD dwPosIndex);
    // 请求购回回购列表里的一样物品
    BOOL DoSoldListBuyRequest(DWORD dwShopID, DWORD dwX, DWORD dwItemID);

    // ---------------------- Trading ----------------------->
    BOOL DoTradingInviteRequest(DWORD dwInviteDst);
    BOOL DoTradingInviteRespond(BOOL bAccept);
    BOOL DoTradingAddItem(DWORD dwBoxIndex, DWORD dwPosIndex, DWORD dwGridIndex);
    BOOL DoTradingDeleteItem(DWORD dwGridIndex);
    BOOL DoTradingSetMoney(int nMoney);
    BOOL DoTradingConfirm(BOOL bConfirm, int nKey);

    BOOL DoRequestTarget(int nTargetType, DWORD dwTargetID);

	BOOL DoApplyExchangeItem(DWORD dwBox1, DWORD dwX1, DWORD dwBox2, DWORD dwX2, DWORD dwAmount);

	BOOL DoWindowSelect(DWORD dwIndex, BYTE bySelect);
	BOOL DoCastProfessionSkill(DWORD dwCraftID, DWORD dwRecipeID, DWORD dwRBookItemID, KTarget* pTarget);

    BOOL DoLootItem(DWORD dwDoodadID, DWORD dwItemID);
    BOOL DoRollItem(DWORD dwDoodadID, DWORD dwItemID, BOOL bCancel);

	BOOL DoApplyLootList(DWORD dwDoodadID);
	BOOL DoCharacterOpen(DWORD dwDoodadID);
    BOOL DoDeathRespond(int nRespondCode);
	BOOL DoApplyUseItem(DWORD dwBox, DWORD dwX, KTarget& rTarget);
	BOOL DoApplyFightFlag(DWORD dwForceID, BOOL bFight);
	BOOL DoFinishLoot(DWORD dwDoodadID);
	BOOL DoLootMoney(DWORD dwDoodadID);
	BOOL DoChangePkMode(int nMode);
	BOOL DoDistributeItem(DWORD dwDoodadID, DWORD dwItemID, DWORD dwDstPlayerID);

    BOOL DoTalkMessage(int nMsgType, const char cszReceiver[], size_t uDataLen, BYTE* pbyData);
    
    // --------------------- 好友相关 --------------------------->
    BOOL DoGetFellowshipListRequest(int nType); // Type 1: Friend, 2: Foe, 3: BlackList
    BOOL DoAddFellowshipRequest(const char cszAlliedPlayerName[], int nType);
    BOOL DoDelFellowshipRequest(DWORD dwAlliedPlayerID, int nType);
    BOOL DoAddFellowshipGroupRequest(const char cszGroupName[]);
    BOOL DoDelFellowshipGroupRequest(DWORD dwGroupID);
    BOOL DoRenameFellowshipGroupRequest(DWORD dwGroupID, const char cszGroupName[]);
    BOOL DoSetFellowshipRemarkRequest(DWORD dwAlliedPlayerID, const char cszRemark[]);
    BOOL DoSetFellowshipGroupRequest(DWORD dwAlliedPlayerID, DWORD dwGroupID);

    // --------------------- 邮件相关 -------------------------->
    BOOL DoSendMailRequest(int nRequestID, char szDestName[], DWORD dwBox, DWORD dwX, int nMoney, char szTitle[], char szText[]);
    BOOL DoGetMailListRequest();
    BOOL DoGetMailContent(DWORD dwMailID);
    BOOL DoGetMailAttachment(DWORD dwMailID, BOOL bGetMoney, BOOL bGetText, BOOL bGetItem);
    BOOL DoSetMailRead(DWORD dwMailID);
    BOOL DoDeleteMail(DWORD dwMailID);
    BOOL DoReturnMail(DWORD dwMailID);

	// --------------------- PK --------------------------->
	BOOL DoApplyPKOperate(BYTE byPKOperate, DWORD dwParam1, DWORD dwParam2);

	BOOL DoSyncUserPreferences(int nOffset, int nLength, BYTE* pbyData);
    BOOL DoSetBankOpenedFlag(BOOL bBankOpenedFlag, DWORD dwNpcID);
	BOOL DoEnableBankPackage();

	BOOL DoSyncMidMapMark(int nMapID, int nX, int nY, int nType, const char* pszComment);

    // --------------------- 技能学习 ------------------------->
	BOOL DoPlayerLearnSkill(DWORD dwSkillID, DWORD dwNpcID);
	BOOL DoPlayerLearnRecipe(DWORD dwCraftID, DWORD dwRecipeID, DWORD dwNpcID);

    // --------------------- 秘籍操作 ------------------------->
    BOOL DoPlayerActiveSkillRecipe(DWORD dwSkillRecipeKey, BOOL bActive);

    // --------------------------------------------------------->
    BOOL DoPeekOtherPlayerEquip(DWORD dwPlayerID);
    BOOL DoPeekOtherPlayerBook(DWORD dwPlayerID);
    BOOL DoPeekOtherPlayerQuest(DWORD dwPlayerID);

    // --------------------- 交通系统 -------------------------->
    BOOL DoRoadTrackStartOut(int nFromNode, int nTargetCity, int nCost);
    BOOL DoRoadTrackForceRequest();

    // --------------------- 经脉相关 -------------------------->
    BOOL DoOpenVenation(int nVenationID);

    BOOL DoSendGmMessage(const char cszMessage[]);

    BOOL DoJoinBattleFieldQueueRequest(DWORD dwNpcID, DWORD dwMapID, int nCopyIndex, DWORD dwGroupID, BOOL bTeamJoin);
    BOOL DoLeaveBattleFieldQueueRequest(DWORD dwMapID);

    BOOL DoAcceptJoinBattleField(DWORD dwMapID, int nCopyIndex);
    BOOL DoGetBattleFieldList(DWORD dwMapID);
    BOOL DoLeaveBattleField();

    // --------------------- 帮会相关 -------------------------->
    BOOL DoApplyTongRosterRequest();
    BOOL DoApplyTongInfoRequest();
    BOOL DoInvitePlayerJoinTongRequest(const char* cpszNewMemberName);
    BOOL DoInvitePlayerJoinTongRespond(DWORD dwInviterID, DWORD dwTongID, BOOL bAccept);
    BOOL DoApplyKickOutTongMember(DWORD dwMemberID);

    BOOL DoModifyTongInfoRequest(int nDataType, BYTE* pbyData, size_t uDataLen);
    BOOL DoModifyTongSchemaRequest(int nGroupIndex, int nModifyType, BYTE* byData, size_t uDataLen);

    BOOL DoApplyQuitTongRequest();

    BOOL DoChangeTongMemberGroupRequest(DWORD dwTargetMemberID, int nTargetGroup);
    BOOL DoChangeTongMasterRequest(DWORD dwNewMasterID);
    BOOL DoChangeMemberRemarkRequest(DWORD dwTargetMemberID, const char* cpszRemark);
    BOOL DoGetTongDescriptionRequest(DWORD dwTongID);
    // -----------------------------------------------------------
    BOOL DoApplyBattleFieldStatistics();
    
    BOOL DoSetHeroFlag(BOOL bHeroFlag);

    // --------------------- 寄卖行相关 -------------------------->
    BOOL DoAuctionLookupRequest(
        DWORD dwNpcID, BYTE byRequestID, const char* pszSaleName, int nAucGenre, int nAucSub, int nLevelMin,
        int nLevelMax, int nQuality, const char* pszSellerName, DWORD dwBidderID, int nTopPrice, int nAuction,
        int nStartIndex, BYTE byOrderType, BOOL bDesc
    );
    BOOL DoAuctionBidRequest(DWORD dwNpcID, DWORD dwSaleID, DWORD dwCRC, int nPrice);
    BOOL DoAuctionSellRequest(DWORD dwNpcID, DWORD dwBox, DWORD dwX, int nStartPrice, int nBuyItNowPrice, int nLeftHour);
    BOOL DoAuctionCancelRequest(DWORD dwNpcID, DWORD dwSaleID);
    
    BOOL DoApplySetCampRequest(KCAMP eNewCamp);

    BOOL DoRemoteLuaCall(const char cszFunction[], BYTE* pbyData, size_t uDataLen);

    BOOL DoPlayerReviveRequest(int nReviveCode);

    // --------------------- 成就系统相关 -------------------------->
    BOOL DoApplyAchievementDataRequest(DWORD dwPlayerID);
    BOOL DoQueryProductIdentity(int nMagic);
	//AutoCode:-发送协议函数结束-

private:
	// 通知消息大杂烩
	void OnMessageNotify(BYTE* pbyData, size_t nSize, int nFrame);

	// 同步玩家数据
	void OnSyncPlayerBaseInfo(BYTE* pbyData, size_t nSize, int nFrame);
	
	void OnSyncQuestData(BYTE* pbyData, size_t nSize, int nFrame);

	// 踢号通知
	void OnAccountKickout(BYTE* pbyData, size_t nSize, int nFrame);
	// 切换服务器的通知
	void OnSwitchGS(BYTE* pbyData, size_t nSize, int nFrame);
    // 切换地图
    void OnSwitchMap(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncNewPlayer(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncNewNpc(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncNewDoodad(BYTE* pbyData, size_t nSize, int nFrame);

	void OnRemoveCharacter(BYTE* pbyData, size_t nSize, int nFrame);
    void OnRemoveDoodad(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncMoveCtrl(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncMoveParam(BYTE* pbyData, size_t nSize, int nFrame);
    void OnAdjustPlayerMove(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncMoveState(BYTE* pbyData, size_t nSize, int nFrame);

	void OnCharacterDeath(BYTE* pbyData, size_t nSize, int nFrame);
	void OnMoveCharacter(BYTE* pbyData, size_t nSize, int nFrame);
    void OnPlayerRevivedNotify(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步角色动作
	void OnCharacterAction(BYTE* pbyData, size_t nSize, int nFrame);

    void OnOverTimeAction(BYTE* pbyData, size_t nSize, int nFrame);

	// 角色显示数据
	void OnPlayerDisplayData(BYTE* pbyData, size_t nSize, int nFrame);

	// 增加经验值
	void OnSyncExperience(BYTE* pbyData, size_t nSize, int nFrame);

    void OnCommonHealth(BYTE* pbyData, size_t nSize, int nFrame);

    void OnPlayerLevelUp(BYTE* pbyData, size_t nSize, int nFrame);

	void OnSyncDoodadState(BYTE* pbyData, size_t nSize, int nFrame);

	void OnFrameSignal(BYTE* pbyData, size_t nSize, int nFrame);

    void OnTerrainTest(BYTE* pbyData, size_t nSize, int nFrame);
    void OnPingSignal(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncSkillData(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncSkillRecipe(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSkillPrepare(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSkillCast(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncCommonSkillAutoCastState(BYTE* pbyData, size_t nSize, int nFrame);
    void OnResetCooldown(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSkillChainEffect(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSkillEffectResult(BYTE* pbyData, size_t nSize, int nFrame);   
    void OnSyncBehitRepresent(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncBuffList(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncBuffSingle(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncRepresentState(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncKungfuMount(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSkillBeatBack(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncSheathState(BYTE* pbyData, size_t nSize, int nFrame);

    // --------------------- 数值同步 --------------------------

    void OnSyncSelfMaxLMRS(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncSelfCurrentLMRS(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncSelfWeakInfo(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncSelectCharacterMaxLMR(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncSelectCharacterCurrentLMR(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncSelectCharacterWeakInfo(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncTeamMemberMaxLMR(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncTeamMemberCurrentLMR(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncTeamMemberMisc(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncTeamMemberPosition(BYTE* pbyData, size_t nSize, int nFrame);
	void OnBroadcastCharacterLife(BYTE* pbyData, size_t nSize, int nFrame);

    // --------------------- party about -----------------------

    // 同步队伍基本信息
    void OnSyncPlayerTeamBaseInfo(BYTE* pbyData, size_t nSize, int nFrame);
    // xxx邀请你组队
    void OnInvitePlayerJoinTeamRequest(BYTE* pbyData, size_t nSize, int nFrame);
    // xxx申请加入你的队伍
    void OnApplyJoinTeamRequest(BYTE* pbyData, size_t nSize, int nFrame);
    // 队伍新加队员的通知
    void OnTeamAddMemberNotify(BYTE* pbyData, size_t nSize, int nFrame);
    // 成员离开队伍的通知
    void OnTeamDelMemberNotify(BYTE* pbyData, size_t nSize, int nFrame);
    void OnTeamChangeAuthorityNotify(BYTE* pbyData, size_t nSize, int nFrame);
    void OnTeamSetLootModeNotify(BYTE* pbyData, size_t nSize, int nFrame);
    
    void OnTeamSetFormationLeaderNotify(BYTE* pbyData, size_t nSize, int nFrame);

    // 队伍解散通知
    void OnTeamDisbandNotify(BYTE* pbyData, size_t nSize, int nFrame);
    // 队员上下线通知
    void OnSyncTeamMemberOnlineFlag(BYTE* pbyData, size_t nSize, int nFrame);
    // 同步小地图标注点
    void OnTeamMemberSignpostNotify(BYTE* pbyData, size_t nSize, int nFrame);
    // 小队标记
    void OnTeamSetMarkRespond(BYTE* pbyData, size_t nSize, int nFrame);
    // 小队阵营
    void OnTeamCampChange(BYTE* pbyData, size_t nSize, int nFrame);
    // 升级为团队
    void OnTeamLevelUpRaidNotify(BYTE* pbyData, size_t nSize, int nFrame);
    // 成员更换位置
    void OnTeamChangeMemberGroupNotify(BYTE* pbyData, size_t nSize, int nFrame);
    // <--------------------- party about ---------------------


    // --------------------- shop about --------------------->
    void OnOpenShopNotify(BYTE* pbyData, size_t nSize, int nFrame);
    void OnUpdateShopItem(BYTE* pbyData, size_t nSize, int nFrame);
    void OnShopBuyRespond(BYTE* pbyData, size_t nSize, int nFrame);
    // <--------------------- shop about ---------------------
    
    // ---------------------- Trading ----------------------->
    void OnTradingInviteRequest(BYTE* pbyData, size_t nSize, int nFrame);
    void OnOpenTradingBoxNotify(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncTradingConfirm(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncTradingItemData(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncTradingMoney(BYTE* pbyData, size_t nSize, int nFrame);
    void OnTradingComplete(BYTE* pbyData, size_t nSize, int nFrame);
    // <--------------------- Trading ------------------------

	// 同步金钱数量
	void OnSyncMoney(BYTE* pbyData, size_t nSize, int nFrame);
    // 同步贡献值
    void OnSyncContribution(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步道具的临时数据
	void OnSyncTempItemData(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步道具数据
	void OnSyncItemData(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步道具删除
	void OnSyncItemDestroy(BYTE* pbyData, size_t nSize, int nFrame);
	// 交换装备
	void OnExchangeItem(BYTE* pbyData, size_t nSize, int nFrame);
    // 换装广播
    void OnSyncEquipRepresent(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步道具获得的消息
	void OnAddItemNotify(BYTE* pbyData, size_t nSize, int nFrame);
	// 更新物品数量或耐久
	void OnUpdateItemAmount(BYTE* pbyData, size_t nSize, int nFrame);
	// 学习生活技能
	void OnLearnProfessionNotify(BYTE* pbyData, size_t nSize, int nFrame);
	// 学习生活技能分支
	void OnLearnBranchNotify(BYTE* pbyData, size_t nSize, int nFrame);
	// 遗忘技能
	void OnForgetProfessionNotify(BYTE* pbyData, size_t nSize, int nFrame);
	// 增加生活技能经验
	void OnAddProfessionProficiency(BYTE* pbyData, size_t nSize, int nFrame);
	// 设置生活技能最大等级
	void OnSetMaxProfessionLevelNotify(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步更新玩家生活技能
	void OnUpdatePlayerProfession(BYTE* pbyData, size_t nSize, int nFrame);
	// 学习配方
	void OnLearnRecipeNotify(BYTE* pbyData, size_t nSize, int nFrame);
	// 遗忘配方
	void OnForgetRecipeNotify(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步配方列表
	void OnSyncRecipeState(BYTE* pbyData, size_t nSize, int nFrame);
	// 打开一个客户端窗口
	void OnOpenWindow(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步配方的使用
	void OnStartUseRecipe(BYTE* pbyData, size_t nSize, int nFrame);

	void OnAcceptQuest(BYTE* pbyData, size_t nSize, int nFrame);
	void OnFinishQuest(BYTE* pbyData, size_t nSize, int nFrame);
	void OnCancelQuest(BYTE* pbyData, size_t nSize, int nFrame);
    void OnClearQuest(BYTE* pbyData, size_t nSize, int nFrame);
	void OnShareQuest(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步任务变量
	void OnSyncQuestValue(BYTE* pbyData, size_t nSize, int nFrame);
	void OnStartEscortQuest(BYTE* pbyData, size_t nSize, int nFrame);

	// 同步拾取列表
	void OnSyncLootList(BYTE* pbyData, size_t nSize, int nFrame);
	// 掷骰子信息
	void OnRollItemMessage(BYTE* pbyData, size_t nSize, int nFrame);
	// 角色拾取物品
	void OnCharacterPick(BYTE* pbyData, size_t nSize, int nFrame);
	// 打开拾取面板
	void OnOpenLootList(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步战斗开关
	void OnSyncFightFlag(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步声望
	void OnSyncRepute(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步所有势力战斗开关
	void OnSyncFightflagList(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步所有的势力声望
	void OnSyncReputeList(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步声望修改标记
	void OnShowReputation(BYTE* pbyData, size_t nSize, int nFrame);

	void OnCharacterOpen(BYTE* pbyData, size_t nSize, int nFrame);
    void OnDistributeItemMessage(BYTE* pbyData, size_t nSize, int nFrame);

	// 结束拾取
	void OnFinishLoot(BYTE* pbyData, size_t nSize, int nFrame);
	// start roll
	void OnBeginRollItem(BYTE* pbyData, size_t nSize, int nFrame);
	// 同步定制的用户消息
	void OnCustomOTAction(BYTE* pbyData, size_t nSize, int nFrame);
	// 添加仇恨
	void OnModifyThreat(BYTE* pbyData, size_t nSize, int nFrame);
	// 修改被仇恨
	void OnModifyThreaten(BYTE* pbyData, size_t nSize, int nFrame);
	// 清除仇恨
	void OnClearThreat(BYTE* pbyData, size_t nSize, int nFrame);
	// 清除被仇恨
	void OnClearThreaten(BYTE* pbyData, size_t nSize, int nFrame);

    void OnTalkMessage(BYTE* pbyData, size_t nSize, int nFrame);

	void OnSyncSelfCurrentST(BYTE* pbyData, size_t nSize, int nFrame);

	void OnDoodadLootOver(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncFellowshipList(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncFellowshipGroupName(BYTE* pbyData, size_t nSize, int nFrame);

	void OnNpcTalkToPlayer(BYTE* pbyData, size_t nSize, int nFrame);
	void OnSetForce(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSendMailRespond(BYTE* pbyData, size_t nSize, int nFrame);
    void OnGetMailListRespond(BYTE* pbyData, size_t nSize, int nFrame);
    void OnNewMailNotify(BYTE* pbyData, size_t nSize, int nFrame);
    void OnGetMailContentRespond(BYTE* pbyData, size_t nSize, int nFrame);
    void OnMailGeneralRespond(BYTE* pbyData, size_t nSize, int nFrame);
    void OnMailReleaseAttachment(BYTE* pbyData, size_t nSize, int nFrame);

	void OnSyncForceId(BYTE* pbyData, size_t nSize, int nFrame);
	void OnSyncUserPreferences(BYTE* pbyData, size_t nSize, int nFrame);
	void OnSyncAllBookState(BYTE* pbyData, size_t nSize, int nFrame);
	void OnSyncBookState(BYTE* pbyData, size_t nSize, int nFrame);
	void OnCharacterChangeModel(BYTE* pbyData, size_t nSize, int nFrame);
	void OnSyncNpcDialogFlag(BYTE* pbyData, size_t nSize, int nFrame);

	void OnSyncPKState(BYTE* pbyData, size_t nSize, int nFrame);
	void OnWinDuelBroadcast(BYTE* pbyData, size_t nSize, int nFrame);
	void OnCancelDuelRespond(BYTE* pbyData, size_t nSize, int nFrame);

	void OnSlayRespond(BYTE* pbyData, size_t nSize, int nFrame);
    
	void OnSyncRoleDataOver(BYTE* pbyData, size_t nSize, int nFrame);

	void OnSyncEnableBankPackage(BYTE* pbyData, size_t nSize, int nFrame);

	void OnUpdateMapMark(BYTE* pbyData, size_t nSize, int nFrame);
	void OnSyncMidMapMark(BYTE* pbyData, size_t nSize, int nFrame);

	void OnPlaySound(BYTE* pbyData, size_t nSize, int nFrame);
	void OnPlayMiniGame(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncHorseFlag(BYTE* pbyData, size_t nSize, int nFrame);

    // 交通系统相关
    void OnRoadTrackForceRespond(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncRouteNodeOpenList(BYTE* pbyData, size_t nSize, int mFrame);

    // 同步玩家状态数据
    void OnSyncPlayerStateInfo(BYTE* pbyData, size_t nSize, int mFrame);

    // 同步玩家的修为
    void OnSyncPlayerTrain(BYTE* pbyData, size_t nSize, int mFrame);

    void OnBroadcastPlayerSchoolID(BYTE* pbyData, size_t nSize, int mFrame);

    void OnSyncPrinkForce(BYTE* pbyData, size_t nSize, int mFrame);
    
    void OnSyncVisitMapInfo(BYTE* pbyData, size_t nSize, int mFrame);

    void OnSyncAccumulateValue(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSetTarget(BYTE* pbyData, size_t nSize, int nFrame);
    
    void OnSyncKillPoint(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSetPlayerRedName(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSetPlayerDialogFlag(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSetOnlyReviveInSitu(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncFormationCoefficient(BYTE* pbyData, size_t nSize, int nFrame);

    //------------------------ BATTLE_FIELD ------------------------------------
    void OnJoinBattleFieldQueueRespond(BYTE* pbyData, size_t nSize, int nFrame);
    void OnLeaveBattleFieldQueueRespond(BYTE* pbyData, size_t nSize, int nFrame);

    void OnBattleFieldNotify(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncBattleFieldList(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncBattleFieldSide(BYTE* pbyData, size_t nSize, int nFrame);

    void OnBattleFieldEnd(BYTE* pbyData, size_t nSize, int nFrame);
    //--------------------------------------------------------------------------

    void OnSyncCampInfo(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncCurrentPrestige(BYTE* pbyData, size_t nSize, int nFrame);

    //------------------------      帮会    ------------------------------------
    void OnSyncTongMemberInfo(BYTE* pbyData, size_t nSize, int nFrame);
    void OnTongDeleteMemberNotify(BYTE* pbyData, size_t nSize, int nFrame);
    void OnApplyTongInfoRespond(BYTE* pbyData, size_t nSize, int nFrame);
    void OnUpdateTongClientDataVersion(BYTE* pbyData, size_t nSize, int nFrame);
    void OnInvitePlayerJoinTongRequest(BYTE* pbyData, size_t nSize, int nFrame);
    void OnChangePlayerTongNotify(BYTE* pbyData, size_t nSize, int nFrame);
    void OnBroadcastTongMessage(BYTE* pbyData, size_t nSize, int nFrame);
    void OnGetTongDescriptionRespond(BYTE* pbyData, size_t nSize, int nFrame);
    void OnBroadcastPlayerChangeTong(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncTongOnlineMessage(BYTE* pbyData, size_t nSize, int nFrame);
    //--------------------------------------------------------------------------

    //------------------------寄卖行相关------------------------------------
    void OnAuctionLookupRespond(BYTE* pbyData, size_t uSize, int nFrame);
    void OnAuctionBidRespond(BYTE* pbyData, size_t uSize, int nFrame);
    void OnAuctionSellRespond(BYTE* pbyData, size_t uSize, int nFrame);
    void OnAuctionCancelRespond(BYTE* pbyData, size_t uSize, int nFrame);
    void OnAuctionMessageNotify(BYTE* pbyData, size_t uSize, int nFrame);

    void OnSetCamp(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncBattleFieldStatistics(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncBattleFieldObjective(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncBattleFieldPQInfo(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncHeroFlag(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncNpcDropTarget(BYTE* pbyData, size_t nSize, int nFrame);
    void OnRemoteLuaCall(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncPlayerReviveCtrl(BYTE* pbyData, size_t nSize, int nFrame);

    //------------------------成就系统相关----------------------------
    void OnSyncAchievementData(BYTE* pbyData, size_t nSize, int nFrame);
    void OnNewAchievementNotify(BYTE* pbyData, size_t nSize, int nFrame);
    void OnAchievementAnnounceNotify(BYTE* pbyData, size_t nSize, int nFrame);
    void OnSyncAchievementPoint(BYTE* pbyData, size_t nSize, int nFrame);

    void OnSyncMaxLevel(BYTE* pbyData, size_t nSize, int nFrame);
    void OnUseItemPrepare(BYTE* pbyData, size_t nSize, int nFrame);
	//AutoCode:-处理协议函数结束-
};

extern KPlayerClient g_PlayerClient;

#endif	//_CLIENT
#endif	//_KPLAYER_CLIENT_H_
