#pragma once

#include "IETools.h"
class KScene;

#define ROAD_FLAT    300

#define WATER_BEGIN	 700
#define WATER_UP     700
#define WATER_DOWN   701
#define WATER_FLAT   702
#define WATER_NOISE  703
#define WATER_SET    704
#define WATER_GET    705
#define WATER_FOLOW    706
#define WATER_ADDCONTOLPOINT 710
#define WATER_END	 711		//要大于或者等于所有WATER_XX的定义

#define TERRIANCELLWIDTH 100.0F
#define TERRIANREGIONCELL 16
#define TERRIANREGIONWIDTH 1600.0F

#define GROUND_BEGIN  100
#define GROUND_UP     100
#define GROUND_DOWN   101
#define GROUND_FLAT   102
#define GROUND_NOISE  103
#define GROUND_SET    104
#define GROUND_GET    105
#define GOURND_NORMAL_UP   110
#define GOURND_NORMAL_DOWN 111
#define GROUND_END	  112	//要大于或者等于所有GROUND_XX的定义

#define MAP_PAINT_GROUND  200
#define MAP_PAINT_COLOR   199

#define MAP_ALPHA_UP      201
#define MAP_ALPHA_DOWN    202
#define MAP_ALPHA_FLAT    203
#define MAP_ALPHA_NOISE   204
#define MAP_ALPHA_SET     205

#define MAP_TERRAINBLOCK_ADD    400
#define MAP_TERRAINBLOCK_DELETE 401

#define MAP_GRASS_UP     500
#define MAP_GRASS_DOWN   501
#define MAP_GRASS_FLAT   502
#define MAP_GRASS_NOISE  503
#define MAP_GRASS_GET    504
#define MAP_GRASS_SET    505
#define MAP_GRASS_DELETE    506
#define MAP_AUTOGRASS_DELETE 507
#define MAP_AUTOGRASS_MANUALADD 508
#define MAP_BARRIER_SET       600
#define MAP_BARRIER_SETHALF   601

#define MAP_CELLLOGICAL 900


#define BRUSH_HEIGHT_BEGION 800
#define BRUSH_HEIGHT_END	801

#define DEFAULT_TERRAIN_SIZE 129

#define MAP_GRASS   0
#define MAP_STONE   1
#define MAP_FRUTEX  2

#define TERRAINTHING_GRASS  1
#define TERRAINTHING_STONE  2
#define TERRAINTHING_FRUTEX 4

enum emKG3DTerrainRenderState
{
	TerrainRS_EnableShowRegionInfo = 0,
	TerrainRS_ShowRegionInfoID = 1,
};



interface IEKG3DSceneSceneEditor;

typedef LRESULT (*ProgressCallBack)(WPARAM wParam, LPARAM lParam);

class IEKG3DTerrain : public IEKG3DConnectionPoint
{
public:
	//////////////////////////////////////////////////////////////////////////
	//笔刷贴图
	virtual HRESULT GetNumBrushMap(INT* pValue) = 0;
	virtual HRESULT GetBrushMap(INT nIndex,LPTSTR pFileName) = 0;
	virtual HRESULT AddBrushMap(LPCTSTR pFileName) = 0;
	virtual HRESULT ChangeBrushMap(INT nIndex,LPCTSTR pFileName) = 0;
	virtual HRESULT DeleteBrushMap(LPCTSTR pFileName) = 0;

	//////////////////////////////////////////////////////////////////////////
	//蒙板贴图
	virtual HRESULT GetEnableShowMaskTexture(BOOL* pValue) = 0;
	virtual HRESULT SetEnableShowMaskTexture(BOOL bEnable) = 0;
	virtual HRESULT GetEnableShowMaskTextureOnGround(BOOL* pValue) = 0;
	virtual HRESULT SetEnableShowMaskTextureOnGround(BOOL bEnable) = 0;

	virtual HRESULT LoadMaskTexture(LPCTSTR pFileName) = 0;
	virtual HRESULT RefreshMaskTexture() = 0;

	virtual HRESULT GetEnableUseSmallMap(BOOL* pValue) = 0;
	virtual HRESULT SetEnableUseSmallMap(BOOL bEnable) = 0;

	virtual HRESULT ComputeSmallMapTexture() = 0;

	virtual HRESULT RefreshGrasses() = 0;
	virtual HRESULT UpdataGrassTextureInformation(LPCTSTR pTextureName,INT nWdithCount,INT nHeightCount) = 0;

	//////////////////////////////////////////////////////////////////////////
	//动态加载

	//////////////////////////////////////////////////////////////////////////
	//地表贴图
	virtual HRESULT SetGroundTexture(INT nLayer,INT nIndex,LPCTSTR pFileName) = 0;
	virtual HRESULT GetGroundTexture(INT nLayer,INT nIndex,LPTSTR pFileName) = 0;
	virtual HRESULT GetGroundTextureCoordIndex(INT nLayer,INT nIndex,INT* pValue) = 0;
	virtual HRESULT SetGroundTextureCoordIndex(INT nLayer,INT nIndex,INT nCoord) = 0;
	virtual HRESULT DeleteGroundTexture(INT nLayer,INT nIndex) = 0;
	virtual HRESULT SetGroundTextureUV(INT nLayer,INT nIndex,FLOAT fXZAngle,FLOAT fYAngle,FLOAT m_fScaleU,FLOAT m_fScaleV) = 0;
	virtual HRESULT GetGroundTextureUV(INT nLayer,INT nIndex,FLOAT* pfXZAngle,FLOAT* pfYAngle,FLOAT* pfScaleU,FLOAT* pfScaleV) = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT SetRenderKind(DWORD kind) = 0;
	//////////////////////////////////////////////////////////////////////////
	//草
	virtual HRESULT GetGrassWidthMax(FLOAT* pValue) = 0;
	virtual HRESULT GetGrassWidthMin(FLOAT* pValue) = 0;
	virtual HRESULT SetGrassWidthMax(FLOAT fValue) = 0;
	virtual HRESULT SetGrassWidthMin(FLOAT fValue) = 0;
	virtual HRESULT GetGrassHeightMax(FLOAT* pValue) = 0;
	virtual HRESULT GetGrassHeightMin(FLOAT* pValue) = 0;
	virtual HRESULT SetGrassHeightMax(FLOAT fValue) = 0;
	virtual HRESULT SetGrassHeightMin(FLOAT fValue) = 0;

	virtual HRESULT SetGrassTextureWidthCount(INT nCount) = 0;
	virtual HRESULT SetGrassTextureHeightCount(INT nCount) = 0;
	virtual HRESULT SetGrassTextureIndexMax(INT nMax) = 0;
	virtual HRESULT SetGrassTextureIndexMin(INT nMin) = 0;
	virtual HRESULT SetGrassNumPerM2(FLOAT fValue) = 0;
	virtual HRESULT SetGrassAlphaRef(INT nAlpha) = 0;

	virtual HRESULT GetGrassTextureWidthCount(INT* pnCount) = 0;
	virtual HRESULT GetGrassTextureHeightCount(INT* pnCount) = 0;
	virtual HRESULT GetGrassTextureIndexMax(INT* pnMax) = 0;
	virtual HRESULT GetGrassTextureIndexMin(INT* pnMin) = 0;
	virtual HRESULT GetGrassNumPerM2(FLOAT* pfValue) = 0;
	virtual HRESULT GetGrassAlphaRef(INT* pnAlpha) = 0;

	virtual HRESULT GetGrassTextureIndexPointer(INT*** pppPoint,INT* pNumIndex) = 0;
	virtual HRESULT SetGrassTextureIndexInfo(INT* pPointer, INT* pNumIndex) = 0;
	virtual HRESULT UpdatePatternList(INT* pList,INT nCount) = 0;

	virtual HRESULT SetGrassTexture(LPCTSTR pFileName,INT nWidth,INT nHeight) = 0;
	virtual HRESULT GrassFitGround() = 0;
	virtual HRESULT ComputeGrassColor() = 0;

    //
    virtual int AutoFillGrass(int nLayer, int nTexIndex, IEKG3DSceneSceneEditor* pSceneEditor, ProgressCallBack CallBack) = 0;

        // ----------- sets -----------
    virtual HRESULT SetAFGAlphaRangeMin(int nAlpha)     = 0;
    virtual HRESULT SetAFGAlphaRangeMax(int nAlpha)     = 0;

    virtual HRESULT SetAFGHeightRangeMin(float fHeight) = 0;
    virtual HRESULT SetAFGHeightRangeMax(float fHeight) = 0;

    virtual HRESULT SetAFGNorRangeMin(float fAngle)     = 0;
    virtual HRESULT SetAFGNorRangeMax(float fAngle)     = 0;

    virtual HRESULT EnableTopperAffact(int nEnable)     = 0;
    virtual HRESULT EnableAlphaRand(int nEnable)        = 0;
    virtual HRESULT EnableOnlyClear(int nEnable)        = 0;
    virtual HRESULT EnableClearAll(int nEnable)         = 0;
    virtual HRESULT EnableIgoreWater(int nEnable)       = 0;

        // ----------- gets ------------
    virtual int GetAFGAlphaRangeMin()    = 0;
    virtual int GetAFGAlphaRangeMax()    = 0;

    virtual float GetAFGHeightRangeMin() = 0;
    virtual float GetAFGHeightRangeMax() = 0;

    virtual float GetAFGNorRangeMin()    = 0;
    virtual float GetAFGNorRangeMax()    = 0;

    virtual int IsEnableTopperAffact()   = 0;
    virtual int IsEnableAlphaRand()      = 0;
    virtual int IsEnableOnlyClear()      = 0;
    virtual int IsEnableClearAll()       = 0;
    virtual int IsIgoreWater()           = 0;
    
    //


	//virtual HRESULT RandomGrass() = 0;

	//////////////////////////////////////////////////////////////////////////
	//线框显示
	virtual HRESULT GetEnableWireFrameMode(BOOL* pValue) = 0;
	virtual HRESULT SetEnableWireFrameMode(BOOL bEnable) = 0;

	//////////////////////////////////////////////////////////////////////////
	//场景光照图
	virtual HRESULT GetEnableGroundLightMap(BOOL* pValue) = 0;
	virtual HRESULT SetEnableGroundLightMap(BOOL bEnable) = 0;

	//////////////////////////////////////////////////////////////////////////
	//水面
	virtual HRESULT UpdateWater(DWORD dwOption) = 0;
	virtual HRESULT SetWaterDefaultHeight(float fHeight) = 0;

	virtual HRESULT GetWaterFxEdgeWidth(FLOAT** ppValue) = 0;
	virtual HRESULT GetWaterFxShinness(FLOAT** ppValue) = 0;
	virtual HRESULT GetWaterFxBumpReflectFactor(FLOAT** ppValue) = 0;
	virtual HRESULT GetWaterFxBumpRefractFactor(FLOAT** ppValue) = 0;
	virtual HRESULT GetWaterFxTexCoordVel1(FLOAT** ppValue) = 0;
	virtual HRESULT GetWaterFxTexCoordVel2(FLOAT** ppValue) = 0;
	virtual HRESULT GetWaterFxTexCoordScale1(FLOAT** ppValue) = 0;
	virtual HRESULT GetWaterFxTexCoordScale2(FLOAT** ppValue) = 0;

	virtual HRESULT GetWaterFxWaterColor(D3DCOLORVALUE** ppValue) = 0;
	virtual HRESULT GetWaterFxSpecularColor(D3DCOLORVALUE** ppValue) = 0;
	virtual HRESULT GetWaterFxReflectColor(D3DCOLORVALUE** ppValue) = 0;
	virtual HRESULT GetWaterFxRefractColor(D3DCOLORVALUE** ppValue) = 0;

	virtual HRESULT GetWaterFxBump1(LPTSTR pFileName) = 0;
	virtual HRESULT GetWaterFxBump2(LPTSTR pFileName) = 0;
	virtual HRESULT GetWaterFxSurfaceDetail(LPTSTR pFileName) = 0;
	virtual HRESULT SetWaterFxBump1(LPCTSTR pFileName) = 0;
	virtual HRESULT SetWaterFxBump2(LPCTSTR pFileName) = 0;
	virtual HRESULT SetWaterFxSurfaceDetail(LPCTSTR pFileName) = 0;

	virtual HRESULT WaterEffectSaveToFile(const char* strFileName) = 0;
	virtual HRESULT WaterEffectLoadFromFile(const char* strFileName) = 0;
	virtual HRESULT WaterEffectRefresh() = 0;

	//////////////////////////////////////////////////////////////////////////
	//
	virtual HRESULT GetMinTerrainHeightValue(INT* pValue) = 0;
	virtual HRESULT GetMaxTerrainHeightValue(INT* pValue) = 0;
	virtual HRESULT SetMinTerrainHeightValue(INT nMinHeight) = 0;
	virtual HRESULT SetMaxTerrainHeightValue(INT nMaxHeight) = 0;

	virtual HRESULT GetTerrainHeightScale(FLOAT* pValue) = 0;
	virtual HRESULT SetTerrainHeightScale(FLOAT HeightScale) = 0;

	virtual HRESULT GetTerrainWidth(INT* pValue) = 0;
	virtual HRESULT GetTerrainHeight(INT* pValue) = 0;
	virtual HRESULT GetBmpFileData(unsigned char ** pValue) = 0;

	virtual HRESULT SmoothMapHeight() = 0;//平滑整个地图的高度
	virtual HRESULT SmoothMapHeight(INT xStart,INT xEnd,INT zStart,INT zEnd,FLOAT K) = 0;

	virtual HRESULT LoadDataFromHeightMap(LPCTSTR FileDir) = 0;
    virtual HRESULT CreateTerrainFromPngFile(LPCTSTR pszFileName) = 0;
    virtual BOOL IsPngFile() = 0;
	virtual HRESULT UpdateMapHeightFromByteData() = 0;
	virtual HRESULT UpDateRegionHeightMap(BOOL bToRegion) = 0;

	virtual HRESULT GetHeightMapFileName(LPTSTR pFileName) = 0;
	virtual HRESULT SetHeightMapFileName(LPCTSTR pFileName) = 0;

	virtual HRESULT InitShader() = 0;

	virtual HRESULT GetGroundHeight(float& fHeight,float x,float y) = 0;
	virtual HRESULT GetGroundNormal(D3DXVECTOR3& vec3Normal, float fX, float fZ) = 0;
	virtual HRESULT GetWaterHeight(float& fHeight,float x,float y) = 0;


	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT GetTextureOpertaorLayter(INT* pValue) = 0;
	virtual HRESULT SetTextureOpertaorLayter(INT nLayer) = 0;
	virtual HRESULT GetBrushState(INT* pValue) = 0;
	virtual HRESULT SetBrushState(INT nState) = 0;

	virtual HRESULT GetCurrentGroundTexture(LPTSTR pFileName) = 0;
	virtual HRESULT SetCurrentGroundTexture(LPCTSTR pFileName) = 0;

	virtual HRESULT GetCurrentBrushMap(LPTSTR pFileName) = 0;
	virtual HRESULT SetCurrentBrushMap(LPCTSTR pFileName) = 0;

	virtual HRESULT GetTerrainState(DWORD* pValue) = 0;
	virtual HRESULT SetTerrainState(DWORD dwState) = 0;

	virtual float GetTerrainModifyAlpha() = 0;
	virtual HRESULT SetTerrainModifyAlpha(FLOAT fValue) = 0;

	virtual float GetTerrainModifyHeight() = 0;
	virtual HRESULT SetTerrainModifyHeight(FLOAT fValue) = 0;

	virtual HRESULT GetIsNewTerrain(BOOL* pValue) = 0;

	//////////////////////////////////////////////////////////////////////////
	//水花
	virtual HRESULT ComputeSpray() = 0;

	virtual HRESULT GetPickRayIntersection(D3DXVECTOR3* pInter) = 0;

	//virtual HRESULT GetIsUseOldLightMap(BOOL* pValue) = 0;
	//virtual HRESULT SetIsUseOldLightMap(BOOL bIsOld) = 0;

	virtual HRESULT SetOppositeHeightValue(FLOAT fOppValue) = 0;
	virtual HRESULT SetIsGetAbsoluteHeight(BOOL bIsAbs) = 0;
	virtual HRESULT SetDiShu(FLOAT fPower) = 0;

	virtual HRESULT CalculateVertexNormal(INT nXStart,INT nXEnd,INT nZStart,INT nZEnd) = 0;
	virtual HRESULT CalculateTriangleNormal(INT nXStart,INT nXEnd,INT nZStart,INT nZEnd) = 0;
	virtual HRESULT UpdateRegionVertexNormal(INT nXStart,INT nXEnd,INT nZStart,INT nZEnd) = 0;

	//virtual HRESULT SetRegionUpdateArea() = 0;
	virtual HRESULT SaveHeightMap(LPCTSTR FileName) = 0;
    virtual HRESULT SavePngFile(const char *szOutFile) = 0;
	virtual HRESULT ReLoadHeightMap() = 0;

	virtual HRESULT SetTgaButtonTextureDic(BOOL bGroundTexture,BOOL bAdd,LPCTSTR pTextureDic,INT nLayerIndex) = 0;
	virtual HRESULT ClearGroundTextureByPath(LPCTSTR pTexturePath,INT nLayer) = 0;
	//virtual HRESULT SetGroundTextureTexCoordIndex(LPCTSTR pTexturePath,INT nIndex,INT nLayer) = 0;
	virtual HRESULT ClearSmallTextureLayer(INT nSmallestValue) = 0;

	virtual HRESULT GetNpcPosition(FLOAT* px,FLOAT* py,FLOAT* pz) = 0;

	virtual HRESULT SetVisibleLayter(BOOL bVisibleLayter[]) = 0;
	virtual HRESULT SetIsSelectAllRegion(BOOL bSelectAllRegion) = 0;
	virtual HRESULT SetIsDrawRegionEdge(BOOL bDrawRegionEdge) = 0;

	virtual HRESULT RotationBrushMap(INT nAngle) = 0;
	virtual HRESULT SetPaintCurrentTextureId(INT nLayerIndex,INT Id) = 0;
	virtual HRESULT SetPaintGroundState() = 0;

	virtual DWORD GetInnerHeightBrushSize() = 0;
	virtual DWORD GetOuterHeightBrushSize() = 0;
	virtual HRESULT SetInnerHeightBrushSize(DWORD dwSize) = 0;
	virtual HRESULT SetOuterHeightBrushSize(DWORD dwSize) = 0;
	virtual HRESULT ReSizeHeightData(const WORD BrushSize) = 0;

	virtual HRESULT SetPaintTextureBrushSize(INT nSize) = 0;
	virtual HRESULT GetPaintTextureBrushSize(INT* pValue) = 0;
	virtual HRESULT SetPaintTextureBrushPower(INT nPower) = 0;
	virtual HRESULT GetPaintTextureBrushPower(INT* pValue) = 0;

	virtual HRESULT SetIsRandRatotionBrush(BOOL bRand) = 0;
	virtual HRESULT SetBrushMapRatotionAngle(INT nAngle) = 0;
	virtual HRESULT SetBrushMapContinuity(INT nMt) = 0;
	virtual HRESULT SetCurrentPaintTextureId(INT nId) = 0;
	virtual HRESULT SetSmallTextureValue(INT nValue) = 0;


	//////////////////////////////////////////////////////////////////////////
	//
	virtual float GetBBoxLength() = 0;
	virtual float GetBBoxWidth() = 0;
	virtual float GetBBoxStartX() = 0;
	virtual float GetBBoxStartZ() = 0;

	virtual HRESULT GetIntersectionPoint(D3DXVECTOR3 *pPos) = 0;
	virtual HRESULT GetBBox(LPVOID* ppvBBox) = 0;

	///允许高度图绘制
	virtual HRESULT EnableHeightMap(BOOL bSetOrGet, BOOL* pEnable) = 0;

	//////////////////////////////////////////////////////////////////////////
	//改变地图大小和尺寸
	virtual HRESULT MoveTerrain(int nXStart,int nZStart) = 0;

	//改变渲染状态
	virtual HRESULT SetRenderState(DWORD_PTR StateType, DWORD_PTR Param) = 0;
	virtual HRESULT GetRenderState(DWORD_PTR StateType, DWORD_PTR* pParam) = 0;

};
#ifdef KG3D_AUTO_TEST 
struct KGEntityData
{
	UINT uEntityInfoHandle;

	D3DXVECTOR3    m_Scaling;
	D3DXQUATERNION m_Rotation;
	D3DXVECTOR3    m_Translation;

	/* world space position */
	D3DXVECTOR3    m_vWorldPos;

	/* world space bbox */
	D3DXVECTOR3    m_vBox_A;//包围盒
	D3DXVECTOR3    m_vBox_B;
};

struct SceneBlockIndex
{
	UINT  nLevel;
	POINT Index;
};
#endif
interface IEKG3DTerrainRoadMgr;
interface IEKG3DTerrainRiverMgr;
class IEKG3DSceneOutDoorSpaceMgr 
{
public:
	virtual HRESULT FindPattern(LPCSTR strFileName, INT *pnRetIndex,DWORD dwType)  = 0;
	virtual HRESULT SetGrassSetTexture(LPCSTR strFileName,DWORD dwType) = 0;
	virtual HRESULT SetCurGrassSet(DWORD dwType) = 0;
	virtual std::string GetGrassSetTextureName(DWORD dwType) = 0;
	//////////////////////////////////////////////////////////////////////////


	virtual HRESULT AddDetailTextureToMaterialMgr(BYTE& OutIndex,LPCTSTR pFileName)=0;
	virtual HRESULT DelDetailTextureFromMaterialMgr(BYTE byIndex) = 0;
	virtual HRESULT GetAllDetailTextureFromMaterialMgr(map<BYTE,std::string>* pmapDetailTex) = 0;
	virtual HRESULT ChangeDetailTexture(BYTE byIndex,LPCTSTR pFileName) = 0;
	virtual	HRESULT SetDetailTextureCoordIndex(INT nIndex,BYTE byCoord) = 0;
	virtual HRESULT GetDetailTextureCoordIndex(INT nIndex,BYTE& byCoord) = 0;
	virtual HRESULT GetDetailTextureUV(INT nIndex,FLOAT* pfScaleU,FLOAT* pfScaleV,FLOAT* pfEnv,FLOAT* pfSpecular,FLOAT* pfEmissive) = 0;
	virtual HRESULT SetDetailTextureUV(INT nIndex,FLOAT m_fScaleU,FLOAT m_fScaleV,FLOAT   fEnv,FLOAT   fSpecular,FLOAT   fEmissive) = 0;
	virtual HRESULT GetDetailTextureGrassData(BYTE byIndex,BOOL* pbGrass,BYTE* pbyGrassTexIndex,BYTE* pbyPatternIndex)=0;
	virtual HRESULT SetDetailTextureGrassData(BYTE byIndex,BOOL* pbGrass,BYTE* pbyGrassTexIndex,BYTE* pbyPatternIndex,BOOL bSave = FALSE)=0;
	//道路的编辑和修改
	virtual HRESULT GetTerrainRoadMgr(IEKG3DTerrainRoadMgr** ppRoadMgr) = 0;
	virtual HRESULT GetTerrainRiverMgr(IEKG3DTerrainRiverMgr** ppRiverMgr)=0;

#ifdef KG3D_AUTO_TEST 
	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption) = 0;
	virtual HRESULT GetAllEntityInfoHandle(vector<UINT>* vecHandle) = 0;
	virtual HRESULT GetEntityInfoformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen) = 0;

	virtual HRESULT GetAllTerrainInfoHandle(vector<UINT>* vecHandle) = 0;
	virtual HRESULT GetTerrainInfoformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData) = 0;

	virtual HRESULT GetAllSceneBlock(vector<SceneBlockIndex>* vecSceneBlockIndex) = 0;
	virtual HRESULT GetSceneBlockData(int nLevel,POINT Index,DWORD& dwLenght,PVOID pData) = 0;//获取地形数据，调用者自己负责该数据的创建和删除，该函数背部完成数据复制

	virtual HRESULT GetEntityData(map<UINT,KGEntityData>* mapEntityData) = 0;
#endif
	
};