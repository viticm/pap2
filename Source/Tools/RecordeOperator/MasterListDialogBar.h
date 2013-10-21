#pragma once
#include "KGMemDateList.h"

// MasterListDialogBar dialog

class MasterListDialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(MasterListDialogBar)

public:
	MasterListDialogBar();   // standard constructor
	virtual ~MasterListDialogBar();
//	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
// Dialog Data
	enum { IDD = IDD_MASTERLIST_DIALOGBAR };

	CTreeCtrl m_treeMasters;
	KGMemDateList m_listCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LONG OnInitDialog ( UINT, LONG );   
	DECLARE_MESSAGE_MAP()
};
