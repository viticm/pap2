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

#define	CURRENT_ITEM_VERSION	0
#define	ITEM_SETTING_DIR	SETTING_DIR"/item"
#define	NORMAL_WEAPON_FILE	"Weapon.tab"
#define	NORMAL_ARMOR_FILE	"Armor.tab"
#define NORMAL_TRINKET_FILE	"Trinket.tab"
#define CUST_WEAPON_FILE	"Custom_Weapon.tab"
#define	CUST_ARMOR_FILE		"Custom_Armor.tab"
#define	CUST_TRINKET_FILE	"Custom_Trinket.tab"
#define	ATTRIB_FILE			"Attrib.tab"
#define	COMMON_ITEM_FILE	"Other.tab"

//道具掉落表列名
#define DROP_LIST_DIR		"DropList"
#define DOODAD_DROP_LIST_DIR "DoodadDropList"
#define MONEY_DROP_FILE		"ClassDrop\\MoneyDrop.tab"

#define MAX_DROP_PER_NPC	4
#define MAX_DROP_PER_DOODAD	4
#define MAX_LOOT_SIZE		32

#define CUSTOM_OTACTION_NAME_LEN 16

#define SCRIPT_DIR		"Scripts"

//Doodad障碍信息配置表
#define OBSTACLE_DIR	"Obstacle"

//创建INT64
#define MAKE_INT64(__A__, __B__)	(((unsigned __int64)(__A__) << 32) + (__int64)__B__)

#define ERROR_ID		0


//脚本回调函数名
#define SCRIPT_ON_ENTER_TRAP	"OnEnterTrap"		//Param1:player;Param2:cell
#define SCRIPT_ON_LEAVE_TRAP	"OnLeaveTrap"		//Param1:player;Param2:old cell

#define SCRIPT_ON_ENTER_SCENE	"OnEnterScene"		//Param1:player;Param2:DestScene
#define SCRIPT_ON_LEAVE_SCENE	"OnLeaveScene"		//Param1:player;Param2:SrcScene

#define SCRIPT_ON_USE_ITEM		"OnUse"				//Param1:player;Param2:item

#define SCRIPT_ON_OPEN_DOODAD	"OnOpen"			//Param1:doodad;Param2:player
#define SCRIPT_ON_BREAK_OPEN	"OnBreak"			//Param1:doodad;Param2:player
#define SCRIPT_ON_PICK_DOODAD	"OnPick"			//Param1:doodad;Param2:player

#define SCRIPT_ON_DIALOGUE		"OnDialogue"		//Param1:npc;	Param2:player
#define SCRIPT_ON_NPC_DEATH		"OnDeath"			//Param1:npc;	Param3:killer

#define SCRIPT_ON_ACCEPT_QUEST	"OnAcceptQuest"		//Param1:player		
#define SCRIPT_ON_FINISH_QUEST	"OnFinishQuest"		//Param1:player
#define SCRIPT_ON_CANCEL_QUEST	"OnCancelQuest"		//Param1:player

#define SCRIPT_ON_CUSTOM_EVENT	"OnCustomEvent"		//Param1:player

//AI用脚本回调函数名
#define SCRIPT_ON_PATROL		"OnPatrol"			//Param1:npc

//场景定时器的回调函数名
#define SCRIPT_ON_TIMER			"OnTimer"			//Param1:int Param2:int


#define MAX_WINDOW_SELECT_COUNT	32					//客户端窗口选项个数
#define MAX_SELECT_FUNCNAME_LEN	32					//选项回调函数名长度

//骰子随机数定义
#define DICE_MAX				100
#define DICE_MIN				1

//技能相关
//#define MAX_SKILL_COUNT			1024

//道具相关
#define	MAX_ITEM_COUNT			1024
#define	MAX_ATTRIB_COUNT		4096

//AI相关
#define MAX_AI_EVENT 6
#define INIT_AI_EVENT_FUNC_NAME "InitAIEvent"

#define AI_RETURN_OVERTIME_FRAME 160

//字符串长度定义
#define ITEM_NAME_LEN			64
#define QUEST_NAME_LEN			32
#define QUEST_LINK_NAME_LEN		32

//Region相关
#define CONNECTED_REGION_TOTAL	8

#define MAX_PLAYER_LEVEL		100

//NPC AI拥有的技能数量
#define MAX_NPC_AI_SKILL		4

#define MAX_OPEN_WINDOW_TEXT_LEN 1000

enum DOODAD_KIND
{
	dkInvalid = 0,

	dkNormal,					//普通的Doodad,所有人都能打开
	dkCorpse,					//尸体
	dkQuest,					//任务相关的Doodad
	dkTreasure,					//宝箱
	dkOrnament,					//装饰物,不能操作
	dkCraftTarget,				//生活技能的采集物
	dkClientOnly,

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

//角色的性别
enum CHARACTER_GENDER
{
	cgInvalid = 0,

	cgMale,
	cgFemale,
	cgGay,
	cgLibido,

	cgTotal
};

//角色种族
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

//任务状态标记
enum QUEST_MARK
{
	qmInvalid = -1,

	qmNoMark = 0,
	qmHide = 1,
	qmWhiteQuestion,
	qmWhiteExclamation,
	qmBlueQuestion,
	qmBlueExclamation,
	qmYellowQuestion,
	qmYellowExclamation,

	qmTotal
};

enum QUEST_EVENT_TYPE
{
	qetInvalid = -1,

	qetUpdateAll = 0,
	qetKillNpc,
	qetGetItem,
	qetDeleteItem,

	qetTotal
};

//技艺类型
enum ALL_CRAFT_TYPE
{
	actInvalid = 0,

	actCollection,						// 收集技能,比如采花,采金
	actProduce,							// 制造技能,比如铸造,医术,烹饪
	actRead,								// 阅读技艺,对道具使用
	// 还有附魔等技能可能以后会做
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
	ibSoldList,
//	ibLootList,
	ibTotal
};

//Npc对话用的频道,用在Npc对玩家广播或者Npc说话的时候
enum NPC_TALK_TYPE
{
	nttInvalid = 0,

	nttNearby,
	nttSence,
	nttWhisper,

	nttTotal
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

    ttTotal
};

class KPlayer;
class KNpc;
class KDoodad;

struct KCaster
{
	TARGET_TYPE	eTargetType;

	union
	{
		KPlayer*	pPlayer;
		KNpc*		pNpc;
		KDoodad*	pDoodad;
	};
};

#pragma	pack(1)


struct KNPC_DATA
{
	// 注意,ID只是对Client only的NPC才有用,服务端在载入的时候不理会这个ID, Doodad也是一样的
	DWORD	dwID;
	char	szName[_NAME_LEN];
	DWORD	dwTemplateID;
	BYTE	byLevel;
	int		nX;
	int		nY;
	int		nZ;
	int		nKind;							// Npc类型，见CHARACTER_KIND
	DWORD	dwScriptID;
	BYTE	byFaceDirection;				// 朝向
	BYTE	byAIType;						// AI类型，见AI_TYPE
	DWORD	dwAIParams[AI_PARAM_COUNT];		// AI参数
	DWORD	dwNpcReliveID;
	DWORD	dwNpcTeamID;
	int		nNpcTeamIndex;
	int		nPatrolPathID;
	BYTE	byAIMainState;					// NPC默认的AI行为,1为发呆,2为闲逛,3为巡逻
	DWORD	dwRepresentID;
	BYTE	byReserved[11];
};

struct KDOODAD_DATA
{
	DWORD	dwID;
	char	szName[_NAME_LEN];
	DWORD	dwTemplateID;
	BYTE	byLevel;
	int		nX;
	int		nY;
	int		nZ;
	DWORD	dwScriptID;
	BYTE	byFaceDirection;	
	DWORD	dwDoodadReliveID;
	BYTE	byReserved[28];
};

#pragma pack()


//8方向的宏
//剑三所有的角度相关数值
//都以正右方为零，逆时针增加，和笛卡儿坐标系相同
//切记切记！！！
#define RIGHT_DIRECTION			0
#define UP_RIGHT_DIRECTION		1
#define UP_DIRECTION			2
#define UP_LEFT_DIRECTION		3
#define LEFT_DIRECTION			4
#define DOWN_LEFT_DIRECTION		5
#define DOWN_DIRECTION			6
#define DOWN_RIGHT_DIRECTION	7
#define CENTER_DIRECTION		8
#define ALL_DIRECTION			8


//杂项
#define TEN_THOUSAND_NUM        10000
#define KILO_NUM                1024        //伤害计算，属性计算的比例基数，即用1/1024取代1/100
#define MILLION_NUM				1048576		//物品掉落率百分比精度基数
#define MAX_SYNC_PER_REGION		(1 << MAX_SYNC_PER_REGION_BIT_NUM)	//同步量控制参数
#define SYNC_CHARACTER_DELAY	GAME_FPS * 2	//同步延时
#define STATE_UPDATE_TIME		8			//魔法状态刷新间隔
#define DEFAULT_MASS			128			//单位：g
#define DEFAULT_GRAVITY			(8 * 512 / GAME_FPS / GAME_FPS)	//单位：点/帧^2
#define LOGOUT_WAITING_TIME		(GAME_FPS * 20)	//玩家下线等待20秒
#define DISCONNECT_TIME			(GAME_FPS * 4)	//跨服断线等待时间
#define LOGIN_TIME_OUT			(GAME_FPS * 200)	//登陆超时时间

//新旧方位转换表
static int g_DirectionChangeList[(ALL_DIRECTION + 1) * 2][ALL_DIRECTION + 1] =
{
	{6, UP_DIRECTION, CENTER_DIRECTION, DOWN_DIRECTION, UP_RIGHT_DIRECTION, RIGHT_DIRECTION, DOWN_RIGHT_DIRECTION, 0, 0},
	{6, UP_LEFT_DIRECTION, LEFT_DIRECTION, DOWN_LEFT_DIRECTION, UP_DIRECTION, CENTER_DIRECTION, DOWN_DIRECTION, 0, 0},

	{4, CENTER_DIRECTION, UP_DIRECTION, RIGHT_DIRECTION, UP_RIGHT_DIRECTION, 0, 0, 0, 0},
	{4, DOWN_LEFT_DIRECTION, LEFT_DIRECTION, DOWN_DIRECTION, CENTER_DIRECTION, 0, 0, 0, 0},

	{6, LEFT_DIRECTION, CENTER_DIRECTION, RIGHT_DIRECTION, UP_LEFT_DIRECTION, UP_DIRECTION, UP_RIGHT_DIRECTION, 0, 0},
	{6, DOWN_LEFT_DIRECTION, DOWN_DIRECTION, DOWN_RIGHT_DIRECTION, LEFT_DIRECTION, CENTER_DIRECTION, RIGHT_DIRECTION, 0, 0},

	{4, CENTER_DIRECTION, LEFT_DIRECTION, UP_DIRECTION, UP_LEFT_DIRECTION, 0, 0, 0, 0},
	{4, DOWN_RIGHT_DIRECTION, DOWN_DIRECTION, RIGHT_DIRECTION, CENTER_DIRECTION, 0, 0, 0, 0},

	{6, UP_DIRECTION, CENTER_DIRECTION, DOWN_DIRECTION, UP_LEFT_DIRECTION, LEFT_DIRECTION, DOWN_LEFT_DIRECTION, 0, 0},
	{6, UP_RIGHT_DIRECTION, RIGHT_DIRECTION, DOWN_RIGHT_DIRECTION, UP_DIRECTION, CENTER_DIRECTION, DOWN_DIRECTION, 0, 0},

	{4, CENTER_DIRECTION, LEFT_DIRECTION, DOWN_DIRECTION, DOWN_LEFT_DIRECTION, 0, 0, 0, 0},
	{4, UP_RIGHT_DIRECTION, UP_DIRECTION, RIGHT_DIRECTION, CENTER_DIRECTION, 0, 0, 0, 0},

	{6, LEFT_DIRECTION, CENTER_DIRECTION, RIGHT_DIRECTION, DOWN_LEFT_DIRECTION, DOWN_DIRECTION, DOWN_RIGHT_DIRECTION, 0, 0},
	{6, UP_LEFT_DIRECTION, UP_DIRECTION, UP_RIGHT_DIRECTION, LEFT_DIRECTION, CENTER_DIRECTION, RIGHT_DIRECTION, 0, 0},

	{4, CENTER_DIRECTION, DOWN_DIRECTION, RIGHT_DIRECTION, DOWN_RIGHT_DIRECTION, 0, 0, 0, 0},
	{4, UP_LEFT_DIRECTION, LEFT_DIRECTION, UP_DIRECTION, CENTER_DIRECTION, 0, 0, 0, 0},

	{0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static int g_SyncRegionList[ALL_DIRECTION + 1][ALL_DIRECTION + 1] =
{
	{3, RIGHT_DIRECTION, UP_RIGHT_DIRECTION, DOWN_RIGHT_DIRECTION, 0, 0, 0, 0, 0},
	{5, UP_RIGHT_DIRECTION, UP_DIRECTION, RIGHT_DIRECTION, UP_LEFT_DIRECTION, DOWN_RIGHT_DIRECTION, 0, 0, 0},
	{3, UP_DIRECTION, UP_LEFT_DIRECTION, UP_RIGHT_DIRECTION, 0, 0, 0, 0, 0},
	{5, UP_LEFT_DIRECTION, LEFT_DIRECTION, UP_DIRECTION, DOWN_LEFT_DIRECTION, UP_RIGHT_DIRECTION, 0, 0, 0},
	{3, LEFT_DIRECTION, UP_LEFT_DIRECTION, DOWN_LEFT_DIRECTION, 0, 0, 0, 0, 0},
	{5, DOWN_LEFT_DIRECTION, LEFT_DIRECTION, DOWN_DIRECTION, UP_LEFT_DIRECTION, DOWN_RIGHT_DIRECTION, 0, 0, 0},
	{3, DOWN_DIRECTION, DOWN_LEFT_DIRECTION, DOWN_RIGHT_DIRECTION, 0, 0, 0, 0, 0},
	{5, DOWN_RIGHT_DIRECTION, DOWN_DIRECTION, RIGHT_DIRECTION, DOWN_LEFT_DIRECTION, UP_RIGHT_DIRECTION, 0, 0, 0},
	{8, RIGHT_DIRECTION, UP_RIGHT_DIRECTION, UP_DIRECTION, UP_LEFT_DIRECTION, LEFT_DIRECTION, DOWN_LEFT_DIRECTION, DOWN_DIRECTION, DOWN_RIGHT_DIRECTION},
};

static int g_Direction2CoordinationList[ALL_DIRECTION][2] =
{
	{ 1, 0},
	{ 1, 1},
	{ 0, 1},
	{-1, 1},
	{-1, 0},
	{-1,-1},
	{ 0,-1},
	{ 1,-1}
};

// 原地复活间隔, 玩家两次原地复活在此间隔内,就会增加原地复活的等待时间
#define SITU_REVIVE_INTERVAL (20 * 60)

#define MAX_DEATH_PUNISH_TIME_SIZE 10
// 死亡惩罚时间,单位: 秒
static int gs_nDeathPunishSecond[MAX_DEATH_PUNISH_TIME_SIZE] = {15, 60, 90, 120, 150, 180, 210, 240, 270, 300};

// 商店每页要显示的最大物品个数
#define MAX_SHOP_PAGE_ITEM_COUNT 40
// 商店最大页数
#define MAX_SHOP_PAGES           8

#define MAX_OPEN_SHOP_DISTANCE   (6 * 6)

#define MAX_TRADING_BOX_SIZE    10

#define MAX_TRADING_DISTANCE    (6 * 6)
#endif	//_GLOBAL_H_
