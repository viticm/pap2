// KSceneSettingPageGround.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingPageGround.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KSceneSettingPageGround dialog

IMPLEMENT_DYNAMIC(KSceneSettingPageGround, CPropertyPage)
KSceneSettingPageGround::KSceneSettingPageGround()
	: CPropertyPage(KSceneSettingPageGround::IDD)
{
	m_lpSceneEditor = NULL;
}

KSceneSettingPageGround::~KSceneSettingPageGround()
{
}

void KSceneSettingPageGround::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KSceneSettingPageGround, CPropertyPage)
END_MESSAGE_MAP()


// KSceneSettingPageGround message handlers
