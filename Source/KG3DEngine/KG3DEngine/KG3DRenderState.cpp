#include "stdafx.h"
#include "kg3drenderstate.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
// KG3DRenderState负责管理渲染状态                                                  
/************************************************************************/
KG3DRenderState& g_GetRenderStateMgr()
{
	static KG3DRenderState g_RenderStateMgr;
	return g_RenderStateMgr;
}

KG3DRenderState::KG3DRenderState(void)
{
	m_vecStates.reserve(32);
}

KG3DRenderState::~KG3DRenderState(void)
{
	Restore();
	//m_vecStates.clear();
}
HRESULT KG3DRenderState::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	HRESULT hr = E_FAIL;
	DWORD dwValue;
	
	hr = g_pd3dDevice->GetRenderState( State, &dwValue ) ;
	_ASSERTE(SUCCEEDED(hr));///如果给定的State都是正常的话，是不应该Failed的
	KG_PROCESS_SUCCESS( Value == dwValue );///如果已经相等就不用添加了
	
	{
      /*  StateBlock *pStateBlock = m_vecStates.push_back();
        ASSERT(pStateBlock);
        pStateBlock->m_dwType = STATE_BLOCK_RENDERSTATE; 
        pStateBlock->m_dwRenderState = State;
        pStateBlock->m_dwValue = dwValue;*/

		StateBlock tempBlock;
		tempBlock.m_dwType = STATE_BLOCK_RENDERSTATE;
		tempBlock.m_dwRenderState = State;
		tempBlock.m_dwValue = dwValue;

		m_vecStates.push_back(tempBlock);	//注意，因为reserve了一段内存，所以这里没有分配内存，跟进去的结果验证了MSDN的说法。

        g_pd3dDevice->SetRenderState(State, Value);
    }
	
Exit1:
	return S_OK;
//Exit0:	
	return E_FAIL;
}

HRESULT KG3DRenderState::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	HRESULT hr = E_FAIL;
	DWORD dwValue;

	hr = g_pd3dDevice->GetTextureStageState( Stage, Type, &dwValue ) ;
	_ASSERTE(SUCCEEDED(hr));///如果给定的State都是正常的话，是不应该Failed的
	KG_PROCESS_SUCCESS( Value == dwValue );///如果已经相等就不用添加了

	
	{
       /* StateBlock *pStateBlock = m_vecStates.push_back();
        ASSERT(pStateBlock);
        pStateBlock->m_dwType = STATE_BLOCK_TEXTURESTATE; 
        pStateBlock->m_dwTextureStageIndex = Stage;
        pStateBlock->m_dwTextureState = Type;
        pStateBlock->m_dwValue = dwValue;*/

		StateBlock tempBlock;
		tempBlock.m_dwType = STATE_BLOCK_TEXTURESTATE;
		tempBlock.m_dwTextureStageIndex = Stage;
		tempBlock.m_dwTextureState = Type;
		tempBlock.m_dwValue = dwValue;

		m_vecStates.push_back(tempBlock);

        g_pd3dDevice->SetTextureStageState(Stage, Type, Value);
	}

Exit1:
	return S_OK;
//Exit0:	
	return E_FAIL;
}

HRESULT KG3DRenderState::SetSamplerState(DWORD Sampler,
                                         D3DSAMPLERSTATETYPE Type,
                                         DWORD Value)
{
    HRESULT hr = E_FAIL;
    DWORD dwValue;

    hr = g_pd3dDevice->GetSamplerState(Sampler, Type, &dwValue) ;
    _ASSERTE(SUCCEEDED(hr));///如果给定的State都是正常的话，是不应该Failed的
    KG_PROCESS_SUCCESS(Value == dwValue);///如果已经相等就不用添加了


    {
        /*StateBlock *pStateBlock = m_vecStates.push_back();
        ASSERT(pStateBlock);
        pStateBlock->m_dwType = STATE_BLOCK_SAMPLERSTATE; 
        pStateBlock->dwSampler = Sampler;
        pStateBlock->dwType = Type;
        pStateBlock->m_dwValue = dwValue;*/

		StateBlock tempBlock;
		tempBlock.m_dwType = STATE_BLOCK_SAMPLERSTATE;
		tempBlock.dwSampler = Sampler;
		tempBlock.dwType = Type;
        tempBlock.m_dwValue = dwValue;

		m_vecStates.push_back(tempBlock);

        g_pd3dDevice->SetSamplerState(Sampler, Type, Value);
    }
Exit1:
    return S_OK;
    //Exit0:	
    return E_FAIL;
}	

void KG3DRenderState::Restore()
{
	HRESULT hr = E_FAIL;
    
    size_t uSize = m_vecStates.size();
    if (uSize == 0)
		return;
	//用back之前记得判空。这里一定要从后向前还原状态，不然重复设的状态就错了
    for (StateBlock *pCurrent = &m_vecStates.back(); uSize > 0; --pCurrent, --uSize)
    {
        switch (pCurrent->m_dwType)
        {
        case STATE_BLOCK_RENDERSTATE:
            hr = g_pd3dDevice->SetRenderState(
                pCurrent->m_dwRenderState, 
                pCurrent->m_dwValue
            );
            break;

        case STATE_BLOCK_TEXTURESTATE:
            hr = g_pd3dDevice->SetTextureStageState(
                pCurrent->m_dwTextureStageIndex,
                pCurrent->m_dwTextureState,
                pCurrent->m_dwValue
            );
            break;

        case STATE_BLOCK_SAMPLERSTATE:
                hr = g_pd3dDevice->SetSamplerState(
                    pCurrent->dwSampler,
                    pCurrent->dwType,
                    pCurrent->m_dwValue
                );
                break;
        default:
            #ifdef _DEBUG
                KGLogPrintf(KGLOG_ERR, "恢复未知渲染状态.");
            #endif
            continue;
        }
        #ifdef _DEBUG
            if(FAILED(hr))
            {
                KGLogPrintf(KGLOG_ERR, "恢复渲染状态失败.");
            }
        #endif
    }
	m_vecStates.clear();
	m_vecStates.reserve(32);
}

