#ifndef _KG_GLOBAL_DEF_H_
#define _KG_GLOBAL_DEF_H_

// 下面几个宏涉及Game server在GameCenter上的索引号分配,以及逻辑里面的ID分配策略
// 所有的GameServer上分配的ID最高4位前缀由GameCenter分配,这个前缀的数值应该等于Game server在
// GameCenter上的索引号,索引号值域为[1, 14]
// 前缀为0x0的ID为玩家角色ID,所以GameCenter不会把0x0分配给GameServer作为索引号(也是ID前缀数值)
// 前缀为0xf的ID为客户端占用,所以GameCenter也不会把0xf分配给GameServer作为索引号(也是ID前缀数值)
// 去掉0x0, 0xf, 所以最多一组服务器允许14个GameServer
#define ID_PREFIX_BIT_NUM	4
#define MAX_GAME_SERVER		14
#define PLAYER_ID_PREFIX	0x0
#define CLIENT_ID_PREFIX	0xf



//场景和游戏世界的空间划分大小定义
//以下数据都是2的幂次
#define MAX_REGION_WIDTH_BIT_NUM	6	//地图中X坐标上最多的Region个数
#define MAX_REGION_HEIGHT_BIT_NUM	6	//地图中Y坐标上最多的Region个数

#define REGION_GRID_WIDTH_BIT_NUM	6	//Region中X坐标上最多的Cell个数
#define REGION_GRID_HEIGHT_BIT_NUM	6	//Region中Y坐标上最多的Cell个数

#define CELL_LENGTH_BIT_NUM			5	//格子中的象素点精度
#define ALTITUDE_BIT_NUM			6

#define MAX_Z_ALTITUDE_BIT_NUM		16	//Z轴的最大值位数(ALTITUDE)
#define MAX_Z_POINT_BIT_NUM         (MAX_Z_ALTITUDE_BIT_NUM + ALTITUDE_BIT_NUM)

#define MOVE_DEST_RANGE_BIT_NUM		12
#define MOVE_DEST_RANGE				(1 << MOVE_DEST_RANGE_BIT_NUM)

#define	MAX_VELOCITY_XY_BIT_NUM		(CELL_LENGTH_BIT_NUM + 2)
#define MAX_VELOCITY_Z_BIT_NUM		(ALTITUDE_BIT_NUM + 4)
// 用于水平速度收敛计算时消除计算误差,注意,这个值最好不要小于收敛计算时的分母数值
#define  VELOCITY_ZOOM_BIT_NUM      4
#define  VELOCITY_ZOOM_COEFFICIENT (1 << VELOCITY_ZOOM_BIT_NUM)

#define MAX_ZOOM_VELOCITY_BIT_NUM   (MAX_VELOCITY_XY_BIT_NUM + VELOCITY_ZOOM_BIT_NUM)
#define MAX_ZOOM_VELOCITY           ((1 << MAX_ZOOM_VELOCITY_BIT_NUM) - 1)

#define MAX_VELOCITY_XY				((1 << MAX_VELOCITY_XY_BIT_NUM) - 1)
// 注意,Z轴速度是有符号,可以为负值的,目前取值区间为[-512, 511], 10 bits
#define MAX_VELOCITY_Z				((1 << (MAX_VELOCITY_Z_BIT_NUM - 1)) - 1)
#define MIN_VELOCITY_Z				(-(1 << (MAX_VELOCITY_Z_BIT_NUM - 1)))

#define MAX_X_COORDINATE_BIT        (MAX_REGION_WIDTH_BIT_NUM  + REGION_GRID_WIDTH_BIT_NUM  + CELL_LENGTH_BIT_NUM)
#define MAX_Y_COORDINATE_BIT        (MAX_REGION_HEIGHT_BIT_NUM + REGION_GRID_HEIGHT_BIT_NUM + CELL_LENGTH_BIT_NUM)
#define MAX_X_COORDINATE            ((1 << MAX_X_COORDINATE_BIT) - 1)
#define MAX_Y_COORDINATE            ((1 << MAX_Y_COORDINATE_BIT) - 1)

#define MAX_SYNC_PER_REGION_BIT_NUM	7	//同步量控制参数


//坐标相关定义
#define MAX_REGION_WIDTH			(1 << MAX_REGION_WIDTH_BIT_NUM)
#define MAX_REGION_HEIGHT			(1 << MAX_REGION_HEIGHT_BIT_NUM)
#define MAX_Z_ALTITUDE				((1 << MAX_Z_ALTITUDE_BIT_NUM) - 1)
#define MAX_Z_POINT				    ((1 << MAX_Z_POINT_BIT_NUM) - 1)

#define REGION_GRID_WIDTH			(1 << REGION_GRID_WIDTH_BIT_NUM)
#define REGION_GRID_HEIGHT			(1 << REGION_GRID_HEIGHT_BIT_NUM)

#define CELL_LENGTH					(1 << CELL_LENGTH_BIT_NUM)
#define POINT_PER_ALTITUDE			(1 << ALTITUDE_BIT_NUM)
#define ALTITUDE_2_CELL				(POINT_PER_ALTITUDE * LOGICAL_CELL_CM_LENGTH/ ALTITUDE_UNIT / CELL_LENGTH)

#define MAX_GRAVITY_BIT_NUM          5
#define MAX_GRAVITY                 ((1 << MAX_GRAVITY_BIT_NUM) - 1)

#define LOGICAL_CELL_CM_LENGTH		50
#define _3D_CELL_CM_LENGTH			100

// 地表高度计量单位(厘米)
#define ALTITUDE_UNIT				((LOGICAL_CELL_CM_LENGTH) / 4.0f)
#define TERRAIN_MIN_HEIGHT			(-65536.0f / 4.0f * ALTITUDE_UNIT)
#define TERRAIN_MAX_HEIGHT			(65536.0f / 4.0f * ALTITUDE_UNIT)

#define ALTITUDE_TO_XYPOINT(Altitude)   ((Altitude) * 8)
#define ZPOINT_TO_XYPOINT(Z)            ((Z) / 8)
#define XYPOINT_TO_ZPOINT(XY)           ((XY) * 8)
#define ZPOINT_TO_ALTITUDE(Z)           ((Z) / POINT_PER_ALTITUDE)

#define AI_PARAM_COUNT		8

#define DIALOG_RANGE		(6 * CELL_LENGTH)	 //可以对话的距离
#define DIALOG_ACTION_ID	1					//对话的表情ID

#define	MAX_TARGET_RANGE		(CELL_LENGTH * REGION_GRID_WIDTH * 2)
#define MAX_LOOT_RANGE			(CELL_LENGTH * REGION_GRID_WIDTH * 2)

// 爬坡障碍高度差
#define CLIMB_ABILITY 	    (XYPOINT_TO_ZPOINT(CELL_LENGTH))

// 主角移动时定位前方目标点的距离,30米
#define MOVE_TARGET_RANGE   (30 * 100 * 32 / 50)

#define MAX_MOVE_STATE_BIT_NUM  4
#define MAX_MOVE_STATE_VALUE    ((1 << MAX_MOVE_STATE_BIT_NUM) - 1)

enum
{
	REPRESENT_EQUIP_MELEE_WEAPON,
	REPRESENT_EQUIP_HELM,
	REPRESENT_EQUIP_CHEST,
	REPRESENT_EQUIP_WAIST,
	REPRESENT_EQUIP_PANTS,
	REPRESENT_EQUIP_BANGLE,
	REPRESENT_EQUIP_EXTEND,

	REPRESENT_FACE,				//不是装备，包括脸型，发型，新手上下装

	REPRESENT_COUNT				//需要显示的装备个数
};



// AI类型最好只往后面增加
// 如果要中间插入或删除,小心有些函数指针数组出错
enum AI_TYPE
{
	aitInvalid = 0,

	aitBase,
	aitPlayer,
	aitWolf,

	aitTotal
};

// AI的行为状态
enum KAI_STATE
{
	aisInvalid = 0,

	aisIdle,        // 发呆,通常指到达某目标后的空闲
	aisWander,		// 闲逛
	aisPatrol,      // 巡逻到某个点的过程中
	aisFollow,      // 跟随
	aisAlert,       // 盯着目标,可能是在aisPatrol中发现一个目标,然后盯着它,也可能是Idle是发现目标,然后盯着它
	aisPursuit,     // 追击
	aisKeepAway,    // 后退
	aisEscape,      // 逃跑
	aisReturn,      // Attack完成以后返回
};

enum AI_SKILL_TYPE
{
	aistInvalid = 0,

	aistDamage,
	aistHeal,
	aistDot,
	aistHot,
	aistBuff,
	aistDebuf,
	aistControl,
	aistStun,
	aistSpecial1,
	aistSpecial2,

	aistTotal
};

//角色体形
enum ROLE_TYPE
{
	rtInvalid = 0,

	rtStandardMale,     // 标准男
	rtStandardFemale,   // 标准女
    rtStrongMale,       // 魁梧男
    rtSexyFemale,       // 性感女
    rtLittleBoy,        // 小男孩
    rtLittleGirl,       // 小孩女

	rtTotal
};

enum KBUFF_REPRESENT_POS
{
    brpInvalid,
    brpTrample,     // 脚下踩着的
    brpBody,        // 身上的
    brpTopHead,     // 顶在头上的
    brpTotal
};


//角色交互动作
enum CHARACTER_ACTION_TYPE
{
	catInvalid = 0,

	catDialogue,		//对话
	catHowl,			//嚎叫

	catTotal
};

//势力相关定义
enum FORCE_RELATION_TYPE
{
	frtInvalid = 0,

	frtExalted,
	frtRevered,
	frtHonored,
	frtFriendly,

	frtNeutral,

	frtUnfriendly,
	frtHostile,
	frtHated,

	frtTotal
};

enum KCAMP
{
    cNeutral, // 中立阵营
    cGood,    // 正义阵营
    cEvil     // 邪恶阵营
};

enum SCENE_OBJ_RELATION_TYPE
{
	sortInvalid = 0,

	sortNone	= 1,
	sortSelf	= 2,
	sortAlly	= 4,
	sortEnemy	= 8,
	sortDialog	= 16,
	sortParty	= 32,

	sortAll	= sortNone | sortAlly | sortEnemy | sortSelf | sortDialog | sortParty,

	sortTotal,
};

struct KForceRelation
{
	int						nRelation;
	FORCE_RELATION_TYPE		eRelationType;
};

// 道具相关
#define MAX_ITEM_BASE_COUNT		6
#define	MAX_ITEM_REQUIRE_COUNT	6
#define	MAX_ITEM_MAGIC_COUNT	6


//配置文件路径及后缀
#define SETTING_DIR			"Settings"	//配置文件目录

#define MAP_LIST_FILE_NAME	"MapList.tab"

//脚本回调函数名
#define SCRIPT_ON_LOAD		"OnLoad"

//地图障碍和场景物体
#define MAP_DIR				"maps"			//目录
#define MAP_FILE_EXT		"map"			//后缀
#define OBJ_FILE_EXT		"obj"			//后缀

//字符串长度定义
#define TITLE_LEN			32
#define TASK_NAME_LEN		256
#define FUNC_NAME_LEN		32
#define MAX_CHAT_TEXT_LEN	768

//扩展点
#define MAX_EXT_POINT_COUNT   8

//势力相关定义
#define MAX_FORCE_COUNT			64
#define FORCE_NAME_LEN			32

//地图相关设定
#define MAX_MAP_INDEX				256			//最大地图编号

#define MAP_MAP_ID					"MapID"
#define MAP_MAP_NAME				"MapName"
#define MAP_DROP_NAME				"MapDrop"
#define MAP_MAP_TYPE				"MapType"
#define MAP_INIT_GROUP				"InitGroup"
#define MAP_MAP_PERFORM				"MapPerform"
#define MAP_RESOURCE_FILE_PATH		"ResourceFilePath"
#define MAP_NPC_RELIVE_TAB_NAME		"NpcReliveList"
#define MAP_DOODAD_RELIVE_TAB_NAME	"DoodadReliveList"
#define MAP_NPC_TEAM_TAB_NAME		"NpcTeamList"
#define MAP_PATROL_PATH_TAB_NAME	"PatrolPathList"
#define MAP_REVIVE_IN_SITU          "ReviveInSitu"

//杂项
#define MAX_EXTERNAL_PACKAGE_SIZE	    1000		//最大外网数据包的大小
#define MAX_EXTERNAL_PACKAGE_SIZE_ALIGN	1024		//最大外网数据包的大小的对齐数字,方便分配

#define _PERFORMANCE_OPTIMIZATION
#ifdef _PERFORMANCE_OPTIMIZATION
    extern int g_nGameFps;
    #define GAME_FPS					g_nGameFps			//游戏帧数
#else
    #define GAME_FPS                    16
#endif


// 组队相关定义
// PARTY: 组队的队伍
#define INVALID_PARTY_ID    ((DWORD)(0x7fff0000))
// 整个游戏世界的最大允许的队伍数
#define MAX_PARTY_COUNT     4096
// 一支队伍允许的最大玩家数目
#define MAX_PARTY_SIZE      8

#define INVALID_SKILL_ID    0

// 队伍的物品分配模式
enum PARTY_LOOT_MODE
{
    plmInvalid = 0,

	plmAbsoluteFree,	// 绝对自由
	plmFreeForAll,		// 自由拾取
    plmRoundRobin,		// 轮流拾取
	plmLeaderLoot,		// 队长分配
	plmGroupLoot,		// 队伍拾取

	plmTotal           
};

enum INVENTORY_TYPE
{
	ivtEquipment = 0,
	ivtPackage,
	ivtBank,
	ivtSlot,
	ivtTrade,
	ivtSoldlist,
	ivtTotal,
};

enum EQUIPMENT_INVENTORY_TYPE
{
	eitMeleeWeapon = 0,		//近战武器
	eitRangeWeapon,			//远程武器
	eitChest,				//上衣
	eitHelm,				//头部
	eitAmulet,				//项链
	eitLeftRing,			//左手戒指
	eitRightRing,			//右手戒指
	eitWaist,				//腰带
	eitPendant,				//腰缀
	eitPants,				//裤子
	eitBoots,				//鞋子
	eitBangle,				//护臂
	eitExtend,				//扩展类装备，酒壶之类的
	eitPackage1,			//扩展背包1
	eitPackage2,			//扩展背包2
	eitPackage3,			//扩展背包3
	eitPackage4,			//扩展背包4
	eitArrow,				//暗器
	eitTotal
};

// ----------------- TALK ---------------------------->
#define CLIENT_TALK_LINK_ITEM_BUFFER_SIZE 1024

#define TALK_ERROR_S_OK                 0
#define TALK_ERROR_E_ERROR              1
#define TALK_ERROR_E_OUT_OF_BUFFER      2
#define TALK_ERROR_E_NOT_IN_PARTY       3
#define TALK_ERROR_E_NOT_IN_SENCE       4
#define TALK_ERROR_E_PLAYER_NOT_FOUND   5

enum TALK_DATA_BLOCK_TYPE
{
    tdbInvalid = 0,
    tdbText,
    tdbLinkName,
    tdbLinkItem,

    tdbTotal
};

// 群聊范围
enum PLAYER_TALK_RANGE
{
    trInvalid = 0,
    trNearby,       // 近聊
    trParty,        // 小队聊天
    trSence,        // 同场景聊天（城聊）
    trSystem,       // 系统消息
    trWhisper,      // 密聊
    trWhisperReply,

    trCustomCh1 = 100,
    trCustomCh2,
    trCustomCh3,
    trCustomCh4,
    trCustomCh5,
    trCustomCh6,
    trCustomCh7,
    trCustomCh8,

    trTotal
};
// <---------------------------------------------------

#ifdef WIN32
#define STR_CASE_CMP(szA, szB) _mbsicmp((unsigned char*)(szA), (unsigned char*)(szB))
#else   // linux
#define STR_CASE_CMP(szA, szB) strcasecmp(szA, szB)
#endif

#endif	//_KG_GLOBAL_DEF_H_