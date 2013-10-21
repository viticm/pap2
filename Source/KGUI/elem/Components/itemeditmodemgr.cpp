////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemeditmodemgr.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-22 15:18:44
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./item.h"
#include "./itemeditmodemgr.h"

////////////////////////////////////////////////////////////////////////////////

#ifndef DISABLE_UI_EDIT

namespace UI
{

KItemEditModeMgr *KItemEditModeMgr::ms_pSelf = NULL;

KItemEditModeMgr::KItemEditModeMgr()
{
    m_pCurrentEditItem = NULL;
    m_nDragMode = false;
    m_nResizeMode = false;
	m_fDragStartX = 0.0f;
	m_fDragStartY = 0.0f;
}

KItemEditModeMgr::~KItemEditModeMgr()
{

}

KItemNull * KItemEditModeMgr::GetCurrentEditItem()
{
    return m_pCurrentEditItem;
}

bool KItemEditModeMgr::Init()
{
	ASSERT(ms_pSelf == NULL);

	ms_pSelf = new(std::nothrow) KItemEditModeMgr;
	KGLOG_PROCESS_ERROR(ms_pSelf);

	return true;
Exit0:
	Exit();
	return false;
}

void KItemEditModeMgr::Exit()
{
	delete ms_pSelf;
}

KItemEditModeMgr& KItemEditModeMgr::GetSelf()
{
	ASSERT(ms_pSelf);
	return *ms_pSelf;
}

LRESULT KItemEditModeMgr::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_LBUTTONUP:
        ClearCurrentEditItem();
        return FALSE;
    case WM_MOUSEMOVE:
        if (m_pCurrentEditItem)
            return m_pCurrentEditItem->WndProc(uMsg, wParam, lParam);
    }
    return FALSE;
}

int KItemEditModeMgr::SetCurrentEditItem(KItemNull *pItem)
{
    m_pCurrentEditItem = pItem;
    return true;
}

int KItemEditModeMgr::ClearCurrentEditItem()
{
    m_pCurrentEditItem = NULL;
    return true;
}

int KItemEditModeMgr::SetDragStartPos(float fX, float fY)
{
    m_fDragStartX = fX;
    m_fDragStartY = fY;
    return true;
}

int KItemEditModeMgr::GetFragStartPos(float &fX, float &fY)
{
    fX = m_fDragStartX;
    fY = m_fDragStartY;
    return true;
}

int KItemEditModeMgr::SetEditMode(int nMode)
{
    if (nMode == EDIT_DRAG_MODE) //Drag
    {
        m_nDragMode = true;
        m_nResizeMode = false;
    }
    else if (nMode == EDIT_RESIZE_MODE)
    {
        m_nDragMode = false;
        m_nResizeMode = true;
    }
    else
    {
        m_nDragMode = false;
        m_nResizeMode = false;
    }
    return true;
}

int KItemEditModeMgr::GetEditMode()
{
    if (m_nDragMode)
        return EDIT_DRAG_MODE;
    if (m_nResizeMode)
        return EDIT_RESIZE_MODE;
    return EDIT_NOT_DRAG_AND_NOT_RESIZE_MODE;
}

} // namespace UI

#endif