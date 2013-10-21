// MeshPropertySet.cpp: implementation of the KModelToolSFX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshPropertySet.h"
#include "./KDlg_PropertyDialog.h"


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

//KModelToolSFX::KModelToolSFX()
//{
//	//m_lpDailog = NULL;
//
//	m_PositionGridA = D3DXVECTOR2(0,0);
//	m_PositionGridB = D3DXVECTOR2(0,0);
//
//	m_pLastPosition = NULL;
//	m_pBSFXNew = NULL;
//	m_lpSFXListBox = NULL;
//
//	m_Tree = NULL;
//
//	m_lpSlider = NULL;
//	m_bHide = TRUE;
//	m_nNumShowItem = 0;
//	m_bShowEdit = FALSE;
//	m_lpValueEdit = NULL;
//	m_nMaxMumItem = 0;
//	m_nItemStart  = 0;
//	m_lpScene = NULL;
//	m_lpSFXBase = NULL;
//}
//
//KModelToolSFX::~KModelToolSFX()
//{
//
//}
//
//HRESULT KModelToolSFX::LoadMesh(LPSTR pFileName)
//{
//	ModelUI::FRECT RECT(0.65f,0.2f,0.175f,0.54f);
//	KModelUIBase::Create(&RECT,"",0,0);
//	KModelUIBase::SetTitleCaption("特效工具条");
//
//	m_Item_Height = 0.03f;
//	m_Item_Sep    = 0.035f;
//
//	m_nMaxMumItem = (int)((0.54f-0.02)/m_Item_Sep);
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpValueEdit),MODELTYPE_EDITCTRL);
//	ModelUI::FRECT Rect2(0.82f,0.59f,0.085f,0.03f);
//	m_lpValueEdit->Create(&Rect2,"",0,0);
//	m_lpValueEdit->SetParentWnd(this);
//
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpSFXListBox),MODELTYPE_LISTBOX);
//		ModelUI::FRECT Rect(0.655f,0.235f,0.13f,0.14f);
//		m_lpSFXListBox->Create(&Rect,"",0,0);
//		m_lpSFXListBox->SetParentWnd(this);
//		m_lpSFXListBox->AddString("公告板",  MODELTYPE_BILLBOARD);
//		m_lpSFXListBox->AddString("粒子系统",MODELTYPE_PARTICLESYSTEM);
//		m_lpSFXListBox->AddString("刀光剑影",MODELTYPE_BLADE);
//		m_lpSFXListBox->AddString("屏幕特效",MODELTYPE_SCREENSFX);
//		m_lpSFXListBox->AddString("动作残影",MODELTYPE_HALO);
//		m_lpSFXListBox->AddString("镜头眩光",MODELTYPE_LIGHTFLARE);
//		m_lpSFXListBox->AddString("天空",    MODELTYPE_SKYBOX);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_pBSFXNew),MODELTYPE_BUTTON);
//		ModelUI::FRECT Rect(0.79f,0.235f,0.03f,0.03f);
//		m_pBSFXNew->Create(&Rect,"",0,0);
//		m_pBSFXNew->SetIconTexture("ToolBar\\Load.tga");
//		m_pBSFXNew->SetParentWnd(this);
//	}
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_Tree),MODELTYPE_TREECTRL);
//		ModelUI::FRECT Rect(0.82f,0.39f,0.185f,0.30f);
//		m_Tree->Create(&Rect,"",0,0);
//		m_Tree->SetParentWnd(this);
//	}
//
//	if (!m_lpSlider)
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&(m_lpSlider),MODELTYPE_SLIDEBAR);
//	}
//
//	float K = 0.9f;
//	ModelUI::FRECT Rectlider(m_RelativeRect.Left+m_RelativeRect.Width,m_RelativeRect.Top,
//		m_RelativeRect.Width*(1-K),m_RelativeRect.Height);
//	m_lpSlider->Create(&Rectlider,"",0,1);
//	m_lpSlider->SetParentWnd(this);
//
//
//	return S_OK;
//}
//
//HRESULT KModelToolSFX::Render()
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
//	LPMODEL pMesh = pMeshScene->GetCurMesh();
//	PropertyBase::KPropertyBase* pCurSFXBase = dynamic_cast <PropertyBase::KPropertyBase*> (pMesh);
//
//	/*if(pCurSFXBase)
//	{
//		if ((pCurSFXBase!=m_lpSFXBase)||(pCurSFXBase->m_bNeedUpdateUI))
//		{
//			Clear();
//			m_lpSFXBase = pCurSFXBase;
//			BuildItemInformation();
//			BuildUI();
//			pCurSFXBase->m_bNeedUpdateUI = FALSE;
//		}
//	}*/
//
//	//g_cGraphicsTool.DrawCommand(&m_CmdBack);
//
//	for(int i=0;i<m_nNumShowItem;i++)
//	{
//		if (i<(int)m_ItemVector.size())
//		{
//			ModelUI::KModelUIItem *pNewItem = m_ItemVector[i];
//			pNewItem->Render();
//		}
//	}
//
//	if (m_bShowEdit)
//	{
//		SAFE_RENDER(m_lpValueEdit);
//	}
//
//	if ((int)m_ItemInfo.size()>m_nMaxMumItem)
//	{
//		SAFE_RENDER(m_lpSlider);
//	}
//
//	SAFE_RENDER(m_lpSFXListBox);
//	SAFE_RENDER(m_pBSFXNew);
//	
//	if(m_pLastPosition)
//	{
//		D3DVIEWPORT9 Viewport;
//		D3DXMATRIX   Projection;
//		D3DXMATRIX   View;
//		g_pd3dDevice->GetViewport(&Viewport);
//		g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&Projection);
//		g_pd3dDevice->GetTransform(D3DTS_VIEW,&View);
//		
//		D3DXMATRIX   World;
//		D3DXMatrixIdentity(&World);
//
//		D3DXVECTOR3  ProjectedPos;
//		D3DXVec3Project(&ProjectedPos,m_pLastPosition,
//			&Viewport,&Projection,&View,&World);
//		D3DXVECTOR2 A(ProjectedPos.x,ProjectedPos.y);
//		D3DXVECTOR2 B(m_Rect.Left,m_Rect.Top);
//		g_cGraphicsTool.DrawScreenLine(&A,&m_PositionGridB,0,0xFF00FF00,0xFF00FF00);
//		g_cGraphicsTool.DrawScreenLine(&m_PositionGridA,&m_PositionGridB,0,0xFF00FF00,0xFF00FF00);
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolSFX::SetMeshEditScene(PVOID pScene)
//{
//	m_lpScene = pScene;
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolSFX::CleanUp()
//{
//	//SAFE_DELETE(m_lpDailog);
//	return KModel::CleanUp();
//}
//
//HRESULT KModelToolSFX::Clear()
//{
//	m_ItemInfo.clear();
//	m_nNumShowItem = 0;
//	m_bShowEdit = FALSE;
//
//	return S_OK;
//}
//
//HRESULT KModelToolSFX::BuildUI()
//{
//	return S_OK;
//
//	m_bShowEdit = FALSE;
//
//	if (!m_lpSFXBase)
//		return S_OK;
//	
//	m_nNumShowItem = 0;
//
//	for(int i=m_nItemStart;i<(int)m_ItemInfo.size();i++)
//	{
//		if (m_nNumShowItem>m_nMaxMumItem)
//			break;
//
//		_ItemInfo ItemInfo = m_ItemInfo[i];
//		m_nNumShowItem ++;
//		
//		int ItemIndex = i - m_nItemStart;
//
//		ModelUI::KModelUIItem *pNewItem = NULL;
//		if (ItemIndex>=(int)m_ItemVector.size())
//		{
//			g_cMeshTable.Get1NewXMesh((LPMODEL*)&pNewItem,MODELTYPE_ITEM);
//			m_ItemVector.push_back(pNewItem);
//		}
//		else
//		{
//			pNewItem = m_ItemVector[ItemIndex];
//		}
//
//		switch(ItemInfo.TYPE)
//		{
//		case GROUP:
//			{
//				PropertyBase::LPVARGROUP pGroup = (PropertyBase::LPVARGROUP) ItemInfo.Pointer;
//				ModelUI::FRECT Rect(m_Group_Left,m_Group_Top + m_Item_Sep * ItemIndex,
//					m_Group_Weigth,m_Item_Height);
//				pNewItem->Create(&Rect,"",0,ITEMSTYLE_TEXTIMAGE);
//				pNewItem->SetText((LPSTR)pGroup->scGroupName.c_str());
//				pNewItem->m_dwValue = i;
//				pNewItem->SetParentWnd(this);
//				if(pGroup->bExtracted)
//					pNewItem->SetIcon("ToolBar\\GroupOpen.tga");
//				else
//					pNewItem->SetIcon("ToolBar\\Group.tga");
//				break;
//			}
//		case GROUP_PROPERTY:
//			{
//				PropertyBase::LPVARPROPERTY pProperty = (PropertyBase::LPVARPROPERTY) ItemInfo.Pointer;
//				ModelUI::FRECT Rect(m_Group_Left+0.005f,m_Group_Top + m_Item_Sep * ItemIndex,
//					m_Group_Weigth-0.005f,m_Item_Height);
//				pNewItem->Create(&Rect,"",0,ITEMSTYLE_TEXTIMAGE);
//				pNewItem->SetText((LPSTR)pProperty->scName.c_str());
//				pNewItem->m_dwValue = i;
//				pNewItem->SetParentWnd(this);
//				pNewItem->SetIcon("ToolBar\\Clock.bmp");
//				break;
//			}
//		case ITEM:
//			{
//				PropertyBase::LPVARITEM pItem = (PropertyBase::LPVARITEM) ItemInfo.Pointer;
//				ModelUI::FRECT Rect(m_Group_Left+0.005f,m_Group_Top + m_Item_Sep * ItemIndex,
//					m_Group_Weigth-0.005f,m_Item_Height);
//				pNewItem->Create(&Rect,"",0,ITEMSTYLE_TEXTIMAGE);
//				pNewItem->SetText(pItem->Name);
//				pNewItem->m_dwValue = i;
//				pNewItem->SetParentWnd(this);
//				if(pItem->Extracted)
//					pNewItem->SetIcon("ToolBar\\ItemOpen.bmp");
//				else
//					pNewItem->SetIcon("ToolBar\\Item.bmp");
//				break;
//			}
//		case ITEM_PROPERTY:
//			{
//				PropertyBase::LPVARPROPERTY pProperty = (PropertyBase::LPVARPROPERTY) ItemInfo.Pointer;
//				ModelUI::FRECT Rect(m_Group_Left+0.01f,m_Group_Top + m_Item_Sep * ItemIndex,
//					m_Group_Weigth-0.01f,m_Item_Height);
//				pNewItem->Create(&Rect,"",0,ITEMSTYLE_TEXTIMAGE);
//				pNewItem->SetText((LPSTR)pProperty->scName.c_str());
//				pNewItem->m_dwValue = i;
//				pNewItem->SetParentWnd(this);
//				pNewItem->SetIcon("ToolBar\\Clock.bmp");
//				break;
//			}
//		}
//	}
//
//	return S_OK;
//}
//
//HRESULT KModelToolSFX::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if (message==WM_KEYDOWN)
//	{
//		if (wParam==VK_F7)
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
//	KModelUIBase::ProcessMessage(message,wParam,lParam);
//	
//	m_lpSFXListBox->ProcessMessage(message,wParam,lParam);
//	
//	if(ITEM_CLICK == m_pBSFXNew->ProcessMessage(message,wParam,lParam))
//	{
//		DWORD MeshType = 0;
//		TCHAR Name[256];
//		KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//		if (!pMeshScene)
//			return S_OK;
//		if(SUCCEEDED(m_lpSFXListBox->GetCurSelectedString(Name,&MeshType)))
//		{
//			LPMODEL pNewSFX = NULL;
//			g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pNewSFX),MeshType);
//			pNewSFX->LoadMesh("");
//			pMeshScene->AddRenderMesh(pNewSFX);
//		
//			if(MODELTYPE_SKYBOX!=MeshType)
//				pMeshScene->AddSelectableMesh(pNewSFX);
//		}
//		m_pBSFXNew->m_dwState = 0;
//	}
//
//	if ((int)m_ItemInfo.size()>m_nMaxMumItem)
//	{
//		DWORD MSG = m_lpSlider->ProcessMessage(message,wParam,lParam);
//		if (MSG == SLIDER_MOVE)
//		{
//			m_nItemStart = (int)(m_lpSlider->m_fPercent * 0.01f * (m_ItemInfo.size()-m_nMaxMumItem));
//			BuildUI();
//		}
//	}
//
//	for(int i=0;i<m_nNumShowItem;i++)
//	{
//		ModelUI::KModelUIItem *pNewItem = m_ItemVector[i];
//		
//		DWORD MSG = pNewItem->ProcessMessage(message,wParam,lParam);
//		if (MSG==ITEM_CLICK)
//		{
//			for(int i=0;i<m_nNumShowItem;i++)
//			{
//				ModelUI::KModelUIItem *pItem = m_ItemVector[i];
//				pItem->m_bSelected = FALSE;
//			}
//			pNewItem->m_bSelected = TRUE;
//
//			_ItemInfo ItemInfo = m_ItemInfo[pNewItem->m_dwValue];
//
//			switch(ItemInfo.TYPE)
//			{
//			case GROUP:
//				{
//					PropertyBase::LPVARGROUP pGroup = (PropertyBase::LPVARGROUP) ItemInfo.Pointer;
//					break;
//				}
//			case GROUP_PROPERTY:
//			case ITEM_PROPERTY:
//				{
//					PropertyBase::LPVARPROPERTY pProperty = (PropertyBase::LPVARPROPERTY) ItemInfo.Pointer;
//					
//					ModelUI::FRECT Rect(pNewItem->m_RelativeRect.Left+pNewItem->m_RelativeRect.Width+0.02f,
//						pNewItem->m_RelativeRect.Top,
//						pNewItem->m_RelativeRect.Width,
//						pNewItem->m_RelativeRect.Height);
//					
//					m_PositionGridA = D3DXVECTOR2(pNewItem->m_Rect.Left,
//						pNewItem->m_Rect.Top+0.5f*pNewItem->m_Rect.Height);
//					m_PositionGridB = m_PositionGridA - D3DXVECTOR2(30,0);
//
//					m_lpValueEdit->SetParentWnd(NULL);
//					m_lpValueEdit->Create(&Rect,"",0,0);
//					m_lpValueEdit->SetParentWnd(this);
//				
//					switch(pProperty->Type)
//					{
//					case PropertyBase::VARPROPERTY::VAR_INT:
//						{
//							int * pInt = (int*) pProperty->Address;
//							if (pInt)
//							{
//								TCHAR str[256];
//								wsprintf(str,"%d",*pInt);
//								m_lpValueEdit->SetText(str);
//							}
//							m_lpValueEdit->m_dwValue = pNewItem->m_dwValue;
//							m_bShowEdit = TRUE;	
//							m_pLastPosition = NULL;
//							break;
//						}
//					/*case PropertyBase::VARPROPERTY::VAR_DWORD:
//						{
//							DWORD * pDword = (DWORD*) pProperty->Address;
//							if (pDword)
//							{
//								TCHAR str[256];
//								wsprintf(str,"%d",*pDword);
//								m_lpValueEdit->SetText(str);
//							}
//							m_lpValueEdit->m_dwValue = pNewItem->m_dwValue;
//							m_bShowEdit = TRUE;	
//							m_pLastPosition = NULL;
//							break;
//						}*/
//					case PropertyBase::VARPROPERTY::VAR_FLOAT:
//						{
//							float * pFloat = (float*) pProperty->Address;
//							if (pFloat)
//							{
//								TCHAR str[256];
//								sprintf(str,"%f",*pFloat);
//								m_lpValueEdit->SetText(str);
//							}
//							m_lpValueEdit->m_dwValue = pNewItem->m_dwValue;
//							m_bShowEdit = TRUE;	
//							m_pLastPosition = NULL;
//							break;
//						}
//					case PropertyBase::VARPROPERTY::VAR_POSITION:
//						{
//							D3DXVECTOR3 * pPos = (D3DXVECTOR3*) pProperty->Address;
//							if (pPos)
//							{
//								KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//								if (pMeshScene)
//								{
//									pMeshScene->RemoveRenderEntity(SCENEENTITY_POSITION,pPos);
//									pMeshScene->RemoveSelectableEntity(SCENEENTITY_POSITION,pPos);
//									pMeshScene->RemoveSelectedEntity(SCENEENTITY_POSITION,pPos);
//									m_pLastPosition = pPos;
//									pMeshScene->AddRenderEntity(SCENEENTITY_POSITION,pPos);
//									pMeshScene->AddSelectableEntity(SCENEENTITY_POSITION,pPos);
//									pMeshScene->AddSelectedEntity(SCENEENTITY_POSITION,pPos);
//								}
//							}
//							m_bShowEdit = FALSE;	
//							break;
//						}	
//					}	
//
//					break;
//				}
//			}			
//		}
//		else if (MSG==ITEM_DBCLICK)
//		{
//			_ItemInfo ItemInfo = m_ItemInfo[pNewItem->m_dwValue];
//			
//			for(int i=0;i<m_nNumShowItem;i++)
//			{
//				ModelUI::KModelUIItem *pItem = m_ItemVector[i];
//				pItem->m_bSelected = FALSE;
//			}
//			pNewItem->m_bSelected = TRUE;
//
//			switch(ItemInfo.TYPE)
//			{
//			case GROUP:
//				{
//					PropertyBase::LPVARGROUP pGroup = (PropertyBase::LPVARGROUP) ItemInfo.Pointer;
//					pGroup->bExtracted = !pGroup->bExtracted;
//					BuildItemInformation();
//					BuildUI();
//					break;
//				}
//			case ITEM:
//				{
//					PropertyBase::LPVARITEM pITEM = (PropertyBase::LPVARITEM) ItemInfo.Pointer;
//					pITEM->Extracted = !pITEM->Extracted;
//					BuildItemInformation();
//					BuildUI();
//					break;
//				}
//			}
//		}
//	}
//
//	if (m_bShowEdit)
//	{
//		//if(EDIT_RETURN == m_lpValueEdit->ProcessMessage(message,wParam,lParam))
//		DWORD msg = m_lpValueEdit->ProcessMessage(message,wParam,lParam);
//		{
//			_ItemInfo ItemInfo = m_ItemInfo[m_lpValueEdit->m_dwValue];
//			switch(ItemInfo.TYPE)
//			{
//			case GROUP:
//				{
//					PropertyBase::LPVARGROUP pGroup = (PropertyBase::LPVARGROUP) ItemInfo.Pointer;
//					break;
//				}
//			case GROUP_PROPERTY:
//			case ITEM_PROPERTY:
//				{
//					PropertyBase::LPVARPROPERTY pProperty = (PropertyBase::LPVARPROPERTY) ItemInfo.Pointer;
//					switch(pProperty->Type)
//					{
//					case PropertyBase::VARPROPERTY::VAR_INT:
//						{
//							if(msg == EDIT_RETURN)
//							{
//								int * pInt = (int*) pProperty->Address;
//								if (pInt)
//									sscanf(m_lpValueEdit->m_szString,"%d",pInt);
//								m_lpSFXBase->OnRefresh();
//							}
//							break;
//						}
//					/*case PropertyBase::VARPROPERTY::VAR_DWORD:
//						{
//							if(msg == EDIT_RETURN)
//							{
//								DWORD * pDword = (DWORD*) pProperty->Address;
//								if (pDword)
//									sscanf(m_lpValueEdit->m_szString,"%d",pDword);
//								m_lpSFXBase->OnRefresh();
//							}
//							break;
//						}*/
//					case PropertyBase::VARPROPERTY::VAR_FLOAT:
//						{
//							float * pFloat = (float*) pProperty->Address;
//							if (pFloat)
//								sscanf(m_lpValueEdit->m_szString,"%f",pFloat);
//							//m_lpSFXBase->OnRefresh();
//							break;
//						}
//					}
//					break;
//				}
//			}
//		}
//	}
//	return S_OK;
//}
//
//
//HRESULT KModelToolSFX::BuildItemInformation()
//{
//	m_bShowEdit = FALSE;
//
//	if (!m_lpSFXBase)
//		return S_OK;
//
//	int N_Item = 0;
//	
//	m_ItemInfo.clear();
//
//	for(int i=0;i<(int)m_lpSFXBase->m_PropertyGroupVector.size();i++)
//	{
//		PropertyBase::LPVARGROUP pGroup = m_lpSFXBase->m_PropertyGroupVector[i];
//		
//		ModelUI::KModelUIItem *pNewItem = NULL;
//
//		_ItemInfo ItemInfo(pGroup,GROUP);
//
//		m_ItemInfo.push_back(ItemInfo);
//		
//		if (pGroup->bExtracted)
//		{
//			for(int i_Item=0;i_Item<(int)pGroup->VarItemVector.size();i_Item++)
//			{
//				PropertyBase::LPVARITEM pItem = pGroup->VarItemVector[i_Item];
//				
//				_ItemInfo ItemInfo(pItem,ITEM);
//				
//				m_ItemInfo.push_back(ItemInfo);
//				
//				if (pItem->Extracted)
//				{
//					for(int i_Pro=0;i_Pro<(int)pItem->PropertyVector.size();i_Pro++)
//					{
//						PropertyBase::LPVARPROPERTY pProperty = pItem->PropertyVector[i_Pro];
//						
//						_ItemInfo ItemInfo(pProperty,ITEM_PROPERTY);
//						
//						m_ItemInfo.push_back(ItemInfo);
//					}
//				}
//			}	
//
//			for(int i_Pro=0;i_Pro<(int)pGroup->PropertyVector.size();i_Pro++)
//			{
//				PropertyBase::LPVARPROPERTY pProperty = pGroup->PropertyVector[i_Pro];
//				
//				_ItemInfo ItemInfo(pProperty,GROUP_PROPERTY);
//				
//				m_ItemInfo.push_back(ItemInfo);
//			}
//		}
//	}
//	return S_OK;
//}
//
//HRESULT KModelToolSFX::ComputeRect()
//{
//	KModelUIBase::ComputeRect();
//	m_Group_Left   = m_RelativeRect.Left + 0.01f;
//	m_Group_Weigth = m_RelativeRect.Width - 0.02f;
//	m_Group_Top    = m_RelativeRect.Top + 0.235f;
//	return S_OK;
//}

};
