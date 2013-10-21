#pragma once
//这个类没有检查到任何引用 Last modify by Chen Tianhong: 2007-11-21 11:02:51


// KDlg_Lightning dialog

class KDlg_Lightning : public CPropertyPage
{
	DECLARE_DYNAMIC(KDlg_Lightning)

public:
	KDlg_Lightning();
	virtual ~KDlg_Lightning();

	void Create();
	//void SetLightning(KLightning* pL);
	void ResetParameters();

// Dialog Data
	enum { IDD = IDD_DIALOG_LIGHTNING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//KLightning* m_pL;
	float m_Length, m_Width;
	unsigned int m_FrameTime;
	unsigned char m_TFactorAlpha;
	string m_TexName;
	IDirect3DTexture9* m_pOldTex;
	IDirect3DTexture9* m_pNewTex;
	string m_TexFile;


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLengthChanged();
	afx_msg void OnWidthChanged();
	afx_msg void OnFrametimeChanged();
	afx_msg void OnTfactorChanged();
	afx_msg void OnTextureChanged();
	afx_msg void OnLoadClicked();
	afx_msg void OnCancelClicked();
	afx_msg void OnResetClicked();
	afx_msg void OnSaveClicked();
	afx_msg void OnOkClicked();
	afx_msg void OnClose();
};
