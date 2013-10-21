#pragma once


// KSceneObjectEditorPropertySubSheet 对话框

class KSceneObjectEditorPropertySubSheet : public CPropertyPage
{
	DECLARE_DYNAMIC(KSceneObjectEditorPropertySubSheet)

public:
	KSceneObjectEditorPropertySubSheet();
	virtual ~KSceneObjectEditorPropertySubSheet();

// 对话框数据
	enum { IDD = IDD_OBJECTEDIT_PROPERTY_SHEET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
