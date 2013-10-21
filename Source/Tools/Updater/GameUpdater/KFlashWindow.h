#ifndef _KFLASHWINDOW_H_
#define _KFLASHWINDOW_H_

#import "Flash10b.ocx" named_guids no_auto_exclude

#include <string.h>
#include <vector>
#include <map>

////////////////////////////////////////////////////////////////////////////////
// @return : return true only if the flash had been installed and the flash version is bigger than 9.
int KG_CheckFlashAvailable();

////////////////////////////////////////////////////////////////////////////////
enum FLASH_WINDOW_ERROR_CODE
{
	fwerr_ok,
	fwerr_not_install_flash,
	fwerr_flash_version_wrong,
	fwerr_flash_file_err,
	fwerr_failed
};

typedef std::vector<tstring> PARAM_LIST;

class IFlashCallBack
{
public:
	//返回1 表示被处理，0 表示未被处理
	virtual int FlashCallback(const tstring &strFunName, const PARAM_LIST &ParamList) = 0;
};

class KFlashWindow :
		public IOleClientSite,
		public IOleInPlaceSiteWindowless,
		public IOleInPlaceFrame,
		public IStorage,
		public ShockwaveFlashObjects::_IShockwaveFlashEvents
{
public:
	KFlashWindow();
	virtual ~KFlashWindow();
public:
	int Create(
		const tstring &strFlashFile, 
		IFlashCallBack *pFlashCallback,
		const tstring &strClassName, 
		const tstring &strWindowName, 
		int x, int y, int nWidth, int nHeight,
		HINSTANCE hInstance, DWORD hIcon,
		HBITMAP h16BPPBackImage = NULL,
		COLORREF crTransColor = RGB(0, 0, 0)
	);
	int Destroy();
	int ShowWnd();
	int SendCommandToFlash(const tstring &strCommand, const PARAM_LIST &Params);

	int GetBPP();
	HWND GetHWND() {return m_hWnd;}
	FLASH_WINDOW_ERROR_CODE GetErrorCode() {return m_nErrorCode;}
	
private:
	int _CreateFlashWindow(const tstring &strClassName, const tstring &strWindowName, int x, int y, int nWidth, int nHeight, HINSTANCE hInstance, DWORD hIcon);
	int _CreateFlashOLE();
	int _LoadFlash(const tstring &strFlashName);

	int _ProcessFlashCall(const tstring &strXmlCommand);	
	int _DispatchFlashCall(const tstring &strCommand, const PARAM_LIST &Params);

	int _ParseXmlFlashCommand(const tstring &strXmlCommand, tstring *pstrCommand, PARAM_LIST *pParams);
	int _MakeXmlFlashCommand(const tstring &strCommand, const PARAM_LIST &Params, tstring *pstrXmlCommand);

	int _CreateBackImage();

protected:
	IFlashCallBack* m_pFlashCallBack;
	long    m_lStartTime;
	long    m_lTotalFrame;
	FLASH_WINDOW_ERROR_CODE m_nErrorCode;
	int     m_nFlashWndMoveFlag;

	typedef std::map<unsigned, tstring> CALL_FLASH_COMMAND_MAP;
	CALL_FLASH_COMMAND_MAP m_CallFlashCommands;

	CRITICAL_SECTION m_CriticalSection;

protected:						 
	ShockwaveFlashObjects::IShockwaveFlash* m_lpControl;
	IOleObject *m_lpO;
	IViewObjectEx *m_lpViewObject;
	IViewObjectEx *m_lpViewObjectEx;
	IOleInPlaceObject *m_lpInPlaceObj;
	IOleInPlaceObjectWindowless *m_lpInPlaceObjWindowless;
	IConnectionPointContainer *m_lpConCont;
	IConnectionPoint *m_lpConPoint;

	HINSTANCE m_hInstance;
	HWND    m_hWnd;
	DWORD   m_dwConPointID;
	HDC     m_hdcBack;
	HBITMAP m_bmpBack;
	HBITMAP m_bmp16BPPBack;
	BYTE*	m_lp16BitsBmp;
	BYTE*   m_lpBitsOnly;
	int     m_nBPP;
	long    m_lFlashVersion;
	COLORREF m_crTransparencyColour;

	RECT    m_rctLastLocation;
	int     m_nWndWidth;
	int     m_nWndHeight;

protected:
	static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
private:
	virtual void Draw(HDC hdcDraw, const RECT *rcDraw, BOOL bErase);
protected:
	//interface methods

	//IUnknown 
 	int m_iRef;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(){m_iRef++; return m_iRef;}
	ULONG STDMETHODCALLTYPE Release(){m_iRef--; return m_iRef;}

	//IOleClientSite
	virtual HRESULT STDMETHODCALLTYPE SaveObject(){ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk){ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetContainer(LPOLECONTAINER FAR* ppContainer);
	virtual HRESULT STDMETHODCALLTYPE ShowObject(){return S_OK;}
	virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow){ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(){ return E_NOTIMPL; }

	//IOleInPlaceSite
	virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd);
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(){return(S_OK);}
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(){return(S_OK);}
	virtual HRESULT STDMETHODCALLTYPE OnUIActivate(){return(S_OK);}
	virtual HRESULT STDMETHODCALLTYPE GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame,LPOLEINPLACEUIWINDOW FAR* lplpDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo);
	virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtent) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable){return(S_OK);}
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(){return(S_OK);}
	virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(){ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(){ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect){return(S_OK);}

	//IOleInPlaceSiteEx
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags);
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(BOOL fNoRedraw){return S_FALSE;}
	virtual HRESULT STDMETHODCALLTYPE RequestUIActivate(void){return S_FALSE;}

	//IOleInPlaceSiteWindowless
	virtual HRESULT STDMETHODCALLTYPE CanWindowlessActivate( void){return S_OK;} 
	virtual HRESULT STDMETHODCALLTYPE GetCapture( void){return S_FALSE;}
	virtual HRESULT STDMETHODCALLTYPE SetCapture( 
		/* [in] */ BOOL fCapture){return S_FALSE;}
	virtual HRESULT STDMETHODCALLTYPE GetFocus( void){return S_OK;}
	virtual HRESULT STDMETHODCALLTYPE SetFocus( 
		/* [in] */ BOOL fFocus){return S_OK;}
	virtual HRESULT STDMETHODCALLTYPE GetDC(LPCRECT pRect, DWORD grfFlags, HDC __RPC_FAR *phDC){return S_FALSE;}
	virtual HRESULT STDMETHODCALLTYPE ReleaseDC(HDC hDC){return S_FALSE;}
	virtual HRESULT STDMETHODCALLTYPE InvalidateRect(LPCRECT pRect, BOOL fErase);
	virtual HRESULT STDMETHODCALLTYPE InvalidateRgn(HRGN hRGN, BOOL fErase) {return S_OK;}
	virtual HRESULT STDMETHODCALLTYPE ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE AdjustRect(LPRECT prc){return S_FALSE;}
	virtual HRESULT STDMETHODCALLTYPE OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT __RPC_FAR *plResult){return S_FALSE;}

	//IOleInPlaceFrame
	virtual HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder) { return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths){ return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS pborderwidths){ return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName){ return(S_OK);}
	virtual HRESULT STDMETHODCALLTYPE InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths){ return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject){ return(S_OK);}
	virtual HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU hmenuShared){ return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR pszStatusText){ return(S_OK);}
	virtual HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable){ return(S_OK);}
	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD wID){ return E_NOTIMPL;}

	//IStorage
	virtual HRESULT STDMETHODCALLTYPE CreateStream(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE OpenStream(const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE CreateStorage(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE OpenStorage(const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE CopyTo(DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude,IStorage *pstgDest){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE MoveElementTo(const OLECHAR *pwcsName,IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE Revert(){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE EnumElements(DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE DestroyElement(const OLECHAR *pwcsName){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE RenameElement(const WCHAR *pwcsOldName, const WCHAR *pwcsNewName){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE SetElementTimes(const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE SetClass(REFCLSID clsid){return(S_OK);}
	virtual HRESULT STDMETHODCALLTYPE SetStateBits(DWORD grfStateBits, DWORD grfMask){return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG * pstatstg, DWORD grfStatFlag){return E_NOTIMPL;}

	//IDispatch
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT __RPC_FAR *pctinfo){return E_NOTIMPL;};
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo){return E_NOTIMPL;};
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR __RPC_FAR *rgszNames, UINT cNames, LCID lcid, DISPID __RPC_FAR *rgDispId){return E_NOTIMPL;}
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, 
		DISPPARAMS __RPC_FAR *pDispParams, VARIANT __RPC_FAR *pVarResult, EXCEPINFO __RPC_FAR *pExcepInfo, UINT __RPC_FAR *puArgErr);

	//DShockwaveFlashEvents
	HRESULT STDMETHODCALLTYPE OnReadyStateChange (long newState) {return S_OK;}
	HRESULT STDMETHODCALLTYPE OnProgress (long percentDone){return S_OK;}
	HRESULT STDMETHODCALLTYPE FSCommand (_bstr_t command, _bstr_t args){return S_OK;}
	HRESULT STDMETHODCALLTYPE FlashCall (_bstr_t Result){return S_OK;};
};

#endif