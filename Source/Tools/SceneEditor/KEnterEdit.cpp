

#include "StdAfx.h"
#include "KEnterEdit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KEnterEdit::KEnterEdit()
    : m_bTextChanged(false)
{

}

KEnterEdit::~KEnterEdit()
{

}

BEGIN_MESSAGE_MAP(KEnterEdit, CEdit)

END_MESSAGE_MAP()


BOOL KEnterEdit::PreTranslateMessage(MSG *pMsg)
{
    if ((pMsg->message == WM_KEYDOWN && VK_RETURN == (UINT)pMsg->wParam))
    {
        CString strText;
        GetWindowText(strText);
        if (::IsWindow(GetParent()->GetSafeHwnd()))
            GetParent()->SendMessage(WM_EDIT_RECEIVE_ENTER, GetDlgCtrlID(), (LPARAM)&strText);
        m_bTextChanged = false;
        return TRUE;
    }

    return CEdit::PreTranslateMessage(pMsg);
}



LRESULT KEnterEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_KILLFOCUS && m_bTextChanged)
    {
        CString strText;
        GetWindowText(strText);
        if (::IsWindow(GetParent()->GetSafeHwnd()))
            GetParent()->SendMessage(WM_EDIT_RECEIVE_ENTER, GetDlgCtrlID(), (LPARAM)&strText);
        m_bTextChanged = false;
    }

    if (message == WM_KEYUP)
    {
        m_bTextChanged = true;
    }

    return CEdit::WindowProc(message, wParam, lParam);
}
