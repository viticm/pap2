#pragma once

class KSceneSceneEdit;
// KSceneSettingPageAmbient dialog

class KSceneSettingPageAmbient : public CPropertyPage
{
	DECLARE_DYNAMIC(KSceneSettingPageAmbient)

public:
	KSceneSettingPageAmbient();
	virtual ~KSceneSettingPageAmbient();

// Dialog Data
	enum { IDD = IDD_SCENESETTING_AMBIENT };
	HRESULT SetSceneEditor(KSceneSceneEdit* pEdit)
	{
		m_lpSceneEditor = pEdit;
		return S_OK;
	}
	KSceneSceneEdit*           m_lpSceneEditor;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
