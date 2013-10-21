
#include "stdafx.h"
#include "SceneEditor.h"
#include "KGModelEditorPaneMisc.h"
#include "KSceneCommonParamDialog.h"
#include "IEEditor.h"
#include "IEKG3DBillboardCloudCreater.h"
#include "IEKG3DSceneCheckers.h"

#define TIMER_EVENT 1

IMPLEMENT_DYNAMIC(KGModelEditorPaneMisc, CPropertyPage)

#define KG_USE_NEW_BILLBOARD_CLOUD_CREATER 0

KGModelEditorPaneMisc::KGModelEditorPaneMisc()
	: CPropertyPage(KGModelEditorPaneMisc::IDD)
    , m_lpBillBoardCreater(NULL)
    , m_pModelEditor(NULL)
    , m_pModel(NULL)
    , m_strMeshInfo(_T(""))
    , m_fPrecision(0.f)
    , m_bTwitterPlane(FALSE)
    , m_strFileName(_T(""))
    , m_szCount(_T(""))
    , m_nCurIndex(-1)
    , m_bBatch(FALSE)
    , m_bPause(TRUE)
    , m_bBuildTrunk(TRUE)
	, m_nTextureWidth(0)
{
    if (g_pGraphicsTool->GetScene())
    {
        g_pGraphicsTool->GetScene()->GetSceneModelEditor(&m_pModelEditor);
        
        if (m_pModelEditor)
			m_lpBillBoardCreater = m_pModelEditor->GetBillBoardCloudCreater();//<BillboardCloudV1>
    }
}

KGModelEditorPaneMisc::~KGModelEditorPaneMisc()
{
}

void KGModelEditorPaneMisc::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INFO, m_strMeshInfo);
	DDX_Text(pDX, IDC_EDIT_K, m_fPrecision);
	DDX_Check(pDX, IDC_CHECK, m_bTwitterPlane);
	DDX_Text(pDX, IDC_EDIT_FileNAME, m_strFileName);
	DDX_Text(pDX, IDC_EDIT_PANE_NUM, m_szCount);
	DDX_Control(pDX, IDC_TREE, m_MeshTree);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Text(pDX, IDC_EDIT1, m_nTextureWidth);
}

void KGModelEditorPaneMisc::UpdateInfo()
{
    if (!m_pModel)
        return;

    INT nNumBone;
    INT nNumSock;
    INT nNumMtrl;

    m_pModel->GetNumBones(&nNumBone);
    m_pModel->GetNumSocket(&nNumSock);
    m_pModel->GetNumMaterial(&nNumMtrl);
    
    m_strMeshInfo.Format(TEXT("Vertex Count :\t%u\r\nFace Count :\t%u\r\n\r\n"
        "Bone Count :\t%d\r\nSocket Count :\t%d\r\nMaterial Count : %d\r\n"),
        m_pModel->GetVertexNum(), m_pModel->GetFaceNum(),nNumBone, nNumSock, nNumMtrl
        );

    //<BillboardCloudV1>
    if (m_pModel->HasBillBoardCloud())
    {
        TCHAR szFileName[MAX_PATH];
        
		{
			LPCTSTR szModelName = 0;
			//m_pModel->GetFullName(&szModelName);
			m_pModel->GetName(&szModelName);
			//wsprintf(szFileName, TEXT("%s%s"), szModelName, TEXT(".bbc"));*/
			KG_CUSTOM_HELPERS::StrReplaceExtension(szModelName, _T("bbc"), szFileName);
		}

        m_strFileName = szFileName;
        DWORD dwCount = 0;//m_pModel->GetBillBoardCloudPlaneNum();
        IFile* pFile = g_OpenFile(szFileName);
        if (pFile)
        {
            pFile->Read(&dwCount, sizeof(DWORD));
            pFile->Release();
        }

		TCHAR szCount[128];
        wsprintf(szCount, TEXT("面片数目 : %u (%u face)"), dwCount, dwCount * 2);
        m_szCount = szCount;
    }

    if (m_lpBillBoardCreater)
    {
        m_fPrecision = m_lpBillBoardCreater->GetPrecision();
        m_bTwitterPlane = m_lpBillBoardCreater->IsTwitterPlane();
    }
	//<BillboardCloudV1>
}

BEGIN_MESSAGE_MAP(KGModelEditorPaneMisc, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON_CREATE, &KGModelEditorPaneMisc::OnBnClickedButtonCreate)
    ON_WM_TIMER()
    ON_EN_CHANGE(IDC_EDIT_K, &KGModelEditorPaneMisc::OnEnChangeEditK)
    ON_BN_CLICKED(IDC_CHECK, &KGModelEditorPaneMisc::OnBnClickedCheck)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &KGModelEditorPaneMisc::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_BAT, &KGModelEditorPaneMisc::OnBnClickedButtonBat)
    ON_BN_CLICKED(IDC_BUTTON_ADD, &KGModelEditorPaneMisc::OnBnClickedButtonAdd)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &KGModelEditorPaneMisc::OnTvnSelchangedTree)
    ON_BN_CLICKED(IDC_BUTTON_STP, &KGModelEditorPaneMisc::OnBnClickedButtonStp)
    ON_BN_CLICKED(IDC_BUTTON_ADD2, &KGModelEditorPaneMisc::OnBnClickedButtonAdd2)
	ON_BN_CLICKED(IDC_BN_OPTION, &KGModelEditorPaneMisc::OnBnClickedButtonOption)
	ON_BN_CLICKED(IDC_BN_SHADER_UPDATE, &KGModelEditorPaneMisc::OnBnClickedButtonShaderUpdate)
	ON_BN_CLICKED(IDC_SHOW, &KGModelEditorPaneMisc::OnBnClickedButtonProcessShow)
	ON_BN_CLICKED(IDC_BN_STEP, &KGModelEditorPaneMisc::OnBnClickedButtonStep)
	ON_BN_CLICKED(IDC_CHECK1, &KGModelEditorPaneMisc::OnBnEnableNormalCheck)
	ON_EN_CHANGE(IDC_EDIT1, &KGModelEditorPaneMisc::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON_COMPUTESIZE, &KGModelEditorPaneMisc::OnBnClickedButtonComputesize)
	ON_BN_CLICKED(IDC_BUTTON_SETSIZE, &KGModelEditorPaneMisc::OnBnClickedButtonSetsize)
	ON_BN_CLICKED(IDC_BUTTON_CREATE2, &KGModelEditorPaneMisc::OnBnClickedButtonCreate2)
END_MESSAGE_MAP()

void KGModelEditorPaneMisc::OnBnClickedButtonCreate()
{
	//<BillboardCloud>//<BillboardCloudV1>
#if KG_USE_NEW_BILLBOARD_CLOUD_CREATER == 1
	if(NULL == m_pModelEditor)
		return;

	IEKG3DBillboardCloudCreater2* pBCC = m_pModelEditor->GetBillboardCloudCreater2();
	if(NULL == pBCC)
		return;

	IEKG3DModel* pModel = NULL;
	HRESULT hr = m_pModelEditor->GetCurModel(&pModel);
	if(FAILED(hr))
		return;

	pBCC->BuildForModel(pModel);

#else
    if (!m_lpBillBoardCreater)
        return;

    m_lpBillBoardCreater->StartBillBoardCreating();

    KillTimer(TIMER_EVENT);
    SetTimer(TIMER_EVENT, 100, NULL);
    m_bBatch = FALSE;


	if(NULL == m_pModelEditor)
		return;
#endif

	//</BillboardCloud>
}

void KGModelEditorPaneMisc::OnTimer(UINT_PTR nIDEvent)
{
	//<BillboardCloudV1>
    switch (nIDEvent)
    {
    case TIMER_EVENT :
        {
            if (m_lpBillBoardCreater->GetState() == EM_BILLBOARD_CREATE_STOP)
            {
                if (m_bBatch)
                {
                    if (++m_nCurIndex < (int)m_vecModelNames.size())
                    {
                        if (m_nCurIndex) 
                        {
                            m_lpBillBoardCreater->SaveBillboardCloud();
                            m_pModel->IEReleaseBillboardCloud();
                        }

                        m_Progress.SetPos(m_nCurIndex);

                        m_pModelEditor->OpenModel(m_vecModelNames[m_nCurIndex].GetBuffer(), NULL, NULL, FALSE, NULL);
                        m_vecModelNames[m_nCurIndex].ReleaseBuffer();
                        m_pModelEditor->GetCurModel(&m_pModel);

                        m_lpBillBoardCreater->StartBillBoardCreating();

                    }
                    else
                    {
                        m_lpBillBoardCreater->SaveBillboardCloud();
                        m_pModel->IEReleaseBillboardCloud();
                        m_Progress.ShowWindow(SW_HIDE);
                        UpdateInfo();
                        m_nCurIndex = -1;
                        m_bPause = TRUE;
                        KillTimer(TIMER_EVENT);
                        m_Progress.SetPos(0);
                        SetDlgItemText(IDC_BUTTON_BAT, TEXT("生成"));
                    }
                }
                else
                {
                    DWORD dwCount = m_lpBillBoardCreater->GetBillBoardCount();
                    TCHAR str[128];
                    wsprintf(str, TEXT("面片数目 : %u (%u face)"), dwCount, dwCount * 2);
                    SetDlgItemText(IDC_EDIT_PANE_NUM, str);
                    KillTimer(TIMER_EVENT);
                }
            }

			//////////////////////////////////////////////////////////////////////////
        }
        break;
    default :
        break;
    }
	//</BillboardCloudV1>
    return CPropertyPage::OnTimer(nIDEvent);
}


void KGModelEditorPaneMisc::OnEnChangeEditK()
{
    UpdateData(TRUE);

	//<BillboardCloud>
    if(m_lpBillBoardCreater)
        m_lpBillBoardCreater->SetPrecision(m_fPrecision);
	//</BillboardCloud>

}

void KGModelEditorPaneMisc::OnBnClickedCheck()
{
    UpdateData(TRUE);

	//<BillboardCloud>
    if(m_lpBillBoardCreater)
        m_lpBillBoardCreater->EnableTwitterPlane(m_bTwitterPlane);
	//</BillboardCloud>
}

void KGModelEditorPaneMisc::OnBnClickedButtonSave()
{
	//<BillboardCloudV1>
    if(m_lpBillBoardCreater)
        m_lpBillBoardCreater->SaveBillboardCloud();

    TCHAR szFileName[MAX_PATH];
    LPCTSTR szModelName = 0;
    //m_pModel->GetFullName(&szModelName);
	m_pModel->GetName(&szModelName);
    //wsprintf(szFileName, TEXT("%s%s"), szModelName, TEXT(".bbc"));
	KG_CUSTOM_HELPERS::StrReplaceExtension(szModelName, _T("bbc"), szFileName);

    m_strFileName = szFileName;
    SetDlgItemText(IDC_EDIT_FileNAME, m_strFileName);

    //m_pModel->IEReleaseBillboardCloud();
	//</BillboardCloudV1>
}

void KGModelEditorPaneMisc::FillTree(CString strPath, HTREEITEM hItem)
{
    strPath += TEXT("*.*");

    CFileFind fileFind;
    fileFind.FindFile(strPath);

    while (fileFind.FindNextFile())
    {
        if (fileFind.IsDots() || fileFind.IsHidden())
            continue;

        if (fileFind.IsDirectory())
        {
            HTREEITEM hInsert = m_MeshTree.InsertItem(fileFind.GetFileName(), IDC_FC, IDC_FC, hItem);
            FillTree(fileFind.GetFilePath() + TEXT("\\"), hInsert);
            continue;
        }

        CString strName = fileFind.GetFileName();
        int nDot = strName.ReverseFind('.');
        CString strExt = strName.Mid(++nDot);
        strExt.MakeLower();
        
        if (strExt != "mesh")
            continue;

        HTREEITEM hInsert = m_MeshTree.InsertItem(strName, IDC_GLEAN, IDC_GLEAN, hItem);
        m_vecModelNames.push_back(fileFind.GetFilePath());
        m_MeshTree.SetItemData(hInsert, static_cast<DWORD_PTR>((m_vecModelNames.size() - 1)));


    }

}

void KGModelEditorPaneMisc::BuildBillboardCloud()//<BillboardCloudV1>
{
    if (m_nCurIndex >= (int)m_vecModelNames.size())
        return;

    m_pModelEditor->OpenModel(m_vecModelNames[m_nCurIndex].GetBuffer(), NULL, NULL, FALSE, NULL);
    m_vecModelNames[m_nCurIndex].ReleaseBuffer();
    m_pModelEditor->GetCurModel(&m_pModel);

   // m_lpBillBoardCreater->CreateBillBoardCloudForModel();
    //SetTimer(TIMER_WAIT_CALCUV_BAT, 100, NULL);

}

void KGModelEditorPaneMisc::OnBnClickedButtonStp()
{
    SetDlgItemText(IDC_BUTTON_BAT, TEXT("生成"));

    m_nCurIndex = -1;
    m_Progress.ShowWindow(SW_HIDE);
    KillTimer(TIMER_EVENT);
    m_bPause = TRUE;
}


void KGModelEditorPaneMisc::OnBnClickedButtonBat()
{
    if (!m_pModelEditor)
        return;

    if (m_bPause)
    {
        SetDlgItemText(IDC_BUTTON_BAT, TEXT("暂停"));

        m_Progress.ShowWindow(SW_SHOW);
        m_Progress.SetRange32(0, (int)m_vecModelNames.size());
        
        KillTimer(TIMER_EVENT);
        SetTimer(TIMER_EVENT, 100, NULL);

        m_bBatch = TRUE;
        m_bPause = FALSE;

    }
    else
    {
        SetDlgItemText(IDC_BUTTON_BAT, TEXT("继续"));
        KillTimer(TIMER_EVENT);
        m_bPause = TRUE;
    }

}

void KGModelEditorPaneMisc::OnBnClickedButtonAdd()
{
    TCHAR strDirectory[MAX_PATH];
    BROWSEINFO   BrowseInfo;
    LPITEMIDLIST pItemList;

    memset(&BrowseInfo, 0, sizeof(BROWSEINFO));
    
    wsprintf(strDirectory, TEXT("%sdata\\source\\maps_source"), g_szDefWorkDirectory);

    USES_CONVERSION;
    SHParseDisplayName(A2CW(strDirectory), NULL, &pItemList, 0, NULL);
    BrowseInfo.hwndOwner = GetSafeHwnd();
    BrowseInfo.pidlRoot = pItemList;

    LPCITEMIDLIST pItem = SHBrowseForFolder(&BrowseInfo);

    if (pItem)
    {
        if (!SHGetPathFromIDList(pItem, strDirectory))
            wsprintf(strDirectory, TEXT("%sdata\\source\\maps_source\\"), g_szDefWorkDirectory);

        if (strDirectory[strlen(strDirectory) - 1] !=  '\\')
           _tcscat(strDirectory, TEXT("\\"));

        m_MeshTree.DeleteAllItems();
        m_vecModelNames.clear();
        FillTree(strDirectory, TVI_ROOT);
    }
}

void KGModelEditorPaneMisc::OnBnClickedButtonAdd2()
{
    TCHAR strDirectory[MAX_PATH];
    wsprintf(strDirectory, TEXT("%sdata\\source\\maps_source"), g_szDefWorkDirectory);

    TCHAR strFileNameBuffer[MAX_PATH * 10] = "";
    TCHAR strFilter[] = "mesh Files (*.mesh)|*.mesh|stree Files (*.stree)|*.stree||";
    TCHAR strDefaultPath[MAX_PATH];
    LPCTSTR pszFullName = NULL;

    strDefaultPath[0] = '\0';

    CFileDialog DlgLoad(TRUE, ".mesh", "test.mesh", OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, strFilter);

    DlgLoad.m_ofn.lpstrFile = strFileNameBuffer;
    DlgLoad.m_ofn.nMaxFile = sizeof(strFileNameBuffer);
    DlgLoad.m_ofn.lpstrInitialDir = strDirectory;

    if (DlgLoad.DoModal() != IDOK)
        return;

    m_MeshTree.DeleteAllItems();
    m_vecModelNames.clear();

    POSITION FilePos = DlgLoad.GetStartPosition();

    while(FilePos)
    {
        CString strPathName = DlgLoad.GetNextPathName(FilePos);
        CString strName = strPathName.Mid(strPathName.ReverseFind('\\') + 1);
        HTREEITEM hInsert = m_MeshTree.InsertItem(strName);
        m_vecModelNames.push_back(strPathName);
        m_MeshTree.SetItemData(hInsert, static_cast<DWORD_PTR>((m_vecModelNames.size() - 1)));

        //if (m_bBuildTrunk)
        //{
        //    TCHAR strTemp[MAX_PATH];
        //    _tcsncpy(strTemp, strPathName.GetBuffer(), sizeof(strTemp));
        //    strPathName.ReleaseBuffer();
        //    _tcslwr(strTemp);
        //    TCHAR* pStr = _tcsstr(strTemp, ".stree");
        //    if (pStr)
        //    {
        //        IEKG3DModelSpeedTree* pTree = NULL;
        //        IKG3DResourceManager* pModelManager = g_pEngineManager->Get3DModelTable();
        //        pModelManager->LoadResourceFromFile(strPathName.GetBuffer(), 0, 0, (IKG3DResourceBase**)&pTree);
        //        if (pTree)
        //        {

        //        }

        //        //*pStr = 0;
        //        //strPathName = strTemp;
        //        //CString strName = strPathName.Mid(strPathName.ReverseFind('\\') + 1);
        //        //HTREEITEM hInsert = m_MeshTree.InsertItem(strName);
        //        //m_vecModelNames.push_back(strPathName);
        //        //m_MeshTree.SetItemData(hInsert, static_cast<DWORD_PTR>((m_vecModelNames.size() - 1)));
        //    }
        //}

    }
}

void KGModelEditorPaneMisc::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    
    if (m_bPause && pNMTreeView->itemNew.hItem)
    {
        HTREEITEM hItem = pNMTreeView->itemNew.hItem;
        DWORD index = (DWORD)m_MeshTree.GetItemData(hItem);

        if (index < m_vecModelNames.size())
        {
            m_pModelEditor->OpenModel(m_vecModelNames[index].GetBuffer(), NULL, NULL, FALSE, NULL);
            m_vecModelNames[index].ReleaseBuffer();
            m_pModelEditor->GetCurModel(&m_pModel);
            UpdateInfo();
            UpdateData(FALSE);
        }   
    }

    *pResult = 0;
}

void KGModelEditorPaneMisc::OnBnClickedButtonOption()
{
	if(NULL == m_pModelEditor)
		return;

	IEKG3DBillboardCloudCreater2* pBC = m_pModelEditor->GetBillboardCloudCreater2();//<BillboardCloud>
	if(NULL == pBC)
		return;

	KG3DBillboardCloudCreaterThresholdStruct threshould;
	HRESULT hr = pBC->GetThresholdStruct(&threshould);
	if(FAILED(hr))
		return;

	KSceneCommonParamDialog OptionDialog;
	BOOL bRet = OptionDialog.AddStruct(threshould);
	if(! bRet)
		return;

	if(IDOK != OptionDialog.DoModal())
		return;

	bRet = OptionDialog.GetStruct(threshould);
	if(! bRet)
		return;

	hr = pBC->SetThresholdStruct(&threshould);
}

void KGModelEditorPaneMisc::OnBnClickedButtonShaderUpdate()
{
	if(NULL == m_pModelEditor)
		return;

	IEKG3DBillboardCloudCreater2* pBC = m_pModelEditor->GetBillboardCloudCreater2();//<BillboardCloud>
	if(NULL == pBC)
		return;

	pBC->UpdateShader();
}
void KGModelEditorPaneMisc::OnBnClickedButtonProcessShow()
{
	if(NULL == m_pModelEditor)
		return;

	IEKG3DBillboardCloudCreater2* pBC = m_pModelEditor->GetBillboardCloudCreater2();//<BillboardCloud>
	if(NULL == pBC)
		return;

	DWORD dwOption = (DWORD)pBC->GetOption();
	if(this->IsDlgButtonChecked(IDC_SHOW))
	{
		pBC->SetOption(dwOption | KG3D_BILLBOARD_CLOUD_CT_OP_SHOW_PROCESS);
	}
	else
	{
		pBC->SetOption(dwOption & ~KG3D_BILLBOARD_CLOUD_CT_OP_SHOW_PROCESS);
	}

	dwOption = (DWORD)pBC->GetOption();
	this->CheckDlgButton(IDC_SHOW, dwOption & KG3D_BILLBOARD_CLOUD_CT_OP_SHOW_PROCESS ? BST_CHECKED : BST_UNCHECKED);
}

void KGModelEditorPaneMisc::OnBnClickedButtonStep()
{
	if(NULL == m_pModelEditor)
		return;

	IEKG3DBillboardCloudCreater2* pBC = m_pModelEditor->GetBillboardCloudCreater2();//<BillboardCloud>
	if(NULL == pBC)
		return;

	pBC->StepCommand();
}

void KGModelEditorPaneMisc::OnBnEnableNormalCheck()
{
	IEKG3DSceneCheckers* p = KSH::GetEngineInterfaceEx<IEKG3DSceneCheckers>();
	if(NULL == p)
		return;

	if (BST_CHECKED == this->IsDlgButtonChecked(IDC_CHECK1))
	{
		p->EnableCheckNormal(TRUE);
	}
	else
	{
		p->EnableCheckNormal(FALSE);
	}
	ULONG IsChecked = p->IsCheckNormalEnable();
	this->CheckDlgButton(IDC_CHECK1, IsChecked ? BST_CHECKED : BST_UNCHECKED);
}

BOOL KGModelEditorPaneMisc::OnSetActive()
{
	IEKG3DSceneCheckers* p = KSH::GetEngineInterfaceEx<IEKG3DSceneCheckers>();
	if(NULL != p)
	{
		ULONG IsChecked = p->IsCheckNormalEnable();
		this->CheckDlgButton(IDC_CHECK1, IsChecked ? BST_CHECKED : BST_UNCHECKED);
	}
	return CPropertyPage::OnSetActive();
}

void KGModelEditorPaneMisc::OnEnChangeEdit1()
{
	UpdateData(TRUE);
}

void KGModelEditorPaneMisc::OnBnClickedButtonComputesize()
{
	if(m_lpBillBoardCreater)
	{
		m_nTextureWidth = m_lpBillBoardCreater->GetTextureWidth();
		UpdateData(FALSE);
	}
}

void KGModelEditorPaneMisc::OnBnClickedButtonSetsize()
{
	if(m_lpBillBoardCreater)
	{
		m_lpBillBoardCreater->SetTextureWidth(m_nTextureWidth);
	}
}

void KGModelEditorPaneMisc::OnBnClickedButtonCreate2()
{
	if (!m_lpBillBoardCreater)
		return;

	m_lpBillBoardCreater->ComputeTexture();
}
