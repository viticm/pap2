#pragma once
#include "../Scene/KG3DSceneEditorBase.h"
#include "../Scene/kg3dsceneselectbase.h"
//#include "./KG3DSceneLogicalEditorBase.h"
#include "./Scene/KG3DSnapTool.h"
#include "IEEditor.h" //by dengzhihui Refactor 2006年11月16日
#include "KG3DRepresentNPCScreenRunner.h"
#include "KG3DMeasureTool.h"
#include "KG3DBsp.h"

#include "KG3DIntersectionBase.h" // add by suntao

class KG3DRepresentPoly;
class KG3DSceneSettings;
class IEKG3DCommonObject;
class KG3DSceneObjectPlacementTool;
//class KG3DLODAdjuster;
struct KG3DTerrainConverMap;

namespace GraphicsStruct
{
	//struct KG3DEnvEffLight;
	struct BBox;
}

class KG3DRepresentNPC;
class KG3DSceneEntityGroup;
class KG3DRepresentObjectVisibleEntitySet;
class KG3DMeasureTool;
interface IEKG3DTerrain;
struct KG3DSceneFloorData;
class KG3DSceneShadowMap;

class KG3DTerrainRoad; 
class KG3DWaterEffect;
class KG3DRepresentObjectNode;
//<SCENE_SETTING>
class KG3DSceneSettings;
class KG3DWaterEffect;
//</SCENE_SETTING>
 

class KG3DSceneSceneEditor :
	public KG3DSceneEditorBase , 
	public IEKG3DSceneSceneEditor
{
public:
	friend class KG3DEngineManager;

	DWORD m_dwShadowmapSize;
	BOOL m_bUseFFTW;//使用快速傅立叶变换的水面
		
	KG3DSceneOutputWnd* m_lpMainWindow;


	KG3DSnapTool m_vSnapTool;

	BOOL       m_bMouseTargetMoving;//
	D3DXVECTOR3 m_vMouseTargetPosition;//移动摄像机
	list<KG3DMeasureTool*>m_listMeasureTool;
        
	struct Lines
	{
		struct Lines() : dwColor(0), nStart(-1) {}

		DWORD dwColor;
		int nStart;		// nStart == -1表示首位相接
		int nSetID;  //表示属于哪个集合 
		vector<D3DXVECTOR3>vecPosition;
		string szName;
	};
	vector<Lines>m_vecHelpLine;

	virtual HRESULT SetHelpLineSize(size_t nLine, size_t nSize);
	virtual HRESULT SetHelpLineStart(size_t nLine, size_t nStart);
	virtual HRESULT SetHelpLine(size_t nLine,int nSetID, size_t nBegin, size_t nCount, D3DCOLOR color, D3DXVECTOR3* pPos);
	virtual HRESULT GetHelpLine(size_t nLine, size_t* pPoint, D3DXVECTOR3** ppPos);
	virtual DWORD GetHelpLineSize();
	virtual HRESULT ClearAllHelpLine();
	virtual HRESULT ClearSetHelpLine(int dwSetID);
	virtual HRESULT SetHelpLineName(size_t nLine,LPSTR pName);

	HRESULT RenderHelpLine();

    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice();

    INT IsPMode(){return !m_bEditing;};

    virtual HRESULT LockSelSpaceNode();
    virtual HRESULT UnLockSpaceNode();

public:

    virtual void EnableShowSelectedBsp(BOOL bShow);
    KG3DBspRender m_BspRender;

	//HRESULT ProcessShadowMap(KG3DCamera* pCamera,D3DXVECTOR4& vLight);

	HRESULT CheckTerrianSelection();//整理地形选择框,使其对齐格子或者REGION

	virtual IKG3DScene* GetIInterface();//得到原始的I接口
	virtual KG3DSceneSceneEditor* GetSceneSceneEditor();

	virtual HRESULT MoveScene(int nXStart,int nZStart);	//把Scene移动到某个位置，要重新设置所有东东的位置

	HRESULT ComputeRegionSelectionRect();

	virtual HRESULT GetIELogicScene(IEKG3DSceneLogicalEditorBase** pLogicScene);

    virtual HRESULT Advise(IEKG3DSimpleSink* pNewSink) //增加一个观察者
    {
        return CKG3DConnectionPoint<IEKG3DSceneSelectBase>::Advise(pNewSink);
    }
    virtual HRESULT Unadvise(IEKG3DSimpleSink* const pNewSink)   //删除一个观察者
    {
        return CKG3DConnectionPoint<IEKG3DSceneSelectBase>::Unadvise(pNewSink);
    }

	struct LayerSaveInfo
	{
		BOOL bUsed;
		BOOL bVisible;
		BOOL bSelecteable;
		DWORD dwID;
		DWORD dwParentID;
		char  Name[MAX_PATH];
		DWORD dwExtend[8];
	};

	struct ObjectGroup
	{
		list<KG3DRepresentObject*>listObject;
	};
	map<std::string,ObjectGroup>m_mapObjectGroup;

	KG3DModel*        m_pTestModel;
	
    KG3DSFXSuede*     m_pSFXSuede;
	KG3DRepresentNpcScreenRunner* m_pRepresentNPC;
	list<KG3DRepresentNpcScreenRunner*>m_listRepresentNPC;

	D3DXVECTOR3 vCamera;
	float m_fZFar;
	float m_fFovy;

	D3DXVECTOR3 m_vInter[256];
	DWORD      m_dwInterCount;

	float m_fSceneRunnerSpeed;

	//////////////////////////////////////////////////////////////////////////
	// camera localize
	D3DXVECTOR3 m_vCameraPos;
	D3DXVECTOR3 m_vCameraLookAtPos;
	D3DXVECTOR3 m_vCameraUpDirection;

	KG3DSceneEntityMgr m_EntityMgrTopView;
	
	KG3DRepresentObjectVisibleEntitySet* m_pCurrentVisibleSet;
	map<int,KG3DTexture*>m_mapLightMapTextureEntity;//entity的lightmap贴图管理器

	int m_nCurrentLightMapIndex;//当前的lightmap最大数值

	DWORD m_dwPreLoadingFrame;//调用预加载的次数

	BOOL  m_bUnderWater;//当前摄象机是否在水面之下
	float m_fWaterHeigh;
public:
	HRESULT LoadRepresentNpcList();

	HRESULT LoadOneMap(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);

    virtual BOOL InUnderWater() const { return m_bUnderWater; }

    virtual IEKG3DWaterEffect* GetWaterEffect();
	//<Select>
	//virtual HRESULT SelectBegin(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,D3DXVECTOR2 Origin2D, DWORD dwOperation);	
	//virtual HRESULT SelectBegin(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,const D3DXVECTOR2 Origin2DLeftUp, const D3DXVECTOR2 Origin2DRightBottom, DWORD dwOperation);
	//virtual HRESULT SelectEnd(D3DVIEWPORT9& Viewport,D3DXMATRIX& Projection,D3DXMATRIX& View);

	virtual HRESULT AddSelectedEntityToAIGroup(DWORD dwSetID);
	virtual HRESULT RemoveSelectedEntityFromAIGroup(DWORD dwSetID);

	virtual HRESULT ClearSelectedEntityList();

	virtual HRESULT AddSelectedEntityToDoodadRefreshSet(DWORD dwSetID);
	virtual HRESULT AddSelectedEntityToNPCRefreshSet(DWORD dwSetID);
	virtual HRESULT RemoveSelectedEntityToDoodadRefreshSet(DWORD dwSetID);
	virtual HRESULT RemoveSelectedEntityToNPCRefreshSet(DWORD dwSetID);
	IEKG3DRepresentObject* GetFirstSelectedObject();
	HRESULT GetFirstSelectedObject(IEKG3DRepresentObject** ppObj);
	HRESULT DrapSelectedEntity();
    virtual void EnablePvsCulling(BOOL bEnable) { KG3DRepresentObjectPVS::m_sCulling = bEnable; }
    BOOL IsSelectedPvs();
	HRESULT UnitSelectEntity();
	HRESULT UnUnitSelectEntity();
	HRESULT GetFirstSelectedModel(IEKG3DModel*& pModel);
	virtual HRESULT DeleteSelectedEntity();
	HRESULT			GetSelecteNPC(IEKG3DModel*& pModel);
	//<SelectModelList Refactor>
	//virtual HRESULT GetSelectModelListForClient(const D3DXVECTOR2 &fScale,std::vector<IKG3DModel*> &pList);
	//virtual HRESULT GetSelectModelList(const D3DXVECTOR2 &fScale,std::vector<IKG3DModel*> &pList);
	//virtual HRESULT GetSelectModelList();
	virtual HRESULT GetSelectModelList(const D3DXVECTOR2& vPosScreen, std::vector<IKG3DModel*>& pList, BOOL bGetNearestOrAll);
	//virtual HRESULT GetSelectModelList(const D3DXVECTOR2 &fScaleLeftUp,const D3DXVECTOR2 &fScale1RightBottom,
	//	std::vector<IKG3DModel*> &pList);//</SelectModelList Refactor>	//全部不实现,除了要用的那个
	virtual HRESULT GetSelectPosition(D3DXVECTOR2 const& vScale, D3DXVECTOR3& vPosition);
	virtual HRESULT ReplaceSelectedObjectWithInHand();//把选择的物件替换成当前Handle的物件
	HRESULT GetSelectedObjectInHand();//将选择的物件放置到手上

	//virtual HRESULT GetSelectedEntityCount(INT* nCount);
	//virtual HRESULT GetSelectedEntity(INT nIndex, IEKG3DSceneEntity** ppEntity);
	virtual HRESULT GetSceneSelector(IEKG3DSceneSelectBase** ppSelector);
	HRESULT AddSelectedEntityByIndex(INT Index);

	DWORD m_dwSceneSelectionState;
	D3DXVECTOR3 m_vPositionSceneSelectionA;
	D3DXVECTOR3 m_vPositionSceneSelectionC;
	//</Select>

	//<Edit>
	virtual HRESULT PasteSelection();
	//</Edit>

	virtual HRESULT SetNPCPosition(BOOL UseRayIntersectionPosOrInputPos, BOOL bTopViewSet, const D3DXVECTOR3* pInputPos);

	HRESULT AddOutputWindow(LPCTSTR szName, HWND hWnd, DWORD dwType, DWORD_PTR dwOption, int* pOutputWindowID);

    virtual HRESULT SetSnapToolScale(FLOAT fScale);
    virtual HRESULT RenderSceneSnap(LPSTR pFileName);
	virtual HRESULT ExprotTerrainTexture(LPSTR pFileName);

	HRESULT RenderMeasureTools(KG3DCamera* pCamera,D3DVIEWPORT9& viewport);
	HRESULT AddOneMeasureTool();
	HRESULT DeleteMeasureTool(KG3DMeasureTool* pMeasureTool);
	HRESULT DeleteAllMeasureTool();

	HRESULT ApplyVisibleSetChange(KG3DRepresentObjectVisibleEntitySet* pSet,BOOL bIn);

	HRESULT AddVisibleSet(D3DXVECTOR3& Position);

	//HRESULT GetStandHeight(DWORD& dwCount,D3DXVECTOR3* pInter,D3DXVECTOR3& vIn,DWORD dwMaxCount);	//得到站立高度
	

	//virtual HRESULT GroupSelectEntity();//将选择的物体打包
	//virtual HRESULT UnGroupSelectEntity();//取消打包

	virtual HRESULT GroupSameNameAndTypeObjects();
	

    void GetWaterClipPane(D3DXPLANE* vFFP, D3DXPLANE* vPP, float fWaterHeight, bool bUnder);
	virtual HRESULT OnRenderReflectTexture(KG3DCamera* pCamera);
	HRESULT OnRenderConverMapTexture(KG3DCamera *pCamera,LPDIRECT3DTEXTURE9 pTLight,KG3DTerrainConverMap* pConvermap);

	HRESULT RenderConverMapTexture(KG3DSceneOutputWnd* pWnd);

    virtual HRESULT RenderRefractTexture(KG3DSceneOutputWnd* pWnd);
    virtual HRESULT RenderReflectTexture(KG3DSceneOutputWnd* pWnd,BOOL bNeedBeginScene);
	virtual HRESULT RenderWaterWaveTexture(KG3DSceneOutputWnd* pWnd);
	virtual HRESULT OnRenderWaterWaveTexture(KG3DCamera *pCamera);

	virtual HRESULT ImportOldFile(LPSTR pFileName);

	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
	virtual HRESULT SaveToFile(const char cszFileName[], unsigned uOption);//注意uOption是有用的，用来指定保存的内容

    virtual HRESULT GetFloor(/*D3DXVECTOR3* pVecOut, const D3DXVECTOR3* pVecIn*/const D3DXVECTOR3* pvInput, FLOAT* pfRet);//输入3D坐标，向下发射的射线，查此点场景的高度
	virtual DWORD   GetGroundType(const D3DXVECTOR3 &vec3Pos);
	const TerrainRepresentInfo *GetGroundRepresentInfo(const D3DXVECTOR3 &vec3Pos);
	virtual HRESULT GetWaterHeight(D3DXVECTOR3* pOut, D3DXVECTOR3* pIn);
	virtual HRESULT GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR3* pIn);
    virtual float   GetHeight(float x, float z);

	virtual HRESULT LogicalToScene(D3DXVECTOR3* pPos, float fX, float fY, float fZ, BOOL bOnFloor);
	virtual HRESULT SceneToLogical(int* pX, int* pY, int* pZ, D3DXVECTOR3 Pos);

	virtual HRESULT RenderToScreen(D3DXVECTOR2 const* pA, D3DXVECTOR2 const* pC, D3DXVECTOR2 const* pUVA, D3DXVECTOR2 const* pUVC, float fZ, DWORD dwFlags);

	virtual LPDIRECT3DCUBETEXTURE9 GetEnvMap(LPTSTR pNameBuffer, size_t uBufferCount);
	virtual HRESULT SetEnvMap(LPCTSTR szPath);
	/*virtual HRESULT SetToEditorTerrainState()
	{
		m_dwEditState = SCENESTATE_TERRAIN;
		return S_OK;
	}*/
	/*INT GetSizeOflistEntityRenderLast()
	{
		return (INT)m_listEntityRenderLast.size();
	}*/
	INT GetSizeOflistRenderEntity()
	{
		return (INT)m_listRenderEntity.size();
	}


    HRESULT RemoveEntityFromRenderList();

    virtual HRESULT ShockwaveEntity(); 
	//HRESULT PrepareLightmapForModel(KG3DModel* pModel,int nLightmapTextureIndex);

	D3DTEXTUREFILTERTYPE m_Sample_Mip;
	D3DTEXTUREFILTERTYPE m_Sample_Mag;
	D3DTEXTUREFILTERTYPE m_Sample_Min;

	float					m_fMipmapLod;

	D3DXVECTOR3 m_vCameraMainPos;//主摄象机的位置
	D3DXVECTOR3 m_vCameraMainFront;//主摄象机的方向
	D3DXVECTOR3 m_vCameraMainLeft;//主摄象机的方向
	D3DXVECTOR4 m_vCameraMainPers; 

	D3DXVECTOR3 m_vCameraTopPos;
	float m_fCameraTopScale;


public :
    virtual HRESULT ShockWaveRender();
protected:
   
	HRESULT ProcessForCameraAnimation(KG3DSceneOutputWnd *pWnd);
	HRESULT ProcessPostRender(KG3DSceneOutputWnd *pWnd);
	HRESULT ProcessCameraForMainWindows(KG3DSceneOutputWnd* pWnd);
	HRESULT ProcessMovieRecForMainWindows(KG3DSceneOutputWnd* pWnd);
	

	HRESULT UnUnitObject(KG3DRepresentObject* pObject,KG3DSceneEntityList* plistTemp);

	HRESULT SaveObjectToFile(FILE* pFile,KG3DRepresentObject* pObject);

	//list<KG3DSceneEntityGroup*>m_listEntityGroup;//场景所使用的群组

	KG3DSceneEntityList m_listEntityRenderLast;//不在地形区域里的实体

	//KG3DSceneEntityList m_listEntityNotInQTree;//不在四插树里的实体

	//KG3DSceneEntityList m_listEntityNotInRegion;//不在地形区域里的实体

	HRESULT OnBrushCellLogical();

	//virtual HRESULT ScaleBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction);
	//virtual HRESULT ScaleEnd();

	//virtual HRESULT RotateBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction);
	//virtual HRESULT RotateEnd();

	//virtual HRESULT MoveBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction);//<Move Refactor>
	//virtual HRESULT MoveEnd();//2009-2-4 代码移动到OnReceiveMessage


    virtual void ZoomCoordinate(float fZoom);
	void MsgOnLButtonDown(WPARAM wParam, LPARAM lParam);
	void MsgOnKeyDown(WPARAM wParam, LPARAM lParam);
	void MsgOnMouseMove(WPARAM wParam, LPARAM lParam);
	void MsgOnLButtonUp(WPARAM wParam, LPARAM lParam);
	void MsgOnMouseWheel(WPARAM wParam, LPARAM lParam);
    void MsgOnRButtonDown(WPARAM wParam, LPARAM lParam);
    void MsgOnRButtonUp(WPARAM wParam, LPARAM lParam);

	virtual HRESULT Init();
	virtual HRESULT UnInit();

	virtual HRESULT RenderForEachWindow(KG3DSceneOutputWnd& wndCur);

	HRESULT RenderTopView_New(KG3DSceneOutputWnd* pWnd);
    HRESULT RenderTopView(KG3DSceneOutputWnd* pWnd);

	HRESULT RenderMainWindow(KG3DSceneOutputWnd& wndCur,const D3DXMATRIX* matProj);
	HRESULT RenderObjectPreviewWindows(KG3DSceneOutputWnd* pWnd);
	HRESULT RenderScenePreviewWindows(KG3DSceneOutputWnd *pWnd);
	HRESULT RenderTextureWindows(KG3DSceneOutputWnd *pWnd);///<CTH>///渲染单张贴图的OutputWnd

	HRESULT GetObjectEffectLight( BBox &ObjectAABB,std::vector<BYTE> &EffectLightID);

    HRESULT SaveMapSetting(LPSTR pFileName);
    HRESULT LoadMapSetting(LPSTR pFileName);

	HRESULT FrameMove();

	void	FrameMoveCamera();
    void	SetCoordMode(DWORD dwMode);

	virtual HRESULT IELoadPoly(LPCTSTR pScriptFileName,LPCTSTR pPolyName,DWORD dwColor,LPCTSTR szLogicalSetName,int nLogicalSetIndex,DWORD dwModifyState,float fVertexHeight,int nNumVertex,D3DXVECTOR3 vPos[]);

	HRESULT EndAddPolyVertex(DWORD dwType,KG3DRepresentPoly**  ppPolyOut);

	//road
public:
	virtual void SetRenderNode(BOOL bRender);
	virtual BOOL GetRenderNode();
	virtual HRESULT AddNodeToScene(KG3DRepresentObjectNode** ppNode,D3DXVECTOR3* pvTrans = NULL,D3DXVECTOR3* pvScale=NULL);
	virtual HRESULT AddNodeToScene(DWORD dwNodeType);
	virtual HRESULT DeleteNodeFromScene(DWORD dwNodeType);
    
    HRESULT GetVisibleModelFromOutDoor(
        std::set<KG3DModel *> &rsetModels,
        const std::vector<KG3DFrustum> &crvecFrustum
    );
private:
    HRESULT _FrameMoveRenderEntifyLast();

    HRESULT PreprocessWaterScene(KG3DSceneOutputWnd& Wnd,BOOL bNeedBeginScene);

	HRESULT RenderClientWindow(KG3DSceneOutputWnd& Wnd, DWORD dwFlags);
	HRESULT RenderClientWindowBegin(KG3DSceneOutputWnd& Wnd, DWORD dwFlags);
	HRESULT RenderClientWindowEnd(KG3DSceneOutputWnd& Wnd);

	HRESULT PrepareRenderTopWindow(KG3DSceneOutputWnd *pWnd);
	HRESULT PrepareRenderMainWindow(KG3DSceneOutputWnd *pWnd);

	DWORD	m_dwClientWindowFlags;
	DWORD	m_CamareState;
public :
	enum enumCarameMoveState{ 
		cmsNone = 0,
		cmsFoward = 1,
		cmsBack = 2, 
		cmsTurnLeft = 4, 
		//cmsTurnBodyLeft = 8, //原地转，但镜头不转
		//cmsTurnRight = 16, 
		//cmsTurnBodyRight = 32, //原地转，但镜头不转
		cmsMoveLeft = 64,
		cmsMoveRight = 128,
		cmsCamareUp = 256,
		cmsSpeedUp = 512,
		cmsSpeedDown = 1024,
		cmsCamareDown = 2048,
	};

	inline void SetCamareState(WORD State, BOOL bSet);
public:
	KG3DSceneSceneEditor(void);
	virtual ~KG3DSceneSceneEditor(void);
private:
	void InvertCurrentOperation(INT nInvert);
	//<Select>
	INT ExecActionSelect(INT nActionID, INT nExecState, INT nXOff, INT nYOff);
	//</Select>
	BOOL m_bTerrainPainting;
	DWORD m_dwPreState;

    vector<KG3DModel*>   m_vecSFXSuede;

    // ------------- movie rec --------------
    LPDIRECT3DSURFACE9   m_lpMovieSurface;
    BOOL                 m_bMovieREC;
    std::string          m_strMovieSavePath;
    INT                  m_nSaveCount;
    INT                  m_nAutoScanl;
    D3DTEXTUREFILTERTYPE m_FiterType;
    D3DXIMAGE_FILEFORMAT m_ImageType;
    SIZE                 m_MovieSize;

	BOOL m_bEnableWireFrameMode;
public:
	virtual void SetUseBoneBox(BOOL val);
	virtual BOOL GetUseBoneBox();
	vector<KG3DModel*>&	GetVecSFXSuede(){return m_vecSFXSuede;}
	VOID ForceFireEvent2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);

public:
	//(by dengzhihui 2006年9月13日 15:38:32 //增加快捷键的特殊处理
	INT ExecAction(INT nActionID, INT nExecState, WPARAM wParam, LPARAM lParam);
	void TrackCamareFrameMove();
	BOOL GetTerrainPainting(){return m_bTerrainPainting;}
	virtual void SyncPickRay();
	virtual void OnCommand(enumCommandType eCommandType);
	
	//)


	//<Add Remove>
	/************************************************************************/
	/*
	Add和Remove分别有三个版本，Model的(IKG3DResourceBase *)，RepresentObject的.
	其中RepresentObject的版本由Scene来管理其生命周期。Selectable表现为一个属性
	不用再AddSelectableEntity。直接用后面两个参数就可以了。
	如果非要显式设Seletable（如Freeze一个物件的选择），可以用SetSelectable
	*/
	/************************************************************************/	
	
    STDMETHOD(AddRepresentObject)(IEKG3DRepresentObject* pObject, ULONG uOption, IEKG3DRepresentObject** ppObjectCreated);	//如果ToFile，那么Object会在OutDoor里面复制出一份，生命周期和传入的指针没有关系，外部的指针也应该Release，如果是不ToFile，那么指针会被Scene记录下来，但不加引用计数
	/*STDMETHOD(AddRepresentObject)(IEKG3DRepresentObject* pObject, BOOL bSeletable, BOOL bSeleted)
	{
		return AddRepresentObject(pObject, bSeletable, bSeleted, FALSE, FALSE);
	}*/
	virtual HRESULT AddRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelect,BOOL bLight);	//一定是Selectable的，不用设
	virtual HRESULT RemoveSelectableModel(IKG3DResourceBase *pEntity);


	//内部的版本
	virtual HRESULT AddRenderEntity(KG3DSceneEntity Entity, BOOL bIsSelectable = TRUE, BOOL bIsSelected = FALSE);

    STDMETHOD(RemoveRepresentObject)(IEKG3DRepresentObject* pObject);
	STDMETHOD(DelRepresentObject)(IEKG3DRepresentObject* pObject);//占住这个名字，省的和RemoveXX的意义不一致	
	virtual HRESULT RemoveRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelect = TRUE);

	//内部的版本
	virtual HRESULT DelRenderEntity(KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList = TRUE);//占名，条用RemoveRenderEntity
	virtual HRESULT RemoveRenderEntity(KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList = TRUE);//是否在SeletedList中也删除Entity
	virtual HRESULT RemoveRenderEntityAndNotifyLogic(KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList = TRUE);
	
	//外部的版本
	virtual HRESULT AddRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer);
	virtual HRESULT RemoveRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer);
	virtual HRESULT RemoveRenderEntityAndNotifyLogic(SCENEENTITYTYPE type,PVOID pPointer);//会通知逻辑

	virtual ULONG	GetObjectCount(DWORD dwTypeFilter);
	virtual HRESULT Traverse(KG3DRepresentObjectVisitor& visitor);
	//</Add Remove>

	//选择器的接口
	//virtual HRESULT SetSelectable(const IEKG3DSceneEntity& Entity, BOOL bSelectable);//<Selectable Refactor>
	virtual HRESULT SetSelected(const IEKG3DSceneEntity& Entity, BOOL bSelected);
	//virtual HRESULT SetSelectable(SCENEENTITYTYPE dwType, LPVOID Pointer, BOOL bSelectable);//<Selectable Refactor>
	virtual HRESULT SetSelected(SCENEENTITYTYPE dwType, LPVOID Pointer, BOOL bSelected);
	virtual HRESULT SetSelected(IEKG3DRepresentObject* pObject, BOOL bAdd);

	//<Object 和 Handle>//用于放置工具
	virtual HRESULT GetObjectInHandle(IEKG3DRepresentObject** ppObj);
	virtual HRESULT SetObjectInHandle(IEKG3DRepresentObject* pObject, int nReleaseFlag = TRUE);//nReleaseFlag如果是TRUE,pObject的生命周期由ObjectPlacementTool接管，自动释放，否则使用者释放。都不加引用计数
	//</Object 和 Handle>

	STDMETHOD(BeginMovieREC)(
        SIZE                 Size, 
        FLOAT                fInterval  = 33.33F/3.0F, 
        D3DTEXTUREFILTERTYPE FiterType  = D3DTEXF_LINEAR,
        D3DXIMAGE_FILEFORMAT ImageType  = D3DXIFF_JPG,
        INT                  nAutoScale = TRUE
        ); 
    STDMETHOD(EndMovieREC)(); 
    STDMETHOD(PlayEntityAnimation)(int nPlay);

	//(by dengzhihui Refactor 2006年11月27日
	virtual HRESULT SetEditState(DWORD dwState);
	virtual HRESULT GetCurOutputWnd(IEKG3DSceneOutputWnd **ppiRetCurOutputWnd);
	
	virtual HRESULT RemoveOutputWindow(int nOutputWindowID);
	virtual HRESULT GetEditState(DWORD* pdwState);
	virtual HRESULT SetSampleMip(D3DTEXTUREFILTERTYPE eType);
	virtual HRESULT SetSampleMag(D3DTEXTUREFILTERTYPE eType);
	virtual HRESULT SetSampleMin(D3DTEXTUREFILTERTYPE eType);
	virtual HRESULT SetMipmapLod(float fValue);
	virtual HRESULT GetScene(IEKG3DScene** ppScene);
	virtual HRESULT GetSceneDetailMaterialMgr(IEKG3DDetailMaterialMgr** ppMtl);
	
    virtual BOOL GetWaterReflect();
	virtual HRESULT SetWaterReflect(BOOL bValue);

    virtual BOOL GetWaterRefract();
    virtual HRESULT SetWaterRefract(BOOL bValue);

	virtual float GetZFar();
	virtual HRESULT SetZFar(float fZFar);
	virtual HRESULT GetSceneEditorBase(IEKG3DSceneEditorBase** ppSceneBase);
	
	virtual HRESULT GetRepresentNPC(IEKG3DRepresentNPC** ppNPC);
	virtual HRESULT GetPrimaryWindow(IEKG3DSceneOutputWnd **ppiRetPrimaryWindow);

	virtual D3DXVECTOR3 GetCameraTopPos();
	virtual HRESULT SetCameraTopPos(float x, float z);
	virtual D3DXVECTOR3 GetCameraMainPos();
	virtual float GetCameraTopScale();
	virtual HRESULT SetCameraTopScale(float fScale);
	virtual HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>
	
	virtual INT GetObjectGroupSize();
	virtual HRESULT GetGroupInfo(INT nIndex, INT& nGroupSize, LPTSTR szName, INT nBufLen);
	
	//)
    virtual HRESULT IESetLogicalScene(IEKG3DSceneLogicalEditorBase* pLogicalScene);
   
    virtual HRESULT IEOnResizeWindow(PRECT const pRect);

	
	struct EnviromentParameter
	{
		D3DCOLORVALUE Ambient;
		D3DCOLORVALUE Diffuse;
		D3DCOLORVALUE LightDiffuse;
		DWORD LightAmbient;
	};
	void GetEnviromentParamenter(const D3DXVECTOR3& vec3Pos, EnviromentParameter& EnvPara);
	virtual HRESULT SetLogicalSceneConnection(IELogicalSceneConnection* pLogicalSceneConnection)
	{
		m_pLogicalSceneConnection = pLogicalSceneConnection;
		return S_OK;
	}

	virtual IEKG3DRepresentNPC* GetIEKG3DRepresentNPC(PVOID pObject);
	virtual IEKG3DRepresentDoodad* GetIEKG3DRepresentDoodad(PVOID pObject);
	virtual IEKG3DRepresentWayPoint* GetIEKG3DRepresentWayPoint(PVOID pObject);
	virtual IEKG3DRepresentTrafficPoint* GetIEKG3DRepresentTrafficPoint(PVOID pObject);
	virtual IEKG3DRepresentRefreshPoint* GetIEKG3DRepresentRefreshPoint(PVOID pObject);

	virtual HRESULT SetEditing(DWORD dwEditing);

	virtual HRESULT ForceLoadAllEntity();
	virtual HRESULT ForceLoadEntity(D3DXVECTOR2 A, D3DXVECTOR2 C);
	virtual HRESULT ComputeAllEntityBBox();
	virtual HRESULT ComputeTerrainNormal();
	virtual HRESULT CompositorTerrainTexture();
	virtual HRESULT RefreshTerrain();
	virtual HRESULT EnableLoadTexture(BOOL bEnable);
	virtual HRESULT PauseMultiThreadLoad(BOOL bEnable);

	//////////////////////////////////////////////////////////////////////////
	//区域选择
	virtual HRESULT BeginSelectSceneBlock(BOOL bZone,BOOL bSection,BOOL bRegion,BOOL bOnlyExist);
	virtual HRESULT EndSelectSceneBlock(int nAddFlag, int nFinishFlag);
	virtual HRESULT AddSelectSceneBlockPoint(int nIndex,D3DXVECTOR3 vPos);
	virtual HRESULT EnableRenderSelectedSceneBlock(BOOL bEnable);
	virtual std::list<POINT>* GetCurSelectedSceneBlock(int nLevel);
	virtual HRESULT CreateSelectedSceneBlock(BOOL bCreateTerrain);
	virtual HRESULT DeleteSelectedSceneBlock(BOOL bOnlyDeleteTerrain);

	HRESULT RenderSceneRegionSelection();

	virtual HRESULT ApplyHeightMapToSelectedSceneBlock(LPTSTR pHeightMap,int nWidth,float fLowest,float fHeightTotal);
	virtual HRESULT ApplyBaseMapToSelectedSceneBlock(LPTSTR pBaseMap);

	virtual IEKG3DRepresentPoly* GetSelectedPoly();

	virtual HRESULT AddOneMap(LPCTSTR pFileName);
	virtual HRESULT LoadDefaultTerrain(KG3DTERRAIN_LOAD_PARAM* dwExtraParam);
	virtual HRESULT AddPolyVetexInTopView(D3DXVECTOR3& vInter);

	virtual BOOL IsSceneSoundEnable();
	virtual HRESULT EnableSceneSound(BOOL bEnable);
	virtual BOOL IsSceneSoundVisiable();
	virtual HRESULT VisiableSceneSound(BOOL bVisiable);
	virtual BOOL IsSoundRangeVisible();
	virtual void VisibleSoundRange(BOOL bVisible);
	///////////////////////////////////////////////////////////////////////
	//地形复制相关  add by liumingod
	
	
	::KG3DRepresentTerrainBlock* m_pTerrainBlock;
	BOOL m_bIsRenderCopyBrush;
	BOOL m_bIsTerrainState;

	virtual HRESULT ResetSelectTerrain();
	virtual HRESULT TerrainCopyWindowOnShow();
	virtual HRESULT TerrainCopyWindowOnHide();
	virtual HRESULT SelectTerrainFinished(); 
	virtual HRESULT PasteSeclctTerrain(DWORD dwMixType,bool bIsPasteBlend,bool bIsPasteObject,bool bIsMax,float fWeight,float fWidth);
	bool m_bIsTerrainPaste;

	virtual HRESULT SaveTerrainBlock(const char cszFileName[]);
	virtual HRESULT LoadTerrainBlock(const char cszFileName[]);

    virtual HANDLE  AddSceneSigns(const D3DXVECTOR3& vPos);
    virtual void    DelSceneSings(HANDLE Handle);



    /* add the pvs protal to pvs object, and add the pvs portal entity to scene
       just use to scene editor */
    virtual HRESULT AddPvsPortal(IEKG3DRepresentPVS* pPvs, const D3DXVECTOR3& vPos, 
        const D3DXVECTOR3& vDir, float fWidth, float fHeight);

    virtual HRESULT AddPvsPortal();


    /* calc the current outdoor frustums use current camera */
   // virtual HRESULT CalcFrustum(KG3DCamera* pCamera);


    /* modify the current edit space node' information 
       to the data interface */
    virtual HRESULT UpdateEditSpaceNodeInformation();
    virtual HRESULT NotifyObjInfomationChanged(IEKG3DRepresentObject* pObj);



	HRESULT GetGroundHeight(float& fHeight,float x,float y);

protected:
    HRESULT RenderTopWindowsColor(KG3DSceneOutputWnd *pWnd,const D3DXMATRIX* matProj);
	HRESULT RenderMainWindowsColor(KG3DSceneOutputWnd *pWnd);
	HRESULT RenderWaterAndSFX();

	list<D3DXVECTOR2>m_listSceneBlockSelectPoint;
	D3DXVECTOR2 m_RegionSelectA;
	D3DXVECTOR2 m_RegionSelectC;
	D3DXVECTOR2 m_RegionSelectStartPos;
	list<POINT> m_listSceneBlockSelected[4];
    list<POINT> m_listTempSelBlock[4];
	BOOL        m_bEnableSceneBlockSelect[4];
	BOOL        m_bSelectSceneBlockExistOnly;
	float       m_fLoadingState;//切场景时候的加载进度条百分比

	//////////////////////////////////////////////////////////////////////////
	//多边形编辑
	vector<D3DXVECTOR3>m_vecPolyVertexes;//多边形的顶点
	float m_fPolyVertexHeight;

	virtual FLOAT GetPolyHeight();
	virtual HRESULT SetPolyHeight(FLOAT fH);

	virtual HRESULT BuildPolyAndSetInHand(int nVergeNum,float fRadii,float fHeight,float fDegree);
	virtual HRESULT BeginAddPolyVertex();
	virtual HRESULT EndAddPolyVertex(DWORD dwType);
	HRESULT AddPolyVetex(int nIndex,D3DXVECTOR3& vPos);

	HRESULT RenderPoly();

	virtual INT GetLogicalPolyIntersectRect(IEKG3DRepresentPoly* pPolyArray[],INT nMax,D3DXVECTOR2 A,D3DXVECTOR2 C);

	virtual HRESULT EnableRenderPoly(BOOL bEnable);
	virtual BOOL IsRenderPoly();

	virtual HRESULT EnableRenderAlertRange(BOOL bEnable);
	virtual HRESULT EnableRenderAttackRange(BOOL bEnable);

	HRESULT 		ShowInfo();

	virtual float GetLoadingProcess();
	float GetLoadingProcessTick();

	//////////////////////////////////////////////////////////////////////////
	//pvs
	virtual HRESULT	GetCurrentSelectedObjectPVS(IEKG3DRepresentPVS** ppPVS);
	virtual HRESULT ConvertCurrentSelectedObjectToPVS(IEKG3DRepresentPVS** ppPVS);
	virtual HRESULT MoveSceneObjectIntoPVS(IEKG3DRepresentPVS* pPVS);
	virtual HRESULT MoveObjectFromPVSToScene(IEKG3DRepresentPVS* pPVS);

    //ISelectFilter*  GetSelFilter()                          { return m_pSelectFilter; }//<ISelectFilter Refactor>
   // void   SetSelFilter(ISelectFilter* pSelectFilter)       { m_pSelectFilter = pSelectFilter; }//<ISelectFilter Refactor>

	//////////////////////////////////////////////////////////////////////////
	//antiportal
	virtual HRESULT	GetCurrentSelectedObjectAntiPortal(IEKG3DRepresentAntiPortal** ppAntiPortal);
	virtual HRESULT ConvertCurrentSelectedObjectToAntiPortal(IEKG3DRepresentAntiPortal** ppAntiPortal);

	//////////////////////////////////////////////////////////////////////////////
    //交通点相关
	virtual HRESULT AddTrafficPoint(IEKG3DRepresentObject** ppPoint,D3DXVECTOR3& vecPos,DWORD dwType,DWORD dwSetID);

	virtual void __stdcall OnReceiveMessage(const KG3DMessage& Message , IKG3DMessageSenderBase* pSender);
private:
	HRESULT ProcessEarlyZ(KG3DSceneOutputWnd *pWnd);
    HRESULT ProcessEarlyZTopWindow(KG3DSceneOutputWnd *pWnd);

private:

	IEKG3DSceneLogicalEditorBase* m_lpLogicEditor;
	IELogicalSceneConnection* m_pLogicalSceneConnection;

public:
	virtual void RenderForPVSGeneration();
	HRESULT BeginPVSRender();
	HRESULT EndPVSRender();
	virtual void EnablePVS(BOOL bEnable);
	virtual BOOL GetPVSState();

private:
	struct PVSParameter
	{
		PVSParameter();

        HRESULT OnLostDevice();
        HRESULT OnResetDevice();

		HRESULT Init();
		HRESULT UnInit();
		BOOL						   bUsePVS;
		IDirect3DTexture9*      pRenderTargetTexture;
		int                     nWidth;
		int                     nHeight;
		IDirect3DSurface9*      pDepth;

		D3DVIEWPORT9            vpSave;
		IDirect3DSurface9*      pDepthSave;
		IDirect3DSurface9*      pRenderTargetSave;
		D3DVIEWPORT9            vpCurrent;
		IDirect3DSurface9*      pRenderSurface;
	};

	PVSParameter m_PVSParameter;
	KG3DWaterEffect* m_lpWaterEffect;
	//<SCENE_SETTING>
public:
	virtual HRESULT SetSceneSettingData(DWORD dwEnum, DWORD_PTR dwData);
	virtual HRESULT GetSceneSettingData(DWORD dwEnum, DWORD_PTR* pdwData);
private:
	KG_CUSTOM_HELPERS::TCommonParamBuffer<KG3DSceneSettings, 52>	m_lpSettings;//注意这是个智能指针，不用释放，不用判断是否为空
	//</SCENE_SETTING>

	//<属性>
private:
	enum
	{
		EM_USE_CROSS_PLAY_MODE = 0x01,	//和放置物件的方式有关系，如果为True，使用Camera的碰撞方式，否则用Terrain的
	};
	KG_CUSTOM_HELPERS::KGBitSet<32>	m_BitSet;//用BOOL来保存属性太浪费了，用KGBitSet可以省空间，而且也不会丧失多少速度
	LPDIRECT3DCUBETEXTURE9 m_lpEvnMap;//<EnvMap Mov>
	std::tstring            m_strEnvMapName;
public:
	virtual HRESULT SetUseCrossPlayMode(BOOL bEnable)		{m_BitSet.Set<EM_USE_CROSS_PLAY_MODE>(bEnable);return S_OK;}
	virtual ULONG IsUseCrossPlayMode()		{return m_BitSet.Is<EM_USE_CROSS_PLAY_MODE>();}
	virtual ULONG GetEditState();	
	//</属性>

    // 摄像机动画
public:
    virtual HRESULT LoadCameraAnimation(LPSTR pszCameraAniFile);
    virtual HRESULT StartPlayCamAni();
    virtual HRESULT StopPlayCamAni();
    virtual HRESULT PausePlayCamAni();

private:
    /* 
	HRESULT OutPutPostRenderShadow(KG3DSceneOutputWnd *pWnd,D3DVIEWPORT9* pView);
    */

public:
	
	HRESULT SaveKG3DEnvironment(LPSTR pFileName);
	HRESULT LoadKG3DEnvironment(LPSTR pFileName);

	//<通用子元件>
public:
	virtual HRESULT GetEngineIEInterface(DWORD dwType,  DWORD_PTR wParam, DWORD_PTR lParam, VOID** ppiInterface);//用于和SceneEditor交互，覆盖KG3DScene的同名函数
	virtual HRESULT GetEngineIEAbstructInterface(DWORD dwAbstructType, DWORD dwType,  DWORD_PTR wParam, DWORD_PTR lParam, VOID** ppiInterface);
	virtual HRESULT GetSelectionToolInterface(IEKG3DSceneSelectionTool** ppTool);
	//</通用子元件>


public:
	HRESULT SetOpenAddTexture(BOOL bFlag);
	BOOL GetOpenAddTexture() const;

	DWORD m_dwSceneMapID;
public://添加草，石头，灌木相关
	virtual HRESULT GetTBModifierTextureStride(int* pX,int* pY ,DWORD dwType);
	virtual HRESULT SetTBModifierTextureStride(int  nX,int  nY ,DWORD dwType);

	virtual HRESULT GetTBModifierWidthMax(FLOAT* pValue);
	virtual HRESULT GetTBModifierWidthMin(FLOAT* pValue);
	virtual HRESULT SetTBModifierWidthMax(FLOAT fValue);
	virtual HRESULT SetTBModifierWidthMin(FLOAT fValue);
	virtual HRESULT GetTBModifierHeightMax(FLOAT* pValue);
	virtual HRESULT GetTBModifierHeightMin(FLOAT* pValue);
	virtual HRESULT SetTBModifierHeightMax(FLOAT fValue);
	virtual HRESULT SetTBModifierHeightMin(FLOAT fValue);
	virtual HRESULT SetTBModifierNumPerM2(FLOAT fValue);
	virtual HRESULT SetTBModifierTextureIndexMax(INT nMax);
	virtual HRESULT SetTBModifierTextureIndexMin(INT nMin);
	virtual HRESULT SetTBModifierTextureIndexInfo(INT* pPointer, INT* pNumIndex);
	virtual DWORD GetTBModifierState();
	virtual HRESULT SetTBModifierState(DWORD dwState);
	virtual HRESULT UpdatePatternList(INT* pList,INT nCount);
	virtual IEKG3DSceneOutDoorSpaceMgr* GetIEKG3DSceneOutDoorSpaceMgr();
	virtual HRESULT GSFFitGround();
	virtual int GetTBModifierGrassDensity();
	virtual HRESULT SetTBModifierGrassDensity(int nDensity);
	virtual HRESULT DeleteAutoGrass(BYTE byDetailTexIndex);
	virtual HRESULT BuildAutoGrass(BYTE byDetailTexIndex);
	virtual HRESULT SetAddAutoGrass(BOOL bVal);
	virtual HRESULT ClearAllTerrainObject();
	//跟地形相关
	virtual HRESULT GetTerrainModifyHeight(float& fHeight);
	virtual HRESULT SetTerrainModifyHeight(float fHeight);
	virtual HRESULT SetOppositeHeightValue(FLOAT fOppValue);
	virtual HRESULT SetIsGetAbsoluteHeight(BOOL bIsAbs);
	virtual float   GetTerrainModifyAlpha();
	virtual HRESULT SetTerrainModifyAlpha(float fAlpha);
	virtual DWORD GetInnerHeightBrushSize();
	virtual DWORD GetOuterHeightBrushSize();
	virtual HRESULT SetInnerHeightBrushSize(DWORD dwSize);
	virtual HRESULT SetOuterHeightBrushSize(DWORD dwSize);
	virtual HRESULT SetDiShu(FLOAT fPower);
	virtual HRESULT ReSizeHeightData(const WORD BrushSize);
	virtual HRESULT SetPaintTexIndex(BYTE byIndex);
	virtual BYTE GetPaintTexIndex();
	virtual HRESULT SetCurrentBrushMap(LPCTSTR pFileName);
	virtual HRESULT SetRenderBlockEdge(BOOL bVal);
	virtual HRESULT SetRenderWaterEdge(BOOL bVal);
	virtual HRESULT PaintTerrainTextureFromFilterMap(LPCSTR pFileName);
	virtual HRESULT ExprotFilterMap(LPCSTR pFileName);
	virtual HRESULT ClearTextureFromTerrain(BYTE byTexIndex);
	virtual HRESULT ExprotNormalMap(LPCSTR pFileName);
	//////////////////////////////////////////////////////////////////////////
	//墙体工具相关
	virtual HRESULT GetDefaultWallDefine(IEKG3DWallDefine** ppWallDefine);
	virtual HRESULT SetWallConnectionPoint(BOOL bShow,D3DXVECTOR3 vA,D3DXVECTOR3 vB);
	virtual IEKG3DWall* GetFirstSelectedObjectWall();
	virtual HRESULT AddWallNodeToScene(IEKG3DWall* pWall,int nWall,int nKeyPoint);

	BOOL m_bShowWallConnectionPoint;
	D3DXVECTOR3 m_vWallConnectionPointA;
	D3DXVECTOR3 m_vWallConnectionPointB;
    KG3DMeasureTool m_vWallMeasureTool;

	virtual HRESULT GetLODProperties(FLOAT* pfLODHightToMiddleSwitchAngleGate, FLOAT* pfLODMiddleToLowSwitchAngleGate);
	virtual HRESULT SetLODProperties(FLOAT fLODHightToMiddleSwitchAngleGate, FLOAT fLODMiddleToLowSwitchAngleGate);

	//////////////////////////////////////////////////////////////////////////
	//地形高度图导出导入相关
	virtual HRESULT ExprotTerrainHeightMap(LPCSTR pFileName,DWORD dwOption);
	virtual HRESULT ImportTerrainHeightMap(LPCSTR pFileName,DWORD dwOption);
	virtual HRESULT GetSceneHeightInfo(int* pLowest,int* pHeightTotal);
	virtual HRESULT SetSceneHeightInfo(int  nLowest,int  nHeightTotal);
	//水面
	virtual HRESULT RefreshWaterEffect();
	HRESULT SetTerrainWaterDefaultHeight(float fHeight);
	virtual HRESULT SetAlwayRenderWater(BOOL bVal);
	virtual BOOL    GetAlwayRenderWater();
	//////////////////////////////////////////////////////////////////////////
	//游戏逻辑区域的显示
	virtual HRESULT EnableShowLogicalSize(BOOL bEnable);
	BOOL m_bEnableShowLogicalSize;
	//显示指定逻辑区域
	virtual HRESULT EnableShowAppointLogicalRegion(BOOL bEnable);
	BOOL m_bEnableShowAppointLogicalRegion;
	///////////////////////////////////////////////////////////////////////////
	//是否线框渲染
	HRESULT SetEnableWireFrameMode(BOOL bEnable);
	//////////////////////////////////////////////////////////////
	virtual void EnableBlockBox(BOOL bEnable);
	///////////////////////////////////////////////////////////////////////////
	virtual DWORD GetEntityNum();
	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT ReloadTerrainRenderShader();

	// 策划场景编辑器
	virtual HRESULT SetCameraSpeed(FLOAT fSpeed);
	virtual HRESULT GetCameraPosLookat(AtlVector3* pos, AtlVector3* lookat);
	virtual HRESULT SetCameraPosLookat(AtlVector3* pos, AtlVector3* lookat);
	virtual HRESULT UpdateSelectableEntityList(LONG iCount);
	virtual HRESULT NpcPlayNextAction();
	virtual HRESULT NpcPlayPreviousAction();
	virtual HRESULT SetCameraLocation(FLOAT fPositionX, FLOAT fPositionY, FLOAT fPositionZ, FLOAT fDirectionX, FLOAT fDirectionY, FLOAT fDirectionZ);
	virtual HRESULT RotateSelectedObject(LPTSTR newRotationStr);
	virtual HRESULT GetSceneRect(LONG* width, LONG* height);
	virtual HRESULT SetObjectSelectMaskDesign(DWORD mask);
	virtual HRESULT CopySelection();
	virtual HRESULT DoPlaceObject();
	virtual HRESULT SetCurrentEditBrushIndex(INT nIndex);
	
	////////////////////////////
	virtual void SetSelectOption(DWORD dwOption);
	virtual DWORD GetSelectOption();
	virtual void SetLimitValue(DWORD dwLimitValue);
	virtual DWORD GetLimitValue();
private://LOD的控制器
	//KG3DLODAdjuster*	m_pLODAdjuster;
	HRESULT LoadPreloadNpc(LPCSTR strFileName);
	std::vector<KG3DModel*> m_PreloadModels;
	std::list  <string>     m_PreloadModelsName;
	int                     m_nNumPreloadModels;

// 策划场景编辑器
private:
	BOOL m_DE_Forward;
	BOOL m_DE_Backward;
	BOOL m_DE_Left;
	BOOL m_DE_Right;

	struct CameraData 
	{ 
		D3DXVECTOR3 position;
		D3DXVECTOR3 direction;
		BOOL valid;
	};

	struct CopyInterFil : public KG3DIntersectionFilter
	{
		vector<KG3DRepresentObject*> m_vectorFiltObject;
		
		virtual HRESULT SetFiltObject(vector<KG3DRepresentObject*> vectorFiltObject)
		{
			m_vectorFiltObject.clear();

			for (vector<KG3DRepresentObject*>::iterator it = vectorFiltObject.begin(); it != vectorFiltObject.end(); it++)
			{
				KG3DRepresentObject* pRepresentObject = *it;
				m_vectorFiltObject.push_back(pRepresentObject);
			}

			return S_OK;
		}

		virtual HRESULT Accept(KG3DRepresentObject& Object)
		{
			for (vector<KG3DRepresentObject*>::iterator it = m_vectorFiltObject.begin(); it != m_vectorFiltObject.end(); it++)
			{
				KG3DRepresentObject* pRepresentObject = *it;
				if (pRepresentObject == &Object)
				{
					return E_FAIL;
				}
			}

			return S_OK;
		}
	};

	CameraData m_ForceSetCameraData; // 摄像机位置

	INT m_PlayActionIndex; // npc播放动作的标记
	INT m_AniFileCount; // ani文件数量
	string m_CurrentModelName; // 当前模型的文件名
	string m_AniFileArray[100]; // npc播放动作ani文件数组	
	vector<KG3DRepresentObject*> m_vectorCopyRepresentObject; // 复制对象的容器
	CopyInterFil m_CopyInterFil; // 复制对象的碰撞过滤
	INT m_nCurrentEditBrushIndex;
	D3DXVECTOR3 m_vecCurrentEditBrushPos;

	HRESULT FrameMoveCameraDesign();
	HRESULT FrameMoveSelection();


	//更改场景中的实体 by likan
	void ReplaceSelectedEntity(bool all);

    //按templateid查找场景中已经加载的npc
	void FindLoadedNpc(vector<IEKG3DRepresentObject *>&npcs, DWORD templateid);

	//按templateid查找场景中已经加载的doodad
	void FindLoadedDoodad(vector<IEKG3DRepresentObject *>&doodads, DWORD templateid);

public:
    virtual HRESULT UpdateSettings(DWORD dwSettingType);
};





//重新加载场景中实体的vistor by Likan
class SceneReLoadEntityVistor:public KG3DRepresentObjectVisitor
{
public:
	TypeEntityAccessProxy* proxy;                        //选择列表迭代器
	KG3DSceneOutDoorSpaceMgr* m_lpSceneOutDoorSpaceMgr;
	vector<UINT> handles;                                //需要重新加载的实体id
	HRESULT Accept(KG3DRepresentObject& Obj);
};


class SceneLoadedNpcVistor:public KG3DRepresentObjectVisitor
{
public:
	DWORD m_Tid;
	vector<IEKG3DRepresentObject*> *m_findObjs;
	HRESULT Accept(KG3DRepresentObject& Obj);
};

class SceneLoadedDoodadVistor:public KG3DRepresentObjectVisitor
{
public:
	DWORD m_Tid;
	vector<IEKG3DRepresentObject*> *m_findObjs;
	HRESULT Accept(KG3DRepresentObject& Obj);
};


extern float g_fCameraNpcDistance;
extern BOOL  g_bEnableAddEntity;
extern bool bRenderFloorList;
extern std::deque<D3DXVECTOR3> g_ListFloorList;
//extern BOOL g_bRenderSnap;