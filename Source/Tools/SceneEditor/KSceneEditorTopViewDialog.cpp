// KSceneEditorTopViewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorTopViewDialog.h"
//#include "KG3DSceneSceneEditor.h"
//#include "KG3DRepresentNPC.h"
// KSceneEditorTopViewDialog dialog
//#include "IEOther.h" //by dengzhihui Refactor 2006年11月2日
#include "IEEditor.h"
#include "IEKG3DTerrain.h"
//#include "IEKG3DSceneOutputWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KSceneEditorTopViewDialog, CDialog)

KSceneEditorTopViewDialog::KSceneEditorTopViewDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorTopViewDialog::IDD, pParent)
	, m_bEnableSelectZone(FALSE)
	, m_bEnableSelectSection(FALSE)
	, m_bEnableSelectRegion(FALSE)
	, m_bEnableOnlyExist(FALSE)
	, m_szHeightmapName(_T(""))
	, m_nHeightMapWidth(0)
	, m_fHeightLowest(0)
	, m_fHeightTotal(0)
	, m_szBaseTexture(_T(""))
	, m_szHeightColor(_T(""))
{
	m_lpSceneEditor = NULL;
	m_nOutputWndID = -1;
	m_dwSelectionPoint = 0;
	//m_bSelecting = FALSE;
	//m_bAddPolyVertex = FALSE;
	m_eState = STATE_NONE;
	m_nRegionXStart = 0;
	m_nRegionZStart = 0;
	m_nCountRegionX = 0;
	m_nCountRegionZ = 0;
}

KSceneEditorTopViewDialog::~KSceneEditorTopViewDialog()
{
}

void KSceneEditorTopViewDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1,m_Button_View);
	DDX_Check(pDX, IDC_CHECK_ZONE, m_bEnableSelectZone);
	DDX_Check(pDX, IDC_CHECK_SECTION, m_bEnableSelectSection);
	DDX_Check(pDX, IDC_CHECK_REGION, m_bEnableSelectRegion);
	DDX_Check(pDX, IDC_CHECK_EXISTONLY, m_bEnableOnlyExist);
	DDX_Text(pDX, IDC_EDIT_HEIGHTMAP, m_szHeightmapName);
	DDX_Text(pDX, IDC_EDIT_HEIGHTWIDTH, m_nHeightMapWidth);
	DDX_Text(pDX, IDC_EDIT_LOW, m_fHeightLowest);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_fHeightTotal);
	DDX_Text(pDX, IDC_EDIT_BASEMAP, m_szBaseTexture);
	DDX_Text(pDX, IDC_EDIT_HEIGTCOLOR, m_szHeightColor);
	DDX_Text(pDX, IDC_EDIT_LRSTARTX, m_nRegionXStart);
	DDX_Text(pDX, IDC_EDIT_LRSTARTZ, m_nRegionZStart);
	DDX_Text(pDX, IDC_EDIT_LRCOUNTX, m_nCountRegionX);
	DDX_Text(pDX, IDC_EDIT_LRCOUNTZ, m_nCountRegionZ);
}


BEGIN_MESSAGE_MAP(KSceneEditorTopViewDialog, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_MBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_TOPVIEW_GROUND, &KSceneEditorTopViewDialog::OnTopviewGround)
	ON_UPDATE_COMMAND_UI(ID_TOPVIEW_GROUND, &KSceneEditorTopViewDialog::OnUpdateTopviewGround)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CLOSE()
	ON_COMMAND(ID_TOPVIEW_ENTITYFAR, &KSceneEditorTopViewDialog::OnTopviewEntityfar)
	ON_COMMAND(ID_TOPVIEW_WATER, &KSceneEditorTopViewDialog::OnTopviewWater)
	ON_COMMAND(ID_TOPVIEWLIGHTMAP, &KSceneEditorTopViewDialog::OnTopviewlightmap)
	ON_COMMAND(ID_TOPVIEW_GROUNDGIRD, &KSceneEditorTopViewDialog::OnTopviewGroundgird)
	ON_COMMAND(ID_REGIONSELECTION, &KSceneEditorTopViewDialog::OnTopviewRegionSelection)
	ON_COMMAND(ID_NEWPOLY, &KSceneEditorTopViewDialog::OnNewpoly)
	ON_COMMAND(ID_ENDADDPOLY, &KSceneEditorTopViewDialog::OnEndaddpoly)
	ON_WM_INITMENUPOPUP()
	ON_UPDATE_COMMAND_UI(ID_TOPVIEWLIGHTMAP, &KSceneEditorTopViewDialog::OnUpdateTopviewlightmap)
	ON_UPDATE_COMMAND_UI(ID_TOPVIEW_GROUNDGIRD, &KSceneEditorTopViewDialog::OnUpdateTopviewGroundgird)
	ON_UPDATE_COMMAND_UI(ID_TOPVIEW_WATER, &KSceneEditorTopViewDialog::OnUpdateTopviewWater)
	ON_UPDATE_COMMAND_UI(ID_TOPVIEW_ENTITYFAR, &KSceneEditorTopViewDialog::OnUpdateTopviewEntityfar)
	ON_COMMAND(ID_FOCELOADALL, &KSceneEditorTopViewDialog::OnFoceloadall)
    ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_CHECK_ZONE, &KSceneEditorTopViewDialog::OnBnClickedCheckZone)
	ON_BN_CLICKED(IDC_CHECK_SECTION, &KSceneEditorTopViewDialog::OnBnClickedCheckSection)
	ON_BN_CLICKED(IDC_CHECK_REGION, &KSceneEditorTopViewDialog::OnBnClickedCheckRegion)
	ON_BN_CLICKED(IDC_CHECK_EXISTONLY, &KSceneEditorTopViewDialog::OnBnClickedCheckExistonly)
	ON_BN_CLICKED(IDC_BUTTON_BLOCKSELECT, &KSceneEditorTopViewDialog::OnBnClickedButtonBlockselect)
	ON_BN_CLICKED(IDC_BUTTON_BLOCKCREATE, &KSceneEditorTopViewDialog::OnBnClickedButtonBlockcreate)
	ON_BN_CLICKED(IDC_BUTTON_BLOCKDELETE, &KSceneEditorTopViewDialog::OnBnClickedButtonBlockdelete)
	ON_BN_CLICKED(IDC_BUTTON_TERRAINCREATE, &KSceneEditorTopViewDialog::OnBnClickedButtonTerraincreate)
	ON_BN_CLICKED(IDC_BUTTON_TERRAINDELETE, &KSceneEditorTopViewDialog::OnBnClickedButtonTerraindelete)
	ON_EN_CHANGE(IDC_EDIT_HEIGHTMAP, &KSceneEditorTopViewDialog::OnEnChangeEditHeightmap)
	ON_EN_CHANGE(IDC_EDIT_HEIGHTWIDTH, &KSceneEditorTopViewDialog::OnEnChangeEditHeightwidth)
	ON_EN_CHANGE(IDC_EDIT_LOW, &KSceneEditorTopViewDialog::OnEnChangeEditLow)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, &KSceneEditorTopViewDialog::OnEnChangeEditHeight)
	ON_BN_CLICKED(IDC_BUTTON_APPLYHEIGHTMAP, &KSceneEditorTopViewDialog::OnBnClickedButtonApplyheightmap)
	ON_BN_CLICKED(IDC_BUTTON_APPLYTEXT, &KSceneEditorTopViewDialog::OnBnClickedButtonApplytext)
	ON_BN_CLICKED(IDC_BUTTON_EXPORTHEIGHT, &KSceneEditorTopViewDialog::OnBnClickedButtonExportheight)
	ON_BN_CLICKED(IDC_BUTTON_IMPORTHEIGHT, &KSceneEditorTopViewDialog::OnBnClickedButtonImportheight)
	ON_BN_CLICKED(IDC_CHECK_SHOWAPPOINTLOGICALREGION, &KSceneEditorTopViewDialog::OnBnClickedCheckShowappointlogicalregion)
	ON_EN_CHANGE(IDC_EDIT_LRSTARTX, &KSceneEditorTopViewDialog::OnEnChangeEditLrstartx)
	ON_EN_CHANGE(IDC_EDIT_LRSTARTZ, &KSceneEditorTopViewDialog::OnEnChangeEditLrstartz)
	ON_EN_CHANGE(IDC_EDIT_LRCOUNTX, &KSceneEditorTopViewDialog::OnEnChangeEditLrcountx)
	ON_EN_CHANGE(IDC_EDIT_LRCOUNTZ, &KSceneEditorTopViewDialog::OnEnChangeEditLrcountz)
	ON_BN_CLICKED(IDC_BUTTON_SELECTLOGICREGION, &KSceneEditorTopViewDialog::OnBnClickedSelectLogicRegion)
	ON_BN_CLICKED(IDC_CHECK_BOX, &KSceneEditorTopViewDialog::OnBnClickedCheckBox)
END_MESSAGE_MAP()


// KSceneEditorTopViewDialog message handlers


BOOL KSceneEditorTopViewDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	
	//IEKG3DSceneSceneEditor* pScene = dynamic_cast<IEKG3DSceneSceneEditor*>(g_pGraphicsTool->GetScene());//KSH::GetEngineInterfaceEx<IEKG3DSceneSceneEditor>();//
    IEKG3DSceneEditorBase* pEditorBase = NULL;
	if (!m_lpSceneEditor)
		return FALSE;
    m_lpSceneEditor->GetSceneEditorBase(&pEditorBase);

    if (pEditorBase && pEditorBase->GetRtsActive())
    {
        DWORD dwState = 0;
        m_lpSceneEditor->GetEditState(&dwState);
        switch (dwState)
        {
        case SCENESTATE_MOVE :
            SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE));
            return TRUE;
        case SCENESTATE_ROTATE :
            SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ROT));
            return TRUE;
        case SCENESTATE_SCALE :
            SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SCL));
            return TRUE;
        default :
            break;
        }
    }
    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void KSceneEditorTopViewDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(!m_lpSceneEditor)
		return;

	if (bShow && m_nOutputWndID ==-1)
	{
		m_lpSceneEditor->AddOutputWindow("TopView",m_Button_View.GetSafeHwnd(),OUTPUTWND_SCENEEDITOR_TOPVIEW,0,&m_nOutputWndID);
		IEKG3DScene* pScene = NULL;
		m_lpSceneEditor->GetScene(&pScene);
		pScene->OnResizeWindow(NULL);

		D3DXVECTOR3 CamMainPos = m_lpSceneEditor->GetCameraMainPos();
		m_lpSceneEditor->SetCameraTopPos(CamMainPos.x, CamMainPos.z);

	}
	else
	{
		m_lpSceneEditor->RemoveOutputWindow(m_nOutputWndID);
		m_nOutputWndID = -1;
	}
	{
		int nLow = -20000,nTotal = 80000;
		m_lpSceneEditor->GetSceneHeightInfo(&nLow,&nTotal);
		m_fHeightLowest = (float)nLow;
		m_fHeightTotal = (float)nTotal;
		UpdateData(FALSE);
	}
}

void KSceneEditorTopViewDialog::MousePosToScene(float& X,float& Z,CPoint point)
{
	RECT vrect,rClient;
	HWND HWnd;
	GetDlgItem(IDC_BUTTON1,&HWnd);
	//::GetWindowRect(HWnd,&vrect);
	::GetClientRect(HWnd,&vrect);
	GetClientRect(&rClient);
	float W = (float)(vrect.right - vrect.left);
	float H = (float)(vrect.bottom - vrect.top);
	float CX = 0.5F*(vrect.right + vrect.left);
	float CZ = 0.5F*(vrect.bottom + vrect.top);
	float fW = rClient.right - rClient.left - W;
	m_vCameraPos = m_lpSceneEditor->GetCameraTopPos();
	X = m_vCameraPos.x + (point.x - fW - CX ) * m_lpSceneEditor->GetCameraTopScale();
	Z = m_vCameraPos.z - (point.y - CZ ) * m_lpSceneEditor->GetCameraTopScale();
}

void KSceneEditorTopViewDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnMouseMove(nFlags, point);

	if(!m_lpSceneEditor)
		return;

	if(nFlags&MK_RBUTTON)
	{
		if(point.y<m_LastMousePos.y)
		{
			m_lpSceneEditor->SetCameraTopScale(m_lpSceneEditor->GetCameraTopScale() * 1.05F);// m_fCameraTopScale *= 1.05F;
		}
		else
		{
			//m_lpSceneEditor->m_fCameraTopScale *= 0.95F;
			//m_lpSceneEditor->m_fCameraTopScale = max(0.01F,m_lpSceneEditor->m_fCameraTopScale);
			m_lpSceneEditor->SetCameraTopScale(m_lpSceneEditor->GetCameraTopScale() * 0.95F);
			m_lpSceneEditor->SetCameraTopScale(max(0.01F,m_lpSceneEditor->GetCameraTopScale()));
		}
	}

	if(nFlags&MK_MBUTTON)
	{
		RECT vrect;
		GetClientRect(&vrect);
		float W = (float)(vrect.right - vrect.left);
		float H = (float)(vrect.bottom - vrect.top);

		//m_lpSceneEditor->m_vCameraTopPos.x = m_vCameraPos.x - (point.x - m_MouseLDown.x) * m_lpSceneEditor->m_fCameraTopScale;
		//m_lpSceneEditor->m_vCameraTopPos.z = m_vCameraPos.z + (point.y - m_MouseLDown.y) * m_lpSceneEditor->m_fCameraTopScale;
		m_lpSceneEditor->SetCameraTopPos(m_vCameraPos.x - (point.x - m_MouseLDown.x) * m_lpSceneEditor->GetCameraTopScale(),
			m_vCameraPos.z + (point.y - m_MouseLDown.y) * m_lpSceneEditor->GetCameraTopScale()
			);
	}

	if(nFlags&MK_LBUTTON)
	{
		if(m_eState == STATE_SELECTING)
		{
			float X,Z;
			D3DXVECTOR3 vPos;
			MousePosToScene(X,Z,point);
			vPos.x = X;vPos.z = Z;vPos.y = 0;

			m_lpSceneEditor->AddSelectSceneBlockPoint(1,vPos);

            if (::GetAsyncKeyState(VK_CONTROL) & 0x8000)
                m_lpSceneEditor->EndSelectSceneBlock(true, false);
            else
			    m_lpSceneEditor->EndSelectSceneBlock(false, false);
		}
	}
	m_LastMousePos = point;
}

void KSceneEditorTopViewDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

    SetFocus();

	CDialog::OnLButtonDown(nFlags, point);

	if(!m_lpSceneEditor)
		return;
	switch(m_eState)
	{
	case STATE_SELECTING:
		{
			float X,Z;
			D3DXVECTOR3 vPos;
			MousePosToScene(X,Z,point);
			vPos.x = X;vPos.z = Z;vPos.y = 0;

			m_lpSceneEditor->AddSelectSceneBlockPoint(0,vPos);
			break;
		}
	case STATE_ADDPOLYVERTEX:
		{
			float X,Z;
			D3DXVECTOR3 vPos;
			MousePosToScene(X,Z,point);
			vPos.x = X;vPos.z = Z;vPos.y = 0;
			m_lpSceneEditor->AddPolyVetexInTopView(vPos);	
			break;
		}
	case STATE_SELECTLOGICREGION:
		{
			float X,Z;
			IEKG3DScene* pScene = NULL;
			if(m_lpSceneEditor)
			{
				m_lpSceneEditor->GetScene(&pScene);
				if(pScene)
				{
					MousePosToScene(X,Z,point);
					int nX_Start,nZ_Start,nX_Count=0,nZ_Count=0;
					nX_Start = max(0,(int)(X / 3200.0f));
					nZ_Start = max(0,(int)(Z / 3200.0f));
					pScene->SetAppointLogicalRegion(nX_Start,nZ_Start,nX_Count,nZ_Count);
				}
			}
			break;
		}
	}
	
	

}

void KSceneEditorTopViewDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnLButtonUp(nFlags, point);

	if(!m_lpSceneEditor)
		return;
	switch(m_eState)
	{
	case STATE_SELECTING:
		{
			float X,Z;
			D3DXVECTOR3 vPos;
			MousePosToScene(X,Z,point);
			vPos.x = X;vPos.z = Z;vPos.y = 0;

			m_lpSceneEditor->AddSelectSceneBlockPoint(1,vPos);
			if (::GetAsyncKeyState(VK_CONTROL) & 0x8000)
				m_lpSceneEditor->EndSelectSceneBlock(true, true);
			else
				m_lpSceneEditor->EndSelectSceneBlock(false, true);
			m_dwSelectionPoint = 0;
			break;
		}
	case STATE_ADDPOLYVERTEX:
		break;
	case STATE_SELECTLOGICREGION:
		{
			float X,Z;
			IEKG3DScene* pScene = NULL;
			if(m_lpSceneEditor)
			{
				m_lpSceneEditor->GetScene(&pScene);
				if(pScene)
				{
					MousePosToScene(X,Z,point);
					int nX_End,nZ_End;
					int nX = 0,nZ = 0;
					pScene->GetAppointLogicalRegion(m_nRegionXStart,m_nRegionZStart,m_nCountRegionX,m_nCountRegionZ);
					nX = (int)(X / 3200.0f);
					nZ = (int)(Z / 3200.0f);
					nX_End = max(m_nRegionXStart,nX);
					nZ_End = max(m_nRegionZStart,nZ);
					m_nRegionXStart = min(m_nRegionXStart,nX);
					m_nRegionZStart = min(m_nRegionZStart,nZ);
					m_nCountRegionX = nX_End - m_nRegionXStart + 1;
					m_nCountRegionZ = nZ_End - m_nRegionZStart + 1;
					pScene->SetAppointLogicalRegion(m_nRegionXStart,m_nRegionZStart,m_nCountRegionX,m_nCountRegionZ);
					UpdateData(FALSE);
				}
			}
			break;
		}
	}
	
}
void KSceneEditorTopViewDialog::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnRButtonDown(nFlags, point);

	if(!m_lpSceneEditor)
		return;
	if (m_eState == STATE_ADDPOLYVERTEX)
	{
		//m_bAddPolyVertex = FALSE;
		m_lpSceneEditor->EndAddPolyVertex(0);
	}

}

void KSceneEditorTopViewDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(!m_lpSceneEditor)
		return;
	IEKG3DScene* pScene = NULL;
	m_lpSceneEditor->GetScene(&pScene);
	_ASSERTE(pScene);
	if (pScene)
		pScene->OnResizeWindow(NULL);

}

void KSceneEditorTopViewDialog::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnMButtonDown(nFlags, point);
	if(!m_lpSceneEditor)
		return;
	m_MouseLDown = point;
	m_vCameraPos = m_lpSceneEditor->GetCameraTopPos();// m_vCameraTopPos;
}

LRESULT KSceneEditorTopViewDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	if(m_lpSceneEditor)
	{
		switch(message)
		{
		case WM_MOUSEWHEEL:
			{
				int k = 0;
				k |=wParam;
				float S = 1.0F;
				if (k>0)
					S = 0.95F;
				else
					S = 1.05F;
				if(m_lpSceneEditor)
				{
					m_lpSceneEditor->SetCameraTopScale(m_lpSceneEditor->GetCameraTopScale() * S);// m_fCameraTopScale *= S;
					m_lpSceneEditor->SetCameraTopScale(max(0.01F,m_lpSceneEditor->GetCameraTopScale()));//m_fCameraTopScale = max(0.01F,m_lpSceneEditor->m_fCameraTopScale);
				}
				break;
			}
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_MOUSEMOVE:
		case WM_CHAR:
		case ID_EDIT_UNDO:
		case ID_EDIT_CUT:
		case ID_EDIT_REDO:
			{
				m_lpSceneEditor->ProcessMessage(message,wParam,lParam);//<PROCESS_MESSAGE>
				break;
			}
		}

	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void KSceneEditorTopViewDialog::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	if(m_lpSceneEditor)
	{
		IEKG3DScene* pScene = NULL;
		m_lpSceneEditor->GetScene(&pScene);
		_ASSERTE(&pScene);
		if (!pScene) return;
		pScene->SetCurrentOutputWindow(m_nOutputWndID);
	}
}

BOOL KSceneEditorTopViewDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::PreTranslateMessage(pMsg);
}

void KSceneEditorTopViewDialog::OnTopviewGround()
{
	// TODO: Add your command handler code here
	if(NULL != g_pEngineManager)
	{
        BOOL b = g_pEngineManager->GetRenderOption(RENDER_TERR_TOPVIEW);
		if(!b)
			g_pEngineManager->SetRenderOption(RENDER_TERR_TOPVIEW, TRUE);
		else
			g_pEngineManager->SetRenderOption(RENDER_TERR_TOPVIEW, FALSE);
	}
}

void KSceneEditorTopViewDialog::OnUpdateTopviewGround(CCmdUI *pCmdUI)
{
	if(NULL != g_pEngineManager)
	{
		BOOL b = g_pEngineManager->GetRenderOption(RENDER_TERR_TOPVIEW);
		pCmdUI->SetCheck(b);
		
	}
}

void KSceneEditorTopViewDialog::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnLButtonDblClk(nFlags, point);

	if(!m_lpSceneEditor)
		return;

    IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
    m_lpSceneEditor->GetCurOutputWnd(&piSceneCurOutputWnd);
	if(!piSceneCurOutputWnd)
		return;

	//(by dengzhihui Refactor 2006年11月6日
	m_lpSceneEditor->SetNPCPosition(TRUE, TRUE, NULL);
	//)
}

void KSceneEditorTopViewDialog::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnClose();

	if(!m_lpSceneEditor)
		return;

	if(m_nOutputWndID!=-1)
	{
		OnEndaddpoly();
		m_lpSceneEditor->SetEditState(SCENESTATE_SELECT);
		m_lpSceneEditor->RemoveOutputWindow(m_nOutputWndID);
		m_nOutputWndID = -1;
	}
}

void KSceneEditorTopViewDialog::OnTopviewEntityfar()
{
	if(NULL != g_pEngineManager)
	{
		BOOL b = g_pEngineManager->GetRenderOption(RENDER_ENTITY_TOPVIEW);
		if(!b)
			g_pEngineManager->SetRenderOption(RENDER_ENTITY_TOPVIEW, TRUE);
		else
			g_pEngineManager->SetRenderOption(RENDER_ENTITY_TOPVIEW, FALSE);
	}
}

void KSceneEditorTopViewDialog::OnTopviewWater()
{
	if(NULL != g_pEngineManager)
	{
		BOOL b = g_pEngineManager->GetRenderOption(RENDER_WATER_TOPVIEW);
		if(!b)
			g_pEngineManager->SetRenderOption(RENDER_WATER_TOPVIEW, TRUE);
		else
			g_pEngineManager->SetRenderOption(RENDER_WATER_TOPVIEW, FALSE);
	}
}

void KSceneEditorTopViewDialog::OnTopviewlightmap()
{
}

void KSceneEditorTopViewDialog::OnTopviewGroundgird()
{
	// TODO: Add your command handler code here
	if(NULL != g_pEngineManager)
	{
		BOOL b = g_pEngineManager->GetRenderOption(RENDER_TERREDGE_TOPVIEW);
		if(!b)
			g_pEngineManager->SetRenderOption(RENDER_TERREDGE_TOPVIEW, TRUE);
		else
			g_pEngineManager->SetRenderOption(RENDER_TERREDGE_TOPVIEW, FALSE);
	}
}


void KSceneEditorTopViewDialog::SetSelState()
{
    if (m_lpSceneEditor)
    {
        m_lpSceneEditor->SetEditState(SCENESTATE_SELECTREGION);
    }
}


void KSceneEditorTopViewDialog::OnTopviewRegionSelection()
{
	// TODO: Add your command handler code here
	if(m_lpSceneEditor)
	{
		m_lpSceneEditor->BeginSelectSceneBlock(m_bEnableSelectZone,m_bEnableSelectSection,m_bEnableSelectRegion,m_bEnableOnlyExist);
		m_dwSelectionPoint = 0;
		m_eState = STATE_SELECTING;
	}
}

void KSceneEditorTopViewDialog::OnNewpoly()
{
	if(!m_lpSceneEditor)
		return;
	m_lpSceneEditor->BeginAddPolyVertex();
	m_eState = STATE_ADDPOLYVERTEX;
}
void KSceneEditorTopViewDialog::OnEndaddpoly()
{
	// TODO: Add your command handler code here
	if(!m_lpSceneEditor)
		return;
	m_lpSceneEditor->EndAddPolyVertex(0);
	m_eState = STATE_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////////
//(by huangjinshou 2007 8.2
void KSceneEditorTopViewDialog::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	ASSERT(pPopupMenu != NULL);   
	//Check the enabled state of various menu items.   

	CCmdUI  state;   
	state.m_pMenu = pPopupMenu;   
	ASSERT(state.m_pOther   ==   NULL);   
	ASSERT(state.m_pParentMenu   ==   NULL);   

	//Determine if menu is popup in top-level menu and set m_pOther to   
	//it if so (m_pParentMenu == NULL indicates that it is secondary popup).   
	HMENU   hParentMenu;   
	if(AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)   
		state.m_pParentMenu = pPopupMenu;     //Parent == child for tracking popup.   
	else if((hParentMenu = ::GetMenu(m_hWnd))!= NULL)   
	{   
		CWnd*   pParent = this;   
		//Child windows don't have menus--need to go to the top!   
		if (pParent != NULL &&(hParentMenu = ::GetMenu(pParent->m_hWnd))!= NULL)   
		{   
			int nIndexMax = ::GetMenuItemCount(hParentMenu);   
			for(int nIndex = 0;nIndex < nIndexMax;nIndex++)   
			{   
				if (::GetSubMenu(hParentMenu,nIndex) == pPopupMenu->m_hMenu)   
				{   
					//When popup is found,m_pParentMenu is containing menu.   
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);   
					break;   
				}   
			}   
		}   
	}   

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();   
	for(state.m_nIndex = 0;state.m_nIndex < state.m_nIndexMax;state.m_nIndex++)   
	{   
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);   
		if(state.m_nID == 0)   
			continue;//Menu separator or invalid cmd - ignore it.   

		ASSERT(state.m_pOther == NULL);   
		ASSERT(state.m_pMenu  != NULL);   
		if(state.m_nID == (UINT)-1)   
		{   
			//Possibly a popup menu,route to first item of that popup.   
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);   
			if(state.m_pSubMenu == NULL||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0||   
				state.m_nID == (UINT)-1)   
			{   
				continue;       //First item of popup can't be routed to.   
			}   
			state.DoUpdate(this,TRUE);  //Popups are never auto disabled.   
		}   
		else   
		{   
			//Normal menu item.   
			//Auto enable/disable if frame window has m_bAutoMenuEnable   
			//set and command is _not_ a system command.   
			state.m_pSubMenu  =  NULL;   
			state.DoUpdate(this,FALSE);   
		}   

		//Adjust for menu deletions and additions.   
		UINT   nCount = pPopupMenu->GetMenuItemCount();   
		if(nCount < state.m_nIndexMax)   
		{   
			state.m_nIndex -= (state.m_nIndexMax  -  nCount);   
			while(state.m_nIndex < nCount &&   
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)   
			{   
				state.m_nIndex++;   
			}   
		}   
		state.m_nIndexMax = nCount;   
	}   

}

void KSceneEditorTopViewDialog::OnUpdateTopviewlightmap(CCmdUI *pCmdUI)
{
}

void KSceneEditorTopViewDialog::OnUpdateTopviewGroundgird(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(NULL != g_pEngineManager)
	{
		BOOL b = g_pEngineManager->GetRenderOption(RENDER_TERREDGE_TOPVIEW);
		pCmdUI->SetCheck(b);

	}
}

void KSceneEditorTopViewDialog::OnUpdateTopviewWater(CCmdUI *pCmdUI)
{
	if(NULL != g_pEngineManager)
	{
		BOOL b = g_pEngineManager->GetRenderOption(RENDER_WATER_TOPVIEW);
		pCmdUI->SetCheck(b);

	}
}

void KSceneEditorTopViewDialog::OnUpdateTopviewEntityfar(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(NULL != g_pEngineManager)
	{
		BOOL b = g_pEngineManager->GetRenderOption(RENDER_ENTITY_TOPVIEW);
		pCmdUI->SetCheck(b);

	}
}
//)
///////////////////////////////////////////////////////////////////////////////////
void KSceneEditorTopViewDialog::OnFoceloadall()
{
	if(m_lpSceneEditor)
	{
		m_lpSceneEditor->PauseMultiThreadLoad(TRUE);

		m_lpSceneEditor->ForceLoadAllEntity();

		m_lpSceneEditor->PauseMultiThreadLoad(FALSE);
	}
}

void KSceneEditorTopViewDialog::OnBnClickedCheckZone()
{
	UpdateData(TRUE);
	OnTopviewRegionSelection();
}

void KSceneEditorTopViewDialog::OnBnClickedCheckSection()
{
	UpdateData(TRUE);
	OnTopviewRegionSelection();
}

void KSceneEditorTopViewDialog::OnBnClickedCheckRegion()
{
	UpdateData(TRUE);
	OnTopviewRegionSelection();
}

void KSceneEditorTopViewDialog::OnBnClickedCheckExistonly()
{
	UpdateData(TRUE);
	OnTopviewRegionSelection();
}

void KSceneEditorTopViewDialog::OnBnClickedButtonBlockselect()
{
	UpdateData(TRUE);
	OnTopviewRegionSelection();
}

void KSceneEditorTopViewDialog::OnBnClickedButtonBlockcreate()
{
	if(m_lpSceneEditor)
	{
		m_lpSceneEditor->CreateSelectedSceneBlock(FALSE);
	}
}

void KSceneEditorTopViewDialog::OnBnClickedButtonBlockdelete()
{
	if(m_lpSceneEditor)
	{
		m_lpSceneEditor->DeleteSelectedSceneBlock(FALSE);
	}
}

void KSceneEditorTopViewDialog::OnBnClickedButtonTerraincreate()
{
	if(m_lpSceneEditor)
	{
		m_lpSceneEditor->CreateSelectedSceneBlock(TRUE);
	}
}

void KSceneEditorTopViewDialog::OnBnClickedButtonTerraindelete()
{
	if(m_lpSceneEditor)
	{
		m_lpSceneEditor->DeleteSelectedSceneBlock(TRUE);
	}
}

void KSceneEditorTopViewDialog::OnEnChangeEditHeightmap()
{
	UpdateData(TRUE);
}

void KSceneEditorTopViewDialog::OnEnChangeEditHeightwidth()
{
	UpdateData(TRUE);

}

void KSceneEditorTopViewDialog::OnEnChangeEditLow()
{
	UpdateData(TRUE);
	if(m_lpSceneEditor)
	{
		m_lpSceneEditor->SetSceneHeightInfo((int)m_fHeightLowest,(int)m_fHeightTotal);
	}
}

void KSceneEditorTopViewDialog::OnEnChangeEditHeight()
{
	UpdateData(TRUE);
	if(m_lpSceneEditor)
	{
		m_lpSceneEditor->SetSceneHeightInfo((int)m_fHeightLowest,(int)m_fHeightTotal);
	}
}



void KSceneEditorTopViewDialog::OnBnClickedButtonApplyheightmap()
{
	if(m_lpSceneEditor)
	{
		;
	}
}

void KSceneEditorTopViewDialog::OnBnClickedButtonApplytext()
{
	if(!m_lpSceneEditor)
		return;

	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	TCHAR szAppDirectory[MAX_PATH] = { 0 };

	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="Texture Files|*.bmp;*.jpg;*.png;*.tga;*.DDS;*.AniTexture|";

	CFileDialog dlg(true, NULL, NULL,
		OFN_HIDEREADONLY , szFilter);
	// Modify OPENFILENAME members directly to point to bigBuff
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	if (dlg.DoModal() == IDOK)
	{
		TCHAR FileName[MAX_PATH];
		TCHAR RetFileName[MAX_PATH];
		wsprintf(FileName,"%s",dlg.GetPathName());
		pManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);
		HRESULT hr = pManager->GetRelativePath(szAppDirectory,FileName,RetFileName);
		
		if(FAILED(hr))
		{
			MessageBox(RetFileName,"相对路径 获取失败");
		}
		else
		{
			m_lpSceneEditor->ApplyBaseMapToSelectedSceneBlock(RetFileName);
		}
	}

	::SetCurrentDirectory(szDir);

}

void KSceneEditorTopViewDialog::OnBnClickedButtonExportheight()
{
	if(!m_lpSceneEditor)
		return ;

	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	TCHAR szAppDirectory[MAX_PATH] = { 0 };

	CFileDialog dlg(false, "r32", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "32位浮点高度文件(*.r32)|*.r32||");
	if(dlg.DoModal() == IDOK)
	{
		TCHAR FileName[MAX_PATH];
		TCHAR RetFileName[MAX_PATH];
		wsprintf(FileName,"%s",dlg.GetPathName());
		pManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);
		pManager->GetRelativePath(szAppDirectory,FileName,RetFileName);

		m_lpSceneEditor->ExprotTerrainHeightMap(RetFileName,0);
	}

	::SetCurrentDirectory(szDir);

}

void KSceneEditorTopViewDialog::OnBnClickedButtonImportheight()
{
	if(!m_lpSceneEditor)
		return ;

	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	TCHAR szAppDirectory[MAX_PATH] = { 0 };

	CFileDialog dlg(true, "r32", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "32位浮点高度文件(*.r32)|*.r32||");
	if(dlg.DoModal() == IDOK)
	{
		TCHAR FileName[MAX_PATH];
		TCHAR RetFileName[MAX_PATH];
		wsprintf(FileName,"%s",dlg.GetPathName());
		pManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);
		pManager->GetRelativePath(szAppDirectory,FileName,RetFileName);

		m_lpSceneEditor->ImportTerrainHeightMap(RetFileName,0);
	}

	::SetCurrentDirectory(szDir);

}


void KSceneEditorTopViewDialog::OnBnClickedCheckShowappointlogicalregion()
{
	// TODO: Add your control notification handler code here
	int nCheck = ((CButton*)GetDlgItem(IDC_CHECK_SHOWAPPOINTLOGICALREGION))->GetCheck();
	if(m_lpSceneEditor)
	{
		m_lpSceneEditor->EnableShowAppointLogicalRegion(nCheck);
	}
}

void KSceneEditorTopViewDialog::UpdataAppiontLogicalRegion()
{
	if(m_lpSceneEditor)
	{
		UpdateData(TRUE);
		IEKG3DScene* pScene = NULL;
		m_lpSceneEditor->GetScene(&pScene);
		if(pScene)
			pScene->SetAppointLogicalRegion(m_nRegionXStart,m_nRegionZStart,m_nCountRegionX,m_nCountRegionZ);
	}
}
void KSceneEditorTopViewDialog::OnEnChangeEditLrstartx()
{
	UpdataAppiontLogicalRegion();
}

void KSceneEditorTopViewDialog::OnEnChangeEditLrstartz()
{
	UpdataAppiontLogicalRegion();
}

void KSceneEditorTopViewDialog::OnEnChangeEditLrcountx()
{
	UpdataAppiontLogicalRegion();
}

void KSceneEditorTopViewDialog::OnEnChangeEditLrcountz()
{
	UpdataAppiontLogicalRegion();
}

void KSceneEditorTopViewDialog::OnBnClickedSelectLogicRegion()
{
	if(m_lpSceneEditor)
	{
		m_lpSceneEditor->SetEditState(SCENESTATE_NONE);
		m_lpSceneEditor->EnableShowAppointLogicalRegion(TRUE);
		((CButton*)GetDlgItem(IDC_CHECK_SHOWAPPOINTLOGICALREGION))->SetCheck(TRUE);
	}
	m_eState = STATE_SELECTLOGICREGION;
}
void KSceneEditorTopViewDialog::OnBnClickedCheckBox()
{
	BOOL nCheck = ((CButton*)GetDlgItem(IDC_CHECK_BOX))->GetCheck();
	if(m_lpSceneEditor)
		m_lpSceneEditor->EnableBlockBox(nCheck);
}
