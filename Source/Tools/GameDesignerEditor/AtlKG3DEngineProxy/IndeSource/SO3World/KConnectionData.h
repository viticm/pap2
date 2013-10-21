/************************************************************************/
/* 游戏服务与客户端的连接的状态数据                                     */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.26	Create												*/
/************************************************************************/
#ifndef _KCONNECTION_DATA_H_
#define _KCONNECTION_DATA_H_

class KPlayer;
#include "Common/KG_Socket.h"
#include "Common/KG_Package.h"

struct KConnectionData
{
    KConnectionData() : SocketPacker(3000)
	{
        piSocketStream    = NULL,
        pPlayer           = NULL;
    };

    KG_Packer               SocketPacker;
    IKG_SocketStream*       piSocketStream;
	KPlayer*			    pPlayer;
    DWORD                   dwLastPingTime;
};

#endif	//_KCONNECTION_DATA_H_
