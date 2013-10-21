#pragma once

#include "KG3DTemplates.h"
#define  GRASS_NUM  16  //每块TerrainBlock上的草数目是GRASS_NUM * GRASS_NUM
class KG3DTexture;
class KG3DMemoryFile;
class KG3DTerrainBlock;
class KG3DSceneSceneEditor;
class KG3DSceneOutDoorSpaceMgr;
struct KG3DModelSubsetInfo;

struct KG3DTerrainGrassVertexSrc
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNormal;
	D3DXVECTOR2 vUV;
};
struct KG3DTerrainGrassVertex
{
	D3DXVECTOR3 vPos;
    D3DCOLOR    dwNormal;
    D3DCOLOR    dwUV;
};

class KG3DTerrainGrassPattern
{
public:
	string m_szName;
    KG3DTerrainGrassVertex * m_lpVertexBuffer;
	DWORD m_dwNumFace;
	DWORD m_dwNumVertex;

	HRESULT Init();
	HRESULT UnInit();

	HRESULT CreateFromMesh(LPCTSTR pFileName);

	KG3DTerrainGrassPattern();
	~KG3DTerrainGrassPattern();
};

struct KG3DTerrainGrass 
{
	D3DCOLOR dwColor;
	D3DXVECTOR3 vPosition;//位置
	D3DXVECTOR3 vNormal;
	float fWidth;
	float fHeight;
	BYTE  byteIndex;//贴图中的第几个
	BYTE  bytePatternIndex;
};

class KG3DTerrainBlockGrass;

class KG3DTerrainGrassSet
{
protected:
	ULONG m_ulRefCount;

public:
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	string m_strTextureFileName;
	KG3DTexture* m_lpGrassTexture;

	DWORD  m_dwStrideX;
	DWORD  m_dwStrideY;

	HRESULT Init();
	HRESULT UnInit();

	HRESULT SetTexture(LPCSTR strFileName);

	HRESULT LoadPattern(LPCTSTR pFileName,int& nIndex);
	KG3DTerrainGrassPattern* FindPattern(int nIndex);
	vector<KG3DTerrainGrassPattern*>m_vecGrassPattern;
	
	HRESULT SaveToInformationBuffer(DWORD& dwLength,KG3DMemoryFile* pFile);
	HRESULT LoadFromInformationBuffer(DWORD dwLength,KG3DMemoryFile* pFile);

	KG3DTerrainGrassSet();
	~KG3DTerrainGrassSet();
};

#define MAX_GRASSVERTEX 1024*1024*4    //每个区块上的顶点数目最多4M大小

//extern KG3DTerrainGrassVertex g_zGrassVertexBuffer[MAX_GRASSVERTEX];

struct KG3DTerrainBlockGrassShareVB_High
{
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	DWORD m_dwNumFacesTotal;
	DWORD m_dwUsedFaces;

	DWORD dwLastTime;
	PVOID pUser;
	BOOL  m_bUserLock;
	BOOL  m_bOkToRender;
	list <KG3DTerrainBlockGrassShareVB_High*>::iterator nPosition;

	KG3DTerrainBlockGrassShareVB_High();
	~KG3DTerrainBlockGrassShareVB_High();

	void GetUVFromIndex(DWORD dwTexIndex,D3DXVECTOR3& vec2UV,
		DWORD m_dwStrideX,DWORD m_dwStrideY);
	
	HRESULT FillGrassVertex(DWORD& dwFaceCount,KG3DTerrainBlockGrass* pGrass,KG3DTerrainGrassVertex* pVertex);
	HRESULT FillXMesh(DWORD& dwPosition,LPD3DXMESH pXMesh,D3DXMATRIX* pMat);
	/*HRESULT FillXMeshSubset(DWORD& dwPosition,LPD3DXMESH pXMesh,DWORD dwSubSetID,D3DXMATRIX* pMat);*/

	HRESULT CheckSize(DWORD dwFace);

	HRESULT Create(KG3DTerrainBlockGrass* pBlock = NULL);

	HRESULT CreateFromModels(DWORD dwTotalFace,vector<KG3DModel*>*pvecModel,PVOID pCallUser);
	//HRESULT CreateFromModels(DWORD dwTotalFace,list<KG3DModelSubsetInfo*>&listSubsetToBuffer,PVOID pCallUser);

	HRESULT UnInit();
	HRESULT Init();

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();

	DWORD   GetResourceUse()
	{
		return m_dwNumFacesTotal * sizeof(KG3DTerrainGrassVertex) * 3;
	}
};

class KG3DTerrainBlockGrass
{
	friend class KG3DTerrainBlockModifier;
	friend struct KG3DTerrainBlockGrassShareVB_High;
	friend class KG3DTerrainBlock;

protected:
	KG3DTerrainBlockGrassShareVB_High* m_lpShareGrassVB;	
	DWORD m_lpAutoGrassData[GRASS_NUM * GRASS_NUM];//自动生成的草
	float m_lpAutoGrassWidth[GRASS_NUM * GRASS_NUM];//草的宽度
	float m_lpAutoGrassHeight[GRASS_NUM * GRASS_NUM];//草的高度
	BOOL m_bAutoGrass;
	
	DWORD m_dwBlockType; //标识此BLOCK是草，石头，还是灌木
	/////////////////////////////////////////////////////
	KG3DTerrainGrassSet* m_lpGrassSet;
	UINT  m_uHandleGrassSet;
	DWORD m_dwNumGrass; //只用于记录草面版上用笔刷刷上去的草数目。（跟地表相关联的草不在计数内）
	KG3DTerrainGrass* m_lpGrassData;

	DWORD m_dwNumGrassFace;

	list<KG3DTerrainGrass> m_listGrass; //用于添加删除草操作，存放草数据，只有在编辑器中旧的草面版用到


	BOOL m_bNeedRefreshGrass;

	DWORD GetTotalGrassFaceCount();
	ULONG m_ulRefCount;
public:
	DWORD m_dwLastVisibleCount;
public:
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT PrepareShareGrassVB();
	HRESULT UnPrepareShareGrassVB();

	DWORD GetGrassCount(){return m_dwNumGrass;};

	KG3DTerrainBlock* m_lpParentTerrainBlock;

	HRESULT CreateGrassVertex();

	LPDIRECT3DTEXTURE9 GetGrassTexture();
	LPDIRECT3DVERTEXBUFFER9 GetShareGrassVBuffer(DWORD& dwFaces);

	HRESULT SetGrassSet(KG3DTerrainGrassSet* pSet,UINT uHandleSet);

	HRESULT CreateGrass(DWORD dwNum);

	HRESULT Init(DWORD dwType);
	HRESULT UnInit();

	KG3DTerrainBlockGrass();
	~KG3DTerrainBlockGrass(void);

public:
	HRESULT AddGrass(KG3DTerrainGrass& sGrass);
	HRESULT RemoveGrass(FRECT rect);
	HRESULT ClearGrassList();//清除listGrass里的数据
	HRESULT GetVecGrass(vector<KG3DTerrainGrass>* pVecGrass);

	HRESULT FitGround(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);

	HRESULT GetAutoGrassData(int nX,int nZ,BOOL& bCreat,BYTE& byDetailTexture,BYTE& byTexIndex,BYTE& byPattrenIndex,float& fWidth,float& fHeight);
	HRESULT SetAutoGrassData(int nX,int nZ,BOOL bCreat,BYTE byDetailTexture,BYTE byTexIndex,BYTE byPattrenIndex,float fWidth,float fHeight);

	DWORD GetBlockType()
	{
		return m_dwBlockType;
	}
};

extern TKG3DResourcePool<KG3DTerrainBlockGrass> g_cTerrainBlockGrassPool;
extern TKG3DResourcePool<KG3DTerrainBlockGrassShareVB_High> g_cTerrainBlockGrassShareVBHighPool;
