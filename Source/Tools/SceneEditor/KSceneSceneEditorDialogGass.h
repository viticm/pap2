#pragma once


// KSceneSceneEditorDialogGass dialog
interface IEKG3DSceneSceneEditor;

class KSceneSceneEditorDialogGass : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogGass)

public:
	KSceneSceneEditorDialogGass(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogGass();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_GRASS };

	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;

	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpengrasstexture();
	afx_msg void OnBnClickedButtonAutoLight();
public:
	afx_msg void OnBnClickedButtonFitground();
};


