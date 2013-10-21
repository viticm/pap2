//////////////////////////////////////////////////////////////////////////////////
////
////  FileName    : KG3DModelSet.h
////  Version     : 1.0
////  Creator     : huangshan
////  Create Date : 2006-11-6 16:11:06
////  Comment     : 
////
//////////////////////////////////////////////////////////////////////////////////
//
//#ifndef _INCLUDE_KG3DMODELSET_H_
//#define _INCLUDE_KG3DMODELSET_H_
//
//////////////////////////////////////////////////////////////////////////////////
//
//#include "KG3DInterface.h"
//#include "kg3dtypes.h"
//#include "IEKG3DModelSet.h"
//
//class KG3DBip;
//class KG3DModel;
//class KG3DAnimationWarper;
//
//
//
//class KG3DModelSet : public IKG3DModelSet, public IEKG3DModelSet 
//{
//	//For animation warper
//	friend class KG3DModelST;
//	friend class KG3DAnimationWarper;
//
//public://for IE interface
//    virtual HRESULT GetModelsCount(DWORD *pdwCount);
//    virtual HRESULT GetModel(
//        DWORD dwPosition, 
//        IEKG3DModel **ppModel
//    );
//	int GetNumModel();
//    virtual HRESULT GetIEBip(IEKG3DBip **ppiBip);
//
//public:
//	KG3DModelSet(void);
//	~KG3DModelSet(void);
//	
//
//    virtual HRESULT ReflushPhysicsSys();
//	//Interfaces from IKG3DModelSet
//	virtual int Attach(IKG3DModel* pModel);
//	virtual void DetachModel(IKG3DModel* pModel);
//	virtual void ChangeModel(IKG3DModel* pModelNew, IKG3DModel* pModelOld);
//	virtual HRESULT ChangeModel(unsigned int uIndex, IKG3DModel* pModel);
//	
//	void Attach(KG3DBip* pBip);
//	void Detach();
//
//
//    void UpdateFlexBoneMatrices();
//
//	virtual HRESULT Render(unsigned int uOption, void* pExtInfo);
//	void FrameMove(const D3DXMATRIX& matBase);
//	void CleanUp();
//
//	HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
//	HRESULT BindToSocket(IKG3DModel* pModel, const TCHAR* strSocketName);
//	HRESULT BindToBone(IKG3DModel* pModel, const char* strBoneName);
//	void UnbindChild(const TCHAR* strBindObjName);
//	HRESULT PlayAnimation(const TCHAR* strFileName, DWORD PlayType, float fOffset, float fSpeed, void* pExtInfo);
//
//	HRESULT FindSocket(const char* strName, BindExtraInfo& Info);
//	HRESULT FindBone(const char* strName, BindExtraInfo& Info);
//	
//	void ComputerAABBBox(AABBOX& Box, D3DXMATRIX& mat);
//
//	BOOL IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3& Origin,D3DXVECTOR3& Direction);
//
//	HRESULT RenderToScreen(const D3DXVECTOR2* pA, 
//		const D3DXVECTOR2* pC, 
//		const D3DXVECTOR2* pUVA, 
//		const D3DXVECTOR2* pUVC, 
//		float fZ, 
//		DWORD dwFlags,
//		void* pExtInfo);
//
//	void SetContainer(KG3DModelST* pModel);
//
//	IKG3DAnimationController* GetAnimationController();
//
//	D3DXMATRIX* GetWorldMatrices();
//
//	void SetHighLightLevel(float fIllumination, BOOL bInherit);
//	HRESULT SetAlpha(float fAlpha, BOOL bUseSpecial);
//
//	const KG3DBip* GetBip();
//
//	HRESULT GetModel(unsigned int uIndex, IKG3DModel** ppModel);
//
//	void PostWarperFrameMove(const D3DXMATRIX& matBase,
//		const D3DXMATRIX *pmatSrc);
//
//    //
//    void UpdateBindBoneMatrices();
//
//	int GetNumBones();
//	int GetNumSocket();
//	LPCSTR GetBoneName(int nIndex);
//	LPCSTR GetSocketName(int nIndex);
//
//	//by huangjinshou 2007 8.8
//	int  GetBindToSocketModel(const char cszSocketName[],vector<IEKG3DModel*> &vecpModel);
//	int  GetBindToSocketModel(const char cszSocketName[],vector<IKG3DModel*> &vecpModel);
//
//	HRESULT RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);
//
//private:
//	
//	KG3DModelST* m_pContainer;
//    struct BindBoneMatrix
//    {
//        KG3DModel* pModel;
//        int nIndex;
//    };
//    std::list<BindBoneMatrix> m_lstBindBone;
//	
//public:
//
//};
//
//
//
//#endif //_INCLUDE_KG3DMODELSET_H_
