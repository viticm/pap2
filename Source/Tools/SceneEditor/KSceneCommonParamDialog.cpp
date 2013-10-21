// KSceneCommonParamDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneCommonParamDialog.h"
#include "KGHelperClasses.h"
#include "KGStringFunctionsPT.h"
//#include "KSceneInterfaceGetters.h"
#include "KSceneCommonParamElement.h"
#include "KSceneSceneHelpers.h"
// KSceneCommonParamDialog dialog
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(KSceneCommonParamDialog, CDialog)

KSceneCommonParamDialog::KSceneCommonParamDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneCommonParamDialog::IDD, pParent)
	, m_bHasInitDialog(FALSE)
{

}

KSceneCommonParamDialog::~KSceneCommonParamDialog()
{
	std::for_each(m_vecElement.begin(), m_vecElement.end(), KSH::CWndDestroyer);
	KG_CUSTOM_HELPERS::TContainerDelete(m_vecElement);
	///剩下没有用指针的，让自己析构好了。

}

void KSceneCommonParamDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KSceneCommonParamDialog, CDialog)
END_MESSAGE_MAP()

INT KSceneCommonParamDialog::AddAnParam( DWORD dwType, CString csTitle, const VOID* pValue, INT nValueBufferSize,BOOL bEnable )
{
	char* pValueBuffer = NULL;
	KSceneCommonParamElement* pElement = NULL;
	int nTypeSize = INT_MAX;
	size_t uParamCount = m_vecElement.size();
	
	KG_PROCESS_ERROR(!m_bHasInitDialog);///InitDialog之后就不能再添加了，因为这样也显示不出来
	KG_PROCESS_ERROR(KSH::ConvertorIsAValidType(dwType));
	{
		nTypeSize = KSH::GetTypeSize(dwType);
		KG_PROCESS_ERROR(nTypeSize <= MAX_VALUE_SIZE);
	}
	if (pValue)
	{
		KG_PROCESS_ERROR(nValueBufferSize >= nTypeSize);
	}	
	KG_PROCESS_ERROR(uParamCount <= MAX_PARAM_COUNT && nTypeSize <= MAX_VALUE_SIZE);
	{
		//_KG3D_DEBUG_SELF_ASSERTE(_CrtCheckMemory());//有内存泄露
		KSceneCommonParamElement* pElement = new KSceneCommonParamElement;
		//_KG3D_DEBUG_SELF_ASSERTE(_CrtCheckMemory());
		KG_PROCESS_ERROR(pElement);


		int nBufferSize = nTypeSize;
		if (nBufferSize % sizeof(DWORD_PTR) != 0)
		{
			int nRest = (nBufferSize/sizeof(DWORD_PTR));
			nBufferSize =  (nRest+1) * sizeof(DWORD_PTR);
		}
		pValueBuffer = new char[nBufferSize];

		KG_PROCESS_ERROR(pValueBuffer);
		
		BOOL bRet = KSH::ConvertorMakeObjectInBuffer(dwType, pValueBuffer, nBufferSize);
		KG_PROCESS_ERROR(bRet);
		if (pValue)
		{	
			BOOL bRet = KSH::ConvertorTypeCopy(dwType, pValue, pValueBuffer, nBufferSize);
			KG_PROCESS_ERROR(bRet);	
		}
		else///默认值
		{
			
		}
		KG_PROCESS_ERROR(pElement->Init(dwType, csTitle, pValueBuffer, nTypeSize));
		SAFE_DELETE_ARRAY(pValueBuffer);
		m_vecElement.push_back(pElement);

		pElement->m_bEnableEditing = bEnable;

		size_t uNewCount = m_vecElement.size();
		KG_PROCESS_ERROR(uNewCount == uParamCount+1);
		return (INT)uParamCount;///这是序号，所以从0开始
	}	
Exit0:
	SAFE_DELETE(pElement);
	SAFE_DELETE_ARRAY(pValueBuffer);
	return -1;
}

// KSceneCommonParamDialog message handlers

BOOL KSceneCommonParamDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_bHasInitDialog = TRUE;

	size_t uParamCount = m_vecElement.size();
		
	int nParamCount = static_cast<int>(uParamCount);
	int nHeight = 0, nWidth = 0, nY = 0;
	for (int i = 0; i < nParamCount; i++)
	{
		KSceneCommonParamElement* pElement = m_vecElement[i];
		_ASSERTE(pElement);
		BOOL bRet = pElement->Create(KSceneCommonParamElement::IDD, this);
	
		if (bRet)
		{	
			if (i == 0)
			{
				CRect ARect;
				pElement->GetWindowRect(&ARect);
				nHeight = ARect.Height();
				nWidth = ARect.Width();
			}
			pElement->SetWindowPos(NULL, 0, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			nY += nHeight;
			pElement->EnableEditing();
		}
	}
	///防缩整个窗口
	INT nTotalHeightOfElements = nHeight * nParamCount;
	CWnd* pButtonOk = GetDlgItem(IDOK);
	CWnd* pButtonCancel = GetDlgItem(IDCANCEL);
	INT nOkButtonHeight = 0;
	if (pButtonOk)
	{
		nOkButtonHeight = KSH::CWndGetWindowHeight(pButtonOk);
		KSH::CWndMoveY(pButtonOk, nTotalHeightOfElements, TRUE, this);
	}
	if (pButtonCancel)
	{
		KSH::CWndMoveY(pButtonCancel, nTotalHeightOfElements, TRUE, this);
	}
	INT nNewHeight = nOkButtonHeight+nTotalHeightOfElements;
	KSH::CWndReSizeClient(this, nWidth, nNewHeight, TRUE);

//Exit0:
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL KSceneCommonParamDialog::GetData( INT nIndex, DWORD dwType, VOID* pValueBuffer, INT nValueBufferSize, BOOL* bHasChanged )
{
	KG_PROCESS_ERROR(nIndex < (INT)m_vecElement.size());
	KSceneCommonParamElement* pElement = m_vecElement[nIndex];
	{
		KG_PROCESS_ERROR(dwType == pElement->GetType());
		KG_PROCESS_ERROR(pElement->IsSucceed());
		return pElement->GetData(pValueBuffer, nValueBufferSize, bHasChanged);
	}
Exit0:
	return FALSE;
}

void KSceneCommonParamDialog::OnOK()
{
	return CDialog::OnOK();
}