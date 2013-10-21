////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DCloud.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-11-21 10:55:25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _INCLUDE_IEKG3DCLOUD_H_
#define _INCLUDE_IEKG3DCLOUD_H_

////////////////////////////////////////////////////////////////////////////////
typedef enum _KG3DCLOUD_MDO
{
    KG3DCLOUD_MDO_DEFAULT = 0x00,		//什么也不改
    KG3DCLOUD_MDO_MESH = 1 << 0,		//改Mesh
    KG3DCLOUD_MDO_MOVEMENT = 1 << 1,
    KG3DCLOUD_MDO_COLORTEX = 1 << 2,	//重新绘制由程序生成的贴图
    KG3DCLOUD_MDO_MASKTEX = 1 << 3,		//重新载入贴图
	KG3DCLOUD_MDO_VISIBILITY = 1 << 4,	//改整体透明度
    KG3DCLOUD_MDO_ALPHAREF   = 1 << 5,
	KG3DCLOUD_MDO_COLOR = 1 << 6,		//用颜色控制云
	KG3DCLOUD_MDO_ENABLE_CONTRL_COLOR = 1 << 7,
    KG3DCLOUD_MDO_TEXFACTOR = 1 << 8,
    KG3DCLOUD_MDO_ALL = 0xffff,			//修改所有的
} KG3D3DCLOUD_MDO;
////////////////////////////////////////////////////////////////////////////////
typedef struct _KG3DCLOUD_DESCSTRUCT
{
    DWORD	m_dwMidificationOption;
    ///由MESH控制
    FLOAT	m_fHeight;
    FLOAT	m_fSize;
    INT		m_nUVRepeat;
    ///由MOVEMENT控制
    FLOAT	m_fSpeed;
    FLOAT	m_fAngel;
    ///由COLORTEX控制
    LPTSTR	m_tszColorMapPath;
    ///由MASKTEX控制
    LPTSTR	m_tszMaskMapPath;
	
	///由KG3DCLOUD_MDO_VISIBILITY控制
	FLOAT	m_fVisibility;

    FLOAT   m_fAlphaRef;
	
	///由COLOR控制
	DWORD	m_diffuseColor;
	DWORD	m_ambientColor;

	BOOL	m_bEnableControlColor;

    FLOAT   m_fTexFector;

    _KG3DCLOUD_DESCSTRUCT()
    {
        memset(this, 0, sizeof(_KG3DCLOUD_DESCSTRUCT));
    }
}KG3DCLOUD_DESCSTRUCT, *LPKG3DCLOUD_DESCSTRUCT;

struct IEKG3DCloud 
{
    virtual HRESULT	RequestModification( const KG3DCLOUD_DESCSTRUCT* pStruct ) = 0;///可以用pStruct中的ModificationOption设定改什么东西
    virtual HRESULT	GetStruct( KG3DCLOUD_DESCSTRUCT* pResultStruct ) = 0;
};

#endif //_INCLUDE_IEKG3DCLOUD_H_
