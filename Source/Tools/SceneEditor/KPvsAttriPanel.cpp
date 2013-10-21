// KPvsAttriPanel.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KPvsAttriPanel.h"
#include "IEEditor.h"
#include "IEKG3DSceneSFXEditor.h"
#include "KPvsEditorToolSheet.h"
#include "KColorDialog.h"


// KPvsAttriPanel dialog

IMPLEMENT_DYNAMIC(KPvsAttriPanel, KGLilisSlipPanel<CPropertyPage>)

KPvsAttriPanel::KPvsAttriPanel()
	: KGLilisSlipPanel<CPropertyPage>(KPvsAttriPanel::IDD),
      m_strOW(TEXT("")),
      m_strIW(TEXT("")),
      m_bRegEvent(false)
{

}

KPvsAttriPanel::~KPvsAttriPanel()
{
    if (GetPvsScene())
        GetPvsScene()->UnRegistEventCallBack(this);
}

void KPvsAttriPanel::DoDataExchange(CDataExchange* pDX)
{
	KGLilisSlipPanel<CPropertyPage>::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_OW, m_strOW);
    DDX_Text(pDX, IDC_EDIT_IW, m_strIW);
    DDX_Control(pDX, IDC_TREE_SUB_OBJ, m_treeSub);
	DDX_Control(pDX, IDC_COLOR_PLIGHT, m_ColorWndPLight);
}


BEGIN_MESSAGE_MAP(KPvsAttriPanel, KGLilisSlipPanel<CPropertyPage>)
    ON_BN_CLICKED(IDC_BUTTON_IW, &KPvsAttriPanel::OnBnClickedButtonIw)
    ON_BN_CLICKED(IDC_BUTTON_OW, &KPvsAttriPanel::OnBnClickedButtonOw)
    ON_NOTIFY(NM_CLICK, IDC_TREE_SUB_OBJ, &KPvsAttriPanel::OnNMClickTreeSubObj)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SUB_OBJ, &KPvsAttriPanel::OnTvnSelchangedTreeSubObj)
    ON_WM_SIZE()
	ON_MESSAGE(WM_COLORPOINTER_CHANGE, OnColorChanged)
    ON_BN_CLICKED(IDC_CHECK_W, &KPvsAttriPanel::OnBnClickedCheckW)
    ON_BN_CLICKED(IDC_CHECK_N, &KPvsAttriPanel::OnBnClickedCheckN)
    ON_BN_CLICKED(IDC_BUTTON_DEL_W, &KPvsAttriPanel::OnBnClickedButtonDelW)
    ON_BN_CLICKED(IDC_BUTTON_DEL_N, &KPvsAttriPanel::OnBnClickedButtonDelN)
	ON_STN_CLICKED(IDC_COLOR_PLIGHT, &KPvsAttriPanel::OnStnClickedColorPlight)
	//ON_CBN_SELCHANGE(IDC_COMBO_LIGHTMAPSIZE, &KPvsAttriPanel::OnCbnSelchangeComboLightmapsize)
	//ON_BN_CLICKED(IDC_CHECK_LIGHTMAP, &KPvsAttriPanel::OnBnClickedCheckLightmap)
	ON_BN_CLICKED(IDC_BUTTON_AUTOSIZE, &KPvsAttriPanel::OnBnClickedButtonAutoLightMapsize)
	ON_BN_CLICKED(IDC_BUTTON_LIGHTMAP, &KPvsAttriPanel::OnBnClickedButtonComputeLightmap)
	ON_BN_CLICKED(IDC_BUTTON_COMPUTESELECTED, &KPvsAttriPanel::OnBnClickedButtonComputeselected)
	ON_BN_CLICKED(IDC_BUTTON_CALL, &KPvsAttriPanel::OnBnClickedButtonCall)
	ON_BN_CLICKED(IDC_BUTTON_CALL2, &KPvsAttriPanel::OnBnClickedButtonCall2)
	ON_BN_CLICKED(IDC_CHECK1, &KPvsAttriPanel::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK33, &KPvsAttriPanel::OnBnClickedCheck33)
END_MESSAGE_MAP()

IEKG3DScenePvsEditor* KPvsAttriPanel::GetPvsScene()
{
    if (!GetParent() || !GetParent()->GetParent())
        return NULL;

    KPvsEditorToolSheet* pSheet = static_cast<KPvsEditorToolSheet*>(GetParent()->GetParent());

    if (!pSheet)
        return NULL;

    return pSheet->GetPvsScene();
}

void KPvsAttriPanel::OnSize(UINT nType, int cx, int cy)
{
    if (IsWindow(m_treeSub) && m_vecChileWnd.size())
    {
        RECT rect;
        GetClientRect(&rect);

        rect.top += 100;
        rect.left += 10;
        rect.right -= 10;
        rect.bottom = rect.bottom - 150;
        m_treeSub.MoveWindow(&rect, TRUE);

        RecalcPaneSize();
    }

    UpdateUI();

    KGLilisSlipPanel<CPropertyPage>::OnSize(nType, cx, cy);
}

BOOL KPvsAttriPanel::OnSetActive()
{
    BOOL bResult = KGLilisSlipPanel<CPropertyPage>::OnSetActive();

    if (GetPvsScene())
    {
        IEKG3DRepresentPVS* pvs = GetPvsScene()->GetPvs();

        if (pvs)
        {
            LPCTSTR pName = NULL;

            IEKG3DModel* mdlow = pvs->GetOutsideBorderModel();
            IEKG3DModel* mdliw = pvs->GetInsideBorderModel();

            if (mdlow)
            {
                mdlow->GetName(&pName);
                m_strOW = pName;
            }

            if (mdliw)
            {
                mdliw->GetName(&pName);
                m_strIW = pName;
            }
        }
    }

    UpdateData(FALSE);
    UpdateSubTree();

    if (GetPvsScene() && !m_bRegEvent)
        GetPvsScene()->RegistEventCallBack(this);

    return bResult;

}

void KPvsAttriPanel::OnAddObject(IEKG3DRepresentObject* obj)
{
    TCHAR szName[128];
    obj->GetName(szName, sizeof(szName));

	IKG3DModel* pModel = NULL;
	obj->GetModel(pModel, 0);

	if (!pModel)
		return;

	HTREEITEM hRoot = TVI_ROOT;

	switch (pModel->GetType())
	{
	case MESHTYPE_DEFAULT:
		{
			hRoot = m_hModelRoot;
			break;
		}
	case MESHTYPE_POINTLIGHT :
		{
			hRoot = m_hLightRoot;
			break;
		}
	case MESHTYPE_SFX:
		{
			hRoot = m_hSFXRoot;
			break;
		}
	default :
		break;
	}

    HTREEITEM hItem = m_treeSub.InsertItem(szName,hRoot,TVI_LAST);
    m_treeSub.SetItemData(hItem, (DWORD_PTR)obj);
}

void KPvsAttriPanel::OnDelObject(IEKG3DRepresentObject* obj)
{
    OnDelObjectForNext(obj,TVI_ROOT);
}

void KPvsAttriPanel::OnDelObjectForNext(IEKG3DRepresentObject* obj,HTREEITEM hItemRoot)
{
	HTREEITEM hItem = m_treeSub.GetNextItem(hItemRoot, TVGN_CHILD);

	while (hItem)
	{
		IEKG3DRepresentObject* pObj = (IEKG3DRepresentObject*)m_treeSub.GetItemData(hItem);

		if (obj == pObj)
		{
			m_treeSub.DeleteItem(hItem);
			break;
		}

		OnDelObjectForNext(obj,hItem);

		hItem = m_treeSub.GetNextItem(hItem, TVGN_NEXT);
	}
}

void KPvsAttriPanel::OnObjModify(IEKG3DRepresentObject* obj)
{

}

BOOL KPvsAttriPanel::OnInitDialog()
{
    int nResult = KGLilisSlipPanel<CPropertyPage>::OnInitDialog();

    m_treeSub.SetTextColor(RGB(0, 0, 0));
    m_treeSub.SetLineColor(RGB(122 , 122, 122));
    m_treeSub.SetBkColor(RGB(255,  255, 255));

    UpdateSubTree();

    CheckDlgButton(IDC_CHECK_W, TRUE);
    CheckDlgButton(IDC_CHECK_N, TRUE);

	//ShowPanel(2, false);
	//ShowPanel(3, false);

    return nResult;
}

void KPvsAttriPanel::UpdateSubTree()
{
    if (!IsWindow(m_treeSub) || !GetPvsScene())
        return;

    IEKG3DRepresentPVS* pvs = GetPvsScene()->GetPvs();

    if (!pvs)
        return;

    m_treeSub.DeleteAllItems();

	m_hModelRoot = m_treeSub.InsertItem("模型");
	m_hSFXRoot   = m_treeSub.InsertItem("特效");
	m_hLightRoot = m_treeSub.InsertItem("灯光");

    DWORD dwCount = pvs->GetInsideObjectCount();

    for (DWORD i = 0; i < dwCount; ++i)
    {
        IEKG3DRepresentObject* obj = pvs->GetInsideObject(i);

        if (obj)
        {
			OnAddObject(obj);
        }
    }
}

void KPvsAttriPanel::OnBnClickedButtonIw()
{
    if (!GetPvsScene())
        return;

    IEKG3DRepresentPVS* pvs = GetPvsScene()->GetPvs();

    if (!pvs)
        return;

    char szFullPath[MAX_PATH];
    char szFilePath[MAX_PATH];

    sprintf(szFilePath, TEXT("data\\source\\pvs\\"));
    g_GetFullPath(szFullPath, szFilePath);

    char szFilters[] = "Mesh Files|*.Mesh|Mdl Files|*.Mdl||";

    CFileDialog fileDlg(
        TRUE,
        szFullPath,
        NULL,
        OFN_EXPLORER|OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
        szFilters,
        this
        );
    fileDlg.m_ofn.lStructSize = sizeof(fileDlg.m_ofn);
    fileDlg.m_ofn.lpstrTitle  = "选择内墙模型";
    fileDlg.m_ofn.lpstrInitialDir = szFullPath;

    if (fileDlg.DoModal() == IDOK)
    {
        m_strIW = fileDlg.GetPathName();

        TCHAR szRetPaht[MAX_PATH];
        if (g_GetFilePathFromFullPath(szRetPaht, m_strIW.GetBuffer()))
            m_strIW = szRetPaht;
        m_strIW.ReleaseBuffer();
        UpdateData(FALSE);

        pvs->SetInsideBorderModel(m_strIW.GetBuffer());
        m_strIW.ReleaseBuffer();
   
    }
}

void KPvsAttriPanel::OnBnClickedButtonOw()
{
    if (!GetPvsScene())
        return;

    IEKG3DRepresentPVS* pvs = GetPvsScene()->GetPvs();

    if (!pvs)
        return;

    char szFullPath[MAX_PATH];
    char szFilePath[MAX_PATH];

    sprintf(szFilePath, TEXT("data\\source\\pvs\\"));
    g_GetFullPath(szFullPath, szFilePath);

    char szFilters[] = "Mesh Files|*.Mesh|Mdl Files|*.Mdl||";

    CFileDialog fileDlg(
        TRUE,
        szFullPath,
        NULL,
        OFN_EXPLORER|OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
        szFilters,
        this
        );
    fileDlg.m_ofn.lStructSize = sizeof(fileDlg.m_ofn);
    fileDlg.m_ofn.lpstrTitle  = "选择外墙模型";
    fileDlg.m_ofn.lpstrInitialDir = szFullPath;

    if (fileDlg.DoModal() == IDOK)
    {
        m_strOW = fileDlg.GetPathName();

        TCHAR szRetPaht[MAX_PATH];
        if (g_GetFilePathFromFullPath(szRetPaht, m_strOW.GetBuffer()))
            m_strOW = szRetPaht;
        m_strOW.ReleaseBuffer();
        UpdateData(FALSE);

        pvs->SetOutsideBorderModel(m_strOW.GetBuffer());
        m_strOW.ReleaseBuffer();
    }
}

void KPvsAttriPanel::OnNMClickTreeSubObj(NMHDR *pNMHDR, LRESULT *pResult)
{
    //POINT pt;
    //
    //GetCursorPos(&pt);
    //m_treeSub.ScreenToClient(&pt);
    //
    //HTREEITEM hItem = m_treeSub.HitTest(pt);

    //if (m_pPvsScene && hItem)
    //{
    //    IEKG3DRepresentObject* pObj = (IEKG3DRepresentObject*)m_treeSub.GetItemData(hItem);
    //    m_pPvsScene->SelectObject(pObj);
    //}
    //
    *pResult = 0;
}

void KPvsAttriPanel::OnSelectedChanged()
{
	if (!GetPvsScene())
		return;

	IEKG3DRepresentObject* pObj = GetPvsScene()->GetFirstSelectedObj();

	if (!pObj)
		return;

	IKG3DModel* pModel = NULL;

	pObj->GetModel(pModel, 0);

	if (!pModel)
		return;

	switch (pModel->GetType())
	{
	case MESHTYPE_DEFAULT:
		{
			//ShowPanel(2,false);
			//ShowPanel(3);
			break;
		}
	case MESHTYPE_POINTLIGHT :
		{
			//ShowPanel(3, false);
			//ShowPanel(2);
			pModel->GetModelColor(0, &m_PointLightColor);
			m_ColorWndPLight.SetColor(D3DCOLOR_COLORVALUE(m_PointLightColor.r, m_PointLightColor.g, m_PointLightColor.b, m_PointLightColor.a));
		}

		break;
	default :
		//ShowPanel(3, false);
		//ShowPanel(2, false);
		break;
	}
}

void KPvsAttriPanel::OnTvnSelchangedTreeSubObj(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    if (GetPvsScene() && pNMTreeView->itemNew.hItem)
    {
        IEKG3DRepresentObject* pObj = (IEKG3DRepresentObject*)m_treeSub.GetItemData(pNMTreeView->itemNew.hItem);
        GetPvsScene()->SelectObject(pObj);
		OnSelectedChanged();
    }

    *pResult = 0;
}

void KPvsAttriPanel::UpdateAttri()
{
	if (GetPvsScene())
	{
		IEKG3DRepresentObject* pObj = GetPvsScene()->GetFirstSelectedObj();

		if (pObj->GetType() == REPRESENTOBJECT_POINTLIGHT)
		{
			int a = 0;
			++a;
		}
	}
}

void KPvsAttriPanel::OnBnClickedCheckW()
{
    if (!GetPvsScene())
        return;

    IEKG3DRepresentPVS* pvs = GetPvsScene()->GetPvs();

    if (!pvs)
        return;

    pvs->ShowOutsideShell(IsDlgButtonChecked(IDC_CHECK_W));
}

void KPvsAttriPanel::OnBnClickedCheckN()
{
    if (!GetPvsScene())
        return;

    IEKG3DRepresentPVS* pvs = GetPvsScene()->GetPvs();

    if (!pvs)
        return;

    pvs->ShowInsdieShell(IsDlgButtonChecked(IDC_CHECK_N));
}

void KPvsAttriPanel::OnBnClickedButtonDelW()
{
    if (!GetPvsScene())
        return;

    IEKG3DRepresentPVS* pvs = GetPvsScene()->GetPvs();

    if (!pvs)
        return;

    pvs->SetOutsideBorderModel(TEXT(""));
    m_strOW = TEXT("");
    UpdateData(FALSE);
}

void KPvsAttriPanel::OnBnClickedButtonDelN()
{
    if (!GetPvsScene())
        return;

    IEKG3DRepresentPVS* pvs = GetPvsScene()->GetPvs();

    if (!pvs)
        return;

    pvs->SetInsideBorderModel(TEXT(""));
    m_strIW = TEXT("");
    UpdateData(FALSE);
}

void KPvsAttriPanel::OnStnClickedColorPlight()
{
	if (!GetPvsScene())
		return;

	IEKG3DRepresentObject* pObj = GetPvsScene()->GetFirstSelectedObj();

	if (!pObj)
		return;

	IKG3DModel* pModel = NULL;

	pObj->GetModel(pModel, 0);

	if (!pModel)
		return;

	if (pModel->GetType() != MESHTYPE_POINTLIGHT)
		return;

	pModel->GetModelColor(0, &m_PointLightColor);
	GetColorPickerDialog()->Init(&m_PointLightColor, "Point Light Color (Alpha To Control Power)", this, TRUE);
}

LRESULT KPvsAttriPanel::OnColorChanged(WPARAM wParam, LPARAM lParam)
{
	if (!GetPvsScene())
		return 0;

	IEKG3DRepresentObject* pObj = GetPvsScene()->GetFirstSelectedObj();

	if (!pObj)
		return 0;

	IKG3DModel* pModel = NULL;

	pObj->GetModel(pModel, 0);

	if (!pModel)
		return 0;

	if (pModel->GetType() != MESHTYPE_POINTLIGHT)
		return 0;


	pModel->SetModelColor(0, m_PointLightColor);
	m_ColorWndPLight.SetColor(D3DCOLOR_COLORVALUE(m_PointLightColor.r, m_PointLightColor.g, m_PointLightColor.b, m_PointLightColor.a));

	return 1;
}
void KPvsAttriPanel::OnCbnSelchangeComboLightmapsize()
{
	// TODO: Add your control notification handler code here
}

void KPvsAttriPanel::OnBnClickedCheckLightmap()
{
	// TODO: Add your control notification handler code here
}

void KPvsAttriPanel::OnBnClickedButtonAutoLightMapsize()
{
	// TODO: Add your control notification handler code here
}

void KPvsAttriPanel::OnBnClickedButtonComputeLightmap()
{
	if (!GetPvsScene())
		return;

	IEKG3DRepresentPVS* pvs = GetPvsScene()->GetPvs();

	if (!pvs)
		return;

	GetPvsScene()->ComputePhotons();

}

void KPvsAttriPanel::OnBnClickedButtonComputeselected()
{
	if (!GetPvsScene())
		return;
	GetPvsScene()->BakeLightMap(FALSE,TRUE,TRUE);
}

void KPvsAttriPanel::OnBnClickedButtonCall()
{
	if (!GetPvsScene())
		return;
	GetPvsScene()->BakeLightMap(TRUE,TRUE,FALSE);
}

void KPvsAttriPanel::OnBnClickedButtonCall2()
{
	if (!GetPvsScene())
		return;
	GetPvsScene()->BakeLightMap(TRUE,FALSE,TRUE);
}

void KPvsAttriPanel::OnBnClickedCheck1()
{
	UpdateData(TRUE);
	int n = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();

	if (!GetPvsScene())
		return;
	GetPvsScene()->SetEnableShowLightSign(n);

}

void KPvsAttriPanel::OnBnClickedCheck33()
{
	UpdateData(TRUE);
	int n = ((CButton*)GetDlgItem(IDC_CHECK33))->GetCheck();

	if (!GetPvsScene())
		return;
	GetPvsScene()->SetEnableShowChessBoradLightmap(n);
}
