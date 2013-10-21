/************************************************************************/
/* 剑三协议基础结构定义						                            */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.11.30	Create												*/
/************************************************************************/
#ifndef _SO3_PROTOCOL_BASIC_
#define _SO3_PROTOCOL_BASIC_

#include "ProtocolBasic.h"
#include "SO3GlobalDef.h"

#define UNUSED_PROTOCOL_ID			-1
#define UNDEFINED_PROTOCOL_SIZE		-1

#define INVALID_PROTOCOL_VERSION	-1
#define SWORD3_PROTOCOL_VERSION		 1

#pragma	pack(1)

/************************************************************************/
/* 常用协议中常用的共用结构												*/

#define QUEST_PARAM_COUNT		4		//任务参数个数

// 道具结构
enum ITEM_SYNC_STRUCT_TYPE
{
	isst_Common = 0,
	isst_CustomEquip,
	isst_Equip,
	isst_Train,
	isst_Total,
};

// 所有类型的装备同步必须从这个结构派生
struct COMMON_ITEM_SYNC
{
	DWORD	dwItemId;
	BYTE	byTabFile;
	BYTE	byVersion;
	WORD	wTabIndex;
	WORD	wDurability;
};

struct CUSTOM_EQUIP_SYNC : COMMON_ITEM_SYNC
{
	DWORD	dwRandSeed;
	WORD	wExtendMagic;
	WORD	wTemporaryMagic;
};

struct RAND_EQUIP_SYNC : COMMON_ITEM_SYNC
{
	BYTE	byQuality;
	WORD	wMagic[MAX_ITEM_MAGIC_COUNT];
	WORD	wExtendMagic;
	WORD	wTemporaryMagic;
	DWORD	dwRandSeed;
};

struct TRAIN_EQUIP_SYNC: RAND_EQUIP_SYNC
{
	char	szMadeBy[_NAME_LEN];
};
/************************************************************************/
/* 外网基础协议头定义						                            */

struct EXTERNAL_PROTOCOL_HEADER
{
	BYTE	byProtocolID;			//协议号
};

struct UPWARDS_PROTOCOL_HEADER : EXTERNAL_PROTOCOL_HEADER
{
	int		nFrame;				//当前帧数	
};

struct DOWNWARDS_PROTOCOL_HEADER : EXTERNAL_PROTOCOL_HEADER
{

};

struct UNDEFINED_SIZE_UPWARDS_HEADER : UPWARDS_PROTOCOL_HEADER
{
	WORD	wSize;
};

struct UNDEFINED_SIZE_DOWNWARDS_HEADER : DOWNWARDS_PROTOCOL_HEADER
{
	WORD	wSize;
};

#pragma pack()

#define REGISTER_EXTERNAL_FUNC(ProtocolID, FuncName, ProtocolSize)	\
{m_ProcessProtocolFuns[ProtocolID] = FuncName;					\
	m_nProtocolSize[ProtocolID] = ProtocolSize;}						

/************************************************************************/
/* 内网基础协议头定义						                            */
//1.协议族名称命名规则：
//	以连接划分命名
//		"Server端名称"_"Client端名称"_connection
//		其中字母全小写
//		e.g		bishop_relay_connection		（Bishop和Relay的连接，其中Bishop是Server端，Relay是Client端）
//	以模块划分命名
//		["Server端名称缩写""Client端名称缩写"]_"模块描述"_module
//		其中字母全部小写
//		e.g.	relay_package_module
//				rg_dynamic_map_module
//2.协议号枚举类型命名规则
//		协议族命名的后缀Connection，替换成Protocol
//		字母全大写
//		e.g.	BISHOP_RELAY_PROTOCOL		（对应于协议族bishop_relay_connection）
//3.协议号名称命名规则
//		["发送端名称缩写"2"接收端名称缩写"_]["功能模块名称缩写"_]"功能描述"
//		字母全小写
//		功能描述的英文单词之间用_分割
//		e.g.	b2r_player_login			（Bishop通知Relay玩家登陆的协议）
//				g2r_dml_apply_mapcopy		（GS向Relay申请地图副本的协议，属于动态地图功能模块）
//4.协议结构名称的命名规则
//		将协议号名称变成全大写即可
//		e.g.	B2R_PLAYER_LOGIN
//5.相关协议处理函数的命名规则
//		On"功能描述"
//		功能描述中英文单词的首字母大写
//		e.g		OnPlayerLogin

enum INTERNAL_PROTOCOL_FAMILY
{
	invalid_protocol_family = 0,
	bishop_relay_connection,
	relay_gs_connection,
	relayserver_relay_connection,
	relay_package_module,


};

//协议个数定义
#define MAX_PROTOCOL_ID_NUM			256
#define MAX_PROTOCOL_FAMILY_NUM		8

#pragma	pack(1)

typedef struct _INTERNAL_PROTOCOL_HEADER
{
	BYTE	byProtocolFamily;		//协议族
	BYTE	bReserved;
	WORD	wProtocolID;			//协议号
}INTERNAL_PROTOCOL_HEADER;

typedef struct _IDENTITY_HEADER : INTERNAL_PROTOCOL_HEADER
{
	DWORD		dwPacketIdentity;		//数据包的唯一标记，供验证返回路由使用
}IDENTITY_HEADER;

#pragma pack()

#define REGISTER_FUNC(_PROTOCOL_FAMILY_, _PROTOCOL_ID_, _FUNC_NAME_)	\
	m_ProcessProtocolFuns[(_PROTOCOL_FAMILY_ << 8) + _PROTOCOL_ID_] = \
	(PROCESS_PROTOCOL_FUNC)_FUNC_NAME_

#endif	//_SO3_PROTOCOL_BASIC_