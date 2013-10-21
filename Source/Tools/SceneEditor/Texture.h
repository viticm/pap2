// Texture.h: interface for the KTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__DB78064D_653D_4625_AEF2_74551A3343B1__INCLUDED_)
#define AFX_TEXTURE_H__DB78064D_653D_4625_AEF2_74551A3343B1__INCLUDED_

#include "Engine.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_PAL_SIZE 256 
#define MAX_BACK_TEXTURE_NUM	121		//用于Cache的Texture
//关于Represent错误的类型定义
#define REPRESENT_OK				0
#define REPRESENT_ERR				1
#define REPRESENT_ERR_FILE_OPEN		2				//文件读取错误
#define REPRESENT_ERR_OUTMEMORY		3				//内存不足
#define REPRESENT_ERR_OUTVIEDOMEMORY	4			//显存不足
#define REPRESENT_ERR_FILENAME		5				//文件名错误
#define REPRESENT_ERR_FILE_CONTENT	6				//文件内容错误

typedef int					int32;
typedef unsigned int        uint32;

/*

typedef struct {
	BYTE		Red;		// 红色分量
	BYTE		Green;		// 绿色分量
	BYTE		Blue;		// 兰色分量
} KPAL24;					// 24位色盘
*/


struct TextureInfo
{
	uint32	nWidth;						// 贴图宽度
	uint32	nHeight;					// 贴图高度
	LPDIRECT3DTEXTURE9 pTexture;		// 贴图指针
	uint32	nFrameX;					// 在这一帧图象上的左上角位置
	uint32	nFrameY;					// 在这一帧图象上的左上角位置
	int32	nFrameWidth;				// 贴图上填充的数据宽度
	int32	nFrameHeight;				// 贴图上填充的数据高度
};

struct FrameToTexture
{
	int32	nTexNum;					// 这一帧拆分为几张贴图
	int32	nWidth;						// 这一帧的宽度
	int32	nHeight;					// 这一帧的高度
	int32	nOffX;						// 相对于整个spr的X坐标偏移
	int32	nOffY;						// 相对于整个spr的Y坐标偏移
	TextureInfo texInfo[4];				// 贴图信息数组，一级拆分，最多4张贴图
	int32	nRawDataLen;				// 原始数据长度
	BYTE	*pRawData;					// 这一帧的原始数据
	void	*pFrame;					// 保存SprGetFrame返回的指针，供SprReleaseFrame使用
};

typedef class KTexture  
{
public:
	FILETIME m_FileTime;

	HRESULT ReloadFromFile();
	HRESULT Exchange2Bump(LPDIRECT3DTEXTURE9& lpTexture);
	virtual HRESULT LoadBumpMap(LPSTR pFileName);

	HRESULT CreateFromText(TCHAR* strFontName,
						   TCHAR* Texts,
						   WORD   FontHeight,
						   DWORD*   pTextureWidth,
						   DWORD*   pTextureHeight,
						   DWORD  nFlag);
	HRESULT CreateFromText(TCHAR* strFontName,	
						   TCHAR* Texts,
						   WORD   FontHeight,
						   DWORD* pTextureWidth,
						   DWORD* pTextureHeight,
						   DWORD  nFlag,
						   DWORD  nChar,
						   DWORD& nCharPos);

	HRESULT LoadCubeMap(LPSTR pFileName);
	HRESULT LoadVolumeMap(LPSTR pFileName);
	HRESULT ComputeNormal();
	virtual HRESULT Update(DWORD Method,float Time);
	virtual HRESULT LoadFromFile(LPSTR pFileName);

	virtual HRESULT CleanUp();
	LPSTR m_lpFileName;
	DWORD m_dOption;
	virtual void Initialize(PVOID pTextureTable);
	DWORD m_dType;
	DWORD ID;

	LPDIRECT3DTEXTURE9  m_lpTexture;

	KTexture();
	virtual ~KTexture();

	HRESULT SetAlpha(D3DCOLOR Color,BYTE Alpha);
	virtual HRESULT InvalidateDeviceObjects(void);
	virtual HRESULT RestoreDeviceObjects(void);
}* LPTEXTURE;

class KAnimateTexture :public KTexture
{
public:

    virtual HRESULT CleanUp();
	HRESULT Update(DWORD Method,float Time);

	HRESULT LoadBumpMap(LPSTR pFileName);
	HRESULT LoadFromFile(LPSTR pFileName);
	LPDIRECT3DTEXTURE9*  m_lpBackTextures;
	DWORD  m_dwNumFrame;
	float  m_fFrameLength;
	KAnimateTexture()
	{
		m_fFrameLength = 100;
		m_dwNumFrame = NULL;
		m_lpBackTextures = NULL;
	};
	virtual ~KAnimateTexture()
	{
	};

};

//class KTextureResSpr : public KTexture
//{
//public:
//	KTextureResSpr();
//	virtual ~KTextureResSpr();
//
//	// 将成员变量置为初始值
//	virtual void ResetVar();
//
//	// 创建内存资源
//	virtual bool CreateImage(const char* szImage, int nWidth, int nHeight, uint32 nType);
//	// 从文件载入资源
//	virtual int LoadImage(char* szImage, uint32 nType);
//
//	// 设备丢失时释放d3d对象
//	virtual HRESULT InvalidateDeviceObjects();
//
//	// 设备恢复时重建d3d对象
//	virtual HRESULT RestoreDeviceObjects();
//
//	// 释放内存
//	void Clear();
//
//	// 释放一帧贴图数据
//	// 返回真表示释放了一些数据，返回假表示所有的贴图数据都已释放完毕
//	virtual bool ReleaseAFrameData();
//
//	// 为绘制准备数据，
//	// szImage：文件名
//	// nFrame：第几帧数据
//	int PrepareFrameData(const char* szImage, int32 nFrame, bool bPrepareTex);
//
//	// 取得nFrame帧的第nIdx张贴图，根据不同情况nIdx取值为1-4
//	LPDIRECT3DTEXTURE9 GetTexture(int32 nFrame, int nIdx);
//
//	uint32 GetCenterX(){ return m_nCenterX; }
//	uint32 GetCenterY(){ return m_nCenterY; }
//	uint32 GetFrameNum(){ return m_nFrameNum; }
//	uint32 GetDirections(){ return m_nDirections; };
//	uint32 GetInterval(){ return m_nInterval; };
//
//	// 取得第nFrame帧图象x，y坐标上的象素点alpha值
//	int32 GetPixelAlpha(const char* szImage, int32 nFrame, int32 x, int32 y);
//
//	//将此函数作为public，破坏了封装，但是为了能做memoy检查，只能这样
//	int CreateTexture16Bit(int32 nFrame,LPDIRECT3DDEVICE9 pD3DDevice);
//	//private:
//public:
//	uint32		m_nCenterX;
//	uint32		m_nCenterY;
//	uint32		m_nColors;
//	uint32		m_nDirections;
//	uint32		m_nInterval;
//
//	// 用于性能统计
//	unsigned long		m_nTexMemUsed;			// 贴图内存占用数量,单位字节
//	unsigned long		m_nSprMemUsed;			// 贴图内存占用数量,单位字节
//
//
//	KPAL24		m_pPal24[MAX_PAL_SIZE];		// 调色板
//	WORD		m_pPal16[MAX_PAL_SIZE];		// 4444调色板
//	int32		m_nFrameNum;				// 资源帧数
//
//	FrameToTexture*		m_pFrameInfo;		// 帧到铁图映射信息数组指针
//	char*		m_pHeader;					// spr头部信息，如果不为空意味着分帧压缩，否则为整体压缩
//
//private:
//	int LoadSprFile(char* szImage);
//
//	void CreateTexture32Bit(uint32 nFrame);
//
//	// 拆分贴图，将贴图拆分为1到4张以充分利用贴图空间
//	int SplitTexture(uint32 nFrame);
//	bool m_bPacked;
//	bool m_bCacheable;
//	bool m_bNeedReDraw;
//
//	int m_nWidth		;
//	int m_nHeight		;
//	uint32		m_nType;
//	char m_szFileName[80];
//	///spr文件在内存中的印像
//	//SPRHEAD*		m_psprHead;			
//};

//class KMovieTexture : public KTexture
//{
//public:
//	//-----------------------------------------------------------------------------
//	// Define GUID for Texture Renderer
//	// {71771540-2017-11cf-AE26-0020AFD79767}
//	//-----------------------------------------------------------------------------
//	struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;
//
//	class CTextureRenderer : public CBaseVideoRenderer
//	{
//	public:
//		CTextureRenderer(LPUNKNOWN pUnk,HRESULT *phr);
//		~CTextureRenderer();
//	public:
//		HRESULT SetDevice(LPDIRECT3DDEVICE9 pDevice);
//		D3DFORMAT g_TextureFormat;
//		LPDIRECT3DTEXTURE9 g_pTexture;
//
//		HRESULT CheckMediaType(const CMediaType *pmt );     // Format acceptable?
//		HRESULT SetMediaType(const CMediaType *pmt );       // Video format notification
//		HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample
//
//		LONG m_lVidWidth;	// Video width
//		LONG m_lVidHeight;	// Video Height
//		LONG m_lVidPitch;	// Video Pitch
//	};
//
//	/*CTextureRenderer        *pCTR;          // DShow Texture renderer
//
//	CComPtr<IGraphBuilder>  g_pGB;          // GraphBuilder
//	CComPtr<IMediaControl>  g_pMC;          // Media Control
//	CComPtr<IMediaPosition> g_pMP;          // Media Postion
//	CComPtr<IMediaEvent>    g_pME;          // Media Event
//
//	CComPtr<IVideoWindow>   g_pVW ;
//	CComPtr<IBasicAudio>    g_pBA ;
//	CComPtr<IBasicVideo>    g_pBV ;
//	CComPtr<IMediaSeeking>  g_pMS ;*/
//
//public:
//	KMovieTexture();
//	~KMovieTexture();
//
//	HRESULT  LoadFormMoveFile(LPCSTR pScrFile);
//	void     Cleanup();
//	void     CleanupDShow();
//	void     CheckMovieStatus();
//
//	HRESULT  InitDShowTextureRenderer(LPDIRECT3DTEXTURE9* ppTexture,LPCSTR pScrFile);
//
//};

#endif // !defined(AFX_TEXTURE_H__DB78064D_653D_4625_AEF2_74551A3343B1__INCLUDED_)
