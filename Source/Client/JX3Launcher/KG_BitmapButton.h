////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_BitmapButton.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-6  16:17:43
//  Comment     : refer : http://www.codeproject.com/KB/buttons/cxskinbutton.aspx
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_BITMAP_BUTTON_H_
#define _KG_BITMAP_BUTTON_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Post message to the parent item if mouse enter or leave button.
#define MY_MSG_MOUSE_IN_BUTTON      WM_USER + 100
#define MY_MSG_MOUSE_LEAVE_BUTTON   WM_USER + 101

////////////////////////////////////////////////////////////////////////////////////////////////////
class KG_BitmapButton : public CButton
{
public:
    KG_BitmapButton();
    ~KG_BitmapButton();

    BOOL LoadBitmaps(UINT nNormal, UINT nDown = 0, UINT nOver = 0);
    COLORREF SetTextColor(COLORREF NewColor);
private:
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void    OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL    OnClicked();
    afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
    afx_msg void    OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()

private:
    CBitmap         m_bmpNormal;
    CBitmap         m_bmpDown;
    CBitmap         m_bmpOver;
    COLORREF        m_TextColor;
    int             m_nMouseInFlag;
    int             m_nButtonDownFlag;
};

#endif  // _KG_BITMAP_BUTTON_H_