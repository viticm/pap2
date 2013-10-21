#pragma once
#include "KG_InterlockSingleList.h"

#include "KG3DTempleteMultiThead.h"
//////////////////////////////////////////////////////////////////////////
//场景区块对象加载器
//
//////////////////////////////////////////////////////////////////////////
class KG3DSceneZone;
class KG3DSceneSection;
class KG3DSceneRegion;
class KG3DTerrainBlock;
class KG3DSceneOutDoorSpaceMgr;
class KG3DSceneSceneEditor;
struct KG3DEntityObjectShell;

class KG3DSceneBlockLoader
{
protected:
	KG3DSceneOutDoorSpaceMgr& m_OutDoor;

	DWORD m_dwEntityLoadCount;//加载区块的个数
	DWORD m_dwLoadLimit;
public:
	BOOL  m_bQuitLoad;//是否让多线程加载尽快退出

	D3DXVECTOR2 m_DynamicLoadPositionLast;
	D3DXVECTOR2 m_DynamicLoadPositionNow;
	D3DXVECTOR2 m_DynamicLoadDirection;
	float m_fRZone;
	float m_fRSection;
	float m_fRRegion;

	//////////////////////////////////////////////////////////////////////////
	struct LoadList
	{
		list<KG3DSceneZone*   >m_listSceneObjectZoneToLoad;
		list<KG3DSceneSection*>m_listSceneObjectSectionToLoad;
		list<KG3DSceneRegion* >m_listSceneObjectRegionToLoad;

		list<KG3DSceneZone*   >m_listTerrainZoneToLoad;
		list<KG3DSceneSection*>m_listTerrainSectionToLoad;
		list<KG3DSceneRegion*> m_listTerrainRegionToLoad;
		set<KG3DEntityObjectShell*>m_setObjectShellToLoad;

		HRESULT CleanUp();

		BOOL IsEmpty();

		HRESULT Sort(D3DXVECTOR2& vCameraPos,D3DXVECTOR2& vDirection);
	};

	LoadList m_LoadList;
	LoadList m_LoadListMultiThread;
	BOOL m_bNeedRefreshLoadList;//是否需要刷新多线程加载队列
	//////////////////////////////////////////////////////////////////////////
	HRESULT CleanUp();

	HRESULT SetLoadRect(D3DXVECTOR2& A_Zone,D3DXVECTOR2& C_Zone,
						D3DXVECTOR2& A_Section,D3DXVECTOR2& C_Section,
						D3DXVECTOR2& A_Region,D3DXVECTOR2& C_Region,BOOL bMultThread);
	DWORD GetLoadRect(D3DXVECTOR2 vPos,D3DXVECTOR2& vSectionA,D3DXVECTOR2& vSectionC,D3DXVECTOR2& vRegionA,D3DXVECTOR2& vRegionC);

	HRESULT DynamicLoadTerrainZone   (KG3DSceneZone*    pZone   ,BOOL bMultiThread);
	HRESULT DynamicLoadTerrainSection(KG3DSceneSection* pSection,BOOL bMultiThread);
	HRESULT DynamicLoadTerrainRegion (KG3DSceneRegion*  pRegion ,BOOL bMultiThread);
	HRESULT DynamicLoadObjectZone    (KG3DSceneZone*    pZone   ,BOOL bMultiThread);
	HRESULT DynamicLoadObjectSection (KG3DSceneSection* pSection,BOOL bMultiThread);
	HRESULT DynamicLoadObjectRegion  (KG3DSceneRegion*  pRegion ,BOOL bMultiThread);

	KG3DSceneBlockLoader(KG3DSceneOutDoorSpaceMgr& OutDoor);
	~KG3DSceneBlockLoader();

	HRESULT ForceLoad();

	HRESULT FrameMove();

	HRESULT OnMultiThreadEnd();

	HRESULT CheckMemoryUse(KG3DSceneSceneEditor* pEditor);

	HRESULT LoadTerrainBlockByPos(float fx,float fz);

	HRESULT MultiThreadLoad();

protected:

	//HRESULT TryPushLoadList(BOOL bMultThread);

	HRESULT ProcessDynamicLoadTerrain();
	HRESULT ProcessDynamicLoadObject();

	HRESULT ComputeLoadZoneByRect(LoadList* pList,D3DXVECTOR2& A_Zone,D3DXVECTOR2& C_Zone,
								  D3DXVECTOR2& A_Section,D3DXVECTOR2& C_Section,
								  D3DXVECTOR2& A_Region,D3DXVECTOR2& C_Region);
	HRESULT ComputeLoadSectionByRect(LoadList* pList,D3DXVECTOR2& A_Section,D3DXVECTOR2& C_Section,
									 D3DXVECTOR2& A_Region,D3DXVECTOR2& C_Region,
									 KG3DSceneZone* pZone);
	HRESULT ComputeLoadRegionByRect(LoadList* pList,D3DXVECTOR2& A,D3DXVECTOR2& C,KG3DSceneSection* pSection);


};


struct SceneBlockList
{
	KG3DSceneBlockLoader* m_lpSceneBlockLoader;

	list<KG3DSceneRegion* >m_listTerrainRegion;
	list<KG3DSceneSection*>m_listTerrainSection;
	list<KG3DSceneZone*   >m_listTerrainZone;
	list<KG3DSceneRegion* >m_listSceneRegion;
	list<KG3DSceneSection*>m_listSceneSection;
	list<KG3DSceneZone*   >m_listSceneZone;
	vector<KG3DEntityObjectShell*>m_vecObjectShell;
	DWORD m_dwState;
	SceneBlockList()
	{
		m_lpSceneBlockLoader = NULL;
		m_dwState = BLOCK_LIST_STATE_NODATA;
	}
	~SceneBlockList()
	{
		m_lpSceneBlockLoader = NULL;
		m_dwState = BLOCK_LIST_STATE_NODATA;
		m_listTerrainRegion.clear();
		m_listTerrainSection.clear();
		m_listTerrainZone.clear();
		m_listSceneRegion.clear();
		m_listSceneSection.clear();
		m_listSceneZone.clear();
		m_vecObjectShell.clear();
	}
};
