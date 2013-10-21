////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorLauncherSelectDlg.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-11-1 14:36:56
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORLAUNCHERSELECTDLG_H_
#define _INCLUDE_KSCENESFXEDITORLAUNCHERSELECTDLG_H_

////////////////////////////////////////////////////////////////////////////////

// KSceneSFXEditorLauncherSelectDlg dialog
#include "Resource.h"
#include "IEKG3DSFX.h"

class KSceneSFXEditorLauncherSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(KSceneSFXEditorLauncherSelectDlg)

public:
	KSceneSFXEditorLauncherSelectDlg(IEKG3DSFX *pSFX, CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSFXEditorLauncherSelectDlg();

// Dialog Data
	enum { IDD = IDD_SFXEDITOR_LAUNCHER_SELECT };

    std::vector<IEKG3DSFXParticleLauncher*>*  GetSelectedLauncherList();
    
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
    IEKG3DSFX *m_pSFX;
    std::vector<IEKG3DSFXParticleLauncher*>   m_vecSelectedLauncher;

    CListCtrl   m_listLauncher;

public:
    virtual BOOL OnInitDialog();
protected:
    virtual void OnOK();
};


#endif //_INCLUDE_KSCENESFXEDITORLAUNCHERSELECTDLG_H_
