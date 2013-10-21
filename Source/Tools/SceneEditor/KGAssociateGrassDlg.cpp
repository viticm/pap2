////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGAssociateGrassDlg.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-4-12 18:19:01
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "SceneEditor.h"
#include "KGAssociateGrassDlg.h"
#include "SceneEditorView.h"
#include "IEEditor.h"
#include "IEKG3DTerrain.h"

BEGIN_MESSAGE_MAP(KGGrassInfoModifyPane, KSceneSceneEditorDialogGrass)
    ON_WM_LBUTTONUP()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void KGGrassInfoModifyPane::OnLButtonUp(UINT nFlags, CPoint point)
{
    SetFocus();
    return KSceneSceneEditorDialogGrass::OnLButtonUp(nFlags, point);
}

HBRUSH KGGrassInfoModifyPane::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (pWnd == &m_StaticPreview)
        return static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));

    return KSceneSceneEditorDialogGrass::OnCtlColor(pDC, pWnd, nCtlColor);
}

void KGGrassInfoModifyPane::DoDataExchange(CDataExchange* pDX)
{
    KSceneSceneEditorDialogGrass::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_STATIC_TEXT, m_Lable_1);
    DDX_Control(pDX, IDC_STATIC_H, m_Lable_2);
    DDX_Control(pDX, IDC_STATIC_V, m_Lable_3);

    DDX_Control(pDX, IDC_BUTTON_REFRESHTEXTURE,   m_Button_1);
    DDX_Control(pDX, IDC_BUTTON_OPENGRASSTEXTURE, m_Button_2);
    DDX_Control(pDX, IDC_EDIT_HSPLIT,             m_Edit_1);
    DDX_Control(pDX, IDC_EDIT_VSPLIT,             m_Edit_2);

    DDX_Control(pDX, IDC_STATIC_WITH, m_Lable_A);
    DDX_Control(pDX, IDC_STATIC_HIGH, m_Lable_B);
    DDX_Control(pDX, IDC_STATIC_T,    m_Lable_C);
    DDX_Control(pDX, IDC_STATIC_A,    m_Lable_D);
    DDX_Control(pDX, IDC_STATIC_M,    m_Lable_E);
    //DDX_Control(pDX, IDC_STATIC_G,    m_Lable_G);

    DDX_Control(pDX, IDC_EDIT_WIDTHMIN,   m_Edit_A);
    DDX_Control(pDX, IDC_EDIT_WIDTHMAX,   m_Edit_B);
    DDX_Control(pDX, IDC_EDIT_HEIGHTMIN,  m_Edit_C);
    DDX_Control(pDX, IDC_EDIT_HEIGHTMAX,  m_Edit_D);
    DDX_Control(pDX, IDC_EDIT_TEXTUREMIN, m_Edit_E);
    DDX_Control(pDX, IDC_EDIT_TEXTUREMAX, m_Edit_F);
    DDX_Control(pDX, IDC_EDIT_ALPHA,      m_Edit_G);
    DDX_Control(pDX, IDC_EDIT_NUMBER,     m_Edit_H);

    DDX_Control(pDX, IDC_CHECK_ALLEY,       m_Button_3);
    DDX_Control(pDX, IDC_BUTTON_RANDOMMAP,  m_Button_4);
    DDX_Control(pDX, IDC_BUTTON_RANDOMPOLY, m_Button_5);

    DDX_Control(pDX, IDC_CHECK_GRASSADD,    m_Button_A);
    DDX_Control(pDX, IDC_CHECK_GRASSREMOVE, m_Button_B);
    DDX_Control(pDX, IDC_BUTTON_AUTOLIGHT,  m_Button_C);
    DDX_Control(pDX, IDC_BUTTON_FITGROUND,  m_Button_D);
 
    DDX_Control(pDX, IDC_BUTTON_REFRESH,    m_Button_E);

}


BOOL KGGrassInfoModifyPane::OnInitDialog()
{
    EnableSelfWeek(true);
    EnableSlip(true);

    BOOL bResult = KSceneSceneEditorDialogGrass::OnInitDialog();
    CWnd* pStatic = NULL;
    
    pStatic = GetDlgItem(IDC_STATIC1); 
    if (pStatic)
        pStatic->DestroyWindow();

    pStatic = GetDlgItem(IDC_STATIC2);
    if (pStatic)
        pStatic->DestroyWindow();

    pStatic = GetDlgItem(IDC_STATIC3);
    if (pStatic)
        pStatic->DestroyWindow();

    pStatic = GetDlgItem(IDC_STATIC4);
    if (pStatic)
        pStatic->DestroyWindow();

    pStatic = GetDlgItem(IDC_STATIC_G);
    if (pStatic)
        pStatic->DestroyWindow();

    Clear();

    m_nTopOffset = 5;

    KGLiliVirtualWnd vWnd_1(this);

    m_Button_1.ModifyStyle(0, BS_OWNERDRAW, SWP_DRAWFRAME);
    m_Button_2.ModifyStyle(0, BS_OWNERDRAW, SWP_DRAWFRAME);
    m_Button_C.ModifyStyle(0, BS_OWNERDRAW, SWP_DRAWFRAME);
    m_Button_D.ModifyStyle(0, BS_OWNERDRAW, SWP_DRAWFRAME);
    m_Button_E.ModifyStyle(0, BS_OWNERDRAW, SWP_DRAWFRAME);

    vWnd_1.AddElement(&m_StaticPreview);
    vWnd_1.AddElement(&m_Lable_1);
    vWnd_1.AddElement(&m_ComboxBoxGrassTexture);
    vWnd_1.AddElement(&m_Lable_2);
    vWnd_1.AddElement(&m_Lable_3);
    vWnd_1.AddElement(&m_Button_1);
    vWnd_1.AddElement(&m_Button_2);
    vWnd_1.AddElement(&m_Edit_1);
    vWnd_1.AddElement(&m_Edit_2);

    AddPanel(vWnd_1, TEXT("贴图"), TRUE);

    KGLiliVirtualWnd vWnd_2(this);

    vWnd_2.AddElement(&m_Lable_A);
    vWnd_2.AddElement(&m_Lable_B);
    vWnd_2.AddElement(&m_Lable_C);
    vWnd_2.AddElement(&m_Lable_D);
    vWnd_2.AddElement(&m_Lable_E);
    vWnd_2.AddElement(&m_Edit_A);
    vWnd_2.AddElement(&m_Edit_B);
    vWnd_2.AddElement(&m_Edit_C);
    vWnd_2.AddElement(&m_Edit_D);
    vWnd_2.AddElement(&m_Edit_E);
    vWnd_2.AddElement(&m_Edit_F);
    vWnd_2.AddElement(&m_Edit_G);
    vWnd_2.AddElement(&m_Edit_H);
    vWnd_2.AddElement(&m_Button_3);

    m_Button_4.ShowWindow(SW_HIDE);
    m_Button_5.ShowWindow(SW_HIDE);

    AddPanel(vWnd_2, TEXT("随机"), TRUE);

    KGLiliVirtualWnd vWnd_3(this);

    vWnd_3.AddElement(&m_Button_A);
    vWnd_3.AddElement(&m_Button_B);
    vWnd_3.AddElement(&m_Button_C);
    vWnd_3.AddElement(&m_Button_D);

    AddPanel(vWnd_3, TEXT("操作"), TRUE);

    KGLiliVirtualWnd vWnd_4(this);

    vWnd_4.AddElement(&m_ListBoxGrassPattern);
    vWnd_4.AddElement(&m_Button_E);

    AddPanel(vWnd_4, TEXT("模型草"), TRUE);
    return bResult;
}

BEGIN_MESSAGE_MAP(KGTopViewPane, KSceneEditorTopViewDialog)
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void KGTopViewPane::OnSetFocus(CWnd* pOldWnd)
{
    SetSelState();
    return KSceneEditorTopViewDialog::OnSetFocus(pOldWnd);
}

namespace {
    CProgressCtrl*   s_pProgress;
}

IMPLEMENT_DYNAMIC(KGAssociateGrassDlg, CDialog)

KGAssociateGrassDlg::KGAssociateGrassDlg(CWnd* pParent /*=NULL*/) 
    : CDialog(KGAssociateGrassDlg::IDD, pParent)
{
    s_pProgress = &m_Progress;
}

KGAssociateGrassDlg::~KGAssociateGrassDlg()
{
    //DeleteObject(m_hCursorArrow);
}

void KGAssociateGrassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_APPLAY, m_ButtonApply);
    DDX_Control(pDX, IDC_EDIT_MESSAGE, m_EditMessage);
    DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}


BEGIN_MESSAGE_MAP(KGAssociateGrassDlg, CDialog)
    ON_WM_CTLCOLOR()
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_APPLAY, &KGAssociateGrassDlg::OnBnClickedApplay)
END_MESSAGE_MAP()


HBRUSH KGAssociateGrassDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH Brush = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd == &m_EditMessage)
        pDC->SetTextColor(RGB(0, 0, 255));
    
    return Brush;
}

BOOL KGAssociateGrassDlg::PreTranslateMessage(MSG* pMsg)
{
     return CDialog::PreTranslateMessage(pMsg);
}

void KGAssociateGrassDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    
    static RECT rectClient;
    GetClientRect(&rectClient);
    
    RECT rectContrl;

    rectContrl = rectClient;
    rectContrl.left   += 280;
    rectContrl.top    += 10;
    rectContrl.right  -= 10;
    rectContrl.bottom -= 40;

    if (IsWindow(m_TopViewPane.m_hWnd))
        m_TopViewPane.MoveWindow(&rectContrl, true);

    rectContrl = rectClient;
    rectContrl.left   += 5;
   // rectContrl.top    += 10;
    rectContrl.right  = rectContrl.left + 250;
    rectContrl.bottom -= 10 ;

    if (IsWindow(m_GrassPane.m_hWnd))
        m_GrassPane.MoveWindow(&rectContrl, true);

    rectContrl.left   = rectClient.right - 100;
    rectContrl.top    = rectClient.bottom - 35;
    rectContrl.right  = rectClient.right - 10;
    rectContrl.bottom = rectClient.bottom - 5;

    if (IsWindow(m_ButtonApply.m_hWnd))
        m_ButtonApply.MoveWindow(&rectContrl, true);

    rectContrl.left   = rectClient.left + 280;
    rectContrl.top    = rectClient.bottom - 30;
    rectContrl.right  = rectClient.right - 150;
    rectContrl.bottom = rectClient.bottom - 5;

    if (IsWindow(m_ButtonApply.m_hWnd))
        m_EditMessage.MoveWindow(&rectContrl, true);

    if (IsWindow(m_Progress.m_hWnd))
        m_Progress.MoveWindow(&rectContrl, true);

}

void KGAssociateGrassDlg::OnSetFocus(CWnd* pOldWnd)
{
    m_TopViewPane.SetSelState();
    return CDialog::OnSetFocus(pOldWnd);
}

BOOL KGAssociateGrassDlg::OnInitDialog()
{
    BOOL bResult = CDialog::OnInitDialog();

    m_TopViewPane.SetSceneEditor(m_lpSceneEditor);
    m_GrassPane.SetSceneEditor(m_lpSceneEditor);

    m_TopViewPane.Create(IDD_SCENEEDITORTOPVIEW, this);
    SetWindowLong(m_TopViewPane.m_hWnd, GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_TABSTOP);
    SetWindowLong(m_TopViewPane.m_hWnd, GWL_EXSTYLE, WS_EX_STATICEDGE);
    m_TopViewPane.SetParent(this);
    m_TopViewPane.SetMenu(NULL);
    m_TopViewPane.ShowWindow(SW_SHOW);
    //SetClassLongPtr(m_TopViewPane.m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)m_hCursorArrow);
    m_TopViewPane.OnTopviewRegionSelection();

    m_GrassPane.Create(IDD_SCENEEDITOR_GRASS, this);
    SetWindowLong(m_GrassPane.m_hWnd, GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_TABSTOP);
    SetWindowLong(m_GrassPane.m_hWnd, GWL_EXSTYLE, 0);
    m_GrassPane.SetParent(this);
    m_GrassPane.SetMenu(NULL);
    m_GrassPane.ShowWindow(SW_SHOW);

    RECT rectClient;
    RECT rectContrl;
    GetClientRect(&rectClient);

    rectContrl = rectClient;
    rectContrl.left   += 280;
    rectContrl.top    += 10;
    rectContrl.right  -= 10;
    rectContrl.bottom -= 40;

    if (IsWindow(m_TopViewPane.m_hWnd))
        m_TopViewPane.MoveWindow(&rectContrl, true);

    rectContrl = rectClient;
    rectContrl.left   += 5;
   // rectContrl.top    += 10;
    rectContrl.right  = rectContrl.left + 250;
    rectContrl.bottom -= 10 ;

    if (IsWindow(m_GrassPane.m_hWnd))
        m_GrassPane.MoveWindow(&rectContrl, true);

    rectContrl.left   = rectClient.right - 100;
    rectContrl.top    = rectClient.bottom - 30;
    rectContrl.right  = rectClient.right - 10;
    rectContrl.bottom = rectClient.bottom - 5;

    if (IsWindow(m_ButtonApply.m_hWnd))
        m_ButtonApply.MoveWindow(&rectContrl, true);


    rectContrl.left   = rectClient.left + 280;
    rectContrl.top    = rectClient.bottom - 30;
    rectContrl.right  = rectClient.right - 150;
    rectContrl.bottom = rectClient.bottom - 5;

    if (IsWindow(m_ButtonApply.m_hWnd))
        m_EditMessage.MoveWindow(&rectContrl, true);

    if (IsWindow(m_Progress.m_hWnd))
    {
        m_Progress.MoveWindow(&rectContrl, true);
        m_Progress.SetRange32(0, 100);
        m_Progress.ShowWindow(SW_HIDE);
    }
    
    return bResult;
}

LRESULT KGAssociateGrassDlg::Progress(WPARAM wParam, LPARAM lParam)
{
    if (s_pProgress)
    {
        s_pProgress->SetPos(static_cast<int>(wParam) / static_cast<int>(lParam) * 100);
        s_pProgress->Invalidate();
        s_pProgress->UpdateWindow();
    }
    return 1;
}

namespace {
    UINT AutoFillThread(void* pParam) 
    {
        KGAssociateGrassDlg* pWnd = (KGAssociateGrassDlg*)pParam;

        pWnd->m_Progress.ShowWindow(SW_SHOW);
        pWnd->m_Progress.SetPos(0);
        pWnd->m_EditMessage.ShowWindow(SW_HIDE);

        int nResult = pWnd->m_pTerrainEx->AutoFillGrass(
            pWnd->m_nLayer, pWnd->m_nTextureIndex, pWnd->m_lpSceneEditor, KGAssociateGrassDlg::Progress
            );

        pWnd->m_Progress.SetPos(100);
        pWnd->m_Progress.ShowWindow(SW_HIDE);
        pWnd->m_EditMessage.ShowWindow(SW_SHOW);

        return 1;
    }
}
void KGAssociateGrassDlg::OnBnClickedApplay()
{
    if (!m_pTerrainEx)
        return;

    //m_Progress.ShowWindow(SW_SHOW);
    //m_Progress.SetPos(0);
    //m_EditMessage.ShowWindow(SW_HIDE);

    //AfxBeginThread(AutoFillThread, this);

    
    int nResult = m_pTerrainEx->AutoFillGrass(m_nLayer, m_nTextureIndex, m_lpSceneEditor, NULL/*Progress*/);

    //m_Progress.SetPos(100);
    //m_Progress.ShowWindow(SW_HIDE);
    //m_EditMessage.ShowWindow(SW_SHOW);

    switch (nResult)
    {
    case  1 :
        m_EditMessage.SetWindowText(TEXT("生成成功:)"));
        break;
    case  0 :
        m_EditMessage.SetWindowText(TEXT("生成出错 : 不知道什么原因 :("));
        break;
    case -1 :
        m_EditMessage.SetWindowText(TEXT("生成出错 : 你还没有选择regions :)"));
        break;
    case -2 :
        m_EditMessage.SetWindowText(TEXT("生成出错 : 你还没有选择草 :)"));
        break;
    case -5 :
        m_EditMessage.SetWindowText(TEXT("生成出错 : 没有此贴图的regions :)"));
        break;
    default :
        m_EditMessage.SetWindowText(TEXT("生成出错 : 不知道什么原因 :("));
        break;
    }
    
}