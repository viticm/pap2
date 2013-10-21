#ifndef UI_ELEM_K_WND_PAGE_SET_H
#define UI_ELEM_K_WND_PAGE_SET_H

#include "./wndwindow.h"

namespace UI
{

//------------------------------------------------------------------------------------------
//	多个页面集合窗口类似于M$ Win32里面的 PropertieSheet
//------------------------------------------------------------------------------------------

class KWndCheckBox;
class KWndWindow;

class KWndPageSet : public KWndWindow
{
private:
	struct KWNDPAGECHECKBOXPAIR
	{
		KWndWindow*	    pWndPage;	//页面窗口指针
		KWndCheckBox*	pWndCheckBox;	//页面窗口关联按钮指针
	};
	
	typedef std::vector<KWNDPAGECHECKBOXPAIR> KWndPageCheckBoxArray;

public:
	KWndPageSet();
	~KWndPageSet();

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
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);//窗口函数
	virtual int	CanCastTo(LPCSTR szType) const;
	virtual LPCSTR GetWndType() const;

	int ActivePage(int nPageIndex);	//激活指定的页面
	int ActivePage(KWndWindow* pWndPage);	
	int ActivePage(KWndCheckBox* pWndCheckBox);

	int	GetActivePageIndex() const;	//取得当前激活页面的索引
	int GetLastActivePageIndex() const;
	int AddPage(KWndWindow* pWndPage, KWndCheckBox* pWndCheckBox);	//添加页面
	int RemovePage(KWndWindow* pWndPage);
	int RemovePage(KWndCheckBox *pWndCheckBox);
	int RemovePage(int nPageIndex);

	KWndWindow* GetActivePage(); //激活指定的页面
	KWndCheckBox* GetActiveCheckBox();

protected:
	virtual void OnShow();
	virtual void OnHide();
	virtual void OnPaint();

private:
	void OnPageBtnClick(KWndWindow* pCheckBox);		//响应按下切换页面按钮的操作
	int GetPageIndex(KWndWindow* pWndPage);
	int GetPageIndex(KWndCheckBox* pWndCheckBox);

	int IsPageExist(KWndWindow* pWndPage, KWndCheckBox* pWndCheckBox);

private:
	KWndPageCheckBoxArray m_aPageCheckBox;

	int	m_nAcitvePage;				//当前激活的页面
	int	m_nLastAcitvePage;			//前一个激活页
};


inline int KWndPageSet::GetActivePageIndex() const 
{
	return m_nAcitvePage; 
}

inline int KWndPageSet::GetLastActivePageIndex() const
{
	return m_nLastAcitvePage;
}

}

#endif // UI_ELEM_K_WND_PAGE_SET_H
