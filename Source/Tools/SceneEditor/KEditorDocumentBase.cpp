#include "stdafx.h"
#include ".\keditordocumentbase.h"

//by dengzhihui Refactor 2006年11月6日
//#include "KG3DEngineManager.h"
//#include "SceneEditorCommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


KEditorDocumentBase::KEditorDocumentBase(void)
{
	m_lpScene = NULL;
}

KEditorDocumentBase::~KEditorDocumentBase(void)
{
	OnClose();
	m_lpScene = NULL;
}

HRESULT KEditorDocumentBase::OnClose()
{
	if(m_lpScene)
	{
		//g_pEngineManager->RemoveScene((IKG3DScene*)m_lpScene);//by dengzhihui Refactor 2006年11月6日
		g_pEngineManager->RemoveScene((IKG3DScene*)m_lpScene);
	}
	return S_OK;
}
