//---------------------------------------------------------------------------
// 剑侠情缘网络版2 GameMasterCentral 互联互通
// 版权所有:       2006-2007 金山软件(Kingsoft)
// 创建日期与作者: 2006.5 越子(wooy)
//---------------------------------------------------------------------------
//	GameMasterCentral是游戏内外互联互通的中枢，其一个重要功能就是把游戏世界的
//GameMaster服务与游戏世界外的各种应用系统挂接起来。GameMasterCentral在转递游
//戏世界提供的各种GameMaster服务，此头文件定义了许多与GameMaster服务的网络数据
//包传递有关的信息，GM协议将引用到这些定义。请参考"GameMasterCentral的GM服务协议体系.wps"
//文档了解GM协议体系。
//---------------------------------------------------------------------------
#ifndef __GAME_MASTER_CENTRAL_GAME_MASTER_SERVICE_DEF_H__
#define __GAME_MASTER_CENTRAL_GAME_MASTER_SERVICE_DEF_H__

//接入GameMasterCentral的MasterService服务程序所提供的服务的类型
enum MASTER_SERVICE_TYPE
{
	MST_GAME_ONLINE_OBJECTIVE	= 0x01,		//提供对在线游戏对象的操作
	MST_GAME_OFFLINE_OBJECTIVE	= 0x02,		//提供对非在线游戏对象的操作
	MST_SERVER_RUNTIME_INFO		= 0x04,		//提供服务程序在线信息状态服务
	MST_SERVER_MAINTENTANCE		= 0x08,		//提供服务程序维护服务
	MST_GAME_DATA_MAINTENTANCE	= 0x10,		//提供游戏数据维护服务
};

//GameMaster协议操作的分类关键字
enum GAME_MASTER_OPERATION_FAMILY
{
	GMOF_RESERVED = 0,
	GMOF_SYSTEM,		//服务系统互联
	GMOF_RET,			//反馈消息(操作结果数据包)

	GMOF_SINFO,			//系统信息监视
	GMOF_SNCTRL,		//游戏服务系统一般控制
	GMOF_SSCTRL,		//游戏服务系统特殊控制
	GMOF_ANCTRL,		//帐号一般控制
	GMOF_RNCTRL,		//角色一般控制
	GMOF_RSCTRL,		//角色特殊控制
	GMOF_RINFO,			//角色信息查询
	GMOF_GSINFO,		//游戏世界信息
	GMOF_GSCTRL,		//游戏世界控制
	GMOF_GMCTRL,		//GM帐号角色控制
	GMOF_ASTINFO,		//辅助信息(玩家发给GM的信息，公聊信息、请求发送短讯等)

	GMOF_NUM,
};

//请求的来源处分类
enum	MSG_PORT_TYPE
{
	MSG_PORT_TYPE_NONE = 0,
	MSG_PORT_TYPE_GAMESPACE,			//游戏世界
	MSG_PORT_TYPE_CONSOLE,				//操作界面直接输入/出
	MSG_PORT_TYPE_ACCOUNT_SYS,			//帐号服务器
	MSG_PORT_TYPE_SVR_DOOR,				//对外提供GM服务的接入口
	MSG_PORT_TYPE_SVR_DOOR_SECURITY,	//带加密的对外提供GM服务的接口
	MSG_PORT_TYPE_GAMESPACE_MAINTENANCE,//游戏世界的维护操作服务接入口
};

#pragma	pack(push, 1)

//请求的流水号
union SERIAL_NO
{
	struct	SERIAL_NO_BY_DETAIL
	{
		unsigned int	uBatchNo;			//批次号
		unsigned char	cSourceIndex;		//来源索引(这个地方限制死了每一个服务最多只有256个连接！)
		unsigned char	cSourceType;		//取值见MSG_PORT_TYPE
		unsigned short	cValidatNo;			//验证码
	}		ByDetail;
	struct SERIAL_NO_BY_REF_NO
	{
		unsigned int	uRefNo[2];			//组合后的流水号
	}		ByRefNo;
};

//GM协议的基本信息
struct GAME_MASTER_PROTOCOL_INFO 
{
	enum
	{
		OPERATION_KEY_WORD_MAX_LEN = 12,
	};
	unsigned char	OperationFamily;	//协议分类关键值,取值来自枚举GAME_MASTER_OPERATION_FAMILY
	SERIAL_NO		SerialNo;
	char			szOperationKeyWord[OPERATION_KEY_WORD_MAX_LEN];
};

//区服信息
struct GAME_MASTER_SERVICE_SVR
{
	short	nAreaIndex;						//分区索引
	short	nGroupIndex;					//分组索引（既甲区乙服中的乙）
};

//对象信息，可用于描述一个账号名、一个角色名、一个帮会名等方式来描述指明一个对象。
struct NAMED_OBJECT_IDENTIFIER
{
	enum
	{
		OBJECT_NAME_MAX_LEN = 32,
	};
	char				szName[OBJECT_NAME_MAX_LEN];
};

//数据内容
struct DATA_CONTENT
{
	unsigned short	uContentLength;
	char			Content[1];		//实际存储长度为uContentLength指出的那么长
};

//--------------------------------------------------------------
//	以上部分已经被wooy检查确定需要保留
//--------------------------------------------------------------
enum COMMAND_TYPE
{
	SHOW_SERVER_LIST,		// 显示服务器列表
	GMTOOLS_COMMAND,		// 命令行模拟执行gmtools的协议

	COMMAND_NUM,
};

struct COMMAND_CELL
{
	enum
	{
		MAX_COMMAND_TYPE_LEN = 16,
		MAX_COMMAND_TEXT_LEN = 256,
	};

	char	szCommandType[MAX_COMMAND_TYPE_LEN];
	char	szCommandText[MAX_COMMAND_TEXT_LEN];
};

#pragma	pack(pop)

#endif //__GAME_MASTER_CENTRAL_GAME_MASTER_SERVICE_DEF_H__
