////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DMeshSkyBox.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-11-17 17:39:38
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DMESHSKYBOX_H_
#define _INCLUDE_IEKG3DMESHSKYBOX_H_

////////////////////////////////////////////////////////////////////////////////
typedef enum _KG3DMESHSKYBOX_MDO
{
	KG3DMESHSKYBOX_MDO_ALL = 0xff,
}KG3DMESHSKYBOX_MDO;
typedef struct _KG3DMESHSKYBOX_DESCSTRUCT
{
	DWORD m_dwModificationOption;

    D3DCOLOR d3dc_xrgb_TopColor;
    D3DCOLOR d3dc_xrgb_MidColor;
    //D3DCOLOR d3dc_xrgb_BotmColor;

    int	nTopPercent;
    int nMidPercent;
    int nBotmPercent;

    int nTop_MidBlendPercent;
    int nMid_BotmBlendPercent;

	FLOAT fRadius;

    //int nTotalPercent;

    _KG3DMESHSKYBOX_DESCSTRUCT()
    {
        memset( this, 0, sizeof(_KG3DMESHSKYBOX_DESCSTRUCT));
    }
}KG3DMESHSKYBOX_DESCSTRUCT;

struct IEKG3DMeshSkyBox
{
    virtual HRESULT	RequestModification( const KG3DMESHSKYBOX_DESCSTRUCT* pSKStruct ) = 0;
    virtual HRESULT GetDescStruct( KG3DMESHSKYBOX_DESCSTRUCT* pSKStruct ) = 0;
    //virtual HRESULT IEEnableSynWithFogColor( BOOL bIsSynWithFogColor, D3DCOLOR d3dc_Fog ) = 0;	
    //virtual HRESULT IsEnableSynWithFogColor( BOOL* pbIsEnable ) = 0;
};
#endif //_INCLUDE_IEKG3DMESHSKYBOX_H_
