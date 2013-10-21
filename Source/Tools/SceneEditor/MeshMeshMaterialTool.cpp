#include "stdafx.h"
#include ".\meshmeshmaterialtool.h"
#include ".\scenemesheditor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace ModelTools
{
//KModelToolMaterial::KModelToolMaterial(void)
//{
//	m_lpDropAlphaOperation = NULL;
//
//	m_lpMtlOptionComboBox = NULL;
//	m_lpMtlOptionWnd = NULL;
//
//	m_nCurTextureOp = -1;
//
//	m_CurModifyModel = NULL;
//	m_bUpdateAlphaEdit = TRUE;
//	m_bUpdatePowerEdit = TRUE;
//
//	m_pLTextUV = NULL;
//	m_lpDropTextOp = NULL;
//
//	m_NowShowTextOp = TEXTURE_OPERATION;
//	m_pITextUV = NULL;
//	m_pITextOp = NULL;
//	m_lpTextOptionWnd = NULL;
//
//	m_pBRemoveTexture = NULL;
//	m_pBAddTexture = NULL;
//	m_pBLoad = NULL;
//	m_BMtlOption = NULL;
//
//	m_lpMaterialListCtrl = NULL;
//
//	m_bShowTextureOption = FALSE;
//
//	m_lpColorOpComboBox = NULL;
//	m_lpAlphaOpComboBox = NULL;
//
//	m_lpResultComboBox = NULL;
//
//	m_lpColorArg1ComboBox = NULL;
//	m_lpColorArg2ComboBox = NULL;
//	m_lpAlphaArg1ComboBox = NULL;
//	m_lpAlphaArg2ComboBox = NULL;
//
//	m_lpPowerEditCtrl = NULL;
//	m_lpAlphaEditCtrl = NULL;
//
//	m_bHide = TRUE;
//
//	m_dwCurColorOperation = COLOR_NONE;
//	m_lpScene = NULL;
//
//	//m_dwListStyle = 0;
//	//m_dwMaterialStart = 0;
//
//	//m_pMaterialSelect = NULL;
//	m_dwNumMaterial = 0;
//
//	//m_lpEditCtrl = NULL;
//
//	IIniFile* Ini = g_OpenIniFile("d:/TEMP.ini");
//
//	float x = 0;
//	Ini->GetFloat("AAA","123",1,&x);
//	Ini->GetFloat("BBB","123",1,&x);
//
//	Ini->Release();
//
//}
//
//KModelToolMaterial::~KModelToolMaterial(void)
//{
//
//}
//
//HRESULT KModelToolMaterial::LoadMesh(LPSTR pFileName)
//{
//	TCHAR Name[256];
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Textures\\Mtltool.tga");
//	ModelUI::FRECT Rect(0.80f,0.05f,0.20f,1.4f);
//	Create(&Rect,"",0,0);
//	SetTitleCaption("材质工具");
//
//	KModelUIBase::m_bEnbaleScroll = TRUE;
//
//	TextOpLoad();
//	TextUVLoad();
//
//	//////////////////////////////////////////////////////////////////////////
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpPowerEditCtrl,MODELTYPE_EDITCTRL);
//	ModelUI::FRECT Rect1(0.82f,0.59f,0.085f,0.03f);
//	m_lpPowerEditCtrl->Create(&Rect1,"",0,0);
//	m_lpPowerEditCtrl->SetParentWnd(this);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpMaterialListCtrl,MODELTYPE_LISTCTRL);
//	ModelUI::FRECT Rect_List(0.805f,0.075f,0.19f,0.34f);
//	m_lpMaterialListCtrl->Create(&Rect_List,"",0,0);
//	m_lpMaterialListCtrl->SetParentWnd(this);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpAlphaEditCtrl,MODELTYPE_EDITCTRL);
//	ModelUI::FRECT Rect2(0.91f,0.59f,0.085f,0.03f);
//	m_lpAlphaEditCtrl->Create(&Rect2,"",0,0);
//	m_lpAlphaEditCtrl->SetParentWnd(this);
//
//	m_fTexturesTop = 0.625f - 0.05f;
//    m_fTexturesHeight = 0.03f;
//
//	for(int i=0;i<8;i++)
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpTexturesItem[i],MODELTYPE_ITEM);
//
//		float Top = m_fTexturesTop + m_RelativeRect.Top;
//		float Height = m_fTexturesHeight;
//		ModelUI::FRECT Rect(0.82f,Top + Height*i,
//			0.175f,Height);
//		m_lpTexturesItem[i]->Create(&Rect,"",0,0);
//		m_lpTexturesItem[i]->SetParentWnd(this);
//
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_BTextureOption[i],MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.805f,Top+Height*i,0.017f,Height);
//		m_BTextureOption[i]->Create(&TRect,"",0,0);
//		m_BTextureOption[i]->SetIconTexture("Toolbar\\MainTool_Left.tga");
//		m_BTextureOption[i]->SetParentWnd(this);
//	}
//
//	RefreshMaterialList(0);
//
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBAll,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+1*0.033f,0.432f,0.03f,0.03f);
//		m_pBAll->Create(&TRect,"",0,0);
//		m_pBAll->SetIconTexture("ToolBar\\All.tga");
//		m_pBAll->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBNone,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+2*0.033f,0.432f,0.03f,0.03f);
//		m_pBNone->Create(&TRect,"",0,0);
//		m_pBNone->SetIconTexture("ToolBar\\None.tga");
//		m_pBNone->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBInvert,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+3*0.033f,0.432f,0.03f,0.03f);
//		m_pBInvert->Create(&TRect,"",0,0);
//		m_pBInvert->SetIconTexture("ToolBar\\Invert.tga");
//		m_pBInvert->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBLoad,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+4*0.033f,0.432f,0.03f,0.03f);
//		m_pBLoad->Create(&TRect,"",0,0);
//		m_pBLoad->SetIconTexture("ToolBar\\Load.tga");
//		m_pBLoad->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBSave,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+5*0.033f,0.432f,0.03f,0.03f);
//		m_pBSave->Create(&TRect,"",0,0);
//		m_pBSave->SetIconTexture("ToolBar\\Save.tga");
//		m_pBSave->SetParentWnd(this);
//	}
//
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBAddTexture,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+4*0.033f,1.352f,0.03f,0.03f);
//		m_pBAddTexture->Create(&TRect,"",0,0);
//		m_pBAddTexture->SetIconTexture("ToolBar\\Load.tga");
//		m_pBAddTexture->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBRemoveTexture,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+5*0.033f,1.352f,0.03f,0.03f);
//		m_pBRemoveTexture->Create(&TRect,"",0,0);
//		m_pBRemoveTexture->SetIconTexture("ToolBar\\AniDelete.tga");
//		m_pBRemoveTexture->SetParentWnd(this);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	//
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBMtlWire,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+0*0.033f,0.47f,0.03f,0.03f);
//		m_pBMtlWire->Create(&TRect,"",0,0);
//		m_pBMtlWire->SetIconTexture("ToolBar\\MtlZEnable.tga");
//		m_pBMtlWire->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBMtlZEnable,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+1*0.033f,0.47f,0.03f,0.03f);
//		m_pBMtlZEnable->Create(&TRect,"",0,0);
//		m_pBMtlZEnable->SetIconTexture("ToolBar\\MtlZEnable.tga");
//		m_pBMtlZEnable->SetParentWnd(this);
//	}	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBMtlGouraud,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+2*0.033f,0.47f,0.03f,0.03f);
//		m_pBMtlGouraud->Create(&TRect,"",0,0);
//		m_pBMtlGouraud->SetIconTexture("ToolBar\\MtlGouraud.tga");
//		m_pBMtlGouraud->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBMtl2Faces,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+3*0.033f,0.47f,0.03f,0.03f);
//		m_pBMtl2Faces->Create(&TRect,"",0,0);
//		m_pBMtl2Faces->SetIconTexture("ToolBar\\Mtl2Faces.tga");
//		m_pBMtl2Faces->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBMtlLighting,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+4*0.033f,0.47f,0.03f,0.03f);
//		m_pBMtlLighting->Create(&TRect,"",0,0);
//		m_pBMtlLighting->SetIconTexture("ToolBar\\MtlLighting.tga");
//		m_pBMtlLighting->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pBMtlSpecular,MODELTYPE_BUTTON);
//		ModelUI::FRECT TRect(0.80f+5*0.033f,0.47f,0.03f,0.03f);
//		m_pBMtlSpecular->Create(&TRect,"",0,0);
//		m_pBMtlSpecular->SetIconTexture("ToolBar\\MtlSpecular.tga");
//		m_pBMtlSpecular->SetParentWnd(this);
//	}
//	///////////////////////////////////////////////////////////////////
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pIAmbient,MODELTYPE_ITEM);
//		ModelUI::FRECT TRect(0.820f+0*0.033f,0.51f,0.085f,0.03f);
//		m_pIAmbient->Create(&TRect,"",0,ITEMSTYLE_COLOR);
//		m_pIAmbient->SetText("Ambient");
//		m_pIAmbient->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pIDiffuse,MODELTYPE_ITEM);
//		ModelUI::FRECT TRect(0.820f+0*0.033f,0.55f,0.085f,0.03f);
//		m_pIDiffuse->Create(&TRect,"",0,ITEMSTYLE_COLOR);
//		m_pIDiffuse->SetText("Diffuse");
//		m_pIDiffuse->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pISpecular,MODELTYPE_ITEM);
//		ModelUI::FRECT TRect(0.910f+0*0.033f,0.51f,0.085f,0.03f);
//		m_pISpecular->Create(&TRect,"",0,ITEMSTYLE_COLOR);
//		m_pISpecular->SetText("Specular");
//		m_pISpecular->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pIEmissive,MODELTYPE_ITEM);
//		ModelUI::FRECT TRect(0.910f+0*0.033f,0.55f,0.085f,0.03f);
//		m_pIEmissive->Create(&TRect,"",0,ITEMSTYLE_COLOR);
//		m_pIEmissive->SetText("Emissive");
//		m_pIEmissive->SetParentWnd(this);
//	}
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_BMtlOption),MODELTYPE_BUTTON);
//	ModelUI::FRECT RBMtlOption(0.802f+0*0.033f,0.51f,0.015f,0.07f);
//	m_BMtlOption->Create(&RBMtlOption,"",0,0);
//	m_BMtlOption->SetIconTexture("Toolbar\\MainTool_Left.tga");
//	m_BMtlOption->SetParentWnd(this);
//
//	MaterialOptionLoad();
//
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::Render(void)
//{
//	if (m_bHide)
//		return S_OK;
//
//	KModelUIBase::Render();
//
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return S_OK;
//
//	if ((pMesh!=m_CurModifyModel)||(m_dwNumMaterial!=pMesh->m_dNumMaterial))
//	{
//		m_dwNumMaterial = pMesh->m_dNumMaterial;
//
//		m_lpMaterialListCtrl->Clear();
//		for(int k =0;k<(int)pMesh->m_dNumMaterial;k++)
//		{
//			KModel::LPMATERIAL pMat = &pMesh->m_lpMaterial[k];
//			pMat->m_dOption |= MATERIAL_OPTION_FILL_SOLID;
//			m_lpMaterialListCtrl->AddItemMaterial(pMat);
//		}
//		m_CurModifyModel = pMesh;
//		m_bUpdatePowerEdit = TRUE;
//		m_bUpdateAlphaEdit = TRUE;
//	}
//
//	SAFE_RENDER(m_lpPowerEditCtrl);
//	SAFE_RENDER(m_lpAlphaEditCtrl);
//
//	SAFE_RENDER(m_lpMaterialListCtrl);
//
//	RenderMaterialList();
//
//	SAFE_RENDER(m_pBAll);
//	SAFE_RENDER(m_pBNone);
//	SAFE_RENDER(m_pBInvert);
//	SAFE_RENDER(m_pBLoad);
//	SAFE_RENDER(m_pBSave);
//
//	SAFE_RENDER(m_pBAddTexture);
//	SAFE_RENDER(m_pBRemoveTexture);
//
//	int index = GetFirstSelectedMaterial();
//	if (index>=0)
//	{
//		m_pIAmbient->SetColor(pMesh->m_lpMaterial[index].
//			m_sMaterial9.Ambient);
//		m_pIDiffuse->SetColor(pMesh->m_lpMaterial[index].
//			m_sMaterial9.Diffuse);
//		m_pISpecular->SetColor(pMesh->m_lpMaterial[index].
//			m_sMaterial9.Specular);
//		m_pIEmissive->SetColor(pMesh->m_lpMaterial[index].
//			m_sMaterial9.Emissive);
//
//		SAFE_RENDER(m_pIAmbient);
//		SAFE_RENDER(m_pIDiffuse);
//		SAFE_RENDER(m_pISpecular);
//		SAFE_RENDER(m_pIEmissive);
//
//		//////////////////////////////////////////////////////////////////////////
//
//		if (pMesh->m_lpMaterial[index].m_dOption&MATERIAL_OPTION_ZBUFFER_TRUE)
//			m_pBMtlZEnable->m_dwState = 2;
//		else
//			m_pBMtlZEnable->m_dwState = 1;
//
//		if (pMesh->m_lpMaterial[index].m_dOption&MATERIAL_OPTION_SHADE_GOURAUD)
//			m_pBMtlGouraud->m_dwState = 2;
//		else
//			m_pBMtlGouraud->m_dwState = 1;
//
//		if (pMesh->m_lpMaterial[index].m_dOption&MATERIAL_OPTION_CULL_NONE)
//			m_pBMtl2Faces->m_dwState = 2;
//		else
//			m_pBMtl2Faces->m_dwState = 1;
//
//		if (pMesh->m_lpMaterial[index].m_dOption&MATERIAL_OPTION_LIGHTING)
//			m_pBMtlLighting->m_dwState = 2;
//		else
//			m_pBMtlLighting->m_dwState = 0;
//
//		if (pMesh->m_lpMaterial[index].m_dOption&MATERIAL_OPTION_SPECULARENABLE)
//			m_pBMtlSpecular->m_dwState = 2;
//		else
//			m_pBMtlSpecular->m_dwState = 0;
//
//		SAFE_RENDER(m_pBMtlWire);
//		SAFE_RENDER(m_pBMtlZEnable);
//		SAFE_RENDER(m_pBMtlGouraud);
//		SAFE_RENDER(m_pBMtl2Faces);
//		SAFE_RENDER(m_pBMtlLighting);
//		SAFE_RENDER(m_pBMtlSpecular);
//
//		TCHAR str[256];
//		if(m_bUpdateAlphaEdit)
//		{
//			sprintf(str,"%2.4f",pMesh->m_lpMaterial[index].m_sMaterial9.Diffuse.a);
//			m_lpAlphaEditCtrl->SetText(str);
//			m_bUpdateAlphaEdit = FALSE;
//		}
//		if(m_bUpdatePowerEdit)
//		{
//			sprintf(str,"%2.4f",pMesh->m_lpMaterial[index].m_sMaterial9.Power);
//			m_lpPowerEditCtrl->SetText(str);
//			m_bUpdatePowerEdit = FALSE;
//		}
//
//
//		KModel::LPMATERIAL pMtl = &pMesh->m_lpMaterial[index];
//
//		for(int k=0;k<8;k++)
//		{
//			if (pMtl->m_dTextureIDs[k]==0)
//				break;
//
//			TCHAR path_buffer[256];
//			TCHAR drive[256];
//			TCHAR dir[256];
//			TCHAR fname[256];
//			TCHAR ext[256];
//			TCHAR str[256];
//			wsprintf(path_buffer,"%s",pMtl->m_scTextureName[k].c_str());
//			_splitpath( path_buffer, drive, dir, fname, ext );
//
//			wsprintf(str,"%s%s",fname,ext);
//			m_lpTexturesItem[k]->SetText(str);
//			m_lpTexturesItem[k]->m_dwStyle = ITEMSTYLE_TEXTIMAGE;
//
//			m_lpTexturesItem[k]->m_lpMaterial[0].m_dTextureIDs[0] =
//				pMtl->m_dTextureIDs[k];
//
//			SAFE_RENDER(m_lpTexturesItem[k]);
//			SAFE_RENDER(m_BTextureOption[k]);
//		}
//
//		if (m_BMtlOption->m_dwState==2)
//		{
//			MaterialOptionRender(pMtl);
//		}
//
//		m_bShowTextureOption = FALSE;
//		DWORD CurTexture = 0;
//		for(k=0;k<8;k++)
//		{
//			if (m_BTextureOption[k]->m_dwState==2)
//			{
//				m_bShowTextureOption = TRUE;
//				CurTexture = k;
//				break;
//			}
//		}
//		if (m_bShowTextureOption)
//		{
//			RenderTextureOption(pMtl,CurTexture);
//		}
//	}
//	else
//	{
//		D3DCOLORVALUE Color;
//		Color.r = 0.5f;
//		Color.g = 0.5f;
//		Color.b = 0.5f;
//		Color.a = 0.5f;
//
//		SAFE_RENDER(m_pIAmbient);
//		SAFE_RENDER(m_pIDiffuse);
//		SAFE_RENDER(m_pISpecular);
//		SAFE_RENDER(m_pIEmissive);
//
//		SAFE_RENDER(m_pBMtlWire);
//		SAFE_RENDER(m_pBMtlZEnable);
//		SAFE_RENDER(m_pBMtlGouraud);
//		SAFE_RENDER(m_pBMtl2Faces);
//		SAFE_RENDER(m_pBMtlLighting);
//		SAFE_RENDER(m_pBMtlSpecular);
//
//		m_lpPowerEditCtrl->SetText("0.0");
//		m_lpAlphaEditCtrl->SetText("1.0");
//
//	}
//
//	SAFE_RENDER(m_BMtlOption);
//
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolMaterial::SetMeshEditScene(PVOID pScene)
//{
//	m_lpScene = pScene;
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolMaterial::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//
//	if (message==WM_KEYDOWN)
//	{
//		if (wParam==VK_F3)
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
//
//	m_lpMaterialListCtrl->ProcessMessage(message,wParam,lParam);
//
//
//	DWORD MSG = 0;
//
//	if (!pMeshScene)
//		return S_OK;
//
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//
//	if (!pMesh)
//		return S_OK;
//
//	BOOL bRefreshOption = FALSE;
//
//	if (EDIT_RETURN == m_lpPowerEditCtrl->ProcessMessage(message,wParam,lParam))
//	{
//		for(DWORD i=0;i<m_dwNumMaterial;i++)
//		{
//			if (m_lpMaterialListCtrl->m_ListItems[i].dwState)
//			{
//				float Power = 0;
//				sscanf(m_lpPowerEditCtrl->m_szString,"%f",&Power);
//				pMesh->m_lpMaterial[i].m_sMaterial9.Power = Power;
//			}
//		}
//		m_bUpdatePowerEdit = TRUE;
//	}
//
//	if(EDIT_RETURN == m_lpAlphaEditCtrl->ProcessMessage(message,wParam,lParam))
//	{
//		for(DWORD i=0;i<m_dwNumMaterial;i++)
//		{
//			if (m_lpMaterialListCtrl->m_ListItems[i].dwState)
//			{
//				float Alpha = 0;
//				sscanf(m_lpAlphaEditCtrl->m_szString,"%f",&Alpha);
//				pMesh->m_lpMaterial[i].m_sMaterial9.Diffuse.a = Alpha;
//			}
//		}
//		m_bUpdateAlphaEdit = TRUE;
//	}
//
//	if (ITEM_CLICK == m_pBAll->ProcessMessage(message,wParam,lParam))
//	{
//		m_lpMaterialListCtrl->SelectAll();
//		RefreshMaterialList(1);
//		m_pBAll->m_dwState = 0;
//	}
//	if (ITEM_CLICK == m_pBNone->ProcessMessage(message,wParam,lParam))
//	{
//		m_lpMaterialListCtrl->SelectNone();
//		RefreshMaterialList(2);
//		m_pBNone->m_dwState = 0;
//	}
//	if (ITEM_CLICK == m_pBInvert->ProcessMessage(message,wParam,lParam))
//	{
//		m_lpMaterialListCtrl->SelectInvert();
//		RefreshMaterialList(3);
//		m_pBInvert->m_dwState = 0;
//	}
//
//	if (ITEM_CLICK == m_pBSave->ProcessMessage(message,wParam,lParam))
//	{
//		SaveMaterial2File();
//		m_pBSave->m_dwState = 0;
//	}
//
//	if (ITEM_CLICK == m_pBLoad->ProcessMessage(message,wParam,lParam))
//	{
//		LoadMaterial2File();
//		m_pBLoad->m_dwState = 0;
//	}
//
//
//	if (ITEM_CLICK == m_pIAmbient->ProcessMessage(message,wParam,lParam))
//	{
//		m_pIAmbient->m_bSelected  = TRUE;
//		m_pIDiffuse->m_bSelected  = FALSE;
//		m_pISpecular->m_bSelected = FALSE;
//		m_pIEmissive->m_bSelected = FALSE;
//		pMeshScene->m_lpColorPick->m_bHide=FALSE;
//		m_dwCurColorOperation = COLOR_AMBIENT;
//	}
//	if (ITEM_CLICK == m_pIDiffuse->ProcessMessage(message,wParam,lParam))
//	{
//		m_pIAmbient->m_bSelected  = FALSE;
//		m_pIDiffuse->m_bSelected  = TRUE;
//		m_pISpecular->m_bSelected = FALSE;
//		m_pIEmissive->m_bSelected = FALSE;
//		pMeshScene->m_lpColorPick->m_bHide=FALSE;
//		m_dwCurColorOperation = COLOR_DIFFUSE;
//	}
//	if (ITEM_CLICK == m_pISpecular->ProcessMessage(message,wParam,lParam))
//	{
//		m_pIAmbient->m_bSelected  = FALSE;
//		m_pIDiffuse->m_bSelected  = FALSE;
//		m_pISpecular->m_bSelected = TRUE;
//		m_pIEmissive->m_bSelected = FALSE;
//		pMeshScene->m_lpColorPick->m_bHide=FALSE;
//		m_dwCurColorOperation = COLOR_SPECULAR;
//	}
//	if (ITEM_CLICK == m_pIEmissive->ProcessMessage(message,wParam,lParam))
//	{
//		m_pIAmbient->m_bSelected  = FALSE;
//		m_pIDiffuse->m_bSelected  = FALSE;
//		m_pISpecular->m_bSelected = FALSE;
//		m_pIEmissive->m_bSelected = TRUE;
//		pMeshScene->m_lpColorPick->m_bHide=FALSE;
//		m_dwCurColorOperation = COLOR_EMISSIVE;
//	}
//
//	DWORD LastState = m_pBMtlWire->m_dwState;
//	m_pBMtlWire->ProcessMessage(message,wParam,lParam);
//	if (LastState!=m_pBMtlWire->m_dwState)
//	{
//		bRefreshOption = TRUE;
//	}
//
//	LastState = m_pBMtlZEnable->m_dwState;
//	m_pBMtlZEnable->ProcessMessage(message,wParam,lParam);
//	if (LastState!=m_pBMtlZEnable->m_dwState)
//	{
//		bRefreshOption = TRUE;
//	}
//
//	LastState = m_pBMtlGouraud->m_dwState;
//	m_pBMtlGouraud->ProcessMessage(message,wParam,lParam);
//	if (LastState!=m_pBMtlGouraud->m_dwState)
//	{
//		bRefreshOption = TRUE;
//	}
//
//	LastState = m_pBMtlLighting->m_dwState;
//	m_pBMtlLighting->ProcessMessage(message,wParam,lParam);
//	if (LastState!=m_pBMtlLighting->m_dwState)
//	{
//		bRefreshOption = TRUE;
//	}
//
//
//	LastState = m_pBMtlSpecular->m_dwState;
//	m_pBMtlSpecular->ProcessMessage(message,wParam,lParam);
//	if (LastState!=m_pBMtlSpecular->m_dwState)
//	{
//		bRefreshOption = TRUE;
//	}
//
//	LastState = m_pBMtl2Faces->m_dwState;
//	m_pBMtl2Faces->ProcessMessage(message,wParam,lParam);
//	if (LastState!=m_pBMtl2Faces->m_dwState)
//	{
//		bRefreshOption = TRUE;
//	}
//
//	if (bRefreshOption)
//	{
//		for(DWORD i=0;i<m_dwNumMaterial;i++)
//		{
//			if (m_lpMaterialListCtrl->m_ListItems[i].dwState)
//			{
//				//////////////////////////////////////////////////////////////////////////
//				if (m_pBMtlWire->m_dwState == 2)
//				{
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_FILL_SOLID);
//					pMesh->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_FILL_WIREFRAME;
//				}
//				else
//				{
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_FILL_WIREFRAME);
//					pMesh->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_FILL_SOLID;
//				}
//
//				if (m_pBMtlZEnable->m_dwState == 2)
//				{
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_ZBUFFER_FALSE);
//					pMesh->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_TRUE;
//				}
//				else
//				{
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_ZBUFFER_TRUE);
//					pMesh->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
//				}
//				if (m_pBMtlGouraud->m_dwState == 2)
//				{
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_SHADE_FLAT);
//					pMesh->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_SHADE_GOURAUD;
//				}
//				else
//				{
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_SHADE_GOURAUD);
//					pMesh->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_SHADE_FLAT;
//				}
//
//				if (m_pBMtl2Faces->m_dwState == 2)
//				{
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_CULL_CCW);
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_CULL_CW);
//					pMesh->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_CULL_NONE;
//				}
//				else
//				{
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_CULL_NONE);
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_CULL_CCW);
//					pMesh->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_CULL_CW;
//				}
//
//
//				if (m_pBMtlLighting->m_dwState == 2)
//					pMesh->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_LIGHTING;
//				else
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_LIGHTING);
//
//				if (m_pBMtlSpecular->m_dwState == 2)
//					pMesh->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_SPECULARENABLE;
//				else
//					pMesh->m_lpMaterial[i].m_dOption &= (~MATERIAL_OPTION_SPECULARENABLE);
//
//			}
//		}
//	}
//
//	m_BMtlOption->ProcessMessage(message,wParam,lParam);
//
//	//////////////////////////////////////////////////////////////////////////
//
//	int index = GetFirstSelectedMaterial();
//	if (index>=0)
//	{
//		KModel::LPMATERIAL pMtl = &pMesh->m_lpMaterial[index];
//
//		if (m_BMtlOption->m_dwState==2)
//		{
//			MaterialOptionMsg(pMtl,message,wParam,lParam);
//		}
//	}
//
//	TextureOptionMsg(message,wParam,lParam);
//
//	return 	KModelUIBase::ProcessMessage(message,wParam,lParam);
//}
//
//HRESULT KModelToolMaterial::RenderMaterialList(void)
//{
//	return S_OK;
//	/*
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return S_OK;
//
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return S_OK;
//
//	switch(m_dwListStyle)
//	{
//	case 0:
//		{
//			for(DWORD i=0;i<12;i++)
//			{
//				DWORD MaterialIndex = i+m_dwMaterialStart;
//				if (MaterialIndex>=pMesh->m_dNumMaterial)
//					break;
//				g_cGraphicsTool.DrawCommand(&m_CmdMaterials[i],pMesh,MaterialIndex);
//
//				if (m_CmdMaterials[i].State==2)
//				{
//					pMesh->m_lpMaterial[MaterialIndex].m_dOption &= (~MATERIAL_OPTION_HIDE);
//					pMesh->m_lpMaterial[MaterialIndex].m_dOption |= MATERIAL_OPTION_FILL_SOLID;
//				}
//				else if (m_CmdMaterials[i].State==1)
//				{
//					pMesh->m_lpMaterial[MaterialIndex].m_dOption &= (~MATERIAL_OPTION_FILL_SOLID);
//				}
//				else
//				{
//					pMesh->m_lpMaterial[MaterialIndex].m_dOption |= MATERIAL_OPTION_FILL_SOLID;;
//				}
//			}
//			break;
//		}
//	case 1:
//		{
//			for(DWORD i=0;i<24;i++)
//			{
//				DWORD MaterialIndex = i+m_dwMaterialStart;
//				if (MaterialIndex>=pMesh->m_dNumMaterial)
//					break;
//				g_cGraphicsTool.DrawCommand(&m_CmdMaterials[i],pMesh,MaterialIndex);
//
//				if (m_CmdMaterials[i].State==2)
//				{
//					pMesh->m_lpMaterial[MaterialIndex].m_dOption &= (~MATERIAL_OPTION_HIDE);
//					pMesh->m_lpMaterial[MaterialIndex].m_dOption |= MATERIAL_OPTION_FILL_SOLID;
//				}
//				else if (m_CmdMaterials[i].State==1)
//				{
//					pMesh->m_lpMaterial[MaterialIndex].m_dOption &= (~MATERIAL_OPTION_HIDE);
//					pMesh->m_lpMaterial[MaterialIndex].m_dOption &= (~MATERIAL_OPTION_FILL_SOLID);
//				}
//				else
//				{
//					pMesh->m_lpMaterial[MaterialIndex].m_dOption |= MATERIAL_OPTION_FILL_SOLID;;
//				}
//			}
//			break;
//		}
//	case 2:
//		break;
//	}
//	return E_NOTIMPL;*/
//}
//
//
//HRESULT KModelToolMaterial::RefreshMaterialList(DWORD Oper)
//{
///*	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//	{
//		DWORD i=0;
//		float Left = 0.805f;
//		float Top  = 0.007f;
//		float Sep_X = 0.06f;
//		float Sep_Y = 0.10f;
//		float Width = 0.055f;
//		float Height = 0.09f;
//
//		for(DWORD Y=0;Y<4;Y++)
//		{
//			for(DWORD X=0;X<3;X++)
//			{
//				switch(Oper)
//				{
//				case 0:
//				case 1:
//					m_CmdMaterials[i].Create(Left+Sep_X*X
//						,Y*Sep_Y+Top,Width,Height,0,i+1,"",2);
//					break;
//				case 2:
//					m_CmdMaterials[i].Create(Left+Sep_X*X
//						,Y*Sep_Y+Top,Width,Height,0,i+1,"",0);
//					break;
//				case 3:
//					if (m_CmdMaterials[i].State==2)
//						m_CmdMaterials[i].Create(Left+Sep_X*X
//						,Y*Sep_Y+Top,Width,Height,0,i+1,"",0);
//					else
//						m_CmdMaterials[i].Create(Left+Sep_X*X
//						,Y*Sep_Y+Top,Width,Height,0,i+1,"",2);
//					break;
//				}
//				i++;
//			}
//
//		}
//		return S_OK;
//	}
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return S_OK;
//
//	for(DWORD i_Mtl=0;i_Mtl<m_dwNumMaterial;i_Mtl++)
//	{
//		switch(Oper)
//		{
//		case 1:
//			m_pMaterialSelect[i_Mtl] = 1;
//			break;
//		case 2:
//			m_pMaterialSelect[i_Mtl] = 0;
//			break;
//		case 3:
//			if (m_pMaterialSelect[i_Mtl])
//				m_pMaterialSelect[i_Mtl] = 0;
//			else
//				m_pMaterialSelect[i_Mtl] = 1;
//			break;
//		}
//	}
//
//	if ((m_dwListStyle==0)&&(m_lpScene))
//	{
//
//		DWORD i=0;
//		float Left = 0.805f;
//		float Top  = 0.007f;
//		float Sep_X = 0.06f;
//		float Sep_Y = 0.10f;
//		float Width = 0.055f;
//		float Height = 0.09f;
//
//		for(DWORD Y=0;Y<4;Y++)
//		{
//			for(DWORD X=0;X<3;X++)
//			{
//				DWORD Index = i+m_dwMaterialStart;
//
//				if (m_pMaterialSelect[Index])
//					m_CmdMaterials[i].Create(Left+Sep_X*X
//					,Y*Sep_Y+Top,Width,Height,0,i+1,"",2);
//				else
//					m_CmdMaterials[i].Create(Left+Sep_X*X
//					,Y*Sep_Y+Top,Width,Height,0,i+1,"",0);
//
//				i++;
//			}
//		}
//	}
//	else
//	{
//		DWORD i=0;
//		for(DWORD Y=0;Y<6;Y++)
//		{
//			for(DWORD X=0;X<4;X++)
//			{
//				DWORD Index = i+m_dwMaterialStart;
//
//				if (m_pMaterialSelect[Index])
//					m_CmdMaterials[i].Create(0.802f+X*0.045f,Y*0.07f+0.002f,
//					0.043f,0.065f,0,Y+1,"",2);
//				else
//					m_CmdMaterials[i].Create(0.802f+X*0.045f,Y*0.07f+0.002f,
//					0.043f,0.065f,0,Y+1,"",0);
//				i++;
//			}
//		}
//	}
//
//	//pMesh->SaveMaterial(pMesh->m_szMtlFileName);*/
//	return E_NOTIMPL;
//}
//
//
//HRESULT KModelToolMaterial::SetMaterialColor(D3DCOLORVALUE Color)
//{
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	if (!pMeshScene)
//		return S_OK;
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return S_OK;
//
//	for(DWORD i=0;i<m_dwNumMaterial;i++)
//	{
//		if (m_lpMaterialListCtrl->m_ListItems[i].dwState)
//		{
//			if (m_dwCurColorOperation&COLOR_AMBIENT)
//				pMesh->m_lpMaterial[i].m_sMaterial9.Ambient  = Color;
//			if (m_dwCurColorOperation&COLOR_DIFFUSE)
//				pMesh->m_lpMaterial[i].m_sMaterial9.Diffuse  = Color;
//			if (m_dwCurColorOperation&COLOR_SPECULAR)
//				pMesh->m_lpMaterial[i].m_sMaterial9.Specular = Color;
//			if (m_dwCurColorOperation&COLOR_EMISSIVE)
//				pMesh->m_lpMaterial[i].m_sMaterial9.Emissive = Color;
//		}
//	}
//	return E_NOTIMPL;
//}
//
//int KModelToolMaterial::GetFirstSelectedMaterial(void)
//{
//	for(DWORD i=0;i<m_dwNumMaterial;i++)
//	{
//		if (m_lpMaterialListCtrl->m_ListItems[i].dwState)
//			return i;
//	}
//	return -1;
//}
//
//HRESULT KModelToolMaterial::SaveMaterial2File()
//{
//
//	char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="Material Files (*.Mtl)|*.Mtl|";
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
//		wsprintf(filename,"%s%s%s.Mtl",drive,dir,fname);
//
//		KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//		if (!pMeshScene)
//			return E_FAIL;
//
//		LPMODEL pMesh = pMeshScene->GetCurMesh();
//		if (!pMesh)
//			return E_FAIL;
//
//		if (FAILED(pMesh->SaveMaterial(filename)))
//			return E_FAIL;
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::LoadMaterial2File()
//{
//	char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="Material Files (*.Mtl)|*.Mtl|";
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
//		wsprintf(filename,"%s%s%s.Mtl",drive,dir,fname);
//
//		KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//		if (!pMeshScene)
//			return E_FAIL;
//
//		LPMODEL pMesh = pMeshScene->GetCurMesh();
//		if (!pMesh)
//			return E_FAIL;
//
//		if (FAILED(pMesh->LoadMaterial(filename)))
//			return E_FAIL;
//	}
//	return S_OK;
//}
//
//KModel::LPTEXTUREOPEX KModelToolMaterial::TextOpGet(KModel::LPMATERIAL pMaterial,DWORD TextIndex)
//{
//	DWORD NunOption = pMaterial->m_lpTextureOptions[TextIndex].size();
//
//	KModel::LPOPTION pOp = NULL;
//	for(DWORD i=0;i<NunOption;i++)
//	{
//		pOp = pMaterial->m_lpTextureOptions[TextIndex][i];
//		if (pOp->Type == OPTION_TEXTURE_OPERATIONEX)
//		{
//			KModel::LPTEXTUREOPEX pTextureOp = (KModel::LPTEXTUREOPEX) pOp->pData;
//			return pTextureOp;
//		}
//	}
//
//	pOp = new KModel::OPTION;
//	KModel::LPTEXTUREOPEX pTextOp = new KModel::_TextureOpEx;
//	pOp->pData = pTextOp;
//	pOp->Type = OPTION_TEXTURE_OPERATIONEX;
//	pMaterial->m_lpTextureOptions[TextIndex].push_back(pOp);
//	pMaterial->m_lpTextureNumOption[TextIndex]++;
//	return pTextOp;
//}
//
//KModel::LPMTLALPHAOPERATION KModelToolMaterial::AlphaOperationGet(KModel::LPMATERIAL pMaterial)
//{
//	DWORD NunOption = pMaterial->m_dwNumOptions;
//
//	KModel::LPOPTION pOp = NULL;
//	for(DWORD i=0;i<NunOption;i++)
//	{
//		pOp = pMaterial->m_lpMatOptions[i];
//		if (pOp->Type == OPTION_MATERIAL_ALPHAOPERATION)
//		{
//			KModel::LPMTLALPHAOPERATION pAlpha = (KModel::LPMTLALPHAOPERATION) pOp->pData;
//			return pAlpha;
//		}
//	}
//
//	pOp = new KModel::OPTION;
//	KModel::LPMTLALPHAOPERATION pAlpha = new KModel::MTLALPHAOPERATION;
//	pOp->pData = pAlpha;
//	pOp->Type = OPTION_MATERIAL_ALPHAOPERATION;
//	pMaterial->m_lpMatOptions.push_back(pOp);
//	pMaterial->m_dwNumOptions++;
//	return pAlpha;
//}
//
//HRESULT KModelToolMaterial::LoadTextureFile(KModel::LPMATERIAL pMaterial,DWORD TextIndex)
//{
//	char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="Texture Files|*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS|";
//
//	CFileDialog dlg(true, NULL, NULL,
//      OFN_HIDEREADONLY , szFilter);
//
//	dlg.m_ofn.lpstrFile = bigBuff;
//	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
//	if (	dlg.DoModal() == IDOK)
//	{
//		CString Name;
//		Name = dlg.GetPathName();
//
//		LPTEXTURE pTexture = NULL;
//		DWORD NewTextureID = 0;
//		TCHAR str[256];
//		wsprintf(str,"%s",Name);
//		if (SUCCEEDED(g_cTextureTable.LoadFromFile(&NewTextureID,str)))
//		{
//			pMaterial->m_dTextureIDs[TextIndex] = NewTextureID;
//			pMaterial->m_scTextureName[TextIndex] = str;
//		}
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::AddTexture(KModel::LPMATERIAL pMaterial)
//{
//	char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="Texture Files|*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS|";
//
//	CFileDialog dlg(true, NULL, NULL,
//      OFN_HIDEREADONLY , szFilter);
//
//	dlg.m_ofn.lpstrFile = bigBuff;
//	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
//	if (	dlg.DoModal() == IDOK)
//	{
//		CString Name;
//		Name = dlg.GetPathName();
//
//		LPTEXTURE pTexture = NULL;
//		DWORD NewTextureID = 0;
//		TCHAR str[256];
//		wsprintf(str,"%s",Name);
//		if (SUCCEEDED(g_cTextureTable.LoadFromFile(&NewTextureID,str)))
//		{
//			for(int i=0;i<8;i++)
//			{
//				if (pMaterial->m_dTextureIDs[i]==0)
//				{
//					pMaterial->m_dTextureIDs[i] = NewTextureID;
//					pMaterial->m_scTextureName[i] = str;
//					break;
//				}
//			}
//		}
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::RemoveTexture(KModel::LPMATERIAL pMaterial,DWORD TextIndex)
//{
//	for(int k = 0;k<(int)pMaterial->m_lpTextureOptions[TextIndex].size();k++)
//	{
//		KModel::LPOPTION pOption = pMaterial->m_lpTextureOptions[TextIndex][k];
//		SAFE_DELETE(pOption->pData);
//		delete pOption;
//	}
//	pMaterial->m_lpTextureOptions[TextIndex].clear();
//	for(int i = TextIndex ; i<8-1;i++)
//	{
//		pMaterial->m_lpTextureOptions[TextIndex] =
//			pMaterial->m_lpTextureOptions[TextIndex+1];
//		pMaterial->m_lpTextureNumOption[TextIndex] =
//			pMaterial->m_lpTextureNumOption[TextIndex+1];
//		pMaterial->m_dTextureIDs[TextIndex] =
//			pMaterial->m_dTextureIDs[TextIndex+1];
//	}
//	pMaterial->m_dTextureIDs[7] = 0;
//	pMaterial->m_lpTextureNumOption[7] = 0;
//	pMaterial->m_lpTextureOptions[7].clear();
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::ClearTextureItemSelection()
//{
//	for(int k=0;k<8;k++)
//	{
//		m_lpTexturesItem[k]->m_bSelected = FALSE;
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::RenderTextureOption(KModel::LPMATERIAL pMaterial,DWORD TextIndex)
//{
//	SAFE_RENDER(m_lpTextOptionWnd);
//
//	SAFE_RENDER(m_pITextOp);
//	SAFE_RENDER(m_pITextUV);
//
//	switch(m_NowShowTextOp)
//	{
//	case TEXTURE_OPERATION:
//		{
//			KModel::LPTEXTUREOPEX pTOp = TextOpGet(pMaterial,TextIndex);
//			if (pTOp)
//			{
//				TextOpRender(pTOp);
//			}
//			break;
//		}
//	case TEXTURE_MAPMETHOD:
//		{
//			KModel::LPTEXTUREMAP pTMap = TextUVGet(pMaterial,TextIndex);
//
//			TextUVRender(pTMap);
//			break;
//		}
//	}
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::TextOpLoad()
//{
//	//////////////////////////////////////////////////////////////////////////
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpTextOptionWnd,MODELTYPE_WINBASE);
//		ModelUI::FRECT Rect(0.60f,0.4f,0.20f,0.44f);
//		m_lpTextOptionWnd->Create(&Rect,"",0,0);
//		m_lpTextOptionWnd->SetTitleCaption("图层选项");
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pITextOp,MODELTYPE_ITEM);
//		ModelUI::FRECT Rect(0.605f,0.425f,0.062f,0.03f);
//		m_pITextOp->Create(&Rect,"",0,0);
//		m_pITextOp->SetText("贴图操作");
//		m_pITextOp->SetParentWnd(m_lpTextOptionWnd);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pITextUV,MODELTYPE_ITEM);
//		ModelUI::FRECT Rect(0.670f,0.425f,0.062f,0.03f);
//		m_pITextUV->Create(&Rect,"",0,0);
//		m_pITextUV->SetText("纹理坐标");
//		m_pITextUV->SetParentWnd(m_lpTextOptionWnd);
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpColorOpComboBox),MODELTYPE_COMBOBOX);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpAlphaOpComboBox),MODELTYPE_COMBOBOX);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpResultComboBox),MODELTYPE_COMBOBOX);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpColorArg1ComboBox),MODELTYPE_COMBOBOX);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpColorArg2ComboBox),MODELTYPE_COMBOBOX);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpAlphaArg1ComboBox),MODELTYPE_COMBOBOX);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpAlphaArg2ComboBox),MODELTYPE_COMBOBOX);
//
//	{
//		ModelUI::FRECT Rect(0.625f,0.47f,0.13f,0.03f);
//		m_lpColorOpComboBox->Create(&Rect,"",0,0);
//		m_lpColorOpComboBox->SetParentWnd(m_lpTextOptionWnd);
//	}
//	{
//		ModelUI::FRECT Rect(0.625f,0.505f,0.13f,0.03f);
//		m_lpColorArg1ComboBox->Create(&Rect,"",0,0);
//		m_lpColorArg1ComboBox->SetParentWnd(m_lpTextOptionWnd);
//	}
//	{
//		ModelUI::FRECT Rect(0.625f,0.540f,0.13f,0.03f);
//		m_lpColorArg2ComboBox->Create(&Rect,"",0,0);
//		m_lpColorArg2ComboBox->SetParentWnd(m_lpTextOptionWnd);
//	}
//
//
//	{
//		ModelUI::FRECT Rect(0.625f,0.620f,0.13f,0.03f);
//		m_lpAlphaOpComboBox->Create(&Rect,"",0,0);
//		m_lpAlphaOpComboBox->SetParentWnd(m_lpTextOptionWnd);
//	}
//	{
//		ModelUI::FRECT Rect(0.625f,0.655f,0.13f,0.03f);
//		m_lpAlphaArg1ComboBox->Create(&Rect,"",0,0);
//		m_lpAlphaArg1ComboBox->SetParentWnd(m_lpTextOptionWnd);
//	}
//	{
//		ModelUI::FRECT Rect(0.625f,0.690f,0.13f,0.03f);
//		m_lpAlphaArg2ComboBox->Create(&Rect,"",0,0);
//		m_lpAlphaArg2ComboBox->SetParentWnd(m_lpTextOptionWnd);
//	}
//	{
//		ModelUI::FRECT Rect(0.625f,0.740f,0.13f,0.03f);
//		m_lpResultComboBox->Create(&Rect,"",0,0);
//		m_lpResultComboBox->SetParentWnd(m_lpTextOptionWnd);
//	}
//
//
//	m_lpColorOpComboBox->AddString("关闭",D3DTOP_DISABLE);
//	m_lpColorOpComboBox->AddString("使用参数1",D3DTOP_SELECTARG1);
//	m_lpColorOpComboBox->AddString("使用参数2",D3DTOP_SELECTARG2);
//	m_lpColorOpComboBox->AddString("乘积",D3DTOP_MODULATE);
//	m_lpColorOpComboBox->AddString("2倍乘积",D3DTOP_MODULATE2X);
//	m_lpColorOpComboBox->AddString("4倍乘积",D3DTOP_MODULATE4X);
//	m_lpColorOpComboBox->AddString("添加",D3DTOP_ADD);
//	m_lpColorOpComboBox->AddString("四舍五入添加",D3DTOP_ADDSIGNED);
//	m_lpColorOpComboBox->AddString("2倍四舍五入添加",D3DTOP_ADDSIGNED2X);
//	m_lpColorOpComboBox->AddString("减去",D3DTOP_SUBTRACT );
//	m_lpColorOpComboBox->AddString("平滑添加",D3DTOP_ADDSMOOTH );
//
//	m_lpColorOpComboBox->AddString("DIFFUSE颜色的APLHA通道混合",D3DTOP_BLENDDIFFUSEALPHA );
//	m_lpColorOpComboBox->AddString("贴图的APLHA通道混合",D3DTOP_BLENDTEXTUREALPHA );
//	m_lpColorOpComboBox->AddString("操作系数的APLHA通道混合",D3DTOP_BLENDFACTORALPHA);
//	m_lpColorOpComboBox->AddString("前一次操作结果的APLHA通道混合",D3DTOP_BLENDCURRENTALPHA);
//	m_lpColorOpComboBox->AddString("前置乘法",D3DTOP_PREMODULATE);
//	m_lpColorOpComboBox->AddString("Arg1.RGB+Arg1.A*Arg2.RGB",D3DTOP_MODULATEALPHA_ADDCOLOR);
//	m_lpColorOpComboBox->AddString("Arg1.RGB*Arg2.RGB+Arg1.A",D3DTOP_MODULATECOLOR_ADDALPHA);
//	m_lpColorOpComboBox->AddString("Arg1.RGB+(1-Arg1.A)*Arg2.RGB",D3DTOP_MODULATEINVALPHA_ADDCOLOR);
//	m_lpColorOpComboBox->AddString("(1-Arg1.RGB)*Arg2.RGB+Arg1.A",D3DTOP_MODULATEINVCOLOR_ADDALPHA);
//	m_lpColorOpComboBox->AddString("凹凸环境贴图",D3DTOP_BUMPENVMAP );
//	m_lpColorOpComboBox->AddString("带亮度的凹凸环境贴图",D3DTOP_BUMPENVMAPLUMINANCE);
//	m_lpColorOpComboBox->AddString("颜色点积",D3DTOP_DOTPRODUCT3);
//	m_lpColorOpComboBox->AddString("相乘并添加",D3DTOP_MULTIPLYADD);
//	m_lpColorOpComboBox->AddString("线性插值",D3DTOP_LERP);
//
//	m_lpColorArg1ComboBox->AddString("常数",D3DTA_CONSTANT);
//	m_lpColorArg1ComboBox->AddString("当前操作结果",D3DTA_CURRENT);
//	m_lpColorArg1ComboBox->AddString("扩散光照颜色",D3DTA_DIFFUSE);
//	m_lpColorArg1ComboBox->AddString("遮盖",D3DTA_SELECTMASK);
//	m_lpColorArg1ComboBox->AddString("高光颜色",D3DTA_SPECULAR);
//	m_lpColorArg1ComboBox->AddString("临时寄存器",D3DTA_TEMP);
//	m_lpColorArg1ComboBox->AddString("贴图",D3DTA_TEXTURE);
//	m_lpColorArg1ComboBox->AddString("贴图操作系数",D3DTA_TFACTOR);
//	m_lpColorArg1ComboBox->AddString("复制ALPHA_常数",D3DTA_CONSTANT|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg1ComboBox->AddString("复制ALPHA_上一次操作结果",D3DTA_CURRENT|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg1ComboBox->AddString("复制ALPHA_扩散光照颜色",D3DTA_DIFFUSE|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg1ComboBox->AddString("复制ALPHA_遮盖",D3DTA_SELECTMASK|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg1ComboBox->AddString("复制ALPHA_高光颜色",D3DTA_SPECULAR|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg1ComboBox->AddString("复制ALPHA_临时寄存器",D3DTA_TEMP|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg1ComboBox->AddString("复制ALPHA_贴图",D3DTA_TEXTURE|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg1ComboBox->AddString("复制ALPHA_贴图操作系数",D3DTA_TFACTOR|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg1ComboBox->AddString("余数_常数",D3DTA_CONSTANT|D3DTA_COMPLEMENT);
//	m_lpColorArg1ComboBox->AddString("余数_上一次操作结果",D3DTA_CURRENT|D3DTA_COMPLEMENT);
//	m_lpColorArg1ComboBox->AddString("余数_扩散光照颜色",D3DTA_DIFFUSE|D3DTA_COMPLEMENT);
//	m_lpColorArg1ComboBox->AddString("余数_遮盖",D3DTA_SELECTMASK|D3DTA_COMPLEMENT);
//	m_lpColorArg1ComboBox->AddString("余数_高光颜色",D3DTA_SPECULAR|D3DTA_COMPLEMENT);
//	m_lpColorArg1ComboBox->AddString("余数_临时寄存器",D3DTA_TEMP|D3DTA_COMPLEMENT);
//	m_lpColorArg1ComboBox->AddString("余数_贴图",D3DTA_TEXTURE|D3DTA_COMPLEMENT);
//	m_lpColorArg1ComboBox->AddString("余数_贴图操作系数",D3DTA_TFACTOR|D3DTA_COMPLEMENT);
//
//	m_lpColorArg2ComboBox->AddString("常数",D3DTA_CONSTANT);
//	m_lpColorArg2ComboBox->AddString("当前操作结果",D3DTA_CURRENT);
//	m_lpColorArg2ComboBox->AddString("扩散光照颜色",D3DTA_DIFFUSE);
//	m_lpColorArg2ComboBox->AddString("遮盖",D3DTA_SELECTMASK);
//	m_lpColorArg2ComboBox->AddString("高光颜色",D3DTA_SPECULAR);
//	m_lpColorArg2ComboBox->AddString("临时寄存器",D3DTA_TEMP);
//	m_lpColorArg2ComboBox->AddString("贴图",D3DTA_TEXTURE);
//	m_lpColorArg2ComboBox->AddString("贴图操作系数",D3DTA_TFACTOR);
//	m_lpColorArg2ComboBox->AddString("复制ALPHA_常数",D3DTA_CONSTANT|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg2ComboBox->AddString("复制ALPHA_上一次操作结果",D3DTA_CURRENT|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg2ComboBox->AddString("复制ALPHA_扩散光照颜色",D3DTA_DIFFUSE|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg2ComboBox->AddString("复制ALPHA_遮盖",D3DTA_SELECTMASK|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg2ComboBox->AddString("复制ALPHA_高光颜色",D3DTA_SPECULAR|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg2ComboBox->AddString("复制ALPHA_临时寄存器",D3DTA_TEMP|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg2ComboBox->AddString("复制ALPHA_贴图",D3DTA_TEXTURE|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg2ComboBox->AddString("复制ALPHA_贴图操作系数",D3DTA_TFACTOR|D3DTA_ALPHAREPLICATE);
//	m_lpColorArg2ComboBox->AddString("余数_常数",D3DTA_CONSTANT|D3DTA_COMPLEMENT);
//	m_lpColorArg2ComboBox->AddString("余数_上一次操作结果",D3DTA_CURRENT|D3DTA_COMPLEMENT);
//	m_lpColorArg2ComboBox->AddString("余数_扩散光照颜色",D3DTA_DIFFUSE|D3DTA_COMPLEMENT);
//	m_lpColorArg2ComboBox->AddString("余数_遮盖",D3DTA_SELECTMASK|D3DTA_COMPLEMENT);
//	m_lpColorArg2ComboBox->AddString("余数_高光颜色",D3DTA_SPECULAR|D3DTA_COMPLEMENT);
//	m_lpColorArg2ComboBox->AddString("余数_临时寄存器",D3DTA_TEMP|D3DTA_COMPLEMENT);
//	m_lpColorArg2ComboBox->AddString("余数_贴图",D3DTA_TEXTURE|D3DTA_COMPLEMENT);
//	m_lpColorArg2ComboBox->AddString("余数_贴图操作系数",D3DTA_TFACTOR|D3DTA_COMPLEMENT);
//
//	m_lpResultComboBox->AddString("当前操作结果",D3DTA_CURRENT);
//	m_lpResultComboBox->AddString("临时寄存器",D3DTA_TEMP);
//
//
//	m_lpAlphaOpComboBox->AddString("关闭",D3DTOP_DISABLE);
//	m_lpAlphaOpComboBox->AddString("使用参数1",D3DTOP_SELECTARG1);
//	m_lpAlphaOpComboBox->AddString("使用参数2",D3DTOP_SELECTARG2);
//	m_lpAlphaOpComboBox->AddString("乘积",D3DTOP_MODULATE);
//	m_lpAlphaOpComboBox->AddString("2倍乘积",D3DTOP_MODULATE2X);
//	m_lpAlphaOpComboBox->AddString("4倍乘积",D3DTOP_MODULATE4X);
//	m_lpAlphaOpComboBox->AddString("添加",D3DTOP_ADD);
//	m_lpAlphaOpComboBox->AddString("四舍五入添加",D3DTOP_ADDSIGNED);
//	m_lpAlphaOpComboBox->AddString("2倍四舍五入添加",D3DTOP_ADDSIGNED2X);
//	m_lpAlphaOpComboBox->AddString("减去",D3DTOP_SUBTRACT );
//	m_lpAlphaOpComboBox->AddString("平滑添加",D3DTOP_ADDSMOOTH );
//
//	m_lpAlphaOpComboBox->AddString("DIFFUSE颜色的APLHA通道混合",D3DTOP_BLENDDIFFUSEALPHA );
//	m_lpAlphaOpComboBox->AddString("贴图的APLHA通道混合",D3DTOP_BLENDTEXTUREALPHA );
//	m_lpAlphaOpComboBox->AddString("操作系数的APLHA通道混合",D3DTOP_BLENDFACTORALPHA);
//	m_lpAlphaOpComboBox->AddString("前一次操作结果的APLHA通道混合",D3DTOP_BLENDCURRENTALPHA);
//	m_lpAlphaOpComboBox->AddString("前置乘法",D3DTOP_PREMODULATE);
//	m_lpAlphaOpComboBox->AddString("Arg1.RGB+Arg1.A*Arg2.RGB",D3DTOP_MODULATEALPHA_ADDCOLOR);
//	m_lpAlphaOpComboBox->AddString("Arg1.RGB*Arg2.RGB+Arg1.A",D3DTOP_MODULATECOLOR_ADDALPHA);
//	m_lpAlphaOpComboBox->AddString("Arg1.RGB+(1-Arg1.A)*Arg2.RGB",D3DTOP_MODULATEINVALPHA_ADDCOLOR);
//	m_lpAlphaOpComboBox->AddString("(1-Arg1.RGB)*Arg2.RGB+Arg1.A",D3DTOP_MODULATEINVCOLOR_ADDALPHA);
//	m_lpAlphaOpComboBox->AddString("凹凸环境贴图",D3DTOP_BUMPENVMAP );
//	m_lpAlphaOpComboBox->AddString("带亮度的凹凸环境贴图",D3DTOP_BUMPENVMAPLUMINANCE);
//	m_lpAlphaOpComboBox->AddString("颜色点积",D3DTOP_DOTPRODUCT3);
//	m_lpAlphaOpComboBox->AddString("相乘并添加",D3DTOP_MULTIPLYADD);
//	m_lpAlphaOpComboBox->AddString("线性插值",D3DTOP_LERP);
//
//	m_lpAlphaArg1ComboBox->AddString("常数",D3DTA_CONSTANT);
//	m_lpAlphaArg1ComboBox->AddString("当前操作结果",D3DTA_CURRENT);
//	m_lpAlphaArg1ComboBox->AddString("扩散光照颜色",D3DTA_DIFFUSE);
//	m_lpAlphaArg1ComboBox->AddString("遮盖",D3DTA_SELECTMASK);
//	m_lpAlphaArg1ComboBox->AddString("高光颜色",D3DTA_SPECULAR);
//	m_lpAlphaArg1ComboBox->AddString("临时寄存器",D3DTA_TEMP);
//	m_lpAlphaArg1ComboBox->AddString("贴图",D3DTA_TEXTURE);
//	m_lpAlphaArg1ComboBox->AddString("贴图操作系数",D3DTA_TFACTOR);
//	m_lpAlphaArg1ComboBox->AddString("复制ALPHA_常数",D3DTA_CONSTANT|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg1ComboBox->AddString("复制ALPHA_上一次操作结果",D3DTA_CURRENT|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg1ComboBox->AddString("复制ALPHA_扩散光照颜色",D3DTA_DIFFUSE|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg1ComboBox->AddString("复制ALPHA_遮盖",D3DTA_SELECTMASK|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg1ComboBox->AddString("复制ALPHA_高光颜色",D3DTA_SPECULAR|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg1ComboBox->AddString("复制ALPHA_临时寄存器",D3DTA_TEMP|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg1ComboBox->AddString("复制ALPHA_贴图",D3DTA_TEXTURE|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg1ComboBox->AddString("复制ALPHA_贴图操作系数",D3DTA_TFACTOR|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg1ComboBox->AddString("余数_常数",D3DTA_CONSTANT|D3DTA_COMPLEMENT);
//	m_lpAlphaArg1ComboBox->AddString("余数_上一次操作结果",D3DTA_CURRENT|D3DTA_COMPLEMENT);
//	m_lpAlphaArg1ComboBox->AddString("余数_扩散光照颜色",D3DTA_DIFFUSE|D3DTA_COMPLEMENT);
//	m_lpAlphaArg1ComboBox->AddString("余数_遮盖",D3DTA_SELECTMASK|D3DTA_COMPLEMENT);
//	m_lpAlphaArg1ComboBox->AddString("余数_高光颜色",D3DTA_SPECULAR|D3DTA_COMPLEMENT);
//	m_lpAlphaArg1ComboBox->AddString("余数_临时寄存器",D3DTA_TEMP|D3DTA_COMPLEMENT);
//	m_lpAlphaArg1ComboBox->AddString("余数_贴图",D3DTA_TEXTURE|D3DTA_COMPLEMENT);
//	m_lpAlphaArg1ComboBox->AddString("余数_贴图操作系数",D3DTA_TFACTOR|D3DTA_COMPLEMENT);
//
//	m_lpAlphaArg2ComboBox->AddString("常数",D3DTA_CONSTANT);
//	m_lpAlphaArg2ComboBox->AddString("当前操作结果",D3DTA_CURRENT);
//	m_lpAlphaArg2ComboBox->AddString("扩散光照颜色",D3DTA_DIFFUSE);
//	m_lpAlphaArg2ComboBox->AddString("遮盖",D3DTA_SELECTMASK);
//	m_lpAlphaArg2ComboBox->AddString("高光颜色",D3DTA_SPECULAR);
//	m_lpAlphaArg2ComboBox->AddString("临时寄存器",D3DTA_TEMP);
//	m_lpAlphaArg2ComboBox->AddString("贴图",D3DTA_TEXTURE);
//	m_lpAlphaArg2ComboBox->AddString("贴图操作系数",D3DTA_TFACTOR);
//	m_lpAlphaArg2ComboBox->AddString("复制ALPHA_常数",D3DTA_CONSTANT|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg2ComboBox->AddString("复制ALPHA_上一次操作结果",D3DTA_CURRENT|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg2ComboBox->AddString("复制ALPHA_扩散光照颜色",D3DTA_DIFFUSE|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg2ComboBox->AddString("复制ALPHA_遮盖",D3DTA_SELECTMASK|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg2ComboBox->AddString("复制ALPHA_高光颜色",D3DTA_SPECULAR|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg2ComboBox->AddString("复制ALPHA_临时寄存器",D3DTA_TEMP|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg2ComboBox->AddString("复制ALPHA_贴图",D3DTA_TEXTURE|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg2ComboBox->AddString("复制ALPHA_贴图操作系数",D3DTA_TFACTOR|D3DTA_ALPHAREPLICATE);
//	m_lpAlphaArg2ComboBox->AddString("余数_常数",D3DTA_CONSTANT|D3DTA_COMPLEMENT);
//	m_lpAlphaArg2ComboBox->AddString("余数_上一次操作结果",D3DTA_CURRENT|D3DTA_COMPLEMENT);
//	m_lpAlphaArg2ComboBox->AddString("余数_扩散光照颜色",D3DTA_DIFFUSE|D3DTA_COMPLEMENT);
//	m_lpAlphaArg2ComboBox->AddString("余数_遮盖",D3DTA_SELECTMASK|D3DTA_COMPLEMENT);
//	m_lpAlphaArg2ComboBox->AddString("余数_高光颜色",D3DTA_SPECULAR|D3DTA_COMPLEMENT);
//	m_lpAlphaArg2ComboBox->AddString("余数_临时寄存器",D3DTA_TEMP|D3DTA_COMPLEMENT);
//	m_lpAlphaArg2ComboBox->AddString("余数_贴图",D3DTA_TEXTURE|D3DTA_COMPLEMENT);
//	m_lpAlphaArg2ComboBox->AddString("余数_贴图操作系数",D3DTA_TFACTOR|D3DTA_COMPLEMENT);
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::TextOpMsg(KModel::LPTEXTUREOPEX pTOp,UINT message, WPARAM wParam, LPARAM lParam)
//{
//	//////////////////////////////////////////////////////////////////////////
//	//同时只允许访问一个COMBOX
//	if (!m_lpDropTextOp)
//	{
//		if (COMBOBOX_DROP == m_lpColorArg1ComboBox->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropTextOp = m_lpColorArg1ComboBox;
//		}
//		else if (COMBOBOX_DROP == m_lpColorArg2ComboBox->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropTextOp = m_lpColorArg2ComboBox;
//		}
//		else if (COMBOBOX_DROP == m_lpColorOpComboBox->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropTextOp = m_lpColorOpComboBox;
//		}
//		else if (COMBOBOX_DROP == m_lpResultComboBox->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropTextOp = m_lpResultComboBox;
//		}
//		//
//		else if (COMBOBOX_DROP == m_lpAlphaArg1ComboBox->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropTextOp = m_lpAlphaArg1ComboBox;
//		}
//		else if (COMBOBOX_DROP == m_lpAlphaArg2ComboBox->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropTextOp = m_lpAlphaArg2ComboBox;
//		}
//		else if (COMBOBOX_DROP == m_lpAlphaOpComboBox->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropTextOp = m_lpAlphaOpComboBox;
//		}
//		if (m_lpDropTextOp)
//		{
//			TextOpUnDropAll();
//			m_lpDropTextOp->m_bDroped = TRUE;
//		}
//	}
//	else
//	{
//		if (COMBOBOX_ITEMSELECT == m_lpDropTextOp->ProcessMessage(message,wParam,lParam))
//		{
//			pTOp->ColorArg1 = m_lpColorArg1ComboBox->GetCuritemValue();
//			pTOp->ColorArg2 = m_lpColorArg2ComboBox->GetCuritemValue();
//			pTOp->ColorOp   = m_lpColorOpComboBox->GetCuritemValue();
//			pTOp->AlphaArg1 = m_lpAlphaArg1ComboBox->GetCuritemValue();
//			pTOp->AlphaArg2 = m_lpAlphaArg2ComboBox->GetCuritemValue();
//			pTOp->AlphaOp   = m_lpAlphaOpComboBox->GetCuritemValue();
//
//			pTOp->Result = m_lpResultComboBox->GetCuritemValue();
//			m_lpDropTextOp = NULL;
//		}
//	}
//
//	return S_OK;
//}
//
//
//HRESULT KModelToolMaterial::TextOpUnDropAll(void)
//{
//	m_lpColorOpComboBox->m_bDroped = FALSE;
//	m_lpColorArg1ComboBox->m_bDroped = FALSE;
//	m_lpColorArg2ComboBox->m_bDroped = FALSE;
//
//	m_lpResultComboBox->m_bDroped = FALSE;
//
//	m_lpAlphaOpComboBox->m_bDroped = FALSE;
//	m_lpAlphaArg1ComboBox->m_bDroped = FALSE;
//	m_lpAlphaArg2ComboBox->m_bDroped = FALSE;
//
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolMaterial::TextOpRender(KModel::LPTEXTUREOPEX pTOp)
//{
//	m_lpColorOpComboBox->  SetCurItembyValue(pTOp->ColorOp);
//	m_lpColorArg1ComboBox->SetCurItembyValue(pTOp->ColorArg1);
//	m_lpColorArg2ComboBox->SetCurItembyValue(pTOp->ColorArg2);
//
//	m_lpAlphaOpComboBox->  SetCurItembyValue(pTOp->AlphaOp);
//	m_lpAlphaArg1ComboBox->SetCurItembyValue(pTOp->AlphaArg1);
//	m_lpAlphaArg2ComboBox->SetCurItembyValue(pTOp->AlphaArg2);
//
//	m_lpResultComboBox->SetCurItembyValue(pTOp->Result);
//
//	SAFE_RENDER(m_lpColorOpComboBox );
//	SAFE_RENDER(m_lpColorArg1ComboBox);
//	SAFE_RENDER(m_lpColorArg2ComboBox);
//	SAFE_RENDER(m_lpResultComboBox);
//
//	SAFE_RENDER(m_lpAlphaOpComboBox );
//	SAFE_RENDER(m_lpAlphaArg1ComboBox);
//	SAFE_RENDER(m_lpAlphaArg2ComboBox);
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolMaterial::TextUVLoad(void)
//{
//	//////////////////////////////////////////////////////////////////////////
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_pLTextUV,MODELTYPE_LISTBOX);
//		ModelUI::FRECT Rect(0.61f,0.5f,0.18f,0.24f);
//		m_pLTextUV->Create(&Rect,"",0,0);
//		m_pLTextUV->SetParentWnd(m_lpTextOptionWnd);
//		m_pLTextUV->AddString("顶点坐标",D3DTSS_TCI_PASSTHRU);
//		m_pLTextUV->AddString("镜面反射",D3DTSS_TCI_CAMERASPACENORMAL);
//		m_pLTextUV->AddString("玻璃透视",D3DTSS_TCI_CAMERASPACEPOSITION);
//		m_pLTextUV->AddString("高强度镜面反射",D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
//		m_pLTextUV->AddString("球型贴图坐标",D3DTSS_TCI_SPHEREMAP);
//
//	}
//
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolMaterial::TextUVRender(KModel::LPTEXTUREMAP pMap)
//{
//	m_pLTextUV->SelectByValue(pMap->TextureMap);
//	SAFE_RENDER(m_pLTextUV);
//	return E_NOTIMPL;
//}
//
//KModel::LPTEXTUREMAP KModelToolMaterial::TextUVGet(KModel::LPMATERIAL pMaterial,DWORD TextIndex)
//{
//	DWORD NunOption = pMaterial->m_lpTextureNumOption[TextIndex];
//
//	KModel::LPOPTION pOp = NULL;
//	for(DWORD i=0;i<NunOption;i++)
//	{
//		pOp = pMaterial->m_lpTextureOptions[TextIndex][i];
//		if (pOp->Type == OPTION_TEXTURE_MAPMETHOD)
//		{
//			KModel::LPTEXTUREMAP pTextureMap = (KModel::LPTEXTUREMAP) pOp->pData;
//			return pTextureMap;
//		}
//	}
//
//	pOp = new KModel::OPTION;
//	KModel::LPTEXTUREMAP pTextMap = new KModel::_TextureMap;
//	pOp->pData = pTextMap;
//	pOp->Type = OPTION_TEXTURE_MAPMETHOD;
//	pMaterial->m_lpTextureOptions[TextIndex].push_back(pOp);
//	pMaterial->m_lpTextureNumOption[TextIndex]++;
//	return pTextMap;
//}
//
//HRESULT KModelToolMaterial::TextureOptionMsg(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//	DWORD MSG = 0;
//
//	if (!pMeshScene)
//		return S_OK;
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	if (!pMesh)
//		return S_OK;
//
//	int index = GetFirstSelectedMaterial();
//	if (index<0)
//		return S_OK;
//
//	KModel::LPMATERIAL pMtl = &pMesh->m_lpMaterial[index];
//
//	if (ITEM_CLICK == m_pBAddTexture->ProcessMessage(message,wParam,lParam))
//	{
//		AddTexture(pMtl);
//		m_pBAddTexture->m_dwState = 0;
//	}
//
//	int CurText = -1 ;
//	for(int k=0;k<8;k++)
//	{
//		if (pMtl->m_dTextureIDs[k]==0)
//			break;
//
//		if (ITEM_CLICK == m_BTextureOption[k]->ProcessMessage(message,wParam,lParam))
//		{
//			DWORD StateSave = m_BTextureOption[k]->m_dwState;
//			for(int s=0;s<8;s++)
//			{
//				m_BTextureOption[s]->m_dwState = 0;
//			}
//			m_BTextureOption[k]->m_dwState = StateSave;
//		}
//
//		MSG = m_lpTexturesItem[k]->ProcessMessage(message,wParam,lParam);
//
//		if (ITEM_CLICK == MSG)
//		{
//			ClearTextureItemSelection();
//			m_lpTexturesItem[k]->m_bSelected = TRUE;
//		}
//		if (ITEM_DBCLICK == MSG)
//		{
//			ClearTextureItemSelection();
//			LoadTextureFile(pMtl,k);
//			m_lpTexturesItem[k]->m_bSelected = TRUE;
//		}
//
//
//		if (m_lpTexturesItem[k]->m_bSelected)
//			CurText = k;
//	}
//
//	if (ITEM_CLICK == m_pBRemoveTexture->ProcessMessage(message,wParam,lParam))
//	{
//		RemoveTexture(pMtl,CurText);
//		m_pBRemoveTexture->m_dwState = 0;
//	}
//
//	m_bShowTextureOption= FALSE;
//
//	for(k=0;k<8;k++)
//	{
//		if (m_BTextureOption[k]->m_dwState==2)
//		{
//			m_bShowTextureOption = TRUE;
//			CurText = k;
//			break;
//		}
//	}
//
//	if(m_nCurTextureOp!=CurText)
//	{
//		float k = 0;
//		for(int i=0;i<8;i++)
//		{
//
//			float Top = k + m_fTexturesTop + m_RelativeRect.Top;
//			float Height = m_fTexturesHeight;
//			ModelUI::FRECT Rect(0.82f,Top + Height*i,
//				0.175f,Height);
//			m_lpTexturesItem[i]->SetParentWnd(NULL);
//			m_lpTexturesItem[i]->Create(&Rect,"",m_lpTexturesItem[i]->m_dwState,0);
//			m_lpTexturesItem[i]->ComputeRect();
//			m_lpTexturesItem[i]->SetParentWnd(this);
//
//
//			ModelUI::FRECT TRect(0.805f,Top+Height*i,0.017f,Height);
//			m_BTextureOption[i]->SetParentWnd(NULL);
//			m_BTextureOption[i]->Create(&TRect,"",m_BTextureOption[i]->m_dwState,0);
//			m_BTextureOption[i]->ComputeRect();
//			m_BTextureOption[i]->SetParentWnd(this);
//
//			if(i==CurText)
//			{
//				float s = Top+Height*(i+1);
//				m_lpTextOptionWnd->SetParentWnd(NULL);
//				ModelUI::FRECT Rect(0.805f,s,0.16f,0.44f);
//				m_lpTextOptionWnd->Create(&Rect,"",0,0);
//				m_lpTextOptionWnd->ComputeRect();
//				m_lpTextOptionWnd->SetParentWnd(this);
//				k = 0.445f ;
//			}
//
//		}
//		m_nCurTextureOp = CurText;
//	}
//
//	if (m_bShowTextureOption)
//	{
//		m_lpTextOptionWnd->ProcessMessage(message,wParam,lParam);
//
//		if (ITEM_CLICK == m_pITextOp->ProcessMessage(message,wParam,lParam))
//		{
//			m_NowShowTextOp = TEXTURE_OPERATION;
//			m_pITextOp->m_bSelected = TRUE;
//			m_pITextUV->m_bSelected = FALSE;
//		}
//		if (ITEM_CLICK == m_pITextUV->ProcessMessage(message,wParam,lParam))
//		{
//			m_NowShowTextOp = TEXTURE_MAPMETHOD;
//			m_pITextOp->m_bSelected = FALSE;
//			m_pITextUV->m_bSelected = TRUE;
//		}
//
//		switch(m_NowShowTextOp)
//		{
//		case TEXTURE_OPERATION:
//			{
//				KModel::LPTEXTUREOPEX pTOp =
//					TextOpGet(pMtl,CurText);
//
//				TextOpMsg(pTOp,message,wParam,lParam);
//				break;
//			}
//		case TEXTURE_MAPMETHOD:
//			{
//				KModel::LPTEXTUREMAP pTMap = TextUVGet(pMtl,CurText);
//				if (LISTBOX_SELECTCHANGE == m_pLTextUV->ProcessMessage(message,wParam,lParam))
//				{
//					TCHAR str[256];
//					m_pLTextUV->GetCurSelectedString(str,&pTMap->TextureMap);
//				}
//				break;
//			}
//		}
//	}
//
//
//	return S_OK;
//
//}
//
//HRESULT KModelToolMaterial::MaterialOptionLoad()
//{
//	//////////////////////////////////////////////////////////////////////////
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpMtlOptionWnd,MODELTYPE_WINBASE);
//		ModelUI::FRECT Rect(0.60f,0.2f,0.18f,0.44f);
//		m_lpMtlOptionWnd->Create(&Rect,"",0,0);
//		m_lpMtlOptionWnd->SetTitleCaption("材质选项");
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpMtlOptionComboBox),MODELTYPE_COMBOBOX);
//	{
//		ModelUI::FRECT Rect(0.61f,0.23f,0.16f,0.03f);
//		m_lpMtlOptionComboBox->Create(&Rect,"",0,0);
//		m_lpMtlOptionComboBox->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	m_lpMtlOptionComboBox->AddString("Alpha操作",OPTION_MATERIAL_ALPHAOPERATION);
//	m_lpMtlOptionComboBox->AddString("Shader",OPTION_MATERIAL_SHADER);
//	m_lpMtlOptionComboBox->AddString("Effect",OPTION_MATERIAL_MSEFFECT);
//	m_lpMtlOptionComboBox->SetCurItembyValue(OPTION_MATERIAL_ALPHAOPERATION);
//	//////////////////////////////////////////////////////////////////////////
//	AlphaOperationLoad();
//	MtlShaderLoad();
//	MtlEffectLoad();
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::MaterialOptionRender(KModel::LPMATERIAL pMaterial)
//{
//	SAFE_RENDER(m_lpMtlOptionWnd);
//	SAFE_RENDER(m_lpMtlOptionComboBox);
//
//	DWORD OptionType = m_lpMtlOptionComboBox->GetCuritemValue();
//
//	switch(OptionType)
//	{
//	case OPTION_MATERIAL_ALPHAOPERATION:
//		{
//			KModel::LPMTLALPHAOPERATION pAlpha = AlphaOperationGet(pMaterial);
//			AlphaOperationRender(pAlpha);
//			break;
//		}
//	case OPTION_MATERIAL_SHADER:
//		{
//			MtlShaderRender();
//			break;
//		}
//	case OPTION_MATERIAL_MSEFFECT:
//		{
//			KModel::LPMTLMSEFFECT pEffect = MtlEffectGet(pMaterial);
//			MtlEffectRender(pEffect);
//			break;
//		}
//	}
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::MaterialOptionMsg(KModel::LPMATERIAL pMaterial,
//											  UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if(COMBOBOX_ITEMSELECT == m_lpMtlOptionComboBox->ProcessMessage(message,wParam,lParam))
//	{
//		;
//	}
//	DWORD OptionType = m_lpMtlOptionComboBox->GetCuritemValue();
//	switch(OptionType)
//	{
//	case OPTION_MATERIAL_ALPHAOPERATION:
//		{
//			KModel::LPMTLALPHAOPERATION pAlpha = AlphaOperationGet(pMaterial);
//			AlphaOperationMsg(pAlpha,message,wParam,lParam);
//			break;
//		}
//	case OPTION_MATERIAL_SHADER:
//		{
//			MtlShaderMsg(message,wParam,lParam);
//			break;
//		}
//	case OPTION_MATERIAL_MSEFFECT:
//		{
//			KModel::LPMTLMSEFFECT pEffect = MtlEffectGet(pMaterial);
//			MtlEffectMsg(pEffect,message,wParam,lParam);
//			break;
//		}
//	}
//
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::AlphaOperationLoad()
//{
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpBAlphaTest,MODELTYPE_BUTTON);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpBAlphaBlend,MODELTYPE_BUTTON);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpBSepAlphaBlend,MODELTYPE_BUTTON);
//
//	{
//		ModelUI::FRECT TRect(0.61f,0.27f,0.04f,0.03f);
//		m_lpBAlphaTest->Create(&TRect,"",0,0);
//		m_lpBAlphaTest->SetIconTexture("Toolbar\\Load.tga");
//		m_lpBAlphaTest->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpBAlphaTest->m_dwState = 2;
//	}
//	{
//		ModelUI::FRECT TRect(0.655f,0.27f,0.04f,0.03f);
//		m_lpBAlphaBlend->Create(&TRect,"",0,0);
//		m_lpBAlphaBlend->SetIconTexture("Toolbar\\Load.tga");
//		m_lpBAlphaBlend->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpBAlphaBlend->m_dwState = 2;
//	}
//	{
//		ModelUI::FRECT TRect(0.70f,0.27f,0.04f,0.03f);
//		m_lpBSepAlphaBlend->Create(&TRect,"",0,0);
//		m_lpBSepAlphaBlend->SetIconTexture("Toolbar\\Load.tga");
//		m_lpBSepAlphaBlend->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpBSepAlphaBlend->m_dwState = 0;
//	}
//
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpEAlphaTestRef,MODELTYPE_EDITCTRL);
//		ModelUI::FRECT Rect1(0.63f,0.31f,0.14f,0.03f);
//		m_lpEAlphaTestRef->Create(&Rect1,"",0,0);
//		m_lpEAlphaTestRef->SetParentWnd(m_lpMtlOptionWnd);
//	}
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpCAlphaFuction),MODELTYPE_COMBOBOX);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpCAlphaBlendOperator),MODELTYPE_COMBOBOX);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpCAlphaBlendFactor),MODELTYPE_EDITCTRL);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpABSrcComboBox),MODELTYPE_COMBOBOX);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpABDesComboBox),MODELTYPE_COMBOBOX);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpCSrcBlendAlpha),MODELTYPE_COMBOBOX);
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpCDestBlendAlpha),MODELTYPE_COMBOBOX);
//
//	float Height = 0.04f;
//	{
//		ModelUI::FRECT Rect(0.63f,0.35f+Height*0,0.14f,0.03f);
//		m_lpCAlphaFuction->Create(&Rect,"",0,0);
//		m_lpCAlphaFuction->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpCAlphaFuction->AddString("NEVER"       ,D3DCMP_NEVER);
//		m_lpCAlphaFuction->AddString("LESS"        ,D3DCMP_LESS);
//		m_lpCAlphaFuction->AddString("EQUAL"       ,D3DCMP_EQUAL);
//		m_lpCAlphaFuction->AddString("LESSEQUAL"   ,D3DCMP_LESSEQUAL);
//		m_lpCAlphaFuction->AddString("GREATER"     ,D3DCMP_GREATER);
//		m_lpCAlphaFuction->AddString("NOTEQUAL"    ,D3DCMP_NOTEQUAL);
//		m_lpCAlphaFuction->AddString("GREATEREQUAL",D3DCMP_GREATEREQUAL);
//		m_lpCAlphaFuction->AddString("ALWAYS"      ,D3DCMP_ALWAYS);
//		m_lpCAlphaFuction->SetCurItembyValue(D3DCMP_GREATEREQUAL);
//	}
//	{
//		ModelUI::FRECT Rect(0.63f,0.35f+Height*1,0.14f,0.03f);
//		m_lpCAlphaBlendOperator->Create(&Rect,"",0,0);
//		m_lpCAlphaBlendOperator->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpCAlphaBlendOperator->AddString("ADD",D3DBLENDOP_ADD);
//		m_lpCAlphaBlendOperator->AddString("SUBTRACT",D3DBLENDOP_SUBTRACT);
//		m_lpCAlphaBlendOperator->AddString("REVSUBTRACT",D3DBLENDOP_REVSUBTRACT);
//		m_lpCAlphaBlendOperator->AddString("MIN",D3DBLENDOP_MIN);
//		m_lpCAlphaBlendOperator->AddString("MAX",D3DBLENDOP_MAX);
//		m_lpCAlphaBlendOperator->SetCurItembyValue(D3DBLENDOP_ADD);
//	}
//	{
//		ModelUI::FRECT Rect(0.63f,0.35f+Height*2,0.14f,0.03f);
//		m_lpCAlphaBlendFactor->Create(&Rect,"",0,0);
//		m_lpCAlphaBlendFactor->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	{
//		ModelUI::FRECT Rect(0.63f,0.35f+Height*3,0.14f,0.03f);
//		m_lpABSrcComboBox->Create(&Rect,"",0,0);
//		m_lpABSrcComboBox->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	{
//		ModelUI::FRECT Rect(0.63f,0.35f+Height*4,0.14f,0.03f);
//		m_lpABDesComboBox->Create(&Rect,"",0,0);
//		m_lpABDesComboBox->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	{
//		ModelUI::FRECT Rect(0.63f,0.35f+Height*5,0.14f,0.03f);
//		m_lpCSrcBlendAlpha->Create(&Rect,"",0,0);
//		m_lpCSrcBlendAlpha->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	{
//		ModelUI::FRECT Rect(0.63f,0.35f+Height*6,0.14f,0.03f);
//		m_lpCDestBlendAlpha->Create(&Rect,"",0,0);
//		m_lpCDestBlendAlpha->SetParentWnd(m_lpMtlOptionWnd);
//	}
//
//
//	m_lpABDesComboBox->AddString("0",D3DBLEND_ZERO);
//	m_lpABDesComboBox->AddString("1",D3DBLEND_ONE);
//	m_lpABDesComboBox->AddString("新图层颜色(Rs，Gs，Bs)",D3DBLEND_SRCCOLOR);
//	m_lpABDesComboBox->AddString("新图层颜色的反色(1-Rs，1-Gs，1-Bs)底片效果",D3DBLEND_INVSRCCOLOR);
//	m_lpABDesComboBox->AddString("新图层ALPHA(As)",D3DBLEND_SRCALPHA);
//	m_lpABDesComboBox->AddString("新图层ALPHA的反色(1-As)",D3DBLEND_INVSRCALPHA);
//	m_lpABDesComboBox->AddString("旧图层ALPHA(Ad)",D3DBLEND_DESTALPHA);
//	m_lpABDesComboBox->AddString("旧图层ALPHA的反色(1-Ad)",D3DBLEND_INVDESTALPHA );
//	m_lpABDesComboBox->AddString("旧图层颜色(Rd，Gd，Bd)",D3DBLEND_DESTCOLOR);
//	m_lpABDesComboBox->AddString("旧图层颜色的反色(1-Rd，1-Gd，1-Bd)底片效果",D3DBLEND_INVDESTCOLOR);
//	m_lpABDesComboBox->AddString("图层SAT ALPHA(f, f, f, 1); f = min(A, 1 - Ad)",D3DBLEND_SRCALPHASAT);
//	m_lpABDesComboBox->AddString("新图层颜色和ALPHA(As)(1-As)",D3DBLEND_BOTHSRCALPHA);
//	m_lpABDesComboBox->AddString("旧图层颜色和ALPHA(Ad)(1-Ad)",D3DBLEND_BOTHINVSRCALPHA );
//	m_lpABDesComboBox->AddString("渲染混合系数(TF)",D3DBLEND_BLENDFACTOR );
//	m_lpABDesComboBox->AddString("渲染混合系数的反色(1-TF)",D3DBLEND_INVBLENDFACTOR);
//
//	m_lpABSrcComboBox->AddString("0",D3DBLEND_ZERO);
//	m_lpABSrcComboBox->AddString("1",D3DBLEND_ONE);
//	m_lpABSrcComboBox->AddString("新图层颜色(Rs，Gs，Bs)",D3DBLEND_SRCCOLOR);
//	m_lpABSrcComboBox->AddString("新图层颜色的反色(1-Rs，1-Gs，1-Bs)底片效果",D3DBLEND_INVSRCCOLOR);
//	m_lpABSrcComboBox->AddString("新图层ALPHA(As)",D3DBLEND_SRCALPHA);
//	m_lpABSrcComboBox->AddString("新图层ALPHA的反色(1-As)",D3DBLEND_INVSRCALPHA);
//	m_lpABSrcComboBox->AddString("旧图层ALPHA(Ad)",D3DBLEND_DESTALPHA);
//	m_lpABSrcComboBox->AddString("旧图层ALPHA的反色(1-Ad)",D3DBLEND_INVDESTALPHA );
//	m_lpABSrcComboBox->AddString("旧图层颜色(Rd，Gd，Bd)",D3DBLEND_DESTCOLOR);
//	m_lpABSrcComboBox->AddString("旧图层颜色的反色(1-Rd，1-Gd，1-Bd)底片效果",D3DBLEND_INVDESTCOLOR);
//	m_lpABSrcComboBox->AddString("图层SAT ALPHA(f, f, f, 1); f = min(A, 1 - Ad)",D3DBLEND_SRCALPHASAT);
//	m_lpABSrcComboBox->AddString("新图层颜色和ALPHA(As)(1-As)",D3DBLEND_BOTHSRCALPHA);
//	m_lpABSrcComboBox->AddString("旧图层颜色和ALPHA(Ad)(1-Ad)",D3DBLEND_BOTHINVSRCALPHA );
//	m_lpABSrcComboBox->AddString("渲染混合系数(TF)",D3DBLEND_BLENDFACTOR );
//	m_lpABSrcComboBox->AddString("渲染混合系数的反色(1-TF)",D3DBLEND_INVBLENDFACTOR);
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::AlphaOperationRender(KModel::LPMTLALPHAOPERATION pAlpha)
//{
//	if(pAlpha->EnableAlphaTest)
//		m_lpBAlphaTest->m_dwState = 2;
//	else
//		m_lpBAlphaTest->m_dwState = 0;
//
//	if(pAlpha->EnableAlphaBlend)
//		m_lpBAlphaBlend->m_dwState = 2;
//	else
//		m_lpBAlphaBlend->m_dwState = 0;
//
//	if(pAlpha->EnableSeparateAlphaBlend)
//		m_lpBSepAlphaBlend->m_dwState = 2;
//	else
//		m_lpBSepAlphaBlend->m_dwState = 0;
//
//	SAFE_RENDER(m_lpBAlphaTest);
//	SAFE_RENDER(m_lpBAlphaBlend);
//	SAFE_RENDER(m_lpBSepAlphaBlend);
//
//	TCHAR str[256];
//	sprintf(str,"%d",pAlpha->AlphaTestRef);
//	m_lpEAlphaTestRef->SetText(str);
//	SAFE_RENDER(m_lpEAlphaTestRef);
//
//	m_lpCAlphaFuction->SetCurItembyValue(pAlpha->AlphaFunction);
//	SAFE_RENDER(m_lpCAlphaFuction);
//
//	m_lpCAlphaBlendOperator->SetCurItembyValue(pAlpha->BackBlendOperator);
//	SAFE_RENDER(m_lpCAlphaBlendOperator);
//
//	SAFE_RENDER(m_lpCAlphaBlendFactor);
//
//	SAFE_RENDER(m_lpCSrcBlendAlpha);
//	SAFE_RENDER(m_lpCDestBlendAlpha);
//
//	m_lpABSrcComboBox->SetCurItembyValue(pAlpha->SrcBlend);
//	m_lpABDesComboBox->SetCurItembyValue(pAlpha->DestBlend);
//	SAFE_RENDER(m_lpABSrcComboBox);
//	SAFE_RENDER(m_lpABDesComboBox);
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::AlphaOperationMsg(KModel::LPMTLALPHAOPERATION pAlpha,
//											  UINT message, WPARAM wParam, LPARAM lParam)
//{
//	m_lpBAlphaTest->ProcessMessage(message,wParam,lParam);
//	m_lpBAlphaBlend->ProcessMessage(message,wParam,lParam);
//	m_lpBSepAlphaBlend->ProcessMessage(message,wParam,lParam);
//
//	if(m_lpBAlphaTest->m_dwState==2)
//		pAlpha->EnableAlphaTest = TRUE;
//	else
//		pAlpha->EnableAlphaTest = FALSE;
//
//	if(m_lpBAlphaBlend->m_dwState==2)
//		pAlpha->EnableAlphaBlend = TRUE;
//	else
//		pAlpha->EnableAlphaBlend = FALSE;
//
//	if(m_lpBSepAlphaBlend->m_dwState==2)
//		pAlpha->EnableSeparateAlphaBlend = TRUE;
//	else
//		pAlpha->EnableSeparateAlphaBlend = FALSE;
//
//	if(EDIT_RETURN == m_lpEAlphaTestRef->ProcessMessage(message,wParam,lParam))
//	{
//		DWORD ref = 0;
//		sscanf(m_lpEAlphaTestRef->m_szString,"%d",&ref);
//		pAlpha->AlphaTestRef = ref;
//	}
//
//	m_lpCAlphaBlendFactor->ProcessMessage(message,wParam,lParam);
//
//	if (!m_lpDropAlphaOperation)
//	{
//		if (COMBOBOX_DROP == m_lpCAlphaFuction->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropAlphaOperation = m_lpCAlphaFuction;
//		}
//		else if (COMBOBOX_DROP == m_lpCAlphaBlendOperator->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropAlphaOperation = m_lpCAlphaBlendOperator;
//		}
//		else if (COMBOBOX_DROP == m_lpCSrcBlendAlpha->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropAlphaOperation = m_lpCSrcBlendAlpha;
//		}
//		else if (COMBOBOX_DROP == m_lpCDestBlendAlpha->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropAlphaOperation = m_lpCDestBlendAlpha;
//		}
//		else if (COMBOBOX_DROP == m_lpABSrcComboBox->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropAlphaOperation = m_lpABSrcComboBox;
//		}
//		else if (COMBOBOX_DROP == m_lpABDesComboBox->ProcessMessage(message,wParam,lParam))
//		{
//			m_lpDropAlphaOperation = m_lpABDesComboBox;
//		}
//		if (m_lpDropAlphaOperation)
//		{
//			AlphaOperationUnDropAll();
//			m_lpDropAlphaOperation->m_bDroped = TRUE;
//		}
//	}
//	else
//	{
//		if (COMBOBOX_ITEMSELECT == m_lpDropAlphaOperation->ProcessMessage(message,wParam,lParam))
//		{
//			pAlpha->AlphaFunction     =          m_lpCAlphaFuction->GetCuritemValue();
//			pAlpha->BackBlendOperator = m_lpCAlphaBlendOperator->GetCuritemValue();
//			pAlpha->SrcBlendAlpha     = m_lpCSrcBlendAlpha->GetCuritemValue();
//			pAlpha->DestBlendAlpha    = m_lpCDestBlendAlpha->GetCuritemValue();
//			pAlpha->SrcBlend          = m_lpABSrcComboBox->GetCuritemValue();
//			pAlpha->DestBlend         = m_lpABDesComboBox->GetCuritemValue();
//
//			m_lpDropAlphaOperation = NULL;
//		}
//
//	}
//
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::AlphaOperationUnDropAll()
//{
//	m_lpCAlphaFuction->m_bDroped = FALSE;
//	m_lpCAlphaBlendOperator->m_bDroped = FALSE;
//	m_lpCSrcBlendAlpha->m_bDroped = FALSE;
//	m_lpCDestBlendAlpha->m_bDroped = FALSE;
//
//	m_lpABSrcComboBox->m_bDroped = FALSE;//Alpha Blend Src
//	m_lpABDesComboBox->m_bDroped = FALSE;//Alpha Blend Dest
//
//	return S_OK;
//}
//
//KModel::LPMTLSHADEROPTION KModelToolMaterial::MtlShaderGet(KModel::LPMATERIAL pMaterial)
//{
//	DWORD NunOption = pMaterial->m_dwNumOptions;
//
//	KModel::LPOPTION pOp = NULL;
//	for(DWORD i=0;i<NunOption;i++)
//	{
//		pOp = pMaterial->m_lpMatOptions[i];
//		if (pOp->Type == OPTION_MATERIAL_SHADER)
//		{
//			KModel::LPMTLSHADEROPTION pShader = (KModel::LPMTLSHADEROPTION) pOp->pData;
//			return pShader;
//		}
//	}
//
//	pOp = new KModel::OPTION;
//	KModel::LPMTLSHADEROPTION pShader = new KModel::MTLSHADEROPTION;
//	pOp->pData = pShader;
//	pOp->Type = OPTION_MATERIAL_SHADER;
//	pMaterial->m_lpMatOptions.push_back(pOp);
//	pMaterial->m_dwNumOptions++;
//	return pShader;
//}
//
//KModel::LPMTLMSEFFECT KModelToolMaterial::MtlEffectGet(KModel::LPMATERIAL pMaterial)
//{
//	if (pMaterial->m_lpEffect)
//	{
//		return pMaterial->m_lpEffect;
//	}
//	else
//	{
//		pMaterial->m_lpEffect = new KModel::MTLMSEFFECT;
//		return pMaterial->m_lpEffect;
//	}
//}
//
//HRESULT KModelToolMaterial::MtlShaderLoad()
//{
//	//////////////////////////////////////////////////////////////////////////
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpLVertexConst),MODELTYPE_LISTBOX);
//	{
//		ModelUI::FRECT Rect(0.61f,0.27f,0.16f,0.09f);
//		m_lpLVertexConst->Create(&Rect,"",0,0);
//		m_lpLVertexConst->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpCVertexInput),MODELTYPE_COMBOBOX);
//	{
//		ModelUI::FRECT Rect(0.61f,0.365f,0.16f,0.03f);
//		m_lpCVertexInput->Create(&Rect,"",0,0);
//		m_lpCVertexInput->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpIVertexFile),MODELTYPE_ITEM);
//	{
//		ModelUI::FRECT Rect(0.61f,0.40f,0.09f,0.03f);
//		m_lpIVertexFile->Create(&Rect,"",0,0);
//		m_lpIVertexFile->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpBVertexEdit),MODELTYPE_BUTTON);
//	{
//		ModelUI::FRECT Rect(0.705f,0.40f,0.03f,0.03f);
//		m_lpBVertexEdit->Create(&Rect,"",0,0);
//		m_lpBVertexEdit->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpBVertexEdit->SetIconTexture("Toolbar\\Load.tga");
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpBVertexRest),MODELTYPE_BUTTON);
//	{
//		ModelUI::FRECT Rect(0.74f,0.40f,0.03f,0.03f);
//		m_lpBVertexRest->Create(&Rect,"",0,0);
//		m_lpBVertexRest->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpBVertexRest->SetIconTexture("Toolbar\\Load.tga");
//	}
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpLPixelConst),MODELTYPE_LISTBOX);
//	{
//		ModelUI::FRECT Rect(0.61f,0.45f,0.16f,0.09f);
//		m_lpLPixelConst->Create(&Rect,"",0,0);
//		m_lpLPixelConst->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpCpixelInput),MODELTYPE_COMBOBOX);
//	{
//		ModelUI::FRECT Rect(0.61f,0.545f,0.16f,0.03f);
//		m_lpCpixelInput->Create(&Rect,"",0,0);
//		m_lpCpixelInput->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpIPixelFile),MODELTYPE_ITEM);
//	{
//		ModelUI::FRECT Rect(0.61f,0.58f,0.09f,0.03f);
//		m_lpIPixelFile->Create(&Rect,"",0,0);
//		m_lpIPixelFile->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpBPixelEdit),MODELTYPE_BUTTON);
//	{
//		ModelUI::FRECT Rect(0.705f,0.58f,0.03f,0.03f);
//		m_lpBPixelEdit->Create(&Rect,"",0,0);
//		m_lpBPixelEdit->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpBPixelEdit->SetIconTexture("Toolbar\\Load.tga");
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpBPixelRest),MODELTYPE_BUTTON);
//	{
//		ModelUI::FRECT Rect(0.74f,0.58f,0.03f,0.03f);
//		m_lpBPixelRest->Create(&Rect,"",0,0);
//		m_lpBPixelRest->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpBPixelRest->SetIconTexture("Toolbar\\Load.tga");
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::MtlShaderRender()
//{
//	SAFE_RENDER(m_lpLVertexConst);
//	SAFE_RENDER(m_lpCVertexInput);
//	SAFE_RENDER(m_lpIVertexFile);
//	SAFE_RENDER(m_lpBVertexEdit);
//	SAFE_RENDER(m_lpBVertexRest);
//
//	SAFE_RENDER(m_lpLPixelConst);
//	SAFE_RENDER(m_lpCpixelInput);
//	SAFE_RENDER(m_lpIPixelFile);
//	SAFE_RENDER(m_lpBPixelEdit);
//	SAFE_RENDER(m_lpBPixelRest);
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::MtlEffectRender(KModel::LPMTLMSEFFECT pEffect)
//{
//	SAFE_RENDER(m_lpLEffectConst);
//	SAFE_RENDER(m_lpCEffectInput);
//	SAFE_RENDER(m_lpIEffectFile);
//	SAFE_RENDER(m_lpBEffectEdit);
//	SAFE_RENDER(m_lpBEffectRest);
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::MtlShaderMsg(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	m_lpBVertexEdit->ProcessMessage(message,wParam,lParam);
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::MtlEffectMsg(KModel::LPMTLMSEFFECT pEffect,UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if (ITEM_CLICK == m_lpBEffectEdit->ProcessMessage(message,wParam,lParam))
//	{
//		MtlEffectOpenFile(pEffect);
//	}
//
//	m_lpLEffectConst->ProcessMessage(message,wParam,lParam);
//
//	if (COMBOBOX_ITEMSELECT == m_lpCEffectInput->ProcessMessage(message,wParam,lParam))
//	{
//		pEffect->m_nCurTechnique = m_lpCEffectInput->GetCuritemValue();
//	}
//
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::MtlEffectOpenFile(KModel::LPMTLMSEFFECT pEffect)
//{
//	HRESULT hr = S_OK;
//
//	char bigBuff[800] = "";  // maximum common dialog buffer size
//	char szFilter[] ="Effect Files|*.fx|";
//
//	CFileDialog dlg(true, NULL, NULL,
//		OFN_HIDEREADONLY , szFilter);
//
//	dlg.m_ofn.lpstrFile = bigBuff;
//	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
//	if (dlg.DoModal() == IDOK)
//	{
//		CString Name;
//		Name = dlg.GetPathName();
//
//		LPTEXTURE pTexture = NULL;
//		DWORD NewTextureID = 0;
//		TCHAR str[256];
//		wsprintf(str,"%s",Name);
//
//	   if (FAILED(hr = g_EffectMgr.GetResource(&pEffect->m_HEffect,str)))
//	   {
//		   return hr;
//	   }
//	   else
//	   {
//		   pEffect->OnLoad();
//
//		   D3DXEFFECT_DESC Desc;
//		   pEffect->m_HEffect->pEffect->GetDesc(&Desc);
//
//		   m_lpCEffectInput->Clear();
//
//		   for(UINT K = 0;K<Desc.Techniques;K++)
//		   {
//			   D3DXHANDLE Handle = pEffect->m_HEffect->pEffect->GetTechnique(K);
//			   if(Handle)
//			   {
//				   D3DXTECHNIQUE_DESC Tec_Desc;
//				   pEffect->m_HEffect->pEffect->GetTechniqueDesc(Handle,&Tec_Desc);
//
//				   TCHAR str[256];
//				   wsprintf(str,"%s",Tec_Desc.Name);
//
//				   m_lpCEffectInput->AddString(str,K);
//
//			   }
//		   }
//
//		   for(UINT K = 0;K<pEffect->InputConstant.size();K++)
//		   {
//			   KModel::SHADERCONSTANT Const = pEffect->InputConstant[K];
//
//			   TCHAR str[256];
//			   wsprintf(str,"%s",Const.scName.c_str());
//
//			   m_lpLEffectConst->AddString(str,K);
//
//		   }
//	   }
//
//
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolMaterial::MtlEffectLoad()
//{
//	//////////////////////////////////////////////////////////////////////////
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpCEffectInput),MODELTYPE_COMBOBOX);
//	{
//		ModelUI::FRECT Rect(0.61f,0.27f,0.16f,0.03f);
//		m_lpCEffectInput->Create(&Rect,"",0,0);
//		m_lpCEffectInput->SetParentWnd(m_lpMtlOptionWnd);
//	}
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpLEffectConst),MODELTYPE_LISTBOX);
//	{
//		ModelUI::FRECT Rect(0.61f,0.31f,0.16f,0.19f);
//		m_lpLEffectConst->Create(&Rect,"",0,0);
//		m_lpLEffectConst->SetParentWnd(m_lpMtlOptionWnd);
//	}
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpIEffectFile),MODELTYPE_ITEM);
//	{
//		ModelUI::FRECT Rect(0.61f,0.52f,0.09f,0.03f);
//		m_lpIEffectFile->Create(&Rect,"",0,0);
//		m_lpIEffectFile->SetParentWnd(m_lpMtlOptionWnd);
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpBEffectEdit),MODELTYPE_BUTTON);
//	{
//		ModelUI::FRECT Rect(0.705f,0.52f,0.03f,0.03f);
//		m_lpBEffectEdit->Create(&Rect,"",0,0);
//		m_lpBEffectEdit->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpBEffectEdit->SetIconTexture("Toolbar\\Load.tga");
//	}
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpBEffectRest),MODELTYPE_BUTTON);
//	{
//		ModelUI::FRECT Rect(0.74f,0.52f,0.03f,0.03f);
//		m_lpBEffectRest->Create(&Rect,"",0,0);
//		m_lpBEffectRest->SetParentWnd(m_lpMtlOptionWnd);
//		m_lpBEffectRest->SetIconTexture("Toolbar\\Load.tga");
//	}
//
//	return S_OK;
//}
};
