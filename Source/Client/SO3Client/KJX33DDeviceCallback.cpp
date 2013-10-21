#include "stdafx.h"
#include "./KJX33DDeviceCallback.h"
#include "./KJX3Sdoa.h"
#include "./SO3Client.h"

int KJX33DDeviceCallback::Init(KJX3Sdoa* pJX3Sdoa)
{
    m_pJX3Sdoa = pJX3Sdoa;

    return true;
}

int KJX33DDeviceCallback::UnInit()
{
    m_pJX3Sdoa = NULL;

    return true;
}

bool KJX33DDeviceCallback::IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed)
{
    return true;
}

bool KJX33DDeviceCallback::ModifyDeviceSettings(KG3DDeviceSettings const* pSettings, D3DCAPS9 const* pCaps)
{
    return true;
}

HRESULT KJX33DDeviceCallback::OnCreateDevice(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC const* pBackBufferSurfaceDesc, D3DPRESENT_PARAMETERS const* pPresentParameters)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    D3DPRESENT_PARAMETERS PresentParameters;

    if (m_pJX3Sdoa)
    {
        if (m_pJX3Sdoa->pSDOADx9)
        {
            memcpy(&PresentParameters, pPresentParameters, sizeof(PresentParameters));

            hr = m_pJX3Sdoa->pSDOADx9->Initialize(pd3dDevice, &PresentParameters, FALSE);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KJX33DDeviceCallback::OnDestroyDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_pJX3Sdoa)
    {
        if (m_pJX3Sdoa->pSDOADx9)
        {
            hr = m_pJX3Sdoa->pSDOADx9->Finalize();
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KJX33DDeviceCallback::OnLostDevice()
{
    HRESULT hrResult = E_FAIL;

    if (m_pJX3Sdoa)
    {
        if (m_pJX3Sdoa->pSDOADx9)
        {
            m_pJX3Sdoa->pSDOADx9->OnDeviceLost();
        }
    }

    g_SO3Client.Enable(FALSE);

    hrResult = S_OK;
    return hrResult;
}
HRESULT KJX33DDeviceCallback::OnResetDevice(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC const* pBackBufferSurfaceDesc, 
                                            D3DPRESENT_PARAMETERS const* pPresentParameters)
{
    HRESULT hrResult = E_FAIL;
    D3DPRESENT_PARAMETERS PresentParameters;

    g_SO3Client.Enable(TRUE);

    if (m_pJX3Sdoa)
    {
        if (m_pJX3Sdoa->pSDOADx9)
        {
            memcpy(&PresentParameters, pPresentParameters, sizeof(PresentParameters));

            m_pJX3Sdoa->pSDOADx9->OnDeviceReset(&PresentParameters);
        }
    }

    hrResult = S_OK;
    return hrResult;
}
