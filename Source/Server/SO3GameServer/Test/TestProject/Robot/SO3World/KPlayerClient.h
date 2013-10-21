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

#include <vector>
#include <map>
#include "Common/KG_Socket.h"
#include "IClient.h"
#include "GS_Client_Protocol.h"
#include "KIDCache.h"
#include "KCircleList.h"

#define	PACKET_MAX_SIZE			1024
#define PACKET_MAX_COUNT		512

class KSO3World;
class KCharacter;
class KPlayer;
class KNpc;
class KTarget;
class KMonster;

enum GAMSERVER_STATE
{
    gmsInvalid,
    gmsOnline,
    gmsConnectFail,
    gmsSyncDataFail,
    gmsDisconnect,
    gmsTotal
};

class KPlayerClient
{
public:
	KPlayerClient(void);
	~KPlayerClient(void);

	BOOL Init(KMonster *pOwner);
	BOOL UnInit(void);

	BOOL Connect(const char* pszIPAddr, int nPort);
	BOOL Disconnect();

	int Breathe(void);

    KMonster *m_pOwner;

    GAMSERVER_STATE GetOnlineState(){return m_nState;}
    const char *GetStateDecription(){return ms_szStateDecription[m_nState];}

private:
    GAMSERVER_STATE m_nState;
    static char ms_szStateDecription[gmsTotal][265];

public:
	//请求玩家数据
	BOOL DoApplyPlayerData(char* pszRoleName, GUID Guid);
	//确认同步玩家数据完成
	BOOL DoAckPlayerData(void);
	//玩家登出
	BOOL DoPlayerLogout(void);
    
    // 角色站立
	BOOL DoCharacterStand(KCharacter* pCharacter);
	// 角色走动
	BOOL DoCharacterWalk(KCharacter* pCharacter, int nDstX, int nDstY);
	//角色跑动
	BOOL DoCharacterRun(KCharacter* pCharacter, int nDstX, int nDstY);
    // 角色游泳
    BOOL DoCharacterSwim(KCharacter* pCharacter, int nDstX, int nDstY, int nDstZ);
	// 角色跳跃
	BOOL DoCharacterJump(KCharacter* pCharacter, BOOL bStandJump, int nDirectionXY);
	// 角色坐下
	BOOL DoCharacterSit(KCharacter* pCharacter);

    BOOL DoStopCurrentAction();
	// 角色放技能
	BOOL DoCharacterSkill(DWORD dwSkillID, DWORD dwSkillLevel, KTarget* pTarget);
	// 角色交互动作
	BOOL DoCharacterAction(DWORD dwCharacterID, BYTE byAction);
	// 角色说话
	BOOL DoCharacterSay(char* pszText);
	// 角色朝向
	BOOL DoCharacterTurn(int nFaceDirection);

	//请求新角色的数据
	BOOL DoApplyNewCharacterData(DWORD dwCharacterID);
	//请求玩家显示数据
	BOOL DoApplyPlayerDisplayData(KPlayer* pPlayer, DWORD dwCheckSum);
	//请求Npc显示数据
	BOOL DoApplyNpcDisplayData(KNpc* pNpc, DWORD dwCheckSum);    

    BOOL DoMountKungfu(DWORD dwID, DWORD dwLevel);
    BOOL DoCancelBuff(int nStyle, int nIndex);

    BOOL DoSetSheathState(BOOL bSheathFlag);

    // -------------------- party about ------------------->

	//邀请xxx组队
    BOOL DoInviteJoinPartyRequest(const char cszInvitePlayer[]);
    //对组队邀请作出应答
    BOOL DoInviteJoinPartyRespond(const char cszInviteSrc[], const GUID &crGuid, BOOL bAgreeFlag);
    //申请加入xxx的队伍
    BOOL DoApplyJoinPartyRequest(const char cszApplyDst[]);
    //对入队申请作出应答
    BOOL DoApplyJoinPartyRespond(const char cszApplySrc[], const GUID &crGuid, BOOL bAgreeFlag);
    //请求离开队伍
    BOOL DoLeavePartyRequest();
    //将xxx踢出队伍
    BOOL DoKickoutPartyMember(const char cszRoleName[]);
    //请求移交队长
    BOOL DoPartyChangeLeaderRequest(const char cszNewLeader[]);
    //设置队伍物品分配模式
    BOOL DoPartySetLootMode(int nLootMode, KPlayer* pPlayer);
	//设置队伍物品需要ROLL的品质
	BOOL DoPartySetRollQuality(int nRollQuality, KPlayer* pPlayer);
    // 发送小地图上的标记给队友
    BOOL DoPartyMemberNotifySignpost(int nX, int nY);
    // <-------------------- party about -------------------

	// 接受任务
	BOOL DoAcceptQuest(KTarget* pTarget, DWORD dwQuestID);
	// 完成任务
	BOOL DoFinishQuest(DWORD dwQuestID, KTarget& rTarget, int nPresentChoice1, int nPresentChoice2);
	// 取消任务
	BOOL DoCancelQuest(int nQuestIndex);
	// 共享任务
	BOOL DoShareQuest(int nQuestIndex);

	//GM指令
	BOOL DoGmCommand(const char* pszGmCommand);

    // --------------------- shop about --------------------->
    BOOL DoOpenShopRequest(DWORD dwShopID);
    BOOL DoQueryShopItems(DWORD dwShopID, DWORD dwPageIndex);
    BOOL DoShopBuyRequest(DWORD dwShopID, DWORD dwPageIndex, DWORD dwPosIndex, DWORD dwItemID, int nCount, int nCost);
    BOOL DoShopSellRequest(DWORD dwShopID, DWORD dwItemID, int nCost, DWORD dwBoxIndex, DWORD dwPosIndex);
    BOOL DoShopRepairRequest(DWORD dwShopID, DWORD dwItemID, int nCost, DWORD dwBoxIndex, DWORD dwPosIndex);
    // 请求购回回购列表里的一样物品
    BOOL DoSoldListBuyRequest(DWORD dwShopID, DWORD dwX, DWORD dwItemID);
    // <--------------------- shop about ---------------------

    // ---------------------- Trading ----------------------->
    BOOL DoTradingInviteRequest(DWORD dwInviteDst);
    BOOL DoTradingInviteRespond(BOOL bAccept);
    BOOL DoTradingAddItem(DWORD dwBoxIndex, DWORD dwPosIndex, DWORD dwGridIndex);
    BOOL DoTradingDeleteItem(DWORD dwGridIndex);
    BOOL DoTradingSetMoney(int nMoney);
    BOOL DoTradingConfirm(BOOL bConfirm, int nKey);
    // <---------------------- Trading ----------------------

    BOOL DoSetTarget(int nTargetType, DWORD dwTargetID);

	// 申请交换装备
	BOOL DoApplyExchangeItem(DWORD dwBox1, DWORD dwX1, DWORD dwBox2, DWORD dwX2, DWORD dwAmount);

	// 选择窗口选项
	BOOL DoWindowSelect(DWORD dwIndex, BYTE bySelect);
	// 配方使用协议
	BOOL DoCastProfessionSkill(DWORD dwCraftID, DWORD dwRecipeID, KTarget* pTarget);
	// 拾取物品或者掷骰子
	BOOL DoLootOrRollItem(DWORD dwDoodadID, DWORD dwItemID, BOOL bCancel);
	// 申请拾取列表
	BOOL DoApplyLootList(DWORD dwDoodadID);
	// 打开
	BOOL DoCharacterOpen(DWORD dwDoodadID);
    // 玩家在接到死亡通知后给的应答
    BOOL DoDeathRespond(int nRespondCode);
	// 申请使用道具
	BOOL DoApplyUseItem(DWORD dwBox, DWORD dwX, KTarget& rTarget);
	// 改变战斗开关
	BOOL DoApplyFightFlag(DWORD dwForceID, BOOL bFight);
	// 结束拾取
	BOOL DoFinishLoot(DWORD dwDoodadID);
	// 拾取金钱
	BOOL DoLootMoney(DWORD dwDoodadID);
	// 改变PK状态
	BOOL DoChangePkMode(int nMode);
	// 队长分配物品
	BOOL DoLeaderDistributeItem(DWORD dwDoodadID, DWORD dwItemID, DWORD dwDstPlayerID);

    // ---------------------- talk about ---------------------->
    // 密聊信息
    BOOL DoPlayerTalkToOne( const char cszTalkTo[], unsigned uTalkDataSize, BYTE* pbyTalkData);
    // 群聊信息
    BOOL DoPlayerTalkToMany(BYTE byTalkRange,       unsigned uTalkDataSize, BYTE* pbyTalkData);
    // <--------------------- talk about -----------------------

    BOOL DoPlayerAddRelationRequest(const char cszAlliedPlayerName[]);
    BOOL DoPlayerDelRelationRequest(DWORD dwAlliedPlayerID);

    //AutoCode:-发送协议函数结束-

private:
	//发送协议函数
	BOOL Send(void* pvData, size_t nSize);

public:
    int GetServerLoop(){ return m_nGSGameLoop; }
private:
    IKG_SocketStream   *m_piSocketStream;

	//协议处理函数指针数组
	typedef void (KPlayerClient::*PROCESS_PROTOCOL_FUNC)(char* pData, size_t nSize, int nFrame);
	PROCESS_PROTOCOL_FUNC	m_ProcessProtocolFuns[gs_client_connection_end];

	//协议大小数组
	int m_nProtocolSize[gs_client_connection_end];

	friend void __stdcall ClientEventNotify(LPVOID lpParam, const unsigned long &ulnEventType);

	//最后的同步到的服务端帧数
	int		m_nGSGameLoop;
	//累计帧数差
	float	m_fDeltaLoop;
	//数据包缓冲
	struct KBufferPacket
	{
		int							m_nGameLoop;
		size_t						m_nSize;
		EXTERNAL_PROTOCOL_HEADER*	m_pHeader;
	};
	//数据包缓冲队列
	KCircleList			m_BufferPacketList;

	//玩家显示数据的Cache
	KIDCache			m_PlayerDisplayDataCache;
	//Npc显示数据的Cache
	KIDCache			m_NpcDisplayDataCache;

	//申请角色数据的队列
	typedef std::map<DWORD, DWORD>		APPLY_NEW_CHARACTER_LIST;
	APPLY_NEW_CHARACTER_LIST			m_ApplyNewCharacterList;

//协议处理函数
private:
	//出错信息
	void OnResponseMessage(char* pData, size_t nSize, int nFrame);

	//同步玩家数据
	void OnSyncPlayerBaseInfo(char* pData, size_t nSize, int nFrame);
	//同步任务状态
	void OnSyncQuestState(char* pData, size_t nSize, int nFrame);
	//同步任务列表
	void OnSyncQuestList(char* pData, size_t nSize, int nFrame);

	//踢号通知
	void OnKickSelfOut(char* pData, size_t nSize, int nFrame);
	//切换服务器的通知
	void OnChangeGameServer(char* pData, size_t nSize, int nFrame);

    void OnSyncRegionPlayer(char* pData, size_t nSize, int nFrame);
    void OnSyncRegionNpc(char* pData, size_t nSize, int nFrame);

    void OnSyncNewPlayer(char* pData, size_t nSize, int nFrame);
    void OnSyncNewNpc(char* pData, size_t nSize, int nFrame);

	void OnRemoveCharacter(char* pData, size_t nSize, int nFrame);

    void OnReviseMoveState(char* pData, size_t nSize, int nFrame);
    void OnSyncMoveState(char* pData, size_t nSize, int nFrame);

    void OnCharacterStand(char* pData, size_t nSize, int nFrame);
	//角色走动同步数据
	void OnCharacterWalk(char* pData, size_t nSize, int nFrame);
	void OnCharacterRun(char* pData, size_t nSize, int nFrame);
    void OnCharacterSwim(char* pData, size_t nSize, int nFrame);
	void OnCharacterJump(char* pData, size_t nSize, int nFrame);
    void OnCharacterSit(char* pData, size_t nSize, int nFrame);
    void OnCharacterMeditate(char* pData, size_t nSize, int nFrame);

	void OnCharacterDash(char* pData, size_t nSize, int nFrame);
	void OnCharacterKnockedDown(char* pData, size_t nSize, int nFrame);
	void OnCharacterKnockedBack(char* pData, size_t nSize, int nFrame);
	void OnCharacterKnockedOff(char* pData, size_t nSize, int nFrame);
    void OnCharacterStun(char* pData, size_t nSize, int nFrame);
    void OnCharacterHalt(char* pData, size_t nSize, int nFrame);
	//角色死亡
	void OnCharacterDeath(char* pData, size_t nSize, int nFrame);
	void OnMoveCharacter(char* pData, size_t nSize, int nFrame);
    //玩家被复活通知
    void OnPlayerRevivedNotify(char* pData, size_t nSize, int nFrame);
	// 同步角色转向
	void OnCharacterTurn(char* pData, size_t nSize, int nFrame);
	// 同步角色动作
	void OnCharacterAction(char* pData, size_t nSize, int nFrame);
	// 同步角色说话
	void OnCharacterSay(char* pData, size_t nSize, int nFrame);

    void OnOverTimeAction(char* pData, size_t nSize, int nFrame);

	// 角色显示数据
	void OnPlayerDisplayData(char* pData, size_t nSize, int nFrame);
	// Npc显示数据
	void OnNpcDisplayData(char* pData, size_t nSize, int nFrame);

	// 增加经验值
	void OnAddExperience(char* pData, size_t nSize, int nFrame);

    void OnCommonHealth(char* pData, size_t nSize, int nFrame);

    //Doodad状态同步数据
	void OnSyncDoodadState(char* pData, size_t nSize, int nFrame);

	//帧间隔信号
	void OnFrameSignal(char* pData, size_t nSize, int nFrame);

    void OnTerrainTest(char* pData, size_t nSize, int nFrame);

    void OnSyncSkillData(char* pData, size_t nSize, int nFrame);    

    void OnSkillPrepare(char* pData, size_t nSize, int nFrame);
    void OnSkillCast(char* pData, size_t nSize, int nFrame);
    void OnSyncCommonSkillAutoCastState(char* pData, size_t nSize, int nFrame);
    void OnResetSkillCooldown(char* pData, size_t nSize, int nFrame);

    void OnSkillEffectResult(char* pData, size_t nSize, int nFrame);   
    void OnSyncBehitRepresent(char* pData, size_t nSize, int nFrame);
    void OnSyncFightState(char* pData, size_t nSize, int nFrame);   	
	void OnSyncBuffList(char* pData, size_t nSize, int nFrame);
    void OnSyncBuffSingle(char* pData, size_t nSize, int nFrame);
    void OnSyncRepresentState(char* pData, size_t nSize, int nFrame);
    void OnSyncKungfuMount(char* pData, size_t nSize, int nFrame);
    void OnSkillBeatBack(char* pData, size_t nSize, int nFrame);
    void OnSyncSheathState(char* pData, size_t nSize, int nFrame);

    // --------------------- 数值同步 --------------------------

    void OnSyncSelfMaxLMRS(char* pData, size_t nSize, int nFrame);
    void OnSyncSelfCurrentLMRS(char* pData, size_t nSize, int nFrame);
    void OnSyncTargetMaxLMR(char* pData, size_t nSize, int nFrame);
    void OnSyncTargetCurrentLMR(char* pData, size_t nSize, int nFrame);
    void OnSyncPartyMemberMaxLMR(char* pData, size_t nSize, int nFrame);
    void OnSyncPartyMemberCurrentLMR(char* pData, size_t nSize, int nFrame);
    void OnSyncPartyMemberMisc(char* pData, size_t nSize, int nFrame);
    void OnSyncPartyMemberCoordinate(char* pData, size_t nSize, int nFrame);
	void OnBroadcastCharacterLife(char* pData, size_t nSize, int nFrame);

    // --------------------- party about -----------------------

    //同步队伍基本信息
    void OnSyncPlayerPartyBaseInfo(char* pData, size_t nSize, int nFrame);
    //同步队伍成员信息
    void OnSyncPlayerPartyMemberInfo(char* pData, size_t nSize, int nFrame);
    //xxx邀请你组队
    void OnInvitePlayerJoinPartyRequest(char* pData, size_t nSize, int nFrame);
    //处理组队邀请的响应(其实只有失败的时候才会有这个响应)
    void OnInvitePlayerJoinPartyRespond(char* pData, size_t nSize, int nFrame);
    //xxx申请加入你的队伍
    void OnApplyJoinPartyRequest(char* pData, size_t nSize, int nFrame);
    //处理入队申请的响应(其实只有失败的时候才会有这个响应)
    void OnApplyJoinPartyRespond(char* pData, size_t nSize, int nFrame);
    //创建队伍
    void OnPartyCreateNotify(char* pData, size_t nSize, int nFrame);
    //通知"你加入了xxx的队伍"
    void OnPlayerJoinPartyNotify(char* pData, size_t nSize, int nFrame);
    //队伍新加队员的通知
    void OnPartyAddMemberNotify(char* pData, size_t nSize, int nFrame);
    //成员离开队伍的通知
    void OnPartyDelMemberNotify(char* pData, size_t nSize, int nFrame);
    //处理玩家队长标志位改变的通知(Player身上的组队旗帜的通知)
    void OnSyncPartyLeaderFlag(char* pData, size_t nSize, int nFrame);
    // 队长移交通知
    void OnPartyChangeLeaderNotify(char* pData, size_t nSize, int nFrame);
    // 物品分配方式变更通知
    void OnPartySetLootModeNotify(char* pData, size_t nSize, int nFrame);
    // 队伍解散通知
    void OnPartyDisbandNotify(char* pData, size_t nSize, int nFrame);
    // 队员上下线通知
    void OnSyncPartyMemberOnlineFlag(char* pData, size_t nSize, int nFrame);
    // 同步小地图标注点
    void OnPartyMemberSignpostNotify(char* pData, size_t nSize, int nFrame);
    // <--------------------- party about ---------------------


    // --------------------- shop about --------------------->
    void OnOpenShopNotify(char* pData, size_t nSize, int nFrame);
    void OnUpdateShopItem(char* pData, size_t nSize, int nFrame);
    void OnShopBuyRespond(char* pData, size_t nSize, int nFrame);
    // <--------------------- shop about ---------------------
    
    // ---------------------- Trading ----------------------->
    void OnTradingInviteRequest(char* pData, size_t nSize, int nFrame);
    void OnOpenTradingBoxNotify(char* pData, size_t nSize, int nFrame);
    void OnSyncTradingConfirm(char* pData, size_t nSize, int nFrame);
    void OnSyncTradingItemData(char* pData, size_t nSize, int nFrame);
    void OnSyncTradingMoney(char* pData, size_t nSize, int nFrame);
    void OnTradingComplete(char* pData, size_t nSize, int nFrame);
    // <--------------------- Trading ------------------------

	// 同步金钱数量
	void OnSyncMoney(char* pData, size_t nSize, int nFrame);
	// 同步道具的临时数据
	void OnSyncTempItemData(char* pData, size_t nSize, int nFrame);
	// 同步道具数据
	void OnSyncItemData(char* pData, size_t nSize, int nFrame);
	// 同步道具删除
	void OnSyncItemRemove(char* pData, size_t nSize, int nFrame);
	// 交换装备
	void OnExchangeItem(char* pData, size_t nSize, int nFrame);
    // 换装广播
    void OnSyncEquipRepresent(char* pData, size_t nSize, int nFrame);
	// 更新物品数量或耐久
	void OnUpdateItemAmount(char* pData, size_t nSize, int nFrame);


	// 学习生活技能
	void OnLearnProfessionNotify(char* pData, size_t nSize, int nFrame);
	// 学习配方
	void OnLearnRecipeNotify(char* pData, size_t nSize, int nFrame);
	// 遗忘技能
	void OnForgetProfessionNotify(char* pData, size_t nSize, int nFrame);
	// 遗忘配方
	void OnForgetRecipeNotify(char* pData, size_t nSize, int nFrame);
	// 增加生活技能经验
	void OnAddProfessionProficiency(char* pData, size_t nSize, int nFrame);
	// 设置生活技能最大等级
	void OnSetMaxProfessionLevelNotify(char* pData, size_t nSize, int nFrame);
	// 同步更新玩家生活技能
	void OnUpdatePlayerProfession(char* pData, size_t nSize, int nFrame);
	// 同步更新玩家配方列表
	void OnUpdatePlayerRecipe(char* pData, size_t nSize, int nFrame);
    // 施放生活技艺
    void OnCastRecipe(char* pData, size_t nSize, int nFrame);
	// 打开一个客户端窗口
	void OnOpenWindow(char* pData, size_t nSize, int nFrame);
	// 同步配方的使用
	void OnStartUseRecipe(char* pData, size_t nSize, int nFrame);
	// 结束技能释放
	void OnEndCast(char* pData, size_t nSize, int nFrame);

	void OnAcceptQuest(char* pData, size_t nSize, int nFrame);
	void OnFinishQuest(char* pData, size_t nSize, int nFrame);
	void OnCancelQuest(char* pData, size_t nSize, int nFrame);
	void OnShareQuest(char* pData, size_t nSize, int nFrame);
	// 同步任务变量
	void OnSyncQuestValue(char* pData, size_t nSize, int nFrame);

	// 同步拾取列表
	void OnSyncLootList(char* pData, size_t nSize, int nFrame);
	// 掷骰子信息
	void OnRollItemMessage(char* pData, size_t nSize, int nFrame);
	// 角色拾取物品
	void OnCharacterPick(char* pData, size_t nSize, int nFrame);
	// 打开拾取面板
	void OnOpenLootList(char* pData, size_t nSize, int nFrame);
	// 同步战斗开关
	void OnSyncFightFlag(char* pData, size_t nSize, int nFrame);
	// 同步声望
	void OnSyncRepute(char* pData, size_t nSize, int nFrame);
	// 同步所有势力战斗开关
	void OnSyncFightflagList(char* pData, size_t nSize, int nFrame);
	// 同步所有的势力声望
	void OnSyncReputeList(char* pData, size_t nSize, int nFrame);
	// 
	void OnCharacterOpen(char* pData, size_t nSize, int nFrame);
	// 组队广播金钱的拾取
	void OnRollMoneyMessage(char* pData, size_t nSize, int nFrame);
	// 结束拾取
	void OnFinishLoot(char* pData, size_t nSize, int nFrame);
	// start roll
	void OnBeginRollItem(char* pData, size_t nSize, int nFrame);
	// 同步定制的用户消息
	void OnCustomOTAction(char* pData, size_t nSize, int nFrame);
	// 同步任务共享的结果
	void OnShareQuestResult(char* pData, size_t nSize, int nFrame);
	// 添加仇恨
	void OnModifyThreat(char* pData, size_t nSize, int nFrame);
	// 修改被仇恨
	void OnModifyThreaten(char* pData, size_t nSize, int nFrame);
	// 清除仇恨
	void OnClearThreat(char* pData, size_t nSize, int nFrame);
	// 清除被仇恨
	void OnClearThreaten(char* pData, size_t nSize, int nFrame);

    // ---------------------- talk about ---------------------->
    // 密聊信息
    void OnPlayerTalkToOne(char* pData, size_t nSize, int nFrame);
    // 群聊信息
    void OnPlayerTalkToMany(char* pData, size_t nSize, int nFrame);

	// 同步双倍经验
	void OnUpdateDoubleExp(char* pData, size_t nSize, int nFrame);
	// 
	void OnUpdatePlayerStamina(char* pData, size_t nSize, int nFrame);
    // '气'的同步
    void OnUpdatePlayerQi(char* pData, size_t nSize, int nFrame);
	// 
	void OnDoodadLootOver(char* pData, size_t nSize, int nFrame);

    void OnPlayerAddRelationRespond(char* pData, size_t nSize, int nFrame);
    void OnPlayerDelRelationRespond(char* pData, size_t nSize, int nFrame);

	//AutoCode:-处理协议函数结束-
};

//extern KPlayerClient g_PlayerClient;

#endif	//_CLIENT
#endif	//_KPLAYER_CLIENT_H_