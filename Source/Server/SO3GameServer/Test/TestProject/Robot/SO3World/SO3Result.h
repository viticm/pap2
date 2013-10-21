#ifndef _SO3_RESULT_H_
#define _SO3_RESULT_H_

//错误返回值分类
enum ERROR_CODE_TYPE
{
	ectInvalid = 0,

	ectSkillErrorCode,
	ectLoginErrorCode,
	ectQuestErrorCode,
	ectItemErrorCode,
    ectTradingErrorCode,
    ectTalkErrorCode,
	ectCraftErrorCode,

	ectTotal
};

//技能相关的错误返回值
enum SKILL_RESULT_CODE
{
	srcInvalid = 0,

	srcSuccess,             // 成功
	srcFailed,              // 失败，未定义原因

    srcInvalidCastMode,     // 释放方式不对
	srcNotEnoughLife,		// 血量不足
	srcNotEnoughMana,		// 内力不足
	srcNotEnoughStamina,	// 体力不足
	srcNotEnoughRage,		// 怒气不足
	srcNotEnoughItem,		// 道具不足
    srcNotEnoughAmmo,       // 弹药不足
	srcSkillNotReady,		// 技能CD时间未到
    srcInvalidSkill,        // 无效的技能
    srcInvalidTarget,       // 无效的目标
    srcNoTarget,            // 没有目标
    srcNeedTarget,          // 需要指定一个目标
    srcTooCloseTarget,      // 目标太近
	srcTooFarTarget,		// 目标太远
    srcOutOfAngle,          // 目标角度不对
    srcTargetInvisible,     // 目标不在视线范围内
    srcWeaponError,         // 武器不匹配
    srcAmmoError,           // 弹药与武器不匹配
    srcNotEquitAmmo,        // 没有装备弹药
    srcMountError,          // 没有装备对应的武功
	srcInventoryIsFull,		// 背包已满

	srcLootFailed,			// 拾取失败
	srcNotExistLootItem,	// 要拾取的物品不存在
	srcAddLootItemFailed,	// 拾取的物品无法放入背包
	srcNoLootTarget,		// 无拾取目标Doodad
	srcTooFarToLoot,		// 太远而不能拾取

    srcInOTAction,          // 正处于其他OTAction行为
	srcTotal
};

//登陆相关的错误返回值
enum LOGIN_RESULT_CODE
{
	lrcInvalid = 0,

	lrcSuccess,
	lrcFailed,

	//从GS返回的错误代码
	lrcCannotFindPlayerInGC,			//总控上找不到该玩家
	lrcErrorDestGS,						//错误的目标GameServer
	lrcErrorRoleStateInGC,				//总控上的玩家状态错误
	lrcCannotFindPlayerInLoginQueue,	//登陆队列中找不到玩家
	lrcErrorGameStatus,					//错误的角色状态
	lrcErrorGUID,						//错误的Guid
	lrcErrorRoleData,					//错误的角色数据
	lrcAddPlayerFailed,					//向游戏世界中添加角色失败

	lrcTotal
};

enum QUEST_RESULT_CODE
{
	qrcInvalid = 0,

	qrcSuccess,
	qrcFailed,

	qrcErrorQuestID,					//错误的任务ID
	qrcQuestListFull,					//任务列表已满
	qrcErrorQuestState,					//错误的任务状态
	qrcAlreadyAcceptQuest,				//已经接受了改任务
	qrcCannotFindQuest,					//任务配置表中找不到指定的任务
	qrcErrorQuestIndex,					//错误的任务表序号
	qrcTooLowLevel,						//角色等级太低
	qrcTooHighLevel,					//角色等级太高
	qrcTooLowMoral,						//角色道德值太低
	qrcTooHighMoral,					//角色道德值太高
	qrcErrorGender,						//角色性别不符合要求
	qrcErrorRoleType,					//角色体型不对
	qrcCooldown,						//Cooldown计时器未到时间
	qrcPrequestUnfinished,				//前置任务没完成
	qrcErrorQuestValue,					//错误的任务变量状态
	qrcErrorItemCount,					//错误的任务物品数量
	qrcErrorKillNpcCount,				//错误的杀怪数量
	qrcTooLongTime,						//做任务时间超过限制
	qrcErrorTaskValue,					//错误的任务变量数值
	qrcAssistQuestListFull,				//协助任务列表错误
	qrcErrorAssistQuestIndex,			//错误的协助任务序号
	qrcErrorStartNpcTarget,				//目标Npc身上不能接该任务
	qrcErrorStartDoodadTarget,			//目标Doodad身上不能接该任务
	qrcErrorEndNpcTarget,				//目标Npc身上不能交该任务
	qrcErrorEndDoodadTarget,			//目标Doodad身上不能交该任务
	qrcNoStartQuestItem,				//没有任务起始道具
	qrcNoEndQuestItem,					//没有结束起始道具
	qrcTooEarlyTime,					//接任务时间太早
	qrcTooLateTime,						//接任务时间太晚
	qrcTooLowSkillLevel,				//武功技能等级太低
	qrcTooLowLifeLevel,					//生活技能等级太低
	qrcErrorGenerateItem,				//生成任务奖励物品失败
	qrcAddItemFailed,					//添加任务奖励物品失败
	qrcAddMoneyFailed,					//添加金钱失败
	qrcNotEnoughStartItem,				//接任务道具不足
	qrcNotEnoughFreeRoom,				//背包空间不足
	qrcErrorMap,						//错误的地图
	qrcUnfinishedQuest,					//未完成任务，不能协助
	qrcCannotFindQuestInDestPlayer,		//无法在目标玩家身上找到需要协助的任务
	qrcNeedAccept,						//必须先接任务才能交

	qrcTotal
};

enum ITEM_RESULT_CODE
{
	ircInvalid = 0,

	ircSuccess,
	ircFailed,

	ircItemNotExist,					//物品不存在
	ircCannotUseItem,					//物品不可被使用
	ircUseInPackage,					//在背包中才能使用
	ircUseWhenEquipped,					//装备上才能使用该物品
	ircNotEnoughDurability,				//耐久度不足或叠加数量不足
	ircCannotCastSkill,					//不能释放物品上的技能
	ircCastSkillFailed,					//释放技能失败
	ircCostItemDurabilityFailed,		//消耗耐久度失败
	ircErrorEquipPlace,					//装备物品栏位置错误
	ircTooLowAgility,					//敏捷太低,不能装备
	ircTooLowStrength,					//力量太低,不能装备
	ircTooLowSpirit,					//根骨太低,不能装备
	ircTooLowVitality,					//体质太低,不能装备
	ircTooLowLevel,						//等级太低,不能装备
	ircTooLowDurability,				//耐久度为零,不能装备
	ircCannotEquip,						//物品不可被装备
	ircCannotDestory,					//物品无法被摧毁
	ircCannotPutToThatPlace,			//物品无法放置到那个位置	
	ircCannotDisablePackage,			//背包内有东西无法卸下
	ircPackageNotEnoughSize,			//背包大小不足
	ircPackageErrorType,				//背包类型不符
	ircTotal
};

enum PARTY_RESULT_CODE
{
    ePartyRetInvalid = 0,
    ePartyDstNotExist,                  
    ePartyInvitedPlayerAlreadInParty,
    ePlayerAlreadyBeingInInvitedState,
    ePartyApplyDstFull,
    ePartyDstDenied,                    
    ePartyDstNotOnline,
    ePartyRetTotal
};

enum SHOP_SYSTEM_RESPOND_CODE
{
    ssrcInvalid= 0,
    ssrcSellSuccess,                    // 出售成功    
    ssrcBuySuccess,                     // 购买成功
    ssrcRepairSuccess,                  // 修理成功
    ssrcSellFailed,                     // 出售失败
    ssrcBuyFailed,                      // 购买失败
    ssrcRepairFailed,                   // 修理成功
    
    ssrcTooFarAway,                     // 距离太远了
    ssrcNotEnoughMoney,                 // 金钱不足
    ssrcNotEnoughRepate,                // 声望不足
    ssrcNotEnoughRank,                  // 称号不达要求
    ssrcItemSoldOut,                    // 商店该物品卖完了
    ssrcBagFull,                        // 背包已满
    ssrcItemExistLimit,                 // 物品拥有限制
    ssrcItemNotNeedRepair,              // 物品不需修理
    ssrcNoneItemNeedRepair,             // 没有物品可以修理
    ssrcCanNotSell,                     // 不能出售改该物品
    ssrcItemHadLimit,                   // 物品拥有达到上限
    ssrcItemModifiedWhileBuy,           // 在你发出购买请求的这段时间内，该栏物品发生了变化

    ssrcTotal
};

enum TRADING_RESULT_CODE
{
    trcInvalid,
    trcSuccess,
    trcFailed,                  

    trcInvalidTarget,
    trcInvalidInvite,           // 交易邀请已过期
    trcRefuseInvite,            // 目标拒绝邀请
    trcTargetNotInGame,         // 邀请的目标不在游戏中
    trcTargetBusy,              // 目标正在忙
    trcYouBusy,                 // 你正在忙别的事情
    trcTooFar,                  // 距离太远了
    trcTradingCancel,           // 交易取消
    trcTradingCanceled,         // 交易被取消
    trcItemBeBound,             // 物品已绑定
    trcNotEnoughMoney,          // 没有足够的金钱
    trcNotEnoughFreeRoom,       // 没有足够的空间    
    trcItemExistAmountLimit,    // 物品拥有超过上限

    trcTotal
};

enum CRAFT_RESULT_CODE
{
	crcInvalid = 0,

	crcSuccess,             // 成功
	crcFailed,              // 失败，未定义原因

	crcInvalidCastMode,     // 释放方式不对
	crcNotEnoughStamina,	// 体力不足
	crcNotEnoughItem,		// 道具不足
	crcSkillNotReady,		// 技能CD时间未到
	crcInvalidProfession,   // 无效的技能
	crcInvalidCraft,		// 无效的技艺
	crcInvalidRecipe,		// 无效的配方
	crcInvalidTarget,       // 无效的目标
	crcInvalidCaster,		// 无效的释放者
	crcInvalidItem,			// 无效的道具
	crcLootFailed,			// 生成掉落表失败
	crcNoTarget,            // 没有目标
	crcNeedTarget,          // 需要指定一个目标
	crcWeaponError,         // 武器不匹配
	crcProfessionNotLearned,  //生活技能还没学会
	crcRecipeNotLearned,		//配方没有学会
	crcTooLowProfessionLevel, //生活技能等级太低
	crcAddItemFaild,			//添加道具失败

	crcInventoryIsFull,		// 背包已满
	crcAddItemFailed,		// 拾取的物品无法放入背包

	crcInOTAction,          // 正处于其他OTAction行为

	crcTotal
};

enum PLAYER_RELATION_RESPOND_CODE
{
    rrcInvalid = 0,

    rrcSucess,              // 成功。

    rrcSucessAdd,
    rrcSucessDel,

    rrcFailed,              // 失败，未定义。

    rrcInvalidName,         // 无效的玩家名字。
    rrcRelationListFull,    // 好友列表已满。
    rrcRelationExists,      // 指定的好友已经存在。
    rrcRelationNotFound,    // 指定的好友不存在。

    rrcTotal
};

//字符串表
extern char* g_QuestRMList[];

#endif	//_SO3_RESULT_H_