////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRepresentObject.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-9-27 14:46:19
//  Comment     : Object Class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DREPRESENTOBJECT_H_
#define _INCLUDE_KG3DREPRESENTOBJECT_H_

#include "../../../Include/kg3dengine/kg3dinterface.h"
#include "KG3DTransformation.h"
#include "KG3DModel.h"
#include "KG3DRepresetObjectPropertyModifior.h"
#include "KG3DRepresentObjectTemplate.h"
#include "IEKG3DRepresentObject.h"
#include "KG3DToolsImpl.h"
#include "KG3DFrustum.h"
#include "KG3DMemoryFile.h"


class KG3DSceneSpaceNode;
struct KG3DIntersectionFilter;
class KG3DCullData;
class KG3DScenePointLightRender;


#define OBJLOAD_NONE       0
#define OBJLOAD_MULTITHEAD 1

enum
{
	TOBF_LUMP_HEADER	=	0x46424F54,		// 'MOBF', map object flag
	TOBF_LUMP_SECT		=	0x46434F4C,		// 'MOBL', map object list
};

/*

TObfHeader_T
TObfSect_T * dwMapObjs;
TObfRegion_T * dwMapObjRegions;

*/

#pragma pack(2)
struct TObfHeader_T
{
	DWORD  dwMask;
	DWORD  dwVersion;

	DWORD  dwMapObjs;			// map object nums, 总计 64k 个对象应该够了吧 (￣ε ￣")
	DWORD  dwMapObjRegions;

};

struct TObfSect_T
{
	UINT   nOffset;
	UINT   nSize;
};

struct TObfRegion_T
{
	SHORT  nIndexX, nIndexZ;			// 表示挂接到哪个 Region
	USHORT nMapObjs;					// region map object nums
	USHORT nMapObjRefs[127];			// region map object ref nums, 总计 127 应该够了吧 (￣ε ￣")

};

#pragma pack()

class KG3DScene;
class KG3DBsp;

class KG3DSceneEntityContainer;
class KG3DRepresentObjectPVS;
class KG3DSceneRegion;
class KG3DSceneSceneEditor;

class KG3DRepresentObject;

struct KG3DRepresentObjectVisitor 
{
	virtual HRESULT Accept(KG3DRepresentObject& Obj) = 0;
	virtual ~KG3DRepresentObjectVisitor() = 0{}
};

class KG3DRenderSorterStratege;
class KG3DRepresentObject : 
	public CKG3DConnectionPoint<IEKG3DRepresentObject>,
	public KG3DTransformation ,
	public IEKG3DSimpleSink
{
	friend class KG3DSceneObjectEditor;
	friend class KG3DSceneSceneEditor;

private:
	//KG3DScene* m_lpScene;//<SetScene Refactor>//减少耦合，这个成员是不需要的
	//list<KG3DSceneRegion*>m_listSceneRegion;//和自己关联的Region,用于记录自身容器的机制
	set<KG3DSceneEntityContainer*>m_setRefrenceContainer;//不要Public

	float m_fDistance;//用来做排序的距离

	DWORD m_dwType;   //这个是很基本的成员，用于标志类型，要private，否则胡改的话，一些强转就死定了。
	DWORD m_dwID;
	KG3DOBJProperty* m_pProperty;

	DWORD m_dwVisibleCount;	//可视计数	用GetSetVisibleCount访问
	KG3DRepresentObject* m_pParentObj;	//用GetSetParentObj访问

	ULONG   m_ulRefCount;      //引用计数千万别public的好
public:
    virtual size_t GetSubModels(vector<KG3DModel*>& vModels);

	struct ModelInformation//模型扩展信息,主要作用在于保存合并近来的模型是否来源于MODELSET,保证分解时刻可以还原为MODELSET
	{
		int  nTypeSource;//来源类型 0,默认 1，modelset
		std::string szSource;//来源文件名
		int  nIndex;//
		ModelInformation()
		{
			nTypeSource = 0;
			szSource = "";
			nIndex = 0;
		}
	};
	enum OBJECT_STATE
	{
		REPRESENTOBJECTSTATE_NONE,
		REPRESENTOBJECTSTATE_RESOURSENOTREADY,
		REPRESENTOBJECTSTATE_ZOOMIN,
		REPRESENTOBJECTSTATE_ZOOMOUT,
	};
protected:

    // use for editor (for group obj)
    D3DXVECTOR3     m_vGroupTrans;
    D3DXVECTOR3     m_vGroupScal;
    D3DXQUATERNION  m_qGroupRota;
    int             m_bGroupPoseValidate;

	vector<ModelInformation*>m_vecModelInformation;	//用于合并等
	vector<KG3DModel*>m_vecModel;
	DWORD m_dwSceneLayerID;//场景中层的序号

	const D3DXMATRIX*  m_pMatParent;
	D3DXMATRIX  m_matWorld;	//用GetWorldMatrix得到

	float m_fAlpha;

	float m_fAlphaWhenStateChanged;//m_fAlphaChangeTime。当状态改变时候的Alpha

	OBJECT_STATE m_eState;//用于多线程淡入淡出	//用GetState得到
public:

    KG3DRepresentObjectPVS* m_pPvsOn;
	float m_fVisibleAngle;//在摄像机下的角度，单位是弧度
	std::string  m_scName;
	AABBOX  m_BBox;
    DWORD m_dwLastCullCounter;
    BOOL    m_bBBoxChanged;
	//BOOL m_bSelected;
public:
	//引用计数
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		REFIID riid, void **ppvObject
		);

	virtual HRESULT Init();
	virtual HRESULT UnInit();

	//KG3DRepresentObjectBuilder可以用Load来绑定内部物件了，不管是模型还是别的，也不管是ini还是别的东西
	virtual HRESULT Load(LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam);//这个用于加载附属的模型或者别的东西

	//Load和LoadFromFile是不一样的，LoadFromFile是保存本身，在Template中用，Load是加载所有的
	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);

	//用于记录自身容器的机制，新
	HRESULT OneInputContainer(KG3DSceneEntityContainer* pContainer);
	HRESULT OnRemoveFromContainer(KG3DSceneEntityContainer* pContainer);
	HRESULT RemoveFromContainer();


    HRESULT OnEnterPvs(KG3DRepresentObjectPVS* pvs);
    HRESULT LeavePvs();

    virtual HRESULT ResetGroupPose();
    virtual void GetBspModel(vector<KG3DModel*>* pvModel);


	//用于记录自身容器的机制，旧
	//HRESULT AddSceneRegion(KG3DSceneRegion* pRegion);
	//HRESULT RemoveFromRegions();
	//HRESULT ClearSceneRegionList();

	virtual void SetPhysicsScene(KG3DPhysiscScene *pScene);
	//HRESULT SetScene(KG3DScene* pScene);//<SetScene Refactor>

	// 获取动态障碍分组索引号, INVALID_OBSTACLE_GROUP表示非动态障碍
	int GetDynamicObstacleGroupIndex();
	virtual HRESULT SetDynamicObstacleGroupIndex(int nindex);

	FLOAT	GetDistance()				{return m_fDistance;}
	VOID	SetDistance(FLOAT fDistance){m_fDistance = fDistance;}

	const D3DXMATRIX& GetMatrixWorld()				{return m_matWorld;}
	const D3DXVECTOR3& GetWorldPos();
	VOID	SetMatrixWorld(const D3DXMATRIX& mat)	{m_matWorld = mat;}

	FLOAT	GetAlpha()					{return m_fAlpha;}
	OBJECT_STATE	GetState()			{return m_eState;}

	virtual BOOL IsLoaded();
    const D3DXMATRIX* GetParentMatrix() { return m_pMatParent; }

	virtual HRESULT OnLostDevice(){return S_OK;};
	virtual HRESULT OnResetDevice(){return S_OK;};

	// use for npc
	virtual HRESULT SetModelFace(LPCTSTR pMeshFile,LPCTSTR pMtlFile,LPCTSTR pAniFile,LPCTSTR pSocketName) {return E_NOTIMPL;};
	virtual HRESULT PlayAnimation(LPCSTR szAniPath) {return E_NOTIMPL;};

	virtual HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>

	virtual HRESULT ChangeModel(INT nChangeCode){return E_FAIL;}	//在OnCommand的地方被调用了

	DWORD GetModelCount();

	KG3DModel* GetModel(DWORD dwIndex);
	KG3DModel* GetModel(DWORD dwIndex,INT nLODLevel);
	virtual HRESULT GetModel(KG3DModel*&  pModel, DWORD dwIndex,BOOL bLod);
	virtual HRESULT GetModel(IKG3DModel*& pModel, DWORD dwIndex);
	virtual HRESULT SetModel(DWORD dwIndex,
		const char pcszMeshFileName[],
		const char pcszMtlFileName[],
		const char pcszAniFileName[],BOOL bForceLoad);

	virtual HRESULT BindModel(LPCSTR strBindTo, 
		KG3DModel* pModel);

	HRESULT GetModelFileName(int nIndex,
		LPSTR pMeshFile,LPSTR pMtlFile,LPSTR pAniName);

	virtual HRESULT GetName(LPTSTR pName, size_t BufferSize);
	virtual const AABBOX& GetBBox();

	virtual HRESULT Reload();

	virtual void UpdateTransformation();
	int  IsSockWave();

	virtual void SinkFunc(INT nSinkID);
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);

	// virtual DWORD GetObjType()   { return REPRESENT_TYPE_NORMAL; }//用GetType来判断就可以了
	virtual BOOL  IsVisible(const vector<KG3DFrustum>& vFrustum);

	KG3DRepresentObject*	GetParentObject()						{return m_pParentObj;}
	VOID					SetParentObject(KG3DRepresentObject* p)	{m_pParentObj = p;}

	DWORD GetVisibleCount(){return m_dwVisibleCount;};
	void  SetVisibleCount(DWORD dwCount){m_dwVisibleCount = dwCount;};

    virtual void UpdateVisiableSubset(KG3DCullData* pCull) { };

	virtual HRESULT RefreshOption(unsigned uOption);
	virtual HRESULT SaveToFile(const char cszFileName[],   unsigned uOption);
	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam)
	{
		return LoadFromFile(cszFileName, uFileNameHash, uOption);
	}
	virtual DWORD GetID();
	void	SetID(DWORD dwID)		{ m_dwID = dwID;}
	virtual unsigned GetType();
	void	SetType(DWORD dwType)	{m_dwType = dwType;}

    virtual KG3DSceneSpaceNode* Conver2SpaceNode() { return NULL; }

	virtual HRESULT PrepareRender(unsigned dwOption);  
	virtual HRESULT PostRender();                          
	virtual HRESULT GetDateInformation(        
		int* pnMem,int* pnVedioMem,            
		int* pnConMem,int* pnConVMem           
		);                                                    
	virtual unsigned   GetLastUseTime();       
	virtual unsigned   GetUseFrequency();      

	virtual HRESULT Copy(KG3DRepresentObject* pSrcObject);//派生类，只需要覆盖这个就好了，而且只需要处理数据，另外的一个Object是由ObjectTable管理生命周期的

	virtual HRESULT GetAnotherIEInterface(DWORD dwType, VOID** ppAnotherInterface){return E_FAIL;}
public:

    virtual HRESULT OnTransformationChange(KG3DScene* pScene);
    virtual void OnRemoveFromEntityMgr(KG3DCullData* pCull) {  }
	virtual HRESULT OnInformationChange(KG3DScene* pScene);
	//<事件>
	virtual HRESULT OnDeleteFromScene(KG3DSceneSceneEditor* pEditor){return E_NOTIMPL;}
	//virtual HRESULT OnAddToScene(KG3DSceneSceneEditor* pEditor){return E_NOTIMPL;}//没有检查到任何调用
	virtual HRESULT OnSelect(){return S_OK;}
	virtual HRESULT OnUnSelect() {return S_OK;}
	//</事件>

    virtual BOOL HasSfx();

	virtual void SetParentMatrix(const D3DXMATRIX* pMat);

	virtual BOOL    IsResourceReady();
	HRESULT ChangeState(OBJECT_STATE eState);

	ModelInformation* GetModelInformation(int nIndex);
	HRESULT SetModelInformation(int nIndex,ModelInformation* pInfo);

	virtual HRESULT RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);

	virtual void GetProfile(DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID);

	virtual HRESULT RenderPlaneShadow(D3DXPLANE& plane,const D3DXVECTOR4& LightDir);
	virtual HRESULT RenderShadowMap(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP);
	virtual HRESULT RenderSelfShadow(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,
		const D3DXMATRIX &matWVP,LPDIRECT3DTEXTURE9 lpDepthMap);
	virtual HRESULT ComputeBBox();
	virtual HRESULT ComputeDynamicBBox();
    virtual HRESULT RenderHierarchy(BOOL bCull = TRUE, KG3DScenePointLightRender* pRender = NULL);
	virtual HRESULT FrameMove();
	virtual HRESULT Render(DWORD dwOption);
	virtual HRESULT RenderSelectMark(DWORD dwColor = 0xFFFF0000);

	//<碰撞检测>
	virtual BOOL IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);
	// bFiltrate 是否过滤属性中设置的不检查标志模型
	virtual BOOL GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction, BOOL bFiltrate = FALSE);//得到所有的碰撞点
	virtual BOOL IsVertexInside(D3DXVECTOR3& vVertex, D3DXVECTOR3& vDir);
	BOOL GetCameraNearRayIntersect(
		IN CONST D3DXVECTOR3 vSrc[],
		IN CONST D3DXVECTOR3 vDst[],
		IN CONST INT         nNum, 
		OUT      FLOAT*      pDis 
		);//得到离Des最近的交点，最内层是Mesh的SSE的代码。注意其使用，主要是给摄像机用的
	virtual BOOL RayIntersection(const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vNormalizedDir
		, FLOAT* pfRet
		, BOOL bIntersectBoxOnly
		, KG3DIntersectionFilter* pFilter = NULL);//只需要和线碰撞的最近点，其它忽略，由vSrc + vNormalizedDir * fRet就可以得到交点坐标
	//GetNearRayIntersect的功能由RayIntersection负责
	/*virtual BOOL GetNearRayIntersect(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir
		, FLOAT* pfRet);*/
	//BOOL GetRayIntersectPointAndDir(DWORD& dwCount,
	//	D3DXVECTOR3* pInterPoint,
	//	float* pAngle,
	//	const D3DXVECTOR3& vec3Org,
	//	const D3DXVECTOR3& vec3Dir);//得到碰撞点和碰撞点的Angle
	//</碰撞检测>

	//<Trans>
	virtual HRESULT SetTranslation(const D3DXVECTOR3* pTrans);
	virtual HRESULT SetRotation(const D3DXQUATERNION* pRotaion);
	virtual HRESULT SetScaling(const D3DXVECTOR3* pScale);
	//</Trans>

	

	KG3DRepresentObject(void);
	virtual ~KG3DRepresentObject(void);

	HRESULT SetLOD(float fLod);

	BOOL IsSecondUVExist();
	
	void SetAlpha(float fAlpha);

	DWORD GetSceneLayerID();
	void  SetSceneLayerID(DWORD dwID);

	HRESULT BindSFXSuedeFromFile(LPCSTR pstrBindFileName, IKG3DScene* pActiveScene = NULL);

	HRESULT SaveToIniFile(LPCTSTR pFileName,DWORD dwOption);
	HRESULT LoadFromIniFile(LPCTSTR pFileName,DWORD dwOption);
	virtual HRESULT GetModelCount(INT* pValue);
	//virtual void SetScaling(D3DXVECTOR3 *pValue);//上面已经有一个SetScaling了

	/*
	把用于创建的内容保存到内存中，然后到时候用LoadFromInformationBuffer创建出来
	dwType传回自己类型？dwLength传回写的多少，pFile是写Buffer。

	参考Load函数，通常因为Build的机制，IE那边会有一个IEKG3DRepresentObject_BuildStruct之类的结构体
	保存那个结构体就好了，LoadFromInformationBuffer的时候，用Load函数来加载就好了
	*/
	virtual HRESULT SaveToInformationBuffer(/*DWORD& dwType,DWORD& dwLength,*/KG3DMemoryFile* pFile);
	virtual HRESULT LoadFromInformationBuffer(/*DWORD dwType,DWORD dwLength,*/const KG3DMemoryFile* pFile,DWORD dwOption);

	virtual BOOL	IsSpecial(){return FALSE;}//特殊，指的是如Dummy那种没有绑定模型或者有特殊模型的物件，这种属性不会单独建一个表来维护的，因为虚函数表本来就是一个表，而且是语言支持的，专门建一个表来维护这种类属性相关的Policy实际上只会变得更复杂	
    virtual HRESULT LoadPostProcess();
protected:
	HRESULT OnReferenceZero();
private:
	UINT m_HandleHeldBySpaceMgr;//保存KG3DSceneOutDoorSpaceMgr中分配的Entity的Handle,以方便释放
public:
	UINT GetHandleHeldBySpaceMgr(){return m_HandleHeldBySpaceMgr;}
	VOID SetHandleHeldBySpaceMgr(UINT Handle){m_HandleHeldBySpaceMgr = Handle;}

	//KG3DRenderSorterStratege& GetRenderSorterStratege();
//<LOD>
private:
	enum{emCheckLODFrameGap = 10,};
	DWORD		m_dwLODFrameCount;   //用于控制每隔多少贴调用一次CheckLODLevel
	INT			m_nLODLevel;       // 模型LOD级别  
	enum
	{
		EM_LOD_EXIST_NONE   = 0, //没有任何的LOD模，不需要做LOD检测
		EM_LOD_EXIST_MIDDLE = 1,//0x00000001，有中模，
		EM_LOD_EXIST_LOW = 2 //0x00000010，有低模。
	};
	DWORD       m_dwLODExist;		//LOD的低模、中模文件是否存在的标志，如果没有，CheckLODLevel什么都不用做。 
protected:
	vector<KG3DModel*> m_vecModelM;
	vector<KG3DModel*> m_vecModelL;
public:
	enum
	{
		EM_LOD_LEVEL_HIGH = 0,	//0是最高精度，因为所有模型都是从最高精度做起的，高精度有限，但简模可以无限低精度
		EM_LOD_LEVEL_MIDDLE = 1,
		EM_LOD_LEVEL_LOW = 2,
	};
	HRESULT		CheckLODLevel();
	INT         GetLODLevel(){return m_nLODLevel;}	
	HRESULT SetLowModel(DWORD dwIndex,const char pcszMeshFileName[],const char scMtlName[],DWORD dwOption);
	HRESULT SetMiddleModel(DWORD dwIndex,const char pcszMeshFileName[],const char scMtlName[],DWORD dwOption);

	HRESULT SetModelTranslation(DWORD dwIndex,D3DXVECTOR3 &vTrans);
	HRESULT SetModelScaling(DWORD dwIndex,D3DXVECTOR3 &vScale);
	HRESULT SetModelRotation(DWORD dwIndex,D3DXQUATERNION &qRot);
	HRESULT UpdateModelTransformation(DWORD dwIndex);
	HRESULT ComputeModelBBox(DWORD dwIndex);

//<LOD>
	HRESULT	SetSelectable(BOOL bSelectable);
	ULONG	IsSelectable();
private:
	enum
	{
		em_state_selectable= 1 << 1,
	};
	DWORD	m_dwState;	//Selected不应要Object自己管，一个Obj为什么要知道自己是选中的，如果要知道其是否是被选中的，查SelectionTool。Selected不是Obj的属性
	int     m_nDynamicObstacleGroupIndex;
};

#endif //_INCLUDE_KG3DREPRESENTOBJECT_H_
