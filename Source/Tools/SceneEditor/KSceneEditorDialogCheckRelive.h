#ifndef _INCLUDE_KSCENEEDITORDIALOGCHECKRELIVE_H_
#define _INCLUDE_KSCENEEDITORDIALOGCHECKRELIVE_H_
#include "afxwin.h"
#include "afxcmn.h"
#include "KSceneEditorDocLogical.h"

class IEKG3DSceneSceneEditor;

class KSceneEditorDialogCheckRelive : public CDialog
{
//	DECLARE_DYNAMIC(KSceneEditorReviveDialog)

protected:
	KSceneEditorDocLogical* m_lpDocLogical;
	IEKG3DSceneSceneEditor* m_pSceneEditor;
	BOOL					m_bReliveEnable;
public:
	KSceneEditorDialogCheckRelive(CWnd* pParentWnd = NULL);
	~KSceneEditorDialogCheckRelive();

	CTreeCtrl		m_Tree;
	enum			{ IDD = IDD_DIALOG_CHECKRELIVE };
	

	void		SetCurrentDir();
	void		SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEditor);
	void		SetDocLogical(KSceneEditorDocLogical* lpDocLogical);
	BOOL		OnInitDialog();
	BOOL		IsLoseRelive();		

public:
	static char* s_szCurrentDir;
	static KSceneEditorDialogCheckRelive* This;
	int GetCheckReliveEnable();
	int SetCheckReliveEnable(bool bEnable);
	static int GetCheckReliveEnableFromFile();
	static int SetCheckReliveEnableToFile();
protected :

	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnBnClickedButtonRecheck();
    afx_msg void OnBnClickedButtonIgnore();
	afx_msg void OnBnClickedButtonCheckEnable();
	afx_msg void OnNMClkTree(NMHDR *pNMHDR, LRESULT *pResult);
	HRESULT LoadTree();
};
#endif