#pragma once



#include "KGPropertySheet.h"
#include "KPvsPlacePanel.h"
#include "KPvsAttriPanel.h"

class IEKG3DScenePvsEditor;

class KPvsEditorToolSheet : public KGPropertySheet
{
	DECLARE_DYNAMIC(KPvsEditorToolSheet)

public :
    IEKG3DScenePvsEditor* GetPvsScene();
	KPvsEditorToolSheet(CWnd* pParentWnd);
	virtual ~KPvsEditorToolSheet();

protected :
    KPvsPlacePanel        m_PlacePanel;
    KPvsAttriPanel        m_AttriPanel;

protected:
	DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};


