// KSceneModelEditorEffectSetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneModelEditorEffectSetDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KSceneModelEditorEffectSetDialog dialog

IMPLEMENT_DYNAMIC(KSceneModelEditorEffectSetDialog, CDialog)
KSceneModelEditorEffectSetDialog::KSceneModelEditorEffectSetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorEffectSetDialog::IDD, pParent)
{
}

KSceneModelEditorEffectSetDialog::~KSceneModelEditorEffectSetDialog()
{
}

void KSceneModelEditorEffectSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KSceneModelEditorEffectSetDialog, CDialog)
END_MESSAGE_MAP()


// KSceneModelEditorEffectSetDialog message handlers
