#pragma once

#include "KGLilisSlipPanel.h"
// KGSfxEditorEditGeoPane dialog

class KGSfxEditorEditGeoPane : public KGLilisSlipPanel<CPropertyPage>
{
	DECLARE_DYNAMIC(KGSfxEditorEditGeoPane)

public:
	KGSfxEditorEditGeoPane();
	virtual ~KGSfxEditorEditGeoPane();

// Dialog Data
	enum { IDD = IDD_SFX_EDIT_GEO_PANEL };

    int  OnSetActive();
    void OnOK(){};
    void OnCancel(){};
    BOOL OnInitDialog();
    int  UpdateUI();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    
	DECLARE_MESSAGE_MAP()
    afx_msg void OnGeoCommand(UINT uId);
};
