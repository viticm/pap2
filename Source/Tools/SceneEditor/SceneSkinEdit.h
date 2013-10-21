// SceneSkinEdit.h: interface for the KSceneSceneEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENESKINEDIT_H__16CCC5FF_63EA_4D45_ADD6_C238124856B5__INCLUDED_)
#define AFX_SCENESKINEDIT_H__16CCC5FF_63EA_4D45_ADD6_C238124856B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Scene.h"
#include "SceneEditBase.h"
#include "KSceneSceneEditorPanelDialog.h"
#include "KSceneSceneEditorDialogLargeTerrian.h"

#include "KSceneSettingDialog.h"
#include "meshskybox.h"

#include "KSO3World.h"
#include <map>

class KSceneSceneEditorDialogGroundTextureMgr;
class KSceneSceneEditorDialogObjectMgr;
class KModelSceneSFX;

class KSceneSceneEdit : public KSceneEditBase  
{
public:

#pragma pack(push,1)
	struct _TGAHeader 
	{
		unsigned char   Id ;
		unsigned char   ColourMapType ;
		unsigned char   ImageType ;
		unsigned short  ColourMapOrigin ;
		unsigned short  ColourMapLength ;
		unsigned char   ColourMapDepth ;
		unsigned short  x_origin ;
		unsigned short  y_origin ;
		unsigned short  width ;
		unsigned short  height ;
		unsigned char   bitsperpixel ;
		unsigned char   imagedescriptor ;
	};
#pragma pack(pop)

	KScene* m_pScene;	//对应的游戏逻辑中的场景类指针

	BOOL        m_bEnablePlaceArray;
	BOOL        m_bEnableRandomRotation;
	BOOL        m_bEnableRandomScale;
	float       m_fRotationDN;
	float       m_fRotationUP;
	float       m_fScaleDN;
	float       m_fScaleUP;
	float       m_fPlaceArrayDest;


	LPTEXTURE   m_lpTextureSky;

	KSceneSettingDialog*                 m_lpDialogSceneSetting;//场景设置对话框
	KSceneSceneEditorPanelDialog*        m_lpDialogPanel;       //场景工具面板
	KSceneSceneEditorDialogLargeTerrian* m_lpDialogLargeTerrian;//大尺寸地形编辑对话框
	KSceneSceneEditorDialogGroundTextureMgr* m_lpDialogGroundTextureMgr;//地表贴图管理器对话框
	KSceneSceneEditorDialogObjectMgr*        m_lpDialogObjectMgr;


	int  m_nDestMapWidth;//用于调整地图大小
	int  m_nDestMapHeigth;
	BOOL m_bNeedReSizeMap;

	struct _SceneHead
	{
		DWORD  FileMask;
		DWORD  BlockLength;

		DWORD  MapsBlock;
		DWORD  ObjectBlock;
		DWORD  Extend[256];

		_SceneHead()
		{
			FileMask = 0xFF00FF00;
			BlockLength = 0;
			MapsBlock = 0;
			ObjectBlock = 0;
			ZeroMemory(Extend,sizeof(DWORD)*256);
		}
	};

	struct _SceneOperatorPlaceObject : public _SceneOperatorBase
	{
		HRESULT Undo(KSceneEditBase* pScene);
		HRESULT AddPlacedObject(LPOBJECT pObject);

		_SceneOperatorPlaceObject();
		virtual ~_SceneOperatorPlaceObject();
	};
	ModelSFX::KModelSFXSkyBox* m_lpSkyBox;

	KModelSceneSFX *m_lpSSFX;

	KModelTerrain* m_lpMapMesh;
	//KModelTerrainLOD*  m_lpTerrainLOD;

	LPMODEL m_lpLightFlare;

	//ModelTools::KModelToolTerrain*        m_lpMainTool;
	//ModelTools::KModelToolObjectTable* m_lpObjectTool;

	//////////////////////////////////////////////////////////////////////////
	//放置物件
	KComponentObject*   m_lpInHandObject;
	vector<string>      m_vecInHandObjectName;
	int                 m_nCurInHandObjectIndex;
	//////////////////////////////////////////////////////////////////////////
	//跑马模式
	KRepGirlNo1*        m_lpGirlNo1;
	
	
	//////////////////////////////////////////////////////////////////////////
	//水面，光影
	//Water::KModelWater*        m_lpWater;

	//////////////////////////////////////////////////////////////////////////
	//子弹相关
	class MissileMgr
	{
	public:
		typedef struct _MissileInfo
		{
			DWORD    dwRepMissileID;
			KMissile* lpLogicMissile; 
		}*LPMISSILEINFO,MISSILEINFO;

		list<MISSILEINFO>m_listMissileInfo;

	public:
		MissileMgr();
		virtual ~MissileMgr();
	};

	//////////////////////////////////////////////////////////////////////////
	//物件分组
	class ObjectGroupLeaf
	{
	public:
		DWORD   m_dwObjectIndex;
		TCHAR   m_szName[256];
		vector<LPOBJECT>m_vecObject;
	public:
		ObjectGroupLeaf();
		virtual ~ObjectGroupLeaf();

		HRESULT Split2SubGroupByMeshRef();//根据物件使用模型的情况切分
	};

	class ObjectGroupTree
	{
	public:
		TCHAR   m_szName[256];
		list<ObjectGroupTree*>m_listSubGroupTree;
		list<ObjectGroupLeaf*>m_listGroupLeaf;
		ObjectGroupTree*  m_lpParentTree;
	public:
		ObjectGroupTree();
		virtual ~ObjectGroupTree();
	};

	ObjectGroupTree m_EntityGroupTree;
	//ObjectGroupLeaf m_GroupSingleMesh;
	vector<ObjectGroupLeaf*>m_vecEntityGroupLeaf;
	vector<ObjectGroupLeaf*>m_vecEntityGroupLeafSelected;
	//////////////////////////////////////////////////////////////////////////
	//摄象机信息
	D3DXVECTOR3 m_CameraPosition;
	D3DXVECTOR3 m_CameraLookAt;
	D3DXVECTOR3 m_CameraAngle;
	float       m_CameraDistance;

	list<D3DXVECTOR3>m_listMoveDestPos;

	BOOL   m_bLOD;
	
	DWORD  m_dwTestTimeStart;
	BOOL   m_bTesting;
	int    m_nFrameCount;
	D3DXVECTOR3 m_Center;

	vector<KSceneEntityGroup>m_vecEntityGroup;

	HWND    m_hWndSmallMap;
	HWND    m_hWndObjectPreView;

	BOOL    m_bNeedRefreshSmallMap;
	DWORD   m_dwSmallMapOption;
	float   m_fSmallMapScaling;
	D3DXVECTOR3   m_SmallMapCenter;
	BOOL    m_bOptimizeSmallEntity;

	float   m_fSunHeight;

	DWORD   m_dwWaterReflectionMapID;//水面反射贴图序列号

	//DWORD   m_dGUIID;
	DWORD   m_dwShowObscale;
public:
	HRESULT PT_Scene2Logical(int* pX,int* pY,int* pZ,D3DXVECTOR3 Pos);
	HRESULT PT_Logical2Scene(D3DXVECTOR3* pPos,int X,int Y,int Z);

	HRESULT  GetEntityGroupIndex(DWORD* pIndex,KSceneEntity Entity);//获取物件对应的物件组的序号.
	HRESULT  GroupObjects();//物件分组
	HRESULT  GroupObjectsSelected();

	HRESULT  RestoreDeviceObjects(void);
	HRESULT  InvalidateDeviceObjects(void);

	HRESULT WaterMapCreate();
	HRESULT WaterMapRelease();
	HRESULT RenderWaterReflection();//计算水面所需要的反射贴图

	HRESULT ToggleEditing();//切换编辑/测试模式
	HRESULT ShowDialogSceneSetting(BOOL bShow);//显示或者关闭场景设置对话框
	HRESULT ShowDialogLargeTerrian(BOOL bShow);//显示或者关闭大尺寸地形编辑器
	HRESULT ShowDialogGroundTextureMgr(BOOL bShow);//显示或者关闭地表贴图管理器
	HRESULT ShowDialogObjectMgr(BOOL bShow);//显示或者关闭物件管理器

	HRESULT OutputSmallMap(LPSTR pFileName);
	HRESULT MoveCamera2SmallMapPos(float X,float Y);

	BOOL    IsAbleToUnGroup();
	HRESULT RenderSmallMap();
	HRESULT RenderObjectPreview();
	HRESULT Render4LargeTerrianDialog(HWND hOutputWnd);

	HRESULT Edit_GroupSelectedEntity();
	HRESULT Edit_UnGroupSelectedEntity();

	HRESULT OnNewScence();
	HRESULT SetInHandObjectFileDirectory(LPCSTR pDir,LPCSTR pCurFileName);//设置放置物件替换的目录
	HRESULT ScrollInHandObject(BOOL m_bUp);

	HRESULT CheckUpMapSize(int& Size);

	void PutEntitytoRegion();
	void PutEntitytoTerrianRegion(KSceneEntity& Entity,BOOL bOptimize);
	
	HRESULT FrameMove();
	HRESULT ComputeCamera();

	HRESULT Edit_PlaceObject();
	HRESULT Edit_Undo();
	HRESULT Edit_Copy();
	HRESULT Edit_Paste();
	HRESULT Edit_Cut();
	HRESULT Edit_Select();

	HRESULT Edit_DropSelectedObject();
	
	HRESULT LoadObjects(FILE *pFile);
	HRESULT SaveObjects(FILE* pFile);

	void ONKEYDOWN    (WPARAM wParam, LPARAM lParam);
	void ONLBUTTONDOWN(WPARAM wParam, LPARAM lParam);
	void ONLBUTTONUP  (WPARAM wParam, LPARAM lParam);
	void ONRBUTTONDOWN(WPARAM wParam, LPARAM lParam);
	void ONMOUSEWHEEL (WPARAM wParam, LPARAM lParam);

	HRESULT Initialize(PVOID pSceneTable);
	HRESULT Render();
	DWORD  m_CurObjectID;
	DWORD  m_CurXMeshID;//当前操作的XMESH，是在当前OBJECT中的位置
	POINT m_LastMousePos;
	HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
	KSceneSceneEdit();
	virtual ~KSceneSceneEdit();

private:
	DWORD	m_dwLogicalMapID;

	BOOL SetEditorData2LogicalData(void);

	void ONMOUSEMOVE(WPARAM wParam, LPARAM lParam);
public:

	HRESULT RenderScene();

	HRESULT Acquire();
	HRESULT Edit_EditMesh(void);
	HRESULT SaveScene(LPSTR pFileName);
	HRESULT LoadScene(LPSTR pFileName);

	virtual HRESULT  GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR2* pIN,DWORD Type);

protected:
	
public:
	HRESULT OnShowSelectedEntityProperties(void);
};

#endif // !defined(AFX_SCENESKINEDIT_H__16CCC5FF_63EA_4D45_ADD6_C238124856B5__INCLUDED_)
