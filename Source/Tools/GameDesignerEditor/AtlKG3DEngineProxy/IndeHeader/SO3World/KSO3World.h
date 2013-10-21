/************************************************************************/
/* 剑三游戏世界							                                */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.31	Create												*/
/* Comment	 :															*/
/*		包括所有的全局数据的声明，和地图的集合							*/
/************************************************************************/
#ifndef _KSO3WORLD_H_
#define _KSO3WORLD_H_

#include <set>
#include <vector>
#include <deque>

#include "Global.h"
#include "KCircleList.h"
#include "KWorldSettings.h"
#include "KFellowship.h"
#include "KScriptCenter.h"
#include "KCenterRemote.h"
#include "KScriptServer.h"
#include "KScriptClient.h"
#include "KItemManager.h"
#include "KShopCenter.h"
#include "KProfessionManager.h"
#include "KRoadManager.h"
#include "KDropCenter.h"
// 下面三个include源自旧的ObjectManager
#include "../../IndeSource/SO3World/KDoodad.h"
#include "../../IndeSource/SO3World/KNpc.h"
#include "KSkillManager.h"
#include "../../IndeSource/SO3World/KBuffManager.h"
#include "KTeamClient.h"
#include "KAIManager.h"
#include "KObjectIndex.h"
#include "KRecorderFactory.h"
#include "KGPQ.h"
#include "../../IndeSource/SO3World/KCampInfo.h"
#include "KTongClient.h"
#include "KAuctionClient.h"
#include "../../IndeSource/SO3World/KLogClient.h"
#include "../../IndeSource/SO3World/KTransmissionList.h"
#include "../../IndeSource/SO3World/KGlobalSystemValueCache.h"
#include "../../IndeSource/SO3World/KTeamServer.h"

class KScene;
class KCharacter;
class KNpc;
class KPlayer;
class KDoodad;
class KItem;
class ITextFilter;

#ifdef _CLIENT

struct _KMAIL
{
    _KMAIL(
        DWORD dwID, 
        char* pszSenderName, char* pszTitle, time_t lTime, 
        BYTE bUnRead, BYTE bMoney, BYTE bText, BYTE bItem, BYTE bSystem
    );

    int GetLeftSecond(int nMailSurvivalTime);

    DWORD       dwMailID;
    char        szSenderName[_NAME_LEN];
    char        szTitle[MAIL_TITLE_LEN];
    time_t      lReceiveTime;
    BYTE        bUnReadFlag;
    BYTE        bMoneyFlag;
    BYTE        bTextFlag;
    BYTE        bItemFlag;
    BYTE        bSystemFlag;
    BYTE        bGotContentFlag;
    int         nMoney;
    char        szText[MAX_MAIL_TEXT_LEN];
    DWORD       dwItemID;
};

#endif

enum KWORLD_MODE
{
    wmInvalid,
    wmNormal,
    wmOfflineEdit,
    wmOnlineEdit,
};

typedef int(*FUNC_LOADPROGRESS)(LPCTSTR pName,float a); 

class KSO3World
{
public:
    KSO3World();
	~KSO3World(void);

	BOOL Init(IRecorderFactory* piFactory);
	void UnInit(void);

#ifdef _CLIENT
    BOOL InitTextFilter();
#endif

	void Activate(void);

#ifdef _CLIENT
	KScene* NewEditorScene(int nRegionWidth, int nRegionHeight);
	KScene* LoadEditorScene(const char cszFileName[],FUNC_LOADPROGRESS);
#endif

#ifdef _SERVER
    void    ProcessLoadedScene();
#endif

#ifdef _CLIENT
	KScene* NewClientScene(DWORD dwMapID, int nMapCopyIndex);
#endif

	BOOL DeleteScene(KScene* pScene);

    // dwMapCopyIndex为0表示任意Copy,只要MapID对即可
	KScene* GetScene(DWORD dwMapID, int nMapCopyIndex);

	KNpc*   NewNpc(DWORD dwNpcID = ERROR_ID);
	BOOL    DeleteNpc(KNpc* pNpc, BOOL bKilled = false);
	//向游戏世界内添加Npc，该Npc的数据必须已经构建完成
	BOOL    AddNpc(KNpc* pNpc, KScene* pScene, int nX, int nY, int nZ);
	BOOL    RemoveNpc(KNpc* pNpc, BOOL bKilled = false);

	KDoodad*    NewDoodad(DWORD dwDoodadID = ERROR_ID);
	BOOL        DelDoodad(KDoodad* pDoodad);
	BOOL        AddDoodad(KDoodad* pDoodad, KScene* pScene, int nX, int nY, int nZ);
	BOOL        RemoveDoodad(KDoodad* pDoodad);

	KPlayer*    NewPlayer(DWORD dwPlayerID);
	BOOL        DelPlayer(KPlayer* pPlayer);
	BOOL        AddPlayer(KPlayer* pPlayer, KScene* pScene, int nX, int nY, int nZ);
	BOOL        RemovePlayer(KPlayer* pPlayer);

#ifdef _CLIENT
	DWORD			    m_dwClientPlayerID;		
	DWORD			    m_dwClientMapID;		
	int			        m_nClientMapCopyIndex;	    
#endif

public:
	int									m_nGameLoop;
    time_t                              m_nBaseTime;    // 基准时间,指第0帧的时间
    DWORD                               m_dwStartTime;  // 起始时间,指本SubWorld开始运行的时间
    int                                 m_nStartLoop;
	time_t								m_nCurrentTime;	
    KWORLD_MODE                         m_eWorldMode;

#ifdef _SERVER
    BOOL                                m_bFastBootMode;

    KTeamServer                         m_TeamServer;
#endif

#ifdef _CLIENT
    KTeamClient                         m_TeamClient;
#endif
    KWorldSettings	                    m_Settings;
    KGFellowshipMgr                     m_FellowshipMgr;
    KScriptCenter	                    m_ScriptCenter;

#ifdef _SERVER
    KCenterRemote                       m_CenterRemote;
    KScriptServer                       m_ScriptServer;
#else
    KScriptClient                       m_ScriptClient;
#endif

    KItemManager                        m_ItemManager;
    KShopCenter                         m_ShopCenter;
    KProfessionManager                  m_ProfessionManager;
    KRoadManager                        m_RoadManager;

	KObjectIndex<KScene>				m_SceneSet;
	KObjectIndex<KPlayer>				m_PlayerSet;
	KObjectIndex<KNpc>				    m_NpcSet;
	KObjectIndex<KDoodad>				m_DoodadSet;
	KObjectIndex<KItem>				    m_ItemSet;

#ifdef _CLIENT
    std::deque<KItem*>                  m_LinkItemQue;
#endif

    KSkillManager                       m_SkillManager;
    KBuffManager                        m_BuffManager;
    KAIManager                          m_AIManager;
    KGPQManager                         m_PQManager;

#ifdef _SERVER
    KDropCenter                         m_DropCenter;
#endif

#ifdef _CLIENT
    ITextFilter*                        m_piTextFilter;

    typedef std::vector<_KMAIL> KCLIENT_MAIL_VECTOR;
    KCLIENT_MAIL_VECTOR                 m_ClientMailList;
#endif

public:
    IRecorderTools*                     m_piRecorderTools;    
#ifdef _SERVER
    IRecorderSceneLoader*               m_piSceneLoader;
    BOOL                                m_bRunFlag;
    int                                 m_nIdleFrame;
#endif
    KCampInfo                           m_CampInfo;

#ifdef _CLIENT
    KTongClient                         m_TongClient;
    KAuctionClient                      m_AuctionClient;
    KGPQ::KPQ_STATISTICS_MAP            m_BattleFieldStatisticsMap;
    DWORD                               m_dwBattleFieldPQID;
    DWORD                               m_dwBattleFieldPQTemplateID;
    time_t                              m_nBattleFieldPQBeginTime;
    time_t                              m_nBattleFieldPQEndTime;
    int                                 m_nBattleFieldObjectiveCurrent[PQ_PARAM_COUNT];
    int                                 m_nBattleFieldObjectiveTarget[PQ_PARAM_COUNT];
#endif
#ifdef _SERVER
    KTransmissionList                   m_TransmissionList;
    KGlobalSystemValueCache             m_GlobalSystemValueCache;
#endif

#ifdef _CLIENT
    BOOL                                m_bForwardCharacterPosition;
#endif
};

extern KSO3World* g_pSO3World;

#ifdef _CLIENT

class IKSO3GameWorldUIHandler;
class IKSO3GameWorldRepresentHandler;
class IKSO3RepresentHandler;

extern IKSO3GameWorldUIHandler* g_pGameWorldUIHandler;
extern IKSO3GameWorldRepresentHandler* g_pGameWorldRepresentHandler;

void g_SetGameWorldUIHandler(IKSO3GameWorldUIHandler *pGameWorldUIHandler);
void g_SetGameWorldRepresentHandler(IKSO3GameWorldRepresentHandler *pGameWorldRepresentHandler);

#endif


struct KGetSceneFunc
{
	BOOL operator()(DWORD dwID, KScene* pScene);
	DWORD		m_dwMapID;
	int		    m_nMapCopyIndex;
	KScene*		m_pScene; // 返回值
};

struct KDeleteSceneFunc
{
	BOOL operator()(DWORD dwID, KScene* pScene);
};

struct KTraverseSceneFunc
{
	BOOL operator()(DWORD dwID, KScene* pScene);
};

#ifdef _SERVER
struct KTraversePlayerFunc
{
	BOOL operator()(DWORD dwID, KPlayer* pPlayer);
};

#endif

#endif	//_KSO3WORLD_H_
