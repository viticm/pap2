////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DMeshFarMountain.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-11-15 10:32:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DMESHFARMOUNTAIN_H_
#define _INCLUDE_IEKG3DMESHFARMOUNTAIN_H_

////////////////////////////////////////////////////////////////////////////////
typedef enum _IEKG3DMESHFARMOUNTAIN_MDO
{
	KG3DMESHFARMOUNTAIN_MDO_DEFAULT = 0x00,		///什么也不改
	KG3DMESHFARMOUNTAIN_MDO_MESH = 0x01,		///改Mesh
	KG3DMESHFARMOUNTAIN_MDO_TEX = 0x02,			///重新绘制由程序生成的贴图
	KG3DMESHFARMOUNTAIN_MDO_TEXPATH = 0x04,		///重新载入贴图
	KG3DMESHFARMOUNTAIN_MDO_SYNWITHFOG = 0x08,	//无用了
	KG3DMESHFARMOUNTAIN_MDO_ENABLETEXCOLOR = 0x10,
	KG3DMESHFARMOUNTAIN_MDO_SPEED = 0x20,		///速度
    KG3DMESHFARMOUNTAIN_MDO_TEXFACTOR = 0x40,   ///贴图明度 
	KG3DMESHFARMOUNTAIN_MDO_ALL = 0xff,			///修改所有的
} IEKG3DMESHFARMOUNTAIN_MDO;
////////////////////////////////////////////////////////////////////////////////
typedef struct _KG3DMESHFARMOUNTAIN_DESCSTRUCT{
	///修改选项
	DWORD	m_dwModificationOption;
	///形状相关
	float m_fRadius;
	float m_fHeight;
	float m_fBotmZPos;
	float	m_fZRotation;
	float	m_fTexCordUTiles;
	
	///下面是贴图相关		
	//D3DCOLOR m_d3dc_Color;			///贴图的主色
	//bool m_bEnableSynWithFogColor;	///允许同步雾色，这个项不能直接使用
	bool m_bEnableTexColor;			///作为远山使用的时候，仅仅使用颜色就够了，不需要使用到贴图的颜色
	D3DCOLOR m_d3dc_FogColor;		///雾的颜色
	float	m_fFogBlendFactor;		///雾颜色与贴图色的混合系数
	float	m_fBotmBlendHeight;			///取实际高度，然后贴图的Blend由比例得出
	
	float	m_fSpeed;				///旋转速度

    float   m_fTexFactor;

	///贴图的路径指针，如果不为0的话就需要修改贴图
	TCHAR	m_strTextureFullPath[MAX_PATH];

	_KG3DMESHFARMOUNTAIN_DESCSTRUCT()///默认构造函数
	{
		memset(this, 0, sizeof(_KG3DMESHFARMOUNTAIN_DESCSTRUCT));
	}	

}KG3DMESHFARMOUNTAIN_DESCSTRUCT, *LPKG3DMESHFARMOUNTAIN_DESCSTRUCT;

////////////////////////////////////////////////////////////////////////////////
struct IEKG3DMeshFarMountain{
	virtual HRESULT GetName( LPTSTR szName, UINT uBufferCount ) = 0;
	virtual HRESULT SetName( LPTSTR szName ) = 0;
	virtual HRESULT	RequestModification( const KG3DMESHFARMOUNTAIN_DESCSTRUCT* pStruct ) = 0;///可以用pStruct中的ModificationOption设定改什么东西
	virtual HRESULT	GetStruct( KG3DMESHFARMOUNTAIN_DESCSTRUCT* pResultStruct ) = 0;
	//virtual HRESULT IsEnableSynWithFogColor( BOOL* pBool ) = 0;	
};


#endif //_INCLUDE_IEKG3DMESHFARMOUNTAIN_H_
