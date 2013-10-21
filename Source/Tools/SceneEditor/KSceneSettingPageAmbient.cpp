// KSceneSettingPageAmbient.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingPageAmbient.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// KSceneSettingPageAmbient dialog

IMPLEMENT_DYNAMIC(KSceneSettingPageAmbient, CPropertyPage)
KSceneSettingPageAmbient::KSceneSettingPageAmbient()
	: CPropertyPage(KSceneSettingPageAmbient::IDD)
{
	m_lpSceneEditor = NULL;
}

KSceneSettingPageAmbient::~KSceneSettingPageAmbient()
{
}

void KSceneSettingPageAmbient::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KSceneSettingPageAmbient, CPropertyPage)
END_MESSAGE_MAP()


// KSceneSettingPageAmbient message handlers
