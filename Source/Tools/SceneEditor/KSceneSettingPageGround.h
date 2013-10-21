#pragma once
class KSceneSceneEdit;

// KSceneSettingPageGround dialog

class KSceneSettingPageGround : public CPropertyPage
{
	DECLARE_DYNAMIC(KSceneSettingPageGround)

public:
	KSceneSettingPageGround();
	virtual ~KSceneSettingPageGround();

// Dialog Data
	enum { IDD = IDD_SCENESETTING_GROUND };
	KSceneSceneEdit*           m_lpSceneEditor;
	HRESULT SetSceneEditor(KSceneSceneEdit* pEdit)
	{
		m_lpSceneEditor = pEdit;
		return S_OK;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
