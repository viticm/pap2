////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DTransformTool.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-10-16 11:32:28
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DSceneObjectTransformTool.h"
#include "KG3DLocalRotationCoord.h"
#include "KG3DSceneSelectionTool.h"
#include "KG3DRepresentObjectTrans.h"
#include "KG3DScene.h"

#include "KG3DLocalTranslationCoord.h"
#include "KG3DLocalScalCoord.h"

#include "KG3DSceneFunctions.h"
#include "KG3DMessenger.h"

#include "KG3DCollocator.h"	//g_Switch

////////////////////////////////////////////////////////////////////////////////

class KG3DTransformToolImp : public KG3DTransformTool
{
public:
	STDMETHOD(Init)() = 0;	
	STDMETHOD_(ULONG, GetType)() = 0;

	STDMETHOD(Render)();
	STDMETHOD(FrameMove)();

	virtual HRESULT	SetCoordMode(const KG3DCOORD& coordMode);
	virtual HRESULT RegisterEventListener(DWORD messageID, IKG3DMessageReceiverBase& receiver);

	virtual HRESULT TransformDirect(const D3DXVECTOR3& vTransform) = 0;
	virtual HRESULT TransformDirectCoord(const D3DXMATRIX& matTransform);
;
#if defined(DEBUG) | defined(_DEBUG)
	virtual KG3DMessageSender* TestBegin();
	virtual VOID TestEnd();
#endif

	KG3DTransformToolImp(KG3DScene& Scene);
	~KG3DTransformToolImp();
protected:
	STDMETHOD(UnInit)();
	static D3DXMATRIX GetSelectionCenter(KG3DScene& Scene);
	VOID	SendOperationEndEvent();

	KG3DCOORD	m_CoordMode;	//暂时都是Local的旋转，没有World的旋转
	KG3DBaseCoord* m_pCoord;
	KG3DScene& m_Scene;
	ULONG		m_CurState;
	D3DXMATRIX	m_matTotalTrans;
	D3DXMATRIX	m_SelectedObjsMatrix;

private:
	KG3DMessageSender m_msgSender;
};

KG3DTransformToolImp::~KG3DTransformToolImp()
{
	UnInit();
}

KG3DTransformToolImp::KG3DTransformToolImp(KG3DScene& Scene) 
:m_Scene(Scene)
,m_pCoord(NULL)
,m_CoordMode(KG3DCOORD_LOCAL)
,m_CurState(EM_EDITOR_TOOL_STATE_NOTATIVATED)
{
	D3DXMatrixIdentity(&m_matTotalTrans);
}

HRESULT KG3DTransformToolImp::SetCoordMode( const KG3DCOORD& coordMode )
{
	m_CoordMode = coordMode; 
	return S_OK;
}
HRESULT STDMETHODCALLTYPE KG3DTransformToolImp::UnInit()
{
	SAFE_RELEASE(m_pCoord);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE KG3DTransformToolImp::FrameMove()
{
	if (! g_Switch.bNewSelection)
	{
		return E_FAIL;
	}
	_ASSERTE(NULL != m_pCoord);
	m_pCoord->FrameMove();

	ULONG NewState = m_pCoord->GetCurStateID();

	//Coord里面代码比较复杂，所以用的是以类方式表现的状态机，这里简单点，用switch比较新旧状态来模拟状态机
	switch(m_CurState)
	{
	case EM_EDITOR_TOOL_STATE_NOTATIVATED:
		{
			if(NewState == m_CurState)
				m_CurState;
			else if(NewState == EM_EDITOR_TOOL_STATE_ATIVATED)
			{
				m_CurState = NewState;
				m_SelectedObjsMatrix = GetSelectionCenter(m_Scene);
				m_pCoord->SetPosition(D3DXMatrixGetTranslationPart(m_SelectedObjsMatrix));
				D3DXMatrixIdentity(&m_matTotalTrans);
			}
			_ASSERTE(NewState != EM_EDITOR_TOOL_STATE_TRANSFORMING);
		}	
		break;
	case EM_EDITOR_TOOL_STATE_ATIVATED:
		{
			if(NewState == m_CurState)
				m_CurState;
			else if(NewState == EM_EDITOR_TOOL_STATE_NOTATIVATED)
			{
				m_CurState = NewState;
			}
			else if (NewState == EM_EDITOR_TOOL_STATE_TRANSFORMING)
			{
				D3DXMatrixIdentity(&m_matTotalTrans);
				m_CurState = NewState;
			}
		}
		break;
	case EM_EDITOR_TOOL_STATE_TRANSFORMING:
		{
			if(NewState == m_CurState)
			{
				D3DXMATRIX matTrans = m_pCoord->GetTrans();

				D3DXMATRIX matInvOfCurTrans;
				if(NULL == D3DXMatrixInverse(&matInvOfCurTrans, NULL, &m_matTotalTrans))
					break;

				D3DXMATRIX matObjTrans = matInvOfCurTrans * matTrans;

				//暂时只有World的部分
				TransformDirectCoord(matObjTrans);

				m_matTotalTrans = matTrans;
			}
			else 
			{
				if (NewState == EM_EDITOR_TOOL_STATE_NOTATIVATED)
				{
					m_CurState = NewState;
				}
				else if(NewState == EM_EDITOR_TOOL_STATE_ATIVATED)	//从移动状态还原为Activate状态的时候，移动一下Coord的坐标
				{
					m_SelectedObjsMatrix *= m_matTotalTrans;

					D3DXMatrixIdentity(&m_matTotalTrans);

					m_pCoord->SetPosition(D3DXMatrixGetTranslationPart(m_SelectedObjsMatrix));
					m_CurState = NewState;		
				}						
			}
		}
		break;
	default:
		break;
	}	

	return S_OK;
}

VOID KG3DTransformToolImp::SendOperationEndEvent()
{
	if (g_Switch.bNewSelection)
	{
		KG3DMessage msgEnd(GetTypeCorrespondingMessage(this->GetType()), 0, EM_EVENT_STATE_END);
		m_msgSender.PostMessage(msgEnd);
	}
}

HRESULT STDMETHODCALLTYPE KG3DTransformToolImp::Render()
{
	if(! g_Switch.bNewSelection)
		return S_OK;

	if(m_pCoord->GetCurStateID() == EM_EDITOR_TOOL_STATE_NOTATIVATED)
		return S_OK;

	_ASSERTE(NULL != m_pCoord);
	m_pCoord->Render();
	return S_FALSE;
}


D3DXMATRIX KG3DTransformToolImp::GetSelectionCenter(KG3DScene& Scene)
{
	//启动之后，要计算中心
	D3DXMATRIX matRet;
	{
		D3DXVECTOR3 vCenter;

		KG3DSceneSelectionTool& selTool = Scene.GetSelectionTool();
		HRESULT hr = KSF::GetSelectionCenter(selTool, vCenter);
		KG_COM_PROCESS_ERROR(hr);
		
		D3DXMatrixTranslation(&matRet, vCenter.x, vCenter.y, vCenter.z);
		return matRet;
	}
Exit0:
	D3DXMatrixTranslation(&matRet, -FLT_MAX, -FLT_MAX, -FLT_MAX);
	return matRet;
}

HRESULT KG3DTransformToolImp::RegisterEventListener( DWORD messageID, IKG3DMessageReceiverBase& receiver )
{
	_ASSERTE(messageID >= KM_TRANSFORM_TYPE_BEGIN && messageID <= KM_TRANSFORM_TYPE_END);
	KG_PROCESS_ERROR(messageID >= KM_TRANSFORM_TYPE_BEGIN && messageID <= KM_TRANSFORM_TYPE_END);

	return m_msgSender.RegisterReceiver(&receiver) ? S_OK : E_FAIL;
Exit0:
	return E_FAIL;
}

#if defined(DEBUG) | defined(_DEBUG)
static KG3DMessageSender s_Sender;
KG3DMessageSender* KG3DTransformToolImp::TestBegin()
{	
	_ASSERTE(NULL != m_pCoord);
	KG3DMessageReceiverBase* pReceiverInterface = m_pCoord->TestGetReceiverInterface();
	_ASSERTE(NULL != pReceiverInterface);
	bool bRet = s_Sender.RegisterReceiver(pReceiverInterface);
	_ASSERTE(bRet);
	return &s_Sender;
}

VOID KG3DTransformToolImp::TestEnd()
{
	_ASSERTE(NULL != m_pCoord);
	KG3DMessageReceiverBase* pReceiverInterface = m_pCoord->TestGetReceiverInterface();
	_ASSERTE(NULL != pReceiverInterface);
	s_Sender.UnRegisterReceiver(pReceiverInterface);
}
#endif

HRESULT KG3DTransformToolImp::TransformDirectCoord(const D3DXMATRIX& matTransform )
{
	KG3DRepresentObjectTransformVisitor& visitorTemp = g_GetRepresentObjectTransVisitor();
	visitorTemp.SetMatrix(matTransform);

	m_Scene.GetSelectionTool().TraverseSelected(visitorTemp);

	SendOperationEndEvent();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
class KG3DRotationToolSolid : public KG3DTransformToolImp
{
	friend KG3DCommonObject* BuildKG3DRotationTool( LPCTSTR, DWORD_PTR pScene, DWORD_PTR );
public:
	KG3DRotationToolSolid(KG3DScene& Scene) : KG3DTransformToolImp(Scene){}
	STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_ROTATION_TOOL;}	
	STDMETHOD(Init)()
	{
		_ASSERTE(NULL == m_pCoord && _T("重复的初始化"));
		m_pCoord = BuildKG3DRotationCoordinateSolid(NULL, reinterpret_cast<DWORD_PTR>((KG3DScene*)&m_Scene.GetBroadcasterGroup()), NULL);
		return NULL != m_pCoord ? S_OK : E_FAIL;
	}
	virtual HRESULT TransformDirect(const D3DXVECTOR3& vTransform)
	{
		D3DXMATRIX matRotation;
		D3DXMatrixRotationYawPitchRoll(&matRotation, vTransform.y, vTransform.x, vTransform.z);	//Yaw是Y轴的，Pitch是x轴的，Roll是Z轴的
		return TransformDirectCoord(matRotation);
	}
};

KG3DCommonObject* BuildKG3DRotationTool( LPCTSTR, DWORD_PTR pScene, DWORD_PTR )
{
	KG3DRotationToolSolid* p = NULL;
	KG3DScene* pSceneSolid = reinterpret_cast<KG3DScene*>(pScene);
	_ASSERTE(NULL != pSceneSolid && _T("需要在wParam那赋值"));
	_ASSERTE(_tcsstr(typeid(*pSceneSolid).name(), _T("KG3DScene")) && _T("传入的对象不是这种类型的"));
	KG_PROCESS_ERROR(NULL != pSceneSolid);
	p = new KG3DRotationToolSolid(*pSceneSolid);
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

//////////////////////////////////////////////////////////////////////////
class KG3DTranslationToolSolid : public KG3DTransformToolImp
{
	friend KG3DCommonObject* BuildKG3DTranslationTool( LPCTSTR, DWORD_PTR pScene, DWORD_PTR );
public:
	KG3DTranslationToolSolid(KG3DScene& Scene) : KG3DTransformToolImp(Scene){}
	STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_TRANSLATION_TOOL;}	
	STDMETHOD(Init)()
	{
		_ASSERTE(NULL == m_pCoord && _T("重复的初始化"));
		m_pCoord = BuildKG3DTranslationCoordinateSolid(NULL, reinterpret_cast<DWORD_PTR>(&m_Scene.GetBroadcasterGroup()), NULL);
		return NULL != m_pCoord ? S_OK : E_FAIL;
	}
	virtual HRESULT TransformDirect(const D3DXVECTOR3& vTransform)
	{
		D3DXMATRIX matTranslation;
		D3DXMatrixTranslation(&matTranslation, vTransform.x, vTransform.y, vTransform.z);
		return TransformDirectCoord(matTranslation);
	}
};

KG3DCommonObject* BuildKG3DTranslationTool( LPCTSTR, DWORD_PTR pScene, DWORD_PTR )
{
	KG3DTranslationToolSolid* p = NULL;
	KG3DScene* pSceneSolid = reinterpret_cast<KG3DScene*>(pScene);
	_ASSERTE(NULL != pSceneSolid && _T("需要在wParam那赋值"));
	_ASSERTE(_tcsstr(typeid(*pSceneSolid).name(), _T("KG3DScene")) && _T("传入的对象不是这种类型的"));
	KG_PROCESS_ERROR(NULL != pSceneSolid);
	p = new KG3DTranslationToolSolid(*pSceneSolid);
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

//////////////////////////////////////////////////////////////////////////
class KG3DScalingToolSolid : public KG3DTransformToolImp
{
	friend KG3DCommonObject* BuildKG3DScalingTool( LPCTSTR, DWORD_PTR pScene, DWORD_PTR );
public:
	KG3DScalingToolSolid(KG3DScene& Scene) : KG3DTransformToolImp(Scene){}
	STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_SCALING_TOOL;}	
	STDMETHOD(Init)()
	{
		_ASSERTE(NULL == m_pCoord && _T("重复的初始化"));
		m_pCoord = BuildKG3DScaleCoordinateSolid(NULL, reinterpret_cast<DWORD_PTR>(&m_Scene.GetBroadcasterGroup()), NULL);
		return NULL != m_pCoord ? S_OK : E_FAIL;
	}
	virtual HRESULT TransformDirect(const D3DXVECTOR3& vTransform)
	{
		D3DXMATRIX matScaling;
		D3DXMatrixScaling(&matScaling, vTransform.x, vTransform.y, vTransform.z);
		return TransformDirectCoord(matScaling);
	}
};

KG3DCommonObject* BuildKG3DScalingTool( LPCTSTR, DWORD_PTR pScene, DWORD_PTR )
{
	KG3DScalingToolSolid* p = NULL;
	KG3DScene* pSceneSolid = reinterpret_cast<KG3DScene*>(pScene);
	_ASSERTE(NULL != pSceneSolid && _T("需要在wParam那赋值"));
	_ASSERTE(_tcsstr(typeid(*pSceneSolid).name(), _T("KG3DScene")) && _T("传入的对象不是这种类型的"));
	KG_PROCESS_ERROR(NULL != pSceneSolid);
	p = new KG3DScalingToolSolid(*pSceneSolid);
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

KG3DTransformTool& GetStaticDummyTransformTool()
{
	static KG3DTransformToolDummy dummy;
	return dummy;
}

DWORD KG3DTransformTool::GetTypeCorrespondingMessage(DWORD dwToolType)
{
	_ASSERTE(dwToolType >= KG3DTYPE_TRANSFORM_TOOL_BEGIN && dwToolType <= KG3DTYPE_TRANSFORM_TOOL_END);

	DWORD dwMessageType = dwToolType - KG3DTYPE_TRANSFORM_TOOL_BEGIN + KM_TRANSFORM_TYPE_BEGIN;
	_ASSERTE(dwMessageType >= KM_TRANSFORM_TYPE_BEGIN && dwMessageType <= KM_TRANSFORM_TYPE_END);
	return dwMessageType;
}