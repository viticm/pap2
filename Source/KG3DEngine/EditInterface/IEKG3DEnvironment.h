////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DEnvironment.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-11-29 14:56:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma  once
#ifndef _INCLUDE_IEKG3DENVIRONMENT_H_
#define _INCLUDE_IEKG3DENVIRONMENT_H_
#include "KG3DTypes.h"
#include "IEKG3DCommonObject.h"
//#include "IEKG3DSkySystem.h"
//IEKG3DEnvEffDynamicWeather会包含这个，所以要小心

////////////////////////////////////////////////////////////////////////////////
typedef enum _KG3DLIGHT_TYPE
{
	KG3DLIGHT_TYPE_SPOT,
	KG3DLIGHT_TYPE_DIRECT,
	KG3DLIGHT_TYPE_POINT,
	KG3DLIGHT_TYPE_SUN,
	KG3DLIGHT_TYPE_MOON,
}KG3DLIGHT_TYPE;
/*
typedef struct _KG3DENVEFF_INFO///用于让KG3DEnvironment返回类型和个数
{
	DWORD m_dwType;
	INT	m_nCount;
	_KG3DENVEFF_INFO()
	{
		memset(this, 0, sizeof(_KG3DENVEFF_INFO));
	}
}KG3DENVEFF_INFO;*/

typedef struct _KG3DLIGHT_DESCSTRUCT
{
	DWORD m_dwType;
	DWORD m_dwModificationOption;
	///颜色
	D3DCOLOR m_d3dc_Embient;
	D3DCOLOR m_d3dc_Diffuse;
	D3DCOLOR m_d3dc_Specular;
	FLOAT	m_Attenuation0;
	FLOAT	m_Attenuation1;
	FLOAT	m_Attenuation2;
	///特性
	FLOAT	m_fRange;
	FLOAT	m_fFallOff;
	FLOAT	m_fInnerAngle;
	FLOAT	m_fOuterAngle;
	///位置
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3	m_vDirection;
	FLOAT	m_fAzimuth;
	FLOAT	m_fElevation;
	_KG3DLIGHT_DESCSTRUCT()
	{
		memset(this, 0, sizeof(_KG3DLIGHT_DESCSTRUCT));
	}
}KG3DLIGHT_DESCSTRUCT;
//////////////////////////////////////////////////////////////////////////
/*
typedef struct _KG3DENVEFF_HANDLE
{
	DWORD m_dwType;
	INT   m_nIndex;
#ifdef __cplusplus
	_KG3DENVEFF_HANDLE():m_dwType(0), m_nIndex(0){}///有构造函数的话，可能拷贝构造就不对了??
	_KG3DENVEFF_HANDLE(DWORD dwType, INT nIndex):m_dwType(dwType),m_nIndex(nIndex){}
	_KG3DENVEFF_HANDLE(const _KG3DENVEFF_HANDLE& Other):m_dwType(Other.m_dwType), m_nIndex(Other.m_nIndex){}
	_KG3DENVEFF_HANDLE& operator=(const _KG3DENVEFF_HANDLE& Other){m_dwType = Other.m_dwType; m_nIndex = Other.m_nIndex;return *this;}
	bool operator==(const _KG3DENVEFF_HANDLE& Other){return 0 == memcmp(this, &Other, sizeof(_KG3DENVEFF_HANDLE));}
#endif
}KG3DENVEFF_HANDLE;*/

typedef KG3DCOBJ_HANDLE KG3DENVEFF_HANDLE;
////////////////////////////////////////////////////////////////////////////////
enum
{
	KG3DENVEFF_STATE_FILTER_DWGC_AVAILABLE = 8,	//内部也要用，不要随便改
};
struct KG3DEnvEffectClassProperty	//用于静态的记录EnvEff的属性
{
	DWORD	m_dwType;//用于索引的Type
	//int		m_nIndex;//第几个根Type
	DWORD	m_dwParentType;//有无父类型
	//int		m_nParentIndex;//父类型的索引，应该和对应的m_dwType的m_nIndex相等
	bool	m_bIsDWGCAvailable;	//可否使用动态天气

	BOOL IsTop(){return m_dwParentType == 0;}

	/*KG3DEnvEffectClassProperty() { ZeroMemory(this, sizeof(KG3DEnvEffectClassProperty)); }

	KG3DEnvEffectClassProperty(DWORD dwType, int nIndex, DWORD dwParentType, int nParentIndex, bool bIsDWGCAvailable)
		: m_dwType(dwType)
		, m_nIndex(nIndex)
		, m_dwParentType(dwParentType)
		, m_nParentIndex(nParentIndex)
		, m_bIsDWGCAvailable(bIsDWGCAvailable)
	{
	}*/
};

struct IEKG3DEnvEffect
{
	virtual HRESULT	Enable(BOOL bEnable) = 0;
	virtual HRESULT IsEnable(BOOL* pbIsEnable) = 0;
	virtual	ULONG	GetType(/*DWORD* pdwType*/) = 0;	//Type必须由创建者自己设定
	virtual HRESULT GetEnvEffName(LPTSTR tszName, INT nBufferSize) = 0;
	virtual HRESULT	GetID(int*	nID) = 0;			
	virtual HRESULT ToSolidInterface(LPVOID* pEffectInterface) = 0;//可以强转成接口的方法
	//////////////////////////////////////////////////////////////////////////
	virtual	HRESULT	EnableControlByDynamicWeather(BOOL	bEnable) = 0;
	virtual	HRESULT IsEnableControlByDynamicWeather(BOOL* pbEnable) = 0;///Enable的话返回S_OK，否则E_FAIL
	virtual HRESULT GetSubEffect(INT nIndex, IEKG3DEnvEffect** ppEnvEffect, KG3DENVEFF_HANDLE* pHandle) = 0;
	virtual HRESULT GetSubEffectCount(INT* pnCount) = 0;
	//////////////////////////////////////////////////////////////////////////
	virtual ULONG	GetState(size_t WhichState) = 0;
};
////////////////////////////////////////////////////////////////////////////////
enum
{
	KG3DENVEFF_MAX = 32,
};
namespace KG_CUSTOM_HELPERS
{
	class IteratorHolder;
};
namespace KGCH = KG_CUSTOM_HELPERS;
interface IEKG3DEnvironment 
{
	virtual	HRESULT NewEnvEffect(DWORD dwType, IEKG3DEnvEffect** ppEnvEffect) = 0;///传入NULL就不返回指针
	virtual	HRESULT DelEnvEffect(DWORD dwType, INT_PTR nIndex) = 0;
	virtual HRESULT GetEnvEffect(DWORD dwType, INT nIndex, IEKG3DEnvEffect** ppEnvEffect) = 0;///nIndex代表第几个同型的Effect,得到之后可以用ToSolidInterface变成接口
	
	virtual HRESULT EnableControlByDynWeather(DWORD dwType, BOOL bEnable) = 0;///统一的为各个Effect交接动态天气的方法
	//////////////////////////////////////////////////////////////////////////
	virtual ULONG GetTopEnvEffectCount() = 0;
	//virtual HRESULT GetTopEnvEffect(INT nIndex, IEKG3DEnvEffect** pEffect, KG3DENVEFF_HANDLE* pHandle) = 0; 
	//////////////////////////////////////////////////////////////////////////
	
	virtual HRESULT GetEnvEffectCount(DWORD dwType, INT* pnCount) = 0;

	virtual KG3DEnvEffectClassProperty* GetFirstClassProperty() = 0;
	virtual KG3DEnvEffectClassProperty* GetNextClassProperty(KG3DEnvEffectClassProperty* pPrev) = 0;

	//virtual HRESULT GetAllEnvEffectThatCanBeAddedCount(INT_PTR* pnResult) = 0;
	//virtual HRESULT GetEnvEffectThatCanBeAdded(INT nIndex, DWORD* pdwType) = 0;

	//////////////////////////////////////////////////////////////////////////

	//virtual IEKG3DEnvEffect* ResetIterator() = 0;
	//virtual IEKG3DEnvEffect* NextIterator() = 0;//注意不能同时改变容器内容
	virtual HRESULT GetFirstTopEnvEffect(KGCH::IteratorHolder& itHolder, IEKG3DEnvEffect** ppEffect) = 0;
	virtual HRESULT GetNextTopEnvEffect(KGCH::IteratorHolder& itHolder, IEKG3DEnvEffect** ppEffect) = 0;//注意不能同时改变容器内容

	//virtual HRESULT GetFirstTopEnvEffect(KG3DENVEFF_HANDLE* pHandle, IEKG3DEnvEffect** ppEffect) = 0;
	//virtual HRESULT GetNextTopEnvEffect(KG3DENVEFF_HANDLE PrevHandle, KG3DENVEFF_HANDLE* pNextHandle, IEKG3DEnvEffect** ppEffect) = 0;
	
	virtual HRESULT Save(LPCTSTR pFilePath) = 0;
	virtual HRESULT Reload(LPCTSTR pFilePath) = 0;

    virtual HRESULT SetSettingType(DWORD dwSettingType) = 0;
    virtual DWORD GetSettingType() = 0;
};
#endif //_INCLUDE_IEKG3DENVIRONMENT_H_
