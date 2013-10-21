////////////////////////////////////////////////////////////////////
//
// IECapt - A Internet Explorer Web Page Rendering Capture Utility
//
// Copyright (C) 2003-2006 Bjoern Hoehrmann <bjoern@hoehrmann.de>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// $Id: IECapt.cpp,v 1.3 2008/01/08 11:07:22 chentianhong Exp $
//
////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#define VC_EXTRALEAN
#include <stdlib.h>
#include <windows.h>
#include <mshtml.h>
#include <exdispid.h>
#include <atlbase.h>
#include <atlwin.h>
#include <atlcom.h>
#include <atlhost.h>
#include <atlimage.h>
#undef VC_EXTRALEAN

class CMain;
class CEventSink;

//////////////////////////////////////////////////////////////////
// CEventSink
//////////////////////////////////////////////////////////////////
class CEventSink :
	public CComObjectRootEx <CComSingleThreadModel>,
	public IDispatch
{
public:
	CEventSink() : m_pMain(NULL) {}

public:

	BEGIN_COM_MAP(CEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(DIID_DWebBrowserEvents2, IDispatch)
	END_COM_MAP()

	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo);
	STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo);
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid);
	STDMETHOD(Invoke)(DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams,
		VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr);

public:
	CMain* m_pMain;
};

//////////////////////////////////////////////////////////////////
// CMain
//////////////////////////////////////////////////////////////////
class CMain :
	public CWindowImpl <CMain>
{
public:

	CMain() : m_dwCookie(0) { }

public:

	BEGIN_MSG_MAP(CMainWindow)
		MESSAGE_HANDLER(WM_CREATE,  OnCreate)
		MESSAGE_HANDLER(WM_SIZE,    OnSize)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

public:

	LRESULT OnCreate  (UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize    (UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy (UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:

	BOOL SaveSnapshot(IDispatch* pdisp, VARIANT* purl);

public:
	LPCSTR m_URI;
	LPCSTR m_fileName;

protected:
	CComPtr<IUnknown> m_pWebBrowserUnk;
	CComPtr<IWebBrowser2> m_pWebBrowser;
	CComObject<CEventSink>* m_pEventSink;
	HWND m_hwndWebBrowser;
	DWORD m_dwCookie;
};

//////////////////////////////////////////////////////////////////
// Implementation of CEventSink
//////////////////////////////////////////////////////////////////
STDMETHODIMP CEventSink::GetTypeInfoCount(UINT* pctinfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CEventSink::GetTypeInfo(UINT itinfo, LCID lcid,
									 ITypeInfo** pptinfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CEventSink::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames,
									   UINT cNames, LCID lcid,
									   DISPID* rgdispid)
{
	return E_NOTIMPL;
}

STDMETHODIMP CEventSink::Invoke(DISPID dispid, REFIID riid, LCID lcid,
								WORD wFlags, DISPPARAMS* pdispparams,
								VARIANT* pvarResult, EXCEPINFO* pexcepinfo,
								UINT* puArgErr)
{
	if (dispid != DISPID_DOCUMENTCOMPLETE)
		return S_OK;

	if (pdispparams->cArgs != 2)
		return S_OK;

	if (pdispparams->rgvarg[0].vt != (VT_VARIANT | VT_BYREF))
		return S_OK;

	if (pdispparams->rgvarg[1].vt != VT_DISPATCH)
		return S_OK;

	if (m_pMain->SaveSnapshot(pdispparams->rgvarg[1].pdispVal,
		pdispparams->rgvarg[0].pvarVal))
		m_pMain->PostMessage(WM_CLOSE);

	return S_OK;
}

//////////////////////////////////////////////////////////////////
// Implementation of CMain Messages
//////////////////////////////////////////////////////////////////
LRESULT CMain::OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr;
	RECT old;
	//IUnknown * pUnk = NULL;
	GetClientRect(&old);

	m_hwndWebBrowser = ::CreateWindow(_T(ATLAXWIN_CLASS), m_URI,
		/*WS_POPUP|*/WS_CHILD|WS_DISABLED, old.top, old.left, old.right,
		old.bottom, m_hWnd, NULL, ::GetModuleHandle(NULL), NULL);

	hr = AtlAxGetControl(m_hwndWebBrowser, &m_pWebBrowserUnk);

	if (FAILED(hr))
		return 1;

	if (m_pWebBrowserUnk == NULL)
		return 1;

	hr = m_pWebBrowserUnk->QueryInterface(IID_IWebBrowser2, (void**)&m_pWebBrowser);

	if (FAILED(hr))
		return 1;

	hr = CComObject<CEventSink>::CreateInstance(&m_pEventSink);

	if (FAILED(hr))
		return 1;

	m_pEventSink->m_pMain = this;

	hr = AtlAdvise(m_pWebBrowserUnk, m_pEventSink->GetUnknown(),
		DIID_DWebBrowserEvents2, &m_dwCookie);

	if (FAILED(hr))
		return 1;

	return 0;
}


LRESULT CMain::OnSize(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_hwndWebBrowser != NULL)
		::MoveWindow(m_hwndWebBrowser, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);

	return 0;
}

LRESULT CMain::OnDestroy(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr;

	if (m_dwCookie != 0)
		hr = AtlUnadvise(m_pWebBrowserUnk, DIID_DWebBrowserEvents2, m_dwCookie);

	m_pWebBrowser.Release();
	m_pWebBrowserUnk.Release();

	PostQuitMessage(0);

	return 0;
}

//////////////////////////////////////////////////////////////////
// Implementation of CMain::SaveSnapshot
//////////////////////////////////////////////////////////////////
BOOL CMain::SaveSnapshot(IDispatch* pdisp, VARIANT* purl)
{
	//这三个不确定要不要Release，但Com规范里面要求Release的
	IHTMLDocument3* pDocument3 = NULL;
	IHTMLDocument2* pDocument  = NULL;
	IHTMLElement2* pElement2   = NULL;

	IHTMLElement* pElement     = NULL;//从get_body来的，不用释放
	IViewObject2* pViewObject  = NULL;//从QueryInterface来的，要释放
	IDispatch* pDispatch       = NULL;//从get_Document来的，不用释放
	IDispatch* pWebBrowserDisp = NULL;//从QueryInterface来的，要释放

	HRESULT hr;
	long bodyHeight;
	long bodyWidth;
	long rootHeight;
	long rootWidth;
	long height;
	long width;

	HDC hdcMain = NULL;

	hr = m_pWebBrowser->get_Document(&pDispatch);
	KG_COM_PROCESS_ERROR(hr);

	hr = m_pWebBrowserUnk->QueryInterface(IID_IDispatch, (void**)&pWebBrowserDisp);

	KG_COM_PROCESS_ERROR(hr);

	if (pWebBrowserDisp != pdisp)
	{
		pWebBrowserDisp->Release();
		pWebBrowserDisp = NULL;
		return false;
	}

	hr = pDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pDocument);
	KG_COM_PROCESS_ERROR(hr);

	hr = pDocument->get_body(&pElement);
	KG_COM_PROCESS_ERROR(hr);

	hr = pElement->QueryInterface(IID_IHTMLElement2, (void**)&pElement2);
	KG_COM_PROCESS_ERROR(hr);

	hr = pElement2->get_scrollHeight(&bodyHeight);
	KG_COM_PROCESS_ERROR(hr);

	hr = pElement2->get_scrollWidth(&bodyWidth);
	KG_COM_PROCESS_ERROR(hr);

	hr = pDispatch->QueryInterface(IID_IHTMLDocument3, (void**)&pDocument3);
	KG_COM_PROCESS_ERROR(hr);

	hr = pDocument3->get_documentElement(&pElement);
	KG_COM_PROCESS_ERROR(hr);

	SAFE_RELEASE(pElement2);
	hr = pElement->QueryInterface(IID_IHTMLElement2, (void**)&pElement2);
	KG_COM_PROCESS_ERROR(hr);

	hr = pElement2->get_scrollHeight(&rootHeight);
	KG_COM_PROCESS_ERROR(hr);

	hr = pElement2->get_scrollWidth(&rootWidth);
	KG_COM_PROCESS_ERROR(hr);

	width = bodyWidth;
	height = rootHeight > bodyHeight ? rootHeight : bodyHeight;

	MoveWindow(0, 0, width, height, TRUE);      
	::MoveWindow(m_hwndWebBrowser, 0, 0, width, height, TRUE);

	hr = m_pWebBrowser->QueryInterface(IID_IViewObject2, (void**)&pViewObject);
	KG_COM_PROCESS_ERROR(hr);

	{
		BITMAPINFOHEADER bih;
		BITMAPINFO bi;
		RGBQUAD rgbquad;

		ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));
		ZeroMemory(&rgbquad, sizeof(RGBQUAD));

		bih.biSize          = sizeof(BITMAPINFOHEADER);
		bih.biWidth         = width;
		bih.biHeight        = height;
		bih.biPlanes        = 1;
		bih.biBitCount      = 24;
		bih.biClrUsed       = 0;
		bih.biSizeImage     = 0;
		bih.biCompression   = BI_RGB;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;

		bi.bmiHeader = bih;
		bi.bmiColors[0] = rgbquad;

		hdcMain = GetDC();

		KG_PROCESS_ERROR(hdcMain);

		HDC hdcMem = CreateCompatibleDC(hdcMain);

		KG_PROCESS_ERROR(hdcMem);

		char* bitmapData = NULL;
		HBITMAP hBitmap = CreateDIBSection(hdcMain, &bi, DIB_RGB_COLORS, (void**)&bitmapData, NULL, 0);

		if (!hBitmap) {
			// TODO: cleanup
			return true;
		}

		SelectObject(hdcMem, hBitmap);

		RECTL rcBounds = { 0, 0, width, height };
		hr = pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, hdcMain,
			hdcMem, &rcBounds, NULL, NULL, 0);

		if (SUCCEEDED(hr))
		{
			CImage image;
			image.Create(width, height, 24);
			CImageDC imageDC(image);
			::BitBlt(imageDC, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
			image.Save(m_fileName);
		}

		DeleteObject(hBitmap);
		DeleteDC(hdcMem);
	}

Exit0:
	SAFE_RELEASE(pViewObject);
	SAFE_RELEASE(pWebBrowserDisp);

	SAFE_RELEASE(pDocument3);
	SAFE_RELEASE(pElement2);
	SAFE_RELEASE(pDocument);

	if (hdcMain)
	{
		ReleaseDC(hdcMain);
	}
	return true;
}

static const GUID myGUID = { 0x445c10c2, 0xa6d4, 0x40a9, { 0x9c, 0x3f, 0x4e, 0x90, 0x42, 0x1d, 0x7e, 0x83 } };
static CComModule _Main;

int main (int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Usage: %s http://www.example.org/ localfile.png\n", argv[0]);
		return EXIT_FAILURE;
	}

	HRESULT hr = _Main.Init(NULL, ::GetModuleHandle(NULL), &myGUID);

	if (FAILED(hr))
		return EXIT_FAILURE;

	if (!AtlAxWinInit())
		return EXIT_FAILURE;

	CMain MainWnd;

	MainWnd.m_URI = argv[1];
	MainWnd.m_fileName = argv[2];
	RECT rcMain = { 0, 0, 800, 600 };
	MainWnd.Create(NULL, rcMain, _T("IECapt"), WS_POPUP);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	_Main.Term();

	return EXIT_SUCCESS;
}

