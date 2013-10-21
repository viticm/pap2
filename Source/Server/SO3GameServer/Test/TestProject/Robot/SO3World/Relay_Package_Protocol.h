/************************************************************************/
/* 中转协议定义								                            */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* Comment	 :															*/
/*		中转协议处理规则												*/
/*		1. 来自子节点													*/
/*			如果接收到的IP地址是本机的IP和端口，则处理之				*/
/*			否则，根据寻址数据查找相连的机器中，是否有符合的，有则发送	*/
/*			否则，向父节点发送											*/
/*		2. 来自父节点													*/
/*			如果接收到的IP地址是本机的IP和端口，则处理之				*/
/*			否则，根据寻址数据查找相连的机器中，是否有符合的，有则发送	*/
/*			否则，返回Loseway出错数据包									*/
/* History	 :															*/
/*		2004.12.13	Create												*/
/************************************************************************/
#ifndef _RELAY_PACKAGE_PROTOCOL_H_
#define _RELAY_PACKAGE_PROTOCOL_H_

enum RELAY_PACKAGE_PROTOCOL
{
	relay_package_module_begin = 0,

	rp_seek_ipport,
	rp_seek_map,
	rp_seek_player,
	rp_seek_account,
	rp_seek_gm,

	rp_loseway,
	
	relay_package_module_end
};

#pragma	pack(1)
struct RP_SEEK_IPPORT : INTERNAL_PROTOCOL_HEADER
{
	IP_ADDR_PORT	DestAddr;
	IP_ADDR_PORT	SrcAddr;
	size_t			nDataSize;
	char			Data[1];
};

struct RP_SEEK_MAP : INTERNAL_PROTOCOL_HEADER
{
	int				nMapID;
	int				nMapCopyIndex;
	IP_ADDR_PORT	DestAddr;
	IP_ADDR_PORT	SrcAddr;
	size_t			nDataSize;
	char			Data[1];
};

struct RP_SEEK_PLAYER : INTERNAL_PROTOCOL_HEADER
{
	__int64			nPlayerIndex;
	IP_ADDR_PORT	DestAddr;
	IP_ADDR_PORT	SrcAddr;
	size_t			nDataSize;
	char			Data[1];
};

struct RP_SEEK_ACCOUNT : INTERNAL_PROTOCOL_HEADER
{
	char			szAccountName[32];
	IP_ADDR_PORT	DestAddr;
	IP_ADDR_PORT	SrcAddr;
	size_t			nDataSize;
	char			Data[1];
};

struct RP_SEEK_GM : INTERNAL_PROTOCOL_HEADER
{
	IP_ADDR_PORT	DestAddr;
	IP_ADDR_PORT	SrcAddr;
	size_t			nDataSize;
	char			Data[1];
};

struct RP_LOSEWAY : INTERNAL_PROTOCOL_HEADER
{
	IP_ADDR_PORT	DestAddr;
	IP_ADDR_PORT	SrcAddr;
	size_t			nDataSize;
	char			Data[1];
};

#pragma pack()

#endif	//_RELAY_PACKAGE_PROTOCOL_H_