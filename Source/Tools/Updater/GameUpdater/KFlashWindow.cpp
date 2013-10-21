#include "StdAfx.h" 
#include "KFlashWindow.h" 
#include "UpdateDefine.h" 
#include "tinyxml.h"
#include <string> 
#include "resource.h" 
#include "KGPublic.h"
#include <io.h>
#include <errno.h>
#include <utility>

#pragma warning(disable : 4533)
////////////////////////////////////////////////////////////////////////////////
int KG_CheckFlashAvailable()
{
    int nResult  = false;
    int nRetCode = false;
    HRESULT hRetCode = E_FAIL;
    IClassFactory *piClassFactory = NULL;
    ShockwaveFlashObjects::IShockwaveFlash* piFlash = NULL;
    long lCurrentVersion = 0;

    hRetCode = CoGetClassObject(
        ShockwaveFlashObjects::CLSID_ShockwaveFlash, CLSCTX_ALL, NULL,
        IID_IClassFactory, (void **)&piClassFactory
    );
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piClassFactory->CreateInstance(
        NULL, __uuidof(ShockwaveFlashObjects::IShockwaveFlash), (void **)&piFlash
    );
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    lCurrentVersion = piFlash->FlashVersion();
    nRetCode = (lCurrentVersion & 0x00FF0000) >= 0x00090000;
    KGLOG_PROCESS_ERROR(nRetCode);
    
    nResult = true;
Exit0:
    KG_COM_RELEASE(piFlash);
    KG_COM_RELEASE(piClassFactory);
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
#define WM_FLASHCALL  WM_USER + 3389
#define WM_FLASHCLOSE WM_USER + 3390

class KOleInitialize
{
public:
	KOleInitialize() {OleInitialize(NULL);}
	~KOleInitialize(){OleUninitialize();}
}g_OleInit;
 
KFlashWindow::KFlashWindow() 
{ 	
	m_lpControl      = NULL; 
	m_lpO            = NULL; 
	m_lpViewObjectEx = NULL; 
	m_lpViewObject   = NULL; 
	m_lpInPlaceObj   = NULL; 
	
	m_lpConCont      = NULL; 
	m_lpConPoint     = NULL; 
 
	m_hdcBack        = NULL; 
	m_bmpBack        = NULL; 
	m_lpBitsOnly     = NULL; 
	m_nBPP			 = 0;
	
	m_dwConPointID   = 0; 
	
	m_nFlashWndMoveFlag = false; 
	m_iRef           = 0; 
	m_lStartTime     = GetTickCount();
	m_lTotalFrame    = 0;
	m_lFlashVersion  = 0; 
	m_pFlashCallBack = NULL;
	m_nErrorCode     = fwerr_failed;
	m_hWnd           = NULL;
	m_bmp16BPPBack   = NULL;
	m_lp16BitsBmp    = NULL;
	m_crTransparencyColour = RGB(0, 0, 0);
	
	m_nWndWidth  = 0;
	m_nWndHeight = 0;

	memset(&m_rctLastLocation, 0, sizeof(RECT));


	m_lpInPlaceObjWindowless = NULL; 

	InitializeCriticalSection(&m_CriticalSection);
}  
 
KFlashWindow::~KFlashWindow()
{ 
	if (m_lpControl) 
	{ 
		if (m_lpConPoint) 
		{ 
			if (m_dwConPointID) 
				m_lpConPoint->Unadvise(m_dwConPointID); 
			m_lpConPoint->Release(); 
		} 
		if (m_lpConCont) 
			m_lpConCont->Release(); 
		m_lpO->Close(OLECLOSE_NOSAVE); 
		if (m_lpViewObjectEx) 
			m_lpViewObjectEx->Release(); 
		if (m_lpViewObject) 
			m_lpViewObject->Release(); 
		if (m_lpInPlaceObjWindowless) 
			m_lpInPlaceObjWindowless->Release(); 
		if (m_lpInPlaceObj) 
			m_lpInPlaceObj->Release(); 
		if (m_lpO) 
			m_lpO->Release(); 
		m_lpControl->Release(); 
	} 
	if (m_hdcBack) 
	{
		::DeleteDC(m_hdcBack); 
		m_hdcBack = NULL;
	}
	if (m_bmpBack) 
	{
		::DeleteObject(m_bmpBack); 
		m_bmpBack = NULL;
	}

	if (m_lp16BitsBmp)
	{
		delete[] m_lp16BitsBmp;
		m_lp16BitsBmp = NULL;
	}

	DeleteCriticalSection(&m_CriticalSection); 
} 

int KFlashWindow::Create(
		   const tstring &strFlashFile,
		   IFlashCallBack *pFlashCallback,
		   const tstring &strClassName, 
		   const tstring &strWindowName, 
		   int x, int y, int nWidth, int nHeight,
		   HINSTANCE hInstance, DWORD hIcon,
		   HBITMAP h16BPPBackImage,
		   COLORREF crTransColor
)
{
	int nRetCode = false;
	int nResult  = false;

	m_pFlashCallBack = pFlashCallback;
	m_hInstance = hInstance;
	m_bmp16BPPBack = h16BPPBackImage;
	m_crTransparencyColour = crTransColor;
	
	GetBPP();

	nRetCode = _CreateFlashWindow(strClassName, strWindowName, x, y, nWidth, nHeight, m_hInstance, hIcon);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _CreateFlashOLE();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _LoadFlash(strFlashFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	m_nErrorCode = fwerr_ok;

	nResult = true;
Exit0:
	KGLogPrintf(KGLOG_INFO, "create window. err = %d", m_nErrorCode);

	return nResult; 
}

int KFlashWindow::Destroy()
{
	if (m_hWnd)
		PostMessage(m_hWnd, WM_FLASHCLOSE, NULL, NULL);
	KGLogPrintf(KGLOG_INFO, "Destroy window. err = %d", m_nErrorCode);
	return true;
}

int KFlashWindow::ShowWnd()
{
	int nExitFlag = false; 
	MSG msg;

	while (!nExitFlag) 
	{
		while (PeekMessage(&msg, NULL, 0, 0, true)) 
		{	 
			if (msg.message == WM_FLASHCLOSE) 
			{ 
				KGLogPrintf(KGLOG_INFO, "Windows close!");
				nExitFlag = true; 
				break; 
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg); 			
		}
		Sleep(10);
	} 

	return true;
}

int KFlashWindow::SendCommandToFlash(const tstring &strCommand, const PARAM_LIST &Params)
{
	int nRetCode = false;
	int nResult  = false;
	static unsigned suIndex = 0;
	tstring strXmlFlashCommand;	

	EnterCriticalSection(&m_CriticalSection);

	nRetCode = _MakeXmlFlashCommand(strCommand, Params, &strXmlFlashCommand);
	KG_PROCESS_ERROR(nRetCode);

	if (suIndex == 0xffffffff)
		suIndex = 0;
	
	m_CallFlashCommands.insert(std::make_pair(++suIndex, strXmlFlashCommand));	

	::PostMessage(m_hWnd, WM_FLASHCALL, (WPARAM)suIndex, NULL);

	nResult = true;
Exit0:

	LeaveCriticalSection(&m_CriticalSection);
	return nResult;
}

int KFlashWindow::GetBPP()
{
	HDC hdc = NULL;

	if (m_nBPP == 0)
	{
		hdc = ::GetDC(NULL);
		KG_PROCESS_ERROR(hdc);
		m_nBPP = ::GetDeviceCaps(hdc, BITSPIXEL);
	}
Exit0:
	if (hdc)
	{
		::ReleaseDC(NULL, hdc);
		hdc = NULL;
	}
	return m_nBPP;
}

int KFlashWindow::_CreateFlashWindow(const tstring &strClassName, const tstring &strWindowName, int x, int y, int nWidth, int nHeight, HINSTANCE hInstance, DWORD hIcon)
{
	int nRetCode = false;
	int nResult = false;
	WNDCLASSEX wcex = {0}; 

	m_nWndWidth  = nWidth;
	m_nWndHeight = nHeight;

	wcex.cbSize         = sizeof(WNDCLASSEX); 
	wcex.lpfnWndProc	= &KFlashWindow::WndProcStatic;; 
	wcex.hInstance		= hInstance; 
	wcex.lpszClassName	= strClassName.c_str();
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(hIcon));
	RegisterClassEx(&wcex); 

	m_hWnd = CreateWindowEx( 
		WS_EX_LAYERED, 
		strClassName.c_str(),  
		strWindowName.c_str(), 
		WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_MINIMIZEBOX, 
		x, y, nWidth, nHeight,
		NULL, 
		NULL, 
		m_hInstance,  
		(void *)this 
	); 
	KG_PROCESS_ERROR(m_hWnd);

	nRetCode = _CreateBackImage();
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult)
	{
		m_hWnd = NULL;
		m_nErrorCode = fwerr_failed;
	}
	return nResult;
}

int KFlashWindow::_CreateFlashOLE()
{
	int nRetCode = false;
	int nResult  = false;
	HRESULT hr = S_FALSE; 

	hr = OleCreate(ShockwaveFlashObjects::CLSID_ShockwaveFlash, IID_IOleObject, OLERENDER_DRAW, 
		0, (IOleClientSite *)this, (IStorage *)this, (void **)&m_lpO);
	if (!SUCCEEDED(hr))
	{
		m_nErrorCode = fwerr_not_install_flash;
		goto Exit0;
	}

	hr = OleSetContainedObject(m_lpO, TRUE); 
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpO->QueryInterface(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (void **)&m_lpControl); 
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpO->QueryInterface(IID_IViewObjectEx, (void **)&m_lpViewObjectEx); 
	if (FAILED(hr)) 
	{ 
		m_lpViewObjectEx = NULL; 
		hr = m_lpO->QueryInterface(IID_IViewObject, (void **)&m_lpViewObject); 
		KGLOG_COM_PROCESS_ERROR(hr);
	} 

	hr = m_lpO->QueryInterface(IID_IOleInPlaceObjectWindowless, (void **)&m_lpInPlaceObjWindowless); 
	if (FAILED(hr)) 
	{ 
		hr = m_lpO->QueryInterface(IID_IOleInPlaceObject, (void **)&m_lpInPlaceObj); 
		KGLOG_COM_PROCESS_ERROR(hr);
	} 

	m_lFlashVersion = m_lpControl->FlashVersion(); 
	if ((m_lFlashVersion & 0x00FF0000) < 0x00090000)
	{
		 m_nErrorCode = fwerr_flash_version_wrong;
		 goto Exit0;
	}

	hr = m_lpControl->QueryInterface(IID_IConnectionPointContainer, (void**)&m_lpConCont); 
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpConCont->FindConnectionPoint(ShockwaveFlashObjects::DIID__IShockwaveFlashEvents, &m_lpConPoint); 
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpConPoint->Advise((ShockwaveFlashObjects::_IShockwaveFlashEvents *)this, &m_dwConPointID); 
	KGLOG_COM_PROCESS_ERROR(hr);

	m_lpControl->PutWMode(L"transparent"); 
	m_lpControl->PutScale(L"showAll"); 
	m_lpControl->PutBackgroundColor(0x000000); 
	m_lpControl->PutEmbedMovie(FALSE);

	hr = m_lpO->DoVerb(OLEIVERB_SHOW, NULL, (IOleClientSite *)this, 0, NULL, NULL); 
	KGLOG_COM_PROCESS_ERROR(hr);
	
	nResult = true;
Exit0:
	return nResult;
}

int KFlashWindow::_LoadFlash(const tstring &strFlashName)
{ 
	HRESULT hr = E_FAIL; 
	int nRetCode = false;
	int nResult = false;
	MSG msg; 

	nRetCode = _access(strFlashName.c_str(), 0);
	if (-1 == nRetCode)//文件不存在
	{
		m_nErrorCode = fwerr_flash_file_err;
		goto Exit0;
	}

	m_lpControl->PutEmbedMovie(TRUE); 
	hr = m_lpControl->LoadMovie(0, strFlashName.c_str());
	if (!SUCCEEDED(hr))
	{
		m_nErrorCode = fwerr_flash_file_err;
		goto Exit0;
	}

	hr = m_lpControl->Play(); 
	KGLOG_COM_PROCESS_ERROR(hr);

	//把剩余消息循环完，让Flash渲染完毕后在进行操作，否则容易导致崩溃。 
	while (PeekMessage(&msg, NULL, 0, 0, true)) 
	{	 
		TranslateMessage(&msg); 
		DispatchMessage(&msg);
	} 

	nResult = true;
Exit0:
	return nResult; 
} 

int KFlashWindow::_ProcessFlashCall(const tstring &strXmlCommand)
{
	int nRetCode = false;
	int nResult  = false;
	tstring strCommand;
	PARAM_LIST Params;

	nRetCode = _ParseXmlFlashCommand(strXmlCommand, &strCommand, &Params);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _DispatchFlashCall(strCommand, Params);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KFlashWindow::_DispatchFlashCall(const tstring &strCommand, const PARAM_LIST &Params)
{
	int nRetCode = false;
	int nResult  = false;
	int nProcessedFlag = false;

	if (m_pFlashCallBack)
	{
		nProcessedFlag = m_pFlashCallBack->FlashCallback(strCommand, Params);
	}

	if (!m_pFlashCallBack || nProcessedFlag == false)
	{
		nProcessedFlag = true;
		if (_tcsicmp(strCommand.c_str(), _T("CanMove")) == 0)
		{
			m_nFlashWndMoveFlag = true;
		}
		else if (_tcsicmp(strCommand.c_str(), _T("MinWindow")) == 0)
		{
			ShowWindow(m_hWnd, SW_SHOWMINIMIZED);
		}
		else if (_tcsicmp(strCommand.c_str(), _T("CloseWindow")) == 0)
		{
			PostMessage(m_hWnd, WM_FLASHCLOSE, NULL, NULL);
		}
		else if (_tcsicmp(strCommand.c_str(), _T("PrintFlashLog")) == 0)
		{
			tstring &strLog = (Params.size() > 0) ? Params[0] : "";
			KGLogPrintf(KGLOG_INFO, "Flash Log: %s ", strLog.c_str());
		}
		else
		{
			nProcessedFlag = false;
		}
	}

	if (!nProcessedFlag)
	{
		KGLogPrintf(KGLOG_INFO, "Do not Process. FlashCall = %s ", strCommand.c_str());
	}

	nResult = true;
//Exit0:
	return nResult;
}

int KFlashWindow::_ParseXmlFlashCommand(const tstring &strXmlCommand, tstring *pstrCommand, PARAM_LIST *pParams)
{
	int nRetCode = false;
	int nResult  = false;

	assert(pstrCommand);
	assert(pParams);

	tstring &strCommand = *pstrCommand;
	PARAM_LIST &Params = *pParams;

	TiXmlDocument FlashParam;
	TiXmlDocument*  pFlashParam = NULL;
	TiXmlElement*   pInvoke     = NULL;
	TiXmlAttribute* pFunName    = NULL;
	TiXmlElement*   pParamElem  = NULL;	

	FlashParam.Parse(strXmlCommand.c_str());
	KG_PROCESS_ERROR(!FlashParam.Error());

	pInvoke = FlashParam.RootElement();
	KG_PROCESS_ERROR(pInvoke);

	pFunName = pInvoke->FirstAttribute();
	KG_PROCESS_ERROR(pFunName);

	strCommand = pFunName->Value(); //取得了函数名

	pParamElem = pInvoke->FirstChildElement()->FirstChildElement();
	while (pParamElem)
	{
		Params.push_back(pParamElem->FirstChild()->Value());

		pParamElem = pParamElem->NextSiblingElement();
	}

	nResult = true;
Exit0:
	return nResult;
}

int KFlashWindow::_MakeXmlFlashCommand(const tstring &strCommand, const PARAM_LIST &Params, tstring *pstrXmlCommand)
{
	int nRetCode = false;
	int nResult  = false;

	KG_PROCESS_ERROR(pstrXmlCommand);

	*pstrXmlCommand += "<invoke name=\"";
	*pstrXmlCommand += strCommand.c_str(); 
	*pstrXmlCommand += "\" returntype=\"xml\"><arguments>"; 

	for (PARAM_LIST::const_iterator itor = Params.begin(); itor != Params.end(); ++itor)
	{
		*pstrXmlCommand += "<string>";
		*pstrXmlCommand += *itor;
		*pstrXmlCommand += "</string>";
	}
	*pstrXmlCommand += "</arguments></invoke>";

	nResult = true;
Exit0:
	return nResult;
}

int KFlashWindow::_CreateBackImage()
{
	int nRetCode = false;
	int nResult  = false;
	HDC hWndDC = NULL;
	HDC tmphdcBack = NULL;

	KG_PROCESS_ERROR(!m_hdcBack);

	hWndDC = ::GetDC(m_hWnd); 
	KGLOG_PROCESS_ERROR(hWndDC);

	BITMAPINFOHEADER bih = {0}; 
	bih.biSize = sizeof(BITMAPINFOHEADER); 
	bih.biBitCount = 32; 
	bih.biCompression = BI_RGB; 
	bih.biPlanes = 1; 
	bih.biWidth = m_nWndWidth;
	bih.biHeight = -m_nWndHeight;

	m_hdcBack = CreateCompatibleDC(hWndDC); 
	KGLOG_PROCESS_ERROR(m_hdcBack);
	m_bmpBack = CreateDIBSection(hWndDC, (BITMAPINFO *)&bih, DIB_RGB_COLORS, (void **)&m_lpBitsOnly, NULL, 0x0); 
	KGLOG_PROCESS_ERROR(m_bmpBack);
	SelectObject(m_hdcBack, m_bmpBack);

	m_lp16BitsBmp = new BYTE[m_nWndWidth * m_nWndHeight * 4];
	memset(m_lp16BitsBmp, 0, m_nWndWidth * m_nWndHeight * 4);

	if (m_bmp16BPPBack && m_nBPP != 32)
	{
		tmphdcBack = CreateCompatibleDC(hWndDC);
		KGLOG_PROCESS_ERROR(tmphdcBack);
		SelectObject(tmphdcBack, m_bmp16BPPBack);

		nRetCode = BitBlt(m_hdcBack, 0, 0, m_nWndWidth, m_nWndHeight, tmphdcBack, 0, 0, SRCCOPY);
		KGLOG_PROCESS_ERROR(nRetCode);
		
		memcpy(m_lp16BitsBmp, m_lpBitsOnly, m_nWndWidth * m_nWndHeight * 4);	
	}
	
	nResult = true;
Exit0:
	if (tmphdcBack)
	{
		::DeleteDC(tmphdcBack);
		tmphdcBack = NULL;
	}

	if (hWndDC)
	{
		::ReleaseDC(m_hWnd, hWndDC);
		hWndDC = NULL;
	}

	return nResult;
}

//IUnknown 
 
HRESULT STDMETHODCALLTYPE KFlashWindow::QueryInterface(REFIID riid, void ** ppvObject) 
{ 
	*ppvObject = 0;
	if (IsEqualGUID(riid, IID_IUnknown)) 
		*ppvObject = (void*)(this); 
	else if (IsEqualGUID(riid, ShockwaveFlashObjects::DIID__IShockwaveFlashEvents)) 
		*ppvObject = (void*)dynamic_cast<ShockwaveFlashObjects::_IShockwaveFlashEvents *>(this); 
	else if (IsEqualGUID(riid, IID_IOleInPlaceSite)) 
		*ppvObject = (void*)dynamic_cast<IOleInPlaceSite *>(this); 
	else if (IsEqualGUID(riid, IID_IOleInPlaceSiteEx)) 
		*ppvObject = (void*)dynamic_cast<IOleInPlaceSiteEx *>(this); 
	else if (IsEqualGUID(riid, IID_IOleInPlaceSiteWindowless)) 
		*ppvObject = (void*)dynamic_cast<IOleInPlaceSiteWindowless *>(this); 
	else if (IsEqualGUID(riid, IID_IOleInPlaceFrame)) 
		*ppvObject = (void*)dynamic_cast<IOleInPlaceFrame *>(this); 
	else if (IsEqualGUID(riid, IID_IStorage)) 
		*ppvObject = (void*)dynamic_cast<IStorage *>(this); 
	else 	 
		goto Exit0;

	m_iRef++; 
	return S_OK; 
Exit0:
	return E_NOINTERFACE; 
} 
 
//IOleClientSite 
 
HRESULT STDMETHODCALLTYPE KFlashWindow::GetContainer(LPOLECONTAINER FAR* ppContainer) 
{ 
	*ppContainer = 0; 
	return E_NOINTERFACE; 
} 
 
//IOleInPlaceSite 
HRESULT STDMETHODCALLTYPE KFlashWindow::GetWindow(HWND FAR* lphwnd) 
{ 
	*lphwnd = GetHWND(); 
	return S_OK; 
} 
HRESULT STDMETHODCALLTYPE KFlashWindow::GetWindowContext(
	LPOLEINPLACEFRAME FAR* lplpFrame, 
	LPOLEINPLACEUIWINDOW FAR* lplpDoc, 
	LPRECT lprcPosRect, LPRECT lprcClipRect, 
	LPOLEINPLACEFRAMEINFO lpFrameInfo
) 
{ 
	*lplpFrame = (LPOLEINPLACEFRAME)this; 
 
	*lplpDoc = 0; 
 
	lpFrameInfo->fMDIApp = FALSE; 
	lpFrameInfo->hwndFrame = GetHWND(); 
	lpFrameInfo->haccel = 0; 
	lpFrameInfo->cAccelEntries = 0; 
 
	RECT r; 
	::GetClientRect(GetHWND(), &r); 
	*lprcPosRect = r; 
	*lprcClipRect = r; 
	return(S_OK); 
} 
 
//IOleInPlaceSiteEx  
HRESULT STDMETHODCALLTYPE KFlashWindow::OnInPlaceActivateEx(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags) 
{ 
	if (pfNoRedraw) 
		*pfNoRedraw = FALSE; 
	return S_OK; 
} 
 
//IOleInPlaceSiteWindowless  
HRESULT STDMETHODCALLTYPE KFlashWindow::InvalidateRect(LPCRECT pRect, BOOL fErase)
{ 
	Draw(NULL, pRect, fErase); 
	return S_OK; 
} 
 
void KFlashWindow::Draw(HDC hdcDraw, const RECT *rcDraw, BOOL bErase) 
{
	int nRetCode = false;
	int nResult  = false;
	HRESULT hr; 
	RECT WndRect; 
	HDC hWndDC = NULL;
 
	//控制帧数率为24fps
	if (m_lTotalFrame >= (GetTickCount() - m_lStartTime) * 24 / 1000)
		goto Exit0;
	m_lTotalFrame++;

	nRetCode = ::GetWindowRect(m_hWnd, &WndRect); 
	KGLOG_PROCESS_ERROR(nRetCode);

 	if (
		(WndRect.right - WndRect.left) > m_nWndWidth || 
		(WndRect.bottom - WndRect.top) > m_nWndHeight
	)
 	{
		KGLogPrintf(KGLOG_ERR, "GetWindowRect Error, Top=%d, left=%d, right=%d, bottom=%d", WndRect.top, WndRect.left, WndRect.right, WndRect.bottom);		
		WndRect = m_rctLastLocation;
 	}	

	IViewObject *lpView = m_lpViewObjectEx ? (IViewObject *)m_lpViewObjectEx : m_lpViewObject; 
	if (m_lpO && lpView) 
	{ 
		RECT rTotal = {0, 0, m_nWndWidth, m_nWndHeight};

		memcpy(m_lpBitsOnly, m_lp16BitsBmp, m_nWndWidth * m_nWndHeight * 4);

		hr = OleDraw(lpView, DVASPECT_TRANSPARENT, m_hdcBack, &rTotal); 
		KGLOG_COM_PROCESS_ERROR(hr);
	} 

	POINT p  = {WndRect.left, WndRect.top}; 
	POINT p2 = {0, 0}; 
	SIZE sz  = {m_nWndWidth, m_nWndHeight}; 
 
	BLENDFUNCTION bf; 
	bf.BlendOp = AC_SRC_OVER; 
	bf.AlphaFormat = AC_SRC_ALPHA; 
	bf.BlendFlags = 0; 
	bf.SourceConstantAlpha = 255;
	nRetCode = UpdateLayeredWindow(m_hWnd, NULL, &p, &sz, m_hdcBack, &p2, m_crTransparencyColour, &bf, (m_nBPP == 32) ? ULW_ALPHA : ULW_COLORKEY); 
	KGLOG_PROCESS_ERROR(nRetCode);

	m_rctLastLocation = WndRect;

Exit0:
	return;
} 
 
HRESULT STDMETHODCALLTYPE KFlashWindow::Invoke(  
	/* [in] */ DISPID dispIdMember, 
	/* [in] */ REFIID riid, 
	/* [in] */ LCID lcid, 
	/* [in] */ WORD wFlags, 
	/* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams, 
	/* [out] */ VARIANT __RPC_FAR *pVarResult, 
	/* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo, 
	/* [out] */ UINT __RPC_FAR *puArgErr) 
{ 
	switch(dispIdMember) 
	{
		case 0x7a6:	
			break; 
		case 0x96:	
			break; 
		case 0xC5: 
			if((pDispParams->cArgs == 1) && (pDispParams->rgvarg[0].vt == VT_BSTR)) 
			{
				_ProcessFlashCall(tstring((_bstr_t)pDispParams->rgvarg[0].bstrVal));
			}	 
			break; 
		case DISPID_READYSTATECHANGE: 
			break; 
		default:			 
			return DISP_E_MEMBERNOTFOUND; 
	}

	return S_OK;
} 
 
LRESULT CALLBACK STDMETHODCALLTYPE KFlashWindow::WndProcStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
	if (uMsg == WM_CREATE) 
	{ 
		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam; 
		SetWindowLong(hWnd, GWL_USERDATA, (long)lpcs->lpCreateParams); 
		return 0; 
	} 	
	KFlashWindow *lpWnd = (KFlashWindow*)GetWindowLong(hWnd, GWL_USERDATA); 
	if (lpWnd) 
		return lpWnd->WndProc(hWnd, uMsg, wParam, lParam); 
	else 
		return DefWindowProc(hWnd, uMsg, wParam, lParam); 
} 
 
LRESULT KFlashWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
	static char temp[1024];
 
	switch (uMsg) 
	{ 
	case WM_LBUTTONDOWN: 
		m_nFlashWndMoveFlag = false;  
		break;  
	case WM_LBUTTONUP: 
		m_nFlashWndMoveFlag = false; 
		break; 
	case WM_MOUSEMOVE: 
		if (m_nFlashWndMoveFlag && wParam == MK_LBUTTON) 
			PostMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0); 
		break;
	case WM_DESTROY:
	case WM_CLOSE:
	case WM_QUIT:
		{
			PARAM_LIST params;
			_DispatchFlashCall(_T("CloseWindow"), params);
			return 0;
		}		
		break;
	case WM_FLASHCALL:
		{
			EnterCriticalSection(&m_CriticalSection);
			CALL_FLASH_COMMAND_MAP::iterator it = m_CallFlashCommands.find((unsigned)wParam);
			if (it != m_CallFlashCommands.end())
			{
				HRESULT hr = m_lpControl->raw_CallFunction(_bstr_t(it->second.c_str()), (BSTR *)temp);
				m_CallFlashCommands.erase(it);
			}
			LeaveCriticalSection(&m_CriticalSection);
		}
		break;
	default:
		break;
	}

	if (m_lpInPlaceObjWindowless) 
	{ 
		if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_LBUTTONDBLCLK
			|| uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_MBUTTONDBLCLK 
			|| uMsg == WM_MOUSEWHEEL  
			|| uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_CHAR 
			|| uMsg == WM_SETCURSOR 
			) 
		{ 
			HRESULT hr; 
			LRESULT res; 
			hr = m_lpInPlaceObjWindowless->OnWindowMessage(uMsg, wParam, lParam, &res); 
			if (hr == S_OK) 
				return res; 
		} 
	} 
	return DefWindowProc(hWnd, uMsg, wParam, lParam); 
} 