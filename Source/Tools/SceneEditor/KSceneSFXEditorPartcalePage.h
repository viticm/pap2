#pragma once


// KSceneSFXEditorPartcalePage dialog

class KSceneSFXEditorParticlePage : public CPropertyPage
{
	DECLARE_DYNAMIC(KSceneSFXEditorParticlePage)

public:
	KSceneSFXEditorParticlePage();
	virtual ~KSceneSFXEditorParticlePage();

// Dialog Data
	enum { IDD = IDD_SFXEDITOR_PARTICLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
