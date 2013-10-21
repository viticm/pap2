#include "stdafx.h"
#include "./wndwebpage.h"
#include "./wndmessage.h"
#include "../common/ksmallobjectpool.h"
#include "../script/kwndclassscripttable.h"
#include <Mshtml.h>
#include <ExDispid.h>

namespace
{
	KSmallObjectPool g_Allocator("KWndWebPage", sizeof(UI::KWndWebPage));
}

namespace UI
{
	LRESULT CALLBACK EmbedWebPageProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
        int nRetCode = false;
        LRESULT lResult = 0;
    	KWndWebPage* pWindow = NULL;

        KG_PROCESS_SUCCESS(uMsg == WM_ERASEBKGND);

    	pWindow = (KWndWebPage*)(LONG_PTR)GetWindowLongPtrA(hWnd, GWL_USERDATA);
        if (pWindow)
            lResult = pWindow->WndProc(uMsg, wParam, lParam);

        if (lResult == 0)
            lResult = DefWindowProc(hWnd, uMsg, wParam, lParam);

        goto Exit0;

Exit1:
        lResult = 1;
Exit0:
        return lResult;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------
	HRESULT STDMETHODCALLTYPE KWndWebStorage::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	ULONG STDMETHODCALLTYPE KWndWebStorage::AddRef()
	{
		return 1;
	}

	ULONG STDMETHODCALLTYPE KWndWebStorage::Release()
	{
		return 1;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::CreateStream(const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::OpenStream(const OLECHAR *pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::CreateStorage(const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::OpenStorage(const OLECHAR *pwcsName, IStorage *pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::CopyTo(DWORD ciidExclude, const IID *rgiidExclude, SNB snbExclude, IStorage *pstgDest)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::MoveElementTo(const OLECHAR *pwcsName, IStorage *pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::Commit(DWORD grfCommitFlags)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::Revert()
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::EnumElements(DWORD reserved1, void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::DestroyElement(const OLECHAR *pwcsName)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::RenameElement(const OLECHAR *pwcsOldName, const OLECHAR *pwcsNewName)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::SetElementTimes(const OLECHAR *pwcsName, const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::SetClass(REFCLSID clsid)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::SetStateBits(DWORD grfStateBits, DWORD grfMask)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebStorage::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}


	//--------------------------------------------------------------------------------------------------------------------------------------
	KWndWebOleClientSite::KWndWebOleClientSite()
		: m_hWnd(NULL)
	{
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (IsEqualIID(riid, IID_IUnknown)|| IsEqualIID(riid, IID_IOleClientSite))
		{
			*ppvObject = (IOleClientSite*)this;
			return S_OK;
		}
		if (IsEqualIID(riid, IID_IOleInPlaceSite))
		{
			*ppvObject = (IOleInPlaceSite*)this;
			return S_OK;
		}
		if (IsEqualIID(riid, IID_IOleInPlaceFrame))
		{
			*ppvObject = (IOleInPlaceFrame*)this;
			return S_OK;
		}
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE KWndWebOleClientSite::AddRef()
	{
		return 1;
	}

	ULONG STDMETHODCALLTYPE KWndWebOleClientSite::Release()
	{
		return 1;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::GetWindow(HWND *phwnd)
	{
		ASSERT(phwnd);

		*phwnd = m_hWnd;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::ContextSensitiveHelp(BOOL fEnterMode)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::GetBorder(LPRECT lprectBorder)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::RemoveMenus(HMENU hmenuShared)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::SetStatusText(LPCOLESTR pszStatusText)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::EnableModeless(BOOL fEnable)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::TranslateAccelerator(LPMSG lpmsg, WORD wID)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}


	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::SaveObject()
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::GetContainer(IOleContainer **ppContainer)
	{
		// Tell the browser that we are a simple object and don't support a container
		*ppContainer = NULL;

		return E_NOINTERFACE;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::ShowObject()
	{
		return NOERROR;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::OnShowWindow(BOOL fShow)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::RequestNewObjectLayout()
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::CanInPlaceActivate()
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::OnInPlaceActivate()
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::OnUIActivate()
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
	{
		*ppFrame = this;
		*ppDoc = NULL;

		lpFrameInfo->fMDIApp = FALSE;
		lpFrameInfo->hwndFrame = m_hWnd;
		lpFrameInfo->haccel = 0;
		lpFrameInfo->cAccelEntries = 0;

		*lprcPosRect = m_rect;
		*lprcClipRect = m_rect;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::Scroll(SIZE scrollExtant)
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::OnUIDeactivate(BOOL fUndoable)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::OnInPlaceDeactivate()
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::DiscardUndoState()
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::DeactivateAndUndo()
	{
		ASSERT(0);
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleClientSite::OnPosRectChange(LPCRECT lprcPosRect)
	{
		return S_OK;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------
	KWndWebOleEventHandler::KWndWebOleEventHandler()
	{

	}

	KWndWebOleEventHandler::~KWndWebOleEventHandler()
	{

	}

	HRESULT STDMETHODCALLTYPE KWndWebOleEventHandler::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		*ppvObject = NULL;

		if (riid == IID_IUnknown || riid == IID_IDispatch)
		{
			*ppvObject = this;
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE KWndWebOleEventHandler::AddRef()
	{
		return 1;
	}

	ULONG STDMETHODCALLTYPE KWndWebOleEventHandler::Release()
	{
		return 1;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleEventHandler::GetTypeInfoCount(UINT *pctinfo)
	{
		if (pctinfo)
			*pctinfo = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleEventHandler::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		if (ppTInfo)
			*ppTInfo = 0;
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleEventHandler::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE KWndWebOleEventHandler::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		if (!pDispParams)
			return E_INVALIDARG;

		switch (dispIdMember)
		{
		case DISPID_COMMANDSTATECHANGE:
			if (m_pWndWebPage)
			{
				if (pDispParams->rgvarg[1].lVal == CSC_NAVIGATEFORWARD)
				{
					m_pWndWebPage->SetCanGoForward(pDispParams->rgvarg[0].boolVal == VARIANT_TRUE);
				}
				else if (pDispParams->rgvarg[1].lVal == CSC_NAVIGATEBACK)
				{
					m_pWndWebPage->SetCanGoBack(pDispParams->rgvarg[0].boolVal == VARIANT_TRUE);
				}
			}
			break;
		case DISPID_STATUSTEXTCHANGE:
		case DISPID_NAVIGATECOMPLETE:
		case  DISPID_TITLECHANGE:
			if (m_pWndWebPage)
				m_pWndWebPage->OnTitleChanged();
			break;
		case DISPID_NEWWINDOW2:
			{
				g_pUI->FireEvent("IE_NEW_WINDOW", 0);
				Lua_State* pLuaState = g_pUI->m_Script.GetLuaState();
				if (!pLuaState)
					return DISP_E_MEMBERNOTFOUND;
				lua_getfield(pLuaState, LUA_GLOBALSINDEX, "arg0");
				if (!Lua_IsTable(pLuaState, -1))
					return DISP_E_MEMBERNOTFOUND;
				KWndWebPage* pWebPage = (KWndWebPage*)KWndClassScriptTable::ToWndUserData("WndWebPage", pLuaState, -1);
				if (!pWebPage)
					return DISP_E_MEMBERNOTFOUND;
				IWebBrowser2* pWebBrowser2 = pWebPage->GetWebBrowser();
				if (!pWebBrowser2)
					return DISP_E_MEMBERNOTFOUND;
				*(pDispParams->rgvarg[1].ppdispVal) = (IDispatch*)pWebBrowser2;
				return S_OK;
			}
			break;
		case DISPID_NAVIGATECOMPLETE2:
			if (m_pWndWebPage)
			{
				IWebBrowser2* pWebBrowser2 = m_pWndWebPage->GetWebBrowser();
				if (!pWebBrowser2)
					break;
				IDispatch* pDispatch = NULL;
				HRESULT hr = pWebBrowser2->get_Document(&pDispatch);
				if (FAILED(hr) || !pDispatch)
					break;
				IHTMLDocument2* pDocument = NULL;
				hr = pDispatch->QueryInterface(IID_IHTMLDocument2, (void **)&pDocument);
				if (FAILED(hr) || !pDocument)
				{
					pDispatch->Release();
					break;
				}
				IHTMLWindow2* pHTMLWindow2 = NULL;
				hr = pDocument->get_parentWindow(&pHTMLWindow2);   
				if (FAILED(hr) || !pHTMLWindow2)
				{
					pDocument->Release();
					pDispatch->Release();
					break;
				}
				CComBSTR bs1(L"function myalert(u1){}; function myerror() {return true}; window.alert = myalert; window.onerror = myerror;");   
				CComBSTR bs2(L"JScript");
				VARIANT v;
				VariantInit(&v);

				pHTMLWindow2->execScript(bs1, bs2, &v);
				VariantClear(&v);

				pHTMLWindow2->Release();
				pDocument->Release();
				pDispatch->Release();
				return S_OK;
			}
			break;
		default:
			break;
		}
		return  DISP_E_MEMBERNOTFOUND;
	}

	void KWndWebOleEventHandler::SetWndWebPage(KWndWebPage * pWndWebPage)
	{
		m_pWndWebPage = pWndWebPage;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------
	KWndWebPage::KWndWebPage()
		: m_pBrowser(NULL)
		, m_pWebBrowser2(NULL)
		, m_hWnd(NULL)
		, m_bCanGoBack(FALSE)
		, m_bCanGoForward(FALSE)
		, m_dwCookie(0)
		, m_pIConnectionPoint(NULL)
		, m_pIConnectionPointContainer(NULL)
	{
		int nRetCode = false;

		m_szLocationName[0] = L'\0';
		m_szLocationURL[0] = L'\0';
		m_Handler.SetWndWebPage(this);

		memset(&m_Image, 0, sizeof(m_Image));

		m_Image.v2UVMin = D3DXVECTOR2(0.0f, 0.0f);
		m_Image.v2UVMax = D3DXVECTOR2(1.0f, 1.0f);
		m_Image.crDiffuse = m_dwAlpha << 24;

		m_hWnd = CreateWindowEx(0, "SOIII_EmbedWebPage", NULL, WS_POPUP, 0, 0, 0, 0, g_pUI->m_hWnd, NULL, g_pUI->m_hInst, NULL);
		KGLOG_PROCESS_ERROR(m_hWnd);

    	::SetWindowLongPtrA(m_hWnd, GWL_USERDATA, (__int3264)(LONG_PTR)this);

		nRetCode = Embed(m_hWnd);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = Navigate(L"about:blank");
		KGLOG_PROCESS_ERROR(nRetCode);

		return;
Exit0:
		Unembed();

		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
		}
	}


	KWndWebPage::~KWndWebPage()
	{
		Unembed();

		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = NULL;
		}
	}

	void* KWndWebPage::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
	{ 
		return g_Allocator.Alloc(); 
	}

	void KWndWebPage::operator delete(void* pData, std::nothrow_t const&) throw()
	{ 
		g_Allocator.Free(pData); 
	}

	void* KWndWebPage::operator new(size_t /* nSize */)
	{ 
		return g_Allocator.Alloc(); 
	}

	void KWndWebPage::operator delete(void* pData) throw()
	{ 
		g_Allocator.Free(pData); 
	}

	BOOL KWndWebPage::Init()
	{
		int nRetCode = false;
        int nResult = false;
		WNDCLASSEX wc;

		wc.lpszClassName = "SOIII_EmbedWebPage";
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = EmbedWebPageProc;
		wc.hInstance = g_pUI->m_hInst;
		wc.hIcon = LoadIcon(g_pUI->m_hInst, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(g_pUI->m_hInst, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;

		nRetCode = RegisterClassEx(&wc);
		KGLOG_PROCESS_ERROR(nRetCode);

		nResult = true;
Exit0:
        if (!nResult)
        {
		    Exit();
        }
		return nResult;
	}

	void KWndWebPage::Exit()
	{
		UnregisterClass("SOIII_EmbedWebPage", g_pUI->m_hInst);
	}

	int KWndWebPage::Embed(HWND hwnd)
	{
		int nRetCode = false;
		int nResult = false;
		HRESULT hr = E_FAIL;
		RECT rect;

		nRetCode = GetClientRect(hwnd, &rect);
		KGLOG_PROCESS_ERROR(nRetCode);

		m_OleClientSite.m_hWnd = hwnd;
		m_OleClientSite.m_rect = rect;

		hr = OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, 0, &m_OleClientSite, &m_Storage, (void**)&m_pBrowser);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = OleSetContainedObject((LPUNKNOWN)m_pBrowser, TRUE);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_pBrowser->SetHostNames(L"JX3", 0);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_pBrowser->SetClientSite(&m_OleClientSite);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_pBrowser->DoVerb(OLEIVERB_SHOW, NULL, &m_OleClientSite, -1, hwnd, NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_pBrowser->QueryInterface(IID_IWebBrowser2, (void**)&m_pWebBrowser2);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_pWebBrowser2->QueryInterface(IID_IConnectionPointContainer, (void **)&m_pIConnectionPointContainer);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_pIConnectionPointContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pIConnectionPoint);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_pIConnectionPoint->Advise(&m_Handler, &m_dwCookie);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_pWebBrowser2->put_Resizable(-1);
		KGLOG_COM_PROCESS_ERROR(hr);

		ResizeWebPage(rect.right - rect.left, rect.bottom - rect.top);
		UpdateViewport();

		nResult = true;
Exit0:
		if (!nResult)
		{
			Unembed();
		}

		return nResult;
	}


	void KWndWebPage::Unembed()
	{
		if (m_Image.dwImageID != 0)
		{
			g_pUI->m_p3DUI->UnloadImage(m_Image.dwImageID);
			m_Image.dwImageID = 0;
		}

		if (m_pIConnectionPoint)
		{
			m_pIConnectionPoint->Unadvise(m_dwCookie);
			m_dwCookie = 0;
		}
		SAFE_RELEASE(m_pIConnectionPoint);
		SAFE_RELEASE(m_pIConnectionPointContainer);

		if (m_pBrowser)
			m_pBrowser->Close(OLECLOSE_NOSAVE);

//		SAFE_RELEASE(m_pWebBrowser2);
		SAFE_RELEASE(m_pBrowser);
	}


	int KWndWebPage::Navigate(LPWSTR szURL)
	{
		int nResult = false;
		HRESULT hr = E_FAIL;
		VARIANT vURL;

		VariantInit(&vURL);

		KGLOG_PROCESS_ERROR(m_pWebBrowser2);

		vURL.vt = VT_BSTR;
		vURL.bstrVal = SysAllocString(szURL);
		KGLOG_PROCESS_ERROR(vURL.bstrVal);

		hr = m_pWebBrowser2->Navigate2(&vURL, 0, 0, 0, 0);
		KGLOG_COM_PROCESS_ERROR(hr);

		nResult = true;
Exit0:
		VariantClear(&vURL);
		return nResult;
	}

	IWebBrowser2* KWndWebPage::GetWebBrowser()
	{
		return m_pWebBrowser2;
	}

	void KWndWebPage::UpdateTexture()
	{
		if (m_pBrowser && m_Image.dwImageID != 0)
		{
			g_pUI->m_p3DUI->DrawOleToImage(m_pBrowser, m_Image.dwImageID);
		}
	}

	void KWndWebPage::UpdateViewport()
	{
		int nRetCode = false;

		LONG lX = (LONG)(m_fAbsX + 0.5f);
		LONG lY = (LONG)(m_fAbsY + 0.5f);
		LONG lWidth = (LONG)(m_fWidth + 0.5f);
		LONG lHeight = (LONG)(m_fHeight + 0.5f);

		m_Image.v2Min = D3DXVECTOR2((float)lX - 0.5f, (float)lY - 0.5f);
		m_Image.v2Max = D3DXVECTOR2((float)(lX + lWidth) - 0.5f, (float)(lY + lHeight) - 0.5f);

		POINT pt = { lX, lY };

		nRetCode = ClientToScreen(g_pUI->m_hWnd, &pt);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = MoveWindow(m_hWnd, (int)pt.x, (int)pt.y, (int)lWidth, (int)lHeight, FALSE);
		KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
		return;
	}

	void KWndWebPage::ResizeWebPage(long lWidth, long lHeight)
	{
		int nRetCode = false;
		HRESULT hr = E_FAIL;
		IOleWindow* pOleWindow = NULL;
		HWND hwndOle = NULL; 

		KG_PROCESS_SUCCESS(m_pBrowser == NULL || m_pWebBrowser2 == NULL);

		m_OleClientSite.m_rect.right = lWidth;
		m_OleClientSite.m_rect.bottom = lHeight;

		hr = m_pBrowser->QueryInterface(IID_IOleInPlaceObject, (void**)&pOleWindow);
		KGLOG_COM_PROCESS_ERROR(hr);
		KGLOG_PROCESS_ERROR(pOleWindow);

		hr = pOleWindow->GetWindow(&hwndOle);
		KGLOG_COM_PROCESS_ERROR(hr);

		nRetCode = MoveWindow(hwndOle, 0, 0, lWidth, lHeight, FALSE);
		KGLOG_PROCESS_ERROR(nRetCode);

		hr = m_pWebBrowser2->put_Width(lWidth);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_pWebBrowser2->put_Height(lHeight);
		KGLOG_COM_PROCESS_ERROR(hr);

		if (m_Image.dwImageID != 0)
		{
			g_pUI->m_p3DUI->UnloadImage(m_Image.dwImageID);
			m_Image.dwImageID = 0;
		}

		if (m_Image.dwImageID != 0)
		{
			g_pUI->m_p3DUI->UnloadImage(m_Image.dwImageID);
			m_Image.dwImageID = 0;
		}

		g_pUI->m_p3DUI->CreateImage(lWidth, lHeight, &m_Image.dwImageID, KG3DUI_IMAGE_FLAGS_HDC);

Exit0:
		SAFE_RELEASE(pOleWindow);
Exit1:
		return;
	}

	LRESULT KWndWebPage::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
        HRESULT hr = E_FAIL;
		LRESULT	lResult = 0;

		if (KWndWindow::WndProc(uMsg, wParam, lParam))
			return 1;

		switch (uMsg)
		{
		case WND_M_SET_FOCUS:
			lResult = OnSetFocus();
			break;
		case WND_M_KILL_FOCUS:
			lResult = OnKillFocus();
			break;
		case WM_MOVE:
			UpdateViewport();
			break;
        case WM_KEYDOWN:
            if (m_pWebBrowser2)
            {
                CComQIPtr<IOleInPlaceActiveObject, &IID_IOleInPlaceActiveObject> pIOIPAO(m_pWebBrowser2);
                if (pIOIPAO)
                {
                    MSG msg;

                    msg.message = uMsg;
                    msg.wParam = wParam;
                    msg.lParam = lParam;

                    hr = pIOIPAO->TranslateAccelerator(&msg);
                    KGLOG_COM_PROCESS_ERROR(hr);
                }
            }
            break;
        default:
			break;
		}

Exit0:
		return lResult;
	}

	void KWndWebPage::OnBreathe()
	{
		if (m_pWebBrowser2 && !IsWindowVisible(m_hWnd))
        {
			UpdateTexture();
        }

        KWndWindow::OnBreathe();
	}

	void KWndWebPage::OnDestroy()
	{
		m_Handler.SetWndWebPage(NULL);
		KWndWindow::OnDestroy();
	}

	void KWndWebPage::OnPaint()
	{
		if (!IsWindowVisible(m_hWnd))
		{
			g_pUI->m_p3DUI->DrawMultiImage(&m_Image, 1);

			KWndWindow::OnPaint();
		}
	}

	void KWndWebPage::OnSize()
	{
		KWndWindow::OnSize();

		ResizeWebPage((int)(m_fWidth + 0.5f), (int)(m_fHeight + 0.5f));
		UpdateViewport();
	}

	void KWndWebPage::OnScale(float fScaleX, float fScaleY)
	{
		KWndWindow::OnScale(fScaleX, fScaleY);

		ResizeWebPage((int)(m_fWidth + 0.5f), (int)(m_fHeight + 0.5f));
		UpdateViewport();
	}


	void KWndWebPage::OnMove()
	{
		KWndWindow::OnMove();

		UpdateViewport();
	}

	int KWndWebPage::CanCastTo(LPCSTR szType) const
	{
		return szType == "WndWebPage" || szType == "WndWindow";
	}

	LPCSTR KWndWebPage::GetWndType() const
	{ 
		return "WndWebPage"; 
	}

	LRESULT KWndWebPage::OnSetFocus()
	{
        int nRetCode = false;
        HRESULT hr = E_FAIL;
        LRESULT lResult = 0;
        CComPtr<IDispatch> pDispatch;
        CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> pHTMLDocument2;
        CComPtr<IHTMLElement> pHTMLElement;
        RECT rcClient;

        ShowWindow(m_hWnd, SW_SHOW);

        if (g_pUI->m_phwndEmbedWebPage)
            *g_pUI->m_phwndEmbedWebPage = m_hWnd;

        if (m_pWebBrowser2 && m_pBrowser)
        {
            hr = m_pWebBrowser2->get_Document(&pDispatch);
            KGLOG_COM_PROCESS_ERROR(hr);

            pHTMLDocument2 = pDispatch;
            if (pHTMLDocument2)
            {
                if (SUCCEEDED(pHTMLDocument2->get_activeElement(&pHTMLElement)) && pHTMLElement == NULL)
                {
                    CComQIPtr<IHTMLWindow2> pHTMLWindow2;
                    if (SUCCEEDED(pHTMLDocument2->get_parentWindow(&pHTMLWindow2)) && pHTMLWindow2)
                    {
                        hr = pHTMLWindow2->focus();
                        KGLOG_COM_PROCESS_ERROR(hr);

                        goto Exit1;
                    }
                }
            }

            nRetCode = GetClientRect(m_hWnd, &rcClient);
            KGLOG_PROCESS_ERROR(nRetCode);

            hr = m_pBrowser->DoVerb(OLEIVERB_UIACTIVATE, NULL, &m_OleClientSite, 0, m_hWnd, &rcClient);
            KGLOG_COM_PROCESS_ERROR(hr);
        }

Exit1:
        lResult = TRUE;
Exit0:
        return lResult;
	}

	LRESULT KWndWebPage::OnKillFocus()
	{
		UpdateTexture();

        if (g_pUI->m_phwndEmbedWebPage)
            *g_pUI->m_phwndEmbedWebPage = NULL;

		ShowWindow(m_hWnd, SW_HIDE);
		return TRUE;
	}

	void KWndWebPage::GoBack()
	{
		if (m_pWebBrowser2)
			m_pWebBrowser2->GoBack();
	}

	void KWndWebPage::GoForward()
	{
		if (m_pWebBrowser2)
			m_pWebBrowser2->GoForward();
	}

	void KWndWebPage::SetCanGoBack(BOOL bCan)
	{
		m_bCanGoBack = bCan;
		g_pUI->m_Script.FireWndEvent(this, "OnHistoryChanged");
	}

	void KWndWebPage::SetCanGoForward(BOOL bCan)
	{
		m_bCanGoForward = bCan;
		g_pUI->m_Script.FireWndEvent(this, "OnHistoryChanged");
	}

	BOOL KWndWebPage::CanGoBack()
	{
		return m_bCanGoBack;
	}

	BOOL KWndWebPage::CanGoForward()
	{
		return m_bCanGoForward;
	}

	void KWndWebPage::Refresh()
	{
		if (m_pWebBrowser2)
			m_pWebBrowser2->Refresh();
	}

	LPCWSTR KWndWebPage::GetLocationName()
	{
		LPWSTR pcwszLocationName = NULL;
		if (m_pWebBrowser2)
			m_pWebBrowser2->get_LocationName(&pcwszLocationName);
		wcsncpy(m_szLocationName, pcwszLocationName, MAX_PATH - 1);
		SysFreeString(pcwszLocationName);
		return m_szLocationName;
	}

	LPCWSTR KWndWebPage::GetLocationURL()
	{
		LPWSTR pcwszLocationURL = NULL;
		if (m_pWebBrowser2)
			m_pWebBrowser2->get_LocationURL(&pcwszLocationURL);
		wcsncpy(m_szLocationURL, pcwszLocationURL, MAX_PATH - 1);
		SysFreeString(pcwszLocationURL);
		return m_szLocationURL;
	}

	void KWndWebPage::OnTitleChanged()
	{
		g_pUI->m_Script.FireWndEvent(this, "OnTitleChanged");
	}


} // namespace UI
