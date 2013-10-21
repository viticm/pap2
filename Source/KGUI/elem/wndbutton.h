#ifndef UI_ELEM_K_WND_BUTTON_H
#define UI_ELEM_K_WND_BUTTON_H

#include "./wndwindow.h"

#define WNDBUTTON_MOUSE_DOWN				0x00000001 
#define WNDBUTTON_ENABLE					0x00000002
#define WNDBUTTON_MOUSE_OVER				0x00000004  
#define WNDBUTTON_PRESSED					0x00000008  
#define WNDBUTTON_DRAG_WIDTH_LBUTTON		0x00000010  
#define WNDBUTTON_DRAG_WIDTH_RBUTTON		0x00000020
#define WNDBUTTON_ON_DRAGING_WIDTH_LBUTTON  0x00000040
#define WNDBUTTON_ON_DRAGING_WIDTH_RBUTTON  0x00000080

namespace UI
{
enum BUTTON_STATUS
{
    BUTTON_STATUS_BEGIN = 0,
    BUTTON_STATUS_NORMAL = BUTTON_STATUS_BEGIN,
    BUTTON_STATUS_MOUSEOVER,
    BUTTON_STATUS_MOUSEDOWN,
    BUTTON_STATUS_DISABLE,
    BUTTON_STATUS_END,
    BUTTON_STATUS_UNKNOWN = BUTTON_STATUS_END,
};


class KWndButton : public KWndWindow
{
public:
    KWndButton();
    virtual ~KWndButton();

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
    static  int Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex);
    virtual int Init(Lua_State *pLua, int nTableIndex);
#endif

public:
    virtual int                 PtInWindow(int x, int y);
    virtual LRESULT             WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual int					CanCastTo(LPCSTR szType) const;
	virtual LPCSTR				GetWndType() const;

	void SetFont(int nNormal, int nMouseOver, int nMouseDown, int nDisable);
	void SetAnimation(int nNormal, int nMouseOver, int nMouseDown, int nDisable, LPCSTR szImagePath);
    void SetAnimateGroupNormal(int nGroup);
    void SetAnimateGroupMouseOver(int nGroup);
    void SetAnimateGroupMouseDown(int nGroup);
    void SetAnimateGroupDisable(int nGroup);

	BOOL IsStatus(DWORD dwStatus) const;
	void SetStatus(DWORD dwStatus, BOOL bEnable);

	int IsButtonEnabled() const;
    int EnableButton(int nEnabled);

    void SetButtonDownState();
    void ClearButtonDownstate();

    void RegisterLButtonDrag();
    void UnregisterLButtonDrag();
    int IsLButtonDragable();

    void RegisterRButtonDrag();
    void UnregisterRButtonDrag();
    int IsRButtonDragable();

    int UpdataAnimateGroup(int nForceToUpData = false);

protected:
    virtual void	            OnPaint();

private:
    int                         OnLButtonDown(LPARAM lParam);
    int                         OnLButtonUp(LPARAM lParam);
	int                         OnRButtonDown(LPARAM lParam);
	int                         OnRButtonUp(LPARAM lParam);
    int                         OnMouseEnter();
    int                         OnMouseLeave();
	int							UpdataOverTextShow();

private:
    int                         m_nAnimateGroup[BUTTON_STATUS_END];
	int                         m_nOverTextFrame[BUTTON_STATUS_END];
    BUTTON_STATUS               m_eCurrentGroupIndex;

    DWORD                       m_dwStatus;  
    int                         m_nAnimateIndex;
};


inline BOOL KWndButton::IsStatus(DWORD dwStatus) const
{
	return (m_dwStatus & dwStatus) != 0; 
}


} // namespace UI

#endif   //UI_ELEM_K_WND_BUTTON_H