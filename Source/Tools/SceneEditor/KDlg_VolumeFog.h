#pragma once


// KDlg_VolumeFog dialog
extern void DecToHex(unsigned int DecNum, string& sHexNum);
extern unsigned int HexToDec(const string& HexNum);

class KDlg_VolumeFog : public CPropertyPage
{
	DECLARE_DYNAMIC(KDlg_VolumeFog)

protected:
	//KVolumeFog* m_pVF;

	string m_Filename;
	unsigned int m_Color;
	float m_XSpan, m_ZSpan;
	float m_ViewOffset;
	float m_DistFP;
	

public:
	KDlg_VolumeFog();
	virtual ~KDlg_VolumeFog();

	void Create();
	void Show();

	//void SetVolumeFog(KVolumeFog* pVF);

// Dialog Data
	enum { IDD = IDD_DIALOG_VOLUMEFOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnCancelClicked();
	afx_msg void OnResetClicked();
	afx_msg void OnOkClicked();
	afx_msg void OnColorChanged();
	afx_msg void OnColorTableClicked();
	afx_msg void OnDensityChanged();
	afx_msg void OnXSpanChanged();
	afx_msg void OnZSpanChanged();
	afx_msg void OnViewOffsetChanged();
	afx_msg void OnDistFPChanged();
	afx_msg void OnSaveClicked();
};
