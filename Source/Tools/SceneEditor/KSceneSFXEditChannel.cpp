////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditChannel.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-10-25 15:10:23
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSFXEditChannel.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSFX.h"
#include "IEKG3DMesh.h"
#include "IEKG3DTexture.h"
//#include "KG3DMOdelST.h"

IMPLEMENT_DYNAMIC(KSceneSFXEditChannel, CDialog)
KSceneSFXEditChannel::KSceneSFXEditChannel(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSFXEditChannel::IDD, pParent)
{
    m_pLModel = NULL;
    m_pRModel = NULL;
}

KSceneSFXEditChannel::~KSceneSFXEditChannel()
{
}

void KSceneSFXEditChannel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_ADD,        m_buttonAdd);
    DDX_Control(pDX, IDC_BUTTON_DEL,        m_buttonDel);
    DDX_Control(pDX, IDC_EDIT_PIECE_COUNT,  m_editPieceCount);
    DDX_Control(pDX, IDC_EDIT_PIECE_WIDTH,  m_editPieceWidth);
    DDX_Control(pDX, IDC_EDIT_PIECE_LENGTH, m_editPieceLength);
    DDX_Control(pDX, IDC_EDIT_V_COUNT,      m_editVCount);
    DDX_Control(pDX, IDC_EDIT_U_SPEED,      m_editUSpeed);
    DDX_Control(pDX, IDC_EDIT_V_SPEED,      m_editVSpeed);
    DDX_Control(pDX, IDC_EDIT_TEX,          m_editTexture);
    DDX_Control(pDX, IDC_COMBO_L_MODEL,     m_comboLModel);
    DDX_Control(pDX, IDC_COMBO_L_TARGER,    m_comboLTargt);
    DDX_Control(pDX, IDC_COMBO_R_MODEL,     m_comboRModel);
    DDX_Control(pDX, IDC_COMBO_R_TARGER,    m_comboRTargt);
    DDX_Control(pDX, IDC_COMBO_R_BLEN,      m_comboBlend);
}

BEGIN_MESSAGE_MAP(KSceneSFXEditChannel, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ADD,        &KSceneSFXEditChannel::OnBnClickedButtonAdd)
    ON_BN_CLICKED(IDC_BUTTON_DEL,        &KSceneSFXEditChannel::OnBnClickedButtonDel)
    ON_BN_CLICKED(IDC_BUTTON_TEX,        &KSceneSFXEditChannel::OnBnClickedSetTexture)
    ON_CBN_SETFOCUS(IDC_COMBO_L_MODEL,   &KSceneSFXEditChannel::OnCbnSetfocusLModel)
    ON_CBN_SETFOCUS(IDC_COMBO_L_TARGER,  &KSceneSFXEditChannel::OnCbnSetfocusLTargt)
    ON_CBN_SETFOCUS(IDC_COMBO_R_MODEL,   &KSceneSFXEditChannel::OnCbnSetfocusRModel)
    ON_CBN_SETFOCUS(IDC_COMBO_R_TARGER,  &KSceneSFXEditChannel::OnCbnSetfocusRTargt)
    ON_CBN_SELCHANGE(IDC_COMBO_L_MODEL,  &KSceneSFXEditChannel::OnCbnSelchangedLModel)
    ON_CBN_SELCHANGE(IDC_COMBO_L_TARGER, &KSceneSFXEditChannel::OnCbnSelchangedLTargt)
    ON_CBN_SELCHANGE(IDC_COMBO_R_MODEL,  &KSceneSFXEditChannel::OnCbnSelchangedRModel)
    ON_CBN_SELCHANGE(IDC_COMBO_R_TARGER, &KSceneSFXEditChannel::OnCbnSelchangedRTargt)
    ON_CBN_SELCHANGE(IDC_COMBO_R_BLEN,   &KSceneSFXEditChannel::OnCbnSelchangedBlend)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

HBRUSH KSceneSFXEditChannel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBursh = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_LISTBOX)
    {
        static HBRUSH hFocus = ::CreateSolidBrush(RGB(240, 200, 85));
        static HBRUSH hNorml = ::CreateSolidBrush(RGB(210, 210, 233));
        COLORREF cFocus = RGB(240, 200, 85);
        COLORREF cNorml = RGB(210, 210, 233);

        if (::GetFocus() == pWnd->m_hWnd)
        {
            hBursh = hFocus;
            pDC->SetBkColor(cFocus);
        }
        else
        {
            hBursh = hNorml;
            pDC->SetBkColor(cNorml);
        }
    }

    return hBursh;
}


void KSceneSFXEditChannel::OnCbnSelchangedLModel()
{
    int nSelIndex = m_comboLModel.GetCurSel();
    if (nSelIndex != -1)
        m_pLModel = reinterpret_cast<IEKG3DModel*>(m_comboLModel.GetItemData(nSelIndex));
    else
        m_pLModel = NULL;
    UpdateChanel();
}

void KSceneSFXEditChannel::OnCbnSelchangedLTargt()
{
    char szName[128];
    int nSelIndex = m_comboLTargt.GetCurSel();
    if (nSelIndex != -1)
    {
        m_comboLTargt.GetLBText(nSelIndex, szName);
        m_strLTar = szName;
        UpdateChanel();
    }
}

void KSceneSFXEditChannel::OnCbnSelchangedRModel()
{
    int nSelIndex = m_comboRModel.GetCurSel();
    if (nSelIndex != -1)
        m_pRModel = reinterpret_cast<IEKG3DModel*>(m_comboRModel.GetItemData(nSelIndex));
    else
        m_pRModel = NULL;
    UpdateChanel();
}

void KSceneSFXEditChannel::OnCbnSelchangedBlend()
{
    int nSelIndex = m_comboBlend.GetCurSel();
    GET_SFX_EDITOR();
    GETSFX(pScene);
    GETCHANNEL(pSFX);
    if (pChannel)
        pChannel->SetBlendMode(nSelIndex);
}

void KSceneSFXEditChannel::OnCbnSelchangedRTargt()
{
    char szName[128];
    int nSelIndex = m_comboRTargt.GetCurSel();
    if (nSelIndex != -1)
    {
        m_comboRTargt.GetLBText(nSelIndex, szName);
        m_strRTar = szName;
        UpdateChanel();
    }
}

void KSceneSFXEditChannel::OnCbnSetfocusLModel()
{
    GET_SFX_EDITOR();
    m_comboLModel.ResetContent();

    /*
for (
        std::KG3DSceneEntityList::iterator i = pScene->m_listRenderEntity.begin();
        i != pScene->m_listRenderEntity.end();
        i++
        )
    {
        if ((*i).m_Type == SCENEENTITY_MODEL)
        {
            KG3DModel* pModel = (KG3DModel*)(*i).m_pPointer;      
            char* pName = strrchr(const_cast<char*>(pModel->m_scName.c_str()), '\\');
            int nIndex = m_comboLModel.AddString(++pName);
            m_comboLModel.SetItemDataPtr(nIndex, (void*)pModel);
        }
    }
    //m_comboLModel.AddString(TEXT("原始点"));
*/
	IEKG3DSceneEntity *piEntity = NULL;
	IEKG3DModel* pSelModel = NULL;
	HRESULT hRetCode = E_FAIL;
	SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
	LPCTSTR pszName = NULL;

	GETSCENE(pScene);

	hRetCode = p3DScene->GetFirstRenderEntity(&piEntity);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	
	while (piEntity)
	{
		hRetCode = piEntity->GetType(&EntityType);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		if (EntityType == SCENEENTITY_MODEL)
		{
			IEKG3DModel* pModel = NULL;
			hRetCode = piEntity->GetPointer((VOID **)&pModel);
			KGLOG_COM_PROCESS_ERROR(hRetCode);

			hRetCode = pModel->GetName(&pszName);
			KGLOG_COM_PROCESS_ERROR(hRetCode);

			char* pName = strrchr(const_cast<char*>(pszName), '\\');
			int nIndex = m_comboLModel.AddString(++pName);
			m_comboLModel.SetItemDataPtr(nIndex, (void*)pModel);
		}
		hRetCode = p3DScene->GetNextRenderEntity(&piEntity);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
	}
Exit0:
	return;
}

void KSceneSFXEditChannel::OnCbnSetfocusLTargt()
{
    GET_SFX_EDITOR();
    if (!m_pLModel)
        return;
    m_comboLTargt.ResetContent();
    m_comboLTargt.AddString(TEXT("原始点"));

    /*KG3DModelST* pModelST = dynamic_cast<KG3DModelST*>(m_pLModel);
    if (pModelST)
    {
        std::vector<KG3DModel*> vecModels = pModelST->m_ModelSet.GetModels();
        m_comboLTargt.AddString(TEXT("原点"));
        for (size_t s = 0; s < vecModels.size(); s++)
        {
            KG3DMeshEx *pMesh     = (KG3DMeshEx*)(vecModels[s]->GetMesh());
            if (!pMesh)
                continue;
            DWORD		dwNumBone = 0;
            pMesh->GetNumBones(dwNumBone);
            for (DWORD i = 0; i < dwNumBone; i++)
            {
                KG3DMesh::_BoneInfo *pBoneInfo = NULL;
                pMesh->GetBoneInfo(i, &pBoneInfo);
                m_comboLTargt.AddString(pBoneInfo->scName.c_str());
            }

            pMesh->Release();
        }
    }
    else if (m_pLModel)
    {
        KG3DMeshEx *pMesh = (KG3DMeshEx*)m_pLModel->GetMesh();
        if (pMesh)
        {
            DWORD dwNumBone = 0;
            pMesh->GetNumBones(dwNumBone);
            m_comboLTargt.AddString(TEXT("原点"));
            for (DWORD i = 0; i < dwNumBone; i++)
            {
                KG3DMesh::_BoneInfo *pBoneInfo = NULL;
                pMesh->GetBoneInfo(i, &pBoneInfo);
                m_comboLTargt.AddString(pBoneInfo->scName.c_str());
            }
            pMesh->Release();
        }
    }*/

	IKG3DModel*   pModel = NULL;
	int nCount = 0;
	IEKG3DMesh *pMesh = NULL;
	//IEKG3DMeshBase *pMeshBase = NULL;
	IEKG3DModelST* pModelST = NULL;

	m_pLModel->GetIEKG3DModelST(&pModelST);
	if (pModelST)
	{
		m_comboLTargt.AddString(TEXT("原点"));
		nCount = m_pLModel->GetNumModel();

		for (int s = 0; s < nCount; s++)
		{
			IEKG3DModel* pieModel = NULL;
			m_pLModel->GetModel(s, &pModel);
			pieModel = dynamic_cast<IEKG3DModel*>(pModel);
			pieModel->GetIEKG3DMesh(&pMesh);
			if (!pMesh)
				continue;
			INT nBoneCount = 0;
			pMesh->GetNumBones(&nBoneCount);
			for (INT i = 0; i < nBoneCount; i++)
			{
				LPCTSTR pszName = NULL;
				pMesh->GetBoneInfoName(i, &pszName);
				m_comboLTargt.AddString(pszName);
			}

		}
	}
	else if (m_pLModel)
	{
		m_pLModel->GetIEKG3DMesh(&pMesh);
        if (pMesh)
		{
			INT nBoneCount = 0;
			pMesh->GetNumBones(&nBoneCount);
			m_comboLTargt.AddString(TEXT("原点"));
			for (int i = 0; i < nBoneCount; i++)
			{
				LPCTSTR pszName = NULL;
				pMesh->GetBoneInfoName(i, &pszName);
				m_comboLTargt.AddString(pszName);
			}
		}
	}
}

void KSceneSFXEditChannel::OnCbnSetfocusRModel()
{
    GET_SFX_EDITOR();
    m_comboRModel.ResetContent();
/*

    for (
        std::KG3DSceneEntityList::iterator i = pScene->m_listRenderEntity.begin();
        i != pScene->m_listRenderEntity.end();
    i++
        )
    {
        if ((*i).m_Type == SCENEENTITY_MODEL)
        {
            KG3DModel* pModel = (KG3DModel*)(*i).m_pPointer;      
            char* pName = strrchr(const_cast<char*>(pModel->m_scName.c_str()), '\\');
            int nIndex = m_comboRModel.AddString(++pName);
            m_comboRModel.SetItemDataPtr(nIndex, (void*)pModel);
        }
    }
    //m_comboRModel.AddString(TEXT("原始点"));
*/
	IEKG3DSceneEntity *piEntity = NULL;
	IEKG3DModel* pSelModel = NULL;
	HRESULT hRetCode = E_FAIL;
	SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
	LPCTSTR pszName = NULL;

	GETSCENE(pScene);

	hRetCode = p3DScene->GetFirstRenderEntity(&piEntity);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	while (piEntity)
	{
		hRetCode = piEntity->GetType(&EntityType);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		if (EntityType == SCENEENTITY_MODEL)
		{
			IEKG3DModel* pModel = NULL;
			hRetCode = piEntity->GetPointer((VOID **)&pModel);
			KGLOG_COM_PROCESS_ERROR(hRetCode);

			hRetCode = pModel->GetName(&pszName);
			KGLOG_COM_PROCESS_ERROR(hRetCode);

			char* pName = strrchr(const_cast<char*>(pszName), '\\');
			int nIndex = m_comboRModel.AddString(++pName);
			m_comboRModel.SetItemDataPtr(nIndex, (void*)pModel);
		}
		hRetCode = p3DScene->GetNextRenderEntity(&piEntity);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
	}	
	//m_comboRModel.AddString(TEXT("原始点"));
Exit0:
	return;
}

void KSceneSFXEditChannel::OnCbnSetfocusRTargt()
{
    GET_SFX_EDITOR();
    if (!m_pRModel)
        return;
    m_comboRTargt.ResetContent();
    m_comboRTargt.AddString(TEXT("原始点"));
/*
    KG3DModelST* pModelST = dynamic_cast<KG3DModelST*>(m_pLModel);
    if (pModelST)
    {
        std::vector<KG3DModel*> vecModels = pModelST->m_ModelSet.GetModels();
        m_comboRTargt.AddString(TEXT("原点"));
        for (size_t s = 0; s < vecModels.size(); s++)
        {
            KG3DMeshEx *pMesh     = (KG3DMeshEx*)(vecModels[s]->GetMesh());
            if (!pMesh)
                continue;
            DWORD		dwNumBone = 0;
            pMesh->GetNumBones(dwNumBone);
            for (DWORD i = 0; i < dwNumBone; i++)
            {
                KG3DMesh::_BoneInfo *pBoneInfo = NULL;
                pMesh->GetBoneInfo(i, &pBoneInfo);
                m_comboRTargt.AddString(pBoneInfo->scName.c_str());
            }

            pMesh->Release();
        }
    }
    else if (m_pLModel)
    {
        KG3DMeshEx *pMesh = (KG3DMeshEx*)m_pLModel->GetMesh();
        if (pMesh)
        {
            DWORD dwNumBone = 0;
            pMesh->GetNumBones(dwNumBone);
            m_comboRTargt.AddString(TEXT("原点"));
            for (DWORD i = 0; i < dwNumBone; i++)
            {
                KG3DMesh::_BoneInfo *pBoneInfo = NULL;
                pMesh->GetBoneInfo(i, &pBoneInfo);
                m_comboRTargt.AddString(pBoneInfo->scName.c_str());
            }
            pMesh->Release();
        }
    }
    else
    {

    }
	*/
	IKG3DModel*   pModel = NULL;
	DWORD nCount = 0;
	IEKG3DMesh *pMesh = NULL;
	IEKG3DModelST* pModelST = NULL;

	m_pRModel->GetIEKG3DModelST(&pModelST);
	if (pModelST)
	{
		m_comboRTargt.AddString(TEXT("原点"));
		
		nCount = m_pRModel->GetNumModel();
		_ASSERTE(nCount < INT_MAX);

		for (DWORD s = 0; s < nCount; s++)
		{
			IEKG3DModel* pieModel = NULL;
			m_pRModel->GetModel(s, &pModel);
			pieModel = dynamic_cast<IEKG3DModel*>(pModel);

			pieModel->GetIEKG3DMesh(&pMesh);
			if (!pMesh)
				continue;
			INT nBoneCount = 0;
			pMesh->GetNumBones(&nBoneCount);
			for (INT i = 0; i < nBoneCount; i++)
			{
				LPCTSTR pszName = NULL;
				pMesh->GetBoneInfoName(i, &pszName);
				m_comboRTargt.AddString(pszName);
			}

		}
	}
	else if (m_pRModel)
	{
		m_pRModel->GetIEKG3DMesh(&pMesh);
        if (pMesh)
		{
			INT nBoneCount = 0;
			pMesh->GetNumBones(&nBoneCount);
			m_comboRTargt.AddString(TEXT("原点"));
			for (int i = 0; i < nBoneCount; i++)
			{
				LPCTSTR pszName = NULL;
				pMesh->GetBoneInfoName(i, &pszName);
				m_comboRTargt.AddString(pszName);
			}
		}
	}
}

void KSceneSFXEditChannel::UpdateChanel()
{
    GET_SFX_EDITOR();
    int nLFlag = 0;
    int nRFlag = 0;

    if (m_strLTar == TEXT("原点"))   
        nLFlag = 0;
    else
        nLFlag = 1;

    if (m_strRTar == TEXT("原点"))   
        nRFlag = 0;
    else
        nRFlag = 1;

	GETSFX(pScene);
    if (pSFX)
    {
        pScene->AdhereToModelEx(
            m_pLModel,
            m_strLTar.c_str(),
            nLFlag,
            m_pRModel,
            m_strRTar.c_str(),
            nRFlag
            );
    }
}

void KSceneSFXEditChannel::Check()
{
    GET_SFX_EDITOR();     
    /*int nHaveLFlag = FALSE;
    int nHaveRFlag = FALSE;
    for (DWORD i = 0; i < pScene->m_vecRefModels.size(); i++)
    {
        if (m_pLModel == pScene->m_vecRefModels[i])
            nHaveLFlag = TRUE;
        if (m_pRModel == pScene->m_vecRefModels[i])
            nHaveRFlag = TRUE;
    }           
    if (!nHaveLFlag)
        m_pLModel = NULL;
    if (!nHaveRFlag)
        m_pRModel = NULL;
	*/
	if (!pScene->HaveModel(m_pLModel))
		m_pLModel = NULL;
	if (!pScene->HaveModel(m_pRModel))
		m_pRModel = NULL;
}

void KSceneSFXEditChannel::OnBnClickedSetTexture()
{
    GET_SFX_EDITOR();
	GETSFX(pScene);
	GETCHANNEL(pSFX);
    if (pSFX && pChannel)
    {
        char szFilters[] = "Texture Files|*.TGA;*.BMP;*.JPG;*.DDS;*.PNG|\
                           (*.TGA)|*.TGA|\
                           (*.BMP)|*.BMP|\
                           (*.JPG)|*.JPG|\
                           (*.DDS)|*.DDS|\
                           (*.PNG)|*.PNG|\
                           All Files (*.*)|*.*||";

        CFileDialog fileDlg(
            TRUE, 
            NULL,
            NULL,
            OFN_EXPLORER|OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, 
            szFilters,
            this
        );
        if (fileDlg.DoModal() == IDOK)
        {
            TCHAR szFullPath[MAX_PATH];
            TCHAR szRetPath[MAX_PATH];
            *szRetPath = '\0';
            sprintf(szFullPath, TEXT("%s"), fileDlg.GetPathName().GetBuffer());
            g_GetFilePathFromFullPath(szRetPath, szFullPath);
            pChannel->SetTexture(szRetPath);
            m_editTexture.SetWindowText(szRetPath);   
            m_wndMinView.Draw(const_cast<LPSTR>(szFullPath));
        }
    }
}

void KSceneSFXEditChannel::OnBnClickedButtonAdd()
{
    GET_SFX_EDITOR();
	GETSFX(pScene);
   
    IEKG3DSFXChannel* pChannel = NULL;
    pSFX->AddChannel(&pChannel);
   
    m_buttonAdd.EnableWindow(FALSE);
    m_buttonDel.EnableWindow(TRUE);
    UpdateUI();
}

void KSceneSFXEditChannel::OnBnClickedButtonDel()
{
    GET_SFX_EDITOR();
	GETSFX(pScene);
	pSFX->RemoveChannel();
    m_buttonAdd.EnableWindow(TRUE);
    m_buttonDel.EnableWindow(FALSE);
    UpdateUI();
}

BOOL KSceneSFXEditChannel::OnSetActive()
{
    UpdateUI();
    return TRUE;
}

BOOL KSceneSFXEditChannel::OnInitDialog()
{
    int nResult  = CDialog::OnInitDialog();
    UpdateUI();
    return nResult;
}

void KSceneSFXEditChannel::UpdateUI()
{
    IEKG3DSceneSFXEditor* pScene = NULL;
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)GetParentFrame();

    if (!pFrame)
        return;

    pScene = pFrame->GetSfxScene();

    if (!pScene)
        return;

  
    //GET_SFX_EDITOR(); 
	GETSFX(pScene);
	GETCHANNEL(pSFX);

    m_editPieceCount.SetWindowText(TEXT(""));
    m_editPieceWidth.SetWindowText(TEXT(""));
    m_editPieceLength.SetWindowText(TEXT(""));
    m_editUSpeed.SetWindowText(TEXT(""));
    m_editVSpeed.SetWindowText(TEXT(""));
    m_editVCount.SetWindowText(TEXT(""));
    m_editTexture.SetWindowText(TEXT(""));
    if (pChannel)
    {
        CString strValue = TEXT("");
        strValue.Format(TEXT("%d"), pChannel->GetPieceCount());
        m_editPieceCount.SetWindowText(strValue);
        strValue.Format(TEXT("%f"), pChannel->GetPieceWidth());
        m_editPieceWidth.SetWindowText(strValue);
        strValue.Format(TEXT("%f"), pChannel->GetPieceLength());
        m_editPieceLength.SetWindowText(strValue);
        strValue.Format(TEXT("%f"), pChannel->GetUSpeed());
        m_editUSpeed.SetWindowText(strValue);
        strValue.Format(TEXT("%f"), pChannel->GetVUpdateFcy());
        m_editVSpeed.SetWindowText(strValue);
        strValue.Format(TEXT("%d"), pChannel->GetVPieceCount());
        m_editVCount.SetWindowText(strValue);
        IEKG3DTexture* pTexture = NULL;
        pChannel->GetTexture(&pTexture);
        if (pTexture)
        {
            LPCTSTR lpstrName = NULL;
            pTexture->GetName(&lpstrName);
            strValue.Format(
                TEXT("%s"), 
                lpstrName
                //pChannel->GetTexture()->m_scName.c_str()
                );
            m_editTexture.SetWindowText(strValue);
            m_wndMinView.Draw(const_cast<LPSTR>(strValue.GetBuffer()));
        }
    }
    

    if (!::IsWindow(m_wndMinView.m_hWnd))
    {       
        RECT rect;
        GetClientRect(&rect);
        rect.top = rect.bottom - 150;
        m_wndMinView.Create(rect, this, pScene);
    }

    m_comboBlend.ResetContent();
    m_comboBlend.AddString(TEXT("Alpha"));
    m_comboBlend.AddString(TEXT("Add"));
    m_comboBlend.AddString(TEXT("Bright"));

    if (pChannel)
        m_comboBlend.SetCurSel(pChannel->GetBlendMode());
}

void KSceneSFXEditChannel::SetPieceCount()
{
    GET_SFX_EDITOR();
	GETSFX(pScene);
	GETCHANNEL(pSFX);
    CString strValue = TEXT("");
    float   fValue = 0.0f;
    int     nVaule = 0;

    m_editPieceCount.GetWindowText(strValue);
    nVaule = atoi(strValue.GetBuffer());
    pChannel->SetPieceCount(nVaule);
}

void KSceneSFXEditChannel::SetPieceWidth()
{
    GET_SFX_EDITOR();
	GETSFX(pScene);
	GETCHANNEL(pSFX);
    CString strValue = TEXT("");
    float   fValue = 0.0f;
    int     nVaule = 0;
    if (pChannel)
    {
        m_editPieceWidth.GetWindowText(strValue);
        fValue = static_cast<float>(atof(strValue.GetBuffer()));
        pChannel->SetPieceWidth(fValue);
    }
}

void KSceneSFXEditChannel::SetPieceLength()
{
    GET_SFX_EDITOR();
	GETSFX(pScene);
	GETCHANNEL(pSFX);
    CString strValue = TEXT("");
    float   fValue = 0.0f;
    int     nVaule = 0;
    if (pChannel)
    {
        m_editPieceLength.GetWindowText(strValue);
        fValue = static_cast<float>(atof(strValue.GetBuffer()));
        //pScene->m_lpSFX->m_pChannel->SetPieceLength(fValue);
        pChannel->SetPieceLength(fValue);
    }
}

void KSceneSFXEditChannel::SetUSpeed()
{
    GET_SFX_EDITOR();
	GETSFX(pScene);
	GETCHANNEL(pSFX);
    
	CString strValue = TEXT("");
    float   fValue = 0.0f;
    int     nVaule = 0;
    if (pChannel)
    {
        m_editUSpeed.GetWindowText(strValue);
        fValue = static_cast<float>(atof(strValue.GetBuffer()));
        pChannel->SetUSpeed(fValue);
    }
}

void KSceneSFXEditChannel::SetVSpeed()
{
    GET_SFX_EDITOR();
	GETSFX(pScene);
	GETCHANNEL(pSFX);

    CString strValue = TEXT("");
    float   fValue = 0.0f;
    int     nVaule = 0;
    if (pChannel)
    {
        m_editVSpeed.GetWindowText(strValue);
        fValue = static_cast<float>(atof(strValue.GetBuffer()));
        pChannel->SetVUpdateFcy(fValue);
    }
}

void KSceneSFXEditChannel::SetVCount()
{
    GET_SFX_EDITOR();
	GETSFX(pScene);
	GETCHANNEL(pSFX);
    CString strValue = TEXT("");
    float   fValue = 0.0f;
    int     nVaule = 0;
    if (pChannel)
    {
        m_editVCount.GetWindowText(strValue);
        nVaule = atoi(strValue.GetBuffer());
        pChannel->SetVPieceCount(nVaule);
    }
}

LRESULT KSceneSFXEditChannel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_EDIT_RECEIVE_ENTER:
        {
            switch (wParam)
            {
            case IDC_EDIT_PIECE_COUNT:
                SetPieceCount();
                break;
            case IDC_EDIT_PIECE_WIDTH:
                SetPieceWidth();
                break;
            case IDC_EDIT_PIECE_LENGTH:
                SetPieceLength();
                break;
            case IDC_EDIT_V_COUNT:
                SetVCount();
                break;
            case IDC_EDIT_U_SPEED:
                SetUSpeed();
                break;
            case IDC_EDIT_V_SPEED:
                SetVSpeed();
                break;
            default:
                break;
            }
            break;
        }
    default:
        break;
    }

//Exit0:
    return CDialog::WindowProc(message, wParam, lParam);
}

