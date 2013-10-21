#ifndef UI_ELEM_K_WND_CHECKBOX_H
#define UI_ELEM_K_WND_CHECKBOX_H

#include "./wndwindow.h"

#define WNDCHECKBOX_CHECKED             0x00000001
#define WNDCHECKBOX_ENABLE              0x00000002
#define WNDCHECKBOX_MOUSEOVER           0x00000004
#define WNDCHECKBOX_PRESSED		        0x00000020
#define WNDCHECKBOX_DRAG_WIDTH_LBUTTON  0x00000040
#define WNDCHECKBOX_DRAG_WIDTH_RBUTTON  0x00000080
#define WNDCHECKBOX_RADIO_BUTTON		0x00000100

namespace UI
{

	// TODO: ·Ö¶à×é
enum WNDCHECKBOX_STATUS
{
    WNDCHECKBOX_STATUS_BEGIN = 0,
    WNDCHECKBOX_UNCHCKED_AND_ENABLE = WNDCHECKBOX_STATUS_BEGIN,
    WNDCHECKBOX_CHCKED_AND_ENABLE,
    WNDCHECKBOX_UNCHCKED_AND_DISABLE,
    WNDCHECKBOX_CHCKED_AND_DISABLE,
	WNDCHECKBOX_NOW_CHECKING,
	WNDCHECKBOX_NOW_UNCHECKING,
    WNDCHECKBOX_CHECKED_AND_ENABLE_MOUSEOVER,
    WNDCHECKBOX_UNCHECKED_AND_ENABLE_MOUSEOVER,
    WNDCHECKBOX_CHECKED_AND_DISABLE_MOUSEOVER,
    WNDCHECKBOX_UNCHECKED_AND_DISABLE_MOUSEOVER,
    WNDCHECKBOX_STATUS_END,
};

class KWndCheckBox : public KWndWindow
{
public:
    KWndCheckBox();
    virtual ~KWndCheckBox();

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
    virtual int                 PtInWindow(int x, int y);
    virtual LRESULT             WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual int					CanCastTo(LPCSTR szType) const;
	virtual LPCSTR				GetWndType() const;

    int                         IsCheckBoxChecked();
    int                         IsCheckBoxActive();
    int                         EnableCheck(int nEnableFlag);
    int                         Check(int nCheckFlag);

	void                        RegisterLButtonDrag();
	void                        UnregisterLButtonDrag();
	int                         IsLButtonDragable();

	void                        RegisterRButtonDrag();
	void                        UnregisterRButtonDrag();
	int                         IsRButtonDragable();

	BOOL IsStatus(DWORD dwStatus) const;
	void SetStatus(DWORD dwStatus, BOOL bEnable);

	void SetAnimation(int nUnCheckAndEnable, int nCheckAndEnable, int nUnCheckAndDisable, int nCheckAndDisable, 
		int nChecking, int nUnChecking, int nCheckedAndEnableWhenMouseOver, int nUncheckedAndEnableWhenMouseOver, 
		int nCheckedAndDisableWhenMouseOver, int nUncheckedAndDisableWhenMouseOver, LPCSTR szImagePath);

	void SetFontScheme(int nCheckFont, int nUncheckFont, int nMouseDownFont, int nMouseOverFont, int nDisableFont);

    int UpdataAnimateGroup();

protected:
    virtual void	            OnPaint();

private:
    int                         OnLButtonDown();
    int                         OnLButtonUp(int x, int y);
	int							UpdataOverTextShow();

private:

    int                         m_nAnimateGroup[WNDCHECKBOX_STATUS_END];
	int                         m_nOverTextFont[WNDCHECKBOX_STATUS_END];
    WNDCHECKBOX_STATUS          m_eCurrentGroupIndex;

    DWORD                       m_dwStatus; 
    int                         m_nAnimateIndex;
};

inline BOOL KWndCheckBox::IsStatus(DWORD dwStatus) const
{
	return (m_dwStatus & dwStatus) != 0; 
}

}

#endif