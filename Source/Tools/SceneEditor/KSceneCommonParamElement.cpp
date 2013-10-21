// KSceneCommonParamElement.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneCommonParamElement.h"
#include "KGStringFunctionsPT.h"
//#include "KGHelperClasses.h"
// KSceneCommonParamElement dialog
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(KSceneCommonParamElement, CDialog)

KSceneCommonParamElement::KSceneCommonParamElement(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneCommonParamElement::IDD, pParent)
	, m_bHasChanged(FALSE)
	, m_dwType(0xffffffff)
	, m_bHasInit(FALSE)
	, m_bHasInitialDialog(FALSE)
	, m_csData(_T(""))
	, m_csTitle(_T(""))
{
	m_bEnableEditing = TRUE;
}

KSceneCommonParamElement::~KSceneCommonParamElement()
{
}

void KSceneCommonParamElement::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_COMMON_PARAM, m_csData);
	DDX_Text(pDX, IDC_ST_COMMON_PARAM, m_csTitle);
}


BEGIN_MESSAGE_MAP(KSceneCommonParamElement, CDialog)
	ON_EN_CHANGE(IDC_ED_COMMON_PARAM, &KSceneCommonParamElement::OnEnChangeEdCommonParam)
	ON_EN_KILLFOCUS(IDC_ED_COMMON_PARAM, &KSceneCommonParamElement::OnEnKillfocusEdCommonParam)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void KSceneCommonParamElement::EnableEditing()
{
	GetDlgItem(IDC_ED_COMMON_PARAM)->EnableWindow(m_bEnableEditing);
}
// KSceneCommonParamElement message handlers
void KSceneCommonParamElement::OnEnChangeEdCommonParam()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_bHasChanged = TRUE;
}

BOOL KSceneCommonParamElement::Init( DWORD dwType, CString& csTitle, VOID* pValueBuffer, INT nValueBufferSize )
{
	KG_PROCESS_ERROR(!m_bHasInit);
	//KG_PROCESS_ERROR(m_bHasInitialDialog);
	KG_PROCESS_ERROR(KSH::ConvertorIsAValidType(dwType) && nValueBufferSize >= KSH::GetTypeSize(dwType));
	{
		m_dwType = dwType;
		m_csTitle = csTitle;
		std::tstring csValue;
		if (pValueBuffer)
		{
			BOOL bRet = KSH::ConvertValueToString(m_dwType, pValueBuffer, nValueBufferSize, csValue);
			KG_PROCESS_ERROR(bRet);
			m_csData = csValue.c_str();
		}
		m_bHasInit = TRUE;
		return TRUE;
	}

Exit0:
	_KG3D_DEBUG_FAIL_REPORT(NULL && "KSceneCommonParamElement::Init");
	return FALSE;	
}

BOOL KSceneCommonParamElement::GetData( VOID* pValueBuffer, INT nValueBufferSize, BOOL* bHasChanged )
{
	KG_PROCESS_ERROR(nValueBufferSize >= KSH::GetTypeSize(m_dwType));
	KG_PROCESS_ERROR(m_bHasInitialDialog && m_bHasInit && pValueBuffer);
	{
		enum
		{
			MAX_CHAR_INPUT = 256,
		};
		std::tstring tsData = (LPCTSTR)m_csData;
		KG_PROCESS_ERROR(KSH::ConvertStringToValue(m_dwType, tsData, pValueBuffer, nValueBufferSize));
		if (NULL != bHasChanged)
		{
			*bHasChanged = m_bHasChanged;
		}
		return TRUE;
	}

Exit0:
	_KG3D_DEBUG_FAIL_REPORT(NULL && "KSceneCommonParamElement::GetData");
	return FALSE;
}
BOOL KSceneCommonParamElement::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_bHasInitialDialog = TRUE;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneCommonParamElement::OnEnKillfocusEdCommonParam()
{
	// TODO: Add your control notification handler code here
	UpdateData();
}

void KSceneCommonParamElement::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == VK_RETURN || nChar == VK_ESCAPE)
	{
		return;
	}
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void KSceneCommonParamElement::OnOK()
{
	CWnd* p = this->GetParent();
	if (p)
	{
		::PostMessage(p->GetSafeHwnd(), WM_COMMAND,MAKELONG(IDOK, BN_CLICKED),NULL);
	}
}