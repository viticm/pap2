// KPvsEditorAboutBox.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KPvsEditorAboutBox.h"


// KPvsEditorAboutBox dialog

IMPLEMENT_DYNAMIC(KPvsEditorAboutBox, CDialog)

KPvsEditorAboutBox::KPvsEditorAboutBox(CWnd* pParent /*=NULL*/)
	: CDialog(KPvsEditorAboutBox::IDD, pParent)
    , m_strText(_T(""))
{

}

KPvsEditorAboutBox::~KPvsEditorAboutBox()
{
}

void KPvsEditorAboutBox::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT, m_strText);
}


BEGIN_MESSAGE_MAP(KPvsEditorAboutBox, CDialog)
    ON_BN_CLICKED(IDOK, &KPvsEditorAboutBox::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL KPvsEditorAboutBox::OnInitDialog()
{
    BOOL res = CDialog::OnInitDialog();

    m_strText = "鼠标\r\n\r\n左键 : 选择物体\r\n\r\n右键 : 旋转镜头\r\n\r\n滚轮 : 缩放镜头\r\n\r\n\r\n"
        "键盘\r\n\r\nW : 移动状态\r\n\r\nE : 旋转状态\r\n\r\nR : 缩放状态\r\n\r\nQ : 恢复选择状态\r\n\r\n"
        "[ : 缩小手中物体\r\n\r\n[ : 放大手中物体\r\n\r\ndelete : 删除选种物体\r\n\r\n"
        "按住Shift键滚动鼠标滚轮 : 切换已经放置过的物件";

    UpdateData(FALSE);

    return res;
}
// KPvsEditorAboutBox message handlers

void KPvsEditorAboutBox::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    OnOK();
}
