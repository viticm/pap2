////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DTextureFrameDrawer.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-29 14:57:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DTextureFrameDrawer.h"
#include "KG3DShaderManager.h"
#include "KG3DShaderHolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

/*
KG3DTextureFrameDrawer::KG3DTextureFrameDrawer()
	:m_pShaderHolder(NULL)
	,m_bHasBegin(false)
{
	
}

KG3DTextureFrameDrawer::~KG3DTextureFrameDrawer()
{
	SAFE_RELEASE(m_pShaderHolder);
}

HRESULT KG3DTextureFrameDrawer::Begin()
{
#if 0
	_ASSERTE(! m_bHasBegin && _T("上次Begin没有调用End吧，检查一下"));
	TCHAR	ShaderPath[] = _T("data/public/shader/TexFrame.fx");
	if (NULL == m_pShaderHolder)
	{
		m_pShaderHolder = g_GetShaderManager().Create(g_pd3dDevice, ShaderPath);
		_ASSERTE(NULL != m_pShaderHolder);
	}

	LPD3DXEFFECT pEffect = m_pShaderHolder->GetEffect();
	_ASSERTE(NULL != pEffect);

	HRESULT hr = pEffect->Begin(NULL, 0);
	if (SUCCEEDED(hr))
	{
		hr = pEffect->BeginPass(0);
		if (SUCCEEDED(hr))
		{
			m_bHasBegin = true;
			return S_OK;
		}
	}
#endif
	return E_FAIL;
}

HRESULT KG3DTextureFrameDrawer::End()
{
	_ASSERTE(m_bHasBegin && _T("Begin要检查成功与否，不成功不要调用End"));
	_ASSERTE(NULL != m_pShaderHolder);
	LPD3DXEFFECT pEffect = m_pShaderHolder->GetEffect();
	_ASSERTE(NULL != pEffect);

	pEffect->EndPass();
	pEffect->End();
	return S_OK;
}

HRESULT KG3DTextureFrameDrawer::Draw( const D3DXVECTOR3* pVerticeBuffer, size_t BufferCount , const D3DXMATRIX* pMat  )
{
	return E_NOTIMPL;
}

HRESULT KG3DTextureFrameDrawer::SetTexture( LPDIRECT3DTEXTURE9 pTexture )
{
	return E_NOTIMPL;
}
*/