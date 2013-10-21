#pragma once

#include "KG3DGraphicsTool.h"
#include "KG3DShaderHolder.h"

#define BLOCK_LOD_NUM  3

class KG3DTerrainBlock;
class KG3DTexture;
class KG3DTerrainDetailMaterialMgr;
class KG3DTerrainGrassesRender;
class OceanSurface;
class KG3DSceneSceneEditor;
class KG3DTerrainRoad;
class KG3DTerrainConverMap;
class KG3DTerrainRiver;
class KG3DScenePointLightRender;
class KG3DWaterCuller;
class KG3DSceneOutDoorSpaceMgr;
class KG3DTerrainRender;



class KG3DTerrainRenderData
{
private:
	vector<KG3DTerrainBlock*> m_vecTerrainBlock;     // 需要渲染的地形块
public:
	vector<KG3DTerrainBlock*>    m_vecTerrainBlockUnsort; //没有被排序筛选的地形块
	vector<KG3DTerrainBlock*>    m_vecVertexWaterBlock;
	map<int,KG3DTerrainRoad*> m_mapRoad;//需要渲染的路
	set<KG3DTerrainRiver*>    m_setRiver;////需要渲染的河流

	enum
	{
		BLOCK_GROUP_COUNT = 8,
	};
    typedef vector<KG3DTerrainBlock *> TERRAIN_BLOCKS_GROUP;
    TERRAIN_BLOCKS_GROUP m_aTerrainBlockGroup[BLOCK_GROUP_COUNT];
	vector<KG3DTerrainBlock*>m_vecTerrainBlockZoomOut;
	vector<KG3DTerrainBlock*> m_vecTerrainBlockLow;
	DWORD   GetVisibleTerrainBlockCount(){return (DWORD) m_vecTerrainBlock.size();};
	HRESULT UnInit();
	HRESULT RenderTerrainBlockDepthOnly(BOOL bTerrain,BOOL bGrass,BOOL bStone,BOOL bFrutex,BOOL bShader,KG3DTerrainRender* pRender);
	HRESULT RenderTerrainBlocks_ConverMap(BOOL bAll,KG3DTerrainRender* pRender);
	HRESULT GroupTerrainBlock(KG3DTerrainBlock* pBlock);
	HRESULT CleanTerrainBlockGroup();
	HRESULT RenderMapTerrainBlock(KG3DTerrainRender* pRender);
	HRESULT RenderBlockEdge(KG3DTerrainRender* pRender);
	HRESULT RenderMaskedTerrain(KG3DTerrainRender* pRender);
	HRESULT RenderTerrainBlocks_Detail(KG3DLightParam& lightParam,BOOL bGrass,BOOL bStone,BOOL bFrutex,KG3DTerrainRender* pRender);
	HRESULT RenderTerrainBlocks_OnlyTexture(KG3DTerrainRender* pRender);
	HRESULT RenderRoad(KG3DTerrainRender* pRender);
	HRESULT RenderRiver(KG3DSceneSceneEditor* pEditor);
	HRESULT RenderWaterPrimitive(KG3DTerrainRender* pRender);
	HRESULT RemoveSortedTerrainBlock(KG3DTerrainBlock* pBlock,KG3DTerrainRender* pRender);
	HRESULT RenderMapTerrainBlock_Low(KG3DTerrainRender* pRender);

	HRESULT PrepareGrass(KG3DTerrainRender* pRender);
	HRESULT GetWaterBoundingPlanes(KG3DWaterCuller* pWaterCuller,KG3DTerrainRender* pRender);
	HRESULT RenderWaterEdge(KG3DTerrainRender* pRender);
	HRESULT SortTerrainBlock();
	HRESULT ProcessPointLighting(KG3DScenePointLightRender* pLights);
	HRESULT ProcessLOD(KG3DSceneOutDoorSpaceMgr* pOutDoorMgr);

	//////////////////////////////////////////////////////////////////////////
	HRESULT CleanVecTerrainBlock();
	HRESULT InputTerrainBlock(KG3DTerrainBlock* pBlock);

	HRESULT BeforeRenderProcess();

	KG3DTerrainRenderData();
	~KG3DTerrainRenderData();
};

class KG3DTerrainCullData
{
public:
	set<KG3DTerrainBlock*>      m_setTerrainBlockCache; // 地形块缓存
	map<int,KG3DTerrainRoad*>   m_mapRoad;              // 需要渲染的路
	set<KG3DTerrainRiver*>      m_setRiver;             // 需要渲染的河流
	//////////////////////////////////////////////////////////////////////////
	vector<KG3DTerrainBlock*>   m_vecTerrainBlockZoomOut;

	HRESULT UnInit();
	HRESULT CheckUnVisible();
	HRESULT TryInputTerrainBlock(KG3DTerrainBlock* pBlock);
	HRESULT PrepareRenderData(KG3DTerrainRenderData* pRenderData,KG3DSceneOutDoorSpaceMgr* pOutDoorMgr);

	KG3DTerrainCullData();
	~KG3DTerrainCullData();

};

class KG3DTerrainRender
{
protected:
	friend class KG3DTerrainCullData;
	friend class KG3DTerrainRenderData;

	KG3DTerrainCullData   m_CullData;
	KG3DTerrainRenderData m_RenderData;
	
	KG3DShaderHolder	  m_TerrainShader;


	D3DXVECTOR4 m_UVTransform[16];
	D3DXVECTOR4 m_MtlFactor[8];

	KG3DTerrainGrassesRender* m_lpGrassRender;
	OceanSurface* m_lpOceanSurfaceRender;
	
	BOOL m_bNeedSortTerrainBlock;

	//////////////////////////////////////////////////////////////////////////
	LPDIRECT3DVERTEXDECLARATION9 m_lpDeclaration_VertexOnly;
	LPDIRECT3DVERTEXDECLARATION9 m_lpDeclaration_Blend1;
//	LPDIRECT3DVERTEXDECLARATION9 m_lpDeclaration_Blend2;
	LPDIRECT3DVERTEXDECLARATION9 m_lpDeclaration_Water;

	LPDIRECT3DINDEXBUFFER9 m_lpEdgeIndex;
	KG3DShaderHolder m_RoadShader;
public:

	set<KG3DTerrainRiver*>*    GetRiverSet();
	map<int,KG3DTerrainRoad*>* GetRoadMap();

	HRESULT CleanRoad();
	HRESULT CleanRiver();

	//////////////////////////////////////////////////////////////////////////
	HRESULT RenderTerrainBlocks_OnlyTexture();
	HRESULT RenderTerrainBlocks_Detail(KG3DLightParam& lightParam,BOOL bGrass,BOOL bStone,BOOL bFrutex);
	HRESULT RenderWaterPrimitive();
	HRESULT ProcessPointLighting(KG3DScenePointLightRender* pLights);

	void EnableSortTerrainBlock(BOOL bVal = TRUE)
	{
		m_bNeedSortTerrainBlock = bVal;
	}

	DWORD GetVisibleTerrainBlockCount();
	DWORD GetVisibleGrassBlockCount();

	HRESULT TryInputTerrainBlock(KG3DTerrainBlock* pBlock);
	HRESULT PrepareRenderData(KG3DSceneOutDoorSpaceMgr* pOutDoorMgr);


    void SetEyePos(D3DXVECTOR3& vPos){m_vEyePos = vPos;}
	HRESULT RenderRiver(KG3DSceneSceneEditor* pEditor);

	//HRESULT PrepareRender(BOOL bUseFFTW);
    HRESULT PostRender();

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

	HRESULT RenderTerrainBlockDepthOnly(BOOL bTerrain,BOOL bGrass,BOOL bStone,BOOL bFrutex,BOOL bShader);

	HRESULT Init();
	HRESULT UnInit();

	KG3DTerrainRender(void);
	~KG3DTerrainRender(void);

	HRESULT SetTerrainMaterialMgr(KG3DTerrainDetailMaterialMgr* pMgr)
	{
		m_lpMtlMgr = pMgr;
		return S_OK;
	}

	HRESULT SetTerrainConverMgr(KG3DTerrainConverMap* pMap)
	{
		m_lpConverMap = pMap;
		return S_OK;
	}

	HRESULT RenderWater(KG3DSceneSceneEditor* pEditor,BOOL bUseFFTW);
	DWORD GetDetailTextureInfo(DWORD dwDetailIndex);
	HRESULT	EnableRenderMask(BOOL bEnable, DWORD dwMaskTexID, const D3DXVECTOR4* pSceneRect);
	HRESULT ReloadShaders();
    
	VOID	EnableCheckVisibleCount(BOOL bEnable){m_bCheckVisibleCount = bEnable;}
    VOID    SetWaterVisible(BOOL bWaterVisible) {m_bWaterVisible = bWaterVisible;}
    
	BOOL    IsWaterVisible() {return m_bWaterVisible;}
    HRESULT GetWaterBoundingPlanes(KG3DWaterCuller* pWaterCuller);

	//////////////////////////////////////////////////////////////////////////
private:

	HRESULT InitShader();

	HRESULT CreateOceanRender();

	HRESULT CreateBlockEdgeIndex();

    HRESULT RenderVertexWater(KG3DSceneSceneEditor* pEditor);
	HRESULT RenderTerrainBlocks_ConverMap(BOOL bAll);
	HRESULT RenderMapTerrainBlock();
	HRESULT RenderMapTerrainBlock_Low();

	HRESULT CleanTerrainBlockGroup();

	HRESULT LoadPatchIndex(const char *FileName,vector<WORD> &IndexList);
	HRESULT CreateIndexBuffer(LPDIRECT3DINDEXBUFFER9 *ppindex,vector<WORD> &data);
	HRESULT LoadIndexBufferFromFile();

	int m_nLevel0FaceCount[16];
	int m_nLevel1FaceCount[16];
	int m_nLevel2FaceCount[16];
	int m_nLevel3FaceCount;
	LPDIRECT3DINDEXBUFFER9 m_lpLevel0Index[16];
	LPDIRECT3DINDEXBUFFER9 m_lpLevel1Index[16];
	LPDIRECT3DINDEXBUFFER9 m_lpLevel2Index[16];
	LPDIRECT3DINDEXBUFFER9 m_lpLevel3Index;  
	struct PatchIndexHeader
	{
		int SzieOfPerIndex;
		int NumOfIndex;
		int Reserve[10];
	};
private:

	HRESULT	SetShaderParameters(const KG3DLightParam& lightParam);

	KG3DTexture* m_lpDefaultTextureDetail0;
	KG3DTexture* m_lpDefaultTextureDetail1;
	KG3DTexture* m_lpDefaultTextureDetail2;
	KG3DTexture* m_lpDefaultTextureDetail3;

	KG3DTerrainDetailMaterialMgr* m_lpMtlMgr;
	KG3DTerrainConverMap*         m_lpConverMap;

	D3DXVECTOR3 m_vEyePos;
	BOOL	m_bCheckVisibleCount;
    BOOL    m_bWaterVisible;  // 地形中是否有可见的水，来决定是否要做水相关的渲染

#if defined(DEBUG) | defined(_DEBUG)
protected:
#else
private:
#endif
	DWORD	m_dwMaskTexID;
	BOOL	m_bRenderMask;
	D3DXVECTOR4 m_vSceneRect;

	HRESULT SetDetailTexture(KG3DTerrainBlock* pBlock);
	HRESULT GetIndexBuffer(KG3DTerrainBlock* pBlock,LPDIRECT3DINDEXBUFFER9& pIndex,DWORD& dwFaceCount);

};
