////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DEnvEffFog.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-1-15 11:00:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DENVEFFFOG_H_
#define _INCLUDE_IEKG3DENVEFFFOG_H_

////////////////////////////////////////////////////////////////////////////////
typedef enum _KG3DENVEFFFOG_MDO
{
	KG3DENVEFFFOG_MDO_ALL = 0xffff,
	KG3DENVEFFFOG_MDO_FOG_BASE = 0x01,///基本参数
	KG3DENVEFFFOG_MDO_FOG_COLOR = 0x02,///特殊，设颜色是要和别的东西同步的
	KG3DENVEFFFOG_MDO_TABLE_FOG = 0x04,///使用TABLEFOG的参数
	KG3DENVEFFFOG_MDO_VERTEX_FOG = 0x08,///使用VertexFog的参数
	KG3DENVEFFFOG_MDO_MODE_PARAM = 0x10,
}KG3DENVEFFFOG_MDO;

typedef struct _KG3DENVEFFFOG_CAPS_STRUCT
{
	BOOL m_bCanDoTableFog;
	BOOL m_bCanDoVertexFog;
	BOOL m_bCanDoWFog;
	BOOL m_bCanDoRangeBasedFog;
	_KG3DENVEFFFOG_CAPS_STRUCT()
	{
		memset(this, 0, sizeof(_KG3DENVEFFFOG_CAPS_STRUCT));
	}
}KG3DENVEFFFOG_CAPS_STRUCT;

typedef struct _KG3DENVEFFFOG_DESCSTRUCT
{
	///KG3DENVEFFFOG_MDO_FOG_BASE
	BOOL       m_bFogEnable;

	D3DCOLOR   m_FogColor;///因为要和天空同步，所以不要直接使用这个，应该用KG3DScene的SetFogColor和GetFogColor
	int        m_nFogMode;

	BOOL       m_bUsingTableFog;
	///KG3DENVEFFFOG_MDO_VERTEX_FOG和KG3DENVEFFFOG_MDO_TABLE_FOG
	BOOL       m_bDeviceUsesWFog;
	BOOL       m_bRangeBasedFog;
	///KG3DENVEFFFOG_MDO_MODE_PARAM
	FLOAT      m_fFogStartValue;
	FLOAT      m_fFogEndValue;
	FLOAT      m_fFogDensity;

	_KG3DENVEFFFOG_DESCSTRUCT()
	{
		memset(this, 0, sizeof(_KG3DENVEFFFOG_DESCSTRUCT));
	}
}KG3DENVEFFFOG_DESCSTRUCT;


struct IEKG3DEnvEffFog
{
	virtual HRESULT RequestModification(const KG3DENVEFFFOG_DESCSTRUCT* pDescStruct, DWORD dwModificationOption) = 0;
	virtual HRESULT GetStruct(KG3DENVEFFFOG_DESCSTRUCT* pDescStruct) = 0;
	virtual HRESULT GetCaps(KG3DENVEFFFOG_CAPS_STRUCT* pRetCaps) = 0;
	virtual HRESULT GetFogColor(D3DCOLOR* pRetColor) = 0;
	virtual HRESULT SetFogColor(D3DCOLOR Color) = 0;
};

#endif //_INCLUDE_IEKG3DENVEFFFOG_H_
