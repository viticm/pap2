#ifndef CASE_RENDER_KANIMATION_MODEL_H
#define CASE_RENDER_KANIMATION_MODEL_H

#include "./irlmodel.h"

enum { RL_SOCKET_NAME_LENGTH = 16 };
enum { RL_MODEL_SLOT_COUNT = 4 };

struct KRLModelInfo
{
    IKG3DModel*	p3DModel;
    BOOL bVisible;
    BOOL bSelectable;
    BOOL bBind;

    D3DXVECTOR3 vPosition;

    TCHAR szSocket[16];
    TCHAR szFilePath[MAX_PATH];

    KRLModelInfo() : p3DModel(NULL), bVisible(FALSE), bSelectable(FALSE), bBind(TRUE)
    {
        vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

        szSocket[0] = _T('\0');
        szFilePath[0] = _T('\0');
    }
};

typedef std::vector<KRLModelInfo> KRLModelInfoList;

struct KRLModelData
{
    KRLModelInfo RLMDL;

    KRLModelInfoList RLModels;
    KRLModelInfoList RLSockets;
    KRLModelInfoList RLMDLSFXs;
    KRLModelInfoList RLEquipSFXs;
    KRLModelInfoList RLSocketSFXs;
};

struct KRLModelSlot
{
    TCHAR szSocket[RL_SOCKET_NAME_LENGTH];

    IKG3DModel*	p3DModel;

    double fExpireTime;
};

class KAnimationModel : public IRLModel
{
public:
	KAnimationModel();
	virtual ~KAnimationModel();

    HRESULT TransferOwnershipFrom(KAnimationModel& rhs);

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

    HRESULT Init(KRLSceneObject* pRLSceneObject, size_t nModelCount, size_t nSocketCount, size_t nMDLSFXCount);
	HRESULT AttachScene(KRLScene* pRLScene);
	HRESULT DetachScene();

    HRESULT UnloadEquipSFX(size_t nIndex);
	HRESULT UnloadSocketSFX(size_t nIndex);

    HRESULT BindTo(IKG3DModel* p3DModelTo, LPCTSTR szSocket);
    HRESULT UnbindFrom(IKG3DModel* p3DModelFrom);

	HRESULT SetModelAlpha(size_t nIndex, float fAlpha);
	HRESULT SetSocketAlpha(size_t nIndex, float fAlpha);
	HRESULT SetMDLSFXAlpha(size_t nIndex, float fAlpha);

    int IsReady() const;

    int RemoveCaption();

private:
	KAnimationModel& operator=(KAnimationModel const& rhs);

	BOOL LookupModel(IKG3DModel* p3DModel, size_t& nIndex) const;
	BOOL LookupSocket(IKG3DModel* p3DModel, size_t& nIndex) const;
	BOOL LookupMDLSFX(IKG3DModel* p3DModel, size_t& nIndex) const;
    BOOL LookupEquipSFX(IKG3DModel* p3DModel, size_t& nIndex) const;
	BOOL LookupSocketSFX(IKG3DModel* p3DModel, size_t& nIndex) const;

    HRESULT UpdateSlots();

private:
	KRLSceneObject* m_pRLSceneObject;
	
	mutable KRLScene* m_pRLScene;

	KRLModelData m_ModelData;

    KRLModelSlot m_ModelSlots[RL_MODEL_SLOT_COUNT];
};



#endif // CASE_RENDER_KANIMATION_MODEL_H