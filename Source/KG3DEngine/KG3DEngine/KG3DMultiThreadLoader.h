#pragma once
#include "KG3DTempleteMultiThead.h"

class KG3DModel;
class KG3DMaterial;
class KG3DClip;
class KG3DAnimationTagContainer;
class KG3DMesh;
class KG3DTexture;

class KG3DMultiThreadLoader
{
public:
	BOOL m_bTryPauseMultiThreadLoad;
	BOOL m_bPauseMultiThreadLoad;

    TKG3DMultiTheadExchanger<KG3DModel>m_cMultiTreadLoadModels;
	TKG3DMultiTheadExchanger<KG3DMaterial>m_cMultiTreadLoadMaterial;
	TKG3DMultiTheadExchanger<KG3DClip>m_cMultiTreadLoadClip;
	TKG3DMultiTheadExchanger<KG3DAnimationTagContainer>m_cMultiTreadLoadTag;
	KG3DMultiThreadLoader(void);
	~KG3DMultiThreadLoader(void);

	HRESULT Init();
	HRESULT UnInit();

	HRESULT TryMultiThreadLoad_Model(KG3DModel* pModel);
	HRESULT StopMultiThreadLoad_Model(KG3DModel* pModel);
	HRESULT TryMultiThreadLoad_Material(KG3DMaterial* pMaterial);
	HRESULT StopMultiThreadLoad_Material(KG3DMaterial* pMaterial);
	HRESULT TryMultiThreadLoad_Tag(KG3DAnimationTagContainer* pTag);
	HRESULT StopMultiThreadLoad_Tag(KG3DAnimationTagContainer* pTag);

	HRESULT TryMultiThreadLoad_Clip(KG3DClip* pClip);
	HRESULT StopMultiThreadLoad_Clip(KG3DClip* pClip);

    HRESULT ProcessMultiThreadLoad();

	HRESULT PauseMultiThreadLoad(BOOL bEnable);
};

extern KG3DMultiThreadLoader g_cMultiThreadLoader;
