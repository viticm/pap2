#pragma once

#include "KG3DModel.h"
#include "IEKG3DModelST.h"
#include "KG3DGraphicsTool.h"

class KG3DMeshSkin;

struct MDLFileContent
{
	const static DWORD s_dwMaxNumMesh = 20;
	DWORD dwNumModels;
	std::string strBipFile;
	std::string strMeshFile[s_dwMaxNumMesh];
	std::string strMaterialFile[s_dwMaxNumMesh];
	MDLFileContent()
	{
		dwNumModels = 0;
	}
};

struct KG3DIntersectionFilter;
class KG3DBip;
class KG3DModelST : public KG3DModel, public IEKG3DModelST
{
private:
	HRESULT _LoadFromFile(const char cszFileName[], 
		unsigned uFileNameHash, 
		unsigned uOption);

	int     _AttachModel(KG3DModel *pModelHigh,KG3DModel *pModelMedium,KG3DModel *pModelLow);
	HRESULT _AttachModelProcess(KG3DModel* pModel);
	void _DetachModelProcess(KG3DModel* pModel);
	HRESULT ChangeModelProcess(KG3DModel* pModel);


protected:
	HRESULT CreateSkinMeshUion();

	HRESULT ComputeBBoxLocal();
	virtual BOOL OnProcessCommands(KG3DModelCommandBase* pCmd);
	HRESULT _SetLOD(float fLod);
	HRESULT FrameMoveAnimations();
    HRESULT _SetDetail(int nDefineTable, int nIndex);
	HRESULT LoadAndBindModel(LPCSTR pszFileName, LPCSTR pszSocketName);
	HRESULT SaveToMDLFile(const char cszFileName[], unsigned uOption);
	HRESULT SaveCompactFromFileIni(IIniFile* pConfigFile, LPSTR pSectionName,unsigned uOption);
	HRESULT LoadCompactFromFileIni(IIniFile* pConfigFile, LPSTR pSectionName,unsigned uOption);

public:
	HRESULT LoadCompactFromModelSTFile(const char cszFileName[], LPSTR pSectionName,unsigned uOption);
	HRESULT SaveCompactToModelSTFile(const char cszFileName[], LPSTR pSectionName,unsigned uOption);

	KG3DMeshSkin* m_lpSkinMeshUion;//骨骼动画模型的合并绘制

	static HRESULT LoadMDLContent(LPCSTR strFileName, MDLFileContent& Content);
	virtual BOOL IsLoaded();
	//virtual HRESULT GetBoneScale(DWORD dwIndex, D3DXVECTOR3 *pScale);
	//virtual HRESULT SetBoneScale(DWORD dwIndex, D3DXVECTOR3 *pScale);
    virtual HRESULT IEChangeModel(DWORD dwIndex, IKG3DModel* pModel);
    virtual HRESULT GetIKG3DModel(IKG3DModel **ppiModel);
	virtual void _SeekSplitAnimation(DWORD dwSplitType, 
		enuAnimationControllerPriority nPrority,
		DWORD dwSeekType, 
		float fOffset);

    virtual HRESULT SetPointLightInfo(KG3D_PointLight_Info& vInfo);

	virtual int GetBindToSocketModel(const char cszSocketName[],vector<IEKG3DModel*> &vecpModel);
	virtual int GetBindToSocketModel(const char cszSocketName[],vector<IKG3DModel*> &vecpModel);

	virtual void SetbDrawBigBox(BOOL val);
	virtual BOOL GetbDrawBigBox();
	virtual HRESULT ComputeMeshBigBox();
	virtual HRESULT ComputeDynamicBBox();
	virtual BOOL IsResourceReady(BOOL bTestText);
	virtual void PauseAnimation(BOOL bPause);
	D3DXMATRIX* GetRenderMatrix();
    void _SetPhysicsScene(KG3DPhysiscScene *pScene);
    virtual void SetPhysicsScene(KG3DPhysiscScene *pScene);

	virtual HRESULT CreateProgressMesh();
	virtual HRESULT SetLOD(float fLod);
	KG3DModelST(void);
	~KG3DModelST(void);
	virtual HRESULT UnInit();

	void Detach();
	ULONG STDMETHODCALLTYPE AddRef();
	
    ULONG STDMETHODCALLTYPE Release()
    {
        ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
        if (uNewRefCount == 0)
            OnReferenceZero(); 
        return uNewRefCount;
    }

	//Interfaces from KG3DModel
	virtual HRESULT Render(unsigned int uOption, void* pExtInfo);
	virtual HRESULT LoadFromFile(const char cszFileName[], 
		unsigned uFileNameHash, 
		unsigned uOption);
	virtual HRESULT SaveToFile(const char cszFileName[], 
		unsigned uOption);
	virtual HRESULT FrameMove();
	void RelaseBip();
	virtual HRESULT ShowBones(BOOL bShow);

    virtual HRESULT Helper_PlayAnimationNormal(
		DWORD dwPlayType,
        LPCSTR strAnimationName, 
        FLOAT fSpeed, 
        int   nOffsetTime, 
        PVOID pReserved,
		enuAnimationControllerPriority Priority,
		enuModelPlayAnimationType AnimationType,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);
	HRESULT Helper_PlayAnimationTagedAnimation(DWORD dwPlayType,
		LPCSTR strFileName,
		FLOAT fSpeed,
        int   nOffsetTime,
		PVOID pReserved,
		enuAnimationControllerPriority Priority,
		enuModelPlayAnimationType AnimationType,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);
	virtual IKG3DAnimationController* GetAnimationController(enuAnimationControllerPriority nPriority);
	virtual void OnBind(IKG3DModel* pModel,KG3DModel* pModelMedium,KG3DModel* pModelLow);
	virtual HRESULT UnbindChild(const TCHAR* strBindObjName);
	virtual int FindSocket(const TCHAR* strSocketName, BindExtraInfo* pExtInfo);
	virtual int FindBone(const TCHAR* strBoneName, BindExtraInfo* pExtInfo);
	virtual HRESULT LoadAnimationFromFile(LPCSTR strFileName, DWORD dwOption);
	virtual HRESULT ComputeBBox();

	virtual BOOL IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);
	virtual BOOL RayIntersection(const D3DXVECTOR3& vSrc , const D3DXVECTOR3& vNormalizedDir, FLOAT* pfRet
		,D3DXVECTOR3* pFaceNormal,BOOL bIntersectBoxOnly,  KG3DIntersectionFilter* pFilter = NULL);	//ModelST只会进行对Box的比较
	//virtual HRESULT Render(unsigned int uOption, void* pExtInfo);

	virtual HRESULT GetBoneMatrix(const BindExtraInfo* pInfo, 
		D3DXMATRIX* pmatResult, 
		D3DXVECTOR3* pvec3Offset);
	
	virtual HRESULT GetSocketMatrix(const BindExtraInfo* pInfo, 
		D3DXMATRIX* pmatResult,
		D3DXVECTOR3* pvec3Offset);
	
	virtual void SetHighLightLevel(float fIllumination, BOOL bInherit);
	virtual HRESULT SetAlpha(float fAlpha, BOOL bUseSpecial);

	virtual HRESULT EnableWarper(BOOL bEnable);

	KG3DBip* GetBip();

	//void PostWarperFrameMove(const D3DXMATRIX& matBase, 
	//	const D3DXMATRIX* pMatSrc = NULL);
	
	virtual HRESULT GetModel(unsigned int uIndex, IKG3DModel** ppModel);
	HRESULT GetModelEx(unsigned int uIndex, KG3DModel** ppModelCurrent,KG3DModel** ppModelHigh);

	virtual HRESULT ChangeModel(unsigned int uIndex, IKG3DModel* pModel);
	void    SetBoneMeshAttachScene(KG3DScene* pScene);

	virtual int AttachModel(IKG3DModel *pModel);
	virtual void DetachModel(IKG3DModel* pModel);
	void _DetachModel(IKG3DModel* pModel);
	
	HRESULT UpdateBoneMesh(KG3DScene* pScene);
	HRESULT UpdateBoneBBox(KG3DScene* pScene);

    virtual vector<KG3DModel*> GetAllChilds();
	virtual HRESULT GetAllChilds(vector<IEKG3DModel*> &vecpChildsModel);
	
	BOOL m_bDrawBigBox;
    // ------------- for ransack childs -----------------

    vector<KG3DModelLod> m_vecChilds;
    int                m_nCurrChild;
    HRESULT GetFirstChildModel(IEKG3DModel **ppModel);
    HRESULT GetNextChildModel(IEKG3DModel **ppModel);
    //
	bool ComparePath(const char cszFileName1[],const char cszFileName2[],const char sign);
	D3DXMATRIX* GetBoneAnimationResultPool();

	virtual HRESULT GetNumBones(INT* pNumBone);
	virtual HRESULT GetNumSocket(INT* pNumSocket);
	virtual LPCSTR GetBoneName(int nIndex);
	virtual HRESULT GetSocketName(int nIndex, LPCTSTR *ppRetName);

	HRESULT _SetBoneTransform(LPCSTR strBoneName,
		const BoneTransformationInputData& Data);
	HRESULT SetBoneTransform(LPCSTR strBoneName,
		const BoneTransformationInputData& Data);

	virtual void EnableBoneTransform(BOOL bEnable);
	virtual BOOL IsBoneTransformEnable();
	HRESULT RenderSpecialAlpha(unsigned int uOption, void* pExtInfo);
	HRESULT RenderCommon(unsigned int uOption, void* pExtInfo);

	virtual void RenderForSelector(DWORD dwOption, void *pExtInfo);
	virtual void SetSelectIndex(DWORD dwIndex);

	HRESULT RenderToScreen(const D3DXVECTOR2* pA, 
		const D3DXVECTOR2* pC, 
		const D3DXVECTOR2* pUVA, 
		const D3DXVECTOR2* pUVC, 
		float fZ, 
		DWORD dwFlags,
		DWORD dwBackColor,
		void* pExtInfo);

	virtual void GetRenderEnviromentParamenter(IKG3DScene* pScene);
	virtual int GetNumModel();
	HRESULT GetIEBip(IEKG3DBip **ppiBip);

	virtual HRESULT OnReferenceZero();
	virtual HRESULT RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);

	//柔体相关
	HRESULT ReflushPhysicsSys();

    virtual BOOL IsModelST() { return TRUE; }

	virtual HRESULT GetMeshFileName(LPTSTR strFileName);
	virtual void SetVisible();
	virtual void SetClientAdd(BOOL bClient);//是否是客户端加入的模型
	virtual void FrameMoveOnce();
    virtual HRESULT LoadPostProcess();
    virtual HRESULT SetDetail(int nDefineTable, int nIndex);
protected:
	virtual HRESULT LoadFromFileMultiThread();
	virtual HRESULT EnableHideFromRenderTools(BOOL bEnable);

friend class KG3DBip;
	KG3DAnimationController m_Controllers[ANICTL_COUNT];

	//优化渲染结束
private:
	std::vector<KG3DModelLod> m_Models;
	int m_nNumModelNeedToAttach;
	BOOL m_bFrameMoved;
};
