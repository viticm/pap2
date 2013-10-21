////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DTexture.h
//  Version     : 1.0
//  Creator     : huyi
//  Create Date : 2006-11-8 16:06:58
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DTEXTURE_H_
#define _INCLUDE_KG3DTEXTURE_H_

////////////////////////////////////////////////////////////////////////////////

#include "kg3dinterface.h"
#include "KG3DTypes.h"
#include "KG3DTemplates.h"
#include "IEKG3DTexture.h"
#include "KG3DMessenger.h"

enum
{
	TEXOP_COMMON = 0,
	TEXOP_ANIMATION = 1,
	TEXOP_MOVIE = 2,
	TEXOP_FLASH = 3,
	TEXOP_RENDERTARGET = 0x100,
	TEXOP_DYNAMIC = 0x200,	// 动态纹理,主要用在界面
	TEXOP_RELOAD = 0x400,   // 容许使用自动重新载入
	TEXOP_DISABLEMIPMAP = 0x1000,  //禁止使用Mipmap
	TEXOP_DISABLEPOW2 = 0x2000,  //禁止使用 pow2 load
	TEXOP_DISABLEDTX = 0x4000,  //禁止使用dtx格式
	TEXOP_DISABLESCALE = 0x8000,  //禁止使用纹理缩放
};

struct KG3DVideoMemTexture
{
	PVOID pUser;
	LPDIRECT3DTEXTURE9 pTexture;
	UINT  uID;
	DWORD dwLastTime;
	PVOID pMgr;

	HRESULT Init()
	{
		ZeroMemory(this,sizeof(KG3DVideoMemTexture));
		return S_OK;
	}

	HRESULT OnResourceUse();
	HRESULT OnFreeResource();

	~KG3DVideoMemTexture();
};

class KG3DTexture : public IEKG3DTexture
{
	friend class KG3DTextureTable;
	friend class KG3DVideoMemTextureTable;
public:
	D3DXIMAGE_FILEFORMAT m_eImageType;

    virtual HRESULT GetName(LPCTSTR *ppszName);
    virtual HRESULT GetID(DWORD *pdwID);
    
	virtual HRESULT OnLostDevice();
	virtual HRESULT OnResetDevice();

	virtual BOOL IsResourceReady();

    virtual HRESULT GetIEKG3DTexture(IEKG3DTexture **ppIEKG3DTexture);
    
	unsigned m_uOption;

protected:
	DWORD m_dwType;//类型，SetType
	DWORD m_dwID;
	DWORD m_dwTextureType;
	BOOL m_bAutoGenMipmap;

	DWORD m_dwTimes;
	BOOL  m_bCanReload;		// 能否被重新装载
    
	enum DynamicState
	{
		STATE_DELAYLOAD,//延迟加载
		STATE_NONE,//没有加载
		STATE_SYSTEMMEM,//加载到内存中了
		STATE_VIDEOMEM,//加载到现存中了
		STATE_LOADFAILED
    }m_eDynamicLoadState;
	
	KG3DTEXTURE_DESC	m_Desc;
	enum { MAX_DIVISOR_OF_BACKBUFFER = 4 };//最小是后缓冲的1/4

	DWORD m_dwRenderTarger;
	int m_nDim;

	LPDIRECT3DTEXTURE9 m_pTexture;
	LPDIRECT3DTEXTURE9 m_pTextureSmall;
	LPDIRECT3DTEXTURE9 m_pTextureSys;
	LPDIRECT3DTEXTURE9 m_pTextureSysSmall;
	LPDIRECT3DTEXTURE9 m_pDiscard;

	KG3DVideoMemTexture* m_pVideoMemShare;

	LPDIRECT3DSURFACE9 m_pDepthStencilSurface;

	HRESULT _CheckOption(unsigned uOption,BOOL bInMultiThread);
	
	KG3DMessageSingleReceiverSender m_KG3DSRMessageSender;///用于处理和Texture的拥有者的交互,暂时主要用于通知拥有者Reload的消息（Reload函数中）// Last modify by Chen Tianhong, 2006-11-12 17:39:58
	static bool _GetFileTime(LPCSTR cszFileName, FILETIME& FileTimeRet);	//用于处理文件时间m_FileTime的初始化,如果失败，不会写FileTimeRet

public:
	FILETIME m_FileTime;
	ULONG    m_ulRefCount;//物品使用计数
	std::string m_scName;
	std::string m_scNameReal;//真正

	KG3DTexture(void);
	virtual ~KG3DTexture(void);

	//////////////////////////////////////////////////////////////////////////
	//引用计数
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void **ppvObject);
	ULONG STDMETHODCALLTYPE ReleaseWithoutDelay();///利用TextureTable的RELEASE_ATONCE参数，如果引用计数为0，立刻删除。
	//////////////////////////////////////////////////////////////////////////
	//初始化,清除数据
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	//////////////////////////////////////////////////////////////////////////
	//加载,保存
	virtual HRESULT _LoadFromMemory( LPCVOID pSrcData, UINT SrcSize, unsigned uOption ,BOOL bRetryIfFailed);
    int _ProcessDelayLoad();
	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);

	virtual HRESULT SaveToFile(const char cszFileName[],   unsigned uOption);
    virtual HRESULT SaveSysToFile(const char cszFileName[],   unsigned uOption);
	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam)
	{
		return LoadFromFile(cszFileName, uFileNameHash, uOption);
	}
	virtual HRESULT RefreshOption(unsigned uOption);
	//////////////////////////////////////////////////////////////////////////
	//ID,type
	virtual DWORD GetID();
	virtual unsigned GetType();
	void SetID(DWORD dwID) { m_dwID = dwID;}
	void SetType(DWORD dwType){m_dwType = dwType;}
	//////////////////////////////////////////////////////////////////////////
	//内存管理
	virtual HRESULT PrepareRender(unsigned uOption);//压缩数据
	virtual HRESULT PostRender();//恢复数据，对象可以立即被使用
	virtual HRESULT GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem);//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
	virtual unsigned   GetLastUseTime();//查询上次使用的时间
	virtual unsigned   GetUseFrequency();//查询使用频率，一段时间里的使用次数

	virtual HRESULT Update(unsigned uMethod, DWORD dwTime);//更新贴图,对动画贴图起作用
	virtual HRESULT Update(int nFrame);//更新贴图

	virtual HRESULT GetSize(unsigned *puWidth, unsigned *puHeight);
	virtual std::pair<UINT, UINT>	GetSize();
	virtual LPDIRECT3DTEXTURE9 GetTexture();	//没有AddRef，这个函数里面千万不能AddRef，所有地方都是按没有AddRef的形式写的

	virtual HRESULT CheckLoadState();

	VOID	PreLoad();//只有Managed类型的才能PreLoad
	HRESULT ForceReload();
	HRESULT UpdateTexture();
	HRESULT SetTexture(LPDIRECT3DDEVICE9 pDevice, int i);


	bool  EvictResources( );
public:
	BOOL IsFileChanged();
	D3DFORMAT GetFormat();
	IKG3DMessageSenderBase* GetMessageSender(){ return &m_KG3DSRMessageSender; };///这个方法不是必须暴露在接口中的
	HRESULT GetSurfaceDesc(UINT dwLevel, D3DSURFACE_DESC* pDesc);

	HRESULT ColorFill(CONST RECT* pRect, D3DCOLOR color);//扩展了DX的ColorFill。Rect可以为NULL，只支持RenderTarget或者D3DFMT_A8R8G8B8和D3DFMT_X8R8G8B8
	HRESULT SetAsRenderTarget(LPDIRECT3DDEVICE9 pDevice, DWORD RTIndex);	//设RT的帮助类，会帮助GetSurface
	HRESULT SetAsDelphStencil(LPDIRECT3DDEVICE9 pDevice);
	HRESULT StretchRect(KG3DTexture& DestTex, CONST RECT* pSrcRect, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE FilterType);//和D3D的一致，把当前贴图放缩到另外一张贴图，只操作0层
protected:
	virtual HRESULT _OnReferenceZero(DWORD dwReleaseOpion = RELEASE_INQUEUE);
	//如果指定uBackBufferSizeDividing，会根据Backbuffer创建,1就是后缓冲大小。
	//后缓冲匹配是需要程序处理而不能硬设Width和Height的，因为后缓冲大小会改变
	HRESULT CreateTexture(unsigned int uWidth,
		unsigned int uHeight,
		D3DFORMAT format,
		TEXTYPE	texType = TEXTYPE_COMMON,	//0是默认，其它参考枚举
		UINT uBackBufferSizeDividing = 0);
private:
	D3DXIMAGE_FILEFORMAT _GetFileTypeFromName( const char* strFileName );
	HRESULT _CreateTexturePrivate(BOOL bUseDevice, UINT Width,UINT Height,UINT MipLevels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool, UINT uBackBufferSizeDividing);

    HANDLE m_hFileLoader;
};

class KG3DClip;
extern int g_nNumTextureDynamicLoadedFrame;
extern int g_nNumAllowTextureDynamicLoadFrame;
//extern std::set<KG3DClip*> g_setMultiThreadReadAni;
//extern KG3DClip* g_pCurrentAni;

void g_MultiThreadLoadProgress(void * pInput);
void g_MultiThreadLoadProgressTerrain(void * pInput);

#ifdef KG3DENGINE_DISABLE_MULTITHREAD
void UpdateMultiThreadModelProgress(void* pInput);
#endif

#endif //_INCLUDE_KG3DTEXTURE_H_
