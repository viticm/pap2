////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneEntityMgr.h
//  Version     : 1.0
//  Creator     :
//  Create Date :
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENEENTITYMGR_H_
#define _INCLUDE_KG3DSCENEENTITYMGR_H_

////////////////////////////////////////////////////////////////////////////////


#define ENTITYCLASS_FAR        0
#define ENTITYCLASS_MIDDLE     1
#define ENTITYCLASS_NEAR       2
#define ENTITYCLASS_LAST       3

#include ".\kg3dsceneentity.h"
#include "KG3DMaterial.h"
#include "KG3DCamera.h"
#include "KG3DTemplates.h"
#include "KG3DSubSetRenderer.h"
#include "KG3DRepresentObjectPVS.h"
#include "KG3DSkinModelRender.h"

class KG3DFrustum;
class KG3DCamera;
class KG3DSceneLayer;
class KG3DSceneSceneEditor;
class KG3DSceneEntityMgr;
class KG3DStaticModelRender;
class KG3DScenePointLightRender;

enum eRenderDataState
{
	STATE_NONE,
	STATE_EMPTY,
	STATE_DATAOK,
	STATE_RENDERED,
};

#define MAX_QUERY_COUNT 256

class KG3DSkinModelRender;

class KG3DOcclusionCuller
{
public:
    KG3DOcclusionCuller();
    ~KG3DOcclusionCuller();

    HRESULT Init();
    HRESULT Unint();

    HRESULT OcclusionCulling();
    HRESULT CheckOccluded(
        std::vector<KG3DRepresentObject*>& rvecRepresentObjects,
        std::set<KG3DModel*>& rsetModels            
    );

    HRESULT QueryResult();
private:
    BOOL    _IsPointNearBBox(BBox& rBbox, D3DXVECTOR3& rPoint3D);
    HRESULT _FillBBox(DWORD dwIndex, BBox ObjectBBox);
    HRESULT _FillObjectBBox(KG3DRepresentObject* pObject);
    HRESULT _FillModelBBox(KG3DModel* pModel);
    HRESULT _RenderBBox(DWORD dwIndex);

    DWORD  m_dwQueryIndex;
    DWORD  m_dwQueryCounter;
    BOOL   m_bDataPrepared;

    struct KG3DQUERY_INFO
    {
        BOOL bOccluded;
        DWORD dwQueryCounter;
        DWORD dwQueryIndex;
    };

    typedef std::map<KG3DRepresentObject*, KG3DQUERY_INFO> REPRESENT_OBJECTS_QUERY_TABLE;
    typedef std::map<KG3DModel*, KG3DQUERY_INFO> MODELS_QUERY_TABLE;

    REPRESENT_OBJECTS_QUERY_TABLE m_RepresentObjectsInQuery;
    MODELS_QUERY_TABLE            m_ModelsInQuery;
    
    LPDIRECT3DQUERY9        m_pd3dOcclusionQuery[MAX_QUERY_COUNT];
    LPDIRECT3DVERTEXBUFFER9 m_lpBBoxVertexBuffer[MAX_QUERY_COUNT];
    LPDIRECT3DINDEXBUFFER9  m_lpBBoxIndexBuffer;
};


class KG3DWaterCuller
{
public:
    KG3DWaterCuller();
    ~KG3DWaterCuller();

    HRESULT Init();
    HRESULT Uninit();

    HRESULT BeginAddWaterBoundingPlanes();
    HRESULT AddWaterBoundingPlanes(const D3DXVECTOR3 &crvLeftBottomPos, const D3DXVECTOR3 &crvRightUpPos);
    HRESULT EndAddWaterBoundingPlanes();

    HRESULT RenderWaterBoundingPlanes();
    HRESULT QueryResult(BOOL *pbWaterVisible);

private:
    LPDIRECT3DQUERY9        m_pd3dOcclusionQuery;
    LPDIRECT3DVERTEXBUFFER9 m_lpVertexBuffer;
    LPDIRECT3DINDEXBUFFER9  m_lpVertexIndexBuffer;

    struct _RECTANGLE
    {
        D3DXVECTOR3 vLeftBottomPos;
        D3DXVECTOR3 vRightUpPos;
    };

    std::map<int, _RECTANGLE> m_mapWaterBoundingPlanes;

    enum
    {
        MAX_WATER_BOUNDING_PLANE_COUNT = 32
    };
};

struct KG3DCullData;

class KG3DRenderData
{
//#define  MAX_RENDERITEM_COUNT 8192
//
//	KG3DModelSpeedTree* m_aSpeedTree[MAX_RENDERITEM_COUNT];
//	int m_nNum_SpeedTree;
//
//	KG3DModel* m_aModelBilloarCloud[MAX_RENDERITEM_COUNT];
//	int        m_nNum_BilloarCloud;
//	
//	KG3DModel* m_aModel_SFX[MAX_RENDERITEM_COUNT];
//	int        m_nNum_Model_SFX;
//
//	KG3DRepresentObject* m_aObjectUnSort[MAX_RENDERITEM_COUNT];
//	int        m_nNum_ObjectUnSort;
//
//	KG3DModel* m_aModel_Skin[MAX_RENDERITEM_COUNT];
//	int        m_nNum_Model_Skin;
//
//	KG3DModel* m_aModel_Shadow[MAX_RENDERITEM_COUNT];
//	int        m_nNum_Model_Shadow;
public:
	eRenderDataState m_eState;

	typedef vector<KG3DModel*>	    ModelGroup;
	typedef std::vector<KG3DModel*> TypeVisibleModelList;
	struct ModelInfo
	{
		float fDistance;
		KG3DModel* pModel;

		bool operator<(ModelInfo const& rhs) const
		{
			return fDistance > rhs.fDistance;
		}
	};
	//typedef std::list<ModelInfo> TypeSortedModelCan;
	/*
	billboard cloud model group, sort by model id (file name hash)
	*/
	TKG3DResourcePool<KG3DSubMaterialGroup>m_cSubMaterialGroupPool;
	TKG3DResourcePool<KG3DModelSubsetInfo> m_cSubMaterialGroupSubInfoPool;

	std::map<DWORD, ModelGroup>         m_mapBillboardCloudGroup;	//<BillboardCloud>

	KG3DSubsetRender                    m_Renders[DEFST_COUNT];

    KG3DSkinShaderRender                m_SkinShaderRender;
	TypeVisibleModelList	            m_vecVisibleModelST;//暂时用于只计算脚底影子
	list<KG3DModel*>                    m_listModelForLimitSort;//客户端模型队列，用了做显示个数的限制
	std::map<DWORD, ModelGroup>         m_mapMaterialGroupSkinShaderNoCharactor;
	std::map<DWORD,ModelGroup>          m_mapMaterialGroupNormalNoSkin;

	std::vector<KG3DModel*>             m_vecVisiableSFX;   //特效类的物件
	std::vector<KG3DModel*>             m_vecPostRenderSFX; //屏幕扭曲类的特效

    std::map<DWORD,ModelGroup>          m_mapMaterialModelST;//聚合模型的集合
    std::vector<KG3DModel*>             m_vecSpecialAlphaModel;

	std::vector<KG3DRepresentObject*>   m_vecVisiableObjectsNotSort;
	std::vector<KG3DRepresentObject*>   m_vecVisableObjectSortByDistance;//需要根据距离远近排序的物件

    std::vector<KG3DRepresentObject*>   m_vecPolygon;
    std::vector<KG3DPvsPortal*>         m_vecPvsPortal;
    
    DWORD m_dwFaceCount;
	DWORD m_dwListObjectRendered;
	DWORD m_dwGroupedObjectCount;

    vector<KG3DModel*>                  m_vecModelRef;
	std::vector<KG3DModel *>            m_vecVisiableModelsRef;//放入了整理好的可见模型
	std::vector<KG3DRepresentObject*>   m_vecVisiableObjectsRef;

	//////////////////////////////////////////////////////////////////////////
	KG3DRenderData();
	~KG3DRenderData();

	HRESULT RemoveEntity(DWORD dwClass,KG3DSceneEntity Entity);

	HRESULT ShockWaveRender();
	//////////////////////////////////////////////////////////////////////////
	HRESULT SortMaterialForOneObject(KG3DRepresentObject* pObject,float fVisibleAngle,KG3DSceneEntityMgr* pMgr);
	HRESULT SortMaterialForOneModel(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr,KG3DModel* pModelWithChild,BOOL bUseNumberLimit);
	HRESULT SortMaterialForVisiableObjects(KG3DSceneEntityMgr* pMgr);

	HRESULT SortVisibleEntity(KG3DCullData* pCullData,KG3DSceneEntityMgr* pMgr);

	HRESULT UnInit();

	HRESULT RenderVisibleEntitySFX(KG3DSceneSceneEditor* pEditor,KG3DCullData* pCullData);
	HRESULT RenderProfile();

	HRESULT RenderVisibleObjectUnSort();
	HRESULT RenderVisibleModelNormalNoSkin();
	HRESULT RenderVisibleModelSkinForDepth();

	HRESULT RenderVisibleModelSkinShader(KG3DTerrainConverMap* pConvermap,BOOL bJustNear);
	HRESULT RenderVisibleBillboardCloud();//<Billboard>
	HRESULT RenderVisibleObjectSortByDistance();
	HRESULT RenderPoly();

	HRESULT RenderBilloarCloudDepth();
	VOID    RenderModelFeetShadow();
	HRESULT RenderVisibleObjectsZoomOut();
	HRESULT RenderPvsPortal();
	HRESULT RenderModelSortedReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);
	HRESULT Clear();
	HRESULT BeforeRenderProcess();
    HRESULT RenderSpecialAlphaModel();
private:
	HRESULT RenderVisibleModelSkinShaderUnion(KG3DTerrainConverMap* pConverMap);

	HRESULT _ProcessMaterialShaderTypeNone(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr);
	HRESULT _ProcessMaterialShaderTypeSkin(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr);
	HRESULT _ProcessMaterialShaderTypeNormalNoSkin(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr);

	HRESULT _ProcessMeshTypeDefault(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr);
	HRESULT _ProcessMeshTypeSpeedTree(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr);
	HRESULT _ProcessMeshTypeModelst(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr);
	HRESULT SortModelsByDistance(float X,float Z,KG3DSceneEntityMgr* pMgr);

	HRESULT ClearSFX();
	HRESULT ClearModelRef();

};

struct KG3DCullData
{
public:
    HRESULT Init(KG3DOcclusionCuller *pOcclusioCuller);
    HRESULT Uninit();
    HRESULT OnLostDevice();
    HRESULT OnResetDevice();
    HRESULT OnObjectVisible(KG3DRepresentObject* pObject);
	HRESULT OnObjectUnVisible(KG3DRepresentObject* pObject);
	HRESULT CheckUnVisibleEntity();

	HRESULT ClearFrustum();
	HRESULT Clear();

	HRESULT FrameMoveVisibleEntity();

	HRESULT RemoveEntity(DWORD dwClass,KG3DSceneEntity Entity);

	HRESULT KG3DCullData::GetEntityFromVisibleContainers(KG3DSceneEntityMgr* pEntityMgr, 
		const vector<KG3DFrustum>& vFrustum, 
		KG3DSceneSceneEditor *pEditor,const D3DXVECTOR3& vFocusPos);

	HRESULT GetVisibleContainersByFrustum(KG3DSceneEntityMgr* pEntityMgr,
		const vector<KG3DFrustum>& vFrustum, 
		KG3DSceneSceneEditor *pEditor,const D3DXVECTOR3& vFocusPos);//主裁剪函数

	HRESULT GetVisibleEntityByCamera(KG3DSceneEntityMgr* pEntityMgr,
		KG3DCamera *pCamera,
		KG3DSceneSceneEditor *pEditor,D3DXVECTOR3& vFocusPos);
	
	//////////////////////////////////////////////////////////////////////////
	HRESULT ProcessMultiThreadCull();

	KG3DCullData();
	~KG3DCullData();

	VOID	EnableCheckVisibleCount(BOOL bEnable){m_bCheckVisibleCount = bEnable;}
	BOOL	IsCheckVisibleCountEnable(){return m_bCheckVisibleCount;}
	HRESULT ClearVisibleObjectSet();

	HRESULT ExportToRenderData(KG3DRenderData* pRenderData,KG3DSceneEntityMgr* pMgr);
public:

	//////////////////////////////////////////////////////////////////////////
	KG3DFrustum* m_pFrustum;

	DWORD        m_dwRenderCount;

	std::vector<KG3DRepresentObject*> m_vecVisiableObjectsRef;

    std::vector<KG3DPvsPortal*>    m_vecPvsPortal;

	std::set<KG3DModel *>   m_setVisiableModels;    //放入了整理好的可见模型

	vector<POINT>           m_vecSceneRegionsIndex;
	vector<POINT>           m_vecSceneZonesIndex;   //可见的地形ZONE编号
	vector<POINT>           m_vecSceneSectionsIndex;

	vector<KG3DSceneEntityContainer *> m_vecEntityContainer;

	DWORD      m_dwLastUseTime;

	//////////////////////////////////////////////////////////////////////////
	
    KG3DSceneEntityMgr* m_lpEntityMgrSave;
		
	D3DXVECTOR3 m_vFocusPos;

    float m_fNearbyObjectVisibleAngleGate;
	float m_fVisibleAngleGate;
	float m_fLODMiddleToLowSwitchAngleGate;//弧度<Billboard>用来决定要不要用Billboard渲染
private:

	HRESULT GetEntityFromContainersDirect();

    HRESULT _GetBlockIndexFromContainers(
        vector<POINT> *pRetZoneIndex, 
        vector<POINT> *pRetSectionIndex, 
        vector<POINT> *pRetRegionIndex, 
        vector<KG3DSceneEntityContainer *> *pvecContainer
    );

    HRESULT GetVisibleSceneRegions(KG3DSceneEntityMgr* pEntityMgr,
		const vector<KG3DFrustum>& vFrustum,
		KG3DSceneSceneEditor *pEditor,const D3DXVECTOR3& vFocusPos);

	HRESULT GetVisibleSceneRegionsPosition(KG3DSceneEntityMgr* pEntityMgr,
		KG3DCamera *pCamera,
		KG3DSceneSceneEditor *pEditor, D3DXVECTOR3& vFocusPos);

    HRESULT _GetEntityShellFromContainers(KG3DSceneEntityContainer* pCon,const D3DXVECTOR3& vPos, KG3DScene* pScene, const vector<KG3DFrustum>& vFrustum);
    HRESULT _GetModelFromContainers(KG3DSceneEntityContainer* pCon, const vector<KG3DFrustum>& vFrustum);
    HRESULT _GetEntityFromContainers(const D3DXVECTOR3& vPos, KG3DScene* pScene, const vector<KG3DFrustum>& vFrustum);

private:
    DWORD m_bCheckVisibleCount;
    KG3DOcclusionCuller* m_pOcclusionCuller;
};

class KG3DSceneOutputWnd;

class KG3DSceneEntityMgr
{
private:
protected:
	bool	m_bHasInited;
	float   m_fCameraFar;

public:
	BOOL  m_bUseMultiThread;
	BOOL  m_bKickMeOutFromMultiThreadCull;//请求将自己从多线程队列中踢出
	BOOL  m_bGetEntityFromContainerDirect;

	BOOL m_bNeedReGetVisibleEntity;//是否需要刷新物件列表

    HRESULT ClearFrustum();
	FLOAT	GetCameraFar(){return m_fCameraFar;}
	INT		GetCullLevelDepth(){return m_nCullLevelDepth;}
	void    SetCullLevelDepth(int nDepth){m_nCullLevelDepth = nDepth;};

	KG3DCullData& GetCullData(size_t i){return m_CullData;}
	VOID EnableCheckVisibleCount(BOOL bEnable);

    virtual int GetModelOverlapBox(KG3DModel* pModels[], int maxNum, D3DXVECTOR3 box[8]);

    void GetVisable(DWORD& dwZoom, DWORD& dwSection, DWORD& dwRegion,DWORD& dwGrass);
    DWORD GetVisableObjectNum();
	DWORD GetVisibleClientModels();

	HRESULT ShockWaveRender();
	
	KG3DCullData m_CullData;

	KG3DRenderData m_RenderData[2];
	
	KG3DRenderData* m_lpCurrentRenderData;

	int m_nCullLevelDepth;//裁剪的深度，0-ZONE,1-SECTION,2-REGION
public:
	KG3DSkinModelRender*   m_lpSKinModelRender  ;//带骨骼动画的模型绘制器
	KG3DStaticModelRender* m_lpStaticModelRender;//静态的模型绘制器
	KG3DScenePointLightRender* m_lpPointLightRender;//点光源的处理函数集合
	KG3DModelSpeedTreeRender*  m_lpSpeedTreeRender;//批量渲染speedtree的工具

	KG3DTerrainRender* m_lpTerrainRender;
	KG3DTerrainRender* GetTerrainRender(){	return m_lpTerrainRender;};
protected:
	HRESULT ProcessPointLightsForTerrain();

	HRESULT CheckCurrentRenderData();
public:

	HRESULT RenderForTranslucence();

	HRESULT AutoReloadModifiedResource();

	virtual HRESULT FrameMove(KG3DSceneSceneEditor* pEditor,BOOL bUpdateSkin);

    //HRESULT RenderWaterRefractWave(KG3DSceneSceneEditor* pEditor);
    HRESULT RenderWaterRefract(KG3DSceneSceneEditor* pEditor);
	HRESULT RenderWaterAndSFX(KG3DSceneSceneEditor* pEditor);

	virtual HRESULT GetSelectableEntityList(/*list<KG3DSceneEntity>*plistEntity, */KG3DScene& scene, BOOL bObject,BOOL bModel);

	HRESULT SetVisibleGate(float fVisible,float fBillBoardChange);
	HRESULT RenderForDepthOnly(KG3DSceneSceneEditor* pEditor,BOOL bTerrain,BOOL bTree,BOOL bObject);

	HRESULT RenderVisibleRegionBox(KG3DSceneSceneEditor* pEditor);
	HRESULT RenderVisibleZoneBox(KG3DSceneSceneEditor* pEditor,BOOL bDetail);
	HRESULT RenderVisibleSectionBox(KG3DSceneSceneEditor* pEditor);

	HRESULT ProcessMultiThreadCull();

	virtual HRESULT OnRenderEnd();
	KG3DCullData* GetUnReadyCullData();
	HRESULT RemoveRenderModel(KG3DModel* pModel);

	vector<POINT>* GetSceneRegionsIndexVector();
	vector<POINT>* GetSceneSectionsIndexVector();

	//HRESULT ProcessSoundBall(KG3DCamera* pCamera);


	HRESULT FrameMoveVisibleEntity();

    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice();

	HRESULT Clear();

	HRESULT Init(BOOL bUseOcclusionCull);
	HRESULT UnInit();

	HRESULT RenderProfile();

	HRESULT SetVisibleEntity(vector<KG3DModel*>* pvecModel);
	HRESULT GetAllModels(vector<IEKG3DModel*>* pvecModels);

    virtual HRESULT GetVisibleEntity(
        const vector<KG3DFrustum>& vFrustum, 
        KG3DSceneSceneEditor*      pEditor,
        const D3DXVECTOR3& vFocusPos
    );//主裁剪函数
    
    virtual HRESULT GetVisibleEntityEx(
		KG3DCamera *pCamera,
		KG3DSceneSceneEditor *pEditor,
		D3DXVECTOR3& vFocusPos);//ShadowMap用

	HRESULT RenderVisibleTerrainBlocks(KG3DSceneSceneEditor* pEditor,BOOL bGrass,BOOL bStone,BOOL bFrutex);
	HRESULT RenderForColorOnly(KG3DSceneSceneEditor* pEditor, KG3DSceneOutputWnd* pWnd);
    HRESULT RenderSpecialAlphaModel();

	HRESULT RemoveEntity(DWORD dwClass,KG3DSceneEntity Entity);

	HRESULT PrintfProfile();

	HRESULT RenderTerrain_Texture();
public:////add by huangjinshou 2008-1-10

	HRESULT RenderReflection(KG3DSceneSceneEditor* pEditor,float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);

	/////////////////////////////////////////////
	HRESULT RenderLogicalCells(BOOL bTerrian, BOOL bItem, BOOL bSelected,IELogicalSceneConnection* pLogicalScene,KG3DScene* pScene);
	HRESULT RenderVisibleObjectBBox();
   
	KG3DSceneEntityMgr(void);
	virtual ~KG3DSceneEntityMgr(void);

    BOOL IsWaterVisible() {return m_bWaterVisible;}
private:
    HRESULT _InitOcclusionCull();
    HRESULT _UnInitOcclusionCull();

    KG3DOcclusionCuller *m_pOcclusionCuller;
    KG3DWaterCuller *m_pWaterCuller;
    BOOL m_bWaterVisible;
    BOOL m_bUseOcclusionCull;
    DWORD m_dwCullIntervalFrame; //裁剪帧间隔，做跳帧用的

    D3DXVECTOR3 m_vCameraPosLast;
    D3DXVECTOR3 m_vCameraDirLast;
};

class KG3DSceneEntityMgrShadow : public KG3DSceneEntityMgr
{
public:
	HRESULT Init(KG3DSceneEntityMgr* pBaseEntityMgr);

	using KG3DSceneEntityMgr::GetVisibleEntity;

	HRESULT RenderForShadowMap(KG3DSceneSceneEditor* pEditor,BOOL bTerrain,BOOL bTree,BOOL bObject,BOOL bSkinModel);

};

extern DWORD g_dwCullCounter;

void g_MultiThreadCullProgress(void * pInput);

#endif //_INCLUDE_KG3DSCENEENTITYMGR_H_
