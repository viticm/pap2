#include "StdAfx.h"
#include ".\kg3denginemanager.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DEngineOption.h"
#include "RInterface.h"
#include "KG3DSceneTable.h"
#include "KG3DScene.h"

#include "KG3DFileLoader.h"
#include "KG3DMeshTable.h"
#include "KG3DModelTable.h"
#include "kg3dcliptable.h"
#include "kg3dmaterialtable.h"
#include "kg3drepresentobjecttable.h"
#include "KG3DSFXTrack.h"
#include "KG3DModelPointLight.h"
#include "KG3DAnimationSFXTag.h"
#include ".\kg3dkeyframedatabase.h"
#include "KG3DGraphicsTool.h"


#include "IPPVideo\IPPBase.h"
#include "IPPVideo\IPPCodec.h"

#include "KG3DTextureTable.h"
#include "kg3dgrasspatterncontainer.h"
#include "kg3drepresentobject.h"
#include "kg3drepresentobjectset.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DBip.h"
#include "KG3DRTTIType.h"

#include "PhysiscSystem//KG3DPhysiscManager.h"
#include "NxPhysics.h"
#include "MathTool.h"
#include "KG3DRegionInfoManager.h"
#include "..\..\..\Include\IKG3DSoundShell.h"

#include "PhysiscSystem\KG3DPhysiscScene.h"
#include "KG3DModelSpeedTree.h"
#pragma warning(default:4244)

#include "KG3DSFXProjectionTextureMgr.h"

#include "IEKG3DSceneCamera.h"
#include "KG3DFaceMotionSettings.h"

#include "KG3DWaterEffect.h"
#include "KG3DRepresentObjectBuilder.h"
#include "KG3DRecorder.h"
#include "KG3DCollocator.h"
#include "KG3DTemplates.h"

#include "KG3DTerrainBlock.h"
#include "KG3DRecorder.h"

#include "KG3DRepresentObjectSoundBall.h"
#include "RecPlayer.h"
#include "KG3DTerrainBlockModifier.h"

#include "KG3DSystemEventListener.h"
#include "kg3dcaption.h"
#include "KG3DEngineInterfaceGetter.h"
#include "KG3DEngineCommandExecutor.h"
#include "KG3DStateInitializer.h"
#include "KG3DTime.h"
#include "kg3dselector.h"
#include "KG3DInstanceBillboard.h"
#include "KG3DPostRenderEffectManager.h"
//////////////////////////////////////////////////////////////////////////
#include "KG3DSceneModelEditor.h"
#include "KG3DStaticModelRender.h"
#include "KG3DModelShadowRenderer.h"
#include "KG3DAnimationTagContainer.h"
#include "KG3DMultiThreadLoader.h"
#include "KG3DTerrainRepresentInfo.h"
#include "KG3DAnimationSoundTagInfo.h"
#include "KG3DProfile.h"

#include "KG3DOutputWindowTable.h"
#include "KG3DResourceDelayReleaseMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//这些东西记得放在EngineManager之前
//---------------------------------------------
KG3DEngineOption g_cEngineOption;			//客户端使用的选项
KG3DEngineEditorOption g_eEngineOption;		//编辑器使用的选项
//---------------------------------------------

KG3DEngineManager          g_cEngineManager;         // 引擎管理器
KG3DFileFinder*             g_pFileFinder = &g_cEngineManager.m_FileFinder;
RecordResourceManager g_cRecordRM;
RecordOption			g_RecordOption;
DWORD                   g_dwRenderCount;

DWORD                   g_dwMainThreadID;//渲染线程ID
BOOL                    g_bShow3DEngineDebugInfo = FALSE;
BOOL                    g_bMultiThreadLoadAnimation = FALSE;
BOOL                    g_b3DVision = FALSE;
int					    g_nPackFile = 0;
BOOL					g_bEnablePointLight = FALSE;
static KG3DLogicalConstances	g_logicalConstants;//逻辑的东西放这里
const KG3DLogicalConstances& g_GetLogicalConstants()
{
	return g_logicalConstants;
}

void (*g_AddAlertMessage)(const char[]) = NULL;


IKG3DEngineManager* g_GetEngineManager()
{
	return &g_cEngineManager;
}

extern TKG3DResourcePool<KG3D_PointLight_Info>g_cPointLightInfoPool;

HWND       g_hBaseWnd;//基础的窗口
HWND       g_hRenderWnd;//默认的渲染窗口

BOOL     g_Mouse_L;//鼠标左键状态//<PROCESS_MESSAGE>
BOOL     g_Mouse_M;//鼠标中键状态
BOOL     g_Mouse_R;//鼠标右键状态     
BYTE     g_KeyState[256];//键盘状态
int          g_nMouseMoveKey;
int          g_nMouseMoveX;
int          g_nMouseMoveY;

TCHAR        g_Def_WorkDirectory[256];//工作的目录
TCHAR        g_Def_AppDirectory[256];//执行文件目录

TCHAR        g_szSceneRunnerModelName[MAX_PATH];
TCHAR        g_szSceneRunnerMtlName[MAX_PATH];
TCHAR        g_szSceneRunnerAniName[MAX_PATH];
TCHAR        g_szSceneRunnerFaceModelName[MAX_PATH];
TCHAR        g_szSceneRunnerFaceMtlName[MAX_PATH];


BOOL g_bClient;//是否处于客户端
BOOL g_bRePlay;//是否处于回放中
BOOL g_bBatchGrassRender = FALSE;
BOOL g_bAutoReloadTexture;//自动更新贴图文件
BOOL g_bFullScreen = FALSE;
BOOL g_bIsRecord = TRUE;
BOOL g_bDesign = FALSE;
int g_bUseNewSelection;//是否使用新的选择方式

//下面三个是动画内存控制相关的
int g_nAniEnableInterpolate = true;
int g_nInterpolateFPS = 25;
int g_nExpireTime = 5;
int g_nAniLimitMemSize = 50;

//BOOL g_bImportOldScene = FALSE;

BOOL g_bUseOtherDiretory = FALSE;//使用非EXE的工作路径

BOOL g_bCanCreateTerrainLess128x128 = FALSE;//add by zhangzhixiong(2009.11.17)
extern BOOL g_bUseVIM3D;

int g_nEnableProfile = false;

KG3DOutputWindowTable g_OutputWndTable;

extern BOOL g_bEnableScaleOutput;//是否使用缩放输出，即渲染使用1024然后拉伸输出到屏幕
extern int  g_nScaleOutputSize;
extern BOOL g_bScaleOutputSmooth;
extern int g_nMDLRenderLimit;

//////////////////////////////////////////////////////////////////////////
//多线程贴图加载
extern HRESULT InitMultiThreadTextureRead();
extern HRESULT UnInitMultiThreadTextureRead();
extern HRESULT InitModelProcessTread();
extern HRESULT UnInitModelProcessTread();

extern HRESULT InitMultiThreadCull();
extern HRESULT UnInitMultiThreadCull();

class KG3DDeviceCallback : public IKG3DeviceCallback
{
public:
    virtual bool IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed);
    virtual bool ModifyDeviceSettings(KG3DDeviceSettings const* pSettings, D3DCAPS9 const* pCaps);
    virtual HRESULT OnCreateDevice(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC const* pBackBufferSurfaceDesc, D3DPRESENT_PARAMETERS const* pPresentParameters);
    virtual HRESULT OnDestroyDevice();
    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC const* pBackBufferSurfaceDesc, D3DPRESENT_PARAMETERS const* pPresentParameters);
};

static KG3DDeviceCallback s_DeviceCallback;

//////////////////////////////////////////////////////////////////////////
extern HRESULT InitOutDoorSpaceMgrRegister();
extern HRESULT UnInitOutDoorSpaceMgrRegister();
//////////////////////////////////////////////////////////////////////////
extern HRESULT InitModelVBFillList();
extern HRESULT UnInitModelVBFillList();

void ProcessMultiThreadLoad()
{

}
////////////////////////////////////////////////////////////////////////////////
// g_fMemoryCoefficient used to control the memory use, the resource pool size 
// normally set to BASE_SIZE * g_fMemoryCoefficient. the BASE_SIZE is resource use
// when g_fMemoryCoefficient == 1.0
float g_fMemoryCoefficient = -1.0f;

////////////////////////////////////////////////////////////////////////////////
static HRESULT _GetMemoryfMemoryCoefficient(float *pfRetMemoryCoefficient)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    float fMemoryCoefficient = 0.0f;
    unsigned uAvailPhysMem_MB = 0;
    unsigned uUsedPhysMem_MB  = 0;
    unsigned uPhysMemWeCanUse_MB = 0;

    MEMORYSTATUSEX MemStatusEx;
    MemStatusEx.dwLength = sizeof(MemStatusEx);
    nRetCode = ::GlobalMemoryStatusEx(&MemStatusEx);
    KGLOG_PROCESS_ERROR(nRetCode);

    uAvailPhysMem_MB = (unsigned)(MemStatusEx.ullAvailPhys / 1024 / 1024);
    uUsedPhysMem_MB  = (unsigned)((MemStatusEx.ullTotalPhys - MemStatusEx.ullAvailPhys) / 1024 / 1024);

    uPhysMemWeCanUse_MB = uAvailPhysMem_MB + uUsedPhysMem_MB / 4; // OS will give more memory from another process is we need more

    if (uPhysMemWeCanUse_MB <= 200)
        fMemoryCoefficient = 0.0f;
    else if (uPhysMemWeCanUse_MB <= 1700)
        fMemoryCoefficient = (float)(uPhysMemWeCanUse_MB - 200)/ 300.0f;
    else
        fMemoryCoefficient = 5.0f;
    
    *pfRetMemoryCoefficient = fMemoryCoefficient;
    hrResult = S_OK;
Exit0:
    return hrResult;
}





////////////////////////////////////////////////////////////////////////////////


void TimeControl::FrameMove(BOOL bSaveToFile)
{
    m_fSpeed += (m_fSpeedDest - m_fSpeed)*m_fSpeedIncrease;

    if(bSaveToFile)
    {
        m_dwLastTime = m_dwNowTime;
        m_dwNowTime += m_dwInterval;//33.33F/3.0F; 
    }
    else
    {
        m_dwLastTime = m_dwNowTime;

        DWORD dwAppTime = timeGetTime();
        DWORD dwElapse  = dwAppTime - m_dwLastAppTime;
        m_dwLastAppTime = dwAppTime;
        m_dwNowTime += (DWORD)(dwElapse * m_fScaling);
        m_fScaling = 1.f;

    }
    m_FrameCount++;
}

TimeControl::TimeControl()
{
    m_fSpeedBack = 0;
    m_dwNowTime = 0;//timeGetTime();
    ///第一帧和第二帧之间会差很大
    m_dwLastTime  = 0;
    m_fSpeed     = 1.0f;
    m_fSpeedDest = 1.0f;
    m_dwTimeGetTimeLast = 0;
    m_fSpeedIncrease = 0.1f;
    m_dwInterval = 10;
    m_FrameCount = 0;
    m_fScaling = 1.f;
    m_dwLastAppTime = timeGetTime();
}

/*
写这个函数要注意：EngineManager是全局变量，其初始化函数不能依赖其它全局类哦。cpp间的全局变量的
初始化顺序是不一致的。使用其它的dll，创建一些类，在这里都是不保险的，C++构造函数出问题会引发超
多问题，还是能免则免。Meyer's singleton可以解决一些问题，但是最好还是放Init里面。
*/
KG3DEngineManager::KG3DEngineManager(void)
	:m_RenderOption(static_cast<DWORD>(RENDER_ALL))
    ,m_FrameMoveOption(static_cast<DWORD>(FRAMEMOVE_ALL))
	,m_pTimeClock(NULL)
{	
	m_bRenderFocusSceneOnly = FALSE;
	m_lpFocusScene = NULL;
	m_pSoundFocusScene = NULL;
	g_Mouse_L = FALSE;
	g_Mouse_M = FALSE;
	g_Mouse_R = FALSE;

	g_hBaseWnd = NULL;
	g_hRenderWnd = NULL;

	g_nMouseMoveX = 0;
	g_nMouseMoveY = 0;
	g_nMouseMoveKey = 0;

	ZeroMemory(&g_cEngineOption, sizeof(g_cEngineOption));
	ZeroMemory(&g_eEngineOption, sizeof(g_eEngineOption));

	//防止读取打开config文件错误，故先手工设定默认字段
	g_cEngineOption.nTextureScale = 2;
	g_eEngineOption.bEnableAutoReLoadTexture = TRUE;
	g_eEngineOption.bRenderTerrain = TRUE;
	g_eEngineOption.bUseLogical = TRUE;
	g_eEngineOption.bUseLogicalObject = TRUE;
	g_eEngineOption.bTerrDynamic = TRUE;
	g_eEngineOption.bUseMultiThreadLoad = TRUE;
	g_eEngineOption.bUseMultiThreadCull = TRUE;
/*
	g_eEngineOption.fBloomExposure1 = 1.f;   
	g_eEngineOption.fBloomExposure2 = 0.3F;
	g_eEngineOption.fBloomRate = 0;          // bloom 当前的变化率
	g_eEngineOption.fBloomDelay = 1000;         // bloom 的过度时间
	g_eEngineOption.fBloomGate = 0.03F;
	g_eEngineOption.fBloomGaussCoefficient = 10;
	g_eEngineOption.nBloomSampleNum = 8;

    g_eEngineOption.nGodRaySampleNum = 3;
    g_eEngineOption.fGodRayDensity = 0.8f;
    g_eEngineOption.fGodRayWeight = 0.58f;
    g_eEngineOption.fGodRayDecay = 0.8f;
    g_eEngineOption.fGodRayExposure = 0.29f;
    g_eEngineOption.fLimitAngle = D3DX_PI * 2;

    g_eEngineOption.nSSAOLevel = SSAO_LEVEL_NORMAL;
    g_eEngineOption.fSSAOSampleRadius = 20.f;
    g_eEngineOption.fSSAODarkFact = 0.1f;
    g_eEngineOption.fBlurWeight = 0.5f;

    g_eEngineOption.fDetailPower = 1.f;
    g_eEngineOption.fBlurPower = 0.f;
    g_eEngineOption.fBlurDistance = 20000.f;
    */

    //g_eEngineOption.nCurveStype = -1;
    g_cEngineOption.fWaterAlphaMin = 0.25f;
    g_cEngineOption.fWaterReduHeight = 400.f;
    g_cEngineOption.nWaterReflTextSize = 512;
    g_cEngineOption.nWaterRefrTextSize = 1024;

	g_cEngineOption.fMinSoundRange = 10.0f;
    g_cEngineOption.bUseModelBsp = TRUE;
	
    //for (int i = 0; i <= 255; ++i)
    //{
    //    g_eEngineOption.CurveRed[i] = (BYTE)i;
    //    g_eEngineOption.CurveGreen[i] = (BYTE)i;
    //    g_eEngineOption.CurveBlue[i] = (BYTE)i;
    //}
/*
	g_eEngineOption.fFogDensityV = 0.001f;
	g_eEngineOption.fSlopeThreshold = 0.1f;
	g_eEngineOption.fHeightFalloff = 0.004f;
	g_eEngineOption.vecFogcolor = D3DXVECTOR4(0.7f,0.7f,0.7f,1.0f);

	g_eEngineOption.fHueScale = 0.68F;   
	g_eEngineOption.fHueBias  = -0.04F;
	g_eEngineOption.fSaturationScale = 1.f;           
	g_eEngineOption.fSaturationBias = 0.09f;      
	g_eEngineOption.fIntensityScale = 0.67f;   
	g_eEngineOption.fIntensityBias = 0.16f;
	g_eEngineOption.fShockWavePower = 0.11f;
<<<<<<< KG3DEngineManager.cpp
*/
	
	//g_eEngineOption.fSoftness = 2.0f;
	//g_eEngineOption.fJitter = 1.0f;

	g_eEngineOption.szWaterConfigFile[0] = '\0';

	g_eEngineOption.nFlySpeedX = 20;
	g_eEngineOption.nFlySpeedY = 10;

	ZeroMemory(g_KeyState,sizeof(BYTE)*256);

	TCHAR AppDir[256] = {0};
	GetCurrentDirectory(256, AppDir);
	wsprintf(g_Def_WorkDirectory,"%s\\",AppDir);
	wsprintf(g_Def_AppDirectory ,"%s\\",AppDir);

	m_FileFinder.Init();

    m_hModuleIPPCodec = NULL;
    m_pCreateIPPCodec = NULL;
    m_pIPPCodec = NULL;
	m_MovieReady = FALSE;
	m_bRecordReady = FALSE;
	g_bClient = FALSE;
	g_bAutoReloadTexture = FALSE;
    m_bPostRenderParamsLockReadOnly = TRUE;

	m_pPhysicsManager = NULL;

#if defined(DEBUG) | defined(_DEBUG)
    RuntimeTest();
#endif

	//LoadConfig();//统一改到Init的开始做，这里做的话，有的变量还没有初始化。cpp之间的全局变量的初始化顺序是不保证的，和编译顺序有关系。

	m_pSound = NULL;
	m_pfOnPresent = NULL;
	m_pfOnRenderEnd = NULL;

	g_dwMainThreadID = GetCurrentThreadId();

	InitOutDoorSpaceMgrRegister();

    m_listEngineOptionCaps.clear();
}

KG3DEngineManager::~KG3DEngineManager(void)
{
	SAFE_RELEASE(m_pTimeClock);
	UnInitOutDoorSpaceMgrRegister();
    UnintEngineOptionCaps();
}

IRecPlayer* KG3DEngineManager::GetNewRecPlayer()
{
	return g_GetNewRecPlayer();
}

RecordOption* KG3DEngineManager::GetRecordOption()
{
	return &g_RecordOption;
}

void KG3DEngineManager::SetRecordOption(const RecordOption *pOption)
{
	g_RecordOption = *pOption;
}


HRESULT KG3DEngineManager::GetRelativePath(const TCHAR* strAbsPath11, const TCHAR* strAbsPath21, TCHAR* strRelativePath)
{   
	return g_pFileFinder->GetRelativePath(strAbsPath11, strAbsPath21, strRelativePath);
}

HRESULT SoundEventRecordCallBack(SoundFunctionType EventFunction,SoundParameters& AParameters)
{
	if (g_cRecorder.IsRecord() && !g_cRecorder.IsLock())
	{
		FunctionType RecEventFunction = (FunctionType)(FT_KG3DSOUND_PLAYBGMUSIC + (EventFunction - SOUNDFT_PLAYBGMUSIC));
		//KG3DRecorder::Parameters RecParameters = *((KG3DRecorder::Parameters*)(&AParameters));
		KG3DRecorder::Parameters RecParameters;
		RecParameters.nNum = AParameters.nNum;
		for (int i = 0;i < KG3DRecorder::Parameters::sMaxNum;i++)
			RecParameters.nFrom[i] = AParameters.nFrom[i];
		RecParameters.size_tDataSize = AParameters.size_tDataSize;
		RecParameters.pData = new char[RecParameters.size_tDataSize];
		memcpy(RecParameters.pData,AParameters.pData,RecParameters.size_tDataSize);
		g_cRecorder.RecordEvent(0,RecEventFunction,0,RecParameters);
	}
	return S_OK;
}

LPDIRECT3DDEVICE9 KG3DEngineManager::GetDevice()
{
	return g_pd3dDevice;
}

LPDIRECT3D9	KG3DEngineManager::GetD3D()
{
	return g_cGraphicsEngine.m_pD3D;
}

HRESULT KG3DEngineManager::Init(unsigned uMode, unsigned int uFlag,  HWND hBaseWindow, HWND hRenderWindow) // uMode = 0 : windowed, uMode = 1: fullscreen
{
	//内存泄漏，必须放最前面
#if defined(DEBUG) | defined(_DEBUG)
	{
		int nFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		_CrtSetDbgFlag(nFlag | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	}
#endif
#ifdef _DEBUG // 暂时Debug版本开启
    // 开启检测除0等浮点数运算错误
    //Set the x86 floating-point control word according to what exceptions you want to trap. 
    
    //Always call _clearfp before setting the control word Because the second parameter  
    //in the following call is 0, it only returns the floating-point control word
    _clearfp(); 
    
    unsigned int cw = _controlfp(0, 0);  //Get the default control word
   
    //Set the exception masks off for exceptions that you want to trap.  
    //When a mask bit is set, the corresponding floating-point exception is 
    //blocked from being generating.
    cw &=~(
        EM_OVERFLOW  |
        //EM_UNDERFLOW |
        EM_ZERODIVIDE//|
        //EM_DENORMAL // |
       // EM_INVALID // 开启后DX内部会有一些异常报出来，查不出原因，先禁用
    );
    //For any bit in the second parameter (mask) that is 1, the corresponding bit
    //in the first parameter is used to update the control word.  
    _controlfp(cw, MCW_EM); //Set it.
#endif 

	ZeroMemory((LPVOID)&g_logicalConstants, sizeof(g_logicalConstants));

	KG3DCollocator::CollocateBeforeInit();//配置类关系，放在这里执行一次就好了

	HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int nRetCode = false;
    int nInitFileLoader = false;
    int nInitGraphicsEngine = false;
    int nInit3DUI = false;
    int nInitPhysicsManager = false;
    int nInitGraphicsTool = false;
    int nInitTextureTable = false;
	int nInitMaterialTable = false;
    int nInitMeshTable = false;
    int nInitTrackTable = false;
	int nInitModelTable = false;
	int nInitClipTable = false;
    int nInitTabConTable = false;
	int nInitSceneTable = false;
	int nInitRepresentObjectTable = false;
    int nInitLoadPostProcessor = false;
	int nInitRegionInfoManagerTable = false;
    int nInitTerrainBlockStaticResource = false;
    int nInitModelSpeedTreeStaticResource = false;
    int nInitSFXParticleContainerStaticResource = false;
    int nInitGrassPatternContainer = false;
    int nInitBillBoardEffect = false;
    int nInitInstanceHighDefGrassStaticResource = false;
    int nInitMultiThreadTextureRead = false;
    int nInitMultiThreadCull = false;
	int nInitEditMoveGrid = false;
	int nInitEditRotationGrid = false;
	int nInitEditScaleGrid = false;
    int nPreloadSpeedTreeLoaded = false;
    int nInitRecorder = false;
    int nCaption2D = false;
	//bool bSelector = false;
	int nInitModelFillVB = false;
    int nInitProfile = false;
    int nInitResourceDelayReleaseMgr = false;
    int nInitClipPrecompute = false;

    hrRetCode = _GetMemoryfMemoryCoefficient(&g_fMemoryCoefficient);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = KG3DResourceDelayReleaseMgr::Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    nInitResourceDelayReleaseMgr = true;

    // client flags, ugly 
    g_bClient = (KG3DENG_CLIENT & uFlag) != 0;

	{
		BOOL bIsFileExit = TRUE;
		LoadConfig(&bIsFileExit);	//注意这句话的位置，很多东西依赖Config的，移动的时候要小心
		if (! bIsFileExit)//注意，这里不是检查LoadConfig的返回，LoadConfig里面做了些别的操作，不确定其影响，所以单独对文件进行判断
		{
			goto Exit0;	
		}
	}

    hrRetCode = GetProfileManager().Init(g_nEnableProfile);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    nInitProfile = true;

    nRetCode = g_FileLoader.Init();
    KGLOG_PROCESS_ERROR(nRetCode);
    nInitFileLoader = true;

    hrRetCode = InitGraphicsEngine(hBaseWindow, hRenderWindow);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitGraphicsEngine = true;

    hrRetCode = g_GetUI().SetHWnd(hBaseWindow);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInit3DUI = true;

    if (g_cEngineOption.bEnablePhysics)
    {
        m_pPhysicsManager = new KG3DPhysiscManager;    
        KGLOG_PROCESS_ERROR(m_pPhysicsManager);

        hrRetCode = m_pPhysicsManager->Init();
        if (SUCCEEDED(hrRetCode))
        {
            nInitPhysicsManager = true;
        }
        else
        {
            KG_DELETE(m_pPhysicsManager);
        }
    }

    hrRetCode = g_cGraphicsTool.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitGraphicsTool = true;

    hrRetCode = g_cTextureTable.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitTextureTable = true;

    hrRetCode = g_cMaterialTable.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nInitMaterialTable = true;

	hrRetCode = g_cMeshTable.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitMeshTable = true;

    hrRetCode = g_cTrackTable.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitTrackTable = true;

	hrRetCode = g_cModelTable.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nInitModelTable = true;

    hrRetCode = g_cClipTable.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nInitClipTable = true;

    hrRetCode = g_ClipPrecompute.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    nInitClipPrecompute = true;

    hrRetCode = g_tagConTable.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitTabConTable = true;

	hrRetCode = g_OutputWndTable.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = g_cSceneTable.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitSceneTable = true;

    hrRetCode = g_GetObjectTable().Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nInitRepresentObjectTable = true;

    hrRetCode = g_LoadPostProcessor.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitLoadPostProcessor = true;

    KG3DModelPointLight::StaticInit();


    hrRetCode = g_cRegionInfoManagerTable.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	g_bDesign = FALSE;
	if ( KG3DENG_DESIGN & uFlag )
	{
		g_bDesign = TRUE;
	}

    nInitRegionInfoManagerTable = true;

	hrRetCode = InitTerrainBlockStaticResource();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitTerrainBlockStaticResource = true;

	hrRetCode = KG3DModelSpeedTree::InitStaticResource();
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitModelSpeedTreeStaticResource = true;

    hrRetCode = KG3DSFXParticleContainer::OnStaticRestoreDevice();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitSFXParticleContainerStaticResource = true;

    hrRetCode = g_GrassPatternContainer.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitGrassPatternContainer = true;

    hrRetCode = g_BillBoardEffect.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitBillBoardEffect = true;

    hrRetCode = KG3DInstanceHighDefGrass::InitStaticResource();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitInstanceHighDefGrassStaticResource = true;

    g_ProjTextureRender.Init();

	
	hrRetCode = InitModelVBFillList();
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	nInitModelFillVB = true;

	if (g_eEngineOption.bUseMultiThreadLoad)
	{
		hrRetCode = InitMultiThreadTextureRead();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        nInitMultiThreadTextureRead = true;
	}

	if (g_eEngineOption.bUseMultiThreadCull)
	{
		hrRetCode = InitMultiThreadCull();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        nInitMultiThreadCull = true;
	}
   
	//////////////////////////////////////////////////////////////////////////
	//初始化移动相关
	if (!g_bClient)
	{
		hrRetCode = m_EditMoveGrid.Init();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

    	nInitEditMoveGrid = true;

		hrRetCode = m_EditRotationGrid.Init();
        KGLOG_COM_CHECK_ERROR(hrRetCode);

        nInitEditRotationGrid = SUCCEEDED(hrRetCode);

		hrRetCode = m_EditScaleGrid.Init();
        KGLOG_COM_CHECK_ERROR(hrRetCode);

    	nInitEditScaleGrid = SUCCEEDED(hrRetCode);
    }

	hrRetCode = g_FaceMotionSettings.Init();
    
    
	//hrRetCode = LoadPreloadSpeedTree("\\data\\source\\maps\\speedtreepreload.dat");
    //KGLOG_COM_CHECK_ERROR(hrRetCode);

    nPreloadSpeedTreeLoaded = SUCCEEDED(hrRetCode);

    // Recorder
    hrRetCode = g_cRecorder.Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nInitRecorder = true;

    g_bRePlay = FALSE;
    if (KG3DENG_RECORD & uFlag)
        g_bRePlay = TRUE;
    //如果是在客户端而且不是在回放，并且配置文件里允许录制则开始录制。
    if ((KG3DENG_CLIENT & uFlag) && !g_bRePlay && g_cEngineOption.bEnableRecord)
        g_cRecorder.Record();
    srand((unsigned int)time(NULL)); 

    if (m_pSound)
    {
        if (g_bRePlay)
        {
            m_pSound->Unlock();
        }
        if (g_cRecorder.IsRecord())
        {
            m_pSound->SetEventRecordFuc(&SoundEventRecordCallBack);
        }
    }

	hrRetCode = g_cCaption2D.Init();
    if (FAILED(hrRetCode))
	{
		KGLogPrintf(KGLOG_ERR, "Initialize caption failed.");
	}


    nCaption2D = true;
	//hrRetCode = g_Selector.Init();
	//KGLOG_COM_PROCESS_ERROR(hrRetCode);

	g_GetTerrainRepresentInfo().Init();
    g_GetAnimationSoundTagInfo().Init();
	//bSelector = true;

	_ASSERTE(NULL != g_pd3dDevice);
	KG3DCollocator::CollocateAfterInit(g_pd3dDevice);

	g_cTextureTable.LoadDetailDefines();

	m_States.set(em_init_finished);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        //if (bSelector)
		//{
			//g_Selector.UnInit();
		//}
		if (nCaption2D)
        {
            g_cCaption2D.UnInit();
        }
        if (nInitRecorder)
        {
            g_cRecorder.UnInit();
        }
        if (nPreloadSpeedTreeLoaded)
        {
            // TODO:
        }
        if (nInitEditScaleGrid)
        {
		    m_EditScaleGrid.UnInit();
        }
        if (nInitEditRotationGrid)
        {
		    m_EditRotationGrid.UnInit();
        }
    	if (nInitEditMoveGrid)
        {
		    m_EditMoveGrid.UnInit();
        }
        if (nInitMultiThreadCull)
        {
		    UnInitMultiThreadCull();
        }
        if (nInitMultiThreadTextureRead)
        {
			UnInitMultiThreadTextureRead();
        }

        g_cMultiThreadLoader.UnInit();

        if (nInitResourceDelayReleaseMgr)
        {
            KG3DResourceDelayReleaseMgr::UnInit();
            nInitResourceDelayReleaseMgr = false;
        }

		if(nInitModelFillVB)
		{
			UnInitModelVBFillList();
		}
        if (nInitInstanceHighDefGrassStaticResource)
        {
            KG3DInstanceHighDefGrass::UnInitStaticResource();
        }
        if (nInitBillBoardEffect)
        {
            g_BillBoardEffect.UnInit();
        }
        if (nInitGrassPatternContainer)
        {
            g_GrassPatternContainer.UnInit();
        }
        if (nInitSFXParticleContainerStaticResource)
        {
            KG3DSFXParticleContainer::OnStaticInvalidDevice();
        }
        if (nInitModelSpeedTreeStaticResource)
        {
            KG3DModelSpeedTree::UnInitStaticResource();
        }
        if (nInitTerrainBlockStaticResource)
        {
            UnInitTerrainBlockStaticResource();
        }
        if (nInitRegionInfoManagerTable)
        {
            g_cRegionInfoManagerTable.UnInit();
        }
        if (nInitLoadPostProcessor)
        {
            g_LoadPostProcessor.UnInt();
        }
        if (nInitRepresentObjectTable)
        {
            g_GetObjectTable().UnInit();
        }
        if (nInitSceneTable)
        {
            g_cSceneTable.UnInit();
        }
        if (nInitTabConTable)
        {
            g_tagConTable.UnInit();
        }
        if (nInitClipPrecompute)
        {
            g_ClipPrecompute.UnInit();
            nInitClipPrecompute = false;
        }
	    if (nInitClipTable)
        {
            g_cClipTable.UnInit();
        }
        if (nInitModelTable)
        {
            g_cModelTable.UnInit();
        }
        if (nInitTrackTable)
        {
            g_cTrackTable.UnInit();
        }
        if (nInitMeshTable)
        {
            g_cMeshTable.UnInit();
        }
	    if (nInitMaterialTable)
        {
            g_cMaterialTable.UnInit();
        }
        if (nInitTextureTable)
        {
            g_cTextureTable.UnInit();
        }
        if (nInitGraphicsTool)
        {
            g_cGraphicsTool.UnInit();
        }
        if (nInitPhysicsManager)
        {
            m_pPhysicsManager->UnInit();
        }
        SAFE_DELETE(m_pPhysicsManager);
        if (nInit3DUI)
        {
            g_GetUI().SetHWnd(NULL);
        }
        if (nInitGraphicsEngine)
        {
            UnInitGraphicsEngine();
        }
        if (nInitFileLoader)
        {
            g_FileLoader.UnInit();
            nInitFileLoader = false;
        }
        if (nInitProfile)
        {
            GetProfileManager().UnInit();
            nInitProfile = false;
        }
    }
	return hrResult;
}

HRESULT KG3DEngineManager::LoadPreloadSpeedTree(LPCSTR strFileName)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	IFile *pFile = NULL;
	TCHAR strInput[MAX_PATH];
	unsigned int uSize = 0;
	KG_PROCESS_ERROR(strFileName);

	pFile = g_OpenFile(strFileName);
	KG_PROCESS_ERROR(pFile);

	uSize = pFile->Read(strInput, MAX_PATH);
	while (uSize)
	{
		KG3DModel *pModel = NULL;
		hResult = g_cModelTable.LoadResourceFromFile(strInput, 0, 0, (IKG3DResourceBase**)&pModel);
		if (FAILED(hResult))
		{
			KGLogPrintf(KGLOG_ERR, "Preload SpeedTree %s failed.\n", strInput);
		}
		else
		{
			//m_PreloadModels.push_back(pModel);
		}
		uSize = pFile->Read(strInput, MAX_PATH);
	}
	hRetCode = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	return hRetCode;
}

BOOL KG3DEngineManager::InitIPPVideo()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    void* pfnCreateIPPCodec = NULL;
    TCHAR szDllFilePath[MAX_PATH];

#ifdef _DEBUG
    static LPCTSTR s_szDllName = _T("IPPVideoD.dll");
#else
    static LPCTSTR s_szDllName = _T("IPPVideo.dll");
#endif

    nRetCode = _sntprintf_s(szDllFilePath, _countof(szDllFilePath), _TRUNCATE, _T("%s%s"), g_szModuleFilePath, s_szDllName) > 0;
    KGLOG_PROCESS_ERROR(nRetCode);

    m_hModuleIPPCodec = LoadLibrary(szDllFilePath);
    KGLOG_PROCESS_ERROR(m_hModuleIPPCodec);

    pfnCreateIPPCodec = GetProcAddress(m_hModuleIPPCodec, "CreateIPPCodec");

    hr = SetIPPVideo(pfnCreateIPPCodec);
    KGLOG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    if (!nResult)
    {
        FreeLibrary(m_hModuleIPPCodec);
        m_hModuleIPPCodec = NULL;
    }
    return nResult;
}

void KG3DEngineManager::UnInitIPPVideo()
{
    if (MovieReady())
        MovieStop();

    SetIPPVideo(NULL);

    FreeLibrary(m_hModuleIPPCodec);
    m_hModuleIPPCodec = NULL;
}

HRESULT KG3DEngineManager::SetIPPVideo(void* pfnCreateIPPCodec)
{
    HRESULT hrResult = E_FAIL;

    m_pCreateIPPCodec = pfnCreateIPPCodec;

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

HRESULT KG3DEngineManager::MovieRecord(LPCTSTR szFolder, int nSizeScale, UINT uFilter, UINT uQuality, int nEncoder, int nFps)
{
	int nRetCode = false;
    HRESULT hrResult = E_FAIL;
	IPPSpec_Params_t Params;
	TCHAR szFileName[64];
	time_t tmNow;
	RECT rcClient;

    if (m_pCreateIPPCodec == NULL)
    {
        nRetCode = InitIPPVideo();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

	KGLOG_PROCESS_ERROR(szFolder && szFolder[0] != _T('\0'));

	MovieStop();

	if (g_bFullScreen)
	{
		::GetWindowRect(g_hBaseWnd, &rcClient);
	}
	else
	{
		::GetClientRect(g_hBaseWnd, &rcClient);
	}

	m_pIPPCodec = ((fnCreateIPPCodec)m_pCreateIPPCodec)();
	KGLOG_PROCESS_ERROR(m_pIPPCodec);

	memset(&Params, 0, sizeof(Params));

	time(&tmNow);

	_tcsncpy(Params.Paths, szFolder, _countof(Params.Paths));

	_tcsftime(szFileName, _countof(szFileName), "%Y-%m-%d_%H-%M-%S", localtime(&tmNow));
	_sntprintf(Params.FileAvis, _countof(Params.FileAvis), _T("%s\\%s.avi"), Params.Paths, szFileName);

	Params.CapsSpec  = nEncoder;//IPPSPEC_MPEG2;
	Params.CapsTimes = 60; // 60 秒

	Params.CapsFps   = nFps;
	Params.CapsDSize = nSizeScale;

	Params.CapsFilter  = uFilter;
	Params.CapsQuality = uQuality;

	Params.ClientWidth = rcClient.right - rcClient.left;
	Params.ClientHeight = rcClient.bottom - rcClient.top;
	Params.pSoundShell = m_pSound;

	nRetCode = m_pIPPCodec->Initialize(g_pd3dDevice, Params);
	KGLOG_PROCESS_ERROR(nRetCode);

	m_MovieReady = TRUE;

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
	    MovieStop();
    }
	return hrResult;
}

HRESULT KG3DEngineManager::MovieStop( )
{
	if (m_pIPPCodec)
	{
		m_pIPPCodec->End();
		m_pIPPCodec->CleanUp();
		m_pIPPCodec = NULL;
	}

	m_MovieReady = FALSE;

	return S_OK;
}

BOOL KG3DEngineManager::MovieReady( ) const
{
	return m_MovieReady;
}

VOID KG3DEngineManager::MoviePresent()
{
	if (m_pIPPCodec && MovieReady())
	{
		if (m_pIPPCodec->Prepare())
		{
			m_pIPPCodec->Flush();
		}
	}
}


VOID KG3DEngineManager::MovieViewport(RECT const& Viewport)
{
	if (m_pIPPCodec && MovieReady())
	{
		SIZE size = { Viewport.right - Viewport.left, Viewport.bottom - Viewport.top };

		m_pIPPCodec->SetViewport(size);
	}
}



BOOL KG3DEngineManager::CanRecord()
{
	return g_cRecorder.IsRecord();
}

BOOL KG3DEngineManager::IsRecording()
{
	return m_bRecordReady;
}

HRESULT KG3DEngineManager::StartRecord(LPCTSTR szFolder,unsigned uMode)
{
	//BOOL bRetCode = FALSE;
	TCHAR szFileName[MAX_PATH];
	TCHAR szPathName[MAX_PATH];
	TCHAR szRecName[MAX_PATH];
	time_t tmNow;

	KGLOG_PROCESS_ERROR(szFolder && szFolder[0] != _T('\0'));
	//StopRecord();
	_tcsncpy(szPathName, szFolder, _countof(szPathName));
	time(&tmNow);
	_tcsftime(szFileName, _countof(szFileName), "%Y-%m-%d_%H-%M-%S", localtime(&tmNow));
	_sntprintf(szRecName, _countof(szRecName), _T("%s\\%s.rec"),szPathName, szFileName);
	KGLOG_COM_PROCESS_ERROR(g_cRecorder.StartRecordEvent(szRecName,uMode));
	m_bRecordReady = TRUE;
	return S_OK;
Exit0:
	//StopRecord();
	return E_FAIL;
}

HRESULT KG3DEngineManager::StopRecord()
{
	HRESULT hr = E_FAIL;
	hr = g_cRecorder.StopRecordEvent();
	m_bRecordReady = FALSE;
	return hr;
}


HRESULT KG3DEngineManager::InitGraphicsEngine(HWND hBaseWindow, HWND hRenderWindow)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hr = E_FAIL;
    KG3DDeviceSettings Settings;
    IIniFile* pFile = NULL;
    RECT rcWindow;
    int nEnableDebug = false;
    int nInitDeviceCallback = false;
    int nInitGraphicsEngine = false;

    hr = g_cGraphicsEngine.SetDeviceCallback(&s_DeviceCallback);
    KGLOG_COM_PROCESS_ERROR(hr);

    nInitDeviceCallback = true;

	g_hBaseWnd = hBaseWindow;
	g_hRenderWnd = hRenderWindow;

	//////////////////////////////////////////////////////////////////////////
    //启动图形引擎

    GetWindowRect(hRenderWindow, &rcWindow);

    memset(&Settings, 0, sizeof(Settings));

    Settings.bFullScreen = FALSE;
    Settings.bPanauision = FALSE;
    Settings.bExclusiveMode = FALSE;
    Settings.uWidth = rcWindow.right - rcWindow.left;
    Settings.uHeight = rcWindow.bottom - rcWindow.top;
    Settings.bTripleBuffering = FALSE;
    Settings.bVSync = FALSE;
    Settings.uRefreshRate = 0;
    Settings.uColorDepth = 32;
    Settings.nMultiSampleType = 0;
    Settings.dwMultiSampleQuality = 0;

    pFile = g_OpenIniFile(g_szConfigFilePath);
    if (pFile)
    {
        pFile->GetInteger("Debug", "3DEngineDebugInfo", 0, &nEnableDebug);
        pFile->GetInteger("KG3DENGINE", "MultiSampleType", 0, &Settings.nMultiSampleType);
        pFile->GetInteger("KG3DENGINE", "MultiSampleQuality", 0, (int*)&Settings.dwMultiSampleQuality);

        if (g_bClient)
        {
            pFile->GetInteger("Main", "FullScreen", 0, &Settings.bFullScreen);
            pFile->GetInteger("Main", "Panauision", 0, &Settings.bPanauision);
            pFile->GetInteger("Main", "ExclusiveMode", 0, &Settings.bExclusiveMode);

            pFile->GetInteger("Main", "CanvasWidth", 640, (int*)&Settings.uWidth);
            pFile->GetInteger("Main", "CanvasHeight", 480, (int*)&Settings.uHeight);
            
            pFile->GetInteger("Main", "RefreshRate", 0, (int*)&Settings.uRefreshRate);

            pFile->GetInteger("KG3DENGINE", "TripleBuffering", 0, &Settings.bTripleBuffering);
            pFile->GetInteger("KG3DENGINE", "VSync", 0, &Settings.bVSync);
        }

        KG_COM_RELEASE(pFile);
    }

	Settings.bVSync = TRUE;//强制打开垂直同步，减少烧显卡的悲剧

	_ASSERTE(NULL == g_pd3dDevice);
    hr = g_cGraphicsEngine.Init(&Settings, nEnableDebug);
    KGLOG_COM_PROCESS_ERROR(hr);

    nInitGraphicsEngine = true;

    hr = ValidateAfterInitDeviceEngineOption(g_cEngineOption);
    KGLOG_COM_PROCESS_ERROR(hr);

	_ASSERTE(NULL != g_pd3dDevice);

	g_GetSystemEventListener().OnD3DDeviceCreate();

    hr = g_GetUI().SetViewportSize(Settings.uWidth, Settings.uHeight);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;

Exit0:
    if (FAILED(hrResult))
    {
        if (nInitGraphicsEngine)
        {
            g_cGraphicsEngine.UnInit();
        }

        g_hBaseWnd = NULL;
        g_hRenderWnd = NULL;

        if (nInitDeviceCallback)
        {
            g_cGraphicsEngine.SetDeviceCallback(NULL);
        }
    }
	return hrResult;
}

HRESULT KG3DEngineManager::UnInitGraphicsEngine()
{
    HRESULT hr = E_FAIL;
    
    hr = g_cGraphicsEngine.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    g_cGraphicsEngine.SetDeviceCallback(NULL);

    return hr;
}

//////////////////////////////////////////////////////////////////////////
//消息处理
HRESULT KG3DEngineManager::ProcessMessage(unsigned message, WPARAM wParam, LPARAM ulParam)//<PROCESS_MESSAGE>
{
	if(m_lpFocusScene)
	{
		return m_lpFocusScene->ProcessMessage(message,wParam,ulParam);
	}
	return S_OK;
}

IKG3DResourceManager* KG3DEngineManager::Get3DModelTable()
{
	g_cRecordRM.SetResourceManager(&g_cModelTable);
	return &g_cRecordRM;
}

IKG3DSoundBallTable* KG3DEngineManager::GetSoundBallTable()
{
	return &g_cSoundBallTable;
}

IEKG3DMesh* KG3DEngineManager::FindMeshBase(DWORD dwID)
{
    KG3DMesh* pReturn = NULL;
    HRESULT hResult = g_cMeshTable.GetResource(&pReturn, dwID);
	KG_COM_PROCESS_ERROR(hResult);

	if (pReturn)
	{
		pReturn->Release();
	}
Exit0:
	return pReturn;
}

IKG3DUI* KG3DEngineManager::Get3DUI()
{
	return &g_GetUI();
}

HRESULT KG3DEngineManager::FrameMove()
{
    KG3PROFILE_DURATION("Engine FrameMove");
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	if(NULL != m_pTimeClock)
		m_pTimeClock->FrameMove();


    m_TimeControl.FrameMove(FALSE);

    g_DefaultFaceMotionMgr.FrameMove();

	g_GetDefaultShaderGroup().CheckShaderModel();

	//只有不在录像回放中manager才调用场景framemove，场景回放中时，回放自行管理各场景framemove
	if (!g_bRePlay)
	{
        if(m_bRenderFocusSceneOnly)
        {
            if (m_lpFocusScene)
            {
                hr = m_lpFocusScene->FrameMove();
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }
        else
        {
            for (list<KG3DScene*>::iterator it = m_listScene.begin(); it != m_listScene.end(); ++it)
            {
                KG3DScene* p3DScene = *it;

                hr = p3DScene->FrameMove();
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }
	}

	IKG3DSoundShell *pSound = GetSound();
	if (pSound)
	{
		pSound->Active();
	}

	//自释放的Model需要在这里调用Framemove以触发释放操作
	SelfDestructedFrameMove();

#ifdef KG3DENGINE_DISABLE_MULTITHREAD

    UpdateMultiThreadBoneAnimation(NULL);
    UpdateMultiThreadModelProgress(NULL);

    g_MultiThreadCullProgress(NULL);
    g_MultiThreadLoadProgress(NULL);
    g_MultiThreadLoadProgressTerrain(NULL);

#endif

	g_cTextureTable.EvictResources(4 * 1024 * 1024);



	g_cSceneTable.CheckReleaseMultiThread();

	g_cRegionInfoManagerTable.CheckReleaseMultiThread();

	g_GetObjectTable().CheckReleaseMultiThread();

	g_cModelTable.CheckReleaseMultiThread();

	g_cTrackTable.CheckReleaseMultiThread();

    g_cClipTable.ReleaseResourceLRU();

	g_cMaterialTable.CheckReleaseMultiThread();

	g_cMeshTable.CheckReleaseMultiThread();

	g_cTextureTable.CheckReleaseMultiThread();

    g_LoadPostProcessor.FrameMove();

    KG3DResourceDelayReleaseMgr::ReleaseAll();

    hrResult = S_OK;
Exit0:
	return hrResult;
}

//渲染

HRESULT KG3DEngineManager::Render()
{
    KG3PROFILE_DURATION("Engine Render");
    HRESULT hr = E_FAIL;

    if (g_cGraphicsEngine.CheckDevice() == D3D_OK)
    {
        for (list<KG3DScene*>::iterator it = m_listScene.begin(); it != m_listScene.end(); ++it)
        {
            KG3DScene* pScene = NULL;

            pScene = *it;
            KGLOG_PROCESS_ERROR(pScene);

            hr = pScene->Render(0);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    hr = S_OK;

Exit0:
    return hr;
}

void KG3DEngineManager::OnRenderEnd(double fValue)
{
	if (m_pfOnRenderEnd)
	{
		(*m_pfOnRenderEnd)(fValue);
	}
}

void KG3DEngineManager::OnPresent(double fValue)
{
	if (m_pfOnPresent)
	{
		(*m_pfOnPresent)(fValue);
	}
}

void KG3DEngineManager::SetOnPresentCallBackFunc(pfOnPresent pfCallback)
{
	m_pfOnPresent = pfCallback;
}

void KG3DEngineManager::SetOnRenderEndCallBackFunc(pfOnRenderEnd pfCallback)
{
	m_pfOnRenderEnd = pfCallback;
}


void KG3DEngineManager::SetSoundShell(IKG3DSoundShell *pShell)
{
	m_pSound = pShell;
}

IKG3DSoundShell* KG3DEngineManager::GetSound()
{
	return m_pSound;
}

extern KG3DAnimationWarperTable g_cAnimationWarperTable;

HRESULT KG3DEngineManager::UnInit()
{
    HRESULT hr = E_FAIL;

	KG3DCollocator::UnCollocateBeforeUnInit(g_pd3dDevice);
	g_GetSystemEventListener().OnD3DDeviceRelease();
		
    UnInitIPPVideo();

    if (g_cRecorder.IsRecord())
    {
        hr = g_cRecorder.UnInit();
        KGLOG_COM_CHECK_ERROR(hr);
    }

	g_cAnimationWarperTable.UnInit();

	g_ModelShadowRendererRelease();
	
    g_GetRenderStateMgr().Restore();
	//g_Selector.UnInit();
	g_cCaption2D.UnInit();
	ClearSelfDestructSfxList();

    hr = SaveConfig();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = m_EditScaleGrid.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = m_EditRotationGrid.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = m_EditMoveGrid.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

    KG3DModelPointLight::StaticUnInit();

    if (g_eEngineOption.bUseMultiThreadCull)
    {
        hr = UnInitMultiThreadCull();
        KGLOG_COM_CHECK_ERROR(hr);
    }

    if (g_eEngineOption.bUseMultiThreadLoad)
    {
        hr = UnInitMultiThreadTextureRead();
        KGLOG_COM_CHECK_ERROR(hr);
    }

    g_cMultiThreadLoader.UnInit();

	hr = UnInitModelVBFillList();
	KGLOG_COM_CHECK_ERROR(hr);

    hr = KG3DResourceDelayReleaseMgr::UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

	hr = g_DefaultFaceMotionMgr.UnInit();
	KGLOG_COM_CHECK_ERROR(hr);

    hr = KG3DInstanceHighDefGrass::UnInitStaticResource();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = g_BillBoardEffect.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = g_GrassPatternContainer.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

	list<KG3DScene*>::iterator i = m_listScene.begin();
	while (i!=m_listScene.end())
	{
		KGLogPrintf(KGLOG_ERR,"场景尚未释放\n");
		++i;
	}
	m_listScene.clear();

    hr = g_cSceneTable.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

	hr = g_OutputWndTable.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = KG3DSFXParticleContainer::OnStaticInvalidDevice();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = KG3DModelSpeedTree::UnInitStaticResource();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = UnInitTerrainBlockStaticResource();
    KGLOG_COM_CHECK_ERROR(hr);

    hr =  g_cRegionInfoManagerTable.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = g_LoadPostProcessor.UnInt();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = g_GetObjectTable().UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = g_tagConTable.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

	hr = g_cModelTable.UnInit();
	KGLOG_COM_CHECK_ERROR(hr);

    hr = g_cTrackTable.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

    g_ClipPrecompute.UnInit();

    hr = g_cClipTable.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = g_cMaterialTable.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = g_cMeshTable.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

	hr = g_cGraphicsTool.UnInit();
	KGLOG_COM_CHECK_ERROR(hr);

    hr = g_cTextureTable.UnInit();
    KGLOG_COM_CHECK_ERROR(hr);
	
	g_cDetailDefinePool.UnInit();

	g_cPointLightInfoPool.UnInit();
   
	g_cSoundBallTable.UnInit();    

    if (m_pPhysicsManager)
    {
        hr = m_pPhysicsManager->UnInit();
        KGLOG_COM_CHECK_ERROR(hr);

        SAFE_DELETE(m_pPhysicsManager);
    }

    hr = g_GetUI().SetHWnd(NULL);
    KGLOG_COM_CHECK_ERROR(hr);

    hr = UnInitGraphicsEngine();
    KGLOG_COM_CHECK_ERROR(hr);

    g_FileLoader.UnInit();

    GetProfileManager().UnInit();

    //if (!g_bClient)
    //    g_GetUI().Exit();

	return S_OK;
}

HRESULT KG3DEngineManager::NewOneScene(unsigned uType, IKG3DScene **ppRetScene)
{
#if defined(DEBUG) | defined(_DEBUG)
	//检查g_logicalConstants是否已经被初始化，客户端和带逻辑的编辑器使用了这个结构，必须由逻辑在创建Scene之前初始化
	if(g_bClient || g_eEngineOption.bUseLogical)
	{
		UINT* plogical = (UINT*)&g_logicalConstants;
		size_t uElems = sizeof(g_logicalConstants)/sizeof(UINT);
		_ASSERTE(uElems * sizeof(UINT) == sizeof(g_logicalConstants) && _T("每个元素都应该被初始化切应该不为0(好像没有为0的项\
									，所以每个元素都可以以非零为条件检查，如果真的有需要是0的元素，请改变检查代码)"));
		for (size_t i = 0; i < uElems; ++i)
		{
			_ASSERTE(0 != plogical[i]);
		}
	}
#endif

	KG3DScene* pNewScene = NULL;
	if(FAILED(g_cSceneTable.NewOneResourse(uType,(IKG3DResourceBase**)(&pNewScene))))
	{
		(*ppRetScene) = NULL;
		return E_FAIL;
	}

    if (m_pPhysicsManager)
    {
        KG3DPhysiscScene* pPhysiscScene = NULL;

        if (SUCCEEDED(m_pPhysicsManager->NewOnePhysiscScene(&pPhysiscScene)))
        {
            pNewScene->SetPhysiscScene(pPhysiscScene);
        }
    }

	(*ppRetScene) = pNewScene;

	m_listScene.push_back(pNewScene);
	SetFocusScene(pNewScene);

	return S_OK;
}

HRESULT KG3DEngineManager::RemoveScene(IKG3DScene *pScene)
{

    if (g_cGraphicsTool.GetCurScene() == pScene)
        g_cGraphicsTool.SetCurScene(NULL);


	list<KG3DScene*>::iterator i = m_listScene.begin();
	while (i!=m_listScene.end())
	{
		KG3DScene* pS = *i;

		if(pScene==pS)
		{
			if(pS==m_lpFocusScene)
			{
				m_lpFocusScene = NULL;
			}
			KG3DPhysiscScene* pPhysiscScene = NULL;
			if (m_pPhysicsManager)
			{
				pPhysiscScene = pS->GetPhysiscScene();
			}

			SAFE_RELEASE(pS);
            m_listScene.erase(i);

			if (m_pPhysicsManager)
			{
				m_pPhysicsManager->DeleteOnePhysiscScene(pPhysiscScene);
			}           

			return S_OK;
		}
		++i;
	}

	return E_FAIL;
}

HRESULT KG3DEngineManager::Reset()
{
	return S_OK;
}

HRESULT KG3DEngineManager::SetFocusScene(KG3DScene* pScene)
{
	// Last modify by Chen Tianhong: 2007-7-23 11:25:30///必须向g_cGraphicsTool注册
	if (pScene)
	{
		KG_COM_PROCESS_ERROR(pScene->OnSetFocus());
	}

	m_lpFocusScene = pScene;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DEngineManager::SetMaxViewScene(KG3DScene* pScene)
{
	m_listScene.remove(pScene);
	m_listScene.push_front(pScene);
	//m_lpFocusScene = pScene;
	//// Last modify by Chen Tianhong: 2007-7-23 11:17:21
	this->SetFocusScene(pScene);
	return S_OK;
}

HRESULT KG3DEngineManager::LoadConfig(BOOL* pIsFileExit)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KSmartIniFile pFile ;
    int nRetCode = false;


    PostRender::KG3DPostRenderEffectManager::StaticConfigLoad();

    pFile = g_OpenIniFile(g_szConfigFilePath);
	if (NULL == pFile)
	{
		std::tstring strError = _T("配置文件读取错误，引擎即将退出，请检查此文件是否存在: ");
		strError += g_szConfigFilePath;
		MessageBox(NULL, strError.c_str(), _T("错误"), MB_OK);
		KGLogPrintf(KGLOG_ERR, _T("%s"), strError.c_str());

		if (NULL != pIsFileExit)
		{
			*pIsFileExit = FALSE;
		}

		goto Exit0;
	}

	if (NULL != pIsFileExit)
	{
		_ASSERTE(NULL != pFile);
		*pIsFileExit = TRUE;
	}

    pFile->GetInteger("Debug", "EnableProfile", false, &g_nEnableProfile);

	pFile->GetInteger("KG3DENGINE", "EnableBladesSmoot", TRUE, &g_cEngineOption.bEnableBladesSmoot);
	pFile->GetInteger("KG3DENGINE", "EnableDynamicUnload", FALSE, &g_cEngineOption.bEnableDynamicUnload);
	g_cEngineOption.bEnableDynamicUnload = TRUE;

	pFile->GetInteger("KG3DENGINE", "SampMipFilter",2,&g_cEngineOption.nSampMipFilter);
	pFile->GetInteger("KG3DENGINE", "SampMinFilter",2,&g_cEngineOption.nSampMinFilter);
	pFile->GetInteger("KG3DENGINE", "SampMagFilter",2,&g_cEngineOption.nSampMagFilter);
	pFile->GetInteger("KG3DENGINE", "MaxAnisotropy",1,(int*)&g_cEngineOption.dwMaxAnisotropy);

	pFile->GetInteger("KG3DENGINE", "TextureScale", 1, &g_cEngineOption.nTextureScale);

	pFile->GetFloat("KG3DENGINE", "CammeraDistance", 40000.0f, &g_cEngineOption.fCameraDistance);
	pFile->GetFloat("KG3DENGINE", "CammeraAngle", D3DX_PI * 0.25f, &g_cEngineOption.fCameraAngle);

	pFile->GetInteger("KG3DENGINE", "UseNewSelection", 0, &g_bUseNewSelection);
	pFile->GetFloat("KG3DENGINE", "MinSoundRange", 10.0f, &g_cEngineOption.fMinSoundRange);

	if (g_cEngineOption.nTextureScale <= 0)	// 容错，此处的Lod不能为0
	{
		g_cEngineOption.nTextureScale = 2;
	}
	if(g_cEngineOption.nTextureScale ==3)
	{
		g_cEngineOption.nTextureScale =2;
	}
	if(g_cEngineOption.nTextureScale >=4)
	{
		g_cEngineOption.nTextureScale =4;
	}
	//pFile->GetInteger("KG3DENGINE", "UseRenderTwiceAlpha", FALSE, &g_cEngineOption.bEnableSpecialAlphaBlend);
	g_cEngineOption.bEnableSpecialAlphaBlend = TRUE;
	pFile->GetInteger("KG3DENGINE", "GrassAnimation", FALSE, &g_cEngineOption.bGrassAnimation);
	pFile->GetInteger("KG3DENGINE", "RenderGrass", TRUE, &g_cEngineOption.bRenderGrass);
	pFile->GetInteger("KG3DENGINE", "GrassAlphaBlend", FALSE,&g_cEngineOption.bGrassAlphaBlend);
	pFile->GetInteger("KG3DENGINE", "AmibentShaderOnly", FALSE, &g_cEngineOption.bAmbientShaderOnly);
	pFile->GetInteger("KG3DENGINE", "PostRenderEffect", FALSE, &g_cEngineOption.bPostEffectEnable);
	
	//是否开启点光源和PostRender高级渲染是连在一起的
	g_bEnablePointLight = FALSE;//g_cEngineOption.bPostEffectEnable;

	pFile->GetInteger("KG3DENGINE", "BloomEnable", FALSE, &g_cEngineOption.bBloomEnable);
	pFile->GetInteger("KG3DENGINE", "bHDR", FALSE, &g_cEngineOption.bHDR);
    pFile->GetInteger("KG3DENGINE", "bGodRay", FALSE, &g_cEngineOption.bGodRay);
    pFile->GetInteger("KG3DENGINE", "bSSAO", FALSE, &g_cEngineOption.bSSAO);
	pFile->GetInteger("KG3DENGINE", "bGlobalFog", FALSE, &g_cEngineOption.bGlobalFog);
	pFile->GetInteger("KG3DENGINE", "bDOF", FALSE, &g_cEngineOption.bDOF);
	pFile->GetInteger("KG3DENGINE", "DetailEnable", FALSE, &g_cEngineOption.bDetail);
	pFile->GetInteger("KG3DENGINE", "MotionBlurEnable", FALSE, &g_cEngineOption.bMotionBlur);
	pFile->GetInteger("KG3DENGINE", "WaterReflection", TRUE, &g_cEngineOption.bWaterReflection);
    SetRenderOption(RENDER_WATER_REFLECTION, g_cEngineOption.bWaterReflection);
	pFile->GetInteger("KG3DENGINE", "WaterReflectEnvironment",TRUE,&g_cEngineOption.bWaterReflectEnvironment);
	pFile->GetInteger("KG3DENGINE", "WaterReflectGround",TRUE,&g_cEngineOption.bWaterReflectGround);
	pFile->GetInteger("KG3DENGINE", "WaterReflectNPC",TRUE,&g_cEngineOption.bWaterReflectNPC);
	pFile->GetInteger("KG3DENGINE", "WaterReflectEntity",TRUE,&g_cEngineOption.bWaterReflectEntity);

	pFile->GetInteger("KG3DENGINE", "WaterRefraction", FALSE, &g_cEngineOption.bWaterRefraction);
	pFile->GetInteger("KG3DENGINE", "ShockWaveEnable", FALSE, &g_cEngineOption.bShockWaveEnable);
	pFile->GetInteger("KG3DENGINE", "HSIEnable", FALSE, &g_cEngineOption.bHSIEnable);
    pFile->GetInteger("KG3DENGINE", "CurveCMYKEnable", FALSE, &g_cEngineOption.bCurveCMYK);
    pFile->GetInteger("KG3DENGINE", "ActiveColorCurve", 0, &g_cEngineOption.nActiveCurveStype);
	//CTH// Last modify by Chen Tianhong: 2007-9-20 11:12:24\

	pFile->GetInteger("KG3DENGINE", "ForceShaderModel", 0, &g_cEngineOption.nForceShaderModel);
	if(g_cEngineOption.nForceShaderModel != 0)
	{
		if (g_cEngineOption.nForceShaderModel < 0 
			|| g_cEngineOption.nForceShaderModel > KG3DEngineCaps::EM_MAX_SHADER_MODEL_SUPPORTED)
		{
			KGLogPrintf(KGLOG_ERR, _T("强制使用ShaderModel%d，数值不对，改回自动判断ShaderModel")
				, g_cEngineOption.nForceShaderModel);
			g_cEngineOption.nForceShaderModel = 0;
		}
		else
		{
			KGLogPrintf(KGLOG_WARNING, _T("强制使用ShaderModel%d"), g_cEngineOption.nForceShaderModel);
		}
	}
	pFile->GetInteger("KG3DENGINE", "EnableDynamicConverMap", 0, &g_cEngineOption.bEnableDynamicConverMap);
	pFile->GetInteger("KG3DENGINE", "CpuSkin", 0, &g_cEngineOption.bCpuSkin);
	pFile->GetInteger("KG3DENGINE", "NumCpuThread", 1, &g_cEngineOption.nNumCpuThread);
	pFile->GetInteger("KG3DENGINE", "FlexBodySmooth", 0, &g_cEngineOption.bFlexBodySmooth);
	pFile->GetInteger("KG3DENGINE", "CpuProcessModelThreadLock", 0, &g_cEngineOption.bCpuProcessModelThreadLock);
	pFile->GetInteger("KG3DENGINE", "EnablePhysics", 0, &g_cEngineOption.bEnablePhysics);
	pFile->GetInteger("KG3DENGINE", "EnableDefaultDynamicWeather", 0, &g_cEngineOption.bEnableDefaultDynamicWeather);
	pFile->GetInteger("KG3DENGINE", "MultiTextureRender", 0, &g_cEngineOption.bMultiTextureRender);
	pFile->GetInteger("KG3DENGINE", "TerrainDetail", 0, &g_cEngineOption.nTerrainDetail);
	g_cEngineOption.nTerrainDetail = 0;//现在整个地图的纹理贴图还有没,暂时把TerrainDetail强制设为0;
	g_cEngineOption.nNumCpuThread = min(8,g_cEngineOption.nNumCpuThread);
	g_cEngineOption.nNumCpuThread = max(1,g_cEngineOption.nNumCpuThread);
	pFile->GetInteger("KG3DENGINE", "VegetationDensity", 1, &g_cEngineOption.nVegetationDensity);
	pFile->GetInteger("KG3DENGINE", "WaterDetail", 1, &g_cEngineOption.nWaterDetail);
    pFile->GetFloat("KG3DENGINE", "fWaterAlphaMin", 0.3f, &g_cEngineOption.fWaterAlphaMin);
    pFile->GetFloat("KG3DENGINE", "fWaterReduHeight", 400.f, &g_cEngineOption.fWaterReduHeight);
    pFile->GetInteger("KG3DENGINE", "nWaterReflTextSize", 512, &g_cEngineOption.nWaterReflTextSize);
    pFile->GetInteger("KG3DENGINE", "nWaterRefrTextSize", 1024, &g_cEngineOption.nWaterRefrTextSize);
	pFile->GetInteger("KG3DENGINE", "EnableNormalMap", 0, &g_cEngineOption.bEnableNormalMap);
	pFile->GetInteger("KG3DENGINE", "EnableGroundNormalMap", 0, &g_cEngineOption.bEnableGroundNormalMap);
	pFile->GetInteger("KG3DENGINE", "EnableSpeedTreeNormalMap", 0, &g_cEngineOption.bEnableSpeedTreeNormalMap);
	pFile->GetInteger("KG3DENGINE", "EnableTimeOptimizeLog", 0, &g_cEngineOption.bEnableTimeOptimizeLog);
	pFile->GetInteger("KG3DENGINE", "EnableDebugBBox", 0, &g_cEngineOption.bEnableDebugBBox);
    pFile->GetInteger("KG3DENGINE", "bUseModelBsp", TRUE, &g_cEngineOption.bUseModelBsp);
	pFile->GetInteger("KG3DENGINE", "bEnableModelLod", TRUE, &g_cEngineOption.bEnableModelLod);

	//g_cEngineOption.bMultiTextureRender = g_cEngineOption.bCpuSkin;//强制相等

    SetRenderOption(RENDER_CPU_MODEL_PROCESS, g_cEngineOption.bCpuSkin);

	pFile->GetInteger("KG3DENGINE", "ModelShadowType", EM_MODEL_SHADOW_TYPE_NONE, (INT*)&g_cEngineOption.ModelShadowType);
	if(g_cEngineOption.ModelShadowType >= EM_MODEL_SHADOW_TYPE_COUNT)
	{
		KGLogPrintf(KGLOG_WARNING, "引擎选项ModelShadowType读取错误，关闭Shadow");
		g_cEngineOption.ModelShadowType = EM_MODEL_SHADOW_TYPE_NONE;
	}
	pFile->GetMultiFloat("KG3DENGINE", "ModelShadowParams", g_cEngineOption.ModelShadowParams, 8);

	
	pFile->GetMultiFloat("KG3DENGINE", "DefautColorCast0", (float*)(&(g_cEngineOption.ColorCast[0])), 4);
	pFile->GetMultiFloat("KG3DENGINE", "DefautColorCast1", (float*)(&(g_cEngineOption.ColorCast[1])), 4);
	pFile->GetMultiFloat("KG3DENGINE", "DefautColorCast2", (float*)(&(g_cEngineOption.ColorCast[2])), 4);
	pFile->GetMultiFloat("KG3DENGINE", "DefautColorCast3", (float*)(&(g_cEngineOption.ColorCast[3])), 4);
	pFile->GetMultiFloat("KG3DENGINE", "DefautColorCast4", (float*)(&(g_cEngineOption.ColorCast[4])), 4);
	pFile->GetMultiFloat("KG3DENGINE", "DefautColorCast5", (float*)(&(g_cEngineOption.ColorCast[5])), 4);
	pFile->GetMultiFloat("KG3DENGINE", "DefautColorCast6", (float*)(&(g_cEngineOption.ColorCast[6])), 4);
	pFile->GetMultiFloat("KG3DENGINE", "DefautColorCast7", (float*)(&(g_cEngineOption.ColorCast[7])), 4);
	
	
	pFile->GetInteger("KG3DENGINE", "EnableRecord"      , FALSE,&g_cEngineOption.bEnableRecord);
	pFile->GetInteger("KG3DENGINE", "TrackCameraOrSimpleCamera", TRUE, &g_cEngineOption.bTrackCameraOrSimpleCamera);
	pFile->GetInteger("KG3DENGINE", "OutputUsedResource", FALSE, &g_cEngineOption.bOutputUsedResource);
	pFile->GetInteger("KG3DENGINE", "UseTerrainLOD"      , TRUE,&g_cEngineOption.bUseTerrainLOD);
	if (! pFile->GetMultiInteger("KG3DENGINE", "TerrainLODGates", g_cEngineOption.nTerrainLODGates, _countof(g_cEngineOption.nTerrainLODGates)) )
	{
		g_cEngineOption.nTerrainLODGates[0] = 11200;
		g_cEngineOption.nTerrainLODGates[1] = 16000;
		g_cEngineOption.nTerrainLODGates[2] = 22400;
	}

	if (! pFile->GetMultiFloat("KG3DENGINE", "ModelLODSwitchGates", g_cEngineOption.fModelLODSwitchGates
		, _countof(g_cEngineOption.fModelLODSwitchGates)) 
		|| g_cEngineOption.fModelLODSwitchGates[EM_LOD_HIGH_TO_MIDDLE] > g_cEngineOption.fModelLODSwitchGates[EM_LOD_MIDDLE_TO_LOW]
		|| g_cEngineOption.fModelLODSwitchGates[EM_LOD_MIDDLE_TO_LOW] < FLT_EPSILON)
	{
		g_cEngineOption.fModelLODSwitchGates[EM_LOD_HIGH_TO_MIDDLE] = 10000.0f * 10000.0f;
		g_cEngineOption.fModelLODSwitchGates[EM_LOD_MIDDLE_TO_LOW] =  20000.0f * 20000.0f;		
	}

	if(! pFile->GetMultiFloat("KG3DENGINE", "LODSwitchAngleGates", g_cEngineOption.fLODSwitchAngleGates
		, _countof(g_cEngineOption.fLODSwitchAngleGates))
		|| g_cEngineOption.fLODSwitchAngleGates[EM_LOD_HIGH_TO_MIDDLE] < g_cEngineOption.fLODSwitchAngleGates[EM_LOD_MIDDLE_TO_LOW]
		|| g_cEngineOption.fLODSwitchAngleGates[EM_LOD_MIDDLE_TO_LOW] < FLT_EPSILON)
	{
		g_cEngineOption.fLODSwitchAngleGates[EM_LOD_HIGH_TO_MIDDLE] = 6 * D3DX_PI / 180;
		g_cEngineOption.fLODSwitchAngleGates[EM_LOD_MIDDLE_TO_LOW] = 3 * D3DX_PI / 180;		
	}

    g_nAniEnableInterpolate = true;
    nRetCode = pFile->GetBool("KG3DENGINE", "AniEnableInterpolate", &g_nAniEnableInterpolate);
    //KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pFile->GetInteger("KG3DENGINE", "AniInterpolateFPS", 25, &g_nInterpolateFPS);
    //KGLOG_PROCESS_ERROR(nRetCode);
	KGLOG_PROCESS_ERROR(g_nInterpolateFPS >= 20 && "Config.ini, [KG3DENGINE]AniInterpolateFPS");

    nRetCode = pFile->GetInteger("KG3DENGINE", "AniMemExpireTime", 5, &g_nExpireTime);
    //KGLOG_PROCESS_ERROR(nRetCode);
	KGLOG_PROCESS_ERROR(g_nExpireTime >= 0 && "Config.ini, [KG3DENGINE]AniMemExpireTime");

    nRetCode = pFile->GetInteger("KG3DENGINE", "AniLimitMemSize", 50, &g_nAniLimitMemSize);
    //KGLOG_PROCESS_ERROR(nRetCode);
	KGLOG_PROCESS_ERROR(g_nAniLimitMemSize >= 20 && "Config.ini, [KG3DENGINE]AniLimitMemSize");

	//-------------------------------------------------------------------------
	// Load the Editor Engine Option
	pFile->GetInteger("ENGINEOPTION", "UseProgressMeshLOD", FALSE, &g_eEngineOption.bUseProgressMeshLOD);
	pFile->GetInteger("ENGINEOPTION", "UseMultiThreadLoad", FALSE, &g_eEngineOption.bUseMultiThreadLoad);
	g_eEngineOption.bUseMultiThreadLoad = TRUE;//强制多线程加载打开
	g_eEngineOption.bUseProgressMeshLOD = FALSE;

	pFile->GetInteger("ENGINEOPTION", "UseMultiThreadCull", FALSE, &g_eEngineOption.bUseMultiThreadCull);
	g_eEngineOption.bUseMultiThreadCull = FALSE;

	pFile->GetInteger("ENGINEOPTION", "nPackFile", 0, &g_nPackFile);
	pFile->GetInteger("ENGINEOPTION", "UseLogical", TRUE, &g_eEngineOption.bUseLogical);
	pFile->GetInteger("ENGINEOPTION", "UseLogicalObject", TRUE, &g_eEngineOption.bUseLogicalObject);
	pFile->GetInteger("ENGINEOPTION", "RenderTerrain", TRUE, &g_eEngineOption.bRenderTerrain);
	pFile->GetInteger("ENGINEOPTION", "RenderGround", TRUE, &g_eEngineOption.bRenderGround);
	pFile->GetInteger("ENGINEOPTION", "TerrDynamic", FALSE, &g_eEngineOption.bTerrDynamic);
	pFile->GetInteger("ENGINEOPTION", "ShowTestDialog", FALSE, &g_eEngineOption.bShowTestDlg);
	pFile->GetFloat("MODLEEDITOR", "ModelEditorLODDistance", 300.0f, &g_fModelRenderTwiceCullDistance);
/*
	pFile->GetFloat("ENGINEOPTION", "fBloomExposure1", 1.0f, &g_eEngineOption.fBloomExposure1);
	pFile->GetFloat("ENGINEOPTION", "fBloomExposure2", 0.3f, &g_eEngineOption.fBloomExposure2);
	pFile->GetFloat("ENGINEOPTION", "BloomRate", 0, &g_eEngineOption.fBloomRate);
	pFile->GetFloat("ENGINEOPTION", "BloomDelay", 300, &g_eEngineOption.fBloomDelay);
	pFile->GetFloat("ENGINEOPTION", "BloomGate", 0.2F, &g_eEngineOption.fBloomGate);
	pFile->GetFloat("ENGINEOPTION", "fBloomGaussCoefficient", 3.0f, &g_eEngineOption.fBloomGaussCoefficient);
	pFile->GetInteger("ENGINEOPTION", "nBloomSampleNum", 8, &g_eEngineOption.nBloomSampleNum);

    pFile->GetFloat("ENGINEOPTION", "fDetailPower", 1.f, &g_eEngineOption.fDetailPower);
    pFile->GetFloat("ENGINEOPTION", "fBlurPower", 0.f, &g_eEngineOption.fBlurPower);
    pFile->GetFloat("ENGINEOPTION", "fBlurDistance", 20000.f, &g_eEngineOption.fBlurDistance);

    pFile->GetInteger("ENGINEOPTION", "nGodRaySampleNum", 3, &g_eEngineOption.nGodRaySampleNum);
    pFile->GetFloat("ENGINEOPTION", "fGodRayDensity", 0.38f, &g_eEngineOption.fGodRayDensity);
    pFile->GetFloat("ENGINEOPTION", "fGodRayWeight", 0.1f, &g_eEngineOption.fGodRayWeight);
    pFile->GetFloat("ENGINEOPTION", "fGodRayDecay", 1.f, &g_eEngineOption.fGodRayDecay);
    pFile->GetFloat("ENGINEOPTION", "fGodRayExposure", 0.27f, &g_eEngineOption.fGodRayExposure);
    pFile->GetFloat("ENGINEOPTION", "fLimitAngle", D3DX_PI * 2, &g_eEngineOption.fLimitAngle);

    pFile->GetInteger("ENGINEOPTION", "nSSAOLevel", 1, &g_eEngineOption.nSSAOLevel);
    pFile->GetFloat("ENGINEOPTION", "fSSAOSampleRadius", 20.f, &g_eEngineOption.fSSAOSampleRadius);
    pFile->GetFloat("ENGINEOPTION", "fSSAODarkFact", 0.5f, &g_eEngineOption.fSSAODarkFact);
    pFile->GetFloat("ENGINEOPTION", "fBlurWeight", 0.5f, &g_eEngineOption.fBlurWeight);

    pFile->GetFloat("ENGINEOPTION", "fFogDensityV", 0.001f, &g_eEngineOption.fFogDensityV);
    pFile->GetFloat("ENGINEOPTION", "fSlopeThreshold", 0.1f, &g_eEngineOption.fSlopeThreshold);
    pFile->GetFloat("ENGINEOPTION", "fHeightFalloff", 0.004f, &g_eEngineOption.fHeightFalloff);
    pFile->GetMultiFloat("ENGINEOPTION", "vecFogcolor",(float*)(&g_eEngineOption.vecFogcolor),4);
*/
	TCHAR RetPath[MAX_PATH] = {0};
	pFile->GetString("ENGINEOPTION", "WaterConfigFile", "", RetPath, MAX_PATH);
	wsprintf(g_eEngineOption.szWaterConfigFile, "%s", RetPath);
/*
	pFile->GetFloat("ENGINEOPTION", "HueScale", 1.f, &g_eEngineOption.fHueScale);
	pFile->GetFloat("ENGINEOPTION", "HueBias", 0.f, &g_eEngineOption.fHueBias);
	pFile->GetFloat("ENGINEOPTION", "SaturationScale", 1.f, &g_eEngineOption.fSaturationScale);
	pFile->GetFloat("ENGINEOPTION", "SaturationBias", 0.f, &g_eEngineOption.fSaturationBias);
	pFile->GetFloat("ENGINEOPTION", "fIntensityScale", 1.f, &g_eEngineOption.fIntensityScale);
	pFile->GetFloat("ENGINEOPTION", "IntensityBias", 0.f, &g_eEngineOption.fIntensityBias);
	pFile->GetFloat("ENGINEOPTION", "ShockWavePower", 0.01f, &g_eEngineOption.fShockWavePower);
<<<<<<< KG3DEngineManager.cpp
*/
	/*pFile->GetFloat("ENGINEOPTION", "Softness", 2.0f, &g_eEngineOption.fSoftness);
	pFile->GetFloat("ENGINEOPTION", "Jitter", 1.0f, &g_eEngineOption.fJitter);*/


	pFile->GetInteger("ENGINEOPTION","ImportOldScene",FALSE,&g_cEngineOption.bImportOldScene);
	g_cEngineOption.bImportOldScene = TRUE;

	pFile->GetInteger("ENGINEOPTION", "StopSpecialElements", FALSE, &g_eEngineOption.bStopSpecialElements);


	//pFile->GetInteger("ENGINEOPTION", "bEnabledAreaShadow", FALSE, &g_eEngineOption.bEnableAreaShadow);
	pFile->GetInteger("ENGINEOPTION", "bEnableAutoReLoadTexture", TRUE, &g_eEngineOption.bEnableAutoReLoadTexture);
	pFile->GetInteger("ENGINEOPTION", "bShow3DEngineDebugInfo", FALSE, &g_bShow3DEngineDebugInfo);
	SetRenderOption(RENDER_INFO,g_bShow3DEngineDebugInfo);
	SetRenderOption(RENDER_VIDEO_MEMERY_INFO,g_bShow3DEngineDebugInfo);
	pFile->GetInteger("ENGINEOPTION", "bMultiThreadLoadAnimation", FALSE, &g_bMultiThreadLoadAnimation);
	pFile->GetInteger("ENGINEOPTION", "b3DVision", FALSE, &g_b3DVision);

    pFile->GetInteger("VIM3D", "UseVIM3D", FALSE,&g_bUseVIM3D);


	//////////////////////////////////////////////////////////////////////////
	pFile->GetString("SCENEEDITOR","RUNNERMODEL","Data\\public\\scenerunner\\scenerunner.Mesh",g_szSceneRunnerModelName,MAX_PATH);
	pFile->GetString("SCENEEDITOR","RUNNERMTL","Data\\public\\scenerunner\\scenerunner.Mtl",g_szSceneRunnerMtlName,MAX_PATH);
	pFile->GetString("SCENEEDITOR","RUNNERANI","Data\\public\\scenerunner\\scenerunner.Ani",g_szSceneRunnerAniName,MAX_PATH);
	pFile->GetString("SCENEEDITOR","RUNNERFACEMODEL","",g_szSceneRunnerFaceModelName,MAX_PATH);
	pFile->GetString("SCENEEDITOR","RUNNERFACEMTL","",g_szSceneRunnerFaceMtlName,MAX_PATH);	


	pFile->GetInteger("SCENEEDITOR","UseOtherWorkDirectory",FALSE,&g_bUseOtherDiretory);
	pFile->GetInteger("SCENEEDITOR","CreateTerrainLess128x128",FALSE,&g_bCanCreateTerrainLess128x128);//add by zhangzhixiong(2009.11.17)
	if(g_bUseOtherDiretory)
	{
		pFile->GetString("SCENEEDITOR","WorkDirectory",g_Def_AppDirectory,g_Def_WorkDirectory,MAX_PATH);
		g_SetRootPath(g_Def_WorkDirectory);
		BOOL bOK = SetCurrentDirectory(g_Def_WorkDirectory);
		KGLOG_PROCESS_ERROR(bOK);
	}

	pFile->GetInteger("KG3DENGINE", "UseSpeedTreeLod", 0, &g_cEngineOption.bUseSpeedTreeLod);
    pFile->GetFloat("KG3DENGINE", "fBillBoardCloudInureDistance", 100000000.f, &g_cEngineOption.fBillBoardCloudInureDistance);


	pFile->GetInteger("MAIN", "FlySpeedX", 20, &g_eEngineOption.nFlySpeedX);
	pFile->GetInteger("MAIN", "FlySpeedY", 20, &g_eEngineOption.nFlySpeedY);
	
	pFile->GetInteger("ENGINEOPTION", "EnableScaleOutput",0,&g_bEnableScaleOutput);
	pFile->GetInteger("ENGINEOPTION", "ScaleOutputSize",512,&g_nScaleOutputSize);
	g_nScaleOutputSize = max(128,g_nScaleOutputSize);
	pFile->GetInteger("ENGINEOPTION", "ScaleOutputSmooth",1,&g_bScaleOutputSmooth);

	pFile->GetInteger("ENGINEOPTION", "MDLRenderLimit",100,&g_nMDLRenderLimit);
	g_nMDLRenderLimit = max(10,g_nMDLRenderLimit);

	if((!g_bClient) && g_eEngineOption.bUseLogical)
		g_cEngineOption.bEnableDynamicUnload = FALSE;
    hr = ValidateEngineOption(g_cEngineOption);
    KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DEngineManager::SaveConfig()
{
	KSmartIniFile pFile ;

    PostRender::KG3DPostRenderEffectManager::StaticConfigSave();
    //------------------------------------------
	//--- Last Modify By FengBo 2007-9-5 10:00
	SaveEngineOptionConfig();
	SaveEngineEditorOptionConfig();
	//------------------------------------------

    pFile = g_OpenIniFile(g_szConfigFilePath, false, true);
	KGLOG_PROCESS_ERROR(pFile);

	pFile->WriteInteger("VIM3D", "UseVIM3D", g_bUseVIM3D);
	//////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
	pFile->WriteString("SCENEEDITOR","RUNNERMODEL",g_szSceneRunnerModelName);
	pFile->WriteString("SCENEEDITOR","RUNNERMTL",g_szSceneRunnerMtlName);
	pFile->WriteString("SCENEEDITOR","RUNNERANI",g_szSceneRunnerAniName);
	pFile->WriteString("SCENEEDITOR","RUNNERFACEMODEL",g_szSceneRunnerFaceModelName);
	pFile->WriteString("SCENEEDITOR","RUNNERFACEMTL",g_szSceneRunnerFaceMtlName);
	
	pFile->WriteInteger("SCENEEDITOR","UseOtherWorkDirectory",g_bUseOtherDiretory);
	pFile->WriteString("SCENEEDITOR","WorkDirectory",g_Def_WorkDirectory);	
	
	//////////////////////////////////////////////////////////////////////////

	pFile->WriteInteger("ENGINEOPTION","ImportOldScene",g_cEngineOption.bImportOldScene);
	//pFile->WriteInteger("ENGINEOPTION", "bEnabledAreaShadow", g_eEngineOption.bEnableAreaShadow);
	
	pFile->WriteMultiFloat("KG3DENGINE", "ModelLODSwitchGates", g_cEngineOption.fModelLODSwitchGates, _countof(g_cEngineOption.fModelLODSwitchGates));
	pFile->WriteMultiInteger("KG3DENGINE", "TerrainLODGates", g_cEngineOption.nTerrainLODGates, _countof(g_cEngineOption.nTerrainLODGates));
	pFile->WriteInteger("KG3DENGINE","UseTerrainLOD",g_cEngineOption.bUseTerrainLOD);
	pFile->WriteFloat("MODLEEDITOR", "ModelEditorLODDistance", g_fModelRenderTwiceCullDistance);
	pFile->WriteInteger("ENGINEOPTION", "bShow3DEngineDebugInfo",g_bShow3DEngineDebugInfo);
	pFile->WriteInteger("ENGINEOPTION", "bMultiThreadLoadAnimation",g_bMultiThreadLoadAnimation);
	pFile->WriteInteger("ENGINEOPTION", "b3DVision", g_b3DVision);
	pFile->WriteInteger("ENGINEOPTION", "EnableScaleOutput", g_bEnableScaleOutput);
	pFile->WriteInteger("ENGINEOPTION", "ScaleOutputSize", g_nScaleOutputSize);
	pFile->WriteInteger("ENGINEOPTION", "ScaleOutputSmooth", g_bScaleOutputSmooth);
	pFile->WriteInteger("ENGINEOPTION", "MDLRenderLimit", g_nMDLRenderLimit);

	pFile->Save(g_szConfigFilePath);

	return S_OK;

Exit0:
	return E_FAIL;
}

HRESULT KG3DEngineManager::SaveEngineOptionConfig()
{
	KSmartIniFile pFile ;

	pFile = g_OpenIniFile(g_szConfigFilePath, false, true);
	KGLOG_PROCESS_ERROR(pFile);

    pFile->WriteInteger("KG3DENGINE", "EnableDynamicUnload", g_cEngineOption.bEnableDynamicUnload);
	pFile->WriteInteger("KG3DENGINE", "EnableBladesSmoot", g_cEngineOption.bEnableBladesSmoot);
	pFile->WriteInteger("KG3DENGINE", "SampMipFilter",g_cEngineOption.nSampMipFilter);
	pFile->WriteInteger("KG3DENGINE", "SampMinFilter",g_cEngineOption.nSampMinFilter);
	pFile->WriteInteger("KG3DENGINE", "SampMagFilter",g_cEngineOption.nSampMagFilter);
	pFile->WriteInteger("KG3DENGINE", "MaxAnisotropy",g_cEngineOption.dwMaxAnisotropy);

	pFile->WriteInteger("KG3DENGINE", "TextureScale", g_cEngineOption.nTextureScale);

	pFile->WriteFloat("KG3DENGINE", "CammeraDistance",g_cEngineOption.fCameraDistance);
	pFile->WriteFloat("KG3DENGINE", "CammeraAngle",g_cEngineOption.fCameraAngle);

	//pFile->WriteInteger("KG3DENGINE", "UseRenderTwiceAlpha", g_cEngineOption.bEnableSpecialAlphaBlend);
	pFile->WriteInteger("KG3DENGINE", "GrassAnimation", g_cEngineOption.bGrassAnimation);
	pFile->WriteInteger("KG3DENGINE", "RenderGrass", g_cEngineOption.bRenderGrass);
	pFile->WriteInteger("KG3DENGINE", "GrassAlphaBlend", g_cEngineOption.bGrassAlphaBlend);
	pFile->WriteInteger("KG3DENGINE", "AmibentShaderOnly", g_cEngineOption.bAmbientShaderOnly);
	pFile->WriteInteger("KG3DENGINE", "PostRenderEffect", g_cEngineOption.bPostEffectEnable);

	pFile->WriteInteger("KG3DENGINE", "BloomEnable", g_cEngineOption.bBloomEnable);
	pFile->WriteInteger("KG3DENGINE", "bHDR", g_cEngineOption.bHDR);
    pFile->WriteInteger("KG3DENGINE", "bGodRay", g_cEngineOption.bGodRay);
    pFile->WriteInteger("KG3DENGINE", "bSSAO", g_cEngineOption.bSSAO);
	pFile->WriteInteger("KG3DENGINE", "bGlobalFog", g_cEngineOption.bGlobalFog);
	pFile->WriteInteger("KG3DENGINE", "bDOF", g_cEngineOption.bDOF);
	pFile->WriteInteger("KG3DENGINE", "DetailEnable", g_cEngineOption.bDetail);
	pFile->WriteInteger("KG3DENGINE", "MotionBlurEnable", g_cEngineOption.bMotionBlur);
	pFile->WriteInteger("KG3DENGINE", "WaterReflection", g_cEngineOption.bWaterReflection);
	pFile->WriteInteger("KG3DENGINE", "WaterRefraction", g_cEngineOption.bWaterRefraction);
	pFile->WriteInteger("KG3DENGINE", "OutputUsedResource", g_cEngineOption.bOutputUsedResource);

	pFile->WriteInteger("KG3DENGINE","WaterReflectEnvironment",g_cEngineOption.bWaterReflectEnvironment);
	pFile->WriteInteger("KG3DENGINE","WaterReflectGround",g_cEngineOption.bWaterReflectGround);
	pFile->WriteInteger("KG3DENGINE","WaterReflectNPC",g_cEngineOption.bWaterReflectNPC);
	pFile->WriteInteger("KG3DENGINE","WaterReflectEntity",g_cEngineOption.bWaterReflectEntity);

	pFile->WriteFloat("KG3DENGINE", "MinSoundRange", g_cEngineOption.fMinSoundRange);

	pFile->WriteInteger("KG3DENGINE", "ShockWaveEnable", g_cEngineOption.bShockWaveEnable);
	pFile->WriteInteger("KG3DENGINE", "HSIEnable", g_cEngineOption.bHSIEnable);
    pFile->WriteInteger("KG3DENGINE", "CurveCMYKEnable", g_cEngineOption.bCurveCMYK);
    pFile->WriteInteger("KG3DENGINE", "ActiveColorCurve", g_cEngineOption.nActiveCurveStype);

	//pFile->WriteInteger("KG3DENGINE", "ForceShaderModel", g_cEngineOption.nForceShaderModel);//这个不保存，不然很容易忘记恢复

	pFile->WriteInteger("KG3DENGINE", "UseSpeedTreeLod", g_cEngineOption.bUseSpeedTreeLod);
    pFile->WriteFloat("KG3DENGINE", "fBillBoardCloudInureDistance", g_cEngineOption.fBillBoardCloudInureDistance);
	_ASSERTE(g_cEngineOption.ModelShadowType >= EM_MODEL_SHADOW_TYPE_NONE && g_cEngineOption.ModelShadowType < EM_MODEL_SHADOW_TYPE_COUNT);
	pFile->WriteInteger("KG3DENGINE", "ModelShadowType", (INT)g_cEngineOption.ModelShadowType);
	pFile->WriteMultiFloat("KG3DENGINE", "ModelShadowParams", g_cEngineOption.ModelShadowParams, 8);

	pFile->WriteInteger("KG3DENGINE", "EnableDynamicConverMap",g_cEngineOption.bEnableDynamicConverMap);
	pFile->WriteInteger("KG3DENGINE", "CpuSkin"               , g_cEngineOption.bCpuSkin);
	pFile->WriteInteger("KG3DENGINE", "NumCpuThread"          , g_cEngineOption.nNumCpuThread);
	pFile->WriteInteger("KG3DENGINE", "FlexBodySmooth"        , g_cEngineOption.bFlexBodySmooth);
	pFile->WriteInteger("KG3DENGINE", "CpuProcessModelThreadLock", g_cEngineOption.bCpuProcessModelThreadLock);
	pFile->WriteInteger("KG3DENGINE", "EnablePhysics", g_cEngineOption.bEnablePhysics);
	pFile->WriteInteger("KG3DENGINE", "EnableDefaultDynamicWeather", g_cEngineOption.bEnableDefaultDynamicWeather);
	pFile->WriteInteger("KG3DENGINE", "MultiTextureRender"    , g_cEngineOption.bMultiTextureRender);
	pFile->WriteInteger("KG3DENGINE", "TerrainDetail"         , g_cEngineOption.nTerrainDetail);
	pFile->WriteInteger("KG3DENGINE", "VegetationDensity"          , g_cEngineOption.nVegetationDensity);
	pFile->WriteInteger("KG3DENGINE", "WaterDetail"          , g_cEngineOption.nWaterDetail);
    pFile->WriteFloat("KG3DENGINE", "fWaterAlphaMin"          , g_cEngineOption.fWaterAlphaMin);
    pFile->WriteFloat("KG3DENGINE", "fWaterReduHeight"          , g_cEngineOption.fWaterReduHeight);
    pFile->WriteInteger("KG3DENGINE", "nWaterReflTextSize"          , g_cEngineOption.nWaterReflTextSize);
    pFile->WriteInteger("KG3DENGINE", "nWaterRefrTextSize"          , g_cEngineOption.nWaterRefrTextSize);
	pFile->WriteInteger("KG3DENGINE", "EnableNormalMap"      , g_cEngineOption.bEnableNormalMap);
	pFile->WriteInteger("KG3DENGINE", "EnableGroundNormalMap"      , g_cEngineOption.bEnableGroundNormalMap);
	pFile->WriteInteger("KG3DENGINE", "EnableSpeedTreeNormalMap"      , g_cEngineOption.bEnableSpeedTreeNormalMap);
	pFile->WriteInteger("KG3DENGINE", "EnableTimeOptimizeLog"      , g_cEngineOption.bEnableTimeOptimizeLog);
	pFile->WriteInteger("KG3DENGINE", "EnableDebugBBox"      , g_cEngineOption.bEnableDebugBBox);
    pFile->WriteInteger("KG3DENGINE", "bUseModelBsp", g_cEngineOption.bUseModelBsp);
	pFile->WriteInteger("KG3DENGINE", "bEnableModelLod", g_cEngineOption.bEnableModelLod);

	pFile->WriteMultiFloat("KG3DENGINE", "DefautColorCast0", (float*)(&(g_cEngineOption.ColorCast[0])), 4);
	pFile->WriteMultiFloat("KG3DENGINE", "DefautColorCast1", (float*)(&(g_cEngineOption.ColorCast[1])), 4);
	pFile->WriteMultiFloat("KG3DENGINE", "DefautColorCast2", (float*)(&(g_cEngineOption.ColorCast[2])), 4);
	pFile->WriteMultiFloat("KG3DENGINE", "DefautColorCast3", (float*)(&(g_cEngineOption.ColorCast[3])), 4);
	pFile->WriteMultiFloat("KG3DENGINE", "DefautColorCast4", (float*)(&(g_cEngineOption.ColorCast[4])), 4);
	pFile->WriteMultiFloat("KG3DENGINE", "DefautColorCast5", (float*)(&(g_cEngineOption.ColorCast[5])), 4);
	pFile->WriteMultiFloat("KG3DENGINE", "DefautColorCast6", (float*)(&(g_cEngineOption.ColorCast[6])), 4);
	pFile->WriteMultiFloat("KG3DENGINE", "DefautColorCast7", (float*)(&(g_cEngineOption.ColorCast[7])), 4);

	pFile->WriteInteger("KG3DENGINE", "EnableRecord"      , g_cEngineOption.bEnableRecord);
    pFile->WriteInteger("KG3DENGINE", "ForceShaderModel", g_cEngineOption.nForceShaderModel);

	pFile->Save(g_szConfigFilePath);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DEngineManager::SaveEngineEditorOptionConfig()
{
	KSmartIniFile pFile ;

    pFile = g_OpenIniFile(g_szConfigFilePath, false, true);
	KGLOG_PROCESS_ERROR(pFile);

	pFile->WriteInteger("ENGINEOPTION", "ShowTestDialog", g_eEngineOption.bShowTestDlg);
	pFile->WriteInteger("ENGINEOPTION", "UseMultiThreadLoad", g_eEngineOption.bUseMultiThreadLoad);
	pFile->WriteInteger("ENGINEOPTION", "UseMultiThreadCull", g_eEngineOption.bUseMultiThreadCull);
	pFile->WriteInteger("ENGINEOPTION", "UseProgressMeshLOD", g_eEngineOption.bUseProgressMeshLOD);
	pFile->WriteInteger("ENGINEOPTION", "UseLogical", g_eEngineOption.bUseLogical);
	pFile->WriteInteger("ENGINEOPTION", "UseLogicalObject", g_eEngineOption.bUseLogicalObject);
	pFile->WriteInteger("ENGINEOPTION", "RenderTerrain", g_eEngineOption.bRenderTerrain);
	pFile->WriteInteger("ENGINEOPTION", "RenderGround", g_eEngineOption.bRenderGround);
	pFile->WriteInteger("ENGINEOPTION", "TerrDynamic", g_eEngineOption.bTerrDynamic);
	pFile->WriteInteger("ENGINEOPTION", "nPackFile", g_nPackFile);
	//////////////////////////////////////////////////////////////////////////

    /*
	pFile->WriteFloat("ENGINEOPTION","fBloomExposure1",g_eEngineOption.fBloomExposure1);
	pFile->WriteFloat("ENGINEOPTION","fBloomExposure2",g_eEngineOption.fBloomExposure2);
	pFile->WriteFloat("ENGINEOPTION","BloomRate",g_eEngineOption.fBloomRate);
	pFile->WriteFloat("ENGINEOPTION","BloomDelay",g_eEngineOption.fBloomDelay);
	pFile->WriteFloat("ENGINEOPTION","BloomGate",g_eEngineOption.fBloomGate);
	pFile->WriteFloat("ENGINEOPTION","fBloomGaussCoefficient",g_eEngineOption.fBloomGaussCoefficient);
	pFile->WriteInteger("ENGINEOPTION","nBloomSampleNum",g_eEngineOption.nBloomSampleNum);

    pFile->WriteFloat("ENGINEOPTION","fDetailPower",g_eEngineOption.fDetailPower);
    pFile->WriteFloat("ENGINEOPTION","fBlurPower",g_eEngineOption.fBlurPower);
    pFile->WriteFloat("ENGINEOPTION","fBlurDistance",g_eEngineOption.fBlurDistance);

    pFile->WriteInteger("ENGINEOPTION", "nGodRaySampleNum", g_eEngineOption.nGodRaySampleNum);
    pFile->WriteFloat("ENGINEOPTION", "fGodRayDensity", g_eEngineOption.fGodRayDensity);
    pFile->WriteFloat("ENGINEOPTION", "fGodRayWeight", g_eEngineOption.fGodRayWeight);
    pFile->WriteFloat("ENGINEOPTION", "fGodRayDecay", g_eEngineOption.fGodRayDecay);
    pFile->WriteFloat("ENGINEOPTION", "fGodRayExposure", g_eEngineOption.fGodRayExposure);
    pFile->WriteFloat("ENGINEOPTION", "fLimitAngle", g_eEngineOption.fLimitAngle);

	pFile->WriteFloat("ENGINEOPTION", "fFogDensityV",g_eEngineOption.fFogDensityV);
    pFile->WriteFloat("ENGINEOPTION", "fSlopeThreshold",g_eEngineOption.fSlopeThreshold);
    pFile->WriteFloat("ENGINEOPTION", "fHeightFalloff", g_eEngineOption.fHeightFalloff);
	pFile->WriteMultiFloat("ENGINEOPTION", "vecFogcolor",(float*)(&g_eEngineOption.vecFogcolor),4);

    pFile->WriteInteger("ENGINEOPTION", "nSSAOLevel", g_eEngineOption.nSSAOLevel);
    pFile->WriteFloat("ENGINEOPTION", "fSSAOSampleRadius", g_eEngineOption.fSSAOSampleRadius);
    pFile->WriteFloat("ENGINEOPTION", "fSSAODarkFact", g_eEngineOption.fSSAODarkFact);
    pFile->WriteFloat("ENGINEOPTION", "fBlurWeight", g_eEngineOption.fBlurWeight);
*/


    pFile->WriteString("ENGINEOPTION","WaterConfigFile", g_eEngineOption.szWaterConfigFile);


    /*
	pFile->WriteFloat("ENGINEOPTION","HueScale", g_eEngineOption.fHueScale);
	pFile->WriteFloat("ENGINEOPTION","HueBias", g_eEngineOption.fHueBias);
	pFile->WriteFloat("ENGINEOPTION","SaturationScale", g_eEngineOption.fSaturationScale);
	pFile->WriteFloat("ENGINEOPTION","SaturationBias", g_eEngineOption.fSaturationBias);
	pFile->WriteFloat("ENGINEOPTION","fIntensityScale", g_eEngineOption.fIntensityScale);
	pFile->WriteFloat("ENGINEOPTION","IntensityBias", g_eEngineOption.fIntensityBias);
	pFile->WriteFloat("ENGINEOPTION","ShockWavePower", g_eEngineOption.fShockWavePower);
<<<<<<< KG3DEngineManager.cpp
    */


	//pFile->WriteFloat("ENGINEOPTION", "IntensityBias", g_eEngineOption.fIntensityBias);
	//pFile->WriteFloat("ENGINEOPTION", "ShockWavePower", g_eEngineOption.fShockWavePower);
	pFile->WriteInteger("ENGINEOPTION", "bEnableAutoReLoadTexture",g_eEngineOption.bEnableAutoReLoadTexture);

	pFile->Save(g_szConfigFilePath);

	return S_OK;
Exit0:
	return E_FAIL;
}

//(by dengzhihui Refactor 2006年11月2日
HRESULT KG3DEngineManager::SetFocusScene(IKG3DScene* pScene)
{
	return SetFocusScene((KG3DScene*)pScene);
}

HRESULT KG3DEngineManager::SetMaxViewScene(IKG3DScene* pScene)
{
	return SetMaxViewScene((KG3DScene*)pScene);
}

HRESULT KG3DEngineManager::GetUseSO3Logical(BOOL* pUseLogic)
{
	//	*pUseLogic = g_bUseSO3Logical;
	*pUseLogic = g_eEngineOption.bUseLogical; //Modify by FengBo 2007-9-5
	return S_OK;	
}
HRESULT KG3DEngineManager::GetUseLogicObject(BOOL* pUseLogicObject)
{
	//	*pUseLogic = g_bUseSO3Logical;
	*pUseLogicObject = g_eEngineOption.bUseLogicalObject; 
	return S_OK;	
}
HRESULT KG3DEngineManager::GetGraphicsTool(IEKG3DGraphicsTool** pTool)
{
	_ASSERTE(pTool);
	if (pTool)
	{
		extern KG3DGraphicsTool g_cGraphicsTool;
		*pTool = &g_cGraphicsTool;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DEngineManager::GetRepresentObjectTable(IKG3DResourceManager** ppObjectTable)
{
	_ASSERTE(ppObjectTable);
	if(ppObjectTable)
	{
		*ppObjectTable = &g_GetObjectTable();
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DEngineManager::GetIEKG3DTextureTable(IEKG3DTextureTable **ppTexture)
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(ppTexture);
	*ppTexture = &g_cTextureTable;
	hResult = S_OK;
Exit0:
	return hResult;

}
HRESULT KG3DEngineManager::GetIEKG3DModelTable(IEKG3DModelTable **ppModelTable)
{

	*ppModelTable = &g_cModelTable;
	return S_OK;
}

HRESULT KG3DEngineManager::GetIEKG3DClipTable(IEKG3DClipTable **ppClipTable)
{
	*ppClipTable = &g_cClipTable;
	return S_OK;
}
HRESULT KG3DEngineManager::GetNewIEKG3DMaterial(IEKG3DMaterial **ppMat)
{
	HRESULT hResult = E_FAIL;
	KGLOG_PROCESS_ERROR(ppMat);

	*ppMat = new KG3DMaterial;
	KGLOG_PROCESS_ERROR(*ppMat);

	hResult = S_OK;
Exit0:
	return hResult;

}
HRESULT KG3DEngineManager::DestroyIEKG3DMaterial(IEKG3DMaterial *pMat)
{
	HRESULT hResult = E_FAIL;

	ASSERT(pMat);

	KG3DMaterial *pMaterial = dynamic_cast<KG3DMaterial *>(pMat);

	KGLOG_PROCESS_ERROR(pMaterial);
	delete pMaterial;

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DEngineManager::ExchangeModelSubsetID(IEKG3DModel* pIModel, int nSubset1, int nSubset2)
{
	HRESULT hr = E_FAIL;
	KG3DMesh *pMesh = NULL;
	KG3DMaterial *pMaterial = NULL;
	KG3DMaterial::KG3DMaterialSubset Subset;
	KG_PROCESS_ERROR(pIModel);

	//(by dengzhihui Refactor 2006年11月7日
	KG3DModel* pModel = static_cast<KG3DModel*>(pIModel);
	pMesh = pModel->GetMesh();
	//)
	KG_PROCESS_ERROR(NULL != pMesh);

	KG_PROCESS_ERROR((DWORD)nSubset1 < pMesh->m_dwNumSubset);
	KG_PROCESS_ERROR((DWORD)nSubset2 < pMesh->m_dwNumSubset);

	pMaterial = pModel->GetMaterial();
	KG_PROCESS_ERROR(pMaterial);

	//交换Mesh中的ID
	//带Skin的
	if (pMesh->m_pBoneCombinationTable)
	{
		LPD3DXBONECOMBINATION pBoneComb = 
			reinterpret_cast<LPD3DXBONECOMBINATION>(pMesh->m_pBoneCombinationTable->GetBufferPointer());

		for (int j = pMesh->m_dwNumAttributeGroups - 1; j >= 0; j--)
		{
			if (pBoneComb->AttribId == (DWORD)nSubset1)
			{
				pBoneComb->AttribId = nSubset2;
				continue;
			}

			if (pBoneComb->AttribId == (DWORD)nSubset2)
			{
				pBoneComb->AttribId = nSubset1;
				continue;
			}
		}
	}
	else
	{
		//不带skin的
		DWORD* pAtt = NULL;
		DWORD dwNumFaces = 0;

		KG_PROCESS_ERROR(pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]);

		dwNumFaces = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetNumFaces();
		pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->LockAttributeBuffer(0, (DWORD**)&pAtt);

		for (DWORD i = 0; i < dwNumFaces; i++)
		{
			if (pAtt[i] == (DWORD)nSubset1)
			{
				pAtt[i] = nSubset2;
				continue;
			}

			if (pAtt[i] == (DWORD)nSubset2)
			{
				pAtt[i] = nSubset1;
				continue;
			}
		}
		pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockAttributeBuffer();

		pMesh->OptimizeUnSkinedMesh();
	}

	//交换材质ID
	memcpy(&Subset, 
		pMaterial->GetSubMaterial(nSubset1), 
		sizeof(KG3DMaterial::KG3DMaterialSubset));

	memcpy(pMaterial->GetSubMaterial(nSubset1),
		pMaterial->GetSubMaterial(nSubset2),
		sizeof(KG3DMaterial::KG3DMaterialSubset));

	memcpy(pMaterial->GetSubMaterial(nSubset2), 
		&Subset, 
		sizeof(KG3DMaterial::KG3DMaterialSubset));

	memset(&Subset, 0, sizeof(KG3DMaterial::KG3DMaterialSubset));
	hr = S_OK;
Exit0:

	//SAFE_RELEASE(pMesh);//原来是带引用计数的，现在不带
	
	return hr;
}

HRESULT KG3DEngineManager::GetNewIEKG3DLight(IEKG3DLight **ppiLight)
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(ppiLight);

	*ppiLight = new KG3DLight;

	KGLOG_PROCESS_ERROR(*ppiLight);

	hResult = S_OK;
Exit0:
	return hResult;
}
HRESULT KG3DEngineManager::GetBaseWnd(HWND *phBaseWnd)
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(phBaseWnd);

	*phBaseWnd = g_hBaseWnd;

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DEngineManager::GetDefWorkDirectory(LPTSTR pszBuf, DWORD dwBufSizeInCharCount)
{
	HRESULT hrResult  = E_FAIL;
	int nRetCode = false;

	KGLOG_PROCESS_ERROR(pszBuf);
	KGLOG_PROCESS_ERROR(dwBufSizeInCharCount > 1);

	nRetCode = _sntprintf(pszBuf, dwBufSizeInCharCount - 1, "%s", g_Def_WorkDirectory);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	pszBuf[dwBufSizeInCharCount - 1] = '\0';

	hrResult = S_OK;
Exit0:
	return hrResult;
}
//void KG3DEngineManager::LPCTSTR2IntInRange(LPCTSTR czSource, INT* pnDes, INT nLowerBound, INT nUpperBound)
//{
//	::LPCTSTR2IntInRange(czSource, pnDes, nLowerBound, nUpperBound);
//}
//D3DCOLOR KG3DEngineManager::ColorValue(D3DCOLORVALUE co)
//{
//	return ::ColorValue(co);
//}
//D3DCOLORVALUE KG3DEngineManager::Color2Value(D3DCOLOR Color)
//{
//	return ::Color2Value(Color);
//}
//void KG3DEngineManager::Color2Ref(const D3DCOLOR &D3DColor, COLORREF &CRColor)
//{
//	::Color2Ref(D3DColor, CRColor);
//}

HRESULT KG3DEngineManager::GetD3DDevice(LPDIRECT3DDEVICE9* ppDevice)
{
	_ASSERTE(ppDevice);

	if (ppDevice && g_pd3dDevice)
	{
		*ppDevice = g_pd3dDevice;
		return S_OK;
	}
	return E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DEngineManager::LoadPattern(LPCSTR strFileName, int& nIndex)
{
	return g_GrassPatternContainer.LoadPattern(strFileName, nIndex);
}

HRESULT KG3DEngineManager::FindPattern(LPCSTR strFileName, INT *pnRetIndex)
{
	return g_GrassPatternContainer.FindPattern(strFileName, pnRetIndex);
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DEngineManager::GetRepresentObjectPropertyCount(DWORD *pdwRetCount)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(pdwRetCount);

	*pdwRetCount = (DWORD)g_cObjectTemplate.m_vecObjectProperty.size();

	//Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DEngineManager::GetRepresentObjectProperty(
	DWORD dwIndex, OBJECT_PROPERTY **ppRetObjProperty
	)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(dwIndex < g_cObjectTemplate.m_vecObjectProperty.size());
	KGLOG_PROCESS_ERROR(ppRetObjProperty);

	*ppRetObjProperty = g_cObjectTemplate.m_vecObjectProperty[dwIndex];

	//Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DEngineManager::GetRepresentNPCPropertyCount(DWORD *pdwRetCount)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(pdwRetCount);

	*pdwRetCount = (DWORD)g_cNpeTemplate.m_vecObjectProperty.size();

	//Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DEngineManager::GetRepresentNPCProperty(
	DWORD dwIndex, NPC_PROPERTY **ppRetNPCProperty
	)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(dwIndex < g_cNpeTemplate.m_vecObjectProperty.size());
	KGLOG_PROCESS_ERROR(ppRetNPCProperty);

	*ppRetNPCProperty = g_cNpeTemplate.m_vecObjectProperty[dwIndex];

	//Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DEngineManager::GetEngineEditorOption(KG3DEngineEditorOption* pEngineEditorOption)
{
	KGLOG_PROCESS_ERROR(pEngineEditorOption);

	*pEngineEditorOption = g_eEngineOption;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DEngineManager::GetEngineOption(KG3DEngineOption* pEngineOption)
{
	KGLOG_PROCESS_ERROR(pEngineOption);

	*pEngineOption = g_cEngineOption;

	return S_OK;
Exit0:
	return E_FAIL;
}

int KG3DEngineManager::GetColorCurveStypeNum()
{
    return PostRender::KG3DPostRenderEffectManager::GetStypeCount();
}

void KG3DEngineManager::GetColorCurveStypeName(char szName[], int nIndex)
{
    PostRender::KG3DPostRenderEffectManager::GetStypeName(nIndex, szName);
}


KG3DEngineOption* KG3DEngineManager::GetEngineOptionPtr()
{
    return &g_cEngineOption;
}

HRESULT KG3DEngineManager::SetEngineOption(const KG3DEngineOption* pEngineOption)
{
   // HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KG3DEngineOption pKG3DEngineOption = *pEngineOption;

	KGLOG_PROCESS_ERROR(pEngineOption);

    ValidateAfterInitDeviceEngineOption(pKG3DEngineOption);

	if (pKG3DEngineOption.bAmbientShaderOnly != g_cEngineOption.bAmbientShaderOnly)
	{
		g_cEngineOption.bAmbientShaderOnly = pKG3DEngineOption.bAmbientShaderOnly;
		// To do..
	}
	if (pKG3DEngineOption.bGrassAnimation != g_cEngineOption.bGrassAnimation)
	{
		g_cEngineOption.bGrassAnimation = pKG3DEngineOption.bGrassAnimation;		
		// To do..
	}

    if (g_cEngineOption.nWaterDetail != pEngineOption->nWaterDetail)
    {
        g_cEngineOption.nWaterDetail = pEngineOption->nWaterDetail;
    }

    if (g_cEngineOption.fWaterAlphaMin != pEngineOption->fWaterAlphaMin)
    {
        g_cEngineOption.fWaterAlphaMin = pEngineOption->fWaterAlphaMin;
    }

    if (g_cEngineOption.fWaterReduHeight != pEngineOption->fWaterReduHeight)
    {
        g_cEngineOption.fWaterReduHeight = pEngineOption->fWaterReduHeight;
    }

    if (g_cEngineOption.nWaterReflTextSize != pEngineOption->nWaterReflTextSize)
    {
        g_cEngineOption.nWaterReflTextSize = pEngineOption->nWaterReflTextSize;
    }

    if (g_cEngineOption.nWaterRefrTextSize != pEngineOption->nWaterRefrTextSize)
    {
        g_cEngineOption.nWaterRefrTextSize = pEngineOption->nWaterRefrTextSize;
    }

    if (g_cEngineOption.bUseModelBsp != pEngineOption->bUseModelBsp)
        g_cEngineOption.bUseModelBsp = pEngineOption->bUseModelBsp;

	// 在bPostRender为false的时候应该无效，但是此逻辑部分留给UI
	if (pKG3DEngineOption.bMotionBlur != g_cEngineOption.bMotionBlur)
	{	
		g_cEngineOption.bMotionBlur = pKG3DEngineOption.bMotionBlur;	

		IEKG3DSceneOutputWnd* pOutWnd = NULL;
		for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
		{
			(*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
			if (pOutWnd == NULL)
			{
				continue;
			}
			pOutWnd->EnablePostEffect(POST_EFFECT_MOTIONBLUE, g_cEngineOption.bMotionBlur);
		}				
	}


    if (pKG3DEngineOption.ModelShadowType != g_cEngineOption.ModelShadowType 
		&& pKG3DEngineOption.ModelShadowType >= EM_MODEL_SHADOW_TYPE_NONE
		&& pKG3DEngineOption.ModelShadowType < EM_MODEL_SHADOW_TYPE_COUNT)
    {
		_ASSERTE(pKG3DEngineOption.ModelShadowType < _countof(g_cGraphicsEngine.GetCaps().IsShadowTypeAvailable));
		//改之前要检查能力标志位，否则不予更改
		if (g_cGraphicsEngine.GetCaps().IsShadowTypeAvailable[pKG3DEngineOption.ModelShadowType])
		{
			//if (pKG3DEngineOption.IsShadowMapEnable() != g_cEngineOption.IsShadowMapEnable())//<Shadow Sign>
			if(pKG3DEngineOption.ModelShadowType != g_cEngineOption.ModelShadowType)
			{			
				IEKG3DSceneOutputWnd* pOutWnd = NULL;
				for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
				{
					(*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
					if (pOutWnd == NULL)
					{
						continue;
					}
					BOOL bUseShadowMap = pKG3DEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW;
					pOutWnd->EnablePostEffect(POST_EFFECT_SHADOW, bUseShadowMap);//<Shadow Sign>
				}
			}
			g_cEngineOption.ModelShadowType = pKG3DEngineOption.ModelShadowType;			
		}
				
    }

	if (pKG3DEngineOption.bBloomEnable != g_cEngineOption.bBloomEnable)
	{	
		g_cEngineOption.bBloomEnable = pKG3DEngineOption.bBloomEnable;	

		IEKG3DSceneOutputWnd* pOutWnd = NULL;
		for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
		{
			(*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
			if (pOutWnd == NULL)
			{
				continue;
			}
			pOutWnd->EnablePostEffect(POST_EFFECT_BLOOM, g_cEngineOption.bBloomEnable);
		}				
	}	

	if (pKG3DEngineOption.bHDR != g_cEngineOption.bHDR)
	{	
		g_cEngineOption.bHDR = pKG3DEngineOption.bHDR;	

		IEKG3DSceneOutputWnd* pOutWnd = NULL;
		for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
		{
			(*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
			if (pOutWnd == NULL)
			{
				continue;
			}
			pOutWnd->EnablePostEffect(POST_EFFECT_HDR, g_cEngineOption.bHDR);
		}				
	}	

    if (pKG3DEngineOption.bGodRay != g_cEngineOption.bGodRay)
    {	
        g_cEngineOption.bGodRay = pKG3DEngineOption.bGodRay;	

        IEKG3DSceneOutputWnd* pOutWnd = NULL;
        for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
        {
            (*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
            if (pOutWnd == NULL)
            {
                continue;
            }
            pOutWnd->EnablePostEffect(POST_EFFECT_GODRAY, g_cEngineOption.bGodRay);
        }				
    }	

    if (pKG3DEngineOption.bSSAO != g_cEngineOption.bSSAO)
    {	
        g_cEngineOption.bSSAO = pKG3DEngineOption.bSSAO;	

        IEKG3DSceneOutputWnd* pOutWnd = NULL;
        for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
        {
            (*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
            if (pOutWnd == NULL)
            {
                continue;
            }
            pOutWnd->EnablePostEffect(POST_EFFECT_SSAO, g_cEngineOption.bSSAO);
        }				
    }	

	if (pKG3DEngineOption.bGlobalFog != g_cEngineOption.bGlobalFog)
    {	
        g_cEngineOption.bGlobalFog = pKG3DEngineOption.bGlobalFog;	

        IEKG3DSceneOutputWnd* pOutWnd = NULL;
        for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
        {
            (*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
            if (pOutWnd == NULL)
            {
                continue;
            }
            pOutWnd->EnablePostEffect(POST_EFFECT_FOG, g_cEngineOption.bGlobalFog);
        }				
    }	

	if (pKG3DEngineOption.bDOF != g_cEngineOption.bDOF)
	{	
		g_cEngineOption.bDOF = pKG3DEngineOption.bDOF;	

		IEKG3DSceneOutputWnd* pOutWnd = NULL;
		for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
		{
			(*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
			if (pOutWnd == NULL)
			{
				continue;
			}
			pOutWnd->EnablePostEffect(POST_EFFECT_DOF, g_cEngineOption.bDOF);
		}				
	}	

	if (pKG3DEngineOption.bShockWaveEnable != g_cEngineOption.bShockWaveEnable)
	{
		g_cEngineOption.bShockWaveEnable = pKG3DEngineOption.bShockWaveEnable;

		IEKG3DSceneOutputWnd* pOutWnd = NULL;
		for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
		{
			(*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
			if (pOutWnd == NULL)	
			{
				continue;
			}
			pOutWnd->EnablePostEffect(POST_EFFECT_SOCKWARE, g_cEngineOption.bShockWaveEnable);
		}		
	}
	if (pKG3DEngineOption.bHSIEnable != g_cEngineOption.bHSIEnable)
	{
		g_cEngineOption.bHSIEnable = pKG3DEngineOption.bHSIEnable;

		IEKG3DSceneOutputWnd* pOutWnd = NULL;
		for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
		{
			(*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
			if (pOutWnd == NULL)	
			{
				continue;
			}
			pOutWnd->EnablePostEffect(POST_EFFECT_HSI, g_cEngineOption.bHSIEnable);
		}				
	}

    if (pKG3DEngineOption.bCurveCMYK != g_cEngineOption.bCurveCMYK)
    {
        g_cEngineOption.bCurveCMYK = pKG3DEngineOption.bCurveCMYK;
        g_cEngineOption.nActiveCurveStype = pKG3DEngineOption.nActiveCurveStype;

        IEKG3DSceneOutputWnd* pOutWnd = NULL;
        for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
        {
            (*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
            if (pOutWnd == NULL)	
            {
                continue;
            }

            pOutWnd->EnablePostEffect(POST_EFFECT_CURVE_CMYK, g_cEngineOption.bCurveCMYK);
        }				
    }
    else if (g_cEngineOption.bCurveCMYK && g_cEngineOption.nActiveCurveStype != pKG3DEngineOption.nActiveCurveStype)
    {
        g_cEngineOption.nActiveCurveStype = pKG3DEngineOption.nActiveCurveStype;

        IEKG3DSceneOutputWnd* pOutWnd = NULL;
        for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
        {
            (*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
            if (pOutWnd == NULL)	
            {
                continue;
            }
            pOutWnd->EnablePostEffect(POST_EFFECT_CURVE_CMYK, g_cEngineOption.bCurveCMYK);
        }
    }

	if (g_cEngineOption.bPostEffectEnable != pKG3DEngineOption.bPostEffectEnable)
	{
		g_cEngineOption.bPostEffectEnable = pKG3DEngineOption.bPostEffectEnable;

		IEKG3DSceneOutputWnd* pOutWnd = NULL;

		for (m_iterListScene = m_listScene.begin(); m_iterListScene !=  m_listScene.end(); ++m_iterListScene)
		{
			(*m_iterListScene)->GetCurOutputWnd(&pOutWnd);
			if (pOutWnd == NULL)	
			{
				continue;
			}
			RECT r;
			pOutWnd->GetRect(&r);
			DWORD dwWidth = r.right - r.left;
			DWORD dwHeight =  r.bottom - r.top;				
			pOutWnd->ResetDevice(pKG3DEngineOption.bPostEffectEnable, dwWidth, dwHeight);
		}			
	}


	if (pKG3DEngineOption.bRenderGrass != g_cEngineOption.bRenderGrass)
	{
		// 草和石头的渲染行为是一致的
		g_cEngineOption.bRenderGrass = pKG3DEngineOption.bRenderGrass;
	}	

    SetRenderOption(RENDER_WATER_REFLECTION, g_cEngineOption.bWaterReflection);

	g_cEngineOption.bWaterReflectEnvironment = pKG3DEngineOption.bWaterReflectEnvironment;
	g_cEngineOption.bWaterReflectGround = pKG3DEngineOption.bWaterReflectGround;
	g_cEngineOption.bWaterReflectNPC = pKG3DEngineOption.bWaterReflectNPC;
	g_cEngineOption.bWaterReflectEntity = pKG3DEngineOption.bWaterReflectEntity;


	if (pKG3DEngineOption.nTextureScale != g_cEngineOption.nTextureScale)
	{
		if (pKG3DEngineOption.nTextureScale <= 0)
		{
			g_cEngineOption.nTextureScale = 2;
		}
		else
		{
			g_cEngineOption.nTextureScale = pKG3DEngineOption.nTextureScale;
		}		
		if(g_cEngineOption.nTextureScale ==3)
		{
			g_cEngineOption.nTextureScale =2;
		}
		if(g_cEngineOption.nTextureScale >=4)
		{
			g_cEngineOption.nTextureScale =4;
		}
		// To do..单纯标志，重启后生效
	}

	g_cEngineOption.bUseSpeedTreeLod = pKG3DEngineOption.bUseSpeedTreeLod;
    g_cEngineOption.fBillBoardCloudInureDistance = pKG3DEngineOption.fBillBoardCloudInureDistance;

    g_cEngineOption.bGrassAlphaBlend = pKG3DEngineOption.bGrassAlphaBlend;

    if (pKG3DEngineOption.fModelLODSwitchGates[EM_LOD_HIGH_TO_MIDDLE] < pKG3DEngineOption.fModelLODSwitchGates[EM_LOD_MIDDLE_TO_LOW] && 
        pKG3DEngineOption.fModelLODSwitchGates[EM_LOD_HIGH_TO_MIDDLE] > 0)
    {
        g_cEngineOption.fModelLODSwitchGates[EM_LOD_HIGH_TO_MIDDLE] = pKG3DEngineOption.fModelLODSwitchGates[EM_LOD_HIGH_TO_MIDDLE];
        g_cEngineOption.fModelLODSwitchGates[EM_LOD_MIDDLE_TO_LOW] = pKG3DEngineOption.fModelLODSwitchGates[EM_LOD_MIDDLE_TO_LOW];
    }

    if (pKG3DEngineOption.fModelLODSwitchGates[EM_LOD_HIGH_TO_MIDDLE] > pKG3DEngineOption.fModelLODSwitchGates[EM_LOD_MIDDLE_TO_LOW] && 
        pKG3DEngineOption.fModelLODSwitchGates[EM_LOD_MIDDLE_TO_LOW] > 0)
    {
        g_cEngineOption.fLODSwitchAngleGates[EM_LOD_HIGH_TO_MIDDLE] = pKG3DEngineOption.fLODSwitchAngleGates[EM_LOD_HIGH_TO_MIDDLE];
        g_cEngineOption.fLODSwitchAngleGates[EM_LOD_MIDDLE_TO_LOW] = pKG3DEngineOption.fLODSwitchAngleGates[EM_LOD_MIDDLE_TO_LOW];
    }
	g_cEngineOption.nTerrainLODGates[0] = pKG3DEngineOption.nTerrainLODGates[0];
	g_cEngineOption.nTerrainLODGates[1] = pKG3DEngineOption.nTerrainLODGates[1];
	g_cEngineOption.nTerrainLODGates[2] = pKG3DEngineOption.nTerrainLODGates[2];
	g_cEngineOption.bUseTerrainLOD = pKG3DEngineOption.bUseTerrainLOD;
    g_cEngineOption.fCameraDistance = pKG3DEngineOption.fCameraDistance;
    g_cEngineOption.fCameraAngle = pKG3DEngineOption.fCameraAngle;


    g_cEngineOption.nSampMinFilter = pKG3DEngineOption.nSampMinFilter;
    g_cEngineOption.nSampMagFilter = pKG3DEngineOption.nSampMagFilter;
    g_cEngineOption.nSampMipFilter = pKG3DEngineOption.nSampMipFilter;
	g_cEngineOption.dwMaxAnisotropy = pKG3DEngineOption.dwMaxAnisotropy;

    if (g_cEngineOption.nVegetationDensity != pKG3DEngineOption.nVegetationDensity)
        g_cEngineOption.nVegetationDensity = pKG3DEngineOption.nVegetationDensity;

    if (g_cEngineOption.nTerrainDetail != pKG3DEngineOption.nTerrainDetail)
        g_cEngineOption.nTerrainDetail = pKG3DEngineOption.nTerrainDetail;

    if (g_cEngineOption.bCpuSkin != pKG3DEngineOption.bCpuSkin)
        g_cEngineOption.bCpuSkin = pKG3DEngineOption.bCpuSkin;

    if (g_cEngineOption.bFlexBodySmooth != pKG3DEngineOption.bFlexBodySmooth)
        g_cEngineOption.bFlexBodySmooth = pKG3DEngineOption.bFlexBodySmooth;

    if (g_cEngineOption.bEnableDefaultDynamicWeather != pKG3DEngineOption.bEnableDefaultDynamicWeather)
        g_cEngineOption.bEnableDefaultDynamicWeather = pKG3DEngineOption.bEnableDefaultDynamicWeather;

    if (g_cEngineOption.bEnableDynamicConverMap != pKG3DEngineOption.bEnableDynamicConverMap)
        g_cEngineOption.bEnableDynamicConverMap = pKG3DEngineOption.bEnableDynamicConverMap;

    if (g_cEngineOption.nForceShaderModel != pKG3DEngineOption.nForceShaderModel)
        g_cEngineOption.nForceShaderModel = pKG3DEngineOption.nForceShaderModel;

    // 保存进config.ini文件
    this->SaveEngineOptionConfig();

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DEngineManager::ExecuteCommand(LPCTSTR szCommand)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(szCommand);

	//在客户端使用 本地指令，KG3DEngine.Exec3DCommand("xx") 就可以进来这里。本地指令在“聊”那里打开
    
	//具体的分析放在这个函数中，这样就不用和引擎搅合
	hrResult = g_ExecuteEngineCommand(szCommand);
Exit0:
    return hrResult;
}

HRESULT KG3DEngineManager::SetEngineEditorOption(KG3DEngineEditorOption* pEngineEditorOption)
{
	KGLOG_PROCESS_ERROR(pEngineEditorOption);

	if (g_eEngineOption.bRenderGround != pEngineEditorOption->bRenderGround)
	{
		g_eEngineOption.bRenderGround = pEngineEditorOption->bRenderGround;		
		// <1>To do..单纯标志，只需赋值，无受此标志影响的代码
	}
	if (g_eEngineOption.bRenderTerrain != pEngineEditorOption->bRenderTerrain)
	{
		g_eEngineOption.bRenderTerrain = pEngineEditorOption->bRenderTerrain;
	}
	if (g_eEngineOption.bShowTestDlg != pEngineEditorOption->bShowTestDlg)
	{
		g_eEngineOption.bShowTestDlg = pEngineEditorOption->bShowTestDlg;
		// <3>To do..单纯标志，只需赋值
	}
	if (g_eEngineOption.bTerrDynamic != pEngineEditorOption->bTerrDynamic)
	{
		g_eEngineOption.bTerrDynamic = pEngineEditorOption->bTerrDynamic;
		// <4>To do..单纯标志，只需赋值，没有受此标志影响的代码
	}
	if (g_eEngineOption.bUseLogical != pEngineEditorOption->bUseLogical)
	{
		g_eEngineOption.bUseLogical = pEngineEditorOption->bUseLogical;
		// <5>To do..单纯标志，只需赋值
	}
	if (g_eEngineOption.bUseLogicalObject != pEngineEditorOption->bUseLogicalObject)
	{
		g_eEngineOption.bUseLogicalObject = pEngineEditorOption->bUseLogicalObject;
		// <6>To do..单纯标志，只需赋值
	}
	if (g_eEngineOption.bUseMultiThreadLoad != pEngineEditorOption->bUseMultiThreadLoad)
	{
		g_eEngineOption.bUseMultiThreadLoad = pEngineEditorOption->bUseMultiThreadLoad;
		// <7>To do..单纯标志，只需赋值
	}
/*
	if (g_eEngineOption.fBloomGaussCoefficient != pEngineEditorOption->fBloomGaussCoefficient)
	{
		g_eEngineOption.fBloomGaussCoefficient = pEngineEditorOption->fBloomGaussCoefficient;
		// <8>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.nBloomSampleNum != pEngineEditorOption->nBloomSampleNum)
	{
		g_eEngineOption.nBloomSampleNum = pEngineEditorOption->nBloomSampleNum;
		// <9>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fBloomDelay != pEngineEditorOption->fBloomDelay)
	{
		g_eEngineOption.fBloomDelay = pEngineEditorOption->fBloomDelay;
		// <10>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fBloomGate != pEngineEditorOption->fBloomGate)
	{
		g_eEngineOption.fBloomGate = pEngineEditorOption->fBloomGate;
		// <11>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fBloomExposure1 != pEngineEditorOption->fBloomExposure1)
	{
		g_eEngineOption.fBloomExposure1 = pEngineEditorOption->fBloomExposure1;
		// <12>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fBloomExposure2 != pEngineEditorOption->fBloomExposure2)
	{
		g_eEngineOption.fBloomExposure2 =  pEngineEditorOption->fBloomExposure2;
		// <13>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fBloomRate != pEngineEditorOption->fBloomRate)
	{
		g_eEngineOption.fBloomRate = pEngineEditorOption->fBloomRate;
		// <14>To do..单纯参数，只需赋值
	}
    if (g_eEngineOption.fDetailPower != pEngineEditorOption->fDetailPower)
    {
        g_eEngineOption.fDetailPower = pEngineEditorOption->fDetailPower;
        // <15>To do..单纯参数，只需赋值
    }
    if (g_eEngineOption.fBlurPower != pEngineEditorOption->fBlurPower)
    {
        g_eEngineOption.fBlurPower = pEngineEditorOption->fBlurPower;
        // <16>To do..单纯参数，只需赋值
    }
    if (g_eEngineOption.fBlurDistance != pEngineEditorOption->fBlurDistance)
    {
        g_eEngineOption.fBlurDistance = pEngineEditorOption->fBlurDistance;
        // <17>To do..单纯参数，只需赋值
    }
	if (g_eEngineOption.fHueBias != pEngineEditorOption->fHueBias)
	{
		g_eEngineOption.fHueBias = pEngineEditorOption->fHueBias;
		// <18>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fHueScale != pEngineEditorOption->fHueScale)
	{
		g_eEngineOption.fHueScale = pEngineEditorOption->fHueScale;
		// <19>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fIntensityBias != pEngineEditorOption->fIntensityBias)
	{
		g_eEngineOption.fIntensityBias = pEngineEditorOption->fIntensityBias;
		// <20>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fIntensityScale != pEngineEditorOption->fIntensityScale)
	{
		g_eEngineOption.fIntensityScale = pEngineEditorOption->fIntensityScale;
		// <21>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fSaturationBias != pEngineEditorOption->fSaturationBias)
	{
		g_eEngineOption.fSaturationBias = pEngineEditorOption->fSaturationBias;
		// <22>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fSaturationScale != pEngineEditorOption->fSaturationScale)
	{
		g_eEngineOption.fSaturationScale = pEngineEditorOption->fSaturationScale;
		// <23>To do..单纯参数，只需赋值
	}
	if (g_eEngineOption.fShockWavePower != pEngineEditorOption->fShockWavePower)
	{
		g_eEngineOption.fShockWavePower = pEngineEditorOption->fShockWavePower;
		// <24>To do..单纯参数，只需赋值
	}
<<<<<<< KG3DEngineManager.cpp

    */


	//if (g_eEngineOption.fSoftness != pEngineEditorOption->fSoftness)
	//{
	//	g_eEngineOption.fSoftness = pEngineEditorOption->fSoftness;
	//	// <24>To do..单纯参数，只需赋值
	//}

	//if (g_eEngineOption.fJitter != pEngineEditorOption->fJitter)
	//{
	//	g_eEngineOption.fJitter = pEngineEditorOption->fJitter;
	//	// <24>To do..单纯参数，只需赋值
	//}
	if (_tcscmp(g_eEngineOption.szWaterConfigFile, pEngineEditorOption->szWaterConfigFile))
	{
		_tcscpy_s(g_eEngineOption.szWaterConfigFile, sizeof(pEngineEditorOption->szWaterConfigFile), 
			pEngineEditorOption->szWaterConfigFile);
		// <25>To do..单纯参数，只需赋值
	}
/*
    if (g_eEngineOption.nGodRaySampleNum != pEngineEditorOption->nGodRaySampleNum)
    {
        g_eEngineOption.nGodRaySampleNum = pEngineEditorOption->nGodRaySampleNum;
        // <26>To do..单纯参数，只需赋值
    }

    if (g_eEngineOption.fGodRayDensity != pEngineEditorOption->fGodRayDensity)
    {
        g_eEngineOption.fGodRayDensity = pEngineEditorOption->fGodRayDensity;
        // <27>To do..单纯参数，只需赋值
    }

    if (g_eEngineOption.fGodRayWeight != pEngineEditorOption->fGodRayWeight)
    {
        g_eEngineOption.fGodRayWeight = pEngineEditorOption->fGodRayWeight;
        // <28>To do..单纯参数，只需赋值
    }

    if (g_eEngineOption.nSSAOLevel != pEngineEditorOption->nSSAOLevel)
    {
        g_eEngineOption.nSSAOLevel = pEngineEditorOption->nSSAOLevel;
        // <29>To do..单纯参数，只需赋值
    }

    if (g_eEngineOption.fSSAOSampleRadius != pEngineEditorOption->fSSAOSampleRadius)
    {
        g_eEngineOption.fSSAOSampleRadius = pEngineEditorOption->fSSAOSampleRadius;
        // <30>To do..单纯参数，只需赋值
    }

    if (g_eEngineOption.fSSAODarkFact != pEngineEditorOption->fSSAODarkFact)
    {
        g_eEngineOption.fSSAODarkFact = pEngineEditorOption->fSSAODarkFact;
        // <31>To do..单纯参数，只需赋值
    }

	if (g_eEngineOption.fFogDensityV != pEngineEditorOption->fFogDensityV)
    {
        g_eEngineOption.fFogDensityV = pEngineEditorOption->fFogDensityV;
        // <32>To do..单纯参数，只需赋值
    } */
/*
	if (g_eEngineOption.fSlopeThreshold != pEngineEditorOption->fSlopeThreshold)
    {
        g_eEngineOption.fSlopeThreshold = pEngineEditorOption->fSlopeThreshold;
        // <33>To do..单纯参数，只需赋值
    }

	if (g_eEngineOption.fHeightFalloff != pEngineEditorOption->fHeightFalloff)
    {
        g_eEngineOption.fHeightFalloff = pEngineEditorOption->fHeightFalloff;
        // <34>To do..单纯参数，只需赋值
    } */
/*
	if (g_eEngineOption.vecFogcolor != pEngineEditorOption->vecFogcolor)
    {
        g_eEngineOption.vecFogcolor = pEngineEditorOption->vecFogcolor;
        // <35>To do..单纯参数，只需赋值
    }

    if (g_eEngineOption.fBlurWeight != pEngineEditorOption->fBlurWeight)
    {
        g_eEngineOption.fBlurWeight = pEngineEditorOption->fBlurWeight;
        // <36>To do..单纯参数，只需赋值
    }


    if (g_eEngineOption.fGodRayDecay != pEngineEditorOption->fGodRayDecay)
    {
        g_eEngineOption.fGodRayDecay = pEngineEditorOption->fGodRayDecay;
        // <37>To do..单纯参数，只需赋值
    }

    if (g_eEngineOption.fGodRayExposure != pEngineEditorOption->fGodRayExposure)
    {
        g_eEngineOption.fGodRayExposure = pEngineEditorOption->fGodRayExposure;
        // <38>To do..单纯参数，只需赋值
    }

    if (g_eEngineOption.fLimitAngle != pEngineEditorOption->fLimitAngle)
    {
        g_eEngineOption.fLimitAngle = pEngineEditorOption->fLimitAngle;
        // <39>To do..单纯参数，只需赋值
    }
*/
	this->SaveEngineEditorOptionConfig();

	return S_OK;
Exit0:
	return E_FAIL;
}


///////////////////////////////////////////////////////// End of Options
HRESULT KG3DEngineManager::GetAutoReloadTexture(BOOL *pbAutoReloadTexture)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(pbAutoReloadTexture);

	*pbAutoReloadTexture = g_bAutoReloadTexture;

	//Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}
HRESULT KG3DEngineManager::SetAutoReloadTexture(BOOL bAutoReloadTexture)
{
	g_bAutoReloadTexture = bAutoReloadTexture;
	return S_OK;
}
HRESULT KG3DEngineManager::SetFullScreen(BOOL bFullScreen)
{
	g_bFullScreen = bFullScreen;
	return S_OK;
}

HRESULT KG3DEngineManager::SetRenderTerrain(BOOL bRenderTerrain)
{
	//	g_bRenderTerrain = bRenderTerrain; Modify by Fengbo 2007-9-5
	g_eEngineOption.bRenderTerrain = bRenderTerrain;
	return S_OK;
}

BOOL KG3DEngineManager::GetRenderTerrain()
{
	//	return g_bRenderTerrain;
	return g_eEngineOption.bRenderTerrain;
}

HRESULT KG3DEngineManager::GetUI(IKG3DUI** pUI)
{
	extern IKG3DUI& g_GetUI();
	*pUI = &g_GetUI();
	return S_OK;
}
HRESULT KG3DEngineManager::GetSpeedTreeLeafScale(FLOAT *pfRetScale)
{
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pfRetScale);

	*pfRetScale = g_fSpeedTreeLeafScale;

	hrResult = S_OK;
Exit0:
	return hrResult;
}
HRESULT KG3DEngineManager::SetSpeedTreeLeafScale(FLOAT fScale)
{
	g_fSpeedTreeLeafScale = fScale;
	return S_OK;
}

HRESULT KG3DEngineManager::GetRenderTreeFlag(BOOL *pbRetRenderTree)
{
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pbRetRenderTree);

	*pbRetRenderTree = g_bRenderTree;

	hrResult = S_OK;
Exit0:
	return hrResult;
}
HRESULT KG3DEngineManager::SetRenderTreeFlag(BOOL bRenderTree)
{
	g_bRenderTree = bRenderTree;
	return S_OK;
}
HRESULT KG3DEngineManager::BuildKG3DRepresentObject(LPCTSTR lpFileName, DWORD dwType, DWORD_PTR wParam
										 , DWORD_PTR lParam, IEKG3DRepresentObject **ppObject)
{
	return KG3DRepresentObjectBuilder::Build(lpFileName, dwType, wParam, lParam, ppObject);
}

//HRESULT KG3DEngineManager::NewKG3DRepresentObjectInTable(IEKG3DRepresentObject **ppObject)
//{
//	HRESULT hResult = E_FAIL;
//	KG3DRepresentObject *pObject = NULL;
//
//	KGLOG_PROCESS_ERROR(ppObject);
//
//	g_GetObjectTable().Get1NewResource(&pObject,REPRESENTOBJECT_DEFAULT,0);
//	*ppObject = dynamic_cast<IEKG3DRepresentObject *>(pObject);
//
//	KGLOG_PROCESS_ERROR(*ppObject);
//	hResult = S_OK;
//Exit0:
//	return hResult;
//}

//HRESULT KG3DEngineManager::NewKG3DRepresentObjectSetInTable(IEKG3DRepresentObjectSet **ppObject)
//{
//	HRESULT hResult = E_FAIL;
//	KG3DRepresentObject *pObject = NULL;
//
//	KGLOG_PROCESS_ERROR(ppObject);
//
//	g_GetObjectTable().Get1NewResource((KG3DRepresentObject **)(&pObject),REPRESENTOBJECT_SET,0);
//	*ppObject = dynamic_cast<KG3DRepresentObjectSet*>(pObject);
//
//	KGLOG_PROCESS_ERROR(*ppObject);
//	hResult = S_OK;
//Exit0:
//	return hResult;
//}

//HRESULT KG3DEngineManager::NewKG3DRepresentObjectSoundBallInTable(IEKG3DRepresentObjectSoundBall **ppObject)
//{
//	HRESULT hResult = E_FAIL;
//	KG3DRepresentObject *pObject = NULL;
//
//	KGLOG_PROCESS_ERROR(ppObject);
//
//	g_GetObjectTable().Get1NewResource((KG3DRepresentObject **)(&pObject),REPRESENTOBJECT_SOUNDBALL,0);
//	*ppObject = dynamic_cast<IEKG3DRepresentObjectSoundBall*>(pObject);
//
//	KGLOG_PROCESS_ERROR(*ppObject);
//	hResult = S_OK;
//Exit0:
//	return hResult;
//}

HRESULT KG3DEngineManager::GetEnableAddEntity(BOOL **ppbEnableAddEntity)
{
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(ppbEnableAddEntity);

	*ppbEnableAddEntity = &g_bEnableAddEntity;

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DEngineManager::ChangeMovingState()
{
	return S_OK;
}

HRESULT KG3DEngineManager::GetKeyStateBuffer(BYTE** ppbyKeyStateBuf)
{
	_ASSERTE(ppbyKeyStateBuf);

	if(ppbyKeyStateBuf)
	{
		*ppbyKeyStateBuf = g_KeyState;
		return S_OK;
	}
	return E_FAIL;
}
HRESULT KG3DEngineManager::GetKG3DRTTITypeManager( IKG3DRTTITypeManager** pRTTI )///<RTTI_INTERFACE>
{
	if (!pRTTI)
	{
		return E_INVALIDARG;
	}
	IKG3DRTTITypeManager* pTemp = g_cGetRTTITypeManager();
	KG_PROCESS_ERROR(pTemp);
	*pRTTI = pTemp;
	return S_OK;
Exit0:
	return E_FAIL;
}

void KG3DEngineManager::RegistPushAlertMessageFunction(void (*fun)(const char[]))
{
    g_AddAlertMessage = fun;
}

BOOL KG3DEngineManager::GetAmibentShaderOnly()
{
	//	return g_bAmibentShaderOnly;
	return g_cEngineOption.bAmbientShaderOnly;
}

HRESULT KG3DEngineManager::SetAmibentShaderOnly(BOOL bValue)
{
	//	g_bAmibentShaderOnly = bValue;
	g_cEngineOption.bAmbientShaderOnly = bValue;
	return S_OK;
}
HRESULT KG3DEngineManager::GetRegionInfoManager( const char cszFileName[]
, unsigned uFileNameHash
, DWORD_PTR dwOption
, IKG3DRegionInfoManager** ppRIM )
{
	//unsigned int uOption = _kg_to_smaller_size_cast<unsigned int>(dwOption);
	KG_PROCESS_ERROR(ppRIM && cszFileName);
	return g_cRegionInfoManagerTable.LoadResourceFromFileEx(cszFileName, uFileNameHash, 0, dwOption, 0, (IKG3DResourceBase**)ppRIM);
Exit0:
	return E_FAIL;
}

HRESULT KG3DEngineManager::GetIEKG3DRegionInfoManagerTable( IEKG3DRegionInfoManagerTable** ppTable )
{
	if (ppTable)
	{
		*ppTable = &g_cRegionInfoManagerTable;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DEngineManager::GetIEInterface( DWORD dwType1, DWORD dwType2, DWORD_PTR wParam, DWORD_PTR lParam, LPVOID* ppInterface )
{
	return g_GetEngineIEInterface(*this, dwType1, dwType2, wParam, lParam, ppInterface);
}

HRESULT KG3DEngineManager::OnLostDevice()
{
    HRESULT hr = S_OK;

	for (list<KG3DScene*>::iterator it = m_listScene.begin(); it != m_listScene.end(); ++it)
    {
        KG3DScene* p3DScene = *it;

		hr = p3DScene->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

Exit0:
    return hr;
}

HRESULT KG3DEngineManager::OnResetDevice()
{
    HRESULT hr = S_OK;

	for (list<KG3DScene*>::iterator i = m_listScene.begin(); 
		i != m_listScene.end(); ++i)
    {
        KG3DScene* p3DScene = *i;

        hr = p3DScene->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

Exit0:
    return hr;
}

bool KG3DDeviceCallback::IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed)
{
    return true;
}

bool KG3DDeviceCallback::ModifyDeviceSettings(KG3DDeviceSettings const* pSettings, D3DCAPS9 const* pCaps)
{
    return true;
}

HRESULT KG3DDeviceCallback::OnCreateDevice(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC const* pBackBufferSurfaceDesc, D3DPRESENT_PARAMETERS const* pPresentParameters)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    int nInitUI = false;

    hr = g_GetUI().OnCreateDevice(pd3dDevice, pBackBufferSurfaceDesc, pPresentParameters);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nInitUI)
        {
            hr = g_GetUI().OnDestroyDevice();
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
    return hrResult;
}

HRESULT KG3DDeviceCallback::OnDestroyDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = g_GetUI().OnDestroyDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DDeviceCallback::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	g_GetSystemEventListener().OnD3DDeviceLost();

	hr = g_GetUI().OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cCaption2D.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	//g_Selector.OnLostDevice();

	// static instances

    hr = g_cGraphicsTool.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cEngineManager.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_BillBoardEffect.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cSceneTable.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_GetObjectTable().OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cModelTable.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cMeshTable.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cMaterialTable.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cTextureTable.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_ProjTextureRender.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KG3DSFXParticleContainer::OnStaticInvalidDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = KG3DModelSpeedTree::OnLostDeviceStatic();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = OnLostDeviceTerrainBlockStaticResource();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KG3DResourceDelayReleaseMgr::ReleaseAll();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DDeviceCallback::OnResetDevice(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC const* pBackBufferSurfaceDesc, 
                                          D3DPRESENT_PARAMETERS const* pPresentParameters)
{
	HRESULT hr = S_OK;
    HRESULT hrResult = E_FAIL;
    
	g_GetSystemEventListener().OnD3DDeviceReset();

    hr = g_BillBoardEffect.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cEngineManager.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = KG3DModelSpeedTree::OnResetDeviceStatic();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cGraphicsTool.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cSceneTable.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_GetObjectTable().OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cModelTable.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cMeshTable.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cMaterialTable.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTextureTable.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KG3DSFXParticleContainer::OnStaticRestoreDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = OnResetDeviceTerrainBlockStaticResource();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_ProjTextureRender.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cCaption2D.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	//g_Selector.OnResetDevice();

	hr = g_GetUI().OnResetDevice(pd3dDevice, pBackBufferSurfaceDesc, pPresentParameters);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        OnLostDevice();
    }
    return hrResult;
}


BOOL KG3DEngineManager::GetImportOldScene()
{
	return g_cEngineOption.bImportOldScene;
}
IKG3DFaceMotionSettings* KG3DEngineManager::GetFaceMotionSettings()
{
	return &g_FaceMotionSettings;
};

void KG3DEngineManager::ReloadAnimation()
{
	//g_cClipTable.Reload();
}

void KG3DEngineManager::ManipulateSceneList(IEKG3DScene* pTargetScene, BOOL bAdd)
{
	KG3DScene* pScene = dynamic_cast<KG3DScene*>(pTargetScene);
	if (bAdd)
		m_listScene.push_back(pScene);
	else
		m_listScene.remove(pScene);
}

KG3DSceneSceneEditor* KG3DEngineManager::GetMainScene()
{
    KG3DScene* pMainScene = NULL;

    list<KG3DScene*>::iterator i = m_listScene.begin();
    while (i != m_listScene.end())
    {
        if ((*i)->GetType() == SCENETYPE_SCENEEDITOR)
        {
            pMainScene = (*i);
            break;
        }
        ++i;
    }

    if (!pMainScene)
        return NULL;

    return pMainScene->GetSceneSceneEditor();
}

HRESULT KG3DEngineManager::NewKG3DRepresentObject( DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, IEKG3DRepresentObject **ppObject )
{
	if(NULL == ppObject)
		return E_INVALIDARG;
	KG3DRepresentObject* p = NULL;
	HRESULT hr = KG3DRepresentObjectBuilder::New(dwType, wParam, lParam, &p);
	if (SUCCEEDED(hr))
	{
		_ASSERTE(NULL != p);
		*ppObject = p;
	}
	return hr;
}

HRESULT KG3DEngineManager::SetRenderOption(RENDER_OPTION opt, BOOL bEnable)
{
	bEnable ? (m_RenderOption |= opt) : (m_RenderOption &= (~opt)); 

    switch (opt)
    {
    case RENDER_WATER_REFLECTION :
        g_cEngineOption.bWaterReflection = bEnable;
        if (GetMainScene())
            GetMainScene()->SetWaterReflect(bEnable);
        break;
    case RENDER_CPU_MODEL_PROCESS :
        {
            if (bEnable == g_cEngineOption.bCpuSkin)
                break;

            if (bEnable)
            {
                g_cMeshTable.ChangeMesh2CpuProcess();
                InitModelProcessTread();
            }
            else
                UnInitModelProcessTread();
            g_cEngineOption.bCpuSkin = bEnable;
        }
        break;
    default :
        break;
    }

	return S_OK;
}

BOOL KG3DEngineManager::GetRenderOption(RENDER_OPTION opt)
{
	return (m_RenderOption & opt) == opt;
}

HRESULT KG3DEngineManager::SetFrameMoveOption(FRAMEMOVE_OPTION opt, BOOL bEnable)
{
    bEnable ? (m_FrameMoveOption |= opt) : (m_FrameMoveOption &= (~opt)); 
    return S_OK;
}

BOOL KG3DEngineManager::GetFrameMoveOption(FRAMEMOVE_OPTION opt)
{
    return (m_FrameMoveOption & opt) == opt;
}

DWORD KG3DEngineManager::GetRenderParam(RENDER_PARAM Param)
{
    switch (Param)
    {
    case RENDER_PARAM_ZFAR :
        {
            KG3DSceneSceneEditor* pMainScene = GetMainScene();

            if (!pMainScene)
                break;

            return (DWORD)pMainScene->m_fZFar;
        }
        break;
    case RENDER_PARAM_CAMERA_ANGEL :
        {
            return (DWORD)D3DXToDegree(g_cEngineOption.fCameraAngle);
        }
        break;
    case RENDER_PARAM_MP_CPU_NUM :
        {
            return (DWORD)g_cEngineOption.nNumCpuThread;
        }
        break;
    default :
        break;
    }
    return 0;
}

HRESULT KG3DEngineManager::SetRenderParam(RENDER_PARAM Param, DWORD Value)
{
    switch (Param)
    {
    case RENDER_PARAM_ZFAR :
        {
            KG3DSceneSceneEditor* pMainScene = GetMainScene();

            if (!pMainScene)
                break;

            g_cEngineOption.fCameraDistance = (FLOAT)Value;
            pMainScene->m_fZFar = (FLOAT)Value;
        }
        break;
    case RENDER_PARAM_CAMERA_ANGEL :
        {
            g_cEngineOption.fCameraAngle = D3DXToRadian((float)Value);
        }
        break;
    case RENDER_PARAM_MP_CPU_NUM :
        {
            if (!g_cEngineOption.bCpuSkin || g_cEngineOption.nNumCpuThread == (int)Value)
                break;
            if (Value < 1 || Value > 8)
                break;
            UnInitModelProcessTread();
            g_cEngineOption.nNumCpuThread = (int)Value;
            InitModelProcessTread();
        }
        break;
    default :
        break;
    }
    return S_OK;
}

PostRenderParams* KG3DEngineManager::LockPostRenderParams(BOOL bReadOnly)
{
    PostRenderParams* pParams = &PostRender::KG3DPostRenderEffectManager::m_sEffectParams;
    m_bPostRenderParamsLockReadOnly = bReadOnly;   
    return pParams;
}

void KG3DEngineManager::UnlockPostRenderParams()
{
    // reserve
}

DWORD g_dwID2Find;
bool g_FindInReleaseList(IKG3DResourceBase *pRes)
{
	if (pRes->GetID() == g_dwID2Find)
		return true;
	return false;
}


TypeInfo g_GloableDefaultResourceType = { "", 0, FALSE };
#if defined(_DEBUG) | defined(DEBUG)
HRESULT g_KG3DEngineInterfaceTest( KG3DEngineManager* pEM )
{
	IEKG3DEngineManager* pIE = pEM;	//转换到接口
	KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
	//<TEST_CASE1>//检查IEGetInterface
	{
		LPVOID pRetPtr = NULL;

		//检查Env
		IEKG3DEnvironment* pEnv = NULL;
		HRESULT hr = pScene->GetEnvironment(&pEnv);
		if (SUCCEEDED(hr))
		{
			_ASSERTE(pEnv);

			struct TypeTestEnvEffect
			{
				TypeTestEnvEffect(IEKG3DEngineManager* pIE, IEKG3DEnvironment* pEnv, DWORD dwType)
				{
					HRESULT hr = E_FAIL;
					LPVOID pRetPtr = NULL;
					_ASSERTE(pEnv);
					IEKG3DEnvEffect* pEnvEffect = NULL;
					hr = pEnv->GetEnvEffect(dwType, 0, &pEnvEffect);
					if (SUCCEEDED(hr))
					{
						//检验得到抽象的KG3DEnvEffect
						hr = pIE->GetIEInterface(KG3DTYPE_ENV_EFFECT, dwType, NULL, NULL, &pRetPtr);
						_ASSERTE(SUCCEEDED(hr));
						_ASSERTE(pRetPtr == (LPVOID)pEnvEffect);

						//检验得到IE接口
						LPVOID pSkySystem = NULL;				
						hr = pEnvEffect->ToSolidInterface(&pSkySystem);
						_ASSERTE(SUCCEEDED(hr));
						hr = pIE->GetIEInterface(NULL, dwType, NULL, NULL, &pRetPtr);
						_ASSERTE(SUCCEEDED(hr));
						_ASSERTE(pRetPtr == (LPVOID)pSkySystem);
					}
				}
			};

			TypeTestEnvEffect(pIE, pEnv, KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM);
			TypeTestEnvEffect(pIE, pEnv, KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX);
			TypeTestEnvEffect(pIE, pEnv, KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN);
			TypeTestEnvEffect(pIE, pEnv, KG3DTYPE_ENVEFF_FOG);
			TypeTestEnvEffect(pIE, pEnv, KG3DTYPE_ENVEFF_LENSFLARE_SUN);
			TypeTestEnvEffect(pIE, pEnv, KG3DTYPE_ENVEFF_LENSFLARE_MOON);
		}
		//检查CameraMov
		{
			IEKG3DSceneCameraMovement* pCamMov = NULL;
			hr = pScene->GetCameraMovement(&pCamMov);
			if(SUCCEEDED(hr))
			{
				hr = pIE->GetIEInterface(NULL, KG3DTYPE_CAM_MOVEMENT, NULL, NULL, &pRetPtr);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(pRetPtr == (LPVOID)pCamMov);

				//检查CameraAni
				IEKG3DSceneCameraAnimation* pAni = NULL;
				hr = pCamMov->GetCurCamAnimation(&pAni);
				if (SUCCEEDED(hr))
				{
					hr = pIE->GetIEInterface(NULL, KG3DTYPE_CAM_ANIMATION, NULL, NULL, &pRetPtr);
					_ASSERTE(SUCCEEDED(hr));
					_ASSERTE(pRetPtr == (LPVOID)pAni);
				}
			}

		}
		//检查RIM
		{
			IEKG3DRegionInfoManager* pRIM = pScene->GetRegionDWInfoManager();
			if (pRIM)
			{
				hr = pIE->GetIEInterface(NULL, KG3DTYPE_REGION_INFO_MANAGER, NULL, NULL, &pRetPtr);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(pRetPtr == (LPVOID)pRIM);
			}
			IEKG3DRegionInfoManagerTable* pRIMTable = NULL;
			hr = pIE->GetIEKG3DRegionInfoManagerTable(&pRIMTable);
			_ASSERTE(SUCCEEDED(hr));
			hr = pIE->GetIEInterface(KG3DTYPE_RESOURCE_TABLE_TYPE, KG3DTYPE_REGION_INFO_MANAGER, NULL, NULL, &pRetPtr);
			_ASSERTE(SUCCEEDED(hr));
			_ASSERTE((LPVOID)pRIMTable == pRetPtr);
		}



	}//<TEST_CASE1>//检查IEGetInterface

	return S_OK;
}
#endif
#if defined(DEBUG) | defined(_DEBUG)
VOID KG3DEngineManager::RuntimeTest()
{
	//RenderOption
	{
		_ASSERTE(this->GetRenderOption(RENDER_ALL));
		RENDER_OPTION dwRenderOptionSave = RENDER_ALL;

		{
			_ASSERTE(this->GetRenderOption(RENDER_ENVIRONMENT));
			this->SetRenderOption(RENDER_ENVIRONMENT, FALSE);
			_ASSERTE(! this->GetRenderOption(RENDER_ENVIRONMENT));
			_ASSERTE(! this->GetRenderOption(RENDER_ALL));
			_ASSERTE(this->GetRenderOption(RENDER_ENVIRONMENT_TOPVIEW));
			this->SetRenderOption(RENDER_ENVIRONMENT, TRUE);
			_ASSERTE(this->GetRenderOption(RENDER_ENVIRONMENT));
			_ASSERTE(this->GetRenderOption(RENDER_ALL));
		}


		this->SetRenderOption(dwRenderOptionSave, TRUE);
		_ASSERTE(this->GetRenderOption(RENDER_ALL));
	}
}
#endif
HRESULT KG3DEngineManager::GetEngineCaps( KG3DEngineCaps* pCaps )
{
	if(NULL != pCaps)
	{
		*pCaps = g_cGraphicsEngine.GetCaps();
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DEngineManager::InitGameContext( KG3D_GAME_INIT_CONTEXT* pContext )
{
	KG3DTimeClock* pCreating = NULL;
	KGLOG_PROCESS_ERROR(NULL != pContext && _T("错误的游戏上下文初始化"));
	{
		//这个全局变量一直是const的，只有这里能特殊的使用const_cast语法来初始化一次
		const_cast<KG3DLogicalConstances&>(g_logicalConstants) = pContext->logicalConstance;	//逻辑的变量由这里传进来

		pCreating =  dynamic_cast<KG3DTimeClock*>(KG3DTimeClock::Build(NULL, reinterpret_cast<DWORD_PTR>(pContext), NULL));
		KG_PROCESS_ERROR(pCreating != NULL);

		SAFE_RELEASE(m_pTimeClock);
		m_pTimeClock = pCreating;
	}
	return S_OK;
Exit0:
	SAFE_RELEASE(pCreating);
	return E_FAIL;
}

HRESULT KG3DEngineManager::GetInterface( DWORD dwType, DWORD_PTR p1, DWORD_PTR p2, LPVOID* ppInterface )
{
	return g_GetEngineInterface(*this, dwType, p1, p2, ppInterface);	
}

KG3DTimeClock& KG3DEngineManager::GetTimeClock()
{
	if(NULL == m_pTimeClock)
	{
		m_pTimeClock = dynamic_cast<KG3DTimeClock*>(KG3DTimeClock::Build(NULL, NULL, NULL));	//这样即使外部没有传一个有效的GameContext也能正常运行

		_ASSERTE(NULL != m_pTimeClock);
		return *m_pTimeClock;
	}

	return *m_pTimeClock;
}
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DEngineManager)
{
	
}
KG_TEST_END(KG3DEngineManager)
#endif


void KG3DEngineManager::SetSoundFocusScene(IKG3DScene *pScene)
{
	m_pSoundFocusScene = pScene;
}

IKG3DScene* KG3DEngineManager::GetSoundFocusScene()
{
	return m_pSoundFocusScene;
}

IKG3DEnventCenter* KG3DEngineManager::GetEventCenter()
{
	return g_cClipTable.GetEventCenter();
}

ULONG KG3DEngineManager::IsInitFinished()
{
	return m_States.test(em_init_finished);
}

void KG3DEngineManager::GetHDFilePath(char szHD1Path[], char szHD2Path[], const char szRetPath[])
{
    char szTemp[MAX_PATH];

    strncpy(szTemp, szRetPath, sizeof(szTemp));

    *szHD1Path = 0;
    *szHD2Path = 0;

    char* p = szTemp;

    if (*p == '\\')
        ++p;

    strlwr(p);

    if (strstr(p, "data") != p)
        return;

    p += strlen("data");

    char* t = strrchr(p, '.');

    if (t)
        *t = 0;

    sprintf(szHD1Path, "hdata%s%s", p, "_n.tga");
    sprintf(szHD2Path, "hdata%s%s", p, "_d.tga");

    return;
}

BOOL KG3DEngineManager::NeedDrawTwice()
{
	BOOL bNeed = FALSE;
	if (g_cEngineOption.bPostEffectEnable && (g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW 
				|| g_cEngineOption.bDOF || g_cEngineOption.bDetail || g_cEngineOption.bSSAO || g_cEngineOption.bGlobalFog || g_cEngineOption.bMotionBlur))
		bNeed = TRUE;
	return bNeed;
}
HRESULT g_ReportResourceUse()
{
	g_cModelTable.ReportResourceUse("Model");
	g_cMeshTable.ReportResourceUse("Mesh");
	g_cMaterialTable.ReportResourceUse("Material");
	g_cTextureTable.ReportResourceUse("Texture");
	g_cClipTable.ReportResourceUse();
	return S_OK;
}

IKG3DEngineOptionCaps* KG3DEngineManager::CreateIKG3DEngineOptionCaps()
{
    KG3DEngineOptionCaps* pEngineOptionCaps = NULL;

    pEngineOptionCaps = new KG3DEngineOptionCaps;
    KG_PROCESS_ERROR(pEngineOptionCaps);

    m_listEngineOptionCaps.push_back(pEngineOptionCaps);
    
Exit0:
    return (IKG3DEngineOptionCaps*)pEngineOptionCaps;
}

void KG3DEngineManager::RemoveKG3DEngineOptionCaps(IKG3DEngineOptionCaps* pIEngineOptionCaps)
{
    list<KG3DEngineOptionCaps*>::iterator itBegin;
    list<KG3DEngineOptionCaps*>::iterator itEnd;
    KG3DEngineOptionCaps* pEngineOptionCaps = NULL;
    KG_PROCESS_ERROR(pIEngineOptionCaps);

    itBegin = m_listEngineOptionCaps.begin();
    itEnd   = m_listEngineOptionCaps.end();
    pEngineOptionCaps = (KG3DEngineOptionCaps*)pIEngineOptionCaps;

    for (; itBegin != itEnd; ++itBegin)
    {
        if ((*itBegin) == pEngineOptionCaps)
        {
            SAFE_DELETE(pEngineOptionCaps);
            m_listEngineOptionCaps.erase(itBegin);
            break;
        }
    }

Exit0:
    return;
}

void KG3DEngineManager::UnintEngineOptionCaps()
{
    list<KG3DEngineOptionCaps*>::iterator itBegin = m_listEngineOptionCaps.begin();
    list<KG3DEngineOptionCaps*>::iterator itEnd   = m_listEngineOptionCaps.end();

    for (; itBegin != itEnd; ++itBegin)
    {
        KG3DEngineOptionCaps* pEngineOptionCaps = *itBegin;
        SAFE_DELETE(pEngineOptionCaps);
    }

    m_listEngineOptionCaps.clear();
}