// ***************************************************************
//
//  Description: 你的选择就是我的选择
//  -------------------------------------------------------------
//	Author: Zeb
//  -------------------------------------------------------------
//	Create Date: 2006-03-08
//	-------------------------------------------------------------
//	History:
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
//
// ***************************************************************
// KSceneSFXEditorSelectDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "MainFrm.h" //by dengzhihui 2006年7月6日 14:46:03
#include "SceneEditor.h"
#include "KSceneSFXEditorSelectDlg.h"
#include "IEKG3DSceneSFXEditor.h"
#include "KSceneSFXEditorFrame.h"
//#include "KG3DSceneEditorBase.h"
#include "KSceneHelperFuncs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KSceneSFXEditorSelectDlg dialog

IMPLEMENT_DYNAMIC(KSceneSFXEditorSelectDlg, CDialog)

KSceneSFXEditorSelectDlg::KSceneSFXEditorSelectDlg(CWnd* pParent /*=NULL*/)
{

}

KSceneSFXEditorSelectDlg::~KSceneSFXEditorSelectDlg()
{
}

void KSceneSFXEditorSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_SFXEDITOR_ELEMENT, m_listSelect);
}


BEGIN_MESSAGE_MAP(KSceneSFXEditorSelectDlg, CDialog)
    ON_WM_SIZE()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_SFXEDITOR_ELEMENT, &KSceneSFXEditorSelectDlg::OnLvnItemchangedSfxeditorElement)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_SFXEDITOR_ELEMENT, &KSceneSFXEditorSelectDlg::OnLvnEndlabeleditSfxeditorElement)
    ON_NOTIFY(LVN_KEYDOWN, IDC_SFXEDITOR_ELEMENT, &KSceneSFXEditorSelectDlg::OnLvnKeydownSfxeditorElement)
END_MESSAGE_MAP()


// KSceneSFXEditorSelectDlg message handlers

BOOL KSceneSFXEditorSelectDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    m_listSelect.InsertColumn(0, _T("东东的名字"),	LVCFMT_CENTER, 100);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSFXEditorSelectDlg::UpdateUI()
{
    UpdateList();
}

void KSceneSFXEditorSelectDlg::UpdateList()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    VOID *pvSeletableEntityPointer = NULL;
    VOID *pvSelectedEntityPointer  = NULL;
    vector<int> vec;

    //(by dengzhihui 2006年7月6日 14:46:22
    CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	//)
    KSceneSFXEditorFrame *pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)pFrame->GetActiveDocument();
    if (!pDoc)
        return;

    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (pScene == NULL)
        return;

    m_listSelect.DeleteAllItems();
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);
	GETSELECT(piSceneEditorBase);
	for (INT i = 0; i < pSelector->GetSelectableEntitySize(); i ++)
    {
		IEKG3DSceneEntity* pEntity = NULL;
		pSelector->GetSelectableEntity(i, &pEntity);
		_ASSERTE(pEntity);

        SFX_ENTITY_DATA *pData = NULL;
        hRetCode = pEntity->GetRefData((VOID **)&pData);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        if (pData)
        {
            if (pData && pData->szEntityName[0] == '\0')
            {
                hRetCode = pEntity->GetType(&EntityType);
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                if (EntityType == SCENEENTITY_SFX_BILLBOARD)
                    pScene->SetBillboardEntityDataNameToDefault(pData);
                else if (EntityType == SCENEENTITY_SFX_LAUNCHER)
                    pScene->SetLauncherEntityDataNameToDefault(pData);
            }
            m_listSelect.InsertItem(
                LVIF_TEXT | LVIF_PARAM,
                m_listSelect.GetItemCount(),
                pData->szEntityName,
                0, 0, 0,
                (LPARAM)pData
            );
        }

        //++i;
    }

	IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(pScene);
	if(NULL == pProxy)
		return;

	//for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
	for(IEKG3DSceneEntity* piEntity = pProxy->Reset(); NULL != piEntity; piEntity = pProxy->Next())
	{
		/*IEKG3DSceneEntity* pSelectedEntity = NULL;
		pSelector->GetSelectedEntity(i, &pSelectedEntity);
		_ASSERTE(pSelectedEntity);*/

		for (INT j = 0; j < pSelector->GetSelectableEntitySize(); j ++)
        {
			IEKG3DSceneEntity* pSelectableEntity = NULL;
			pSelector->GetSelectableEntity(j, &pSelectableEntity);
			_ASSERTE(pSelectableEntity);

			hRetCode = pSelectableEntity->GetPointer(&pvSeletableEntityPointer);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = piEntity->GetPointer(&pvSelectedEntityPointer);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            if (pvSeletableEntityPointer != pvSelectedEntityPointer)
                continue;

            LVFINDINFO FindInfo;
            int nIndex = 0;

            FindInfo.flags  = LVFI_PARAM;
            hRetCode = pSelectableEntity->GetRefData((VOID **)&FindInfo.lParam);
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            nIndex = m_listSelect.FindItem(&FindInfo);
            if (nIndex != -1)
            {
                vec.push_back(nIndex);
            }
            break;
        }
    }

    for (int ii = 0; ii < (int)vec.size(); ++ii)
        m_listSelect.SetItemState(vec[ii], LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

Exit0:
    return;
}

void KSceneSFXEditorSelectDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (m_listSelect.m_hWnd == NULL)
        return;

    RECT rc;
    GetWindowRect(&rc);
    m_listSelect.MoveWindow(10, 10, rc.right - rc.left - 30, rc.bottom - rc.top - 50, TRUE);
}

void KSceneSFXEditorSelectDlg::OnLvnItemchangedSfxeditorElement(NMHDR *pNMHDR, LRESULT *pResult)
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;

    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
    return;
}

void KSceneSFXEditorSelectDlg::OnLvnEndlabeleditSfxeditorElement(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
    return;
}

void KSceneSFXEditorSelectDlg::OnLvnKeydownSfxeditorElement(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
    return;
}
