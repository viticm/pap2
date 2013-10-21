/*****************************************************************************************
//	剑侠引擎，界面窗口体系结构的最基本窗口对象
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-9
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef UI_ELEM_K_WND_WINDOW_H
#define UI_ELEM_K_WND_WINDOW_H


#ifdef KGUI_LUA_LOADER
#include "./uiobject.h"
#endif

//============窗口风格================
#define WND_S_VISIBLE				0x80000000	//可见
#define WND_S_AREA_BY_EVENT_ITEM    0x40000000  //由ItemHandle中的注册的事件的item决定其范围
#define	WND_S_TOPMOST				0x20000000	//置顶窗口
#define	WND_S_MOVEABLE				0x10000000	//可被拖动
#define WND_S_MOUSE_PENETRABLE      0x08000000  //鼠标完全穿透，不接收任何消息
#define	WND_S_DISABLE				0x04000000	//窗口不可被操作
#define	WND_S_TOBEDESTROY			0x02000000	//window is to be destroy
#define	WND_S_SIZE_WITH_ALL_CHILD	0x00100000	//以子窗口的范围测试的集合作为自己的范围测试的集合
#define WND_S_SELF_HOLD_MOUSE_HOVER 0x00080000
#define WND_S_DISABLE_BRING_TO_TOP  0x00040000  //禁止bringtotop
#define WND_S_EVENT_LIST_CHANGED    0x00020000  //事件列表发生了改变
#define WND_S_DRAW_LIST_CHANGED     0x00010000  //绘制列表发生了改变
#define WND_S_BREAHTE_WHEN_HIDE     0x00008000  //即使隐藏页调用breathe

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

namespace UI
{
	enum { WND_TYPE_NAME_LEN = 32 };
	enum { WND_SECTION_NAME_LEN = 64 };

	enum 
    {
        WND_DO_NOT_DESTROY_MY_BROTHER = 0,
        WND_DESTROY_MY_BROTHER = 1,
    };

    enum
    {
        WND_SIDE_BEGIN,

        WND_SIDE_TOP		= WND_SIDE_BEGIN,
        WND_SIDE_BOTTOM,
        WND_SIDE_LEFT,
        WND_SIDE_RIGHT,
        WND_SIDE_TOPLEFT,
        WND_SIDE_TOPRIGHT,
        WND_SIDE_BOTTOMLEFT,
        WND_SIDE_BOTTOMRIGHT,
        WND_SIDE_CENTER,
        WND_SIDE_LEFTCENTER,
        WND_SIDE_RIGHTCENTER,
        WND_SIDE_TOPCENTER,
        WND_SIDE_BOTTOMCENTER,

        WND_SIDE_END,
    };

    class KItemHandle;
    class KItemNull;
    class KItemEventMgr;

    class KWndWindow 
#ifdef KGUI_LUA_LOADER
		: public IKUIObject
#endif
    {
    private:
        typedef std::vector<KItemNull*> KItemList;

    public:
        KWndWindow();
        virtual	~KWndWindow();

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

#ifdef KGUI_LUA_LOADER
    public:
        static int   Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex);
        virtual void Release();
        virtual int  Init(Lua_State *pLua, int nTableIndex);
        virtual int  AddChild(IKUIObject *pChild);
#endif

    public:
        static int GetSide(LPCSTR szSide);

        virtual int	PtInWindow(int x, int y);

        virtual void	            Paint();												//绘制
        virtual void	            PaintDebugInfo();
        virtual void	            SetSize(float fWidth, float fHeight);					//设置窗口大小
        virtual	void	            Enable(int bEnable);									//禁止或者允许使窗口被操作
        virtual int 	            Scale(float fScaleX, float fScaleY);					//放大缩小
        virtual KWndWindow*	        TopChildFromPoint(int x, int y);						//得到处于指定坐标位置的最上层窗口
        virtual LRESULT             WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);		//窗口函数
        virtual int					CanCastTo(LPCSTR szType) const;
        virtual LPCSTR				GetWndType() const;

        virtual int					SetAlpha(int nAlpha);
        int							SetSelfAlpha(int nAlpha);
        int							GetAlpha();

        void	                    Destroy(int nDestroyMyBrother);
        void             			BringToTop();											//把窗口置顶
        void 			            GetSize(float *pWidth, float *pHeight) const;			//获取窗口大小
        void			            SetCursorAbove();										//使鼠标指针以移动到悬浮在此窗口中的位置上
        BOOL				        IsVisible() const;										//判断窗口是否被显示
        BOOL				        IsDisable() const;
        BOOL			            IsStyle(DWORD dwStyle) const;
        void			            SetStyle(DWORD dwStyle, BOOL bEnable);					//修改窗口风格
        void			            Breathe();												//让窗口活动
        void						Show();													//显示窗口
        void						Hide();													//隐藏窗口
        void			            SplitSmallFamily();										//把自己（及子窗口）从窗口树里面里面分离出来
        void			            LeaveAlone();											//世间再无窗在我左右，一无牵连

		float GetRelX() const;
		float GetRelY() const;

		float GetAbsX() const;
		float GetAbsY() const;

		void SetRelPos(float fX, float fY);	//设置窗口位置，相对坐标
        void SetAbsPos(float fX, float fY);	//设置窗口位置，绝对坐标

        virtual void			    AbsMove(float fXOffset, float fYOffset);				//绝对坐标的调整

        int 			            AddChild(KWndWindow *pChild);							//添加子窗口
        int 			            AddBrother(KWndWindow *pBrother, int nJustBehindMe = false);						//添加兄弟窗口
        void 			            GetAllChildLayoutRect(float *pfLeft, float *pfTop, float *pfRight, float* pfBottom) const;		//取得包含所有子窗口分布区域的最小区域
        KWndWindow*		            GetPrev() const;										//得到前一个兄弟窗口
        KWndWindow*		            GetNext() const;										//得到后一个兄弟窗口
        KWndWindow*		            GetParent() const;										//得到父窗口
        KWndWindow*		            GetFirstChild() const;									//得到第一个子窗口
        KWndWindow*		            GetOwner();												//获得不再有父窗口的祖先窗口
        void                        SetName(LPCSTR szName);
        LPCSTR                      GetName() const;

        int                         SetItemHandle(KItemHandle *pItemHandle);
        int                         ReleaseItemHandle();
        KItemHandle*                GetItemHandle() const;

        void						UpdateEvent();
        void						OnItemRelease(KItemNull *pItem);

        void						UpdateDrawList();

		void SetTip(int nTipIndex, int nShowTipType, int nRichText);
		void SetAreaTestFile(LPCSTR szFilePath);

        int	GetLuaRef();
        void SetLuaRef(int nRef);

        bool SetPoint(float fX, float fY);
        bool SetPoint(int nSide, float fX, float fY, KWndWindow const* pRelWnd, int nRelSide, float fRelOffsetX, float fRelOffsetY);

        int CreateItemHandle(IIniFile* pIni, LPCSTR pszSectionName);

        int UpdateLevel(int nLevel);

    protected:
        virtual void				OnDestroy();
        virtual void	            OnBreathe();											//窗口的持续行为
        virtual void	            OnPaint();												//窗体绘制
        virtual void	            OnShow();												//显示窗口
        virtual void	            OnHide();												//隐藏窗口
        virtual void				OnSize();
        virtual void				OnScale(float fScaleX, float fScaleY);
        virtual void				OnMove();
        void						EventCheck();
        void						DrawCheck();

    private:
        BOOL PtInThisWindow(float x, float y) const;

#ifndef DISABLE_UI_EDIT
        LRESULT						EditModeProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

    protected:
        float		                m_fRelX;			//左上角X坐标，相对于父窗口
        float		                m_fRelY;			//左上角Y坐标，相对于父窗口
        float		                m_fAbsX;			//窗口左上角的绝对坐标x
        float		                m_fAbsY;			//窗口左上角的绝对坐标y
        float		                m_fWidth;			//宽度
        float		                m_fHeight;			//高度

        DWORD						m_dwAlpha;			//Alpha

        char                        m_szName[64];

    private:
        DWORD		                m_dwStyle;			//窗口风格,见Wnd.h中窗口风格的定义

		KWndWindow*	                m_pPrev;			//前一个同级窗口
        KWndWindow*	                m_pNext;			//后一个同级窗口
        KWndWindow*	                m_pFirstChild;		//第一个子窗口
        KWndWindow*	                m_pParent;			//父窗口

        KItemHandle*				m_pItemHandle;      //挂接ItemHandle
        KItemEventMgr*				m_pEventMgr;
        KItemList	                m_aDrawItem;

        int							m_nTipIndex;
        int							m_nAreaIndex;

        int							m_nLuaRef;	//在lua中的引用

        int                         m_nLevel;
    };


    //----------------------------------------------------------------------------------------------------------
    inline BOOL KWndWindow::IsStyle(DWORD dwStyle) const
    { 
        return (m_dwStyle & dwStyle) != 0; 
    }

    inline KWndWindow* KWndWindow::GetPrev() const 
    { 
        return m_pPrev; 
    }

    inline KWndWindow* KWndWindow::GetNext() const 
    { 
        return m_pNext; 
    }

    inline KWndWindow* KWndWindow::GetParent() const
    { 
        return m_pParent; 
    }

    inline KWndWindow* KWndWindow::GetFirstChild() const
    { 
        return m_pFirstChild; 
    }

    inline KItemHandle* KWndWindow::GetItemHandle() const
    {
        return m_pItemHandle;
    }

    inline int KWndWindow::GetLuaRef()
    {
        return m_nLuaRef;
    }

    inline void KWndWindow::SetLuaRef(int nRef)
    {
        m_nLuaRef = nRef;
    }

    inline void KWndWindow::UpdateEvent()
    {
        SetStyle(WND_S_EVENT_LIST_CHANGED, TRUE);
    }

    inline void KWndWindow::UpdateDrawList()
    {
        SetStyle(WND_S_DRAW_LIST_CHANGED, TRUE);
    }

    void* CastWndWindow(KWndWindow* pWnd, LPCSTR szType);

} // namespace UI

#endif // UI_ELEM_K_WND_WINDOW_H

