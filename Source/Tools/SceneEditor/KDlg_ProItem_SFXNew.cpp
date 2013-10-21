// KDlg_ProItem_SFXNew.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_SFXNew.h"
//#include "MeshBlade.h"
//#include "KModelSFXBillBoard.h"
//#include "KModelBelt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_SFXNew dialog

namespace PropertyBase
{
KDlg_ProItem_SFXNew::KDlg_ProItem_SFXNew(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_SFXNew::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_SFXNew)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void KDlg_ProItem_SFXNew::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_SFXNew)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_SFXNew, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_SFXNew)
	ON_BN_CLICKED(IDC_BBILL, OnBbill)
	ON_BN_CLICKED(IDC_BBLADE, OnBblade)
	ON_BN_CLICKED(IDC_BPART, OnBpart)
	ON_BN_CLICKED(IDC_BBELT, OnBbelt)
	ON_BN_CLICKED(IDC_BLVF, OnBLVF)
	ON_BN_CLICKED(IDC_BDL, OnBDL)
	ON_BN_CLICKED(IDC_BST, OnBST)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_SFXNew message handlers

void KDlg_ProItem_SFXNew::OnBbill() 
{
	// TODO: Add your control notification handler code here
	//if(!m_lpProperty)
	//	return;
	//KSceneModelEditor* pScene = (KSceneModelEditor*)
	//	m_lpProperty->Address;
	//if(pScene)
	//{
	//	ModelSFX::KModelSFXBillBoard* pBillboard = NULL;

	//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pBillboard),MODELTYPE_BILLBOARD);
	//	pBillboard->LoadMesh("");

	//	pScene->AddRenderMesh(pBillboard);
	//	pScene->AddSelectableMesh(pBillboard);

	//}
}

void KDlg_ProItem_SFXNew::OnBblade() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpProperty)
		return;
	KSceneModelEditor* pScene = (KSceneModelEditor*)
		m_lpProperty->Address;
	if(pScene)
	{
		ModelSFX::KModelSFXBlade* pModel = NULL;

		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pModel),MODELTYPE_BLADE);
		pModel->LoadMesh("");

		pScene->AddRenderMesh(pModel);
		pScene->AddSelectableMesh(pModel);

	}*/
}

void KDlg_ProItem_SFXNew::OnBpart() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpProperty)
		return;
	KSceneModelEditor* pScene = (KSceneModelEditor*)
		m_lpProperty->Address;
	if(pScene)
	{
		ModelSFX::KModelSFXParticleSystem* pModel = NULL;

		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pModel),MODELTYPE_PARTICLESYSTEM);
		pModel->LoadMesh("");

		pScene->AddRenderMesh(pModel);
		pScene->AddSelectableMesh(pModel);

	}*/
}

void KDlg_ProItem_SFXNew::OnBbelt() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpProperty)
		return;
	KSceneModelEditor* pScene = (KSceneModelEditor*)
		m_lpProperty->Address;
	if(pScene)
	{
		ModelSFX::KModelBelt* pModel = NULL;

		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pModel),MODELTYPE_BELT);
		pModel->LoadMesh("");

		pScene->AddRenderMesh(pModel);
		pScene->AddSelectableMesh(pModel);

	}*/
}

void KDlg_ProItem_SFXNew::OnBLVF() 
{
	// TODO: Add your control notification handler code here
	//if(!m_lpProperty)
	//	return;
	//KSceneModelEditor* pScene = (KSceneModelEditor*)
	//	m_lpProperty->Address;
	//if(pScene)
	//{
	//	KVolumeFog* pModel = NULL;

	//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pModel),MODELTYPE_LAYEREDVOLUMETRICFOG);
	//	pModel->LoadMesh("模型文件\\Test.lvf");

	//	pScene->AddRenderMesh(pModel);
	//	pScene->AddSelectableMesh(pModel);

	//}
}

void KDlg_ProItem_SFXNew::OnBDL()
{
	//// TODO: Add your control notification handler code here
	//if(!m_lpProperty)
	//	return;
	//KSceneModelEditor* pScene = (KSceneModelEditor*)
	//	m_lpProperty->Address;
	//if(pScene)
	//{
	//	KLightning* pModel = NULL;

	//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pModel),MODELTYPE_DYNAMICLIGHTNING);
	//	pModel->LoadMesh(0);

	//	pScene->AddRenderMesh(pModel);
	//	pScene->AddSelectableMesh(pModel);
	//}
}

void KDlg_ProItem_SFXNew::OnBST()
{
	// TODO: Add your control notification handler code here
	//char szFilter[] ="剑网3D模型文件(*.mesh)|*.mesh||";

	//CFileDialog dlg(true, "mesh", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"剑网3D模型文件(*.mesh)|*.mesh||");
	//if(dlg.DoModal() != IDOK)
	//{
	//	return;
	//}
	//
	//Common::String sMeshname = dlg.GetPathName();
	//Common::String sMtlname = sMeshname.substr(0, sMeshname.find_last_of('.'));
	//sMtlname += Common::String(".mtl");
	//
	//if(!m_lpProperty)
	//	return;
	//KSceneModelEditor* pScene = (KSceneModelEditor*)
	//	m_lpProperty->Address;
	//if(pScene)
	//{
	//	KSpeedTree* pModel = NULL;

	//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pModel),MODELTYPE_SPEEDTREE);
	//	//pModel->LoadBoneBlendSetFromMeshFile((LPSTR)sMeshname.c_str());
	//	pModel->LoadMesh((LPSTR)sMeshname.c_str());
	//	pModel->LoadMaterial((LPSTR)sMtlname.c_str());

	//	pScene->AddRenderMesh(pModel);
	//	pScene->AddSelectableMesh(pModel);
	//}
}

}