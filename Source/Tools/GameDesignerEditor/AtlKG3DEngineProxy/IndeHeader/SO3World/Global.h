/************************************************************************/
/* 全局量定义							                                */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* Comment	 : 此处定义整个SO3World中使用的常量和一些可能需要配置的常量 */
/*			   常量定义在KG_GlobalDef.h中，不跨Project的在这里！		*/
/* History	 :															*/
/*		2005.01.12	Create												*/
/************************************************************************/
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "SO3GlobalDef.h"
#include "ProtocolBasic.h"

#define DIRECTION_BIT_NUM       8
#define DIRECTION_COUNT			(1 << DIRECTION_BIT_NUM)
#define DIRECTION_TURN_SPEED    5

#define	CURRENT_ITEM_VERSION	1
#define	ITEM_SETTING_DIR	SETTING_DIR"/item"
#define	NORMAL_WEAPON_FILE	"Weapon.tab"
#define	NORMAL_ARMOR_FILE	"Armor.tab"
#define NORMAL_TRINKET_FILE	"Trinket.tab"
#define CUST_WEAPON_FILE	"Custom_Weapon.tab"
#define	CUST_ARMOR_FILE		"Custom_Armor.tab"
#define	CUST_TRINKET_FILE	"Custom_Trinket.tab"
#define	ATTRIB_FILE			"Attrib.tab"
#define	COMMON_ITEM_FILE	"Other.tab"
#define SET_FILE			"Set.tab"

//道具掉落表列名
#define DROP_LIST_DIR		"DropList"
#define DOODAD_DROP_LIST_DIR "DoodadDropList"
#define MONEY_DROP_FILE		"ClassDrop\\MoneyDrop.tab"

#define MAX_DROP_PER_NPC	10
#define MAX_DROP_PER_DOODAD	4
#define MAX_LOOT_SIZE		32

#define CUSTOM_OTACTION_NAME_LEN 16
#define USE_ITEM_PROGRESS_NAME_LEN 16

#define SCRIPT_DIR		"scripts"


// 脚本回调函数名
#define SCRIPT_ON_ENTER_TRAP	"OnEnterTrap"		//Param1:player;
#define SCRIPT_ON_LEAVE_TRAP	"OnLeaveTrap"		//Param1:player;

#define SCRIPT_ON_INIT_SCENE	"OnInitScene"		//Param1:Scene
#define SCRIPT_ON_ENTER_SCENE	"OnEnterScene"		//Param1:player;Param2:DestScene
#define SCRIPT_ON_LEAVE_SCENE	"OnLeaveScene"		//Param1:player;Param2:SrcScene

#define SCRIPT_ON_USE_ITEM		"OnUse"				//Param1:player;Param2:item

#define SCRIPT_ON_OPEN_DOODAD	    "OnOpen"			//Param1:doodad;Param2:player
#define SCRIPT_ON_BREAK_OPEN	    "OnBreak"			//Param1:doodad;Param2:player
#define SCRIPT_ON_PICK_DOODAD	    "OnPick"			//Param1:doodad;Param2:player
#define SCRIPT_ON_PER_OPEN_DOODAD	"OnPerOpen"			//Param1:doodad;Param2:player

#define SCRIPT_ON_DIALOGUE		"OnDialogue"		//Param1:npc;	Param2:player
#define SCRIPT_ON_NPC_DEATH		"OnDeath"			//Param1:npc;	Param2:killer
#define SCRIPT_ON_PLAYER_DEATH	"OnDeath"			//Param1:player Param2:killer

#define SCRIPT_ON_ACCEPT_QUEST	"OnAcceptQuest"		//Param1:player	 Param2:dwQuestID	
#define SCRIPT_ON_FINISH_QUEST	"OnFinishQuest"		//Param1:player  Param2:nPresentChoice  Param3:nPresentChoice2 Param4:dwQuestID	
#define SCRIPT_ON_CANCEL_QUEST	"OnCancelQuest"		//Param1:player  Param2:dwQuestID	
#define SCRIPT_ON_START_QUEST	"OnStartQuest"		//

#define SCRIPT_ON_CUSTOM_EVENT	"OnCustomEvent"		//Param1:player Param2:target
#define SCRIPT_ON_CUSTOM_BREAK	"OnCustomBreak"		//Param1:player

//AI用脚本回调函数名
#define SCRIPT_ON_PATROL		"OnPatrol"			//Param1:npc
#define SCRIPT_ON_AI_EVENT		"OnAIEvent"			//Param1:npc

//场景定时器的回调函数名
#define SCRIPT_ON_TIMER			"OnTimer"			//Param1:int Param2:int

//生活技能	
#define SCRIPT_ON_MEMORIZE_BOOK	"OnMemorizeBook"	//Param1:player Param2:BookID Param3:BookSubID
#define SCRIPT_ON_GET_ALL_BOOK	"OnGetAllBook"		//Param1:player Param2:BookID 

#define SCRIPT_ON_BEGIN_CAST_CRAFT  "OnBeginCastCraft"
#define SCRIPT_ON_BREAK_CAST_CRAFT  "OnBreakCastCraft"
#define SCRIPT_ON_END_CAST_CRAFT    "OnEndCastCraft"

#define START_USE_ITEM      "OnStartUse"
#define USE_ITEM_SUCCEED    "OnUseSucceed"
#define USE_ITEM_FAILD      "OnUseFailed"

#define MAX_WINDOW_SELECT_COUNT	32					//客户端窗口选项个数
#define MAX_SELECT_FUNCNAME_LEN	32					//选项回调函数名长度

//骰子随机数定义
#define DICE_MAX				100
#define DICE_MIN				1

//生活技能相关
#define MAX_PROFESSION_COUNT		128	
#define MAX_CRAFT_COUNT 16

#define OLD_MAX_RECIPE_ID 256
#define MAX_RECIPE_ID 512
#define MAX_RECIPE_NAME_LEN 32
#define MAX_BELONG_NAME_LEN 32

#define MAX_MATERIAL 8
#define MAX_RESULTANT 8

#define MAX_READ_BOOK_ID_BIT 9			
#define MAX_READ_BOOK_SUB_ID_BIT 3		

#define MAX_READ_BOOK_ID (1 << MAX_READ_BOOK_ID_BIT)
#define MAX_READ_BOOK_SUB_ID (1 << MAX_READ_BOOK_SUB_ID_BIT)

// 道具相关
#define	MAX_ITEM_COUNT         65536

#define INIT_BANK_SIZE		   28	//初始银行格子大小
#define MAX_BANK_PACKAGE_COUNT 5	//银行的扩展背包数量

//AI相关
#define MAX_AI_EVENT 6
#define INIT_AI_EVENT_FUNC_NAME "InitAIEvent"

#define PLAYER_THREAT_TIME	(GAME_FPS * 6) //玩家的仇恨消退时间

#define AI_RETURN_OVERTIME_FRAME 160

// 字符串长度定义
#define ITEM_NAME_LEN			64
#define QUEST_NAME_LEN			32
#define QUEST_LINK_NAME_LEN		32

// NPC AI拥有的技能数量
#define MAX_NPC_AI_SKILL		8

#define MAX_OPEN_WINDOW_TEXT_LEN 1000

// 用户数据最大存盘长度
#define MAX_USER_PREFERENCES_LEN 2000

// 小地图标记的更新间隔
#define UPDATE_MINIMAP_MARK_INTERVAL 16

#define START_ESCORT_QUEST_DELAY (GAME_FPS * 8)
// 只能追加
enum DOODAD_KIND
{
	dkInvalid = 0,

	dkNormal,					// 普通的Doodad,有Tip,不能操作
	dkCorpse,					// 尸体
	dkQuest,					// 任务相关的Doodad
	dkRead,						// 可以看的Doodad
	dkDialog,					// 可以对话的Doodad
	dkAcceptQuest,				// 可以接任务的Doodad,本质上上面3个类型是一样的,只是图标不同而已
	dkTreasure,					// 宝箱
	dkOrnament,					// 装饰物,不能操作
	dkCraftTarget,				// 生活技能的采集物
	dkClientOnly,				// 客户端用
	dkNull,					    // 摆摊Doodad(已删除，位置可给新的类型使用)
	dkChair,					// 可以坐的Doodad
	dkGuide,                    // 路标
	dkDoor,                     // 门之类有动态障碍的Doodad

	dkTotal
};

//角色类型（由于该枚举值存盘，所以严禁更改顺序，只可追加！！！）
enum CHARACTER_KIND
{
	ckInvalid = 0,

	ckPlayer,
	ckFightNpc,
	ckDialogNpc,
	ckCannotOperateNpc,	//不能被攻击被操作的Npc
	ckClientNpc,

	ckTotal
};

enum PATROL_REPEAT_STYLE
{
	prsInvalid = 0,

	prsNoRepeat,
	prsCircle,
	prsLine,

	prsTotal
};

// 性别
enum CHARACTER_GENDER
{
	cgMale = 1,
	cgFemale = 2,
	cgGay = 3
};

// 角色种族
enum CHARACTER_RACE
{
	crInvalid = 0,

	crNone,				//无种族
	crHuman,			//人类
	crAnimal,			//动物
	crRobot,			//机械
	crMonster,			//妖怪
	crHolyBeast,		//神兽	注：HolyBeast由胡子原创

	crTotal
};

enum QUEST_DIFFICULTY_LEVEL  // 任务的困难程度
{   
	qdlInvalid = 0,

	qdlProperLevel,
	qdlHighLevel,
	qdlHigherLevel,
	qdlLowLevel,
	qdlLowerLevel
};

enum QUEST_EVENT_TYPE
{
	qetInvalid = -1,

	qetUpdateAll = 0,
	qetKillNpc,
	qetGetItem,
	qetDeleteItem,
	qetSetQuestValue,

	qetTotal
};

//技艺类型
enum ALL_CRAFT_TYPE
{
	actInvalid = 0,

	actCollection,						// 收集技能,比如采花,采金
	actProduce,							// 制造技能,比如铸造,医术,烹饪
	actRead,							// 阅读技能,对道具使用
	actEnchant,							// 附魔技能
	actRadar,							// 雷达技能(纯客户端实现)
	actCopy,                            // 抄录

	actTotal
};

enum ITEM_BOX
{
	ibInvalid = 0x000000ff,		// 会强转成bye（同步时，所以只能定义0xff为非法）
	ibEquip = 0,				// 必须保证这个是第一个有效的，存诸顺序会影响加载玩家时的装备生成
	ibPackage,
	ibPackage1,
	ibPackage2,
	ibPackage3,
	ibPackage4,
	ibBank,
	ibBankPackage1,
	ibBankPackage2,
	ibBankPackage3,
	ibBankPackage4,
	ibBankPackage5,
	ibSoldList,
	ibTotal
};

//Npc发起对话的时机
enum NPC_TALK_ACTION_TYPE
{
	ntatTurnToFight = 0,
	ntatDeath,
	ntatTeammateDeath,
	ntatIdle,
	ntatCustom,
	ntatTurnToIdle,
	ntatKillEnemy,
	ntatEscape,

	ntatTotal
};

enum DROP_TYPE
{
	dtInvalid = -1,

	dtNpcDrop,
	dtDoodadDrop,

	dtTotal
};

enum TARGET_TYPE
{
	ttInvalid,

	ttNoTarget,
	ttCoordination,
	ttNpc,
	ttPlayer,
	ttDoodad,
	ttItem,
	ttItemPos,

	ttTotal
};

enum DATE_FORMAT
{
	dfInvalid = 0x0000,

	dfSecond = 0x0001,
	dfMinute = 0x0002,
	dfHour = 0x0004,
	dfDay = 0x0008,
	dfMonth = 0x0010,
	dfYear = 0x0020,
	dfWeek = 0x0040
};

enum MINIMAP_RADAR_TYPE
{
	mrtInvalid = -1,

	mrtNoRadar = 0,
	mrtFindNpc,
	mrtFindCraftDoodad,

	mrtTotal
};

enum AI_EVENT_CONDITION
{
	aecInvalid = 0,

	//以下是每帧检测的条件
	aecSelfHPBelow,
	aecTargetHPBelow,
	aecFrameInterval,
	aecRandom,

	//以下是主动触发的条件
	aecEvent,
	aecTurnToFight = aecEvent,
	aecTurnToIdle,
	aecReturnToOrigin,
	aecSelfDead,
	aecTargetDead,

	aecTotal
};

enum AI_EVENT_ACTION
{
	aeaInvalid = 0,

	aeaEscape,
	aeaEscapeForHelp,
	aeaCallHelp,
	aeaDeath,
	aeaRunScript,

	aeaTotal
};

enum MOVE_MODE
{
	mmInvalid = 0,
	mmGround,
	mmWater,
	mmGroundAndWater,

	mmTotal
};

class KPlayer;
class KNpc;
class KDoodad;

struct KCaster
{
	TARGET_TYPE	eTargetType;

	DWORD dwCharacterID;

	union 
	{
		ROLE_TYPE nRoleType;
		DWORD dwRepresentID;
	};
};

//杂项
#define TEN_THOUSAND_NUM        10000
#define KILO_NUM                1024            // 伤害计算，属性计算的比例基数，即用1/1024取代1/100
#define MILLION_NUM				1048576		    // 物品掉落率百分比精度基数
#define MAX_SYNC_PER_REGION		64	            // 同步量控制参数
#define VIEW_SYNC_DELAY	        GAME_FPS        // 视野同步延时
#define STATE_UPDATE_TIME		8			    // 魔法状态刷新间隔
#define DEFAULT_MASS			128			    // 单位：g

#define DEFAULT_GRAVITY			(8 * 512 / GAME_FPS / GAME_FPS)	//单位：点/帧^2

#define LOGOUT_WAITING_TIME		(GAME_FPS * 20)	    // 玩家下线等待20秒
#define LOGIN_TIMEOUT			(GAME_FPS * 8)	    // 登陆超时时间

#define MAX_LIFE_VALUE          (INT_MAX / 2)
#define MAX_MANA_VALUE          (INT_MAX / 2)

// 商店每页要显示的最大物品个数
#define MAX_SHOP_PAGE_ITEM_COUNT    10
// 商店最大页数
#define MAX_SHOP_PAGES              10

#define MAX_OPEN_SHOP_DISTANCE      (6 * CELL_LENGTH)

#define MAX_TRADING_BOX_SIZE        10

#define MAX_TRADING_DISTANCE        (6 * CELL_LENGTH)

#define MAX_OPEN_AUCTION_DISTANCE   (6 * CELL_LENGTH)

#define MONEY_LIMIT 2000000000
#define CONTRIBUTION_LIMIT 75000

#define GS_SETTINGS_FILENAME	"gs_settings.ini"

#pragma pack(1)
struct KMOVE_CRITICAL_PARAM
{
	BYTE byMoveState;
	int  nRunSpeed;
	int  nGravity;
	int  nJumpSpeed;
	int  nX;
	int  nY;
	int  nZ;
};
#pragma pack()

#define INVALID_SCENE_PROGRESS_ID   (-1)
#define MAX_SCENE_PROGRESS_ID       UCHAR_MAX
#define SCENE_PROGRESS_DATA_LEN     (MAX_SCENE_PROGRESS_ID / CHAR_BIT + 1)

#define MAX_ACHIEVEMENT_ID 1000

enum KACHIEVEMENT_ANNOUNCE_TYPE
{
	aatInvalid = 0,
	aatNearby,
	aatScene,
	aatGlobal,
};

// 称号同步数据长度
#define DESIGNATION_DATA_LEN    71
#endif	//_GLOBAL_H_

