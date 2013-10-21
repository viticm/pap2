#ifndef CASE_ACTIONOBJECT_MISSILE_KMODELMISSILEPROCESSOR_H
#define CASE_ACTIONOBJECT_MISSILE_KMODELMISSILEPROCESSOR_H

#include "../../render/krlresource.h"
#include "../../gameworld/ktabledefine.h"

class IKG3DAnimationController;
class KRLScene;
class KCompositeMissileProcessor;

class KModelMissileProcessor
{
private:
	struct K3DSFXModelItem
	{
		IKG3DModel*	p3DModel;
        int nFinished; 
	};

public:
	KModelMissileProcessor();
	~KModelMissileProcessor();

	int Init(DWORD dwMissileParamID, KCompositeMissileProcessor* pCompositeMissileProcessor);
	int UnInit(KCompositeMissileProcessor* pCompositeMissileProcessor);
	int Update(double fTime, double fTimeLast, KCompositeMissileProcessor* pCompositeMissileProcessor);
	int IsFinished() const;
	int Start(double fTime, KCompositeMissileProcessor* pCompositeMissileProcessor);
	int Stop(KCompositeMissileProcessor* pCompositeMissileProcessor);

	int AppendModel(int nModelType, LPCTSTR szFilePath, KCompositeMissileProcessor* pCompositeMissileProcessor);

	int	GetSFXModelCount() const;
	int	GetSFXModelCount(int nModelType) const;

    HRESULT SetLightningSFXModel(int nModelType, int nSFXModel, float fModelScale, D3DXVECTOR3 vPositionCaster, D3DXVECTOR3 vPositionTarget);
	HRESULT	GetSFXModel(int nModelType, int nSFXModel, IKG3DModel** pp3DModel) const;

    int SetSFXModelParam(int nModelType, int nSFXModel, float fModelScale, D3DXVECTOR3 vWorldPosition, D3DXQUATERNION qWorldRotation);

	HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
	HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
    HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);

private:
	int	FindFreeModelItemIndex(int nModelType) const;

    int PlayModelsAnimation(KCompositeMissileProcessor* pCompositeMissileProcessor);
    int BindModels(KCompositeMissileProcessor* pCompositeMissileProcessor);

private:
    KRLMissileModelParam m_Param;

	K3DSFXModelItem	m_a3DSFXModelItem[MISSILE_PHASE_SFX_TYPE_COUNT][MISSILE_PHASE_SFX_COUNT];
};

#endif // CASE_ACTIONOBJECT_MISSILE_KMODELMISSILEPROCESSOR_H