// MeshMeshTool.cpp: implementation of the KModelToolMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshMeshTool.h"

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
//KModelToolMesh::KModelToolMesh()
//{
//	m_CurModel = NULL;
//	m_bHide = TRUE;
//	m_lpSocketListBox = NULL;
//	//m_lpBoneScalingSlideBar = NULL;
//	//m_lpBonePositionSlideBar = NULL;
//	m_lpBoneListBox = NULL;
//
//	m_nNumBoneBlendMesh = 0;
//	m_lpBoneBlendListBox = NULL;
//	//m_lpMeshListBox = NULL;
//	//m_nNumMesh = 0;
//	m_nNumBoneBlendMesh = -1;
//}
//
//KModelToolMesh::~KModelToolMesh()
//{
//
//}
//HRESULT KModelToolMesh::SetMeshEditScene(PVOID pScene)
//{
//	m_lpScene = pScene;
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolMesh::LoadMesh(LPSTR pFileName)
//{
//	TCHAR Name[256];
//
//	ModelUI::FRECT rect(0.0f,0,0.15f,0.84f);
//	KModelUIBase::Create(&rect,"",0,0);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpMeshListBox,MODELTYPE_LISTBOX);
//	ModelUI::FRECT rect_1(0.155f,0.6f,0.14f,0.14f);
//	m_lpMeshListBox->Create(&rect_1,"",0,0);
//	m_lpMeshListBox->SetParentWnd(this);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\Load.tga");
//	m_CmdMeshOpen.Create(0.155f,0.745f,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdMeshOpen);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\AniDelete.tga");
//	m_CmdMeshDelete.Create(0.190f,0.745f,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdMeshDelete);
//	
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\Cancle.tga");
//	m_CmdMeshUnBind.Create(0.225f,0.745f,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdMeshUnBind);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\Bind.tga");
//	m_CmdMeshBind.Create(0.260f,0.745f,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdMeshBind);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\Save.tga");
//	m_CmdMeshSave.Create(0.145f,0.08f,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdMeshSave);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpBoneBlendListBox,MODELTYPE_LISTBOX);
//	ModelUI::FRECT rect_2(0.005f,0.16f,0.14f,0.12f);
//	m_lpBoneBlendListBox->Create(&rect_2,"",0,0);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\Load.tga");
//	m_CmdBoneBlendOpen.Create(0.145f,0.161f,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdBoneBlendOpen);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpBoneListBox,MODELTYPE_LISTBOX);
//	ModelUI::FRECT rect_3(0.005f,0.30f,0.14f,0.24f);
//	m_lpBoneListBox->Create(&rect_3,"",0,0);
//	m_lpBoneListBox->SetParentWnd(this);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpSocketListBox,MODELTYPE_LISTBOX);
//	ModelUI::FRECT rect_4(0.005f,0.545f,0.14f,0.12f);
//	m_lpSocketListBox->Create(&rect_4,"",0,0);
//	m_lpSocketListBox->SetParentWnd(this);
//	
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpSocketName,MODELTYPE_EDITCTRL);
//	ModelUI::FRECT Rect1(0.005f,0.67f,0.14f,0.03f);
//	m_lpSocketName->Create(&Rect1,"",0,0);
//
//	float S_Up = 0.545f;
//	float S_Height = 0.036f;
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\add.tga");
//	m_CmdSocketAdd.Create(0.005f,0.705f,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdSocketAdd);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\AniDelete.tga");
//	m_CmdSocketDelete.Create(0.005f+0.035f,0.705f,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdSocketDelete);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\Adjust.tga");
//	m_CmdSocketAdjust.Create(0.005f+0.035f*2,0.705f,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdSocketAdjust);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\Bind.tga");
//	m_CmdSocketBind.Create(0.005f+0.035f*3,0.705f,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdSocketBind);
//
//	/*wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\AniDelete.tga");
//	m_CmdSocketUnBind.Create(0.145f,S_Up+4*S_Height,0.03f,0.03f,0,0+1,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdSocketUnBind);*/
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpSocketYawSlideBar,MODELTYPE_SLIDEBAR);
//	ModelUI::FRECT RectYaw(0.155f,0.45f,0.14f,0.03f);
//	m_lpSocketYawSlideBar->Create  (&RectYaw,"",0,0);
//	m_lpSocketYawSlideBar->m_fPercent = 50;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpSocketPitchSlideBar,MODELTYPE_SLIDEBAR);
//	ModelUI::FRECT RectPitch(0.155f,0.49f,0.14f,0.03f);
//	m_lpSocketPitchSlideBar->Create(&RectPitch,"",0,0);
//	m_lpSocketPitchSlideBar->m_fPercent = 50;
//	
//	ModelUI::FRECT RectRoll(0.155f,0.53f,0.14f,0.03f);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpSocketRollSlideBar,MODELTYPE_SLIDEBAR);
//	m_lpSocketRollSlideBar->Create (&RectRoll,"",0,0);
//	m_lpSocketRollSlideBar->m_fPercent = 50;
//	
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpSocketX,MODELTYPE_SLIDEBAR);
//		ModelUI::FRECT Rect(0.155f,0.30f,0.14f,0.03f);
//		m_lpSocketX->Create  (&Rect,"",0,0);
//		m_lpSocketX->m_fPercent = 50;
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpSocketY,MODELTYPE_SLIDEBAR);
//		ModelUI::FRECT Rect(0.155f,0.34f,0.14f,0.03f);
//		m_lpSocketY->Create  (&Rect,"",0,0);
//		m_lpSocketY->m_fPercent = 50;
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpSocketZ,MODELTYPE_SLIDEBAR);
//		ModelUI::FRECT Rect(0.155f,0.38f,0.14f,0.03f);
//		m_lpSocketZ->Create  (&Rect,"",0,0);
//		m_lpSocketZ->m_fPercent = 50;
//	}
//
//
//	return S_OK;
//}
//
//HRESULT KModelToolMesh::Render()
//{
//	if (m_bHide)
//	{
//		return S_OK;
//	}
//
//	KModelUIBase::Render();
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return S_OK;
//
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if ((pMesh)&&(pMesh!=m_CurModel))
//	{
//		FillBoneListBox();
//		m_CurModel = pMesh;
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	//察看高度
//	TCHAR str[256];
//	sprintf(str,"H%.1f",pMesh->m_BBox_B.y - pMesh->m_BBox_A.y);
//	g_cGraphicsTool.DrawText(200,200,1,1,0xFFFF00FF,24,"宋体",str,0);
//
//	SAFE_RENDER(m_lpBoneBlendListBox);
//	SAFE_RENDER(m_lpBoneListBox);
//	SAFE_RENDER(m_lpSocketListBox);
//	SAFE_RENDER(m_lpSocketName);
//
//	g_cGraphicsTool.DrawCommand(&m_CmdSocketAdd);
//	g_cGraphicsTool.DrawCommand(&m_CmdSocketDelete);
//	g_cGraphicsTool.DrawCommand(&m_CmdSocketAdjust);
//	g_cGraphicsTool.DrawCommand(&m_CmdSocketBind);
//	
//	if (m_CmdSocketBind.State==2)
//	{
//		SAFE_RENDER(m_lpMeshListBox);
//		
//		g_cGraphicsTool.DrawCommand(&m_CmdMeshOpen);
//		g_cGraphicsTool.DrawCommand(&m_CmdMeshDelete);
//		g_cGraphicsTool.DrawCommand(&m_CmdMeshUnBind);
//		g_cGraphicsTool.DrawCommand(&m_CmdMeshBind);
//		
//		SAFE_RENDER(m_lpSocketYawSlideBar);
//		SAFE_RENDER(m_lpSocketPitchSlideBar);
//		SAFE_RENDER(m_lpSocketRollSlideBar);
//
//		SAFE_RENDER(m_lpSocketX);
//		SAFE_RENDER(m_lpSocketY);
//		SAFE_RENDER(m_lpSocketZ);
//	}
//		
//	g_cGraphicsTool.DrawCommand(&m_CmdMeshSave);
//	
//	g_cGraphicsTool.DrawCommand(&m_CmdBoneBlendOpen);
//
//	return S_OK;
//}
//
//void KModelToolMesh::FillMeshListBox()
//{
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return ;
//	m_lpMeshListBox->Clear();
//	LPMODEL pCurMesh = pMeshScene->GetCurMesh();
//	if (!pCurMesh)
//		return ;
//
//	TCHAR Name[256];
//
//	K3DScene::ENTITYLIST::iterator i = pMeshScene->m_RenderEntityList.begin();
//	while(i!=pMeshScene->m_RenderEntityList.end())
//	{
//		KSceneEntity Entity = *i;
//		if (Entity.Type==SCENEENTITY_MODEL)
//		{
//			LPMODEL pMesh = (LPMODEL) Entity.Pointer;
//			if (pCurMesh!=pMesh)
//			{
//				TCHAR path_buffer[256];
//				TCHAR drive[256];
//				TCHAR dir[256];
//				TCHAR fname[256];
//				TCHAR ext[256];
//				
//				wsprintf(path_buffer,"%s",pMesh->m_szMeshFileName);
//				_splitpath( path_buffer, drive, dir, fname, ext );
//				
//				wsprintf(Name,"%s",fname);
//				m_lpMeshListBox->AddString(Name,(DWORD)pMesh);
//			}
//
//		}
//		i++;
//	}
//}
//
//HRESULT KModelToolMesh::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if (message==WM_KEYDOWN)
//	{
//		if (wParam==VK_F2)
//		{
//			if (m_bHide == FALSE)
//				m_bHide = TRUE;
//			else
//				m_bHide = FALSE;
//		}
//	}
//	if (m_bHide)
//	{
//		return S_OK;
//	}
//	
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return E_FAIL;
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return E_FAIL;
//	DWORD MSG =0;
//
//	//m_lpMeshListBox->ProcessMessage(message,wParam,lParam);
//	m_lpBoneBlendListBox->ProcessMessage(message,wParam,lParam);
//	
//	MSG = m_lpBoneListBox->ProcessMessage(message,wParam,lParam);
//	switch(MSG) 
//	{
//	case LISTBOX_SELECTCHANGE:
//		{
//			TCHAR str[256];
//			KModel::LPBONEINFO pInfo = NULL;
//			if (SUCCEEDED(m_lpBoneListBox->GetCurSelectedString(str,(DWORD*)(&pInfo))))
//			{
//				if (pInfo)
//				{
//					FillSocketListBox(pInfo);
//				}
//			}
//			break;
//		}
//	case LISTBOX_ITEMDBCLICK:
//		break;
//	}
//
//	MSG = m_lpSocketListBox->ProcessMessage(message,wParam,lParam);
//	switch(MSG) 
//	{
//	case LISTBOX_SELECTCHANGE:
//		{
//			TCHAR str[256];
//			KModel::LPSOCKET pSocket = NULL;
//			if (SUCCEEDED(m_lpSocketListBox->GetCurSelectedString(str,(DWORD*)(&pSocket))))
//			{
//				if (pSocket)
//				{
//					m_lpSocketName->SetText(str);
//				}
//			}
//			break;
//		}
//	case LISTBOX_ITEMDBCLICK:
//		break;
//	}
//
//	MSG = m_lpSocketName->ProcessMessage(message,wParam,lParam);
//	if (MSG == EDIT_TEXTCHANGE)
//	{
//		TCHAR str[256];
//		KModel::LPSOCKET pSocket = NULL;
//		if (SUCCEEDED(m_lpSocketListBox->GetCurSelectedString(str,(DWORD*)(&pSocket))))
//		{
//			if (pSocket)
//			{
//				m_lpSocketListBox->SetCurSelectedString(m_lpSocketName->m_szString);
//				pSocket->scName = str;
//			}
//		}
//	}
//	
//	MSG = 0;
//	
//	/*if (g_cGraphicsTool.OnClickCommand(&m_CmdMeshOpen,message,wParam,lParam))
//	{
//		m_CmdMeshOpen.State  = 1;
//		OpenMeshFile();
//	}
//
//	if (g_cGraphicsTool.OnClickCommand(&m_CmdMeshDelete,message,wParam,lParam))
//	{
//		m_CmdMeshDelete.State  = 1;
//	}*/
//
//	if (g_cGraphicsTool.OnClickCommand(&m_CmdMeshSave,message,wParam,lParam))
//	{
//		m_CmdMeshSave.State  = 1;
//		SaveMeshFile();
//	}
//
//	if (g_cGraphicsTool.OnClickCommand(&m_CmdSocketAdd,message,wParam,lParam))
//	{
//		m_CmdSocketAdd.State  = 1;
//		TCHAR str[256];
//		KModel::LPBONEINFO pInfo = NULL;
//		if (SUCCEEDED(m_lpBoneListBox->GetCurSelectedString(str,(DWORD*)(&pInfo))))
//		{
//			if (pInfo)
//			{
//				pMesh->AddSocket((LPSTR)pInfo->scName.c_str(),"New Socket");
//				FillSocketListBox(pInfo);
//			}
//		}
//	}
//		
//	if (g_cGraphicsTool.OnClickCommand(&m_CmdSocketDelete,message,wParam,lParam))
//	{
//		m_CmdSocketDelete.State  = 1;
//		TCHAR str[256];
//		KModel::LPSOCKET pSocket = NULL;
//		if (SUCCEEDED(m_lpSocketListBox->GetCurSelectedString(str,(DWORD*)(&pSocket))))
//		{
//			if (pSocket)
//			{
//				pMesh->DeleteSocket(pSocket->scName.c_str());
//				m_lpSocketListBox->Clear();
//				m_lpSocketName->SetText("");
//			
//				KModel::LPBONEINFO pInfo = NULL;
//				if (SUCCEEDED(m_lpBoneListBox->GetCurSelectedString(str,(DWORD*)(&pInfo))))
//				{
//					if (pInfo)
//					{
//						FillSocketListBox(pInfo);
//					}
//				}
//			}
//		}
//	}
//
//	if (g_cGraphicsTool.OnClickCommand(&m_CmdSocketAdjust,message,wParam,lParam))
//	{
//		m_CmdSocketAdjust.State  = 1;
//	}
//
//	if (g_cGraphicsTool.OnClickCommand(&m_CmdSocketBind,message,wParam,lParam))
//	{
//		FillMeshListBox();
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	TCHAR SocketName[256];
//	KModel::LPSOCKET pSocket = NULL;
//	if (SUCCEEDED(m_lpSocketListBox->GetCurSelectedString(SocketName,(DWORD*)(&pSocket))))
//	{
//		if ((pSocket)&&(m_CmdSocketBind.State == 2))
//		{
//			m_lpMeshListBox->ProcessMessage(message,wParam,lParam);
//			
//			BOOL SocketRot = FALSE; 
//			if (SLIDER_MOVE == m_lpSocketYawSlideBar->ProcessMessage(message,wParam,lParam))
//			{
//				SocketRot = TRUE;
//			}	
//			if (SLIDER_MOVE == m_lpSocketPitchSlideBar->ProcessMessage(message,wParam,lParam))
//			{
//				SocketRot = TRUE;
//			}
//			if (SLIDER_MOVE == m_lpSocketRollSlideBar->ProcessMessage(message,wParam,lParam))
//			{
//				SocketRot = TRUE;
//			}
//			if (SLIDER_MOVE == m_lpSocketX->ProcessMessage(message,wParam,lParam))
//			{
//				SocketRot = TRUE;
//			}	
//			if (SLIDER_MOVE == m_lpSocketY->ProcessMessage(message,wParam,lParam))
//			{
//				SocketRot = TRUE;
//			}
//			if (SLIDER_MOVE == m_lpSocketZ->ProcessMessage(message,wParam,lParam))
//			{
//				SocketRot = TRUE;
//			}		
//			if (SocketRot)
//			{
//				float Y  = (m_lpSocketYawSlideBar->m_fPercent-50)*0.02f*PI;
//				float P  = (m_lpSocketPitchSlideBar->m_fPercent-50)*0.02f*PI;
//				float R  = (m_lpSocketRollSlideBar->m_fPercent-50)*0.02f*PI;
//				float XX = (m_lpSocketX->m_fPercent-50)*1.0f;
//				float YY = (m_lpSocketY->m_fPercent-50)*1.0f;
//				float ZZ = (m_lpSocketZ->m_fPercent-50)*1.0f;
//				D3DXMATRIX Mat = pSocket->MatrixOffset;
//				D3DXMatrixRotationYawPitchRoll(&Mat,Y,P,R);
//				Mat._41 = XX;
//				Mat._42 = YY;
//				Mat._43 = ZZ;
//				pSocket->MatrixOffset = Mat;
//			}
//			
//			if (g_cGraphicsTool.OnClickCommand(&m_CmdMeshOpen,message,wParam,lParam))
//			{
//				m_CmdMeshOpen.State  = 1;
//				OpenMeshFile();
//				FillMeshListBox();
//			}
//
//			if (g_cGraphicsTool.OnClickCommand(&m_CmdMeshBind,message,wParam,lParam))
//			{
//				m_CmdMeshBind.State  = 1;
//				TCHAR MeshName[256];
//				KModel* pBindMesh = NULL;
//				if (SUCCEEDED(m_lpMeshListBox->GetCurSelectedString(MeshName,(DWORD*)(&pBindMesh))))
//				{
//					if (pBindMesh)
//					{
//						pBindMesh->Bind(pMesh,SocketName);
//					}
//				}
//			}
//		}
//	}
//
//	return KModelUIBase::ProcessMessage(message,wParam,lParam);
//}
//
//HRESULT KModelToolMesh::OpenMeshFile()
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
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolMesh::LoadBoneBlendFromMeshFile()
//{
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return E_FAIL;
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return E_FAIL;
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
//		if (SUCCEEDED(pMesh->LoadBoneBlendSetFromMeshFile(filename)))
//		{
//			FillBoneBlendListBox();
//		}
//	}
//	return S_OK;
//}
//
//void KModelToolMesh::FillBoneBlendListBox()
//{
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return ;
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return ;
//
//	m_lpBoneBlendListBox->Clear();
//
//	TCHAR Name[256];
//	for(int i=0;i<(int)pMesh->m_lpBoneBlendSet.size();i++)
//	{
//		KModel::LPBONEBLENDSET pSet = pMesh->m_lpBoneBlendSet[i];
//		wsprintf(Name,"%s",pSet->SetName);
//		m_lpBoneBlendListBox->AddString(Name);
//	}
//
//	if (pMesh->m_lpBoneBlendSet.size()>=2)
//	{
//		pMesh->m_nNumCurBlendSet = 2;
//		pMesh->m_dwBoneBlendsetID[0] = 0;
//		pMesh->m_dwBoneBlendsetID[1] = 1;
//	}
//}
//
//HRESULT KModelToolMesh::FillBoneListBox()
//{
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return E_FAIL;
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return E_FAIL;
//	m_lpBoneListBox->Clear();
//
//	TCHAR Name[256];
//	for(int i=0;i<(int)pMesh->m_dNumBone;i++)
//	{
//		KModel::LPBONEINFO pInfo = &pMesh->m_lpBoneInfo[i];
//		wsprintf(Name,"%s",pInfo->scName.c_str());
//		m_lpBoneListBox->AddString(Name,(DWORD)pInfo);
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolMesh::FillSocketListBox(KModel::LPBONEINFO pInfo)
//{
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return E_FAIL;
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return E_FAIL;
//	m_lpSocketListBox->Clear();
//
//	TCHAR Name[256];
//	for(int i=0;i<(int)pMesh->m_dNumSocket;i++)
//	{
//		KModel::LPSOCKET pSocket = &pMesh->m_lpSockets[i];
//		if (_strcmpi(pSocket->scParentBoneName.c_str(),pInfo->scName.c_str())==0)
//		{
//			wsprintf(Name,"%s",pSocket->scName.c_str());
//			m_lpSocketListBox->AddString(Name,(DWORD)pSocket);
//		}
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolMesh::SaveMeshFile()
//{
//	char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh|";
//
//	CFileDialog dlg(false, NULL, NULL,
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
//		LPMODEL pMesh = pMeshScene->GetCurMesh();
//		if (pMesh)
//		{
//			pMesh->SaveMesh(filename);
//		}
//	}
//	/*char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="X Files (*.X)|*.X|";
//
//	CFileDialog dlg(false, NULL, NULL,
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
//		wsprintf(filename,"%s%s%s.X",drive,dir,fname);
//
//		KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//		if (!pMeshScene)
//			return E_FAIL;
//
//		LPMODEL pMesh = pMeshScene->GetCurMesh();
//		if (pMesh)
//		{
//			pMesh->Save2XFile(filename);
//		}
//	}*/
//	return S_OK;
//}
};