#pragma once


// KSceneSceneEditorSaveOption dialog

struct IButtonOperator 
{
	virtual VOID SetCheck(CButton& bt, INT nCheckState) = 0;
	virtual INT GetCheck(CButton& bt)= 0;
	virtual ~IButtonOperator() = 0{}
};

class KSceneSceneEditorSaveOption : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorSaveOption)

public:
	KSceneSceneEditorSaveOption(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorSaveOption();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_SAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bComputeCells;
	BOOL m_bLogicalExchange;
	BOOL m_b3DScene;
	BOOL m_bComputeBBox;
	BOOL m_bUpdateAllRest;
	BOOL m_bAllRest;
	std::pair<DWORD, std::tstring> GetSceneSaveOption();
	BOOL m_bTerrainMap;
	float m_fScale;
public:
	afx_msg void OnBnClickedCheckCell();
	afx_msg void OnBnClickedCheckLogicalexchange();
	afx_msg void OnBnClickedCheck3dscene();
	virtual BOOL OnInitDialog();

	VOID ShowSaveOption();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedClearcell();
	BOOL m_bClearCell;
	afx_msg void OnBnClickedCheckComputebbox();
	BOOL m_bOnlyAppointCell;
	afx_msg void OnBnClickedCheckOnlyappointcell();
	BOOL OnCommand( WPARAM wParam, LPARAM lParam);
	void CheckSaveOptionCheckBoxes();
	void UpdateOptionState(BOOL bToDataOrToInterface);
private:
	DWORD m_dwSceneSaveOp;
	typedef std::vector<CButton*> TypeButtonCt;
	TypeButtonCt m_vecSaveOptionCheckBoxes;
public:
	afx_msg void OnBnClickedCheckUpdateAllRest();
	afx_msg void OnBnClickedRadioRest();
	afx_msg void OnBnClickedRadioNotRest();
	afx_msg void OnBnClickedCheckTerrainmap();
	afx_msg void OnEnChangeEditScale();
	afx_msg void OnBnClickedOk();
};
