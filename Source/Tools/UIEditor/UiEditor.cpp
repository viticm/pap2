#include "Stdafx.h"

#define LUGIN_INTERNAL_SIGNATURE
#include "UiEditor.h"
#include "resource.h"
#include "UiWndButton.h"
#include "UiWndButtonCommon.h"
#include "UiWndImageCommon.h"
#include "UiWndImage.h"
#include "UiWndEdit.h"
#include "UiWndPage.h"
#include "UiWndPageSet.h"
#include "ClientManage.h"
#include "UiColorManage.h"
#include "UiFontManage.h"
#include "UiWindowManage.h"
#include "UiProjectManage.h"
#include "UiWndFrame.h"
#include "UiWndCheckBox.h"
#include "UiHotKeyManage.h"
#include "UiItemHandle.h"
#include "UiItemText.h"
#include "UiItemImage.h"
#include "UiItemAnimate.h"
#include "UiItemShadow.h"
#include "UiItemTreeLeaf.h"
#include "UiWndNewScrollbar.h"
#include "UiGlobalStringValueMgr.h"
#include "UiItemBox.h"
#include "UiItemScene.h"
#include "UiWndMinimap.h"
#include "UiWndScene.h"
#include "UiWndWebPage.h"

//for debug only!
/*
struct __CONSOLE
{
    __CONSOLE(){ AllocConsole(); }
    ~__CONSOLE(){ FreeConsole(); }
} g_Console;
*/

HANDLE				g_hPluginInstance = NULL;
//游戏窗口
HWND				g_hDawnWnd = NULL;
//## 控制客户端的行为
FnPluginBehaviorCallback	g_pfnPluginBehavior = NULL;
FnPluginBehaviorCallback	g_pfnUIBehavior = NULL;

KUiEditor *g_pUiEditor = NULL;

LRESULT g_UiEditorOperation(UINT uOper, WPARAM wParam, LPARAM lParam)
{
	if (g_pUiEditor)
		return g_pUiEditor->Operation(uOper, wParam, lParam);
	return NULL;
}

//用于保存自定义颜色值。
COLORREF KUiEditor::CustClr[16];
//存储错误信息
KUiEditor::KUIWNDERRORINFOMATION KUiEditor::ms_ErrorInfomation;
KUiEditor::KSwordPlusSetting KUiEditor::m_Settings;

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hPluginInstance = hModule;
	InitCommonControls();
    return TRUE;
}


extern "C"
__declspec(dllexport)
IPluginShell* CreatePlugin()
{
	g_pUiEditor = new KUiEditor;
	return ((IPluginShell*)(g_pUiEditor));
}

KUiEditor::KUiEditor()
{
	m_bEnable = true;
	m_hWnd = m_hTreeWnd = m_hWndTab = NULL;
	m_szCurrentWndUnit[0] = 0;
	m_pDetailInfoWnd = NULL;
	m_szCurrentWndUnit[0] = 0;
	m_nNewWndCounter = 0;
    m_szCurrentFile[0] = 0;
    m_szCurrentSelWndName[0] = 0;
	m_pSettingIniFile = NULL;

	//--初始化各种设置--
    memset(&m_Settings, 0, sizeof(m_Settings));
}

void KUiEditor::Release()
{
	Destroy();
	delete this;
}

int KUiEditor::Create(HWND hWnd, void* pReserved)
{
	g_hDawnWnd = hWnd;
	if (!Create())
		return false;
	return true;
}

int KUiEditor::Init()
{
	LoadSetting();
	return 1;
}

void KUiEditor::Breathe()
{

}

//## 设置操作回调函数
void KUiEditor::SetCallback(PLUGIN_CALLBACK_FUNC eFuncIndex, FnPluginBehaviorCallback pFunc)
{
	switch (eFuncIndex)
	{
	case PCF_PLUGIN_BEHAVIOR:
		g_pfnPluginBehavior = pFunc;
		g_pfnPluginBehavior(PCB_PLUGIN_SET_CALLBACK, PCF_UIEDITOR_BEHAVIOR, (LPARAM)g_UiEditorOperation);
		break;
	case PCF_UI_BEHAVIOR:
		g_pfnUIBehavior = pFunc;
		break;
	}
}

LRESULT KUiEditor::EventNotify(UINT uEvent, WPARAM wParam, LPARAM lParam)
{
	if (!m_bEnable)
		return 0;
	LRESULT lRet = FALSE;
	return lRet;
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
LRESULT CALLBACK KUiEditor::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    KUiEditor*	pDlg = (KUiEditor*)GetWindowLong(hWnd, GWL_USERDATA);
	switch(uMsg)
	{
	case WM_CLOSE:
		pDlg->SaveSetting();
		::ShowWindow(hWnd, SW_HIDE);
		if (pDlg)
			pDlg->m_bEnable = false;
		break;
	case WM_COMMAND:
		if (pDlg)
		{
			int nNotify = HIWORD(wParam);
			switch(nNotify)
			{
			case EN_CHANGE:
				pDlg->OnUpdateEditText(LOWORD(wParam));
				break;
			case 0:
				pDlg->OnButtonClick(LOWORD(wParam), (HWND)lParam);
				break;
            }
		}
        break;
    case WM_SYSCOMMAND:
        if (pDlg) 
        {
            switch(wParam) 
            {
            case SC_HOTKEY:
            	break;
            default:
                break;
            }
        }
        break;
    case WM_NOTIFY:
        if (pDlg)
        {
			LPNMHDR pNotify = (LPNMHDR)lParam;
            if(pNotify->hwndFrom == pDlg->m_hTreeWnd)
            {
                pDlg->OnTreeNotify(pNotify, lParam);
            }
            else if (pNotify->hwndFrom ==pDlg->m_hWndTab) 
            {
                pDlg->OnTabNotify(pNotify);
            }

            if (::GetKeyState(VK_CONTROL) & 0x8000)
            {
                NMLVKEYDOWN *pnkd = (LPNMLVKEYDOWN)lParam;
                if(pnkd->wVKey == 'S')
                     pDlg->OnUiSave();
            }
        }
        break;
	case WM_DESTROY:
		if (pDlg)
			pDlg->SaveSetting();
		break;
    case WM_HOTKEY:
        if(pDlg &&
            (LOWORD(lParam) & MOD_CONTROL)
        )
        {
            switch(HIWORD(lParam)) 
            {
            case 'S':
                {
                    HWND hParent = ::GetFocus();
                    while (hParent)
                    {
                        if (hParent == pDlg->m_hWnd)
                        {
                            pDlg->OnUiSave();
                            break;
                        }
                        hParent = ::GetParent(hParent);
                    }                    
                }
            	break;
            default:
                break;
            }
        }
                    
        break;
	}

	return 0;
}


void KUiEditor::OnTreeNotify(LPNMHDR pNotify, LPARAM lParam)
{
    char szName[128];
    szName[0] = '\0';


    LPNMTREEVIEWA pMsg = (LPNMTREEVIEWA)pNotify;
	switch(pMsg->hdr.code)
	{

	case TVN_SELCHANGED:
        m_ShowWndTree.GetNamebyHItem(pMsg->itemNew.hItem, szName);
		if (strcmp(szName, m_szCurrentWndUnit))
			FillDetailInfo(szName, false, false);        
		break;
    case NM_RCLICK:
        /*
        {
            POINT Pos;
            ::GetCursorPos(&Pos);
            //RECT rc;
	        //::GetWindowRect(m_hTreeWnd, &rc);
            //Pos.x -= rc.left;
            //Pos.y -= rc.top;
            ::ScreenToClient(m_hTreeWnd, &Pos);
            ::SendMessage(m_hTreeWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(Pos.x, Pos.y));

            HTREEITEM hItemNow = TreeView_GetSelection(m_hTreeWnd);
            if (hItemNow) 
            {
                KSHOWWNDNODE *pNode = m_ShowWndTree.GetNodeByHItem(hItemNow);
                if (pNode) 
                {
                    TreeView_SelectItem(m_hTreeWnd, pNode->hItem);
                    OnChangeParent(pNode);
                }
            }
        }
        */
        OnTreeItemRightButtonClick();
        break;
    case TVN_KEYDOWN:
        if (::GetKeyState(VK_CONTROL) & 0x8000)
        {
            NMLVKEYDOWN *pnkd = (LPNMLVKEYDOWN)lParam;
            switch(pnkd->wVKey) 
            {
            case 'C':
                UIWINDOWMANAGE::AddWindow(m_szCurrentWndUnit, g_Ui.GetIni(), m_szCurrentSelWndName);
                ::SetFocus(m_hTreeWnd);
            	break;
            case 'V':
                {
                    char szName[128] = "";
                    if(UIWINDOWMANAGE::SelWindow(m_hWnd, g_Ui.GetIni(), m_szCurrentSelWndName, m_szCurrentWndUnit, szName))
						UpdateAll(m_szCurrentWndUnit, false, true);
                        //UpdateAll(szName, false, true);
                    ::SetFocus(m_hTreeWnd);
                }
                break;
            case 'S':
                OnUiSave();
                break;
            default:
                break;
            }
        }
        break;

    default:
        break;
	}
 }

void KUiEditor::OnUpdateEditText(int nId)
{
	if (!m_szCurrentWndUnit[0] || !g_Ui.GetIni())
		return;
	if (nId == IDC_WND_REMARK)
	{
		char	szString[128] = "";
		::GetDlgItemText(m_hWnd, nId, szString, sizeof(szString));
		if (szString[0])
			g_Ui.GetIni()->WriteString(m_szCurrentWndUnit, "._Comment", szString);
		else
			g_Ui.GetIni()->EraseKey(m_szCurrentWndUnit, "._Comment");
	}
}

void KUiEditor::OnTreeItemRightButtonClick()
{
    KSHOWWNDNODE *pNode = NULL;
    HTREEITEM hItemNow = NULL;
    POINT Pos;
    char szType[128];
    szType[0] = '\0';

    ::GetCursorPos(&Pos);
    ::ScreenToClient(m_hTreeWnd, &Pos);
    ::SendMessage(m_hTreeWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(Pos.x, Pos.y));

    hItemNow = TreeView_GetSelection(m_hTreeWnd);

    if (!hItemNow)
    {
        OnNewFrame();
        KG_PROCESS_SUCCESS(true);
    }

    pNode = m_ShowWndTree.GetNodeByHItem(hItemNow);
    KG_PROCESS_ERROR(pNode);
    TreeView_SelectItem(m_hTreeWnd, pNode->hItem);

    g_Ui.GetIni()->GetString(pNode->szName, "._WndType", "", szType, sizeof(szType));
    switch(g_GetWndType(szType))
    {
    case UI_WND_WINDOW:
    case UI_WND_BUTTON:
    case UI_WND_CHECKBOX:
    case UI_WND_SCROLLBAR:
    case UI_WND_EDIT:
	case UI_WND_PAGE:
	case UI_WND_PAGESET:
	case UI_WND_MINIMAP:
	case UI_WND_SCENE:
	case UI_WND_WEBPAGE:
        OnWndRightButtonClick(pNode);
        break;
    case UI_WND_FRAME:
        OnFrameRightButtonClick(pNode);
        break;
    case UI_ITEM_NULL:
    case UI_ITEM_TEXT:
    case UI_ITEM_IMAGE:
    case UI_ITEM_SHADOW:
    case UI_ITEM_ANIAMTE:
	case UI_ITEM_BOX:
    case UI_ITEM_SCENE:
        OnItemRightButtonClick(pNode);
        break;
    case UI_ITEM_HANDLE:
    case UI_ITEM_TREE_LEAFE:
        OnHandleRightButtonClick(pNode);
        break;
    case UI_WND_TYPE_UNKNOWN:
    default:
        break;
    }

Exit1:
Exit0:
    return;
}
void KUiEditor::OnNewFrame()
{
    HMENU hMenu = NULL;
    POINT msPos;
    int nId = -1;

    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_hTreeWnd);

    hMenu = ::LoadMenu((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDR_WND_FRAME_MENU));
    KG_PROCESS_ERROR(hMenu);

	::EnableMenuItem(hMenu, ID_UNIT_COPY, MF_BYCOMMAND | MF_GRAYED);
	if (!m_szCurrentSelWndName[0])
		::EnableMenuItem(hMenu, ID_UNIT_PAST, MF_BYCOMMAND | MF_GRAYED);

    ::GetCursorPos(&msPos);
    nId = ::TrackPopupMenu(
        ::GetSubMenu(hMenu, 0),
        TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY,
		msPos.x, msPos.y, 0, m_hWnd, NULL
    );

    switch(nId)
    {
    case ID_WNDTYPE_FRAME:
        CreateWnd(UI_WND_FRAME, g_GetFrameLayerName(LAYER_NORMAL));     
        break;
	case ID_UNIT_COPY:
		break;
	case ID_UNIT_PAST:
		{
			char szName[128] = "";
			if(UIWINDOWMANAGE::SelWindow(m_hWnd, g_Ui.GetIni(), m_szCurrentSelWndName, "", szName))
				UpdateAll(szName, false, true);
			::SetFocus(m_hTreeWnd);
		}
		break;
	case ID_UNIT_CHECKOUT:
		{
			char szName[128] = "";
			if(UIWINDOWMANAGE::SelWindow(m_hWnd, g_Ui.GetIni(), "", szName))
				UpdateAll(szName, false, true);
		}
		break;
    default:
        break;
    }

Exit0:
    if (hMenu)
    {
        ::DestroyMenu(hMenu);
        hMenu = NULL;
    }
    return;
}

void KUiEditor::OnFrameRightButtonClick(KSHOWWNDNODE *pNode)
{
    int nRetCode = false;
    HMENU hMenu = NULL;
    POINT msPos;
    int nId = -1;

    KG_PROCESS_ERROR(pNode);
    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_hTreeWnd);

    hMenu = ::LoadMenu((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDR_FRAME_RCLICK_MENU));
    KG_PROCESS_ERROR(hMenu);

    if (g_Ui.IsWndHasHandle(pNode->szName))
        ::EnableMenuItem(hMenu, IDM_FRAME_ADD_ITEMHANDLE, MF_BYCOMMAND | MF_GRAYED);

	if (!m_szCurrentSelWndName[0])
		::EnableMenuItem(hMenu, ID_UNIT_PAST, MF_BYCOMMAND | MF_GRAYED);

    ::GetCursorPos(&msPos);
    nId = ::TrackPopupMenu(
        ::GetSubMenu(hMenu, 0),
        TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY,
		msPos.x, msPos.y, 0, m_hWnd, NULL
    );
    if (hMenu)
    {
        ::DestroyMenu(hMenu);
        hMenu = NULL;
    }
    switch(nId)
    {
        case IDM_FRAME_ADD_ITEMHANDLE:
            CreateWnd(UI_ITEM_HANDLE, pNode->szName);
            break;
        case IDM_FRAME_ADD_WND_WINDOW:
            CreateWnd(UI_WND_WINDOW, pNode->szName);
            break;
        case IDM_FRAME_ADD_WND_SCROLLBAR:
            CreateWnd(UI_WND_SCROLLBAR, pNode->szName);
            break;
        case IDM_FRAME_ADD_WND_BUTTON:
            CreateWnd(UI_WND_BUTTON, pNode->szName);
            break;
        case IDM_FRAME_ADD_WND_CKECKBOX:
            CreateWnd(UI_WND_CHECKBOX, pNode->szName);
            break;
        case IDM_FRAME_ADD_WND_EDIT:
            CreateWnd(UI_WND_EDIT, pNode->szName);
            break;
		case IDM_FRAME_ADD_WND_PAGE:
			CreateWnd(UI_WND_PAGE, pNode->szName);
			break;
		case IDM_FRAME_ADD_WND_PAGESET:
			CreateWnd(UI_WND_PAGESET, pNode->szName);
			break;
		case IDM_FRAME_ADD_WND_MINIMAP:
			CreateWnd(UI_WND_MINIMAP, pNode->szName);
			break;
		case IDM_FRAME_ADD_WND_SCENE:
			CreateWnd(UI_WND_SCENE, pNode->szName);
			break;
		case IDM_FRAME_ADD_WND_WEB_PAGE:
			CreateWnd(UI_WND_WEBPAGE, pNode->szName);
			break;
        case IDM_FRAME_SET_LEVEL_LOWEST:
            ChangeParent(pNode->szName, g_GetFrameLayerName(LAYER_LOWEST));
            break;
		case IDM_FRAME_SET_LEVEL_LOWEST1:
			ChangeParent(pNode->szName, g_GetFrameLayerName(LAYER_LOWEST1));
			break;
		case IDM_FRAME_SET_LEVEL_LOWEST2:
			ChangeParent(pNode->szName, g_GetFrameLayerName(LAYER_LOWEST2));
			break;
        case IDM_FRAME_SET_LEVEL_NORMAL:
            ChangeParent(pNode->szName, g_GetFrameLayerName(LAYER_NORMAL));
            break;
		case IDM_FRAME_SET_LEVEL_NORMAL1:
			ChangeParent(pNode->szName, g_GetFrameLayerName(LAYER_NORMAL1));
			break;
		case IDM_FRAME_SET_LEVEL_NORMAL2:
			ChangeParent(pNode->szName, g_GetFrameLayerName(LAYER_NORMAL2));
			break;
        case IDM_FRAME_SET_LEVEL_TOPMOST:
            ChangeParent(pNode->szName, g_GetFrameLayerName(LAYER_TOPMOST));
            break;
		case IDM_FRAME_SET_LEVEL_TOPMOST1:
			ChangeParent(pNode->szName, g_GetFrameLayerName(LAYER_TOPMOST1));
			break;
		case IDM_FRAME_SET_LEVEL_TOPMOST2:
			ChangeParent(pNode->szName, g_GetFrameLayerName(LAYER_TOPMOST2));
			break;

        case IDM_FRAME_SET_LEVEL_TEXTURE_END:
            ChangeParent(pNode->szName, g_GetFrameLayerName(LAYER_TEXTURE));
            break;
        case IDM_FRAME_DELETE:
            OnWndDel();
            break;
		case ID_UNIT_COPY:
			UIWINDOWMANAGE::AddWindow(m_szCurrentWndUnit, g_Ui.GetIni(), m_szCurrentSelWndName);
			::SetFocus(m_hTreeWnd);
			break;
		case ID_UNIT_PAST:
			{
				char szName[128] = "";
				if(UIWINDOWMANAGE::SelWindow(m_hWnd, g_Ui.GetIni(), m_szCurrentSelWndName, pNode->szName, szName))
					UpdateAll(szName, false, true);
				::SetFocus(m_hTreeWnd);
			}
			break;
		case ID_UNIT_CHECKOUT:
			{
				char szName[128] = "";
				if(UIWINDOWMANAGE::SelWindow(m_hWnd, g_Ui.GetIni(), pNode->szName, szName))
					UpdateAll(szName, false, true);
			}
			break;
    }

Exit0:
    if (hMenu)
    {
        ::DestroyMenu(hMenu);
        hMenu = NULL;
    }
    return;
}

void KUiEditor::OnItemRightButtonClick(KSHOWWNDNODE *pNode)
{
    int nRetCode = false;
    HMENU hMenu = NULL;
    POINT msPos;
    int nId = -1;

    KG_PROCESS_ERROR(pNode);
    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_hTreeWnd);

    hMenu = ::LoadMenu((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDR_ITEM_RCLICKMENU));
    KG_PROCESS_ERROR(hMenu);

    ::GetCursorPos(&msPos);
    nId = ::TrackPopupMenu(
        ::GetSubMenu(hMenu, 0),
        TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY,
		msPos.x, msPos.y, 0, m_hWnd, NULL
    );
    if (hMenu)
    {
        ::DestroyMenu(hMenu);
        hMenu = NULL;
    }
    switch(nId)
    {
    case IDM_ITEM_MOVEUP:
        OnMoveWndUp();
        break;
    case IDM_ITEM_MOVEDOWN:
        OnMoveWndDown();
        break;
    case IDM_ITEM_DELETE:
        OnWndDel();
        break;
	case ID_UNIT_COPY:
		UIWINDOWMANAGE::AddWindow(m_szCurrentWndUnit, g_Ui.GetIni(), m_szCurrentSelWndName);
		::SetFocus(m_hTreeWnd);
		break;
    }

Exit0:
    if (hMenu)
    {
        ::DestroyMenu(hMenu);
        hMenu = NULL;
    }
    return;
}

void KUiEditor::OnHandleRightButtonClick(KSHOWWNDNODE *pNode)
{
    int nRetCode = false;
    HMENU hMenu = NULL;
    POINT msPos;
    int nId = -1;

    KG_PROCESS_ERROR(pNode);
    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_hTreeWnd);

    hMenu = ::LoadMenu((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDR_HANDLE_RCLICK_MENU));
    KG_PROCESS_ERROR(hMenu);

	if (!m_szCurrentSelWndName[0])
		::EnableMenuItem(hMenu, ID_UNIT_PAST, MF_BYCOMMAND | MF_GRAYED);

    g_Ui.GetIni()->GetInteger(pNode->szName, "HandleType", 0, &nRetCode);
    switch(nRetCode)
    {
    case ITEM_HANDLE_TREE_ITEM_HANDLE:
        ::EnableMenuItem(hMenu, IDM_HANDLE_ADD_ITEM_NULL, MF_BYCOMMAND | MF_GRAYED);
        ::EnableMenuItem(hMenu, IDM_HANDLE_ADD_ITEM_TEXT, MF_BYCOMMAND | MF_GRAYED);
        ::EnableMenuItem(hMenu, IDM_HANDLE_ADD_ITEM_IMAGE, MF_BYCOMMAND | MF_GRAYED);
        ::EnableMenuItem(hMenu, IDM_HANDLE_ADD_ITEM_SHADOW, MF_BYCOMMAND | MF_GRAYED);
        ::EnableMenuItem(hMenu, IDM_HANDLE_ADD_ITEM_ANIMATE, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(hMenu, IDM_HANDLE_ADD_ITEM_BOX, MF_BYCOMMAND | MF_GRAYED);
        ::EnableMenuItem(hMenu, IDM_HANDLE_ADD_ITEM_SENCE, MF_BYCOMMAND | MF_GRAYED);
        ::EnableMenuItem(hMenu, IDM_HANDLE_ADD_ITEM_HANDLE, MF_BYCOMMAND | MF_GRAYED);
        break;
    case ITEM_HANDLE_CUSTOM:
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS:
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX:
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM:
	case ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE:
    case ITEM_HANDLE_CHAT_ITEM_HANDLE:
    default:
        ::EnableMenuItem(hMenu, IDM_HANDLE_ADD_ITEM_TREELEAF, MF_BYCOMMAND | MF_GRAYED);
        break;
    }

    ::GetCursorPos(&msPos);
    nId = ::TrackPopupMenu(
        ::GetSubMenu(hMenu, 0),
        TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY,
		msPos.x, msPos.y, 0, m_hWnd, NULL
    );
    if (hMenu)
    {
        ::DestroyMenu(hMenu);
        hMenu = NULL;
    }
    switch(nId)
    {
    case IDM_HANDLE_ADD_ITEM_NULL:
        CreateWnd(UI_ITEM_NULL, pNode->szName);
        break;
    case IDM_HANDLE_ADD_ITEM_TEXT:
        CreateWnd(UI_ITEM_TEXT, pNode->szName);
        break;
    case IDM_HANDLE_ADD_ITEM_IMAGE:
        CreateWnd(UI_ITEM_IMAGE, pNode->szName);
        break;
    case IDM_HANDLE_ADD_ITEM_SHADOW:
        CreateWnd(UI_ITEM_SHADOW, pNode->szName);
        break;
    case IDM_HANDLE_ADD_ITEM_ANIMATE:
        CreateWnd(UI_ITEM_ANIAMTE, pNode->szName);
        break;
	case IDM_HANDLE_ADD_ITEM_BOX:
		CreateWnd(UI_ITEM_BOX, pNode->szName);
		break;
    case IDM_HANDLE_ADD_ITEM_SENCE:
		CreateWnd(UI_ITEM_SCENE, pNode->szName);
        break;
    case IDM_HANDLE_ADD_ITEM_TREELEAF:
        CreateWnd(UI_ITEM_TREE_LEAFE, pNode->szName);
        break;
    case IDM_HANDLE_ADD_ITEM_HANDLE:
        CreateWnd(UI_ITEM_HANDLE, pNode->szName);
        break;
    case IDM_HANDLE_MOVE_UP:
        OnMoveWndUp();
        break;
    case IDM_HANDLE_MOVE_DOWN:
        OnMoveWndDown();
        break;
    case IDM_HANDLE_DELETE:
        OnWndDel();
        break;
	case ID_UNIT_COPY:
		UIWINDOWMANAGE::AddWindow(m_szCurrentWndUnit, g_Ui.GetIni(), m_szCurrentSelWndName);
		::SetFocus(m_hTreeWnd);
		break;
	case ID_UNIT_PAST:
		{
			char szName[128] = "";
			if(UIWINDOWMANAGE::SelWindow(m_hWnd, g_Ui.GetIni(), m_szCurrentSelWndName, pNode->szName, szName))
				UpdateAll(szName, false, true);
			::SetFocus(m_hTreeWnd);
		}
		break;
	case ID_UNIT_CHECKOUT:
		{
			char szName[128] = "";
			if(UIWINDOWMANAGE::SelWindow(m_hWnd, g_Ui.GetIni(), pNode->szName, szName))
				UpdateAll(szName, false, true);
		}
		break;
    }

Exit0:
    if (hMenu)
    {
        ::DestroyMenu(hMenu);
        hMenu = NULL;
    }
    return;
}

void KUiEditor::OnWndRightButtonClick(KSHOWWNDNODE *pNode)
{
    int nRetCode = false;
    HMENU hMenu = NULL;
    POINT msPos;
    int nFirstID = 70000;
    int nCurrentID = nFirstID;
    int nId = -1;
    char szNext[128];
    szNext[0] = '\0';
    char szType[128];
    szType[0] = '\0';
    IIniFile *pIni = g_Ui.GetIni();
    KG_PROCESS_ERROR(pIni);
    KG_PROCESS_ERROR(pNode);
    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_hTreeWnd);

    hMenu = ::LoadMenu((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDR_WND_RCLICK_MENU));
    KG_PROCESS_ERROR(hMenu);

    if (g_Ui.IsWndHasHandle(pNode->szName))
        ::EnableMenuItem(hMenu, IDM_WND_ADD_ITEMHANDLE, MF_BYCOMMAND | MF_GRAYED);

	if (!m_szCurrentSelWndName[0])
		::EnableMenuItem(hMenu, ID_UNIT_PAST, MF_BYCOMMAND | MF_GRAYED);

    while (g_Ui.GetNextUnit(szNext))
    {
        if (stricmp(szNext, pNode->szName) == 0)
            continue;
        pIni->GetString(szNext, "._WndType", "", szType, sizeof(szType));
        switch(g_GetWndType(szType))
        {
        case UI_WND_WINDOW:
        case UI_WND_BUTTON:
        case UI_WND_CHECKBOX:
        case UI_WND_SCROLLBAR:
        case UI_WND_EDIT:
        case UI_WND_FRAME:
		case UI_WND_PAGE:
		case UI_WND_PAGESET:
		case UI_WND_MINIMAP:
		case UI_WND_SCENE:
		case UI_WND_WEBPAGE:
			if (g_Ui.IsUnitMyFather(pNode->szName, szNext))
				break;
			if (g_Ui.IsUnitMyForefather(szNext, pNode->szName))
				break;
			::InsertMenu(hMenu, IDM_WND_PARENT_1, MF_BYCOMMAND, nCurrentID, szNext);
			++nCurrentID;
            break;
        }
    }
    ::RemoveMenu(hMenu, IDM_WND_PARENT_1, MF_BYCOMMAND);

    ::GetCursorPos(&msPos);
    nId = ::TrackPopupMenu(
        ::GetSubMenu(hMenu, 0),
        TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY,
		msPos.x, msPos.y, 0, m_hWnd, NULL
    );
    switch(nId)
    {
    case IDM_ADD_WND_WINDOW:
        CreateWnd(UI_WND_WINDOW, pNode->szName);
        break;
    case IDM_ADD_WND_SCROLLBAR:
        CreateWnd(UI_WND_SCROLLBAR, pNode->szName);
        break;
    case IDM_ADD_WND_BUTTON:
        CreateWnd(UI_WND_BUTTON, pNode->szName);
        break;
    case IDM_ADD_WND_CHECKBOX:
        CreateWnd(UI_WND_CHECKBOX, pNode->szName);
        break;
    case IDM_ADD_WND_EDIT:
        CreateWnd(UI_WND_EDIT, pNode->szName);
        break;
	case IDM_ADD_WND_PAGE:
		CreateWnd(UI_WND_PAGE, pNode->szName);
		break;
	case IDM_ADD_WND_PAGESET:
		CreateWnd(UI_WND_PAGESET, pNode->szName);
		break;	
	case IDM_ADD_WND_MINIMAP:
		CreateWnd(UI_WND_MINIMAP, pNode->szName);
		break;
	case IDM_ADD_WND_SCENE:
		CreateWnd(UI_WND_SCENE, pNode->szName);
		break;
	case IDM_ADD_WND_WEB_PAGE:
		CreateWnd(UI_WND_WEBPAGE, pNode->szName);
		break;
    case IDM_WND_ADD_ITEMHANDLE:
        CreateWnd(UI_ITEM_HANDLE, pNode->szName);
        break;
    case IDM_WND_MOVE_UP:
        OnMoveWndUp();
        break;
    case IDM_WND_MOVE_DOWN:
        OnMoveWndDown();
        break;
    case IDM_WND_DELETE:
        OnWndDel();
        break;
	case ID_UNIT_COPY:
		UIWINDOWMANAGE::AddWindow(m_szCurrentWndUnit, g_Ui.GetIni(), m_szCurrentSelWndName);
		::SetFocus(m_hTreeWnd);
		break;
	case ID_UNIT_PAST:
		{
			char szName[128] = "";
			if(UIWINDOWMANAGE::SelWindow(m_hWnd, g_Ui.GetIni(), m_szCurrentSelWndName, pNode->szName, szName))
				UpdateAll(szName, false, true);
			::SetFocus(m_hTreeWnd);
		}
		break;
	case ID_UNIT_CHECKOUT:
		{
			char szName[128] = "";
			if(UIWINDOWMANAGE::SelWindow(m_hWnd, g_Ui.GetIni(), pNode->szName, szName))
				UpdateAll(szName, false, true);
		}
		break;
    default:
        if (nId >= nFirstID && nId < nCurrentID)
        {
            ::GetMenuString(hMenu, nId, szNext, sizeof(szNext), MF_BYCOMMAND);
            ChangeParent(pNode->szName, szNext);
        }
        break;
    }

Exit0:
    if (hMenu)
        ::DestroyMenu(hMenu);
    return;
}

void KUiEditor::ChangeParent(const char *pszMe, const char *pszNewParent)
{
    bool bNeedFoceRecreate = false;
    char szParent[128];
    KG_PROCESS_ERROR(pszMe);
    KG_PROCESS_ERROR(pszNewParent);

    g_Ui.GetIni()->GetString(pszMe, "._Parent", "", szParent, sizeof(szParent));

    g_Ui.SafeChangeUnitParent(pszMe, pszNewParent);
    m_ShowWndTree.NomalrizeIni(&g_Ui);


	UpdateAll(pszMe, bNeedFoceRecreate, true);

Exit0:
    return;
}

void KUiEditor::OnChangeParent(KSHOWWNDNODE *pNode)
{
    bool bNeedFoceRecreate = false;
    IIniFile *pIni = g_Ui.GetIni();
    char szParent[128];
    szParent[0] = '\0';
    char szUpdataWndName[128];
    szUpdataWndName[0] = '\0';
    KG_PROCESS_ERROR(pNode);
    KG_PROCESS_ERROR(pIni);

    pIni->GetString(pNode->szName, "._Parent", "", szParent, sizeof(szParent));
    szParent[0] = '\0';

    m_ShowWndTree.SelParent(m_hWnd, m_hTreeWnd, pNode, szParent);
    g_Ui.SafeChangeUnitParent(pNode->szName, szParent);
    m_ShowWndTree.NomalrizeIni(&g_Ui);

    pIni->GetString(pNode->szName, "._Parent", "", szParent, sizeof(szParent));

    strcpy(szUpdataWndName, pNode->szName);
	UpdateAll(szUpdataWndName, bNeedFoceRecreate, true);

Exit0:
    return;

}


bool KUiEditor::Create()
{
	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_UI_EDITOR), g_hDawnWnd, (DLGPROC)WndProc);
	if (m_hWnd == NULL)
		return false;
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
	if (g_hDawnWnd)
	{
		RECT	rectMainWnd, rectDesktop;
		HWND hDesktop = ::GetDesktopWindow();
		::GetWindowRect(g_hDawnWnd, &rectMainWnd);
		::GetWindowRect(hDesktop, &rectDesktop);
		POINT	pos;
		if (rectDesktop.right >= rectMainWnd.right + 4)
		{
			pos.x = rectMainWnd.right;
			pos.y = rectMainWnd.top;
		}
		else if (rectDesktop.bottom >= rectMainWnd.bottom + 4)
		{
			pos.x = rectMainWnd.left;
			pos.y = rectMainWnd.bottom;
		}
		else
		{
			pos.x = 0;
			pos.y = 0;
		}
		::SetWindowPos(m_hWnd, g_hDawnWnd, pos.x, pos.y, 0, 0, SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE);
	}

//    int nReSult = RegisterHotKey(m_hWnd, 0xC100, MOD_CONTROL, 'S');
    //设置显示的树形结构。
    m_hTreeWnd = ::GetDlgItem(m_hWnd, IDC_TREE_INI);
	//TreeView_SetBkColor( m_hTreeWnd, RGB(255,255,255));
	//TreeView_SetTextColor( m_hTreeWnd, RGB(255,0,0));

    //设置属性表。
    m_hWndTab = ::GetDlgItem(m_hWnd, IDC_UI_EDITOR_TAB);


	g_Ui.New();
    strcpy(m_szCurrentFile, "New.ini");
    ::SetWindowText(m_hWnd, m_szCurrentFile);
    
    RECT rc;
	::GetWindowRect(m_hWnd, &rc);
	m_sizeWindow.cx = rc.right - rc.left;
	m_sizeWindow.cy = rc.bottom - rc.top;
	m_bLarge = true;

	KUiGlobalStrMgr::Init();

	m_bShowMainUI = false;

	return true;
}

LRESULT KUiEditor::Operation(UINT uOper, WPARAM wParam, LPARAM lParam)
{
    switch(uOper)
    {
    case PLUGIN_ENABLE:
        m_bEnable = (lParam != 0);
        break;
    case PLUGIN_UI_EDITOR_SHOW_INFO:
        if (wParam)
        {
            LPCWSTR wszMessage = (LPCWSTR)wParam;
            if (wszMessage)
                ::SetDlgItemTextW(m_hWnd, IDC_WND_ERROR, wszMessage);
        }
        break;
    case PLUGIN_UI_EDITOR_SEL_WND:
        if (wParam)
        {
            LPCTSTR pszName = ((LPCTSTR)wParam);
            if(
                pszName &&
                (_tcsicmp(m_szCurrentWndUnit, pszName) != 0) &&
                g_Ui.GetIni()->IsSectionExist(pszName)
            )
            {
                strcpy(m_szCurrentSelWndName, pszName);
                UpdateAll(pszName, false, false);
            }
        }
        break;
    }

	return 0;
}

void KUiEditor::Destroy()
{
	KUiGlobalStrMgr::Exit();
//    UnregisterHotKey(m_hWnd, 0xC100);
    m_ShowWndTree.ClearTree();

	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
    if (m_pSettingIniFile) 
    {
        m_pSettingIniFile->Clear();
		m_pSettingIniFile->Release();
        m_pSettingIniFile = NULL;
    }
}

void KUiEditor::OnButtonClick(int nBtnId, HWND hWnd)
{
	switch(nBtnId)
	{
        //菜单。
	case ID_SEE_INI:
		OnUiOpenIni(); //用记事本打开Ini文件。
		break;
    case ID_FONT_MANAGE:
        {
            UIFONTMANAGE::FontManage(m_hWnd);
        }
        break;
    case ID_COLOR_MAMAGE:
        {
            UICOLORMANAGE::ColorManage(m_hWnd);
        }
        break;
	case ID_WND_STR_MANAGE:
		{
			KUiGlobalStrMgr::GetSelf().StringManage(m_hWnd);
		}
		break;
    case ID_WND_SAVE_MANAGE:
        {
            UIWINDOWMANAGE::WindowManage(m_hWnd);
        }        
        break;
    case ID_SAVE_WINDOW:
        {
            UIWINDOWMANAGE::AddWindow(m_szCurrentWndUnit, g_Ui.GetIni(), m_szCurrentSelWndName);
        }
        break;
    case ID_CHECK_OUT_WND:
        {
            char szName[128] = "";
            if(UIWINDOWMANAGE::SelWindow(m_hWnd, g_Ui.GetIni(), m_szCurrentWndUnit, szName))
                UpdateAll(szName, false, true);
        }
        break;
    case ID_SOLUTION_MANAGE:
        {
            //项目管理
            char szOpenIniName[MAX_PATH] = "";
            if(UIPROJECTMANAGE::ProjectManage(m_hWnd, szOpenIniName))
            {

            }
        }
        break;
	case ID_OPEN_INI:        
		OnUiSel();
		break;
    case ID_NEW_INI:
		if (g_Ui.IsModified())
			if (MessageBox(m_hWnd, "文件已修改，是否保存？", "注意：", MB_YESNO | MB_ICONQUESTION) == IDYES)
				OnUiSave();
		OnClear();
		g_Ui.New();
        strcpy(m_szCurrentFile, "New.ini");
        ::SetWindowText(m_hWnd, m_szCurrentFile);
		break;
    case ID_SAVE_INI:
		OnUiSave();
		break;
    case ID_SAVE_INI_AS:
        OnUiSaveAs();
        break;
    case ID_UIEDITOR_ADD_LUA:
        {
             HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_UIEDITOR_ADD_LUA, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_UIEDITOR_ADD_LUA, MF_UNCHECKED);
                g_ClientManager.BindScript(false, g_Ui.GetIni());
            }
            else
            {
                CheckMenuItem(hMenu, ID_UIEDITOR_ADD_LUA, MF_CHECKED);
                g_ClientManager.BindScript(true, g_Ui.GetIni());
            }
        }
        break;
    case ID_CHOSE_WND_FROM_CLIENT:
        {
             HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_CHOSE_WND_FROM_CLIENT, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_CHOSE_WND_FROM_CLIENT, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_ENABLE_SEL_WND_IN_CLIENT, (WPARAM)(NULL), (LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_CHOSE_WND_FROM_CLIENT, MF_CHECKED);
                g_pfnUIBehavior(PCB_UI_ENABLE_SEL_WND_IN_CLIENT, (WPARAM)(NULL), (LPARAM)1);
            }
        }
        break;
    case ID_SHOW_MAIN_UI:
        {
            HMENU hMenu = GetMenu(m_hWnd);
            m_bShowMainUI = !m_bShowMainUI;
            if (m_bShowMainUI) 
            {
                CheckMenuItem(hMenu, ID_SHOW_MAIN_UI, MF_CHECKED);
                g_pfnUIBehavior(PCB_UI_LOAD_MAIN_UI, (WPARAM)(1), (LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_SHOW_MAIN_UI, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_LOAD_MAIN_UI, (WPARAM)(0), (LPARAM)0);
            }
        }
        break;
    case ID_ENABLE_RENDER_UI:
        {
            HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_ENABLE_RENDER_UI, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_ENABLE_RENDER_UI, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_ENABLE_RENDER_UI, (WPARAM)NULL, (LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_ENABLE_RENDER_UI, MF_CHECKED);
                g_pfnUIBehavior(PCB_UI_ENABLE_RENDER_UI, (WPARAM)NULL, (LPARAM)1);
            }
        }
        break;
    case ID_SHOW_EDGE:
        {
            HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_SHOW_EDGE, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_SHOW_EDGE, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_SHOW_FRAME, (WPARAM)NULL, (LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_SHOW_EDGE, MF_CHECKED);
                g_pfnUIBehavior(PCB_UI_SHOW_FRAME, (WPARAM)NULL, (LPARAM)1);
            }
        }
        break;
    case ID_SHOW_NAME:
        {
            HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_SHOW_NAME, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_SHOW_NAME, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_SHOW_NAME, (WPARAM)NULL, (LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_SHOW_NAME, MF_CHECKED);
                g_pfnUIBehavior(PCB_UI_SHOW_NAME, (WPARAM)NULL, (LPARAM)1);
            }
        }
        break;
    case ID_SHOW_RICH_TEXT_DEGE:
        {
            HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_SHOW_RICH_TEXT_DEGE, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_SHOW_RICH_TEXT_DEGE, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_SHOW_TEXT_ATOM_BOUNDARY, (WPARAM)NULL, (LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_SHOW_RICH_TEXT_DEGE, MF_CHECKED);
                g_pfnUIBehavior(PCB_UI_SHOW_TEXT_ATOM_BOUNDARY, (WPARAM)NULL, (LPARAM)1);
            }
        }
        break;
    case ID_ENABLE_RENDER_SAMPLER:
        {
            HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_ENABLE_RENDER_SAMPLER, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_ENABLE_RENDER_SAMPLER, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_ENABLE_RENDER_SAMPLER, (WPARAM)0,(LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_ENABLE_RENDER_SAMPLER, MF_CHECKED);
				g_pfnUIBehavior(PCB_UI_ENABLE_RENDER_SAMPLER, (WPARAM)1,(LPARAM)0);
            }
        }
        break;
    case ID_ENABLE_GLOBAL_BACK_BUFFER:
        {
            HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_ENABLE_GLOBAL_BACK_BUFFER, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_ENABLE_GLOBAL_BACK_BUFFER, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_ENABLE_GLOBAL_BACK_BUFFER, (WPARAM)0,(LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_ENABLE_GLOBAL_BACK_BUFFER, MF_CHECKED);
				g_pfnUIBehavior(PCB_UI_ENABLE_GLOBAL_BACK_BUFFER, (WPARAM)1,(LPARAM)0);
            }
        }
        break;
    case ID_SHOW_MOUSE_INFORMATION:
        {
            HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_SHOW_MOUSE_INFORMATION, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_SHOW_MOUSE_INFORMATION, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_SHOW_MOUSE_WND, (WPARAM)NULL, (LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_SHOW_MOUSE_INFORMATION, MF_CHECKED);
                g_pfnUIBehavior(PCB_UI_SHOW_MOUSE_WND, (WPARAM)NULL, (LPARAM)1);
            }
        }
        break;
    case ID_SHOW_FRAME_DRAG_AREA:
        {
            HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_SHOW_FRAME_DRAG_AREA, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_SHOW_FRAME_DRAG_AREA, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_SHOW_FRAME_DRAG_AREA_BODER, (WPARAM)NULL, (LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_SHOW_FRAME_DRAG_AREA, MF_CHECKED);
                g_pfnUIBehavior(PCB_UI_SHOW_FRAME_DRAG_AREA_BODER, (WPARAM)NULL, (LPARAM)1);
            }
        }
        break;
    case ID_UIEDITOR_MINIMAZE:
        {
			RECT rc;
			::GetWindowRect(m_hWnd, &rc);
            HMENU hMenu = GetMenu(m_hWnd);
			if (m_bLarge)
			{
                CheckMenuItem(hMenu, ID_UIEDITOR_MINIMAZE, MF_CHECKED);
				::MoveWindow(m_hWnd, rc.left, rc.top, m_sizeWindow.cx, 42, FALSE);
			}
			else
			{
                CheckMenuItem(hMenu, ID_UIEDITOR_MINIMAZE, MF_UNCHECKED);
				::MoveWindow(m_hWnd, rc.left, rc.top, m_sizeWindow.cx, m_sizeWindow.cy, TRUE);
			}
			m_bLarge = !m_bLarge;
        }
        break;
    case ID_DRAG_WND_AND_ITEM_MODE:
        {
            HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_DRAG_WND_AND_ITEM_MODE, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_DRAG_WND_AND_ITEM_MODE, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_DRAG_WND_AND_ITEM_MODE, (WPARAM)NULL, (LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_DRAG_WND_AND_ITEM_MODE, MF_CHECKED);
                g_pfnUIBehavior(PCB_UI_DRAG_WND_AND_ITEM_MODE, (WPARAM)NULL, (LPARAM)1);
            }
        }
        break;
    case ID_RESIZE_WND_AND_ITEM_MODE:
        {
            HMENU hMenu = GetMenu(m_hWnd);        
            if (GetMenuState(hMenu, ID_RESIZE_WND_AND_ITEM_MODE, MF_BYCOMMAND) == MF_CHECKED) 
            {
                CheckMenuItem(hMenu, ID_RESIZE_WND_AND_ITEM_MODE, MF_UNCHECKED);
                g_pfnUIBehavior(PCB_UI_RESIZE_WND_AND_ITEM_MODE, (WPARAM)NULL, (LPARAM)0);
            }
            else
            {
                CheckMenuItem(hMenu, ID_RESIZE_WND_AND_ITEM_MODE, MF_CHECKED);
                g_pfnUIBehavior(PCB_UI_RESIZE_WND_AND_ITEM_MODE, (WPARAM)NULL, (LPARAM)1);
            }
        }
        break;
	case ID_RESIZE_EDIT_MODE:
		{
			HMENU hMenu = GetMenu(m_hWnd);        
			if (GetMenuState(hMenu, ID_RESIZE_EDIT_MODE, MF_BYCOMMAND) == MF_CHECKED) 
			{
				CheckMenuItem(hMenu, ID_RESIZE_EDIT_MODE, MF_UNCHECKED);
				g_pfnUIBehavior(PCB_UI_EDIT_MODE, (WPARAM)NULL, (LPARAM)0);
			}
			else
			{
				CheckMenuItem(hMenu, ID_RESIZE_EDIT_MODE, MF_CHECKED);
				g_pfnUIBehavior(PCB_UI_EDIT_MODE, (WPARAM)NULL, (LPARAM)1);
			}
		}
		break;
    case IDC_LEAF_MOVE_UP:
        OnMoveWndUp();
        break;
    case IDC_LEAF_MOVE_DOWN:
        OnMoveWndDown();
        break;
	case IDC_LEAF_DEL:
		OnWndDel();
		break;
	case IDC_WND_NAME_CHANGE:
		OnWndNameChange();
		break;
	case IDC_WND_GET_POS_FORM_SHOWER:
        g_ClientManager.GetAllItemSizeAndPos();
        UpdateAll(m_szCurrentSelWndName, false, true);
		break;
    case IDC_WND_REFRESH:
        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        if (m_hWnd) 
        {
            if (ms_ErrorInfomation.bHasError) 
                ::SetDlgItemText(m_hWnd, IDC_WND_ERROR, ms_ErrorInfomation.szErrorMsg);
            else
                ::SetDlgItemText(m_hWnd, IDC_WND_ERROR, "Ok! No Error!");
        }
        break;
	case IDC_WND_RELOADIMAGE:
		g_ClientManager.ReloadImageFromFile();
		break;
	default:
		break;
	}
}

void KUiEditor::OnMoveWndUp()
{
    if (m_szCurrentWndUnit[0]) 
    {
        char szName[128] = "";
        strcpy(szName, m_szCurrentWndUnit);
        KSHOWWNDNODE *pNode = m_ShowWndTree.IsNodeExist(szName);
        if (pNode) 
        {
            m_ShowWndTree.MoveUp(pNode, &g_Ui);
            UpdateAll(szName, false, true);
        }     
    }
}

void KUiEditor::OnMoveWndDown()
{
    if (m_szCurrentWndUnit[0]) 
    {
        char szName[128] = "";
        strcpy(szName, m_szCurrentWndUnit);
        KSHOWWNDNODE *pNode = m_ShowWndTree.IsNodeExist(szName);
        if (pNode) 
        {
            m_ShowWndTree.MoveDown(pNode, &g_Ui);
            UpdateAll(szName, false, true);
        }     
    }

}

int KUiEditor::NormalizeIniData(IIniFile *pIni)
{
    //该函数用于去除Section；._Parent；WndScrollBar下的ThumbButtonSectionName，
    //WndScrollTreeList下的ListSectionName，ScrollSectionName；WndPageSet下的Page_n,Button_n
    //WndScrollMessageListBox下的MessageListBoxSectionName,ScrollSectionName
    //的值存在"[]"的问题。
    //如果 父窗口不存在并且不为特殊窗口Frame,该窗口将被删除。
    //没有父窗口将删除
    //如果有超过一个特殊窗口只保留第一个特殊窗口，其他特殊窗口及其相关的窗口都将被删除。
    //如果ini里面没有特殊窗口，所有窗口都将被删除。

    int nResult = false;
    KG_PROCESS_ERROR(pIni);
    int nRetCode = false;
    char szUnitName[128];
    szUnitName[0] = 0;
    char szNextUnitName[128];
    szNextUnitName[0] = 0;
    char szKeyName[128];
    szKeyName[0] = 0;
    char szKeyValue[128];
    szKeyValue[0] = 0;
    char szKeyNewValue[128];
    szKeyNewValue[0] = 0;
    int nKeyValue = 0;
    int i = 0;

	while(pIni->GetNextSection(szUnitName, szNextUnitName))
	{
        NormalizeIniKey(pIni, szNextUnitName, "._Parent");

        pIni->GetString(szNextUnitName, "._WndType", "", szKeyValue, sizeof(szKeyValue));
        if (!strcmp(szKeyValue, "WndScrollBar")) 
        {
            NormalizeIniKey(pIni, szNextUnitName, "ThumbButtonSectionName");
        }
        else if (!strcmp(szKeyValue, "WndScrollTreeList")) 
        {
            NormalizeIniKey(pIni, szNextUnitName, "ListSectionName");
            NormalizeIniKey(pIni, szNextUnitName, "ScrollSectionName");
        }
        else if (!strcmp(szKeyValue, "WndScrollMessageListBox")) 
        {
            NormalizeIniKey(pIni, szNextUnitName, "MessageListBoxSectionName");
            NormalizeIniKey(pIni, szNextUnitName, "ScrollSectionName");
        }
        else if (!strcmp(szKeyValue, "WndPageSet")) 
        {
            pIni->GetInteger(szNextUnitName, "PageCount", 0, &nKeyValue);
            for (i = 0 ; i < nKeyValue; i++)
            {
                szKeyName[0] = 0; 
                sprintf(szKeyName, "Page_%d", i);
                NormalizeIniKey(pIni, szNextUnitName, "ScrollSectionName");
                szKeyName[0] = 0; 
                sprintf(szKeyName, "CheckBox_%d", i);
                NormalizeIniKey(pIni, szNextUnitName, "ScrollSectionName");
            }
        }
        strcpy(szUnitName, szNextUnitName);
	}

    BOOL bHaveFrame     = FALSE;
    szUnitName[0]       = 0;
    szNextUnitName[0]   = 0;
    while (pIni->GetNextSection(szUnitName, szNextUnitName)) 
    {
        pIni->GetString(szNextUnitName, "._Parent", "", szKeyValue, sizeof(szKeyValue));
        if (!pIni->IsSectionExist(szKeyValue))
        {
            //父窗口不存在
            pIni->GetString(szNextUnitName, "._WndType", "", szKeyValue, sizeof(szKeyValue));
            if (!bHaveFrame && _tcsicmp(szKeyValue, g_GetWndTypeKey(UI_WND_FRAME)) == 0) 
            {
                //如果是Frame
                pIni->GetString(szNextUnitName, "._Parent", "", szKeyValue, sizeof(szKeyValue));
                if (!g_IsStringFrameLayer(szKeyValue))
                    pIni->WriteString(szNextUnitName, "._Parent", g_GetFrameLayerName(LAYER_NORMAL));
                bHaveFrame = TRUE;

                strcpy(szUnitName, szNextUnitName);
            }
            else
            {
                //如果不是Frame或者不是第一个Frame
                pIni->EraseSection(szNextUnitName);
                bHaveFrame = FALSE;
                szUnitName[0] = 0;
            }
        }
        else
        {
            //父窗口存在
            strcpy(szUnitName, szNextUnitName);
        }

    }

    if (!bHaveFrame) 
    {
        pIni->Clear();
    }

    nResult =true;
Exit0:
    return nResult;
}

int KUiEditor::NormalizeIniKey(IIniFile *pIni, const char *pszSectionName, const char *pszKeyName)
{
    int nResult = false;
    int nRetCode = false;
    KG_PROCESS_ERROR(pIni);
    KG_PROCESS_ERROR(pszSectionName);
    KG_PROCESS_ERROR(pszKeyName);

    char szKeyName[128];
    szKeyName[0] = 0;
    char szKeyValue[128];
    szKeyValue[0] = 0;
    char szKeyNewValue[128];
    szKeyNewValue[0] = 0;
    char *pszClip;

    nRetCode = pIni->GetString(pszSectionName, pszKeyName, "", szKeyValue, sizeof(szKeyValue));
    KG_PROCESS_ERROR(nRetCode);
    if (szKeyValue[0] == '[') 
    {
        strcpy(szKeyNewValue, szKeyValue + 1);
        pszClip = strrchr(szKeyNewValue, ']');
        if (pszClip) 
            *pszClip = 0;
        if (pIni->IsSectionExist(szKeyNewValue)) 
        {
            pIni->WriteString(pszSectionName, pszKeyName, szKeyNewValue);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KUiEditor::CopyWndAndChildWnd(IIniFile *pIn, LPCTSTR pszWndName, IIniFile *pOut, OUT LPTSTR pszNewWndName, LPCSTR pszParent)
{
    int nResult = FALSE;
    int nRetCode = false;
    int MaxWndMun = 0;
    int nWndNeedCopy = 0;

    struct COPYWND
    {
        char szName[128];
        int nParentIndex;
    };

    COPYWND *pNeedCopy = NULL;
    COPYWND *pCopyed = NULL;

    KG_PROCESS_ERROR(pIn);
    KG_PROCESS_ERROR(pOut);
    KG_PROCESS_ERROR(pszWndName && pszWndName[0]);
    KG_PROCESS_ERROR(pszParent && pszParent[0]);

    KG_PROCESS_ERROR(pszNewWndName);

    //获得所有子窗口列表
    nRetCode = pIn->IsSectionExist(pszWndName);
    KG_PROCESS_ERROR(nRetCode);

    //获得所有需要拷贝的窗口
    MaxWndMun = pIn->GetSectionCount();
    
    pNeedCopy = new COPYWND[MaxWndMun];

    strcpy(pNeedCopy[0].szName, pszWndName);
    pNeedCopy[0].nParentIndex = -1;
    nWndNeedCopy++;

    int i = 0;
    for (i = 0; i < nWndNeedCopy; i++) 
    {
        char szChild[128] = "";
        while (FindNextChildWnd(pIn, pNeedCopy[i].szName, szChild, pNeedCopy[nWndNeedCopy].szName)) 
        {
            pNeedCopy[nWndNeedCopy].nParentIndex = i;
            strcpy(szChild, pNeedCopy[nWndNeedCopy].szName);
            nWndNeedCopy++;
            _ASSERT(nWndNeedCopy <= MaxWndMun);
        }
        
    }

    //获得所有新产生的窗口名
    pCopyed = new COPYWND[nWndNeedCopy];
    for (i = 0; i < nWndNeedCopy; i++) 
    {
        strcpy(pCopyed[i].szName, pNeedCopy[i].szName);
        pCopyed[i].nParentIndex = pNeedCopy[i].nParentIndex;

        pCopyed[i].szName[110] = 0;

        char szNewName[128] = "";
        sprintf(szNewName, "%s", pCopyed[i].szName); //加copy后缀.

        int j = 0;
        while (pOut->IsSectionExist(szNewName)) 
        {
            sprintf(szNewName, "%s_%d", pCopyed[i].szName, j++);
        }

        strcpy(pCopyed[i].szName, szNewName);
    }


    //拷贝数据, todo：绑定窗口绑定的情况
    for (i = 0; i < nWndNeedCopy; i++) 
    {
        char szKey[128] = "";
        char szNextKey[128] = "";
        char szKeyValue[1024] = "";
        int  j = 0;

        while (pIn->GetNextKey(pNeedCopy[i].szName, szKey, szNextKey)) 
        {
            pIn->GetString(pNeedCopy[i].szName, szNextKey, "", szKeyValue, sizeof(szKeyValue));
            if (!strcmp(szNextKey, "._Parent")) 
            {
                if (i == 0) 
                {
                    strcpy(szKeyValue, pszParent);
                }
                else
                {
                    strcpy(szKeyValue, pCopyed[pCopyed[i].nParentIndex].szName);
                }
            }
            else
            {
                for (j = 0; j < nWndNeedCopy; j++) 
                {
                    if (
						(!strcmp(szKeyValue, pNeedCopy[j].szName)) && 
						strcmp(szNextKey, "._WndType")
					) 
                    {
                        strcpy(szKeyValue, pCopyed[j].szName);
                    }
                }
            }

            pOut->WriteString(pCopyed[i].szName, szNextKey, szKeyValue);
            strcpy(szKey, szNextKey);
        }
    }

    strcpy(pszNewWndName, pCopyed[0].szName);

    nResult = TRUE;
Exit0:
    if (pCopyed) 
    {
        delete[] pCopyed;
        pCopyed = NULL;
    }
    if(pNeedCopy)
    {
        delete[] pNeedCopy;
        pNeedCopy = NULL;
    }
    return nResult;
}

int KUiEditor::CopyAllChildWnd(IIniFile *pIn, LPCTSTR pszWndName, IIniFile *pOut, OUT LPTSTR pszNewWndName, LPCSTR pszParent)
{
    int nResult = FALSE;
    int nRetCode = false;

    KG_PROCESS_ERROR(pIn);
    KG_PROCESS_ERROR(pOut);
    KG_PROCESS_ERROR(pszWndName && pszWndName[0]);
    KG_PROCESS_ERROR(pszParent && pszParent[0]);

    KG_PROCESS_ERROR(pszNewWndName);

    nRetCode = pIn->IsSectionExist(pszWndName);
    KG_PROCESS_ERROR(nRetCode);

    //获得所有子窗口
    char szUnit[128] = "";
    char szNextUnit[128] = "";
    char szParent[128] = "";
    while (pIn->GetNextSection(szUnit, szNextUnit)) 
    {
        pIn->GetString(szNextUnit, "._Parent", "", szParent, sizeof(szParent));
        if (strcmp(szParent, pszWndName) == 0) 
        {
            nRetCode = CopyWndAndChildWnd(pIn, szNextUnit, pOut, pszNewWndName, pszParent);
            KG_PROCESS_ERROR(nRetCode);
        }
        strcpy(szUnit, szNextUnit);
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

int KUiEditor::FindNextChildWnd(IIniFile *pIn, LPCTSTR pszParent, LPCTSTR pszChild, LPTSTR pszNextChild)
{
    int nResult = FALSE;

    KG_PROCESS_ERROR(pIn);
    KG_PROCESS_ERROR(pszChild);
    KG_PROCESS_ERROR(pszNextChild);
    KG_PROCESS_ERROR(pszParent && pszParent[0]);

    char szWndName[128] = "";
    char szNextWndName[128] = "";
    char szKeyParent[128] = "";
    strcpy(szWndName, pszChild);

    while (pIn->GetNextSection(szWndName, szNextWndName)) 
    {
        pIn->GetString(szNextWndName, "._Parent", "", szKeyParent, sizeof(szKeyParent));
        if (!strcmp(szKeyParent, pszParent)) 
        {
            nResult = TRUE;
            strcpy(pszNextChild, szNextWndName);
            break;
        }

        strcpy(szWndName, szNextWndName);
    }
    
Exit0:
    return nResult;
}

int KUiEditor::GetFrameWndName(IIniFile *pIni, OUT LPTSTR pszFrameName)
{
    int nResult  = false;
    char szSection[128] = "";
    char szNextSection[128] = "";
    char szKey[128] = "";

    KG_PROCESS_ERROR(pIni);
    KG_PROCESS_ERROR(pszFrameName);

    while (pIni->GetNextSection(szSection, szNextSection)) 
    {
        pIni->GetString(szNextSection, "._WndType", "", szKey, sizeof(szKey));
        if(_tcsicmp(szKey, g_GetWndTypeKey(UI_WND_FRAME)) == 0)
        {
            nResult = true;
            strcpy(pszFrameName, szNextSection);
            break;
        }
        strcpy(szSection, szNextSection);
    }

Exit0:
    return nResult;
}

void KUiEditor::OnWndNameChange()
{
    char	szNewName[128] = "";
    char    szUnitName[128] = "";
    char    szUnitParentName[128] = "";

    ::GetDlgItemText(m_hWnd, IDC_WND_NAME, szNewName, sizeof(szNewName));
	if (!m_szCurrentWndUnit[0] || !szNewName[0] || !strcmp(szNewName, m_szCurrentWndUnit))
	{
		return;
	}

	if (!g_Ui.RenameUnit(m_szCurrentWndUnit, szNewName))
        return;

	UpdateAll(szNewName, false, true);
}

void KUiEditor::OnClear()
{
	g_Ui.Clear();
	g_ClientManager.Clear();
    m_ShowWndTree.ClearTree();
	::SetDlgItemText(m_hWnd, IDC_UI_NAME, "");
	TreeView_DeleteAllItems(m_hTreeWnd);
	ClearDetailInfo();
}

void KUiEditor::OnUiOpenIni()
{
    if (!strcmp(m_szCurrentFile, "New.ini")) 
    {
        MessageBox(m_hWnd, "该新建文件没有保存，清先保存", "提示：", MB_OK | MB_ICONINFORMATION);
        return;
    }

	char	szFile[MAX_PATH] = "";
	if (m_Settings.m_szLastUiIniPath[0])
	{
		sprintf(szFile, "%s\\%s", m_Settings.m_szLastUiIniPath, m_szCurrentFile);
		ShellExecute(NULL, "open", szFile, NULL, m_Settings.m_szLastUiIniPath, SW_SHOW);
	}
}

void KUiEditor::OnUiSel()
{
	OPENFILENAME	ofn;
	char			szFile[MAX_PATH] = "";
	szFile[0] = '\0';
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR |
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = "请选择界面配置文件";
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = "界面配置ini文件\0*.ini\0\0";
	if (m_Settings.m_szLastUiIniPath[0])
		ofn.lpstrInitialDir = m_Settings.m_szLastUiIniPath;
	if (!GetOpenFileName(&ofn))
		return;
	OnClear();
	char* pSplit = strrchr(szFile, '\\');
	*pSplit = 0;
	strcpy(m_Settings.m_szLastUiIniPath, szFile);
	*pSplit = '\\';

    strcpy(m_szCurrentFile, pSplit + 1);
    ::SetWindowText(m_hWnd, m_szCurrentFile);

	g_Ui.Load(szFile);

	UpdateAll("", false, true);
}

void KUiEditor::OnUiSave()
{
	char	szFile[128] = "", szFullName[MAX_PATH];
    char    szUnitName[512] = "";
    char    szUnitTypeName[512] = "";
    char    szKeyData[512] = "";
    HANDLE  hFile = NULL;
    WIN32_FIND_DATAA FindData;
    ZeroMemory(&FindData, sizeof(FindData));

	::GetDlgItemText(m_hWnd, IDC_UI_NAME, szFile, sizeof(szFile));

    if (!strcmp(m_szCurrentFile, "New.ini")) 
    {
        OnUiSaveAs();
        return;
    }

    strcpy(szFile, m_szCurrentFile);

	char*	pExt = strrchr(szFile, '.');
	if (!pExt || stricmp(pExt, ".ini"))
    {
        OnUiSaveAs();
        return;
    }

	if (m_Settings.m_szLastUiIniPath[0])
	{
		strcpy(szFullName, m_Settings.m_szLastUiIniPath);
		strcat(szFullName, "\\");
		strcat(szFullName, szFile);
	}
	else
	{
		OnUiSaveAs();
        return;
	}

    m_ShowWndTree.NomalrizeIni(&g_Ui);
	if (!g_Ui.Save(szFullName))
    {
       OnUiSaveAs();
       return;
    }

    strcat(szFullName, "。文件已经保存！");
    ::SetDlgItemText(m_hWnd, IDC_WND_ERROR, szFullName);
}

void KUiEditor::OnUiSaveAs()
{
	OPENFILENAME	ofn;
	char			szFile[MAX_PATH] = "";
	szFile[0] = 0;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = "请选择需要保存的界面配置文件";
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = "界面配置ini文件\0*.ini\0\0";
	if (m_Settings.m_szLastUiIniPath[0])
		ofn.lpstrInitialDir = m_Settings.m_szLastUiIniPath;
	if (!GetSaveFileName(&ofn))
		return;

	char* pSplit = strrchr(szFile, '.');
    if (pSplit == NULL) 
    {
        strcat(szFile, ".ini");
    }
    pSplit = strrchr(szFile, '\\');
	*pSplit = 0;
	strcpy(m_Settings.m_szLastUiIniPath, szFile);
	*pSplit = '\\';

    m_ShowWndTree.NomalrizeIni(&g_Ui);
	if (!g_Ui.Save(szFile))
    {
        MessageBox(m_hWnd, szFile, "存盘失败", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        strcpy(m_szCurrentFile, pSplit + 1);
        ::SetWindowText(m_hWnd, m_szCurrentFile);

    }
}

//根据g_Ui的内容作全部更新，包括窗口列表编辑器界面，g_WndTree等
void KUiEditor::UpdateAll(const char* pszDetailWnd, bool bForceReCreate, bool bUpdataInClient)
{
    char szWndUpdata[128];
    KSHOWWNDNODE* pNode = NULL;
    KG_PROCESS_ERROR(pszDetailWnd);
    strncpy(szWndUpdata, pszDetailWnd, sizeof(szWndUpdata));

    if(bUpdataInClient)
	    g_ClientManager.Clear();
	ClearDetailInfo();
    CreatShowTreeWnd();
    
	if (!szWndUpdata[0])
	{
        char szNext[128];
        szNext[0] = '\0';		
		while (g_Ui.GetNextUnit(szNext))
        {
            strncpy(szWndUpdata, szNext, sizeof(szWndUpdata));
        }
	}

    KG_PROCESS_ERROR(szWndUpdata[0]);
    pNode = m_ShowWndTree.IsNodeExist(szWndUpdata);
    KG_PROCESS_ERROR(pNode);
    m_ShowWndTree.Expand(m_hTreeWnd, pNode);
    FillDetailInfo(szWndUpdata, bForceReCreate, bUpdataInClient);
    TreeView_SelectItem(m_hTreeWnd, pNode->hItem);

Exit0:
    return;
}

int KUiEditor::CreatShowTreeWnd()
{
    int nResult = FALSE;
    IIniFile *pIni = g_Ui.GetIni();
    KG_PROCESS_ERROR(pIni);
    KG_PROCESS_ERROR(m_hTreeWnd);

    m_ShowWndTree.ClearTree();
    m_ShowWndTree.BuildTree(pIni);
    m_ShowWndTree.BuildUpShowWnd(m_hTreeWnd);
  
    nResult = TRUE;
Exit0:
    return nResult;
}

void KUiEditor::ClearDetailInfo()
{
	m_szCurrentWndUnit[0] = 0;
	::SetDlgItemText(m_hWnd, IDC_WND_NAME, "");
	::SetDlgItemText(m_hWnd, IDC_WND_TYPE, "");
	::SetDlgItemText(m_hWnd, IDC_WND_REMARK, "");
	if (m_pDetailInfoWnd)
		m_pDetailInfoWnd->Show(false);

    ClearTab();
}

void KUiEditor::FillDetailInfo(const char* pszWndUnit, bool bForceReCreate, bool bUpdataInClient)
{
	KUiWndWindowData	wnd;
	char				szType[32];

	if (pszWndUnit[0])
	{
		ClearDetailInfo();
		strcpy(m_szCurrentWndUnit, pszWndUnit);
        ::SetDlgItemText(m_hWnd, IDC_WND_NAME, m_szCurrentWndUnit);
	}

	if (!g_Ui.GetUnitBaseData(m_szCurrentWndUnit, wnd))
		return;

	strcpy(szType, g_GetWndTypeString(wnd.Type));
	::SetDlgItemText(m_hWnd, IDC_WND_TYPE, szType);
	if (CreateDetailInfoWnd(wnd.Type, m_szCurrentWndUnit, bForceReCreate))
    {
        g_ClientManager.EnableUpDataData(false);
        m_pDetailInfoWnd->UpdateData(m_szCurrentWndUnit, wnd, g_Ui.GetIni());
        g_ClientManager.EnableUpDataData(true);
        if(bUpdataInClient)
            g_ClientManager.UpDataWndInClient(g_Ui.GetIni());
        if (m_hWnd) 
        {
            if (ms_ErrorInfomation.bHasError) 
                ::SetDlgItemText(m_hWnd, IDC_WND_ERROR, ms_ErrorInfomation.szErrorMsg);
            else
                ::SetDlgItemText(m_hWnd, IDC_WND_ERROR, "Ok! No Error!");
        }
    }
	::SetDlgItemText(m_hWnd, IDC_WND_REMARK, wnd.Comment);
}

void KUiEditor::InitWindowShow(RECT *pRc, const char *pszName)
{
    KG_PROCESS_ERROR(m_pDetailInfoWnd);
    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(pszName && pszName[0]);
    KG_PROCESS_ERROR(pRc);

	if (m_pDetailInfoWnd->Init(m_hWnd, pRc, pszName))
    {
        m_pDetailInfoWnd->FillPageInfomation(&m_PageInfomation);
        ReBulidTab();
    }
    else
    {
		m_pDetailInfoWnd->Release();
        m_pDetailInfoWnd = NULL;
	}

Exit0:
    return;
}

int	KUiEditor::CreateDetailInfoWnd(UI_WND_TYPE eType, char * pszName, bool bForceReCreate)
{
	if (m_pDetailInfoWnd)
	{
		if (strcmp(m_pDetailInfoWnd->SectionName(), pszName) != 0 || 
            bForceReCreate
        )
		{
			m_pDetailInfoWnd->Release();
			m_pDetailInfoWnd = NULL;
		}
		else
		{
            ReBulidTab();
			m_pDetailInfoWnd->Show(true);
			return true;
		}
	}

	RECT	rc1, rc2;
	HWND	hFrame = ::GetDlgItem(m_hWnd, IDC_DETAIL_INFO);
	::GetWindowRect(hFrame, &rc1);
	::GetWindowRect(m_hWnd, &rc2);
	rc1.left -= rc2.left;
	rc1.right -= rc2.left;
	rc1.top -= rc2.top + 40;
	rc1.bottom -= rc2.top + 40;
	switch(eType)
	{
	case UI_WND_WINDOW:
		m_pDetailInfoWnd = KUiWndWindowCommon::Create();
		break;
	case UI_WND_IMAGE:
		m_pDetailInfoWnd = KUiWndImage::Create();
		break;
	case UI_WND_BUTTON:
		m_pDetailInfoWnd = KUiWndButton::Create();
        break;
	case UI_WND_SCROLLBAR:
        m_pDetailInfoWnd = KUiWndNewScrollBar::Create();
		break;
	case UI_WND_EDIT:
		m_pDetailInfoWnd = KUiWndEdit::Create();
		break;
	case UI_WND_PAGE:
		m_pDetailInfoWnd = KUiWndPage::Create();
		break;
	case UI_WND_PAGESET:
		m_pDetailInfoWnd = KUiWndPageSet::Create();
		break;
	case UI_WND_MINIMAP:
		m_pDetailInfoWnd = KUiWndMinimap::Create();
		break;
	case UI_WND_SCENE:
		m_pDetailInfoWnd = KUiWndScene::Create();
		break;		
	case UI_WND_WEBPAGE:
		m_pDetailInfoWnd = KUiWndWebPage::Create();
		break;		
    case UI_WND_FRAME:
        m_pDetailInfoWnd = KUiWndFrame::Create();
        break;
    case UI_WND_CHECKBOX:
        m_pDetailInfoWnd = KUiWndCheckBox::Create();
        break;
	case UI_WND_MODEL:
	case UI_WND_TEXTURE:
		// TODO : 临时
		m_pDetailInfoWnd = KUiWndWindowCommon::Create();
		break;
    case UI_ITEM_HANDLE:
        m_pDetailInfoWnd = KUiItemHandle::Create();
        break;
    case UI_ITEM_NULL:
        m_pDetailInfoWnd = KUiItemCommon::Create();
        break;
    case UI_ITEM_TEXT:
        m_pDetailInfoWnd = KUiItemText::Create();
        break;
    case UI_ITEM_IMAGE:
        m_pDetailInfoWnd = KUiItemImage::Create();
        break;
    case UI_ITEM_ANIAMTE:
        m_pDetailInfoWnd = KUiItemAnimate::Create();
        break;
	case UI_ITEM_SCENE:
		m_pDetailInfoWnd = KUiItemScene::Create();
		break;
	case UI_ITEM_BOX:
		m_pDetailInfoWnd = KUiItemBox::Create();
		break;
    case UI_ITEM_SHADOW:
        m_pDetailInfoWnd = KUiItemShadow::Create();
        break;
    case UI_ITEM_TREE_LEAFE:
        m_pDetailInfoWnd = KUiItemTreeLeaf::Create();
        break;
	}

    if (m_pDetailInfoWnd) 
    {
        InitWindowShow(&rc1, pszName);
    }

	return (m_pDetailInfoWnd != NULL);
}

void KUiEditor::OnTabNotify(LPNMHDR pNotify)
{
    KG_PROCESS_ERROR(m_hWndTab);
    KG_PROCESS_ERROR(m_pDetailInfoWnd);
    
    int nPage = TabCtrl_GetCurSel(m_hWndTab);

    m_pDetailInfoWnd->Show(true, nPage);

Exit0:
    return;

}

void KUiEditor::ReBulidTab()
{
    KG_PROCESS_ERROR(m_hWndTab);
    ClearTab();

    TCITEM PageAblity;
    ZeroMemory(&PageAblity, sizeof(TCITEM));

    int i = 0;
    for (i = 0; i < m_PageInfomation.m_PageCount; i++) 
    {
        PageAblity.mask = TCIF_TEXT;
        PageAblity.pszText = m_PageInfomation.m_szPage[i];
        PageAblity.cchTextMax = 32;
        TabCtrl_InsertItem(m_hWndTab, i, &PageAblity);
    }
Exit0:
    return;

}

void KUiEditor::ClearTab()
{
    KG_PROCESS_ERROR(m_hWndTab);
    TabCtrl_DeleteAllItems(m_hWndTab);

Exit0:
    return;
}


void KUiEditor::CreateNewWndByType(IN UI_WND_TYPE eType, IN const char *pPrefixTex, OUT char *pNewWndName, IN const char *pParent)
{
	TCHAR szName[128] = "";
	int nCounter = 0;
	do
	{
        _sntprintf(szName, sizeof(szName), _T("%s_New_%d"), pPrefixTex, nCounter++);
	} 
	while (g_Ui.GetIni()->IsSectionExist(szName));

	++m_nNewWndCounter;

	SetDefaultData(szName, eType);

    if(pParent)
    {
        g_Ui.GetIni()->WriteString(szName, "._Parent", pParent);
    }

    strcpy(pNewWndName, szName);
}

void KUiEditor::CreateWnd(
    IN UI_WND_TYPE eType, IN const char *pszParent /* = NULL */, OUT char *pszCreatedWnd /* = NULL */
)
{
	char szName[128];
    int nCounter = 0;
    const char *pszPrefixTex = g_GetWndTypePrefix(eType);

    KG_PROCESS_ERROR(pszPrefixTex);
	
	do
	{
        _sntprintf(szName, sizeof(szName), _T("%s_New_%d"), pszPrefixTex, nCounter++);
	} 
	while (g_Ui.GetIni()->IsSectionExist(szName));

	++m_nNewWndCounter;

	SetDefaultData(szName, eType);

    if (pszParent)
        g_Ui.GetIni()->WriteString(szName, "._Parent", pszParent);

    if (pszCreatedWnd)
        strcpy(pszCreatedWnd, szName);

    UpdateAll(szName, false, true);
Exit0:
    return;
}

//when create a new type of unit,you must add the defaul data to avoid confusion.
void KUiEditor::SetDefaultData(const char * pszUnitName, const UI_WND_TYPE cnType)
{
    char szCurrentKey[128] = "";
    char szNextKey[128] = "";
    char szKeyValue[1024] = "";
    char szSectionName[128] = "";

    if (cnType == UI_WND_FRAME) 
    {
        _ASSERT(g_Ui.GetFrameUnit(szSectionName) == FALSE);
        g_Ui.GetIni()->WriteString(pszUnitName, "._WndType", g_GetWndTypeKey(cnType));
        g_Ui.GetIni()->WriteString(pszUnitName, "._Parent", g_GetFrameLayerName(LAYER_NORMAL));
    }
    else
    {
        _ASSERT(g_Ui.GetFrameUnit(szSectionName) == TRUE);
        g_Ui.GetIni()->WriteString(pszUnitName, "._WndType", g_GetWndTypeKey(cnType));
        g_Ui.GetIni()->WriteString(pszUnitName, "._Parent", szSectionName);
    }
    
    szSectionName[0] = 0;
    strcat(szSectionName, g_GetWndTypeKey(cnType));
    strcpy(szCurrentKey, "._Parent");
    if (m_pSettingIniFile == NULL) 
        return;
    while (m_pSettingIniFile->GetNextKey(szSectionName, szCurrentKey, szNextKey)) 
    {
        m_pSettingIniFile->GetString(szSectionName, szNextKey, "", szKeyValue, sizeof(szKeyValue));
        g_Ui.GetIni()->WriteString(pszUnitName, szNextKey, szKeyValue);
        strcpy(szCurrentKey, szNextKey);
    }
}

void KUiEditor::OnWndDel()
{
	if (!m_szCurrentWndUnit[0])
		return;
    char szNextUnit[128] = "";
    g_Ui.GetIni()->GetNextSection(m_szCurrentWndUnit, szNextUnit);

	g_Ui.RemoveUnit(m_szCurrentWndUnit);

    NormalizeIniData(g_Ui.GetIni());

    if (!g_Ui.GetIni()->IsSectionExist(szNextUnit)) 
    {
        szNextUnit[0] = 0;
        while (g_Ui.GetNextUnit(szNextUnit)) { ; }
    }
	UpdateAll(szNextUnit, false, true);
}


void KUiEditor::GetSettingFileName(LPTSTR szPath)
{
	g_pfnUIBehavior(PCB_UI_GET_ROOT_PATH, (WPARAM)szPath, 0);
	_tcscat(szPath, _T("\\plugin\\UiEditor.ini"));
}

void KUiEditor::LoadSetting()
{
	UIPROJECTMANAGE::InitProjectManage();
	UIWINDOWMANAGE::InitWindowManage();

	char	szBuffer[MAX_PATH] = "";
	GetSettingFileName(szBuffer);

    m_pSettingIniFile = g_OpenIniFile(szBuffer);
    if (m_pSettingIniFile && g_pfnUIBehavior)
	{
        NormalizeIniData(m_pSettingIniFile);
		int	x = 0, y = 0;
		if (m_hWnd && m_pSettingIniFile->GetInteger2("Wnd", "LastPos", &x, &y))
		{
			::SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE);
		}

		m_pSettingIniFile->GetString("PathOption", "UiIniPath", "", m_Settings.m_szLastUiIniPath, sizeof(m_Settings.m_szLastUiIniPath));
		m_pSettingIniFile->GetString("PathOption", "WndImagePath", "", KUiWndImageCommon::ms_szLastImgPath, sizeof(KUiWndImageCommon::ms_szLastImgPath));

		int nValue = 0;
        HMENU hMenu = GetMenu(m_hWnd);

		m_pSettingIniFile->GetInteger("Preference", "ShowMainFrame", 1, &nValue);
        m_bShowMainUI = nValue ? true : false;
		if (!m_bShowMainUI)
			g_pfnUIBehavior(PCB_UI_LOAD_MAIN_UI, (WPARAM)nValue, (LPARAM)NULL);
		CheckMenuItem(hMenu, ID_SHOW_MAIN_UI, nValue ? MFS_CHECKED : MF_UNCHECKED);

		m_pSettingIniFile->GetInteger("Preference", "EnableRenderUI", 1, &nValue);
		g_pfnUIBehavior(PCB_UI_ENABLE_RENDER_UI, (WPARAM)NULL, (LPARAM)nValue);
        CheckMenuItem(hMenu, ID_ENABLE_RENDER_UI, nValue ? MFS_CHECKED : MF_UNCHECKED);

		m_pSettingIniFile->GetInteger("Preference", "ShowFrame", 0, &nValue);
		g_pfnUIBehavior(PCB_UI_SHOW_FRAME, (WPARAM)NULL, (LPARAM)nValue);
        CheckMenuItem(hMenu, ID_SHOW_EDGE, nValue ? MFS_CHECKED : MF_UNCHECKED);

		m_pSettingIniFile->GetInteger("Preference", "ShowName", 0, &nValue);
		g_pfnUIBehavior(PCB_UI_SHOW_NAME, (WPARAM)NULL, (LPARAM)nValue);
        CheckMenuItem(hMenu, ID_SHOW_NAME, nValue ? MFS_CHECKED : MF_UNCHECKED);

		m_pSettingIniFile->GetInteger("Preference", "ShowFrameDragArea", 0, &nValue);
		g_pfnUIBehavior(PCB_UI_SHOW_FRAME_DRAG_AREA_BODER, (WPARAM)NULL, (LPARAM)nValue);
        CheckMenuItem(hMenu, ID_SHOW_FRAME_DRAG_AREA, nValue ? MFS_CHECKED : MF_UNCHECKED);

		m_pSettingIniFile->GetInteger("Preference", "ShowMouseWnd", 0, &nValue);
		g_pfnUIBehavior(PCB_UI_SHOW_MOUSE_WND, (WPARAM)NULL, (LPARAM)nValue);
		CheckMenuItem(hMenu, ID_SHOW_MOUSE_INFORMATION, nValue ? MFS_CHECKED : MF_UNCHECKED);


		m_pSettingIniFile->GetInteger("Preference", "ShowTextAtomBoundary", 0, &nValue);
		g_pfnUIBehavior(PCB_UI_SHOW_TEXT_ATOM_BOUNDARY, (WPARAM)NULL, (LPARAM)nValue);
		CheckMenuItem(hMenu, ID_SHOW_RICH_TEXT_DEGE, nValue ? MFS_CHECKED : MF_UNCHECKED);

        // TODO: remove
		m_pSettingIniFile->GetInteger("Preference", "EnableRenderSampler", 0, &nValue);
		g_pfnUIBehavior(PCB_UI_ENABLE_RENDER_SAMPLER, (WPARAM)nValue, 0);
		CheckMenuItem(hMenu, ID_ENABLE_RENDER_SAMPLER, nValue ? MFS_CHECKED : MF_UNCHECKED);

		m_pSettingIniFile->GetInteger("Preference", "BindScript", 0, &nValue);
        g_ClientManager.BindScript(nValue, g_Ui.GetIni());
		CheckMenuItem(hMenu, ID_UIEDITOR_ADD_LUA, nValue ? MFS_CHECKED : MF_UNCHECKED);

		m_pSettingIniFile->GetInteger("Preference", "ClientSelWnd", 0, &nValue);
        g_pfnUIBehavior(PCB_UI_ENABLE_SEL_WND_IN_CLIENT, (WPARAM)NULL, (LPARAM)nValue);
		CheckMenuItem(hMenu, ID_CHOSE_WND_FROM_CLIENT, nValue ? MFS_CHECKED : MF_UNCHECKED);

		m_pSettingIniFile->GetInteger("Preference", "DragWndMode", 0, &nValue);
        g_pfnUIBehavior(PCB_UI_DRAG_WND_AND_ITEM_MODE, (WPARAM)NULL, (LPARAM)nValue);
		CheckMenuItem(hMenu, ID_DRAG_WND_AND_ITEM_MODE, nValue ? MFS_CHECKED : MF_UNCHECKED);

		m_pSettingIniFile->GetInteger("Preference", "ResizeWndMode", 0, &nValue);
        g_pfnUIBehavior(PCB_UI_RESIZE_WND_AND_ITEM_MODE, (WPARAM)NULL, (LPARAM)nValue);
		CheckMenuItem(hMenu, ID_RESIZE_WND_AND_ITEM_MODE, nValue ? MFS_CHECKED : MF_UNCHECKED);

		m_pSettingIniFile->GetInteger("Preference", "EditMode", 0, &nValue);
		g_pfnUIBehavior(PCB_UI_EDIT_MODE, (WPARAM)NULL, (LPARAM)nValue);
		CheckMenuItem(hMenu, ID_RESIZE_EDIT_MODE, nValue ? MFS_CHECKED : MF_UNCHECKED);
		

		m_pSettingIniFile->GetInteger("Preference", "Large", 1, &nValue);
		m_bLarge = nValue != 0 ? true : false;
		if (!m_bLarge)
		{
			RECT rc;
			::GetWindowRect(m_hWnd, &rc);
			::MoveWindow(m_hWnd, rc.left, rc.top, m_sizeWindow.cx, 42, FALSE);
		}
        CheckMenuItem(hMenu, ID_UIEDITOR_MINIMAZE, nValue ? MFS_UNCHECKED : MF_CHECKED);

        m_pSettingIniFile->GetMultiInteger("UserDefault", "UserSetColor", (int *)CustClr, 16);

	}
}

void KUiEditor::SaveSetting()
{
	if (!g_pfnUIBehavior)
	{
		return;
	}

	char	szBuffer[MAX_PATH] = "";
	GetSettingFileName(szBuffer);

    if (m_pSettingIniFile == NULL)
	{
        m_pSettingIniFile = g_CreateIniFile();
		if (m_pSettingIniFile)
		{
			m_pSettingIniFile->WriteString("TopParent", "._Parent", "Normal");
			m_pSettingIniFile->WriteString("TopParent", "._WndType", "WndFrame");

			m_pSettingIniFile->WriteString("Wnd", "._Parent", "TopParent");
			m_pSettingIniFile->WriteString("PathOption", "._Parent", "TopParent");
			m_pSettingIniFile->WriteString("Preference", "._Parent", "TopParent");
			m_pSettingIniFile->WriteString("UserDefault", "._Parent", "TopParent");
		}
	}
    if (m_pSettingIniFile == NULL) 
        return;

	if (m_hWnd)
	{
		RECT	rc;
		::GetWindowRect(m_hWnd, &rc);
		m_pSettingIniFile->WriteInteger2("Wnd", "LastPos", rc.left, rc.top);
	}

	if (m_Settings.m_szLastUiIniPath[0])
		m_pSettingIniFile->WriteString("PathOption", "UiIniPath", m_Settings.m_szLastUiIniPath);
	if (KUiWndImageCommon::ms_szLastImgPath[0])
		m_pSettingIniFile->WriteString("PathOption", "WndImagePath", KUiWndImageCommon::ms_szLastImgPath);

    HMENU hMenu = GetMenu(m_hWnd);
    m_pSettingIniFile->WriteInteger("Preference", "ShowMainFrame", 
        ::GetMenuState(hMenu, ID_SHOW_MAIN_UI, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
    m_pSettingIniFile->WriteInteger("Preference", "ShowFrame", 
        ::GetMenuState(hMenu, ID_SHOW_EDGE, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
	m_pSettingIniFile->WriteInteger("Preference", "ShowName", 
        ::GetMenuState(hMenu, ID_SHOW_NAME, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
    m_pSettingIniFile->WriteInteger("Preference", "ShowFrameDragArea", 
        ::GetMenuState(hMenu, ID_SHOW_FRAME_DRAG_AREA, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
	m_pSettingIniFile->WriteInteger("Preference", "ShowMouseWnd", 
        ::GetMenuState(hMenu, ID_SHOW_MOUSE_INFORMATION, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
	m_pSettingIniFile->WriteInteger("Preference", "ShowTextAtomBoundary", 
        ::GetMenuState(hMenu, ID_SHOW_RICH_TEXT_DEGE, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
    m_pSettingIniFile->WriteInteger("Preference", "BindScript", 
        ::GetMenuState(hMenu, ID_UIEDITOR_ADD_LUA, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
    m_pSettingIniFile->WriteInteger("Preference", "ClientSelWnd", 
        ::GetMenuState(hMenu, ID_CHOSE_WND_FROM_CLIENT, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
    m_pSettingIniFile->WriteInteger("Preference", "DragWndMode", 
        ::GetMenuState(hMenu, ID_DRAG_WND_AND_ITEM_MODE, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
    m_pSettingIniFile->WriteInteger("Preference", "ResizeWndMode", 
        ::GetMenuState(hMenu, ID_RESIZE_WND_AND_ITEM_MODE, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
	m_pSettingIniFile->WriteInteger("Preference", "EditMode", 
		::GetMenuState(hMenu, PCB_UI_EDIT_MODE, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);
	m_pSettingIniFile->WriteInteger("Preference", "EnableRenderUI", 
		::GetMenuState(hMenu, ID_ENABLE_RENDER_UI, MF_BYCOMMAND) == MF_CHECKED ? 1 : 0);

	m_pSettingIniFile->WriteInteger("Preference", "Large", m_bLarge ? 1 : 0);

    m_pSettingIniFile->WriteMultiInteger("UserDefault", "UserSetColor", (int *)CustClr, 16);

	m_pSettingIniFile->Save(szBuffer);
	
}
