//---------------------------------------------------------------------------
// 剑侠情缘网络版2 GameMasterCentral 互联互通
// 版权所有:       2006-2007 金山软件(Kingsoft)
// 创建日期与作者: 2006 越子(wooy)
//---------------------------------------------------------------------------
// GameMasterCentral与游戏世界的各种Master服务之间的协议接口
//---------------------------------------------------------------------------
#ifndef __GAME_MASTER_CENTRAL_GAMEMASTER_PROTOCOL_H__
#define __GAME_MASTER_CENTRAL_GAMEMASTER_PROTOCOL_H__
#include "ProtocolBasic.h"
#include "GameMasterServiceDef.h"

#pragma	pack(push, 1)

enum GAME_MASTER_PROTOCOL_LIST
{
	GM_P_NORMAL	= PROTOCOL_NORMAL_PACKAGE_BEGIN,
	GM_P_OPERATION_REQUEST,					//操作请求，数据包格式见GAME_MASTER_PROTOCOL_HEADER
	GM_P_OPERATION_REQUEST_ON_ACCOUNT,		//带被操作帐号名称信息的操作请求，数据包格式见GAME_MASTER_PROTOCOL_WITH_OBJNAME
	GM_P_OPERATION_REQUEST_ON_ROLE,			//带被操作角色名称信息的操作请求，数据包格式见GAME_MASTER_PROTOCOL_WITH_OBJNAME
	GM_P_OPERATION_REQUEST_ON_OBJECT,		//带被操作对象名称信息的操作请求，数据包格式见GAME_MASTER_PROTOCOL_WITH_OBJNAME
	GM_P_OPERATION_RESPONSE,				//请求处理结果
};

//跟随GM_P_NORMAL协议的子协议
enum GAME_MASTER_NORMAL_SUB_PROTOCOL
{
	GMN_P_SERVICE_INFO = 1,		//服务程序信息
	GMN_P_SERVICE_PING,
};

struct GAME_MASTER_SERVICE_PING : tagBiProtoHeaderNoLen
{
	unsigned int uTime;
};

//每个GameMaster服务连上GameMaster后先给GameMasterCentral发此数据包，说明自己所提供的服务类型与自己对应的区服
struct GAME_MASTER_SERVICE_INFO : tagBiProtoHeaderNoLen
{
	int					nAreaIndex;		//Area与Group如果同时为0表示不了解自己所地应的区服
	int					nGroupIndex;	//Area不为0同时Group为0表示对应于整个区，不区分服
	unsigned int		uServiceType;	//接入GameMasterCentral的MasterService服务程序所提供的服务的类型,见MASTER_SERVICE_TYPE
	char				szOperatorAccount[32];
	char				szOperatorPasswd[64];
	char				Reserved[12];
};

//带流水号的协议包头
struct GAME_MASTER_PROTOCOL_HEADER : PrtocolLengthHeader
{
	GAME_MASTER_PROTOCOL_INFO	ProtInfo;
};

//操作帐号的协议包头
struct GAME_MASTER_PROTOCOL_WITH_OBJNAME : GAME_MASTER_PROTOCOL_HEADER
{
	NAMED_OBJECT_IDENTIFIER	Obj;
};

//带流水号和数据块的协议包
struct GAME_MASTER_PROTOCOL_WITH_DATA : GAME_MASTER_PROTOCOL_HEADER
{
	DATA_CONTENT	Data;
};
#define GAME_MASTER_PROTOCOL_WITH_DATA_SIZE(n)		\
	((n > 0) ? (sizeof(GAME_MASTER_PROTOCOL_WITH_DATA) + n - 1) : sizeof(GAME_MASTER_PROTOCOL_HEADER))

//操作帐号和数据块的协议包
struct GAME_MASTER_PROTOCOL_WITH_OBJNAME_AND_DATA : GAME_MASTER_PROTOCOL_WITH_OBJNAME
{
	DATA_CONTENT	Data;
};
#define GAME_MASTER_PROTOCOL_WITH_OBJNAME_AND_DATA_SIZE(n)	\
	((n > 0) ? (sizeof(GAME_MASTER_PROTOCOL_WITH_OBJNAME_AND_DATA) + n - 1) : sizeof(GAME_MASTER_PROTOCOL_WITH_OBJNAME))

//带流水号和区服信息的协议包的包头
struct OPERATION_PROTOCOL_WITH_SVR_IDX_HEADER : GAME_MASTER_PROTOCOL_HEADER
{
	GAME_MASTER_SERVICE_SVR		ServiceSvr;
};

#pragma	pack(pop)

#endif// #ifndef __GAME_MASTER_CENTRAL_GAMEMASTER_PROTOCOL_H__
