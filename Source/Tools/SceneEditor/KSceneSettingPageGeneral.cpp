// KSceneSettingPageGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingPageGeneral.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KSceneSettingPageGeneral dialog

IMPLEMENT_DYNAMIC(KSceneSettingPageGeneral, CPropertyPage)
KSceneSettingPageGeneral::KSceneSettingPageGeneral()
	: CPropertyPage(KSceneSettingPageGeneral::IDD)
{
	m_lpSceneEditor = NULL;
}

KSceneSettingPageGeneral::~KSceneSettingPageGeneral()
{
}

void KSceneSettingPageGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KSceneSettingPageGeneral, CPropertyPage)
END_MESSAGE_MAP()


// KSceneSettingPageGeneral message handlers
