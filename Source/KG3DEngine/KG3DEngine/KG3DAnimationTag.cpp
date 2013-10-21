#include "StdAfx.h"
#include "KG3DAnimationTag.h"
#include "KG3DSFX.h"
#include "KG3DAnimationController.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT FindKeyFrame(UINT uFrame, 
                     FindFrameType FindType, 
                     const std::vector<UINT>& KeyedFrame,
                     UINT& uFrameIndex)
{
    HRESULT hr = E_FAIL;
    uFrameIndex = 0;
    switch(FindType)
    {
    case FFT_GREATEREQUAL:
        {
            for (UINT i = 0; i < static_cast<UINT>(KeyedFrame.size()); i++)
            {
                if (KeyedFrame[i] >= uFrame)
                {
                    uFrameIndex = i;
                    hr = S_OK;
                    break;
                }
            }
        }
        break;
    case FFT_LESSEQUAL:
        {
            for (UINT i = 0;i < static_cast<UINT>(KeyedFrame.size()); i++)
            {
                if (KeyedFrame[i] > uFrame)
                {
                    break;
                }
                hr = S_OK;
                uFrameIndex = i;
            }
        }
        break;
    default:
        assert(0);
    }
    return hr;
}

HRESULT IKG3DAnimationTag::Apply(KG3DAnimationController* pController, D3DXVECTOR3 *pvPos)
{
	return Helper_Apply(pController, pController->m_dwLastFrame, pController->m_dwCurrentFrame, pvPos);
}