#ifndef CASE_ACTIONOBJECT_MISSILE_KCOMPOSITEMISSILEPROCESSOR_H
#define CASE_ACTIONOBJECT_MISSILE_KCOMPOSITEMISSILEPROCESSOR_H

#include "../../render/krlresource.h"
#include "../../../base/missile/krepresentmissileprocessor.h"
#include "./kmodelmissileprocessor.h"
#include "./khomingmissileprocessor.h"
#include "./kyaxismissileprocessor.h"
#include "./ktimelinemissileprocessor.h"
#include "KG3DEngine/KG3DInterface.h"

class KRLMissile;

class KCompositeMissileProcessor : 
	public KRepresentMissileProcessor, 
	public IKG3DAnimationEventHandler
{
public:
	KCompositeMissileProcessor();

	int SetMissile(KRLMissile* pMissile);
    int SetParam(KRLMissileProcessorParam const& Param);

protected:
	virtual ~KCompositeMissileProcessor();
	
	virtual int OnInit();
    virtual int OnUnInit();
	virtual int OnUpdate(double fTime, double fTimeLast);
	virtual int OnIsFinished() const;
	virtual int OnStart(double fTime);
	virtual int OnStop(double fTime);

	virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
	virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
    virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);

public:
	KRLMissile* m_pRLMissile;

    KRLMissileProcessorParam m_Param;

    KModelMissileProcessor m_ModelMissileProcessor;
    KHomingMissileProcessor m_HomingMissileProcessor;
    KYAxisMissileProcessor m_YAxisMissileProcessor;
    KTimelineMissileProcessor m_TimelineMissileProcessor;
};

#endif // CASE_ACTIONOBJECT_MISSILE_KCOMPOSITEMISSILEPROCESSOR_H