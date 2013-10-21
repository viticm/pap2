#pragma once
#include <XTToolkitPro.h> 


struct NormalMapParam
{
    int np[8];  // norm map param
    int sp[8];  // spec map param
    int dp[8];  // disp map param
    int ap[8];  // ambo map param
};

class KGNormalMapCtreatorSetting : public CDialog
{
	DECLARE_DYNAMIC(KGNormalMapCtreatorSetting)

public:
	KGNormalMapCtreatorSetting(NormalMapParam param, CWnd* pParent = NULL);   // standard constructor
	virtual ~KGNormalMapCtreatorSetting();

	enum { IDD = IDD_DIALOG_NORMAL_CTEATOR_SETTING };

    BOOL OnInitDialog();

    NormalMapParam GetParams() const 
    {
        return m_Params;
    }

    NormalMapParam m_Params;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    int m_nNp0;
    int m_nNp1;
    int m_nSp0;
    int m_nSp1;
    int m_nSp2;
    int m_nDp0;
    int m_nDp1;
    int m_nDp2;
    int m_nAp0;
    int m_nAp1;
    int m_nAp2;
    int m_nAp3;
    int m_nAp4;
    int m_nAp5;
    afx_msg void OnBnClickedButtonDef();
    CXTHyperLink m_HelpLink;
};
