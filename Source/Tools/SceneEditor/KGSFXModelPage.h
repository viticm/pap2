#pragma once
#include "KSceneSFXEditorModel.h"

// KGSFXModelPage dialog

class KGSFXModelPage : public CPropertyPage
{
	DECLARE_DYNAMIC(KGSFXModelPage)

public:
	KGSFXModelPage();
	virtual ~KGSFXModelPage();
    KSceneSFXEditorModel         m_page;
    KGLilisPanel                 m_panel;
    int SetSFXEditorDoc(KSceneSFXEditorDoc* pDoc)
    {
        m_page.SetSFXEditorDoc(pDoc);
        return TRUE;
    }
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
	enum { IDD = IDD_DIALOG_MODEL };

protected:
    BOOL OnInitDialog();
    void OnOK(){};
    void OnCancel(){};
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
