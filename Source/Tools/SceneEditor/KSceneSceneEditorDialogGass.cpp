// KSceneSceneEditorDialogGass.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogGass.h"
#include ".\kscenesceneeditordialoggass.h"
//#include "KG3DSceneSceneEditor.h"
//#include "Terrain/KG3DTerrainEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KSceneSceneEditorDialogGass dialog


IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogGass, CDialog)
KSceneSceneEditorDialogGass::KSceneSceneEditorDialogGass(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogGass::IDD, pParent)
{
	m_lpSceneSceneEditor = NULL;
}

KSceneSceneEditorDialogGass::~KSceneSceneEditorDialogGass()
{
}

void KSceneSceneEditorDialogGass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogGass, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPENGRASSTEXTURE, OnBnClickedButtonOpengrasstexture)
	ON_BN_CLICKED(IDC_BUTTON_AUTOLIGHT, OnBnClickedButtonAutoLight)
	ON_BN_CLICKED(IDC_BUTTON_FITGROUND, &KSceneSceneEditorDialogGass::OnBnClickedButtonFitground)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogGass message handlers

void KSceneSceneEditorDialogGass::OnBnClickedButtonOpengrasstexture()
{
	if(!m_lpSceneSceneEditor)
		return;

	char szFilter[] ="Texture Files|*.bmp;*.jpg;*.png;*.tga;*.DDS|";

	CFileDialog dlg(true, NULL, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	CString sz = dlg.GetPathName();
	TCHAR name[256];
	wsprintf(name,"%s",sz);
	if(m_lpSceneSceneEditor->m_lpTerrainEx)
	{
		m_lpSceneSceneEditor->m_lpTerrainEx->m_GrassRender.
			OpenGrassTexure(name);
	}
	// TODO: Add your control notification handler code here
}

void KSceneSceneEditorDialogGass::OnBnClickedButtonAutoLight()
{
	if(!m_lpSceneSceneEditor)
		return;
	if(m_lpSceneSceneEditor->m_lpTerrainEx)
	{
		m_lpSceneSceneEditor->m_lpTerrainEx->ComputeGrassColor();
	}
}


void KSceneSceneEditorDialogGass::OnBnClickedButtonFitground()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
	if(m_lpSceneSceneEditor->m_lpTerrainEx)
	{
		m_lpSceneSceneEditor->m_lpTerrainEx->GrassFitGround();
	}
}
