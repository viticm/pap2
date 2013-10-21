#include "stdafx.h"
#include "./wndtexture.h"
#include "../common/ksmallobjectpool.h"
#include "../KGUI.h"

namespace
{
	KSmallObjectPool	g_Allocator("KWndTexture", sizeof(UI::KWndTexture));
}

namespace UI
{

#ifdef KGUI_LUA_LOADER
int KWndTexture::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KWndWindow *pWnd = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pWnd = new(std::nothrow) KWndTexture;
    KGLOG_PROCESS_ERROR(pWnd);

    if (pLua)
    {
        nRetCode = pWnd->Init(pLua, nTableIndex);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    *ppObj = pWnd;
    // Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE(pWnd);
    }
    return nResult;
}
#endif


KWndTexture::KWndTexture()
{
}


KWndTexture::~KWndTexture()
{
}

void* KWndTexture::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KWndTexture::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KWndTexture::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KWndTexture::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

void KWndTexture::RenderCallback(DWORD dwTextureID, void* pUserData)
{
    HRESULT hr = E_FAIL;
	KWndTexture* pWndTexture = NULL;
    BOOL bBeginRenderToTexture = FALSE;
    
    pWndTexture = reinterpret_cast<KWndTexture*>(pUserData);
    KGLOG_PROCESS_ERROR(pWndTexture);

    hr = g_pUI->m_p3DUI->BeginRenderToTexture(dwTextureID, 0);
    KG_COM_PROCESS_ERROR(hr);

    bBeginRenderToTexture = TRUE;

    pWndTexture->OnPaint();

Exit0:
    if (hr == D3DERR_DRIVERINTERNALERROR || hr == D3DERR_INVALIDCALL)
    {
        KGLogPrintf(KGLOG_ERR, "KGUI KWndTexture::RenderCallback %u.\n", hr);

        PostMessage(g_pUI->m_hWnd, WM_QUIT, 0, 0);
    }
    if (bBeginRenderToTexture)
    {
        hr = g_pUI->m_p3DUI->EndRenderToTexture();
        KGLOG_COM_CHECK_ERROR(hr);
    }
}


bool KWndTexture::SetTexture(IKG3DModel* p3DModel)
{
	HRESULT hr = E_FAIL;

	ASSERT(p3DModel);

	hr = p3DModel->SetCallbackRender(&RenderCallback, this);
	KGLOG_COM_PROCESS_ERROR(hr);

	return true;
Exit0:
	return false;
}




void KWndTexture::OnPaint()
{
	if (IsVisible())
	{
		KWndWindow::OnPaint();
	}
}

int KWndTexture::CanCastTo(LPCSTR szType) const
{
	return szType == "WndTexture" || szType == "WndWindow";
}

LPCSTR KWndTexture::GetWndType() const
{ 
	return "WndTexture"; 
}

} // namespace UI
