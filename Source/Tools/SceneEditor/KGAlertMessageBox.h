#pragma once
#include "afxwin.h"
#include "resource.h"

// KGAlertMessageBox dialog



class KGAlertMessageBox : public CDialog
{
	DECLARE_DYNAMIC(KGAlertMessageBox)

private :
	KGAlertMessageBox(CWnd* pParent = NULL);   // standard constructor

public :
    virtual ~KGAlertMessageBox();


    static KGAlertMessageBox m_sAlertMessageBox;

// Dialog Data
	enum { IDD = IDD_DIALOG_ALERTBOX };

    void Clear();
    void AddMessage(const char strMessage[]);
    int  Size();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CListBox m_ListMessage;
    afx_msg void OnBnClickedOk();
};

extern KGAlertMessageBox& g_AlertMessageBox;
