// KModelEditorPlayerToolsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KModelEditorPlayerToolsDialog.h"
//#include "KG3DSceneModelEditor.h"
#include "IEKG3DModelTable.h"
#include "IEKG3DModelST.h"
//#include "KG3DSceneModelEditorPlayerTools.h"
//#include "SceneEditorCommon.h"
// KModelEditorPlayerToolsDialog dialog
#include "IEEditor.h"
#include "IEKG3DMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KModelEditorPlayerToolsDialog, CDialog)

KModelEditorPlayerToolsDialog::KModelEditorPlayerToolsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KModelEditorPlayerToolsDialog::IDD, pParent)
{
	m_lpModelEditor = NULL;
	m_dwTotalPartCount = 0;
	m_dwCurrentCount = 0;
}

KModelEditorPlayerToolsDialog::~KModelEditorPlayerToolsDialog()
{

}

void KModelEditorPlayerToolsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_HEAD, m_Combo_Head);
	DDX_Control(pDX, IDC_COMBO_FACE, m_Combo_Face);
	DDX_Control(pDX, IDC_COMBO_BODY, m_Combo_Body);
	DDX_Control(pDX, IDC_COMBO_HAND, m_Combo_Hand);
	DDX_Control(pDX, IDC_COMBO_LEG1, m_Combo_Leg);
	DDX_Control(pDX, IDC_COMBO_BELT, m_Combo_Belt);
}


BEGIN_MESSAGE_MAP(KModelEditorPlayerToolsDialog, CDialog)
	ON_BN_CLICKED(IDC_LOADALL, &KModelEditorPlayerToolsDialog::OnBnClickedLoadall)
	ON_CBN_SELCHANGE(IDC_COMBO_HEAD, &KModelEditorPlayerToolsDialog::OnCbnSelchangeComboHead)
	ON_CBN_SELCHANGE(IDC_COMBO_FACE, &KModelEditorPlayerToolsDialog::OnCbnSelchangeComboFace)
	ON_CBN_SELCHANGE(IDC_COMBO_BODY, &KModelEditorPlayerToolsDialog::OnCbnSelchangeComboBody)
	ON_CBN_SELCHANGE(IDC_COMBO_HAND, &KModelEditorPlayerToolsDialog::OnCbnSelchangeComboHand)
	ON_CBN_SELCHANGE(IDC_COMBO_LEG1, &KModelEditorPlayerToolsDialog::OnCbnSelchangeComboLeg1)
	ON_CBN_SELCHANGE(IDC_COMBO_BELT, &KModelEditorPlayerToolsDialog::OnCbnSelchangeComboBelt)
	ON_BN_CLICKED(IDC_BUTTON_SEP, &KModelEditorPlayerToolsDialog::OnBnClickedButtonSep)
	ON_BN_CLICKED(IDC_BUTTON_STAND, &KModelEditorPlayerToolsDialog::OnBnClickedButtonStand)
	ON_BN_CLICKED(IDC_BUTTON_WED, &KModelEditorPlayerToolsDialog::OnBnClickedButtonWed)
	ON_BN_CLICKED(IDC_LOADMDL, &KModelEditorPlayerToolsDialog::OnBnClickedLoadmdl)
	ON_BN_CLICKED(IDC_LOADANI, &KModelEditorPlayerToolsDialog::OnBnClickedLoadani)
	ON_BN_CLICKED(IDC_LOADALL2, &KModelEditorPlayerToolsDialog::OnBnClickedLoadall2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_H, &KModelEditorPlayerToolsDialog::OnBnClickedButtonH)
	ON_BN_CLICKED(IDC_BUTTON_M, &KModelEditorPlayerToolsDialog::OnBnClickedButtonM)
	ON_BN_CLICKED(IDC_BUTTON_L, &KModelEditorPlayerToolsDialog::OnBnClickedButtonL)
END_MESSAGE_MAP()


// KModelEditorPlayerToolsDialog message handlers

void KModelEditorPlayerToolsDialog::OnBnClickedLoadall()
{
	//////////////////////////////////////////////////////////////////////////
	CFileDialog dlg(true, "ini", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型集合文件(*.ini)|*.ini||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}
	TCHAR Name[256];
	wsprintf(Name,"%s",dlg.GetPathName());
	//////////////////////////////////////////////////////////////////////////
	if(m_lpModelEditor)
	{
		m_lpModelEditor->PlayerNormalTool_AddModelFromIni(Name);
	}

	FillAllCombo();
}

void KModelEditorPlayerToolsDialog::OnCbnSelchangeComboHead()
{
	ChangeModel(ST_PART_HEAD,&m_Combo_Head,&m_vecHead);
}

void KModelEditorPlayerToolsDialog::OnCbnSelchangeComboFace()
{
	ChangeModel(ST_PART_FACE,&m_Combo_Face,&m_vecFace);
}

void KModelEditorPlayerToolsDialog::OnCbnSelchangeComboBody()
{
	ChangeModel(ST_PART_BODY,&m_Combo_Body,&m_vecBody);
}

void KModelEditorPlayerToolsDialog::OnCbnSelchangeComboHand()
{
	ChangeModel(ST_PART_HAND,&m_Combo_Hand,&m_vecHand);
}

void KModelEditorPlayerToolsDialog::OnCbnSelchangeComboLeg1()
{
	ChangeModel(ST_PART_LEG,&m_Combo_Leg,&m_vecLeg);
}

void KModelEditorPlayerToolsDialog::OnCbnSelchangeComboBelt()
{
	ChangeModel(ST_PART_BELT,&m_Combo_Belt,&m_vecBelt);
}

void KModelEditorPlayerToolsDialog::FillCombo(CComboBox* pComBo,vector<string>* pvecName)
{
	int nCount = pComBo->GetCount();
	for(int i=0;i<nCount;i++)
	{
		pComBo->DeleteString(0);
	}
	//////////////////////////////////////////////////////////////////////////
	for(int i=0;i<(int)pvecName->size();i++)
	{
		string& name = (*pvecName)[i];
		int nIndex = pComBo->AddString(name.c_str());
		pComBo->SetItemData(nIndex,(DWORD)i);
	}
}

void KModelEditorPlayerToolsDialog::FillAllCombo()
{
	m_lpModelEditor->PlayerNormalTool_GetModelPartVector(ST_PART_BODY,&m_vecBody);
	m_lpModelEditor->PlayerNormalTool_GetModelPartVector(ST_PART_HEAD,&m_vecHead);
	m_lpModelEditor->PlayerNormalTool_GetModelPartVector(ST_PART_HAND,&m_vecHand);
	m_lpModelEditor->PlayerNormalTool_GetModelPartVector(ST_PART_BELT,&m_vecBelt);
	m_lpModelEditor->PlayerNormalTool_GetModelPartVector(ST_PART_LEG ,&m_vecLeg );
	m_lpModelEditor->PlayerNormalTool_GetModelPartVector(ST_PART_FACE,&m_vecFace);

	FillCombo(&m_Combo_Head,&m_vecHead);
	FillCombo(&m_Combo_Face,&m_vecFace);
	FillCombo(&m_Combo_Body,&m_vecBody);
	FillCombo(&m_Combo_Hand,&m_vecHand);
	FillCombo(&m_Combo_Belt,&m_vecBelt);
	FillCombo(&m_Combo_Leg ,&m_vecLeg);
}

//void KModelEditorPlayerToolsDialog::LoadPartFromIni(LPSTR pFileName)
//{
//	IIniFile* pFile = NULL;
//	int nIndex = 0;
//
//	//KGLOG_PROCESS_ERROR(pFileName);
//
//	TCHAR path_buffer[256];
//	TCHAR drive[256];
//	TCHAR dir[256];
//	TCHAR fname[256];
//	TCHAR ext[256];
//	TCHAR BaseDir[256];
//
//	wsprintf(path_buffer,"%s",pFileName);
//	_splitpath( path_buffer, drive, dir, fname, ext );
//	wsprintf(BaseDir,"%s%s",drive,dir);
//	m_szModelBaseDir = BaseDir;
//
//	pFile = g_OpenIniFile(pFileName, false, true);
//	if(!pFile)
//		return;
//	//KGLOG_PROCESS_ERROR(pFile);
//
//	m_vecHead.clear();
//	m_vecFace.clear();
//	m_vecBody.clear();
//	m_vecHand.clear();
//	m_vecLeg.clear();
//	m_vecBelt.clear();
//	m_vecMDL.clear();
//
//	for(nIndex=0;nIndex<=1000;nIndex++)
//	{
//		TCHAR SectionName[256];
//		TCHAR IDName[256];
//		TCHAR ModelName[256];
//
//		wsprintf(SectionName,"Name%d",nIndex);
//		if(pFile->IsSectionExist(SectionName))
//		{
//			pFile->GetString(SectionName,"ID","",IDName,256);
//
//			if(_strcmpi(IDName,"")!=0)
//			{
//				wsprintf(ModelName,"%s",IDName);
//				m_vecMDL.push_back(ModelName);
//
//				wsprintf(ModelName,"%s_head",IDName);
//				m_vecHead.push_back(ModelName);
//
//				wsprintf(ModelName,"%s_body",IDName);
//				m_vecBody.push_back(ModelName);
//
//				wsprintf(ModelName,"%s_hand",IDName);
//				m_vecHand.push_back(ModelName);
//
//				wsprintf(ModelName,"%s_belt",IDName);
//				m_vecBelt.push_back(ModelName);
//
//				wsprintf(ModelName,"%s_leg",IDName);
//				m_vecLeg.push_back(ModelName);
//
//				wsprintf(ModelName,"%s_face",IDName);
//				m_vecFace.push_back(ModelName);
//			}
//		}
//		else
//			break;
//	}
//
//	pFile->Release();
//}

//void KModelEditorPlayerToolsDialog::SetModel(IEKG3DModel** ppModel,LPCSTR pModelName)
//{
//	HRESULT hr = S_OK;
//	TCHAR MeshName[256];
//	TCHAR MtlName[256];
//    IEKG3DModelTable *pModelTable = NULL;
//    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
//	if ((*ppModel))
//	{
//		(*ppModel)->Release();
//		(*ppModel) = NULL;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	wsprintf(MeshName,"%s%s.Mesh",m_szModelBaseDir.c_str(),pModelName);
//	wsprintf(MtlName ,"%s%s.Mtl" ,m_szModelBaseDir.c_str(),pModelName);
//    //Get3DEngineInterface((void **)(&pManager));
//    pManager->GetIEKG3DModelTable(&pModelTable);
//    hr = pModelTable->IELoadResourceFromFile(MeshName,0,0,(IKG3DResourceBase**)ppModel);
//	KGLOG_COM_PROCESS_ERROR(hr);
//
//	return;
//Exit0:
//	return;
//}

void KModelEditorPlayerToolsDialog::ChangeModel(int nPos,CComboBox* pCombox,vector<string>*plist)
{
	int nIndex = pCombox->GetCurSel();
	m_lpModelEditor->PlayerNormalTool_SetCurrentModelPart(nPos,nIndex);

	/*if ((nIndex<0)||(nIndex>=(int)plist->size()))
		return;

	if(m_lpModelST)
	{
		SetModel(ppModel,(*plist)[nIndex].c_str());

		m_lpModelST->IEChangeModel((DWORD)nPos,(*ppModel));
	}*/
}

void KModelEditorPlayerToolsDialog::OnBnClickedButtonSep()
{


}

void KModelEditorPlayerToolsDialog::OnBnClickedButtonStand()
{
	CFileDialog dlg(true, "Mesh", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型文件(*.Mesh)|*.Mesh||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}
	TCHAR Name[256];
	wsprintf(Name,"%s",dlg.GetPathName());

	//m_lpModelEditor->m_lpPlayerTools->LoadStandModel(Name);
	m_lpModelEditor->PlayerNormalTool_LoadStandModel(Name);//by dengzhihui Refactor 2006年11月24日
}


//HRESULT KModelEditorPlayerToolsDialog::CreateSphereVertexIndexFromModel(VFormat::FACES_NORMAL_TEXTURE1* pVer,int nNumVertex)
//{
//	KGLOG_PROCESS_ERROR(pVer);
//
//	m_vecStandVertexIndex.clear();
//
//	for (int i=0;i<(int)m_vecSplitSphere.size();i++)
//	{
//		SphereVertexIndex vIndex;
//
//		Sphere& she = m_vecSplitSphere[i];
//
//		for(int j=0;j<nNumVertex;j++)
//		{
//			if(she.IsPositionIn(pVer[j].p))
//			{
//				vIndex.vecIndex.push_back(j);
//			}
//		}
//
//		m_vecStandVertexIndex.push_back(vIndex);
//
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	for (int i=0;i<(int)m_vecStandVertexIndex.size();i++)
//	{
//		SphereVertexIndex& vIndex = m_vecStandVertexIndex[i];
//		//ASSERT(vIndex.vecIndex.size()!=0);
//	}
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

//HRESULT KModelEditorPlayerToolsDialog::CreateSphereVertexIndex(IEKG3DModel* pModel)
//{
//	HRESULT hr = S_OK;
//	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
//	DWORD dwFVF = 0;
//	DWORD dwNumVertex = 0;
//    //IEKG3DMeshBase *piMeshBase = NULL;
//    IEKG3DMesh* pMesh = NULL;
//    BOOL bMeshSys = FALSE;
//
//	KGLOG_PROCESS_ERROR(pModel);
//
//    pModel->GetIEKG3DMesh(&pMesh);
//    KGLOG_PROCESS_ERROR(pMesh);
//
//    pMesh->IsMeshSysNotNull(&bMeshSys);
//    KGLOG_PROCESS_ERROR(bMeshSys);
//
//	pMesh->GetMeshSysFVF(&dwFVF);
//	KGLOG_PROCESS_ERROR(dwFVF==(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));
//
//	pMesh->GetMeshSysNumVertices(&dwNumVertex);
//
//    hr = pMesh->LockVertexBufferOfMesfSys(0, (void**)&pVers);
//    KGLOG_COM_PROCESS_ERROR(hr);
//
//	CreateSphereVertexIndexFromModel(pVers,(int)dwNumVertex);
//
//    hr = pMesh->UnLockVertexBufferOfMesfSys();
//    KGLOG_COM_PROCESS_ERROR(hr);
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

//HRESULT KModelEditorPlayerToolsDialog::WedVertexForModel(IEKG3DModel* pModel,
//														 VFormat::FACES_NORMAL_TEXTURE1* pVerStand,D3DXMATRIX* pmat)
//{
////	HRESULT hr = S_OK;
////	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
////	DWORD dwFVF = 0;
////	DWORD dwNumVertex = 0;
////	D3DXMATRIX matInverse;
////	D3DXVECTOR3 Pos;
////    //IEKG3DMeshBase *piMeshBase = NULL;
////    IEKG3DMesh* pMesh = NULL;
////    BOOL bMeshSys = FALSE;
////    DWORD dwSize = 0;
////
////	D3DXMatrixInverse(&matInverse,NULL,pmat);
////	KGLOG_PROCESS_ERROR(pModel);
////
////    pModel->GetIEKG3DMesh(&pMesh);
////    KGLOG_PROCESS_ERROR(pMesh);
////
////    pMesh->IsMeshSysNotNull(&bMeshSys);
////    KGLOG_PROCESS_ERROR(bMeshSys);
////
////	pMesh->GetMeshSysFVF(&dwFVF);
////	KGLOG_PROCESS_ERROR(dwFVF==(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));
////
////	pMesh->GetMeshSysNumVertices(&dwNumVertex);
////
////    hr = pMesh->LockVertexBufferOfMesfSys(0, (void**)&pVers);
////    KGLOG_COM_PROCESS_ERROR(hr);
////	//////////////////////////////////////////////////////////////////////////
////	for(DWORD i = 0; i < dwNumVertex; i++)
////	{
////        dwSize = (DWORD)m_vecSplitSphere.size();
////		for(DWORD k = 0; k < dwSize; k++)
////		{
////			Sphere& she = m_vecSplitSphere[k];
////			SphereVertexIndex& vIndex = m_vecStandVertexIndex[k];
////
////			D3DXVec3TransformCoord(&Pos,&pVers[i].p,pmat);
////
////			if(she.IsPositionIn(Pos))
////			{
////				//ASSERT(vIndex.vecIndex.size()!=0);
////				//int nIndex = vIndex.vecIndex[0];
////
////				//D3DXVec3TransformNormal(&pVers[i].Normal,&pVerStand[nIndex].Normal,&matInverse);
////				pVers[i].Normal = D3DXVECTOR3(1,0,0);
////			}
////		}
////	}
////	//////////////////////////////////////////////////////////////////////////
////    hr = pMesh->UnLockVertexBufferOfMesfSys();
////    KGLOG_COM_PROCESS_ERROR(hr);
////
////	return S_OK;
////Exit0:
//	return E_FAIL;
//}

//HRESULT KModelEditorPlayerToolsDialog::WedVertex()
//{
////	HRESULT hr = S_OK;
////	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
////	DWORD dwFVF = 0;
////	DWORD dwNumVertex = 0;
////	static D3DXMATRIX matId;
////	D3DXMatrixIdentity(&matId);
////    IKG3DResourceBase *piMeshResource = NULL;
////    IEKG3DMesh *pieMesh = NULL;
////    LPCTSTR pszName = NULL;
////    D3DXMATRIX *pMatWorld = NULL;
////
////	//m_lpModelEditor->m_lpPlayerTools->m_vecFoundPos.clear();
////	m_lpModelEditor->ClearFoundPos();//by dengzhihui Refactor 2006年11月24日
////
////	//WedVertexForModel(m_lpModelHead,pVers,&matId);
////	//WedVertexForModel(m_lpModelBody,pVers,&matId);
////
////	if(m_lpModelHead)
////	{
////		//m_lpModelEditor->m_lpPlayerTools->WedVertexForModel(m_lpModelHead,&matId);
////		m_lpModelEditor->WedVertexForModel(m_lpModelHead,&matId);
////	}
////	if(m_lpModelHand)
////	{
////		//m_lpModelEditor->m_lpPlayerTools->WedVertexForModel(m_lpModelHand,&matId);
////		m_lpModelEditor->WedVertexForModel(m_lpModelHand,&matId);
////	}
////	if(m_lpModelBody)
////	{
////		//m_lpModelEditor->m_lpPlayerTools->WedVertexForModel(m_lpModelBody,&matId);
////		m_lpModelEditor->WedVertexForModel(m_lpModelBody,&matId);
////	}
////
////	if(m_lpModelFace)
////	{
////        hr = m_lpModelFace->GetMatWorld(&pMatWorld);
////		KGLOG_COM_PROCESS_ERROR(hr);
////
////                //m_lpModelEditor->m_lpPlayerTools->WedVertexForModel(m_lpModelFace, pMatWorld);
////		//m_lpModelEditor->m_lpPlayerTools->WedVertexForModel(m_lpModelFace,&m_lpModelFace->m_matWorld);
////		m_lpModelEditor->WedVertexForModel(m_lpModelFace,pMatWorld);
////	}
////
////
////
////	if(m_lpModelBody)
////	{
////        piMeshResource = NULL;
////        m_lpModelBody->GetMeshIKG3DResourceBase(&piMeshResource);
////        if (piMeshResource)
////        {
////            m_lpModelBody->GetIEKG3DMesh(&pieMesh);
////            pieMesh->GetName(&pszName);
////
////            piMeshResource->SaveToFile(pszName, 0);
////            piMeshResource->LoadFromFile(pszName, 0, 0);
////        }
////	}
////	if(m_lpModelFace)
////	{
////        piMeshResource = NULL;
////        m_lpModelFace->GetMeshIKG3DResourceBase(&piMeshResource);
////        if (piMeshResource)
////        {
////            m_lpModelFace->GetIEKG3DMesh(&pieMesh);
////            pieMesh->GetName(&pszName);
////
////            piMeshResource->SaveToFile(pszName, 0);
////            piMeshResource->LoadFromFile(pszName, 0, 0);
////        }
////
////	}
////    if(m_lpModelHead)
////    {
////        piMeshResource = NULL;
////        m_lpModelHead->GetMeshIKG3DResourceBase(&piMeshResource);
////        if (piMeshResource)
////        {
////            m_lpModelHead->GetIEKG3DMesh(&pieMesh);
////            pieMesh->GetName(&pszName);
////
////            piMeshResource->SaveToFile(pszName, 0);
////            piMeshResource->LoadFromFile(pszName, 0, 0);
////        }
////
////    }
////    if(m_lpModelHand)
////    {
////        piMeshResource = NULL;
////        m_lpModelHand->GetMeshIKG3DResourceBase(&piMeshResource);
////        if (piMeshResource)
////        {
////            m_lpModelHand->GetIEKG3DMesh(&pieMesh);
////            pieMesh->GetName(&pszName);
////
////            piMeshResource->SaveToFile(pszName, 0);
////            piMeshResource->LoadFromFile(pszName, 0, 0);
////        }
////
////    }
////	return S_OK;
////Exit0:
////	return E_FAIL;
//return E_NOTIMPL;
//}

void KModelEditorPlayerToolsDialog::OnBnClickedButtonWed()
{
	//m_dwCurrentCount = 0;
	//m_dwTotalPartCount = m_vecHead.size() + m_vecFace.size() + m_vecBody.size() + m_vecHand.size() + m_vecLeg.size() + m_vecBelt.size();

	//this->SetTimer(0, 500, NULL);
}

void KModelEditorPlayerToolsDialog::OnBnClickedLoadmdl()
{
	CFileDialog dlg(true, "Mdl", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型文件(*.mdl)|*.mdl||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}
	TCHAR Name[256];
	wsprintf(Name,"%s",dlg.GetPathName());	
	
	m_lpModelEditor->PlayerNormalTool_AddMDL(Name);//by dengzhihui Refactor 2006年11月24日


	//this->m_lpModelEditor->get


 //   IEKG3DModelTable *pModelTable = NULL;
 //   IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	//if(m_lpModelST)
	//{
 //       IKG3DModel *pModel = NULL;
 //       m_lpModelST->GetIKG3DModel(&pModel);

	//	IEKG3DScene* pScene = NULL;
	//	m_lpModelEditor->GetScene(&pScene);
	//	_ASSERTE(pScene);
 //       pScene->RemoveRenderEntity(SCENEENTITY_MODEL, m_lpModelST);
	//}
	//SAFE_RELEASE(m_lpModelST);

 //   //Get3DEngineInterface((void **)(&pManager));
	//IKG3DModel *piModel = NULL;
 //   pManager->GetIEKG3DModelTable(&pModelTable);
 //   pModelTable->IELoadResourceFromFile(Name,0,0,(IKG3DResourceBase**)&piModel);

	//m_lpModelST->GetIKG3DModel(&piModel);


	//if(m_lpModelST)
	//{
 //       IKG3DModel *pModel = NULL;
 //       m_lpModelST->GetIKG3DModel(&pModel);

 //       m_lpModelEditor->AddRenderEntity(SCENEENTITY_MODEL, m_lpModelST);
	//}
}

void KModelEditorPlayerToolsDialog::OnBnClickedLoadani()
{
	//if(!m_lpModelST)
	//	return;

	//CFileDialog dlg(true, "Ani", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型文件动画(*.ani)|*.ani||");
	//if(dlg.DoModal() != IDOK)
	//{
	//	return;
	//}
	//TCHAR Name[256];
	//wsprintf(Name,"%s",dlg.GetPathName());

	//m_lpModelST->LoadAnimationFromFile(Name,0);
}

void KModelEditorPlayerToolsDialog::OnBnClickedLoadall2()
{
	// TODO: Add your control notification handler code here
}

void KModelEditorPlayerToolsDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	DWORD K1 = (DWORD)m_vecHead.size();
	DWORD K2 = (DWORD)m_vecFace.size() + K1;
	DWORD K3 = (DWORD)m_vecBody.size() + K2;
	DWORD K4 = (DWORD)m_vecHand.size() + K3;
	DWORD K5 = (DWORD)m_vecLeg.size() + K4;
	DWORD K6 = (DWORD)m_vecBelt.size() + K5;

	if(m_dwCurrentCount < K1)
	{
		m_lpModelEditor->PlayerNormalTool_SetCurrentModelPart(ST_PART_HEAD,m_dwCurrentCount);
	}
	else if( m_dwCurrentCount < K2)
	{
		m_lpModelEditor->PlayerNormalTool_SetCurrentModelPart(ST_PART_FACE,m_dwCurrentCount - K1);
	}
	else if( m_dwCurrentCount < K3)
	{
		m_lpModelEditor->PlayerNormalTool_SetCurrentModelPart(ST_PART_BODY,m_dwCurrentCount - K2);
	}
	else if( m_dwCurrentCount < K4)
	{
		m_lpModelEditor->PlayerNormalTool_SetCurrentModelPart(ST_PART_HAND,m_dwCurrentCount - K3);
	}
	else if( m_dwCurrentCount < K5)
	{
		m_lpModelEditor->PlayerNormalTool_SetCurrentModelPart(ST_PART_LEG ,m_dwCurrentCount - K4);
	}
	else if( m_dwCurrentCount < K6)
	{
		m_lpModelEditor->PlayerNormalTool_SetCurrentModelPart(ST_PART_BELT,m_dwCurrentCount - K5);
	}

	TCHAR szName[MAX_PATH];
	wsprintf(szName,"Process %d of %d ",m_dwCurrentCount,m_dwTotalPartCount);
	this->SetWindowText(szName);

	m_dwCurrentCount++;
	
	if(m_dwCurrentCount>=m_dwTotalPartCount)
		KillTimer(0);

	CDialog::OnTimer(nIDEvent);
}

void KModelEditorPlayerToolsDialog::OnBnClickedButtonH()
{
	// TODO: Add your control notification handler code here
}

void KModelEditorPlayerToolsDialog::OnBnClickedButtonM()
{
	// TODO: Add your control notification handler code here
}

void KModelEditorPlayerToolsDialog::OnBnClickedButtonL()
{
	// TODO: Add your control notification handler code here
}
