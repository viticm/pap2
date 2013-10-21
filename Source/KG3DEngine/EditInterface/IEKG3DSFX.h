#pragma once

#include "KG3DKeyFrameDataBase.h"

enum SFX_ANIMATION_PLAY
{
	SFX_AP_PAUSE,
	SFX_AP_LOOP,
	SFX_AP_ONE_TIME,

	SFX_AP_MAX
};
enum SFX_LAUNCHER_BIND_TO_TRACK_MODE
{
	SFX_LBTTM_NORMAL,
	SFX_LBTTM_PARTICLE_TO_TRACK,
};

enum SFX_LAUNCHER_SHAPE
{
	SFX_LS_SPHERE,
	SFX_LS_CIRCLE,
	SFX_LS_RECTANGLE,
	SFX_LS_CYLINDER,
};

enum 
{
    SFX_MOTION_WROLD,
    SFX_MOTION_SFX,
    SFX_MOTION_LAUCHER
};

enum SFX_PARTICLE_TYPE
{
	SFX_PT_BILLBOARD_POINT,       // complete orient eyes
	SFX_PT_BILLBOARD_ORIENTSELF,  // orient eyes base on move direction
	SFX_PT_MODEL,                 // model particle
	SFX_PT_LEAVES,                // particle which has random rotation
    SFX_PT_BILLBOARD_LOCK_WORLD_Y,
    SFX_PT_BILLBOARD_LOCK_WORLD_Z,
    SFX_PT_BILLBOARD_LOCK_LOCAL_Y,
    SFX_PT_BILLBOARD_LOCK_LOCAL_Z
};

enum SFX_PARTICLE_UPDATE_MODE
{
	SFX_PUM_FOLLOW_SFX,
	SFX_PUM_SELF,
};

enum SFX_BLEND_MODE
{
	SFX_BM_SRCALPHA_INVSRCALPHA = 0,
	SFX_BM_SRCALPHA_ONE,
    SFX_BM_SRCALPHA_INVSRCCOLOR,
    SFX_BM_SRCALPHA_SCREEN,

	SFX_BM_NUM
};

enum {
    SFX_IMPORT_SCANL_IMPSFX,
    SFX_IMPORT_CUT_IMPSFX,
    SFX_IMPORT_SCANL_CURSFX
};

enum {
    BILLBOARE_TYPE_NORMAL = 0,
    BILLBOARE_TYPE_LOCK_Y = 1,
    BILLBOARE_TYPE_LOCK_X = 2,
    BILLBOARE_TYPE_FREE   = 3,
    BILLBOARE_TYPE_COUNT
};

/* aoe type */
enum {
    AOE_NORMAL,
    AOE_PROJECT,
    AOE_FOOTPRINT,
    AOE_SELECTOR
};

/* aoe face type */
enum {
    AOE_FACE_NONE,
    AOE_FACE_EYES,
    AOE_FACE_PLAYER
};

struct IEKG3DSFXParticleLifeTimeLine;
class IEKG3DSFXParticleForceField;
class IEKG3DSFXParticleLauncher
{
public:
	virtual BOOL IsBindToTrack() const = 0;
	virtual DWORD GetBindTrackIndex() const = 0;
	virtual HRESULT BindToTrack(DWORD dwIndex) = 0;
	virtual HRESULT UnBindToTrack() = 0;
	virtual void SetBindToTrackMode(SFX_LAUNCHER_BIND_TO_TRACK_MODE eMode) = 0;
	virtual SFX_LAUNCHER_BIND_TO_TRACK_MODE GetBindToTrackMode() const = 0;

    virtual KGRTSTimeLine* GetRtsTimeLine() = 0;
    virtual void InsertRtsKeyFrame(int nFrame, DWORD dwFlag)  = 0;
    /*
	virtual TKG3DTimeLinebase* GetTranslationTimeLine() = 0;
	virtual TKG3DTimeLinebase* GetScalingTimeLine() = 0;
	virtual TKG3DTimeLinebase* GetRotationTimeLine() = 0;
    */

    virtual void SetTranslation(D3DXVECTOR3* pValue) = 0;
    virtual void SetRotation(D3DXQUATERNION* pValue) = 0;
    virtual void SetRotationCenter(D3DXVECTOR3* pValue) = 0;
    virtual void SetScaling(D3DXVECTOR3* pValue) = 0;
    virtual void SetScalingRotation(D3DXQUATERNION* pValue) = 0;
    virtual void SetScalingCenter(D3DXVECTOR3* pValue) = 0;

    virtual void GetTranslation(D3DXVECTOR3* pValue) = 0;
    virtual void GetRotation(D3DXQUATERNION* pValue) = 0;
    virtual void GetRotationCenter(D3DXVECTOR3* pValue) = 0;
    virtual void GetScaling(D3DXVECTOR3* pValue) = 0;
    virtual void GetScalingRotation(D3DXQUATERNION* pValue) = 0;
    virtual void GetScalingCenter(D3DXVECTOR3* pValue) = 0;

    virtual void UpdateTransformation() = 0;


    /*
	virtual void GetTranslation(D3DXVECTOR3* pValue) = 0;
	virtual void GetRotation(D3DXVECTOR3* pValue) = 0;
	virtual void GetScaling(D3DXVECTOR3* pValue) = 0;
	virtual void GetRotationCenter(D3DXVECTOR3* pValue) = 0;
	virtual void GetScalingCenter(D3DXVECTOR3* pValue) = 0;
    */

	virtual IEKG3DSFXParticleLifeTimeLine* GetParticleLifeTime() = 0;
	virtual void  SetParticleLifeTimeLine(IEKG3DSFXParticleLifeTimeLine* pParticleLifeTimeLine) = 0;

	virtual SFX_LAUNCHER_SHAPE GetLauncherShape() const =0;
	virtual SFX_PARTICLE_TYPE GetParticleType() const = 0;
	virtual SFX_PARTICLE_UPDATE_MODE GetParticleUpdateMode() const = 0;
	virtual void SetParticleUpdateMode(SFX_PARTICLE_UPDATE_MODE eParticleUpdateMode) = 0;
	virtual SFX_BLEND_MODE GetBlendMode() = 0;
	virtual HRESULT SetBlendMode(SFX_BLEND_MODE eBlendMode) = 0;
    virtual DWORD GetMotionType() = 0;
    virtual void  SetMotionType(DWORD dwType) = 0;
	virtual void SetParticleShape(SFX_LAUNCHER_SHAPE eParticleShape) =0;
	virtual void SetParticleType( SFX_PARTICLE_TYPE eParticleType ) = 0;
    virtual void SetForceOption(DWORD dwOption) = 0;
    virtual DWORD GetForceOption() = 0;

	virtual float GetShapeParam1() const = 0;
	virtual float GetShapeParam2() const = 0;
	virtual void SetShapeParam1( float fValue ) = 0;
	virtual void SetShapeParam2( float fValue ) = 0;

	virtual HRESULT SetRandTexDelay(DWORD dwDelay) = 0;
    virtual DWORD   GetRandTexDelay() = 0;

    virtual HRESULT SetRandColDelay(DWORD dwDelay) = 0;
    virtual DWORD   GetRandColDelay() = 0;

    virtual HRESULT SetRandRotDelay(DWORD dwDelay) = 0;
    virtual DWORD   GetRandRotDelay() = 0;

    virtual HRESULT SetRandAlpDelay(DWORD dwDelay) = 0;
    virtual DWORD   GetRandAlpDelay() = 0;

    virtual HRESULT SetRandForceRotDelay(DWORD dwDelay) = 0;
    virtual DWORD   GetRandForceRotDelay() = 0;

	virtual TKG3DTimeLinebase* GetParticleNumLine() = 0;
	virtual TKG3DTimeLinebase* GetParticleSpeedMinLine() = 0;
	virtual TKG3DTimeLinebase* GetParticleSpeedMaxLine() = 0;
	virtual TKG3DTimeLinebase* GetParticleLifeMinLine() = 0;
	virtual TKG3DTimeLinebase* GetParticleLifeMaxLine() = 0;
	virtual TKG3DTimeLinebase* GetParticleScatterMin() = 0;
	virtual TKG3DTimeLinebase* GetParticleScatterMax() = 0;

    virtual HRESULT AddEffForce(IEKG3DSFXParticleForceField* pForce) = 0;
    virtual HRESULT DelEffForce(IEKG3DSFXParticleForceField* pForce) = 0;
    virtual BOOL    HasEffForce(IEKG3DSFXParticleForceField* pForce) = 0;


    // editor
    virtual BOOL IsHide() = 0;
    virtual void Show(BOOL enable) = 0;
    virtual LPCTSTR GetName() = 0;
    virtual void SetName(LPCTSTR szName) = 0;
};

class IEKG3DSFXTrack
{
public:
	virtual void SetTranslation(D3DXVECTOR3* pValue) = 0;
	virtual void SetRotation(D3DXQUATERNION* pValue) = 0;
	virtual void SetRotationCenter(D3DXVECTOR3* pValue) = 0;
	virtual void SetScaling(D3DXVECTOR3* pValue) = 0;
	virtual void SetScalingRotation(D3DXQUATERNION* pValue) = 0;
	virtual void SetScalingCenter(D3DXVECTOR3* pValue) = 0;

	virtual void GetTranslation(D3DXVECTOR3* pValue) = 0;
	virtual void GetRotation(D3DXQUATERNION* pValue) = 0;
	virtual void GetRotationCenter(D3DXVECTOR3* pValue) = 0;
	virtual void GetScaling(D3DXVECTOR3* pValue) = 0;
	virtual void GetScalingRotation(D3DXQUATERNION* pValue) = 0;
	virtual void GetScalingCenter(D3DXVECTOR3* pValue) = 0;

	virtual DWORD GetInterval() const = 0;
	virtual void SetInterval(DWORD dwInterval) = 0;
	virtual DWORD GetFrameNum() const = 0;

    // editor
    virtual BOOL IsHide() = 0;
    virtual void Show(BOOL enable) = 0;
    virtual LPCTSTR GetName() = 0;
    virtual void SetName(LPCTSTR szName) = 0;

};

struct IEKG3DTexture;
struct IKG3DTexture;

interface IEKG3DSFXBillboard
{
public:
	virtual HRESULT UnBindToTrack() = 0;

	virtual BOOL IsBindToTrack() const = 0;

	virtual DWORD GetBindTrackIndex() const = 0;

	virtual HRESULT BindToTrack(DWORD dwIndex) = 0;

	virtual INT DiffuseTimeLineGetSize() = 0;
	virtual HRESULT DiffuseTimeLineGetKeyFrameByIndex( int *pnFrame, D3DXCOLOR *pData, DWORD dwIndex ) = 0;
	virtual HRESULT DiffuseTimeLineRemoveKeyFrame(INT nFrame) = 0;
	virtual HRESULT DiffuseTimeLineGetData(D3DXCOLOR *pData,float fFrame) = 0;
	virtual HRESULT DiffuseTimeLineIsKeyFrame(int nFrame) = 0;
	virtual HRESULT DiffuseTimeLineInsertKeyFrame(int nFrame,D3DXCOLOR Data) = 0;


	virtual INT WidthTimeLineGetSize() = 0;
	virtual HRESULT WidthTimeLineGetKeyFrameByIndex( int *pnFrame, float *pData, DWORD dwIndex ) = 0;
	virtual HRESULT WidthTimeLineRemoveKeyFrame(INT nFrame) = 0;
	virtual HRESULT WidthTimeLineGetData(float *pData,float fFrame) = 0;
	virtual HRESULT WidthTimeLineIsKeyFrame(int nFrame) = 0;
	virtual HRESULT WidthTimeLineInsertKeyFrame(int nFrame,float Data) = 0;

	virtual INT PositionTimeLineGetSize() = 0;
	virtual HRESULT PositionTimeLineGetKeyFrameByIndex( int *pnFrame, D3DXVECTOR3 *pData, DWORD dwIndex ) = 0;
	virtual HRESULT PositionTimeLineRemoveKeyFrame(INT nFrame) = 0;
	virtual HRESULT PositionTimeLineGetData(D3DXVECTOR3 *pData,float fFrame) = 0;
	virtual HRESULT PositionTimeLineIsKeyFrame(int nFrame) = 0;
	virtual HRESULT PositionTimeLineInsertKeyFrame(int nFrame,D3DXVECTOR3 Data) = 0;

	virtual INT HeightTimeLineGetSize() = 0;
	virtual HRESULT HeightTimeLineGetKeyFrameByIndex( int *pnFrame, float *pData, DWORD dwIndex ) = 0;
	virtual HRESULT HeightTimeLineRemoveKeyFrame(INT nFrame) = 0;
	virtual HRESULT HeightTimeLineGetData(float *pData,float fFrame) = 0;
	virtual HRESULT HeightTimeLineIsKeyFrame(int nFrame) = 0;
	virtual HRESULT HeightTimeLineInsertKeyFrame(int nFrame,float Data) = 0;

	virtual INT TextureFrameIndexGetSize() = 0;
	virtual HRESULT TextureFrameIndexGetKeyFrameByIndex( int *pnFrame, INT *pData, DWORD dwIndex ) = 0;
	virtual HRESULT TextureFrameIndexRemoveKeyFrame(INT nFrame) = 0;
	virtual HRESULT TextureFrameIndexGetData(int *pData,float fFrame) = 0;
	virtual HRESULT TextureFrameIndexIsKeyFrame(int nFrame) = 0;
	virtual HRESULT TextureFrameIndexInsertKeyFrame(int nFrame,int Data) = 0;

	virtual INT Texture2FrameIndexGetSize() = 0;
	virtual HRESULT Texture2FrameIndexGetKeyFrameByIndex( int *pnFrame, INT *pData, DWORD dwIndex ) = 0;
	virtual HRESULT Texture2FrameIndexRemoveKeyFrame(INT nFrame) = 0;
	virtual HRESULT Texture2FrameIndexGetData(int *pData,float fFrame) = 0;
	virtual HRESULT Texture2FrameIndexIsKeyFrame(int nFrame) = 0;
	virtual HRESULT Texture2FrameIndexInsertKeyFrame(int nFrame,int Data) = 0;
	
	virtual INT RotationTimeLineGetSize() = 0;
	virtual HRESULT RotationTimeLineGetKeyFrameByIndex( int *pnFrame, float *pData, DWORD dwIndex ) = 0;
	virtual HRESULT RotationTimeLineRemoveKeyFrame(INT nFrame) = 0;
	virtual HRESULT RotationTimeLineGetData(float *pData,float fFrame) = 0;
	virtual HRESULT RotationTimeLineIsKeyFrame(int nFrame) = 0;
	virtual HRESULT RotationTimeLineInsertKeyFrame(int nFrame,float& Data) = 0;

	virtual INT UVTimeLineGetSize() = 0;
	virtual HRESULT UVTimeLineGetKeyFrameByIndex( int *pnFrame, D3DXVECTOR2 *pData, DWORD dwIndex ) = 0;
	virtual HRESULT UVTimeLineRemoveKeyFrame(INT nFrame) = 0;
	virtual HRESULT UVTimeLineGetData(D3DXVECTOR2 *pData,float fFrame) = 0;
	virtual HRESULT UVTimeLineIsKeyFrame(int nFrame) = 0;
	virtual HRESULT UVTimeLineInsertKeyFrame(int nFrame,D3DXVECTOR2 Data) = 0;


    virtual void  SetWidht(float fValue) = 0;
    virtual float GetWidht() = 0;
    virtual void  SetHeight(float fValue) = 0;
    virtual float GetHeight() = 0;
    virtual void  InsertRtsKeyFrame(int nFrame, DWORD dwFlag) = 0;
    virtual void  SetBillboardType(DWORD dwType) = 0;
    virtual DWORD GetBillboardType() = 0;

	virtual void GetTranslation(D3DXVECTOR3* pValue) = 0;

	virtual DWORD GetRenderTargetTexFlag() = 0;
	virtual void SetRenderTargetTexFlag(DWORD val) = 0;

	virtual IEKG3DTexture* GetTexture() = 0;
	virtual IEKG3DTexture* GetTexture2() = 0;

	//virtual BOOL IsRotation() = 0;
	virtual DWORD GetTextureCutNum() = 0;
	virtual DWORD GetTexture2CutNum() = 0;
	//virtual BOOL IsLockAxis() = 0;
	//virtual HRESULT EnableLockAxis(BOOL bEnable) = 0;
	//virtual HRESULT EnableRotation(BOOL bEnable) = 0;

	virtual SFX_BLEND_MODE GetBlendMode() = 0;
	virtual void SetBlendMode(SFX_BLEND_MODE bm) = 0;

	virtual HRESULT LoadTexture2(const char szTextureName[]) = 0;
	virtual HRESULT LoadTexture(const char szTextureName[]) = 0;

	virtual TKG3DTimeLinebase* GetWidthTimeLine() = 0;
	virtual TKG3DTimeLinebase* GetHeightTimeLine() = 0;
	virtual TKG3DTimeLinebase* GetDiffuseTimeLine() = 0;
	virtual TKG3DTimeLinebase* GetTextureFrameIndex() = 0;
	virtual TKG3DTimeLinebase* GetTexture2FrameIndex() = 0;
	virtual TKG3DTimeLinebase* GetRotationTimeLine() = 0;
	virtual TKG3DTimeLinebase* GetUVTimeLine() = 0;
	virtual TKG3DTimeLinebase* GetPositionLine() = 0;

	virtual HRESULT SetTextureCutNum(DWORD dwCuttingNum) = 0;
	virtual HRESULT SetTexture2CutNum(DWORD dwCuttingNum) = 0;

    // editor
    virtual BOOL IsHide() = 0;
    virtual void Show(BOOL enable) = 0;
    virtual LPCTSTR GetName() = 0;
    virtual void SetName(LPCTSTR szName) = 0;

};


interface IEKG3DSfxExp 
{
    virtual IKG3DTimeLine* GetTimeControlTimeLine() = 0;
    virtual IKG3DTimeLine* GetCameraSwingTimeLine() = 0;
    virtual IKG3DTimeLine* GetCameraFrequencyTimeLine() = 0;
    virtual IKG3DTimeLine* GetModelScanlTimeLine() = 0;
};

struct IECOLOR_LINE_T
{
	virtual HRESULT GetData(D3DXCOLOR* pValue,float fFrame) = 0;
	virtual HRESULT IsKeyFrame(int nFrame) = 0;
	virtual HRESULT RemoveKeyFrame(int nFrame) = 0;
	virtual HRESULT InsertKeyFrame(int nFrame,D3DXCOLOR Data) = 0;

};

enum SFX_FORCE_FIELD_TYPE
{
	SFX_FFT_G,
	SFX_FFT_WIND,
	SFX_FFT_WHIRLWIND,
    SFX_FFT_ROTATION,
    SFX_FFT_BROWNIAN 
};

enum {
    SFX_FORCE_OPT_NORMAL,
    SFX_FORCE_OPT_FRANK

};
struct IEKG3DSFXParticleForceField
{
	virtual SFX_FORCE_FIELD_TYPE GetType() const = 0;
	virtual void SetType(SFX_FORCE_FIELD_TYPE eType) = 0;

    virtual HRESULT AddLauncherRef(IEKG3DSFXParticleLauncher* pLauncher) = 0;
    virtual HRESULT DelLauncherRef(IEKG3DSFXParticleLauncher* pLauncher) = 0;

	virtual TKG3DTimeLinebase* GetForceTimeLine()    = 0;
    virtual TKG3DTimeLinebase* GetForcePosTimeLine() = 0;
    virtual TKG3DTimeLinebase* GetForceGTimeLine()   = 0;

    // editor
    virtual BOOL IsHide() = 0;
    virtual void Show(BOOL enable) = 0;
    virtual LPCTSTR GetName() = 0;
    virtual void SetName(LPCTSTR szName) = 0;

};

struct IEKG3DSFXMeteor
{
	virtual float GetLife() = 0;
	virtual void SetLife(float fLife) = 0;

	virtual float GetWidth() = 0;
	virtual void SetWidth(float fWidth) = 0;

	virtual int GetBindTrackIdx() =0;
	virtual void SetBindTrackIdx(int nTrackIdx) = 0;

	virtual DWORD GetTextureIdx() =0;
	virtual void SetTextureIdx(DWORD idx) = 0;

	virtual float GetEmitMinInterval() = 0;
	virtual void SetEmitMinInterval(float fInterval) =0;

	virtual D3DXVECTOR3 GetOffset() =0;
	virtual void SetOffset(D3DXVECTOR3 vOffset) = 0;

	virtual BOOL IsEnableRandomOffset() = 0;
	virtual void EnableRandomOffset(BOOL bEnable) = 0;

	virtual SFX_BLEND_MODE GetBlendMode() = 0;
	virtual void SetBlendMode(SFX_BLEND_MODE BlendMode) = 0;
    
    virtual HRESULT SetMeteorInterpolate(float fInterval) = 0;
    virtual float GetMeteorInterpolate() = 0;

    virtual void SetDuration(float dur) = 0;
    virtual float GetDuration() = 0;
    
    virtual float GetRallyingScale() = 0;
    virtual HRESULT SetRallyingScale(float fScanl) = 0;

    virtual KGRTSTimeLine* GetRtsTimeLine() = 0;
    virtual void InsertRtsKeyFrame(int nFrame, DWORD dwFlag)  = 0;

	//by huangjinshou
	virtual void ClearListNode() = 0;
	virtual void AddListNode() = 0;
	virtual void SetSampling(BOOL bSampling) = 0;
	virtual BOOL GetSampling() = 0;
	///////////////////////////////////////////////////

    // editor
    virtual BOOL IsHide() = 0;
    virtual void Show(BOOL enable) = 0;
    virtual LPCTSTR GetName() = 0;
    virtual void SetName(LPCTSTR szName) = 0;
};


struct IEKG3DSFXMagicParpet
{
	virtual int GetRadius() = 0;
	virtual void SetRadius(int nRadius, int bBaseRadius = false) = 0;

    virtual DWORD GetType() = 0;
    virtual void SetType(DWORD t, bool bForce) = 0;
    virtual D3DXCOLOR GetCurrColor() = 0;

    virtual DWORD GetFaceType() = 0;
    virtual void SetFaceType(DWORD t) = 0;

	virtual float GetOffset() = 0;
	virtual void SetOffset(float fOffset) = 0;

	virtual int IsFaceCamera() = 0;
	virtual void EnableFaceCamera(int nFlag) = 0;
	virtual float GetHeightLimit() = 0;
	virtual void SetHeightLimit(float fLimit) = 0;

	virtual IEKG3DTexture* GetTexture() = 0;
	virtual HRESULT SetTexture(const char cszTextureName[]) = 0;
	virtual float GetLife() = 0;
	virtual void SetLife(float fLife) = 0;
    virtual IKG3DTimeLine* GetRadiusTimeLine() = 0;
    virtual IKG3DTimeLine* GetColorsTimeLine() = 0;
    virtual void SetTexCull(int x, int y) = 0;
    virtual void SetTexSpeed(float speed) = 0;
    virtual void GetTexCull(int* x, int* y) = 0;
    virtual float GetTexSpeed() = 0;


    // editor
    virtual BOOL IsHide() = 0;
    virtual void Show(BOOL enable) = 0;
    virtual LPCTSTR GetName() = 0;
    virtual void SetName(LPCTSTR szName) = 0;
};

enum
{
    BOX_PROJ_Y = 0,
    BOX_PROJ_X,
    BOX_PROJ_DURM,

    BOX_PROJ_COUNT
};

struct IKG3DSfxBoxProjection
{
    virtual DWORD GetProjType() = 0;
    virtual void  SetProjType(DWORD dwType) = 0;

    virtual IKG3DTimeLine* GetColorTimeLine() = 0;
    virtual IKG3DTimeLine* GetUVTimeLine() = 0;

    virtual SFX_BLEND_MODE GetBlendMode() = 0;
    virtual void SetBlendMode(SFX_BLEND_MODE eBlend) = 0;

    virtual void SetTexture(char szFileName[]) = 0;
    virtual void GetTexture(char szFileName[]) = 0;
    virtual void SetTexClip(int u, int v) = 0;
    virtual void GetTexClip(int& u, int& v) = 0;
    virtual void SetUVFrameSpeed(float fSpeed) = 0;
    virtual float GetUVFrameSpeed() = 0;
};

//////////////////////////////////////////////////////////////////////////
// Add Select Texture SFX   by huangjinshou 2007.11.15
interface IEKG3DSFXSelectTexture
{
	//virtual HRESULT Update() = 0;
	virtual HRESULT ShutDown() = 0;

	virtual IEKG3DTexture* GetTexture() = 0;
	virtual HRESULT LoadTexture(const char* szTextureFileName) = 0;
	virtual HRESULT SetTexture(IEKG3DTexture* pTexture) = 0;
	virtual HRESULT SetTexturePos(D3DXVECTOR3* pNewPos) = 0;
	virtual HRESULT GetTexturePos(D3DXVECTOR3* pPos) = 0;

	virtual DWORD GetTextureWidth() = 0;
	virtual DWORD GetTextureHeight() = 0;
	virtual void  SetTextureWidth(DWORD dwWidth) =0;
	virtual void  SetTextureHeight(DWORD dwHeight) = 0;
};
interface IEKG3DSFXSelectMark
{
	virtual IEKG3DTexture* GetTexture() = 0;
	virtual HRESULT LoadTexture(const char* szTextureFileName) = 0;

	virtual DWORD GetTextureWidth() = 0;
	virtual DWORD GetTextureHeight() = 0;
	virtual void  SetTextureWidth(DWORD dwWidth) =0;
	virtual void  SetTextureHeight(DWORD dwHeight) = 0;
	//virtual DWORD GetenFoot() = 0;
	//virtual void  SetenFoot(DWORD &dwVal) =0;
	//virtual	std::string GetFootprintType() = 0;
	//virtual void  SetFootprintType(std::string& scType) = 0;
};
//////////////////////////////////////////////////////////////////////////
class IKG3DModel;

class IEKG3DSFXSuede
{
public:
	virtual DWORD GetModelCount() = 0;
	virtual IKG3DModel* GetModel(DWORD dwIndex) = 0;
	virtual HRESULT DelModel(DWORD dwIndex) = 0;
};

interface IEKG3DMaterial;
interface IEKG3DModel;
interface IEKG3DSFXChannel;

enum {
    PARTICLE_TYPE_NORMAL = 0,
    PARTICLE_TYPE_METEOR = 1,
    PARTICLE_TYPE_PLIGHT = 2
};

enum {
    SHOCK_WAVE_ONLY,
    SHOCK_WAVE_ADD
};


struct TagSourceInfo;
class IEKG3DSFX
{
public:
	virtual void SetScaleSize(float f) = 0;
	virtual float GetScaleSize() = 0;
	virtual DWORD GetParticleLauncherNum() const = 0;

	virtual HRESULT GetParticleLauncher(
		/* [in] */  DWORD dwIndex,
		/* [out] */ IEKG3DSFXParticleLauncher **ppParticleLauncher) = 0;

	virtual DWORD GetTrackNum() const = 0;

	virtual HRESULT GetTrack(
		/* [in] */ DWORD dwIndex,
		/* [out] */ IEKG3DSFXTrack **ppTrack
		) = 0 ;

	virtual HRESULT PlayAnimation(SFX_ANIMATION_PLAY sfxAnimationPlay, FLOAT fSpeed, int nOffsetTime) = 0;

	virtual DWORD GetBillboardNum() const = 0;

	virtual HRESULT GetBillboard(
		/* [in] */ DWORD dwIndex,
		/* [out] */ IEKG3DSFXBillboard **ppBillboard
		) = 0;

	virtual HRESULT GetForceField(
		/* [out] */ DWORD dwIndex,
		/* [out] */ IEKG3DSFXParticleForceField **ppForceField
		) = 0;

	virtual HRESULT GetMeteor(DWORD dwIndex, IEKG3DSFXMeteor **ppMeteor) =0;

	virtual HRESULT AddMagicParpet(IEKG3DSFXMagicParpet **ppMagicParpet, DWORD dwType) = 0;

	virtual HRESULT AddParticleLifeTimeLine(
		/* [out] */ IEKG3DSFXParticleLifeTimeLine **ppLifeTimeLine
		) = 0;

	virtual HRESULT GetParticleLifeTimeLine(
		/* [in] */  DWORD dwIndex,
		/* [out] */ IEKG3DSFXParticleLifeTimeLine **ppLifeTimeLine
		) = 0;

	virtual HRESULT AddSubsetColorDiffuseLine(DWORD dwIndex, D3DCOLORVALUE initColor) = 0;
	virtual HRESULT DeleteSubsetColorDiffuseLine(DWORD dwIndex) = 0;

	virtual HRESULT AddMutiSubsetColorDiffuseLine(DWORD dwIndex, IEKG3DMaterial *piMat) = 0;

	virtual HRESULT AddTimeLineModelAniBound() = 0;
	virtual TKG3DTimeLinebase* GetSubsetColorLine(DWORD dwSubset,DWORD dwIndex) = 0;
	virtual int GetSubsetColorLineSize(DWORD dwSubset) = 0;

	virtual HRESULT GetIKG3DTexture(UINT uTextureIndex, IKG3DTexture **ppTexture) const = 0;

	virtual int GetIsCloseFloor() = 0;    
	virtual int GetIsShockWave() = 0;  
    virtual void SetShockMode(DWORD mode) = 0;
    virtual DWORD GetShockMode() = 0;
	virtual HRESULT SetIsShockWave(int nValue) = 0;    
    virtual DWORD   GetMaxParticleNum() const = 0;
    virtual void    SetMaxParticleNum(DWORD dwNum) = 0;

	virtual HRESULT ReflushTexture() = 0;

	virtual DWORD GetTotalFrameNum() const = 0;
	virtual DWORD GetFrameInterval() const = 0;
	virtual float GetCurrentFrame() const = 0;
	virtual HRESULT SetTotalFrameNum(DWORD dwTotalFrame) = 0;
	virtual HRESULT SetFrameInterval(DWORD dwInterval) = 0;
	virtual HRESULT SetCurrentFrame(float fFrame) = 0;
	virtual void TransformationFrameMove() = 0;

	
//	virtual IEKG3DSFXSelectTexture* GetProTexture() = 0; //by huangjinshou 2008 1-18
    virtual size_t GetMagicParpetNum() = 0;
	virtual IEKG3DSFXMagicParpet* GetMagicParpet(int index) = 0;
	virtual HRESULT RemoveMagicParpet(int index) = 0;
    virtual HRESULT RemoveMagicParpet(IEKG3DSFXMagicParpet* aoe) = 0;

    virtual size_t GetBoxProjectionNum() = 0;
    virtual HRESULT AddBoxProjection(IKG3DSfxBoxProjection** ppBoxProjection) = 0;
    virtual HRESULT RemoveBoxProjection(IKG3DSfxBoxProjection* pBoxProjection) = 0;
    virtual HRESULT RemoveBoxProjection(int index) = 0;
    virtual IKG3DSfxBoxProjection* GetBoxProjection(int index) = 0;

	virtual DWORD GetMeteorNum() const = 0;
    virtual DWORD GetMeteorBindTrackIndex(DWORD dwIndex) = 0;

	virtual void ClearMeteorListNode() = 0;//by huangjinshou 2007 8.1
	virtual void SetMeteorSampling(BOOL val) = 0;
	virtual int GetTimeLineModelAniBoundSize() = 0;

	virtual TKG3DTimeLinebase* GetModelAniBoundTimeLine(DWORD dwIndex) = 0;
    virtual TKG3DTimeLinebase* GetSoundTimeLine() = 0;

	virtual DWORD GetMdlNumFrame(size_t index) = 0;
	virtual FLOAT GetNdlFrameTime(size_t index) = 0;
	virtual int IsMdlAniConnect(size_t index) = 0;

	virtual IKG3DModel* GetModel(size_t index) = 0;
	virtual HRESULT GetModelAnimationSpeed(float *pValue, size_t index) = 0;
	virtual HRESULT GetModelAnimationStart(float *pValue, size_t index) = 0;
	virtual HRESULT GetModelAnimationMode(DWORD *pValue, size_t index) = 0;
	virtual HRESULT GetModelAnimationFrameCount(DWORD* pValue, size_t index) = 0;
	virtual HRESULT GetModelAnimationFrameTime(float* pValue, size_t index) = 0;

	virtual void SetModelAnimationFrameTime(float Value, size_t index) = 0;
	virtual void SetModelAnimationFrameCount(DWORD Value, size_t index) = 0;
	virtual void SetModelAnimationSpeed(float  Value, size_t index) = 0;
	virtual void SetModelAnimationStart(float Value, size_t index) = 0;
	virtual void SetModelAnimationMode(DWORD Value, size_t index) = 0;

	virtual DWORD GetNumModels() const = 0;
    virtual HRESULT GetModel(size_t index, IEKG3DModel **ppModel) = 0;
    virtual HRESULT GetModelBindTrackIndex(size_t index, int* pTrackIndex) = 0;
	virtual HRESULT EnagleMdlAniConnect(int nEnagle, size_t index) = 0;
    virtual HRESULT EnagleMdlBillBoard(int nEnagle,  size_t index) = 0;
    virtual BOOL   IsMdlBillBoard(size_t index) = 0;

	virtual HRESULT RemoveForceField( /* [in] */ DWORD dwIndex ) = 0;
	virtual	HRESULT RemoveParticleLifeTimeLine(
		/* [in] */ DWORD dwIndex
		) = 0;
	virtual DWORD GetTextureNum() const = 0;
	virtual DWORD GetForceFieldNum() const = 0;
	virtual DWORD GetParticleLifeTimeLineNum() const = 0;

	virtual HRESULT AddForceField(
		/* [in] */  SFX_FORCE_FIELD_TYPE eForceFieldType,
		/* [out] */ IEKG3DSFXParticleForceField **ppForceField) = 0;

	virtual HRESULT GetTextureCutNum(UINT uTextureIndex, UINT *pCutNum) const = 0;
	virtual HRESULT RemoveTexture(UINT uTextureIndex) = 0;
	virtual HRESULT AddTexture(const char szTextureName[]) = 0;
	virtual HRESULT SetTexture(UINT uTextureIndex, const char szTextureName[]) = 0;
	virtual HRESULT SetTextureCutNum(UINT uTextureIndex, UINT uCuttingNum) = 0;

	virtual HRESULT AddChannel(IEKG3DSFXChannel** ppChannel) = 0;
	virtual HRESULT RemoveChannel() = 0;
	virtual HRESULT GetChannel(IEKG3DSFXChannel ** pChannel) = 0;

    virtual DWORD   IsFixedSize() = 0;
    virtual void    EnableEixedSize(DWORD dwEnable) = 0;

    virtual DWORD   IsLoacAxisY() = 0;
    virtual void    EnableLockY(DWORD dwEnable) = 0;

    virtual HRESULT InsertSoundKey(LPCTSTR szFileName,     
                                   DWORD  dwMode,
                                   FLOAT  fVolume,
                                   FLOAT  fSFXKey,
                                   FLOAT  fMinDis,
                                   FLOAT  fMaxDis,
                                   BOOL   bLoop,
                                   BOOL   bContinue,
                                   int    nProbability,
                                   int    nSubIndex
                                   ) = 0;
    virtual HRESULT RemoveSoundKey(FLOAT fSFXKey, int nSubIndex) = 0;

    virtual HRESULT ModifySoundKey(LPCTSTR szFileName,
                                   DWORD  dwMode,
                                   FLOAT  fVolume,
                                   FLOAT  fSFXKey,
                                   FLOAT  fMinDis,
                                   FLOAT  fMaxDis,
                                   BOOL   bLoop,
                                   BOOL   bContinue,
                                   int    nProbability,
                                   int    nSubIndex
                                   ) = 0;
    virtual HRESULT QuerySoundKey(LPTSTR  szFileName,
                                  DWORD&  dwMode,
                                  FLOAT&  fVolume,
                                  FLOAT   fSFXKey,
                                  FLOAT&  fMinDis,
                                  FLOAT&  fMaxDis,
                                  BOOL&   bLoop,
                                  BOOL&   bContinue,
                                  int&    nProbability,
                                  int     nSubIndex
                                  ) = 0;
    virtual int GetSoundKeyNum(FLOAT fSFXKey) = 0;

	virtual HRESULT AddSelectMark(IEKG3DSFXSelectMark **ppSelectMark) =0;
	virtual HRESULT RemoveSelectMark() = 0;
	virtual IEKG3DSFXSelectMark* GetSelectMark() =0;

    virtual IEKG3DSfxExp* GetExpEffect() = 0;

    virtual LPCTSTR GetName() = 0;

    virtual void  SetShockWavePower(float power) = 0;
    virtual float GetShockWaePower()             = 0;

	virtual DWORD GetSourceReference(TagSourceInfo *pInfo, DWORD dwSize) = 0;
	virtual HRESULT ChangeResource(UINT uFrame, UINT uIndex, LPCSTR strNew) = 0;
	virtual HRESULT DeleteResource(UINT uFrame, UINT uIndex) = 0;
	virtual HRESULT AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault) = 0;
	virtual HRESULT ChangePlayType(UINT uFrame, UINT uIndex, void *pDefault) = 0;
	virtual HRESULT DeleteAllSound() = 0;
};

struct IEKG3DSFXParticleLifeTimeLine
{
	virtual UINT GetTextureIndex() = 0;
	virtual void SetTextureIndex(UINT uValue) = 0;

	virtual float GetRotationFactor() =0;
	virtual void  SetRotationFactor(float fValue) = 0;

    virtual WORD GetParticleType() = 0;
    virtual WORD GetParticleFlag() = 0;
    virtual void SetParticleType(const WORD wType) = 0;
    virtual void SetParticleFlag(const WORD wFlag) = 0;
    virtual DWORD GetSmooth() = 0;
    virtual void  SetSmooth(DWORD dw) = 0;

	//virtual HRESULT WidthTimeLineGetData(float* pValue,float fFrame) = 0;

	//virtual HRESULT HeightTimeLineGetData(float* pValue,float fFrame) = 0;
	//virtual HRESULT RotationTimeLineGetData(float* pValue,float fFrame) = 0;
	//virtual HRESULT ColorTimeLineGetData(D3DXCOLOR* pValue,float fFrame) = 0;
	//virtual HRESULT TextureFrameTimeLineGetData(int* pValue,float fFrame) = 0;

	virtual TKG3DTimeLinebase* GetColorTimeLineL() = 0;
    virtual TKG3DTimeLinebase* GetColorTimeLineR() = 0;
	virtual TKG3DTimeLinebase* GetWidthTimeLine() = 0;
	virtual TKG3DTimeLinebase* GetHeightTimeLine() = 0;
	virtual TKG3DTimeLinebase* GetRotationTimeLineMin() = 0;
	virtual TKG3DTimeLinebase* GetRotationTimeLineMax() = 0;
	virtual TKG3DTimeLinebase* GetTextureFrameTimeLine() = 0;
};

