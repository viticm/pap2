// KDlg_ProItem_Terrain_Ground_Texture.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_Terrain_Ground_Texture.h"
//#include "SceneSkinEdit.h"
//#include "MeshTerran.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Terrain_Ground_Texture dialog
namespace PropertyBase
{

KDlg_ProItem_Terrain_Ground_Texture::KDlg_ProItem_Terrain_Ground_Texture(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_Terrain_Ground_Texture::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_Terrain_Ground_Texture)
	m_CurTextureName = _T("");
	m_Size = 0;
	m_Alpha = 0;
	//}}AFX_DATA_INIT
}


void KDlg_ProItem_Terrain_Ground_Texture::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_Terrain_Ground_Texture)
	DDX_Control(pDX, IDC_BCOLOR, m_ColorButton);
	DDX_Control(pDX, IDC_SALPHA, m_SliderAlpha);
	DDX_Control(pDX, IDC_SSIZE, m_SliderSize);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Text(pDX, IDC_CURTEXTURENAME, m_CurTextureName);
	DDX_Slider(pDX, IDC_SSIZE, m_Size);
	DDX_Slider(pDX, IDC_SALPHA, m_Alpha);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_Terrain_Ground_Texture, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_Terrain_Ground_Texture)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_BN_CLICKED(IDC_RSHAPE1, OnRshape1)
	ON_BN_CLICKED(IDC_RSHAPE2, OnRshape2)
	ON_BN_CLICKED(IDC_RSHAPE3, OnRshape3)
	ON_BN_CLICKED(IDC_RSHAPE4, OnRshape4)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BCOLOR, OnBcolor)
	ON_BN_CLICKED(IDC_REGION_ADD, OnRegionAdd)
	ON_BN_CLICKED(IDC_REGION_DELETE, OnRegionDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Terrain_Ground_Texture message handlers

BOOL KDlg_ProItem_Terrain_Ground_Texture::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FillTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_ProItem_Terrain_Ground_Texture::FillTree()
{
	m_Tree.DeleteAllItems();

	//if(!m_lpProperty)
	//	return;

	//KSceneSceneEdit* pScene = (KSceneSceneEdit*) m_lpProperty->Address;
	//KModelTerrain* pTerrain = pScene->m_lpMapMesh;

	//for(DWORD i=0;i<pTerrain->m_vecGroundTextGroups.size();i++)
	//{
	//	KModelTerrain::_GroundTextureGroup* pGroup = 
	//		pTerrain->m_vecGroundTextGroups[i];
	//	HTREEITEM hGroup = m_Tree.InsertItem(pGroup->SetName);
	//	m_Tree.SetItemData(hGroup,0xFFFFFFFF);

	//	for(DWORD j=0;j<pGroup->vecTextureID.size();j++)
	//	{
	//		HTREEITEM hTexture = 
	//			m_Tree.InsertItem(pGroup->vecTextureName[j].c_str(),hGroup);
	//		m_Tree.SetItemData(hTexture,pGroup->vecTextureIndex[j]);
	//		//pGroup->
	//	}
	//}

	//m_SliderSize.SetRange(1,100);
	//m_SliderAlpha.SetRange(1,100);
	return;
}

void KDlg_ProItem_Terrain_Ground_Texture::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	DWORD Data = (DWORD)m_Tree.GetItemData(hItem);

	/*if(!m_lpProperty)
		return;

	KSceneSceneEdit* pScene = (KSceneSceneEdit*) m_lpProperty->Address;
	KModelTerrain* pTerrain = pScene->m_lpMapMesh;

	if(0xFFFFFFFF != Data)
	{
		m_CurTextureName = m_Tree.GetItemText(hItem);
		UpdateData(FALSE);

		pScene->m_dwState = EDITSTATE_TERRANEDIT;

		pTerrain->m_dwState = MAP_PAINT_GROUND;
		pTerrain->m_nCurTextureIndex = Data;
	}*/
	*pResult = 0;
}


void KDlg_ProItem_Terrain_Ground_Texture::OnRshape1() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpProperty)
		return;

	KSceneSceneEdit* pScene = (KSceneSceneEdit*) m_lpProperty->Address;
	KModelTerrain* pTerrain = pScene->m_lpMapMesh;

	pTerrain->m_BrushGroundTexture.m_nShape = 0;*/
}

void KDlg_ProItem_Terrain_Ground_Texture::OnRshape2() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpProperty)
		return;

	KSceneSceneEdit* pScene = (KSceneSceneEdit*) m_lpProperty->Address;
	KModelTerrain* pTerrain = pScene->m_lpMapMesh;

	pTerrain->m_BrushGroundTexture.m_nShape = 1;*/
}

void KDlg_ProItem_Terrain_Ground_Texture::OnRshape3() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpProperty)
		return;

	KSceneSceneEdit* pScene = (KSceneSceneEdit*) m_lpProperty->Address;
	KModelTerrain* pTerrain = pScene->m_lpMapMesh;

	pTerrain->m_BrushGroundTexture.m_nShape = 2;	*/
}

void KDlg_ProItem_Terrain_Ground_Texture::OnRshape4() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpProperty)
		return;

	KSceneSceneEdit* pScene = (KSceneSceneEdit*) m_lpProperty->Address;
	KModelTerrain* pTerrain = pScene->m_lpMapMesh;

	pTerrain->m_BrushGroundTexture.m_nShape = 3;	*/
}

void KDlg_ProItem_Terrain_Ground_Texture::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	UpdateData(TRUE);

	/*if(!m_lpProperty)
		return;

	KSceneSceneEdit* pScene = (KSceneSceneEdit*) m_lpProperty->Address;
	KModelTerrain* pTerrain = pScene->m_lpMapMesh;

	pTerrain->m_BrushGroundTexture.m_nWidth = m_Size;
	pTerrain->m_BrushGroundTexture.m_nAlpha = m_Alpha;*/

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void KDlg_ProItem_Terrain_Ground_Texture::OnBcolor() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpProperty)
		return;

	KSceneSceneEdit* pScene = (KSceneSceneEdit*) m_lpProperty->Address;
	KModelTerrain* pTerrain = pScene->m_lpMapMesh;
	
	pScene->m_dwState = EDITSTATE_TERRANEDIT;
	pTerrain->m_dwState = MAP_PAINT_COLOR;

	m_ColorButton.m_dwColor = pTerrain->m_CurColor;

	CColorDialog Dlg(m_ColorButton.m_dwColor);
	if(IDOK==Dlg.DoModal())
	{
		m_ColorButton.m_dwColor = Dlg.GetColor();
		m_ColorButton.RedrawWindow();

		DWORD R = (m_ColorButton.m_dwColor & 0x000000FF);
		DWORD G = (m_ColorButton.m_dwColor & 0x0000FF00)>>8;
		DWORD B = (m_ColorButton.m_dwColor & 0x00FF0000)>>16;

		pTerrain->m_CurColor = 0xFF000000 | (R<<16) | (G<<8) | B;
	}*/

}



void KDlg_ProItem_Terrain_Ground_Texture::OnRegionAdd() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpProperty)
		return;

	KSceneSceneEdit* pScene = (KSceneSceneEdit*) m_lpProperty->Address;
	KModelTerrain* pTerrain = pScene->m_lpMapMesh;

	pScene->m_dwState = EDITSTATE_TERRANEDIT;
	pTerrain->m_dwState = MAP_REGION_ADD;*/}

void KDlg_ProItem_Terrain_Ground_Texture::OnRegionDelete() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpProperty)
		return;

	KSceneSceneEdit* pScene = (KSceneSceneEdit*) m_lpProperty->Address;
	KModelTerrain* pTerrain = pScene->m_lpMapMesh;

	pScene->m_dwState = EDITSTATE_TERRANEDIT;
	pTerrain->m_dwState = MAP_REGION_DELETE;	*/
}

}