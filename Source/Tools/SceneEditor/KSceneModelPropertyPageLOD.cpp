// KSceneModelPropertyPageLOD.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneModelPropertyPageLOD.h"
#include ".\kscenemodelpropertypagelod.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KSceneModelPropertyPageLOD dialog

IMPLEMENT_DYNAMIC(KSceneModelPropertyPageLOD, CPropertyPage)
KSceneModelPropertyPageLOD::KSceneModelPropertyPageLOD()
	: CPropertyPage(KSceneModelPropertyPageLOD::IDD)
	, m_fWeight(0)
{
	m_pCurEditor = NULL;
	m_pCurModel = NULL;
	m_fWeight = 1.0f;
}

KSceneModelPropertyPageLOD::~KSceneModelPropertyPageLOD()
{
}

void KSceneModelPropertyPageLOD::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorPropertyPageMesh)
	DDX_Text(pDX, IDC_EDIT_WEIGHT, m_fWeight);
	DDX_Control(pDX, IDC_SPIN_WEIGHT, m_ctlWeight);
	DDX_Control(pDX, IDC_LIST_VERTEX, m_ctlVerticesList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneModelPropertyPageLOD, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_RESET2, OnBnClickedButtonReset2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WEIGHT, OnDeltaposSpinWeight)
	ON_EN_CHANGE(IDC_EDIT_WEIGHT, OnEnChangeEditWeight)
	ON_BN_CLICKED(IDC_BUTTON_SELECTALL, OnBnClickedButtonSelectall)
	ON_BN_CLICKED(IDC_BUTTON_REVSEL, OnBnClickedButtonRevsel)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SETWEIGHT, OnBnClickedButtonSetweight)
	ON_LBN_SELCHANGE(IDC_LIST_VERTEX, OnLbnSelchangeListVertex)
END_MESSAGE_MAP()


// KSceneModelPropertyPageLOD message handlers
void KSceneModelPropertyPageLOD::SetCurModel(KSceneModelEditor* pModelEditor,LPMODEL pCurModel)
{
	m_pCurEditor = pModelEditor;

	if(m_pCurModel!=pCurModel)
	{
		m_pCurModel = pCurModel;
	}
}

void KSceneModelPropertyPageLOD::OnBnClickedButtonReset()
{
	if(m_pCurModel)
	{
		m_pCurModel->m_VertiesOp.SetSelectVerticesWeights(1.0f);
	}
}

void KSceneModelPropertyPageLOD::OnBnClickedButtonReset2()
{
	if(m_pCurModel)
	{
		for(DWORD i = 0; i < m_pCurModel->m_VertiesOp.m_dwNumActVertices; i++)
		{
			m_pCurModel->m_VertiesOp.SetVertexWeight(i, 1.0f);
		}
	}
}

void KSceneModelPropertyPageLOD::OnDeltaposSpinWeight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_fWeight = pNMUpDown->iPos / 100.0f;
	UpdateData(FALSE);
	*pResult = 0;
}

void KSceneModelPropertyPageLOD::OnEnChangeEditWeight()
{
	UpdateData();
	m_ctlWeight.SetPos((int)(m_fWeight * 100.0f));
}

BOOL KSceneModelPropertyPageLOD::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_ctlWeight.SetRange(0, 100);
	m_ctlWeight.SetPos(100);
    m_fWeight = 1.0f;
	FillListBox();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneModelPropertyPageLOD::FillListBox(int nIndex /*= 0*/)
{
	TCHAR strVertexIndex[256];
	if(m_pCurModel)
	{
		int nNumItem = m_ctlVerticesList.GetCount();
		for(int i = nNumItem - 1; i >= 0; i--)
			m_ctlVerticesList.DeleteString(i);
		for(DWORD i = 0; i < m_pCurModel->m_VertiesOp.m_dwNumVertices; i++ )
		{
			if(m_pCurModel->m_VertiesOp.m_pSelectedVertices[i])
			{
				sprintf(strVertexIndex, "Vertex %d", i);
				m_ctlVerticesList.AddString(strVertexIndex);
				int nIndex = m_ctlVerticesList.GetCount() - 1;
				m_ctlVerticesList.SetItemData(nIndex, (DWORD_PTR)i);
			}
		}
	}
}


void KSceneModelPropertyPageLOD::OnBnClickedButtonSelectall()
{
	if(m_pCurModel)
	{
		int nNumItem = m_ctlVerticesList.GetCount();
		m_ctlVerticesList.SelItemRange(TRUE, 0, nNumItem - 1);
		memcpy(m_pCurModel->m_VertiesOp.m_pEditorSelect, m_pCurModel->m_VertiesOp.m_pSelectedVertices, sizeof(BOOL) * m_pCurModel->m_VertiesOp.m_dwNumVertices);
	}
}

void KSceneModelPropertyPageLOD::OnBnClickedButtonRevsel()
{
	if(m_pCurModel)
	{
		UpdateData();
		int nNumItem = m_ctlVerticesList.GetCount();
		int nNumSelItem = m_ctlVerticesList.GetSelCount();
		int nRevNumSelItem = m_pCurModel->m_VertiesOp.m_dwNumVertices - nNumSelItem;
		int* pSel = new int[nNumSelItem];
		int* pRevSel = new int[nRevNumSelItem];
		memset(m_pCurModel->m_VertiesOp.m_pEditorSelect, 0, sizeof(BOOL) * m_pCurModel->m_VertiesOp.m_dwNumVertices);
		m_ctlVerticesList.GetSelItems(nNumItem, pSel);
		m_ctlVerticesList.SelItemRange(TRUE, 0, nNumItem - 1);
		for(int i = 0; i < nNumSelItem; i++)
		{
			m_ctlVerticesList.SetSel(pSel[i], FALSE);
		}
		m_ctlVerticesList.GetSelItems(nNumItem, pRevSel);
		for(int i = 0; i < nNumSelItem; i++)
		{
			int nIndex = m_ctlVerticesList.GetItemData(pRevSel[i]);
			m_pCurModel->m_VertiesOp.m_pEditorSelect[nIndex] = TRUE;
		}
		SAFE_DELETE_ARRAY(pRevSel);
		SAFE_DELETE_ARRAY(pSel);
	}
}

void KSceneModelPropertyPageLOD::OnBnClickedButtonClear()
{
	OnBnClickedButtonSelectall();
	OnBnClickedButtonRevsel();
}

void KSceneModelPropertyPageLOD::OnBnClickedButtonSetweight()
{
	if(m_pCurModel)
	{
		UpdateData();
		int nNumItem = m_ctlVerticesList.GetCount();
		int nNumSelItem = m_ctlVerticesList.GetSelCount();
		int* pSel = new int[nNumSelItem];
		m_ctlVerticesList.GetSelItems(nNumItem, pSel);
		for(int i = 0;i < nNumSelItem; i++)
		{
			int nIndex = m_ctlVerticesList.GetItemData(pSel[i]);
			m_pCurModel->m_VertiesOp.SetVertexWeight(nIndex, m_fWeight);
		}
		SAFE_DELETE_ARRAY(pSel);
	}
}

void KSceneModelPropertyPageLOD::OnLbnSelchangeListVertex()
{
	if(m_pCurModel)
	{
		UpdateData();
		int nNumItem = m_ctlVerticesList.GetCount();
		int nNumSelItem = m_ctlVerticesList.GetSelCount();
		int* pSel = new int[nNumSelItem];
		m_ctlVerticesList.GetSelItems(nNumItem, pSel);
		memset(m_pCurModel->m_VertiesOp.m_pEditorSelect, 0, sizeof(BOOL) * m_pCurModel->m_VertiesOp.m_dwNumVertices);
		for(int i = 0; i < nNumSelItem; i++)
		{
			int nIndex = m_ctlVerticesList.GetItemData(pSel[i]);
			m_pCurModel->m_VertiesOp.m_pEditorSelect[nIndex] = TRUE;
		}
		SAFE_DELETE_ARRAY(pSel);
	}
}
