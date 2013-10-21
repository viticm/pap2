#pragma once

#include "..\..\..\include\KG3DEngine\KG3DInterface.h"
#include "kg3dfilefinder.h"
#include "KG3DRepresentNPCTemplate.h"
#include "Scene/KG3DSceneWndManager.h"

#include "../EditInterface/IEOther.h" //by dengzhihui Refactor 2006年11月2日

#include "KG3DEditMoveGrid.h"
#include "KG3DEditRotationGrid.h"
#include "KG3DEditScaleGrid.h"
#include "KG3DEngineOptionCaps.h"

using namespace std;

#define MULTITRHEAD_NONE      0
#define MULTITRHEAD_ANIFINISH 1
#define MULTITRHEAD_TEXTURELOADFINISH 2
#define MULTITRHEAD_MESHLOADFINISH 3

class KG3DScene;
class IKG3DUI;
class IIPPCodec;
class KG3DPhysiscManager;
struct IKG3DSoundShell;
class KG3DTimeClock;

class TimeControl
{
protected:
    float m_fSpeedBack;
    float m_fSpeedDest;
    float m_fSpeedIncrease;
private:
    UINT m_FrameCount;//注意类型，UINT_MAX在32位系统下面，以一秒60帧算，也能运行19884小时不溢出，够用了
public:
    DWORD m_dwNowTime;
    DWORD m_dwLastTime;

    DWORD Now;
    DWORD Lst;
    float m_fSpeed;
    DWORD m_dwTimeGetTimeLast;
    DWORD m_dwInterval;

    DWORD m_dwLastAppTime;
    float m_fScaling;

    TimeControl();
    void FrameMove(BOOL bSaveToFile);

    void SetScaling(float fScal)
    {
        m_fScaling = fScal;
    }

    DWORD GetNowTime()
    {
        return m_dwNowTime;
    }

    float GetTinyTime()
    {
        return (float)(m_dwNowTime - m_dwLastTime);
    }

    DWORD GetTinyTimeOriginal()
    {
        return m_dwNowTime - m_dwLastTime;
    }

    void Pause()
    {
        m_fSpeedBack = m_fSpeed;
        m_fSpeed = 0;
    }
    void Restore()
    {
        m_fSpeed = m_fSpeedBack;
    }
    void SetDestSpeed(float Value,float Increate = 0.1f)
    {
        m_fSpeedIncrease = Increate;
        m_fSpeedDest = Value;
    }
    UINT GetFrameCount()const {return m_FrameCount;}
};

class KG3DEngineManager : public IEKG3DEngineManager //by dengzhihui Refactor 2006年11月2日
{
private:

	HINSTANCE	m_hSoundDll;//声音模块句柄
	BOOL		m_MovieReady;
	BOOL        m_bRecordReady;

	list<KG3DScene*> m_listScene;
	//------------------------------------------------------
	// 为方便迭代场景，定义迭代器
	typedef list<KG3DScene*>::iterator IterListScene;	// Add by FengBo
	IterListScene			m_iterListScene;
	//------------------------------------------------------
	KG3DScene*         m_lpFocusScene;
    //----------------------------------------------------

public:
	BOOL m_bRenderFocusSceneOnly;
	KG3DFileFinder m_FileFinder;

	KG3DEngineManager();
	virtual ~KG3DEngineManager();

	KG3DPhysiscManager* m_pPhysicsManager;

	virtual IRecPlayer* GetNewRecPlayer();
	virtual RecordOption* GetRecordOption();
	virtual void SetRecordOption(const RecordOption *pOption);

public:
	virtual IKG3DEnventCenter* GetEventCenter();

	virtual IKG3DFaceMotionSettings* GetFaceMotionSettings();
	virtual BOOL GetImportOldScene(); //获取是否加载旧地形的标志

	virtual HRESULT Init(unsigned uMode, unsigned int uFlag, HWND hBaseWindow, HWND hRenderWindow); // uMode = 0 : windowed, uMode = 1: fullscreen
	virtual HRESULT UnInit();

    virtual KG3DScene* GetFocusScene() { return m_lpFocusScene; };

    virtual IKG3DResourceManager* Get3DModelTable();
	virtual IKG3DSoundBallTable* GetSoundBallTable();
    virtual IKG3DUI* Get3DUI();
	virtual LPDIRECT3DDEVICE9 GetDevice();
	virtual LPDIRECT3D9	GetD3D();

    virtual void GetHDFilePath(char szHD1Path[], char szHD2Path[], const char szRetPath[]);

	//////////////////////////////////////////////////////////////////////////
	//消息处理
	virtual HRESULT ProcessMessage(unsigned message, WPARAM uParam, LPARAM ulParam);//<PROCESS_MESSAGE>
	//渲染
	virtual HRESULT FrameMove();
	virtual HRESULT Render();

    virtual KG3DSceneSceneEditor* GetMainScene();

    virtual BOOL	GetRenderOption(RENDER_OPTION opt);
    virtual HRESULT SetRenderOption(RENDER_OPTION opt, BOOL bEnable);

    virtual BOOL    GetFrameMoveOption(FRAMEMOVE_OPTION opt);
    virtual HRESULT SetFrameMoveOption(FRAMEMOVE_OPTION opt, BOOL bEnable);

    virtual DWORD   GetRenderParam(RENDER_PARAM Param);
    virtual HRESULT SetRenderParam(RENDER_PARAM Param, DWORD Value);

    virtual PostRenderParams* LockPostRenderParams(BOOL bReadOnly);
    virtual void UnlockPostRenderParams();

	virtual	HRESULT GetEngineCaps(KG3DEngineCaps* pCaps);
	//virtual HRESULT GetCaps(D3DCAPS9 **ppRetCaps9);

	//////////////////////////////////////////////////////////////////////////
	//场景管理
	virtual HRESULT NewOneScene(unsigned uType, IKG3DScene **ppRetScene);
	virtual HRESULT RemoveScene(IKG3DScene *pScene);
	
	//(by dengzhihui Refactor 2006年11月2日
	virtual HRESULT Reset();//释放所有资源,恢复为刚启动状态 
	virtual HRESULT SetFocusScene(IKG3DScene* pScene);		
	virtual HRESULT SetMaxViewScene(IKG3DScene* pScene);	
	//)
	HRESULT SetFocusScene(KG3DScene* pScene);
	HRESULT SetMaxViewScene(KG3DScene* pScene);	

	//////////////////////////////////////////////////////////////////////////
	// Add Option Func . By FengBo 2007-9-5
	virtual HRESULT SetEngineOption(const KG3DEngineOption* pEngineOption);
	virtual HRESULT GetEngineOption(KG3DEngineOption* pEngineOption);

    virtual int GetColorCurveStypeNum();
    virtual void GetColorCurveStypeName(char szName[], int nIndex);


    virtual KG3DEngineOption* GetEngineOptionPtr();
	virtual HRESULT SetEngineEditorOption(KG3DEngineEditorOption* pEngineEditorOption);
	virtual HRESULT GetEngineEditorOption(KG3DEngineEditorOption* pEngineEditorOption);

	virtual HRESULT ExecuteCommand(LPCTSTR szCommand);

    virtual void SetOnPresentCallBackFunc(pfOnPresent pfCallback);
	virtual void SetOnRenderEndCallBackFunc(pfOnRenderEnd pfCallback);

    // 视频录像
    BOOL InitIPPVideo();
    void UnInitIPPVideo();

    virtual HRESULT SetIPPVideo(void* pfnCreateIPPCodec);
	virtual HRESULT MovieRecord(LPCTSTR szFolder, int nSizeScale, UINT uFilter, UINT uQuality, int nEncoder, int nFps);
	virtual HRESULT MovieStop();
	virtual BOOL    MovieReady() const;
	virtual VOID    MoviePresent();
	virtual VOID    MovieViewport(RECT const& Viewport);

	//事件录像
	virtual BOOL    CanRecord();
	virtual BOOL    IsRecording();
	virtual HRESULT StartRecord(LPCTSTR szFolder,unsigned uMode);
	virtual HRESULT StopRecord();


	//区域设定<CTH>///////////////////////////////////////////////////////////////////////
	virtual HRESULT GetRegionInfoManager(const char cszFileName[]
									, unsigned uFileNameHash
									, DWORD_PTR dwOption
									, IKG3DRegionInfoManager** ppRIM);
	virtual HRESULT GetIEKG3DRegionInfoManagerTable(IEKG3DRegionInfoManagerTable** ppTable);
	virtual HRESULT GetIEInterface(DWORD dwType1, DWORD dwType2, DWORD_PTR wParam, DWORD_PTR lParam, LPVOID* ppInterface);//第一个Type是抽象类型，第二个是具体类型，后面两个是获取参数
	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT GetUseLogicObject(BOOL* pUseLogicObject);
	//by dengzhihui Refactor 2006年11月6日
	virtual HRESULT GetUseSO3Logical(BOOL* pUseLogic);
	HRESULT GetGraphicsTool(IEKG3DGraphicsTool** pTool);
	
	HRESULT GetDefWorkDirectory(LPTSTR pszBuf, DWORD dwBufSizeInCharCount);
	HRESULT GetD3DDevice(LPDIRECT3DDEVICE9* ppDevice);
	HRESULT SetRenderTerrain(BOOL bRenderTerrain);
	BOOL GetRenderTerrain();
	HRESULT GetUI(IKG3DUI** pUI);

	IEKG3DMesh* FindMeshBase(DWORD dwID);
    
	HRESULT OnLostDevice();
	HRESULT OnResetDevice();

    TimeControl& GetTimeControl()  { return m_TimeControl; }

    //by xiayong
    virtual HRESULT GetIEKG3DTextureTable(IEKG3DTextureTable **ppTexture);
    virtual HRESULT GetIEKG3DModelTable(IEKG3DModelTable **ppModelTable);
	virtual HRESULT GetIEKG3DClipTable(IEKG3DClipTable **ppClipTable);
    virtual HRESULT GetNewIEKG3DMaterial(IEKG3DMaterial **ppMat);
    virtual HRESULT DestroyIEKG3DMaterial(IEKG3DMaterial *pMat);
    virtual HRESULT ExchangeModelSubsetID(IEKG3DModel* pIModel, int nSubset1, int nSubset2);
    virtual HRESULT GetNewIEKG3DLight(IEKG3DEnvEffLight **ppiLight);
    virtual HRESULT GetBaseWnd(HWND *phBaseWnd); //取全局变量g_hBaseWnd

    virtual HRESULT GetSpeedTreeLeafScale(FLOAT *pfRetScale);
    virtual HRESULT SetSpeedTreeLeafScale(FLOAT fScale);

    virtual HRESULT GetRenderTreeFlag(BOOL *pbRetRenderTree);
    virtual HRESULT SetRenderTreeFlag(BOOL bRenderTree);

    virtual HRESULT ChangeMovingState();
    
    virtual HRESULT GetAutoReloadTexture(BOOL *pbAutoReloadTexture);
    virtual HRESULT SetAutoReloadTexture(BOOL bAutoReloadTexture);

    virtual HRESULT SetFullScreen(BOOL bFullScreen);
    
	//<Object相关>
	virtual HRESULT GetRepresentObjectTable(IKG3DResourceManager** ppObjectTable);
	/*
	Build是万用函数，如果lpFileName不为NULL，那么Type不是必须的，会根据FileName来判断类型，
	有些东西是不需要文件的，则必须有Type，不需要FileName
	*/
	virtual HRESULT BuildKG3DRepresentObject(LPCTSTR lpFileName, DWORD dwType, DWORD_PTR wParam
		, DWORD_PTR lParam, IEKG3DRepresentObject **ppObject);
	virtual HRESULT NewKG3DRepresentObject(DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, IEKG3DRepresentObject **ppObject);
	//</Object相关>

    //MathTool 
   // virtual void LPCTSTR2IntInRange(LPCTSTR czSource, INT* pnDes, INT nLowerBound, INT nUpperBound);
    //virtual D3DCOLOR ColorValue(D3DCOLORVALUE co);
    //virtual D3DCOLORVALUE Color2Value(D3DCOLOR Color);
    //virtual void Color2Ref( const D3DCOLOR &D3DColor, COLORREF &CRColor);

    // GrassPattern
    virtual HRESULT LoadPattern(LPCSTR strFileName, int& nIndex);
    virtual HRESULT FindPattern(LPCSTR strFileName, INT *pnRetIndex);

    //  Represent object template
    virtual HRESULT GetRepresentObjectPropertyCount(DWORD *pdwRetCount);
    virtual HRESULT GetRepresentObjectProperty(DWORD dwIndex, OBJECT_PROPERTY **ppRetObjProperty);

    // Represent NPC template
    virtual HRESULT GetRepresentNPCPropertyCount(DWORD *pdwRetCount);
    virtual HRESULT GetRepresentNPCProperty(DWORD dwIndex, NPC_PROPERTY **ppRetNPCProperty);
    
    virtual HRESULT GetRelativePath(const TCHAR* strAbsPath11, const TCHAR* strAbsPath21, TCHAR* strRelativePath);
    virtual HRESULT GetEnableAddEntity(BOOL **ppbEnableAddEntity);

	virtual HRESULT GetKeyStateBuffer(BYTE** ppbyKeyStateBuf);
	//CTH <RTTI_INTERFACE>
	virtual HRESULT GetKG3DRTTITypeManager(IKG3DRTTITypeManager** pRTTI);

    virtual void RegistPushAlertMessageFunction(void (*)(const char[]));

	virtual BOOL GetAmibentShaderOnly();
	virtual HRESULT SetAmibentShaderOnly(BOOL bValue);
	virtual void ReloadAnimation();

	// 策划编辑器
	virtual void ManipulateSceneList(IEKG3DScene* pTargetScene, BOOL bAdd);

	void OnPresent(double fValue);
	void OnRenderEnd(double fValue);

	virtual HRESULT InitGameContext(KG3D_GAME_INIT_CONTEXT* pContext);
	virtual HRESULT GetInterface(DWORD dwType, DWORD_PTR p1, DWORD_PTR p2, LPVOID* ppInterface);
    
	IKG3DSoundShell* GetSound();
	void SetSoundFocusScene(IKG3DScene *pScene);
	IKG3DScene* GetSoundFocusScene();

    virtual IKG3DEngineOptionCaps* CreateIKG3DEngineOptionCaps();
    virtual void RemoveKG3DEngineOptionCaps(IKG3DEngineOptionCaps* pIEngineOptionCaps);
    void UnintEngineOptionCaps();
    list<KG3DEngineOptionCaps*> m_listEngineOptionCaps;

	// 移动场景相关  by liumingod
	KG3DEditMoveGrid        m_EditMoveGrid;
	KG3DEditRotationGrid    m_EditRotationGrid;
	KG3DEditScaleGrid       m_EditScaleGrid;

	KG3DTimeClock& GetTimeClock();
	ULONG	IsInitFinished();//因为EngineManager是全局变量，在其它全局函数中调用的时候，加上对它是否已经初始化的判断比较安全。外部千万千万不要用这种状态量，应该初始化完成了再用，也不要把这个函数暴露到接口。请回想d3d的函数，如果你得到一个Device指针，有没有处处判断它IsInit？对外部来说，保证得到的都是状态完整的量比较好，不然很难处理

	BOOL NeedDrawTwice();
private:
	HRESULT InitGraphicsEngine(HWND hBaseWindow, HWND hRenderWindow);
    HRESULT UnInitGraphicsEngine();

	void SetSoundShell(IKG3DSoundShell *pShell);

	HRESULT LoadConfig(BOOL* pFileIsNotExit);
	//----------------- Add  by FengBo 2007-9-5	
	HRESULT SaveEngineOptionConfig();			// 保存客户端改变的引擎选项
	HRESULT SaveEngineEditorOptionConfig();	// 保存调用EditorEngineOption的选项
	//---------------------------------
	HRESULT SaveConfig();

	HRESULT LoadPreloadSpeedTree(LPCSTR strFileName);	

#if defined(DEBUG) | defined(_DEBUG)
	VOID	RuntimeTest();
#endif

private:
	IKG3DSoundShell *m_pSound;
	pfOnPresent m_pfOnPresent;
	pfOnRenderEnd m_pfOnRenderEnd;

    DWORD   m_FrameMoveOption;
	DWORD	m_RenderOption;

    HMODULE m_hModuleIPPCodec;
    void* m_pCreateIPPCodec;
    IIPPCodec* m_pIPPCodec;
	KG3DTimeClock* m_pTimeClock;
	IKG3DScene *m_pSoundFocusScene;
    TimeControl m_TimeControl;

    BOOL m_bPostRenderParamsLockReadOnly;

	enum
	{
		em_init_finished = 1,
	};
	std::bitset<sizeof(DWORD)>	m_States;
};

//////////////////////////////////////////////////////////////////////////
//全局变量
extern KG3DEngineManager           g_cEngineManager;

extern HWND                        g_hBaseWnd;//基础的窗口
extern HWND                        g_hRenderWnd;//默认的渲染窗口

extern BOOL         g_Mouse_L;//鼠标左键状态
extern BOOL         g_Mouse_M;//鼠标中键状态
extern BOOL         g_Mouse_R;//鼠标右键状态
extern BYTE         g_KeyState[256];//键盘状态
extern int          g_nMouseMoveX;
extern int          g_nMouseMoveY;
extern int          g_nMouseMoveKey;
extern TCHAR        g_Def_WorkDirectory[256];//执行文件目录
extern KG3DFileFinder* g_pFileFinder;

extern BOOL g_bClient;//是否处于客户端
extern BOOL g_bRePlay;
extern BOOL g_bAutoReloadTexture;
extern BOOL g_bFullScreen;
// Modify by FengBo 2007-9-5
extern KG3DEngineOption g_cEngineOption;			//客户端使用的选项
extern KG3DEngineEditorOption g_eEngineOption;	//编辑器端使用的选项
extern DWORD              g_dwRenderCount;
const KG3DLogicalConstances& g_GetLogicalConstants();	//逻辑有关的变量放这里，由逻辑初始化
	
extern void (*g_AddAlertMessage)(const char[]);

#define MULTITRHEAD_ANI_BUFFERSIZE 128

extern HRESULT g_ReportResourceUse();
extern BOOL g_bEnablePointLight;

extern TCHAR g_szModuleFilePath[MAX_PATH];
extern TCHAR g_szConfigFilePath[MAX_PATH];

#if defined(_DEBUG) | defined(DEBUG)
HRESULT	g_KG3DEngineInterfaceTest(KG3DEngineManager* pEM);	//测试引擎的接口//用Debug模式单步的时候得到指针，然后在Watch窗口中调用//CTH

#endif