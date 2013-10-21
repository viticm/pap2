#pragma once
#include "afxwin.h"

// KDlg_TextureStageState dialog



class KDlg_TextureStageState : public CDialog
{
public:
	struct TextureStageStateRestore
	{
		DWORD        ColorOp;
		DWORD        ColorArg1;
		DWORD        ColorArg2;

		DWORD        AlphaOp;
		DWORD        AlphaArg1;
		DWORD        AlphaArg2;
	};

	struct Operation
	{
		D3DTEXTUREOP opOperation;
		TCHAR strDesc[256];
	};

	struct Arg
	{
		DWORD dwArg;
		TCHAR strDesc[256];
	};

	DECLARE_DYNAMIC(KDlg_TextureStageState)

public:
	KDlg_TextureStageState(CWnd* pParent = NULL);   // standard constructor
	
	virtual ~KDlg_TextureStageState();

// Dialog Data
	enum { IDD = IDD_DIALOG_TEXTURESTAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRestore();
	void Init(DWORD *pdwTextureIDs, CWnd* pParent);
	TextureStageStateRestore m_RestoreData[8];
	//KG3DMaterial::KG3DMaterialSubset* m_pMaterial;
    DWORD *m_pdwTextureIDs;
	int m_nNumTexture;
	
public:
	virtual BOOL OnInitDialog();
	CComboBox m_ctlColorOp;
	CComboBox m_ctlColorArg1;
	CComboBox m_ctlColorArg2;
	CComboBox m_ctlAlphaOp;
	CComboBox m_ctlAlphaArg1;
	CComboBox m_ctlAlphaArg2;
	CComboBox m_ctlStage;

	void FillLists();


};


static KDlg_TextureStageState::Operation Ops[] = {
	{D3DTOP_DISABLE, "D3DTOP_DISABLE"},
	{D3DTOP_SELECTARG1, "D3DTOP_SELECTARG1"},
	{D3DTOP_SELECTARG2, "D3DTOP_SELECTARG2"},
};

static KDlg_TextureStageState::Arg Args[] = 
{
	{D3DTA_CURRENT, "D3DTA_CURRENT"},
};

KDlg_TextureStageState& g_GetTextureStageStateDlg();