// MeshMeshSelect.cpp: implementation of the KModelToolSelect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshMeshSelect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace ModelTools
{
//KModelToolSelect::KModelToolSelect()
//{
//	m_bHide = TRUE;
//	m_nNumMesh = 0;
//	m_lpMeshListBox = NULL;
//	m_lpScene = NULL;
//}
//
//KModelToolSelect::~KModelToolSelect()
//{
//
//}
//
//HRESULT KModelToolSelect::LoadMesh(LPSTR pFileName)
//{
//	//TCHAR Name[256];
//	//wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Textures\\Mtltool.tga");
//	//m_CmdBack.Create(0.4f,0.2f,0.27f,0.24f,0,0,Name,0);
//	//g_cGraphicsTool.LoadTexture4Command(&m_CmdBack);
//	ModelUI::FRECT Rect(0.4f,0.175f,0.27f,0.265f);
//	KModelUIBase::Create(&Rect,"",0,0);
//	KModelUIBase::SetTitleCaption("Ñ¡ÔñÄ£ÐÍ");
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpMeshListBox,MODELTYPE_LISTBOX);
//	ModelUI::FRECT rect_1(0.405f,0.205f,0.22f,0.23f);
//	m_lpMeshListBox->Create(&rect_1,"",0,0);
//	m_lpMeshListBox->SetParentWnd(this);
//
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBMeshOpen,MODELTYPE_BUTTON);
//		ModelUI::FRECT rect_1(0.63f,0.21f,0.03f,0.03f);
//		m_pBMeshOpen->Create(&rect_1,"",0,0);
//		m_pBMeshOpen->SetIconTexture("ToolBar\\Load.tga");
//		m_pBMeshOpen->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBMeshSave,MODELTYPE_BUTTON);
//		ModelUI::FRECT rect_1(0.63f,0.25f,0.03f,0.03f);
//		m_pBMeshSave->Create(&rect_1,"",0,0);
//		m_pBMeshSave->SetIconTexture("ToolBar\\Save.tga");
//		m_pBMeshSave->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBMeshDelete,MODELTYPE_BUTTON);
//		ModelUI::FRECT rect_1(0.63f,0.29f,0.03f,0.03f);
//		m_pBMeshDelete->Create(&rect_1,"",0,0);
//		m_pBMeshDelete->SetIconTexture("ToolBar\\AniDelete.tga");
//		m_pBMeshDelete->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBMeshSelect,MODELTYPE_BUTTON);
//		ModelUI::FRECT rect_1(0.63f,0.33f,0.03f,0.03f);
//		m_pBMeshSelect->Create(&rect_1,"",0,0);
//		m_pBMeshSelect->SetIconTexture("ToolBar\\Select.tga");
//		m_pBMeshSelect->SetParentWnd(this);
//	}
//
//	return S_OK;
//}
//
//HRESULT KModelToolSelect::Render()
//{
//	if (m_bHide)
//		return S_OK;
//
//	KModelUIBase::Render();
//
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return S_OK;
//		
//
//	if (m_nNumMesh!=pMeshScene->m_RenderEntityList.size())
//	{
//		FillMeshListBox();
//		m_nNumMesh = pMeshScene->m_RenderEntityList.size();
//	}
//	
//	//g_cGraphicsTool.DrawCommand(&m_CmdBack);
//
//	SAFE_RENDER(m_lpMeshListBox);
//	SAFE_RENDER(m_pBMeshOpen);
//	SAFE_RENDER(m_pBMeshSave);
//	SAFE_RENDER(m_pBMeshDelete);
//	SAFE_RENDER(m_pBMeshSelect);
//
//	return S_OK;
//}
//
//HRESULT KModelToolSelect::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if (message==WM_KEYDOWN)
//	{
//		if (wParam==VK_F5)
//		{
//			if (m_bHide == FALSE)
//				m_bHide = TRUE;
//			else
//				m_bHide = FALSE;
//		}
//	}
//
//	if (m_bHide)
//		return S_OK;
//
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return S_OK;
//
//	DWORD MSG = m_lpMeshListBox->ProcessMessage(message,wParam,lParam);
//	switch(MSG) 
//	{
//	case LISTBOX_SELECTCHANGE:
//		{
//			LPMODEL pCurMesh = NULL;
//			TCHAR str[256];
//			if (SUCCEEDED(m_lpMeshListBox->GetCurSelectedString(str,(DWORD*)(&pCurMesh))))
//			{
//				if (pCurMesh)
//				{
//					pMeshScene->SetCurMesh(pCurMesh);
//				}
//			}
//			m_pBMeshSelect->m_dwState = 0;
//			break;
//		}
//	case LISTBOX_ITEMDBCLICK:
//		{
//			LPMODEL pCurMesh = NULL;
//			TCHAR str[256];
//			if (SUCCEEDED(m_lpMeshListBox->GetCurSelectedString(str,(DWORD*)(&pCurMesh))))
//			{
//				if (pCurMesh)
//				{
//					pMeshScene->SetCurMesh(pCurMesh);
//				}
//			}
//			m_pBMeshSelect->m_dwState = 0;
//			m_bHide = TRUE;
//			break;
//		}
//	}
//	
//	if (ITEM_CLICK == m_pBMeshOpen->ProcessMessage(message,wParam,lParam))
//	{
//		OpenMeshFile();
//		m_pBMeshOpen->m_dwState = 0;
//	}
//
//	if (ITEM_CLICK == m_pBMeshSave->ProcessMessage(message,wParam,lParam))
//	{
//		m_pBMeshSave->m_dwState = 0;
//	}
//
//	if (ITEM_CLICK == m_pBMeshDelete->ProcessMessage(message,wParam,lParam))
//	{
//		LPMODEL pCurMesh = NULL;
//		TCHAR str[256];
//		if (SUCCEEDED(m_lpMeshListBox->GetCurSelectedString(str,(DWORD*)(&pCurMesh))))
//		{
//			if (pCurMesh)
//			{
//				pMeshScene->RemoveRenderMesh(pCurMesh);
//				pMeshScene->SetCurMesh(NULL);
//			}
//		}
//		m_pBMeshSelect->m_dwState = 0;
//		m_pBMeshDelete->m_dwState = 0;
//		FillMeshListBox();
//	}
//
//	if (ITEM_CLICK == m_pBMeshSelect->ProcessMessage(message,wParam,lParam))
//	{
//		LPMODEL pCurMesh = NULL;
//		TCHAR str[256];
//		if (SUCCEEDED(m_lpMeshListBox->GetCurSelectedString(str,(DWORD*)(&pCurMesh))))
//		{
//			if (pCurMesh)
//			{
//				pMeshScene->SetCurMesh(pCurMesh);
//			}
//		}
//		m_pBMeshSelect->m_dwState = 0;
//		m_bHide = TRUE;
//
//	}
//	
//	return KModelUIBase::ProcessMessage(message,wParam,lParam);
//}
//
//HRESULT KModelToolSelect::SetMeshEditScene(PVOID pScene)
//{
//	m_lpScene = pScene;
//	return E_NOTIMPL;
//}
//
//void KModelToolSelect::FillMeshListBox()
//{
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return ;
//	m_lpMeshListBox->Clear();
//	TCHAR Name[256];
//
//	K3DScene::ENTITYLIST::iterator i = pMeshScene->m_RenderEntityList.begin();
//	while(i!=pMeshScene->m_RenderEntityList.end())
//	{
//		KSceneEntity Entity = *i;
//		if (Entity.Type==SCENEENTITY_MODEL)
//		{
//			LPMODEL pMesh = (LPMODEL) Entity.Pointer;
//					
//			TCHAR path_buffer[256];
//			TCHAR drive[256];
//			TCHAR dir[256];
//			TCHAR fname[256];
//			TCHAR ext[256];
//
//			wsprintf(path_buffer,"%s",pMesh->m_szMeshFileName);
//			_splitpath( path_buffer, drive, dir, fname, ext );
//
//			wsprintf(Name,"%s",fname);
//			m_lpMeshListBox->AddString(Name,(DWORD)pMesh);
//		}
//		i++;
//	}
//}
//
//HRESULT KModelToolSelect::OpenMeshFile()
//{
//	char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh|";
//
//	CFileDialog dlg(true, NULL, NULL,
//		OFN_HIDEREADONLY , szFilter);
//	// Modify OPENFILENAME members directly to point to bigBuff
//	dlg.m_ofn.lpstrFile = bigBuff;
//	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
//	if (	dlg.DoModal() == IDOK)
//	{
//		TCHAR path_buffer[256];
//		TCHAR drive[256];
//		TCHAR dir[256];
//		TCHAR fname[256];
//		TCHAR ext[256];
//		
//		CString Vs = dlg.GetPathName();
//
//		wsprintf(path_buffer,"%s",Vs);
//		_splitpath( path_buffer, drive, dir, fname, ext );
//
//
//		TCHAR filename[256];
//		wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);
//
//		KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//		if (!pMeshScene)
//			return E_FAIL;
//		
//		LPMODEL pMesh = NULL;
//		DWORD   NewMeshID  = 0;
//		
//		if (FAILED(g_cMeshTable.LoadFromFile(&NewMeshID,filename)))
//			return E_FAIL;
//
//		if (FAILED(g_cMeshTable.GetXMesh(&pMesh,NewMeshID)))
//			return E_FAIL;
//	
//		wsprintf(filename,"%s%s%s.Mtl",drive,dir,fname);
//		pMesh->LoadMaterial(filename);
//
//		pMeshScene->AddRenderMesh(pMesh);
//		pMeshScene->AddSelectableMesh(pMesh);
//	}
//	return S_OK;
//}
};