/************************************************************************/
/* 游戏服务器与客户端的连接                                             */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* Modify	 : Spe														*/
/* Remark	 : 不要随意修改分隔区块的注释，在自动生成代码的宏中需要使用 */
/* History	 :															*/
/*		2005.01.26	Create												*/
/************************************************************************/
#ifndef _KPLAYER_SERVER_H_
#define _KPLAYER_SERVER_H_

#if defined(_SERVER)
#include "Common/KG_Socket.h"
#include "Common/KG_Package.h"

#include <map>
#include <vector>
#include "IServer.h"
#include "KConnectionData.h"
#include "GS_Client_Protocol.h"
#include "KRecorderFactory.h"

class KSceneObject;
class KCharacter;
class KNpc;
class KPlayer;
class KDoodad;
class IKG_Buffer;
class KTarget;
class KRegion;
class KItem;
class KQuest;
class KShop;
class KSkill;
class KScene;
struct KPARTY_INFO;
struct KPARTY_MEMBER_INFO;
struct KG_SKILL_RECIPE_LIST_NODE;
struct KSKILL_CHAIN_TARGET;
struct KG_FELLOWSHIP_ALL_GS;
struct KG_FOE_ALL_GS;
struct KG_BLACK_LIST_ALL_GS;
struct KTongMemberInfo;
struct KPQ_STATISTICS_DATA;
struct KTEAM_MEMBER_INFO;
struct KTeam;

class KPlayerServer
{
public:
	KPlayerServer(void);
	~KPlayerServer(void);

	BOOL Init(IRecorderFactory* piFactory);

	BOOL UnInit(void);

	void Breathe(void);

    BOOL Attach(KPlayer* pPlayer, int nConnIndex);
    BOOL Detach(int nConnIndex);

	BOOL Send(int nConnIndex, void* pvData, size_t uDataLen);
	void FlushSend();

    int   GetConnectionCount()
    {
        return m_nMaxConnection - (int)m_ConnectionDataListFreeVector.size();
    }

    size_t GetNetworkFlux()
    {
        return m_uSecondFlux;
    }
    
public:
	int					m_nListenPort;		
	DWORD				m_dwInternetAddr;
    char                m_szLocalIP[16];

private:
    int                 m_nPingCycle;
	KConnectionData*	m_ConnectionDataList;
	int					m_nMaxConnection;
    int                 m_nClientGM;
    int                 m_nNextCheckPingConnection;
    DWORD               m_dwTimeNow;
    size_t              m_uAddUpFlux;
    size_t              m_uSecondFlux;
    DWORD               m_dwNextFluxUpdateTime;

    ISocketServer*      m_piSocketServer;
    int                 m_nSocketEventCount;                            
    KG_SOCKET_EVENT*    m_pSocketEventArray;

    typedef vector<unsigned>                KG_CONNECTION_DATA_LIST_FREE_VECTOR;
    KG_CONNECTION_DATA_LIST_FREE_VECTOR     m_ConnectionDataListFreeVector;

	typedef void (KPlayerServer::*PROCESS_PROTOCOL_FUNC)(char*, size_t, int, int);
	PROCESS_PROTOCOL_FUNC	m_ProcessProtocolFuns[client_gs_connection_end];

	int m_nProtocolSize[client_gs_connection_end];

    struct KPROTOCOL_STAT_INFO
    {
        DWORD   dwPackCount;
        int64_t nTotalSize;
    };
    KPROTOCOL_STAT_INFO m_DownwardProtocolStatTable[gs_client_connection_end];
    BYTE                m_byTempData[MAX_EXTERNAL_PACKAGE_SIZE];

    int _Construct();
    int _Destroy();

    inline KPlayer* GetPlayerByConnection(int nConnIndex)
    {
        if (nConnIndex != -1)
        {
            assert(nConnIndex >= 0 && nConnIndex < m_nMaxConnection);
            return m_ConnectionDataList[nConnIndex].pPlayer;
        }
        return NULL;
    }

	BOOL Shutdown(int nConnIndex);

    BOOL ProcessNewConnection(IKG_SocketStream* piSocket);
    BOOL ProcessPackage(IKG_SocketStream* piSocket);
    BOOL DumpPakckageStat();
	BOOL CheckPackage(BYTE* pbyData, size_t uDataLen);

public:
	// 返回应答信息
	BOOL DoMessageNotify(int nConnIndex, int nType, int nCode, const void* pvData = NULL, size_t uDataLen = 0);
	// 同步玩家数据
	BOOL DoSyncPlayerBaseInfo(int nConnIndex, KPlayer* pPlayer);
	// 同步任务数据
	BOOL DoSyncQuestData(int nConnIndex, DWORD dwDestPlayerID, BYTE* pbyData, size_t uDataLen, BYTE byDataType);
	// 通知客户端自动断线踢号
	BOOL DoAccountKickout(int nConnIndex);
	// 通知客户端切换服务器
	BOOL DoSwitchGS(int nConnIndex, DWORD dwPlayerID, GUID& Guid, DWORD dwIPAddr, WORD wPort);
	// 通知客户端换地图
    BOOL DoSwitchMap(KPlayer* pPlayer, DWORD dwMapID, int nCopyIndex, int nX, int nY, int nZ);

    // ---------------- Player, NPC, Doodad 的出现/消失同步 --------------------------
    // 同步某个Player到单个客户端玩家
    BOOL DoSyncNewPlayer(int nConnIndex, KPlayer* pPlayer);

    // 同步某个NPC到单个客户端玩家
    BOOL DoSyncNewNpc(int nConnIndex, KNpc* pNpc);

    // 同步某个doodad到某个客户端玩家
    BOOL DoSyncNewDoodad(int nConnIndex, KDoodad* pDoodad);
    
	// 广播Doodad的状态同步
	BOOL DoSyncDoodadState(KDoodad* pDoodad);
	// 通知客户端某个角色被移除了
	BOOL DoRemoveCharacter(KCharacter* pCharacter, BOOL bKilled);
    // 移除Doodad
    BOOL DoRemoveDoodad(KDoodad* pDoodad);
    
    // ----------------- 移动同步 --------------------------------------------
    BOOL DoSyncMoveCtrl(KPlayer* pPlayer);
    BOOL DoSyncMoveParam(KCharacter* pCharacter);
    BOOL DoAdjustPlayerMove(KPlayer* pPlayer);
    BOOL DoSyncMoveState(KCharacter* pCharacter, BOOL bSyncSelf);
	BOOL DoCharacterDeath(KCharacter* pCharacter, DWORD dwKillerID, int nSituReviveFrame);
	BOOL DoMoveCharacter(KCharacter* pCharacter, int nX, int nY, int nZ, BOOL bIgnoreBlock);

    // ------------------------------------------------------------------------------

    // 玩家被复活通知
    BOOL DoPlayerRevivedNotiy(int nConnIndex, KCharacter* pCharacter);
	// 角色交互动作同步
	BOOL DoCharacterAction(KCharacter* pCharacter, DWORD dwCharacterID, DWORD dwAction);
	// 角色转向同步
	BOOL DoCharacterTurn(KCharacter* pCharacter, BOOL bForceTurn, BOOL bTurnImmediately);

    BOOL DoOverTimeAction(KCharacter* pCharacter, int nActionType, DWORD dwActionID, int nDirection, int nDurationFrame);

	// 发送玩家显示数据
	BOOL DoPlayerDisplayData(DWORD dwPlayerID, DWORD dwCheckSum, int nConnIndex);
	// 发送Npc显示数据
	BOOL DoNpcDisplayData(DWORD dwNpcID, DWORD dwCheckSum, int nConnIndex);

	// 同步当前经验值,协助任务经验,双倍经验等数值
	BOOL DoSyncExperience(KPlayer* pPlayer, int nDeltaExp);

    BOOL DoPlayerLevelUp(KPlayer* pPlayer);

    BOOL DoCommonHealth(KCharacter* pCharacter, int nDeltaLife);

	BOOL DoFrameSignal(int nConnIndex);

    // ----------------- fight about --------------------------

	BOOL DoSyncSkillData(int nConnIndex, DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwSkillExp, BOOL bInitial);

    BOOL DoSyncSkillRecipe(int nConnIndex, size_t uArrayCount, KG_SKILL_RECIPE_LIST_NODE* pdwRecipeArray);

	BOOL DoSkillPrepare(
        KCharacter* pCharacter, DWORD dwSkillID, DWORD dwSkillLevel, 
        int nPrepareFrame, KTarget &rTarget
    );
    BOOL DoSkillCast(KCharacter* pCharacter, KSkill* pSkill, KTarget &rTarget,  BOOL bSubCast);
    BOOL DoSyncCommonSkillAutoCastState(int nConnIndex, BOOL bStartOrStop, BOOL bMeleeOrRange);
    BOOL DoResetCooldown(int nConnIndex, DWORD dwCoolDownID, int nEndFrame, int nInterval);
    BOOL DoSkillChainEffect(KCharacter* pCaster, DWORD dwSkillID, DWORD dwSkillLevel, size_t uTargetCount, KSKILL_CHAIN_TARGET* pTarget);

    BOOL DoSkillEffectResult(
        KCharacter* pSkillSrc, KCharacter* pSkillDst, KCharacter* pEffectDst, 
        struct KSKILL_CALCULATE_RESULT *pSkillResult, DWORD dwBulletID
    );

    BOOL DoSyncBehitRepresent(
        KCharacter* pSkillSrc, KCharacter* pSkillDst, KCharacter* pEffectDst,
        struct KSKILL_CALCULATE_RESULT *pSkillResult, DWORD dwBulletID
    );

    // 每隔段时间,检查Player的目标的BuffList是否变化,如果变化,则同步目标的整个List
    BOOL DoSyncTargetBuffList(KPlayer* pPlayer);
    BOOL DoSyncTargetTargetBuffList(KPlayer* pPlayer);

    // 同步玩家所选目标掉落所属的信息
    BOOL DoSyncTargetDropID(KPlayer* pPlayer);

    // 同步玩家自己的Buff list(目前应用在登陆的时候)
	BOOL DoSyncBuffList(KPlayer* pPlayer, KPlayer* pDstPlayer);
    // 玩家自己的Buff变化时,向整个队伍同步(单个Buff)
    BOOL DoSyncBuffSingle(
        KCharacter* pCharacter, int nIndex,
        DWORD dwBuffID, int nLevel, DWORD dwRecipeKey[],
        int nStackNum, int nEndFrame
    );

    BOOL DoSyncRepresentState(KCharacter* pCharacter);

    BOOL DoSyncKungfuMount(int nConnIndex, DWORD dwKungfuID, DWORD dwKungfuLevel);

    BOOL DoSkillBeatBack(KCharacter* pCharacter, int nBeatBackFrame, BOOL bBreak = false);

    BOOL DoSyncSheathState(KCharacter* pCharacter);

    // ---------------- 数值同步 -------------------------------------

    // 参考文档: 人物数值同步概要.txt
    // L,M,S, 分别是Life, Mana, Stamina的首字母

    BOOL DoSyncSelfMaxLMRS(KPlayer* pPlayer);
    BOOL DoSyncSelfCurrentLMRS(KPlayer* pPlayer);
    BOOL DoSyncSelfWeakInfo(KPlayer* pPlayer);
    BOOL DoSyncTargetMaxLMR(KPlayer* pPlayer);
    BOOL DoSyncTargetCurrentLMR(KPlayer* pPlayer);
    BOOL DoSyncTargetWeakInfo(KPlayer* pPlayer);
    BOOL DoSyncTargetTargetMaxLMR(KPlayer* pPlayer);
    BOOL DoSyncTargetTargetCurrentLMR(KPlayer* pPlayer);
    BOOL DoSyncTeamMemberMaxLMR(DWORD dwTeamID, KTEAM_MEMBER_INFO* pMember);
    BOOL DoSyncTeamMemberCurrentLMRGlobal(DWORD dwTeamID, KTEAM_MEMBER_INFO* pMember);
    BOOL DoSyncTeamMemberCurrentLMRLocal(KPlayer* pPlayer);
    BOOL DoSyncTeamMemberMisc(DWORD dwTeamID, KTEAM_MEMBER_INFO* pMember);
    BOOL DoSyncTeamMemberPositionGlobal(DWORD dwTeamID, KTEAM_MEMBER_INFO* pMember);
    BOOL DoSyncTeamMemberPositionLocal(KPlayer* pPlayer);
	BOOL DoBroadcastCharacterLife(KCharacter* pCharacter);
    BOOL DoSyncPlayerTrain(KPlayer* pPlayer);
    BOOL DoBroadcastPlayerSchoolID(KPlayer* pPlayer);

    // ----------------- party about --------------------------------

    // 玩家登陆服务器时同步队伍数据
    BOOL DoLoginPartySync(KPlayer* pPlayer);
    // 同步玩家所在队伍的基本信息
    BOOL DoSyncTeamBaseInfo(KPlayer* pPlayer, BOOL bAddPartyMemberFlag = false);
    // 将组队邀请发送到目标玩家
    BOOL DoInvitePlayerJoinPartyRequest(DWORD dwInviteDst, const char cszInviteSrc[]);
    // 将Relay转发过来的入队申请,发送到队长客户端
    BOOL DoApplyJoinTeamRequest(DWORD dwLeader, const char cszApplySrc[]);
    // 队伍新添成员的通知
    BOOL DoTeamAddMemberNotify(DWORD dwPartyID, int nGroupIndex, DWORD dwMemberID);
    // xxx离开队伍的通知
    BOOL DoTeamDelMemberNotify(DWORD dwPartyID, int nGroupIndex, DWORD dwMemberID);
    BOOL DoTeamChangeAuthorityNotify(DWORD dwPartyID, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID);
    BOOL DoTeamSetLootModeNotify(DWORD dwPartyID, int nLootMode, int nRollQuality);
    
    BOOL DoTeamSetFormationLeaderNotify(DWORD dwPartyID, int nGroupIndex, DWORD dwNewFormationLeader);

    // 队伍解散通知
    BOOL DoTeamDisbandNotify(DWORD dwPartyID);
    // 同步成员在线状态的改变
    BOOL DoSyncTeamMemberOnlineFlag(DWORD dwPartyID, DWORD dwMemberID, BOOL bOnlineFlag);
    // 同步玩家状态信息
    BOOL DoSyncPlayerStateInfo(KPlayer* pPlayer);

    // 队伍标记
    BOOL DoTeamSetMarkRespond(DWORD dwPartyID, int nMarkType, DWORD dwTargetID);

    // 组队阵营发生变化
    BOOL DoTeamCampChange(DWORD dwPartyID, KCAMP eCamp);

    BOOL DoTeamLevelUpRaidNotify(DWORD dwTeamID, int nGroupNum);
    
    BOOL DoTeamChangeMemberGroupNotify(DWORD dwTeamID, int nSrcGroupIndex, int nDstGroupIndex, DWORD dwMemberID);

    // ----------------- shop about -------------------------------

    BOOL DoOpenShopNotify(int nConnectIndex, DWORD dwShopID);
    BOOL DoSyncShopItem(int nConnIndex, const KShop *pcShop, DWORD dwPageIndex, DWORD dwPosIndex, BOOL bIDChanged);

    // ------------------ Trading about ---------------------------
    BOOL DoTradingInviteRequest(KPlayer* pTradingInviteSrc, KPlayer *pTradingInviteDst);
    BOOL DoOpenTradingBoxNotify(int nConnIndex, DWORD dwOtherPlayer);
    BOOL DoSyncTradingConfirm(int nConnIndex, DWORD dwPlayerID, BOOL bConfirm);
    BOOL DoSyncTradingItemData(
        int nConnIndex, KItem* pcItem, DWORD dwGridIndex, DWORD dwItemOwnerID, 
        DWORD dwBoxIndex, DWORD dwPosIndex, int nKey
    );
    BOOL DoSyncTradingMoney(int nConnIndex, DWORD dwPlayerID, int nMoney, int nKey);
    BOOL DoTradingComplete(int nConnIndex, BOOL bComplete);
    // ------------------------------------------------------------

	// 同步金钱数量
	BOOL DoSyncMoney(int nConnIndex, int nMoney, BOOL bShowMsg);
    BOOL DoSyncContribution(int nConnIndex, int nContribution);
	// 同步道具数据
	BOOL DoSyncItemData(int nConnIndex, DWORD dwPlayerID, KItem* pItem, DWORD dwBox, DWORD dwX);
	// 同步道具删除
	BOOL DoSyncItemDestroy(int nConnIndex, DWORD dwOwnerID, DWORD dwBox, DWORD dwX);
	// 交换装备
	BOOL DoExchangeItem(int nConnIndex, DWORD dwBox1, DWORD dwX1, DWORD dwBox2, DWORD dwX2, DWORD dwAmount);
	// 更新物品数量或耐久
	BOOL DoUpdateItemAmount(int nConnIndex, DWORD dwBox, DWORD dwX, int nAmount);
    // 玩家换装广播
    BOOL DoSyncEquipRepresent(KPlayer* pPlayer, int nIndex, DWORD dwRepresentID);
	// 同步道具获得的消息
	BOOL DoAddItemNotify(KPlayer* pAddItemPlayer, DWORD dwItemID, int nCount, int nDestConnIndex = -1);
    
	// 学习生活技能
	BOOL DoLearnProfessionNotify(int nConnIndex, DWORD dwProfessionID);
	// 学习生活技能分支
	BOOL DoLearnBranchNotify(int nConnIndex, DWORD dwProfessionID, DWORD dwBranchID);
	// 遗忘技能
	BOOL DoForgetProfessionNotify(int nConnIndex, DWORD dwProfessionID);
	// 增加生活技能经验
	BOOL DoAddProfessionProficiency(int nConnIndex, DWORD dwProfessionID, DWORD dwExp);
	// 设置生活技能最大等级
	BOOL DoSetMaxProfessionLevelNotify(int nConnIndex, DWORD dwProfessionID, DWORD dwLevel);
	// 同步更新玩家生活技能
	BOOL DoUpdatePlayerProfession(int nConnIndex, DWORD dwProfessionID, DWORD dwMaxLevel, 
										DWORD dwCurrentLevel, DWORD dwCurrentProficiency, DWORD dwBranchID);

	// 学习配方
	BOOL DoLearnRecipeNotify(int nConnIndex, DWORD dwCraftID, DWORD dwRecipeID);
	// 遗忘配方
	BOOL DoForgetRecipeNotify(int nConnIndex, DWORD dwCraftID);
	// 同步配方列表
	BOOL DoSyncRecipeState(int nConnIndex, BYTE* pbyData, size_t uDataLen);

	// 打开一个客户端窗口
	BOOL DoOpenWindow(DWORD dwIndex, TARGET_TYPE eTarget, DWORD dwTargetID, const char* pcszText, int nConnIndex);

	// 同步配方的使用
	BOOL DoStartUseRecipe(KPlayer* pPlayer, DWORD dwCraftID, DWORD dwRecipeID, KTarget &rTarget);

	// 接受任务
	BOOL DoAcceptQuest(int nQuestIndex, DWORD dwQuestID, int nConnIndex, BOOL bDailyQuest = false);
	// 完成任务
	BOOL DoFinishQuest(DWORD dwQuestID, int nConnIndex, BOOL bForceFinished);
	// 取消任务
	BOOL DoCancelQuest(int nQuestIndex, int nConnIndex);
	// 清除任务
	BOOL DoClearQuest(int nConnIndex, DWORD dwQuestID);
	// 共享任务
	BOOL DoShareQuest(KPlayer* pDstPlayer, KPlayer* pPlayer, DWORD dwQuestID);
	// 同步任务变量
	BOOL DoSyncQuestValue(int nQuestIndex, int nConnIndex);
	// 同步开始小队任务
	BOOL DoStartEscortQuest(int nConnIndex, DWORD dwStartPlayerID, DWORD dwQuestID);

	// 同步拾取列表
	BOOL DoSyncLootList(KPlayer* pPlayer, KDoodad* pDoodad);

	BOOL DoRollItemMessage(int nDestConnIndex, KPlayer* pRoller, int nRollPoint, DWORD dwDoodadID, DWORD dwItemID);

	// 打开拾取面板
	BOOL DoOpenLootList(int nConnIndex, KPlayer *pPlayer, KDoodad *pDoodad);

	// 同步战斗开关
	BOOL DoSyncFightFlag(int nConnIndex, DWORD dwForceID, BOOL bFight);

	// 同步声望
	BOOL DoSyncRepute(int nConnIndex, DWORD dwForceID, int nLevel, int nRepute, int nDiffValue);
	// 同步所有势力战斗开关
	BOOL DoSyncFightflagList(int nConnIndex, int nCount, KPlayer* pPlayer);
	// 同步所有的势力声望
	BOOL DoSyncReputeList(int nConnIndex, int nCount, KPlayer* pPlayer);
	// 同步声望修改标记
	BOOL DoShowReputation(int nConnIndex, DWORD dwForceID, BOOL bLoginLoading);

    BOOL DoCharacterOpen(KCharacter *pCharacter, KDoodad* pDoodad);
	BOOL DoDistributeItem(int nConnIndex, DWORD dwDestPlayerID, DWORD dwItemID);

    BOOL DoFinishLoot(KPlayer* pPlayer, KDoodad* pDoodad);
	BOOL DoBeginRollItem(int nConnIndex, KItem* pItem, KDoodad* pDoodad, int nLeftRollFrame);

	// 同步定制的用户消息
	BOOL DoCustomOTAction(
        KPlayer* pPlayer, int nType, BOOL bIsBreakable, int nFrame, int nTargetType, 
        DWORD dwTargetID, DWORD dwScriptID, const char* pszActionName
    );

	// 添加仇恨
	BOOL DoModifyThreat(int nConnIndex, DWORD dwCharacterID);

	// 修改被仇恨
	BOOL DoModifyThreaten(int nConnIndex, DWORD dwCharacterID);

	// 清除仇恨
	BOOL DoClearThreat(int nConnIndex, DWORD dwCharacterID);

	// 清除被仇恨
	BOOL DoClearThreaten(int nConnIndex, DWORD dwCharacterID);

    // 聊天消息
    BOOL DoTalkMessage(
        int nMsgType, 
        DWORD dwTalkerID, const char cszTalker[], 
        DWORD dwReceiverID, const char cszReceiver[], 
        size_t uDataLen, BYTE* pbyTalkData, 
        int nRange = 0
    );

    BOOL DoSceneMessage(KScene* pScene, const char* pszMessage);

	// 精力体力
	BOOL DoSyncSelfCurrentST(KPlayer* pPlayer);

	// 
	BOOL DoDoodadLootOver(KDoodad* pDoodad);

    BOOL DoSyncFellowshipList(DWORD dwPlayerID, int nCount, KG_FELLOWSHIP_ALL_GS* pData);
    BOOL DoSyncFoeList(DWORD dwPlayerID, int nCount, KG_FOE_ALL_GS* pData);
    BOOL DoSyncBlackList(DWORD dwPlayerID, int nCount, KG_BLACK_LIST_ALL_GS* pData);
    BOOL DoSyncFellowshipGroupName(DWORD dwPlayerID);

	BOOL DoNpcTalkToPlayer(KNpc* pNpc, DWORD dwDialogID, NPC_TALK_ACTION_TYPE eTalkActionType, DWORD dwTalkIndex, CHARACTER_TALK_TYPE eTalkType, KTarget& rTarget);

	BOOL DoSetForce(KCharacter* pCharacter);
	
    // -------------------- 邮件系统 ----------------------------------->
    BOOL DoSendMailRespond(int nConnIndex, BYTE byRespondID, KMAIL_RESULT_CODE eResult);
    BOOL DoGetMailListRespond(int nConnIndex, int nMailCount, KMAIL_LIST_NODE* pMailListNode);
    BOOL DoNewMailNotify(int nConnIndex, KMAIL_LIST_NODE* pMailListNode);
    BOOL DoGetMailContentRespond(DWORD dwPlayerID, DWORD dwMailID, BYTE byResult, KMAIL_CONTENT* pContent);
    BOOL DoMailGeneralRespond(DWORD dwPlayerID, DWORD dwMailID, BYTE byResult);
    BOOL DoMailReleaseAttachment(int nConnIndex, DWORD dwMailID, BOOL bReleaseMoney, BOOL bReleaseText, BOOL bReleaseItem);
    // <------------------- 邮件系统 ------------------------------------

	// --------------------- PK About ----------------------------------->
	BOOL DoSyncPKState(int nConnIndex, KPlayer* pPlayer, int nEndFrame = 0, BOOL bRefuseDuelFlag = false);
    BOOL DoBroadcastPKState(KPlayer* pPlayer);
	BOOL DoWinDuelBroadcast(DWORD dwWinnerID, DWORD dwLosserID);

	// 打开配方书
	BOOL DoOpenRecipeBook(int nConnIndex, DWORD dwRecipeBookItemID);

	// 打开阅读用书
	BOOL DoOpenBook(int nConnIndex, DWORD dwBox, DWORD dwX);

	BOOL DoSyncUserPreferences(int nConnIndex, int nOffset, int nLength, BYTE* pbyData);
 
	BOOL DoSyncAllBookState(int nConnIndex, KPlayer* pPlayer);

	BOOL DoSyncBookState(int nConnIndex, DWORD dwBookID, BYTE byState);
 
	BOOL DoCharacterChangeModel(KCharacter* pCharacter);

	BOOL DoSyncNpcDialogFlag(KNpc* pNpc);

	BOOL DoSyncRoleDataOver(int nConnIndex);

	BOOL DoSyncEnableBankPackage(int nConnIndex, int nEnabledCount);

	BOOL DoUpdateMapMark(int nConnIndex, int nX, int nY, int nZ, int nType, const char* pszComment);

	BOOL DoSyncMidMapMark(DWORD dwPartyID, DWORD dwSrcPlayerID, int nMapID, int nX, int nY, int nType, const char* pszComment);

	BOOL DoPlaySound(int nConnIndex, DWORD dwSoundID);
	BOOL DoPlayMiniGame(int nConnIndex, DWORD dwGameID);

    BOOL DoSyncHorseFlag(KPlayer* pPlayer);

    // ---------------------- 交通相关 --------------------------------
    BOOL DoRoadTrackForceRespond(int nConnIndex);
    BOOL DoSyncRouteNodeOpenList(int nConnIndex, int nCount, int* pnNodeList);
    
    BOOL DoSyncPrinkForce(KPlayer* pPlayer, DWORD dwForceID, int nLevel);

    BOOL DoSyncVisitMapInfo(KPlayer* pPlayer);

    BOOL DoSyncAccumulateValue(KPlayer* pPlayer);

    BOOL DoSetTarget(KPlayer* pPlayer, KCharacter* pCharacter);
    BOOL DoBroadcastTarget(KCharacter* pCharacter);
    BOOL DoSyncKillPoint(KPlayer* pPlayer);
    BOOL DoSetPlayerRedName(KPlayer* pPlayer);
    BOOL DoSetPlayerDialogFlag(KPlayer* pPlayer);
    BOOL DoSetOnlyReviveInSitu(KPlayer* pPlayer);
    BOOL DoSyncFormationCoefficient(KPlayer* pPlayer, DWORD dwMemberID, int nCoefficient);

    BOOL DoJoinBattleFieldRespond(DWORD dwPlayerID, DWORD dwMapID, int nErrorCode);
    BOOL DoLeaveBattleFieldRespond(DWORD dwPlayerID, DWORD dwMapID, int nErrorCode);
    BOOL DoBattleFieldNotify(
        DWORD dwPlayerID, time_t nAvgQueueTime, int nPassTime,
        DWORD dwMapID, int nCopyIndex, BATTLE_FIELD_NOTIFY_TYPE eType
    );
    BOOL DoSyncBattleFieldList(DWORD dwPlayerID, DWORD dwMapID, int nCopyIndex);
    BOOL DoSyncBattleFieldSide(KPlayer* pPlayer);
    BOOL DoBattleFieldEnd(DWORD dwMapID, int nCopyIndex);

    BOOL DoSyncCampInfo(KPlayer* pPlayer);
    BOOL DoSyncCurrentPrestige(KPlayer* pPlayer);
    
    // ---------------------- 帮会相关 --------------------------------
    BOOL DoSyncTongMemberInfo(DWORD dwPlayerID, const KTongMemberInfo& crTongMember);
    BOOL DoTongDeleteMemberNotify(DWORD dwPlayerID, DWORD dwMemberID);
    BOOL DoApplyTongInfoRespond(DWORD dwPlayerID, BYTE byRespondType, BYTE* pbyRespondData, size_t uDataLen);
    BOOL DoUpdateTongClientDataVersion(DWORD dwPlayerID, KTONG_UPDATE_FLAG eFrameType, int nUpdateFrame);    
    BOOL DoInvitePlayerJoinTongRequest(DWORD dwPlayerID, DWORD dwInviterID, DWORD dwTongID, char szInviterName[], char szTongName[]);
    BOOL DoChangePlayerTongNotify(int nConnIndex, DWORD dwTongID, BYTE byReason, char szTongName[]);
    BOOL DoBroadcastTongMessage(int nConnIndex, BYTE byCode, BYTE* pbyData, size_t uDataLen);
    BOOL DoGetTongDescriptionRespond(int nConnIndex, DWORD dwTongID, char szTongName[]);
    BOOL DoBroadcastPlayerChangeTong(KPlayer* pPlayer);
    BOOL DoSyncTongOnlineMessage(int m_nConnIndex, char szOnlineMessage[], size_t uDataLen);
    //-----------------------------------------------------------------
     
    // ---------------------- 寄卖行相关 --------------------------------
    BOOL DoAuctionLookupRespond(DWORD dwPlayerID, BYTE byRespondID, BYTE byCode, KAUCTION_PAGE_HEAD* pPage);
    BOOL DoAuctionBidRespond(DWORD dwPlayerID, BYTE byCode);
    BOOL DoAuctionSellRespond(DWORD dwPlayerID, BYTE byCode);
    BOOL DoAuctionCancelRespond(DWORD dwPlayerID, BYTE byCode);
    BOOL DoAuctionMessageNotify(DWORD dwPlayerID, BYTE byCode, const char* pszSaleName, int nPrice);

    BOOL DoSetCamp(KPlayer* pPlayer);
    
    BOOL DoSyncBattleFieldStatistics(int nConnIndex, KPlayer* pPlayer, const KPQ_STATISTICS_DATA& crStatisticsData);
    BOOL DoSyncBattleFieldObjective(int nConnIndex, int nIndex, int nCurrentValue, int nTargetValue);
    BOOL DoSyncBattleFieldPQInfo(int nConnIndex, DWORD dwPQID, DWORD dwPQTemplateID, int nLapseTime, int nLeftTime);

    BOOL DoSyncHeroFlag(KPlayer* pPlayer);
    BOOL DoRemoteLuaCall(int nConnIndex, const char cszFunction[], BYTE* pbyData, size_t uDataLen);

    BOOL DoSyncPlayerReviveCtrl(KPlayer* pPlayer);

    BOOL DoSyncAchievementData(KPlayer* pPlayer);
    BOOL DoNewAchievementNotify(KPlayer* pPlayer, int nAchievementID);
    BOOL BroadcastAchievementAnnounce(DWORD dwPlayerID, BYTE byType, const char* pszRoleName, int nAchievementID);
    BOOL DoSyncAchievementPoint(KPlayer* pPlayer, int nDeltaPoint);

    BOOL DoSyncMaxLevel(int nConnIndex, int nMaxLevel);

    BOOL DoUseItemPrepare(KPlayer* pPlayer, int nFrames, DWORD dwOTAniID, DWORD dwEndAniID, const char* pcszText);
    //AutoCode:-发送协议函数结束-

private:
	// 请求玩家数据
	void OnHandshakeRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 客户端加载资源完毕,请求进入场景
	void OnApplyEnterScene(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 玩家登出
	void OnPlayerLogout(char* pData, size_t nSize, int nConnIndex, int nFrame);
    // 上行Ping
    void OnPingSignal(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnMoveCtrl(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnApplyMoveAdjust(char* pData, size_t nSize, int nConnIndex, int nFrame);
    // 角色站立指令
	void OnCharacterStand(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 角色走动指令
	void OnCharacterWalk(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 角色跑动指令
	void OnCharacterRun(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 角色游泳指令
	void OnCharacterSwim(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 角色跳跃指令
	void OnCharacterJump(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 角色坐下指令
	void OnCharacterSit(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 角色转向指令
	void OnSyncDirection(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // 中断玩家当前行为
    void OnStopCurrentAction(char* pData, size_t nSize, int nConnIndex, int nFrame);
    // 角色释放技能指令
	void OnCharacterSkill(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 角色交互动作指令
	void OnCharacterAction(char* pData, size_t nSize, int nConnIndex, int nFrame);

	// 玩家显示数据
	void OnApplyPlayerDisplayData(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 申请低频目标和队友数据
	void OnApplyLoFreqData(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnMountKungfu(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnCancelBuff(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnSetSheathState(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnSetHideHatFlag(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnApplyPlayerBuffList(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // ---------------------- party about ---------------------->

    //邀请某玩家组队
    void OnInviteJoinTeamRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    //组队邀请应答
    void OnInviteJoinTeamRespond(char* pData, size_t nSize, int nConnIndex, int nFrame);
    //申请加入xxx的队伍
    void OnApplyJoinTeamRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    //对xxx入队声请的应答
    void OnApplyJoinTeamRespond(char* pData, size_t nSize, int nConnIndex, int nFrame);
    //队伍删除成员: 离队或者踢人
    void OnTeamDelMemberRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnTeamChangeAuthorityRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    
    void OnTeamSetLootMode(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnTeamSetRollQuality(char* pData, size_t nSize, int nConnIndex, int nFrame);
    
    void OnTeamSetFormationLeader(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // 发送小地图上的标记给队友
    void OnTeamMemberNotifySignpost(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // 收到小队标记
    void OnTeamSetMarkRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnTeamLevelUpRaidRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnTeamChangeMemberGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // <---------------------- party about ----------------------

	//接受任务
	void OnAcceptQuest(char* pData, size_t nSize, int nConnIndex, int nFrame);
	//完成任务
	void OnFinishQuest(char* pData, size_t nSize, int nConnIndex, int nFrame);
	//取消任务
	void OnCancelQuest(char* pData, size_t nSize, int nConnIndex, int nFrame);
	// 共享任务
	void OnShareQuest(char* pData, size_t nSize, int nConnIndex, int nFrame);
	//接受护送任务
	void OnAcceptEscortQuest(char* pData, size_t nSize, int nConnIndex, int nFrame);

	//Gm指令
	void OnGmCommand(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // ---------------------- shop about ---------------------->
    void OnOpenShopRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnQueryShopItems(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnShopBuyRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnShopSellRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnShopRepairRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    // 请求购回回购列表里的一样物品
    void OnSoldListBuyRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    // <---------------------- shop about ----------------------

    // ----------------------- Trading about ----------------------------
    void OnTradingInviteRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnTradingInviteRespond(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnTradingAddItem(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnTradingDeleteItem(char*pData, size_t nSize, int nConnIndex, int nFrame);
    void OnTradingSetMoney(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnTradingConfirm(char* pData, size_t nSize, int nConnIndex, int nFrame);
    // <----------------------- Trading about ----------------------------

    void OnRequestTarget(char* pData, size_t nSize, int nConnIndex, int nFrame);

	void OnApplyExchangeItem(char* pData, size_t nSize, int nConnIndex, int nFrame);

	void OnWindowSelect(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnCastProfessionSkill(char* pData, size_t nSize, int nConnIndex, int nFrame);

	void OnLootItem(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnRollItem(char* pData, size_t nSize, int nConnIndex, int nFrame);
    
	void OnApplyLootList(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnCharacterOpen(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnDeathRespond(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnApplyUseItem(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnApplyFightFlag(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnFinishLoot(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnLootMoney(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnDistributeItem(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // ---------------------- talk about ---------------------->
    // 聊天
    void OnTalkMessage(char* pData, size_t nSize, int nConnIndex, int nFrame);


    // ---------------------- 好友系统 ----------------------->
    void OnGetFellowshipListRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnAddFellowshipRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnDelFellowshipRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnAddFellowshipGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnDelFellowshipGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnRenameFellowshipGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnSetFellowshipRemarkRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnSetFellowshipGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // <--------------------- 好友系统 -----------------------

    // ---------------------- 邮件系统 ----------------------->
    void OnSendMailRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnGetMaillistRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnGetMailContent(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnGetMailAttachment(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnSetMailRead(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnDeleteMail(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnReturnMail(char* pData, size_t nSize, int nConnIndex, int nFrame);
    // <--------------------- 邮件系统 -----------------------

	void OnApplyPKOperate(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnSyncUserPreferences(char* pData, size_t nSize, int nConnIndex, int nFrame);

	void OnSetBankOpenedFlag(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnEnableBankPackage(char* pData, size_t nSize, int nConnIndex, int nFrame);

	void OnSyncMidMapMark(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnPlayerLearnSkill(char* pData, size_t nSize, int nConnIndex, int nFrame);
	void OnPlayerLearnRecipe(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnPlayerActiveSkillRecipe(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnPeekOtherPlayerEquip(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnPeekOtherPlayerBook(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnPeekOtherPlayerQuest(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // ------------------------- 交通相关 -----------------------------------
    void OnRoadTrackStartOut(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnRoadTrackForceRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // ------------------------- 经脉相关 -----------------------------------
    void OnOpenVenation(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnSendGmMessage(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnJoinBattleFieldQueueRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnLeaveBattleFieldQueueRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnAcceptJoinBattleField(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnGetBattleFieldList(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnLeaveBattleField(char* pData, size_t nSize, int nConnIndex, int nFrame);
    
    // ------------------------- 帮会相关 -----------------------------------
    void OnApplyTongRosterRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnApplyTongInfoRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnInvitePlayerJoinTongRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnInvitePlayerJoinTongRespond(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnApplyKickOutTongMemberRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnModifyTongInfoRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnModifyTongScheamRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnApplyQuiteTongRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnChangeTongMemberGroupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnChangeTongMasterRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnChangeTongMemberRemarkRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnGetTongDescriptionRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // ------------------------------------------------------------------------
    void OnApplyBattleFieldStatistics(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnSetHeroFlag(char* pData, size_t nSize, int nConnIndex, int nFrame);

    // ------------------------- 寄卖行相关 -----------------------------------
    void OnAuctionLookupRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnAuctionBidRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnAuctionSellRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnAuctionCancelRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    
    void OnApplySetCampRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnRemoteLuaCall(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnPlayerReviveRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);

    void OnApplyAchievementDataRequest(char* pData, size_t nSize, int nConnIndex, int nFrame);
    void OnQueryProductIdentity(char* pData, size_t nSize, int nConnIndex, int nFrame);
    //AutoCode:-处理协议函数结束-

private:
    BOOL PartyBroadcast(DWORD dwTeamID, void* pvData, size_t uSize);
    BOOL PartyBroadcastTalk(KTeam* pTeam, DWORD dwTalkerID, void* pvData, size_t uSize);
};

extern KPlayerServer g_PlayerServer;

struct KBroadcastFunc
{
    KBroadcastFunc();
	BOOL operator()(KPlayer* pPlayer);
	void*	        m_pvData;			
	size_t	        m_uSize;			
	int	            m_nMaxSyncCount;
    DWORD           m_dwExcept;         // 广播时排除这个ID的玩家
    KSceneObject*   m_pCenterObject;    // 以此对象为中心来计算距离
    int             m_nRange;           // 距离在此范围内才广播(单位点),除非该值为0
    DWORD           m_dwExclusive;      // 广播时,只是形式上的广播,其实只发给这个ID的玩家
};

struct KBroadcastBeHitFunc
{
    KBroadcastBeHitFunc();
	BOOL operator()(KPlayer* pPlayer);
	void*	        m_pvData;
	size_t	        m_uSize;
	int	            m_nMaxSyncCount;
    KPlayer*        m_pExceptSrc;       // 广播时排除这个玩家及其队友
    KPlayer*        m_pExceptDst;       // 广播时排除这个玩家及其队友
};

struct KBroadcastTalkFunc
{
    KBroadcastTalkFunc();
    BOOL operator()(KPlayer* pPlayer);
    void*	        m_pvData;
    size_t	        m_uSize;
    int	            m_nMaxSyncCount;
    KPlayer*        m_pTalker;          // 说话人，广播时排除将此人列入黑名单的人。
    KSceneObject*   m_pCenterObject;    // 以此对象为中心来计算距离
    int             m_nRange;           // 距离在此范围内才广播(单位点),除非该值为0
    DWORD           m_dwExclusive;
};

struct KBroadcastTargetFunc
{
    BOOL operator()(KPlayer* pPlayer);
    KCharacter* m_pSrcCharacter;
};

struct KTeamBroadcast
{
    DWORD  m_dwTeamID;
    DWORD  m_dwExceptID;
    void*  m_pvData;
    size_t m_uSize;

    BOOL operator () (KTEAM_MEMBER_INFO* pMemberInfo);
};

#endif	//_SERVER
#endif	//_KPLAYER_SERVER_H_
