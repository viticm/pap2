#pragma once


// KSceneModelEditorEffectSetDialog dialog

class KSceneModelEditorEffectSetDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneModelEditorEffectSetDialog)

public:
	KSceneModelEditorEffectSetDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneModelEditorEffectSetDialog();

// Dialog Data
	enum { IDD = IDD_MODELEDITOR_EFFECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
