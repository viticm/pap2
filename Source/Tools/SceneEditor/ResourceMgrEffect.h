// ResourceMgrEffect.h: interface for the ResourceMgrEffect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEMGREFFECT_H__C38675FA_EC89_4EBB_BA09_4A917535FE1B__INCLUDED_)
#define AFX_RESOURCEMGREFFECT_H__C38675FA_EC89_4EBB_BA09_4A917535FE1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "./ResourceManager.h"
using namespace std;
using namespace stdext;


class CStateManager : public ID3DXEffectStateManager
{
	ULONG refCount;
public:

	~CStateManager() {}


	CStateManager():refCount(1) {}

	STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);
	STDMETHOD(SetMaterial)(THIS_ CONST D3DMATERIAL9 *pMaterial);
	STDMETHOD(SetLight)(THIS_ DWORD Index, CONST D3DLIGHT9 *pLight);
	STDMETHOD(LightEnable)(THIS_ DWORD Index, BOOL Enable);
	STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE State, DWORD Value);
	STDMETHOD(SetTexture)(THIS_ DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture);
	STDMETHOD(SetTextureStageState)(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
	STDMETHOD(SetSamplerState)(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
	STDMETHOD(SetNPatchMode)(THIS_ FLOAT NumSegments);
	STDMETHOD(SetFVF)(THIS_ DWORD FVF);
	STDMETHOD(SetVertexShader)(THIS_ LPDIRECT3DVERTEXSHADER9 pShader);
	STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount);
	STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount);
	STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount);
	STDMETHOD(SetPixelShader)(THIS_ LPDIRECT3DPIXELSHADER9 pShader);
	STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount);
	STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount);
	STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount);
};

typedef class KResourceHandleEffect : public KResourceHandle
{
public:
	LPD3DXEFFECT pEffect;
	LPD3DXBUFFER m_lpFxErrorMsg;

public:
	HRESULT LoadFromFile(const char *szFilename,DWORD Option = 0,DWORD Operator=0);//从文件中载入

	KResourceHandleEffect();
	virtual ~KResourceHandleEffect();

	HRESULT OnLostDevice(void);
	HRESULT OnResetDevice(void);

}* HEFFECT;

extern KResourceManager<KResourceHandleEffect>g_EffectMgr;


#endif // !defined(AFX_RESOURCEMGREFFECT_H__C38675FA_EC89_4EBB_BA09_4A917535FE1B__INCLUDED_)
