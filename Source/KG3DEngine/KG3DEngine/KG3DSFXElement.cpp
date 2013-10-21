#include "StdAfx.h"
#include "KG3DSFXElement.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DSFXElement::KG3DSFXElement(void)
{
    m_fCurrentFrame = 0;
}

KG3DSFXElement::~KG3DSFXElement(void)
{
}


HRESULT KG3DSFXElement::SetCurrentFrame(float fFrame)
{
	return S_OK;
}

HRESULT KG3DSFXElement::RenderInEditor()
{
	return S_OK;
}
