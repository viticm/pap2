#include "StdAfx.h"
#include "KG3DTagSFXAnimationController.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DTagSFXAnimationController::KG3DTagSFXAnimationController(KG3DSFX* pSrcSFX) : KG3DSFXAnimationController(pSrcSFX)
{
	m_dwSFXID = 0xffffffff;
}

KG3DTagSFXAnimationController::~KG3DTagSFXAnimationController(void)
{
}

KG3DTagSFXAnimationController::KG3DTagSFXAnimationController(KG3DSFXAnimationController* pSrc) : KG3DSFXAnimationController(NULL)
{
	if (pSrc)
	{
		Clone(pSrc);
	}
	m_dwSFXID = 0xffffffff;
}

enuAnimationControllerPriority KG3DTagSFXAnimationController::GetPriority()
{
	return ANICTL_PRIMARY;
}

int KG3DTagSFXAnimationController::GetCycleTime()
{
	return 0;
}

DWORD KG3DTagSFXAnimationController::GetPlayType()
{
	return 0;
}