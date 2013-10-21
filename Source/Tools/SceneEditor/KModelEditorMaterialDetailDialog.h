#pragma once
#include "afxwin.h"
#include "IEEditor.h"
#include "KColorDialog.h"

enum enumButton
{
    ENUM_BUTTON_BODY1 = 0,
    ENUM_BUTTON_BODY2 = 1,
    ENUM_BUTTON_DETAIL1 = 2,
    ENUM_BUTTON_DETAIL2 = 3,
	ENUM_BUTTON_FLOW = 4,
    ENUM_BUTTON_COUNT = 5,
    ENUM_BUTTON_NONE = 0xffffffff,
};


class KModelEditorMaterialDetailDialog : public CDialog
{
	enum ModifyState
	{
		MODIFY_NONE,
		Modify_Table_Body,
		Modify_Mtl,
	}m_eModifyState;

	DECLARE_DYNAMIC(KModelEditorMaterialDetailDialog)

	IEKG3DMaterial* m_lpMaterial;
	IEKG3DModel   * m_lpModel;
public:
	HRESULT SetMaterial(IEKG3DMaterial* pMtl,IEKG3DModel* pModel)
	{
		m_lpMaterial = pMtl;
		m_lpModel    = pModel;
		return S_OK;
	}
    HRESULT SetParentHwnd(CWnd* pCwnd)
    {
        m_hWnd = (pCwnd == NULL) ? NULL : pCwnd->GetSafeHwnd();
        return S_OK;
    }
	KModelEditorMaterialDetailDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KModelEditorMaterialDetailDialog();

// Dialog Data
	enum { IDD = IDD_MODELEDITOR_MATERIAL_DETAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	struct CopyData
	{
		float m_fBody_Max;
		float m_fBodyMin;
		float m_fBody_Scale;
		float m_fDetail0_ColorScale;
		float m_fDetail0_UVScale;
		float m_fDetail1_ColorScale;
		float m_fDetail1_UVScale;
		D3DCOLORVALUE m_vBodyColor;
		D3DCOLORVALUE m_vBodyColor1;
		D3DCOLORVALUE m_vDetail0Color;
		D3DCOLORVALUE m_vDetail1Color;
		string m_szChannelFile;
		string m_szDetailFile0;
		string m_szDetailFile1;
		float m_fBody_Env;
		float m_fBody_Specular;
		float m_fDetail0_Env;
		float m_fDetail0_Specular;
		float m_fDetail1_Env;
		float m_fDetail1_Specular;
	}m_CopyData;
	BOOL m_bHasCopyData;

	float m_fBody_Max;
	float m_fBodyMin;
	float m_fBody_Scale;
	float m_fDetail0_ColorScale;
	float m_fDetail0_UVScale;
	float m_fDetail1_ColorScale;
	float m_fDetail1_UVScale;
	D3DCOLORVALUE m_vBodyColor;
	D3DCOLORVALUE m_vBodyColor1;
	D3DCOLORVALUE m_vDetail0Color;
	D3DCOLORVALUE m_vDetail1Color;
	string m_szChannelFile;
	string m_szDetailFile0;
	string m_szDetailFile1;
	float m_fBody_Env;
	float m_fBody_Specular;
	float m_fDetail0_Env;
	float m_fDetail0_Specular;
	float m_fDetail1_Env;
	float m_fDetail1_Specular;

	CButton m_Button_Detail0_Texture;
	CButton m_Button_Detail1_Texture;

    KColorPickerButton m_Button_Color[ENUM_BUTTON_COUNT];

	afx_msg void OnEnChangeEditBodymax();
	afx_msg void OnEnChangeEditBodymin();
	afx_msg void OnEnChangeEditBodyscale();

	afx_msg void OnEnChangeEditDetail0Colorscale();
	afx_msg void OnEnChangeEditDetail0Uvscale();
	afx_msg void OnBnClickedButtonDetail0Texture();
	afx_msg void OnEnChangeEditDetail1Colorscale();
	afx_msg void OnEnChangeEditDetail1Uvscale();
	afx_msg void OnBnClickedButtonDetail1Texture();

	HRESULT Update(BOOL bToModel);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	void  FillTableCombo();
	void  FillListMainColor();
	void  FillListMtlDefine();

	void Copy();
	void Paste();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnInitDialog();
public:
	afx_msg void OnCbnSelchangeComboTableindex();
	afx_msg void OnBnClickedButtonAddtable();
	afx_msg void OnLbnSelchangeListBodydefine();
	afx_msg void OnBnClickedButtonAddbody();
	afx_msg void OnBnClickedButtonDeletebody();
	afx_msg void OnLbnSelchangeListMtldefine();
	afx_msg void OnBnClickedButtonAddmtldefine();
	afx_msg void OnBnClickedButtonDeletemtldefine();
	CComboBox m_Combo_Table;
	CListBox m_List_BodyDefine;
	CListBox m_List_MtlDefine;
	int m_nCurrentTableIndex;
	int m_nCurrentTableBodyIndex;
	int m_nCurrentMtlDefineIndex;
    int m_nPreMtlDefineIndex;
	afx_msg void OnBnClickedButtonDeletetable();
	afx_msg void OnBnClickedButtonMainrename();
	afx_msg void OnBnClickedButtonDetailrename();
	afx_msg void OnBnClickedButtonMtlrename();
	afx_msg void OnBnClickedButtonTablerename();
	CString m_szTableName;
	CString m_szMainName;
	CString m_szMtlDefineName;
	afx_msg void OnEnChangeEditBodyenv();
	afx_msg void OnEnChangeEditBodyspecular();
	afx_msg void OnBnClickedButtonDetail0Clear();
	afx_msg void OnBnClickedButton();

	afx_msg void OnEnChangeEditBodyenv2();
	afx_msg void OnEnChangeEditBodyspecular2();
	afx_msg void OnEnChangeEditBodyenv3();
	afx_msg void OnEnChangeEditBodyspecular3();
	afx_msg void OnBnClickedButtonSavetable2();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedButtonPaste();
	CButton m_ButtonPaste;
	afx_msg void OnBnClickedButtonSavemtl();
    afx_msg void OnBnClickedButtonChangetexture();

    afx_msg void OnBnClickedCheckWhole();
    afx_msg void OnCbnSelchangeComboSubset();

private:
    HRESULT SetComboTableIndex(LPCTSTR pszCaption);
    HRESULT ColorUpdate(DWORD dwColor, HWND hWnd);
    void SetButtonColor();
    HRESULT FillComboSubSet();

    HWND m_hWnd;

    CButton m_Button_ColorCastWhole;
    BOOL m_bColorCastWhole;
    CComboBox m_Combo_SubsetList;

    int m_nCurSubSetIndex;
    string m_strTextureFileName;
    string m_strCurTextureFileName;
public:
	afx_msg void OnEnChangeEditFlowColorscale();
	float m_fFlow_ColorScale;
	float m_fFlow_UVScale;
	float m_fFlow_SpeedX;
	float m_fFlow_SpeedY;
	D3DCOLORVALUE m_vFlow_Color;
	afx_msg void OnEnChangeEditFlowUvscale();
	afx_msg void OnEnChangeEditFlowSpeed();
	afx_msg void OnBnClickedButtonFlowTexture();
	afx_msg void OnBnClickedButtonFlowTextureclear();
	string  m_strFlowTexture;
	CButton m_Button_FlowTexture;
	afx_msg void OnBnClickedButtonDetail1Color();
	afx_msg void OnBnClickedButtonFlowColor();
	afx_msg void OnEnChangeEditFlowSpeedy();
};
