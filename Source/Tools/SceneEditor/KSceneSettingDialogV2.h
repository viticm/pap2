#pragma once
//#include "KSceneSceneGridCtrl.h"
#include <XTToolkitPro.h>
// KSceneSettingDialogV2 dialog

class KSceneSettingDialogV2 : public CDialog
{
public:
	virtual  BOOL OnInitDialog();
private:
	//typedef KSH::COMMON_CTRL::KSceneSceneGridCtrl TypeGridCtrl;
	//TypeGridCtrl m_GridCtrl;

	CXTPPropertyGrid	m_Grid;
private:
	VOID InitGrid();
//////////////////////////////////////////////////////////////////////////
	DECLARE_DYNAMIC(KSceneSettingDialogV2)

public:
	KSceneSettingDialogV2(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSettingDialogV2();

	enum { IDD = IDD_SCENESETTING_ENVIRONMENT2 };	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
};
