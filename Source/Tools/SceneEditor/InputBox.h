#pragma once


int InputBox(const char szTitle[], CWnd* pParent, char szText[]);

class CInputBox : public CDialog
{
	DECLARE_DYNAMIC(CInputBox)

public:
	CInputBox(CString szInitText, CWnd* pParent = NULL);   // standard constructor
	virtual ~CInputBox();

    CString GetText();
    void GetText(char szText[]);

    CString m_szText;

// Dialog Data
	enum { IDD = IDD_INPUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
};
