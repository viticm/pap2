#ifndef KJX33DDEVICECALLBACK_H
#define KJX33DDEVICECALLBACK_H

#include "KG3DEngine/KG3DTypes.h"
#include "KG3DEngine/KG3DInterface.h"

struct KJX3Sdoa;

class KJX33DDeviceCallback : public IKG3DeviceCallback
{
public:
    int Init(KJX3Sdoa* pJX3Sdoa);
    int UnInit();

protected:
    virtual bool IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed);
    virtual bool ModifyDeviceSettings(KG3DDeviceSettings const* pSettings, D3DCAPS9 const* pCaps);
    virtual HRESULT OnCreateDevice(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC const* pBackBufferSurfaceDesc, D3DPRESENT_PARAMETERS const* pPresentParameters);
    virtual HRESULT OnDestroyDevice();
    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice(LPDIRECT3DDEVICE9 pd3dDevice, D3DSURFACE_DESC const* pBackBufferSurfaceDesc, D3DPRESENT_PARAMETERS const* pPresentParameters);

private:
    KJX3Sdoa* m_pJX3Sdoa;
};

#endif // KJX33DDEVICECALLBACK_H
