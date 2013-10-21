////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DSceneCamera.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2006-11-07 11:44:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DSCENECAMERA_H_
#define _INCLUDE_IEKG3DSCENECAMERA_H_
#include "KG3DInterface.h"
////////////////////////////////////////////////////////////////////////////////
struct IEKG3DSpecialCurve
{
	FLOAT	fTime;
	FLOAT	fSize;
	
};

typedef VOID (CALLBACK *KG3DSCAniSelKeyChangeCallback)(DWORD dwKeyFrameTime);

enum
{
	emKG3DSceneCameraAnimationSpecialKeyTrack_Circle,
};

typedef struct _KG3DSceneCameraAnimationSPTrack_Struct
{
	FLOAT m_fRadius;
	D3DXVECTOR3 m_vPos;
	BOOL	m_bClockWiseOrCounter;
#ifdef __cplusplus
	_KG3DSceneCameraAnimationSPTrack_Struct()
		:m_fRadius(100)
		,m_vPos(0, 0, 0)
		,m_bClockWiseOrCounter(FALSE)//默认逆时针
	{

	}
#endif
}KG3DSceneCameraAnimationSPTrack_Struct;


interface IEKG3DSceneCameraAnimation : public IKG3DSceneCameraAnimation
{
    virtual HRESULT ComputeTangent() = 0;
    virtual HRESULT AddKeyFrame() = 0;
	virtual HRESULT AddKeyFrameToHeadFramePosition() = 0;
    virtual HRESULT RemoveKeyFrame(DWORD dwKeyFrameTime) = 0;
    virtual HRESULT Clear() = 0;
    virtual HRESULT StartPlay(BOOL bLoop) = 0;// 光Start还不会真的播放，还需要设SceneSceneEditor的SetEditState(SCENESTATE_CAMERAANIMATION)
    virtual HRESULT StopPlay() = 0;
	virtual HRESULT PausePlay(IKG3DSceneCameraKeyFrame* pPauseKey, BOOL* bIsPaused) = 0;
    virtual HRESULT GetName(LPTSTR ppcszRetName, size_t uBufferSize) = 0;
    virtual HRESULT SetName(LPCTSTR pcszName) = 0;
    virtual HRESULT GetPlayState(BOOL *pbRetIsPlaying) = 0;
    virtual HRESULT IsKeyFrameEditable(BOOL *pbRetIsKeyFreeEditable) = 0;
    virtual HRESULT GetKeyFrameCount(DWORD *pdwKeyFrameCount) = 0;
	//virtual HRESULT CreateCircleAnimation();
	virtual HRESULT GetCurSelectedKeyTime(DWORD *pdwRetSelectedKeyTime, DWORD *pdwRetPreKeyTime, DWORD *pdwRetNextKeyTime) = 0;
	virtual HRESULT GetCurSelectedKey(IKG3DSceneCameraKeyFrameEx* pKey) = 0;
	virtual HRESULT SetCurSelectedKey(const IKG3DSceneCameraKeyFrameEx* pKey) = 0;
	virtual HRESULT SetKeySelChangeCallback(KG3DSCAniSelKeyChangeCallback pNewCallback, KG3DSCAniSelKeyChangeCallback* ppOldCallback) = 0;
	virtual HRESULT CreateSpecialTrack(DWORD dwType, DWORD_PTR dwOption) = 0;
	virtual float GetPlaySpeed() = 0;
	virtual HRESULT SetPlaySpeed(float fSpeed) = 0;
	virtual float GetPlayAngle() = 0;
	virtual HRESULT SetPlayAngle(float fAngle) = 0;
	virtual HRESULT EnableEditCamAniKeyFrame(BOOL) = 0;
};



interface IEKG3DSceneCameraMovement : public IKG3DSceneCameraMovement
{
    virtual HRESULT GetCurCamAnimation(IEKG3DSceneCameraAnimation **ppiRetCameraAnimation) = 0;
    virtual HRESULT	SetCurCamAnimation(int nIndex) = 0;
    virtual HRESULT	NewOneCamAnimation() = 0;
    virtual HRESULT RemoveCurCamAnimation() = 0;
    virtual HRESULT Clear() = 0;
	virtual HRESULT FindCamAniByName(LPCTSTR pstrCamAniName, int nFinded, int* nRetIndex) = 0;///nFinded用1表示要第一个找到的，一般传1
    virtual HRESULT GetAnimationCount(DWORD *pdwRetAnimationCount) = 0;
    virtual HRESULT	LetCurCamAniPosMoveUp() = 0;
    virtual HRESULT LetCurCamAniPosMoveDown() = 0;
    virtual HRESULT	SetCurCamAniName( LPCTSTR pCamAniName ) = 0;
    virtual HRESULT GetCurCamAniIndex(DWORD *pdwRetAnimationIndex) = 0;
	virtual HRESULT LoadFromFile(LPSTR pFileName) = 0;
};

#endif //_INCLUDE_IEKG3DSCENECAMERA_H_
