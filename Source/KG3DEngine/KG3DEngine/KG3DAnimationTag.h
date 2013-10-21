#pragma once
#include "KG3DInterface.h"
#include "IEKG3DAnimationTag.h"
#include "IEKG3DSFX.h"
#include "kg3dmodel.h"
class KG3DSFXSuede;
class KG3DAnimationController;

enum enuTagRunState
{
	TAG_STATE_RUNNING,
	TAG_STATE_FINISHED,
};

class IKG3DAnimationTag
{
public:
	virtual ~IKG3DAnimationTag(void){};
    virtual HRESULT Clear() = 0;
    virtual HRESULT LoadFromFile(IFile* pFile, DWORD dwVersion, DWORD dwNumFrames) = 0;
	virtual HRESULT Apply(KG3DAnimationController* pController, D3DXVECTOR3 *pvPos);
	virtual BOOL CheckValidation() = 0;
	virtual void SetAnimationSpeed(float fSpeed) = 0;
	virtual void OnMotionChange() = 0;
	virtual void SetParentModel(IKG3DModel* pModel) = 0;
    virtual void Reset() = 0;
	virtual enuTagRunState GetState() = 0;

	virtual HRESULT Clone(IKG3DAnimationTag* pSrc) = 0;
    //virtual HRESULT CloneRuntimeInfo(const IKG3DAnimationTag& Animation) = 0;
protected:
	virtual HRESULT Helper_Apply(KG3DAnimationController* pController, 
		UINT uStartFrame, 
		UINT uEndFrame, 
		D3DXVECTOR3 *pvPos) = 0;
};

struct AnimationTagBlockHeader
{
    DWORD dwType;
    DWORD dwVersion;
    DWORD dwNumKeyFrames;
};


extern IKG3DAnimationEventHandler* GetSFXTagMotionSwitchEventHandeler();
HRESULT FindKeyFrame(UINT uFrame, 
                     FindFrameType FindType, 
                     const std::vector<UINT>& KeyedFrame,
                     UINT& uFrameIndex);