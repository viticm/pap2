////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingDialogKeyTable.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-2-9 9:59:07
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
// KSceneSettingDialogKeyTable.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingDialogKeyTable.h"
//#include "KGHelperClasses.h"
#include "KSceneSettingDWGCKeyEdit.h"
#include "KSceneSettingPropertySheet.h"
//#include "KSceneInterfaceGetters.h"
#include "IEKG3DEnvEffDynamicWeather.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// KSceneSettingDialogKeyTable dialog

/*
IMPLEMENT_DYNAMIC(KSceneSettingDialogKeyTable, CDialog)

KSceneSettingDialogKeyTable::KSceneSettingDialogKeyTable(CWnd* pParent / *=NULL* /)
	: CDialog(KSceneSettingDialogKeyTable::IDD, pParent)
	, m_csCaption(_T(""))
	, m_PropertySheetHWnd(NULL)
{
	//m_pKeyTable = NULL;
	//m_pEffect = NULL;
	ZeroMemory(&m_EnvEffHandle, sizeof(KG3DENVEFF_HANDLE));
}

KSceneSettingDialogKeyTable::~KSceneSettingDialogKeyTable()
{
}

void KSceneSettingDialogKeyTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_DWGC, m_csCaption);
	DDX_Control(pDX, IDC_LIST_DWGC_KEY_LIST, m_ListBox);
}

BEGIN_MESSAGE_MAP(KSceneSettingDialogKeyTable, CDialog)
	ON_BN_CLICKED(IDC_BN_DWGC_KEY_ADD, &KSceneSettingDialogKeyTable::OnBnClickedBnDwgcKeyAdd)
	ON_BN_CLICKED(IDC_BN_DWGC_KEY_REMOVE, &KSceneSettingDialogKeyTable::OnBnClickedBnDwgcKeyRemove)
	ON_BN_CLICKED(IDC_BN_DWGC_KEY_MOD, &KSceneSettingDialogKeyTable::OnBnClickedBnDwgcKeyMod)
	//ON_BN_CLICKED(IDC_BN_DWGC_KEY_SYN, &KSceneSettingDialogKeyTable::OnBnClickedBnDwgcKeySyn)
	ON_LBN_DBLCLK(IDC_LIST_DWGC_KEY_LIST, &KSceneSettingDialogKeyTable::OnLbnDblclkListDwgcKeyList)
END_MESSAGE_MAP()

HRESULT KSceneSettingDialogKeyTable::InitKeyTable(KG3DENVEFF_HANDLE EffHandle, HWND PropertySheetHandle)
{
	KG_PROCESS_ERROR(PropertySheetHandle != NULL && ::IsWindow(PropertySheetHandle));
	IEKG3DEnvEffect* pEnvEff = KSH::GetEngineAbstractInterface<KG3DTYPE_ENV_EFFECT>(
		EffHandle.m_dwType
		, EffHandle.m_nIndex);
	KG_PROCESS_ERROR(pEnvEff);
	m_EnvEffHandle = EffHandle;
	ReloadKeyTable();

	{
		TCHAR csTemp[64];
		if (SUCCEEDED(pEnvEff->GetEnvEffName(csTemp, _countof(csTemp))))
		{
			m_csCaption = csTemp;
			UpdateData(FALSE);
		}	
	}
	m_PropertySheetHWnd = PropertySheetHandle;
	return S_OK;
Exit0:
	_KG3D_DEBUG_SELF_REPORT("KeyTable没有正确被初始化");
	return E_FAIL;
}
VOID KSceneSettingDialogKeyTable::ReloadKeyTable()
{
	_ASSERTE(m_EnvEffHandle.m_dwType);
	IEKG3DEnvEffDWGC_KeyTable0* pKeyTable = KSH::GetEngineAbstractInterface<KG3DTYPE_ENVEFF_KEY_TABLE>(m_EnvEffHandle.m_dwType);
	if (!pKeyTable)
	{
		_KG3D_DEBUG_SELF_REPORT("不能得到正确的KeyTable");
		return;
	}
	int* pnTempArray = NULL;
	int nSize = 0;
	m_ListBox.ResetContent();
	nSize = pKeyTable->GetSize();	
	KG_PROCESS_SUCCESS(nSize == 0);	
	if (nSize > 120)///随便设一个阈值
	{
		OutputDebugString(_STRINGER(KSceneSettingDialogKeyTable)":为什么关键帧那么多？\n");
		goto Exit0;
	}
	pnTempArray = new int[nSize];
	KG_PROCESS_ERROR(pnTempArray);
	{
		CString csTempString;
		int nWritedIndexCount = 0;
		pKeyTable->GetElementIndices(pnTempArray, nSize, &nWritedIndexCount);
		for (int i = 0; i < nWritedIndexCount; i++)
		{
			csTempString.Format(_T("%d"), pnTempArray[i]);
			int nAddedPlace = m_ListBox.AddString((LPCTSTR)csTempString);
			if (nAddedPlace >= 0)
			{
				m_ListBox.SetItemData(nAddedPlace, (DWORD_PTR)(pnTempArray[i]));
			}
		}
	}
Exit1:
	SAFE_DELETE_ARRAY(pnTempArray);
	return;
Exit0:
	SAFE_DELETE_ARRAY(pnTempArray);	
}
int KSceneSettingDialogKeyTable::GetCurSelData()
{
	CString czTemp;
	int nSel = m_ListBox.GetCurSel();
	/ *
	if (nSel >= 0)
		{
			int nTemp = m_ListBox.GetTextLen(nSel); 
			if (nTemp >= 0)
			{
				m_ListBox.GetText(nSel, czTemp.GetBuffer(nTemp));
				czTemp.ReleaseBuffer();
				return _ttoi((LPCTSTR)czTemp);
			}			
		}* /
	if (nSel >= 0)
	{
		DWORD_PTR ItemData = m_ListBox.GetItemData(nSel);
		if (ItemData > 0)
		{
			return (INT)(ItemData);
		}
	}
	return -1;
}
void KSceneSettingDialogKeyTable::OnOK()
{
	///什么也不做，不能串联CDialog的OnOk，否则他调用EndDialog就会出错
}

void KSceneSettingDialogKeyTable::OnCancel()
{
	///什么也不做，不能串联CDialog的OnOk，否则他调用EndDialog就会出错
}

// KSceneSettingDialogKeyTable message handlers

void KSceneSettingDialogKeyTable::OnBnClickedBnDwgcKeyAdd()
{
	// TODO: Add your control notification handler code here
	IEKG3DEnvEffDynamicWeather* pDW = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_DYNAMIC_WEATHER>();
	if (pDW)
	{
		pDW->SKCaptureStateToKey(m_EnvEffHandle.m_dwType, m_EnvEffHandle.m_nIndex, NULL);
		ReloadKeyTable();
	}
}

void KSceneSettingDialogKeyTable::OnBnClickedBnDwgcKeyRemove()
{
	// TODO: Add your control notification handler code here
	int nIndex = GetCurSelData();
	if(nIndex > 0)		
	{
		IEKG3DEnvEffDWGC_KeyTable0* pTable = KSH::GetEngineAbstractInterface<KG3DTYPE_ENVEFF_KEY_TABLE>(m_EnvEffHandle.m_dwType);
		if (pTable)
		{
			pTable->RemoveKey(nIndex);
			ReloadKeyTable();
		}
	}		
	
}



void KSceneSettingDialogKeyTable::OnBnClickedBnDwgcKeyMod()
{
	// TODO: Add your control notification handler code here
	KSceneSettingDWGCKeyEdit tempKeyEdit;
	int nSel = GetCurSelData();
	if (nSel > 0)
	{
		//m_ListBox.get
		IEKG3DEnvEffect* pEnvEff = KSH::GetEngineAbstractInterface<KG3DTYPE_ENV_EFFECT>(
						m_EnvEffHandle.m_dwType
						, m_EnvEffHandle.m_nIndex);
		IEKG3DEnvEffDWGC_KeyTable0* pKeyTable = KSH::GetEngineAbstractInterface<KG3DTYPE_ENVEFF_KEY_TABLE>(m_EnvEffHandle.m_dwType);
		tempKeyEdit.Init((LPCTSTR)m_csCaption, pEnvEff, pKeyTable, nSel);
		tempKeyEdit.DoModal();
	}
}

void KSceneSettingDialogKeyTable::OnBnClickedBnDwgcKeySyn()
{
	// TODO: Add your control notification handler code here
	int nIndex = GetCurSelData();
	IEKG3DEnvEffDynamicWeather* pDW = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_DYNAMIC_WEATHER>();
	if (pDW && nIndex > 0)
	{
		pDW->SKSynStateWithKey(m_EnvEffHandle.m_dwType, m_EnvEffHandle.m_nIndex, nIndex);
	}
}
/ *
CDialog* KSceneSettingDialogKeyTable::GreatKeyEditDialog()
{
	return NULL;
}* /


void KSceneSettingDialogKeyTable::OnLbnDblclkListDwgcKeyList()
{
	// TODO: Add your control notification handler code here
	
	int nSel = GetCurSelData();
	if (nSel > 0)
	{
		
		KG_PROCESS_ERROR(m_PropertySheetHWnd && ::IsWindow(m_PropertySheetHWnd));
		{
			LRESULT lResult = ::SendMessage(m_PropertySheetHWnd, WM_IS_REGISTER_PAGE, (WPARAM)m_EnvEffHandle.m_dwType, 0/ *(LPARAM)nSel* /);
			KG_PROCESS_ERROR(lResult);
			KSceneSettingPropertySheet::StructChangePage TempStruct;
			TempStruct.m_EnvEffectHandle = m_EnvEffHandle;
			TempStruct.m_nKeyIndex= nSel;
			::SendMessage(m_PropertySheetHWnd, WM_DWGC_TO_KEY_TYPE_PAGE, (WPARAM)&TempStruct, TRUE);
		}

		
	}
	return;
Exit0:
	MessageBox(_T("不能跳转到对应的属性页。原因可能是没有对应的属性页。你可以试试直接编辑关键帧。"),_T("提示"), MB_OK | MB_ICONINFORMATION);
	return;

}*/

