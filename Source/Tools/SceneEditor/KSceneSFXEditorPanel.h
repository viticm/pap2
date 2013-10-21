////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorPanel.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:12:45
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORPANEL_H_
#define _INCLUDE_KSCENESFXEDITORPANEL_H_

////////////////////////////////////////////////////////////////////////////////
#include "Resource.h"
#include "KSceneSFXEditorPropertySheet.h"

class KSceneSFXEditorPanel : public CDialog
{
	DECLARE_DYNCREATE(KSceneSFXEditorPanel)

public:
	KSceneSFXEditorPanel();          
	virtual ~KSceneSFXEditorPanel();

    DWORD    m_dwWndState;

public:
    KSceneSFXEditorPropertySheet* m_lpPropertySheet;

	enum { IDD = IDD_SFXEDITOR_TOOLVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    BOOL PreTranslateMessage(MSG* pMsg);
    //LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    int SetDependWnd(CView* pDependWnd);
    CView* m_pDependWnd;

    DWORD UpdateWndState();
    void UpdateUI()
    {
        if( m_lpPropertySheet )
            m_lpPropertySheet->UpdateUI();
    }

    int GetActivePageIndex()
    {
        return m_lpPropertySheet->GetActiveIndex();
    }

    void SetActivePage(int nIndex)
    {
        m_lpPropertySheet->SetActivePage(nIndex);
    }

    KSceneSFXEditorParticlePage* GetParticlePage()
    {
        return m_lpPropertySheet->GetParticlePage();
    }

    KSceneSFXEditorBillboardPage* GetBillboardPage()
    {
        return m_lpPropertySheet->GetBillboardPage();
    }

    KSceneSFXEditorModel* GetModelPage()
    {
        return m_lpPropertySheet->GetModelPage();
    }

    KSceneSFXEditorTrackPage* GetTrackPage()
    {
        return m_lpPropertySheet->GetTrackPage();
    }

    KSceneSFXEditorMeteor* GetMeteorPage()
    {
        return m_lpPropertySheet->GetMeteorPage();
    }

    KGSFXBindPage*  GetBindPage()
    {
        return m_lpPropertySheet->GetBindPage();
    }

    KSceneSFXEditChannel* GetChannelPage()
    {
        return m_lpPropertySheet->GetChannelPage();
    }

    KGSFXSoundPanel* GetSoundPage()
    {
        return m_lpPropertySheet->GetSoundPage();
    }

    void OnOK(){};
    void OnCancel(){};
    void OnLButtonDblClick();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	BOOL OnInitDialog();
};




#endif //_INCLUDE_KSCENESFXEDITORPANEL_H_
