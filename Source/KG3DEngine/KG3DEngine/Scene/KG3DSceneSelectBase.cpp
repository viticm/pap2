#include "StdAfx.h"
#include "kg3dsceneselectbase.h"
#include "kg3dgraphicstool.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DSceneEditorBase.h"

#include "KG3DSceneSFXEditor.h"
#include "KG3DSceneLayer.h"
#include "KG3DSFXParticleLauncher.h"
#include "IEKG3DSFX.h"
#include "MathTool.h"
#include "KG3DEngineManager.h"

#include "KG3DLocalMoveCoord.h"
#include "KG3DLocalRotationCoord.h"
#include "KG3DLocalScalCoord.h"

#include "KG3DCollocator.h"
#include "KG3DSceneFunctions.h"
#include "KG3DSceneObjectTransformTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern BOOL g_bClient;
extern BOOL g_bDesign;
extern KG3DEngineManager g_cEngineManager; 

KG3DSceneSelectBase::KG3DSceneSelectBase(KG3DScene& scene)
:m_accessProxy(m_listSelectedEntity)
,m_coordScale(scene)
,m_coordRotate(scene)
,m_coordMove(scene)
,m_pEngineMsgReceiver(NULL)
,m_Scene(scene)
,m_pSender(NULL)
,m_pIEAccessProxy(NULL)
{
	m_bSelecting = FALSE;

	m_dwIncreaseMode = EM_SELECTION_INCREASE_MODE_DEFAULT;
	m_SelectionCenter = D3DXVECTOR3(0,0,0);

	m_bMoving = FALSE;
	m_bRotating = FALSE;
	m_bScaling = FALSE;

	ZeroMemory(m_TransformTools, _countof(m_TransformTools) * sizeof(KG3DTransformTool*));
}
KG3DSceneSelectBase::~KG3DSceneSelectBase(void)
{
	SAFE_DELETE(m_pEngineMsgReceiver);
	SAFE_DELETE(m_pSender);
	SAFE_DELETE(m_pIEAccessProxy);
	UnInit();
}
void KG3DSceneSelectBase::HandleEngineMessages( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )	//其实这里用状态机的写法会简单些的，暂时就这样了
{
	//<Selecting Refactor>//里面的代码是从各处消息响应的地方过来的

	HRESULT hr = E_FAIL;
	UINT msgID = Message.m_uMessage;

	if (msgID == KM_WINDOWS_MESSAGE)
	{
		KG3DWindowsMessage* pWindowsMessage = reinterpret_cast<KG3DWindowsMessage*>(Message.m_dwWParam);
		_ASSERTE(NULL != pWindowsMessage);
		if(NULL == pWindowsMessage)
			return;
		HandleWindowsMessages(pWindowsMessage->m_uMessage, pWindowsMessage->m_dwWParam, pWindowsMessage->m_dwLParam);
		return;
	}

	if(msgID == KM_STATE_CHANGE)
	{
		DWORD_PTR uNewID = Message.m_dwWParam;
		DWORD_PTR uOldID = Message.m_dwLParam;

		do 
		{
			if (uNewID == uOldID)
				break;

			if(uOldID == SCENESTATE_MOVE)
				MoveEnd();

			if (uNewID == SCENESTATE_MOVE)
				m_bMoving = FALSE;	//<OnToggleStateMove Refactor>

			if(uOldID == SCENESTATE_ROTATE)
				RotateEnd();//<OnToggleStateRotation Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态

			if(uNewID == SCENESTATE_ROTATE)
				m_bRotating = FALSE;

		}while(false);
		return;
	}

	if(msgID != KM_EXECUTE_ACTION)
		return;

	KG3DMessageActionExecutionStruct* pExecActionStruct = reinterpret_cast<KG3DMessageActionExecutionStruct*>(Message.m_dwWParam);
	if(NULL == pExecActionStruct)
		return;

	switch(pExecActionStruct->nActionID)
	{
	case EXEACTION_MOUSE_MOVE:
		{
			if(! m_bSelecting)
				break;

			IEKG3DSceneOutputWnd* pIEOutWnd = NULL;
			hr = m_Scene.GetCurOutputWnd(&pIEOutWnd);
			if(! SUCCEEDED(hr))
				return;
			//MouseMove消息的播放是有延时的，所以不用这里延时
			D3DXVECTOR3 RayOrig,RayDir;
			D3DXVECTOR2 RayOrig2D;
			pIEOutWnd->GetPickRay(&RayOrig,&RayDir,&RayOrig2D);
			SelectFrameMove(RayOrig, RayDir, RayOrig2D);
			LaunchSelect();			
		}
		break;
	case EXEACTION_LEFE_KEY_DOWN://点选键按下
	case EXEACTION_SELECTION_ADD://在当前选择范围中，再增加选择内容
	case EXEACTION_SELECTION_SUTRACT:
		{
			ExecActionClick(pExecActionStruct);
		}
		break;
	default:
		break;
	}
}
void KG3DSceneSelectBase::HandleWindowsMessages( UINT uWindowsMsgID, DWORD_PTR wParam, DWORD_PTR lParam )
{
	if(uWindowsMsgID != WM_LBUTTONDOWN && uWindowsMsgID != WM_LBUTTONUP && uWindowsMsgID != WM_MOUSEMOVE)//WM_LBUTTONDOWN被编辑器截了还是啥回事，在PVS编辑器中不能响应，只能用UP
		return;

	IEKG3DSceneOutputWnd* pOutWnd = NULL;
	HRESULT hr = m_Scene.GetCurOutputWnd(&pOutWnd);
	if(FAILED(hr))
		return;

	switch(m_Scene.GetEditState())
	{
	case SCENESTATE_SELECT:
		{
			if(!m_bSelecting && uWindowsMsgID == WM_LBUTTONDOWN)
			{
				D3DXVECTOR3 RayOrig,RayDir;
				D3DXVECTOR2 RayOrig2D;
				pOutWnd->GetPickRay(&RayOrig,&RayDir,&RayOrig2D);
				if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
					SelectBegin(RayOrig, RayDir, RayOrig2D, EM_SELECTION_INCREASE_MODE_ADD);
				else if (GetAsyncKeyState(VK_LMENU)  & 0x8000)
					SelectBegin(RayOrig, RayDir, RayOrig2D, EM_SELECTION_INCREASE_MODE_SUBTRACT);
				else
					SelectBegin(RayOrig, RayDir, RayOrig2D, EM_SELECTION_INCREASE_MODE_DEFAULT);
			}
			else if (m_bSelecting && uWindowsMsgID == WM_LBUTTONUP)
			{
				D3DXMATRIX Proj,View;

				KG3DSceneOutputWnd* pOutWndSolid = NULL;

				try
				{
					pOutWndSolid = dynamic_cast<KG3DSceneOutputWnd*>(pOutWnd);	//OutWnd没有改造，暂时先这样

					if(NULL != pOutWndSolid)
					{
						Proj = pOutWndSolid->GetCamera().GetProjectionMatrix();
						View = pOutWndSolid->GetCamera().GetViewMatrix();

						SelectEnd(pOutWndSolid->m_Viewport,Proj,View);
					}
				}
				catch (...)
				{
					
				}
			}
			else if(m_bSelecting && uWindowsMsgID == WM_MOUSEMOVE)
			{
				D3DXVECTOR3 RayOrig,RayDir;
				D3DXVECTOR2 RayOrig2D;
				pOutWnd->GetPickRay(&RayOrig,&RayDir,&RayOrig2D);
				SelectFrameMove(RayOrig, RayDir, RayOrig2D);
				LaunchSelect();	
			}
		}
		break;
	case SCENESTATE_MOVE:
		{
			if (uWindowsMsgID == WM_LBUTTONDOWN)
			{
				D3DXVECTOR3 RayOrig,RayDir;
				D3DXVECTOR2 RayOrig2D;
				pOutWnd->GetPickRay(&RayOrig,&RayDir,&RayOrig2D);
				MoveBegin(RayOrig,RayDir);
				MoveFrameMove(/*&m_SceneWndManager->m_lpCurOutputWnd->GetCamera(),RayOrig,RayDir,0,0*/);//<Move Refactor>参数无效
			}
			else if (uWindowsMsgID == WM_LBUTTONUP)
			{
				MoveEnd();
			}
		}
		break;
	case SCENESTATE_ROTATE:
		{
			if (uWindowsMsgID == WM_LBUTTONDOWN)
			{
			D3DXVECTOR3 RayOrig,RayDir;
			D3DXVECTOR2 RayOrig2D;
			pOutWnd->GetPickRay(&RayOrig,&RayDir,&RayOrig2D);
			RotateBegin(RayOrig,RayDir);
			}
			else if (uWindowsMsgID == WM_LBUTTONUP)
			{
				RotateEnd();
			}
		}
		break;
	case SCENESTATE_SCALE:
		{
			if (uWindowsMsgID == WM_LBUTTONDOWN)
			{
				D3DXVECTOR3 RayOrig,RayDir;
				D3DXVECTOR2 RayOrig2D;
				pOutWnd->GetPickRay(&RayOrig,&RayDir,&RayOrig2D);
				ScaleBegin(RayOrig,RayDir);
			}
			else if (uWindowsMsgID == WM_LBUTTONUP)
			{
				ScaleEnd();
			}
		}
		break;
	default:
		break;
	}
}
void KG3DSceneSelectBase::ExecActionClick( KG3DMessageActionExecutionStruct* pExecActionStruct )
{
	HRESULT hr = E_FAIL;

	//得到OutWnd有点麻烦,不过下面用了些非接口方法
	KG3DSceneOutputWnd* pOutWnd = NULL;
	{
		IEKG3DSceneOutputWnd* pIEOutWnd = NULL;
		hr = m_Scene.GetCurOutputWnd(&pIEOutWnd);
		if(! SUCCEEDED(hr))
			return;
		try
		{
			pOutWnd = dynamic_cast<KG3DSceneOutputWnd*>(pIEOutWnd);
			if(NULL == pOutWnd)
				return;
		}
		catch (...)
		{
			return;
		}
	}

	D3DXVECTOR3 RayOrig,RayDir;
	D3DXVECTOR2 RayOrig2D;
	hr = pOutWnd->GetPickRay(&RayOrig,&RayDir, &RayOrig2D);
	if(! SUCCEEDED(hr))
		return;

	ULONG curEditState = m_Scene.GetEditState();

	if(pExecActionStruct->nExecState)
	{
		BOOL bSrtFlag = TRUE;
		if(curEditState == SCENESTATE_MOVE)
		{
			if(m_bMoving || m_bSelecting)
				return;

			MoveBegin(RayOrig,RayDir);     
			bSrtFlag = m_bMoving;

			MoveFrameMove(/*&pOutWnd->GetCamera(),RayOrig,RayDir,0,0*/);//<Move Refactor>参数无效
		}
		else if(curEditState == SCENESTATE_ROTATE)
		{
			if(m_bRotating || m_bSelecting)
				return;

			RotateBegin(RayOrig,RayDir);
			bSrtFlag = m_bRotating;
		}
		else if(curEditState == SCENESTATE_SCALE)
		{
			if(m_bScaling || m_bSelecting)
				return;

			ScaleBegin(RayOrig,RayDir);
			bSrtFlag = m_bScaling;
		}

		if (curEditState == SCENESTATE_SELECT || !bSrtFlag)
		{
			if(m_bSelecting)
				return;

			if (EXEACTION_SELECTION_ADD == pExecActionStruct->nActionID)
				SelectBegin(RayOrig, RayDir, RayOrig2D, EM_SELECTION_INCREASE_MODE_ADD);
			else if (EXEACTION_SELECTION_SUTRACT == pExecActionStruct->nActionID)
				SelectBegin(RayOrig, RayDir, RayOrig2D, EM_SELECTION_INCREASE_MODE_SUBTRACT);
			else
				SelectBegin(RayOrig, RayDir, RayOrig2D, EM_SELECTION_INCREASE_MODE_DEFAULT);
		}	
	}
	else
	{
		BOOL bStrFlag = TRUE;

		if(curEditState==SCENESTATE_MOVE)
		{
			if (m_bMoving)
				MoveEnd();
			bStrFlag = m_bMoving;
		}
		else if(curEditState==SCENESTATE_ROTATE)
		{
			if(m_bRotating)
			{
				RotateEnd();
			}
			bStrFlag = m_bRotating;
		}
		else if(curEditState==SCENESTATE_SCALE)
		{
			if(m_bScaling)
			{
				ScaleEnd();
			}
			bStrFlag = m_bScaling;
		}

		if(curEditState==SCENESTATE_SELECT || !bStrFlag)
		{
			if(m_bSelecting)
			{

				{
					D3DXMATRIX Proj,View;
					Proj = pOutWnd->GetCamera().GetProjectionMatrix();
					View = pOutWnd->GetCamera().GetViewMatrix();

					SelectEnd(pOutWnd->m_Viewport,Proj,View);

					//寒，下面这段好像怎么都没有办法调用上吧，state已经都是Select了
					//if (!bStrFlag && /*m_listSelectedEntity.size()*/ 0 != this->GetSelectionCount())
					//{
					//	switch (curEditState)
					//	{
					//	//case SCENESTATE_MOVE :
					//		//OnToggleStateMove();//<OnToggleStateMove Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
					//	//	return;
					//	//case SCENESTATE_ROTATE :
					//		//	OnToggleStateRotation();//<OnToggleStateRotation Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
					//	//	return;
					//	/*case SCENESTATE_SCALE :
					//	OnToggleStateScaling();//<OnToggleStateScaling Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
					//		return;*/
					//	default :
					//		ASSERT(false);
					//		return;
					//	}
					//}
				}

			}
		}

	}
}


HRESULT KG3DSceneSelectBase::Init()
{
	//if(!g_bClient)
	//{
	//	m_EditMoveGrid.Init();
	//	m_EditRotationGrid.Init();
	//	m_EditScaleGrid.Init();
	//}

	//向Scene注册消息接受，然后把消息返回给自身
	struct CustomReceiver : public KG3DMessageReceiver 
	{
		KG3DSceneSelectBase& m_SelectBase;
		virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
		{	
			m_SelectBase.HandleEngineMessages(Message, pSender);
		}

		CustomReceiver(KG3DSceneSelectBase& sceneSelBase):m_SelectBase(sceneSelBase){}
	};

	//Scene 必须比SelectBase先构造完毕，不然这里会访问到不正确的成员
	m_pEngineMsgReceiver = new CustomReceiver(*this);
	if (NULL != m_pEngineMsgReceiver)
	{
		HRESULT hr = m_Scene.RegisterListener(KM_EXECUTE_ACTION, m_pEngineMsgReceiver);
		_ASSERTE(SUCCEEDED(hr) && _T("注册不成功会造成消息没有响应，然后相应功能就不正常了"));
		hr = m_Scene.RegisterListener(KM_STATE_CHANGE, m_pEngineMsgReceiver);
		_ASSERTE(SUCCEEDED(hr));
		hr = m_Scene.RegisterListener(KM_MOUSE_MOUSE, m_pEngineMsgReceiver);
		_ASSERTE(SUCCEEDED(hr));
		hr = m_Scene.RegisterListener(KM_WINDOWS_MESSAGE, m_pEngineMsgReceiver);
		_ASSERTE(SUCCEEDED(hr));
	}

	_ASSERTE(NULL == m_pSender);
	m_pSender = new KG3DMessageSender;

	struct  CustomProxy : public IEKG3DSceneSelectionToolAccessProxy
	{
		KG3DSceneSelectBase* m_pSelBase;
		virtual IEKG3DSceneEntity* Reset()
		{
			m_pSelBase->m_accessProxy.Begin();
			if(! m_pSelBase->m_accessProxy.IsEnd())
				return &m_pSelBase->m_accessProxy.GetCur();
			return NULL;
		}
		virtual IEKG3DSceneEntity* Next()
		{
			if(! m_pSelBase->m_accessProxy.IsEnd())
			{
				m_pSelBase->m_accessProxy.StepForward();
				if(! m_pSelBase->m_accessProxy.IsEnd())
					return &m_pSelBase->m_accessProxy.GetCur();
			}
			return NULL;
		}
		
		CustomProxy(KG3DSceneSelectBase& selBase):m_pSelBase(&selBase){}
	};

	_ASSERTE(NULL == m_pIEAccessProxy);
	m_pIEAccessProxy = new CustomProxy(*this);

	return S_OK;
}

HRESULT KG3DSceneSelectBase::UnInit()
{
	//m_EditMoveGrid.UnInit();
	//m_EditRotationGrid.UnInit();
	//m_EditScaleGrid.UnInit();

	ClearSelection();
	ClearSelectable();
	//m_listFreezedEntity.clear();
	
	//m_listClipBoardEntity.clear();//<Copy Refactor>
	
	//m_listTempSelectedEntity.clear();//<m_listTempSelectedEntity Refactor>

	for (size_t i = 0; i < _countof(m_TransformTools); ++i)
	{
		SAFE_RELEASE(m_TransformTools[i]);
	}

	return S_OK;
}

#if defined(DEBUG) | defined(_DEBUG)
LPCTSTR s_strTestSelectableGroup = _T("TDDTest");
LPCTSTR s_strTestSelectedGroup = _T("TDDTestSelected");
static size_t s_uSelectableCount = 0;
static size_t s_uSelectedCount = 0;
KG3DMessageSender* KG3DSceneSelectBase::TestBegin()
{
	static KG3DMessageSender s_Sender;
	size_t i = s_Sender.GetReceiverCount();
	if (i == 0)
	{
		bool bRet = s_Sender.RegisterReceiver(m_pEngineMsgReceiver);
		_ASSERTE(bRet);
	}
	
	//要开始测试，要把各个Coord准备好
	this->GetCoordMove().TestSelectCoord(1);
	this->GetCoordRotate().TestSelectCoord(1);
	this->GetCoordScale().TestSelectCoord(1);

	s_uSelectableCount = m_listSelectableEntity.size();
	s_uSelectedCount = m_listSelectedEntity.size();

	this->AllocSelGroup(s_strTestSelectableGroup);
	this->AllocSelGroup(s_strTestSelectedGroup);
	this->CopySelectableToSelGroup(s_strTestSelectableGroup);
	this->CopyCurSelectionToSelGroup(s_strTestSelectedGroup);
	this->ClearSelectable();
	this->ClearSelection();

	_ASSERTE(0 == this->GetSelectionCount() 
		&& 0 == m_listSelectableEntity.size()  
		&& _T("如果当前选择不为空，那么要考虑重新放置测试，测试不能影响功能。但是暂时测试代码无法分离，这个是以代码结构为前提的"));

	return &s_Sender;
}
VOID KG3DSceneSelectBase::TestEnd()
{
	this->GetCoordMove().TestSelectCoord(0xffffffff);
	this->GetCoordRotate().TestSelectCoord(0xffffffff);
	this->GetCoordScale().TestSelectCoord(0xffffffff);

	this->ClearSelection();
	this->ClearSelectable();

	this->CopySelGroupToSelectable(s_strTestSelectableGroup);
	this->CopySelGroupToCurSelection(s_strTestSelectedGroup);

	_ASSERTE(s_uSelectableCount == m_listSelectableEntity.size() 
		&& s_uSelectedCount == this->GetSelectionCount()
		&& _T("测试没有正确的恢复状态"));
}
#endif


//HRESULT KG3DSceneSelectBase::GetSelectedEntityBBox(AABBOX& SelectedEntityBox) //代码移动到GetSelectionBBoxGetter
//{
//	if(m_listSelectedEntity.size()==0)
//		return E_FAIL;
//	SelectedEntityBox.Clear();
//
//	KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
//	while(i!=m_listSelectedEntity.end())
//	{
//		KG3DSceneEntity Entity = *i;
//		AABBOX Box;
//		Entity.GetBBox(&Box);
//		SelectedEntityBox.AddPosition(Box.A);
//		SelectedEntityBox.AddPosition(Box.B);
//		i++;
//	}
//	return S_OK;
//}

//HRESULT KG3DSceneSelectBase::GetSelectedEntityRotation(D3DXQUATERNION* pRot, D3DXVECTOR3* pRotXYZ)
//{
//	if(m_listSelectedEntity.size()==0)
//		return E_FAIL;
//
//	KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
//	while(i!=m_listSelectedEntity.end())
//	{
//		KG3DSceneEntity Entity = *i;
//		Entity.GetRotation(pRot, pRotXYZ);
//		return S_OK;
//
//		i++;
//	}
//
//	return E_FAIL;
//}
//HRESULT KG3DSceneSelectBase::RenderSelectedEntityCenter()//<RenderSelectedEntityCenter Refactor>2009-1-15 去掉,合并到RenderSelectedEntityMark
//{
//	if(m_listSelectedEntity.size()==0)
//		return S_OK;
//	D3DXVECTOR3 X(10,0,0);
//	D3DXVECTOR3 Y(0,10,0);
//	D3DXVECTOR3 Z(0,0,10);
//	g_cGraphicsTool.DrawLine(&(m_SelectionCenter-X),&(m_SelectionCenter+X),0xFF00FF00,0xFF00FF00);
//	g_cGraphicsTool.DrawLine(&(m_SelectionCenter-Y),&(m_SelectionCenter+Y),0xFF00FF00,0xFF00FF00);
//	g_cGraphicsTool.DrawLine(&(m_SelectionCenter-Z),&(m_SelectionCenter+Z),0xFF00FF00,0xFF00FF00);
//
//	return S_OK;
//}

HRESULT KG3DSceneSelectBase::SelectBegin(D3DXVECTOR3 vRaySrc,D3DXVECTOR3 vRayDir,
										 D3DXVECTOR2 vRaySrcPosWindow, DWORD dwOperation)
{
	if(NULL != m_pSender)
		(static_cast<KG3DMessageSender*>(m_pSender))->PostMessage(KG3DMessage(KM_SELECT, NULL, EM_EVENT_STATE_BEGIN));

	m_bSelecting = TRUE;
	m_dwIncreaseMode = dwOperation;

	m_SelInput.vRaySrcA = vRaySrc;
	m_SelInput.vRayDirA = vRayDir;
	m_SelInput.vPosWindowA = vRaySrcPosWindow;	
	m_SelInput.vRaySrcC = vRaySrc;

	m_SelInput.vRaySrcC = vRayDir;
	m_SelInput.vPosWindowC = vRaySrcPosWindow;	
	
	if (m_dwIncreaseMode == EM_SELECTION_INCREASE_MODE_DEFAULT)
	{			
		ClearSelection();
	}
	else if (m_dwIncreaseMode == EM_SELECTION_INCREASE_MODE_ADD)
	{
		NULL;	//什么也不用做
	}
	else 
	{
		NULL;	//什么也不用做
	}

	return S_OK;
}

HRESULT KG3DSceneSelectBase::SelectBegin(D3DXVECTOR3 vRaySrc,D3DXVECTOR3 vRayDir,const D3DXVECTOR2 vPosWindowUpperLeft,
										 const D3DXVECTOR2 vPosWindowBottomRight, DWORD dwOperation)
{
//#if defined(DEBUG) | defined(_DEBUG)
	m_bSelecting = TRUE;
	m_dwIncreaseMode = dwOperation;


	//m_Selection_Origin_A = vRaySrc;
	m_SelInput.vRaySrcA = vRaySrc;

	m_SelInput.vRayDirA = vRayDir;
	m_SelInput.vPosWindowA = vPosWindowUpperLeft;	
	m_SelInput.vRaySrcC = vRaySrc;
	m_SelInput.vRaySrcC = vRayDir;
	m_SelInput.vPosWindowC = vPosWindowUpperLeft;	
	
	//m_listTempSelectedEntity.clear();//<m_listTempSelectedEntity Refactor>


	if (m_dwIncreaseMode == EM_SELECTION_INCREASE_MODE_DEFAULT)
	{
		/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
		KG3DSceneEntityList::iterator iEnd = m_listSelectedEntity.end();
		while (i != iEnd)
		{
			if (i->GetType()==SCENEENTITY_OBJECT)
			{
				KG3DRepresentObject* pObject = 
					static_cast<KG3DRepresentObject*>(i->m_pPointer);
				pObject->m_bSelected = FALSE;
			}
			i++;
		}*/	//去掉Selected
		
		//m_listTempSelectedEntity.clear();//<m_listTempSelectedEntity Refactor>
		m_listSelectableEntity.clear();
	}
	else if (m_dwIncreaseMode == EM_SELECTION_INCREASE_MODE_ADD)
	{
		//m_listTempSelectedEntity = m_listSelectedEntity;//<m_listTempSelectedEntity Refactor>
		NULL;
	}
	else 
	{
		//m_listTempSelectedEntity = m_listSelectedEntity;//<m_listTempSelectedEntity Refactor>
		NULL;
	}
//#else
//NULL
//#endif
	return S_OK;
}

HRESULT KG3DSceneSelectBase::SelectEnd(D3DVIEWPORT9& Viewport,D3DXMATRIX& Projection,D3DXMATRIX& View)
{
	if (g_Switch.bNewSelection)
	{
		return S_OK;
	}

	HRESULT hResult = E_FAIL;

	hResult = GenerateTempSelectedList(Viewport, View, Projection, m_SelInput, m_listSelectableEntity, m_listSelectedEntity, m_dwIncreaseMode);
	//KGLOG_COM_PROCESS_ERROR_CONSOLE(hResult);//不需要检查这个

	//m_listTempSelectedEntity.clear();//<m_listTempSelectedEntity Refactor>

	//重新计算中心
	//GetSelectedEntityCenter(NULL);
	KSF::GetSelectionCenter(*this, m_SelectionCenter);


	m_bSelecting = FALSE;
	
	if(NULL != m_pSender)
		(static_cast<KG3DMessageSender*>(m_pSender))->PostMessage(KG3DMessage(KM_SELECT, NULL, EM_EVENT_STATE_END));

	m_dwIncreaseMode = EM_SELECTION_INCREASE_MODE_DEFAULT;
	hResult = S_OK;


	return hResult;

}

HRESULT KG3DSceneSelectBase::SelectFrameMove(D3DXVECTOR3 vRaySrc,D3DXVECTOR3 vRayDir,
											 D3DXVECTOR2 vPosWindow/*,DWORD dwOperation*/)
{
	m_SelInput.vRaySrcC = vRaySrc;
	m_SelInput.vRaySrcC = vRayDir;
	m_SelInput.vPosWindowC = vPosWindow;	

	return S_OK;
}

//HRESULT KG3DSceneSelectBase::SelectNone(DWORD dwClass)
//{
//	m_listSelectedEntity.clear();
//	return S_OK;
//}

//HRESULT KG3DSceneSelectBase::SelectAll(DWORD dwClass)
//{
//	if (m_listSelectableEntity.size() > EM_SELECTION_SELECT_ALL_MAX)
//	{
//		return E_FAIL;
//	}
//	m_listSelectedEntity.clear();
//	m_listSelectedEntity= m_listSelectableEntity;
//	return S_OK;
//}

HRESULT KG3DSceneSelectBase::SelectAll()
{
	if (m_listSelectableEntity.size() > EM_SELECTION_SELECT_ALL_MAX)
	{
		return E_FAIL;
	}
	m_listSelectedEntity.clear();
	m_listSelectedEntity= m_listSelectableEntity;
	return S_OK;
}
//HRESULT KG3DSceneSelectBase::SelectInvert(DWORD dwClass)
//{
//	KG3DSceneEntityList::iterator i = m_listSelectableEntity.begin();
//	while(i!=m_listSelectableEntity.end())
//	{
//		KG3DSceneEntity Entity = *i;
//
//		KG3DSceneEntityList::iterator j = m_listSelectedEntity.begin();
//		while(j!=m_listSelectedEntity.end())
//		{
//			KG3DSceneEntity EntitySelect = *j;
//			if(EntitySelect==Entity)
//			{
//				m_listSelectedEntity.remove(EntitySelect);
//				break;
//			}
//			j++;
//		}
//		if(j==m_listSelectedEntity.end())
//		{
//			m_listSelectedEntity.push_back(Entity);
//		}
//		i++;
//	}
//	return S_OK;
//}

HRESULT KG3DSceneSelectBase::SelectInvert()
{
	if (m_listSelectableEntity.size() > EM_SELECTION_SELECT_ALL_MAX)
	{
		return E_FAIL;
	}
	KG3DSceneEntityList::iterator i = m_listSelectableEntity.begin();
	while(i!=m_listSelectableEntity.end())
	{
		KG3DSceneEntity Entity = *i;

		KG3DSceneEntityList::iterator j = m_listSelectedEntity.begin();
		while(j!=m_listSelectedEntity.end())
		{
			KG3DSceneEntity EntitySelect = *j;
			if(EntitySelect==Entity)
			{
				m_listSelectedEntity.remove(EntitySelect);
				break;
			}
			++j;
		}
		if(j==m_listSelectedEntity.end())
		{
			m_listSelectedEntity.push_back(Entity);
		}
		++i;
	}
	return S_OK;
}

//BOOL KG3DSceneSelectBase::IsDarkSelectedOneEntity(
//	D3DVIEWPORT9& Viewport, D3DXMATRIX& Projection,
//	D3DXMATRIX&   View,     vector<KG3DSceneLayer*>* pvecLayer,
//	KG3DSceneEntity* pEntity
//)
//{
//	BOOL	bResult  = FALSE;
//	HRESULT hRetCode = E_FAIL;
//
//	KG3DSceneEntityList::iterator iTor = m_listSelectableEntity.begin();
//
//    BOOL bLayerSelectable = FALSE;
//	KG3DSceneEntity Entity;
//	D3DXMATRIX		World;
//	int				nHitCount = 0;
//	D3DXMatrixIdentity(&World);
//
//	while(iTor != m_listSelectableEntity.end())
//	{
//		AABBOX		Box;
//		AABBOX		BoxScreen;
//		D3DXVECTOR3 BoxPos[8];
//
//		Entity = *iTor;
//		iTor++;
//
//		DWORD dwLayerID = Entity.GetSceneLayerID();
//		if (dwLayerID>=pvecLayer->size())
//			continue;
//
//		KG3DSceneLayer* pLayer = (*pvecLayer)[dwLayerID];
//		ASSERT(pLayer);
//		hRetCode = pLayer->GetSelectable(&bLayerSelectable);
//		KGLOG_COM_PROCESS_ERROR(hRetCode);
//
//		if(!bLayerSelectable)
//			continue;
//
//		Entity.GetBBox(&Box);
//		Box.Extract(BoxPos);
//
//		D3DXVec3ProjectArray(BoxPos,sizeof(D3DXVECTOR3),
//			BoxPos,sizeof(D3DXVECTOR3),
//			&Viewport, &Projection,
//			&View, &World,8);
//
//		BoxScreen.AddPosition(BoxPos,8);
//
//		if ((BoxScreen.A.z < 0) || (BoxScreen.B.z > 1.0F))
//			continue;
//
//		float fDistance = PointDistanceLine(
//			&Box.GetCenter(), 
//			&m_SelInput.vRaySrcA,
//			&(m_SelInput.vRaySrcA + m_SelInput.vRayDirA)
//			);
//		float R = D3DXVec3Length(&(Box.A - Box.B)) * 0.58f;
//
//		if (fDistance < R)
//		{
//			nHitCount++;
//
//			if (nHitCount == 1)
//			{
//				if (pEntity)
//					*pEntity = Entity;
//			}
//			else
//			{
//				break;
//			}
//		}	
//
//	}
//
//	if (nHitCount == 1)
//		bResult = TRUE;
//	else
//		bResult = FALSE;
//
//Exit0 :
//	return bResult;
//}

//BOOL EntityIsLogicObj(KG3DSceneEntity* entity)
//{
//	if (entity->GetType() != SCENEENTITY_OBJECT)
//	{
//		return FALSE;
//	}
//
//	KG3DRepresentObject* obj = (KG3DRepresentObject*)entity->m_pPointer;
//	DWORD t = obj->GetType();
//	if (t == REPRESENTOBJECT_NPC ||
//		t == REPRESENTOBJECT_DOODAD ||
//		t == REPRESENTOBJECT_WAYPOINT ||
//		t == REPRESENTOBJECT_POLY ||
//		t == REPRESENTOBJECT_REFRESHPOINT ||
//		t == REPRESENTOBJECT_TRAFFICPOINT
//		)
//	{
//		return TRUE;
//	}
//
//	return FALSE;
//}

BOOL KG3DSceneSelectBase::FindNearestOrFindAll( const D3DXVECTOR2& vUpLeft, const D3DXVECTOR2 vBottomRight )
{
	return D3DXVec2Length(&(vBottomRight - vUpLeft)) < em_max_distance_for_point_selection;
}

HRESULT KG3DSceneSelectBase::PointSelectionPrivate(ULONG uIncreaseMode, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir  
			, SelectionInput& selInput, KG3DSceneEntityList& listSelectable, KG3DSceneEntityList& listSelected, BOOL bNearestOrAll)
{
	KG3DSceneEntity entityNearest;
	//FLOAT fMin = FLT_MAX;
	D3DXVECTOR3 vInter;

	InitSelection(listSelected, uIncreaseMode);

	FLOAT fDis = FLT_MAX;

	//当选不中物件的时候要对最近的物体的BBOX进行检查
	FLOAT fDisForBox = FLT_MAX;
	KG3DSceneEntity entityNearestForBox;

	typedef std::map<FLOAT, KG3DSceneEntity> TypeCtForSelectAll;
	TypeCtForSelectAll ctForSelectAll;	//用于排序选择一串的情况

	for (KG3DSceneEntityList::iterator it = listSelectable.begin(); it != listSelectable.end(); ++it)
	{
		KG3DSceneEntity& entity = *it;

		AABBOX boxTemp;
		HRESULT hrTemp = entity.GetBBox(&boxTemp);
		if (FAILED(hrTemp))
			continue;

		BOOL bIntersectBox = FALSE;
		{
			//当碰撞失败的时候，需要计算出BBOX的碰撞，最后如果碰撞失败，就用BBOX的结果代替，这样对于选择是好事。
			//对于线选择的碰撞是可以这么做的，但如果是摄像机的碰撞就不行
			FLOAT fTemp = FLT_MAX;
			bIntersectBox = boxTemp.RayIntersection(vSrc, vNormalizedDir, &fTemp, NULL)
							/*&& (g_bClient || (! boxTemp.IsPointInside(vSrc)))*/;	//发射点在盒子内部也会有交点，这种情况要排除, 客户端不做这个判断
			if (bIntersectBox && fTemp < fDisForBox)
			{
				_ASSERTE(fTemp > -FLT_EPSILON);
				fDisForBox = fTemp;
				entityNearestForBox = entity;				
			}
		}

		if(! bIntersectBox)
			continue;

		
		FLOAT fTemp = FLT_MAX;
		BOOL bRetTemp = entity.RayIntersectionForEntity(vSrc, vNormalizedDir, &fTemp);
		if (! bRetTemp)
			continue;

		_ASSERTE(fTemp > -FLT_EPSILON);
		if (bNearestOrAll)
		{
			if (fTemp < fDis)
			{
				fDis = fTemp;
				entityNearest = entity;
			}
		}
		else	//如果不是寻找最近的，那么全部放入vecSelectAllSortCt中排序
		{
			if (fTemp < fDis)
				fDis = fTemp;
			if (EntityIsSelectable(listSelectable, selInput, entity))
			{
				TypeCtForSelectAll::_Pairib pib = ctForSelectAll.insert(std::make_pair(fTemp, entity));	//map的插入会自动排序
				if(ctForSelectAll.size() > EM_SELECTION_POINT_SELECT_ALL_MAX)	//如果已经够多了，删除掉后面的
				{
					if(pib.second)
					{
						_ASSERTE(pib.first != ctForSelectAll.end());
						TypeCtForSelectAll::iterator itErase = pib.first;
						++itErase;
						ctForSelectAll.erase(itErase);
					}
				}
			}			
		}		
	}

	if (fDis < FLT_MAX)		//选择到有东西的时候
	{
		if (bNearestOrAll)
		{
			_ASSERTE(fDis < FLT_MAX && entityNearest.m_pPointer);
			if (EntityIsSelectable(listSelectable, selInput, entityNearest))
			{
				PushSelected(listSelected, uIncreaseMode, entityNearest);
			}
		}
		else	//如果是选择一串的，这个时候就已经排好序的了
		{
#if defined(DEBUG) | defined(_DEBUG)
			if (ctForSelectAll.size() > 1)
			{
				_ASSERTE(ctForSelectAll.begin()->first < (--ctForSelectAll.end())->first);
			}
#endif
			for (TypeCtForSelectAll::iterator it = ctForSelectAll.begin(), itend = ctForSelectAll.end(); it != itend; ++it)
			{
				PushSelected(listSelected, uIncreaseMode, it->second);
			}
		}
	}
	else if (fDisForBox < FLT_MAX)//选择不到的时候，对Box进行检查 //如果没有找到，那么无论是找最近的还是找全部一串，都算选择
	{
		if(g_eEngineOption.bUseLogical)
		{
			_ASSERTE(entityNearestForBox.m_pPointer);
			if (EntityIsSelectable(listSelectable, selInput, entityNearestForBox))
			{
				PushSelected(listSelected, uIncreaseMode, entityNearestForBox);
			}
		}
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}
HRESULT KG3DSceneSelectBase::PointSelection(ULONG uIncreaseMode, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, BOOL bNearestOrAll)
{
	return PointSelectionPrivate(uIncreaseMode, vSrc, vNormalizedDir, m_SelInput, m_listSelectableEntity, m_listSelectedEntity, bNearestOrAll);	
}
HRESULT KG3DSceneSelectBase::RectSelectionPrivate(ULONG uIncreaseMode, const D3DXVECTOR4& vLeftTopRightBottom
										   , const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXMATRIX& matViewport, FLOAT fSelRange
										   , SelectionInput& selInput, KG3DSceneEntityList& listSelectable, KG3DSceneEntityList& listSelected)
{
	InitSelection(listSelected, uIncreaseMode);

	D3DXMATRIX matViewProjView = matView * matProj * matViewport;

	
	
	D3DXVECTOR3 vBoxPoints[8];

	FLOAT fMaxX = max(vLeftTopRightBottom.x, vLeftTopRightBottom.z);
	FLOAT fMinX = min(vLeftTopRightBottom.x, vLeftTopRightBottom.z);
	FLOAT fMaxY = max(vLeftTopRightBottom.y, vLeftTopRightBottom.w);
	FLOAT fMinY = min(vLeftTopRightBottom.y, vLeftTopRightBottom.w);

	for (KG3DSceneEntityList::iterator it = listSelectable.begin(); it != listSelectable.end(); ++it)
	{
		KG3DSceneEntity& entity = *it;

		AABBOX bbBox;
		entity.GetBBox(&bbBox);
		bbBox.Extract(vBoxPoints);


		D3DXVec3TransformCoordArray(vBoxPoints, sizeof(D3DXVECTOR3), vBoxPoints, sizeof(D3DXVECTOR3), &matViewProjView, _countof(vBoxPoints));

		AABBOX bbBoxScreen;
		bbBoxScreen.AddPosition(vBoxPoints,8);

		if (bbBoxScreen.A.z < 0 || bbBoxScreen.B.z > 1.0F)
			continue;

		if((fMinX < bbBoxScreen.B.x) && (bbBoxScreen.A.x < fMaxX) &&
			(fMinY < bbBoxScreen.B.y) && (bbBoxScreen.A.y < fMaxY))
		{
			if(EntityIsSelectable(listSelectable, selInput, entity))
			{
				PushSelected(listSelected, uIncreaseMode, entity);
			}
		}

	}
	return S_OK;
}

HRESULT KG3DSceneSelectBase::RectSelection(ULONG uIncreaseMode, const D3DXVECTOR4& vLeftTopRightBottom
										   , const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXMATRIX& matViewport, FLOAT fSelRange)
{
	return RectSelectionPrivate(uIncreaseMode, vLeftTopRightBottom, matView, matProj, matViewport, fSelRange, m_SelInput, m_listSelectableEntity, m_listSelectedEntity);
}
HRESULT KG3DSceneSelectBase::GenerateTempSelectedList(const D3DVIEWPORT9& viewPort, const D3DXMATRIX& matView, const D3DXMATRIX& matProj,SelectionInput& selInput
													  , KG3DSceneEntityList& listSelectable, KG3DSceneEntityList& listSelected, DWORD dwIncreaseMode)
{
//<SELECTION_TOOL>//不要删除这一段，光靠这一段就可以禁止掉SelectBase的运作，很方便的

#if 0
	return S_OK;
#endif

	if(g_Switch.bNewSelection)
		return S_OK;

	BOOL	bFindNearest = FindNearestOrFindAll(selInput.vPosWindowA, selInput.vPosWindowC);

	if (bFindNearest)
	{
		PointSelectionPrivate(dwIncreaseMode, selInput.vRaySrcA, selInput.vRayDirA, selInput, listSelectable, listSelected, TRUE);
		return S_OK;
	}
	
	//这里是在搞啥？
	D3DXVECTOR4 vPosRect(selInput.vPosWindowA.x, selInput.vPosWindowA.y, selInput.vPosWindowC.x, selInput.vPosWindowC.y);

	D3DXMATRIX matViewport;
	D3DXMatrixViewPort(viewPort, matViewport);

	RectSelectionPrivate(dwIncreaseMode, vPosRect, matView, matProj, matViewport, EM_RAY_INTERSECTION_MAX_RANGE * 2, selInput, listSelectable, listSelected);
	return S_OK;

	//下面是旧的代码
//#if 0
//	return S_OK;
//#endif
//
//	if(g_Switch.bNewSelection)
//		return S_OK;
//
//	HRESULT hResult = E_FAIL;
//    //HRESULT hRetCode = E_FAIL;
//
//	static std::vector<KG3DSceneEntity> vecEntitysForDarkSel;
//	vecEntitysForDarkSel.clear();
//
//	KG3DSceneEntityList	listCurrent;
//	D3DXVECTOR3 Inter;
//	KG3DSceneEntity Entity;
//	KG3DSceneEntity NearestEntity;
//
//	BOOL	bFindNearest = FindNearestOrFindAll(selInput.vPosWindowA, selInput.vPosWindowC);
//
//    float fMin = 1000000000.0f;
//
//	KG3DSceneEntityList::iterator i = listSelectable.begin();
//
//	D3DXMATRIX matViewProjView;
//	{
//		D3DXMATRIX matViewport;
//		D3DXMatrixViewPort(viewPort, matViewport);
//		matViewProjView = matView * matProj * matViewport;
//	}
//
//	{
//		while(i != listSelectable.end())
//		{
//			AABBOX Box;
//			AABBOX BoxScreen;
//			D3DXVECTOR3 BoxPos[8];
//
//			Entity = *i;
//			i++;
//
//			Entity.GetBBox(&Box);
//			Box.Extract(BoxPos);
//
//	
//			D3DXVec3TransformCoordArray(BoxPos, sizeof(D3DXVECTOR3), BoxPos, sizeof(D3DXVECTOR3), &matViewProjView, _countof(BoxPos));
//
//			BoxScreen.AddPosition(BoxPos,8);
//			
//
//			if ((BoxScreen.A.z < 0) || (BoxScreen.B.z > 1.0F))
//				continue;
//
//			
//			if (bFindNearest)
//			{
//				if (Entity.IsRayIntersect(Inter, selInput.vRaySrcA, selInput.vRayDirA, &vecEntitysForDarkSel))
//				{
//					D3DXVECTOR3 V1 = Inter - selInput.vRaySrcA;
//					float fDis = D3DXVec3Length(&V1);
//
//					if (fDis < fMin)
//					{
//						NearestEntity = Entity;
//						fMin = fDis;
//					}
//				}
//			}
//			else
//			{
//				float AX = selInput.vPosWindowA.x;
//				float AY = selInput.vPosWindowA.y;		
//				float CX = selInput.vPosWindowC.x;
//				float CY = selInput.vPosWindowC.y;
//				float fTemp;
//
//				if (AX > CX)
//				{
//					fTemp = CX;
//					CX = AX;
//					AX = fTemp;
//				}
//
//				if (AY > CY)
//				{
//					fTemp = CY;
//					CY = AY;
//					AY = fTemp;
//				}
//
//				if((AX<BoxScreen.B.x)&&(CX>BoxScreen.A.x)&&
//					(AY<BoxScreen.B.y)&&(CY>BoxScreen.A.y))
//				{
//					if(EntityIsSelectable(listSelectable, selInput, Entity))//if ( EntityIsLogicObj(&Entity) || !g_bDesign) // 策划场景编辑器选择过滤
//						listCurrent.push_back(Entity);
//				}
//			}
//		}
//	}
// 
//	if (bFindNearest && !NearestEntity.m_pPointer && vecEntitysForDarkSel.size())
//	{
//		D3DXVECTOR3 vPointA = selInput.vRaySrcA;
//		D3DXVECTOR3 vPointB = selInput.vRaySrcA + selInput.vRayDirA * 100000000.f;
//
//		float fDis2BBoxCenter = 100000000.f;
//		for (size_t i = 0; i < vecEntitysForDarkSel.size(); i++)
//		{
//			D3DXVECTOR3 vInsert;
//			AABBOX		Box;
//			D3DXVECTOR3 BoxPos[8];
//
//			vecEntitysForDarkSel[i].GetBBox(&Box);
//			
//			float fDistance = PointDistanceLine(&Box.GetCenter(), &vPointA, &vPointB);
//
//			if (fDistance < fDis2BBoxCenter)
//			{
//				NearestEntity   = vecEntitysForDarkSel[i];
//				fDis2BBoxCenter = fDistance;
//			}
//		}
//	}
//
//
//	if (bFindNearest && NearestEntity.m_pPointer)
//	{
//		ASSERT(listCurrent.size() == 0);
//		if(EntityIsSelectable(listSelectable, selInput, Entity))//if (!g_bDesign || EntityIsLogicObj(&NearestEntity) ) // 策划场景编辑器选择过滤
//			listCurrent.push_back(NearestEntity);
//	}
//
//	switch (dwIncreaseMode)
//	{
//	case EM_SELECTION_INCREASE_MODE_DEFAULT:
//		{
//			listSelected.clear();
//			listSelected = listCurrent;
//		}
//		break;
//	case EM_SELECTION_INCREASE_MODE_ADD:
//		{
//			//这里查了一次重复。list是线性查找的，所以可能比较慢。
//			for (KG3DSceneEntityList::iterator it = listCurrent.begin(); it != listCurrent.end(); ++it)
//			{
//				KG3DSceneEntity& entity = *it;
//				KG3DSceneEntityList::iterator itFound = std::find(listSelected.begin(), listSelected.end(), entity);
//				if (itFound == listSelected.end())
//				{
//					listSelected.push_back(entity);
//				}
//			}
//		}
//		break;
//	case EM_SELECTION_INCREASE_MODE_SUBTRACT:
//		{
//			i = listCurrent.begin();
//			while (i != listCurrent.end())
//			{
//				Entity = *i;
//				listSelected.remove(Entity);
//				i++;
//			}
//		}
//		break;
//	default:
//		break;
//	}
//
//	hResult = S_OK;
//
//	return hResult;


	
}

VOID KG3DSceneSelectBase::InitSelection( KG3DSceneEntityList& listSelected, DWORD dwIncreaseMode )
{
	if (dwIncreaseMode == EM_SELECTION_INCREASE_MODE_DEFAULT)
	{
		listSelected.clear();
	}
}

VOID KG3DSceneSelectBase::PushSelected( KG3DSceneEntityList& listSelected, DWORD dwIncreaseMode, KG3DSceneEntity& entitySelected )
{
	if (dwIncreaseMode == EM_SELECTION_INCREASE_MODE_DEFAULT || dwIncreaseMode == EM_SELECTION_INCREASE_MODE_ADD)
	{
		KG3DSceneEntityList::iterator itFound = std::find(listSelected.begin(), listSelected.end(), entitySelected);
		if (itFound == listSelected.end())
		{
			listSelected.push_back(entitySelected);
			entitySelected.OnSelect();
		}
	}
	else if (dwIncreaseMode == EM_SELECTION_INCREASE_MODE_SUBTRACT)
	{
		listSelected.remove(entitySelected);
	}
}
//这个函数没有用，只是生成选择而已
//HRESULT KG3DSceneSelectBase::RenderSelectableEntityMark(D3DVIEWPORT9& Viewport,D3DXMATRIX& Projection,D3DXMATRIX& View)
//{
//	DWORD dwZWrite=0;
//	HRESULT hResult = E_FAIL;
//
//	g_pd3dDevice->GetRenderState(D3DRS_ZWRITEENABLE,&dwZWrite);
//	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
//
//	if (m_bSelecting && !g_bDesign)
//	{
////#if defined(DEBUG) | defined(_DEBUG)
////		NULL;
////#else
//		hResult = GenerateTempSelectedList(Viewport,Projection,View, m_SelInput, m_listSelectableEntity, m_listSelectedEntity, m_dwIncreaseMode);
//		KGLOG_COM_PROCESS_ERROR_CONSOLE(hResult);
////		NULL
////#endif
//	}
//
//	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
//
//	hResult = S_OK;
//Exit0:
//	return hResult;
//}

HRESULT KG3DSceneSelectBase::RenderSelectedEntityMark(DWORD dwEditState,D3DVIEWPORT9& Viewport,D3DXMATRIX& Projection,D3DXMATRIX& View,BOOL bShowScreenMask)
{
	if(0 != GetSelectionCount())
	{
		D3DXVECTOR3 X(10,0,0);
		D3DXVECTOR3 Y(0,10,0);
		D3DXVECTOR3 Z(0,0,10);
		g_cGraphicsTool.DrawLine(&(m_SelectionCenter-X),&(m_SelectionCenter+X),0xFF00FF00,0xFF00FF00);
		g_cGraphicsTool.DrawLine(&(m_SelectionCenter-Y),&(m_SelectionCenter+Y),0xFF00FF00,0xFF00FF00);
		g_cGraphicsTool.DrawLine(&(m_SelectionCenter-Z),&(m_SelectionCenter+Z),0xFF00FF00,0xFF00FF00);
	}

	GraphicsStruct::RenderState zenable(D3DRS_ZENABLE,FALSE);
	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE,0xF);
	if (((m_bSelecting)&&(bShowScreenMask)))
	{
		GraphicsStruct::RenderState zWrite(D3DRS_ZWRITEENABLE,FALSE);
		GraphicsStruct::RenderStateAlpha Apha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

		g_pd3dDevice->SetTexture(0, NULL);
		g_cGraphicsTool.DrawScreenRect(&m_SelInput.vPosWindowA,&m_SelInput.vPosWindowC,0,0x30FFFFFF);

	}

	//RenderSelectableEntityMark(Viewport,Projection,View);//这个函数没有用，只是生成选择而已，这个可以移动到SelectFrameMove的附近

	//<m_listTempSelectedEntity Refactor>
	//if(m_bSelecting && dwEditState == SCENESTATE_SELECT)
	//{
	//	GraphicsStruct::RenderStateAlpha Apha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	//	KG3DSceneEntityList::iterator i = m_listTempSelectedEntity.begin();
	//	while (i != m_listTempSelectedEntity.end())
	//	{
	//		KG3DSceneEntity Entity = *i;
	//		Entity.RenderSelectMark(0x8800FF00);
	//		i++;
	//	}
	//}
	//else if (dwEditState == SCENESTATE_SELECT)
	//{
	//	GraphicsStruct::RenderStateAlpha Apha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	//	KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	//	while(i != m_listSelectedEntity.end())
	//	{
	//		KG3DSceneEntity Entity = *i;
	//		Entity.RenderSelectMark(g_bDesign ? 0x8800FF00 : 0x88FF0000); // 策划场景编辑器 选中后也是绿色，只有king是红色
	//		i++;
	//	}
	//}

	{
		GraphicsStruct::RenderStateAlpha Apha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
		D3DCOLOR colorForSelectMark = g_bDesign ? 0x8800FF00 : 0x88FF0000;// 策划场景编辑器 选中后也是绿色，只有king是红色
		for (KG3DSceneEntityList::iterator it = m_listSelectedEntity.begin(); it != m_listSelectedEntity.end(); ++it)
		{
			KG3DSceneEntity& entity = *it;
			entity.RenderSelectMark(colorForSelectMark);
		}
	}
	


	if (g_bDesign && m_listSelectedEntity.size() > 0) // 策划场景编辑器 king 的选择框红色
	{
		GraphicsStruct::RenderStateAlpha Apha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

		//m_SelectedKingEntity.RenderSelectMark(0x88FF0000);//这个就是最后一个选择的，用选择器得到就可以了
		KG3DSceneEntity& entity = this->GetSelectionAccessProxy().GetBack();
		entity.RenderSelectMark(0x88FF0000);

		//GetSelectedEntityCenter(NULL);
		//重新计算中心
		KSF::GetSelectionCenter(*this, m_SelectionCenter);
	}


	DWORD dwLight;
	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &dwLight);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	BOOL LightEnable[8];
	for(int i=0;i<8;i++)
	{
		g_pd3dDevice->GetLightEnable(i,&LightEnable[i]);
		g_pd3dDevice->LightEnable(i,FALSE);
	}

	switch(dwEditState)
	{
	case SCENESTATE_MOVE:
		this->GetCoordMove().Render();
		break;
	case SCENESTATE_ROTATE:
		this->GetCoordRotate().Render();
		break;
	case SCENESTATE_SCALE:
		this->GetCoordScale().Render();
		break;
	}
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLight);
	for(int i=0;i<8;i++)
	{
		g_pd3dDevice->LightEnable(i,LightEnable[i]);
	}
	return S_OK;
}

//HRESULT KG3DSceneSelectBase::OnToggleStateMove()
//{
//	m_bMoving = FALSE;
//	return S_OK;
//}

//HRESULT KG3DSceneSelectBase::OnToggleStateRotation()
//{
//    //this->GetCoordRotate().AttachEntity(dynamic_cast<KG3DScene*>(this), m_listSelectedEntity);
//    m_bRotating = FALSE;
//
//    return S_OK;
//}

HRESULT KG3DSceneSelectBase::MoveBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction)
{
    if (SUCCEEDED(this->GetCoordMove().MoveBegin()))
        m_bMoving = TRUE;
    else
        m_bMoving = FALSE;
    return S_OK;
	//if(m_listSelectedEntity.size()==0)
	//	return S_OK;	
	//
	//if(SUCCEEDED(m_EditMoveGrid.CheckState(Origin,Direction)))
	//{
	//	m_bMoving = TRUE;
	//	m_EditMoveGrid.ComputeMovePlane(Origin,Direction);

	//	//////////////////////////////////////////////////////////////////////////
	//	m_listMovingEntityPositionStart.clear();
	//	KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	//	while(i!=m_listSelectedEntity.end())
	//	{
	//		KG3DSceneEntity &Entity = *i;

	//		D3DXVECTOR3 vPos(0,0,0);
	//		Entity.GetTranslation(&vPos);
	//		m_listMovingEntityPositionStart.push_back(vPos);

	//		i++;
	//	}

	//	return S_OK;
	//}
	//return E_FAIL;
}

HRESULT KG3DSceneSelectBase::MoveDirect(const D3DXVECTOR3& Trans)
{
	D3DXVECTOR3 A(0,0,0);
	D3DXVECTOR3 B(0,0,0);

	MoveBegin(A,B);

	KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while(i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity &Entity = *i;
		D3DXVECTOR3 Pos;
		Entity.GetTranslation(&Pos);
		Entity.SetTranslation(&(Pos+Trans));
		Entity.UpdateTransformation();
		++i;
	}
	MoveEnd();
	return S_OK;
}

HRESULT KG3DSceneSelectBase::MoveFrameMove(/*KG3DCamera* pCamera,D3DXVECTOR3& Origin,D3DXVECTOR3& Direction,float XX,float YY*/)
{
    this->GetCoordMove().FrameMove();
	if(! g_Switch.bNewSelection)
	{
		(static_cast<KG3DMessageSender*>(m_pSender))->PostMessage(KG3DMessage(KM_TRANSLATION, NULL, EM_EVENT_STATE_FRAMEMOVE));
	}
	return S_OK;
}

HRESULT KG3DSceneSelectBase::MoveEnd()
{
    this->GetCoordMove().MoveEnd();
	m_bMoving = FALSE;

	if(! g_Switch.bNewSelection)
	{
		(static_cast<KG3DMessageSender*>(m_pSender))->PostMessage(KG3DMessage(KM_TRANSLATION, NULL, EM_EVENT_STATE_END));
	}

	return S_OK;
}

HRESULT KG3DSceneSelectBase::RotateBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction)
{
    if (SUCCEEDED(this->GetCoordRotate().RotateBegin()))
        m_bRotating = TRUE;
    else
        m_bRotating = FALSE;
    return S_OK;
}

HRESULT KG3DSceneSelectBase::RotateFrameMove(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction,float XX,float YY,float ZZ)
{
    this->GetCoordRotate().FrameMove();
    return S_OK;
}

HRESULT KG3DSceneSelectBase::RotateEnd()
{
    this->GetCoordRotate().RotateEnd();
	m_bRotating = FALSE;

	if(! g_Switch.bNewSelection)
	{
		(static_cast<KG3DMessageSender*>(m_pSender))->PostMessage(KG3DMessage(KM_ROTATION, NULL, EM_EVENT_STATE_END));
	}

	return S_OK;
}

HRESULT KG3DSceneSelectBase::RotateDirect(const D3DXQUATERNION& Rot, const D3DXVECTOR3 *pRotXYZ)
{
	KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while(i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity &Entity = *i;
		D3DXQUATERNION Rotaion;
        D3DXVECTOR3 vRotXYZ;
		Entity.GetRotation(&Rotaion, &vRotXYZ);
		Rotaion *= Rot;
        vRotXYZ.x += pRotXYZ->x;
        vRotXYZ.y += pRotXYZ->y;
        vRotXYZ.z += pRotXYZ->z;
		D3DXQuaternionNormalize(&Rotaion,&Rotaion);
		Entity.SetRotation(&Rotaion, &vRotXYZ);
		Entity.UpdateTransformation();
		++i;
	}

	RotateEnd();

	return S_OK;
}

//HRESULT KG3DSceneSelectBase::OnToggleStateScaling()
//{
//    //this->GetCoordScale().AttachEntity(dynamic_cast<KG3DScene*>(this), m_listSelectedEntity);
//	m_bScaling = FALSE;
//	return S_OK;
//}

HRESULT KG3DSceneSelectBase::ScaleBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction)
{
    if (SUCCEEDED(this->GetCoordScale().ScalBegin()))
        m_bScaling = TRUE;
    else
        m_bScaling = FALSE;
    return S_OK;
}

HRESULT KG3DSceneSelectBase::ScaleFrameMove(KG3DCamera* pCamera,D3DXVECTOR3& Origin,D3DXVECTOR3& Direction,float XX,float YY,float ZZ)
{
     this->GetCoordScale().FrameMove();
	return S_OK;
}

HRESULT KG3DSceneSelectBase::ScaleEnd()
{
    this->GetCoordScale().ScalEnd();
	m_bScaling = FALSE;

	if(! g_Switch.bNewSelection)
	{
		(static_cast<KG3DMessageSender*>(m_pSender))->PostMessage(KG3DMessage(KM_SCALING, NULL, EM_EVENT_STATE_END));
	}

	return S_OK;
}

HRESULT KG3DSceneSelectBase::ScaleDirect(const D3DXVECTOR3& Scales)
{
	KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while(i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity &Entity = *i;
		D3DXVECTOR3 S;
		Entity.GetScaling(&S);
		S.x*=Scales.x;
		S.y*=Scales.y;
		S.z*=Scales.z;
		Entity.SetScaling(&S);
		Entity.UpdateTransformation();
		++i;
	}
	ScaleEnd();
	return S_OK;
}

//HRESULT KG3DSceneSelectBase::ScaleDirectSize(float fSizeScale)//2009-2-10移动到KSF::SetSelectionSFXScaleSize
//{
//    KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
//    while (i != m_listSelectedEntity.end())
//    {
//        KG3DSceneEntity &Entity = *i;
//        float fValue;
//        Entity.GetScaleSize(&fValue);
//        fValue *= fSizeScale;
//        Entity.SetScaleSize(fValue);
//        i++;
//    }
//
//    return S_OK;
//}
//2009-1-12 <Copy Refactor>复制改成3dsmax的方式，不需要用Ctrl+C记录选择，而是直接用Ctrl+V就复制出当前的选择。记录选择这个操作需要考虑太多物件相关性的问题，而且也应该尽量向Max的操作看齐
//HRESULT KG3DSceneSelectBase::CopySelectedEntity()
//{
//	m_listClipBoardEntity.clear();
//	m_listClipBoardEntity = m_listSelectedEntity;
//	return S_OK;
//}

//INT KG3DSceneSelectBase::GetSelectedEntitySize()
//{
//	return (INT)m_listSelectedEntity.size();
//}

//HRESULT KG3DSceneSelectBase::GetSelectedEntity(INT nIndex, IEKG3DSceneEntity** ppEntity)//用KG3DSceneSelectionToolAccessProxy来访问成员
//{
//	KG_PROCESS_ERROR(m_listSelectedEntity.size() > 0);
//	_ASSERTE(NULL != ppEntity && nIndex >= 0 && nIndex < (INT)m_listSelectedEntity.size());
//
//	KG_PROCESS_ERROR(NULL != ppEntity && nIndex >= 0 && nIndex < (INT)m_listSelectedEntity.size());
//	{
//		KG3DSceneEntityList::iterator iter;
//		INT nCount = 0;
//		for (iter = m_listSelectedEntity.begin(); iter != m_listSelectedEntity.end(); ++iter)
//		{
//			if (nCount == nIndex)
//			{
//				*ppEntity = &(*iter); //todo:want get real refrerence point
//				break;
//			}
//			nCount ++;
//		}
//		KG_PROCESS_ERROR((size_t)nCount < m_listSelectedEntity.size());
//		return S_OK;
//	}
//Exit0:
//	return E_FAIL;
//}

INT KG3DSceneSelectBase::GetSelectableEntitySize()
{
	return (INT)m_listSelectableEntity.size();
}

HRESULT KG3DSceneSelectBase::GetSelectableEntity(INT nIndex, IEKG3DSceneEntity** ppEntity)
{
	_ASSERTE(ppEntity && nIndex >= 0 && nIndex < (INT)m_listSelectableEntity.size());

	if (ppEntity && nIndex >= 0 && nIndex < (INT)m_listSelectableEntity.size())
	{
		KG3DSceneEntityList::iterator iter;
		INT nCount = 0;
		for (iter = m_listSelectableEntity.begin(); iter != m_listSelectableEntity.end(); ++iter)
		{
			if (nCount == nIndex)
			{
				*ppEntity = &(*iter); //todo:want get real refrerence point
				break;
			}
			nCount ++;
		}
		return S_OK;
	}
	return E_FAIL;
}

//HRESULT KG3DSceneSelectBase::RemoveSelectableEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer, BOOL bEitherRemoveInSelectedList)
//{
//	KG3DSceneEntity e(EntityType, EntityPointer);
//	return RemoveSelectableEntity(e, bEitherRemoveInSelectedList);
//}

//HRESULT KG3DSceneSelectBase::RemoveSelectableEntity( const KG3DSceneEntity& Entity,  BOOL bEitherRemoveInSelectedList /*TRUE*/)
//{
//	//std::list 这样Remove的话，会触发一个线性比较，很慢的，有空改成Set之类的
//	if(bEitherRemoveInSelectedList)
//		m_listSelectedEntity.remove(Entity);
//	m_listSelectableEntity.remove(Entity);
//	return S_OK;
//}
//HRESULT KG3DSceneSelectBase::RemoveSelectedEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer)
//{
//	KG3DSceneEntity e(EntityType, EntityPointer);
//	m_listSelectedEntity.remove(e);
//	return S_OK;
//}


//HRESULT KG3DSceneSelectBase::AddSelectedEntity(IEKG3DSceneEntity* Entity)
//{
//	_ASSERTE(Entity);
//	if (Entity)
//	{
//		KG3DSceneEntity RefEntity = *((KG3DSceneEntity*)Entity);
//		m_listSelectedEntity.push_back(RefEntity);
//		return S_OK;
//	}
//	return E_FAIL;
//}


//HRESULT KG3DSceneSelectBase::AddSelectedEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer)
//{
//	KG3DSceneEntity e(EntityType, EntityPointer);
//	m_listSelectedEntity.push_back(e);
//	return S_OK;
//}

//HRESULT KG3DSceneSelectBase::ClearSelectedEntity()
//{
//	m_listSelectedEntity.clear();
//	return S_OK;
//}

HRESULT KG3DSceneSelectBase::CopySelectable2SelectedSet(LPVOID pBillboard)
{
	VOID *pvEntityPointer = NULL;
	KG3DSceneEntityList::iterator it = m_listSelectableEntity.begin();
	while (it != m_listSelectableEntity.end())
	{
		it->GetPointer(&pvEntityPointer);		
		if (pvEntityPointer == (VOID *)pBillboard)
		{
			m_listSelectedEntity.push_back(*it);
			//pFrame->m_dlgSelect.UpdateList();
			//pFrame->GetKeysFrame()->UpdateTransformUI();
			break;
		}
		++it;
	}

	return S_OK;
}

HRESULT KG3DSceneSelectBase::SetSelectable( const IEKG3DSceneEntity& Entity, BOOL bSelectable )
{
	/*return bSelectable ? this->AddSelectableEntity((const KG3DSceneEntity&)(Entity))
		:this->RemoveSelectableEntity((const KG3DSceneEntity&)(Entity), FALSE);*/

	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DSceneEntity, KG3DSceneEntity);

	const KG3DSceneEntity& EntitySolid = static_cast<const KG3DSceneEntity&>(Entity);
	if (bSelectable)
	{
		if (! m_listSelectableEntity.empty())
		{
			m_listSelectableEntity.remove(EntitySolid);
		}
		m_listSelectableEntity.push_back(EntitySolid);
	}
	else
	{
		if (! m_listSelectableEntity.empty())
		{
			m_listSelectableEntity.remove(EntitySolid);
		}
	}

	if (SCENEENTITY_OBJECT == EntitySolid.GetType())
	{
		reinterpret_cast<KG3DRepresentObject*>(EntitySolid.m_pPointer)->SetSelectable(bSelectable);
	}

	return S_OK;
}

HRESULT KG3DSceneSelectBase::SetSelectable( SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelectable )
{
	return this->SetSelectable(KG3DSceneEntity(eEntityType, pvEntityPointer), bSelectable);
}

HRESULT KG3DSceneSelectBase::SetSelectable( IEKG3DRepresentObject& obj, BOOL bSelectable )
{
	return this->SetSelectable(KG3DSceneEntity(SCENEENTITY_OBJECT, &obj), bSelectable);
}
HRESULT KG3DSceneSelectBase::SetSelected( const IEKG3DSceneEntity& Entity, BOOL bSelected )
{
	KG3DSceneEntity entitySolid = static_cast<const KG3DSceneEntity&>(Entity);

	if (bSelected)
	{
		if(! m_listSelectedEntity.empty())	//微软std伟大的设计，remove之前居然要用户检查一下empty，不然就当机
		{
			m_listSelectedEntity.remove(entitySolid);
		}
		entitySolid.OnSelect();
		m_listSelectedEntity.push_back(entitySolid);
	}
	else
	{
		if(! m_listSelectedEntity.empty())
		{
			KG3DSceneEntityList::iterator it = std::find(m_listSelectedEntity.begin(), m_listSelectedEntity.end(), entitySolid);
			if (it != m_listSelectedEntity.end())
			{
				entitySolid.OnUnSelect();
				m_listSelectedEntity.erase(it);
			}
		}
	}
	return S_OK;
	
}

HRESULT KG3DSceneSelectBase::SetSelected( IEKG3DRepresentObject& obj, BOOL bSelected )
{
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DRepresentObject, KG3DRepresentObject);
	
	if (bSelected)
	{
		KG3DSceneEntity entity(SCENEENTITY_OBJECT, &obj);
		if (IsSelectable(entity))
		{
			m_listSelectedEntity.push_back(entity);
			return S_OK;
		}
	}
	else
	{
		if(m_listSelectedEntity.empty())
			return S_OK;


		for (KG3DSceneEntityList::iterator it = m_listSelectedEntity.begin(); it != m_listSelectedEntity.end(); ++it)
		{
			KG3DSceneEntity& entity = *it;
			if (entity.m_pPointer == &obj)
			{
				m_listSelectedEntity.erase(it);
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT KG3DSceneSelectBase::SetSelected( SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelected )
{
	KG3DSceneEntity entity(eEntityType, pvEntityPointer);
	return this->SetSelected(entity, bSelected);
}
HRESULT KG3DSceneSelectBase::TraverseSelected( KG3DRepresentObjectVisitor& visitor )
{
	HRESULT hr = S_OK;
	for (KG3DSceneEntityList::iterator it = m_listSelectedEntity.begin(); it != m_listSelectedEntity.end(); ++it)
	{
		KG3DSceneEntity& entity = *it;

		if (entity.GetType() == SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject* p = static_cast<KG3DRepresentObject*>(entity.m_pPointer);
			hr = visitor.Accept(*p);
			KG_COM_PROCESS_ERROR(hr);
		}
	}
Exit0:
	return hr;
}

HRESULT KG3DSceneSelectBase::TraverseSelected( KG3DSceneEntityVisitor& visitor )
{
	HRESULT hr = S_OK;
	for (KG3DSceneEntityList::iterator it = m_listSelectedEntity.begin(); it != m_listSelectedEntity.end(); ++it)
	{
		KG3DSceneEntity& entity = *it;
		hr = visitor.Accept(entity);
		KG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return hr;
}
HRESULT KG3DSceneSelectBase::TraverseSelectable( KG3DSceneEntityVisitor& visitor )
{
	HRESULT hr = S_OK;
	for (KG3DSceneEntityList::iterator it = m_listSelectableEntity.begin(); it != m_listSelectableEntity.end(); ++it)
	{
		KG3DSceneEntity& entity = *it;
		hr = visitor.Accept(entity);
		KG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return hr;
}
ULONG KG3DSceneSelectBase::GetSelectionCount()
{
	return static_cast<ULONG>(m_listSelectedEntity.size());
}
ULONG KG3DSceneSelectBase::IsSelected(const KG3DRepresentObject& obj )
{
	const KG3DRepresentObject* pTarget = &obj;
	for (KG3DSceneEntityList::iterator it = m_listSelectedEntity.begin(); it != m_listSelectedEntity.end(); ++it)
	{
		KG3DSceneEntity& entity = *it;
		if(entity.m_pPointer == pTarget)
			return TRUE;
	}
	return FALSE;
}

HRESULT KG3DSceneSelectBase::ClearSelectable()
{
	m_listSelectableEntity.clear();
	return S_OK;
}

HRESULT KG3DSceneSelectBase::ClearSelection()
{
	//m_listTempSelectedEntity.clear();//2009-1-13这句移动到ClearSelection里面，ClearSelection 的时候这个Temp的应该也被清空的
	KG3DSceneEntityList::iterator it = m_listSelectedEntity.begin();
	KG3DSceneEntityList::iterator itEnd = m_listSelectedEntity.end();
	while (it != itEnd)
	{
		(*it).OnUnSelect();
		it++;
	}
	m_listSelectedEntity.clear();
	return S_OK;
}

ULONG KG3DSceneSelectBase::IsSelectable( const KG3DSceneEntity& entity )
{
	LPVOID p = entity.m_pPointer;
	for (KG3DSceneEntityList::iterator it = m_listSelectableEntity.begin(); it != m_listSelectableEntity.end(); ++it)
	{
		KG3DSceneEntity& entityCur = *it;
		if(p == entityCur.m_pPointer)
			return TRUE;
	}
	return FALSE;
}

HRESULT KG3DSceneSelectBase::AllocSelGroup( LPCTSTR pGroupName )
{
	KG_PROCESS_ERROR(NULL != pGroupName);
	try
	{
		do 
		{
			std::tstring strName(pGroupName);
			TypeCtList::_Pairib pib = m_SelGroups.insert(TypeCtList::value_type(std::tstring(strName), KG3DSceneEntityList()));
			if(! pib.second)
				break;

			return S_OK;
		} while(false);		
	}
	catch(...)
	{

	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSelectBase::ClearSelGroup( LPCTSTR pGroupName )
{
	KG3DSceneEntityList* p = GetSelGroup(pGroupName);
	KG_PROCESS_ERROR(NULL != p);

	p->clear();
	return S_OK;

Exit0:
	return E_FAIL;
}

KG3DSceneEntityList* KG3DSceneSelectBase::GetSelGroup( LPCTSTR pName)
{
	KG_PROCESS_ERROR(NULL != pName);
	{
		std::tstring strName(pName);

		TypeCtList::iterator it = m_SelGroups.find(strName);
		KG_PROCESS_ERROR(it != m_SelGroups.end());

		KG3DSceneEntityList& entityList = it->second;

		return &entityList;
	}
Exit0:
	return NULL;
}

ULONG KG3DSceneSelectBase::GetSelGroupSize( LPCTSTR pGroupName )
{
	KG3DSceneEntityList* p = GetSelGroup(pGroupName);
	KG_PROCESS_ERROR(NULL != p);

	return static_cast<ULONG>(p->size());
Exit0:
	return 0;
}


HRESULT KG3DSceneSelectBase::CopySelGroupToCurSelection( LPCTSTR pGroupName )
{
	KG3DSceneEntityList* p = GetSelGroup(pGroupName);
	KG_PROCESS_ERROR(NULL != p);

	m_listSelectedEntity.clear();

	m_listSelectedEntity = *p;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSelectBase::CopyCurSelectionToSelGroup( LPCTSTR pGroupName )
{
	KG3DSceneEntityList* p = GetSelGroup(pGroupName);
	KG_PROCESS_ERROR(NULL != p);

	p->clear();

	*p = m_listSelectedEntity;
	return S_OK;
Exit0:
	return E_FAIL;
}
BOOL KG3DSceneSelectBase::EntityIsSelectable(KG3DSceneEntityList& listSelectable, SelectionInput& selInput, KG3DSceneEntity& entity)
{
	if((selInput.dwSelMask & EM_SELMASK_NOSMALLOBJ))
	{
		BBox box;
		entity.GetBBox(&box);
		if(box.GetDiameter() < selInput.fSizeLimit)
			return FALSE;
	}
	if(selInput.dwSelMask & EM_SELMASK_NOBIGOBJ)
	{
		BBox box;
		entity.GetBBox(&box);
		if(box.GetDiameter() >= selInput.fSizeLimit)
			return FALSE;
	}
	if(selInput.dwSelMask == EM_SELMASK_ALL)
		return TRUE;

	if (entity.GetType() != SCENEENTITY_OBJECT)
		return FALSE;
	KG3DRepresentObject* pObj = (KG3DRepresentObject*)entity.m_pPointer;
	if(!pObj)
		return FALSE;

	DWORD dwType = pObj->GetType();

    if (dwType == REPRESENTOBJECT_PVS)
        return !!(selInput.dwSelMask & EM_SELMASK_MODEL);

	if((dwType == REPRESENTOBJECT_DEFAULT) )
	{
		KG3DModel* pModel = NULL;
		pObj->GetModel(pModel,0,FALSE);
		if(!pModel)
			return FALSE;
		DWORD dwModelType =	pModel->GetType();
		switch(dwModelType)
		{
		case MESHTYPE_DEFAULT:
			return !!(selInput.dwSelMask & EM_SELMASK_MODEL);
		case MESHTYPE_MODELST:
			return !!(selInput.dwSelMask & EM_SELMASK_MODELSET);
		case MESHTYPE_SPEEDTREE:
			return !!(selInput.dwSelMask & EM_SELMASK_SPEEDTREE);
		case MESHTYPE_SFX:
			return !!(selInput.dwSelMask & EM_SELMASK_SFX);
		default:
			return FALSE;
		}
	}
	if(dwType == REPRESENTOBJECT_SET)
	{
		return !!(selInput.dwSelMask & EM_SELMASK_OBJSET);
		/*if(selInput.dwSelMask & EM_SELMASK_OBJSET)
			return TRUE;
		for(DWORD i =0;i < pObj->GetModelCount();i++)
		{
			BOOL bRet = FALSE;
			KG3DModel* pModel = NULL;
			pObj->GetModel(pModel,i);
			if(!pModel)
				return FALSE;
			DWORD dwModelType =	pModel->GetType();
			switch(dwModelType)
			{
			case MESHTYPE_DEFAULT:
				bRet = selInput.dwSelMask & EM_SELMASK_MODEL;
				break;
			case MESHTYPE_MODELST:
				bRet =  selInput.dwSelMask & EM_SELMASK_MODELSET;
				break;
			case MESHTYPE_SPEEDTREE:
				bRet =  selInput.dwSelMask & EM_SELMASK_SPEEDTREE;
				break;
			case MESHTYPE_SFX:
				bRet =  selInput.dwSelMask & EM_SELMASK_SFX;
				break;
			default:
				break;
			}
			if(bRet)
				return TRUE;
		}*/
	}
	if (dwType == REPRESENTOBJECT_NPC ||
		dwType == REPRESENTOBJECT_DOODAD ||
		dwType == REPRESENTOBJECT_WAYPOINT ||
		dwType == REPRESENTOBJECT_POLY ||
		dwType == REPRESENTOBJECT_REFRESHPOINT ||
		dwType == REPRESENTOBJECT_TRAFFICPOINT
		)
	{
		if (g_bDesign)
			return !!(selInput.dwSelMask & EM_SELMASK_LOGICOBJ) && !!(selInput.dwSelMaskDesign >> dwType & 0x1);
		else
			return !!(selInput.dwSelMask & EM_SELMASK_LOGICOBJ);
	}

	return FALSE;
}

HRESULT KG3DSceneSelectBase::CopySelGroupToSelectable( LPCTSTR pGroupName )
{
	KG3DSceneEntityList* p = GetSelGroup(pGroupName);
	KG_PROCESS_ERROR(NULL != p);

	m_listSelectableEntity.clear();

	m_listSelectableEntity = *p;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSelectBase::CopySelectableToSelGroup( LPCSTR pGroupName )
{
	KG3DSceneEntityList* p = GetSelGroup(pGroupName);
	KG_PROCESS_ERROR(NULL != p);

	p->clear();

	*p = m_listSelectableEntity;
	return S_OK;
Exit0:
	return E_FAIL;
}

void KG3DSceneSelectBase::SetSelectOption(DWORD dwOption, DWORD dwOptionDesign)
{
	m_SelInput.dwSelMask = dwOption;
	m_SelInput.dwSelMaskDesign = dwOptionDesign;
}
DWORD KG3DSceneSelectBase::GetSelectOption()
{
	return m_SelInput.dwSelMask;
}
//
//HRESULT KG3DSceneSelectBase::GetFrontAccessProxy( LPCTSTR pGroupName, KG_CUSTOM_HELPERS::FrontAccessProxy** pProxy )
//{
//	KG_CUSTOM_HELPERS::FrontAccessProxy* pReturn = NULL;
//	KG3DSceneEntityList* p = GetSelGroup(pGroupName);
//	KG_PROCESS_ERROR(NULL != p && NULL != pProxy);
//
//	pReturn = new TFrontAccessProxySolid<KG3DSceneEntityList>(*p);
//	KG_PROCESS_ERROR(NULL != pReturn);
//
//	*pProxy = pReturn;
//
//	return S_OK;
//Exit0:
//	SAFE_DELETE(pReturn);
//	return E_FAIL;
//}
void KG3DSceneSelectBase::SetLimitValue(DWORD dwLimitValue)
{
	m_SelInput.fSizeLimit = (FLOAT)dwLimitValue;
}
DWORD KG3DSceneSelectBase::GetLimitValue()
{
	return (DWORD)m_SelInput.fSizeLimit;
}

HRESULT KG3DSceneSelectBase::GetSelectableList( KG3DSceneEntityList& entities )
{
	entities = m_listSelectableEntity;
	return S_OK;
}

TypeEntityAccessProxy& KG3DSceneSelectBase::GetSelectionAccessProxy()
{
	return m_accessProxy;
}

HRESULT KG3DSceneSelectBase::RegisterEventListener( DWORD messageID, IKG3DMessageReceiverBase& receiver )
{
	if(NULL == m_pSender || KM_SELECT != messageID)
		return E_FAIL;

	bool bRet = m_pSender->RegisterReceiver(&receiver);
	return  bRet ? S_OK : E_FAIL;
}

static KG3DMessageSender s_Sender;
KG3DTransformTool& KG3DSceneSelectBase::GetTransformToolInterface( DWORD dwType )
{
	struct   KG3DSceneSelectBaseTransformToolInterface : public KG3DTransformTool 
	{
		KG3DSceneSelectBase* m_pSceneSelectBase;
		KG3DMessageSender	m_Sender;
		DWORD m_dwType;

		STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_NONE;}
		STDMETHOD(Render)(){return E_NOTIMPL;}
		STDMETHOD(FrameMove)(){return E_NOTIMPL;}

		virtual HRESULT	SetCoordMode(const KG3DCOORD& coordMode)
		{
			_ASSERTE(NULL != m_pSceneSelectBase);
			m_pSceneSelectBase->GetCoordMove().SetMode(coordMode);
			m_pSceneSelectBase->GetCoordRotate().SetMode(coordMode);
			m_pSceneSelectBase->GetCoordScale().SetMode(coordMode);
			return S_OK;
		}
		virtual HRESULT RegisterEventListener(DWORD messageID, IKG3DMessageReceiverBase& receiver)
		{
			if (messageID < KM_TRANSFORM_TYPE_BEGIN && messageID > KM_TRANSFORM_TYPE_END)
			{
				return E_FAIL;
			}
			return m_Sender.RegisterReceiver(&receiver);
		}

		virtual HRESULT TransformDirect(const D3DXVECTOR3& vTransform)
		{
			_ASSERTE(NULL != m_pSceneSelectBase);
			if (m_dwType == KG3DTYPE_TRANSLATION_TOOL)
			{
				return m_pSceneSelectBase->MoveDirect(vTransform);
			}
			else if (m_dwType == KG3DTYPE_ROTATION_TOOL)
			{
				D3DXQUATERNION quaterTemp;
				D3DXQuaternionRotationYawPitchRoll(&quaterTemp, vTransform.y, vTransform.x, vTransform.z);//注意Yaw是Y轴的旋转
				return m_pSceneSelectBase->RotateDirect(quaterTemp, &vTransform);
			}
			else if (m_dwType == KG3DTYPE_SCALING_TOOL)
			{
				return m_pSceneSelectBase->ScaleDirect(vTransform);
			}

			_ASSERTE(NULL && _T("自身类型错误，否则程序流不会到这里"));
			return E_FAIL;
		}
		virtual HRESULT TransformDirectCoord(const D3DXMATRIX& matTransform)
		{
			_ASSERTE(NULL && _T("旧的编辑器不支持这种接口"));
			return E_FAIL;
		}


#if defined(DEBUG) | defined(_DEBUG)		
		virtual KG3DMessageSender* TestBegin()
		{
			_ASSERTE(NULL != m_pSceneSelectBase && NULL != m_pSceneSelectBase->m_pEngineMsgReceiver);
			bool bRet  = s_Sender.RegisterReceiver(m_pSceneSelectBase->m_pEngineMsgReceiver);
			_ASSERTE(NULL != bRet);
			return &s_Sender;
		}
		virtual VOID TestEnd()
		{
			_ASSERTE(NULL != m_pSceneSelectBase && NULL != m_pSceneSelectBase->m_pEngineMsgReceiver);
			s_Sender.UnRegisterReceiver(m_pSceneSelectBase->m_pEngineMsgReceiver);
		}
#endif

		KG3DSceneSelectBaseTransformToolInterface(KG3DSceneSelectBase& SelBase, DWORD dwType):m_pSceneSelectBase(&SelBase), m_dwType(dwType){}
	};

	_ASSERTE(dwType >= KG3DTYPE_TRANSFORM_TOOL_BEGIN && dwType <= KG3DTYPE_TRANSFORM_TOOL_END);

	//聚合的工作方式，做出一个类内部的类，把调用返回给类本身

	KG3DTransformTool*& pRefTool = m_TransformTools[dwType - KG3DTYPE_TRANSFORM_TOOL_BEGIN];
	if (NULL == pRefTool)
	{
		pRefTool = new KG3DSceneSelectBaseTransformToolInterface(*this, dwType);
	}

	if (NULL == pRefTool)
	{
		return ::GetStaticDummyTransformTool();
	}

	return *pRefTool;
}

HRESULT KG3DSceneSelectBase::LaunchSelect()
{
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
			return E_FAIL;
		}
	}

	{
		IEKG3DSceneOutputWnd* pOutputWnd = NULL;
		HRESULT hr = m_Scene.GetCurOutputWnd(&pOutputWnd);
		KG_COM_PROCESS_ERROR(hr);

		const D3DVIEWPORT9* pViewport = pOutputWnd->GetViewPort();
		KG_PROCESS_ERROR(NULL != pViewport);

		IEKG3DCamera* pCam = pOutputWnd->GetIECamera();
		KG_PROCESS_ERROR(NULL != pCam);

		D3DXMATRIX matView, matProj;

		hr = pCam->GetMatrix(&matView, &matProj);
		KG_COM_PROCESS_ERROR(hr);

		return GenerateTempSelectedList(*pViewport, matView, matProj, m_SelInput, m_listSelectableEntity, m_listSelectedEntity, m_dwIncreaseMode);
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSelectBase::GetAccessProxy( IEKG3DSceneSelectionToolAccessProxy** ppAccessProxy )
{
	if (NULL != ppAccessProxy)
	{
		*ppAccessProxy = m_pIEAccessProxy;
		return S_OK;
	}
	return E_FAIL;
}
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DSceneSelectBase)
{

}
KG_TEST_END(KG3DSceneSelectBase)
#endif

KG3DSceneSelectBase::SelectionInput::SelectionInput()
{
	vRayDirA = D3DXVECTOR3(1,0,0);
	ZeroMemory(this, sizeof(this));

	dwSelMask = (DWORD)EM_SELMASK_ALL;
	fSizeLimit = 1000.0f;
}