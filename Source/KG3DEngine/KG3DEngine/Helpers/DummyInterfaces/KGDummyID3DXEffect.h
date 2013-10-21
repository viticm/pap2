////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGDummyID3DXEffect.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-13 11:16:47
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGDUMMYID3DXEFFECT_H_
#define _INCLUDE_KGDUMMYID3DXEFFECT_H_

////////////////////////////////////////////////////////////////////////////////
struct KGDummyID3DXEffect : public ID3DXEffect 
{
	// ID3DXBaseEffect
	STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) {return E_NOTIMPL;}
	STDMETHOD_(ULONG, AddRef)(THIS) {_ASSERTE(NULL); return 1;}
	STDMETHOD_(ULONG, Release)(THIS) {_ASSERTE(NULL); return 0;}

	// Descs
	STDMETHOD(GetDesc)(THIS_ D3DXEFFECT_DESC* pDesc) {return E_NOTIMPL;}
	STDMETHOD(GetParameterDesc)(THIS_ D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc) {return E_NOTIMPL;}
	STDMETHOD(GetTechniqueDesc)(THIS_ D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc) {return E_NOTIMPL;}
	STDMETHOD(GetPassDesc)(THIS_ D3DXHANDLE hPass, D3DXPASS_DESC* pDesc) {return E_NOTIMPL;}
	STDMETHOD(GetFunctionDesc)(THIS_ D3DXHANDLE hShader, D3DXFUNCTION_DESC* pDesc) {return E_NOTIMPL;}

	// Handle operations
	STDMETHOD_(D3DXHANDLE, GetParameter)(THIS_ D3DXHANDLE hParameter, UINT Index) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetParameterByName)(THIS_ D3DXHANDLE hParameter, LPCSTR pName) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetParameterBySemantic)(THIS_ D3DXHANDLE hParameter, LPCSTR pSemantic) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetParameterElement)(THIS_ D3DXHANDLE hParameter, UINT Index) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetTechnique)(THIS_ UINT Index) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetTechniqueByName)(THIS_ LPCSTR pName) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetPass)(THIS_ D3DXHANDLE hTechnique, UINT Index) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetPassByName)(THIS_ D3DXHANDLE hTechnique, LPCSTR pName) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetFunction)(THIS_ UINT Index) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetFunctionByName)(THIS_ LPCSTR pName) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetAnnotation)(THIS_ D3DXHANDLE hObject, UINT Index) {return NULL;}
	STDMETHOD_(D3DXHANDLE, GetAnnotationByName)(THIS_ D3DXHANDLE hObject, LPCSTR pName) {return NULL;}

	// Get/Set Parameters
	STDMETHOD(SetValue)(THIS_ D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes) {return E_NOTIMPL;}
	STDMETHOD(GetValue)(THIS_ D3DXHANDLE hParameter, LPVOID pData, UINT Bytes) {return E_NOTIMPL;}
	STDMETHOD(SetBool)(THIS_ D3DXHANDLE hParameter, BOOL b) {return E_NOTIMPL;}
	STDMETHOD(GetBool)(THIS_ D3DXHANDLE hParameter, BOOL* pb) {return E_NOTIMPL;}
	STDMETHOD(SetBoolArray)(THIS_ D3DXHANDLE hParameter, CONST BOOL* pb, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(GetBoolArray)(THIS_ D3DXHANDLE hParameter, BOOL* pb, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(SetInt)(THIS_ D3DXHANDLE hParameter, INT n) {return E_NOTIMPL;}
	STDMETHOD(GetInt)(THIS_ D3DXHANDLE hParameter, INT* pn) {return E_NOTIMPL;}
	STDMETHOD(SetIntArray)(THIS_ D3DXHANDLE hParameter, CONST INT* pn, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(GetIntArray)(THIS_ D3DXHANDLE hParameter, INT* pn, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(SetFloat)(THIS_ D3DXHANDLE hParameter, FLOAT f) {return E_NOTIMPL;}
	STDMETHOD(GetFloat)(THIS_ D3DXHANDLE hParameter, FLOAT* pf) {return E_NOTIMPL;}
	STDMETHOD(SetFloatArray)(THIS_ D3DXHANDLE hParameter, CONST FLOAT* pf, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(GetFloatArray)(THIS_ D3DXHANDLE hParameter, FLOAT* pf, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(SetVector)(THIS_ D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector) {return E_NOTIMPL;}
	STDMETHOD(GetVector)(THIS_ D3DXHANDLE hParameter, D3DXVECTOR4* pVector) {return E_NOTIMPL;}
	STDMETHOD(SetVectorArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(GetVectorArray)(THIS_ D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(SetMatrix)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) {return E_NOTIMPL;}
	STDMETHOD(GetMatrix)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) {return E_NOTIMPL;}
	STDMETHOD(SetMatrixArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(GetMatrixArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(SetMatrixPointerArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(GetMatrixPointerArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(SetMatrixTranspose)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) {return E_NOTIMPL;}
	STDMETHOD(GetMatrixTranspose)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) {return E_NOTIMPL;}
	STDMETHOD(SetMatrixTransposeArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(GetMatrixTransposeArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(SetMatrixTransposePointerArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(GetMatrixTransposePointerArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) {return E_NOTIMPL;}
	STDMETHOD(SetString)(THIS_ D3DXHANDLE hParameter, LPCSTR pString) {return E_NOTIMPL;}
	STDMETHOD(GetString)(THIS_ D3DXHANDLE hParameter, LPCSTR* ppString) {return E_NOTIMPL;}
	STDMETHOD(SetTexture)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture) {return E_NOTIMPL;}
	STDMETHOD(GetTexture)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 *ppTexture) {return E_NOTIMPL;}
	STDMETHOD(SetPixelShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9 pPShader) {return E_NOTIMPL;}
	STDMETHOD(GetPixelShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9 *ppPShader) {return E_NOTIMPL;}
	STDMETHOD(SetVertexShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9 pVShader) {return E_NOTIMPL;}
	STDMETHOD(GetVertexShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9 *ppVShader) {return E_NOTIMPL;}

	//Set Range of an Array to pass to device
	//Usefull for sending only a subrange of an array down to the device
	STDMETHOD(SetArrayRange)(THIS_ D3DXHANDLE hParameter, UINT uStart, UINT uEnd) {return E_NOTIMPL;} 
	// ID3DXBaseEffect


	// Pool
	STDMETHOD(GetPool)(THIS_ LPD3DXEFFECTPOOL* ppPool) {return E_NOTIMPL;}

	// Selecting and setting a technique
	STDMETHOD(SetTechnique)(THIS_ D3DXHANDLE hTechnique) {return E_NOTIMPL;}
	STDMETHOD_(D3DXHANDLE, GetCurrentTechnique)(THIS) {return NULL;}
	STDMETHOD(ValidateTechnique)(THIS_ D3DXHANDLE hTechnique) {return E_NOTIMPL;}
	STDMETHOD(FindNextValidTechnique)(THIS_ D3DXHANDLE hTechnique, D3DXHANDLE *pTechnique) {return E_NOTIMPL;}
	STDMETHOD_(BOOL, IsParameterUsed)(THIS_ D3DXHANDLE hParameter, D3DXHANDLE hTechnique) {return E_NOTIMPL;}

	// Using current technique
	// Begin           starts active technique
	// BeginPass       begins a pass
	// CommitChanges   updates changes to any set calls in the pass. This should be called before
	//                 any DrawPrimitive call to d3d
	// EndPass         ends a pass
	// End             ends active technique
	STDMETHOD(Begin)(THIS_ UINT *pPasses, DWORD Flags) {return E_NOTIMPL;}
	STDMETHOD(BeginPass)(THIS_ UINT Pass) {return E_NOTIMPL;}
	STDMETHOD(CommitChanges)(THIS) {return E_NOTIMPL;}
	STDMETHOD(EndPass)(THIS) {return E_NOTIMPL;}
	STDMETHOD(End)(THIS) {return E_NOTIMPL;}

	// Managing D3D Device
	STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) {return E_NOTIMPL;}
	STDMETHOD(OnLostDevice)(THIS) {return E_NOTIMPL;}
	STDMETHOD(OnResetDevice)(THIS) {return E_NOTIMPL;}

	// Logging device calls
	STDMETHOD(SetStateManager)(THIS_ LPD3DXEFFECTSTATEMANAGER pManager) {return E_NOTIMPL;}
	STDMETHOD(GetStateManager)(THIS_ LPD3DXEFFECTSTATEMANAGER *ppManager) {return E_NOTIMPL;}

	// Parameter blocks
	STDMETHOD(BeginParameterBlock)(THIS) {return E_NOTIMPL;}
	STDMETHOD_(D3DXHANDLE, EndParameterBlock)(THIS) {return NULL;}
	STDMETHOD(ApplyParameterBlock)(THIS_ D3DXHANDLE hParameterBlock) {return E_NOTIMPL;}
	STDMETHOD(DeleteParameterBlock)(THIS_ D3DXHANDLE hParameterBlock) {return E_NOTIMPL;}

	// Cloning
	STDMETHOD(CloneEffect)(THIS_ LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT* ppEffect) {return E_NOTIMPL;}

	// Fast path for setting variables directly in ID3DXEffect
	STDMETHOD(SetRawValue)(THIS_ D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes) {return E_NOTIMPL;}
	virtual ~KGDummyID3DXEffect() = 0{}
};

struct KGDummyID3DXEffectRefImp : public KGDummyID3DXEffect
{
	STDMETHOD_(ULONG, AddRef)(THIS) {
        return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
    }
	STDMETHOD_(ULONG, Release)(THIS) 
    {
        ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
        if (uNewRefCount == 0)
            delete this; 
        return uNewRefCount;
    }
	KGDummyID3DXEffectRefImp():m_ulRefCount(1){}
protected:
	ULONG m_ulRefCount;
};

#endif //_INCLUDE_KGDUMMYID3DXEFFECT_H_
