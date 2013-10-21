#pragma once
#include "..\..\..\include\kg3dengine\kg3dinterface.h"
#include "Scene/KG3DSceneEntity.h"
#include "KG3DCamera.h"
#include "KG3DSceneWndManager.h"
#include "KG3DEnvEffLight.h"
#include "KG3DSceneEntityMgr.h"
#include "IEKG3DScene.h"
//#include "KG3DSceneCameraMovement.h"
#include "KG3DEnvEffTimeInformer.h"
#include "KG3DGPS.h"
#include "KG3DSceneEntity.h"
#include "KG3DCommonObjectNode.h"
#include "KG3DCommonObjectPool.h"

class KG3DSceneSelectionTool;

class KG3DPhysiscScene;

class KG3DEnvironment;
class KG3DSceneOutDoorSpaceMgr;

class KG3DRegionInfoManager;
class KG3DSceneCameraMovement;
class KG3DSceneSpaceNode;
class KG3DSceneSceneEditor;
class KG3DSceneEntity;
class KG3DCommonObjectBuilder;
class KG3DSceneObjectPlacementTool;

struct KG3DMessageBroadcasterGroup;
struct KG3DTransformTool;

class KG3DScene : public IEKG3DScene
{
public:// for IEKG3DScene
	virtual HRESULT RefreshPhysisc();
	virtual HRESULT GetSceneSFXEditor(IEKG3DSceneSFXEditor **ppiRetSFXEditor);
    virtual IEKG3DScenePvsEditor* GetScenePvsEditor() { return NULL; }
    virtual IEKG3DSceneSelectBase* GetSelectBase() { return NULL; }

	// if no more entity, function return S_OK, and set *ppiRetEntity to null
	virtual HRESULT GetFirstRenderEntity(IEKG3DSceneEntity **ppiRetEntity);
	virtual HRESULT GetNextRenderEntity(IEKG3DSceneEntity **ppiRetEntity);
	virtual HRESULT FindRenderEntity(
		SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, IEKG3DSceneEntity **ppiRetEntity
		);

	//virtual HRESULT GetLightCount(DWORD *pdwRetLightCount);

	//virtual D3DCOLOR* GetAmbient();
	D3DCOLOR	GetAmbient();
	virtual HRESULT GetAmbient(DWORD *pdwRetAmbient);
	virtual HRESULT SetAmbient(DWORD dwAmbient);

	//(by dengzhihui Refactor 2006年11月23日
	virtual HRESULT GetSceneModelEditor(IEKG3DSceneModelEditor** ppModelEditor);
	virtual HRESULT GetSceneSceneEditor(IEKG3DSceneSceneEditor** ppSceneEditor);
	virtual HRESULT GetSceneObjectEditor(IEKG3DSceneObjectEditor** ppObjectEditor);
	virtual HRESULT GetSceneDataFlowEditor(IEKG3DSceneDataFlowEditor** ppDataFlowEditor);
	virtual KG3DSceneSceneEditor* GetSceneSceneEditor();

    virtual BOOL InUnderWater() const { return FALSE; }

	//)
	virtual HRESULT TraverseEntityMeshForGenerateLogicObstacle(
		float fWorldStartX, float fWorldStartY, int nRegionPosX, int nRegionPosY,
		KTraverseEntityMeshForGenerateLogicObstacleCallback* pCallback    
		);
	
	virtual HRESULT Traverse(KG3DRepresentObjectVisitor& visitor);

	virtual HRESULT ClearAllHelpLine();
	virtual HRESULT SetHelpLineSize(size_t nLine, size_t nSize);
	virtual HRESULT SetHelpLineStart(size_t nLine, size_t nStart);
	virtual HRESULT SetHelpLine(size_t nLine, int nSetID, size_t nBegin, size_t nCount, D3DCOLOR color, D3DXVECTOR3* pPos);
	virtual HRESULT GetHelpLine(size_t nLine, size_t* pPoint, D3DXVECTOR3** ppPos);

	virtual LPDIRECT3DCUBETEXTURE9 GetEnvMap(LPTSTR pNameBuffer, size_t uBufferCount);
	virtual HRESULT SetEnvMap(LPCTSTR szPath){return E_FAIL;}
	//virtual std::string GetEvnMapName() { return m_szEvnMapName; }
    
    virtual HRESULT ShockWaveRender() { return E_NOTIMPL; }

	virtual HRESULT RefreshOption(unsigned uOption);
	virtual HRESULT OnLostDevice();
	virtual HRESULT OnResetDevice();

    virtual DWORD GetDataInterfaceVersion();

	virtual ULONG GetEditState();

    virtual int GetModelOverlapBox(KG3DModel* pModels[], int maxNum, D3DXVECTOR3 box[8]);

	virtual HRESULT GetSelectionToolInterface(IEKG3DSceneSelectionTool** ppTool);
	virtual HRESULT GetPickRayPos(D3DXVECTOR3* pOutPos);

public:
    virtual HRESULT LoadCameraAnimation(LPSTR pszCameraAniFile);
    virtual HRESULT StartPlayCamAni();
    virtual HRESULT StopPlayCamAni();
    virtual HRESULT PausePlayCamAni();

private:
	HRESULT TraverseModelsForGenerateLogicObstacle(vector<KG3DModel*>& vModels,int nDynamicObstacleGroupIndex,
				int nRegionPosX, int nRegionPosY, D3DXVECTOR2& vRegionLeftBottom,D3DXVECTOR2& vRegionRightTop,
				BBox& RegionBox,KTraverseEntityMeshForGenerateLogicObstacleCallback* pCallback);
private:
	KG3DSceneEntityList::iterator m_itRenderEntity;
public:
	KG3DPhysiscScene* m_lpPhysicsScene;
    KG3DSceneOutputWnd* m_pCurRenderWnd;

	//LPDIRECT3DCUBETEXTURE9 m_lpEvnMap;//<EnvMap Mov>
	//std::string            m_szEvnMapName;

	UINT m_uOption;

	int m_nLogicalSceneX_Start;//以米为单位的游戏逻辑场景在场景中的位置
	int m_nLogicalSceneZ_Start;
	int m_nLogicalSceneX_Width;
	int m_nLogicalSceneZ_Width;
	
	int m_nLogicalRegionX_Start;
	int m_nLogicalRegionZ_Start;
	int m_nCountLogicalRegionX;
	int m_nCountLogicalRegionZ;
public:
	KG3DSceneEntityMgr m_EntityMgr;
	KG3DSceneOutDoorSpaceMgr* m_lpSceneOutDoorSpaceMgr;

    KG3DScenePointLightRender* GetPointLightRender() { return m_EntityMgr.m_lpPointLightRender; }

    /* in editor, edit node will point to the locked node
       in client, edit node ever point to the root node  
       */
    KG3DSceneSpaceNode* m_pEditSpaceNode;

    KG3DSceneSpaceNode* GetEditSpaceNode() { return m_pEditSpaceNode; }

	KG3DSceneOutDoorSpaceMgr* GetOutDoorSpaceMgr(){return m_lpSceneOutDoorSpaceMgr;}
	//////////////////////////////////////////////////////////////////////////
	//场景空间分割节点漫游
	HRESULT ProcessSpaceNodeByPosition(UINT& uCurrentNode,UINT nStartNode,D3DXVECTOR3& vPosition);
	HRESULT ProcessSpaceNodeByCamera(KG3DCamera* pCamera);
    HRESULT CalcFrustum(KG3DCamera* pCamera);
	UINT m_uCurrentSpaceNodeHandle;
	map<UINT,KG3DSceneSpaceNode*>m_mapSceneSpaceNodeHandle;
	KG3DSceneSpaceNode* GetSpaceNodeByHandle(UINT hHandle);
	HRESULT RegisterSpaceNode(KG3DSceneSpaceNode* pNode);
	HRESULT UnRegisterSpaceNode(KG3DSceneSpaceNode* pNode);
	UINT GetOneNewSpaceNodeHandle();
	KG3DSceneSpaceNode* m_lpCurrentSpaceNode;//当前所处的空间节点

	//KG3DWaterEffect* m_lpWaterEffect; //this object is very aspect-specific, and it is more proper to move it to KG3DSceneSceneEditor

public:
	KG3DScene(void);
	virtual ~KG3DScene(void);

public:
	DWORD m_dwType;
	DWORD m_dwID;
	DWORD m_dwRecordID;

	KG3DSceneEntityList m_listRenderEntity;

    PostRenderParams m_EffectParams;

	//DWORD m_dwAmbient;

public:
	ULONG   m_ulRefCount;
	std::string m_scName;

public:
	virtual HRESULT AutoReleadModifiedResource();
	HRESULT SetPhysiscScene(KG3DPhysiscScene* pPhyScene)
	{
		m_lpPhysicsScene = pPhyScene;
		return S_OK;
	}
	KG3DPhysiscScene* GetPhysiscScene()
	{
		return m_lpPhysicsScene;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef( void);
	virtual ULONG STDMETHODCALLTYPE Release(void);
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		REFIID riid, void **ppvObject
		);

	virtual HRESULT Init();
	virtual HRESULT UnInit();

	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
	virtual HRESULT SaveToFile(const char cszFileName[],   unsigned uOption);

	virtual HRESULT OnSetFocus();	//// Last modify by Chen Tianhong: 2007-7-23 11:20:03///起码要在GraphicsTool中SetCurScene

	virtual DWORD GetID();
	virtual unsigned GetType();
	void SetID(DWORD dwID) { m_dwID = dwID;}
	void SetType(DWORD dwType){m_dwType = dwType;}

	virtual HRESULT PrepareRender(unsigned uOption);
	virtual HRESULT PostRender();
	virtual HRESULT GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem);//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
	virtual unsigned   GetLastUseTime(); 
	virtual unsigned   GetUseFrequency();

	virtual HRESULT FrameMove();
	virtual HRESULT Render(unsigned uOption);		//渲染
	virtual HRESULT RenderToScreen(D3DXVECTOR2 const* pA, D3DXVECTOR2 const* pC, D3DXVECTOR2 const* pUVA, D3DXVECTOR2 const* pUVC, float fZ, DWORD dwFlags);

	virtual HRESULT AddOutputWindow(LPCTSTR szName, HWND hWnd, DWORD dwType, DWORD_PTR dwOption, int* pOutputWindowID);
	virtual HRESULT RemoveOutputWindow(int nOutputWindowID);
	virtual HRESULT EnablePostRenderEffect(int nOutputWindowID, BOOL bEnable);
	virtual HRESULT EnablePostRenderEffect(int nOutputWindowID, PostRenderEffectType nType, BOOL bEnable);
	virtual HRESULT AddPostRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable = TRUE);
	
	//<Add Remove>//Add Remove一共有三种：Model(IKG3DResourceBase*),Entity,Object
public:	
	virtual HRESULT AddRenderEntity(IKG3DResourceBase *pEntity,    unsigned uClass, BOOL bSelectable,BOOL bLight);
	virtual HRESULT RemoveRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable = TRUE);
	virtual HRESULT RemoveSelectableModel(IKG3DResourceBase *pEntity);

	//STDMETHOD(AddRepresentObject)(IEKG3DRepresentObject* pObject, BOOL bSelectable, BOOL bSeleted){return E_NOTIMPL;}	
	STDMETHOD(AddRepresentObject)(IEKG3DRepresentObject* pObject, ULONG uOption, IEKG3DRepresentObject** ppCreatedObject){return E_NOTIMPL;}
	STDMETHOD(RemoveRepresentObject)(IEKG3DRepresentObject* pObject){return E_NOTIMPL;}	

	virtual HRESULT AddRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer);
	virtual HRESULT RemoveRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer);


	//在内部使用这些函数
	virtual HRESULT AddRenderEntity(KG3DSceneEntity Entity, BOOL bIsSelectable = TRUE, BOOL bIsSelected = FALSE);
	virtual HRESULT DelRenderEntity(KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList = TRUE);//占住这个名字，省的和DelXX的意义不一致
	virtual HRESULT RemoveRenderEntity(KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList = TRUE);//是否在SeletedList中也删除Entity

	virtual ULONG	GetObjectCount(DWORD dwTypeFilter);//dwTypeFilter是REPRESENTOBJECT_NONE的时候，计算所有物件，否则只计算对应类型的物件
	//</Add Remove>
	

	virtual HRESULT ProcessMessage( UINT message, WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>

	virtual IKG3DCamera* GetCurrentCamera(); 
	virtual HRESULT      SetCurrentCamera(IKG3DCamera* pCamera); 

	virtual HRESULT LoadScene(LPSTR pFileName,DWORD Option);

	virtual HRESULT GetFloor(/*D3DXVECTOR3* pVecOut, const D3DXVECTOR3* pVecIn*/const D3DXVECTOR3* pvInput, FLOAT* pfRet);//输入3D坐标，向下发射的射线，查此点场景的高度
	virtual DWORD   GetGroundType(const D3DXVECTOR3 &vec3Pos);//返回地表类型

	virtual HRESULT GetWaterHeight(D3DXVECTOR3* pOut, D3DXVECTOR3* pIn);
	virtual HRESULT GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR3* pIn);
	virtual HRESULT GetForce (D3DXVECTOR4* pOut,D3DXVECTOR3* pIn);
	virtual float   GetHeight(float x, float z) {return 0;}


	virtual HRESULT LogicalToScene(D3DXVECTOR3* pPos, float fX, float fY, float fZ, BOOL bOnFloor);
	virtual HRESULT SceneToLogical(int* pX, int* pY, int* pZ, D3DXVECTOR3 Pos);
	virtual HRESULT ScenePosToScreenPos(D3DXVECTOR3 const& vScenePos, float& fScreenX, float& fScreenY);

	HRESULT RenderToTexture(DWORD dwTextureID, D3DXVECTOR2 const* pA, D3DXVECTOR2 const* pC, D3DXVECTOR2 const* pUVA,
		D3DXVECTOR2 const* pUVC, float fZ, DWORD dwFlags)
	{
		return S_OK;
	}

	virtual HRESULT OnResizeWindow(PRECT const pRect);

	//virtual HRESULT GetStandHeight(DWORD& dwCount,D3DXVECTOR3* pInter,D3DXVECTOR3& vIn,DWORD dwMaxCount);

	//////////////////////////////////////////////////////////////////////////

	virtual HRESULT GetMinimapLayer(IN const D3DVECTOR &vPos, OUT int &nLayer);
	virtual HRESULT SetFocus(D3DXVECTOR3& vecPos);
	virtual HRESULT GetFocus(D3DXVECTOR3* pvecPos);

	//protected:
public:
	virtual HRESULT OnReferenceZero();

	virtual HRESULT RenderForEachWindow(KG3DSceneOutputWnd& wndCur);

	//virtual HRESULT GetSelectModelListForClient(const D3DXVECTOR2 &fScale,std::vector<IKG3DModel*> &pList){return E_NOTIMPL;}
	//virtual HRESULT GetSelectModelList(const D3DXVECTOR2 &fScale,std::vector<IKG3DModel*> &pList){return E_NOTIMPL;}
	virtual HRESULT GetSelectModelList(const D3DXVECTOR2& vPosScreen, std::vector<IKG3DModel*>& pList, BOOL bGetNearestOrAll){return E_NOTIMPL;}
	//virtual HRESULT GetSelectModelList(const D3DXVECTOR2 &fScaleLeftUp,const D3DXVECTOR2 &fScale1RightBottom,
	//	std::vector<IKG3DModel*> &pList){return E_NOTIMPL;}
	virtual HRESULT GetSelectPosition(D3DXVECTOR2 const& vScale, D3DXVECTOR3& vPosition);

    virtual HRESULT GetTerrainCross(D3DXVECTOR3* vInter, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDst) { return E_NOTIMPL; }

protected:
	KG_CUSTOM_HELPERS::TCommonParamBuffer<KG3DSceneCameraMovement, 36> m_pCameraMovement;	//摄像机动画
	KG3DRegionInfoManager*	m_lpRegionDWInfoManager;	//天气对应的地形信息
	
	KG3DGPS_Dummy	m_GameGPS; //为了得到摄像机位置而做的
private:
	//把天气换成CommonBuffer，这样就不存在new了，要保证其恒定存在，全局设置Light和Fog需要其恒定存在
	KG_CUSTOM_HELPERS::TCommonParamBuffer<KG3DEnvironment, 1026> m_lpEnvironment;	//这里不要嫌大，用个指针，然后new出来本质是一样的	
public:
	KG3DEnvironment& GetEnvironment();
public:
	virtual HRESULT RenderWindowBegin(IEKG3DSceneOutputWnd* pWnd,BOOL bSetViewPort);
	virtual HRESULT RenderWindowEnd(IEKG3DSceneOutputWnd* pWnd);
	virtual HRESULT GetCameraMovement(IEKG3DSceneCameraMovement** ppiCameraMovement);
	virtual HRESULT GetCameraMovement(IKG3DSceneCameraMovement** ppiCameraMovement);
	virtual HRESULT GetEnvironment(IEKG3DEnvironment** ppiEnvironment);

    virtual HANDLE  AddSceneSigns(const D3DXVECTOR3& vPos) { return 0; }
    virtual void    DelSceneSings(HANDLE Handle)           {}

	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WOption, DWORD_PTR LOption);

	KG3DRegionInfoManager* GetRegionDWInfoManager(){return m_lpRegionDWInfoManager;}
	KG3DGPSBase*	GetCurGameGPS(){return &m_GameGPS;}
	virtual HRESULT GetEngineIEInterface(DWORD dwType,  DWORD_PTR wParam, DWORD_PTR lParam, VOID** ppiInterface);
	virtual HRESULT GetEngineIEAbstructInterface(DWORD dwAbstructType, DWORD dwType,  DWORD_PTR wParam, DWORD_PTR lParam, VOID** ppiInterface);

	//</CTH>
protected:
	KG3DSceneWndManager* m_SceneWndManager;
public:
	//<窗口管理>
	virtual HRESULT GetOutputWindow(int nID, IEKG3DSceneOutputWnd **ppiRetSceneOutputWnd);
	virtual HRESULT SetCurrentOutputWindow(int nID)
	{
		return m_SceneWndManager->SetCurrentOutputWindow(nID);
	}
	HRESULT GetPrimaryWindow(IEKG3DSceneOutputWnd **ppiRetPrimaryWindow)
	{
		return m_SceneWndManager->GetPrimaryWindow(ppiRetPrimaryWindow);
	}
	virtual HRESULT GetCurOutputWnd(IEKG3DSceneOutputWnd **ppiRetCurOutputWnd)
	{
		return m_SceneWndManager->GetCurOutputWnd(ppiRetCurOutputWnd);
	}
    virtual HRESULT GetCurRenderWnd(IEKG3DSceneOutputWnd **ppiRetCurOutputWnd);

	virtual HRESULT GetSceneEditorBase(IEKG3DSceneEditorBase** pBase);//by dengzhihui Refactor 2006年12月11日

	KG3DSceneOutputWnd* GetPrimaryWindow();	//得到主输出窗口
	//</窗口管理>
	virtual float GetLoadingProcess()
	{
		return 1.0F;
	}
	virtual HRESULT SetSceneSettingData(DWORD dwEnum, DWORD_PTR dwData);
	virtual HRESULT GetSceneSettingData(DWORD dwEnum, DWORD_PTR* pdwData);

	virtual HRESULT GetLogicalSceneSize(int* pXStart,int* pZStart,int* pWidthX,int* pWidthZ);
	virtual HRESULT SetLogicalSceneSize(int  nXStart,int  nZStart,int  nWidthX,int  nWidthZ);

	virtual HRESULT GetName(LPTSTR pNameBuffer, size_t uBufferSize);
	
	virtual HRESULT GetAppointLogicalRegion(int& nRegionXStart,int& nRegionZStart,int& nCountRegionX,int& nCountRegionZ);
	virtual HRESULT SetAppointLogicalRegion(int nRegionXStart,int nRegionZStart,int nCountRegionX,int nCountRegionZ);
//<Container>
public:
	static 	KG3DCommonObjectBuilder& GetCommonObjectBuilder();//IOC 注入机制
	KG3DCommonObjectNode&	GetCommonObjectNode();
	
private:
	class KG3DCommonObjectNodeScene : public KG3DCommonObjectNodeSimple 
	{
	public:
		KG3DCommonObjectNodeScene(KG3DCommonObjectBuilder& Builder);//里面创建出加速表
		STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_COMMON_OBJECT_NODE;}
		STDMETHOD_(KG3DCommonObjectBuilder*, GetCommonObjectBuilder)();
		STDMETHOD_(KG3DCommonObjectPool&, GetCommonObjectPool)(){return m_Pool;}
	private:
		KG3DCommonObjectBuilder& m_Builder;
		KG3DCommonObjectPool	m_Pool;
	};

	KG3DCommonObjectNodeScene	m_CommonObjNode;

	D3DXVECTOR3 m_vec3FocusPos;
//</Container>

	//<Member>
public:
	virtual KG3DTransformTool& GetTransformTool(DWORD dwType);
	KG3DSceneObjectPlacementTool*	GetObjectPlacementTool();	
	virtual KG3DSceneSelectionTool&	GetSelectionTool();
	
	//</Member>

	//<特殊子元件>
private:
	//下面这些，注册了之后就可以在对应的事件发生的时候得到消息，那么SSE本身就不用在事件发生的时候主动调用那些类，从而就不用知道那些类的实现，减少了依赖
	KG3DMessageBroadcasterGroup* m_pBroadcaster;
public:
	virtual KG3DMessageBroadcasterGroup&	GetBroadcasterGroup();
	virtual KG3DMessageSender&				GetMessageSender(KM_MESSEGE emType);
	//事件侦听
	virtual HRESULT RegisterListener(DWORD ListenerType, IKG3DMessageReceiverBase* pReceiver);
	virtual HRESULT UnRegisterListener(DWORD ListenerType, IKG3DMessageReceiverBase* pReceiver);

	void BoardCastAction( INT nActionID, INT nExecState, WPARAM wParam, LPARAM lParam );
	//</特殊子元件>
//protected:
//	list<KG3DModel*> m_listKeepModel;  // 用于记录那些从特效标效上加载入来生命周期由特效自己管理的特效型。
//public:
//	HRESULT PushModel(KG3DModel* pModel);
//	HRESULT EraseModel(KG3DModel* pModel);
//	HRESULT ClearListKeepModel();
};

