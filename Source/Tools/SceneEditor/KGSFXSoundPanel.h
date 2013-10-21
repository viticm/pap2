#pragma once


#include "KEnterEdit.h"
#include "KGLilisPanel.h"
#include "afxwin.h"

class KGSFXSoundPanel : public CDialog
{
	DECLARE_DYNAMIC(KGSFXSoundPanel)

public:
	KGSFXSoundPanel(CWnd* pParent = NULL);   
	virtual ~KGSFXSoundPanel();

	enum { IDD = IDD_SFXEDITOR_SOUND };

    void UpdateUI();
    void UpdateInfo();
    BOOL OnSetActive();
    BOOL OnInitDialog();
    void Modify();
    void OnOK(){}
    void OnCancel(){};

    BOOL PreTranslateMessage(MSG* pMsg);

    KGLilisButton m_buttonAdd;
    KGLilisButton m_buttonDel;
    KGLilisButton m_buttonEdit;

    FLOAT   m_fVolume;
    FLOAT   m_fSFXKey;
    FLOAT   m_fMinDis;
    FLOAT   m_fMaxDis;

    int     m_bLoop;
    int     m_bContinue;

    CComboBox m_Mode;
    CString   m_EditFileName;
   

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
    afx_msg void OnButtonAdd();
    afx_msg void OnButtonDel();
    afx_msg void OnButtonAddNull();
    afx_msg void OnCbnSelchangeComboMode();
    afx_msg void OnBnClickedCheckLoop();
    afx_msg void OnBnClickedCheckContinue();
public:
    CListBox m_ListBoxSound;
    afx_msg void OnLbnSelchangeListSound();
    afx_msg void OnLbnDblclkListSound();
    int m_nProbability;
};
