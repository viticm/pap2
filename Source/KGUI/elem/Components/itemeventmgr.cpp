////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemeventmgr.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-15 13:43:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./itemeventmgr.h"
#include "../wndmessage.h"
#include "../tipcenter.h"
#include "../../common/ksmallobjectpool.h"
#include "../../script/kscriptmgr.h"
#include "../../KGUI.h"


////////////////////////////////////////////////////////////////////////////////

namespace
{
	KSmallObjectPool	g_Allocator("KItemEventMgr", sizeof(UI::KItemEventMgr));
}

namespace UI
{

void* KItemEventMgr::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KItemEventMgr::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KItemEventMgr::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KItemEventMgr::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

KItemEventMgr::KItemEventMgr()
{
	m_pLastMouseOverItem = NULL;
	m_nInLoop = false;
}

KItemEventMgr::~KItemEventMgr()
{

}

KItemEventMgr *KItemEventMgr::Create(KWndWindow *pWnd)
{
	KItemEventMgr *pMgr = new(std::nothrow)KItemEventMgr;
	if (pMgr)
	{
		if(!pMgr->ReBuild(pWnd))
		{
			SAFE_RELEASE(pMgr);
		}
	}
	return pMgr;
}

void KItemEventMgr::Release()
{
	OnMouseEnterOrLeave(0.0f, 0.0f, true);
	delete this;
}


int KItemEventMgr::ReBuild(KWndWindow *pWnd)
{
	int nResult = false;
	KItemHandle *pHandle = NULL;
	KItemArray aItem;
	KItemArray::iterator it;
	KItemArray::iterator ie;

	KG_PROCESS_ERROR(pWnd);

	pHandle = pWnd->GetItemHandle();
	KG_PROCESS_ERROR(pHandle);

	pHandle->PushAllEventItem(aItem);

	m_Event.clear();

	it = aItem.begin();
	ie = aItem.end();
	for (; it != ie; ++it)
	{
		KItemNull *pItem = (*it);
		DWORD dwEvent = pItem->GetEvent();
		for (DWORD dwE = 1; dwE < ITEM_EVENT_END; dwE <<= 1)
		{
			if (dwE & dwEvent)
			{
				switch(dwE)
				{
				case ITEM_EVENT_LBUTTONCLICK:
				case ITEM_EVENT_LBUTTONDRAG:
					if (!(dwEvent & ITEM_EVENT_LBUTTONDOWN))
						m_Event[ITEM_EVENT_LBUTTONDOWN].push_back(pItem);
					if (!(dwEvent & ITEM_EVENT_LBUTTONUP))
						m_Event[ITEM_EVENT_LBUTTONUP].push_back(pItem);
					break;
				case ITEM_EVENT_RBUTTONCLICK:
				case ITEM_EVENT_RBUTTONDRAG:
					if (!(dwEvent & ITEM_EVENT_RBUTTONDOWN))
						m_Event[ITEM_EVENT_RBUTTONDOWN].push_back(pItem);
					if (!(dwEvent & ITEM_EVENT_RBUTTONUP))
						m_Event[ITEM_EVENT_RBUTTONUP].push_back(pItem);
					break;
				case ITEM_EVENT_MBUTTONCLICK:
				case ITEM_EVENT_MBUTTONDRAG:
					if (!(dwEvent & ITEM_EVENT_MBUTTONDOWN))
						m_Event[ITEM_EVENT_MBUTTONDOWN].push_back(pItem);
					if (!(dwEvent & ITEM_EVENT_MBUTTONUP))
						m_Event[ITEM_EVENT_MBUTTONUP].push_back(pItem);
					break;
				default:
					m_Event[dwE].push_back(pItem);
					break;
				}
			}
		}
	}

	KG_PROCESS_ERROR(m_Event.size());

	if (m_pLastMouseOverItem)
	{
		KEventMap::iterator it = m_Event.find(ITEM_EVENT_MOUSEENTER);
		if (it == m_Event.end() 
			|| std::find(it->second.begin(), it->second.end(), m_pLastMouseOverItem) == it->second.end())
		{
			OnMouseEnterOrLeave(0.0f, 0.0f, true);
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KItemEventMgr::PtInEventItem(float fAbsX, float fAbsY)
{
	KEventMap::iterator itEvent = m_Event.find(ITEM_EVENT_MOUSETEST);
	if (itEvent == m_Event.end())
		return false;

	KItemArray& aItem = itEvent->second;

	KItemArray::iterator itItem = aItem.begin();
	KItemArray::iterator ieItem = aItem.end();
	for (; itItem != ieItem; ++itItem)
	{
		if ((*itItem)->PtInItem(fAbsX, fAbsY))
			return true;
	}
	return false;
}

LRESULT KItemEventMgr::EventItemProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	float fX = (float)(GET_X_LPARAM(lParam));
	float fY = (float)(GET_Y_LPARAM(lParam));

	switch(uMsg) 
	{
	case WM_LBUTTONDOWN:
		OnMouseEnterOrLeave(fX, fY, false);
		return OnLButtonDown(fX, fY);
	case WM_LBUTTONUP:
		OnMouseEnterOrLeave(fX, fY, false);
		return OnLButtonUp(fX, fY);
	case WM_LBUTTONDBLCLK:
		OnMouseEnterOrLeave(fX, fY, false);
		return OnLButtonDBClick(fX, fY);
	case WM_RBUTTONDOWN:
		OnMouseEnterOrLeave(fX, fY, false);
		return OnRButtonDown(fX, fY);
	case WM_RBUTTONUP:
		OnMouseEnterOrLeave(fX, fY, false);
		return OnRButtonUp(fX, fY);
	case WM_RBUTTONDBLCLK:
		OnMouseEnterOrLeave(fX, fY, false);
		return OnRButtonDBClick(fX, fY);
	case WM_MBUTTONDOWN:
		OnMouseEnterOrLeave(fX, fY, false);
		return OnMButtonDown(fX, fY);
	case WM_MBUTTONUP:
		OnMouseEnterOrLeave(fX, fY, false);
		return OnMButtonUp(fX, fY);
	case WM_MBUTTONDBLCLK:	
		OnMouseEnterOrLeave(fX, fY, false);
		return OnMButtonDBClick(fX, fY);
	case WM_MOUSEHOVER:
		OnMouseEnterOrLeave(fX, fY, false);
		return OnMouseHover(fX, fY);
	case WM_MOUSEMOVE:
		OnMouseEnterOrLeave(fX, fY, false);
		return OnMouseMove(fX, fY);
	case WND_M_MOUSE_ENTER:
		{
			int nX = 0;
			int nY = 0;
			g_pUI->m_Pointer.GetCursorPos(nX, nY);
			fX = (float)nX;
			fY = (float)nY; 
			OnMouseEnterOrLeave(fX, fY, false);
		}
		break;
	case WND_M_REFRESH_TIP:
		if (m_pLastMouseOverItem)
		{
			g_pUI->m_Script.FireItemEvent(m_pLastMouseOverItem, "OnItemRefreshTip");
		}
		break;
	case WND_M_MOUSE_LEAVE:
		OnMouseEnterOrLeave(0.0f, 0.0f, true);
		break;
	case WM_MOUSEWHEEL:
		{
			OnMouseEnterOrLeave(fX, fY, false);
			int  nRet = OnFireMouseWheelEvent(-(GET_Y_LPARAM(wParam)) / WHEEL_DELTA, fX, fY);
			OnMouseEnterOrLeave(fX, fY, false);
			return nRet;
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		return OnFireKeyDownEvent(wParam);
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		return OnFireKeyUpEvent(wParam);
		break;
	}

	return FALSE;
}


int KItemEventMgr::OnLButtonDown(float fX, float fY)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_LBUTTONDOWN);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);
			DWORD dwEvent = pItem->GetEvent();
			int nCaculate = true;
			int nInItem = false;

			if (dwEvent & ITEM_EVENT_LBUTTONDOWN)
			{			
				if (!pItem->PtInItem(fX, fY))
					continue;
				nCaculate = false;
				nInItem = true;
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemLButtonDown", &nRet);
			}

			int nNotifyUp = dwEvent & ITEM_EVENT_LBUTTONUP;
			dwEvent &= (ITEM_EVENT_LBUTTONCLICK | ITEM_EVENT_LBUTTONDRAG);
			if (dwEvent)
			{
				if (nCaculate)
					nInItem = pItem->PtInItem(fX, fY);
				if (nInItem)
				{
					if (nNotifyUp)
						dwEvent |= ITEM_EVENT_LBUTTONUP;
					g_pUI->m_WndStation.SetLastMouseLButtonDownInfo(pItem, dwEvent);
				}
			}
			if (nInItem)
				break;
		}
	}
	return nRet;
}

int KItemEventMgr::OnLButtonUp(float fX, float fY)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_LBUTTONUP);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);
			DWORD dwEvent = pItem->GetEvent();
			int nCaculate = true;
			int nInItem = false;
			ASSERT(pItem);

			if (dwEvent & ITEM_EVENT_LBUTTONUP)
			{			
				if (!pItem->PtInItem(fX, fY))
					continue;
				nCaculate = false;
				nInItem = true;
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemLButtonUp", &nRet);
			}

			if (dwEvent & ITEM_EVENT_LBUTTONDRAG && g_pUI->m_WndStation.GetCurrentDragStatus() & ITEM_EVENT_LBUTTONDRAG)
			{
				if (nCaculate)
					nInItem = pItem->PtInItem(fX, fY);
				nCaculate = false;
				if (nInItem)
					g_pUI->m_Script.FireItemEvent(pItem, "OnItemLButtonDragEnd", &nRet);
			}

			if (dwEvent & ITEM_EVENT_LBUTTONCLICK && g_pUI->m_WndStation.GetLastMouseLButtonDownItem() == pItem)
			{
				if (nCaculate)
					nInItem = pItem->PtInItem(fX, fY);
				if (nInItem)
                {
					g_pUI->m_Script.FireItemEvent(pItem, "OnItemLButtonClick", &nRet);
                    if (pItem->IsLink())
                    {
                        g_pUI->PushArg("arg0", pItem->GetLinkInfo());
                        g_pUI->m_Script.FireEvent("EVENT_LINK_NOTIFY", 0);
                    }
                }
			}
			if (nInItem)
				break;
		}
	}

	return nRet;
}

int KItemEventMgr::OnLButtonDBClick(float fX, float fY)
{
	int  nRet = FALSE;
	KEventMap::iterator it = m_Event.find(ITEM_EVENT_LBUTTONDBCLICK);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);
			ASSERT(pItem);

			if (pItem->PtInItem(fX, fY))
			{			
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemLButtonDBClick", &nRet);
				break;
			}
		}
	}

	return nRet;
}


int KItemEventMgr::OnRButtonDown(float fX, float fY)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_RBUTTONDOWN);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);
			DWORD dwEvent = pItem->GetEvent();
			int nCaculate = true;
			int nInItem = false;
			ASSERT(pItem);

			if (dwEvent & ITEM_EVENT_RBUTTONDOWN)
			{			
				if (!pItem->PtInItem(fX, fY))
					continue;
				nCaculate = false;
				nInItem = true;
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemRButtonDown", &nRet);
			}

			int nNotifyUp = dwEvent & ITEM_EVENT_RBUTTONUP;
			dwEvent &= (ITEM_EVENT_RBUTTONCLICK | ITEM_EVENT_RBUTTONDRAG);
			if (dwEvent)
			{
				if (nCaculate)
					nInItem = pItem->PtInItem(fX, fY);
				if (nInItem)
				{
					if (nNotifyUp)
						dwEvent |= ITEM_EVENT_RBUTTONUP;
					g_pUI->m_WndStation.SetLastMouseRButtonDownInfo(pItem, dwEvent);
				}
			}
			if (nInItem)
				break;
		}
	}

	return nRet;
}

int KItemEventMgr::OnRButtonUp(float fX, float fY)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_RBUTTONUP);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);
			DWORD dwEvent = pItem->GetEvent();
			int nCaculate = true;
			int nInItem = false;
			ASSERT(pItem);

			if (dwEvent & ITEM_EVENT_RBUTTONUP)
			{			
				if (!pItem->PtInItem(fX, fY))
					continue;
				nCaculate = false;
				nInItem = true;
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemRButtonUp", &nRet);
			}

			if (dwEvent & ITEM_EVENT_RBUTTONDRAG && g_pUI->m_WndStation.GetCurrentDragStatus() & ITEM_EVENT_RBUTTONDRAG)
			{
				if (nCaculate)
					nInItem = pItem->PtInItem(fX, fY);
				nCaculate = false;
				if (nInItem)
					g_pUI->m_Script.FireItemEvent(pItem, "OnItemRButtonDragEnd", &nRet);
			}

			if (dwEvent & ITEM_EVENT_RBUTTONCLICK && g_pUI->m_WndStation.GetLastMouseRButtonDownItem() == pItem)
			{
				if (nCaculate)
					nInItem = pItem->PtInItem(fX, fY);
				if (nInItem)
					g_pUI->m_Script.FireItemEvent(pItem, "OnItemRButtonClick", &nRet);
			}
			if (nInItem)
				break;
		}
	}

	return nRet;
}

int KItemEventMgr::OnRButtonDBClick(float fX, float fY)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_RBUTTONDBCLICK);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);

			if (pItem->PtInItem(fX, fY))
			{			
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemRButtonDBClick", &nRet);
				break;
			}
		}
	}

	return nRet;
}

////////////////////////////////////////////////////////////////////////////////////
int KItemEventMgr::OnMButtonDown(float fX, float fY)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_MBUTTONDOWN);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);
			DWORD dwEvent = pItem->GetEvent();
			int nCaculate = true;
			int nInItem = false;
			ASSERT(pItem);

			if (dwEvent & ITEM_EVENT_MBUTTONDOWN)
			{			
				if (!pItem->PtInItem(fX, fY))
					continue;
				nCaculate = false;
				nInItem = true;
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemMButtonDown", &nRet);
			}

			int nNotifyUp = dwEvent & ITEM_EVENT_MBUTTONUP;
			dwEvent &= (ITEM_EVENT_MBUTTONCLICK | ITEM_EVENT_MBUTTONDRAG);
			if (dwEvent)
			{
				if (nCaculate)
					nInItem = pItem->PtInItem(fX, fY);
				if (nInItem)
				{
					if (nNotifyUp)
						dwEvent |= ITEM_EVENT_MBUTTONUP;
					g_pUI->m_WndStation.SetLastMouseMButtonDownInfo(pItem, dwEvent);
				}
			}
			if (nInItem)
				break;
		}
	}

	return nRet;
}

int KItemEventMgr::OnMButtonUp(float fX, float fY)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_MBUTTONUP);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);
			DWORD dwEvent = pItem->GetEvent();
			int nCaculate = true;
			int nInItem = false;
			ASSERT(pItem);

			if (dwEvent & ITEM_EVENT_MBUTTONUP)
			{			
				if (!pItem->PtInItem(fX, fY))
					continue;
				nCaculate = false;
				nInItem = true;
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemMButtonUp", &nRet);
			}

			if (dwEvent & ITEM_EVENT_MBUTTONDRAG && g_pUI->m_WndStation.GetCurrentDragStatus() & ITEM_EVENT_MBUTTONDRAG)
			{
				if (nCaculate)
					nInItem = pItem->PtInItem(fX, fY);
				nCaculate = false;
				if (nInItem)
					g_pUI->m_Script.FireItemEvent(pItem, "OnItemMButtonDragEnd", &nRet);
			}

			if (dwEvent & ITEM_EVENT_MBUTTONCLICK && g_pUI->m_WndStation.GetLastMouseMButtonDownItem() == pItem)
			{
				if (nCaculate)
					nInItem = pItem->PtInItem(fX, fY);
				if (nInItem)
					g_pUI->m_Script.FireItemEvent(pItem, "OnItemMButtonClick", &nRet);
			}
			if (nInItem)
				break;
		}
	}

	return nRet;
}

int KItemEventMgr::OnMButtonDBClick(float fX, float fY)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_MBUTTONDBCLICK);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);

			if (pItem->PtInItem(fX, fY))
			{			
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemMButtonDBClick", &nRet);
				break;
			}
		}
	}

	return nRet;
}

int KItemEventMgr::OnMouseMove(float fX, float fY)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_MOUSEMOVE);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);

			if (pItem->PtInItem(fX, fY))
			{		
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemMouseMove", &nRet);
				break;
			}
		}
	}

	return nRet;
}


int KItemEventMgr::OnMouseHover(float fX, float fY)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_MOUSEHOVER);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);

			if (pItem->PtInItem(fX, fY))
			{			
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemMouseHover", &nRet);
				break;
			}
		}
	}

	return nRet;
}

int KItemEventMgr::OnFireMouseWheelEvent(int nDistance, float fX, float fY)
{
	int  nRet = FALSE;
	
	KEventMap::iterator it = m_Event.find(ITEM_EVENT_MOUSEWHEEL);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);

			if (pItem->PtInItem(fX, fY))
			{
				m_nInLoop = true;
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemMouseWheel", &nRet);
				if (nRet)
				{
					m_nInLoop = false;
					break;
				}
				m_nInLoop = false;
				it = std::find(aItem.rbegin(), aItem.rend(), pItem);
				if (it == aItem.rend())
					break;
			}
		}
	}

	return nRet;
}

int KItemEventMgr::OnFireKeyDownEvent(WPARAM wKey)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_KEYDOWN);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);
			if (pItem->IsVisibleInWnd())
			{
				m_nInLoop = true;
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemKeyDown", &nRet);
				if (nRet)
				{
					m_nInLoop = false;
					break;
				}
				m_nInLoop = false;
				it = std::find(aItem.rbegin(), aItem.rend(), pItem);
				if (it == aItem.rend())
					break;
			}
		}
	}

	return nRet;
}

int KItemEventMgr::OnFireKeyUpEvent(WPARAM wKey)
{
	int  nRet = FALSE;

	KEventMap::iterator it = m_Event.find(ITEM_EVENT_KEYUP);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);

			if (pItem->IsVisibleInWnd())
			{
				m_nInLoop = true;
				g_pUI->m_Script.FireItemEvent(pItem, "OnItemKeyUp", &nRet);
				if (nRet)
				{
					m_nInLoop = false;
					break;
				}
				m_nInLoop = false;
				it = std::find(aItem.rbegin(), aItem.rend(), pItem);
				if (it == aItem.rend())
					break;
			}
		}
	}

	return nRet;
}

int KItemEventMgr::OnMouseEnterOrLeave(float fX, float fY, int nOnlyForceLeave)
{
	KItemNull *pItemNewOver = NULL;
	KEventMap::iterator it;
	if (nOnlyForceLeave)
		goto Exit1;

	it = m_Event.find(ITEM_EVENT_MOUSEENTER);
	if (it != m_Event.end())
	{
		KItemArray &aItem = it->second;
		for (KItemArray::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it) 
		{
			KItemNull *pItem = (*it);
			if (pItem->PtInItem(fX, fY))
			{
				pItemNewOver = pItem;
				if (pItemNewOver == m_pLastMouseOverItem)
					goto Exit0;
				goto Exit1;
			}
		}
	}

Exit1:
	if (m_pLastMouseOverItem)
	{
		if (m_pLastMouseOverItem->GetTipIndex() >= 0)
			KTipCenter::GetSelf().HideTip();
		g_pUI->m_Script.FireItemEvent(m_pLastMouseOverItem, "OnItemMouseLeave");
		m_pLastMouseOverItem = NULL;
	}

	if (pItemNewOver)
	{
		pItemNewOver->ShowTip();
		g_pUI->m_Script.FireItemEvent(pItemNewOver, "OnItemMouseEnter");
		m_pLastMouseOverItem = pItemNewOver;
	}

	return TRUE;
Exit0:
	return FALSE;
}

}//namespace UI