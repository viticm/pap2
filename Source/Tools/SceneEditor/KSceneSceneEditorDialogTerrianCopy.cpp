// KSceneSceneEditorDialogTerrianCopy.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogTerrianCopy.h"
#include "SceneEditorView.h"
#include "SceneEditorDoc.h"
#include "KSO3World.h"
#include "SO3Represent/SO3RepresentInterface.h"
#include "IEEditor.h"


// KSceneSceneEditorDialogTerrianCopy dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogTerrianCopy, CDialog)

KSceneSceneEditorDialogTerrianCopy::KSceneSceneEditorDialogTerrianCopy(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogTerrianCopy::IDD, pParent)
	, m_nMixtype(0)
{
	m_lpSceneSceneEditor =	NULL;
	m_pTerrainBlock = NULL;
	m_bIsOn = false;
}

KSceneSceneEditorDialogTerrianCopy::~KSceneSceneEditorDialogTerrianCopy()
{

}

void KSceneSceneEditorDialogTerrianCopy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SELECT, m_TerrianSelect);
	DDX_Control(pDX, IDC_EDIT1, editName);
	DDX_Control(pDX, IDC_LIST1, TerrainBlockList);
	DDX_Control(pDX, IDC_EDIT3, editMixValue);
	DDX_Control(pDX, IDC_CHECK1, buttonIsPasteTerrain);
	DDX_Control(pDX, IDC_CHECK3, buttonIsPasteBlend);
	DDX_Control(pDX, IDC_CHECK4, buttonIsPasteObject);
	DDX_Control(pDX, IDC_CHECK5, buttonIsMax);
	DDX_Radio(pDX, IDC_RADIO1, m_nMixtype);
	DDX_Control(pDX, IDC_EDIT4, editWidth);
	InitBlockList();
	editMixValue.SetWindowText("1.0");
	buttonIsPasteTerrain.SetCheck(BST_CHECKED);
	editWidth.SetWindowText("100.0");
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogTerrianCopy, CDialog)
	
	ON_BN_CLICKED(IDC_SELECT, &KSceneSceneEditorDialogTerrianCopy::OnBnClickedSelect)
	ON_BN_CLICKED(IDC_BUTTON4, &KSceneSceneEditorDialogTerrianCopy::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, &KSceneSceneEditorDialogTerrianCopy::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &KSceneSceneEditorDialogTerrianCopy::OnBnClickedButton3)
	ON_LBN_DBLCLK(IDC_LIST1, &KSceneSceneEditorDialogTerrianCopy::OnLbnDblclkList1)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// KSceneSceneEditorDialogTerrianCopy message handlers

void KSceneSceneEditorDialogTerrianCopy::OnBnClickedSelect()
{
	if(!m_lpSceneSceneEditor) 
		return;
	if (!m_bIsOn)
	{
		m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAINSELECTION);
		m_TerrianSelect.SetWindowText("确定选择");		
	}
	else
	{	
		m_lpSceneSceneEditor->SelectTerrainFinished(); //地表复制
		editName.SetWindowText("noname");
		m_TerrianSelect.SetWindowText("选择区域");
	}
	m_bIsOn = !m_bIsOn;
	return;
}
void KSceneSceneEditorDialogTerrianCopy::OnBnClickedButton4() //粘贴
{
	DWORD dwMixType;
	CString strWeight;
	CString strWidth;
	LPTSTR pstrWeight;
	float fWeight,fWidth;
	bool bIsPasteBlend,bIsPasteObject,bIsMax;
	
	bIsPasteBlend = (buttonIsPasteBlend.GetCheck() == BST_CHECKED ? true : false);
	bIsPasteObject = (buttonIsPasteObject.GetCheck() == BST_CHECKED ? true : false);
	editMixValue.GetWindowText(strWeight);
	pstrWeight = strWeight.GetBuffer();
	fWeight = static_cast<float>(atof(pstrWeight));
	if (fWeight < 0 )
		fWeight = 0;
	if (fWeight > 10)
		fWeight = 10;
	editWidth.GetWindowText(strWidth);
	pstrWeight = strWidth.GetBuffer();
	fWidth = static_cast<float>(atof(pstrWeight));
	if (fWidth < 0 )
		fWidth = 0;
	bIsMax = (buttonIsMax.GetCheck() == BST_CHECKED ? true:false);
	if (buttonIsPasteTerrain.GetCheck() == BST_UNCHECKED)
		dwMixType = static_cast<DWORD>(-1);
	else
	{
		UpdateData(TRUE);	
		dwMixType = m_nMixtype;
		UpdateData(FALSE);
	}
	editMixValue.SetWindowText(strWeight);
	editWidth.SetWindowText(strWidth);
	m_lpSceneSceneEditor->PasteSeclctTerrain(dwMixType,bIsPasteBlend,bIsPasteObject,bIsMax,fWeight,fWidth);
	return;
}

void KSceneSceneEditorDialogTerrianCopy::InitBlockList() //列表添加
{
	CFileFind finder;
	TerrainBlockList.ResetContent();
	TCHAR szPath[MAX_PATH];
	TCHAR szRootPath[MAX_PATH];
	g_GetRootPath(szRootPath);
	sprintf(szPath,"%s%s",szRootPath,"\\TerrainBlock\\*.*");
	BOOL bWorking = finder.FindFile(szPath);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		TerrainBlockList.AddString((LPCTSTR)finder.GetFileName());
	}
	TerrainBlockList.DeleteString(0);
	TerrainBlockList.DeleteString(0);
}

void KSceneSceneEditorDialogTerrianCopy::OnBnClickedButton2() //保存复制的地形块
{
	if(!m_lpSceneSceneEditor) 
		return;
	CString strName;
	LPTSTR lpNames;
	TCHAR szPath[MAX_PATH];
	TCHAR szRootPath[MAX_PATH];
	editName.GetWindowText(strName);
	lpNames = strName.GetBuffer();
	g_GetRootPath(szRootPath);
	sprintf(szPath,"%s%s%s",szRootPath,"\\TerrainBlock\\",lpNames);
	mkdir("TerrainBlock");
	m_lpSceneSceneEditor->SaveTerrainBlock(szPath);
	InitBlockList();
}
void KSceneSceneEditorDialogTerrianCopy::OnBnClickedButton3() //删除复制的地形块
{
	if(!m_lpSceneSceneEditor) 
		return;
	CString strName;
	LPTSTR lpNames;
	TCHAR szPath[MAX_PATH];
	TCHAR szRootPath[MAX_PATH];
	int nSelect = TerrainBlockList.GetCurSel();
	TerrainBlockList.GetText(nSelect,strName);
	lpNames = strName.GetBuffer();
	g_GetRootPath(szRootPath);
	sprintf(szPath,"%s%s%s",szRootPath,"\\TerrainBlock\\",lpNames);
	DeleteFile(szPath);
	InitBlockList();
}

void KSceneSceneEditorDialogTerrianCopy::OnLbnDblclkList1() //双击复制地形块列表
{
	if(!m_lpSceneSceneEditor) 
		return;
	CString strName;
	LPTSTR lpNames;
	TCHAR szPath[MAX_PATH];
	TCHAR szRootPath[MAX_PATH];
	int nSelect = TerrainBlockList.GetCurSel();
	TerrainBlockList.GetText(nSelect,strName);
	lpNames = strName.GetBuffer();
	g_GetRootPath(szRootPath);
	sprintf(szPath,"%s%s%s",szRootPath,"\\TerrainBlock\\",lpNames);
	m_lpSceneSceneEditor->LoadTerrainBlock(szPath);
	editName.SetWindowText(strName);
}



void KSceneSceneEditorDialogTerrianCopy::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(!m_lpSceneSceneEditor) 
		return;
	if (bShow)
		m_lpSceneSceneEditor->TerrainCopyWindowOnShow();
	else
		m_lpSceneSceneEditor->TerrainCopyWindowOnHide();
}
