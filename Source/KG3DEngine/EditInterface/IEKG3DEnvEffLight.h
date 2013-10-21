////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DEnvEffLight.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-11-22 11:51:14
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DENVEFFLIGHT_H_
#define _INCLUDE_IEKG3DENVEFFLIGHT_H_

////////////////////////////////////////////////////////////////////////////////
typedef enum _KG3DENVEFFLIGHT_MDO
{
	KG3DENVEFFLIGHT_MDO_ENABLELIGHT = 0x001,
	KG3DENVEFFLIGHT_MDO_D3DLIGHT_DIFFUSE = 0x002,
	KG3DENVEFFLIGHT_MDO_D3DLIGHT_AMBIENT = 0x004,
	KG3DENVEFFLIGHT_MDO_D3DLIGHT_SPECULAR = 0x008,
	KG3DENVEFFLIGHT_MDO_D3DLIGHT_DIRECTION = 0x010,
	KG3DENVEFFLIGHT_MDO_D3DLIGHT_POSITION = 0x20,
	KG3DENVEFFLIGHT_MDO_D3DLIGHT_OTHER = 0x040,
	KG3DENVEFFLIGHT_MDO_D3DLIGHT = 0xff,
	KG3DENVEFFLIGHT_MDO_SKYLIGHT = 0x100,
	KG3DENVEFFLIGHT_MDO_COMPLIGHT = 0x200,
	KG3DENVEFFLIGHT_MDO_SCENEAMBIENT = 0x400,
	KG3DENVEFFLIGHT_MDO_COLORARRAY_3 = 0x800,
	KG3DENVEFFLIGHT_MDO_COLORARRAY = KG3DENVEFFLIGHT_MDO_SKYLIGHT | KG3DENVEFFLIGHT_MDO_COMPLIGHT 
						| KG3DENVEFFLIGHT_MDO_SCENEAMBIENT | KG3DENVEFFLIGHT_MDO_COLORARRAY_3,
	KG3DENVEFFLIGHT_MDO_ALL = 0xffff,
}KG3DENVEFFLIGHT_MDO;
typedef struct _KG3DENVEFFLIGHT_DESCSTRUCT
{
	DWORD m_dwModificationOption;
	
	enum
	{
		color_arg_sky_light = 0,
		color_arg_comp_light = 1,
		color_arg_scene_ambient = 2,
		color_arg_not_use_current0 = 3,	//保留的一个arg
		color_arg_num = 4,	//保留最后一个arg
	};
	
	D3DLIGHT9	m_D3DLight;
	D3DCOLOR	m_ColorArgs[color_arg_num];	//保存4个颜色，作为Shader的使用
	
	void SetLightEnable(BOOL bEnable){m_dwModificationOption = bEnable?(m_dwModificationOption | KG3DENVEFFLIGHT_MDO_ENABLELIGHT):(m_dwModificationOption & (~KG3DENVEFFLIGHT_MDO_ENABLELIGHT));}
	BOOL IsLightEnable()const{return m_dwModificationOption & KG3DENVEFFLIGHT_MDO_ENABLELIGHT;  }
#ifdef __cplusplus
	_KG3DENVEFFLIGHT_DESCSTRUCT(){ZeroMemory(this,sizeof(_KG3DENVEFFLIGHT_DESCSTRUCT));this->SetLightEnable(TRUE);}
#endif
}KG3DENVEFFLIGHT_DESCSTRUCT;

typedef struct IEKG3DEnvEffLight
{
	virtual HRESULT RequestModification(const KG3DENVEFFLIGHT_DESCSTRUCT* pDescStruct) = 0;
	virtual HRESULT GetStruct(KG3DENVEFFLIGHT_DESCSTRUCT* pDescStruct) = 0;
	virtual HRESULT Copy(IEKG3DEnvEffLight* pOtherLight) = 0;
}IEKG3DLight;

#endif //_INCLUDE_IEKG3DENVEFFLIGHT_H_
