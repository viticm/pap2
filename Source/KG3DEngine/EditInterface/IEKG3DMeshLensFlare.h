////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DMeshLensFlare.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-13 16:51:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DMESHLENSFLARE_H_
#define _INCLUDE_IEKG3DMESHLENSFLARE_H_

////////////////////////////////////////////////////////////////////////////////
struct LensFlareInfo
{
    INT     nIndexX;
    INT     nIndexY;
    FLOAT   fHeight;	//浮点
    FLOAT   fSize;	//浮点
    FLOAT   fRate;
#ifdef __cplusplus
	LensFlareInfo()
	{
		ZeroMemory(this, sizeof(LensFlareInfo));
	}
	bool operator==(const LensFlareInfo& Other)
	{
		return 0 == memcmp(this, &Other, sizeof(LensFlareInfo));
	}
	bool operator!=(const LensFlareInfo& Other)
	{
		return ! this->operator==(Other);
	}
#endif
};

enum
{
	KG3DMeshLensFlare_MDO_TexSplitNum = 0x01,
	KG3DMeshLensFlare_MDO_Visibility =0x02,
	KG3DMeshLensFlare_MDO_TexPath = 0x04,
	KG3DMeshLensFlare_MDO_BoundAspect = 0x08,
	KG3DMeshLensFlare_MDO_DummyLenSize = 0x10,
	KG3DMeshLensFlare_MDO_State	= 0x20,
	KG3DMeshLensFlare_MDO_All = 0xff,
};

enum
{
	KG3DMeshLensFlare_State_DrawDummyLen = 0x01,
};

typedef struct _IEKG3DMeshLensFlareDescStruct 
{
	DWORD m_dwModificationOption;
	INT m_nTexSplitNumX;	///贴图分块
	INT m_nTexSplitNumY;
	FLOAT	m_fVisibility;	///可视性
	FLOAT	m_fVisibleOutterBoundAspect;///以屏幕大小为单位1，在屏幕内的光晕亮度权值为1，外部的从边沿向外递减。这个值用于设定到屏幕外部多少的地方，亮度权值递减为0
	FLOAT	m_fDummyLenSize;	//浮点，指从中心到边沿的距离，是个屏幕百分比，等于0的时候可以禁止阻塞检查
	INT		m_dwState;					///不保存
	TCHAR	m_tczTexName[MAX_PATH];
#ifdef __cplusplus
	_IEKG3DMeshLensFlareDescStruct()
		:m_dwModificationOption(0)
		,m_nTexSplitNumX(1)
		,m_nTexSplitNumY(1)
		,m_fVisibility(1.0f)
		,m_fVisibleOutterBoundAspect(0.5)
		,m_fDummyLenSize(0.01f)
		,m_dwState(0)
	{
		m_tczTexName[0] = _T('\0');		
	}
	_IEKG3DMeshLensFlareDescStruct& operator=(const _IEKG3DMeshLensFlareDescStruct& Other)
	{
		memcpy_s(this, sizeof(_IEKG3DMeshLensFlareDescStruct), &Other, sizeof(_IEKG3DMeshLensFlareDescStruct));
		return *this;
	}
	_IEKG3DMeshLensFlareDescStruct(const _IEKG3DMeshLensFlareDescStruct& Other)
	{
		this->operator=(Other);
	}
	
#endif
}IEKG3DMeshLensFlareDescStruct;

interface IEKG3DMeshLensFlare
{
    virtual ULONG GetLensFlareCount() = 0;
    virtual HRESULT GetLensFlareItem(size_t dwPosition, LensFlareInfo* ppFlare) = 0;
    virtual HRESULT SetLensFlareItem(size_t dwPosition, const LensFlareInfo* pFlareInfo, size_t* newPos) = 0;//改Height的时候会触发排序
    virtual HRESULT DefaultBuild() = 0;
	virtual HRESULT AddLensFlare(const LensFlareInfo& lensFlare) = 0;
	virtual HRESULT DelLensFlare(size_t uIndex) = 0;
    
	virtual HRESULT GetTexID(DWORD* pID) = 0;

	virtual HRESULT RequestModification(const IEKG3DMeshLensFlareDescStruct* pDescStruct) = 0;
	virtual HRESULT GetDescStruct(IEKG3DMeshLensFlareDescStruct* pRetStruct) = 0;

	virtual HRESULT ReloadShader() = 0;
};
#endif //_INCLUDE_IEKG3DMESHLENSFLARE_H_
