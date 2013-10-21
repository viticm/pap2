////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorTrackPage.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-12-1 16:08:35
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORTRACKPAGE_H_
#define _INCLUDE_KSCENESFXEDITORTRACKPAGE_H_

////////////////////////////////////////////////////////////////////////////////
#include "KSceneSFXEditorDoc.h"
#include "KEnterEdit.h"
#include "Resource.h"
#include "KGLilisPanel.h"

// KSceneSFXEditorTrackPage dialog

class KSceneSFXEditorTrackPage : public CDialog
{
	DECLARE_DYNAMIC(KSceneSFXEditorTrackPage)

public:
	KSceneSFXEditorTrackPage(CWnd* pWnd = NULL);
	virtual ~KSceneSFXEditorTrackPage();

// Dialog Data
	enum { IDD = IDD_SFXEDITOR_TRACK };

    void SetSFXEditorDoc( KSceneSFXEditorDoc* pDoc )
    {
        ASSERT( pDoc );
        m_pDoc = pDoc;
    }

    void UpdateUI();

    void UpdateBindInfo();
    void UpdateTrackUI();
    void UpdateLauncherUI();
    void UpdateBillboardUI();
    void UpdateModelUI();
    void UpdateMeteorUI();

private:
    KSceneSFXEditorDoc *m_pDoc;

    KEnterEdit  m_editTrackX;
    KEnterEdit  m_editTrackY;
    KEnterEdit  m_editTrackZ;

    KEnterEdit  m_editTrackInterval;
    KEnterEdit  m_editTrackFrameNum;

    CListCtrl   m_listTrack;
    CListCtrl   m_listLauncher;
    CListCtrl   m_listBillboard;
    CListCtrl   m_listModel;
    CListCtrl   m_listMeteor;

    CComboBox   m_comboBindMode;

    KGLilisButton     m_button1;
    KGLilisButton     m_button2;
    KGLilisButton     m_button3;
    KGLilisButton     m_button4;
    KGLilisButton     m_button5;
    KGLilisButton     m_button6;
    KGLilisButton     m_button7;
    KGLilisButton     m_button8;
    KGLilisButton     m_button9;
    KGLilisButton     m_button10;
    KGLilisButton     m_button11;

    void OnSetTrackX();
    void OnSetTrackY();
    void OnSetTrackZ();

    void OnSetInterval();

    BOOL BrowseTrackFileName(CString *pStrName);

    void OnOK(){};
    void OnCancel(){};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedButtonAddTrack();
    afx_msg void OnBnClickedButtonDelTrack();
    virtual BOOL OnSetActive();
    afx_msg void OnNMRclickListTrack(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonLauncherBindToTrack();
    afx_msg void OnNMClickListTrack(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonEditTrack();
    afx_msg void OnBnClickedButtonLauncherBindCancel();
    afx_msg void OnCbnSelchangeComboBindMode();
    afx_msg void OnNMClickListLauncher(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonBillboardBindToTrack();
    afx_msg void OnBnClickedButtonBillboardBindCancel();
    afx_msg void OnBnClickedButtonModelBindToTrack();
    afx_msg void OnBnClickedButtonModelBindCancel();
    afx_msg void OnBnClickedButtonMeteorBindToTrack();
    afx_msg void OnBnClickedButtonMeteorBindCancel();
};


#endif //_INCLUDE_KSCENESFXEDITORTRACKPAGE_H_
