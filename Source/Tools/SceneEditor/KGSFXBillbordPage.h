#pragma once
#include "KSceneSFXEditorBillboardPage.h"

// KGSFXBillbordPage dialog

class KGSFXBillbordPage : public CPropertyPage
{
	DECLARE_DYNAMIC(KGSFXBillbordPage)

public:
	KGSFXBillbordPage();
	virtual ~KGSFXBillbordPage();

public :
    KSceneSFXEditorBillboardPage m_page;
    KGLilisPanel                 m_panel;
    virtual BOOL OnSetActive()
    {
        if (::IsWindow(m_page.m_hWnd))
            m_page.OnSetActive();
        return TRUE;
    }
    int UpdateUI()
    {
        if (::IsWindow(m_page.m_hWnd))
            m_page.UpdateUI();
        return TRUE;
    };

// Dialog Data
	enum { IDD = IDD_DIALOG_BILLBORD };

protected:
    BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
