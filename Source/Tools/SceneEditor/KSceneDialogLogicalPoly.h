#pragma once

#include "IEEditor.h"
#include "afxwin.h"
// KSceneDialogLogicalPoly dialog

class KSceneDialogLogicalPoly : public CDialog
{
	DECLARE_DYNAMIC(KSceneDialogLogicalPoly)

public:
	KSceneDialogLogicalPoly(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneDialogLogicalPoly();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOGICALPOLY };

	IEKG3DRepresentPoly* m_lpPoly;
	HRESULT SetPoly(IEKG3DRepresentPoly* pPoly)
	{
		m_lpPoly = pPoly;
		return S_OK;
	}
	struct LogicalSet 
	{
		DWORD dwColor;
		INT   nID;
		string szName;
		string szScriptFile;
		DWORD dwModifyState;
	};
	vector<LogicalSet>m_vecLogicalSet;

	HRESULT AddLogicalSet(DWORD dwColor,INT nID,LPCTSTR pName,LPCTSTR pScriptName,DWORD dwModifyState);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int FindLogicalSetByName(DWORD& dwModify,LPSTR pScriptName,LPSTR pName);

	virtual BOOL OnInitDialog();
	CComboBox m_ComBo;
	CString m_szName;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnEnChangeEdit1();
	float m_fHeight;
	afx_msg void OnEnChangeEdit7();
};
