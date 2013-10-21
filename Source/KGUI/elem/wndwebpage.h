#ifndef UI_ELEM_K_WND_WEB_PAGE_H
#define UI_ELEM_K_WND_WEB_PAGE_H


#include <exdisp.h>
#include "./wndwindow.h"
#include "../KGUI.h"


namespace UI
{
	class KWndWebStorage : public IStorage
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();

		virtual HRESULT STDMETHODCALLTYPE CreateStream(const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm);
		virtual HRESULT STDMETHODCALLTYPE OpenStream(const OLECHAR *pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm);
		virtual HRESULT STDMETHODCALLTYPE CreateStorage(const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg);
		virtual HRESULT STDMETHODCALLTYPE OpenStorage(const OLECHAR *pwcsName, IStorage *pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg);
		virtual HRESULT STDMETHODCALLTYPE CopyTo(DWORD ciidExclude, const IID *rgiidExclude, SNB snbExclude, IStorage *pstgDest);
		virtual HRESULT STDMETHODCALLTYPE MoveElementTo(const OLECHAR *pwcsName, IStorage *pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags);
		virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
		virtual HRESULT STDMETHODCALLTYPE Revert();
		virtual HRESULT STDMETHODCALLTYPE EnumElements(DWORD reserved1, void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum);
		virtual HRESULT STDMETHODCALLTYPE DestroyElement(const OLECHAR *pwcsName);
		virtual HRESULT STDMETHODCALLTYPE RenameElement(const OLECHAR *pwcsOldName, const OLECHAR *pwcsNewName);
		virtual HRESULT STDMETHODCALLTYPE SetElementTimes(const OLECHAR *pwcsName, const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime);
		virtual HRESULT STDMETHODCALLTYPE SetClass(REFCLSID clsid);
		virtual HRESULT STDMETHODCALLTYPE SetStateBits(DWORD grfStateBits, DWORD grfMask);
		virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag);
	};

	class KWndWebOleClientSite 
		: public IOleClientSite
		, public IOleInPlaceSite
		, public IOleInPlaceFrame
	{
	public:
		KWndWebOleClientSite();

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();

		virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND *phwnd);
		virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);

		virtual HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder);
		virtual HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths);
		virtual HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS pborderwidths);
		virtual HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName);

		virtual HRESULT STDMETHODCALLTYPE InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
		virtual HRESULT STDMETHODCALLTYPE SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
		virtual HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU hmenuShared);
		virtual HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR pszStatusText);
		virtual HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable);
		virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD wID);

		virtual HRESULT STDMETHODCALLTYPE SaveObject();
		virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
		virtual HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer **ppContainer);
		virtual HRESULT STDMETHODCALLTYPE ShowObject();
		virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow);
		virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();

		virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate();
		virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate();
		virtual HRESULT STDMETHODCALLTYPE OnUIActivate();
		virtual HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
		virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant);
		virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);
		virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate();
		virtual HRESULT STDMETHODCALLTYPE DiscardUndoState();
		virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo();
		virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect);

		HWND m_hWnd;
		RECT m_rect;
	};

	class KWndWebPage;
	class KWndWebOleEventHandler : public IDispatch
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();

		virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo);
		virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
		virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
		virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

		void SetWndWebPage(KWndWebPage * pWndWebPage);

		KWndWebOleEventHandler();
		virtual ~KWndWebOleEventHandler();
	private:
		KWndWebPage * m_pWndWebPage;
	};

	class KWndWebPage : public KWndWindow
	{
	public:
		KWndWebPage();
		virtual ~KWndWebPage();

		void* operator new(size_t nSize, std::nothrow_t const&) throw();
		void operator delete(void* pData, std::nothrow_t const&) throw();

		void* operator new(size_t nSize);
		void operator delete(void* pData) throw();

	protected:
		void* operator new[](std::size_t, std::nothrow_t const&) throw();
		void operator delete[](void*, std::nothrow_t const&) throw();

		void* operator new[](std::size_t) throw(std::bad_alloc);
		void operator delete[](void*) throw();

		void* operator new(size_t, void*) throw();
		void operator delete(void*, void*) throw();

		void* operator new[](std::size_t, void*) throw();
		void operator delete[](void*, void*) throw();

	public:
		static BOOL Init();
		static void Exit();

		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual int	CanCastTo(LPCSTR szType) const;
		virtual LPCSTR GetWndType() const;

		int Navigate(LPWSTR szURL);

		void GoBack();
		void GoForward();
		void SetCanGoBack(BOOL bCan);
		void SetCanGoForward(BOOL bCan);
		BOOL CanGoBack();
		BOOL CanGoForward();
		void Refresh();
		LPCWSTR GetLocationName();
		LPCWSTR GetLocationURL();
		void OnTitleChanged();

		IWebBrowser2* GetWebBrowser();

	protected:
		virtual void OnDestroy();
		virtual void OnBreathe();
		virtual void OnPaint();
		virtual void OnSize();
		virtual void OnScale(float fScaleX, float fScaleY);
		virtual void OnMove();

	private:
		LRESULT OnSetFocus();
		LRESULT OnKillFocus();

		int Embed(HWND hwnd);
		void Unembed();

		void ResizeWebPage(long lWidth, long lHeight);

		void UpdateTexture();
		void UpdateViewport();

	private:
		KWndWebOleClientSite m_OleClientSite;
		KWndWebStorage m_Storage;
		KWndWebOleEventHandler m_Handler;

		IOleObject* m_pBrowser;
		IWebBrowser2* m_pWebBrowser2;

		DWORD m_dwCookie;
		IConnectionPoint* m_pIConnectionPoint;
		IConnectionPointContainer* m_pIConnectionPointContainer;

		KG3DUIImage2 m_Image;

		HWND m_hWnd;

		BOOL m_bCanGoBack;
		BOOL m_bCanGoForward;
		WCHAR m_szLocationName[MAX_PATH];
		WCHAR m_szLocationURL[MAX_PATH];
	};

}   // namespace UI

#endif // UI_ELEM_K_WND_WEB_PAGE_H

