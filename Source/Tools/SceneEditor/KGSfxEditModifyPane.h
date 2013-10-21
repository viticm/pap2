#pragma once


#include "KGLilisSlipPanel.h"
#include "KGSfxTreeCtrl.h"

class KGSfxEditModifyPane : public KGLilisSlipPanel<CPropertyPage>
{
	DECLARE_DYNAMIC(KGSfxEditModifyPane)

public:
	KGSfxEditModifyPane();
	virtual ~KGSfxEditModifyPane();

	enum { IDD = IDD_SFX_EDIT_MODIFY };

    KGSfxTreeCtrl   m_Tree;

    int  OnSetActive();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
