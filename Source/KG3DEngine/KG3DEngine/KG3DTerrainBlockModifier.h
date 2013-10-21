#pragma once

enum enExportOption
{
	EXPORT_FilterMap = 1,
	EXPORT_NormalMap = 2,
};
const WORD	MAX_BRUSH_SIZE1 =128;

class KG3DTerrainBlock;
class KG3DTerrainBlockGrass;
class KG3DTexture;
class KG3DHeightMap;
class KG3DTerrainGrassSet;
class KG3DSceneDataInterface;
class KG3DSceneOutDoorSpaceMgr;
class KG3DScene;

class KG3DTerrainBlockModifier
{
public:
	//HRESULT SetBlockConverTexture(int nLevel,POINT Index,UINT uTerrainInfo,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface);

	/*HRESULT SetBlockConverTexture(int nLevel,
		vector<POINT>*pvecTerrainBlockIndex,
		UINT uTerrainInfo,
		D3DXVECTOR2 A,D3DXVECTOR2 C,
		KG3DSceneDataInterface* pDataInterface);*/

    HRESULT Init();
    HRESULT UnInit();

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

	HRESULT SetBlockHeight(int nLevel,vector<POINT>*pvecTerrainBlockIndex,KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT SetBlockHeight(int nLevel,POINT Index,KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
  
	HRESULT SetBlockWaterHeight(int nLevel,POINT Index,KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface);
	HRESULT SetBlockWaterHeight(int nLevel,vector<POINT>*pvecTerrainBlockIndex,KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface);

	KG3DTerrainBlockModifier(void);
	~KG3DTerrainBlockModifier(void);

private:
	struct _Brush  //笔刷,用来绘制贴紧地面的笔刷和查询操作值
	{
		D3DXVECTOR3					m_Center;
		float						m_fWidth;

		LPD3DXMESH					m_lpOuterXMesh;
        LPD3DXMESH					m_lpInsideXMesh;

		LPDIRECT3DTEXTURE9			m_lpTexture1;
		LPDIRECT3DTEXTURE9			m_lpTexture2;

        LPDIRECT3DTEXTURE9			m_lpTargetTexture;
        LPDIRECT3DSURFACE9          m_lpTargetDepthStencilSurface;

        LPDIRECT3DTEXTURE9			m_lpTargetCopyTexture;

        LPDIRECT3DTEXTURE9			m_lpBrushMap;
        LPDIRECT3DTEXTURE9			m_lpRotationBrushMap;
        LPDIRECT3DTEXTURE9			m_lpErrorTexture;
        LPDIRECT3DTEXTURE9			m_lpTerrainHeightBrushTexture;

		LPDIRECT3DVERTEXBUFFER9		m_lpPaintTextureBrushVertex;
		LPDIRECT3DINDEXBUFFER9		m_lpPaintTextureBrushIndex;

        LPDIRECT3DVERTEXBUFFER9		m_lpDataQuadsVertex;

		BYTE						m_nShape;
		WORD						wBrushSize,wlastBeushSize;
		WORD                        wOuterHeightBrushSize,wInsideHeightBrushSize;
		WORD						wPaintTextureBrushSize;
		float						m_fPaintTexturePool[MAX_BRUSH_SIZE1 * MAX_BRUSH_SIZE1];
		float						m_fTerrainHeightPool[MAX_BRUSH_SIZE1 * MAX_BRUSH_SIZE1];

		INT							m_nBrushState;		//0 = PaintTexture 1 = ClearRexture
		std::string					strCurrentBrushMap;
		int							nPaintTexturePower;
		int							nBrushMapRatotionAngle;
		int							nBrushMapContinuity;

		HRESULT OnLostDevice();
		HRESULT OnResetDevice();
		HRESULT	Init();
		HRESULT	UnInit();

		HRESULT	SetPosition(D3DXVECTOR2 Pos,float OuterSize,float InsideSize,float PaintSize,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BOOL bWater = FALSE);
		HRESULT	FillDataPool(LPDIRECT3DTEXTURE9 lpSrcTex,float *pDesData,const WORD BrushSize);
		HRESULT	ReSizeQuads(const int BrushSize);
		HRESULT	ChangeBrushMap(const TCHAR *FileDirc);
		HRESULT	RotationBrushMap(LPDIRECT3DTEXTURE9 lpTexture, int nAngle);
		HRESULT	InitVertexBuffer();
		HRESULT ReSizeHeightData(const WORD BrushSize);

		inline float GetWeight(const int&X,const int &Z) const
		{
			if ((X>=MAX_BRUSH_SIZE1)||(Z>=MAX_BRUSH_SIZE1))
				return 0;
			return m_fPaintTexturePool[Z * MAX_BRUSH_SIZE1 + X];
		}

		inline float GetTerrainWeight(const int&X,const int &Z) const
		{
			if ((X>=MAX_BRUSH_SIZE1)||(Z>=MAX_BRUSH_SIZE1))
				return 0;
			return m_fTerrainHeightPool[Z * MAX_BRUSH_SIZE1 + X];
		}

		_Brush();
		~_Brush();

    private:
        HRESULT	InitBrushTexture();
        HRESULT	UnInitBrushTexture();
	};
	//(by dengzhihui 2006年9月19日 17:06:17
private:
	BOOL m_bInvertOperation;
public:
	void SetInvertOperation(BOOL bSet);
	BOOL GetInvertOperation(){return m_bInvertOperation;}
	//)
protected:
	DWORD m_dwType;   //三种类型：MAP_GRASS,MAP_STONE,MAP_FRUTEX
	//int m_nCurPaintTexture;//用于刷地表的贴图
	D3DXVECTOR3 m_PickRayIntersection;//鼠标选择射线与地面的交点
	KG3DHeightMap* m_pHeightMap;
	BYTE* m_pDetailTextureIndexMap; //类似楼上的m_pHeightMap,用于修改地形的纹理索引—_—。
	int m_nHegihtMapStartIndex_X,m_nHegihtMapStartIndex_Z;//以Section来记算
	int m_nTextureIndexMapStartIndex_X,m_nTextureIndexMapStartIndex_Z;
	int m_nTextureIndexMapWidth,m_nTextureIndexMapHeight;
	BOOL m_bFillHeightMap;
	float  m_fWidthMin;
	float  m_fWidthMax;
	float  m_fHeightMin;
	float  m_fHeightMax;
public:
	_Brush m_Brush;

	DWORD m_dwState;//编辑修改状态
	//DWORD m_dwCurrentState;
	//DWORD m_dwLastState;

	BOOL m_bIsGetAbsoluteHeight;
	float m_fOppositeHeightValue;
	float m_fDiShu;
	//int   m_nBrushShape;
	float m_fAlphaTest;
	float m_fAlpha;
	float m_fHeight;
	
	//std::string          m_CurrentPaintTextureName;

	int    m_nTextureMin;
	int    m_nTextureMax;
	int*   m_pTextureIndex;
	int    m_nNumTextureIndex;
	float  m_fNumPerM2;//每平方米的个数
	int	   m_nAlphaRef;
//	int    m_nStrideX;
//	int    m_nStrideY;
	int    m_nGrassDensity;
	vector<int> m_vecPatternList;

	BYTE m_byPaintTexIndex; // 刷地表贴图用的当前所选贴图的Index
	BOOL m_bAddAutoGrass; //新格式下，刷地表贴图时是否同时刷草的标志
public:

	float GetWidthMax();
	float GetWidthMin();
	float GetHeightMax();
	float GetHeightMin();
	
	HRESULT HandlePickOperation(KG3DScene& sceneCur, D3DXVECTOR3 vSrc, D3DXVECTOR3 vNormalizedDir);

	HRESULT GetTBModifierTextureStride(int* pX,int* pY,DWORD dwType,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT SetTBModifierTextureStride(int  nX,int  nY,DWORD dwType,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);

	HRESULT   RenderBrush();
	HRESULT   SetBrushPosition(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT	  OnBrush(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT	  OnBrushStart(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT	  OnBrushEnd(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	
	HRESULT SetTerrainWaterDefaultHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,float fHeight);
	HRESULT ModifyWaterHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);//修改水面高度
	HRESULT PaintGroundTexture(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);//刷地表贴图
	HRESULT ModifyGroundHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,float fAlpha);//修改地形高度
	HRESULT FlatTerrain(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);//平滑地形
	HRESULT SetGroundHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);//设置地形高度
	HRESULT ComputeTerrainNormal(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);//重新计算地形Normal值
	HRESULT CompositorTerrainTexture(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT GSFFitGround(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT AddTerrainObject(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);//添加草，碎石，灌木
	HRESULT DeleteTerrainObject(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BOOL bIsModifierData);//删除草，碎石，灌木
	HRESULT ClearAllTerrainObject(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);//清除整个地图中所有的草或碎或灌木，（自动生成的草这个函数不会清除）
	HRESULT PaintTextureFromFilterMap(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,LPCSTR pFileName);
	HRESULT ExportTexture(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,LPCSTR pFileName,DWORD dwType);
	HRESULT ClearTexture(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BYTE byTexIndex);
	HRESULT RefreshTerrain(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	virtual HRESULT SetOppositeHeightValue(FLOAT fOppValue);
	virtual HRESULT SetIsGetAbsoluteHeight(BOOL bIsAbs);
	virtual HRESULT SetModifyHeight(float& fHeight);
	virtual float GetModifyHeight();
	virtual DWORD GetTerrainState();
	virtual HRESULT SetTerrainState(DWORD dwState);
	virtual HRESULT GetWidthMax(FLOAT* pValue);
	virtual HRESULT GetWidthMin(FLOAT* pValue);
	virtual HRESULT SetWidthMax(FLOAT fValue);
	virtual HRESULT SetWidthMin(FLOAT fValue);
	virtual HRESULT GetHeightMax(FLOAT* pValue);
	virtual HRESULT GetHeightMin(FLOAT* pValue);
	virtual HRESULT SetHeightMax(FLOAT fValue);
	virtual HRESULT SetHeightMin(FLOAT fValue);
	virtual HRESULT SetNumPerM2(FLOAT fValue);
	virtual HRESULT SetTextureIndexMax(INT nMax);
	virtual HRESULT SetTextureIndexMin(INT nMin);
	virtual float   GetTerrainModifyAlpha();
	virtual HRESULT SetTerrainModifyAlpha(FLOAT fValue);
	virtual DWORD   GetInnerHeightBrushSize();
	virtual HRESULT SetInnerHeightBrushSize(DWORD dwSize);
	virtual DWORD   GetOuterHeightBrushSize();
	virtual HRESULT SetOuterHeightBrushSize(DWORD dwSize);
	virtual HRESULT SetDiShu(FLOAT fPower);
	virtual HRESULT ReSizeHeightData(const WORD BrushSize);
	virtual HRESULT UpdatePatternList(int* pList, const unsigned int uNumPattern);
	virtual int GetPatternIndexBySelection();
	virtual HRESULT SetPaintTexIndex(BYTE byIndex);
	virtual BYTE GetPaintTexIndex();
	virtual HRESULT SetCurrentBrushMap(LPCTSTR pFileName);
	virtual int GetGrassDensity();
	virtual HRESULT SetGrassDensity(int nDensity);
	virtual HRESULT BuildAutoGrass(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BYTE byDetailTexIndex);
	virtual HRESULT DeleteAutoGrass(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BYTE byDetailTexIndex);
	HRESULT DeleteAutoGrassByBrush(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT AddAutoGrassByBrush(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	int GetRandomTextureIndex();
	HRESULT SetTextureIndexInfo(INT* pPointer, INT* pNumIndex);

	HRESULT SetPickRayIntersection(D3DXVECTOR3 vInter);
	D3DXVECTOR3 GetPickRayIntersection();
	HRESULT SetObjectDataToTerrainBlock(int nLevel,POINT Index,DWORD dwDataType,KG3DTerrainBlockGrass* pBlock,UINT uHandleSet,KG3DSceneDataInterface* pDataInterface);
	DWORD   GetType();
	void    SetType(DWORD dwType);
	
	HRESULT FillHeightMap(D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT FillDetailTextureIndexMap(D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT UpdateTerrainBlockHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT UpdateDetailTextureIndex(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	KG3DHeightMap* GetHeightMap(){return m_pHeightMap;}
	BYTE* GetDetailTextureIndexMap(){return m_pDetailTextureIndexMap;}
	int GetHegihtMapStartIndexX(int nLevel);
	int GetHegihtMapStartIndexZ(int nLevel);
	int GetTextureIndexMapWidth(){return m_nTextureIndexMapWidth;}
	int GetTextureIndexMapHeight(){return m_nTextureIndexMapHeight;}
	int GetTextureIndexMapStartIndexX(){return m_nTextureIndexMapStartIndex_X;}
	int GetTextureIndexMapStartIndexZ(){return m_nTextureIndexMapStartIndex_Z;}
protected:
	HRESULT SetTerrainBlockTextureIndex(int nLevel,vector<POINT>*pvecTerrainBlockIndex,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	HRESULT SetTerrainBlockTextureIndex(int nLevel,POINT Index,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
protected:
	

	HRESULT TryAddSection(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,int nX,int nZ);
	HRESULT TryAddRegion(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,int nX,int nZ);

public:
	HRESULT OnAddTerrainBlock(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,list<POINT> listSection);
	HRESULT OnDeleteTerrainBlock(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,list<POINT> listSection);

};

extern KG3DTerrainBlockModifier g_cTerrainBlockModifier;
