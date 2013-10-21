#include "stdafx.h"
#include ".\scenemesheditor.h"
//#include "kdlg_timepointedit.h"
#include "KSceneEditBaseSelectDialog.h"
#include "kdlg_prt_att.h"
#include "KSceneModelEditorSFXBillBoardDialog.h"
#include "KDlg_PRT_CONE_ATT.h"
//#include"KDlg_VolumeFog.h"
//#include"KDlg_Lightning.h"
#include "KSceneModelEditorSFXBladeDialog.h"
#include "KSceneModelEditorSFXSpeedTreeDialog.h"
#include "KSceneModelEditorSFXSceneSFXDialog.h"
#include "KModelSpeedTree.h"
#include "KModelSceneSFX.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

using namespace PropertyBase;
KSceneModelEditor::KSceneModelEditor(void)
{
	m_bShowGrid = TRUE;
	//m_lpWinBase = NULL;

	m_bMoveMesh = FALSE;

	/*m_lpMaterialTool  = NULL;
	m_lpColorPick     = NULL;
	m_lpAnimationTool = NULL;
	m_lpMeshTool      = NULL;*/
	//m_lpMeshSelect    = NULL;
	m_lpCurMesh       = NULL;
	/*m_pPS             = NULL;*/

	m_lpDialogSFXBillBoard = NULL;
	m_lpDialogSFXBlade = NULL;
	m_lpDialogSFXSpeedTree = NULL;
	m_lpDialogSFXSceneSFX = NULL;
}

KSceneModelEditor::~KSceneModelEditor(void)
{
	/*SAFE_DELETE(m_pPS);*/

	SAFE_DELETE(m_lpDialogSFXBillBoard);
	SAFE_DELETE(m_lpDialogSFXBlade);
	SAFE_DELETE(m_lpDialogSFXSpeedTree);

}

HRESULT KSceneModelEditor::Acquire()
{
	KSceneEditBase::Acquire();
	int PageIndex = this->AddPage("特效");

	PropertyBase::_PropertyPage* pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		PropertyBase::_PropertyGroup Group(0,"新建特效",FALSE);
		int index_Group = pPage->AddGroup(Group);
		PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

		pGroup->AddPropertySFXNew("NewSFX",this);
	}

	return S_OK;
}

HRESULT KSceneModelEditor::FrameMove(void)
{
	m_bShowSelectionRect = FALSE;

	K3DScene::FrameMove();

	return S_OK;
}

HRESULT KSceneModelEditor::Render(void)
{
	//Wnd_Heartbeat();

	if(m_bShowGrid)
	{
		g_cGraphicsTool.DrawGroundGrid(100);
	}

	K3DScene::Render();

	LPMODEL pCurMesh = GetCurMesh();
	/*if (pCurMesh)
	{
		pCurMesh->DrawBBox();
	}*/

	if ((pCurMesh)&&(m_bMoveMesh))//在这里旋转选中的物体
	{
		float f = timeGetTime()*0.0003f;
		D3DXMatrixRotationX(&pCurMesh->m_Matrix,f);
	//	pCurMesh->m_lpBoneInfo
	}

	ENTITYLIST::iterator i = m_RenderEntityList.begin();
	while(i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		if (Entity.Type==SCENEENTITY_MODEL)
		{
			LPMODEL pMesh = (LPMODEL) Entity.Pointer;

			if(m_bShowBone)
			{
				pMesh->DrawBones();
			}
			if(m_bShowSocket)
			{
				//pMesh->DrawSocket();
			}
		}
		i++;
	}

	/*if (m_lpMaterialTool)
		m_lpMaterialTool->Render();

	if (m_lpColorPick)
		m_lpColorPick->Render();

	if (m_lpAnimationTool)
		m_lpAnimationTool->Render();

	if (m_lpMeshTool)
		m_lpMeshTool->Render();*/

	//SAFE_RENDER(m_lpMeshSelect);

	//SAFE_RENDER(m_lpMeshPropertySet);

	//SAFE_RENDER(m_lpWinBase);

	//Wnd_RenderWindows();

	KSceneEditBase::Render();


	//g_cGraphicsTool.DrawShadowVolumeMask();

	return S_OK;
}

HRESULT KSceneModelEditor::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	//Wnd_ProcessInput(message,wParam,lParam);

	KSceneEditBase::ProcessMessage(message,wParam,lParam);

	//if (m_lpMaterialTool)
	//{
	//	if (m_lpMaterialTool->ProcessMessage(message,wParam,lParam)==MSG_PROCESSED)
	//	{
	//		m_bShowSelectionRect = FALSE;
	//		return S_OK;
	//	}
	//	if (m_lpAnimationTool->ProcessMessage(message,wParam,lParam)==MSG_PROCESSED)
	//	{
	//		m_bShowSelectionRect = FALSE;
	//		return S_OK;
	//	}
	//	if (m_lpMeshTool->ProcessMessage(message,wParam,lParam)==MSG_PROCESSED)
	//	{
	//		m_bShowSelectionRect = FALSE;
	//		return S_OK;
	//	}
	//
	//	/*if (m_lpMeshSelect->ProcessMessage(message,wParam,lParam)==MSG_PROCESSED)
	//	{
	//		m_bShowSelectionRect = FALSE;
	//		return S_OK;
	//	}*/

	//	if (m_lpMeshPropertySet->ProcessMessage(message,wParam,lParam)==MSG_PROCESSED)
	//	{
	//		m_bShowSelectionRect = FALSE;
	//		return S_OK;
	//	}
	//}

	//if (m_lpColorPick)
	//{
	//	if (m_lpColorPick->ProcessMessage(message,wParam,lParam)==MSG_PROCESSED)
	//	{
	//		m_bShowSelectionRect = FALSE;
	//		return S_OK;
	//	}
	//}

	switch(message)
	{
	case WM_KEYUP:
		{
			g_KeyState[wParam] = 0;

			GraphicsStruct::KCamera* pCamera = &g_GraphiceEngine.m_cCamera;

			switch(wParam)
			{
			case 'H':
				pCamera->Position  = D3DXVECTOR3(500,500,-500);
				pCamera->ViewPoint = D3DXVECTOR3(0,0,0);
				break;
			/*case 'J':
				m_bMoveMesh = !m_bMoveMesh;
				break;
			case 'K':
				{
					OpenSubdvMesh();
					break;
				}
			case 'P':
				{
					static bool bCreated = false;
					static LPMODEL pMesh = NULL;
					//static int iID = 0;
					if ( !bCreated )
					{
						if (SUCCEEDED(g_cMeshTable.Get1NewXMesh(&pMesh,MODELTYPE_PARTICLESYSTEM)))
						{
							pMesh->LoadMesh("");
							AddRenderMesh(pMesh);
							bCreated = true;
						}
					}
					else
					{
						ModelSFX::KLauncher *pLan = new ModelSFX::KLauncher;
						ModelSFX::KModelSFXParticleSystem* pTemp = static_cast<ModelSFX::KModelSFXParticleSystem*>(pMesh);
						pLan->SetTargetBarrier( pTemp->GetBarrier());
						pLan->SetTargetForceField( pTemp->GetForceField());
						pTemp->AddLauncher(pLan);
						if ( m_pPS )
							m_pPS->UpdateMenu(m_pPS->GetMenu());


					}
					break;
				}*/

			case VK_F9:
				{
					KSceneEditBaseSelectDialog Dlg;
					Dlg.SetSceneEditor((KSceneEditBase*)this);
					Dlg.DoModal();

					KSceneEntity Entity;

					if(SUCCEEDED(GetFirstSelectedEntity(Entity)))
					{
						switch(Entity.Type)
						{
						case SCENEENTITY_MODEL:
							{
								LPMODEL pModel = (LPMODEL) Entity.Pointer;
								SetCurMesh(pModel);

								break;
							}
						}
					}
					else
					{
						SetCurMesh(NULL);
					}

					break;
				}
			case VK_F6:
				{
					KModel* pMesh = GetCurMesh();
					if ( pMesh )
					{
						KDlg_PRT_ATT Dlg(AfxGetMainWnd(), GetCurMesh());
						Dlg.DoModal();
					}

				}
			case VK_F8:
				{
					KModel* pMesh = GetCurMesh();
					if ( pMesh )
					{
						KDlg_PRT_CONE_ATT* pDlgConAtt = new KDlg_PRT_CONE_ATT(pMesh, AfxGetMainWnd());
					}
				}
			}
		}
	}
	return S_OK;
}

HRESULT KSceneModelEditor::Edit_Select()
{
	return S_OK;
}
/*void KSceneModelEditor::ONMOUSEMOVE(WPARAM wParam, LPARAM lParam)
{
	int fwKeys = wParam;
	int xPos = LOWORD(lParam);
	int yPos = HIWORD(lParam);

	KSceneTable* pSceneTable = (KSceneTable*) m_pSceneTable;
	KEngineManager* pManager = (KEngineManager*) pSceneTable->m_pManager;
	KGraphicsEngine* pGEngine = (KGraphicsEngine*) &pManager->g_GraphiceEngine;
	GraphicsStruct::KCamera* pCamera = &pGEngine->m_cCamera;

	D3DXVECTOR3 Front,Up,Left;
	Front = pCamera->ViewPoint - pCamera->Position;
	float Radius = D3DXVec3Length(&Front);
	D3DXVec3Normalize(&Front,&Front);
	Up = pCamera->DefUp;
	D3DXVec3Cross(&Left,&Up,&Front);
	D3DXVec3Cross(&Up,&Front,&Left);

	float X = 0.005f*Radius*(xPos-m_LastMousePos.x);
	float Y = 0.005f*Radius*(yPos-m_LastMousePos.y);
	if ((fwKeys&MK_MBUTTON)&&(!(fwKeys&MK_RBUTTON)))
	{
		/*if (SUCCEEDED(GetSelectCenter(&m_SelectedCenter)))
		{
		pCamera->ViewPoint += (m_SelectedCenter - pCamera->ViewPoint)*0.1f;
		}

		D3DXVECTOR3 M = -X*Left+Y*Up + pCamera->Position;
		D3DXVECTOR3 N = M - pCamera->ViewPoint;
		D3DXVec3Normalize(&N,&N);
		M = Radius*N;
		pCamera->Position = M + pCamera->ViewPoint;
	}
	else if ((fwKeys&MK_LBUTTON)&&(g_KeyState[VK_SPACE]==1))
	{
		//m_bShowSelectionRect = FALSE;

		D3DXVECTOR3 m_Left  = - Left;
		D3DXVECTOR3 m_Front = Front;
		m_Left.y  = 0;
		m_Front.y = 0;
		D3DXVec3Normalize(&m_Left,&m_Left);
		D3DXVec3Normalize(&m_Front,&m_Front);

		pCamera->Position  += X*m_Left + Y*m_Front;
		pCamera->ViewPoint += X*m_Left + Y*m_Front;
	}
	else if ((fwKeys&MK_RBUTTON)&&(g_KeyState[VK_SPACE]==1))
	{
		D3DXVECTOR3 m_Left  = - Left;
		D3DXVECTOR3 m_Front = Up;
		m_Left.y  = 0;
		m_Front.x = 0;
		m_Front.z = 0;

		D3DXVec3Normalize(&m_Left,&m_Left);
		D3DXVec3Normalize(&m_Front,&m_Front);

		pCamera->Position  += X*m_Left + Y*m_Front;
		pCamera->ViewPoint += X*m_Left + Y*m_Front;
	}
	else if (!(fwKeys&MK_MBUTTON)&&((fwKeys&MK_RBUTTON)))
	{
		D3DXVECTOR3 M = -X*Left+Y*Up + pCamera->ViewPoint;
		D3DXVECTOR3 N = M - pCamera->Position;
		D3DXVec3Normalize(&N,&N);
		M = Radius*N;
		pCamera->ViewPoint = pCamera->Position + M ;
	}
	pCamera->SetCamera();
	m_LastMousePos.x = xPos;
	m_LastMousePos.y = yPos;
}*/

HRESULT KSceneModelEditor::Initialize(PVOID pSceneTable)
{
	KSceneEditBase::Initialize(pSceneTable);

	//g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpMaterialTool),MODELTYPE_MATERIALTOOL);
	//m_lpMaterialTool->LoadMesh("");
	//m_lpMaterialTool->SetMeshEditScene(this);

	//g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpColorPick),MODELTYPE_COLORPICK);
	//m_lpColorPick->LoadMesh("");
	//m_lpColorPick->SetMeshEditScene(this);
	//m_lpColorPick->m_bHide = TRUE;

	//g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpAnimationTool),MODELTYPE_ANIMATIONTOOL);
	//m_lpAnimationTool->LoadMesh("");
	//m_lpAnimationTool->SetMeshEditScene(this);

	//g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpMeshTool),MODELTYPE_MESHTOOL);
	//m_lpMeshTool->LoadMesh("");
	//m_lpMeshTool->SetMeshEditScene(this);

	//g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpMeshSelect),MODELTYPE_MESHSELECT);
	//m_lpMeshSelect->LoadMesh("");
	//m_lpMeshSelect->SetMeshEditScene(this);

	//g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpMeshPropertySet),MODELTYPE_PROPERTYSET);
	//m_lpMeshPropertySet->LoadMesh("");
	//m_lpMeshPropertySet->SetMeshEditScene(this);

	/*g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpWinBase),MODELTYPE_WINBASE);
	//ModelUI::FRECT rect(0.3f,0.3f,0.5f,0.4f);
	//m_lpWinBase->Create(&rect,"",0,0);
	KIniFile MIni;
	TCHAR Name[256];
	wsprintf(Name,"%s\\Test.ini",g_Def_AppDirectory);
	if(MIni.Load(Name))
	{
		m_lpWinBase->Init(&MIni,"Main");
	}*/
	return S_OK;
}

LPMODEL KSceneModelEditor::GetCurMesh()
{
	if (m_lpCurMesh)
		return m_lpCurMesh;

	/*ENTITYLIST::iterator i = m_RenderEntityList.begin();
	while(i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		if (Entity.Type==SCENEENTITY_MODEL)
		{
			LPMODEL pMesh = (LPMODEL) Entity.Pointer;
			return pMesh;
		}
		i++;
	}*/
	return NULL;
}

void KSceneModelEditor::OpenSubdvMesh()
{
	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh|";

	CFileDialog dlg(true, NULL, NULL,
		OFN_HIDEREADONLY , szFilter);
	// Modify OPENFILENAME members directly to point to bigBuff
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	if (	dlg.DoModal() == IDOK)
	{
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		CString Vs = dlg.GetPathName();

		wsprintf(path_buffer,"%s",Vs);
		_splitpath( path_buffer, drive, dir, fname, ext );


		TCHAR filename[256];
		wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);

		LPMODEL pMesh = NULL;
		DWORD   NewMeshID  = 0;

		if (FAILED(g_cMeshTable.Get1NewXMesh(&pMesh,MODELTYPE_SUBDEV)))
			return ;

		pMesh->LoadMesh(filename);
		wsprintf(filename,"%s%s%s.Mtl",drive,dir,fname);
		pMesh->LoadMaterial(filename);

		AddRenderMesh(pMesh);
		AddSelectableMesh(pMesh);
	}
}

HRESULT KSceneModelEditor::OnChangeMaterialColor(void)
{
	/*if (m_lpColorPick)
	{
		D3DCOLORVALUE Color = m_lpColorPick->m_CurColor;
		if (m_lpMaterialTool)
		{
			m_lpMaterialTool->SetMaterialColor(Color);
		}
	}*/
	return E_NOTIMPL;
}

HRESULT KSceneModelEditor::SetCurMesh(LPMODEL pMesh)
{
	if(pMesh)
	{
		pMesh->Acquire();
		m_lpDlgPropertySet->SetPropertyBase(pMesh);
	}
	else
	{
		m_lpDlgPropertySet->SetPropertyBase(this);
	}

	m_lpCurMesh = pMesh;
	return S_OK;
}

HRESULT KSceneModelEditor::OnSelectChanged()
{
	return S_OK;
}

//KDlg_SpeedTree* g_pDlgST = 0;
//KDlg_VolumeFog* g_pDlgVF = 0;
//KDlg_Lightning* g_pDlgL = 0;

HRESULT KSceneModelEditor::ShowProperty(BOOL bShow)
{
	LPMODEL pCurMesh = GetCurMesh();
	if ( pCurMesh )
	{
		if ( pCurMesh->m_dwType == MODELTYPE_PARTICLESYSTEM )
		{
			//if ( !m_pPS )
			//{
			//	//Create Window;
			//	ModelSFX::KModelSFXParticleSystem* pPS = static_cast<ModelSFX::KModelSFXParticleSystem*>(pCurMesh);
			//	m_pPS = new KDlg_PS_Main( AfxGetApp()->GetMainWnd(), pPS);
			//	m_pPS->Create(IDD_DIALOG_PS_MAIN, AfxGetApp()->GetMainWnd());
			//	RECT rect;
			//	m_pPS->GetWindowRect(&rect);
			//	rect.left += 500;
			//	rect.right += 500;
			//	m_pPS->MoveWindow(&rect, FALSE);
			//	m_pPS->ShowWindow(SW_SHOW);
			//	return S_OK;
			//}
			//else
			//{
			//	m_pPS->ShowWindow(SW_SHOW);
			//	return S_OK;
			//}
		}
		else if(pCurMesh->m_dwType == MODELTYPE_SPEEDTREE)
		{
			KModelSpeedTree* pTree = static_cast<KModelSpeedTree*>(pCurMesh);

			if(!m_lpDialogSFXSpeedTree)
			{
				m_lpDialogSFXSpeedTree = new KSceneModelEditorSFXSpeedTreeDialog;
				m_lpDialogSFXSpeedTree->SetSpeedTree(pTree,this);
				m_lpDialogSFXSpeedTree->Create(IDD_SFX_SPEEDTREE);
			}

			m_lpDialogSFXSpeedTree->ShowWindow(TRUE);

			return S_OK;
		}
		else if(pCurMesh->m_dwType == MODELTYPE_SCENESFX)
		{
			KModelSceneSFX* pSFX = static_cast<KModelSceneSFX*>(pCurMesh);

			if(!m_lpDialogSFXSceneSFX)
			{
				m_lpDialogSFXSceneSFX = new KSceneModelEditorSFXSceneSFXDialog;
				m_lpDialogSFXSceneSFX->SetSceneSFX(pSFX);
				m_lpDialogSFXSceneSFX->Create(IDD_SFX_SCENESFX);
			}

			m_lpDialogSFXSceneSFX->ShowWindow(TRUE);

			return S_OK;
		}
		else if(pCurMesh->m_dwType == MODELTYPE_LAYEREDVOLUMETRICFOG)
		{
			/*KVolumeFog* pVF = static_cast<KVolumeFog*>(pCurMesh);

			if(!g_pDlgVF)
			{
				g_pDlgVF = new KDlg_VolumeFog;
				g_pDlgVF->Create();
			}
			g_pDlgVF->SetVolumeFog(pVF);
			g_pDlgVF->Show();*/

			return S_OK;
		}
		else if(pCurMesh->m_dwType == MODELTYPE_DYNAMICLIGHTNING)
		{/*
			KLightning* pL = static_cast<KLightning*>(pCurMesh);

			if(!g_pDlgL)
			{
				g_pDlgL = new KDlg_Lightning;
				g_pDlgL->Create();
			}
			g_pDlgL->SetLightning(pL);
			g_pDlgL->ShowWindow(true);*/

			return S_OK;
		}
		else if(pCurMesh->m_dwType == MODELTYPE_BILLBOARD)
		{
			ModelSFX::KModelSFXBillBoard* pBillBoard =
				static_cast<ModelSFX::KModelSFXBillBoard*>(pCurMesh);

			if(!m_lpDialogSFXBillBoard)
			{
				m_lpDialogSFXBillBoard = new KSceneModelEditorSFXBillBoardDialog;
				m_lpDialogSFXBillBoard->SetBillBoard(this,pBillBoard);
				m_lpDialogSFXBillBoard->Create(IDD_SFX_BILLBOARD);
			}
			m_lpDialogSFXBillBoard->SetBillBoard(this,pBillBoard);

			m_lpDialogSFXBillBoard->ShowWindow(TRUE);

			return S_OK;
		}
		else if(pCurMesh->m_dwType == MODELTYPE_BLADE)
		{
			ModelSFX::KModelSFXBlade* pBlade =
				static_cast<ModelSFX::KModelSFXBlade*>(pCurMesh);
			if(!m_lpDialogSFXBlade)
			{
				m_lpDialogSFXBlade = new KSceneModelEditorSFXBladeDialog;
				m_lpDialogSFXBlade->SetBlade(this,pBlade);
				m_lpDialogSFXBlade->Create(IDD_SFX_BLADE);
			}
			m_lpDialogSFXBlade->SetBlade(this,pBlade);

			m_lpDialogSFXBlade->ShowWindow(TRUE);

			return S_OK;
		}
	}

	//if(m_pPS)
	{
		//m_lpDlgPropertySet->SetPropertyBase(this);
		m_lpDlgPropertySet->ShowWindow(TRUE);
	}

	return S_OK;
}