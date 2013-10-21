#ifndef CASE_RENDER_KRLLOADINGMODEL_H
#define CASE_RENDER_KRLLOADINGMODEL_H

#include "./irlmodel.h"

class KAnimationModel;

class KRLLoadingModel : public IRLModel
{
public:
    KRLLoadingModel();
    virtual ~KRLLoadingModel();

	virtual HRESULT Reset();
    virtual HRESULT Update();

	virtual KRLScene* GetRLScene() const;

	virtual HRESULT SetMDLSelectable(BOOL bSelectable);
	virtual HRESULT SetMDLVisible(BOOL bVisible);
    virtual HRESULT SetMDLSFXInfo(size_t nIndex, BOOL bBind);

	virtual HRESULT LoadMDL(LPCTSTR szFilePath, float fScale, int nColorChannelTable, int nColorChannel);
	virtual HRESULT UnloadMDL();

	virtual HRESULT LoadModel(size_t nIndex, LPCTSTR szMesh, LPCTSTR szMaterial, float fScale, int nColorChannelTable, int nColorChannel);
	virtual HRESULT UnloadModel(size_t nIndex);

	virtual HRESULT LoadSocket(size_t nIndex, LPCTSTR szSocket, LPCTSTR szMesh, LPCTSTR szMaterial, float fScale, int nColorChannelTable, int nColorChannel);
	virtual HRESULT UnloadSocket(size_t nIndex);

    virtual HRESULT LoadMDLSFX(size_t nIndex, KRLSFXParam const& SFXParam);
	virtual HRESULT UnloadMDLSFX(size_t nIndex);

    virtual HRESULT LoadEquipSFX(size_t nIndex, LPCTSTR szFilePath, float fScale);
	virtual HRESULT LoadSocketSFX(size_t nIndex, LPCTSTR szFilePath, float fScale);

	virtual HRESULT UnloadAllModel();
	virtual HRESULT UnloadAllSocket();
	virtual HRESULT UnloadAllMDLSFX();
    virtual HRESULT UnloadAllEquipSFX();
	virtual HRESULT UnloadAllSocketSFX();

    virtual int IsReady() const;

	virtual BOOL HasLoadedMDL() const;
	virtual BOOL HasLoadedModel(size_t nIndex) const;
	virtual BOOL HasLoadedSocket(size_t nIndex) const;
	virtual BOOL HasLoadedMDLSFX(size_t nIndex) const;
    virtual BOOL HasLoadedEquipSFX(size_t nIndex) const;
	virtual BOOL HasLoadedSocketSFX(size_t nIndex) const;

    virtual BOOL IsVisible() const;
    virtual BOOL IsMDLSelectable() const;

    virtual HRESULT SetMDLSFXPosition(size_t nIndex, D3DXVECTOR3 vPosition);
    virtual HRESULT SetMDLSFXOffsetDirection(size_t nIndex, D3DXVECTOR3 vOffset);

	virtual HRESULT BindSocket(size_t nIndex, LPCTSTR szSocket);
	virtual HRESULT UnbindSocket(size_t nIndex);

    virtual HRESULT AppendSlot(LPCTSTR szSocket, IKG3DModel* p3DModel, double fExpireTime);
    virtual HRESULT ClearSlots();

	virtual HRESULT SetScale(float fScale);

	virtual HRESULT SetAlpha(float fAlpha);
	
    virtual HRESULT SetPosition(D3DXVECTOR3 const& vPosition);
	virtual HRESULT GetPosition(D3DXVECTOR3& vPosition) const;

	virtual HRESULT SetRotation(D3DXQUATERNION const& qRotation);
	virtual HRESULT GetRotation(D3DXQUATERNION& qRotation) const;

    virtual HRESULT GetBonePosition(D3DXVECTOR3& vPosition, D3DXVECTOR3 vOffset, LPCTSTR szBone) const;

	virtual HRESULT GetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const;

    virtual HRESULT SetPercentageVisible(BOOL bVisible);
    virtual HRESULT SetPercentage(DWORD dwColor, float fPercentage);
    
	virtual HRESULT SetCaptionVisible(DWORD dwIndex, BOOL bVisible);
	virtual HRESULT SetCaption(DWORD dwIndex, LPCSTR szContent);
	virtual HRESULT SetCaptionColor(DWORD dwIndex, DWORD dwColor);
	virtual HRESULT SetCaptionFloorHeight(float fHeight);

    virtual HRESULT PauseAnimation(BOOL bPause);
	virtual HRESULT PlayMonoAnimation(KRLMonoAnimationParam const& Param);
	virtual HRESULT PlaySplitAnimation(KRLSplitAnimationParam const& Param);

    virtual HRESULT SeekMonoAnimation(KRLMonoAnimationParam const& Param);
    virtual HRESULT SeekSplitAnimation(KRLSplitAnimationParam const& Param);

    virtual HRESULT SetMonoAnimationSpeed(float fSpeed);
    virtual HRESULT SetSplitAnimationSpeed(DWORD dwPartIndex, float fSpeed);

    virtual HRESULT AnimationFinished(IKG3DModel* p3DModel);

    virtual HRESULT BindTo(IRLModel* pRLModelTo, LPCTSTR szSocket);
    virtual HRESULT UnbindFrom(IRLModel* pRLModelFrom);

	virtual HRESULT SetBoneTransform(LPCSTR szBone, BoneTransformationInputData const& Data);
	virtual HRESULT EnableBoneTransform(BOOL bEnable);

private:
    KAnimationModel* m_pRLModel;
    KAnimationModel* m_pLoadingRLModel;
};

#endif // CASE_RENDER_KRLLOADINGMODEL_H
