#ifndef CASE_RENDER_IRLMODEL_H
#define CASE_RENDER_IRLMODEL_H

struct BoneTransformationInputData;
struct KRLSFXParam;
struct KRLMonoAnimationParam;
struct KRLSplitAnimationParam;

class IKG3DModel;
class KRLSceneObject;
class KRLScene;
class KAnimationModel;

class IRLModel
{
public:
	virtual HRESULT Reset() = 0;
    virtual HRESULT Update() = 0;

	virtual KRLScene* GetRLScene() const = 0;

	virtual HRESULT SetMDLSelectable(BOOL bSelectable) = 0;
	virtual HRESULT SetMDLVisible(BOOL bVisible) = 0;
    virtual HRESULT SetMDLSFXInfo(size_t nIndex, BOOL bBind) = 0;

	virtual HRESULT LoadMDL(LPCTSTR szFilePath, float fScale, int nColorChannelTable, int nColorChannel) = 0;
	virtual HRESULT UnloadMDL() = 0;

	virtual HRESULT LoadModel(size_t nIndex, LPCTSTR szMesh, LPCTSTR szMaterial, float fScale, int nColorChannelTable, int nColorChannel) = 0;
	virtual HRESULT UnloadModel(size_t nIndex) = 0;

	virtual HRESULT LoadSocket(size_t nIndex, LPCTSTR szSocket, LPCTSTR szMesh, LPCTSTR szMaterial, float fScale, int nColorChannelTable, int nColorChannel) = 0;
	virtual HRESULT UnloadSocket(size_t nIndex) = 0;

    virtual HRESULT LoadMDLSFX(size_t nIndex, KRLSFXParam const& SFXParam) = 0;
	virtual HRESULT UnloadMDLSFX(size_t nIndex) = 0;

    virtual HRESULT LoadEquipSFX(size_t nIndex, LPCTSTR szFilePath, float fScale) = 0;
	virtual HRESULT LoadSocketSFX(size_t nIndex, LPCTSTR szFilePath, float fScale) = 0;

	virtual HRESULT UnloadAllModel() = 0;
	virtual HRESULT UnloadAllSocket() = 0;
	virtual HRESULT UnloadAllMDLSFX() = 0;
    virtual HRESULT UnloadAllEquipSFX() = 0;
	virtual HRESULT UnloadAllSocketSFX() = 0;

	virtual BOOL HasLoadedMDL() const = 0;
	virtual BOOL HasLoadedModel(size_t nIndex) const = 0;
	virtual BOOL HasLoadedSocket(size_t nIndex) const = 0;
	virtual BOOL HasLoadedMDLSFX(size_t nIndex) const = 0;
    virtual BOOL HasLoadedEquipSFX(size_t nIndex) const = 0;
	virtual BOOL HasLoadedSocketSFX(size_t nIndex) const = 0;

    virtual BOOL IsVisible() const = 0;
    virtual BOOL IsMDLSelectable() const = 0;

    virtual HRESULT SetMDLSFXPosition(size_t nIndex, D3DXVECTOR3 vPosition) = 0;
    virtual HRESULT SetMDLSFXOffsetDirection(size_t nIndex, D3DXVECTOR3 vOffset) = 0;

	virtual HRESULT BindSocket(size_t nIndex, LPCTSTR szSocket) = 0;
	virtual HRESULT UnbindSocket(size_t nIndex) = 0;

    virtual HRESULT AppendSlot(LPCTSTR szSocket, IKG3DModel* p3DModel, double fExpireTime) = 0;
    virtual HRESULT ClearSlots() = 0;

	virtual HRESULT SetScale(float fScale) = 0;

	virtual HRESULT SetAlpha(float fAlpha) = 0;
	
    virtual HRESULT SetPosition(D3DXVECTOR3 const& vPosition) = 0;
	virtual HRESULT GetPosition(D3DXVECTOR3& vPosition) const = 0;

	virtual HRESULT SetRotation(D3DXQUATERNION const& qRotation) = 0;
	virtual HRESULT GetRotation(D3DXQUATERNION& qRotation) const = 0;

    virtual HRESULT GetBonePosition(D3DXVECTOR3& vPosition, D3DXVECTOR3 vOffset, LPCTSTR szBone) const = 0;

	virtual HRESULT GetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const = 0;

    virtual HRESULT SetPercentageVisible(BOOL bVisible) = 0;
    virtual HRESULT SetPercentage(DWORD dwColor, float fPercentage) = 0;

    virtual HRESULT SetCaptionVisible(DWORD dwIndex, BOOL bVisible) = 0;
	virtual HRESULT SetCaption(DWORD dwIndex, LPCSTR szContent) = 0;
	virtual HRESULT SetCaptionColor(DWORD dwIndex, DWORD dwColor) = 0;
	virtual HRESULT SetCaptionFloorHeight(float fHeight) = 0;

    virtual HRESULT PauseAnimation(BOOL bPause) = 0;
	virtual HRESULT PlayMonoAnimation(KRLMonoAnimationParam const& Param) = 0;
	virtual HRESULT PlaySplitAnimation(KRLSplitAnimationParam const& Param) = 0;

    virtual HRESULT SeekMonoAnimation(KRLMonoAnimationParam const& Param) = 0;
    virtual HRESULT SeekSplitAnimation(KRLSplitAnimationParam const& Param) = 0;

    virtual HRESULT SetMonoAnimationSpeed(float fSpeed) = 0;
    virtual HRESULT SetSplitAnimationSpeed(DWORD dwPartIndex, float fSpeed) = 0;

    virtual HRESULT AnimationFinished(IKG3DModel* p3DModel) = 0;

    virtual HRESULT BindTo(IRLModel* pRLModelTo, LPCTSTR szSocket) = 0;
    virtual HRESULT UnbindFrom(IRLModel* pRLModelFrom) = 0;

	virtual HRESULT SetBoneTransform(LPCSTR szBone, BoneTransformationInputData const& Data) = 0;
	virtual HRESULT EnableBoneTransform(BOOL bEnable) = 0;
};


#endif // CASE_RENDER_IRLMODEL_H
