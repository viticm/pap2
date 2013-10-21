#pragma once



class KPvsEditorToolSheet;
class IEKG3DScenePvsEditor;

class KPvsEditorToolView : public CFormView
{
	DECLARE_DYNCREATE(KPvsEditorToolView)

public :
    IEKG3DScenePvsEditor* GetPvsScene();
    void OnInitialUpdate();

protected:
	KPvsEditorToolView();           // protected constructor used by dynamic creation
	virtual ~KPvsEditorToolView();

public:
	enum { IDD = IDD_KPVSEDITORTOOLVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected :
    KPvsEditorToolSheet*  m_pSheet;
    IEKG3DScenePvsEditor* m_pPvsScene;


protected :
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    afx_msg void OnSize(UINT nType, int cx, int cy);
};


