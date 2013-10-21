////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSceneEditorDialogLight.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-3 16:04:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogLight.h" 
#include "IEKG3DCommonContainer.h"
#include "IEKG3DTypes.h"
#include "IEKG3DPointLightTable.h"
#include "IEKG3DCommonObject.h"
#include "IEEditor.h"
// KSceneSceneEditorDialogLight dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogLight, CDialog)

KSceneSceneEditorDialogLight::KSceneSceneEditorDialogLight(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogLight::IDD, pParent)
{

}

KSceneSceneEditorDialogLight::~KSceneSceneEditorDialogLight()
{
	
}

void KSceneSceneEditorDialogLight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_ListBox);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogLight, CDialog)
	ON_BN_CLICKED(IDC_BN_ADD, &KSceneSceneEditorDialogLight::OnBnClickedBnAdd)
	ON_BN_CLICKED(IDC_BN_DEL, &KSceneSceneEditorDialogLight::OnBnClickedBnDel)
	ON_BN_CLICKED(IDC_BN_CLEAR, &KSceneSceneEditorDialogLight::OnBnClickedBnClear)
	ON_LBN_SELCHANGE(IDC_LIST, &KSceneSceneEditorDialogLight::OnListSelChange)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// KSceneSceneEditorDialogLight message handlers

void KSceneSceneEditorDialogLight::OnBnClickedBnAdd()
{
	// TODO: Add your control notification handler code here
	IEKG3DRepresentObject* pHandleObj = NULL;
	IEKG3DCommonContainer* pLightTable = GetLightTable();
	KG_PROCESS_ERROR(pLightTable);
	{
		IEKG3DSceneSceneEditor* pScene = KSH::GetEngineInterfaceEx<IEKG3DSceneSceneEditor>();
		KG_PROCESS_ERROR(pScene);
		
		{
			//LightTable转成加强接口
			IEKG3DPointLightTable* pSolidLightTable 
				= reinterpret_cast<IEKG3DPointLightTable*>(pLightTable->ToAnotherInterface(0));
			_ASSERTE(pSolidLightTable);

			HRESULT hr = pSolidLightTable->GetRepresentObjetOfLight(NULL, &pHandleObj);
			KG_COM_PROCESS_ERROR(hr);
		}

		pScene->SetObjectInHandle(pHandleObj, TRUE);	
		//pScene->SetEditState(SCENESTATE_PLACEOBJ);//新的放置工具不需要自己SetState
	}
	//m_ListBox.Reload();
	return;
Exit0:
	SAFE_RELEASE(pHandleObj);
	return;
}

void KSceneSceneEditorDialogLight::OnBnClickedBnDel()
{
	IEKG3DCommonContainer* pLightTable = GetLightTable();
	KG_PROCESS_ERROR(pLightTable);
	{
		INT nCurSel = m_ListBox.GetCurSel();
		HRESULT hr = pLightTable->Del(static_cast<DWORD_PTR>(nCurSel));
		KG_COM_PROCESS_ERROR(hr);

		//m_ListBox.Reload();
	}
Exit0:
	return;
}

void KSceneSceneEditorDialogLight::OnBnClickedBnClear()
{
	IEKG3DCommonContainer* pLightTable = GetLightTable();
	KG_PROCESS_ERROR(pLightTable);
	{
		pLightTable->Clear();
		//m_ListBox.Reload();
	}
Exit0:
	return;
}
void KSceneSceneEditorDialogLight::OnListSelChange()
{

}

VOID __stdcall KSceneSceneEditorDialogLight::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	if (Message.m_uMessage == KM_COMMON_CONTAINER_UPDATED)
	{
		m_ListBox.Reload();
	}
}
IEKG3DCommonContainer* KSceneSceneEditorDialogLight::GetLightTable()
{
	return KSH::GetEngineAbstractInterface<KG3DTYPE_COMMON_OBJECT_CONTAINER>(KG3DTYPE_ENVEFF_POINTLIGHT, 0, 0);
}

void KSceneSceneEditorDialogLight::OnTimer( UINT_PTR nIDEvent )
{
	/*
	if (nIDEvent == 0)
		{
			IEKG3DCommonContainer* pCon = this->GetLightTable();
			if(pCon)
			{
				if (pCon->GetCount() != static_cast<ULONG>(m_ListBox.GetCount()))
				{
					m_ListBox.Reload();
				}
			}
		}*/
	
	CDialog::OnTimer(nIDEvent);
}

void KSceneSceneEditorDialogLight::OnShowWindow( BOOL bShow, UINT nStatus )
{
	/*
	if (bShow)
		{
			this->SetTimer(0, 1000, NULL);
		}
		else
		{
			this->KillTimer(0);
		}
		::OutputDebugString("SW\n");*/
	if (bShow)
	{
		IEKG3DCommonContainer* pContainer = this->GetLightTable();
		if (pContainer)
		{
			IKG3DMessageSenderBase* pSender = NULL;
			HRESULT hr = pContainer->GetMessageSenderInterface(&pSender);
			if (SUCCEEDED(hr))
			{
				_ASSERTE(pSender);
				pSender->RegisterReceiver(this);
			}
		}
		m_ListBox.Reload();
	}
	else
	{
		IEKG3DCommonContainer* pContainer = this->GetLightTable();
		if (pContainer)
		{
			IKG3DMessageSenderBase* pSender = NULL;
			HRESULT hr = pContainer->GetMessageSenderInterface(&pSender);
			if (SUCCEEDED(hr))
			{
				_ASSERTE(pSender);
				pSender->UnRegisterReceiver(this);
			}
		}
	}
	CDialog::OnShowWindow(bShow, nStatus);
}

HRESULT KSceneSceneEditorDialogLight::List::Reload()
{
	this->ResetContent();
	IEKG3DCommonContainer* pLightTable = KSH::GetEngineAbstractInterface<KG3DTYPE_COMMON_OBJECT_CONTAINER>(KG3DTYPE_ENVEFF_POINTLIGHT, 0, 0);
	KG_PROCESS_ERROR(pLightTable);
	{
		ULONG uCount = pLightTable->GetCount();
		TCHAR TempSTR[MAX_PATH];
		for (ULONG i = 0; i < uCount; ++i)
		{
			_stprintf_s(TempSTR, _T("Light_%3d"), i);
			this->AddString(TempSTR);
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}