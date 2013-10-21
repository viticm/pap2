#pragma once

#include "IEKG3DScene.h"
#include "IEEditor.h"
#include "KSceneSettingPropertySheet.h"

class KSceneSettingPageBase;
class KSceneSceneEdit;

class KSceneSettingDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneSettingDialog)

public:
	KSceneSettingDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSettingDialog();

// Dialog Data
	enum { IDD = IDD_SCENESETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    enum SCENESETTINGTYPE
    {
        SETTING_LOW,
        SETTING_ADVANCED
    };

protected:
	KSceneSettingPropertySheet	m_PropertySheet;
	std::vector<KSceneSettingPageBase*>	m_vecPages;

public:
	virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	virtual void OnCancel();
	virtual void OnOK();
private:
	bool m_bIsMyselfModal;
public:
	virtual INT_PTR DoModal();///<CTH>覆盖这个方法，简单的设置一个标志就可以很容易的判断这个东西究竟是不是一个模态对话框了
	///然后就可以在OnOk和OnCancel上面根据这个标志做文章了,有的地方这个Dialog是模态的。
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedDiscard();
    afx_msg void OnBnClickedRadioSettinglow();
    afx_msg void OnBnClickedRadioSettingadvanced();
};
