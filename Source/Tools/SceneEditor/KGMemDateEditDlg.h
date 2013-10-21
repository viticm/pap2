


#include "KGMemDateList.h"

class KGMemDateEditDlg : public CDialog
{
	DECLARE_DYNAMIC(KGMemDateEditDlg)

public:
	KGMemDateEditDlg(
		BYTE* pDate, KGMEMDATEELEMENT element[], 
		INT nWitdhLeft = 180, INT nWitdhRigh = 500,
		CWnd* pParent = NULL
	);   // standard constructor

	virtual ~KGMemDateEditDlg();
	enum { IDD = IDD_INI_EDIT1 };

    BOOL OnInitDialog();

public :
    BYTE* m_pDateBuffer;
    KGMEMDATEELEMENT* m_pElement;
    KGMemDateList m_list;

	INT m_nWitdhLeft;
	INT m_nWitdhRigh;
	
	CString m_cTitleName;
	void SetTitleName(LPCTSTR title);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOK();

};
