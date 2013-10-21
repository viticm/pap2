#pragma once
#include "afxwin.h"


// KSceneSettingDialogAddEnvEffect dialog

class KSceneSettingDialogAddEnvEffect : public CDialog
{
	DECLARE_DYNAMIC(KSceneSettingDialogAddEnvEffect)

public:
	KSceneSettingDialogAddEnvEffect(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSettingDialogAddEnvEffect();

// Dialog Data
	enum { IDD = IDD_SCENESETTING_DLG_ADD_ENVEFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbEnvEffList;
	virtual BOOL OnInitDialog();
private:
	DWORD m_dwRetType;
public:
	DWORD GetResultType(void);
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
