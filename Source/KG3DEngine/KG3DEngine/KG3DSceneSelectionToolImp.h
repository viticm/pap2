////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneSelectionToolSolid.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-5-13 15:33:57
//  Comment     : 这个头文件不要暴露出去，外面用IE版本的接口就可以了
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENESELECTIONTOOLIMP_H_
#define _INCLUDE_KG3DSCENESELECTIONTOOLIMP_H_
#include "KG3DCommonObject.h"
#include "KG3DMessageWMGListner.h"
#include "KG3DSceneEntity.h"
#include "KG3DSceneSelectionTool.h"
#include "KG3DFrustum.h"
////////////////////////////////////////////////////////////////////////////////
class KG3DScene;
class KG3DScene;
//class PrivateObjectContainer;
class KG3DRepresentObjectVisitor;

class KG3DSceneSelectionToolSolid : public KG3DCommonObjectSimple
	, public KG3DMessageMSGListner
	, public KG3DSceneSelectionTool
{
public:
	//<KG3DCommonObject>
	STDMETHOD_(ULONG, GetType)();
	STDMETHOD_(LPVOID, ToAnotherInterface)(DWORD_PTR Param);

	STDMETHOD(Render)();
	STDMETHOD(FrameMove)();
	//</KG3DCommonObject>

	//<Messenger>
	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );
	//</Messenger>

	//<SelectionTool>
	virtual HRESULT SetSelectable( SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelectable );//临时方法
	virtual HRESULT SetSelectable(const IEKG3DSceneEntity& Entity, BOOL bSelectable);//如果是非Selectable的话，会自动去掉Selected
	virtual HRESULT SetSelectable(IEKG3DRepresentObject& obj, BOOL bSelectable);

	virtual HRESULT SetSelected(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelected);//临时方法
	virtual HRESULT SetSelected(const IEKG3DSceneEntity& Entity, BOOL bSelected);
	virtual HRESULT SetSelected(IEKG3DRepresentObject& obj, BOOL bSelected);

	virtual HRESULT SelectAll();//全选，物件超过EM_SELECTION_SELECT_ALL_MAX的话，失败，并不会选择。不然对于大场景的话会出问题，大部分3D软件都无法避免Ctrl+A之后变很慢的问题。
	virtual HRESULT SelectInvert();//反选，有物件数限制，同SelectAll

	virtual HRESULT TraverseSelected(KG3DSceneEntityVisitor& Visitor);
	virtual HRESULT TraverseSelected(KG3DRepresentObjectVisitor& Visitor);
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

	virtual HRESULT GetSelectableList(KG3DSceneEntityList& entities);

	virtual TypeEntityAccessProxy& GetSelectionAccessProxy();
	virtual HRESULT GetAccessProxy(IEKG3DSceneSelectionToolAccessProxy** ppAccessProxy);

	virtual HRESULT RegisterEventListener(DWORD messageID, IKG3DMessageReceiverBase& receiver);
	
	virtual HRESULT PointSelection(ULONG uIncreaseMode, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, BOOL bNearestOrAll);//PointSelection必须遵循以下原则：如果选择点找不到，那么找最近的物件的BBOX的碰撞点，不然有些物件会很难选择
	virtual HRESULT RectSelection(ULONG uIncreaseMode, const D3DXVECTOR4& vLeftTopRightBottom
		, const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXMATRIX& matViewport, FLOAT fSelRange);



	virtual ULONG	IsNewSelectionTool(){return TRUE;}

#if defined(DEBUG) | defined(_DEBUG)
	virtual KG3DMessageSender* TestBegin();
	virtual VOID TestEnd();
#endif
	//</SelectionTool>

	static KG3DCommonObject* Build(LPCTSTR lpFile, DWORD_PTR wParam, DWORD_PTR);

	KG3DSceneSelectionToolSolid(KG3DScene& scene);
private:
	~KG3DSceneSelectionToolSolid();//用Release析构

	static DWORD m_ObjectSelectMask; // 对象选择掩码 add by suntao

	static VOID	PushSelected(KG3DSceneSelectionTool& selTool, ULONG uIncreaseMode, const KG3DSceneEntity* pEntity, KG3DRepresentObject* pObject);
	static VOID	InitSelection(KG3DSceneSelectionTool& selTool, ULONG uIncreaseMode);

	//注意两个选择函数，Scene和SelectionTool是分开传的，并没有假定SelectionTool是从属于Scene的，而且这样也符合入参与出参分离的原则
	static VOID	PointSelectionPrivate(KG3DScene& scene, ULONG uIncreaseMode
		, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDir
		, KG3DSceneSelectionToolSolid& selToolToPushResul, BOOL bNearestOrAll);	

	static VOID	RectSelectionPrivate(KG3DScene& scene, ULONG uIncreaseMode
		, const KG3DRay& rayA, const KG3DRay& rayC, FLOAT fSelRange
		, KG3DSceneSelectionToolSolid& selToolToPushResult);

	HRESULT	SetOneSelected(KG3DRepresentObject& obj, BOOL bSelected);

	typedef std::set<KG3DRepresentObject*> TypeObjCt;

	TypeObjCt*	GetSelGroup(LPCTSTR pName);
	struct SelectionData;
private:
	KG3DScene& m_parentScene;	

	typedef std::set<KG3DSceneEntity>	TypeEntityCt;

	TypeEntityCt	m_SelectedEntities;
	TypeEntityCt	m_SelectableEntities;//对于旧的格式，是这样设是否可选的，但是对于新格式，这个应该是一个属性

	//PrivateObjectContainer* m_pObjectContainer;//里面的Object可以随便用，Object析构的时候会自动去除在这里的引用的	
	TypeObjCt m_selectedObjCt;

	typedef std::map<std::tstring, TypeObjCt>	TypeCtList;
	TypeCtList	m_selGroups;

	KGCH::TFrontAccessProxySolid<TypeEntityCt>	m_accessProxy;

	//用于选择的数据
	enum
	{
		em_selection_none = 0,
		em_selection_point = 1,
		em_selection_rect = 2,
	};
	enum
	{
		emPointSelectionOrRectSectionGap = 5,//选择框多大的时候由线选择变框选择
	};
	struct SelectionData 
	{
		RECT		rectSel;
		D3DXMATRIX	matViewProjViewport;
		KG3DRay		selRayA;
		KG3DRay		selRayC;
		FLOAT		fSelRange;
		KG3DFrustum	m_frustum;
		SelectionData();
	};
	SelectionData m_selData;

	INT			m_nSelState;	//用于判断要线选择还是框选择
	ULONG		m_uIncreaseMode;		//默认，加选，还是减选

	KG3DMessageSender	m_Sender;

	BOOL		m_bSelectionExecutionAvailable;	//选择需要隔开时间来做，一个FrameMove只允许一次

	struct CustomProxy : public IEKG3DSceneSelectionToolAccessProxy 
	{
		KG3DSceneSelectionToolSolid& m_selTool;
		
		virtual IEKG3DSceneEntity* Reset()
		{
			m_selTool.m_accessProxy.Begin();
			if(! m_selTool.m_accessProxy.IsEnd())
				return &m_selTool.m_accessProxy.GetCur();
			return NULL;
		}
		virtual IEKG3DSceneEntity* Next()
		{
			if(! m_selTool.m_accessProxy.IsEnd())
			{
				m_selTool.m_accessProxy.StepForward();
				if(! m_selTool.m_accessProxy.IsEnd())
					return &m_selTool.m_accessProxy.GetCur();
			}			
			return NULL;
		}

		CustomProxy(KG3DSceneSelectionToolSolid& tool):m_selTool(tool){}
	};

	CustomProxy m_IEAccessProxy;
};
#endif //_INCLUDE_KG3DSCENESELECTIONTOOL_H_

