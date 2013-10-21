#pragma once
#include "KG3DKeyFrameDataBase.h"
#include "KG3DTemplates.h"

class KG3DTexture;
class KG3DHeightMap;
class KG3DTerrainDetailTexture;
class KG3DTerrainBlockGrass;
class KG3DSceneDataInterface;
class KG3DMemoryFile;
class KG3DFrustum;

struct KG3D_PointLight_Info;

struct KG3DTerrainConverMap
{
	string m_szTextureFileName;
	string m_szHeightColorFileName;
	
	KG3DTexture* m_lpTextureLight;
	KG3DTexture* m_lpTextureCloud;

	float  m_fLowest;
	float  m_fHeightTotal;
	DWORD  m_dwHeightMapSize;
	

	KG3DTerrainConverMap();
	~KG3DTerrainConverMap();

	HRESULT UnInit();
	/*
	GetRenderTargetTex
	如果是Shader的话，直接用就好了：
	m_lpTerrainShader->SetTexture("tConver",m_lpConverMap->GetRenderTargetTex());
	不要检查是否为空，然后里面用一个默认贴图。里面已经做了这个操作了。
	如果不小心改成一个不同格式的，会引发Shader重编译
	*/
	LPDIRECT3DTEXTURE9 GetRenderTargetTex();

	HRESULT LoadFromInformationBuffer(DWORD dwLength,KG3DMemoryFile* pFile);
	HRESULT SaveToInformationBuffer(DWORD& dwLength,KG3DMemoryFile* pFile);

	HRESULT ComputeHeightMapSize();
	LPDIRECT3DTEXTURE9	GetColorTex();
	const D3DXVECTOR4&	GetRect()	{return m_vRect;}//分别是x,y,宽,高
	VOID	SetRectVector(FLOAT StartX, FLOAT StartY, FLOAT Width, FLOAT Height);

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

	HRESULT Default();
	HRESULT EnableDynamic(BOOL bEnable)
	{
		m_bEnableDynamic = bEnable;
		return S_OK;
	}

	static KG3DTerrainConverMap& GetUniqueDefault();
	static HRESULT InitUniqueDefault();
	static HRESULT UnInitUniqueDefault();
private:
	LPDIRECT3DTEXTURE9 m_lpRenderTarget;
	KG3DTexture* m_lpColorTex;//底色图
	D3DXVECTOR4  m_vRect;
	BOOL m_bEnableDynamic;
	//static LPDIRECT3DTEXTURE9	m_stpColorTexDummy; 


	KG3DTexture* m_lpTerrainMap;
	string m_szTerrainMap;
public:
	HRESULT LoadTerrainMap(const char* cszFileName);
	LPDIRECT3DTEXTURE9 GetTerrainMap();
	void SetTerrainMapName(const char* cszFileName);
};

 
struct KG3DTerrainBlockShareVB
{
	VFormat::FACES_NORMAL_DIFFUSE_TEXTURE0 * m_lpLockVertex;
	BOOL m_bUserLock;//禁止别人使用，独占
	enum ShareVBState
	{
		STATE_NODATA,
		STATE_LOCKED,
		STATE_DATAOK,
	}m_eState;
	LPDIRECT3DVERTEXBUFFER9 pVB;
	DWORD dwLastTime;
	PVOID pUser;
	list<KG3DTerrainBlockShareVB*>::iterator nPosition;
	
	KG3DTerrainBlockShareVB();
	~KG3DTerrainBlockShareVB();

	HRESULT Init();
	HRESULT UnInit();

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();
};

struct KG3DTerrainBlockWaterShareVB
{
	LPDIRECT3DVERTEXBUFFER9 pVB;
	DWORD dwLastTime;
	PVOID pUser;
	BOOL  m_bUserLock;
	list<KG3DTerrainBlockWaterShareVB*>::iterator nPosition;

	KG3DTerrainBlockWaterShareVB();
	~KG3DTerrainBlockWaterShareVB();

	HRESULT Init();
	HRESULT UnInit();

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();
};

struct KG3DTerrainBlockBlendFactorShareVB
{
	LPDIRECT3DVERTEXBUFFER9 pVB;
	DWORD dwLastTime;
	PVOID pUser;
	BOOL  m_bUserLock;
	list<KG3DTerrainBlockBlendFactorShareVB*>::iterator nPosition;
	KG3DTerrainBlockBlendFactorShareVB();
	~KG3DTerrainBlockBlendFactorShareVB();

	HRESULT Init();
	HRESULT UnInit();

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();
};

struct KG3DTerrainBlockBlendFactor
{
	//D3DXVECTOR4 vBlend[17*17];
	float	vBlend[17*17];
	HRESULT UnInit() { return S_OK; }
};

#define TERRAIN_BLOCK_MAX_DETAILMAP   8
//#define TERRAIN_BLOCK_MAX_BLENDFACTOR 1//2

class KG3DTerrainBlock;
class KG3DSceneOutDoorSpaceMgr;
class KG3DTerrainRoad;
class KG3DTerrainRiver;
class KG3DSceneEntityContainer;
struct KG3DTerrainBlockWaterVertexs
{
	VFormat::_Faces_Normal_Diffuse_Texture0 m_WaterVertexs[17*17];//顶点

	HRESULT UnInit() { return S_OK; }
	HRESULT Init(KG3DTerrainBlock* pBlock);

	float* GetVertexPointer(int nX,int nZ);

};

class KG3DTerrainBlock : public IUnknown
{
protected:
	friend class KG3DTerrainBlockModifier;
	friend class KG3DSceneRegion;
	friend class KG3DTerrainRender;
	friend class KG3DTerrainRenderData;
	friend class KG3DSceneBlockLoader;

	KG3DTexture* m_lpConverTexture;
	UINT m_uHandleConverMap;

	UINT m_uHandleDetailMgr;

	KG3DTerrainBlockGrass* m_lpGrassData;//草相关信息
	KG3DTerrainBlockGrass* m_lpStoneData;//石头相关信息
	KG3DTerrainBlockGrass* m_lpFrutexData;//灌木相关信息

	map<UINT,KG3DTerrainRoad*> m_mapRoad;//道路相关信息
	map<UINT,KG3DTerrainRiver*>m_mapRiver;//河流相关信息

	vector<DWORD>m_vecDataLoadType;
	DWORD m_dwLastVisibleFrameCount;
	ULONG m_ulRefCount;

public:
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        REFIID riid,
        void __RPC_FAR *__RPC_FAR *ppvObject);

    KG3DSceneEntityContainer* m_Container;
	int m_nIndex_X;
	int m_nIndex_Z;
	int m_nLevel;

	int m_nLodLevel;//在Section、Region下分别再做一次LOD~
	int m_nIndexIB;//m_nNeighborLevel[4];

	float m_fHeightMin;
	float m_fHeightMax;
    float m_fWaterHeightMin;

	float m_fViewDistance;

	D3DXVECTOR4  m_vConverUV;
	
	
	VFormat::_Faces_Normal_Texture0 m_Vertexs[17*17];//顶点
	//VFormat::_Faces_Normal_Texture0* m_pVertexsEx;//m_VertexsEx[65*2];
	
	int  m_nNumDetailTexture;                        //细节贴图的数量，每四个细节贴图需要一组BlendFactor
	BYTE m_byteDetailTextureIndex[TERRAIN_BLOCK_MAX_DETAILMAP];

	BYTE m_byteTextureIndex[17*17];

	int  m_nNumBlendFactor;
	float m_fMainWaterHeight;
	KG3DTerrainBlockWaterVertexs* m_lpWaterVertex;
	//////////////////////////////////////////////////////////////////////////
	HRESULT Init(int nLevel,int nIndex_X,int nIndex_Z);
	HRESULT UnInit();

	KG3DTerrainBlock(void);
	~KG3DTerrainBlock(void);

	HRESULT CreateGrassData();
	HRESULT DeleteGrassData();
	KG3DTerrainBlockGrass* GetTerrainGrassBlock(BOOL bCreate = FALSE);
	
	HRESULT CreateStoneData();
	HRESULT DeleteStoneData();
	KG3DTerrainBlockGrass* GetTerrainStoneBlock(BOOL bCreate = FALSE);

	HRESULT CreateFrutexData();
	HRESULT DeleteFrutexData();
	KG3DTerrainBlockGrass* GetTerrainFrutexBlock(BOOL bCreate = FALSE);

	HRESULT GSFFitGround(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);//使草，碎石，灌木紧贴地表
	
	LPDIRECT3DVERTEXBUFFER9 GetGroundVertexBuffer();
	LPDIRECT3DVERTEXBUFFER9 GetBlendFactorBuffer();
	LPDIRECT3DVERTEXBUFFER9 GetWaterVertexBuffer();

	//HRESULT SetConverTexture(UINT uHandleConverMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface);
	//LPDIRECT3DTEXTURE9 GetConverTexture();
	//HRESULT SetHeight(KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface,BOOL bToFile);
	//HRESULT SetWaterHeight(KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface,BOOL bToFile);

	float GetGroundHeight(float fX,float fZ);
	BYTE GetDetailTextureIndex(float fX,float fZ); 
	HRESULT GetWaterHeight(float& fHeight,float fX,float fZ);

	HRESULT WedNormal(KG3DTerrainBlock* pLeft,KG3DTerrainBlock* pRight,KG3DTerrainBlock* pUp,KG3DTerrainBlock* pDown);

	HRESULT CheckBorderMtl(KG3DTerrainBlock* pNear[],KG3DSceneDataInterface* lpDataInterface);
	HRESULT SetDetailTexture(set<BYTE>*psetIndex,BYTE byteTextureIndex[],KG3DSceneDataInterface* pDataInterface,UINT uHandleMtlMgr);
	HRESULT ComputeMaterialKey();
	unsigned long long  m_ullMaterialKey;
	BOOL    m_bWaterVisible;
    
	HRESULT CreateVertexWater();
	HRESULT ReleaseVertexWater();
	HRESULT OnTerrainBlockModified(DWORD dwDataType,KG3DSceneDataInterface* pDataInterface,KG3DSceneOutDoorSpaceMgr* pMgr,BOOL bMultiThread);
	
	HRESULT AddAutoGrassByBrush(D3DXVECTOR2& vA,D3DXVECTOR2& vC);
	HRESULT PaintTextureByFilterMap(float& fWidth,float& fHeight,D3DSURFACE_DESC& desc,DWORD* pColor,BYTE byTexIndex);
	HRESULT PaintToFilterMap(float& fWidth,float& fHeight,D3DSURFACE_DESC& desc,DWORD* pColor,BYTE byTexIndex);
	HRESULT PaintToNormalMap(float& fWidth,float& fHeight,D3DSURFACE_DESC& desc,DWORD* pColor);
	HRESULT PaintTexture(D3DXVECTOR2& vA,D3DXVECTOR2& vC,BYTE TextureIndex);
	HRESULT ModifyDetailTexture(KG3DSceneDataInterface* lpDataInterface);
	HRESULT ClearTexture(BYTE byTexIndex);
	HRESULT InputRoad(KG3DTerrainRoad* pRoad,KG3DSceneDataInterface* lpDataInterface);
	HRESULT EraseRoad(UINT uHandle,KG3DSceneDataInterface* lpDataInterface);
	HRESULT ModifyRoadData(KG3DSceneDataInterface* lpDataInterface);
	HRESULT GetVisibleRoad(map<int,KG3DTerrainRoad*>* pmapRoad,const KG3DFrustum* pFrustum);

	HRESULT InputRiver(KG3DTerrainRiver* pRoad,KG3DSceneDataInterface* lpDataInterface);
	HRESULT EraseRiver(UINT uHandle,KG3DSceneDataInterface* lpDataInterface);
	HRESULT ModifyRiverData(KG3DSceneDataInterface* lpDataInterface);
	HRESULT GetVisibleRiver(set<KG3DTerrainRiver*>* psetRiver,const KG3DFrustum* pFrustum);

	DWORD GetLastVisibleFrameCount(){return m_dwLastVisibleFrameCount;};
	void  SetLastVisibleFrameCount(DWORD dwFrameCount){m_dwLastVisibleFrameCount = dwFrameCount;};

	HRESULT PrepareRender();
	HRESULT UnPrepareRender();
	D3DXVECTOR3 GetVertexPos(int nX,int nZ);
	D3DXVECTOR3 GetVertexNormal(int nX,int nZ);
	////////水面/////////////////////////////////
	HRESULT SetWaterHeight(D3DXVECTOR2& vA,D3DXVECTOR2& vC,float fHeight,KG3DSceneDataInterface* pDataInterface,BOOL bToFile);

	HRESULT OnLostDevice();

	HRESULT SetPointLightInfo(KG3D_PointLight_Info& vInfo);

	BOOL IsLoaded()
	{
		return m_bLoaded;
	}
	KG3D_PointLight_Info* m_lpPointLightInfo;
protected:

	HRESULT PocessDetailFactors(BYTE byteTextureIndex[]);

	int FindDetailTexture(BYTE bIndex);
	HRESULT Test();

	HRESULT      CheckWaterVisible();

	HRESULT      CleanBlendFactor();
	D3DXVECTOR3* GetVertexNormalPointer(int nX,int nZ);
	FLOAT GetVertexHeight(int nX,int nZ);

	HRESULT      PrepareSharedGroundVB();
	HRESULT      UnPrepareSharedGroundVB();

	HRESULT      PrepareSharedWaterVB();
	HRESULT      UnPrepareSharedWaterVB();

	HRESULT      PrepareSharedBlendFactorVB();
	HRESULT      UnPrepareSharedBlendFactorVB();

	HRESULT      SetNumBlendFactor(int nNumFactor);

//	HRESULT      SetBlendFactorPos(int nPos,int nX,int nZ);
	//////////////////////////////////////////////////////////////////////////
	BOOL m_bLoaded;
	enum enLoadedState
	{
		LOADED_NONE              =  0,
		LOADED_GROUNDVERTEX      =  0x1,
		LOADED_WATERVERTEX       = (0x1 << 1),	
	};
	DWORD m_dwLoadedState;

private:
    HRESULT      _PickUpWaterBoundingPlanes();

    struct _RECTANGLE
    {
        D3DXVECTOR3 vLeftBottomPos;
        D3DXVECTOR3 vRightUpPos;
    };

    std::map<int, _RECTANGLE> m_mapWaterBoundingPlanes;

	KG3DTerrainBlockShareVB* m_lpShareGroundVB;
	KG3DTerrainBlockWaterShareVB* m_lpShareWaterVB;
	/////Modify by huangjinshou 2009-417////////////////////////////////////////////////////////
	//把TerrainBlock改为无论有多少张贴图都只用一个BlendFactor
	KG3DTerrainBlockBlendFactorShareVB* m_lpzBlendFactorShareVB;//[TERRAIN_BLOCK_MAX_BLENDFACTOR];
	KG3DTerrainBlockBlendFactor* m_lpBlendFactor;               //[TERRAIN_BLOCK_MAX_BLENDFACTOR];
	BOOL m_bNeedRefreshBlendFacetorVB;                          //[TERRAIN_BLOCK_MAX_BLENDFACTOR];
	//////////////////////////////////////////////////////////////////////////////////////////////
	BOOL m_bNeedRefreshVectexVB;
	BOOL m_bNeedRefreshWaterVectexVB;
	
	D3DXVECTOR4  m_vBlockRect;                       //xstart,zstart,xlength,zlength
};

HRESULT InitTerrainBlockStaticResource();
HRESULT UnInitTerrainBlockStaticResource();
HRESULT OnLostDeviceTerrainBlockStaticResource();
HRESULT OnResetDeviceTerrainBlockStaticResource();

extern TKG3DResourcePool<KG3DTerrainBlockShareVB>g_cTerrainBlockShareVB;
