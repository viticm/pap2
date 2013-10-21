////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGLilisSlipPanel.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-4-21 14:35:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGLilisSlipPanel.h"
#include "assert.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


inline BOOL equelrect(RECT r1, RECT r2)
{
    if (
        (r1.left    == r2.left)  &&
        (r1.top     == r2.top)   &&
        (r1.right   == r2.right) &&
        (r1.bottom  == r2.bottom)
        )
        return TRUE;
    else
        return FALSE;
}

inline void _ScreenToClient(HWND hWnd, LPRECT Rect)
{
    POINT Point1 = {Rect->left, Rect->top};
    POINT Point2 = {Rect->right, Rect->bottom};

    ::ScreenToClient(hWnd, &Point1);
    ::ScreenToClient(hWnd, &Point2);

    Rect->left   = Point1.x;
    Rect->top    = Point1.y;
    Rect->right  = Point2.x;
    Rect->bottom = Point2.y;
}


KGLilisVirtualWnd::KGLilisVirtualWnd(HWND hDependWnd)
{
    ASSERT(hDependWnd);
    m_hDependWindow = hDependWnd;
    
    m_Rect.left     = INT_MAX;
    m_Rect.top      = INT_MAX;
    m_Rect.right    = INT_MIN;
    m_Rect.bottom   = INT_MIN;
}

KGLilisVirtualWnd::~KGLilisVirtualWnd()
{

}

void KGLilisVirtualWnd::GetWindowRect(LPRECT lpRect) const
{
    lpRect->left       = m_Rect.left;
    lpRect->top        = m_Rect.top;
    lpRect->right      = m_Rect.right;
    lpRect->bottom     = m_Rect.bottom;
}

int KGLilisVirtualWnd::ShowWindow(int nCmdShow)
{
    for (size_t s = 0; s < m_vecElements.size(); s++)
        ::ShowWindow(m_vecElements[s], nCmdShow);
    return TRUE;
}

int KGLilisVirtualWnd::IsGetFocus()
{
    for (size_t s = 0; s < m_vecElements.size(); s++)
    {
        if (::GetFocus() == m_vecElements[s])
            return TRUE;
    }
    return FALSE;
}

int KGLilisVirtualWnd::DrawWindow()
{
    for (size_t s = 0; s < m_vecElements.size(); s++)
        ::InvalidateRect(m_vecElements[s], NULL, TRUE);
    return TRUE;
}

int KGLilisVirtualWnd::AjustFrame()
{
    size_t size = m_vecElements.size();

    m_vecEleRates.clear();

    for (size_t i = 0; i < size; i++)
    {
        RECT rectElement;
        RATE rectEOffset;

        ::GetWindowRect(m_vecElements[i], &rectElement);
        _ScreenToClient(m_hDependWindow, &rectElement);

        float height = (float)m_Rect.bottom - m_Rect.top;
        float width  = (float)m_Rect.right  - m_Rect.left;

        rectEOffset.left   = (rectElement.left - m_Rect.left) / width;
        rectEOffset.right  = (m_Rect.right - rectElement.right) / width;
        rectEOffset.top    = (rectElement.top - m_Rect.top) / height;
        rectEOffset.bottom = (m_Rect.bottom - rectElement.bottom) / height;

        m_vecEleRates.push_back(rectEOffset);
    }

    return true;
}

int KGLilisVirtualWnd::AddElement(HWND hElement)
{
    ASSERT(hElement);

    if (hElement && m_hDependWindow)
    {
        m_vecElements.push_back(hElement);

        RECT rectElement;
        
        ::GetWindowRect(hElement, &rectElement);
        _ScreenToClient(m_hDependWindow, &rectElement);

        m_Rect.bottom = max(m_Rect.bottom, rectElement.bottom);
        m_Rect.top    = min(m_Rect.top,    rectElement.top);
        m_Rect.left   = min(m_Rect.left,   rectElement.left);
        m_Rect.right  = max(m_Rect.right,  rectElement.right);

        return TRUE;
    }
    else
        return FALSE;
}

int KGLilisVirtualWnd::MoveWindow(LPCRECT lpRect)
{
    if (equelrect(*lpRect, m_Rect) && !m_hDependWindow)
        return TRUE;

    for (size_t s = 0; s < m_vecElements.size(); s++)
    {
        RECT rectWnd;
        int witdh      = lpRect->right - lpRect->left;
        int height     = lpRect->bottom - lpRect->top;
        rectWnd.left   = (LONG)(lpRect->left + witdh    * m_vecEleRates[s].left);
        rectWnd.right  = (LONG)(lpRect->right - witdh   * m_vecEleRates[s].right);
        rectWnd.top    = (LONG)(lpRect->top + height    * m_vecEleRates[s].top);
        rectWnd.bottom = (LONG)(lpRect->bottom - height * m_vecEleRates[s].bottom);
        ::MoveWindow(
            m_vecElements[s], 
            rectWnd.left, 
            rectWnd.top,
            rectWnd.right - rectWnd.left,
            rectWnd.bottom - rectWnd.top,
            TRUE
            );
    }

    m_Rect.left     = lpRect->left;
    m_Rect.top      = lpRect->top;
    m_Rect.right    = lpRect->right;
    m_Rect.bottom   = lpRect->bottom;

    return TRUE;
}
