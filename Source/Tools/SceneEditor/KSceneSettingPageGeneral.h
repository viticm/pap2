#pragma once

class KSceneSceneEdit;
// KSceneSettingPageGeneral dialog

class KSceneSettingPageGeneral : public CPropertyPage
{
	DECLARE_DYNAMIC(KSceneSettingPageGeneral)

public:
	KSceneSettingPageGeneral();
	virtual ~KSceneSettingPageGeneral();

// Dialog Data
	enum { IDD = IDD_SCENESETTING_GENERAL };

	HRESULT SetSceneEditor(KSceneSceneEdit* pEdit)
	{
		m_lpSceneEditor = pEdit;
		return S_OK;
	}
protected:
	KSceneSceneEdit* m_lpSceneEditor;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
