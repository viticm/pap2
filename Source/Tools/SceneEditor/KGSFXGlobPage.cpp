////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXGlobPage.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-12 11:44:46
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KSceneSFXEditorFrame.h"
#include "SceneEditor.h"
#include "KGSFXGlobPage.h"
#include "IEKG3DSFX.h"
//#include "KG3DSceneEditorBase.h"
#include "IEKG3DSceneOutputWnd.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGSFXGlobPage, KGLilisSlipPanel<CPropertyPage>)
KGSFXGlobPage::KGSFXGlobPage()
    : KGLilisSlipPanel<CPropertyPage>(KGSFXGlobPage::IDD)
{
    m_bSockWave = FALSE;
    m_bScalable = FALSE;
    m_bSockWaveAdd = FALSE;
	
	m_bShowBGTexture = FALSE;
}

KGSFXGlobPage::~KGSFXGlobPage()
{
}

void KGSFXGlobPage::DoDataExchange(CDataExchange* pDX)
{
	KGLilisSlipPanel<CPropertyPage>::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RESETSFX, m_bottonReSetFX);
    DDX_Check(pDX, IDC_CHECK_WAVE, m_bSockWave);
    DDX_Check(pDX, IDC_CHECK_WAVE_ADD, m_bSockWaveAdd);
    DDX_Check(pDX, IDC_SCANL_DEPAND, m_bScalable);
    DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Check(pDX, IDC_CLOSE_FLOOR, m_bCloseFloor);
    DDX_Control(pDX, IDC_BKCOLOR, m_bottonBkColor);
    DDX_Check(pDX, IDC_LOCK_Y, m_bLockY);
	DDX_Check(pDX,IDC_BGTEXTURE,m_bShowBGTexture);
	DDX_Control(pDX,IDC_BKTEXTURE,m_bottonBKTexture);
    DDX_Control(pDX, IDC_EDIT_MAX_NUM, m_EditMaxParticleNum);
    DDX_Control(pDX, IDC_EDIT_WAVE_POWER, m_EditWavePoser);
}


BEGIN_MESSAGE_MAP(KGSFXGlobPage, KGLilisSlipPanel<CPropertyPage>)
    ON_MESSAGE(WM_COLORDIALOG_CLOSE, OnColorBkDialogClose)
    ON_BN_CLICKED(IDC_CHECK_WAVE, &KGSFXGlobPage::OnBnClickedCheckWave)
    ON_BN_CLICKED(IDC_RESETSFX, &KGSFXGlobPage::OnBnClickReSetFX)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &KGSFXGlobPage::OnTvnSelchangedTree)
    ON_NOTIFY(NM_SETFOCUS, IDC_TREE, &KGSFXGlobPage::OnNMSetfocusTree)
	ON_BN_CLICKED(IDC_CLOSE_FLOOR, &KGSFXGlobPage::OnCheckCloseFloor)
    ON_BN_CLICKED(IDC_LOCK_Y, &KGSFXGlobPage::OnCheckLockY)
    ON_BN_CLICKED(IDC_BKCOLOR, &KGSFXGlobPage::OnBnClickedBkcolor)
    ON_BN_CLICKED(IDC_SCANL_DEPAND, &KGSFXGlobPage::OnBnClickedScanlDepand)
	ON_BN_CLICKED(IDC_BKTEXTURE,&KGSFXGlobPage::OnBnClickedBkTexture)
	ON_BN_CLICKED(IDC_BGTEXTURE, &KGSFXGlobPage::OnBnClickedBgtexture)
    ON_BN_CLICKED(IDC_CHECK_WAVE_ADD, &KGSFXGlobPage::OnBnClickedCheckWaveAdd)
    ON_BN_CLICKED(IDC_CHECK_GROUND, &KGSFXGlobPage::OnBnClickedCheckGround)
END_MESSAGE_MAP()

LRESULT KGSFXGlobPage::OnColorBkDialogClose(WPARAM w, LPARAM l)
{
    GET_SFX_EDITOR_RET(0);

    TCHAR szIniPath[MAX_PATH];
    g_GetFullPath(szIniPath, TEXT("config.ini"));
    IIniFile* pIniFile = g_OpenIniFile(szIniPath, true, true);

    if (pIniFile)
    {
        pIniFile->WriteStruct(TEXT("SfxEditor"), TEXT("bkcolor"), pScene->GetBkColorPtr(), sizeof(DWORD));
        pIniFile->Save(szIniPath);
        pIniFile->Release();
    }

    return 1;
}

BOOL KGSFXGlobPage::OnInitDialog()
{
    int nResult  = KGLilisSlipPanel<CPropertyPage>::OnInitDialog();

    m_tree.SetTextColor(RGB(255, 255, 255));
    m_tree.SetLineColor(RGB(122, 122, 122));
    m_tree.SetBkColor(RGB(92,  92, 92));

    return nResult;
}

int KGSFXGlobPage::OnSetActive()
{
    Init();
    return TRUE;
}

void KGSFXGlobPage::Init()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    IEKG3DSFX *pSFX = NULL;
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
	IEKG3DSceneSelectBase *pSelector = NULL;
    DWORD dwType = 0;
    CString str;
	//IEKG3DSceneSFXEditor *pScene = NULL;
    GET_SFX_EDITOR();

    hRetCode = pScene->GetSFX(&pSFX);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pScene->GetSceneEditBase(&piSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = piSceneEditorBase->GetSceneSelector(&pSelector);
	_ASSERTE(pSelector);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

    m_bSockWave = false;
    m_bSockWaveAdd = false;

    if (pSFX->GetIsShockWave())
    {
        if (pSFX->GetShockMode() == SHOCK_WAVE_ONLY)
            m_bSockWave = true;
        else
            m_bSockWaveAdd = true;
    }

    if (pSFX->IsFixedSize())
        m_bScalable = true;
    else
        m_bScalable = false;

	if (pSFX->GetIsCloseFloor())
		m_bCloseFloor = true;
	else
		m_bCloseFloor = false;

    if (pSFX->IsLoacAxisY())
        m_bLockY = true;
    else
        m_bLockY = false;

    m_tree.DeleteAllItems();

    HTREEITEM hMdlItem = m_tree.InsertItem(TEXT("模型"));
    HTREEITEM hSfxItem = m_tree.InsertItem(TEXT("特效"));

    m_tree.SetItemData(hMdlItem, (DWORD_PTR)0);
    m_tree.SetItemData(hMdlItem, (DWORD_PTR)0);

	INT nSelectSize = pSelector->GetSelectableEntitySize();
    if (pScene)
    {
        for (
            //std::KG3DSceneEntityList::iterator i = piSceneEditorBase->m_listSelectableEntity.begin();
            //i != piSceneEditorBase->m_listSelectableEntity.end();
			int i = 0; i < nSelectSize; i++
            )
        {
			IEKG3DSceneEntity* pEntity = NULL;
			hRetCode = pSelector->GetSelectableEntity(i, &pEntity);
			KGLOG_COM_PROCESS_ERROR(hRetCode);
			_ASSERTE(pEntity);
            hRetCode = pEntity->GetType(&EntityType);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            if (EntityType == SCENEENTITY_MODEL)
            {
                IEKG3DModel* pModel = NULL;
                LPCTSTR pszName = NULL;

                TCHAR szName[MAX_PATH];
                *szName = '\0';
                hRetCode = pEntity->GetPointer((VOID **)&pModel);
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pModel->GetName(&pszName);
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                if (!g_GetFilePathFromFullPath(szName, pszName))
                    strcpy(szName, pszName);


                hRetCode = pModel->GetType(&dwType);
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                if (dwType == MESHTYPE_SFX)
                {
                    HTREEITEM hItem = m_tree.InsertItem(szName, hSfxItem);
                    if (hItem)
                        m_tree.SetItemData(hItem, (DWORD_PTR)pModel);
                }
                else
                {
                    HTREEITEM hItem = m_tree.InsertItem(szName, hMdlItem);
                    if (hItem)
                        m_tree.SetItemData(hItem, (DWORD_PTR)pModel);
                }

            }
        }
    }
    m_tree.Expand(hMdlItem, TVE_EXPAND);
    m_tree.Expand(hSfxItem, TVE_EXPAND);


    str.Format("%u", pSFX->GetMaxParticleNum());
    m_EditMaxParticleNum.SetWindowText(str);

    UpdateData(FALSE);
Exit0:
    return;
}

void KGSFXGlobPage::OnBnClickedCheckWave()
{
    GET_SFX_EDITOR();
    UpdateData();
    pScene->EnableSoceWave(m_bSockWave);
    
    if (m_bSockWave)
    {
        pScene->SetShockMode(SHOCK_WAVE_ONLY);
        m_bSockWaveAdd = FALSE;
        UpdateData(FALSE);
    }
}

void KGSFXGlobPage::OnCheckCloseFloor()
{
	GET_SFX_EDITOR();
	UpdateData();
	pScene->EnableSfxCloseFloor(m_bCloseFloor);
}

void KGSFXGlobPage::OnCheckLockY()
{
    GET_SFX_EDITOR();
    UpdateData();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    if (pSFX)
        pSFX->EnableLockY(m_bLockY);

}

void KGSFXGlobPage::OnBnClickReSetFX()
{
    GET_SFX_EDITOR();
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

    IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	IEKG3DScene* pSceneBase = NULL;
	pScene->GetScene(&pSceneBase);
	_ASSERTE(pSceneBase);
    pSceneBase->GetCurOutputWnd(&piSceneCurOutputWnd);
    piSceneCurOutputWnd->ReloadFX();
}

void KGSFXGlobPage::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    GET_SFX_EDITOR();
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    IEKG3DModel* pModel = (IEKG3DModel*)m_tree.GetItemData(pNMTreeView->itemNew.hItem);

    if (pModel)
    {
        //KSceneSFXEditorView::GetActiveSFXView()->m_paneSFXKeyFrame.FillComb();

  /*      RECT rect;
        KSceneSFXEditorView::GetActiveSFXView()->m_paneSFXMainWnd.GetWindowRect(&rect);
        POINT point;
        GetCursorPos(&point);
        SetCursorPos(rect.left + 2, rect.top + 2);
        mouse_event(
            MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,
            0,
            0,
            0,
            GetMessageExtraInfo()
        );
        mouse_event(
            MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,
            0,
            0,
            0,
            GetMessageExtraInfo()
         );
        SetCursorPos(point.x, point.y);*/

        //KSceneSFXEditorView::GetActiveSFXView()->m_paneSFXMainWnd.SetActiveWindow();
        IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
        pScene->GetSceneEditBase(&piSceneEditorBase);

        //KG3DSceneEntity e(SCENEENTITY_MODEL, pModel);
        //piSceneEditorBase->m_listSelectedEntity.clear();
        //piSceneEditorBase->m_listSelectedEntity.push_back(e);
		IEKG3DSceneSelectBase* pSelector = NULL;
		piSceneEditorBase->GetSceneSelector(&pSelector);
		_ASSERTE(pSelector);
		//pSelector->ClearSelectedEntity();
		
		
		//pSelector->AddSelectedEntity(SCENEENTITY_MODEL, pModel);

		IEKG3DSceneSelectionTool* pTool = NULL;
		HRESULT hr = pScene->GetSelectionToolInterface(&pTool);
		if (SUCCEEDED(hr))
		{
			pTool->ClearSelection();
			hr = pTool->SetSelected(SCENEENTITY_MODEL, pModel, TRUE);
		}

       // pModel->AjustWorldDirection(&D3DXVECTOR3(0.f, 1.f, 0.f), &D3DXVECTOR3(1.f, 0.f, 0.f));

        KSceneSFXEditorView::GetActiveSFXView()->m_paneSFXEditerWnd.m_lpPropertySheet->m_pageBind.UpdateSFXSuedeList();

    }


    *pResult = 0;
}

namespace {

    int GetInt(LPARAM lParam) {
        CString* str = (CString*)(DWORD_PTR)lParam;
        int n = atoi(str->GetBuffer());
        str->ReleaseBuffer();
        return n;
    }

    float GetFloat(LPARAM lParam) {
        CString* str = (CString*)(DWORD_PTR)lParam;
        float f = (float)atof(str->GetBuffer());
        str->ReleaseBuffer();
        return f;
    }

}

LRESULT KGSFXGlobPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    IEKG3DSFX* sfx = GetSfx();

    switch (message)
    {
    case WM_EDIT_RECEIVE_ENTER :
        {
            switch (wParam)
            {
            case IDC_EDIT_MAX_NUM :
                {
                    if (GetInt(lParam) < 0)
                        break;
                    if (sfx)
                        sfx->SetMaxParticleNum(GetInt(lParam));
                }
                break;
            case IDC_EDIT_WAVE_POWER :
                {
                    if (GetFloat(lParam) < 0)
                        break;
                    if (sfx)
                        sfx->SetShockWavePower(GetFloat(lParam));
                }
                break;
            default :
                break;
            }
        }
        break;
    default  :
        break;
    }
    return CDialog::WindowProc(message, wParam, lParam);
}

void KGSFXGlobPage::OnNMSetfocusTree(NMHDR *pNMHDR, LRESULT *pResult)
{

    *pResult = 0;
}


void KGSFXGlobPage::OnBnClickedBkcolor()
{
    GET_SFX_EDITOR();
    GetColorPickerDialog()->Init(pScene->GetBkColorPtr(), TEXT("特效编辑器背景"), this, 0);
}

void KGSFXGlobPage::OnBnClickedScanlDepand()
{
    GET_SFX_EDITOR();
    UpdateData();
    IEKG3DSFX* pSfx = NULL;
    pScene->GetSFX(&pSfx);

    if (pSfx)
        pSfx->EnableEixedSize(m_bScalable);
}

void KGSFXGlobPage::OnBnClickedBkTexture()
{
	HRESULT hRetCode =E_FAIL;
	
	/*GetColorPickerDialog()->Init(pScene->GetBkColorPtr(), TEXT("特效编辑器背景"), this, 0);*/
	char bigBuff[800] = "";
	char szFilter[] ="Texture (*.*)|*.*|";
	TCHAR FileDir[256];
	std::string DiablogPath;

	CFileDialog dlg(false, NULL, NULL,
		OFN_HIDEREADONLY , szFilter);

	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	
	DiablogPath = g_szDefWorkDirectory;
		DiablogPath += "Data\\source\\maps_source\\Texture\\地表贴图";
		memcpy(FileDir,DiablogPath.c_str(),DiablogPath.length());
		FileDir[DiablogPath.size()] = '\0';
		dlg.m_ofn.lpstrInitialDir = FileDir;
	

	if (dlg.DoModal() == IDOK)
	{
		TCHAR fullName[256];
	/*	TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];*/

		CString Vs = dlg.GetPathName();

		wsprintf(fullName,"%s",Vs);
		/*_splitpath( path_buffer, drive, dir, fname, ext );


		wsprintf(FileDir,"%s%s%s.tga",drive,dir,fname);*/
		IEKG3DTexture* pTexture = NULL;
		
	
		GET_SFX_EDITOR();
		pScene->LoadBGTexture(fullName);
	}
}
void KGSFXGlobPage::OnBnClickedBgtexture()
{
	// TODO: Add your control notification handler code here
	GET_SFX_EDITOR();
	UpdateData();
	pScene->EnableShowBGTexture(m_bShowBGTexture);
}

void KGSFXGlobPage::OnBnClickedCheckWaveAdd()
{
    GET_SFX_EDITOR();
    UpdateData();
    
    pScene->EnableSoceWave(m_bSockWaveAdd);
    if (m_bSockWaveAdd)
    {
        pScene->SetShockMode(SHOCK_WAVE_ADD);
        m_bSockWave = FALSE;
        UpdateData(FALSE);
    }
}

void KGSFXGlobPage::OnBnClickedCheckGround()
{
    GET_SFX_EDITOR();
    pScene->EnableShowGround(IsDlgButtonChecked(IDC_CHECK_GROUND));
}
