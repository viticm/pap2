/************************************************************************/
/* 总控服务器类，封装了Relay的主线程调度                                */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.11.19	Create												*/
/************************************************************************/
#ifndef _KSO3_GAME_CENTER_H_
#define _KSO3_GAME_CENTER_H_
#include "KMapManager.h"
#include "KRoleManager.h"
#include "KTongManager.h"
#include "KFellowshipCenter.h"
#include "KGameServer.h"
#include "KMailManager.h"
#include "KSO3GameCenterSettings.h"
#include "KRoleDB.h"
#include "KGameCenterEyes.h"
#include "KGatewayAgency.h"
#include "KScriptManager.h"
#include "KMiscDB.h"
#include "KBattleFieldManager.h"
#include "KGPQCenter.h"
#include "KCampManager.h"
#include "KRemoteScript.h"
#include "KGlobalCustomDataManager.h"
#include "KGlobalSystemValueManager.h"
#include "KTeamCenter.h"
#include "KIDSequence.h"
#include "KStatDataManager.h"
#include "KRoadCenter.h"
#include "KActivityManager.h"
#include "KApexProxy.h"
#include "KGameCardManager.h"
#include "KLogServerAgency.h"
#include "KAntiFarmerManager.h"
#include "KMentorCenter.h"

class KSO3GameCenter
{
public:
    KSO3GameCenter();

    BOOL Init(void);
    void UnInit();

    void Run();
    
    void Activate();
    
    void Quit();

    BOOL CallMainScript(const char cszFunction[]);

    BOOL CheckQuitComplete();

public:
    time_t                      m_nBaseTime;
    time_t                      m_nTimeNow;
    int                         m_nWorkLoop;
    int                         m_nIdleLoop;

    KGatewayAgency              m_Gateway;

    KGameServer                 m_GameServer;

    KMapManager		            m_MapManager;
    KRoleManager                m_RoleManager;

    KTongManager		        m_TongManager;
    KGFellowshipCenter	        m_FellowshipCenter;

    KMailManager                m_MailManager;

    KAuctionManager             m_AuctionManager;

    KTeamCenter                 m_TeamCenter;

    KMiscDB                     m_MiscDB;

    KSO3GameCenterSettings      m_Settings;

    KHowntownList               m_HometownList;
    
    KRoleDB                     m_RoleDB;

    KBattleFieldManager         m_BattleFieldManager;
    
    KGameCenterEyes             m_Eyes;

    KScriptManager              m_ScriptManager;

    KRemoteScript               m_RemoteScript;

    KGPQCenter                  m_PQCenter;
    
    KCampManager                m_CampManager;

    KGlobalCustomDataManager    m_GlobalCustomDataManager;

    KGlobalSystemValueManager   m_GlobalSystemValueManager;

    KStatDataManager            m_StatDataManager;

    IKIDSequence*               m_piSequence;

    ITextFilter*                m_piTextFilter;

    KRoadCenter                 m_RoadCenter;

    KActivityManager            m_ActivityManager;

    KApexProxy                  m_ApexProxy;

    KGameCardManager            m_GameCardManager;

    KLogServerAgency            m_LogServerAgency;

    KAntiFarmerManager          m_AntiFarmerManager;

    KMentorCenter               m_MentorCenter;

    BOOL                        m_bQuitFlag;

    int                         m_nZoneChargeFlag;
};

extern KSO3GameCenter* g_pSO3GameCenter;

#endif	//_KSO3_GAME_CENTER_H_
