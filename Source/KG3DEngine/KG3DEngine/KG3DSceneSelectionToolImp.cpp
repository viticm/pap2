////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneSelectionToolImp.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-5-13 15:34:01
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DSceneSelectionToolImp.h"
#include "KG3DRepresentObject.h"
#include "KG3DScene.h"
//#include "IEKG3DSceneOutputWnd.h"
#include "KG3DIntersection.h"
#include "KG3DSceneBlock.h"
#include "KG3DSceneOutDoorSpaceMgr.h"

#include "KG3DGraphicsFunctions.h"

#include "KG3DCollocator.h"
#include "KG3DSceneFunctions.h"

////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bDesign;

DWORD KG3DSceneSelectionToolSolid::m_ObjectSelectMask = 0xFFFFFFFF; // 初始化对象选择掩码 add by suntao 

//class PrivateObjectContainer : public KG3DSceneEntityContainer 
//{
//	virtual ULONG GetType()const{return 0;}
//	virtual HRESULT ComputeBBox(BOOL bClear) { return E_NOTIMPL; }
//};
TypeEntityAccessProxy& KG3DSceneSelectionToolDummy::GetSelectionAccessProxy()
{
	static KG3DSceneEntityList	s_Instance;
	static KGCH::TFrontAccessProxySolid<KG3DSceneEntityList> s_Proxy(s_Instance);
	return s_Proxy;
}
KG3DSceneSelectionToolSolid::KG3DSceneSelectionToolSolid( KG3DScene& scene )
:m_parentScene(scene)
,m_uIncreaseMode(EM_SELECTION_INCREASE_MODE_DEFAULT)
,m_nSelState(em_selection_none)
//,m_pObjectContainer(NULL)
,m_accessProxy(m_SelectedEntities)
,m_bSelectionExecutionAvailable(FALSE)
,m_IEAccessProxy(*this)
{
	//有的Editor不是SceneSceneEditor但又用到了其中一些东西
	HRESULT hr = scene.RegisterListener(KM_EXECUTE_ACTION, this);
	_ASSERTE(SUCCEEDED(hr));

	
	//m_pObjectContainer = new PrivateObjectContainer;	

	hr = scene.RegisterListener(KM_STATE_CHANGE, this);
	_ASSERTE(SUCCEEDED(hr));
}

KG3DSceneSelectionToolSolid::~KG3DSceneSelectionToolSolid()
{
	//SAFE_DELETE(m_pObjectContainer);
	//SAFE_DELETE(m_pObjectContainer);

	for (TypeCtList::iterator it = m_selGroups.begin(); it != m_selGroups.end(); ++it)
	{
		TypeObjCt& ct = it->second;
		KG_CUSTOM_HELPERS::TContainerRelease(ct);
	}
	m_selGroups.clear();

	ClearSelection();
}

KG3DCommonObject* KG3DSceneSelectionToolSolid::Build( LPCTSTR lpFile, DWORD_PTR wParam, DWORD_PTR )
{
	KG3DSceneSelectionToolSolid* p = NULL;
	KG_PROCESS_ERROR(NULL != wParam);
	p = new KG3DSceneSelectionToolSolid(*reinterpret_cast<KG3DScene*>(wParam));
	KG_PROCESS_ERROR(NULL != p);
	{
		HRESULT hr = p->Init();
		KG_COM_PROCESS_ERROR(hr);
	}
	return p;
Exit0:
	SAFE_RELEASE(p);
	return NULL;
}

LPVOID STDMETHODCALLTYPE KG3DSceneSelectionToolSolid::ToAnotherInterface( DWORD_PTR Param)
{
	IEKG3DSceneSelectionTool* p = this;
	return reinterpret_cast<LPVOID>(p);
}

ULONG STDMETHODCALLTYPE KG3DSceneSelectionToolSolid::GetType()
{
	return KG3DTYPE_SCENE_SELECTION_TOOL;
}

HRESULT STDMETHODCALLTYPE KG3DSceneSelectionToolSolid::Render()
{

	struct EntityVisitor : public KG3DSceneEntityVisitor
	{
		virtual HRESULT Accept(KG3DSceneEntity& Entity)
		{
			return Entity.RenderSelectMark(0xFFFF0000);
		}
	};
	struct ObjVisitor : public KG3DRepresentObjectVisitor
	{
		virtual HRESULT Accept(KG3DRepresentObject& Obj)
		{
			return Obj.RenderSelectMark(0xFFFF0000);
		}
	};

	GraphicsStruct::RenderStateAlpha Apha(0x20,FALSE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

	GraphicsStruct::RenderState RsZ(D3DRS_ZENABLE, D3DZB_TRUE);
	GraphicsStruct::RenderState RsZFunc(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	EntityVisitor entityVisitor;
	ObjVisitor objVisitor;
	TraverseSelected(entityVisitor);
	TraverseSelected(objVisitor);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE KG3DSceneSelectionToolSolid::FrameMove()
{
	m_bSelectionExecutionAvailable = TRUE;

	if(m_parentScene.GetEditState() != SCENESTATE_SELECT)
		return S_OK;

	if (! g_Switch.bNewSelection)
	{
		return S_OK;
	}

	return S_OK;
}
void __stdcall KG3DSceneSelectionToolSolid::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	switch(Message.m_uMessage)
	{
	case KM_STATE_CHANGE:
		{
			DWORD_PTR NewState = Message.m_dwWParam;
			if (NewState != SCENESTATE_SELECT)
			{
				m_nSelState = em_selection_none;
			}
		}
		break;
	case KM_EXECUTE_ACTION:
		{
			if(m_parentScene.GetEditState() != SCENESTATE_SELECT)
				return;

			KG3DMessageActionExecutionStruct* p = reinterpret_cast<KG3DMessageActionExecutionStruct*>(Message.m_dwWParam);
			_ASSERTE(NULL != p);

			if (p->nExecState != 1)
			{
				m_nSelState = em_selection_none;
				m_uIncreaseMode = EM_SELECTION_INCREASE_MODE_DEFAULT;
				break;
			}

			if(p->nActionID < EXEACTION_LEFE_KEY_DOWN && p->nActionID > EXEACTION_SELECTION_SUTRACT)
				break;

			if(p->nActionID == EXEACTION_LEFE_KEY_DOWN)//开始选择或者已经已经在选择中拖动选择框
			{
				m_uIncreaseMode = EM_SELECTION_INCREASE_MODE_DEFAULT;
			}
			else if(p->nActionID == EXEACTION_SELECTION_ADD)
			{
				m_uIncreaseMode = EM_SELECTION_INCREASE_MODE_ADD;	
			}
			else if (p->nActionID == EXEACTION_SELECTION_SUTRACT)
			{
				m_uIncreaseMode = EM_SELECTION_INCREASE_MODE_SUBTRACT;
			}
			else
			{
				m_nSelState = em_selection_none;
				m_uIncreaseMode = EM_SELECTION_INCREASE_MODE_DEFAULT;
				break;
			}

			INT nOldSelState = m_nSelState;
			//得到选择框
			{
				POINT posScreen;
				posScreen.x = LOWORD(p->lParam);
				posScreen.y = HIWORD(p->lParam);

				
				if (em_selection_none == nOldSelState)//还没有开始选择
				{
					//那么开始选择
					m_selData.rectSel.left = posScreen.x;
					m_selData.rectSel.top = posScreen.y;

					m_selData.rectSel.right = posScreen.x;
					m_selData.rectSel.bottom = posScreen.y;
				}
				else	//否则，拉开选择框
				{
					m_selData.rectSel.right = posScreen.x;
					m_selData.rectSel.bottom = posScreen.y;				
				}
			}

			//从选择框求出选择类型
			if (GetRectDiagonalLength(m_selData.rectSel) < 5)
			{
				m_nSelState = em_selection_point;
			}
			else
			{
				m_nSelState = em_selection_rect;
			}

			KG3DSceneOutputWnd* pWnd = m_parentScene.GetPrimaryWindow();
			KG_PROCESS_ERROR(NULL != p);

			//再计算出选择线
			KG3DRay	rayTemp;
			D3DXMATRIX matViewProjViewport;
			
			_ASSERTE (m_nSelState != em_selection_none);
			{
				HRESULT hr = pWnd->GetPickRay(&rayTemp.m_vSrc, &rayTemp.m_vNormalizedDir, NULL);
				KG_COM_PROCESS_ERROR(hr);

				hr = pWnd->GetMatrixViewProjViewport(matViewProjViewport);
				_ASSERTE(SUCCEEDED(hr));
			}

			if (m_nSelState == em_selection_point)
			{				
				
			}
			else	//计算出用于选择的Frustum
			{
				HRESULT hr = ComputeFrustrumBySelectionRect(m_selData.rectSel, *pWnd, m_selData.m_frustum);
				KG_COM_PROCESS_ERROR(hr);
			}

			//根据选择状态，开是第一选择线还是第二选择线
			if (em_selection_none == nOldSelState)//还没有开始选择
			{
				m_selData.selRayA = rayTemp;
				m_selData.selRayC = rayTemp;

				m_selData.matViewProjViewport = matViewProjViewport;
			}
			else
			{
				m_selData.selRayC = rayTemp;
			}

			//得到选择范围
			
			m_selData.fSelRange = EM_RAY_INTERSECTION_MAX_RANGE * 2;
		}
		break;
	case KM_MSG_SENDER_UNREGISTERED:
	case KM_MSG_SENDER_DESTRUCTED:
		{
			//这两个消息只能是SceneEditor发过来的
		}
		break;
	default:
		break;
	}


	if(! g_Switch.bNewSelection)
	{
		m_nSelState = em_selection_none;
		m_uIncreaseMode = EM_SELECTION_INCREASE_MODE_DEFAULT;
	}

	//下面做一个延时，避免频繁选择
#if defined(DEBUG) | defined(_DEBUG)
	BOOL bUseTimeGapBreak = ! TDD::IsTDDWorking();
#else
	BOOL bUseTimeGapBreak = TRUE;
#endif

	if (bUseTimeGapBreak)
	{
		static DWORD s_dwLastTime = 0;
		DWORD dwCurTime = g_cGraphicsTool.GetNowTime();
		if (dwCurTime - s_dwLastTime > EM_SELECTION_MIN_TIME_GAP)
		{
			s_dwLastTime = dwCurTime;
		}
		else
		{
			return;
		}
	}

	if (m_nSelState == em_selection_point)
	{
		PointSelectionPrivate(m_parentScene, m_uIncreaseMode, m_selData.selRayA.m_vSrc, m_selData.selRayA.m_vNormalizedDir, *this, TRUE);
		m_bSelectionExecutionAvailable = FALSE;
	}
	else if (m_nSelState == em_selection_rect)
	{
		RectSelectionPrivate(m_parentScene, m_uIncreaseMode, m_selData.selRayA, m_selData.selRayC, m_selData.fSelRange, *this);
		m_bSelectionExecutionAvailable = FALSE;
	}

	return;
Exit0:
	m_nSelState = em_selection_none;
	return;
}
HRESULT KG3DSceneSelectionToolSolid::SetOneSelected(KG3DRepresentObject& obj, BOOL bSelected)
{
	if (bSelected)
	{
		if (obj.IsSelectable())
		{
			bool bSecceeded = m_selectedObjCt.insert(&obj).second;
			if(bSecceeded)
			{
				obj.AddRef();
				return S_OK;
			}
		}
	}
	else
	{
		TypeObjCt::iterator it = m_selectedObjCt.find(&obj);
		if (it != m_selectedObjCt.end())
		{
			m_selectedObjCt.erase(it);
			KG3DRepresentObject* p = &obj;
			SAFE_RELEASE(p);
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT KG3DSceneSelectionToolSolid::SetSelectable( const IEKG3DSceneEntity& EntityBase, BOOL bSelectable )
{
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DSceneEntity, KG3DSceneEntity);
	const KG3DSceneEntity& Entity = static_cast<const KG3DSceneEntity&>(EntityBase);
	if (bSelectable)
	{
		bool bRet = m_SelectableEntities.insert(Entity).second;
		if (bRet)
		{
			return S_OK;
		}
	}
	else
	{
		m_SelectableEntities.erase(Entity);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneSelectionToolSolid::SetSelectable( SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelectable )
{
	return this->SetSelectable(KG3DSceneEntity(eEntityType, pvEntityPointer), bSelectable);
}

HRESULT KG3DSceneSelectionToolSolid::SetSelectable( IEKG3DRepresentObject& obj, BOOL bSelectable )
{
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DRepresentObject, KG3DRepresentObject);
	return static_cast<KG3DRepresentObject&>(obj).SetSelectable(bSelectable);
}
HRESULT KG3DSceneSelectionToolSolid::SetSelected( const IEKG3DSceneEntity& EntityBase, BOOL bSelected )
{
	const KG3DSceneEntity& Entity = static_cast<const KG3DSceneEntity&>(EntityBase);
	/*
	if (std::find(m_SelectableEntities.begin(), m_SelectableEntities.end(), Entity) 
	!= m_SelectableEntities.end())*/
	if(bSelected)	
	{
		bool bRet = m_SelectedEntities.insert(Entity).second;
		if(bRet)
		{
			return S_OK;
		}	
	}
	else
	{
		m_SelectedEntities.erase(Entity);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneSelectionToolSolid::SetSelected( IEKG3DRepresentObject& obj, BOOL bSelected )
{
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DRepresentObject, KG3DRepresentObject);
	return this->SetOneSelected(static_cast<KG3DRepresentObject&>(obj), bSelected);
}

HRESULT KG3DSceneSelectionToolSolid::SetSelected( SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelected )
{
	KG3DSceneEntity entity(eEntityType, pvEntityPointer);
	return this->SetSelected(entity, bSelected);
}
HRESULT KG3DSceneSelectionToolSolid::TraverseSelected( KG3DSceneEntityVisitor& Visitor )
{
	HRESULT hr = S_OK;
	for (std::set<KG3DSceneEntity>::iterator it = m_SelectedEntities.begin(); it != m_SelectedEntities.end()
		;++it)
	{
		hr = Visitor.Accept(*it);
		KG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return hr;
}

HRESULT KG3DSceneSelectionToolSolid::TraverseSelected( KG3DRepresentObjectVisitor& visitor )
{
	/*if (NULL == m_pObjectContainer)
	{
	return E_FAIL;
	}*/

	HRESULT hr = E_FAIL;
	for (TypeObjCt::iterator it = m_selectedObjCt.begin(); it != m_selectedObjCt.end(); ++it)
	{
		KG3DRepresentObject* p = *it;
		_ASSERTE(NULL != p);
		hr = visitor.Accept(*p);
		KG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return hr;
}

HRESULT KG3DSceneSelectionToolSolid::TraverseSelectable( KG3DSceneEntityVisitor& Visitor )
{
	HRESULT hr = S_OK;
	for (std::set<KG3DSceneEntity>::iterator it = m_SelectableEntities.begin(); it != m_SelectableEntities.end()
		;++it)
	{
		hr = Visitor.Accept(*it);
		KG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return hr;
}

ULONG KG3DSceneSelectionToolSolid::GetSelectionCount()
{
	return static_cast<ULONG>(m_selectedObjCt.size()) + static_cast<ULONG>(m_SelectedEntities.size());
}

ULONG KG3DSceneSelectionToolSolid::IsSelected(const KG3DRepresentObject& Obj )
{
	KG3DRepresentObject* p = const_cast<KG3DRepresentObject*>(&Obj);
	TypeObjCt::iterator it = m_selectedObjCt.find(p);
	if (it != m_selectedObjCt.end())
	{
		return TRUE;
	}
	return FALSE;
}

HRESULT KG3DSceneSelectionToolSolid::ClearSelection()
{
	m_SelectedEntities.clear();
	KG_CUSTOM_HELPERS::TContainerRelease(m_selectedObjCt);
	return S_OK;
}

HRESULT KG3DSceneSelectionToolSolid::ClearSelectable()
{
	m_SelectableEntities.clear();
	return S_OK;
}

HRESULT KG3DSceneSelectionToolSolid::AllocSelGroup(LPCTSTR strName)
{
	TypeObjCt* pCt = NULL;
	KG_PROCESS_ERROR(m_selGroups.size() < EM_SELECTION_GROUP_MAX);
	KG_PROCESS_ERROR(NULL != strName);

	try
	{
		do 
		{
			TypeCtList::_Pairib pib = m_selGroups.insert(TypeCtList::value_type(std::tstring(strName), TypeObjCt()));
			if(! pib.second)
				break;

			return S_OK;
		} while(false);
	}
	catch (...)
	{

	}
Exit0:
	SAFE_DELETE(pCt);
	return E_FAIL;
}

ULONG KG3DSceneSelectionToolSolid::GetSelGroupSize( LPCTSTR pGroupName )
{
	TypeObjCt* p = GetSelGroup(pGroupName);
	KG_PROCESS_ERROR(NULL == p);

	return static_cast<ULONG>(p->size());
Exit0:
	return 0;
}

HRESULT KG3DSceneSelectionToolSolid::ClearSelGroup( LPCTSTR pGroupName )
{
	TypeObjCt* p = GetSelGroup(pGroupName);
	KG_PROCESS_ERROR(NULL == p);
	KG_CUSTOM_HELPERS::TContainerRelease(*p);
	return S_OK;
Exit0:
	return E_FAIL;
}

KG3DSceneSelectionToolSolid::TypeObjCt* KG3DSceneSelectionToolSolid::GetSelGroup( LPCTSTR pName )
{
	KG_PROCESS_ERROR(NULL != pName);

	{
		std::tstring strName(pName);

		TypeCtList::iterator it = m_selGroups.find(strName);
		KG_PROCESS_ERROR(it != m_selGroups.end());

		TypeObjCt& ct = it->second;
		return &ct;
	}
Exit0:
	return NULL;
}

HRESULT KG3DSceneSelectionToolSolid::CopySelGroupToCurSelection( LPCTSTR pGroupName )
{
	TypeObjCt* p = GetSelGroup(pGroupName);
	KG_PROCESS_ERROR(NULL != p);

	{
		KG_CUSTOM_HELPERS::TContainerRelease(m_selectedObjCt);

		m_selectedObjCt = *p;

		for (TypeObjCt::iterator it = m_selectedObjCt.begin(); it != m_selectedObjCt.end(); ++it)
		{
			KG3DRepresentObject* pObj = *it;
			pObj->AddRef();
		}

		return S_OK;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSelectionToolSolid::CopyCurSelectionToSelGroup( LPCTSTR pGroupName )
{
	TypeObjCt* p = GetSelGroup(pGroupName);
	KG_PROCESS_ERROR(NULL != p);

	{
		KG_CUSTOM_HELPERS::TContainerRelease(*p);

		*p = m_selectedObjCt;

		for (TypeObjCt::iterator it = m_selectedObjCt.begin(); it != m_selectedObjCt.end(); ++it)
		{
			KG3DRepresentObject* pObj = *it;
			pObj->AddRef();
		}

		return S_OK;
	}
Exit0:
	return E_FAIL;
}

ULONG KG3DSceneSelectionToolSolid::IsSelectable( const KG3DSceneEntity& entity )
{
	TypeEntityCt::iterator it = m_SelectableEntities.find(entity);
	return it != m_SelectableEntities.end();
}

HRESULT KG3DSceneSelectionToolSolid::CopySelGroupToSelectable( LPCTSTR pGroupName )
{
	_ASSERTE(NULL && _T("没有实现，这种调用是有问题的，需要转化调用"));
	return E_NOTIMPL;
}

HRESULT KG3DSceneSelectionToolSolid::CopySelectableToSelGroup( LPCSTR pGroupName )
{
	_ASSERTE(NULL && _T("没有实现，这种调用是有问题的，需要转化调用"));
	return E_NOTIMPL;
}

HRESULT KG3DSceneSelectionToolSolid::GetSelectableList(KG3DSceneEntityList& entities)
{
	_ASSERTE(NULL && _T("没有实现，这种调用是有问题的，需要转化调用"));
	return E_NOTIMPL;
}

TypeEntityAccessProxy& KG3DSceneSelectionToolSolid::GetSelectionAccessProxy()
{
	return m_accessProxy;
}

HRESULT KG3DSceneSelectionToolSolid::RegisterEventListener( DWORD messageID, IKG3DMessageReceiverBase& receiver )
{
	if(KM_SELECT != messageID)
		return E_FAIL;

	return m_Sender.RegisterReceiver(&receiver);
}

HRESULT KG3DSceneSelectionToolSolid::PointSelection(ULONG uIncreaseMode, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, BOOL bNearestOrAll)
{
	PointSelectionPrivate(m_parentScene, uIncreaseMode, vSrc, vNormalizedDir, *this, bNearestOrAll);
	return S_OK;
}
HRESULT KG3DSceneSelectionToolSolid::RectSelection(ULONG uIncreaseMode, const D3DXVECTOR4& vLeftTopRightBottom
							  , const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXMATRIX& matViewport, FLOAT fSelRange)
{
	KG3DRay rayA, rayC;

	D3DXMATRIX matViewProjViewport = matView * matProj * matViewport;

	D3DXVECTOR2 vSrcA(static_cast<FLOAT>(m_selData.rectSel.left), static_cast<FLOAT>(m_selData.rectSel.top));
	D3DXVECTOR2 vSrcC(static_cast<FLOAT>(m_selData.rectSel.right), static_cast<FLOAT>(m_selData.rectSel.bottom));

	::CalculatePickRay(vSrcA, matViewProjViewport, rayA.m_vSrc, rayA.m_vNormalizedDir);
	::CalculatePickRay(vSrcC, matViewProjViewport, rayC.m_vSrc, rayC.m_vNormalizedDir);

	RectSelectionPrivate(m_parentScene, uIncreaseMode, rayA, rayC, fSelRange, *this);
	return S_OK;
}

KG3DSceneSelectionToolSolid::SelectionData::SelectionData() 
	:fSelRange(EM_RAY_INTERSECTION_MAX_RANGE)
{

}

VOID KG3DSceneSelectionToolSolid::PushSelected(KG3DSceneSelectionTool& selTool, ULONG uIncreaseMode, const KG3DSceneEntity* pEntity, KG3DRepresentObject* pObject )
{
	if (NULL != pObject)
	{
		switch(uIncreaseMode)
		{
		case EM_SELECTION_INCREASE_MODE_DEFAULT:
		case EM_SELECTION_INCREASE_MODE_ADD:
			{
				BOOL bPassFilter = FALSE;
				if (!g_bDesign)
				{
					bPassFilter = TRUE;
				}
				else
				{
					DWORD dwType = pObject->GetType();

					// 加入了一个判断对象类型的掩码 add by suntao
					if ((dwType == REPRESENTOBJECT_NPC ||
						dwType == REPRESENTOBJECT_DOODAD ||
						dwType == REPRESENTOBJECT_WAYPOINT ||
						dwType == REPRESENTOBJECT_POLY ||
						dwType == REPRESENTOBJECT_REFRESHPOINT ||
						dwType == REPRESENTOBJECT_TRAFFICPOINT) &&
						((m_ObjectSelectMask >> dwType) & 0x00000001) 
						)
					{
						bPassFilter = TRUE;
					}
				}


				if (bPassFilter)
					selTool.SetSelected(*pObject, TRUE);
			}
			break;
		case EM_SELECTION_INCREASE_MODE_SUBTRACT:
			{
				selTool.SetSelected(*pObject, FALSE);
			}
			break;
		default:
			break;
		}
	}
	else if(NULL != pEntity)
	{
		switch(uIncreaseMode)
		{
		case EM_SELECTION_INCREASE_MODE_DEFAULT:
		case EM_SELECTION_INCREASE_MODE_ADD:
			{
				selTool.SetSelected(*pEntity, TRUE);
			}
			break;
		case EM_SELECTION_INCREASE_MODE_SUBTRACT:
			{
				selTool.SetSelected(*pEntity, FALSE);
			}
			break;
		default:
			break;
		}

	}
}
VOID KG3DSceneSelectionToolSolid::InitSelection(KG3DSceneSelectionTool& selTool, ULONG uIncreaseMode)
{
	switch(uIncreaseMode)
	{
	case EM_SELECTION_INCREASE_MODE_DEFAULT:
		{
			selTool.ClearSelection();
		}
		break;
	case EM_SELECTION_INCREASE_MODE_ADD:
	case EM_SELECTION_INCREASE_MODE_SUBTRACT:
		{
			NULL;
		}
		break;
	default:
		break;
	}

}
VOID KG3DSceneSelectionToolSolid::PointSelectionPrivate(KG3DScene& scene, ULONG uIncreaseMode
					, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDir
					, KG3DSceneSelectionToolSolid& selToolToPushResult, BOOL bNearestOrAll)
{

	{
		//选择的开始与结束不应该用覆盖虚函数的方式进行，因为这是一个调用时机的问题而不是一个改变行为的问题。为了每个行为覆盖一个虚函数只会令
		//耦合性变强。Listener并不是最好的解决方案，但暂时可以解决问题。
		selToolToPushResult.m_Sender.PostMessage(KG3DMessage(KM_SELECT, NULL, EM_EVENT_STATE_BEGIN));

		InitSelection(selToolToPushResult, uIncreaseMode);

#if 0	//和GetPickRay可以比较一下哪个是正确的，新的Ray是直接从矩阵逆运算得到的，少些操作
		static bool bUseOutputWndRay = false;
		if(bUseOutputWndRay)
		{
			IEKG3DSceneOutputWnd* pCurOutputWnd = NULL;
			HRESULT hr = m_pSceneEditor->GetCurOutputWnd(&pCurOutputWnd);
			KG_COM_PROCESS_ERROR(hr);
			pCurOutputWnd->GetPickRay(&Ray.m_vSrc, &Ray.m_vNormalizedDir, NULL);
		}
#endif

		KG3DRepresentObject* pObject = NULL;
		KG3DSceneEntity* pEntity = NULL;

		//创建一个选择过滤器，用过滤器来限制不在Selectable表里面的东东不被选中，新的地形可能不会这么做
		struct SelectionFilter : public KG3DIntersectionFilter
		{
			KG3DSceneSelectionTool* m_pTool;
			virtual HRESULT Accept(KG3DSceneEntity& entity)
			{
				if (m_pTool->IsSelectable(entity))
				{
					return S_OK;
				}
				return E_FAIL;
			}
		};

		SelectionFilter TempFilter;
		TempFilter.m_pTool = &selToolToPushResult;

		_ASSERTE(bNearestOrAll && _T("暂时没有实现GetAll"));
		BOOL bRet = ::RayIntersection(scene, vSrc
			, vDir, NULL, &pObject, &pEntity, EM_RAY_INTERSECTION_MAX_RANGE, &TempFilter);
		KG_PROCESS_ERROR(bRet);

		//不用管新旧格式，RayIntersection会自己管
		PushSelected(selToolToPushResult, uIncreaseMode, pEntity, pObject);
	}

Exit0:
	selToolToPushResult.m_Sender.PostMessage(KG3DMessage(KM_SELECT, NULL, EM_EVENT_STATE_END));
	return;
}
VOID KG3DSceneSelectionToolSolid::RectSelectionPrivate(KG3DScene& scene, ULONG uIncreaseMode
				   , const KG3DRay& rayA, const KG3DRay& rayC, FLOAT fSelRange
				   , KG3DSceneSelectionToolSolid& selToolToPushResult)
{
	selToolToPushResult.m_Sender.PostMessage(KG3DMessage(KM_SELECT, NULL, EM_EVENT_STATE_BEGIN));
	InitSelection(selToolToPushResult, uIncreaseMode);

	KG3DFrustum frustum;
	HRESULT hr = frustum.CreateBy2Rays(rayA.m_vSrc, rayA.m_vNormalizedDir, rayC.m_vSrc, rayC.m_vNormalizedDir, fSelRange);
	KG_COM_PROCESS_ERROR(hr);

	{
		struct CustomVisitor : public KG3DEntityCtVisitorForObj
		{
			KG3DSceneSelectionTool* m_pTool;
			ULONG		m_uIncreaseMode;
			KG3DFrustum* m_pFrustum;
			virtual HRESULT Accept(KG3DRepresentObject& Obj)
			{
				if(m_pFrustum->IsAABBoxVisible(Obj.m_BBox))
					PushSelected(*m_pTool, m_uIncreaseMode, NULL, &Obj);
				return S_OK;
			}
		};
		CustomVisitor visitor;

		/*旧的选择，不管3721，所有Selectable遍历一次，暂时就这样
		另外，对于选择的裁剪有两种方法，1是在World空间用Frustrum裁剪，另外是在屏幕空间或者裁剪空间裁剪
		前者跳出快，而且只需要3*3*2*6 = 101次乘法，后者要3*3*4*2 = 72次乘法但不能在之间跳出，还有SSE加速
		有空的时候两种都可以试试，不过框选只有编辑器有用，所以不用太在乎这些差别。
		*/


		KG3DSceneOutDoorSpaceMgr* pOutDoor = scene.GetOutDoorSpaceMgr();
		KG_PROCESS_ERROR(NULL != pOutDoor);

		visitor.m_pTool = &selToolToPushResult;
		visitor.m_uIncreaseMode = uIncreaseMode;
		visitor.m_pFrustum = &frustum;

		pOutDoor->Traverse(visitor, frustum);
		selToolToPushResult.m_Sender.PostMessage(KG3DMessage(KM_SELECT, NULL, EM_EVENT_STATE_END));
	}
Exit0:
	return;
}

#if defined(DEBUG) | defined(_DEBUG)
KG3DMessageSender* KG3DSceneSelectionToolSolid::TestBegin()
{
	static KG3DMessageSender s_Sender;
	if (0 == s_Sender.GetReceiverCount())
	{
		bool bRet = s_Sender.RegisterReceiver(this);
		_ASSERTE(bRet);
	}

	_ASSERTE(0 == this->GetSelectionCount()
		&& 0 == m_SelectableEntities.size()
		&& _T("如果当前选择不为空，那么要考虑重新放置测试，测试不能影响功能。但是暂时测试代码无法分离，这个是以代码结构为前提的"));

	return &s_Sender;
}
VOID KG3DSceneSelectionToolSolid::TestEnd()
{
	this->ClearSelection();
	this->ClearSelectable();
}
#endif


HRESULT KG3DSceneSelectionToolSolid::SelectAll()
{
	ULONG uCount = m_parentScene.GetObjectCount(REPRESENTOBJECT_NONE);
	KG_PROCESS_ERROR(uCount <= EM_SELECTION_SELECT_ALL_MAX);
	{
		struct Visitor : public KG3DRepresentObjectVisitor 
		{
			KG3DSceneSelectionTool* m_pSelTool;
			virtual HRESULT Accept(KG3DRepresentObject& obj)
			{
				m_pSelTool->SetSelected(obj, TRUE);
				return S_OK;
			}
		};
		Visitor visitorTemp;
		visitorTemp.m_pSelTool = this;
		m_parentScene.Traverse(visitorTemp);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSelectionToolSolid::SelectInvert()
{
	ULONG uCount = m_parentScene.GetObjectCount(REPRESENTOBJECT_NONE);
	KG_PROCESS_ERROR(uCount <= EM_SELECTION_SELECT_ALL_MAX);
	{
		typedef std::list<KG3DRepresentObject*> TypeTempList;
		struct Visitor : public KG3DRepresentObjectVisitor 
		{
			KG3DSceneSelectionTool* m_pSelTool;
			TypeTempList	m_listObjects;
			virtual HRESULT Accept(KG3DRepresentObject& obj)
			{
				if (! m_pSelTool->IsSelected(obj))
				{
					m_listObjects.push_back(&obj);
				}
				return S_OK;
			}
		};

		Visitor visitorTemp;
		visitorTemp.m_pSelTool = this;
		m_parentScene.Traverse(visitorTemp);

		for(TypeTempList::iterator it = visitorTemp.m_listObjects.begin(); it != visitorTemp.m_listObjects.end(); ++it)
		{
			KG3DRepresentObject* p = *it;
			this->SetSelected(*p, TRUE);
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSelectionToolSolid::GetAccessProxy( IEKG3DSceneSelectionToolAccessProxy** ppAccessProxy )
{
	if (NULL != ppAccessProxy)
	{
		*ppAccessProxy = &m_IEAccessProxy;
		return S_OK;
	}
	return E_FAIL;
}
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DSceneSelectionToolSolid)
{
	//测试消息响应
	{
		
	}
}
KG_TEST_END(KG3DSceneSelectionToolSolid)
#endif