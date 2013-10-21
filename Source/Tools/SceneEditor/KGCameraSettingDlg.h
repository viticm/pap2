#pragma once
//#include "KG3DTrackCamera.h"//by dengzhihui Refactor 2006年11月6日
#include "IEOther.h"//by dengzhihui Refactor 2006年11月6日
#include "SceneEditorView.h"

class KGCameraSettingDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(KGCameraSettingDlg)

public :
	KGCameraSettingDlg();
	virtual ~KGCameraSettingDlg();
	enum { IDD = IDD_CAMERA_SETTING };
	BOOL OnInitDialog();
	int  Update();
    void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

public :
	IEKG3DCameraStation* m_pCameraStation;	//by dengzhihui Refactor 2006年11月6日

    // 追尾
	CButton					   m_comboAways;
	CButton					   m_comboNoUse;
    CButton                    m_comboAptit;
    int                        m_nChaseRate;

    int                        m_nflexCoefficient;
    int                        m_nDampCoefficient;

    int                        m_nMaxAngelVel;
    int                        m_nMinAngelVel;

    int		                   m_nAngelRateHor;		
    int		                   m_nAngelRateVel;		

    int		                   m_nMaxDistance;

    int                        m_nMouseWeel;
    // 遮挡
    CButton                    m_combObstrt;




protected :
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedCheck0();
	afx_msg void OnBnClickedCheck1();
    afx_msg void OnBnClickedCheck2();
};
