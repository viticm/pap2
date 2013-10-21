////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXGlobPage.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-12 11:44:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSFXGLOBPAGE_H_
#define _INCLUDE_KGSFXGLOBPAGE_H_

#include "KGLilisPanel.h"
#include "KColorDialog.h"
#include "KGLilisSlipPanel.h"
#include "kenteredit.h"

class KGSFXGlobPage : public KGLilisSlipPanel<CPropertyPage>
{
	DECLARE_DYNAMIC(KGSFXGlobPage)

public :
	KGSFXGlobPage();
	virtual ~KGSFXGlobPage();
	enum { IDD = IDD_DIALOG_MODEL_GLOB };
    void Init();

public :
    KGLilisButton  m_bottonReSetFX;
    KGLilisButton  m_bottonBkColor;
	KGLilisButton  m_bottonBKTexture;
    BOOL  m_bSockWave;
    BOOL  m_bSockWaveAdd;
	BOOL  m_bCloseFloor;
    BOOL  m_bScalable;
    BOOL  m_bLockY;
	BOOL  m_bShowBGTexture;
    CTreeCtrl   m_tree;

protected :
	virtual void DoDataExchange(CDataExchange* pDX);   
    int OnSetActive();
    
    void OnOK(){};
    void OnCancel(){};
    BOOL OnInitDialog();
    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnColorBkDialogClose(WPARAM w, LPARAM l);
    afx_msg void OnBnClickReSetFX();
    afx_msg void OnBnClickedCheckWave();
    afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMSetfocusTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCheckCloseFloor();
    afx_msg void OnCheckLockY();
public:
    afx_msg void OnBnClickedBkcolor();
    afx_msg void OnBnClickedScanlDepand();
	afx_msg void OnBnClickedBkTexture();
	afx_msg void OnBnClickedBgtexture();
    afx_msg void OnBnClickedCheckWaveAdd();
    afx_msg void OnBnClickedCheckGround();
    KEnterEdit m_EditMaxParticleNum;
    KEnterEdit m_EditWavePoser;
};

#endif //_INCLUDE_KGSFXGLOBPAGE_H_
