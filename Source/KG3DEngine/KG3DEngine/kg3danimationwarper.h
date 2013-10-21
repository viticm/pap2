#pragma once

#include "KG3DClip.h"
#include "KG3DAnimationController.h"
#include "IEKG3DAnimationWarper.h"

class KG3DModel;
class KG3DModelST;
class KG3DRTSInfoCollector;

class KG3DSkeleton
{
    struct Bone
    {
        int nBone;
        int nParent;
        std::vector<int> Children;
        TCHAR strBoneName[MAX_BONE_NAME_LENGTH];
        unsigned char ReserveBuffer[8]; // make the sizeof struct to 64, more quick when use struct array
    };

public:
    struct SerializeData
    {
        DWORD dwMask;
        DWORD dwNumBones;
        SerializeData();
        const static DWORD cdwMask = 0x3f3f3f3f;
    };

    int FindBone(LPCSTR strBoneName);
    void Create(KG3DModel* pModel);
    int GetParnet(int nIndex);
    int GetNumChildren(int nIndex)
    {
        ASSERT(nIndex < static_cast<int>(m_Skeleton.size()));
        return static_cast<int>(m_Skeleton[nIndex].Children.size());
    }

    int GetChild(int nBone, int nChild)
    {
        ASSERT(nBone < static_cast<int>(m_Skeleton.size()));
        ASSERT(nChild < static_cast<int>(m_Skeleton[nBone].Children.size()));
        return m_Skeleton[nBone].Children[nChild];
    }

    int GetNumBaseBone();
    int GetBaseBone(int nIndex);
    int GetNumBones()
    {
        return static_cast<int>(m_Skeleton.size());
    }

    LPCSTR GetBoneName(int nIndex);
private:
    void CreateModelST(KG3DModel *pModel);
    std::vector<Bone> m_Skeleton;
    std::vector<int> m_BaseBones;
};

class KG3DAnimationComposer : public IEKG3DAnimationComposer
{
    struct BoneInfoItem
    {
        DWORD                                  dwIndex;//Index in animation warper animation list;
        KG3DAnimationUpdateExtraInfoCollector* pCollector;
        float                                  fWeight;
        DWORD                                  dwType;
        BoneInfoItem();
    };

    struct BoneInfo
    {
        RTS rts;
        std::vector<BoneInfoItem> Info;
    };

public:
    enum 
    {
        MAX_COMPOSE = 8,
    };

    struct BoneInfoSerializeData
    {
        DWORD dwCollectorIndex;
        float fWeight;
        DWORD dwType;
    };

	struct TweenTimeInfo
	{
		RTS* pTweenStartInfo;
		RTS* pTweenEndInfo;
		DWORD dwStartTime;//Tween开始时间
		DWORD dwNumBones;
		DWORD dwSpan;//Tween持续时间
		TweenTimeInfo();
		HRESULT Init(DWORD dwNumBones);
		void UnInit();
	};

    struct SerializeData
    {
        DWORD dwMask;
        DWORD dwVersion;
        DWORD dwNumBones;
        DWORD dwNumItemPerBone;
        DWORD dwBlockLength;

        SerializeData();
        void Init();
        ~SerializeData();
        const static DWORD cdwMask = 0x43505344;//"CPSD" ComPoser Serialize Data
        const static DWORD cdwVersion = 0;
    };


    KG3DAnimationComposer();
    ~KG3DAnimationComposer();
	HRESULT Init(KG3DSkeleton *pSkeleton, KG3DModel *pModel);
    void SetSkeleton(KG3DSkeleton* pSkeleton);
	void CreateTweenCollector(KG3DModel *pModel);
    void* GetData(int nIndex);

    virtual HRESULT SetBoneInfo(
        unsigned int nAnmation,
        unsigned int nBone,
        DWORD dwType,
        float fWeight
    );

    virtual HRESULT GetBoneInfo(unsigned int nAnmation, unsigned int nBone, DWORD& dwType, float& fWeight);

    void AddAnimation(KG3DAnimationUpdateExtraInfoCollector *pCollector, DWORD dwIndex);
    void RemoveAnimation(DWORD dwIndex);
    void FrameMove();
    void FrameMove(KG3DAnimationController *pController, int nComposer, int nAnimationIndex, const D3DXMATRIX *pcDownRootMat, BOOL bInTween);
	void Helper_TweenFrameMove(const RTS* pTweenFrame);

	HRESULT SetBoneTransform(int nBoneIndex, const BoneTransformationInputData& Data);
	void SaveTweenTimeInfo(DWORD dwAnimationIndex, KG3DAnimationController *pStart, DWORD dwTweenTime);
	BOOL IsBoneTransformEnable() { return m_bBoneTransformEnable; }
	void EnableBoneTransformEnable(BOOL bEnable) { m_bBoneTransformEnable = bEnable; }
	void UpdateCollectorByTweenInfo(DWORD dwCollectorIndex);
    KG3DAnimationUpdateExtraInfoCollector* GetCollector(unsigned int uIndex);

    const D3DXMATRIX& GetResultWorld(int nIndex);
    D3DXMATRIX* GetResultByIndex(int nIndex);

    KG3DSkeleton* m_pSkeleton;

    HRESULT SaveBlock(SerializeData** ppData, KG3DAnimationUpdateExtraInfoCollector **ppCollectors, DWORD dwNumCollector);

    HRESULT ReadBlock(IFile *pFile, KG3DAnimationUpdateExtraInfoCollector **ppCollectors, DWORD dwNumCollectors);

	HRESULT Clone(KG3DAnimationComposer* pDest);

    KG3DAnimationController* GetAnimationController(unsigned int uIndex) 
    {
        KG3DAnimationController *pReturn = NULL;
        KGLOG_PROCESS_ERROR(uIndex < m_Controller.size());
        pReturn = &m_Controller[uIndex];
Exit0:
        return pReturn;
    };

    size_t GetCollectorIndex(size_t uIndex)
    {
        size_t uReturn = 0xffffffff;
        KG_PROCESS_ERROR(m_SkeletonInfo.size());
        KG_PROCESS_ERROR(uIndex < m_SkeletonInfo[0].Info.size());
        uReturn = m_SkeletonInfo[0].Info[uIndex].dwIndex;
Exit0:
        return uReturn;
    }

    virtual HRESULT GetAnimationCount(DWORD *pdwRetAnimationCount);
    void CloneAnimationController(unsigned int uControllerIndex, const KG3DAnimationController& Src);
	DWORD m_dwPauseTime;
    BOOL m_bInTween;
	std::vector<TweenTimeInfo>	m_TweenTimeInfo;
private:
    void UpdateRelativeBone(int nIndex, const D3DXMATRIX& BaseMatrice);
    std::vector<BoneInfo> m_SkeletonInfo;
    D3DXMATRIX* m_pWorldResult;
    std::vector<KG3DAnimationController> m_Controller;
	BOOL m_bBoneTransformEnable;
	std::map<int, float> m_BoneTransforms;
	
	KG3DAnimationUpdateExtraInfoCollector* m_pCollector;
	KG3DModel *m_pModel;
};

class KG3DAnimationTagContainer;
class KG3DAnimationWarper : public IEKG3DAnimationWarper
{
	friend class KG3DAnimationWarperTable;

    struct SerializeData
    {
        DWORD dwMask;
        DWORD dwVersion;
        const static DWORD cdwVersion = 0x0;
        const static DWORD cdwMask = 0x57415250;

        DWORD dwNumAnimationComposer;
        DWORD dwNumAnimation;
        DWORD dwNumBone;

        SerializeData();
        ~SerializeData();
    };

    struct AnimationInfo
    {
        //IEKG3DAnimation* pAnimation;
		IEKG3DClip* pClip;
        KG3DAnimationTagContainer* pTagContainer;
        KG3DRTSInfoCollector* pCollector;
    };

    struct WaperInfo
    {
        KG3DAnimationComposer* Composer;
        std::vector<BOOL> BoneWeight;
		WaperInfo()
		{
			Composer = NULL;
		}
    };

public:
    KG3DAnimationWarper();
    ~KG3DAnimationWarper();
	void UnInit();

	HRESULT Clone(KG3DAnimationWarper* pDest);

	virtual HRESULT ChangeAnimation(unsigned int nComposer,
		unsigned int nAnimation, 
		//IEKG3DAnimation* pAnimation,
		IEKG3DClip* pClip,
		IKG3DAnimationController* pStart,
        DWORD dwTweenTime);

    virtual HRESULT AddAnimation(IEKG3DClip *pClip, int nComposerIndex);
    virtual HRESULT SetWeight(int nComposer, int nBoneIndex, float fWeight, BOOL bInherit);

    void SetWeightInherit(int nComposer, int nBone, float fWeight);

    virtual HRESULT GetWeight(int nComposer, int nBoneIndex, FLOAT *pfRetWeight);

    void CloneAnimationController(unsigned int uComposerIndex, unsigned int uControllerIndex, const KG3DAnimationController& Src);

    HRESULT FrameMove(D3DXMATRIX *pResult);

    virtual HRESULT AddComposer();
    virtual HRESULT GetComposerCount(DWORD *pnRetComposerCount);

    BOOL GetEnable();

    virtual HRESULT Enable(BOOL bEnable);
    virtual HRESULT GetComposer(int nIndex, IEKG3DAnimationComposer **ppiRetComposer);
    virtual HRESULT Save(const TCHAR *strFileName);
    virtual HRESULT Load(const TCHAR *strFileName);

    void SetModel(KG3DModel *pModel);

	void Clear();
	void Pause(BOOL bPause);
	HRESULT SetBoneTransformation(LPCSTR strBoneName, const BoneTransformationInputData& Data);
	void EnableBoneTransformEnable(BOOL bEnable);
	BOOL IsBoneTransformEnable();

protected:
    HRESULT Helper_SaveBipExtraInfo(FILE *fpFile);
    HRESULT Helper_LoadBipExtraInfo(IFile *pFile);
	const static DWORD          s_dwSecondaryControllerCnt = 2;
public:
    std::vector<KG3DAnimationTagContainer*> m_vecTagContainers;
	KG3DAnimationController                 m_SecondaryControllers[s_dwSecondaryControllerCnt];
	enuAnimationControllerPriority          m_ControllerPriority[s_dwSecondaryControllerCnt];//当前控制器的优先级， 0为上半身， 1为下半身
	KG3DAnimationTagContainer*              m_SecondaryTagContainers[s_dwSecondaryControllerCnt];
	TweenTimeSpan                           m_TweenSpan[s_dwSecondaryControllerCnt];
protected:
    KG3DSkeleton                m_Skeleton;
	std::vector<AnimationInfo>	m_vecAnimations;
    std::vector<WaperInfo>		m_Data;
    std::vector<int>			m_BoneMatchTable;
    KG3DModel*					m_pModel;
    BOOL						m_bEnabled;
	void CheckSychornize(KG3DClip *pNewClip, 
		DWORD& dwTweenTime);
};

class KG3DAnimationWarperTable
{
public:
	map<UINT,KG3DAnimationWarper*>m_mapAnimationWarper;

	HRESULT Init();
	HRESULT UnInit();

	KG3DAnimationWarperTable()
	{
		;
	}
	~KG3DAnimationWarperTable()
	{
		UnInit();
	}

	HRESULT LoadWarper(KG3DAnimationWarper* pDest,LPSTR pFileName);
};
