// MeshMainTool.cpp: implementation of the KModelToolTerrain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshMainTool.h"
#include "SceneSkinEdit.h"
#include "MeshTerran.h"

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

//KModelToolTerrain::KModelToolTerrain()
//{
//	m_lpHeightOpIcon = NULL;
//
//	m_pTerrainListCtrlUp = NULL;
//	m_pTerrainTitleItemUp = NULL;
//	//m_pTerrainListCtrlDn = NULL;
//	//m_pTerrainTitleItemDn = NULL;
//
//	m_lpMap = NULL;
//	m_bHide = FALSE;
//
//
//	MaterialUp = 0;
//	MaterialDn = 0;
//
//}
//
//KModelToolTerrain::~KModelToolTerrain()
//{
//
//}
//
//HRESULT KModelToolTerrain::CleanUp()
//{    
//
//	SAFE_DELETE_ARRAY(m_lpHeightOpIcon);
//    return KModelUIBase::CleanUp();
//}
//
//
//HRESULT KModelToolTerrain::LoadMesh(LPSTR pFileName)
//{
//	TCHAR Name[256];
//	
//	wsprintf(Name,"%s\\ToolTerrain.ini",g_Def_AppDirectory);
//	IIniFile* pIni = g_OpenIniFile(Name);
//
//	if(!pIni)
//	{
//		return E_FAIL;
//	}	
//	KModelUIBase::Init(pIni,"Main");
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainTitleItemUp),MODELTYPE_ITEM);
//	m_pTerrainTitleItemUp->Init(pIni,"TerrainUp");
//	m_pTerrainTitleItemUp->SetParentWnd(this);
//	m_pTerrainTitleItemUp->m_bDrawBorder = FALSE;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainListCtrlUp),MODELTYPE_LISTCTRL);
//	m_pTerrainListCtrlUp->Init(pIni,"TerrainMtlListUp");
//	m_pTerrainListCtrlUp->SetParentWnd(this);
//	m_pTerrainListCtrlUp->m_bMultiSelect = FALSE;
//
//	/*g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainTitleItemDn),MODELTYPE_ITEM);
//	m_pTerrainTitleItemDn->Init(&MIni,"TerrainDn");
//	m_pTerrainTitleItemDn->SetParentWnd(this);
//	m_pTerrainTitleItemDn->m_bDrawBorder = FALSE;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainListCtrlDn),MODELTYPE_LISTCTRL);
//	m_pTerrainListCtrlDn->Init(&MIni,"TerrainMtlListDn");
//	m_pTerrainListCtrlDn->SetParentWnd(this);
//	m_pTerrainListCtrlDn->m_bMultiSelect = FALSE;*/
//
//	//////////////////////////////////////////////////////////////////////////
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainHeightItem),MODELTYPE_ITEM);
//	m_pTerrainHeightItem->Init(pIni,"TerrainHeight");
//	m_pTerrainHeightItem->SetParentWnd(this);
//	m_pTerrainHeightItem->m_bDrawBorder = FALSE;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainHeightListCtrl),MODELTYPE_LISTCTRL);
//	m_pTerrainHeightListCtrl->Init(pIni,"TerrainHeightList");
//	m_pTerrainHeightListCtrl->SetParentWnd(this);
//	m_pTerrainHeightListCtrl->m_bMultiSelect = FALSE;
//	
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainHeightValueItem),MODELTYPE_ITEM);
//	m_pTerrainHeightValueItem->Init(pIni,"TerrainHeightValue");
//	m_pTerrainHeightValueItem->SetParentWnd(this);
//	m_pTerrainHeightValueItem->m_bDrawBorder = FALSE;
//	
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainHeightValueEdit),MODELTYPE_EDITCTRL);
//	m_pTerrainHeightValueEdit->Init(pIni,"TerrainHeightValueInput");
//	m_pTerrainHeightValueEdit->SetParentWnd(this);
//	m_pTerrainHeightValueEdit->SetText("1.0");
//	
//
//	//////////////////////////////////////////////////////////////////////////
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainAlphaItem),MODELTYPE_ITEM);
//	m_pTerrainAlphaItem->Init(pIni,"TerrainAlpha");
//	m_pTerrainAlphaItem->SetParentWnd(this);
//	m_pTerrainAlphaItem->m_bDrawBorder = FALSE;
//
//	/*g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainAlphaListCtrl),MODELTYPE_LISTCTRL);
//	m_pTerrainAlphaListCtrl->Init(&MIni,"TerrainAlphaList");
//	m_pTerrainAlphaListCtrl->SetParentWnd(this);
//	m_pTerrainAlphaListCtrl->m_bMultiSelect = FALSE;*/
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainAlphaValueItem),MODELTYPE_ITEM);
//	m_pTerrainAlphaValueItem->Init(pIni,"TerrainAlphaValue");
//	m_pTerrainAlphaValueItem->SetParentWnd(this);
//	m_pTerrainAlphaValueItem->m_bDrawBorder = FALSE;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainAlphaValueEdit),MODELTYPE_EDITCTRL);
//	m_pTerrainAlphaValueEdit->Init(pIni,"TerrainAlphaValueInput");
//	m_pTerrainAlphaValueEdit->SetParentWnd(this);
//	//////////////////////////////////////////////////////////////////////////
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainXuanYaItem),MODELTYPE_ITEM);
//	m_pTerrainXuanYaItem->Init(pIni,"TerrainXuanYa");
//	m_pTerrainXuanYaItem->SetParentWnd(this);
//	m_pTerrainXuanYaItem->m_bDrawBorder = FALSE;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainXuanYaListCtrl),MODELTYPE_LISTCTRL);
//	m_pTerrainXuanYaListCtrl->Init(pIni,"TerrainXuanYaList");
//	m_pTerrainXuanYaListCtrl->SetParentWnd(this);
//	m_pTerrainXuanYaListCtrl->m_bMultiSelect = FALSE;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainXuanYaValueItem),MODELTYPE_ITEM);
//	m_pTerrainXuanYaValueItem->Init(pIni,"TerrainXuanYaValue");
//	m_pTerrainXuanYaValueItem->SetParentWnd(this);
//	m_pTerrainXuanYaValueItem->m_bDrawBorder = FALSE;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainXuanYaValueEdit),MODELTYPE_EDITCTRL);
//	m_pTerrainXuanYaValueEdit->Init(pIni,"TerrainXuanYaValueInput");
//	m_pTerrainXuanYaValueEdit->SetParentWnd(this);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainXuanYaTypeItem),MODELTYPE_ITEM);
//	m_pTerrainXuanYaTypeItem->Init(pIni,"TerrainXuanYaType");
//	m_pTerrainXuanYaTypeItem->SetParentWnd(this);
//	m_pTerrainXuanYaTypeItem->m_bDrawBorder = FALSE;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pTerrainXuanYaTypeListCtrl),MODELTYPE_LISTCTRL);
//	m_pTerrainXuanYaTypeListCtrl->Init(pIni,"TerrainXuanYaTypeList");
//	m_pTerrainXuanYaTypeListCtrl->SetParentWnd(this);
//	m_pTerrainXuanYaTypeListCtrl->m_bMultiSelect = FALSE;
//	//////////////////////////////////////////////////////////////////////////
//
//	LoadHeightOpIcon();
//
//	pIni->Release();
//	return S_OK;
//}
//
//HRESULT KModelToolTerrain::Render()
//{	
//
//	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
//	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
//	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
//	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
//	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
//
//	if (!m_bHide)
//	{
//		KModelUIBase::Render();
//		SAFE_RENDER(m_pTerrainTitleItemUp);
//		SAFE_RENDER(m_pTerrainListCtrlUp);
//		//SAFE_RENDER(m_pTerrainTitleItemDn);
//		//SAFE_RENDER(m_pTerrainListCtrlDn);
//	
//		SAFE_RENDER(m_pTerrainHeightItem);
//		SAFE_RENDER(m_pTerrainHeightListCtrl);
//		SAFE_RENDER(m_pTerrainHeightValueItem);
//		SAFE_RENDER(m_pTerrainHeightValueEdit);
//
//		SAFE_RENDER(m_pTerrainAlphaItem);
//		//SAFE_RENDER(m_pTerrainAlphaListCtrl);
//		SAFE_RENDER(m_pTerrainAlphaValueItem);
//		SAFE_RENDER(m_pTerrainAlphaValueEdit);
//		
//		SAFE_RENDER(m_pTerrainXuanYaItem);
//		SAFE_RENDER(m_pTerrainXuanYaListCtrl);
//		SAFE_RENDER(m_pTerrainXuanYaValueItem);
//		SAFE_RENDER(m_pTerrainXuanYaValueEdit);
//		SAFE_RENDER(m_pTerrainXuanYaTypeItem);
//		SAFE_RENDER(m_pTerrainXuanYaTypeListCtrl);
//		
//	}
//
//	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
//	return S_OK;
//}
//
//
//HRESULT KModelToolTerrain::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	
//	KSceneSceneEdit* pSceneEditor = (KSceneSceneEdit*) m_lpEdit;
//
//	if (!m_lpMap)
//		return S_OK;
//
//	
//	if (m_bHide)
//		return S_OK;
//
//	if(ITEM_CLICK == m_pTerrainListCtrlUp->ProcessMessage(message,wParam,lParam))
//	{
//		pSceneEditor->m_dwState = EDITSTATE_TERRANEDIT;
//		for(int i=0;i<(int)m_pTerrainListCtrlUp->m_ListItems.size();i++)
//		{
//			if(m_pTerrainListCtrlUp->m_ListItems[i].dwState)
//			{
//				m_lpMap->m_dwState = MAP_PAINT_GROUND;
//				TCHAR Str[256];
//				wsprintf(Str,"应用下层纹理—%s",m_lpMap->m_vecGroundTextGroups[i]->SetName);
//				m_pTerrainTitleItemUp->SetText(Str);
//				//wsprintf(Str,"应用上层纹理—无");
//				//m_pTerrainTitleItemDn->SetText(Str);
//
//				TCHAR Name[256];
//				wsprintf(Name,"%s\\Music\\BUTTON1.WAV",g_Def_AppDirectory);
//				g_pSoundSys->PlayAmusic(Name,PlayNormal);
//				break;
//			}
//		}
//	}
//
//	/*if(ITEM_CLICK == m_pTerrainListCtrlDn->ProcessMessage(message,wParam,lParam))
//	{
//		pSceneEditor->m_dwState = EDITSTATE_TERRANEDIT;
//		for(int i=0;i<(int)m_pTerrainListCtrlDn->m_ListItems.size();i++)
//		{
//			if(m_pTerrainListCtrlDn->m_ListItems[i].dwState)
//			{
//				m_lpMap->m_dwState = MAP_PAINT_GROUND_DN;
//				m_lpMap->m_CurMaterialSetDn = i;
//				TCHAR Str[256];
//				wsprintf(Str,"应用上层纹理—%s",m_lpMap->m_vecGroundTextGroups[i]->SetName);
//				m_pTerrainTitleItemDn->SetText(Str);
//				wsprintf(Str,"应用下层纹理—无");
//				m_pTerrainTitleItemUp->SetText(Str);
//
//				TCHAR Name[256];
//				wsprintf(Name,"%s\\Music\\BUTTON2.WAV",g_Def_AppDirectory);
//				g_pSoundSys->PlayAmusic(Name,PlayNormal);
//
//				break;
//			}
//		}
//	}*/
//
//
//	if(ITEM_CLICK == m_pTerrainHeightListCtrl->ProcessMessage(message,wParam,lParam))
//	{
//		pSceneEditor->m_dwState = EDITSTATE_TERRANEDIT;
//		if(m_pTerrainHeightListCtrl->m_ListItems[0].dwState)
//		{
//			m_lpMap->m_dwState = GROUND_UP;
//			m_pTerrainHeightItem->SetText("应用高度—升高地表");
//		}
//		else if(m_pTerrainHeightListCtrl->m_ListItems[1].dwState)
//		{
//			m_lpMap->m_dwState = GROUND_DOWN;
//			m_pTerrainHeightItem->SetText("应用高度—降低地表");
//		}
//		else if(m_pTerrainHeightListCtrl->m_ListItems[2].dwState)
//		{
//			m_lpMap->m_dwState = GROUND_FLAT;
//			m_pTerrainHeightItem->SetText("应用高度—平整地表");
//		}
//		else if(m_pTerrainHeightListCtrl->m_ListItems[3].dwState)
//		{
//			m_lpMap->m_dwState = GROUND_NOISE;
//			m_pTerrainHeightItem->SetText("应用高度—混乱地表");
//		}
//		else if(m_pTerrainHeightListCtrl->m_ListItems[4].dwState)
//		{
//			m_lpMap->m_dwState = GROUND_SET;
//			m_pTerrainHeightItem->SetText("应用高度—设定地表高度");
//		}	
//
//		TCHAR Name[256];
//		wsprintf(Name,"%s\\Music\\BUTTON3.WAV",g_Def_AppDirectory);
//		g_pSoundSys->PlayAmusic(Name,PlayNormal);
//
//	}
//	
//	/*if(ITEM_CLICK == m_pTerrainAlphaListCtrl->ProcessMessage(message,wParam,lParam))
//	{
//		pSceneEditor->m_dwState = EDITSTATE_TERRANEDIT;
//		if(m_pTerrainAlphaListCtrl->m_ListItems[0].dwState)
//		{
//			m_lpMap->m_dwState = MAP_ALPHA_UP;
//			m_pTerrainAlphaItem->SetText("应用混合—增大系数");
//		}
//		else if(m_pTerrainAlphaListCtrl->m_ListItems[1].dwState)
//		{
//			m_lpMap->m_dwState = MAP_ALPHA_DOWN;
//			m_pTerrainAlphaItem->SetText("应用混合—减小系数");
//		}
//		else if(m_pTerrainAlphaListCtrl->m_ListItems[2].dwState)
//		{
//			m_lpMap->m_dwState = MAP_ALPHA_FLAT;
//			m_pTerrainAlphaItem->SetText("应用混合—平整系数");
//		}
//		else if(m_pTerrainAlphaListCtrl->m_ListItems[3].dwState)
//		{
//			m_lpMap->m_dwState = MAP_ALPHA_NOISE;
//			m_pTerrainAlphaItem->SetText("应用混合—随机系数");
//		}
//		else if(m_pTerrainAlphaListCtrl->m_ListItems[4].dwState)
//		{
//			m_lpMap->m_dwState = MAP_ALPHA_SET;
//			m_pTerrainAlphaItem->SetText("应用混合—系数设置");
//		}
//
//		TCHAR Name[256];
//		wsprintf(Name,"%s\\Music\\BUTTON4.WAV",g_Def_AppDirectory);
//		g_pSoundSys->PlayAmusic(Name,PlayNormal);
//
//	}*/
//
//	if(EDIT_RETURN == m_pTerrainHeightValueEdit->ProcessMessage(message,wParam,lParam))
//	{
//		float Value = 0;
//		sscanf(m_pTerrainHeightValueEdit->m_szString,"%f",&Value);
//		m_lpMap->m_fHeightValue = Value;
//		
//		TCHAR Name[256];
//		wsprintf(Name,"%s\\Music\\BUTTON5.WAV",g_Def_AppDirectory);
//		g_pSoundSys->PlayAmusic(Name,PlayNormal);
//	}
//
//	if(EDIT_RETURN == m_pTerrainAlphaValueEdit->ProcessMessage(message,wParam,lParam))
//	{
//		float Value = 0;
//		sscanf(m_pTerrainAlphaValueEdit->m_szString,"%f",&Value);
//		m_lpMap->m_fAlphaValue = Value;
//
//		TCHAR Name[256];
//		wsprintf(Name,"%s\\Music\\BUTTON6.WAV",g_Def_AppDirectory);
//		g_pSoundSys->PlayAmusic(Name,PlayNormal);
//
//	}
//
//	return KModelUIBase::ProcessMessage(message,wParam,lParam);
// 
//}
//
//HRESULT KModelToolTerrain::SetMap(KModelTerrain *pMap,BOOL NeedClear)
//{
//	m_lpMap = pMap;
//	
//	if(NeedClear)
//	{
//		if(m_pTerrainListCtrlUp)
//			m_pTerrainListCtrlUp->Clear();
//		//if(m_pTerrainListCtrlDn)
//		//	m_pTerrainListCtrlDn->Clear();
//	}
//
//
//	DWORD TextIndex = 0;
//	for(DWORD i=0;i<m_lpMap->m_vecGroundTextGroups.size();i++)
//	{
//		KModelTerrain::_GroundTextureGroup* pGroup = m_lpMap->m_vecGroundTextGroups[i];
//		if(pGroup->vecTextureID.size()>0)
//		{
//			DWORD Index = TextIndex*m_lpMap->m_dwNumGroundTexture+TextIndex;
//			if(NeedClear)
//			{
//				if(m_pTerrainListCtrlUp)
//					m_pTerrainListCtrlUp->AddItemMaterial(&m_lpMap->m_lpMaterial[Index]);
//				//if(m_pTerrainListCtrlDn)
//				//	m_pTerrainListCtrlDn->AddItemMaterial(&m_lpMap->m_lpMaterial[Index]);
//			}
//			else
//			{
//				if(m_pTerrainListCtrlUp)
//					m_pTerrainListCtrlUp->m_ListItems[i].dwData = 
//						(DWORD) &m_lpMap->m_lpMaterial[Index];
//				//if(m_pTerrainListCtrlDn)
//				//	m_pTerrainListCtrlDn->m_ListItems[i].dwData = 
//				//		(DWORD) &m_lpMap->m_lpMaterial[Index];
//			}
//
//		}
//		TextIndex += pGroup->vecTextureID.size();
//	}
//
//	if(NeedClear)
//	{
//		TCHAR str[256];
//		sprintf(str,"%.1f",m_lpMap->m_fAlphaValue);
//		m_pTerrainAlphaValueEdit->SetText(str);
//		sprintf(str,"%.1f",m_lpMap->m_fHeightValue);
//		m_pTerrainHeightValueEdit->SetText(str);
//	}
//
//	return S_OK;
//}
//
//
//
//HRESULT KModelToolTerrain::SetSceneEditor(PVOID pSceneEditor)
//{
//	m_lpEdit = pSceneEditor;
//	return S_OK;
//}
//
//
//HRESULT KModelToolTerrain::LoadHeightOpIcon()
//{
//	SAFE_DELETE_ARRAY(m_lpHeightOpIcon);
//	m_lpHeightOpIcon = new MATERIAL[5];
//	
//	TCHAR Name[256];
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Toolbar\\Ground_Up.tga");
//	if(FAILED(g_cTextureTable.LoadFromFile(&m_lpHeightOpIcon[0].m_dTextureIDs[0],Name)))
//	{
//		MessageBox(g_hBaseWnd,Name,"Failed to Load!",0);
//	}
//
//	m_pTerrainHeightListCtrl->AddItemMaterial(&m_lpHeightOpIcon[0]);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Toolbar\\Ground_Down.tga");
//	g_cTextureTable.LoadFromFile(&m_lpHeightOpIcon[1].m_dTextureIDs[0],Name);
//	m_pTerrainHeightListCtrl->AddItemMaterial(&m_lpHeightOpIcon[1]);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Toolbar\\Ground_Flat.tga");
//	g_cTextureTable.LoadFromFile(&m_lpHeightOpIcon[2].m_dTextureIDs[0],Name);
//	m_pTerrainHeightListCtrl->AddItemMaterial(&m_lpHeightOpIcon[2]);
//		
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Toolbar\\Ground_Noise.tga");
//	g_cTextureTable.LoadFromFile(&m_lpHeightOpIcon[3].m_dTextureIDs[0],Name);
//	m_pTerrainHeightListCtrl->AddItemMaterial(&m_lpHeightOpIcon[3]);
//	
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Toolbar\\Ground_Flat.tga");
//	g_cTextureTable.LoadFromFile(&m_lpHeightOpIcon[4].m_dTextureIDs[0],Name);
//	m_pTerrainHeightListCtrl->AddItemMaterial(&m_lpHeightOpIcon[4]);
//
//	//////////////////////////////////////////////////////////////////////////
//
//	/*m_pTerrainAlphaListCtrl->AddItemMaterial(&m_lpHeightOpIcon[0]);
//
//
//	m_pTerrainAlphaListCtrl->AddItemMaterial(&m_lpHeightOpIcon[1]);
//
//
//	m_pTerrainAlphaListCtrl->AddItemMaterial(&m_lpHeightOpIcon[2]);
//
//
//	m_pTerrainAlphaListCtrl->AddItemMaterial(&m_lpHeightOpIcon[3]);
//
//
//	m_pTerrainAlphaListCtrl->AddItemMaterial(&m_lpHeightOpIcon[4]);*/
//
//	return S_OK;
//}
};