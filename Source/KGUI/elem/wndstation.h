#ifndef UI_ELEM_K_WND_STATION_H
#define UI_ELEM_K_WND_STATION_H

#include "KG3DEngine/KG3DTypes.h"
#include "./wndwindow.h"

struct KG3DDeviceSettings;

namespace UI
{
	enum WND_LAYER_LIST
	{
		WL_BEGIN  = 0,

		WL_LOWEST = WL_BEGIN,		    //最底层
		WL_LOWEST1,
		WL_LOWEST2,
		WL_NORMAL,					    //一般层
		WL_NORMAL1,
		WL_NORMAL2,
		WL_TOPMOST,					    //最顶层
		WL_TOPMOST1,
		WL_TOPMOST2,

		WL_VISIBLE,

		WL_TEXTURE = WL_VISIBLE,		//绘制到贴图上，贴图可以是场景上的，也可以是界面上的，但贴图必须是3D引擎管理的

		WL_END, 
		WL_UNKNOWN = WL_END,
	};

    class KWndFrame;

    class KWndStation
	{
		friend class KWndWindow;
		friend class KWndFrame;

	public:
		enum { WND_TREE_PATH_LEN = 256 };

	public:
        KWndStation();
        ~KWndStation();

		int Init();
		void Exit();
		bool Breathe();
		void Activate(bool bActivated);
		bool ReInit();

		static int	GetLayerIndexByName(LPCSTR pszLayerName);

		bool		AddFrame(KWndFrame *pWnd, int nLayer);	

		void		SetFocusWindow(KWndWindow* pWnd);
		KWndWindow*	GetFocusWindow();

		void        SetActiveFrame(KWndFrame *pFrame, bool bBringToTop);
		KWndFrame*  GetActiveFrame(); 
		KWndFrame*  GetTopFrame(int nLayer);
		KWndFrame*  GetTopFrame(int nX, int nY);
		KWndFrame*  GetRootFrame(KWndWindow* pWnd);
		KWndFrame*  GetLastFrame(int nLayer);
		bool        IsRootWindow(KWndWindow const *pWnd) const; 

		KWndFrame*	SearchFrame(LPCSTR szFrameName);
		KWndWindow* SearchWindow(KWndWindow *pRoot, LPCSTR szSectionName);
		KWndWindow* SearchWindow(LPCSTR szTreePath, LPCSTR szDelimit = "/\\");
		KWndWindow* SearchChildWindowByPath(KWndWindow *pFather, LPCSTR szTreePath, LPCSTR szDelimit = "/\\");
		LPCSTR		GetTreePath(KWndWindow const* pWnd);

		void		SetCapture(KWndWindow* pWnd, bool bDontSendToChild = true);
		KWndWindow* GetCapture();
		void		ReleaseCapture();

        HIMC        GetHIMC() const;

		void		Paint();
		LRESULT		HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam);

		bool		IsEnabled() const;
		void		Enable(bool bEnable);

		bool		LoadFrameList(LPCSTR szFilePath);
		bool		UnloadFrameList(LPCSTR szFilePath);

		bool		AdjustFrameListPosition(LPCSTR szFilePath);

		BOOL ToggleFrame(LPCSTR szFileName, LPCSTR szFrameName);
		KWndFrame* OpenFrame(LPCSTR szFileName, LPCSTR szFrameName);
		BOOL CloseFrame(LPCSTR szFrameName);

		int			GetCursorPosInClient(int &nX, int &nY);

		UINT		GetMessage();
		int			GetMessagePos(int &nX, int &nY);
		float		GetMessageWheelDelta();
		int			GetMessageKey();


		//-----------------item事件相关--------------------
		int			SetLastMouseLButtonDownInfo(KItemNull *pItem, DWORD dwReason);
		KItemNull*  GetLastMouseLButtonDownItem();

		int			SetLastMouseRButtonDownInfo(KItemNull *pItem, DWORD dwReason);
		KItemNull*  GetLastMouseRButtonDownItem();

		int			SetLastMouseMButtonDownInfo(KItemNull *pItem, DWORD dwReason);
		KItemNull*  GetLastMouseMButtonDownItem();

		DWORD		GetCurrentDragStatus();
		//-----------------item事件相关--------------------

		void		SetBlurItem(KItemNull* pItem);

		int GetClientX() const;
		int GetClientY() const;
		void SetClientPosition(int nX, int nY);

		int GetWindowX() const;
		int GetWindowY() const;
		void SetWindowPosition(int nX, int nY);

		//取得界面的缩放比例，用于创建窗口，界面对脚本的接口，和脚本对界面的接口和其他地方
		int SetUIScale(float fScale, BOOL bForceUpdate = false);
		float GetUIScale() const;	
		float GetMaxUIScale() const;

		int SetStandardSize(float fW, float fH);
		float GetStandardWidth() const;
		float GetStandardHeight() const;

		int	SetClientSize(UINT uWidth, UINT uHeight);
		float GetClientWidth() const;
		float GetClientHeight() const;

		int IsFullScreen() const;
		int IsPanauision() const;
		int IsExclusiveMode() const;
		int IsClipCursor() const;

		int SetVideoSettings(KG3DDeviceSettings* pSettings);

		DWORD GetFPS() const;

		void Hide();
		void Show();
		int IsVisible();

		void SetProcessNCSizing(BOOL bProcess);
		BOOL IsProcessNCSizing();

		DWORD GetIdleTime();
		void ClearIdleTime();

		void DestoryWindow(KWndWindow * pWnd);

		bool IsInUserAction() { return m_bUserAction; };

		KWndWindow * GetMouseOverWindow();

		void AddMovingWindow(KWndWindow * pWindow);
		void RemoveMovingWindow(KWndWindow * pWindow);
	private:
		void UpdateFPS();

		void SetBlurWindow(KWndWindow* pWnd);
		
		KWndWindow* SearchChildWindow(KWndWindow *pParent, LPCSTR szSectionName);

		void		UpdateMouseOver(KWndWindow* pMouseOverWnd);

		LRESULT		PreProcessInput(UINT& uMsg, WPARAM &wParam, LPARAM &lParam);
		LRESULT		ProcessInputModeless(UINT uMsg, WPARAM wParam, LPARAM lParam);

		LRESULT		ProcessGlobalHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lProcessed);

		LRESULT		ProcessItemInvalidMouseKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam);

		LRESULT		EndProcessInput(UINT& uMsg, WPARAM &wParam, LPARAM &lParam);

		bool		AdjustFrameListPosition(KSmartIniFile& Ini, bool bRemainVisibleState);

		void OnDestory();
	private:
        KWndWindow		m_LayerRoot[WL_END];			//所有层的根

        KWndFrame*      m_pActiveFrame;					//处于激活状态的Frame

		KWndWindow*		m_pMouseOverWnd;				//鼠标正下方的第一个窗口部件
		KWndWindow*		m_pCaptureMouseWnd;				//捕获了鼠标的窗口部件

		KWndWindow*		m_pLastFocusWnd;				//当Station非激活的时候时记录下来的上次Active时的焦点窗口。

		//-----------------item事件相关--------------------
		KItemNull*		m_pLastMouseLButtonDownItem;
		KItemNull*		m_pLastMouseRButtonDownItem;
		KItemNull*		m_pLastMouseMButtonDownItem;

		DWORD			m_dwKeepLastButtonDownReason;
		DWORD			m_dwCurrentDragStatus;
		//-----------------item事件相关--------------------

		UINT			m_uMsg;		//当前信息
		WPARAM			m_wParam;	//当前信息WPARAM
		LPARAM			m_lParam;	//当前信息LPARAM
        int             m_nCursorX;
        int             m_nCursorY;

		bool			m_bEnabled;
		bool			m_bSwapMouseButton;

		bool			m_bUserAction;

		//--------------新的界面坐标系------------

		float m_fStandardWidth;		//标准宽，即我们制作时候的宽
		float m_fStandardHeight;	//标准高，即我们制作时候的高

		float m_fUIScale;				//界面的缩放比率

		float m_fClientWidth;		//屏幕宽，也就是绘制区域的宽
		float m_fClientHeight;		//屏幕高，也就是绘制区域的高

		int m_nHideUIMode; //隐藏UI模式

		int m_nClientX;
		int m_nClientY;

		int m_nWindowX;
		int m_nWindowY;

        HIMC m_hImc;

		std::string m_strTreePath;

		DWORD m_dwLastUserInputTime;

		DWORD m_dwFPS;

		BOOL m_bProcessNCSizing;
        KG3DDeviceSettings m_DeviceSettings;

		std::vector<KWndWindow *> m_aDestoryWindow;

		struct  MovingWindow
		{
			KWndWindow * pWindow;
			float fX;
			float fY;
			int nMouseX;
			int nMouseY;
		};
		std::vector<MovingWindow> m_MovingWindow;
	};

}

#endif
