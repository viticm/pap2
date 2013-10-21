////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemeventmgr.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-15 13:43:59
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEMEVENTMGR_H_
#define _INCLUDE_ITEMEVENTMGR_H_

////////////////////////////////////////////////////////////////////////////////

#include "itemhandle.h"
#include "../wndwindow.h"

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

#define ITEM_EVENT_LBUTTONDOWN      0x00000001
#define ITEM_EVENT_RBUTTONDOWN      0x00000002
#define ITEM_EVENT_LBUTTONUP        0x00000004
#define ITEM_EVENT_RBUTTONUP        0x00000008
#define ITEM_EVENT_LBUTTONCLICK     0x00000010
#define ITEM_EVENT_RBUTTONCLICK     0x00000020
#define ITEM_EVENT_LBUTTONDBCLICK   0x00000040
#define ITEM_EVENT_RBUTTONDBCLICK   0x00000080
#define ITEM_EVENT_MOUSEENTER       0x00000100
#define ITEM_EVENT_MOUSETEST		0x00000200
#define ITEM_EVENT_MOUSEMOVE        0x00000400
#define ITEM_EVENT_MOUSEWHEEL       0x00000800
#define ITEM_EVENT_KEYDOWN          0x00001000
#define ITEM_EVENT_KEYUP            0x00002000
#define ITEM_EVENT_MBUTTONDOWN		0x00004000
#define ITEM_EVENT_MBUTTONUP		0x00008000
#define ITEM_EVENT_MBUTTONCLICK		0x00010000
#define ITEM_EVENT_MBUTTONDBCLICK	0x00020000
#define ITEM_EVENT_MOUSEHOVER		0x00040000
#define ITEM_EVENT_LBUTTONDRAG		0x00080000
#define ITEM_EVENT_RBUTTONDRAG		0x00100000
#define ITEM_EVENT_MBUTTONDRAG		0x00200000

#define ITEM_EVENT_END				0x00400000

//下面的是辅助用的，不需要注册事件
#define ITEM_EVENT_MOUSELEAVE       0x10000000
#define ITEM_EVENT_LBUTTONDRAGEND	0x20000000
#define ITEM_EVENT_RBUTTONDRAGEND	0x40000000
#define ITEM_EVENT_MBUTTONDRAGEND	0x80000000

class KItemEventMgr
{
	typedef std::vector<KItemNull *> KItemArray;
	typedef std::map<DWORD, KItemArray> KEventMap;
public:
	static KItemEventMgr *Create(KWndWindow *pWnd);
	void Release();

	int ReBuild(KWndWindow *pWnd);
	void ForceLeave(KItemNull *pItem);

	LRESULT	EventItemProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	int PtInEventItem(float fAbsX, float fAbsY);

private:
	int	OnFireKeyDownEvent(WPARAM wKey);
	int	OnFireKeyUpEvent(WPARAM wKey);
	int	OnMouseEnterOrLeave(float fX, float fY, int nOnlyForceLeave);
	int	OnFireMouseWheelEvent(int nDistance, float fX, float fY);

	int	OnLButtonDown(float fX, float fY);
	int	OnLButtonUp(float fX, float fY);
	int	OnLButtonDBClick(float fX, float fY);
	int	OnRButtonDown(float fX, float fY);
	int	OnRButtonUp(float fX, float fY);
	int	OnRButtonDBClick(float fX, float fY);
	int	OnMButtonDown(float fX, float fY);
	int	OnMButtonUp(float fX, float fY);
	int	OnMButtonDBClick(float fX, float fY);
	int	OnMouseMove(float fX, float fY);
	int	OnMouseHover(float fX, float fY);

public:
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

protected:
	KItemEventMgr();
	virtual ~KItemEventMgr();

private:
	KEventMap m_Event;
	KItemNull *m_pLastMouseOverItem;
	int m_nInLoop;
};

inline void KItemEventMgr::ForceLeave(KItemNull *pItem)
{
	if (m_pLastMouseOverItem && pItem == m_pLastMouseOverItem)
	{
		OnMouseEnterOrLeave(0.0f, 0.0f, true);
	}

	if (m_nInLoop && pItem)
	{
		DWORD dwEvent = pItem->GetEvent();
		if (dwEvent & ITEM_EVENT_KEYDOWN)
		{
			KEventMap::iterator it = m_Event.find(ITEM_EVENT_KEYDOWN);
			if (it != m_Event.end())
			{
				KItemArray &aItem = it->second;
				for (KItemArray::iterator it = aItem.begin(); it != aItem.end(); ++it)
				{
					if (*it == pItem)
					{
						aItem.erase(it);
						break;
					}
				}
			}
		}
		if (dwEvent & ITEM_EVENT_KEYUP)
		{
			KEventMap::iterator it = m_Event.find(ITEM_EVENT_KEYUP);
			if (it != m_Event.end())
			{
				KItemArray &aItem = it->second;
				for (KItemArray::iterator it = aItem.begin(); it != aItem.end(); ++it)
				{
					if (*it == pItem)
					{
						aItem.erase(it);
						break;
					}
				}
			}
		}
		if (dwEvent & ITEM_EVENT_MOUSEWHEEL)
		{
			KEventMap::iterator it = m_Event.find(ITEM_EVENT_MOUSEWHEEL);
			if (it != m_Event.end())
			{
				KItemArray &aItem = it->second;
				for (KItemArray::iterator it = aItem.begin(); it != aItem.end(); ++it)
				{
					if (*it == pItem)
					{
						aItem.erase(it);
						break;
					}
				}
			}
		}
	}
}


}//namespace UI


#endif //_INCLUDE_ITEMEVENTMGR_H_
