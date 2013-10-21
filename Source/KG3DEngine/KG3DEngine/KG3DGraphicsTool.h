#pragma once
#include "../../../Include/kg3dengine/KG3DInterface.h"
#include "Scene/KG3DCamera.h"
#include "IEEditor.h"
#include "KG3DStructs.h"
#include "KG3DDefaultShaderGroup.h"
#include "KG3DShaderHolder.h"

class KG3DTexture;
class KG3DFrustum;
class IEKG3DScene;
class KG3DScene;
class KCell;
class KScene;
class KG3DMesh;
class KG3DRegionInfoManager;
class KG3DGPSBase;
class KG3DSceneShadowMap;

struct KG3DTerrainConverMap;
class KG3DRenderTargetTool;

namespace KG3D_ENVIRONMENT_EFFECT
{
	class KG3DEnvEffFog;
	class KG3DEnvEffLightSet;
	class KG3DEnvEffLight;
};

typedef KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffFog KG3DFog;
typedef KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffLight KG3DLight;
typedef KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffLightSet KG3DLightSet;

class KG3DGraphicsTool : public IEKG3DGraphicsTool//public IKG3DGraphicsTool
{
private:
	struct _ScreenPoint
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex
		DWORD color;        // The vertex color
		FLOAT tu,tv;
	};

    struct _ScreenXYZW
    {
        D3DXVECTOR4 pos;
        DWORD color;
        D3DXVECTOR2 uv;

        enum {FVF = D3DFVF_XYZW | D3DFVF_DIFFUSE | D3DFVF_TEX1};
    };

#define FVF_SCREEN D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1
#define FVF_WORLD D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1

	LPDIRECT3DSURFACE9       m_lpRenderTarget0;
	LPDIRECT3DSURFACE9       m_lpRenderTarget1;
	LPDIRECT3DSURFACE9       m_lpRenderTargetSave;

	//KG3DSceneShadowMap*      m_lpCurrentShadowMap;
	KG3DTerrainConverMap*    m_lpCurrentConverMap;
	LPDIRECT3DSURFACE9       m_lpShareDepthSurface512;
	LPDIRECT3DSURFACE9       m_lpShareColorSurface512;

    LPDIRECT3DVERTEXBUFFER9  m_pShareDynamicVertexBuffer512;
	LPDIRECT3DVERTEXBUFFER9  m_lpLine;
	LPDIRECT3DVERTEXBUFFER9  m_lpHermiteSpline;
	LPDIRECT3DVERTEXBUFFER9  m_lpScreenRect;
    LPDIRECT3DVERTEXBUFFER9  m_lpScreenRectXYZW;
	LPDIRECT3DVERTEXBUFFER9  m_lpBox;
	LPDIRECT3DVERTEXBUFFER9	 m_lpLogicalTerrian;
	LPDIRECT3DVERTEXBUFFER9	 m_lpLogicalTerrianFace;
	LPDIRECT3DVERTEXBUFFER9	 m_lpLogicalItem;
	LPDIRECT3DVERTEXBUFFER9	 m_lpLogicalItemFace;
    LPD3DXLINE               m_lpD3DLine;
	LPDIRECT3DVERTEXBUFFER9  m_lpTexture;// huangjinshou

    KG3DCamera*    m_pCamera;
	KG3DScene*     m_pCurScene;	//当前的Scene，用于方便子类得到上层的Scene的数据

	LPDIRECT3DVERTEXDECLARATION9 m_lpGrassDeclaration;
	///<CTH>
	enum
	{
		//LOCK_FOG = 0x01,
		emFONT_UNAVAILABLE = 0x02, 
	};
	std::bitset<8> m_StateContainer;
	///</CTH>

	LPDIRECT3DVOLUMETEXTURE9     m_lpTextureNoise3D;
	KG3DTexture*                 m_lpTextureSkin;

	KG3DRenderTargetTool* m_lpRenderTargetToolForScaleOutPut;//缩放输出用的渲染工具

public:
	KG3DGraphicsTool(void);
	virtual ~KG3DGraphicsTool(void);

    HRESULT Init();
	HRESULT UnInit();

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();

	void LoadAllShaders();

public:
	HRESULT InitJitter(LPDIRECT3DVOLUMETEXTURE9* lppJitterTexture);

	KG3DRenderTargetTool* GetScaleOutputRenderTool();

	HRESULT DrawScreenText3D(D3DXVECTOR3& vPos,LPSTR pName,D3DXCOLOR vColor);

	LPDIRECT3DTEXTURE9 GetSkinTexture();

    virtual double CubicSplineIntl(double x[], double y[], int n, int k, double t, double s[5]);

	//下面几个RenderTarget系列函数不要随便用，它们主要是给PostRender渲染用的
	HRESULT SetRenderTarget(LPDIRECT3DSURFACE9 pSurface1,LPDIRECT3DSURFACE9 pSurface2);
	HRESULT EnableRenderTarget(int nRenderTarget);
	HRESULT DisableRenderTarget();	//会还原默认的RenderTarget
	HRESULT SaveDefaultRenderTarget();


	LPDIRECT3DVOLUMETEXTURE9 GetShareNoiseTexture();

    bool IsCurrRender2MainWnd();

	//KG3DSceneShadowMap* GetCurrentShadowMap();
	//HRESULT SetCurrentShadowMap(KG3DSceneShadowMap* pMap);

	KG3DTerrainConverMap* GetCurrentConverMap();
	HRESULT SetCurrentConverMap(KG3DTerrainConverMap* pMap);


	LPDIRECT3DSURFACE9 GetShareDepthSurface512();
	LPDIRECT3DSURFACE9 GetShareColorSurface512();

	LPDIRECT3DVERTEXDECLARATION9 GetGrassVertexDec();

	HRESULT DrawArc(D3DXVECTOR3& vCenter,float R,float fAngleStart,float fAngleEnd,float fHeightInc,DWORD dwColor,DWORD dwNumCount);

	HRESULT DrawPoint(D3DXVECTOR3 O,D3DXCOLOR dwColor);
    enuModelPlayAnimationType GetAnimationTypeByFileName(LPCSTR strFileName);
    void DrawFocusRect(const D3DVIEWPORT9& ViewPort, FLOAT weight, DWORD color);

	HRESULT DrawLogicalCells(
		IELogicalSceneConnection* pLogicalScene,
		int nIndex_X,int nIndex_Z,int nXStart,int nZStart,
		int nLogicalStartX,int nLogicalStartZ,
		BOOL bTerrian,
		BOOL bItem,
		BOOL bSelected);
	HRESULT DrawScreenLine(D3DXVECTOR2 *pPointA,D3DXVECTOR2 *pPointB,float fZ, DWORD dwColor1,DWORD dwColor2);


	DWORD GetNowTime();///毫秒
	float GetTinyTime();///毫秒

    DWORD GetTinyTimeOriginal();///毫秒
	
	KG3DRegionInfoManager* GetSceneRegionDWInfoManager();
	KG3DGPSBase*	GetSceneCurGPS();
	HRESULT GetCurCameraData(D3DXVECTOR3* pPos, D3DXVECTOR3* pLookAt);//其它也可以加在这里
	HRESULT	DemandFullControlOfFog(KG3DScene& whichScene);///请求全局控制之后，Environment的KG3DEnvEffFog就不管Fog设置了
	HRESULT	ReleaseFullControlOfFog(KG3DScene& whichScene);
	ULONG IsFogUnderFullControl(KG3DScene& Scene);
	//</CTH>
	HRESULT DrawBackGroudTexture(LPDIRECT3DTEXTURE9 pTexture);

	HRESULT DrawHermiteSpline(D3DXVECTOR3 *pPoint0,D3DXVECTOR3 *pPoint0Tan,
														 D3DXVECTOR3 *pPoint1,D3DXVECTOR3 *pPoint1Tan,
														 DWORD dwColor0, DWORD dwColor1,DWORD dwNumCount);

	HRESULT DrawFrustum(const KG3DFrustum* pFrustum,D3DCOLOR dwColor, DWORD dwPlaneFilter);

	virtual HRESULT DrawLine(D3DXVECTOR3 *pPoint0, D3DXVECTOR3 *pPoint1, DWORD dwColor0, DWORD dwColor1);

    HRESULT DrawFullViewPortRect(DWORD color = 0xffffffff);

	HRESULT DrawScreenRect(const D3DXVECTOR2 *pPointA,
		const D3DXVECTOR2 *pPointB,
		const D3DXVECTOR2 *pPointC,
		const D3DXVECTOR2 *pPointD,
		float fZ,
		DWORD dwColor);

	virtual HRESULT DrawScreenRect(const D3DXVECTOR2 *pPointA,
		const D3DXVECTOR2 *pPointC,
		float fZ, 
		DWORD dwColor);

	HRESULT DrawScreenRect(const D3DXVECTOR2* pA,
		const D3DXVECTOR2* pC,
		float Z,
		float ScaleU,
		float ScaleV,
		D3DCOLOR Diffuse,
		DWORD TextureID);

	HRESULT DrawScreenRect(const D3DXVECTOR2 *pPointA, 
		const D3DXVECTOR2 *pPointC,
		float fZ, 
		DWORD dwColor,
		DWORD dwTextureID);

	HRESULT DrawScreenRect(const D3DXVECTOR2 *pPointA, 
		const D3DXVECTOR2 *pPointC,
		float fZ, 
		DWORD dwColor,
		KG3DTexture* pTexture);

	HRESULT DrawScreenRectUVTrans(const D3DXVECTOR2* pA,
		const D3DXVECTOR2* pC,
		float Z,
		D3DXVECTOR2 vUVScale,
		D3DXVECTOR2 vUVTrans,
		D3DCOLOR Diffuse,
		KG3DTexture* pTexture);

	HRESULT DrawScreenRect(const D3DXVECTOR2 *pPointA, 
		const D3DXVECTOR2 *pPointC,
		float fZ, 
		DWORD dwColor,
		LPDIRECT3DTEXTURE9 pTexture);
	HRESULT DrawPainScreenRect();//用Up来加速，不设状态

	HRESULT DrawGroundGrid(float Weight, DWORD dwColor = 0xffff0000);

	HRESULT DrawAABBox(const AABBOX* pBox,D3DCOLOR dwColor , D3DXMATRIX* pMat,BOOL bZEnable = FALSE);
	HRESULT DrawAABBox(const AABBOX* pBox,D3DCOLOR dwColor);
	HRESULT DrawOBBox(D3DXVECTOR3* BoxPos,D3DCOLOR dwColor);
	HRESULT DrawBoxAtPos(D3DXVECTOR3& vPos, FLOAT fSize, D3DCOLOR color, BOOL bEnableZ = TRUE);///在指定坐标下绘制一个Box

    HRESULT DrawTrapezia(const D3DXVECTOR3& a, 
        const D3DXVECTOR3& b,
        const D3DXVECTOR3& c, 
        const D3DXVECTOR3& d,
        const DWORD dwColor);

    HRESULT DrawRect(D3DXVECTOR3* pUpperLeft,
        D3DXVECTOR3* pUpperRight,
        D3DXVECTOR3* pLowerLeft,	 
        D3DCOLOR Diffuse, 
        IDirect3DTexture9* pTexture,
        FLOAT fCoordDU = 1, 
        FLOAT fCoordDV = 1,
        FLOAT fCoordAU = 0,
        FLOAT fCoordAV = 0
        );//最后一个LowerRight由3个点算出来就好了
	HRESULT DrawText(FLOAT sx, FLOAT sy,FLOAT scx, FLOAT scy,DWORD dwColor,WORD FontHeight,TCHAR* strFont,
					 TCHAR* strText, DWORD dwFlags);

	HRESULT DrawText3D(	D3DXVECTOR3 Position,D3DXVECTOR3 Adir,D3DXVECTOR3 Bdir,
						FLOAT scx, FLOAT scy,
						DWORD dwColor,WORD FontHeight,
						TCHAR* strFont,
						TCHAR* strText, DWORD dwFlags);

    HRESULT SetCamera( KG3DCamera *pCamera );
    KG3DCamera* GetCamera();

    KG3DCamera* GetPrimaryScenePrimaryCamera() const;
    
	HRESULT SetCurScene(KG3DScene* pScene);//可以Set成NULL。每个Editor的FrameMove和Render开头都有用这句话
	IEKG3DScene* GetScene(); 
	KG3DScene* GetCurScene();	//保证在FrameMove和Render链中间用是没有问题的

	//用于方便的得到Scene中的Fog和Light，免去节点路由，也不用include中间节点的头文件
	HRESULT	SetFogParam(KG3DScene& Scene, const KG3DFogParam& fogParam);
	KG3DFogParam GetFogParam(KG3DScene& Scene);
	static HRESULT SetFogDistancePercent(KG3DScene& Scene, FLOAT fPercent);
	KG3DLight& GetSunLight(KG3DScene& Scene);
	KG3DLight& GetPlayerLight(KG3DScene& Scene);
	KG3DLightSet& GetLightSet(KG3DScene& Scene);	//ApplyLight需要这个
	HRESULT		ApplyLight(KG3DScene& Scene);		//用Scene里面的LightSet的Apply，其实如果Environment的Render能够在所有Editor中起作用的话，是不需要这个的
	const KG3DLightParam& GetSunLightParam(KG3DScene& Scene);
	const KG3DLightParam& GetPlayerLightParam(KG3DScene& Scene);
	HRESULT SetSunLightParam(KG3DScene& Scene, const KG3DLightParam& lightParam);
	HRESULT SetPlayerLightParam(KG3DScene& Scene, const KG3DLightParam& lightParam);
	static KG3DLightParam GetDefaultLightParam();
	static KG3DFogParam GetDefaultFogParam();


	//<Font>
	LPD3DXFONT GetFont(int size = 18);
    LPD3DXFONT GetFont2(int size = 10);
	virtual HRESULT IEDrawDebugText(LPCTSTR lpText, DWORD dwTimeStay, DWORD* pdwHandle){return DrawDebugText(lpText, -1, pdwHandle, 0xff00ffff, dwTimeStay);};
	//nTextLength如果是-1的话，自动计算长度；pfHanlde会返回添加的Handle；fTimeStay填0的话自动，；Line的话，如果不指定就按调用的顺序绘制；调用了之后，结果会暂时存起来，直到帧的最后渲染
	HRESULT	DrawDebugText(LPCTSTR lpText, INT nTextLength = -1, DWORD* pdwHandle = NULL, D3DCOLOR Color = 0xFF00FFFF, DWORD dwTimeStay = 0, INT nLine = -1, UINT nWhichFont = 0);	//如果不用fTimeStay参数的话，注意不要每帧重复调用否则只会把屏幕塞满
	HRESULT DrawAllDebugTextsAtLast();//最后渲染
	HRESULT FlushDebugTexts();//最后如果不渲染的话，就需要把所有的Text清空
	virtual HRESULT DeleteDebugText(DWORD dwHandle);
	//</Font>

	HRESULT		DrawSRect(const int Index,const LPDIRECT3DTEXTURE9 Texture);
    
    // -------------- draw left hand coordinate ---------------
    HRESULT DrawLeftHandCoordinate();
    HRESULT DrawCube(
        const D3DXVECTOR3   position,
        const D3DCOLORVALUE color,
        const float side_len = 10.0f
    );

	HRESULT DrawScreenRectNoChange(const D3DXVECTOR2 *pPointA, 
		const D3DXVECTOR2 *pPointC,
		float fZ, 
		DWORD dwColor);

	HRESULT DrawTriangle(D3DXVECTOR3* pPos,D3DXVECTOR3* pNormal,D3DXVECTOR2* pUV);

	HRESULT DrawSphere(D3DXMATRIX* pMat, const D3DCOLORVALUE color, const float radius);
	HRESULT DrawSphereEx(D3DXMATRIX* pMat, const D3DCOLORVALUE color, const float radius,BOOL bZEnable);

	HRESULT DrawCylinder(
		D3DXMATRIX* pMat, const D3DCOLORVALUE color,
		const float radius,
		const float height
		);
	HRESULT DrawCylinderEx(D3DXMATRIX* pMat, const D3DCOLORVALUE color,
		const float radius,
		const float height,
		BOOL bZEnable
		);

	HRESULT DrawCapsule(D3DXMATRIX* pMat, const D3DCOLORVALUE color,
		const float radius,
		const float height
		);
    HRESULT DrawAxis(const D3DXMATRIX& Mat, float fLength);
    // -------------- draw video information -------------------
    HRESULT DrawVideoInfo(int x, int y, DWORD color = 0xFF0000FF);

    HRESULT RecordMovie(LPCTSTR szFilePath, int nIndex);


    HRESULT CreateChessboardTexture(UINT uWidth, UINT uHeight, DWORD dwColor, LPDIRECT3DTEXTURE9* ppTexture);  


    HRESULT DrawCircle(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, FLOAT fRadius, DWORD color);
    HRESULT DrawFan(
        const D3DXVECTOR3& vCenter, 
        const D3DXVECTOR3& vNormal, 
        FLOAT fRadius,
        const D3DXVECTOR3& vBegin, 
        FLOAT fAngle, 
        DWORD color
        ); 

private:
	//<Font>
	enum{font1_size = 12,};
	HRESULT InitFont(int size);
	HRESULT InitFont2(int size);
	//</Font>

	HRESULT InitVertexBuffer();
    


	LPD3DXMESH  m_pMeshCube;
	LPD3DXMESH  m_pMeshSphere;
	LPD3DXMESH  m_pMeshCylinder;

    DWORD  m_dwPrevTime;

	LPDIRECT3DVERTEXBUFFER9		m_lpRect;
	LPDIRECT3DVERTEXBUFFER9		m_lpSRect[2];
	float						m_fRectSize;

	//<Font>
	LPD3DXFONT					m_lpd3dFont;	
    LPD3DXFONT					m_lpd3dFont2;//Fonts主要用于Video录制
	struct DEBUG_TEXT 
	{
        std::tstring stText;
		D3DCOLOR	 Color;

        DWORD        dwHandle;
        DWORD        dwRemoveTime;
	};
	typedef std::vector<DEBUG_TEXT> DEBUG_TEXT_VECTOR;
	DEBUG_TEXT_VECTOR		m_vecDebugText;	//用于最后渲染
	//</Font>

	struct VERTEXWATER
	{
		D3DXVECTOR3 p;
		D3DXVECTOR3 n;
		D3DCOLOR    c;
		float tu,tv;
	};
#define FVFWATER (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

public:
	/*HRESULT FillLogicalCellsInfo(
		D3DXVECTOR3 A,int X,int Z,int& nCellCount,KCell* pCell,IELogicalVertex* pLLine, IELogicalVertex* pLFace, 
		BOOL, BOOL, BOOL, int);*/	//移动到IELogicalSceneConnection//<SO3World Refactor>

public:

	void RenderMeshLogicalBBox(LogicCellBBox* pBox, int nCellInX, int nCellInY, int nCellInZ);
    
public:
	HRESULT Render3DInfomation(LPCTSTR pName,D3DXVECTOR3 vPos,D3DVIEWPORT9& viewport,D3DXMATRIX& matView,D3DXMATRIX& matProj ,D3DXMATRIX& matWorld);

    static void AlignValUp(float &fVal, float Align)
    {
        float t = fVal - (int)(fVal / Align) * Align;
        if (t > 0)
            fVal = (int)(fVal / Align) * Align + Align;
        else if (t < 0)
            fVal = (int)(fVal / Align) * Align;
    }

    static void AlignValDown(float &fVal, float Align)
    {
        float t = fVal - (int)(fVal / Align) * Align;
        if (t > 0)
            fVal = (int)(fVal / Align) * Align;
        else if (t < 0)
            fVal = (int)(fVal / Align) * Align - Align;
    }

    static void AlignBBox(BBox &box, float AlignX, float AlignY, float AlignZ)
    {
        AlignValDown(box.A.x, AlignX);
        AlignValDown(box.A.y, AlignY);
        AlignValDown(box.A.z, AlignZ);

        AlignValUp(box.B.x, AlignX);
        AlignValUp(box.B.y, AlignY);
        AlignValUp(box.B.z, AlignZ);
    }

private:
	unsigned int m_uRenderTargetTexWidth;
	unsigned int m_uRenderTargetTexHeight;
	//<MAIN_OUTPUTWND>
public:
	virtual HRESULT GetSceneMainCamera(IEKG3DCamera** pRet);
	KG3DCamera* GetSceneMainCamera();
	//</MAIN_OUTPUTWND>
	
public:
	LPD3DXEFFECT GetDefaultShader(KG3DDefaultShaderType Type);//尽量不要用这个函数.用g_GetDefaultShaderGroup().GetDefaultShader.好些。现在改成不加引用计数
    LPVOID GetDefaultShaderParamHandleBuffer(KG3DDefaultShaderType Type, size_t *puRetBufferSize);
	virtual void RefreshShader();

	//<Default Shader机制>

public:
	const static DWORD MAX_NUM_BONE_PER_SET = 13;
	const static DWORD MAX_NUM_BONE_PER_SET_SHADER_1 = 26;
	const static DWORD MAX_NUM_BONE_PER_SET_SHADER_2 = 68;
	static int          m_nMaxNumBone;
};

HRESULT KG3DD3DXLoadMeshFromX(LPCSTR pFilename, 
                              DWORD Options, 
                              LPDIRECT3DDEVICE9 pD3DDevice, 
                              LPD3DXBUFFER *ppAdjacency,
                              LPD3DXBUFFER *ppMaterials, 
                              LPD3DXBUFFER *ppEffectInstances, 
                              DWORD *pNumMaterials,
                              LPD3DXMESH *ppMesh);

HRESULT KG3DD3DXCreateEffectFromFile(LPDIRECT3DDEVICE9 pDevice,
                                     LPCTSTR pSrcFile,
                                     const D3DXMACRO *pDefines,
                                     LPD3DXINCLUDE pInclude,
                                     DWORD Flags,
                                     LPD3DXEFFECTPOOL pPool,
                                     LPD3DXEFFECT *ppEffect,
                                     LPD3DXBUFFER *ppCompilationErrors);


HRESULT KG3DD3DXCreateTextureFromFile(LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, LPDIRECT3DTEXTURE9* ppTexture);

HRESULT KG3DD3DXGetImageInfoFromFile(LPCSTR pSrcFile, D3DXIMAGE_INFO *pSrcInfo);

HRESULT KG3DD3DXCreateCubeTextureFromFile(LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, LPDIRECT3DCUBETEXTURE9* ppCubeTexture);

extern KG3DGraphicsTool g_cGraphicsTool;
extern const char* g_strErrorTexture;
