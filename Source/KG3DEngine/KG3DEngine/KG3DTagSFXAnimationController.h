#pragma once
#include "kg3dsfxanimationcontroller.h"

class KG3DTagSFXAnimationController :
	public KG3DSFXAnimationController
{
public:
	KG3DTagSFXAnimationController(KG3DSFX* pSrcSFX);
	~KG3DTagSFXAnimationController(void);
	KG3DTagSFXAnimationController(KG3DSFXAnimationController* pSrc);
	DWORD m_dwSFXID;
	virtual enuAnimationControllerPriority GetPriority();
	virtual int GetCycleTime();
	virtual DWORD GetPlayType();
};
