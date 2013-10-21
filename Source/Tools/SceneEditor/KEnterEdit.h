

#ifndef _INCLUDE_KENTEREDIT_H_
#define _INCLUDE_KENTEREDIT_H_

#include "afxwin.h"

#define WM_EDIT_RECEIVE_ENTER WM_USER + 100

class KEnterEdit : public CEdit
{
public :
    KEnterEdit();
    virtual ~KEnterEdit();

protected :
    DECLARE_MESSAGE_MAP()
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public :

private :
    bool m_bTextChanged;
    CSpinButtonCtrl m_Spin;
};


#endif //_INCLUDE_KENTEREDIT_H_
