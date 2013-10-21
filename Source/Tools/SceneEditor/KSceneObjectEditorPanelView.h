
#ifndef _KSCENEOBJECTEDITORPANELVIEW_H_
#define _KSCENEOBJECTEDITORPANELVIEW_H_

#include "KSceneObjectEditorPropertySheet.h"
#include "KGPropertyEditTabDlg.h"
#include "KGObjectPropertyEditDlg.h"
#include "KG3DObjectEditor.h"

class KSceneObjectEditorPanelView : public CFormView
{
protected:
	KSceneObjectEditorPanelView();          
	DECLARE_DYNCREATE(KSceneObjectEditorPanelView)

public:
	enum { IDD = IDD_OBJECTEDITORMAINPANEL };
	//KSceneObjectEditorPropertySheet*  m_pPropertySheet;

public:
	KG3DObjectEditor* m_pObjectDlg;
	KGObjectPropertyEditDlg* m_pNpcDlg;
	KGObjectPropertyEditDlg* m_pPropDlg;
	KGObjectPropertyEditDlg* m_pMagicDlg;
	KGObjectPropertyEditDlg* m_pEffectDlg;
	KGObjectPropertyEditDlg* m_pSkillDlg;
	KGObjectPropertyEditDlg* m_pPlayerAinDlg;
	KGObjectPropertyEditDlg* m_pAniDlg;
	KGObjectPropertyEditDlg* m_pEquipDlg;

	KGObjectPropertyEditTabDlg* m_pTaseDlg;

	CButton m_bnObject;

public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);   

protected:
	virtual ~KSceneObjectEditorPanelView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnObjectClick();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedNpc();
	afx_msg void OnBnClickedProp();
	afx_msg void OnBnClickedMagic();
	afx_msg void OnBnClickedEffect();
	afx_msg void OnBnClickedSkill();
	afx_msg void OnBnClickedEquip();
	afx_msg void OnBnClickedPlayeranimatiom();
	afx_msg void OnBnClickedNpcanimatiom();

	afx_msg void OnBnClickedTase(); 

};

#endif // _KSCENEOBJECTEDITORPANELVIEW_H_