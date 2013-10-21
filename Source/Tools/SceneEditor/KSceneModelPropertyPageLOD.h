#pragma once
#include "xmesh.h"
#include "SceneMeshEditor.h"
#include <set>
using namespace std;
// KSceneModelPropertyPageLOD dialog

class KSceneModelPropertyPageLOD : public CPropertyPage
{
	DECLARE_DYNAMIC(KSceneModelPropertyPageLOD)

public:
	KSceneModelPropertyPageLOD();
	virtual ~KSceneModelPropertyPageLOD();

// Dialog Data
	enum { IDD = IDD_MODELEDITOR_MESHLOD };
	void SetCurModel(KSceneModelEditor* pModelEditor,LPMODEL pCurModel);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	LPMODEL m_pCurModel;
	KSceneModelEditor* m_pCurEditor;
	float m_fWeight;
	CSpinButtonCtrl m_ctlWeight;
	CListBox        m_ctlVerticesList;
	set<DWORD>     m_setSelect;
	
public:
	void FillListBox(int nIndex = 0);
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonReset2();
	afx_msg void OnDeltaposSpinWeight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditWeight();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSelectall();
	afx_msg void OnBnClickedButtonRevsel();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonSetweight();
	afx_msg void OnLbnSelchangeListVertex();
};
