/************************************************************************/
/* 游戏服务器类				                                            */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.04	Create												*/
/* Comment	 :															*/
/*		封装了整个游戏服务器											*/
/************************************************************************/
#ifndef	_KSO3_GAME_SERVER_H_
#define _KSO3_GAME_SERVER_H_

#include "KGameServerEyes.h"

struct IRecorderTools;

class KSO3GameServer
{    
public:
    KSO3GameServer();

	BOOL Init();
	void UnInit();

	void Run();

    BOOL ShowPerformance(int nIdleFrame);

    BOOL LoadConfig();

private:
    KGameServerEyes m_Eyes;
    float           m_fIdleFrame;
    float           m_fLastShowIdle;
    IRecorderTools* m_piRecorderTools;
    int             m_nRunMode;
    char            m_szLocale[64];
};

#endif	//_KSO3_GAME_SERVER_H_
