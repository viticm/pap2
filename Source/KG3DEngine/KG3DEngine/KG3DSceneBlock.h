#pragma once
#include "KG3DTemplates.h"
#include "KG3DFrustum.h"
#include "KG3DSceneEntity.h"

enum
{
	EM_SCENE_REGION_WIDTH = 1600,

	EM_SCENE_SECTION_REGION_PER_EDGE = 8,
	EM_SCENE_SECTION_REGION_COUNT = EM_SCENE_SECTION_REGION_PER_EDGE * EM_SCENE_SECTION_REGION_PER_EDGE,//64
	EM_SCENE_SECTION_WIDTH = EM_SCENE_REGION_WIDTH * EM_SCENE_SECTION_REGION_PER_EDGE,//1600 * 8 = 12800
	
	EM_SCENE_ZONE_SECTION_PER_EDGE = EM_SCENE_SECTION_REGION_PER_EDGE,
	EM_SCENE_ZONE_SECTION_COUNT = EM_SCENE_ZONE_SECTION_PER_EDGE * EM_SCENE_ZONE_SECTION_PER_EDGE,	//64,
	EM_SCENE_ZONE_WIDTH = EM_SCENE_SECTION_WIDTH * EM_SCENE_ZONE_SECTION_PER_EDGE, //12800 * 8 = 102400

	EM_SUB_CONTAINER_COUNT = EM_SCENE_SECTION_REGION_PER_EDGE * EM_SCENE_SECTION_REGION_PER_EDGE,	//64
};

#define SCENESECTIONNUMREGION EM_SCENE_SECTION_REGION_PER_EDGE//8//指每边的，不是全部的

#define SCENEZONENUMREGION    EM_SCENE_SECTION_REGION_PER_EDGE*EM_SCENE_SECTION_REGION_PER_EDGE//64//指每边的，不是全部的

#define SCENEZONEWIDTH        EM_SCENE_ZONE_WIDTH		//102400
#define SCENESECTIONWIDTH     EM_SCENE_SECTION_WIDTH	//12800
#define SCENEREGIONWIDTH      EM_SCENE_REGION_WIDTH		//1600

#define BLOCK_CELL_COUNT      16

#define BLOCK_TYPE_REGION    0
#define BLOCK_TYPE_SECTION   1
#define BLOCK_TYPE_ZONE      2
#define BLOCK_TYPE_AREA      3

#define BLOCK_LOAD_NONE   0
#define BLOCK_LOAD_NODATA       1
#define BLOCK_LOAD_LISTOK       2

#define BLOCK_LOAD_TERRAIN_FINISH   10	//注意这个要比上面的那些大
#define BLOCK_LOAD_ALL_FINISH   100

///////////////////////////////////////////////////////////////////////
//创建Zone时用的控制选项    add by huangjinshou 2008-5-8
#define CREATE_DEFAULT               0xFF  //默认全部都创建
#define CREATE_SECTION				  1     //创建Section
#define CREATE_REGION				 (1<<1) //创建Region
#define CREATE_ZONE_TERRAINBLOCK	 (1<<2) //创建Zone的TerrainBlock
#define CREATE_SECTION_TERRAINBLOCK  (1<<3) //创建Section的TerrainBlock
#define CREATE_REGION_TERRAINBLOCK   (1<<4) //创建Retion的TerrainBlock
////////////////////////////////////////////////////////////////////////


class KG3DRepresentObject;
class KG3DModel;
class KG3DFrustum;
class KG3DTerrainBlock;
class KG3DSceneZone;
class KG3DSceneDataInterfaceCore;
class KG3DSceneDataInterface;
class KG3DSceneOutDoorSpaceMgr;
class KG3DRepresentObjectSoundBall;
class KG3DTerrainRoad;
class KG3DTerrainRiver;
class KG3DSceneEntityContainer;
class KG3DTerrainRender;
class KG3DModelPointLight;
struct IKG3DSceneEntityContainer;


struct KG3D_PointLight_Info 
{
	int m_nNumPointLights;
	D3DXVECTOR4 m_vPointLightPosition[MAX_POINTLIGHT];
	D3DXVECTOR4 m_vPointLightColor   [MAX_POINTLIGHT];

	KG3D_PointLight_Info()
	{
		m_nNumPointLights = 0;
		ZeroMemory(m_vPointLightPosition,sizeof(m_vPointLightPosition));
		ZeroMemory(m_vPointLightColor   ,sizeof(m_vPointLightColor))   ;
	}

	HRESULT UnInit()
	{
		m_nNumPointLights = 0;
		return S_OK;
	}

	HRESULT InputLight(KG3DModelPointLight* pLight);
};

class KG3DRepresentObjectPVS;

struct KG3DEntityObjectShell//object的包装类
{
	KG3DSceneOutDoorSpaceMgr* m_lpSpaceMgr;
	UINT m_uHandle;
	KG3DRepresentObject* m_lpObject;
	D3DXVECTOR3 m_vPosition;
	D3DXVECTOR3 m_vBBox_A;
	D3DXVECTOR3 m_vBBox_B;
	float       m_fDiameter;
	set<IKG3DSceneEntityContainer*>m_setRefrenceContainer;

	DWORD m_dwVisibleCount;
	DWORD GetVisibleCount(){return m_dwVisibleCount;};
	void  SetVisibleCount(DWORD dwCount){m_dwVisibleCount = dwCount;};

	HRESULT RemoveFromAllRefrenceContainer();

	HRESULT Init();
	HRESULT UnInit();

	HRESULT TryDynamicLoad(DWORD dwOption);
	HRESULT TryDynamicUnLoad();

	KG3DEntityObjectShell();
	~KG3DEntityObjectShell();

	BOOL m_bLoaded;

	KG3DRepresentObjectPVS* GetPvs();
};

struct KG3DEntityObjectShellVisitor 
{
	virtual HRESULT Accept(KG3DEntityObjectShell& objShell) = 0;
	virtual ~KG3DEntityObjectShellVisitor() = 0{}
};

struct KG3DRepresentObjectVisitor;//在KG3DRepresentObject.h中定义
struct KG3DEntityCtVisitor;
class KG3DSceneEntityContainer;
class KG3DScene;
struct KG3DCullData;
class KG3DRepresentObjectPVS;

struct KG3DSceneEntityContainerData
{
	std::set<KG3DModel*>setModel;	//主要是boqiang传进来的东西,由KG3DModel::EnterSceneRegion插进来
	std::set<KG3DEntityObjectShell*>setObjectShell;
	set<UINT>m_setUnLoadObjectHandle;
	KMutex m_Lock;
	KG3DFrustum::INTERSECT_TYPE  m_eFrustumIntersectType;
	//set<KG3DEntityObjectShell*>m_setPvs;

	KG3DSceneEntityContainerData();
	~KG3DSceneEntityContainerData();

	//HRESULT AddPVS(KG3DRepresentObjectPVS* pPVS);
	//HRESULT RemovePVS(KG3DRepresentObjectPVS* pPVS);

	HRESULT RemoveEntityObjectShell(KG3DEntityObjectShell* pShell);
	HRESULT UnInit();
	HRESULT GetObjectShellIntersectPoint2D(list<KG3DEntityObjectShell*>*plistShell,D3DXVECTOR2& vPos);
	HRESULT Traverse( KG3DRepresentObjectVisitor& Visitor );
	HRESULT Traverse( KG3DEntityObjectShellVisitor& visitorIn );
	VOID    Clear(KG3DSceneEntityContainer* pCon);
	HRESULT Add( KG3DModel* pModel,KG3DSceneEntityContainer* pCon );
	void    ReInputObject(BOOL bClient);
	bool    InputObjectToPvs(KG3DEntityObjectShell* objShell);
	BOOL    InputObjectShellToPvs(KG3DEntityObjectShell* objShell);
	HRESULT ComputeBBox(float x,float z,AABBOX& bbox,BOOL bClear);
	HRESULT InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs,KG3DSceneEntityContainer* pCon );
	
	HRESULT GetUnLoadObjectShellHandles(DWORD& dwCount,UINT* pHandle,DWORD dwMaxCount);
	HRESULT ClearUnLoadObjectShellHandles();
	HRESULT EraseUnLoadObjectShellHandles(DWORD dwCount,UINT* pHandle);
	HRESULT SetUnLoadObjectShellHandles(set<UINT>*pHandle);
	DWORD GetUnLoadObjectShellHandlesSize();

	HRESULT GetAllObjectShell(set<KG3DEntityObjectShell*>* pSetShell);

	HRESULT CheckInput2Pvs(D3DXVECTOR3& vPos,KG3DModel* pModel);

	HRESULT FindUnLoadedObjectShellInRect(set<KG3DEntityObjectShell*>*psetShells);
	HRESULT GetEntityIntersectRect(KG3DSceneEntityList*plistEntity,D3DXVECTOR2& A,D3DXVECTOR2& C,BOOL bShellMustLoaded);

	HRESULT OnModelsVisible(set<KG3DModel*>*psetModel,int nLevel);
	HRESULT _GetModelFromContainers(set<KG3DModel*>*psetModel,const vector<KG3DFrustum>& vFrustum);
	HRESULT EraseModel(KG3DModel* pModel);

	HRESULT GetVisibleEntityShell(DWORD& dwCount,KG3DRepresentObject** pObject,DWORD dwMaxCount,const vector<KG3DFrustum> vFrustum);
	HRESULT GetAllObject(DWORD& dwCount,KG3DRepresentObject** pObject,DWORD dwMaxCount);

	HRESULT _GetEntityShellFromContainers(
		BOOL bCheckVisibleCount,float fVisibleAngleGate,float fNearbyObjectVisibleAngleGate,
		DWORD& dwCount,KG3DRepresentObject** pObject,DWORD dwMaxCount,
		const D3DXVECTOR3& vCameraPos, KG3DScene *pScene, const vector<KG3DFrustum>& vFrustum);

};

class KG3DSceneEntityContainer : public IKG3DSceneEntityContainer
{
	friend class KG3DSceneOutDoorSpaceMgr;
private:
	//std::set<KG3DModel*>setModel;	//主要是boqiang传进来的东西,由KG3DModel::EnterSceneRegion插进来
	//std::set<KG3DEntityObjectShell*>setObjectShell;
	//set<UINT>m_setUnLoadObjectHandle;
public:
	KG3DSceneEntityContainerData m_Data;

	KG3DTerrainBlock* m_lpTerrainBlock;

	DWORD m_dwVisibleCount;
	DWORD GetVisibleCount(){return m_dwVisibleCount;};
	void  SetVisibleCount(DWORD dwCount){m_dwVisibleCount = dwCount;};

	float m_fSortValue;
    DWORD m_dwLastCullCounter;
protected:
	AABBOX  m_BBox;	//用GetAABBox就可以得到Box了，Set的话用SetAABBox，如果要只改一部分，

private:	//一步一步把东西向private方向移，直到这个类可以变成接口
	std::set<KG3DRepresentObject*>setObject;

	DWORD m_dwTerrainLoadState;//加载的状态
	DWORD m_dwObjectLoadState;
public:
    virtual HRESULT ComputeBBox(BOOL bClear) = 0;

	HRESULT AddPVS(KG3DRepresentObjectPVS* pPVS);
	HRESULT RemovePVS(KG3DRepresentObjectPVS* pPVS);

	
	HRESULT RemoveRepresentObject(KG3DRepresentObject* pObject);	//只在set中移除Object，不做其它操作
	HRESULT RemoveEntityObjectShell(KG3DEntityObjectShell* pShell);

	HRESULT UnInit();

	//参考cpp中的说明
	//<属性>
	virtual ULONG	GetType()const = 0;//得到类型，故意写成虚函数的，请不要搞成实函数。意义是不想这个类直接被实例化，接口类实例化之后要改行为就变得很难了。
	DWORD	GetTerrainLoadState(){return m_dwTerrainLoadState;}
	VOID	SetTerrainLoadState(DWORD State){m_dwTerrainLoadState = State;}

	DWORD	GetObjectLoadState(){return m_dwObjectLoadState;}
	VOID	SetObjectLoadState(DWORD State){m_dwObjectLoadState = State;}

    AABBOX& GetAABBox(){return m_BBox;}
	KG3DTerrainBlock* GetTerrainBlock();
	//</属性>
	//<遍历>
	virtual ULONG	GetSubContainerCountX()const{return 0;}
	virtual ULONG	GetSubContainerCountZ()const{return 0;}
	virtual KG3DSceneEntityContainer* GetSubContainer(INT nLocalX, INT nLocalZ){return NULL;}//这样就可以避免直接访问数组了	
	
	virtual HRESULT Traverse(KG3DEntityObjectShellVisitor& visitorIn);
	virtual HRESULT Traverse(KG3DRepresentObjectVisitor& Visitor);//不访问下面的层级
	virtual HRESULT TraverseOldFormat(KG3DRepresentObjectVisitor& Visitor);//旧格式里面没有ObjectShell这个东东
	virtual HRESULT	TraverseSubContainer(KG3DEntityCtVisitor& visitor){return E_FAIL;}//深度优先
	//</遍历>
	
	virtual VOID	Clear();
	virtual HRESULT Add(KG3DRepresentObject* pObject);
	virtual HRESULT Add(KG3DModel* pModel);

    virtual bool InputObjectToPvs(KG3DEntityObjectShell* objShell);
    virtual void OnInputObject(KG3DEntityObjectShell* objShell) {}
    virtual void ReInputObject(BOOL bClient);
  //  virtual bool CheckConnectNode(KG3DEntityObjectShell* objShell);

	HRESULT InputRepresentObject(KG3DRepresentObject* pObject);

	typedef auto_ptr<KGCH::TFrontAccessProxy<KG3DRepresentObject*> > TypeAccessProxy;
	TypeAccessProxy	GetSetObjects();	//setObject要一步步重构掉，这里开始封闭其作用域。不想写太多的函数，直接用Proxy模板来做，可能复杂了一些，但不是最终代码，所以可以暂时忽略其复杂性

	virtual ~KG3DSceneEntityContainer() = 0{};
	KG3DSceneEntityContainer();

};

struct KG3DEntityCtVisitor 
{
	virtual HRESULT Accept(KG3DSceneEntityContainer& Container) = 0;//允许返回上面3种值来控制遍历
	virtual ~KG3DEntityCtVisitor() = 0{}
};

struct KG3DEntityCtVisitorForObj : public KG3DEntityCtVisitor
{
	virtual HRESULT Accept(KG3DRepresentObject& Obj) = 0;
	virtual HRESULT Accept(KG3DSceneEntityContainer& Container);
};

struct KG3DEntityCtVisitorForObjVisitor : public KG3DEntityCtVisitor
{
	KG3DRepresentObjectVisitor& m_objVisitor;
	KG3DEntityCtVisitorForObjVisitor(KG3DRepresentObjectVisitor& objVisitor):m_objVisitor(objVisitor){}
	virtual HRESULT Accept(KG3DSceneEntityContainer& Container);	//会转调给ObjectVisitor
};

struct KG3DEntityCtVisitorForShell : public KG3DEntityCtVisitor
{
	virtual HRESULT Accept(KG3DEntityObjectShell& objShell) = 0;
	virtual HRESULT Accept(KG3DSceneEntityContainer& Container);
};

class KG3DSceneRegion : public KG3DSceneEntityContainer//16*16
{
public:
	KG3DSceneRegion();
	~KG3DSceneRegion();

	HRESULT Init(int nX,int nZ);
	HRESULT UnInit();
	
	HRESULT CreateTerrainBlock();
	HRESULT DeleteTerrainBlock();
	virtual HRESULT ComputeBBox(BOOL bClear);

	float   GetGroundHeight(float fX,float fZ);
	HRESULT GetWaterHeight(float& fHeight,float fX,float fZ);
	BYTE GetDetailTextureIndex(float fX,float fZ); 

    virtual void OnInputObject(KG3DEntityObjectShell* objShell);
	HRESULT WedTerrainNormal(KG3DSceneRegion* pLeft,KG3DSceneRegion* pRight,KG3DSceneRegion* pUp,KG3DSceneRegion* pDown);
	HRESULT InputRepresentObject(KG3DRepresentObject* pObject);
	HRESULT InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs);

	DWORD GetTerrainType(const D3DXVECTOR2& vec2Pos);
	int m_nIndexX;
	int m_nIndexZ;
	//AABBOX m_bbox;
	//D3DXVECTOR3 m_BoxPos[8];//不需要，Box一定是轴对其的，没有OO的情况

	DWORD dwViewClass;

	virtual ULONG	GetType()const;
	virtual ULONG	GetSubContainerCountX(){return 0;}
	virtual ULONG	GetSubContainerCountZ(){return 0;}
	virtual KG3DSceneEntityContainer* GetSubContainer(INT nLocalX, INT nLocalZ){return NULL;}
	virtual HRESULT	TraverseSubContainer(KG3DEntityCtVisitor& visitor){return EM_VISITOR_CONTINUE;}//不要改返回值

	HRESULT CheckLodLevel();
	
};
//////////////////////////////////////////////////////////////////////////
class KG3DSceneSection : public KG3DSceneEntityContainer//128*128
{
public:
	BOOL m_bRegionDetail;//是否拥有Region作为详细数据
	BOOL m_bAllRegionReady;

	int m_nIndexX;
	int m_nIndexZ;
	//AABBOX m_bbox;
	//D3DXVECTOR3 m_BoxPos[8];//不需要，Box一定是轴对其的，没有OO的情况
	//////////////////////////////////////////////////////////////////////////
	HRESULT Init(int nX,int nZ);
	HRESULT UnInit();

	enum
	{
		EM_REGIONS_COUNT = SCENESECTIONNUMREGION * SCENESECTIONNUMREGION,//64
	};
	KG3DSceneRegion* m_lpRegions[EM_REGIONS_COUNT];

	HRESULT CreateTerrainBlock();
	HRESULT DeleteTerrainBlock();

	HRESULT CreateAllRegions();
	HRESULT FreeAllRegions();

	HRESULT NewOneRegion(KG3DSceneRegion** ppNewRegion,int nRegionX,int nRegionZ,int nX_Local,int nZ_Local);
	HRESULT DeleteRegion(KG3DSceneRegion* pRegion,int nRegionX,int nRegionZ,int nX_Local,int nZ_Local);

	KG3DSceneRegion* GetRegionByIndex(int nSectionX,int nSectionZ,int nX,int nZ);

	HRESULT GetVisiableSceneRegions(
		vector<KG3DSceneEntityContainer *> *pvecContainer,
		KG3DTerrainRender *pTerrainRender, 
		const KG3DFrustum *pFrustum, 
        const D3DXVECTOR3 &vPosCamera,
		int nLevelDeep);

	virtual HRESULT ComputeBBox(BOOL bClear);

	HRESULT CheckRegionDetial();
	HRESULT CheckRegionLoad();
	HRESULT CheckMemoryUseMultiThread(DWORD dwTime,BOOL* pbStop);

	float GetGroundHeight(float fX,float fZ);
	HRESULT GetWaterHeight(float& fHeight,float fX,float fZ);
	BYTE GetDetailTextureIndex(float fX,float fZ); 

	HRESULT WedTerrainNormal(KG3DSceneSection* pLeft,KG3DSceneSection* pRight,KG3DSceneSection* pUp,KG3DSceneSection* pDown);

    virtual void OnInputObject(KG3DEntityObjectShell* objShell);
	HRESULT InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs);
	HRESULT InputRepresentObject(KG3DRepresentObject* pObject);
	HRESULT SelectRegion(vector<KG3DSceneRegion*>*pvecRegion,D3DXVECTOR2 A,D3DXVECTOR2 C);

	virtual ULONG	GetType()const;
	virtual ULONG	GetSubContainerCountX(){return SCENESECTIONNUMREGION;}
	virtual ULONG	GetSubContainerCountZ(){return SCENESECTIONNUMREGION;}
	virtual KG3DSceneEntityContainer* GetSubContainer(INT nLocalX, INT nLocalZ);
	virtual HRESULT	TraverseSubContainer(KG3DEntityCtVisitor& visitor);

	HRESULT FindSoundBall(KG3DRepresentObjectVisitor& visitor,D3DXVECTOR2 A,D3DXVECTOR2 C);
	
	HRESULT CheckLodLevel();

	KG3DSceneSection();
	~KG3DSceneSection();
};

class KG3DSceneDataInterfaceCore;

//////////////////////////////////////////////////////////////////////////
class KG3DSceneZone : public KG3DSceneEntityContainer//1024*1024
{
	HRESULT CheckSectionDetial();
public:
	//AABBOX m_bbox;
	//D3DXVECTOR3 m_BoxPos[8];//不需要，Box一定是轴对其的，没有OO的情况

	enum
	{
		EM_SECTIONS_COUNT = SCENESECTIONNUMREGION * SCENESECTIONNUMREGION,
	};
	KG3DSceneSection* m_lpSections[EM_SECTIONS_COUNT];//Zone所拥有Section,64个

	KG3DSceneZone();
	~KG3DSceneZone();

    virtual HRESULT ComputeBBox(BOOL bClear);

	HRESULT GetVisiableSceneSections(
		vector<KG3DSceneEntityContainer *> *pvecContainer,
		KG3DTerrainRender *pTerrainRender,
		map<int,KG3DTerrainRoad *> *pmapRoad,
		set<KG3DTerrainRiver *> *psetRiver,
		const KG3DFrustum *pFrustum,const D3DXVECTOR3& vPosCamera,int nLevelDeep);

	HRESULT NewOneSection(KG3DSceneSection** ppNewSection,int nSectionX,int nSectionZ,int nX_Local,int nZ_Local);
	HRESULT DeleteSection(KG3DSceneSection* pSection,int nSectionX,int nSectionZ,int nX_Local,int nZ_Local);

	HRESULT CreateTerrainBlock();
	HRESULT DeleteTerrainBlock();

	HRESULT CreateSections(BOOL bCreateAllRegion);

	HRESULT Init(int nX,int nZ);

	HRESULT CleanUp();
	HRESULT UnInit();

	float GetGroundHeight(float fX,float fZ);
	HRESULT GetWaterHeight(float& fHeight,float fX,float fZ);
	BYTE GetDetailTextureIndex(float fX,float fZ); 

	HRESULT WedTerrainNormal(KG3DSceneZone* pLeft,KG3DSceneZone* pRight,KG3DSceneZone* pUp,KG3DSceneZone* pDown);

    virtual void OnInputObject(KG3DEntityObjectShell* objShell);
	HRESULT InputRepresentObject(KG3DRepresentObject* pObject);
	HRESULT InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs);

	HRESULT SelectSection(vector<KG3DSceneSection*>*pvecSection,D3DXVECTOR2 A,D3DXVECTOR2 C);

	HRESULT FindSoundBall(KG3DRepresentObjectVisitor& visitor,D3DXVECTOR2 A,D3DXVECTOR2 C);

	virtual ULONG	GetType()const;
	virtual ULONG	GetSubContainerCountX(){return SCENESECTIONNUMREGION;}
	virtual ULONG	GetSubContainerCountZ(){return SCENESECTIONNUMREGION;}
	virtual KG3DSceneEntityContainer* GetSubContainer(INT nLocalX, INT nLocalZ);
	virtual HRESULT TraverseSubContainer(KG3DEntityCtVisitor& visitor);

	BOOL	HasSectionDetail(){return m_bSectionDetail;}
	INT		GetIndexX()		{return m_nIndexX;}
	INT		GetIndexZ()		{return m_nIndexZ;}

	HRESULT CheckMemoryUseMultiThread(DWORD dwTime,BOOL* pbStop);

private:
	BOOL m_bSectionDetail;//是否拥有Section作为详细数据
	int m_nIndexX;
	int m_nIndexZ;
};
//////////////////////////////////////////////////////////////////////////
class KG3DSceneBlock
{
public:
	KG3DSceneBlock(void);
	~KG3DSceneBlock(void);
};





extern INT GetSceneBlockIndex(FLOAT In, FLOAT BlockWidth);
extern __forceinline INT GetSceneRegionIndex(FLOAT In){return GetSceneBlockIndex(In, SCENEREGIONWIDTH);}
extern __forceinline INT GetSceneSectionIndex(float In){return GetSceneBlockIndex(In, SCENESECTIONWIDTH);}
extern __forceinline INT GetSceneZoneIndex(float In){return GetSceneBlockIndex(In, SCENEZONEWIDTH);}


extern inline int GetZoneIndex(float In){return GetSceneZoneIndex(In);}
