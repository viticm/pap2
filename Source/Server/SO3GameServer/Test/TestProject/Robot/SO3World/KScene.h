/************************************************************************/
/* 地图类								                                */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.31	Create												*/
/* Comment	 :															*/
/*		封装了一张游戏世界中地图的所有操作和数据						*/
/************************************************************************/
#ifndef _KSCENE_H_
#define	_KSCENE_H_

#include <list>
#include <map>
#include <string>
#include "Engine/KList.h"
#include "Global.h"
#include "KBaseObject.h"
#include "KCell.h"
#include "Luna.h"
#include "Engine/KThread.h"

class KRegion;
class KScene;
class KNpc;
class KDoodad;
class KObjAlloctor;
class KServerObjAlloctor;
struct KREVIVE_POSITION;

enum SCENE_STATE
{
	ssInvalid = 0,			//无效状态
	ssLoading,				//正在载入中
	ssLoaded,				//载入刚完成
	ssLoadFailed,			//载入失败
	ssNormal,				//正常状态
	ssDeleting,				//安全删除中
	ssDeleted,				//已删除

	ssTotal
};


enum RESIZE_SCENE_ALIGN_TYPE
{
	rsatInvalid = 0,

	rsatLeftTop,
	rastLeftBottom,
	rastRightTop,
	rastRightBottom,

	rastTotal
};

class KScene : public KBaseObject
{
//外部接口函数
public:
	// 设定地表格子信息
	BOOL SetCellData(int nXCell, int nYCell, KCell::KBaseInfo* pBaseInfo, WORD wLowLayer, WORD wHighLayer);
	// 获取地表格子信息
	BOOL GetCellData(int nXCell, int nYCell, KCell::KBaseInfo& BaseInfo, WORD& wLowLayer, WORD& wHighLayer);


	BOOL NewObstacle(int nXCell, int nYCell, KCell::KBaseInfo* pBaseInfo, WORD wLowLayer, WORD wHighLayer);
	BOOL DelObstacle(int nXCell, int nYCell, int nZ);
	BOOL GetObstacle(int nXCell, int nYCell, int nZ, KCell::KBaseInfo& BaseInfo, WORD& wLowLayer, WORD& wHighLayer);
	BOOL CheckObstacle(int nXCell, int nYCell, int nLayer);

	DWORD GetScriptID(int nXCell, int nYCell);
//	BOOL SetScriptID(int nXCell, int nYCell, DWORD dwScriptID);


	// 预处理障碍数据
	typedef void (*PREPROCESS_PROGRESS_CALLBACK_FUNC)(int nYCell, int nCellWidth, int nCellHeight);
    BOOL PreprocessCalculateData();

private:
    BOOL ReviseCalculateGradient();
	BOOL CalculateObstacleRange();

public:
    // 可见性检测,障碍检测,检测两点之间是否可见
    // X, Y坐标为"点", Z坐标为Altitude,如果检测到障碍则返回false,否则返回true
    BOOL IsVisible(int nSrcX, int nSrcY, int nSrcZ, int nDstX, int nDstY, int nDstZ);
    
    BOOL IsReachable(int nSrcX, int nSrcY, int nSrcZ, int nDstX, int nDstY, int nDstZ, int nHeight);


public:
	DWORD							m_dwMapID;						//地图编号
	DWORD							m_dwMapCopyIndex;				//副本序号
	SCENE_STATE						m_eSceneState;					//地图状态
	DWORD							m_dwCreatorID;					//地图副本的创建者的ID
	char							m_szName[_NAME_LEN];			//地图名称
    BOOL                            m_bReviveInSitu;                //玩家是否能在原地复活
	DWORD							m_dwScriptID;					//地图脚本ID


public:
	KScene();
	virtual ~KScene();

	BOOL Init(void);
    void UnInit();

    BOOL PreLoadScene();
    BOOL ClientLoadRegion(int nRegionX, int nRegionY);

	char m_szFilePath[MAX_PATH];



	BOOL SetRegionWidth(int nWidth);
	BOOL SetRegionHeight(int nHeight);
	int GetRegionWidth(void);
	int GetRegionHeight(void);

	BOOL SetRegion(int nRegionX, int nRegionY, KRegion* pRegion);
	KRegion* GetRegion(int nRegionX, int nRegionY);

	// 由象素点坐标得到Region指针
	KRegion* GetRegionByPoint(int nX, int nY);
	BOOL DelRegion(int nRegionX, int nRegionY);
	BOOL DeleteAllRegion(void);
	BOOL DeleteAllObstacle(void);

	//NPC相关函数
	BOOL AddNpc(KNpc* pNpc);
	BOOL RemoveNpc(KNpc* pNpc);
    BOOL MoveNpc(KNpc* pNpc, int nX, int nY, int nZ);

	// 场景装饰物相关函数
	BOOL AddDoodad(KDoodad* pDoodad);
	BOOL RemoveDoodad(KDoodad* pDoodad);
	BOOL MoveDoodad(KDoodad* pDoodad, int nX, int nY, int nZ = 0);

	//玩家相关函数
	BOOL AddPlayer(KPlayer* pPlayer);
	BOOL RemovePlayer(KPlayer* pPlayer);


//私有数据
private:
	KRegion*						m_RegionList[MAX_REGION_WIDTH][MAX_REGION_HEIGHT];
	int							    m_nRegionWidth;		//Region在X方向上的个数
	int  							m_nRegionHeight;	//Region在Y方向上的个数

    KMutex m_Lock;

	friend class KSO3World;
	friend class KCharacter;

public:
	// 脚本接口函数
	DECLARE_LUA_CLASS(KScene);

	DECLARE_LUA_STRING(Name, _NAME_LEN);
	DECLARE_LUA_DWORD(MapID);
	DECLARE_LUA_DWORD(ID);
	DECLARE_LUA_DWORD(MapCopyIndex);
	DECLARE_LUA_ENUM(SceneState);
    DECLARE_LUA_BOOL(ReviveInSitu);
};

//得到全局象素点坐标所在的Region指针
inline KRegion* KScene::GetRegionByPoint(int nX, int nY)
{
	KRegion*    pRegion     = NULL;
	int         nRegionX    = nX / REGION_GRID_WIDTH / CELL_LENGTH;
	int         nRegionY    = nY / REGION_GRID_HEIGHT / CELL_LENGTH;

	KG_PROCESS_ERROR(nRegionX >= 0 && nRegionX < m_nRegionWidth);
	KG_PROCESS_ERROR(nRegionY >= 0 && nRegionY < m_nRegionHeight);

	pRegion = m_RegionList[nRegionX][nRegionY];

Exit0:
	return pRegion;
}


#endif