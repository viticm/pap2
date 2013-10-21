// MeshObjectTableTool.cpp: implementation of the KModelToolObjectTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshObjectTableTool.h"
#include ".\meshobjecttabletool.h"
#include "SceneSkinEdit.h"

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
//KModelToolObjectTable::KModelToolObjectTable()
//{
//	wsprintf(m_LastDir,"%s",g_Def_ModelDirectory);
//
//	m_lpListBox = NULL;
//
//	m_bHide = TRUE;
//
//	m_FileStart = 0;
//	m_lpEdit = NULL;
//	
//	m_bObjectInHand = FALSE;
//
//}
//KModelToolObjectTable::~KModelToolObjectTable()
//{
//
//}
//
//HRESULT KModelToolObjectTable::LoadMesh(LPSTR pFileName)
//{
//	TCHAR Name[256];
//
//	wsprintf(Name,"%s\\ToolObject.ini",g_Def_AppDirectory);
//	IIniFile* pIni = g_OpenIniFile(Name);
//	if(!pIni)
//	{
//		return E_FAIL;
//	}	
//	KModelUIBase::Init(pIni,"Main");
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpListBox),MODELTYPE_LISTBOX);
//	m_lpListBox->Init(pIni,"MainList");
//	m_lpListBox->SetParentWnd(this);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpBOpen),MODELTYPE_BUTTON);
//	m_lpBOpen->Init(pIni,"MeshOpen");
//	m_lpBOpen->SetParentWnd(this);
//	m_lpBOpen->SetIconTexture("ToolBar\\Load.tga");
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_LpCObjectType),MODELTYPE_COMBOBOX);
//	m_LpCObjectType->Init(pIni,"TypeCombo");
//	m_LpCObjectType->AddString("模型",0);
//	m_LpCObjectType->AddString("任务NPC",1);
//	m_LpCObjectType->AddString("战斗NPC",2);
//	m_LpCObjectType->SetCurItembyValue(0);
//	m_LpCObjectType->SetParentWnd(this);
//
//	FindMeshFiles();	
//	RefreshCmd();
//	
//	pIni->Release();
//	return S_OK;
//}
//
//HRESULT KModelToolObjectTable::Render()
//{
//	if (!m_bHide)
//	{
//		ModelUI::KModelUIBase::Render();
//		
//		SAFE_RENDER(m_LpCObjectType);
//
//		SAFE_RENDER(m_lpListBox);
//		SAFE_RENDER(m_lpBOpen);
//
//	}
//
//	
//	return S_OK;
//}
//
//HRESULT KModelToolObjectTable::OpenMeshFile()
//{
//	KSceneSceneEdit* pSceneEditor = (KSceneSceneEdit*) m_lpEdit;
//
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
//		TCHAR filename[256];
//		wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);
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
//		pMesh->m_dwShadow = 0;
//		pSceneEditor->m_dwState = EDITSTATE_SELECT;
//
//		g_cObjectTable.Get1NewObject(&pSceneEditor->m_lpInHandObject,0);
//		pSceneEditor->m_lpInHandObject->AddMesh(NewMeshID);
//
//		wsprintf(m_LastDir,"%s%s",drive,dir);
//		FindMeshFiles();
//	};
//
//	return S_OK;
//}
//
//HRESULT KModelToolObjectTable::OpenNPCFile()
//{
//	KSceneSceneEdit* pSceneEditor = (KSceneSceneEdit*) m_lpEdit;
//
//	char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="NPC Define Files (*.Ini)|*.Ini|";
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
//		TCHAR filename[256];
//		wsprintf(filename,"%s%s%s.ini",drive,dir,fname);
//
//		LPMODEL pMesh = NULL;
//		DWORD   NewMeshID  = 0;
//
//		LPOBJECT pObject = NULL;
//		if (FAILED(g_cObjectTable.Get1NewObject(&pObject,OBJECTTYPE_NPC)))
//			return E_FAIL;
//
//		IIniFile* Ini = g_OpenIniFile(filename);
//		
//		if(SUCCEEDED(pObject->LoadByIni(Ini,filename)))
//		{
//			pSceneEditor->m_dwState = EDITSTATE_SELECT;
//			pSceneEditor->m_lpInHandObject = pObject;
//
//			wsprintf(m_LastDir,"%s%s",drive,dir);
//			FindNPCFiles();
//		}
//	};
//
//	return S_OK;
//}
//
//HRESULT KModelToolObjectTable::OnSelectMeshFile()
//{
//	KSceneSceneEdit* pSceneEditor = (KSceneSceneEdit*) m_lpEdit;
//
//	DWORD MeshID = 0;
//	TCHAR str[256];
//	TCHAR FileName[256];
//	m_lpListBox->GetCurSelectedString(FileName);
//
//	wsprintf(str,"%s\\%s",m_LastDir,FileName);
//	if (SUCCEEDED(g_cMeshTable.LoadFromFile(&MeshID,str)))
//	{
//		LPMODEL pMesh = NULL;
//		g_cMeshTable.GetXMesh(&pMesh,MeshID);
//
//		TCHAR path_buffer[256];
//		TCHAR drive[256];
//		TCHAR dir[256];
//		TCHAR fname[256];
//		TCHAR ext[256];
//		wsprintf(path_buffer,"%s",str);
//		_splitpath( path_buffer, drive, dir, fname, ext );
//		wsprintf(str,"%s%s%s.Mtl",drive,dir,fname);
//		pMesh->LoadMaterial(str);
//
//		pMesh->m_dwShadow = 0;
//		pSceneEditor->m_dwState = EDITSTATE_SELECT;
//
//		g_cObjectTable.Get1NewObject(&pSceneEditor->m_lpInHandObject,0);
//		pSceneEditor->m_lpInHandObject->AddMesh(MeshID);
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolObjectTable::OnSelectNPCFile()
//{
//	KSceneSceneEdit* pSceneEditor = (KSceneSceneEdit*) m_lpEdit;
//
//	TCHAR str[256];
//	TCHAR FileName[256];
//	m_lpListBox->GetCurSelectedString(FileName);
//
//	wsprintf(str,"%s\\%s",m_LastDir,FileName);
//
//	LPOBJECT pObject = NULL;
//	if (FAILED(g_cObjectTable.Get1NewObject(&pObject,OBJECTTYPE_NPC)))
//		return E_FAIL;
//
//	IIniFile* Ini = g_OpenIniFile(str);
//	
//
//	if(SUCCEEDED(pObject->LoadByIni(Ini,str)))
//	{
//		pSceneEditor->m_dwState = EDITSTATE_SELECT;
//		pSceneEditor->m_lpInHandObject = pObject;
//	}
//	
//	Ini->Release();
//	return S_OK;
//}
//
//HRESULT KModelToolObjectTable::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	KSceneSceneEdit* pSceneEditor = (KSceneSceneEdit*) m_lpEdit;
//	
//	if (m_bHide)
//		return S_OK;
//
//	if(m_bObjectInHand)
//	{
//		pSceneEditor->m_dwState = EDITSTATE_PLACEOBJECT;
//		m_bObjectInHand = FALSE;
//	}
//
//
//	if (COMBOBOX_ITEMSELECT == m_LpCObjectType->ProcessMessage(message,wParam,lParam))
//	{
//		DWORD Type = m_LpCObjectType->GetCuritemValue();
//		if(Type==0)
//			FindMeshFiles();
//		else
//			FindNPCFiles();
//	}
//
//	if(!m_LpCObjectType->m_bDroped)
//	{
//		DWORD Type = m_LpCObjectType->GetCuritemValue();
//
//		if(LISTBOX_SELECTCHANGE == m_lpListBox->ProcessMessage(message,wParam,lParam))
//		{
//			if(Type==0)
//				OnSelectMeshFile();
//			else
//				OnSelectNPCFile();
//			m_bObjectInHand = TRUE;
//		}
//
//		if(ITEM_CLICK == m_lpBOpen->ProcessMessage(message,wParam,lParam))
//		{
//			if(Type==0)
//				OpenMeshFile();
//			else
//				OpenNPCFile();
//			m_bObjectInHand = TRUE;
//		}
//	}
//
//	return KModelUIBase::ProcessMessage(message,wParam,lParam);
//
//}
//
///*BOOL KModelToolObjectTable::IsPositionInCommand(D3DXVECTOR2 *pPos, _Command *pCmd)
//{
//	D3DXVECTOR2 A,C;
//	A.x = m_MainLeft + m_MainWidth  * pCmd->X;
//	A.y = m_MainTop  + m_MainHeight * pCmd->Y;
//	
//	float R_W = m_MainWidth*m_RadioWidth   * pCmd->XS;
//	float R_H = m_RadioHeight*m_MainHeight * pCmd->YS;
//	C.x = A.x + R_W;
//	C.y = A.y + R_H;
//	
//	if ((pPos->x>=A.x)&&(pPos->x<=C.x)&&(pPos->y>=A.y)&&(pPos->y<=C.y))
//		return  TRUE;
//	else
//		return FALSE;
//}*/
//
///*HRESULT KModelToolObjectTable::CleanObjectState(DWORD State)
//{
//	for(DWORD i=0;i<m_NumCmd;i++)
//	{
//		_Command* pCmd = &m_Objects[i];
//		if (pCmd->m_State==State)
//			pCmd->m_State = 0;
//	}
//	return S_OK;
//}*/
//
//HRESULT KModelToolObjectTable::FindMeshFiles(void)
//{
//	m_lpListBox->Clear();
//
//	TCHAR path_buffer[256];
//	TCHAR drive[256];
//	TCHAR dir[256];
//	TCHAR fname[256];
//	TCHAR ext[256];
//
//	WIN32_FIND_DATA Find;
//	TCHAR str[256];
//	wsprintf(str,"%s\\*.Mesh",m_LastDir);
//	HANDLE hFind = FindFirstFile(str,&Find);
//	if (hFind != INVALID_HANDLE_VALUE)
//	{
//		wsprintf(path_buffer,"%s",Find.cFileName);
//		_splitpath( path_buffer, drive, dir, fname, ext );
//		wsprintf(str,"%s%s",fname,ext);
//		m_lpListBox->AddString(str);
//
//		while(FindNextFile(hFind,&Find))
//		{
//			wsprintf(path_buffer,"%s",Find.cFileName);
//			_splitpath( path_buffer, drive, dir, fname, ext );
//			wsprintf(str,"%s%s",fname,ext);
//			m_lpListBox->AddString(str);
//		}
//		FindClose(hFind);
//	}
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolObjectTable::FindNPCFiles(void)
//{
//	m_lpListBox->Clear();
//
//	TCHAR path_buffer[256];
//	TCHAR drive[256];
//	TCHAR dir[256];
//	TCHAR fname[256];
//	TCHAR ext[256];
//
//	WIN32_FIND_DATA Find;
//	TCHAR str[256];
//	wsprintf(str,"%s\\*.ini",m_LastDir);
//	HANDLE hFind = FindFirstFile(str,&Find);
//	if (hFind != INVALID_HANDLE_VALUE)
//	{
//		wsprintf(path_buffer,"%s",Find.cFileName);
//		_splitpath( path_buffer, drive, dir, fname, ext );
//		wsprintf(str,"%s%s",fname,ext);
//		m_lpListBox->AddString(str);
//
//		while(FindNextFile(hFind,&Find))
//		{
//			wsprintf(path_buffer,"%s",Find.cFileName);
//			_splitpath( path_buffer, drive, dir, fname, ext );
//			wsprintf(str,"%s%s",fname,ext);
//			m_lpListBox->AddString(str);
//		}
//		FindClose(hFind);
//	}
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolObjectTable::RefreshCmd(void)
//{
//
////End:
//
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolObjectTable::SetSceneEditor(PVOID pSceneEditor)
//{
//	m_lpEdit = pSceneEditor;
//	return E_NOTIMPL;
//}
};
