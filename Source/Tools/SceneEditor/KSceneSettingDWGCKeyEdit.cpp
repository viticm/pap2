// KSceneSettingDWGCKeyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingDWGCKeyEdit.h"
#include "IEKG3DScene.h"
#include "IEKG3DEnvironment.h"
#include "IEKG3DEnvEffDynamicWeather.h"
#include "KSceneSettingINIDataEdit.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// KSceneSettingDWGCKeyEdit dialog

/*
IMPLEMENT_DYNAMIC(KSceneSettingDWGCKeyEdit, CDialog)

KSceneSettingDWGCKeyEdit::KSceneSettingDWGCKeyEdit(CWnd* pParent / *=NULL* /)
	: CDialog(KSceneSettingDWGCKeyEdit::IDD, pParent)
	, m_pEnvEffect(NULL)
	, m_pKeyTable(NULL)
	, m_nSeletedIndex(-1)
{
	ZeroMemory(m_pDataEdit, sizeof(m_pDataEdit));
}

KSceneSettingDWGCKeyEdit::~KSceneSettingDWGCKeyEdit()
{
	for (int i= 0; i < _countof(m_pDataEdit); i++)
	{
		SAFE_DELETE(m_pDataEdit[i]);
	}
}

void KSceneSettingDWGCKeyEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KSceneSettingDWGCKeyEdit, CDialog)
	ON_MESSAGE(MSG_CHILD_WINDOW_CLOSE, OnChildWindowClose)
END_MESSAGE_MAP()


// KSceneSettingDWGCKeyEdit message handlers

BOOL KSceneSettingDWGCKeyEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	typedef TCHAR TValueString[32];
	TValueString pDescription;
	TValueString pValueString;
	KSceneSettingINIDataEdit* pDataEdit = NULL;
	KG_PROCESS_ERROR(m_pEnvEffect&&m_pKeyTable);
	SetWindowText((LPCTSTR)m_CaptionTemp);	
	INT nElementCout = 0;
	KG_COM_PROCESS_ERROR(m_pKeyTable->KeyElementCount(&nElementCout));
	KG_PROCESS_ERROR(nElementCout <= MAX_EDIT_COUT);
	int nY = 0, nEditElementHeight = 0;
	
	for (int i = 0; i < nElementCout; i++)
	{
		pDataEdit = new KSceneSettingINIDataEdit;
				
		KG_PROCESS_ERROR(pDataEdit);
		
		if (m_pDataEdit[i])
		{
			m_pDataEdit[i]->DestroyWindow();
			SAFE_DELETE(m_pDataEdit[i]);
		}
		m_pDataEdit[i] = pDataEdit;
		BOOL bIsCreated = pDataEdit->Create(KSceneSettingINIDataEdit::IDD, this);
		KG_PROCESS_ERROR(bIsCreated);
		nY += nEditElementHeight;
		if (i == 0)
		{
			RECT tempRect;
			pDataEdit->GetWindowRect(&tempRect);
			nEditElementHeight = tempRect.bottom - tempRect.top;
		}
		pDataEdit->SetWindowPos(NULL, 0, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		//////////////////////////////////////////////////////////////////////////

		m_pKeyTable->GetDescription(i, pDescription, _countof(pDescription));

		if (SUCCEEDED(m_pKeyTable->ConvertValueString(m_nSeletedIndex, i, pValueString, _countof(pValueString), TRUE)))
		{
			pDataEdit->Init(pDescription, pValueString);
		}
		
	}
	if (nEditElementHeight != 0)
	{
		CRect tempRect;
		//GetWindowRect(&tempRect);
		GetClientRect(&tempRect);
		int nMyHeight = tempRect.Height();
		int nAllElementHeight = nEditElementHeight*(nElementCout+1);///要+1，不然最后一个就漏了
		if (nAllElementHeight > nMyHeight)
		{
			tempRect.bottom = tempRect.top + nAllElementHeight;
			ClientToScreen(&tempRect);
			MoveWindow(&tempRect);
			//SetWindowPos(NULL, 0, 0, 0, nAllElementHeight, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		}
	}
	return TRUE;
Exit0:
	SAFE_DELETE(pDataEdit);
	SetWindowText("Dialog初始化错误");
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

VOID KSceneSettingDWGCKeyEdit::Init( LPCTSTR tczCaption, IEKG3DEnvEffect* pEffect, IEKG3DEnvEffDWGC_KeyTable0* pKeyTable, INT nWhichKey)
{
	m_pEnvEffect = pEffect;
	m_pKeyTable = pKeyTable;
	m_CaptionTemp = tczCaption;
	m_nSeletedIndex = nWhichKey;
}
void KSceneSettingDWGCKeyEdit::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	KG_PROCESS_ERROR(m_pEnvEffect&&m_pKeyTable);
	///回收数据
	INT nElementCout = 0;
	KG_COM_PROCESS_ERROR(m_pKeyTable->KeyElementCount(&nElementCout));
	for (int i = 0; i < nElementCout; i++)
	{
		_ASSERTE(m_pDataEdit[i]);
		KSceneSettingINIDataEdit* pDataEdit = m_pDataEdit[i];
		CString TempCString = pDataEdit->StringData();
		if (pDataEdit->m_bHasMySelfChanged)
		{
			m_pKeyTable->ConvertValueString(m_nSeletedIndex, i, TempCString.GetBuffer(), TempCString.GetLength(), FALSE);
			TempCString.ReleaseBuffer();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CDialog::OnOK();
	return;
Exit0:
	CDialog::OnOK();
	MessageBox(_T("内部错误，关键帧没有正确被编辑"), _T("提示"), MB_OK);
}

LRESULT KSceneSettingDWGCKeyEdit::OnChildWindowClose( WPARAM, LPARAM )
{
	OnOK();
	return 0;
}
*/
