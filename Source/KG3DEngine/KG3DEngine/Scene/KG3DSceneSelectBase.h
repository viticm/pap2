#pragma once
#include "KG3DSceneEntity.h"
#include "KG3DFrustum.h"
#include "KG3DEditMoveGrid.h"
#include "KG3DEditRotationGrid.h"
#include "KG3DEditScaleGrid.h"
#include "KG3DToolsImpl.h"
#include "IEKG3DSceneSelectionTool.h"//by dengzhihui Refactor 2006年11月24日
#include "KG3DSceneSelectionTool.h"

#include "KG3DLocalScalCoord.h"
#include "KG3DLocalRotationCoord.h"
#include "KG3DLocalMoveCoord.h"

struct KG3DTransformTool;

class KG3DScene;

class KG3DCamera;
class KSceneModelEditorMeshBindDialog;
class KG3DSceneLayer;
interface IEKG3DSFXBillboard;
class KG3DSceneEntity;
class KG3DMessageReceiverBase;
class KG3DMessageSenderBase;
class KG3DMessage;
class IKG3DMessageSenderBase;
class KG3DMessageActionExecutionStruct;

class KG3DSceneSelectBase : public CKG3DConnectionPoint<IEKG3DSceneSelectBase>, public KG3DSceneSelectionTool
{
	friend class KSceneModelEditorMeshBindDialog;
private:
	DWORD m_dwLimitValue;
	
public:
	virtual void SetSelectOption(DWORD dwOption, DWORD dwOptionDesign);
	virtual DWORD GetSelectOption();
	virtual void SetLimitValue(DWORD dwLimitValue);
	virtual DWORD GetLimitValue();
	
	virtual HRESULT SetSelectable(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelectable);	//临时方法
	virtual HRESULT SetSelectable(const IEKG3DSceneEntity& Entity, BOOL bSelectable);
	virtual HRESULT SetSelectable(IEKG3DRepresentObject& obj, BOOL bSelectable);

	virtual HRESULT SetSelected(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelected);	//历史方法
	virtual HRESULT SetSelected(const IEKG3DSceneEntity& Entity, BOOL bSelected);

	virtual HRESULT SetSelected(IEKG3DRepresentObject& obj, BOOL bSelected);

	virtual HRESULT SelectAll();//全选，物件超过EM_SELECTION_SELECT_ALL_MAX的话，失败，并不会选择。不然对于大场景的话会出问题，大部分3D软件都无法避免Ctrl+A之后变很慢的问题。
	virtual HRESULT SelectInvert();//反选，有物件数限制，同SelectAll

	virtual HRESULT TraverseSelected(KG3DRepresentObjectVisitor& visitor);
	virtual HRESULT TraverseSelected(KG3DSceneEntityVisitor& visitor);
	virtual HRESULT TraverseSelectable(KG3DSceneEntityVisitor& Visitor);

	virtual HRESULT ClearSelectable();
	virtual HRESULT ClearSelection();

	virtual ULONG	GetSelectionCount();
	virtual ULONG	IsSelected(const KG3DRepresentObject& Obj);
	virtual ULONG	IsSelectable(const KG3DSceneEntity& entity);

	virtual HRESULT AllocSelGroup(LPCTSTR pGroupName);
	virtual HRESULT ClearSelGroup(LPCTSTR pGroupName);
	virtual ULONG	GetSelGroupSize(LPCTSTR pGroupName);

	virtual HRESULT CopySelGroupToCurSelection(LPCTSTR pGroupName);
	virtual HRESULT CopyCurSelectionToSelGroup(LPCTSTR pGroupName);
	virtual HRESULT CopySelGroupToSelectable(LPCTSTR pGroupName);
	virtual HRESULT CopySelectableToSelGroup(LPCSTR pGroupName);

	virtual TypeEntityAccessProxy& GetSelectionAccessProxy();
	virtual HRESULT GetAccessProxy(IEKG3DSceneSelectionToolAccessProxy** ppAccessProxy);

	virtual HRESULT GetSelectableList(KG3DSceneEntityList& entities);

	virtual HRESULT RegisterEventListener(DWORD messageID, IKG3DMessageReceiverBase& receiver);

	virtual HRESULT PointSelection(ULONG uIncreaseMode, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, BOOL bNearestOrAll);//PointSelection必须遵循以下原则：如果选择点找不到，那么找最近的物件的BBOX的碰撞点，不然有些物件会很难选择。bNearestOrAll表示如果是Nearest，一切按正常走，如果是All，线上所有都选中
	virtual HRESULT RectSelection(ULONG uIncreaseMode, const D3DXVECTOR4& vLeftTopRightBottom
		, const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXMATRIX& matViewport, FLOAT fSelRange);

	virtual ULONG	IsNewSelectionTool(){return FALSE;}

#if defined(DEBUG) | defined(_DEBUG)
	virtual KG3DMessageSender* TestBegin();
	virtual VOID TestEnd();
#endif
protected:
	//BOOL       m_bFocusing;//这个成员没有存在的必要
	
	//BOOL m_bShowSelectableEntityMark;
	
	enum{em_max_distance_for_point_selection = 5,};
	

	DWORD GetIncreaseMode(){return m_dwIncreaseMode;}
	HRESULT SetIncreaseMode(DWORD dwMode){m_dwIncreaseMode = dwMode; return S_OK;}

	KG3DTransformTool& GetTransformToolInterface(DWORD dwType);
//public:
	//ahpho
	//KG3DSceneEntity	m_SelectedKingEntity;//选中的king //2009-1-13。其实就是选择列表最后的一个，直接操作选择列表得到最后一个就行了。

	//KG3DSceneEntityList m_listClipBoardEntity;	//用于复制//<Copy Refactor>

	//KG3DSceneEntityList m_listTempSelectedEntity;//2009-1-16 <m_listTempSelectedEntity Refactor> 去掉，和m_listSelectedEntity合并就行了

    //ISelectFilter* m_pSelectFilter;	//<ISelectFilter Refactor>
private:	//移动为private的成员不要再暴露为protected了，基本所有操作都可以用方法进行
	BOOL m_bSelecting;
	BOOL m_bMoving;
	BOOL m_bRotating;
	BOOL m_bScaling;

	//D3DXVECTOR3 m_Selection_Origin_A;//选择椎体的两个位置
	//D3DXVECTOR3 m_Selection_Direction_A;

	//D3DXVECTOR2 m_Selection_Origin2D_A;
	//D3DXVECTOR3 m_Selection_Origin_C;
	//D3DXVECTOR3 m_Selection_Direction_C;
	//D3DXVECTOR2 m_Selection_Origin2D_C;

	struct SelectionInput 
	{
		D3DXVECTOR3	vRaySrcA;	//选择椎体的两个位置
		D3DXVECTOR3 vRayDirA;

		D3DXVECTOR3 vRaySrcC;	//拉开的选择框的点
		D3DXVECTOR3 vRayDirC;

		D3DXVECTOR2	vPosWindowA;	//在窗口的屏幕坐标
		D3DXVECTOR2 vPosWindowC;

		DWORD dwSelMask;
		FLOAT fSizeLimit;

		DWORD dwSelMaskDesign;  //策划编辑器用

		SelectionInput();
	};
	SelectionInput	m_SelInput;

	D3DXVECTOR3 m_SelectionCenter;
	DWORD m_dwIncreaseMode;

	typedef std::map<std::tstring, KG3DSceneEntityList>	TypeCtList;
	TypeCtList	m_SelGroups;

	KG3DSceneEntityList m_listSelectedEntity;//选中的
	KG3DSceneEntityList m_listSelectableEntity;

	KGCH::TFrontAccessProxySolid<KG3DSceneEntityList> m_accessProxy;	//注意，这个必须比m_listSelectedEntity后声明，因为要依赖它来初始化。
	IEKG3DSceneSelectionToolAccessProxy* m_pIEAccessProxy;

	KG3DScalCoordinate	m_coordScale;
	KG3DRotationCoordinateOld m_coordRotate;
	KG3DMoveCoordinate m_coordMove;

	KG3DMessageReceiverBase* m_pEngineMsgReceiver;
	KG3DMessageSenderBase*	m_pSender;
	KG3DScene&	m_Scene;	//这里不属于耦合，以后Scene是作为一个统一基类操作对象而存在的，

	enum{
		em_transformToolCount = KG3DTYPE_TRANSFORM_TOOL_END - KG3DTYPE_TRANSFORM_TOOL_BEGIN + 1,
	};
	KG3DTransformTool* m_TransformTools[em_transformToolCount];	//这里用聚合的方式来模拟出KG3DTransformTool的接口。SceneSelectBase同时做了选择工具和移动，旋转，缩放工具，所以只能几个接口都做在里面。而新的移动，旋转，缩放工具是不需要这样子的。

	

	KG3DSceneEntityList* GetSelGroup(LPCTSTR strName);
	

	void ExecActionClick( KG3DMessageActionExecutionStruct* pExecActionStruct );
	void HandleWindowsMessages(UINT uWindowsMsgID, DWORD_PTR wParam, DWORD_PTR lParam);	//部分编辑器没有完整的EXECACTION机制，暂时用着旧的Messages机制

	static BOOL FindNearestOrFindAll(const D3DXVECTOR2& vUpLeft, const D3DXVECTOR2 vBottomRight);
	static HRESULT GenerateTempSelectedList(const D3DVIEWPORT9& Viewport, const D3DXMATRIX& View, const D3DXMATRIX& Projection,SelectionInput& selInput
										, KG3DSceneEntityList& listSelectable, KG3DSceneEntityList& listSelected, DWORD dwIncreaseMode);
	HRESULT LaunchSelect();
	static BOOL EntityIsSelectable(KG3DSceneEntityList& listSelectable, SelectionInput& selInput, KG3DSceneEntity& entity);	//选择过滤

	//这些都不再可以在派生类中继承，如果需要接收对应的消息，走Observer那一套，用RegisterEventListener来注册
	/*virtual*/ HRESULT SelectBegin(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,D3DXVECTOR2 Origin2D, DWORD dwOperation);
	/*virtual*/ HRESULT SelectBegin(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,const D3DXVECTOR2 Origin2DLeftUp,
		const D3DXVECTOR2 Origin2DRightBottom, DWORD dwOperation);
	HRESULT SelectFrameMove(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,D3DXVECTOR2 Origin2D/*,DWORD dwOperation*/);
	/*virtual*/ HRESULT SelectEnd(D3DVIEWPORT9& Viewport,D3DXMATRIX& Projection,D3DXMATRIX& View);	//不再允许继承，应该用RegisterEventListener的方式得到消息

	/*virtual*/ HRESULT RotateBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction);
	/*virtual*/ HRESULT RotateFrameMove(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction,float XX,float YY,float ZZ);
	/*virtual*/ HRESULT RotateEnd();
	HRESULT RotateDirect(const D3DXQUATERNION& Rot, const D3DXVECTOR3* pRotXYZ);//用Scene派生类的GetTransformTool(KG3DTYPE_ROTATION_TOOL).TransformDirect(XX)来代替

	/*virtual*/ HRESULT MoveBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction);
	/*virtual*/ HRESULT MoveFrameMove(/*KG3DCamera* pCamera,D3DXVECTOR3& Origin,D3DXVECTOR3& Direction,float XX,float YY*/);
	/*virtual*/ HRESULT MoveEnd();
	HRESULT MoveDirect(const D3DXVECTOR3& Trans);

	/*virtual*/ HRESULT ScaleBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction);
	/*virtual*/ HRESULT ScaleFrameMove(KG3DCamera* pCamera,D3DXVECTOR3& Origin,D3DXVECTOR3& Direction,float XX,float YY,float ZZ);
	/*virtual*/ HRESULT ScaleEnd();
	HRESULT ScaleDirect(const D3DXVECTOR3& Scales); 

	const D3DXVECTOR3& GetCenterPos(){return m_SelectionCenter;}

	//下面这些函数之所以变成那么多参数的静态函数，是因为这些都是独立的算法，变成C形式的函数更有利于整理和分析函数之间的相互作用，这样子能发现出参数打包的途径
	//类的结构就会慢慢变清晰
	static VOID InitSelection(KG3DSceneEntityList& listSelected, DWORD dwIncreaseMode);
	static VOID PushSelected(KG3DSceneEntityList& listSelected, DWORD dwIncreaseMode, KG3DSceneEntity& entitySelected);


	static HRESULT PointSelectionPrivate(ULONG uIncreaseMode, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir
		, SelectionInput& selInput, KG3DSceneEntityList& listSelectable, KG3DSceneEntityList& listSelected, BOOL bNearsetOrAll);

	static HRESULT RectSelectionPrivate(ULONG uIncreaseMode, const D3DXVECTOR4& vLeftTopRightBottom , const D3DXMATRIX& matView, const D3DXMATRIX& matProj
		, const D3DXMATRIX& matViewport, FLOAT fSelRange , SelectionInput& selInput, KG3DSceneEntityList& listSelectable, KG3DSceneEntityList& listSelected);
protected:
	virtual void HandleEngineMessages( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );//由MessageReceiver成员接收到消息之后转调,可以覆盖，覆盖的话，记得调用这个基类的同名函数
	
	KG3DScalCoordinate& GetCoordScale(){return m_coordScale;}	
	KG3DRotationCoordinateOld& GetCoordRotate(){return m_coordRotate;}
	KG3DMoveCoordinate& GetCoordMove(){return m_coordMove;}

	HRESULT Init();
	HRESULT UnInit();
public:
	//////////////////////////////////////////////////////////////////////////
	//virtual HRESULT AddSelectableEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer);
	//virtual HRESULT AddSelectableEntity(const KG3DSceneEntity& Entity, BOOL bSelected = FALSE);//是否立刻选中
	//virtual HRESULT RemoveSelectableEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bEitherRemoveInSelectedList);	//在Engine里面少用这个函数啦，这个函数兼具了Entity的Factory的功能，和Entity搅合在一块了。外面暂时没有办法去掉，有空也经过改成直接用Entity做参数。
	//virtual HRESULT RemoveSelectableEntity(const KG3DSceneEntity& Entity, BOOL bEitherRemoveInSelectedEntity = TRUE);//既然不Selectable了，也应该从Selected中移除

	//HRESULT AddSelectedEntity(KG3DSceneEntity Entity);
	//virtual HRESULT RemoveSelectedEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer);

	//virtual HRESULT GetSelectedEntityBBox(AABBOX& Box);//得到选择的物体的包围盒，主要用于缩放Z//代码移动到GetSelectionBBoxGetter
	//virtual HRESULT GetSelectedEntityRotation(D3DXQUATERNION* pRot, D3DXVECTOR3* pRotXYZ);	//没有用到
	//virtual HRESULT RenderSelectedEntityCenter();//<RenderSelectedEntityCenter Refactor>2009-1-15 去掉,合并到RenderSelectedEntityMark

	//所有的不是操作选择的功能，请全部用全局函数的方式（KG3DSceneFunctions）,操作KG3DSceneSelectionTool接口来实现

	//HRESULT SelectNone(DWORD dwClass);	//改成用ClearSelection
	//HRESULT SelectAll(DWORD dwClass);		
	//HRESULT SelectInvert(DWORD dwClass);

	//virtual HRESULT OnToggleStateMove();//<OnToggleStateMove Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
	//virtual HRESULT OnToggleStateRotation();//<OnToggleStateRotation Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
	//virtual HRESULT OnToggleStateScaling();//<OnToggleStateScaling Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
	

    // special for SFX
    //HRESULT ScaleDirectSize(float fSizeScale);//废弃这个函数，改成用KSF::SetSelectionSFXScaleSize

	HRESULT RenderSelectedEntityMark(DWORD dwEditState,D3DVIEWPORT9& Viewport,D3DXMATRIX& Projection,D3DXMATRIX& View,BOOL bShowScreenMask);
	//HRESULT RenderSelectableEntityMark(D3DVIEWPORT9& Viewport,D3DXMATRIX& Projection,D3DXMATRIX& View);
	
	
	// 粗略选择的时候是否只选中一个Entity
	/*BOOL    IsDarkSelectedOneEntity(
		D3DVIEWPORT9& Viewport,
		D3DXMATRIX&   Projection,
		D3DXMATRIX&   View,
		vector<KG3DSceneLayer*>* pvecLayer,
		KG3DSceneEntity* pSelEntity
		);*/	//没有检查到使用

	//HRESULT CopySelectedEntity();//<Copy Refactor>
	//virtual HRESULT ParseCopiedEntity();//2009-2-11 这个函数改动到SceneEditBase的PasteSelection
	//virtual INT GetCopiedEntityCount() { return (INT)m_listClipBoardEntity.size(); }//2009-1-12<Copy Refactor>

	KG3DSceneSelectBase(KG3DScene& scene);
	virtual ~KG3DSceneSelectBase(void);

//(by dengzhihui Refactor 2006年11月24日
	//virtual INT GetSelectedEntitySize();
	//virtual HRESULT GetSelectedEntity(INT nIndex, IEKG3DSceneEntity** ppEntity);//用KG3DSceneSelectionToolAccessProxy来访问成员
	virtual INT GetSelectableEntitySize();
	virtual HRESULT GetSelectableEntity(INT nIndex, IEKG3DSceneEntity** ppEntity);
	//virtual HRESULT RemoveSelectedEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer);
	//virtual HRESULT AddSelectedEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer);
	//virtual HRESULT AddSelectedEntity(IEKG3DSceneEntity* Entity);
	//virtual HRESULT AddSelectableEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer);
	//virtual HRESULT ClearSelectedEntity();
	virtual HRESULT CopySelectable2SelectedSet(LPVOID pBillboard);
	

	//)
};
