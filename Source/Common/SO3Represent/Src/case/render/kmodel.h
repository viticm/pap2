#ifndef CASE_RENDER_K_MODEL_H
#define CASE_RENDER_K_MODEL_H

#include "KG3DEngine/KG3DTypes.h"

struct BoneTransformationInputData;
class IKG3DModel;
class IKG3DScene;
class IKG3DAnimationEventHandler;
class KRLScene;
class KRLSceneObject;

struct KRLSFXParam
{
    LPCTSTR	szFilePath;
    LPCTSTR	szBoneName;
    DWORD dwPlayType;
    float fSpeed;
    float fScale;
};

struct KRLMonoAnimationParam
{
    DWORD dwPlayType;
    enuAnimationControllerPriority nTrackType;
    int nOffsetTime;
    DWORD dwTweenTime;
    LPCTSTR szFilePath;
    float fSpeed;
    void* pUserdata;

    double fStartTime;
};

struct KRLSplitAnimationParam : KRLMonoAnimationParam
{
    DWORD dwSplitType;
    DWORD dwExtraInfo;
};

class KModel
{
public:
	static HRESULT LoadMDL(KRLScene* pRLScene, IKG3DModel** pp3DModel, KRLSceneObject* pRLSceneObject, LPCTSTR szMDLFilePath, BOOL bVisible, BOOL bSelectable);
	static HRESULT UnloadMDL(KRLScene* pRLScene, IKG3DModel** pp3DModel, KRLSceneObject* pRLSceneObject, BOOL bVisible, BOOL bSelectable);

	static HRESULT LoadModel(KRLScene* pRLScene, IKG3DModel** pp3DModel, IKG3DAnimationEventHandler* p3DAnimationEventHandler, LPCTSTR szModelFilePath);
	static HRESULT UnloadModel(KRLScene* pRLScene, IKG3DModel** pp3DModel, IKG3DAnimationEventHandler* p3DAnimationEventHandler);

	static HRESULT RegisterAnimationEventHandler(KRLScene* pRLScene, IKG3DModel* p3DModel, KRLSceneObject* pRLSceneObject, BOOL bSelectable);
	static HRESULT UnregisterAnimationEventHandler(KRLScene* pRLScene, IKG3DModel* p3DModel, KRLSceneObject* pRLSceneObject, BOOL bSelectable);

	static HRESULT LoadMaterial(IKG3DModel* p3DModel, LPCTSTR szFilePath);

	static HRESULT AttachModel(IKG3DModel* p3DModelTo, IKG3DModel* p3DModel);
	static HRESULT DetachModel(IKG3DModel* p3DModelTo, IKG3DModel* p3DModel);

    static HRESULT SetFaceMotionMode(IKG3DModel* p3DModel);

	static HRESULT Bind(KRLScene* pRLScene, IKG3DModel* p3DModelTo, IKG3DModel* p3DModel, LPCTSTR szName);
	static HRESULT Unbind(KRLScene* pRLScene, IKG3DModel* p3DModelFrom, IKG3DModel* p3DModel);

	static HRESULT PlayMonoAnimation(IKG3DModel* p3DModel, KRLMonoAnimationParam Param);
	static HRESULT PlaySplitAnimation(IKG3DModel* p3DModel, KRLSplitAnimationParam Param);

    static HRESULT SeekMonoAnimation(IKG3DModel* p3DModel, KRLMonoAnimationParam const& Param);
    static HRESULT SeekSplitAnimation(IKG3DModel* p3DModel, KRLSplitAnimationParam const& Param);

    static HRESULT PauseAnimation(IKG3DModel* p3DModel, BOOL bPause);

    static HRESULT SetOffsetDirection(IKG3DModel* p3DModel, D3DXVECTOR3 vOffset);
    static HRESULT SetBrightness(IKG3DModel* p3DModel, float fScale);
    static HRESULT SetAlpha(IKG3DModel* p3DModel, float fScale);
    static HRESULT GetBBox(IKG3DModel* p3DModel, D3DXVECTOR3& vMin, D3DXVECTOR3& vMax);

	static HRESULT SetScale(IKG3DModel* p3DModel, float fScale);
	static HRESULT SetScale(IKG3DModel* p3DModel, D3DXVECTOR3 vScale);

	static HRESULT SetPosition(IKG3DModel* p3DModel, D3DXVECTOR3 vPosition);
	static HRESULT GetPosition(IKG3DModel* p3DModel, D3DXVECTOR3& vPosition);

	static HRESULT SetRotation(IKG3DModel* p3DModel, D3DXQUATERNION qRotation);
	static HRESULT GetRotation(IKG3DModel* p3DModel, D3DXQUATERNION& qRotation);

	static HRESULT AppendRenderEntity(IKG3DScene* p3DScene, IKG3DModel* p3DModel, BOOL bSelectable);
	static HRESULT RemoveRenderEntity(IKG3DScene* p3DScene, IKG3DModel* p3DModel, BOOL bSelectable);

    static HRESULT SetMonoAnimationSpeed(IKG3DModel* p3DModel, float fSpeed);
    static HRESULT SetSplitAnimationSpeed(IKG3DModel* p3DModel, DWORD dwPartIndex, float fSpeed);

	static HRESULT SetCaptionVisible(IKG3DModel* p3DModel, DWORD dwIndex, BOOL bVisible);
	static HRESULT SetCaption(IKG3DModel* p3DModel, DWORD dwIndex, LPCSTR szContent);
	static HRESULT SetCaptionColor(IKG3DModel* p3DModel, DWORD dwIndex, D3DCOLORVALUE const& color);
	static HRESULT SetCaptionFloorHeight(IKG3DModel* p3DModel, float fHeight);
	static HRESULT SetPercentage(IKG3DModel* p3DModel, float fPercentage);
	static HRESULT SetPercentageColor(IKG3DModel* p3DModel, D3DCOLORVALUE const& color);
	static HRESULT SetPercentageVisible(IKG3DModel* p3DModel, BOOL bVisible);

	static HRESULT SetDetail(IKG3DModel* p3DModel, int nDefineTable, int nIndex);

	static HRESULT SetBoneTransform(IKG3DModel* p3DModel, LPCSTR szBone, BoneTransformationInputData const& Data);
	static HRESULT EnableBoneTransform(IKG3DModel* p3DModel, BOOL bEnable);
};

int GetAnimationPlayTime(double fTime);

#endif // CASE_RENDER_K_MODEL_H