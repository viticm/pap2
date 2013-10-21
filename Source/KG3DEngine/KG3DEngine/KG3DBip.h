////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBip.h
//  Version     : 1.0
//  Creator     : huangshan
//  Create Date : 2006-11-5 11:52:20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DBIP_H_
#define _INCLUDE_KG3DBIP_H_

#include "KG3DAnimationController.h"
#include "IEKG3DBip.h"
#include "KG3DAnimationWarper.h"
#include "KG3DRTSInfoCollector.h"

class KG3DMesh;
class KG3DModel;
class KG3DClipTools;
class KG3DAnimationWarper;
class KG3DAnimationComposer;

class KG3DBip : public IEKG3DBip
{
	friend class KG3DClipTools;
	friend class KG3DModelST;
	friend class KG3DMesh;
	friend class KG3DModel;
	friend class KG3DFuctionCommand;

public://for IEKG3DBip interface
    virtual HRESULT GetBoneInfoDataPoint(BoneInfoData **ppBoneInfo);
    virtual HRESULT GetBaseBoneCount(DWORD *pdwCount);
    virtual HRESULT GetBaseBoneByPosition(DWORD dwPositon, INT *pnBaseBone);

	KG3DBip(void);
	~KG3DBip(void);

    HRESULT LoadFromTextFile(const char cszFileName[]);

	const D3DXMATRIX& GetBoneObjMatrix(unsigned int uIndex);
	int FindBone(const TCHAR* strBoneName) const;
	void FrameMove(KG3DAnimationController* pControllers);
	void Render();
	HRESULT PlayAnimation(const TCHAR* strFileName, 
        DWORD PlayType, 
        int nOffsetTime, 
        float fSpeed, 
        void* pExtInfo,
		enuAnimationControllerPriority Priority,
		KG3DAnimationController* pControllers,
		KG3DClip* pClip);

	HRESULT ChangeAnimation(IEKG3DClip *pClip,
		float fSpeed,
		DWORD dwType,
        DWORD dwTweenTime,
		float fTweenWeight,
		enuAnimationControllerPriority Src,
		enuAnimationControllerPriority Dest,
		KG3DAnimationController* pControllers);

	KG3DAnimationComposer::TweenTimeInfo* m_pTweenTimeInfo;

	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	void OnModelSTAttach(KG3DModel *pModel,KG3DAnimationController* pControllers);
	int          GetNumBones() const;
	const TCHAR* GetBoneName(int nIndex) const;
	int          GetParentIndex(int nIndex) const;
	int          GetNumChildren(int nIndex) const;
	int          GetChildIndex(int nBoneIndex, int nChildIndex) const;
	const std::vector<int>& GetBaseBoneID() const;
    HRESULT GetSkeleton(vector<int>& vSkeleton, int nBoneIndex);
    int GetNumBaseBone() const;
    int GetBaseBone(int nIndex) const;

	HRESULT SetBoneScale(LPCSTR strBoneName, D3DXVECTOR3& vScale);
	HRESULT GetBoneScale(LPCSTR strBoneName, D3DXVECTOR3* pvScale);

    HRESULT SetBoneTransform(LPCSTR strBoneName, const BoneTransformationInputData& Data);
    void EnableBoneTransform(BOOL bEnable) { m_bEnableBoneTransform = bEnable; }
	void EnableBoneScale(BOOL bEnable) { m_bEnableBoneScale = bEnable; }
    BOOL IsBoneTransformEnable() { return m_bEnableBoneTransform; }
	BOOL IsBoneScaleEnable() { return m_bEnableBoneScale; }
	const TCHAR* GetBipFileName() const { return m_scName.c_str(); }
    void Clone(const KG3DBip *pSrc);
	enuAnimationControllerPriority GetCurrentPriority(KG3DAnimationController* pControllers);
    D3DXMATRIX *GetObjMatricesArray() { return m_pObjMatrices; }

	HRESULT GenerateTweenInfo(KG3DAnimationComposer::TweenTimeInfo* pTweenTimeInfo,KG3DAnimationController* pAniController,BOOL bStart);
private:
    std::map<int, float> m_BoneTransforms;
	std::map<int,D3DXVECTOR3>m_BoneScale;
private:
	HRESULT _PlayAnimation(KG3DClip* pClip, 
		DWORD PlayType, 
		int nOffsetTime,
		float fSpeed, 
		TweenTimeSpan Span,
		enuAnimationControllerPriority Priority,
		KG3DAnimationController* pControllers);

    struct _CHILD_BONE 
    {
        char szChildName[MAX_NUM_CHILD_BONE][MAX_BONE_NAME_LENGTH];
    };

    HRESULT InitTweenData();
    HRESULT UnInitTweenData();

    HRESULT _LineToBoneInfo(const char szLineBuffer[], BoneInfoData *pRetBoneInfo, _CHILD_BONE *pRetChildBone);
    HRESULT _ConstructSkeleton(int nIndex, const _CHILD_BONE &crChildBone);
	void CleanUp();
	void Helper_UpdateToObj(int nIndex, const D3DXMATRIX& matBase);
	int           m_nNumBones;
	BoneInfoData* m_pSkeleton;
    D3DXMATRIX*   m_pObjMatrices;

	ULONG         m_ulRefCount;
	std::vector<int> m_BaseBones;
    BOOL          m_bEnableBoneTransform;
	BOOL          m_bEnableBoneScale;
	KG3DRTSInfoCollector *m_pRTSCollector;
	TweenTimeSpan m_TweenSpan;
	enuAnimationControllerPriority m_CurrentPriority;
	KG3DFuctionCommand m_Commands;

	HRESULT ProcessCommands();

public:
	HRESULT OnSecondaryAnimationFinish();
	DWORD m_dwID;
	void SetModelST(KG3DModelST* pST)
	{
		m_lpOwenerModelST = pST;
	}
private:
	std::string m_scName;
	BOOL m_bInTween;
	KG3DModelST* m_lpOwenerModelST;

protected:
	void UpdateToObj();
	BOOL OnProcessCommands(KG3DModelCommandBase* pCmd);
};

#endif //_INCLUDE_KG3DBIP_H_
