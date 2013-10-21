// ResourceMgrEffect.cpp: implementation of the ResourceMgrEffect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "ResourceMgrEffect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KResourceManager<KResourceHandleEffect>g_EffectMgr;

CStateManager g_EffectStateManager;

//KTResourceManagerBase<LPD3DXEFFECT>g_cEffectTable;

KResourceHandleEffect::KResourceHandleEffect()
{
	pEffect = NULL;
	m_lpFxErrorMsg = NULL;
}

KResourceHandleEffect::~KResourceHandleEffect()
{
	SAFE_RELEASE(m_lpFxErrorMsg);
}

HRESULT KResourceHandleEffect::LoadFromFile(const char *szFilename,DWORD Option,DWORD Operator)
{
	HRESULT hr = S_OK;
	//TCHAR str[256];

	SAFE_RELEASE(pEffect);

	//D3DXMACRO aDefines[3];

	DWORD dwShaderFlags = 0;
#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

	//hr = D3DXCreateEffectFromFile( g_pd3dDevice, szFilename, NULL, NULL, 
	//	dwShaderFlags, NULL, &pEffect, &m_lpFxErrorMsg );

	//// If this fails, there should be debug output as to 
	//// they the .fx file failed to compile
	////pEffect->SetStateManager(&g_EffectStateManager);
	//
	//if( FAILED( hr ) )
	//{
	//	// If there was an error lets get the errors so we can display them in an message box.
	//	LPVOID compileErrors = m_lpFxErrorMsg->GetBufferPointer();

	//	// Show the errors to the user.
	//	MessageBox(NULL, (const char*)compileErrors, "Shader Compile Error...",
	//		MB_OK | MB_ICONEXCLAMATION);

	//	return hr ;//DXTRACE_ERR( TEXT("D3DXCreateEffectFromFile"), hr );
	//}

	//m_szFileName = szFilename;
	return S_OK;
}

HRESULT KResourceHandleEffect::InvalidateDeviceObjects(void)
{
	if(pEffect)
	{
		return pEffect->OnLostDevice();
	}
	
	return S_OK;
}

HRESULT KResourceHandleEffect::RestoreDeviceObjects(void)
{
	if(pEffect)
	{
		return pEffect->OnResetDevice();
	}

	return S_OK;

	HRESULT hr = S_OK;
	DWORD dwShaderFlags = 0;
#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

	//hr = D3DXCreateEffectFromFile( g_pd3dDevice, m_szFileName.c_str(), NULL, NULL, 
	//	dwShaderFlags, NULL, &pEffect, NULL );

	//// If this fails, there should be debug output as to 
	//// they the .fx file failed to compile
	////pEffect->SetStateManager(&g_EffectStateManager);

	//if( FAILED( hr ) )
	//	return hr ;//DXTRACE_ERR( TEXT("D3DXCreateEffectFromFile"), hr );
	return S_OK;
}


HRESULT CStateManager::QueryInterface(REFIID iid, void ** ppvObject)
{
	if(iid == IID_ID3DXEffectStateManager) 
	{
		*ppvObject=this;
		return S_OK;
	}
	*ppvObject=0;
	return  E_NOINTERFACE;
}

ULONG CStateManager::AddRef()
{
	refCount++; 
	return refCount;
}

ULONG CStateManager::Release()
{
	if((--refCount) == 0 ) 
	{
		delete this; 
		return 0;
	} 
	return refCount;
}

HRESULT CStateManager::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix)
{
	return S_OK;
}

HRESULT CStateManager::SetMaterial(const D3DMATERIAL9* pMaterial)
{
	return S_OK;
}

HRESULT CStateManager::SetLight(DWORD Index, const D3DLIGHT9* pLight)
{
	return S_OK;
}

HRESULT CStateManager::LightEnable(DWORD Index, BOOL Enable)
{
	return S_OK;
}

HRESULT CStateManager::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	return S_OK;
}

HRESULT CStateManager::SetTexture(DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture)
{
	return S_OK;
}

HRESULT CStateManager::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	return S_OK;
}

HRESULT CStateManager::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	return S_OK;
}

HRESULT CStateManager::SetNPatchMode(FLOAT NumSegments)
{
	return S_OK;
}

HRESULT CStateManager::SetFVF(DWORD FVF)
{
	return S_OK;
}

HRESULT CStateManager::SetVertexShader(LPDIRECT3DVERTEXSHADER9 pShader)
{
	return S_OK;
}

HRESULT CStateManager::SetVertexShaderConstantF(UINT RegisterIndex, const FLOAT* pConstantData, UINT RegisterCount)
{
	return S_OK;
}

HRESULT CStateManager::SetVertexShaderConstantI(UINT RegisterIndex, const INT* pConstantData, UINT RegisterCount)
{
	return S_OK;
}

HRESULT CStateManager::SetVertexShaderConstantB(UINT RegisterIndex, const BOOL* pConstantData, UINT RegisterCount)
{
	return S_OK;
}

HRESULT CStateManager::SetPixelShader(LPDIRECT3DPIXELSHADER9 pShader)
{
	return S_OK;
}

HRESULT CStateManager::SetPixelShaderConstantF(UINT RegisterIndex, const FLOAT* pConstantData, UINT RegisterCount)
{
	return S_OK;
}

HRESULT CStateManager::SetPixelShaderConstantI(UINT RegisterIndex, const INT* pConstantData, UINT RegisterCount)
{
	return S_OK;
}

HRESULT CStateManager::SetPixelShaderConstantB(UINT RegisterIndex, const BOOL* pConstantData, UINT RegisterCount)
{
	return S_OK;
}
