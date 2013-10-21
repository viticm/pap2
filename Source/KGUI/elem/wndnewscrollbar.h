////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : wndnewscrollbar.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-2 10:25:47
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_WNDNEWSCROLLBAR_H_
#define _INCLUDE_WNDNEWSCROLLBAR_H_

////////////////////////////////////////////////////////////////////////////////

#include "./wndwindow.h"
#include "./wndbutton.h"


namespace UI
{

#define WNDNEWSCROLL_VERTICAL			          0x0001		//垂直型的滚动条
#define WNDNEWSCROLL_AUTO_HIDE_SLIDE	          0x0002		//是否自动隐藏滑动按钮
#define WNDNEWSCROLL_AUTO_RESIZE_SLIDE            0x0004		//滚动条的滑块会根据滚动范围自动改变大小
#define	WNDNEWSCROLL_DRAGGING_SLIDE		          0x0008		//滚动条的滑块正在被拖动

#define WNDNEWSCROLL_MIN_STEP                     0.00001f

class KWndNewScrollBar : public KWndWindow
{

public:
	KWndNewScrollBar();

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
    static int  Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex);
    virtual int Init(Lua_State *pLua, int nTableIndex);
#endif

public:
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);	
	virtual int	CanCastTo(LPCSTR szType) const;
	virtual LPCSTR GetWndType() const;


    int SetScrollPos(int nScrollPos);     
    int GetScrollPos();

    int SetStepCount(int nStepCount);
    int GetStepCount();

    int ScrollPrev(int nStep = 1);          
    int ScrollNext(int nStep = 1);          

    int ScrollHome();
    int ScrollEnd();

    int ScrollPagePrev();
    int ScrollPageNext();

    int SetPageStepCount(int nStepCount);
    int GetPageStepCount();

	void BindSlideButton(KWndButton* pButton);

	BOOL IsStatus(DWORD dwStatus) const;
	void SetStatus(DWORD dwStatus, BOOL bEnable);

protected:
	virtual void	            OnPaint();	

private:
    int         UpdateScrollPos();
    int         UpdateScrollSize();

    int         OnLButtonDown(float fX, float fY);
    int         OnDraggingSlide(float fX, float fY);
    int         OnSlideBtnPressed(float x, float y);

private:
	DWORD       m_dwStatus;

    float       m_fDragPosOffset;
    int         m_nPageStepCount; //每页可显示的步数

    int         m_nTotalStepCount; //总步数
    int         m_nScrollPos;      //当前步数

	KWndButton* m_pSlideButton;	    //滑动按钮
};

inline BOOL KWndNewScrollBar::IsStatus(DWORD dwStatus) const
{
	return (m_dwStatus & dwStatus) != 0; 
}

} //namespace UI

#endif //_INCLUDE_WNDNEWSCROLLBAR_H_