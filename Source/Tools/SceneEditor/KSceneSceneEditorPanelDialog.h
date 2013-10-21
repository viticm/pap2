#if !defined(AFX_KSCENESCENEEDITORPANELDIALOG_H__2ACD6324_AACC_4690_A187_04DC9DCFC4A5__INCLUDED_)
#define AFX_KSCENESCENEEDITORPANELDIALOG_H__2ACD6324_AACC_4690_A187_04DC9DCFC4A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSceneEditorPanelDialog.h : header file
//
#include "KSceneSceneEditorDialogTerrian.h"
#include "KSceneSceneEditorDialogOranment.h"
#include "KSceneSceneEditorDialogDistrict.h"
#include "KSceneSceneEditorDialogCamera.h"
#include "KSceneSceneEditorDialogGrass.h"
#include "KSceneSceneEditorDialogWater.h"
#include "KSceneSceneEditorWaterPanel.h"
#include "KSceneSceneEditorDialogMisc.h"///
#include "KSceneSceneEditorDialogLogical.h"
#include "KSceneSceneEditorDialogPostRender.h"
#include "KSceneSceneEditorDialogNPC.h"
#include "KSceneSceneEditorDialogDoodad.h"
#include "KSceneSceneEditorDialogWayPoint.h"
#include "KSceneSceneEditorDialogGroup.h"
#include "KSceneSceneEditorDialogAIGroup.h"
#include "KSceneSceneEditorDialogNickName.h"
#include "KSceneSceneEditorDialogOption.h"
#include "KSceneSceneEditorDialogMovie.h"
#include "KSceneSceneEditorDialogPoly.h"
#include "KSceneSceneEditorDialogTerrianCopy.h"
#include "KSceneSceneEditorDialogLight.h"
#include "KGSceneEditorPvsPane.h"
#include "KSceneSceneEditorDialogTrafficPoint.h"

//////
//////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorPanelDialog dialog
//class KG3DSceneSceneEditor;
class KSceneEditorDocLogical;
interface IEKG3DSceneSceneEditor;//by dengzhihui Refactor 2006年11月27日

class KSceneSceneEditorPanelDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorPanelDialog)
	friend class KSceneEditorView;
// Construction
public:
	KSceneSceneEditorPanelDialog(CWnd* pParent = NULL);   // standard constructor
	~KSceneSceneEditorPanelDialog();
	struct DialogInfo
	{
		CDialog* pDialog;
		TCHAR    Title[256];
		int            Index;
		DialogInfo(CDialog* pDlg,LPSTR pTitle)
		{
			pDialog = pDlg;
			wsprintf(Title,"%s",pTitle);
			Index = 0;
		}
	};
	vector<DialogInfo>m_vecDialog;
// Dialog Data
	//{{AFX_DATA(KSceneSceneEditorPanelDialog)
	enum { IDD = IDD_SCENEEDITORMAINPANEL };
	CComboBox	m_ComboBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSceneEditorPanelDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KSceneSceneEditorPanelDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void MoveAllWindow();
	void AddToComboBox();
    void ShowDialog(LPCTSTR strName);
	void ShowDialog(int nIndex);

    void UpdateUI();

protected:
	KSceneSceneEditorDialogTerrian        m_DialogTerrian;
	KSceneSceneEditorDialogOranment   m_DialogOranment;
	KSceneSceneEditorDialogDistrict        m_DialogDistrict;
	KSceneSceneEditorDialogCamera       m_DialogCamera;
	KSceneSceneEditorDialogGrass           m_DialogGrass;
	KSceneSceneEditorWaterPanel           m_DialogWater;
	KSceneSceneEditorDialogMisc             m_DialogMisc;
	KSceneSceneEditorDialogLogical         m_DialogLogical;
	KSceneSceneEditorDialogPostRender m_DialogPostRender;
	KSceneSceneEditorDialogNPC              m_DialogNPC;
	KSceneSceneEditorDialogDoodad           m_DialogDoodad;
	KSceneSceneEditorDialogWayPoint			m_DialogWayPoint;
	KSceneSceneEditorDialogGroup			m_DialogGroup;
	KSceneSceneEditorDialogNickName			m_DialogNickName;
	KSceneSceneEditorDialogOption			m_DialogOption;
    KSceneSceneEditorDialogMovie            m_DialogMovie;
	KSceneSceneEditorDialogPoly             m_DialogPoly;
	KSceneSceneEditorDialogTerrianCopy      m_DialogTCopy;
	KSceneSceneEditorDialogLight			m_DialogLight;
    KGSceneEditorPvsPane                    m_DialogPvs;
	KSceneSceneEditorDialogTrafficPoint		m_DialogTrafficPoint;

public:

	void SetSceneEditor(IEKG3DSceneSceneEditor* pEditor,KSceneEditorDocLogical* pLogical,KSceneEditorDoc* pDoc);
	IEKG3DSceneSceneEditor			*m_lpSceneEditor;
	//(by dengzhihui 2006年8月4日 13:26:36
	void SetDocument(CDocument* pDoc, BOOL bSave);
	//)
protected:
	virtual void OnOK();
	virtual void OnCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESCENEEDITORPANELDIALOG_H__2ACD6324_AACC_4690_A187_04DC9DCFC4A5__INCLUDED_)
