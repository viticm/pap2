#include "stdafx.h"
#include ".\sceneeditbase.h"
#include "SceneSkinEdit.h"
#include "KColorDialog.h"
#include "KTimeLineEditorDialog.h"
#include "KTimeLineColorEditWindow.h"
#include "ktimeline.h"
#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KSceneEditBase::KSceneEditBase(void)
{
	m_lpDlgPropertySet = NULL;
	m_dwSelectState = 0;

	m_bEditing = TRUE;
	m_lpMoveGrid = NULL;
	m_lpRotateGrid = NULL;

	m_bShowSelectionRect = FALSE;

	m_dwState = 0;
	m_dwSubState = 0;

	m_GridTextureID = 0;

	m_SelectedCenter = D3DXVECTOR3(0,0,0);

}

KSceneEditBase::~KSceneEditBase(void)
{
	;
}

HRESULT KSceneEditBase::Edit_Select(void)
{
	if(GetAsyncKeyState(VK_LCONTROL))
	{
		m_dwSelectState = 2;//加选
	}
	else if(GetAsyncKeyState(VK_LMENU))
	{
		m_dwSelectState = 1;//减选
	}
	else
	{
		m_dwSelectState = 0;
	}

	D3DXVECTOR2 A,C;
	POINT PA = m_PointSelectStart;
	POINT PC = m_PointSelectEnd;
	//ScreenToClient( g_hRenderWnd, &PA );
	//ScreenToClient( g_hRenderWnd, &PC );

	A = D3DXVECTOR2(PA.x*1.0f,PA.y*1.0f);
	C = D3DXVECTOR2(PC.x*1.0f,PC.y*1.0f);
	if (A.x>C.x)
	{
		float temp = A.x;
		A.x = C.x;
		C.x = A.x;
	}
	if (A.y>C.y)
	{
		float temp = A.y;
		A.y = C.y;
		C.y = A.y;
	}

	ModelUI::FRECT SelectRect;
	SelectRect.Left = A.x;
	SelectRect.Width = C.x - A.x;
	SelectRect.Bottom = A.y;
	SelectRect.Height = C.y - A.y;

	if(SelectRect.Width*SelectRect.Height<=25)
	{
		KSceneEntity Selected;
		float        MinDistance = 100000;

		KGraphicsEngine* pEng = &g_GraphiceEngine;

		D3DXVECTOR3 Orig,Dir;
		pEng->GetPickRay(&Orig,&Dir);

		K3DScene::ENTITYLIST::iterator i = m_SelectableList.begin();
		while(i!=m_SelectableList.end())
		{
			KSceneEntity Entity = *i;
			switch(Entity.Type)
			{
			case SCENEENTITY_OBJECT:
				{
					LPOBJECT pObject = (LPOBJECT) Entity.Pointer;



					float Distance = 0;
					if(pObject->SelectByPickLine(&Distance,&Orig,&Dir))
					{
						if(MinDistance>Distance)
						{
							MinDistance = Distance;
							Selected = Entity;
						}
					}
					break;
				}
			case SCENEENTITY_GROUP:
				{
					KSceneSceneEdit* pEdit = (KSceneSceneEdit*) Entity.RefData;
					DWORD Index = (DWORD) Entity.Pointer;
					KSceneEntityGroup* pGroup = &pEdit->m_vecEntityGroup[Index] ;

					D3DXVECTOR3 Inter;
					D3DXVECTOR3 B = Orig + Dir*100000;
					D3DXVECTOR3 BoxPos[8];
					pGroup->m_BBox.Extract(BoxPos);

					if(::IsLineIntersectBox(&Inter,&Orig,&B,BoxPos))
					{
						D3DXVECTOR3 Len = Inter - Orig;
						float Length = D3DXVec3Length(&Len);

						if(MinDistance>Length)
						{
							MinDistance = Length;
							Selected = Entity;
						}
					}
					break;
				}
			}

			i++;
		}

		if(MinDistance<100000)
		{
			if(m_dwSelectState==2)
			{
				m_SelectedList.push_back(Selected);
			}
			else if(m_dwSelectState==1)
			{
				m_SelectedList.remove(Selected);
			}
			else
			{
				m_SelectedList.clear();
				m_SelectedList.push_back(Selected);
			}
		}
		else
		{
			if(m_dwSelectState==2)
			{
				;
			}
			else if(m_dwSelectState==1)
			{
				;
			}
			else
			{
				m_SelectedList.clear();
			}
		}

		OnSelectChanged();
	}
	else
	{
		D3DVIEWPORT9 Viewport;
		D3DXMATRIX   Projection;
		D3DXMATRIX   View;
		g_pd3dDevice->GetViewport(&Viewport);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&Projection);
		g_pd3dDevice->GetTransform(D3DTS_VIEW,&View);

		D3DXVECTOR3 CameraPos;
		D3DXVECTOR3 CameraDir;
		g_cGraphicsTool.GetCameraPos(&CameraPos);
		g_cGraphicsTool.GetCameraFront(&CameraDir);

		if (m_dwSelectState==0)
			CleanSelection();

		K3DScene::ENTITYLIST::iterator i = m_SelectableList.begin();
		while(i!=m_SelectableList.end())
		{
			KSceneEntity Entity = *i;
			switch(Entity.Type)
			{
			case SCENEENTITY_OBJECT:
				{
					LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
					D3DXVECTOR3 BoxPos[8];
					D3DXVECTOR3 Intersect;
					D3DXVECTOR3 Center = pObject->m_BBox.GetCenter();
					D3DXVec3TransformCoord(&Center,&Center,&pObject->m_Matrix);
					D3DXVECTOR3 Nor = Center - CameraPos;

					float Length = D3DXVec3Dot(&Nor,&CameraDir);
					if(Length<=1000)
						break;

					GraphicsStruct::_BoundingBox Box;

					pObject->m_BBox.Project(BoxPos,&Viewport,&Projection,&View,&pObject->m_Matrix);

					Center = (BoxPos[BOX_A1] + BoxPos[BOX_C2]) * 0.5f;


					D3DXComputeBoundingBox(BoxPos,8,sizeof(D3DXVECTOR3),
						&Box.PositionA,&Box.PositionB);

					if(Box.PositionA.z<0)
						break;

					ModelUI::FRECT ObjectRect;

					ObjectRect.Left = Box.PositionA.x;
					ObjectRect.Width = Box.PositionB.x - Box.PositionA.x;
					ObjectRect.Bottom = Box.PositionA.y;
					ObjectRect.Height = Box.PositionB.y - Box.PositionA.y;

					if(SelectRect.IsIntersect(ObjectRect))
					{
						if(m_dwSelectState==1)
						{
							RemoveSelectedObject(pObject);
						}
						else
						{
							AddSelectedObject(pObject);
						}
					}
				}
				break;
			case SCENEENTITY_GROUP:
				{
					KSceneSceneEdit* pEdit = (KSceneSceneEdit*) Entity.RefData;
					DWORD Index = (DWORD) Entity.Pointer;
					KSceneEntityGroup* pGroup = &pEdit->m_vecEntityGroup[Index] ;

					D3DXVECTOR3 BoxPos[8];
					D3DXVECTOR3 Intersect;
					D3DXVECTOR3 Center = pGroup->m_BBox.GetCenter();
					D3DXVECTOR3 Nor = Center - CameraPos;

					float Length = D3DXVec3Dot(&Nor,&CameraDir);
					if(Length<=1000)
						break;

					GraphicsStruct::_BoundingBox Box;

					D3DXMATRIX Mat;
					D3DXMatrixIdentity(&Mat);

					pGroup->m_BBox.Project(BoxPos,&Viewport,&Projection,&View,
						&Mat);

					Center = (BoxPos[BOX_A1] + BoxPos[BOX_C2]) * 0.5f;


					D3DXComputeBoundingBox(BoxPos,8,sizeof(D3DXVECTOR3),
						&Box.PositionA,&Box.PositionB);

					if(Box.PositionA.z<0)
						break;

					ModelUI::FRECT ObjectRect;

					ObjectRect.Left = Box.PositionA.x;
					ObjectRect.Width = Box.PositionB.x - Box.PositionA.x;
					ObjectRect.Bottom = Box.PositionA.y;
					ObjectRect.Height = Box.PositionB.y - Box.PositionA.y;

					if(SelectRect.IsIntersect(ObjectRect))
					{
						if(m_dwSelectState==1)
						{
							RemoveSelectedEntity(Entity);
						}
						else
						{
							m_SelectedList.push_back(Entity);
						}
					}
				}
				break;
			case SCENEENTITY_MODEL:
				{
					LPMODEL pMesh = (LPMODEL) Entity.Pointer;
					D3DXVECTOR3 BoxPos[8];
					D3DXVECTOR3 Intersect;
					D3DXVECTOR3 Center = (pMesh->m_BBox_A+pMesh->m_BBox_B)*0.5f;

					GraphicsStruct::_BoundingBox Box;
					Box.PositionA = pMesh->m_BBox_A;
					Box.PositionB = pMesh->m_BBox_B;

					BoundingBox_Project(BoxPos,&Box,&Viewport,&Projection,&View,&pMesh->m_Matrix);
					D3DXComputeBoundingBox(BoxPos,8,sizeof(D3DXVECTOR3),
						&Box.PositionA,&Box.PositionB);
					if (Box.PositionA.z<=-20)
						break;
					//if (IsLineIntersectBox(&Intersect,&Orig,&A,BoxPos))
					D3DXVECTOR2  TestPos[4];
					TestPos[0] = D3DXVECTOR2(A.x,A.y);
					TestPos[1] = D3DXVECTOR2(A.x,C.y);
					TestPos[2] = D3DXVECTOR2(C.x,C.y);
					TestPos[3] = D3DXVECTOR2(C.x,A.y);
					for(int s=0;s<4;s++)
					{
						if ((TestPos[s].x>=Box.PositionA.x)&&(TestPos[s].x<=Box.PositionB.x)&&
							(TestPos[s].y>=Box.PositionA.y)&&(TestPos[s].y<=Box.PositionB.y))
						{
							AddSelectedMesh(pMesh);
							break;
						}
					}
					if (s<4)
						break;
					TestPos[0] = D3DXVECTOR2(Box.PositionA.x,Box.PositionA.y);
					TestPos[1] = D3DXVECTOR2(Box.PositionA.x,Box.PositionB.y);
					TestPos[2] = D3DXVECTOR2(Box.PositionB.x,Box.PositionB.y);
					TestPos[3] = D3DXVECTOR2(Box.PositionB.x,Box.PositionA.y);

					for( s=0;s<4;s++)
					{
						if ((TestPos[s].x>=A.x)&&(TestPos[s].x<=C.x)&&
							(TestPos[s].y>=A.y)&&(TestPos[s].y<=C.y))
						{
							AddSelectedMesh(pMesh);
							break;;
						}
					}
					if (s<4)
						break;
					if ((Box.PositionA.y<A.y)&&(Box.PositionB.y>C.y))
					{
						if ((Box.PositionA.x>A.x)&&(Box.PositionB.x<C.x))
						{
							AddSelectedMesh(pMesh);
						}
					}
					else if ((Box.PositionA.x<A.x)&&(Box.PositionB.x>C.x))
					{
						if ((Box.PositionA.y>A.y)&&(Box.PositionB.y<C.y))
						{
							AddSelectedMesh(pMesh);
						}
					}

				}
				break;
			case SCENEENTITY_POSITION:
				{
					D3DXMATRIX   World;
					D3DXMatrixIdentity(&World);

					D3DXVECTOR3* pPosition = (D3DXVECTOR3*) Entity.Pointer;
					D3DXVECTOR3  ProjectedPos;
					D3DXVec3Project(&ProjectedPos,pPosition,
						&Viewport,&Projection,&View,&World);

					if ((ProjectedPos.x>=A.x)&&(ProjectedPos.x<=C.x)&&
						(ProjectedPos.y>=A.y)&&(ProjectedPos.y<=C.y)&&
						(ProjectedPos.z>0))
					{
						AddSelectedEntity(SCENEENTITY_POSITION,pPosition);
						break;;
					}
					break;
				}
			}
			i++;
		}

		OnSelectChanged();
	}

	return S_OK;
}

HRESULT KSceneEditBase::OnSelectChanged()
{
	if(m_lpDlgPropertySet->IsWindowVisible())
	{
		KSceneEntity FirstSel;
		if(SUCCEEDED(GetFirstSelectedEntity(FirstSel)))
		{
			PropertyBase::KPropertyBase* pProperty =
				FirstSel.Acquire();
			if(pProperty)
			{
				pProperty->Acquire();
				m_lpDlgPropertySet->SetPropertyBase(pProperty);
				m_lpDlgPropertySet->CreateUI();
			}
		}
		else
		{
			m_lpDlgPropertySet->SetPropertyBase(this);
			m_lpDlgPropertySet->CreateUI();
		}
	}
	return S_OK;
}

HRESULT KSceneEditBase::AddSelectableObject(LPOBJECT pObject)
{
	KSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
	m_SelectableList.push_back(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::AddSelectableEntity(SCENEENTITYTYPE EntityType,PVOID pEntity)
{
	KSceneEntity Entity(EntityType,pEntity);
	m_SelectableList.push_back(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::AddSelectableMesh(LPMODEL pMesh)
{
	KSceneEntity Entity(SCENEENTITY_MODEL,pMesh);
	m_SelectableList.push_back(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::AddSelectedMesh(KModel *pMesh)
{
	KSceneEntity Entity(SCENEENTITY_MODEL,pMesh);
	m_SelectedList.push_back(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::AddSelectedEntity(SCENEENTITYTYPE EntityType,PVOID pEntity)
{
	KSceneEntity Entity(EntityType,pEntity);
	m_SelectedList.push_back(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::RemoveSelectedEntity(KSceneEntity Entity)
{
	m_SelectedList.remove(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::RemoveSelectableEntity(KSceneEntity Entity)
{
	m_SelectableList.remove(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::RemoveSelectedEntity(SCENEENTITYTYPE EntityType,PVOID pEntity)
{
	KSceneEntity Entity(EntityType,pEntity);
	m_SelectedList.remove(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::AddSelectedObject(LPOBJECT pObject)
{
	KSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
	m_SelectedList.push_back(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::CleanSelection()
{
	m_SelectedList.clear();
	return S_OK;
}
HRESULT KSceneEditBase::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{

	switch(message)
	{
	case WM_LBUTTONDOWN:
		{
			g_Mouse_L = TRUE;
			ONLBUTTONDOWN(wParam,lParam);
			break;
		}
	case WM_LBUTTONUP:
		{
			g_Mouse_L = FALSE;
			ONLBUTTONUP(wParam,lParam);
			break;
		}
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		{
			int xPos = LOWORD(lParam);
			int yPos = HIWORD(lParam);
			m_LastMousePos.x = xPos;
			m_LastMousePos.y = yPos;
			break;
		}
	case WM_RBUTTONDOWN:
		{
			ONRBUTTONDOWN(wParam,lParam);
			break;
		}
	case WM_MOUSEWHEEL:
		{
			//int fwKeys = wParam;
			ONMOUSEWHEEL(wParam,lParam);
			break;

		}
	case WM_MOUSEMOVE:
		{
			ONMOUSEMOVE(wParam,lParam);
			break;
		}
	case WM_KEYDOWN:
		{
			g_KeyState[wParam] = 1;

			ONKEYDOWN(wParam,lParam);
			break;
		}
	case WM_KEYUP:
		{
			g_KeyState[wParam] = 0;
			break;
		}

	case ID_EDIT_UNDO:
		{
			Edit_Undo();
			break;
		}
	case ID_EDIT_CUT:
		{
			Edit_Cut();
			break;
		}
	case ID_EDIT_COPY:
		{
			Edit_Copy();
			break;
		}
	case ID_EDIT_PASTE:
		{
			Edit_Paste();
			break;
		}
	}
	return S_OK;
}

HRESULT KSceneEditBase::CleanUp()
{
	SAFE_DELETE(m_lpDlgPropertySet);

	K3DScene::CleanUp();
	EmptyHistory();

	return S_OK;
}
HRESULT KSceneEditBase::Edit_Undo()
{
	if(m_vecOperation.size()==0)
		return S_OK;
	DWORD Last = m_vecOperation.size() - 1;

	KEditOperation* pOper = m_vecOperation[Last];
	//_SceneOperatorBase* pOper = (_SceneOperatorBase*)

	pOper->Undo();


	SAFE_DELETE(pOper);
	m_vecOperation.pop_back();

	TCHAR Name[256];
	wsprintf(Name,"%s\\Music\\取消.WAV",g_Def_AppDirectory);
	g_pSoundSys->PlayAmusic(Name,PlayNormal);

	return S_OK;
}

HRESULT KSceneEditBase::Edit_Copy()
{
	m_ClipBoardList = m_SelectedList;

	TCHAR Name[256];
	wsprintf(Name,"%s\\Music\\取消.WAV",g_Def_AppDirectory);
	g_pSoundSys->PlayAmusic(Name,PlayNormal);
	return S_OK;
}

HRESULT KSceneEditBase::Edit_Cut()
{
	m_ClipBoardList = m_SelectedList;
	m_SelectedListBack = m_SelectedList;
	_SceneOperatorBase* pOper = new _SceneOperatorBase(OPERATION_CUT);
	m_vecOperation.push_back(pOper);

	pOper->BackupData(this);

	K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();
	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		RemoveRenderEntity(Entity);
		RemoveSelectableEntity(Entity);

		i++;
	}
	m_SelectedList.clear();
	return S_OK;
}

HRESULT KSceneEditBase::Edit_Paste()
{
	m_SelectedList.clear();
	K3DScene::ENTITYLIST::iterator i = m_ClipBoardList.begin();
	while(i!=m_ClipBoardList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				LPOBJECT pNewObject = NULL;
				if(SUCCEEDED(pObject->Copy(&pNewObject)))
				{
					//pNewObject->m_Position += D3DXVECTOR3(100,0,100);
					AddRenderEntity(SCENEENTITY_OBJECT,pNewObject);
					AddSelectableEntity(SCENEENTITY_OBJECT,pNewObject);
					AddSelectedEntity(SCENEENTITY_OBJECT,pNewObject);
				}
				break;
			}
		}
		i++;
	}
	m_dwState = EDITSTATE_MOVE;
	return S_OK;
}

HRESULT KSceneEditBase::RecordLastOperation_Select()
{
	ClearLastOperationList();
	K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();
	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				AddLastOperation_Select(pObject);
				break;
			}
		}
		i++;
	}
	m_LastOperation = OPERATION_SELECT;
	return S_OK;
}

HRESULT KSceneEditBase::AddLastOperation_Select(LPOBJECT pObject)
{
	KSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
	m_LastOperationList.push_back(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::ClearLastOperationList()
{
	K3DScene::ENTITYLIST::iterator i = m_LastOperationList.begin();
	m_LastOperationList.clear();
	return S_OK;
}

void KSceneEditBase::ONKEYDOWN(WPARAM wParam, LPARAM lParam)
{
	if(!m_bEditing)
		return ;

	GraphicsStruct::KCamera* pCamera = &g_GraphiceEngine.m_cCamera;

	D3DXVECTOR3 Front,Up,Left;
	Front = pCamera->ViewPoint - pCamera->Position;

	D3DXVec3Normalize(&Front,&Front);
	Up = pCamera->DefUp;
	D3DXVec3Cross(&Left,&Up,&Front);
	D3DXVec3Cross(&Up,&Front,&Left);

	float S_Speed = 25.0f;

	switch(wParam)
	{
	case 'Q':
		m_dwState = EDITSTATE_SELECT;
		break;
	case 'W':
		m_bShowSelectionRect = FALSE;
		m_dwState = EDITSTATE_MOVE;
		pCamera->Position  += Front*S_Speed;
		pCamera->ViewPoint += Front*S_Speed;
		break;
	case 'E':
		m_bShowSelectionRect = FALSE;
		m_dwState = EDITSTATE_ROTATE;
		break;
	case 'R':
		m_bShowSelectionRect = FALSE;
		m_dwState = EDITSTATE_SCALE;
		break;
	case 'H':
		pCamera->Position  = D3DXVECTOR3(500,500,-500);
		pCamera->ViewPoint = D3DXVECTOR3(0,0,0);
		break;
	case VK_PAUSE:
		if(m_TimeControl.m_fSpeed>0)
		{
			m_TimeControl.Pause();
		}
		else
		{
			m_TimeControl.Restore();
		}
		break;
	}

	switch(m_dwState)
	{
	case EDITSTATE_MOVE:
		{
			switch(wParam)
			{
			case VK_LEFT:
				{
					D3DXVECTOR3 Pos(10,0,0);
					for_each(m_SelectedList.begin(),m_SelectedList.end(),
						bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));
					OnEditMoving();
					break;
				}
			case VK_RIGHT:
				{
					D3DXVECTOR3 Pos(-10,0,0);
					for_each(m_SelectedList.begin(),m_SelectedList.end(),
						bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));
					OnEditMoving();
					break;
				}
			case VK_UP:
				{
					D3DXVECTOR3 Pos(0,0,-10);
					for_each(m_SelectedList.begin(),m_SelectedList.end(),
						bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));
					OnEditMoving();
					break;
				}
			case VK_DOWN:
				{
					D3DXVECTOR3 Pos(0,0,10);
					for_each(m_SelectedList.begin(),m_SelectedList.end(),
						bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));
					OnEditMoving();
					break;
				}
			}
			break;
		}
	case EDITSTATE_ROTATE:
		{
			switch(wParam)
			{
			case VK_LEFT:
				{
					D3DXVECTOR3 Pos(D3DX_PI/18,0,0);
					for_each(m_SelectedList.begin(),m_SelectedList.end(),
						bind2nd(mem_fun_ref(&KSceneEntity::Rotation),Pos));
					OnEditRotating();
					break;
				}
			case VK_RIGHT:
				{
					D3DXVECTOR3 Pos(-D3DX_PI/18,0,0);
					for_each(m_SelectedList.begin(),m_SelectedList.end(),
						bind2nd(mem_fun_ref(&KSceneEntity::Rotation),Pos));
					OnEditRotating();
					break;
				}
			}
			break;
		}
	case EDITSTATE_SCALE:
		{
			switch(wParam)
			{
			case VK_UP:
				{
					D3DXVECTOR3 Pos(1.1f,1.1f,1.1f);
					for_each(m_SelectedList.begin(),m_SelectedList.end(),
						bind2nd(mem_fun_ref(&KSceneEntity::Scale),Pos));
					OnEditScaling();
					break;
				}
			case VK_DOWN:
				{
					D3DXVECTOR3 Pos(0.9f,0.9f,0.9f);
					for_each(m_SelectedList.begin(),m_SelectedList.end(),
						bind2nd(mem_fun_ref(&KSceneEntity::Scale),Pos));
					OnEditScaling();
					break;
				}
			}
			break;
		}
	default:
		{
			switch(wParam)
			{
			case VK_DELETE:
				{
					Edit_DeleteSelectedObject();
					break;
				}
			case VK_UP:
				{
					D3DXVECTOR3 Dir = Front;
					Dir.y = 0;
					D3DXVec3Normalize(&Dir,&Dir);
					pCamera->Position  += Dir*S_Speed;
					pCamera->ViewPoint += Dir*S_Speed;
					break;
				}
			case VK_DOWN:
				{
					D3DXVECTOR3 Dir = Front;
					Dir.y = 0;
					D3DXVec3Normalize(&Dir,&Dir);
					pCamera->Position  -= Dir*S_Speed;
					pCamera->ViewPoint -= Dir*S_Speed;
					break;
				}
			case VK_RIGHT:
				{
					D3DXVECTOR3 Dir = Left;
					Dir.y = 0;
					D3DXVec3Normalize(&Dir,&Dir);
					pCamera->Position  += Dir*S_Speed;
					pCamera->ViewPoint += Dir*S_Speed;
					break;
				}
			case VK_LEFT:
				{
					D3DXVECTOR3 Dir = Left;
					Dir.y = 0;
					D3DXVec3Normalize(&Dir,&Dir);
					pCamera->Position  -= Dir*S_Speed;
					pCamera->ViewPoint -= Dir*S_Speed;
					break;
				}

			case VK_F5:
				{
					ShowProperty(TRUE);
					//////////////////////////////////////////////////////////////////////////
					//TEST
					//DWORD* pColor = new DWORD;
					//*pColor = 0x00ffffff;//should be deleted
					//////////////////////////////////////////////////////////////////////////
					//KTimeLine4Editor<float, float>* m_pTimeLine = new KTimeLine4Editor<float, float>;

					//KTimeLine4Editor<float, float>* m_pTimeLine1 = new KTimeLine4Editor<float, float>;
					////m_pTimeLine->AddFrame(0.0f, 0x7fff0000);
					////m_pTimeLine->AddFrame(1.0f, 0x7f00ff00);
					//
					////KColorBarDialog* pColorDlg = new KColorBarDialog(m_pTimeLine, AfxGetMainWnd());
					//KTimeLineEditorDialog* pDlg = new KTimeLineEditorDialog();
					//pDlg->AddTimeLine("", m_pTimeLine);
					//pDlg->AddTimeLine("", m_pTimeLine1);
					break;
				}
			}
			break;
		}
	}

	lParam = 0;
}

HRESULT KSceneEditBase::ShowProperty(BOOL bShow)
{
	m_lpDlgPropertySet->ShowWindow(bShow);

	return S_OK;
}
void BackupEntityData(KSceneEntity& Entity)
{
	switch(Entity.Type)
	{
	case SCENEENTITY_OBJECT:
		{
			LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
			pObject->BackupData();
			break;
		}
	}
}
void KSceneEditBase::ONLBUTTONUP(WPARAM wParam, LPARAM lParam)
{
	if(!m_bEditing)
		return ;

	switch(m_dwState)
	{
	case EDITSTATE_SELECT:
	case EDITSTATE_MOVE:
	case EDITSTATE_ROTATE:
	case EDITSTATE_SCALE:
		{
			if (m_bShowSelectionRect)
			{
				GetCursorPos( &m_PointSelectEnd );
				m_bShowSelectionRect = FALSE;
				RecordLastOperation_Select();

				m_SelectedListBack = m_SelectedList;

				_SceneOperatorBase* pOper = new _SceneOperatorBase(OPERATION_SELECT);
				m_vecOperation.push_back(pOper);

				pOper->BackupData(this);

				TCHAR Name[256];
				wsprintf(Name,"%s\\Music\\BUTTON8.WAV",g_Def_AppDirectory);
				g_pSoundSys->PlayAmusic(Name,PlayNormal);

				Edit_Select();
			}

			if ((EDITSTATE_MOVE == m_dwState )&&(m_dwSubState))
			{
				if(m_SelectedList.size()>0)
				{
					m_SelectedListBack = m_SelectedList;

					_SceneOperatorBase* pOper = new _SceneOperatorBase(OPERATION_MOVE);
					m_vecOperation.push_back(pOper);
					pOper->BackupData(this);

					TCHAR Name[256];
					wsprintf(Name,"%s\\Music\\BUTTON8.WAV",g_Def_AppDirectory);
					g_pSoundSys->PlayAmusic(Name,PlayNormal);
				}
			}

			if ((EDITSTATE_ROTATE == m_dwState )&&(m_dwSubState))
			{
				if(m_SelectedList.size()>0)
				{
					m_SelectedListBack = m_SelectedList;

					_SceneOperatorBase* pOper = new _SceneOperatorBase(OPERATION_ROTATE);
					m_vecOperation.push_back(pOper);
					pOper->BackupData(this);

					TCHAR Name[256];
					wsprintf(Name,"%s\\Music\\BUTTON8.WAV",g_Def_AppDirectory);
					g_pSoundSys->PlayAmusic(Name,PlayNormal);
				}
			}

			if ((EDITSTATE_SCALE== m_dwState )&&(m_dwSubState))
			{
				if(m_SelectedList.size()>0)
				{
					m_SelectedListBack = m_SelectedList;

					_SceneOperatorBase* pOper = new _SceneOperatorBase(OPERATION_ROTATE);
					m_vecOperation.push_back(pOper);
					pOper->BackupData(this);

					TCHAR Name[256];
					wsprintf(Name,"%s\\Music\\BUTTON8.WAV",g_Def_AppDirectory);
					g_pSoundSys->PlayAmusic(Name,PlayNormal);
				}
			}

			m_dwSubState = 0;
			break;
		}
	};
}

void KSceneEditBase::ONRBUTTONDOWN(WPARAM wParam, LPARAM lParam)
{
	if(!m_bEditing)
		return ;
}

void KSceneEditBase::ONLBUTTONDOWN(WPARAM wParam, LPARAM lParam)
{
	if(!m_bEditing)
		return ;

	switch(m_dwState)
	{
	case EDITSTATE_SCALE:
	case EDITSTATE_MOVE:
		{
			GetCursorPos( &m_PointSelectStart );
			GetCursorPos( &m_PointSelectEnd );
			m_bShowSelectionRect = TRUE;
			m_dwSubState = 0;
			if (m_lpMoveGrid->m_bSelectedX)
				m_dwSubState |= MOVEAXIS_WORLD_X;
			if (m_lpMoveGrid->m_bSelectedY)
				m_dwSubState |= MOVEAXIS_WORLD_Y;
			if (m_lpMoveGrid->m_bSelectedZ)
				m_dwSubState |= MOVEAXIS_WORLD_Z;
			if (m_lpMoveGrid->m_bSelectedXYZ)
				m_dwSubState |= MOVEAXIS_WORLD_XYZ;
			if (m_lpMoveGrid->m_bSelectedYZ)
				m_dwSubState |= MOVEAXIS_WORLD_YZ;
			if (m_lpMoveGrid->m_bSelectedYX)
				m_dwSubState |= MOVEAXIS_WORLD_YX;
			if (m_lpMoveGrid->m_bSelectedXZ)
				m_dwSubState |= MOVEAXIS_WORLD_XZ;

			if (m_dwSubState)
			{
				if(m_SelectedList.size()>0)
				{
					m_SelectedListBack = m_SelectedList;
					for_each(m_SelectedList.begin(),m_SelectedList.end(),::BackupEntityData);
				}
			}

			break;
		}
	case EDITSTATE_ROTATE:
		{
			GetCursorPos( &m_PointSelectStart );
			GetCursorPos( &m_PointSelectEnd );
			m_bShowSelectionRect = TRUE;
			m_dwSubState = 0;
			if (m_lpRotateGrid->m_bSelectedYaw)
				m_dwSubState |= MOVEAXIS_WORLD_X;
			if (m_lpRotateGrid->m_bSelectedPitch)
				m_dwSubState |= MOVEAXIS_WORLD_Y;
			if (m_lpRotateGrid->m_bSelectedRoll)
				m_dwSubState |= MOVEAXIS_WORLD_Z;

			if (m_dwSubState)
			{
				if(m_SelectedList.size()>0)
				{
					m_SelectedListBack = m_SelectedList;
					for_each(m_SelectedList.begin(),m_SelectedList.end(),::BackupEntityData);
				}
			}

			break;
		}
	case EDITSTATE_SELECT:
		{
			GetCursorPos( &m_PointSelectStart );
			GetCursorPos( &m_PointSelectEnd );
			m_bShowSelectionRect = TRUE;

			m_dwSubState = 0;

			TCHAR Name[256];
			wsprintf(Name,"%s\\Music\\BUTTON7.WAV",g_Def_AppDirectory);
			g_pSoundSys->PlayAmusic(Name,PlayNormal);

			break;
		}
	}

	wParam = 0;
	lParam = 0;
}


void EntityMove(KSceneEntity& Entity)
{
	;
}

void KSceneEditBase::ONMOUSEWHEEL(WPARAM wParam, LPARAM lParam)
{
	if(!m_bEditing)
		return ;

	GraphicsStruct::KCamera* pCamera = &g_GraphiceEngine.m_cCamera;

	D3DXVECTOR3 Front,Up,Left;
	Front = pCamera->ViewPoint - pCamera->Position;
	float Radius = D3DXVec3Length(&Front);
	D3DXVec3Normalize(&Front,&Front);
	Up = pCamera->DefUp;
	D3DXVec3Cross(&Left,&Up,&Front);
	D3DXVec3Cross(&Up,&Front,&Left);
	int k = 0;
	k |=wParam;
	float S = 0;
	if (k<0)
		S = -0.1f;
	else
		S = 0.1f;
	pCamera->Position += S * Front * Radius;
	pCamera->SetCamera();
}

void KSceneEditBase::ONMOUSEMOVE(WPARAM wParam, LPARAM lParam)
{
	if(!m_bEditing)
		return ;

	if (m_bShowSelectionRect)
		GetCursorPos( &m_PointSelectEnd );

	int fwKeys = wParam;
	int xPos = LOWORD(lParam);
	int yPos = HIWORD(lParam);

	GraphicsStruct::KCamera* pCamera = &g_GraphiceEngine.m_cCamera;

	D3DXVECTOR3 Front,Up,Left;
	Front = pCamera->ViewPoint - pCamera->Position;
	float Radius = D3DXVec3Length(&Front);
	D3DXVec3Normalize(&Front,&Front);
	Up = pCamera->DefUp;
	D3DXVec3Cross(&Left,&Up,&Front);
	D3DXVec3Cross(&Up,&Front,&Left);

	float X = 0.002f*Radius*(xPos-m_LastMousePos.x);
	float Y = 0.002f*Radius*(yPos-m_LastMousePos.y);


	if ((fwKeys&MK_RBUTTON)&&(!GetAsyncKeyState(VK_LMENU )))
	{
		m_bShowSelectionRect = FALSE;

		D3DXVECTOR3 m_Left  = - Left;
		D3DXVECTOR3 m_Front = Front;
		m_Left.y  = 0;
		m_Front.y = 0;
		D3DXVec3Normalize(&m_Left,&m_Left);
		D3DXVec3Normalize(&m_Front,&m_Front);

		pCamera->Position  += X*m_Left + Y*m_Front;
		pCamera->ViewPoint += X*m_Left + Y*m_Front;
	}
	else if ((fwKeys&MK_RBUTTON)&&(GetAsyncKeyState(VK_LMENU )))
	{
		if (SUCCEEDED(GetSelectCenter(&m_SelectedCenter)))
		{
			D3DXPLANE  WaterLevel;
			//D3DXVECTOR3 Normal(Front.x,0,Front.z);
			D3DXPlaneFromPointNormal(&WaterLevel,&m_SelectedCenter,&Front);
			D3DXVECTOR3 Inter;
			D3DXVECTOR3 B = pCamera->Position + Front;

			if(D3DXPlaneIntersectLine(&Inter,&WaterLevel,&pCamera->Position,&B))
			{
				pCamera->ViewPoint += (Inter - pCamera->ViewPoint) * 0.1F;
			}

		}

		D3DXVECTOR3 M = -X*Left+Y*Up + pCamera->Position;
		D3DXVECTOR3 N = M - pCamera->ViewPoint;
		D3DXVec3Normalize(&N,&N);
		M = Radius*N;
		pCamera->Position = M + pCamera->ViewPoint;
	}
	/*else if ((fwKeys&MK_LBUTTON)&&(GetAsyncKeyState(VK_LMENU)))
	{
		m_bShowSelectionRect = FALSE;
		pCamera->Position -= Y * Front * Radius *0.001f ;
		pCamera->SetCamera();
	}*/
	else if (fwKeys&MK_LBUTTON)
	{
		switch(m_dwState)
		{
		case EDITSTATE_SELECT:
			{
				break;
			}
		case EDITSTATE_MOVE:
			{
				switch(m_dwSubState)
				{
				case MOVEAXIS_WORLD_X:
					{
						D3DXVECTOR3 Axis(0,0,1);

						float Screen_X = D3DXVec3Dot(&Axis,&Left);
						float Screen_Y = D3DXVec3Dot(&Axis,&Up);

						D3DXVECTOR2 ScreenVec(Screen_X,Screen_Y);
						D3DXVec2Normalize(&ScreenVec,&ScreenVec);

						float Length = Screen_X*X - Screen_Y*Y;

						D3DXVECTOR3 Pos = Length * Axis * Radius * 0.0004f;

						if(m_bShowSelectionRect)
						{
							m_bShowSelectionRect = FALSE;
							TCHAR Name[256];
							wsprintf(Name,"%s\\Music\\BUTTON7.WAV",g_Def_AppDirectory);
							g_pSoundSys->PlayAmusic(Name,PlayNormal);

							for_each(m_SelectedList.begin(),m_SelectedList.end(),BackupEntityData);

						}

						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));

						OnEditMoving();
						break;
					}
				case MOVEAXIS_WORLD_Y:
					{
						D3DXVECTOR3 Axis(0,1,0);
						float Screen_X = D3DXVec3Dot(&Axis,&Left);
						float Screen_Y = D3DXVec3Dot(&Axis,&Up);

						D3DXVECTOR2 ScreenVec(Screen_X,Screen_Y);
						D3DXVec2Normalize(&ScreenVec,&ScreenVec);

						float Length = Screen_X*X - Screen_Y*Y;

						D3DXVECTOR3 Pos = Length * Axis * Radius * 0.0004f;
						if(m_bShowSelectionRect)
						{
							m_bShowSelectionRect = FALSE;
							TCHAR Name[256];
							wsprintf(Name,"%s\\Music\\BUTTON7.WAV",g_Def_AppDirectory);
							g_pSoundSys->PlayAmusic(Name,PlayNormal);

							for_each(m_SelectedList.begin(),m_SelectedList.end(),BackupEntityData);

						}

						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));

						OnEditMoving();
						break;
					}
				case MOVEAXIS_WORLD_Z:
					{
						D3DXVECTOR3 Axis(1,0,0);
						float Screen_X = D3DXVec3Dot(&Axis,&Left);
						float Screen_Y = D3DXVec3Dot(&Axis,&Up);

						D3DXVECTOR2 ScreenVec(Screen_X,Screen_Y);
						D3DXVec2Normalize(&ScreenVec,&ScreenVec);

						float Length = Screen_X*X - Screen_Y*Y;

						D3DXVECTOR3 Pos = Length * Axis * Radius * 0.0004f;

						if(m_bShowSelectionRect)
						{
							m_bShowSelectionRect = FALSE;
							TCHAR Name[256];
							wsprintf(Name,"%s\\Music\\BUTTON7.WAV",g_Def_AppDirectory);
							g_pSoundSys->PlayAmusic(Name,PlayNormal);

							for_each(m_SelectedList.begin(),m_SelectedList.end(),BackupEntityData);

						}

						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));

						OnEditMoving();
						break;
					}
				case MOVEAXIS_WORLD_XYZ:
					{
						D3DXVECTOR3 Pos = X * Left - Up *Y;

						if(m_bShowSelectionRect)
						{
							m_bShowSelectionRect = FALSE;
							TCHAR Name[256];
							wsprintf(Name,"%s\\Music\\BUTTON7.WAV",g_Def_AppDirectory);
							g_pSoundSys->PlayAmusic(Name,PlayNormal);

							for_each(m_SelectedList.begin(),m_SelectedList.end(),BackupEntityData);

						}

						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));

						OnEditMoving();
						break;
					}
				case MOVEAXIS_WORLD_YZ:
					{
						D3DXVECTOR3 Pos = X * Left - Up *Y;
						Pos.z = 0;

						if(m_bShowSelectionRect)
						{
							m_bShowSelectionRect = FALSE;
							TCHAR Name[256];
							wsprintf(Name,"%s\\Music\\BUTTON7.WAV",g_Def_AppDirectory);
							g_pSoundSys->PlayAmusic(Name,PlayNormal);

							for_each(m_SelectedList.begin(),m_SelectedList.end(),BackupEntityData);
						}

						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));

						OnEditMoving();
						break;
					}
				case MOVEAXIS_WORLD_YX:
					{
						D3DXVECTOR3 Pos = X * Left - Up *Y;
						Pos.x = 0;

						if(m_bShowSelectionRect)
						{
							m_bShowSelectionRect = FALSE;
							TCHAR Name[256];
							wsprintf(Name,"%s\\Music\\BUTTON7.WAV",g_Def_AppDirectory);
							g_pSoundSys->PlayAmusic(Name,PlayNormal);

							for_each(m_SelectedList.begin(),m_SelectedList.end(),BackupEntityData);
						}

						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));

						OnEditMoving();
						break;
					}
				case MOVEAXIS_WORLD_XZ:
					{
						D3DXVECTOR3 Pos = X * Left - Up *Y;
						Pos.y = 0;

						if(m_bShowSelectionRect)
						{
							m_bShowSelectionRect = FALSE;
							TCHAR Name[256];
							wsprintf(Name,"%s\\Music\\BUTTON7.WAV",g_Def_AppDirectory);
							g_pSoundSys->PlayAmusic(Name,PlayNormal);

							for_each(m_SelectedList.begin(),m_SelectedList.end(),BackupEntityData);
						}

						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));

						OnEditMoving();
						break;
					}
				}

				break;
			}
		case EDITSTATE_ROTATE:
			{
				K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();
				while(i!=m_SelectedList.end())
				{
					KSceneEntity Entity = *i;
					switch(Entity.Type)
					{
					case SCENEENTITY_OBJECT:
						{
							LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
							D3DXVECTOR3 Rot = pObject->m_Rotation;

							switch(m_dwSubState)
							{
							case MOVEAXIS_WORLD_X:
								{
									Rot.x -= X*0.001f;
									m_bShowSelectionRect = FALSE;
									break;
								}
							case MOVEAXIS_WORLD_Y:
								{
									Rot.y -= Y*0.001f;
									m_bShowSelectionRect = FALSE;
									break;
								}
							case MOVEAXIS_WORLD_Z:
								{
									Rot.z -= Y*0.001f;
									m_bShowSelectionRect = FALSE;
									break;
								}
							}
							pObject->Edit_Rotate(&Rot);
						}
						break;
					case SCENEENTITY_OBJECT_PART:
						{
							LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
							DWORD    MeshIndex = (DWORD)  Entity.RefData;
							D3DXMATRIX* pMatrix = &pObject->m_vecMeshKeeper[MeshIndex].m_Matrix;

							D3DXVECTOR3 Rot(0,0,0);

							switch(m_dwSubState)
							{
							case MOVEAXIS_WORLD_X:
								{
									Rot.x -= X*0.001f;
									m_bShowSelectionRect = FALSE;
									break;
								}
							case MOVEAXIS_WORLD_Y:
								{
									Rot.y -= Y*0.001f;
									m_bShowSelectionRect = FALSE;
									break;
								}
							case MOVEAXIS_WORLD_Z:
								{
									Rot.z -= Y*0.001f;
									m_bShowSelectionRect = FALSE;
									break;
								}
							}
							D3DXMATRIX Rotation;

							D3DXMatrixRotationYawPitchRoll(&Rotation,
								Rot.x,Rot.y,Rot.z);

							D3DXMatrixMultiply(pMatrix,pMatrix,&Rotation);
						}
						break;
					}
					i++;
				}
				OnEditRotating();
				break;
			}
		case EDITSTATE_SCALE:
			{
				switch(m_dwSubState)
				{
				case MOVEAXIS_WORLD_X:
					{
						D3DXVECTOR3 Axis(0,0,1);
						float Screen_X = D3DXVec3Dot(&Axis,&Left);
						float Screen_Y = D3DXVec3Dot(&Axis,&Up);

						D3DXVECTOR2 ScreenVec(Screen_X,Screen_Y);
						D3DXVec2Normalize(&ScreenVec,&ScreenVec);

						float Length = Screen_X*X - Screen_Y*Y;

						D3DXVECTOR3 Scale = D3DXVECTOR3(1,1,1-Length*0.01f);
						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Scale),Scale));
						OnEditScaling();
						break;
					}
				case MOVEAXIS_WORLD_Y:
					{
						D3DXVECTOR3 Axis(0,1,0);
						float Screen_X = D3DXVec3Dot(&Axis,&Left);
						float Screen_Y = D3DXVec3Dot(&Axis,&Up);

						D3DXVECTOR2 ScreenVec(Screen_X,Screen_Y);
						D3DXVec2Normalize(&ScreenVec,&ScreenVec);

						float Length = Screen_X*X - Screen_Y*Y;

						D3DXVECTOR3 Scale = D3DXVECTOR3(1,1+Length*0.01f,1);
						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Scale),Scale));
						OnEditScaling();
						break;
					}
				case MOVEAXIS_WORLD_Z:
					{
						D3DXVECTOR3 Axis(1,0,0);
						float Screen_X = D3DXVec3Dot(&Axis,&Left);
						float Screen_Y = D3DXVec3Dot(&Axis,&Up);

						D3DXVECTOR2 ScreenVec(Screen_X,Screen_Y);
						D3DXVec2Normalize(&ScreenVec,&ScreenVec);

						float Length = Screen_X*X - Screen_Y*Y;

						D3DXVECTOR3 Scale = D3DXVECTOR3(1-Length*0.01f,1,1);
						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Scale),Scale));
						OnEditScaling();
						break;
					}
				case MOVEAXIS_WORLD_YZ:
					{
						D3DXVECTOR3 Scale = D3DXVECTOR3(1-Y*0.01f,1-Y*0.01f,1);
						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Scale),Scale));
						OnEditScaling();
						break;
					}
				case MOVEAXIS_WORLD_YX:
					{
						D3DXVECTOR3 Scale = D3DXVECTOR3(1,1-Y*0.01f,1-Y*0.01f);
						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Scale),Scale));
						OnEditScaling();
						break;
					}
				case MOVEAXIS_WORLD_XZ:
					{
						D3DXVECTOR3 Scale = D3DXVECTOR3(1-Y*0.01f,1,1-Y*0.01f);
						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Scale),Scale));
						OnEditScaling();
						break;
					}
				case MOVEAXIS_WORLD_XYZ:
					{
						D3DXVECTOR3 Scale = D3DXVECTOR3(1-Y*0.01f,1-Y*0.01f,1-Y*0.01f);
						for_each(m_SelectedList.begin(),m_SelectedList.end(),bind2nd(mem_fun_ref(&KSceneEntity::Scale),Scale));
						OnEditScaling();
						break;
					}
				}
				break;
			}
		}
	}

	pCamera->SetCamera();
	m_LastMousePos.x = xPos;
	m_LastMousePos.y = yPos;
}

HRESULT KSceneEditBase::Initialize(PVOID pSceneTable)
{
	K3DScene::Initialize(pSceneTable);

	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpMoveGrid),7);
	m_lpMoveGrid->LoadMesh    ("MoveGrid");

	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpRotateGrid),8);
	m_lpRotateGrid->LoadMesh    ("RotateGrid");

	g_cTextureTable.LoadFromFile(&m_GridTextureID,"Textures\\Grid.Tga");

	//////////////////////////////////////////////////////////////////////////
	//
	Acquire();

	PropertyBase::KDlg_PropertyDialog* pDialog = new PropertyBase::KDlg_PropertyDialog();
	pDialog->SetPropertyBase(this);

	pDialog->Create(IDD_PRODLG,NULL);
	pDialog->ShowWindow(FALSE);

	m_lpDlgPropertySet = pDialog;

	return S_OK;
}

HRESULT KSceneEditBase::Edit_DeleteSelectedObject()
{
	RecordLastOperation_Delete();

	K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();

	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;

		RemoveSelectableEntity(Entity);
		RemoveRenderEntity(Entity);
		i++;
	}
	m_SelectedList.clear();
	return S_OK;
}

HRESULT KSceneEditBase::GetFirstSelectedEntity(KSceneEntity& Entity)
{
	if(m_SelectedList.size()>0)
	{
		Entity = *m_SelectedList.begin();
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT KSceneEditBase::GetSelectCenter(D3DXVECTOR3* pCenter)
{
	AABBOX  AABBox;
	D3DXVECTOR3 BoxPos[8];

	K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();
	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;

				pObject->m_BBox.TransformCoord(BoxPos,&pObject->m_Matrix);

				AABBox.AddPosition(BoxPos,8);
			}
			break;
		case SCENEENTITY_MODEL:
			{
				LPMODEL pMesh = (LPMODEL) Entity.Pointer;
				GraphicsStruct::_BoundingBox Box;
				Box.PositionA = pMesh->m_BBox_A;
				Box.PositionB = pMesh->m_BBox_B;

				BoundingBox_TransformCoord(BoxPos,&Box,&pMesh->m_Matrix);

				AABBox.AddPosition(BoxPos,8);
			}
			break;
		case SCENEENTITY_POSITION:
			{
				D3DXVECTOR3* pPos = (D3DXVECTOR3*) Entity.Pointer;

				AABBox.AddPosition(*pPos);

				break;
			}
		case SCENEENTITY_OBJECT_PART:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;

				pObject->m_BBox.TransformCoord(BoxPos,&pObject->m_Matrix);

				AABBox.AddPosition(BoxPos,8);
			}
			break;

		case SCENEENTITY_GROUP:
			{
				KSceneSceneEdit* pEdit = (KSceneSceneEdit*) Entity.RefData;
				DWORD Index = (DWORD) Entity.Pointer;
				KSceneEntityGroup* pGroup = &pEdit->m_vecEntityGroup[Index] ;

				pGroup->m_BBox.Extract(BoxPos);

				AABBox.AddPosition(BoxPos,8);
				break;
			}
		}
		i++;

	}
	if (AABBox.bInit==FALSE)
		return E_FAIL;
	(*pCenter) = AABBox.GetCenter();

	return S_OK;
}

HRESULT KSceneEditBase::RemoveSelectableObject(LPOBJECT pObject)
{
	K3DScene::ENTITYLIST::iterator i = m_SelectableList.begin();
	while(i!=m_SelectableList.end())
	{
		KSceneEntity Entity = *i;
		if (Entity.Type == SCENEENTITY_OBJECT)
		{
			if (pObject==Entity.Pointer)
			{
				m_SelectableList.remove(Entity);
				return S_OK;
			}
		}
		i++;
	}
	return S_OK;
}

HRESULT KSceneEditBase::RemoveSelectedObject(LPOBJECT pObject)
{
	KSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
	m_SelectedList.remove(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::RemoveSelectableEntity(SCENEENTITYTYPE EntityType,PVOID pEntity)
{
	KSceneEntity Entity(EntityType,pEntity);
	m_SelectableList.remove(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::RecordLastOperation_Delete()
{
	K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();

	ClearLastOperationList();

	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				AddLastOperation_Cut(pObject);
				break;
			}
		}
		i++;
	}
	m_LastOperation = OPERATION_DELETE;
	return S_OK;
}

HRESULT KSceneEditBase::RestoreLastOperation_Select()
{
	K3DScene::ENTITYLIST::iterator i = m_LastOperationList.begin();

	m_SelectedList.clear();

	while(i!=m_LastOperationList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				AddSelectedObject(pObject);
				break;
			}
		}
		i++;
	}

	m_LastOperation = OPERATION_NONE;
	ClearLastOperationList();
	return S_OK;
}

HRESULT KSceneEditBase::AddLastOperation_Cut(LPOBJECT pObject)
{
	KSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
	m_LastOperationList.push_back(Entity);
	return S_OK;
}

HRESULT KSceneEditBase::RecordLastOperation_Cut()
{
	K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();

	ClearLastOperationList();

	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				AddLastOperation_Cut(pObject);
				break;
			}
		}
		i++;
	}
	m_LastOperation = OPERATION_CUT;
	return S_OK;
}

HRESULT KSceneEditBase::RestoreLastOperation_Cut()
{
	K3DScene::ENTITYLIST::iterator i = m_LastOperationList.begin();

	m_SelectedList.clear();

	while(i!=m_LastOperationList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				AddSelectedObject(pObject);
				AddSelectableObject(pObject);
				AddRenderObject(pObject);
				break;
			}
		}
		i++;
	}

	m_LastOperation = OPERATION_NONE;
	ClearLastOperationList();
	return S_OK;
}

HRESULT KSceneEditBase::AddLastOperation_Delete(LPOBJECT pObject)
{
	KSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
	m_LastOperationList.push_back(Entity);
	return S_OK;
}


HRESULT KSceneEditBase::RestoreLastOperation_Delete()
{
	K3DScene::ENTITYLIST::iterator i = m_LastOperationList.begin();

	m_SelectedList.clear();

	while(i!=m_LastOperationList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				AddSelectedObject(pObject);
				AddSelectableObject(pObject);
				AddRenderObject(pObject);
				break;
			}
		}
		i++;
	}

	m_LastOperation = OPERATION_NONE;
	ClearLastOperationList();
	return S_OK;
}
HRESULT KSceneEditBase::Render(void)
{
	K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();
	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		DrawSelectObjectBox(&Entity);
		i++;
	}

	if (m_bShowSelectionRect)
	{
		D3DXVECTOR2 A,C;
		POINT PA = m_PointSelectStart;
		POINT PC = m_PointSelectEnd;

		A = D3DXVECTOR2(PA.x*1.0f,PA.y*1.0f);
		C = D3DXVECTOR2(PC.x*1.0f,PC.y*1.0f);
		float U = (C.x - A.x) *0.1f;
		float V = (C.y - A.y) *0.1f;

		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		g_cGraphicsTool.DrawScreenRect(&A,&C,0,U,V,0xFFFFFFFF,m_GridTextureID);
	}

	if (m_dwState == EDITSTATE_MOVE)
	{
		if (SUCCEEDED(GetSelectCenter(&m_SelectedCenter)))
		{
			D3DXMATRIX Scaling,Trans,Temp;
			D3DXVECTOR3 PO;
			g_cGraphicsTool.GetCameraPos(&PO);
			PO = PO - m_SelectedCenter;

			float S = D3DXVec3Length(&PO)*0.0006f;
			D3DXMatrixScaling(&Scaling,S,S,S);
			D3DXMatrixTranslation(&Trans,m_SelectedCenter.x,m_SelectedCenter.y,m_SelectedCenter.z);

			D3DXMatrixMultiply(&Temp,&Scaling,&Trans);
			m_lpMoveGrid->SetMatrix(&Temp);

			if (m_dwSubState==0)
			{
				KGraphicsEngine* pEng = &g_GraphiceEngine;

				D3DXVECTOR3 Orig,Dir;
				pEng->GetPickRay(&Orig,&Dir);
				m_lpMoveGrid->SelectByPickRay(&Orig,&Dir);
			}

			m_lpMoveGrid->Render();
		}
	}
	else if (m_dwState == EDITSTATE_ROTATE)
	{
		if (SUCCEEDED(GetSelectCenter(&m_SelectedCenter)))
		{
			D3DXVECTOR3 PO;
			g_cGraphicsTool.GetCameraPos(&PO);
			PO = PO - m_SelectedCenter;

			float S = D3DXVec3Length(&PO)*0.002f;
			m_lpRotateGrid->SetMatrix(S,&m_SelectedCenter);

			if (m_dwSubState==0)
			{
				KGraphicsEngine* pEng = &g_GraphiceEngine;

				D3DXVECTOR3 Orig,Dir;
				pEng->GetPickRay(&Orig,&Dir);
				m_lpRotateGrid->SelectByPickRay(&Orig,&Dir);
			}

			m_lpRotateGrid->Render();
		}
	}
	else if (m_dwState == EDITSTATE_SCALE)
	{
		if (SUCCEEDED(GetSelectCenter(&m_SelectedCenter)))
		{
			D3DXMATRIX Scaling,Trans,Temp;
			D3DXVECTOR3 PO;
			g_cGraphicsTool.GetCameraPos(&PO);
			PO = PO - m_SelectedCenter;

			float S = D3DXVec3Length(&PO)*0.0006f;
			D3DXMatrixScaling(&Scaling,S,S,S);
			D3DXMatrixTranslation(&Trans,m_SelectedCenter.x,m_SelectedCenter.y,m_SelectedCenter.z);

			D3DXMatrixMultiply(&Temp,&Scaling,&Trans);
			m_lpMoveGrid->SetMatrix(&Temp);

			if (m_dwSubState==0)
			{
				KGraphicsEngine* pEng = &g_GraphiceEngine;

				D3DXVECTOR3 Orig,Dir;
				pEng->GetPickRay(&Orig,&Dir);
				m_lpMoveGrid->SelectByPickRay(&Orig,&Dir);
			}

			m_lpMoveGrid->Render();
		}
	}

	return S_OK;
}

HRESULT KSceneEditBase::DrawSelectObjectBox(LPSCENEENTITY pObj)
{
	switch(pObj->Type)
	{
	case SCENEENTITY_OBJECT:
		{
			LPOBJECT pObject = (LPOBJECT) pObj->Pointer;
			pObject->DrawBBox();
		}
		break;
	case SCENEENTITY_MODEL:
		{
			KModel* pMesh = (KModel*) pObj->Pointer;
			pMesh->DrawBBox();
		}
		break;
	case SCENEENTITY_OBJECT_PART:
		{
			LPOBJECT pObject = (LPOBJECT) pObj->Pointer;
			DWORD    MeshIndex = (DWORD)  pObj->RefData;

			LPMODEL pModel = NULL;

			if(SUCCEEDED(pObject->GetMesh(&pModel,MeshIndex+1)))
			{
				D3DXMATRIX MatSave;
				g_pd3dDevice->GetTransform(D3DTS_WORLD,&MatSave);

				g_pd3dDevice->SetTransform(D3DTS_WORLD,&pObject->m_vecMeshKeeper[MeshIndex].m_Matrix);

				g_cGraphicsTool.DrawAABBox(&pModel->m_BBox_A,&pModel->m_BBox_B,0xFF0000FF);

				g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatSave);
			}
		}
		break;
	case SCENEENTITY_GROUP:
		{
			KSceneSceneEdit* pEdit = (KSceneSceneEdit*) pObj->RefData;
			DWORD Index = (DWORD) pObj->Pointer;
			KSceneEntityGroup* pGroup = &pEdit->m_vecEntityGroup[Index] ;

			D3DXMATRIX MatSave;
			g_pd3dDevice->GetTransform( D3DTS_WORLD, &MatSave );
			g_pd3dDevice->SetTransform( D3DTS_WORLD, &pGroup->m_Matrix );

			g_cGraphicsTool.DrawAABBox(&pGroup->m_BBox.A,&pGroup->m_BBox.B,0xFF0000FF);

			g_pd3dDevice->SetTransform( D3DTS_WORLD, &MatSave );

		}
		break;
	}
	return S_OK;

}

HRESULT KSceneEditBase::DrawSelectObjectInfo(LPSCENEENTITY pObj)
{
	switch(pObj->Type)
	{
	case SCENEENTITY_OBJECT:
		{
			LPOBJECT pObject = (LPOBJECT) pObj->Pointer;
			TCHAR str[256];
			sprintf(str,"Pos:%.1f %.1f %.1f Rot %.1f",
				pObject->m_Position.x,
				pObject->m_Position.y,
				pObject->m_Position.z,
				pObject->m_Rotation.x * 180 / D3DX_PI);

			g_cGraphicsTool.DrawText(100,250,1,1,0xFFFF00FF,16,"宋体",str,0);

		}
		break;
	case SCENEENTITY_MODEL:
		{
			KModel* pMesh = (KModel*) pObj->Pointer;
			//pMesh->DrawBBox();
		}
		break;
	}
	return S_OK;

}

HRESULT KSceneEditBase::Acquire()
{
	if (!m_bNeedReAcquire)
		return S_OK;

	PropertyBase::_PropertyPage* pPage = NULL;
	int PageIndex = -1;

	//PageIndex = this->AddPage("Information");
	/*pPage = this->FindPage(PageIndex);
	if(pPage)
	{
	PropertyBase::_PropertyGroup Group(0,"Group1",FALSE);
	pPage->AddGroup(Group);

	PropertyBase::_PropertyGroup Group2(0,"Group2",FALSE);
	pPage->AddGroup(Group2);

	PropertyBase::_PropertyGroup Group3(0,"Group3",FALSE);
	pPage->AddGroup(Group3);

	PropertyBase::_PropertyGroup Group4(0,"Group4",FALSE);
	int index_Group = pPage->AddGroup(Group4);

	PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

	pGroup->AddPropertyColor("Scene Ambient",&m_Ambient);
	/*pGroup->AddPropertyInt("Int1",(int*)(&m_bHide),0,10);
	pGroup->AddPropertyBool("Hide SFX Tool bar",&m_bHide);
	pGroup->AddPropertyBool("Show Title",&m_bShowTitle);

	pGroup->AddPropertyFloat("Tool Bar Left",&m_RelativeRect.Left,0,1.0f);

	pGroup->AddPropertyString("Mesh File Name",&m_TitleCaption);

	int    EnumIndex[3];
	LPSTR  EnumName [3];
	EnumIndex[0] = 0;
	EnumIndex[1] = 1;
	EnumIndex[2] = 2;
	EnumName [0] = "Enum 1";
	EnumName [1] = "Enum 2";
	EnumName [2] = "Enum 3";

	pGroup->AddPropertyEnum("Enum",(int*)(&m_bHide),3,EnumIndex,EnumName);

	pGroup->AddPropertyColor("Back Color",&m_BackColor);
	pGroup->AddPropertyColor("Border Color",&m_BorderColor);*/
	//}

	PageIndex = this->AddPage("光照");
	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		PropertyBase::_PropertyGroup Group(0,"场景主光源",FALSE);
		int index_Group = pPage->AddGroup(Group);
		PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

		pGroup->AddPropertyColor("Scene Ambient",&m_Ambient);

		pGroup->AddPropertyColor("Sun Ambient",&m_SunLight.Ambient);
		pGroup->AddPropertyColor("Sun Diffuse",&m_SunLight.Diffuse);
		pGroup->AddPropertyColor("Sun Specular",&m_SunLight.Specular);

		PropertyBase::_PropertyGroup GroupFog(0,"场景雾效果",FALSE);
		index_Group = pPage->AddGroup(GroupFog);
		pGroup = pPage->FindGroup(index_Group);

		pGroup->AddPropertyBool ("开启 雾 效果",&m_Fog.m_bFogEnable);
		pGroup->AddPropertyFloat("起始距离",&m_Fog.m_fFogStartValue,0,50000,1);
		pGroup->AddPropertyFloat("结束距离",&m_Fog.m_fFogEndValue,0,100000,1);
		pGroup->AddPropertyFloat("浓度",&m_Fog.m_fFogDensity,0,2,0.01f);
		pGroup->AddPropertyColor("颜色",&m_Fog.m_FogColor);

		pGroup->AddPropertyColor("光晕阀值",&m_Glow.m_GateColor);
		pGroup->AddPropertyColor("光晕叠加值",&m_Glow.m_AddColor);
	}

	//this->AddPage("Screen & Viewport");
	//this->AddPage("Setting");

	PageIndex = this->AddPage("选择");
	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		PropertyBase::_PropertyGroup Group(0,"选择",FALSE);
		int index_Group = pPage->AddGroup(Group);
		PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

		pGroup->AddPropertyColor("Scene Ambient",&m_Ambient);

		static D3DXVECTOR3 XXXX(100.0f, 100.0f, 100.0f);
		pGroup->AddPropertyPosition("fdfdfd",&XXXX);

	}

	m_bNeedReAcquire = FALSE;

	return S_OK;
}

HRESULT KSceneEditBase::OnEditMoving()
{
	return S_OK;
}

HRESULT KSceneEditBase::OnEditRotating()
{
	return S_OK;
}

HRESULT KSceneEditBase::OnEditScaling()
{
	return S_OK;
}

HRESULT KSceneEditBase::_SceneOperatorBase::Undo(KSceneEditBase* pScene)
{
	switch(m_dwOperator)
	{
	case OPERATION_SELECT:
		{
			pScene->m_SelectedList = m_EntityList;
			break;
		}
	case OPERATION_CUT:
		{
			K3DScene::ENTITYLIST::iterator i = m_EntityList.begin();
			while(i!=m_EntityList.end())
			{
				KSceneEntity Entity = *i;
				if (Entity.Type == SCENEENTITY_OBJECT)
				{
					LPOBJECT pObject = (LPOBJECT) Entity.Pointer;

					pScene->AddRenderEntity(SCENEENTITY_OBJECT,pObject);
					pScene->AddSelectableEntity(SCENEENTITY_OBJECT,pObject);
				}
				i++;
			}
			pScene->m_SelectedList = m_EntityList;
			break;
		}
	case OPERATION_MOVE:
		{
			D3DXVECTOR3* Pos = (D3DXVECTOR3*) m_pData;

			int K = 0;
			K3DScene::ENTITYLIST::iterator i = m_EntityList.begin();
			while(i!=m_EntityList.end())
			{
				KSceneEntity Entity = *i;
				if (Entity.Type == SCENEENTITY_OBJECT)
				{
					LPOBJECT pObject = (LPOBJECT) Entity.Pointer;

					pObject->Edit_Move(&Pos[K]);
				}
				i++;
				K++;
			}

			pScene->m_SelectedList = m_EntityList;
			break;
		}
	case OPERATION_ROTATE:
		{
			D3DXVECTOR3* Rot = (D3DXVECTOR3*) m_pData;

			int K = 0;
			K3DScene::ENTITYLIST::iterator i = m_EntityList.begin();
			while(i!=m_EntityList.end())
			{
				KSceneEntity Entity = *i;
				if (Entity.Type == SCENEENTITY_OBJECT)
				{
					LPOBJECT pObject = (LPOBJECT) Entity.Pointer;

					pObject->m_Rotation = Rot[K];
				}
				i++;
				K++;
			}

			pScene->m_SelectedList = m_EntityList;
			break;
		}
	}
	return S_OK;
}

HRESULT KSceneEditBase::_SceneOperatorBase::BackupData(KSceneEditBase* pScene)
{
	m_lpSceneEditBase = pScene;

	switch(m_dwOperator)
	{
	case OPERATION_SELECT:
		{
			m_EntityList = pScene->m_SelectedListBack;
			break;
		}
	case OPERATION_CUT:
		{
			m_EntityList = pScene->m_SelectedListBack;
			break;
		}
	case OPERATION_MOVE:
		{
			m_EntityList = pScene->m_SelectedListBack;
			DWORD Size = m_EntityList.size();

			D3DXVECTOR3* Pos = new D3DXVECTOR3[Size];
			ZeroMemory(Pos,sizeof(D3DXVECTOR3)*Size);

			int K = 0;
			K3DScene::ENTITYLIST::iterator i = m_EntityList.begin();
			while(i!=m_EntityList.end())
			{
				KSceneEntity Entity = *i;
				if (Entity.Type == SCENEENTITY_OBJECT)
				{
					LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
					Pos[K] = pObject->m_PositionBack;
				}
				i++;
				K++;
			}
			m_pData = Pos;
			break;
		}
	case OPERATION_ROTATE:
		{
			m_EntityList = pScene->m_SelectedListBack;
			DWORD Size = m_EntityList.size();

			D3DXVECTOR3* Rot = new D3DXVECTOR3[Size];
			ZeroMemory(Rot,sizeof(D3DXVECTOR3)*Size);

			int K = 0;
			K3DScene::ENTITYLIST::iterator i = m_EntityList.begin();
			while(i!=m_EntityList.end())
			{
				KSceneEntity Entity = *i;
				if (Entity.Type == SCENEENTITY_OBJECT)
				{
					LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
					Rot[K] = pObject->m_RotationBack;
				}
				i++;
				K++;
			}
			m_pData = Rot;
			break;
		}
	}
	return S_OK;
}