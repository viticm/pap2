////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DStateBlock.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008-7-15 17:38:48
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DStateBlock.h"
#include "Common/CRC32.h"

////////////////////////////////////////////////////////////////////////////////

enum _STATE_TYPE
{
    STATE_TYPE_LIGHT_ENABLE = 0,
    STATE_TYPE_CLIP_PLANE = 1,
    STATE_TYPE_CURRENT_TEXTURE_PALETTE = 2,
    STATE_TYPE_FVF = 3,
    //STATE_TYPE_INDICES = 4,
    STATE_TYPE_LIGHT = 5,
    STATE_TYPE_MATERIAL = 6,
    //STATE_TYPE_NPATCH_MODE = 7,
    //STATE_TYPE_PIXEL_SHADER = 8,
    //STATE_TYPE_PIXEL_SHADER_CONSTANT_B = 9,
    //STATE_TYPE_PIXEL_SHADER_CONSTANT_F = 10,
    //STATE_TYPE_PIXEL_SHADER_CONSTANT_I = 11,
    STATE_TYPE_RENDER_STATE = 12,
    STATE_TYPE_SAMPLER_STATE = 13,
    //STATE_TYPE_SCISSOR_RECT = 14,
    //STATE_TYPE_STREAM_SOURCE = 15,
    //STATE_TYPE_STREAM_SOURCE_FREQ = 16,
    //STATE_TYPE_TEXTURE = 17,
    STATE_TYPE_TEXTURE_STAGE_STATE = 18,
    STATE_TYPE_TRANSFORM = 19,
    STATE_TYPE_VIEWPORT = 20,
    //STATE_TYPE_VERTEX_DECLARATION = 21,
    //STATE_TYPE_VERTEX_SHADER = 22,
    //STATE_TYPE_VERTEX_SHADER_CONSTANT_B = 23,
    //STATE_TYPE_VERTEX_SHADER_CONSTANT_F = 24
    //STATE_TYPE_VERTEX_SHADER_CONSTANT_I = 25,
};
struct _STATE 
{
    _STATE_TYPE eStateType;
    union
    {
        struct{DWORD LightIndex; BOOL bEnable;} LightEnable;
        struct{DWORD Index; float aPlane[4];} ClipPlane;
        struct{UINT PaletteNumber;} CurrentTexturePalette;
        struct{DWORD FVF;} FVF;
        //struct{IDirect3DIndexBuffer9 *pIndexData;} Indices;
        struct{DWORD Index; D3DLIGHT9 Light;} Light;
        struct{D3DMATERIAL9 Material;} Material;    
        //struct{float nSegments;} NPatchMode;
        //struct{IDirect3DPixelShader9 *pShader;} PixelShader;
        //struct{UINT StartRegister; CONST BOOL *pConstantData; UINT BoolCount;} PixelShaderConstantB;
        //struct{UINT StartRegister; CONST float *pConstantData; UINT Vector4fCount;} PixelShaderConstantF;
        //struct{UINT StartRegister; CONST int *pConstantData; UINT Vector4iCount;} PixelShaderConstantI;
        struct{D3DRENDERSTATETYPE State; DWORD Value;} RenderState;
        struct{DWORD Sampler; D3DSAMPLERSTATETYPE Type; DWORD Value;} SamplerState;
        //struct{CONST RECT *pRect;} ScissorRect;    
        //struct{UINT StreamNumber; IDirect3DVertexBuffer9 *pStreamData; UINT OffsetInBytes; UINT Stride;} StreamSource;
        //struct{UINT StreamNumber; UINT FrequencyParameter;} StreamSourceFreq;    
        //struct{DWORD Sampler; IDirect3DBaseTexture9 *pTexture;} Texture;
        struct{DWORD Stage; D3DTEXTURESTAGESTATETYPE Type; DWORD Value;} TextureStageState;
        struct{D3DTRANSFORMSTATETYPE State; D3DMATRIX Matrix;} Transform;
        struct{D3DVIEWPORT9 Viewport;} Viewport;
        //struct{IDirect3DVertexDeclaration9 *pDecl;} VertexDeclaration;
        //struct{IDirect3DVertexShader9 *pShader;} VertexShader;
        //struct{UINT StartRegister; CONST BOOL *pConstantData; UINT BoolCount;} VertexShaderConstantB;
        //struct{UINT StartRegister; CONST float *pConstantData;UINT Vector4fCount;} VertexShaderConstantF;
        //struct{UINT StartRegister; CONST int *pConstantData; UINT Vector4iCount;} VertexShaderConstantI;
    };

    ////////////////////////////////////////////////////////////////////////////
    int operator<(const _STATE &crRight) const
    {
        return memcmp((void *)this, (void *)&crRight, sizeof(this)) < 0;
    }
};

struct KG3DStateBlock::_STATE_BLOCK 
{
    IDirect3DStateBlock9   *piStateBlock;
    IDirect3DStateBlock9   *piStateBlockSave;
    unsigned                uStateBlockHash; // to speed up compare state block
    std::vector<_STATE>     States;

#ifdef _DEBUG
    unsigned uRefCount;
#endif // _DEBUG
};

////////////////////////////////////////////////////////////////////////////////
KG3DStateBlock g_StateBlock;

KG3DStateBlock::KG3DStateBlock(void):
    m_pNewStatBlock(NULL),
    m_uCurApplytatBlockID(INVALID_STATE_BLOCK_ID)
{
    m_StatBlockList.reserve(32);
};

KG3DStateBlock::~KG3DStateBlock(void)
{
    std::vector<_STATE_BLOCK *>::iterator it;
    _STATE_BLOCK *pStateBlock = NULL;
    
    ASSERT(!m_pNewStatBlock);
    KG_DELETE(m_pNewStatBlock);

    for (it = m_StatBlockList.begin(); it != m_StatBlockList.end(); ++it)
    {
        pStateBlock = *it;
        ASSERT(pStateBlock);
        ASSERT(pStateBlock->piStateBlock);
        KG_COM_RELEASE(pStateBlock->piStateBlock);
        KG_COM_RELEASE(pStateBlock->piStateBlockSave);
        
        KG_DELETE(pStateBlock);
        *it = NULL;
    }
    m_StatBlockList.clear();
};

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DStateBlock::BeginStateBlock()
{
    HRESULT hResult  = E_FAIL;

    KG_ASSERT_EXIT(!m_pNewStatBlock);

    m_pNewStatBlock = new _STATE_BLOCK;
    KGLOG_PROCESS_ERROR(m_pNewStatBlock);

    m_pNewStatBlock->piStateBlock = NULL;
    m_pNewStatBlock->piStateBlockSave = NULL;
    m_pNewStatBlock->uStateBlockHash = 0;
#ifdef _DEBUG
    m_pNewStatBlock->uRefCount = 0;
#endif // _DEBUG

    hResult = S_OK;
Exit0:
    if (FAILED(hResult))
    {
        KG_DELETE(m_pNewStatBlock);
    }
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DStateBlock::EndStateBlock(unsigned *puRetStateBlockID)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    
    KG_ASSERT_EXIT(puRetStateBlockID);

    KG_ASSERT_EXIT(m_pNewStatBlock);
    
    sort(m_pNewStatBlock->States.begin(), m_pNewStatBlock->States.end());
    m_pNewStatBlock->uStateBlockHash = _ComputeStateBlockHash();

    *puRetStateBlockID = _FindStateBlock();
    KG_PROCESS_SUCCESS(*puRetStateBlockID != INVALID_STATE_BLOCK_ID); // use exist
    
    // create new
    hRetCode = _CreateD3DStateBlock(&(m_pNewStatBlock->piStateBlock));
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    // the second one if for save state before apply
    hRetCode = _CreateD3DStateBlock(&(m_pNewStatBlock->piStateBlockSave));
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    *puRetStateBlockID = (unsigned)m_StatBlockList.size();
    m_StatBlockList.push_back(m_pNewStatBlock);
    m_pNewStatBlock = NULL;
Exit1:
#ifdef _DEBUG
    ++m_StatBlockList[*puRetStateBlockID]->uRefCount;
#endif // _DEBUG
    hResult = S_OK;
Exit0:
    if (FAILED(hResult))
    {
        if (m_pNewStatBlock)
        {
            KG_COM_RELEASE(m_pNewStatBlock->piStateBlockSave);
            KG_COM_RELEASE(m_pNewStatBlock->piStateBlock);
        }
    }
    KG_DELETE(m_pNewStatBlock);
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DStateBlock::Apply(unsigned uStateBlockID)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    _STATE_BLOCK *pStateBlock = NULL;

    KG_ASSERT_EXIT(uStateBlockID < m_StatBlockList.size());

    pStateBlock = m_StatBlockList[uStateBlockID];

    // save current state
    hRetCode = pStateBlock->piStateBlockSave->Capture();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pStateBlock->piStateBlock->Apply();
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    m_uCurApplytatBlockID = uStateBlockID;

    hResult = S_OK;
Exit0:
    if (FAILED(hResult))
    {
        if (m_uCurApplytatBlockID != INVALID_STATE_BLOCK_ID)
        {
            hRetCode = pStateBlock->piStateBlockSave->Apply();
            m_uCurApplytatBlockID = INVALID_STATE_BLOCK_ID;
        }
    }
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DStateBlock::Restore()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG_ASSERT_EXIT(m_uCurApplytatBlockID < m_StatBlockList.size());

    hRetCode = m_StatBlockList[m_uCurApplytatBlockID]->piStateBlockSave->Apply();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    m_uCurApplytatBlockID = INVALID_STATE_BLOCK_ID;

    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock::LightEnable(DWORD LightIndex, BOOL bEnable)
{
    _STATE state;

    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_LIGHT_ENABLE;
    state.LightEnable.LightIndex = LightIndex;
    state.LightEnable.bEnable    = bEnable;
    
    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock::SetClipPlane(DWORD Index, CONST float *pPlane)
{
    _STATE state;

    ASSERT(pPlane);

    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_CLIP_PLANE;
    state.ClipPlane.Index = Index;
    memcpy(state.ClipPlane.aPlane, (void *)pPlane, sizeof(state.ClipPlane.aPlane));

    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock::SetCurrentTexturePalette(UINT PaletteNumber)
{
    _STATE state;

    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_CURRENT_TEXTURE_PALETTE;
    state.CurrentTexturePalette.PaletteNumber = PaletteNumber;

    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock::SetFVF(DWORD FVF)
{
    _STATE state;

    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_FVF;
    state.FVF.FVF    = FVF;

    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
//void KG3DStateBlock::SetIndices(IDirect3DIndexBuffer9 *pIndexData);
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock:: SetLight(DWORD Index, CONST D3DLIGHT9 *pLight)
{
    _STATE state;

    ASSERT(pLight);

    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_LIGHT;
    state.Light.Index = Index;
    state.Light.Light = *pLight;

    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock::SetMaterial(CONST D3DMATERIAL9 *pMaterial)
{
    _STATE state;

    ASSERT(pMaterial);

    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_MATERIAL;
    state.Material.Material = *pMaterial;

    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
//void KG3DStateBlock::SetNPatchMode(float nSegments);
//void KG3DStateBlock::SetPixelShader(IDirect3DPixelShader9 *pShader);
//void KG3DStateBlock::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount);
//void KG3DStateBlock::SetPixelShaderConstantF(UINT StartRegister, CONST float *pConstantData, UINT Vector4fCount);
//void KG3DStateBlock::SetPixelShaderConstantI(UINT StartRegister, CONST int *pConstantData, UINT Vector4iCount);
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
    _STATE state;

    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_RENDER_STATE;
    state.RenderState.State = State;
    state.RenderState.Value = Value;

    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    _STATE state;

    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_SAMPLER_STATE;
    state.SamplerState.Sampler = Sampler;
    state.SamplerState.Type    = Type;
    state.SamplerState.Value   = Value;

    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
//void KG3DStateBlock::SetScissorRect(CONST RECT *pRect);    
//void KG3DStateBlock::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride);
//void KG3DStateBlock::SetStreamSourceFreq(UINT StreamNumber, UINT FrequencyParameter);    
//void KG3DStateBlock::SetTexture(DWORD Sampler, IDirect3DBaseTexture9 *pTexture);
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    _STATE state;

    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_TEXTURE_STAGE_STATE;
    state.TextureStageState.Stage = Stage;
    state.TextureStageState.Type  = Type;
    state.TextureStageState.Value = Value;

    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock::SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX *pMatrix)
{
    _STATE state;

    ASSERT(pMatrix);
    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_TRANSFORM;
    state.Transform.State  = State;
    state.Transform.Matrix = *pMatrix;

    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
void KG3DStateBlock::SetViewport(CONST D3DVIEWPORT9 *pViewport)
{
    _STATE state;

    ASSERT(pViewport);
    memset(&state, 0, sizeof(state));
    state.eStateType = STATE_TYPE_VIEWPORT;
    state.Viewport.Viewport = *pViewport;

    ASSERT(m_pNewStatBlock);
    m_pNewStatBlock->States.push_back(state);
}
////////////////////////////////////////////////////////////////////////////////
//void KG3DStateBlock::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl);
//void KG3DStateBlock::SetVertexShader(IDirect3DVertexShader9 *pShader);
//void KG3DStateBlock::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount);
//void KG3DStateBlock::SetVertexShaderConstantF(UINT StartRegister, CONST float *pConstantData,UINT Vector4fCount);
//void KG3DStateBlock::SetVertexShaderConstantI(UINT StartRegister, CONST int *pConstantData, UINT Vector4iCount);
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
unsigned KG3DStateBlock::_ComputeStateBlockHash()
{
    HRESULT hResult  = E_FAIL;
    unsigned uCRC = 0;
    unsigned uStateCount = 0;
    unsigned char *pbyReadPos = NULL;

    KG_ASSERT_EXIT(m_pNewStatBlock);

    uStateCount = (DWORD)m_pNewStatBlock->States.size();
    if (uStateCount > 0)
    {
        pbyReadPos = (unsigned char *)&(m_pNewStatBlock->States[0]);
        uCRC = CRC32(uCRC, pbyReadPos, sizeof(m_pNewStatBlock->States[0]) * uStateCount);
    }

    hResult = S_OK;
Exit0:
    return uCRC;
}
////////////////////////////////////////////////////////////////////////////////
unsigned KG3DStateBlock::_FindStateBlock()
{
    HRESULT hResult  = E_FAIL;
    int nRetCode = false;
    unsigned uStatBlockID = INVALID_STATE_BLOCK_ID;
    unsigned i = 0;
    size_t uStateBlockCount = 0;
    size_t uStateCount = 0;
    _STATE_BLOCK *pStateBlock = NULL;

    KG_ASSERT_EXIT(m_pNewStatBlock);

    // just a little, not need use map

    uStateBlockCount = m_StatBlockList.size();
    for (i = 0; i < uStateBlockCount; ++i)
    {
        pStateBlock = m_StatBlockList[i];
        if (pStateBlock->uStateBlockHash != m_pNewStatBlock->uStateBlockHash)
            continue;

        uStateCount = (DWORD)m_pNewStatBlock->States.size();
        if (pStateBlock->States.size() != uStateCount)
            continue;
        
        nRetCode = memcmp(
            &(pStateBlock->States[0]),
            &(m_pNewStatBlock->States[0]),
            sizeof(m_pNewStatBlock->States[0]) * uStateCount
        );
        if (nRetCode != 0)
            continue;

        uStatBlockID = i;
        break; // found
    }

    hResult = S_OK;
Exit0:
    return uStatBlockID;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DStateBlock::_CreateD3DStateBlock(IDirect3DStateBlock9 **ppiRetD3DStateBlock)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    int nBeginStateBlockFlag = false;
    unsigned i = 0;
    size_t uStateCount = 0;
    _STATE *pState = NULL;

    ASSERT(m_pNewStatBlock);
    ASSERT(ppiRetD3DStateBlock);
    ASSERT(!(*ppiRetD3DStateBlock));

    hRetCode = g_pd3dDevice->BeginStateBlock();
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    nBeginStateBlockFlag = true;

    uStateCount = m_pNewStatBlock->States.size();
    pState = &(m_pNewStatBlock->States[0]);
    for (i = 0; i < uStateCount; ++i)
    {
        switch (pState->eStateType)
        {
        case STATE_TYPE_LIGHT_ENABLE:
            hRetCode = g_pd3dDevice->LightEnable(
                pState->LightEnable.LightIndex, pState->LightEnable.bEnable
            );
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        case STATE_TYPE_CLIP_PLANE:
            hRetCode = g_pd3dDevice->SetClipPlane(
                pState->ClipPlane.Index, pState->ClipPlane.aPlane
            );
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        case STATE_TYPE_CURRENT_TEXTURE_PALETTE:
            hRetCode = g_pd3dDevice->SetCurrentTexturePalette(
                pState->CurrentTexturePalette.PaletteNumber
            );
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        case STATE_TYPE_FVF:
            hRetCode = g_pd3dDevice->SetFVF(pState->FVF.FVF);
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        //case STATE_TYPE_INDICES:
        case STATE_TYPE_LIGHT:
            hRetCode = g_pd3dDevice->SetLight(
                pState->Light.Index, &(pState->Light.Light)
            );
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        case STATE_TYPE_MATERIAL:
            hRetCode = g_pd3dDevice->SetMaterial(&(pState->Material.Material));
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        //case STATE_TYPE_NPATCH_MODE:
        //case STATE_TYPE_PIXEL_SHADER:
        //case STATE_TYPE_PIXEL_SHADER_CONSTANT_B:
        //case STATE_TYPE_PIXEL_SHADER_CONSTANT_F:
        //case STATE_TYPE_PIXEL_SHADER_CONSTANT_I:
        case STATE_TYPE_RENDER_STATE:
            hRetCode = g_pd3dDevice->SetRenderState(
                pState->RenderState.State, pState->RenderState.Value
            );
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        case STATE_TYPE_SAMPLER_STATE:
            hRetCode = g_pd3dDevice->SetSamplerState(
                pState->SamplerState.Sampler, pState->SamplerState.Type, pState->SamplerState.Value
            );
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        //case STATE_TYPE_SCISSOR_RECT:
        //case STATE_TYPE_STREAM_SOURCE:
        //case STATE_TYPE_STREAM_SOURCE_FREQ:
        //case STATE_TYPE_TEXTURE:
        case STATE_TYPE_TEXTURE_STAGE_STATE:
            hRetCode = g_pd3dDevice->SetTextureStageState(
                pState->TextureStageState.Stage, pState->TextureStageState.Type, pState->TextureStageState.Value
            );
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        case STATE_TYPE_TRANSFORM:
            hRetCode = g_pd3dDevice->SetTransform(
                pState->Transform.State, &(pState->Transform.Matrix)
            );
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        case STATE_TYPE_VIEWPORT:
            hRetCode = g_pd3dDevice->SetViewport(&(pState->Viewport.Viewport));
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            break;

        //case STATE_TYPE_VERTEX_DECLARATION = 21,
        //case STATE_TYPE_VERTEX_SHADER = 22,
        //case STATE_TYPE_VERTEX_SHADER_CONSTANT_B = 23,
        //case STATE_TYPE_VERTEX_SHADER_CONSTANT_F = 24
        //case STATE_TYPE_VERTEX_SHADER_CONSTANT_I = 25,

        default:
            ASSERT(false && "Unknow type");
        }

        ++pState;
    }

    hRetCode = g_pd3dDevice->EndStateBlock(ppiRetD3DStateBlock);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    nBeginStateBlockFlag = false;

    hResult = S_OK;
Exit0:
    if (FAILED(hResult))
    {
        if (nBeginStateBlockFlag)
        {
            hRetCode = g_pd3dDevice->EndStateBlock(ppiRetD3DStateBlock);
            nBeginStateBlockFlag = false;
        } 
        KG_COM_RELEASE(*ppiRetD3DStateBlock);
    }
    return hResult;
}


////////////////////////////////////////////////////////////////////////////////
