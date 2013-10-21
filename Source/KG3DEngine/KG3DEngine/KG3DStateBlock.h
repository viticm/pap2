////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DStateBlock.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008-7-15 17:38:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSTATEBLOCK_H_
#define _INCLUDE_KG3DSTATEBLOCK_H_

#include <vector>

////////////////////////////////////////////////////////////////////////////////

const unsigned INVALID_STATE_BLOCK_ID = (unsigned)-1;

class KG3DStateBlock
{
public:
    KG3DStateBlock(void);
    ~KG3DStateBlock(void);

    HRESULT BeginStateBlock();
    HRESULT EndStateBlock(unsigned *puRetStateBlockID); 

    HRESULT Apply(unsigned uStateBlockID); // call Restore() to restore state before call apply()
    HRESULT Restore();

    // ------------------------------------------------------------
    // The following methods can be recorded in a state block, 
    // after calling BeginStateBlock() and before EndStateBlock()
    void LightEnable(DWORD LightIndex, BOOL bEnable);
    void SetClipPlane(DWORD Index, CONST float *pPlane);
    void SetCurrentTexturePalette(UINT PaletteNumber);
    void SetFVF(DWORD FVF);
    //void SetIndices(IDirect3DIndexBuffer9 *pIndexData);
    void SetLight(DWORD Index, CONST D3DLIGHT9 *pLight);
    void SetMaterial(CONST D3DMATERIAL9 *pMaterial);    
    //void SetNPatchMode(float nSegments);
    //void SetPixelShader(IDirect3DPixelShader9 *pShader);
    //void SetPixelShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount);
    //void SetPixelShaderConstantF(UINT StartRegister, CONST float *pConstantData, UINT Vector4fCount);
    //void SetPixelShaderConstantI(UINT StartRegister, CONST int *pConstantData, UINT Vector4iCount);
    void SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
    void SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
    //void SetScissorRect(CONST RECT *pRect);    
    //void SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride);
    //void SetStreamSourceFreq(UINT StreamNumber, UINT FrequencyParameter);    
    //void SetTexture(DWORD Sampler, IDirect3DBaseTexture9 *pTexture);
    void SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
    void SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX *pMatrix);
    void SetViewport(CONST D3DVIEWPORT9 *pViewport);
    //void SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl);
    //void SetVertexShader(IDirect3DVertexShader9 *pShader);
    //void SetVertexShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount);
    //void SetVertexShaderConstantF(UINT StartRegister, CONST float *pConstantData,UINT Vector4fCount);
    //void SetVertexShaderConstantI(UINT StartRegister, CONST int *pConstantData, UINT Vector4iCount);

private:
    unsigned _ComputeStateBlockHash();
    unsigned _FindStateBlock();
    HRESULT _CreateD3DStateBlock(IDirect3DStateBlock9 **ppiRetD3DStateBlock);

private:
    struct _STATE_BLOCK;
    std::vector<_STATE_BLOCK *> m_StatBlockList;
    _STATE_BLOCK               *m_pNewStatBlock;
    unsigned m_uCurApplytatBlockID;
};

////////////////////////////////////////////////////////////////////////////////

extern KG3DStateBlock g_StateBlock;

#endif //_INCLUDE_KG3DSTATEBLOCK_H_
