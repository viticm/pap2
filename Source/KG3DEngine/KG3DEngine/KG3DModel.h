#pragma once
#include "kg3dinterface.h"
#include "kg3dmesh.h"
#include "Scene/KG3DTransformation.h"
#include "kg3dstructs.h"
#include "kg3danimationwarper.h"
#include "KG3DMeshBone.h"
#include "IEKG3DAnimationController.h"
#include "IEEditor.h"//by dengzhihui Refactor 2006年11月13日
#include "IEKG3DModelST.h"
#include "KG3DAnimationSplitter.h"
#include "KG3DRuntimeMaterial.h"
#include "KG3DToolsImpl.h"
#include "kg3dcaption.h"
#include "KG3DFuctionCommand.h"

#define MODEL_ILLU_HIGHLIGHT 1

#define LOAD_ANI_OPT_REPLACE 1

#define KG3DEVENT_MESH_LOADED 1



class KG3DMeshBase;
class KG3DMaterial;
class KG3DClip;
class KG3DAni;
class KG3DBip;
class KG3DAnimationController;
class KG3DSFXSuede;
class KG3DTexture;
class KG3DScene;
class KG3DSceneEditorBase;
class IEKG3DSFXSuede;
class KG3DAnimationTagContainer;
class IEKG3DAnimationTagContainer;
class KG3DBillboardCloud;
class KG3DRepresentObjectPVS;
class KG3DModelST;
class KG3DSceneRegion;
class IKG3DFlexibleBody;
class KG3DBsp;

struct KG3D_PointLight_Info;
class KG3DModelLightMap;

struct KG3DModelLod
{
	KG3DModel* pModelHigh;
	KG3DModel* pModelMedium;
	KG3DModel* pModelLow;
	KG3DModel* pModelCurrent;

	KG3DModelLod()
	{
		pModelHigh = NULL;
		pModelMedium = NULL;
		pModelLow = NULL;
		pModelCurrent = NULL;
	}
	friend bool operator<(const KG3DModelLod& A, const KG3DModelLod& B)
	{
		return A.pModelHigh < B.pModelHigh;
	}

	friend bool operator == (const KG3DModelLod& A, const KG3DModelLod& B)
	{
		return A.pModelHigh == B.pModelHigh;
	}
};

class KG3DModel : public CKG3DConnectionPoint<IEKG3DModel> , public KG3DTransformation , public IEKG3DSimpleSink
{
    friend class KG3DSceneEntity;
	friend class KG3DClip;
	friend class KG3DRepresentObject;
	friend class KG3DSceneSceneEditor;
	friend class KG3DPhysiscScene;
	friend class KG3DModelST;
	friend class KG3DFuctionCommand;
	friend class KG3DMeshSkin;
	friend class KG3DSkinShaderRender;
public:
	KG3DModel();
	virtual ~KG3DModel();
public:
	BOOL    IsReadyToAttchModelST();
	HRESULT CheckModelLodFiles(KG3DModel* pModelDefault,UINT uOption);
	HRESULT CheckMeshLodFiles(KG3DMesh* pDefaultMesh,UINT uOption);

	virtual BOOL IsNeedFrameMove();

	HRESULT AjustWorldDirection(const D3DXVECTOR3* pVecDir, const D3DXVECTOR3* pSelfDir);
	void* m_lpSceneRegionOn;
	HRESULT OnEnterSceneRegion(KG3DSceneRegion& Region);//只能由Region调用
	HRESULT LeaveSceneRegion(BOOL bErase);//可以主动调用

    HRESULT OnEnterPvs(KG3DRepresentObjectPVS* pvs);
    HRESULT LeavePvs(BOOL bRemoveFromPVS);

    virtual KG3DBsp* GetBsp();

    virtual void ReSet() {};

	virtual HRESULT OnLostDevice();
	virtual HRESULT OnResetDevice();

    virtual HRESULT RefurbishTexts();
    virtual HRESULT GetIEKG3DAnimation(IEKG3DClip **ppiClip);
    virtual HRESULT GetIEKG3DAnimationController(
        IEKG3DAnimationController **ppiAniController
    );
    virtual HRESULT GetMeshIKG3DResourceBase(IKG3DResourceBase **ppiMesh);
    virtual HRESULT GetIEKG3DMesh(IEKG3DMesh **ppiMesh);
    virtual HRESULT GetIEKG3DMaterial(IEKG3DMaterial **ppiMat);
    virtual HRESULT GetIEKG3DModelST(IEKG3DModelST **ppiModelST);
    virtual HRESULT GetIEKG3DModelSpeedTree(IEKG3DModelSpeedTree **ppiSpeedTree);
    virtual HRESULT GetIEKG3DAnimationWarper(IEKG3DAnimationWarper **ppAniWarper);
    virtual HRESULT GetAnimationTagContainer(IEKG3DAnimationTagContainer** pContainer);

    virtual HRESULT GetMatWorld(D3DXMATRIX **ppMatWorld);
    virtual HRESULT GetName(LPCTSTR *ppszName);
   // virtual HRESULT GetFullName(LPCTSTR *ppszFullName);
    virtual HRESULT GetType(DWORD *pdwType);

    virtual HRESULT GetIEKG3DMeshBone(IEKG3DMeshBone **ppMeshBone);
       
    virtual HRESULT GetFirstChildModel(IEKG3DModel **ppModel);
    virtual HRESULT GetNextChildModel(IEKG3DModel **ppModel);
    
    virtual HRESULT GetTransformationMatrix(D3DXMATRIX *pMatrix);
    virtual HRESULT SetTransformationMatrix(D3DXMATRIX *pMatrix);

	virtual void ResetTransform();

    virtual HRESULT IEReSet();

	virtual BOOL    IsResourceReady(BOOL bTestTexture);
	BOOL IsNeedCheckResourceReady();

	virtual void SinkFunc(INT nSinkID);
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);

	virtual BOOL IsLoaded();

    virtual BOOL IsVisible() const;

    virtual void SetVisible();

	DWORD   m_dwRecordID;

	virtual BOOL   IsSortAsSFX();
	virtual void SeekSplitAnimation(DWORD dwSplitType, 
		enuAnimationControllerPriority nPrority,
		DWORD dwSeekType, 
		float fOffset);

	BOOL   m_bHideFromRenderTools;//从渲染器中隐藏，当客户端删除某个物件时
	BOOL   m_bNeedCheckResourceReady;

	KG3DDefaultShaderType m_eRuntimeShaderType;//用作渲染器排序的SHADER编号

	HRESULT CheckRuntimeShaderType();
protected:
	DWORD   m_dwType;//类型
	DWORD   m_dwID;



	DWORD   m_dwDefaultColorCastIndex;

	DWORD m_dwFrameCount;//framemove计数器

	int m_nDetailTableIndex;
	int m_nDetailDefineIndex;
	KG3DMaterialDetail* m_lpDetail;
	HRESULT EnableDetail(BOOL bEnable);
	HRESULT _SetDetail(int nDefineTable,int nIndex);

	KG3D_PointLight_Info* m_lpPointLightInfo;
public:
	HRESULT EnableUseLightmap2(BOOL bEnable,float ratioA,float ratioB);
	HRESULT EnableLightmap(BOOL bEnable,int nWidth,BOOL bComputing,LPSTR pFileName);
	virtual HRESULT MultiThreadProcess();
	virtual HRESULT IESetLod(float fLod);

	KG3D_PointLight_Info* GetPointLightInfo()
	{
		return m_lpPointLightInfo;
	}
	HRESULT GetMaterilDetail(int nDetailType, int nSubSetID, LPSTR pszName, LPSTR pszTextureFileName, float* pfEnv,float* pfSpecular,
		float* pBodyMax,float* pBodyMin,float* pBodyScale,D3DCOLORVALUE* pBodyColor,D3DCOLORVALUE* pBodyColor1,LPSTR pChannelFile,
		float* pDetail0_ColorScale,float* pDetail0_UVScale,float* pfDetail0_Env,float* pfDetail0_Specular,D3DCOLORVALUE* pDetail0_Color,LPSTR pDetail0_File,
		float* pDetail1_ColorScale,float* pDetail1_UVScale,float* pfDetail1_Env,float* pfDetail1_Specular,D3DCOLORVALUE* pDetail1_Color,LPSTR pDetail1_File,
		D3DCOLORVALUE* pFlow_Color,float* pfFlow_ColorScale,float* pfFlow_UVScale,D3DXVECTOR2* pvSpeed,LPSTR pFlowTexture);

	HRESULT SetMaterilDetail(int nDetailType, int nSubSetID, LPSTR pszName, LPSTR pszTextureFileName, float fEnv,float fSpecular,
		float fBodyMax,float fBodyMin,float fBodyScale,D3DCOLORVALUE vBodyColor,D3DCOLORVALUE vBodyColor1,LPSTR pChannelFile,
		float fDetail0_ColorScale,float fDetail0_UVScale,float fDetail0_Env,float fDetail0_Specular,D3DCOLORVALUE vDetail0_Color,LPSTR pDetail0_File,
		float fDetail1_ColorScale,float fDetail1_UVScale,float fDetail1_Env,float fDetail1_Specular,D3DCOLORVALUE vDetail1_Color,LPSTR pDetail1_File,
		D3DCOLORVALUE Flow_Color,float fFlow_ColorScale,float fFlow_UVScale,D3DXVECTOR2 vSpeed,LPSTR pFlowTexture);

	KG3DMaterialDetail* GetDetail(int nSubSetID);

	virtual HRESULT SetDetail(int nDefineTable,int nIndex);
	virtual HRESULT GetDetail(int* pnDefineTable,int* pnIndex);
	BOOL HasDetail();

	virtual HRESULT EnableHideFromRenderTools(BOOL bEnable);

	DWORD m_dwVisibleCount;
	DWORD GetVisibleCount(){return m_dwVisibleCount;};
	void  SetVisibleCount(DWORD dwCount){m_dwVisibleCount = dwCount;};

	virtual HRESULT CreateProgressMesh();

	HRESULT ModifySubsetID(DWORD* pSubsetID){return E_FAIL;}//by dengzhihui Refactor 2006年11月7日

protected:
	KG3DMesh*		m_lpMesh;//用GetMesh得到
    BOOL m_bBBoxChanged;

public:
	KG3DModelLightMap*    m_lpLightMap;

    BOOL m_bMeshPostLoadProcessed;//模型加载后处理工作是否完成
	BOOL m_bInMultiThreadLoadList; 
	UINT m_uOption;
	
	KG3DRepresentObjectPVS* m_pPvsOn;
	D3DXMATRIX          m_matWorld;
    D3DXMATRIX m_matWorldSave;
	ULONG               m_ulRefCount;//物品使用计数
	std::string         m_scName;
	//std::string         m_scFullName;//不需要记录这个,可以省点内存，用m_scName就足够了
	AABBOX              m_BBox;			//动态变动大小的BBOX，目的主要是能够在运动的时候能够精确点选 
	AABBOX              m_BBoxLocal;	//LoadFromFile之后，记录的m_BBox，如果是ModelST的话，是整体的大小
	FLOAT				m_fShadowScaleFactor;	//静态计算的影子大小，根据m_BBoxLocal载入时候的状态
	//KG3DMesh*       m_lpMesh;//改成protected的，用GetMesh得到吧
	KG3DMaterial*       m_lpMaterial;
	KG3DClip*			m_lpClip;
	
	BOOL                m_bCreateDynamicLight;//是否绑定一个光源并产生光照效果
	BOOL                m_bSelectable;//该模型是否能被选择，客户端使用此变量//去掉，没有检索到有效应用，而且由于Select机制的原因，很容易出错 Last modify by Chen TianHong:2008-4-16 16:06:51 
	BOOL                m_bVisible;//是否被看见，客户端使用此变量

	DWORD               m_dwPicthOn;
	BOOL                m_bDrawBigBox;
	vector<BBox>        m_BoneBoxInfo;
	static BOOL			m_Static_bUseBoneBox;

	BOOL m_bHasAnimation;

	vector<KG3DClip*>   m_vecAnimationLoaded;
	

	//////////////////////////////////////////////////////////////////////////
	KG3DModelST* m_lpParentModelST;//modelST容器的指针，如果是孤立的model,这里为空

	//////////////////////////////////////////////////////////////////////////
	//引用计数
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

	//////////////////////////////////////////////////////////////////////////
	//初始化,清除数据
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	//////////////////////////////////////////////////////////////////////////
	//加载,保存
	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
	virtual HRESULT RefreshOption(unsigned uOption);
	virtual HRESULT SaveToFile(const char cszFileName[],   unsigned uOption);
	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam)
	{
		return LoadFromFile(cszFileName, uFileNameHash, uOption);
	}
	//////////////////////////////////////////////////////////////////////////
	//ID,type
	virtual DWORD GetID();
	virtual unsigned GetType();
	void SetID(DWORD dwID) { m_dwID = dwID;}
	void SetType(DWORD dwType){m_dwType = dwType;}
	//////////////////////////////////////////////////////////////////////////
	//内存管理
    
    //HRESULT ApplayLighting();
    //HRESULT RestoreLighting();

	virtual HRESULT PrepareRender(unsigned uOption);//压缩数据
	virtual HRESULT PostRender();//恢复数据，对象可以立即被使用
	virtual HRESULT GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem);//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
	virtual unsigned   GetLastUseTime();//查询上次使用的时间
	virtual unsigned   GetUseFrequency();//查询使用频率，一段时间里的使用次数

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//网格
	virtual HRESULT LoadMeshFromFile(LPCSTR strFileName, DWORD Option);	//从文件中载入网格，如果对象中已经有网格数据，则数据将被覆盖
	virtual KG3DMesh* GetMesh();	//得到最高精度的那个Mesh不带加引用计数，如果要保存指针的话，要加引用计数//原来是带引用计数的，只检查到内部暂存式的使用，所以不需要引用计数了 Last modify by Chen TianHong:2008-6-18 9:50:29
	virtual HRESULT SetMesh(KG3DMesh* pMesh);
	virtual HRESULT MeshPostLoadProcess();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//材质
	virtual HRESULT _LoadMaterialFromFile(LPCSTR strFileName,DWORD dwOption);//从文件中载入材质，根据文件中材质的数目和模型材质的数目进行自动匹配
	virtual HRESULT LoadMaterialFromFile(LPCSTR strFileName,DWORD dwOption);//从文件中载入材质，根据文件中材质的数目和模型材质的数目进行自动匹配
	virtual HRESULT LoadMaterialFromFileEx(LPSTR strFileName,DWORD dwOption, DWORD dwSrcID, DWORD dwDestID);//手动配置材质，将模型中第DestID的材质替换为文件中第SrcID号材质，材质记数从0开始
	virtual KG3DMaterial* GetMaterial();
	HRESULT _SetMaterial(IEKG3DMaterial* pMtl);
	virtual HRESULT SetMaterial(IEKG3DMaterial* pMtl);
	virtual void SetHighLightLevel(float fIllumination, BOOL bInherit);
	virtual float GetHightLightLevel();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	virtual HRESULT SetAnimation(KG3DClip* pClip);
	//virtual void PauseAnimation(BOOL bPause); 
	// 柔体
    KG3DPhysiscScene* m_lpPhysicsScene;
	virtual void _SetPhysicsScene(KG3DPhysiscScene *pScene);
    virtual void SetPhysicsScene(KG3DPhysiscScene *pScene);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual HRESULT ChainAdd( IKG3DModel *pOtherModel );
	virtual HRESULT ChainRemove( IKG3DModel *pOtherModel );

	virtual HRESULT ChainEnable( ) ;
	virtual HRESULT ChainDisable( ) ;

    ///////////////////////////////////////////////////////////////////
    // 特效内使用的模型动画接口
    virtual HRESULT SetModelColor(DWORD dwSubset, D3DCOLORVALUE color);
    virtual HRESULT GetModelColor(DWORD dwSubset, D3DCOLORVALUE* color);
	//在动画标签上用于取得绑定在插槽上的模型 by huangjinshou
	virtual int GetBindToSocketModel(const char cszSocketName[],vector<IEKG3DModel*> &vecpModel);
	virtual int GetBindToSocketModel(const char cszSocketName[],vector<IKG3DModel*> &vecpModel);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual HRESULT RenderSubset(DWORD SubsetID,DWORD Option);	//渲染其中一部分
	virtual HRESULT Render(unsigned uOption, void* pExtInfo);//渲染
    virtual HRESULT RenderShadowMap(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP);
	virtual HRESULT RenderSelfShadow(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,
		                          const D3DXMATRIX &matWVP,LPDIRECT3DTEXTURE9 lpDepthMap);

	virtual HRESULT RenderToScreen(D3DXVECTOR2 const* pA, 
		D3DXVECTOR2 const* pC, 
		D3DXVECTOR2 const* pUVA, 
		D3DXVECTOR2 const* pUVC, 
		float fZ, 
		DWORD dwFlags,
		DWORD dwBackColor,
		void* pExtInfo);
	
	virtual HRESULT RenderToTexture(DWORD dwTextureID, 
		D3DXVECTOR2 const* pA, 
		D3DXVECTOR2 const* pC, 
		D3DXVECTOR2 const* pUVA, 
		D3DXVECTOR2 const* pUVC,
		float fZ, 
		DWORD dwFlags,
		DWORD dwBackColor,
		DWORD dwRenderOption,
		void* pExtInfo);

	virtual HRESULT RenderToPic(LPCSTR strOutputFile,
		DWORD dwWidth,
		DWORD dwHeight,
		DWORD dwBackColor,
		BOOL bUsedForMeshMdlThumbnail = FALSE );
	virtual HRESULT RenderAnimationToPicSerial(LPCSTR strAnimationName, 
		LPCSTR strSerialName,
		DWORD dwWidth,
		DWORD dwHeight,
		DWORD dwBackColor);

	HRESULT RenderChain(unsigned int uOption, void* pExtInfo);
	HRESULT RenderChildModels(unsigned int uOption, void* pExtInfo);

	virtual HRESULT RenderPlaneShadow(D3DXPLANE& plane,const D3DXVECTOR4& LightDir);
	virtual HRESULT RenderWithoutStateChange();

	HRESULT RenderReflect(unsigned uOption, void* pExtInfo,float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);

    //<碰撞检测>
	BOOL GetCameraNearRayIntersect(
		IN CONST D3DXVECTOR3 vSrc[],
		IN CONST D3DXVECTOR3 vDst[],
		IN CONST INT         nNum, 
		OUT      FLOAT*      pDis 
		);
	virtual BOOL RayIntersection(const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vNormalizedDir, FLOAT* pfRet
		,D3DXVECTOR3* pFaceNormal
		, BOOL bIntersectBoxOnly
		, KG3DIntersectionFilter* pFilter = NULL);//只需要和线碰撞的最近点，其它忽略，由vSrc + vNormalizedDir * fRet就可以得到交点坐标
	virtual BOOL IsVertexInside(D3DXVECTOR3& vVertex, D3DXVECTOR3& vDir);
	virtual BOOL IsRayIntersectBoneBox(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,D3DXMATRIX* matWorld);
	virtual BOOL IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);
	virtual BOOL GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);
	virtual BOOL GetRayIntersectPointAndDir(DWORD& dwCount,
		D3DXVECTOR3* pInterPoint,
		float* pAngle,
		const D3DXVECTOR3& vec3Org,
		const D3DXVECTOR3& vec3Dir);
	//GetNearRayIntersect的功能由RayIntersection负责
	/*virtual BOOL GetNearRayIntersect(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir
		, FLOAT* pfRet);*///只需要和线碰撞的最近点，其它忽略，由vSrc + vNormalizedDir * fRet就可以得到交点坐标;
	//</碰撞检测>
	virtual HRESULT FrameMove();
	virtual void UpdateChildModel();

	virtual HRESULT SetAlpha(float fAlpha, BOOL bUseSpecial);
	virtual float   GetAlpha();
	//virtual HRESULT	SetAnimationSpeed(float fSpeed);

	virtual HRESULT	RenderSelectMark(DWORD dwColor = 0xFFFFFFFF);
    virtual BOOL IsBBoxChanged();
    virtual void SetBBoxChanged(BOOL bBBoxChanged);
	virtual HRESULT ComputeBBox();
	virtual HRESULT ComputeDynamicBBox();
	virtual VOID SetTranslation(D3DXVECTOR3 *pValue);
	virtual VOID SetRotation(D3DXQUATERNION *pValue);
	virtual void SetRotationCenter(D3DXVECTOR3 *pValue);
	virtual VOID SetScaling(D3DXVECTOR3 *pValue);
	virtual void SetScalingRotation(D3DXQUATERNION* pValue);
	virtual void SetScalingCenter(D3DXVECTOR3* pValue);

	virtual VOID GetTranslation(D3DXVECTOR3 *pValue);
	virtual void GetRotation(D3DXQUATERNION* pValue);
	virtual void GetRotationCenter(D3DXVECTOR3* pValue);
	virtual void GetScaling(D3DXVECTOR3* pValue);
	virtual void GetScalingRotation(D3DXQUATERNION* pValue);
	virtual void GetScalingCenter(D3DXVECTOR3* pValue);

	virtual VOID SetAttachType(MODELCLOSEGROUND_TYPE nType,IKG3DScene* pScene = NULL);

	virtual HRESULT GetMeshFileName(LPTSTR strFileName);
	virtual HRESULT GetMaterialFileName(LPTSTR strFileName);
	virtual HRESULT GetAnimationFileName(LPTSTR strFileName, size_t uSize);

    virtual HRESULT Reload();

   
    virtual IKG3DSFXSuede* GetSFXSuede(){return NULL;};


	virtual HRESULT LoadFromFileMultiThread();
	//<BillboardCloudV1>
public :
	virtual HRESULT SetPointLightInfo(KG3D_PointLight_Info& vInfo);

    virtual DWORD GetBillBoardCloudPlaneNum();
    virtual BOOL HasBillBoardCloud()                    { return NULL != m_pBillboardCloud; }
    virtual void IEReleaseBillboardCloud();	//只有Editor会用

    HRESULT RenderBillboardCloud_Begin();
    HRESULT RenderBillboardCloud(const D3DXMATRIX* matOffset = NULL);
    HRESULT RenderBillboardCloud_End();
	KG3DBillboardCloud* GetBillboardCloud(){return m_pBillboardCloud;}
	VOID	SetBillboardCloud(KG3DBillboardCloud* pCloud);
protected:
	KG3DBillboardCloud* m_pBillboardCloud;
	BOOL				m_bBillboardCloudCanBeLoaded;
	//<BillboardCloudV1>
public:
	DWORD GetDefaultColorCastIndex();
	void SetDefaultColorCastIndex(DWORD dwIndex);
	virtual void GetProfile(DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID);

	HRESULT RegistereEventHandler(IKG3DAnimationEventHandler* pHandler);
	HRESULT UnregisterEventHandler(IKG3DAnimationEventHandler* pHandler);

	virtual HRESULT SetLOD(float fLod);
	virtual HRESULT GetLOD(float& fLod);

	virtual HRESULT GetModel(unsigned int uIndex, IKG3DModel** ppModel);
	virtual HRESULT ChangeModel(unsigned int uIndex, IKG3DModel* pModel);
	virtual int GetNumModel();
protected:
	HRESULT _BindToSocketProcess(KG3DModel* pParent,int nSocket,KG3DModel *pTemp, const char cszSocketName[]);

	HRESULT SetMatrix();
	HRESULT RestoreMatrix();

	//IKG3DAnimationEventHandler* m_pEventHandler;
	float  m_fLOD;//lod系数,1,正常,0不显示.
private:
    IKG3DFlexibleBody *m_piFlexibleBody;
protected:

    virtual HRESULT OnReferenceZero();
	
	///////////////////////////////////////////////////////////
	//Bind data
	KG3DModel*        m_pParentModel;
    std::string       m_scBindToObjName;
	enuModelBindType  m_BindType;
	int               m_nBindIndex;
	//Bind data end
	///////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////
	// chain member
	/////////////////////////////////////////////////////////
	typedef std::vector< IKG3DModel* >  ChainVec_t;
	typedef ChainVec_t::iterator ChainIter_t;
	
	std::set<KG3DModel*> m_BindedOrgModel;

	//ChainVec_t		m_vecChainModel;
	//BOOL			m_bChainEnable;
	float           m_fAlpha;
	float           m_fIlluminationLevel;
	BOOL            m_bShowBones;
	BOOL            m_bIsAnimationed;

    // bone mesh
    KG3DMeshBone    m_boneMash; 
public:
	HRESULT GetMeshBoneToBipInfo(int& nNumBoneBip,int& nNumBoneMesh);

	float GetIlluminationLevel(){ return m_fIlluminationLevel; }
	void GetBindToOrgModel(set<IKG3DModel*> &set_OrgModel);
	void OnBindOrgModel(KG3DModel* pModel);

	std::set<KG3DModelLod>m_ChildModels;//Models bind to this model
    std::set<KG3DModelLod>::iterator m_itChildModel;
	virtual HRESULT ShowBones(BOOL bShow);
    BOOL IsShowBones(){return m_bShowBones;}

	virtual HRESULT UnbindChild(const TCHAR* strBindObjName);
	HRESULT AttachBip(KG3DBip* pBip);

	virtual int AttachModel(IKG3DModel *pModel);
	virtual void DetachModel(IKG3DModel* pModel);

	std::vector<int> m_Mesh2Bip;
	KG3DBip *m_pBip;
	
	D3DXMATRIX* m_pBoneLocalMatrixBuffer;
	D3DXMATRIX* m_pBoneMatricesForRender;
	virtual HRESULT EnableWarper(BOOL bEnable);
		
	void GetMatrixWorldInv(D3DXMATRIX& matWorldInv);

	HRESULT SetBoneMatrix(
		UINT uIndex, 
		D3DXMATRIX* pMat,D3DXMATRIX& matWorldInv
		);

	HRESULT SetBoneMatrixLocal(UINT uIndex, D3DXMATRIX* pMat);

	typedef HRESULT (KG3DModel::*pHelper_PlayAnimation)(
		DWORD dwPlayType, 
		LPCTSTR strAnimationName, 
		FLOAT fSpeed, 
        int   nOffsetTime, 
		PVOID pReserved,
		enuAnimationControllerPriority Priority,
		enuModelPlayAnimationType AnimationType,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);

	typedef BOOL (KG3DModel::pOnProcessComand)(KG3DModelCommandBase* pCmd);

	float  m_fViewDistance;//和摄像机的距离.

public:
	//动画
	virtual HRESULT LoadAnimationFromFile(LPCSTR strFileName, DWORD Option);//从文件中载入动画，可以根据Option选择覆盖还是添加，或者去处文件中指定类型的动作
	virtual HRESULT LoadAnimationFromFileEx(LPCSTR strFileName, DWORD Option,DWORD SrcID,DWORD DestID) ;//从文件中载入指定动画，并覆盖指定DestID的动画
	virtual void PauseAnimation(BOOL bPause);
	virtual HRESULT PlayAnimation(DWORD dwPlayType, 
		FLOAT fSpeed, 
		int   nOffsetTime, 
		PVOID pReserved, 
		PVOID pUserdata,
		enuAnimationControllerPriority Priority);//用指定播放方式播放指定动画，Time 是相对于动画起点的相对时间
	virtual HRESULT PlayAnimation(DWORD dwPlayType, 
		LPCTSTR strAnimationName, 
		FLOAT fSpeed, 
		int   nOffsetTime, 
		PVOID pReserved, 
		PVOID pUserdata,
		enuAnimationControllerPriority Priority);//用指定播放方式播放指定动画，Time 是相对于动画起点的相对时间
	virtual HRESULT PlaySplitAnimation(LPCSTR strAnimationName,
		DWORD SplitType,
		DWORD dwExtraInfo,
		DWORD dwPlayType, 
		FLOAT fSpeed,
        int   nOffsetTime,
		PVOID pReserved,
		PVOID pUserdata,
		enuAnimationControllerPriority Priority);
    HRESULT PlayTaggedAnimation(LPCSTR strFileName, 
		float fSpeed, 
		DWORD dwType, 
		enuAnimationControllerPriority Priority);
	KG3DAnimationWarper* GetAnimationWarper();


	virtual HRESULT Helper_PlayAnimationNormal(
		DWORD dwPlayType, 
		LPCTSTR strAnimationName, 
		FLOAT fSpeed, 
        int   nOffsetTime, 
		PVOID pReserved,
		enuAnimationControllerPriority Priority,
		enuModelPlayAnimationType AnimationType,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);

	HRESULT _Helper_PlayAnimationNormal(
		DWORD dwPlayType, 
		KG3DClip* pClip, 
		FLOAT fSpeed, 
		int   nOffsetTime, 
		DWORD dwTweenTime,
		enuAnimationControllerPriority Priority);	

protected:
	virtual HRESULT Helper_PlayAnimationTagedAnimation(
		DWORD dwPlayType,
		LPCSTR strFileName,
		FLOAT fSpeed,
        int   nOffsetTime,
		PVOID pReserved,
		enuAnimationControllerPriority Priority,
		enuModelPlayAnimationType AnimationType,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);

protected:
	KG3DAnimationSplitter m_Splitter;
	
public:
	
	HRESULT UpdateBoneBBox(KG3DScene* pScene);
	HRESULT UpdateSelBoneBBox(KG3DScene* pScene ,D3DXMATRIX* matWorld);
	virtual HRESULT UpdateBoneBBoxInfo();
    virtual HRESULT UpdateBoneMesh(KG3DScene* pScene);
	virtual void SetBoneMeshAttachScene(KG3DScene* pScene);
    HRESULT UpdateBoneMeshSel();
    KG3DMeshBone* GetBoneMesh(){return &m_boneMash;};

public:
	virtual IKG3DAnimationController* GetAnimationController(enuAnimationControllerPriority nPriority);
    IKG3DAnimationController* GetSplitAnimationContorller(DWORD dwPartIndex, DWORD dwControllerIndex);
    virtual HRESULT IEGetAnimationController(IKG3DAnimationController **ppiAniController);
    virtual HRESULT GetSFXSuede(IEKG3DSFXSuede **ppSFX);

	KG3DAnimationController m_AnimationController;
protected:
    KG3DSFXSuede*   m_pSFXSuede;
//Bone and sockets
private:
	
	D3DXMATRIX* m_pSocketMatrix;
	DWORD       m_dwNumSockets;

public:
	virtual int FindSocket(const TCHAR* strSocketName, BindExtraInfo* pExtInfo);
	virtual int FindBone(const TCHAR* strBoneName, BindExtraInfo* pExtInfo);
    virtual HRESULT FindBone(const TCHAR* strBoneName, BindExtraInfo* pExtInfo, INT *pnRet);
	virtual HRESULT GetNumBones(INT *pRetNumBone);
	virtual HRESULT GetNumSocket(INT *pRetNumSocket);
	virtual LPCSTR GetBoneName(int nIndex);
	virtual HRESULT GetSocketName(int nIndex, LPCTSTR *ppRetName);

    virtual DWORD GetVertexNum();
    virtual DWORD GetFaceNum();

    virtual BOOL IsModelST() { return FALSE; }

	struct PostMeshLoadData
	{
		D3DXMATRIX** ppLocalMatrices;
		D3DXMATRIX** ppRenderMatrices;
		DWORD*       pNumSockets;
		D3DXMATRIX** ppSocketMatrices;
	};

	//Get Bone matrix
	virtual HRESULT GetBoneMatrix(
        UINT nBoneIndex, 
		D3DXMATRIX*  pMat, 
		D3DXVECTOR3* pvec3Offset = NULL
    );
	HRESULT GetBoneMatrixLocal(
		UINT uIndex, 
		D3DXMATRIX* pMat, 
		D3DXVECTOR3* pvec3LocalOffset
		);

    virtual HRESULT GetBoneMatrixOrg(UINT nBoneIndex, D3DXMATRIX* pMatOrg); 
    virtual HRESULT GetBoneFamily(
        DWORD dwBoneIndex,
        DWORD *pdwRetParentIndex, 
        DWORD *pdwRetChildBoneNum,
        const DWORD **ppdwRetChildBoneIndexs
    );

	HRESULT UpdataBoneFamily(DWORD dwBoneIndex);


	//Get Socket matrix
	virtual HRESULT GetSocketMatrix(unsigned int uSocketIndex, 
		D3DXMATRIX& mat, 
		D3DXVECTOR3* pvec3Offset = NULL);

	//Bind a model to another model's socket
	virtual HRESULT BindToSocket(IKG3DModel *pModel, const char cszSocketName[]);
    virtual HRESULT _BindToSocket(KG3DModel *pModel, const char cszSocketName[]);
	
	//Bind a model to another model's bone
	virtual HRESULT BindToBone(IKG3DModel* pModel, const char* strBoneName);
	virtual HRESULT _BindToBone(KG3DModel* pModel, const char* strBoneName);

	//UnBind from another model
	virtual HRESULT UnBindFromOther();
	
	//Get current parent model
	virtual HRESULT GetParentModel(IKG3DModel **ppModel);

    //Get Current bind bone name
	virtual HRESULT GetBindBoneName(std::string &strName);
	virtual HRESULT IEGetBindBoneName(LPCTSTR *ppszName);

	//Get bind type
	virtual enuModelBindType GetBindType();
    virtual HRESULT IEGetBindType(enuModelBindType *pEnu);

	//bind callback function
	virtual void OnBind(IKG3DModel* pModel,KG3DModel* pModelMedium,KG3DModel* pModelLow);


	virtual HRESULT GetBoneMatrix(const BindExtraInfo* pInfo, 
		D3DXMATRIX* pmatResult, 
		D3DXVECTOR3* pvec3Offset);
	
	virtual HRESULT GetSocketMatrix(const BindExtraInfo* pInfo, 
		D3DXMATRIX* pmatResult,
		D3DXVECTOR3* pvec3Offset);

	virtual void GetBBox(D3DXVECTOR3& vec3A, D3DXVECTOR3& vec3B);

//	virtual HRESULT ComputeBigBox(D3DXMATRIX* pmatScale = NULL);
	virtual HRESULT ComputeMeshBigBox();
	virtual void SetbDrawBigBox(BOOL val);
	virtual BOOL GetbDrawBigBox();
	HRESULT DrawBoneBBox();
	AABBOX	GetBBox(){return m_BBox;}

public:
    virtual void SetBoneLODLevel(unsigned int uLevel) { ASSERT(FALSE); }
	HRESULT UpdateSocketsEx(int nIndex);

protected:
	BOOL m_bBindChanged;

public:
	//D3DLIGHT9 m_LightSave;
	//DWORD     m_AmbSave;

	void RenderLogicCellBox();
private:
	LogicCellBBox* m_pLogicalBox;
    int m_nCellInX;
    int m_nCellInY;
    int m_nCellInZ;

	//Update Sockets
	void UpdateSockets(D3DXMATRIX* matSrc,int nCount);
	void UpdateSockets(D3DXMATRIX* matSrc,int nCount,int nIndex);

	virtual void _SeekSplitAnimation(DWORD dwSplitType, 
		enuAnimationControllerPriority nPrority,
		DWORD dwSeekType, 
		float fOffset);

	HRESULT _UpdataBoneFamily(KG3DMesh::BoneInfo1 *pBoneInfo,D3DXMATRIX& matBone);

public:
	KG3DMaterial::KG3DMaterialSubset* GetMaterialSubset(DWORD dwIndex);

	LPD3DXMESH GetRenderXMesh();

	HRESULT RenderSubsetDepthOnly(int nSubsetIndex,KG3DMaterial::KG3DMaterialSubset* pSubMtl);

	HRESULT Shader_DrawMeshSubset_Begin(int nSubsetIndex,KG3DMaterial::KG3DMaterialSubset* pSubMtl,LPD3DXEFFECT pShader);
	HRESULT Shader_DrawMeshSubset_NoChange(int nSubsetIndex,KG3DMaterial::KG3DMaterialSubset* pSubMtl,LPD3DXEFFECT pShader);
	HRESULT	Shader_DrawMeshSubset_End(int nSubsetIndex,KG3DMaterial::KG3DMaterialSubset* pSubMtl,LPD3DXEFFECT pShader);

	//////////////////////////////////////////////////////////////////////////
	//HRESULT Shader_DrawMesh_Begin();
	//HRESULT Shader_DrawMesh_NoChange();
	//HRESULT Shader_DrawMesh_End();

	//////////////////////////////////////////////////////////////////////////
	// Last modify by huangjinshou 2007.11.15
	// 增加一个tech参数，供选择使用shader文件里的tech，默认为DefaultTech
	//HRESULT Shader_DrawMeshAmbientMap_Begin();
	//////////////////////////////////////////////////////////////////////////
	//HRESULT Shader_DrawMeshAmbientMap_NoChange();
   // HRESULT	Shader_DrawMeshAmbientMap_End();

	HRESULT Shader_RenderSkinMesh_Begin(BOOL bUseConverTexture);
	HRESULT Shader_RenderSkinMesh_NoChange();
	HRESULT Shader_RenderSkinMesh_End();

    HRESULT Shader_RenderSkinMesh_SingleSubset();

	HRESULT Shader_RenderSkinMeshDepth_Begin();
	HRESULT Shader_RenderSkinMeshDepth_NoChange();
	HRESULT Shader_RenderSkinMeshDepth_End();

	static HRESULT Shader_DrawMeshNormalNoSkin_Begin();
	static HRESULT Shader_DrawMeshNormalNoSkin_End();
	HRESULT Shader_DrawMeshNormalNoSkin_NoChange();

	DWORD GetShaderType();

	virtual HRESULT RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);
	virtual BOOL IsSecondUVExist();
	virtual void SetParentObjMatrix(D3DXMATRIX* pMatrix);
	virtual D3DXMATRIX* GetParentObjMarrix(D3DXMATRIX* pMatrix);
	void CopyBindInfo(KG3DModel* pModel);

	DWORD GetSceneLayerID();
	void  SetSceneLayerID(DWORD dwID);

    // SFX
    //HRESULT UndateSuedeBindInfo();
    HRESULT BindSFXSuedeFromFile(LPCSTR pstrBindFileName, IKG3DScene* pActiveScene = NULL);
    DWORD   GetSocketCount();
    //HRESULT EnabelSFXSuede(int nEnabel = TRUE);

    virtual vector<KG3DModel*> GetAllChilds();
	virtual HRESULT GetAllChilds(vector<IEKG3DModel*> &vecpChildsModel); // by huangjinshou 2007 8.9
	virtual HRESULT GetAllChilds(vector<IKG3DModel*> &vecpChildsModel);

    virtual HRESULT GetRenderTargetTextureCount(OUT DWORD* pTexCount);
    virtual HRESULT GetRenderTargetTexture(OUT DWORD* pResourceId, IN DWORD dwTexIndex = 0);

    virtual HRESULT SetCallbackRender(CallbackRender fpCallbackRender, void* pUserData);
	HRESULT EnableCloseFloor(int nEnableFlag){m_nCloseFloorFlag = nEnableFlag; return S_OK;};

    STDMETHOD(UpdateChannelEffectPos)(D3DXVECTOR3 v1, D3DXVECTOR3 v2) {return E_FAIL;}

	virtual HRESULT SetEditMode(BOOL bEditMode, 
		IEKG3DSceneModelEditor* pScene);
	virtual D3DXMATRIX* GetBoneAnimationResultPool();

    virtual bool  CheckNeedRender();
    virtual void  SetShockWavePower(float power) { m_fShockWavePower = power; }
    virtual float GetShockWaePower()             { return m_fShockWavePower;  }

public :
    DWORD   m_dwPlayType;
    int     m_nIsSockWave;      
    DWORD   m_dwShockMode;
	int		m_nCloseFloorFlag;
    float   m_fShockWavePower;

	KG3DMesh* m_lpMeshMedium;//自己对应的低模
	KG3DMesh* m_lpMeshlLow;
	KG3DMesh* m_lpMeshCurrent;

	KG3DModel* m_lpModelMedium;
	KG3DModel* m_lpModelLow;
protected:
	D3DXMATRIX* m_pParentMatrix;

	KG3DOBJProperty* m_pPropertys;//物件属性

	DWORD m_dwSceneLayerID;


    // ----------------------- afterimage -------------------------------
public :
    int     m_nEnagleAfterimage;
    int     m_nAftImageCallFlag;
    int     m_nAftImageLife;
    struct _AfterimageNode
    {
        D3DXMATRIX matWorld;
        DWORD      dwAniOffset;
        INT        nLife;
    };
    vector<_AfterimageNode> m_vecAftImageNode;
    float   m_fAfterimageLayDelay;
    float   m_fCurAftiimageLayDelay;

    DWORD   m_dwPrevTime;

public :
	KG3DFuctionCommand m_Commands;
    KG3DSFXSuede*    m_pParentSFXSuede;
	BOOL m_bSelfDestructFramemoveFlag;

	D3DXVECTOR3 m_Translation_Last;
public:
	virtual HRESULT GetBoneScale(DWORD dwIndex, D3DXVECTOR3 *pScale);
	virtual HRESULT SetBoneScale(DWORD dwIndex, D3DXVECTOR3 *pScale);
	virtual HRESULT SetBoneTransform(LPCSTR strBoneName,
		const BoneTransformationInputData& Data);
	virtual void EnableBoneTransform(BOOL bEnable);
	virtual BOOL IsBoneTransformEnable();
protected:
	BOOL m_bRenderSpecialAlpha;
	BOOL m_bBoneTransformEnable;
	KG3DRuntimeMaterial m_RuntimeMaterial;
	HRESULT RenderCommon(unsigned uOption, void *pExtInfo);
	
public:
	void UpdateChildRuntimMaterial();
	void SetRenderSpecialAlpha(BOOL bEnable);
	virtual HRESULT RenderSpecialAlpha(unsigned int uOption, void* pExtInfo);
	KG3DRuntimeMaterial* GetRuntimeMaterial();
	void GetRenderEnviromentParamenter(IKG3DScene* pScene);
//(by dengzhihui Refactor 2006年11月9日
public:
	//HRESULT ComputeapBoneMatInObjSpace(DWORD dwOption,D3DXMATRIX* pmatBones);

	HRESULT FrameMove(DWORD dwOption,void *pExtInfo);
	HRESULT Helper_UpdateTransformation(BOOL bUpdateBBox);

    virtual D3DCOLORVALUE* GetMaterialSubsetColorCast(int nSubIndex,int nColorCastIndex);
    virtual float*         GetMaterialSubsetSpecPower(int nSubIndex);
    virtual float*         GetMaterialSubsetEmssPower(int nSubIndex);

	virtual HRESULT GetMaterialSubsetOption(INT nSubIndex, DWORD* dwOption);	//pSubMat->m_dwOption
	virtual HRESULT SetMaterialSubsetOption(INT nSubIndex, DWORD dwOption);		//pSubMat->m_dwOption
	virtual HRESULT GetNumMaterial(INT* pnCount);
	virtual HRESULT GetMaterialSubsetMtlOptionData(INT nSubIndex, DWORD dwType, DWORD dwExtInfo, INT* pnData);
	virtual HRESULT GetMaterialSubSetMaterial9(INT nSubIndex, D3DMATERIAL9** ppMaterial9);

	virtual HRESULT RemoveTexture(DWORD dwSubsetIndex, 
		DWORD dwIndex);
	
	virtual HRESULT ChangeTexture(DWORD dwSubsetIndex, 
		DWORD dwIndex, 
		LPCSTR strNewTextureFileName);

	virtual HRESULT RelaseMaterial();
	virtual HRESULT SetMaterialSubSetAlphaOption(INT nSubIndex, DWORD dwSrcBlend, DWORD dwDestBlend, BOOL bEnableAlphaBlend);
	virtual HRESULT SetMaterialSubsetRenderTwiceAlphaOption(INT nSubsetIndex,
		BOOL bEnable,
		DWORD dwTopRef,
		DWORD dwBottomRef);

	virtual HRESULT GetMaterialSubsetRenderTwiceAlphaOption(INT nSubsetIndex,
		BOOL& bEnable,
		DWORD& dwTopRef,
		DWORD& dwBottomRef);
	
	virtual HRESULT CopyMaterialOption(DWORD dwType, DWORD dwExtInfo, unsigned int nSrc, unsigned int nDest);
	
	virtual HRESULT GetMaterialName(LPTSTR lpName, INT nBufLen);	//lMat->m_scName.c_str()
	virtual HRESULT SaveMaterialToFile(LPCTSTR lpFileName, unsigned uOption); //m_lpCurModel->m_lpMaterial->SaveToFile
    virtual HRESULT SaveMeshToFile(LPCTSTR lpFileName, unsigned uOption);
    virtual HRESULT ResizeSocketMatrix();

	virtual BOOL IsAnimationed();

public :
	void OnSecondaryAnimationFinish(KG3DAnimationController *pController);
	virtual void SetClientAdd(BOOL bClient);//是否是客户端加入的模型
	virtual BOOL IsClientAdd();
	virtual void FrameMoveOnce();
protected:
	FLOAT	ComputeShadowScaleFactor(BBox& BBox);
	BOOL m_bClientAdd;
private:
    KG3DAnimationTagContainer* m_pTaggedAnimationContainer[ANICTL_COUNT];
	// Data Members For Attach Ground
	IKG3DScene* m_pSceneToAttach;
	MODELCLOSEGROUND_TYPE m_AttachType;
	D3DXVECTOR3 m_vec3LastPosInModel;
	D3DXQUATERNION m_LastDirInModel;
	float m_fLastAngleInModel;
	int  m_nMtlLoadCmd;
protected:
	HRESULT AdjustToAttachGround();
public:
	virtual HRESULT Playing(){return S_OK;}//// huangjinshou 2008-9-12

	virtual void SetFaceMotionMode();
	HRESULT RestoreFace();
	//头顶文字相关
public:
	virtual void SetCaptionFloorHeight(float fHeight);
	virtual HRESULT SetCaptionVisible(DWORD dwIndex, BOOL bVisible);
	virtual HRESULT SetCaption(DWORD dwIndex, LPCSTR strContent);
	virtual void RemoveCaption();
	virtual HRESULT SetCaptionColor(DWORD dwIndex, const D3DCOLORVALUE& Color);
	virtual HRESULT SetPercentage(float fPercentage);
	virtual HRESULT SetPercentageStyle(const D3DCOLORVALUE &Color);
	virtual HRESULT SetPercentageVisible(BOOL bVisible);
protected:
	void Helper_CaptionFrameMove();
private:
	DWORD m_dwHeadLineID[KG3DCaption2D::MAX_NUM_CAPTION];
	float m_fHeadLineHeight;
	DWORD m_dwSelectIndex;//用于KG3DSelector的Index

public:
	virtual void SetSelectIndex(DWORD dwIndex);
	DWORD GetSelectIndex();
	virtual void RenderForSelector(DWORD dwOption, void *pExtInfo);

	virtual HRESULT ReloadMesh();
    virtual HRESULT LoadPostProcess();
	
protected:
	HRESULT ProcessCommands();
	virtual BOOL OnProcessCommands(KG3DModelCommandBase* pCmd);
	DWORD m_dwPlayAnimationCommandCnt;

	HRESULT _PlayAnimation(DWORD dwPlayType, 
		LPCTSTR strAnimationName, 
		FLOAT fSpeed, 
		int   nOffsetTime, 
		PVOID pReserved,
		PVOID pUserdata,
		enuAnimationControllerPriority Priority,
		enuModelPlayAnimationType Type,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);

	HRESULT _PlaySplitAnimation(LPCSTR strAnimationName,
		DWORD SplitType,
		DWORD dwExtraInfo,
		DWORD dwPlayType, 
		FLOAT fSpeed,
		int   nOffsetTime,
		PVOID pReserved,
		PVOID pUserdata,
		enuAnimationControllerPriority Priority,
		enuModelPlayAnimationType AnimationType,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);

	HRESULT _LoadFromFile(const char cszFileName[], 
		unsigned uFileNameHash, 
		unsigned uOption);
};

#ifdef KG3DENGINE_DISABLE_MULTITHREAD
void UpdateMultiThreadBoneAnimation(void * pInput);
#endif
