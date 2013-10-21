/************************************************************************/
/* 游戏世界对象管理器                                                   */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.03.30	Create												*/
/* Comment	 :															*/
/*		将所有牵涉内存分配的对象，都集中到这里，这个类本身没什么功能	*/
/************************************************************************/
#ifndef _KOBJECT_MANAGER_H_
#define _KOBJECT_MANAGER_H_

#include <vector>
using namespace std;

#include "KBaseObjectSet.h"
#include "KBaseObjLiteSet.h"

#include "KAttribute.h"
#include "KCircleList.h"


#include "KNpc.h"
#include "KPlayer.h"
#include "KDoodad.h"

#include "KItem.h"
#include "KLootList.h"

#include "KThreatList.h"

//#include "KSkill.h"
//#include "KBuffList.h"

class KScene;
class KRegion;
class KNpc;
class KPlayer;
class KItem;
class KDoodad;


class KObjectManager
{
public:
	KObjectManager(void);
	~KObjectManager(void);

	BOOL Init(int nWorldIndex);
//场景
	KBaseObjectSet<KScene>				m_SceneSet;
	KBaseObjLiteSet<KRegion>			m_RegionSet;
	KBaseObjLiteSet<KCell>				m_CellSet;
//场景物体
	//KBaseObjectSet<KPlayer>				m_PlayerSet;
	//KBaseObjectSet<KNpc>				m_NpcSet;
	//KBaseObjectSet<KDoodad>				m_DoodadSet;
//道具
	KBaseObjectSet<KItem>				m_ItemSet;
//其他
	KBaseObjLiteSet<KLootList>			m_LootListSet;
	KBaseObjLiteSet<KAttribute>			m_AttributeSet;
	
	//不带值的仇恨节点
	KBaseObjLiteSet<KThreatLiteNode>	m_ThreatLiteNodeSet;

#if defined(_SERVER)
	KBaseObjLiteSet<KThreatNode>		m_ThreatNodeSet;
    KBaseObjLiteSet<KSKILL_BULLET>      m_BulletSet;
#endif	//_SERVER

};

extern KObjectManager g_ObjectManager;

// ---------------- 对象分配器 ------------------------------>
// 在服务端,地图的加载是异步进行的,所以,在主线程通知加载线程作加载的时候,需要做对象预分配(以避免对分配器加锁)
// 但是在客户端以及编辑器里面,预分配是不必要的,这个对象分配器的目的是为了统一客户端/服务端因对象分配方式不同而
// 带来的接口差异.
// 使用预分配的时候,注意检查一下预分配的对象个数最后是否恰好够用,多了或者少了都是不对的

struct KObjAlloctor
{
    virtual KNpc *NewServerNpc(DWORD dwId = ERROR_ID) { return NULL; };
    virtual KNpc *NewClientNpc(DWORD dwId = ERROR_ID) { return NULL; };

    virtual KDoodad *NewServerDoodad(DWORD dwId = ERROR_ID) { return NULL; }
    virtual KDoodad *NewClientDoodad(DWORD dwId = ERROR_ID) { return NULL; }

    virtual KCell *NewCell() { return NULL; };

    virtual void DeleteServerNpc(KNpc *pNpc) {};
    virtual void DeleteClientNpc(KNpc *pNpc) {};

    virtual void DeleteServerDoodad(KDoodad *pDoodad) {};
    virtual void DeleteClientDoodad(KDoodad *pDoodad) {};

    virtual void DeleteCell(KCell *pCell) {};
};

//class KServerObjAlloctor : public KObjAlloctor
//{
//public:
//    BOOL Init(int nNpcCount, int nDoodadCount, int nCellCount);        
//    void UnInit();
//
//public:
//
//    KNpc* NewServerNpc(DWORD dwId);
//
//    KDoodad *NewServerDoodad(DWORD dwId);
//
//    KCell *NewCell();
//
//    void DeleteServerNpc(KNpc *pNpc);
//
//    void DeleteServerDoodad(KDoodad *pDoodad);
//
//    void DeleteCell(KCell *pCell);
//
//private:
//    vector<KNpc *>      m_NpcVector;
//    vector<KDoodad *>   m_DoodadVector;
//    vector<KCell *>     m_CellVector;
//};

class KClientObjAlloctor : public KObjAlloctor
{
public:

//    KNpc *NewServerNpc(DWORD dwId) { return g_ObjectManager.m_NpcSet.New(dwId); };
 //   KNpc *NewClientNpc(DWORD dwId) { return g_ObjectManager.m_NpcSet.New(dwId); };

//    KDoodad *NewServerDoodad(DWORD dwId) { return g_ObjectManager.m_DoodadSet.New(dwId); };
//    KDoodad *NewClientDoodad(DWORD dwId) { return g_ObjectManager.m_DoodadSet.New(dwId); };

   KCell *NewCell() { return g_ObjectManager.m_CellSet.New(); };

    //void DeleteServerNpc(KNpc *pNpc) { g_ObjectManager.m_NpcSet.Delete(pNpc); };
    //void DeleteClientNpc(KNpc *pNpc) { g_ObjectManager.m_NpcSet.Delete(pNpc); };

//    void DeleteServerDoodad(KDoodad *pDoodad) { g_ObjectManager.m_DoodadSet.Delete(pDoodad); };
//    void DeleteClientDoodad(KDoodad *pDoodad) { g_ObjectManager.m_DoodadSet.Delete(pDoodad); };

    //void DeleteCell(KCell *pCell) { g_ObjectManager.m_CellSet.Delete(pCell); };
};


// <---------------- 对象分配器 ------------------------------

#endif	//_KOBJECT_MANAGER_H_