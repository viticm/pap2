// KSceneModelEditorModelSaveDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorModelSaveDialog.h"
#include "KGLog.h"
#include "IEEditor.h"
#include "IEKG3DMaterial.h"
#include "iekg3dmesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorModelSaveDialog dialog


KSceneModelEditorModelSaveDialog::KSceneModelEditorModelSaveDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorModelSaveDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneModelEditorModelSaveDialog)
	m_bSaveAni = TRUE;
	m_bSaveMtl = TRUE;
	m_bSaveMesh = TRUE;
	m_CString_Ani = _T("");
	m_CString_Mtl = _T("");
	m_CString_Mesh = _T("");
	//}}AFX_DATA_INIT
	m_lpModel = NULL;
}


void KSceneModelEditorModelSaveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorModelSaveDialog)
	DDX_Check(pDX, IDC_CHECK_ANIMATION, m_bSaveAni);
	DDX_Check(pDX, IDC_CHECK_MATRIAL, m_bSaveMtl);
	DDX_Check(pDX, IDC_CHECK_MESH, m_bSaveMesh);
	DDX_Text(pDX, IDC_EDIT_ANIMATION, m_CString_Ani);
	DDX_Text(pDX, IDC_EDIT_MATERIAL, m_CString_Mtl);
	DDX_Text(pDX, IDC_EDIT_MESH, m_CString_Mesh);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneModelEditorModelSaveDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneModelEditorModelSaveDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_MESH, OnBnClickedCheckMesh)
	ON_BN_CLICKED(IDC_CHECK_MATRIAL, OnBnClickedCheckMatrial)
	ON_BN_CLICKED(IDC_CHECK_ANIMATION, OnBnClickedCheckAnimation)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorModelSaveDialog message handlers

BOOL KSceneModelEditorModelSaveDialog::OnInitDialog() 
{
	LPCTSTR pszName = NULL;
    CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(!m_lpModel)
		return TRUE;
	IEKG3DMaterial* pMat = NULL;

    m_lpModel->GetIEKG3DMaterial(&pMat);

	IEKG3DMesh* pMesh = NULL;

    m_lpModel->GetIEKG3DMesh(&pMesh);
    
	if(pMesh)	
	{
        pMesh->GetName(&pszName);
		m_CString_Mesh.Format("%s", pszName);
	}	
	if(pMat)	
	{
        pMat->GetName(&pszName);
		m_CString_Mtl .Format("%s", pszName);
	}

	if(m_CString_Ani=="")
	{
		m_bSaveAni = FALSE;
	}
	if(m_CString_Mesh=="")
	{
		m_bSaveMesh = FALSE;
	}	
	if(m_CString_Mtl=="")
	{
		m_bSaveMtl = FALSE;
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneModelEditorModelSaveDialog::OnOK() 
{
	// TODO: Add extra validation here
	HRESULT hr = E_FAIL;

	TCHAR strName[MAX_PATH];
	UpdateData(TRUE);
	
	KGLOG_PROCESS_ERROR(m_lpModel);

	if(m_bSaveMesh)
	{
		wsprintf(strName,"%s",m_CString_Mesh);
		IKG3DResourceBase* pMesh = NULL;
        m_lpModel->GetMeshIKG3DResourceBase(&pMesh);
		if(pMesh)
		{
			hr = pMesh->Release();
			KGLOG_COM_PROCESS_ERROR(hr);
			hr = pMesh->SaveToFile(strName, 0);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
	}
	if(m_bSaveMtl)
	{
		wsprintf(strName,"%s",m_CString_Mtl);
		IEKG3DMaterial* pMat = NULL;
        m_lpModel->GetIEKG3DMaterial(&pMat);
		if(pMat)
		{
			hr = pMat->SaveToFile(strName,0);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
	}

	CDialog::OnOK();

	return;
Exit0:
	return;
}

void KSceneModelEditorModelSaveDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void KSceneModelEditorModelSaveDialog::OnBnClickedCheckMesh()
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorModelSaveDialog::OnBnClickedCheckMatrial()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

}

void KSceneModelEditorModelSaveDialog::OnBnClickedCheckAnimation()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

}
