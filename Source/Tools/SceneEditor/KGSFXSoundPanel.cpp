// KGSFXSoundPanel.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSFXSoundPanel.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSFX.h"
#include "..\KG3DEngine\KG3DSFXFileDefine.h"

// KGSFXSoundPanel dialog

IMPLEMENT_DYNAMIC(KGSFXSoundPanel, CDialog)

KGSFXSoundPanel::KGSFXSoundPanel(CWnd* pParent /*=NULL*/)
	: CDialog(KGSFXSoundPanel::IDD, pParent)
    , m_nProbability(0)
{
    m_fSFXKey = -1.f;
}

KGSFXSoundPanel::~KGSFXSoundPanel()
{
}

void KGSFXSoundPanel::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_ADD,        m_buttonAdd);
    DDX_Control(pDX, IDC_BUTTON_DEL,        m_buttonDel);
    DDX_Control(pDX, IDC_BUTTON_ADD_NULL,        m_buttonEdit);
    DDX_Text(pDX, IDC_EDIT_FILE,            m_EditFileName);
    DDX_Text(pDX, IDC_EDIT_VOL,             m_fVolume);
    DDX_Text(pDX, IDC_EDIT_MIN_DIS,         m_fMinDis);
    DDX_Text(pDX, IDC_EDIT_MAX_DIS,         m_fMaxDis);
    DDX_Text(pDX, IDC_EDIT_FRAME,           m_fSFXKey);
    DDV_MinMaxFloat(pDX, m_fVolume, 0.f, 100.f);
    DDV_MinMaxFloat(pDX, m_fMinDis, 0.f, 100000000.f);
    DDV_MinMaxFloat(pDX, m_fMaxDis, 0.f, 100000000.f);
    DDX_Control(pDX, IDC_COMBO_MODE,        m_Mode);
    DDX_Check(pDX, IDC_CHECK_LOOP,      m_bLoop);
    DDX_Check(pDX, IDC_CHECK_CONTINUE,  m_bContinue);
    DDX_Control(pDX, IDC_LIST_SOUND, m_ListBoxSound);
    DDX_Text(pDX, IDC_EDIT_PROBABILITY, m_nProbability);
	DDV_MinMaxInt(pDX, m_nProbability, 0, 10000);
}


BEGIN_MESSAGE_MAP(KGSFXSoundPanel, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ADD,  OnButtonAdd)
    ON_BN_CLICKED(IDC_BUTTON_DEL,  OnButtonDel)
    ON_BN_CLICKED(IDC_BUTTON_ADD_NULL, OnButtonAddNull)
    ON_CBN_SELCHANGE(IDC_COMBO_MODE, &KGSFXSoundPanel::OnCbnSelchangeComboMode)
    ON_BN_CLICKED(IDC_CHECK_LOOP,       &KGSFXSoundPanel::OnBnClickedCheckLoop)
    ON_BN_CLICKED(IDC_CHECK_CONTINUE,   &KGSFXSoundPanel::OnBnClickedCheckContinue)
    ON_LBN_SELCHANGE(IDC_LIST_SOUND, &KGSFXSoundPanel::OnLbnSelchangeListSound)
    ON_LBN_DBLCLK(IDC_LIST_SOUND, &KGSFXSoundPanel::OnLbnDblclkListSound)
END_MESSAGE_MAP()

BOOL KGSFXSoundPanel::PreTranslateMessage(MSG* pMsg)
{   
    if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_RETURN || (pMsg->message == WM_KILLFOCUS && pMsg->hwnd != GetSafeHwnd()))
    {
        UpdateData();
        Modify();
        UpdateInfo();
        return true;
    }
    return  CDialog::PreTranslateMessage(pMsg);
}

void KGSFXSoundPanel::OnButtonAdd()
{
    GET_SFX_EDITOR();

    IEKG3DSFX* pSFX = NULL;
    pScene->GetSFX(&pSFX); 

    if (!pSFX)
        return;

    int nSel = m_ListBoxSound.GetCurSel();

    if (nSel == CB_ERR)
        nSel = 0;

    TCHAR szDef[MAX_PATH];
    g_GetFullPath(szDef, "\\data\\sound\\");

    CFileDialog Dlg(true);
    Dlg.GetOFN().lpstrInitialDir = szDef;


    if (Dlg.DoModal() == IDOK)
    {
        TCHAR szRetPath[MAX_PATH];
        m_EditFileName = Dlg.GetPathName();
        g_GetFilePathFromFullPath(szRetPath, m_EditFileName.GetBuffer());
        m_EditFileName = szRetPath;
        UpdateData(false);
        pSFX->InsertSoundKey(
            szRetPath, 
            0, 
            100.f,
            pSFX->GetCurrentFrame(), 
            1.f, 
            50.f,
            false,
            false,
            1,
            nSel
            );
        UpdateUI();
        pFrame->GetKeysFrame()->UpdateSliderTics<_SoundInfo>();
        

        //UpdateUI(m_fSFXKey);
    }
}

void KGSFXSoundPanel::OnButtonDel()
{
    if (m_fSFXKey == -1.f)
        return;

    GET_SFX_EDITOR();

    IEKG3DSFX* pSFX = NULL;
    pScene->GetSFX(&pSFX);

    if (!pSFX)
        return;

    int nSel = m_ListBoxSound.GetCurSel();

    if (nSel == CB_ERR)
        return;

    pSFX->RemoveSoundKey(m_fSFXKey, nSel);
    UpdateUI();

    pFrame->GetKeysFrame()->UpdateSliderTics<_SoundInfo>();


}

void KGSFXSoundPanel::Modify()  
{
    if (m_fSFXKey == -1.f)
        return;

    GET_SFX_EDITOR();

    IEKG3DSFX* pSFX = NULL;
    pScene->GetSFX(&pSFX);

    if (!pSFX)
        return;

    int nSel = m_ListBoxSound.GetCurSel();

    if (nSel == CB_ERR)
        return;

    UpdateData();

    pSFX->ModifySoundKey(
        m_EditFileName.GetBuffer(), 
        m_Mode.GetCurSel(),
        m_fVolume, 
        m_fSFXKey,
        m_fMinDis,
        m_fMaxDis,
        m_bLoop,
        m_bContinue,
        m_nProbability,
        nSel
        );



}

void KGSFXSoundPanel::OnButtonAddNull()
{
    GET_SFX_EDITOR();

    IEKG3DSFX* pSFX = NULL;
    pScene->GetSFX(&pSFX); 

    if (!pSFX)
        return;

    m_fSFXKey = pSFX->GetCurrentFrame();

    int nSel = m_ListBoxSound.GetCurSel();

    if (nSel == CB_ERR)
        nSel = 0;

    pSFX->InsertSoundKey(
        NULL, 
        0, 
        100.f,
        m_fSFXKey,
        1.f, 
        50.f,
        false,
        false,
        1,
        nSel
        );

    UpdateUI();
    pFrame->GetKeysFrame()->UpdateSliderTics<_SoundInfo>();

}

void KGSFXSoundPanel::UpdateUI()
{
    GET_SFX_EDITOR();

    IEKG3DSFX* pSFX = NULL;
    pScene->GetSFX(&pSFX);

    if (!pSFX)
        return;

    float fFrame = pSFX->GetCurrentFrame();
    m_fSFXKey = fFrame;

    TCHAR szFileName[MAX_PATH];
    DWORD Mode;
    int nSoundKeyNum = pSFX->GetSoundKeyNum(m_fSFXKey);

    m_ListBoxSound.ResetContent();

    if (nSoundKeyNum > 0)
    {
        for (int i = 0; i < nSoundKeyNum; ++i)
        {
            char text[MAX_PATH];

            HRESULT hr = pSFX->QuerySoundKey(
                szFileName,
                Mode,
                m_fVolume,
                m_fSFXKey, 
                m_fMinDis,
                m_fMaxDis,
                m_bLoop,
                m_bContinue,
                m_nProbability,
                i
                );
            if (SUCCEEDED(hr))
            {
                if (*szFileName == 0)
                {
                      sprintf(text, "%02d : null", i);
                }
                else
                {
                    char* t = strrchr(szFileName, '\\');

                    if (!t)
                        t = szFileName;
                    else
                        ++t;

                    sprintf(text, "%02d : %s", i, t);
                }


            }
            else
            {
                sprintf(text, "%02d : null", i);
            }

            m_ListBoxSound.AddString(text);
        }

        m_ListBoxSound.SetCurSel(nSoundKeyNum - 1);

        m_EditFileName = szFileName;
        m_ListBoxSound.EnableWindow(true);
        //m_buttonAdd.EnableWindow(false);
        //m_buttonDel.EnableWindow(true);
        //m_buttonEdit.EnableWindow(true);
        GetDlgItem(IDC_EDIT_VOL)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_MIN_DIS)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_MAX_DIS)->EnableWindow(true);
        GetDlgItem(IDC_CHECK_LOOP)->EnableWindow(true);
        GetDlgItem(IDC_CHECK_CONTINUE)->EnableWindow(true);
        m_Mode.EnableWindow(true);

        m_Mode.SetCurSel(Mode);
    }
    else
    {
        m_EditFileName = TEXT("无音效帧");
        m_ListBoxSound.EnableWindow(false);
        //m_buttonAdd.EnableWindow(true);
        //m_buttonDel.EnableWindow(false);
        //m_buttonEdit.EnableWindow(false);
        GetDlgItem(IDC_EDIT_VOL)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_MIN_DIS)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_MAX_DIS)->EnableWindow(false);
        GetDlgItem(IDC_CHECK_LOOP)->EnableWindow(false);
        GetDlgItem(IDC_CHECK_CONTINUE)->EnableWindow(false);
        m_Mode.EnableWindow(false);

        m_Mode.SetCurSel(0);
        m_fVolume = 0;
        m_fSFXKey = -1.f;
        m_fMaxDis = 0;
        m_fMinDis = 0;
        m_bLoop   = false;
        m_bContinue = false;
    }

    UpdateData(FALSE);

    //pFrame->GetKeysFrame()->SetTimeLine(
    //    "音效关键帧", pSFX->GetSoundTimeLine(0), SFX_TLDT_SOUND
    //    );
    
}

void KGSFXSoundPanel::UpdateInfo()
{
    GET_SFX_EDITOR();

    IEKG3DSFX* pSFX = NULL;
    pScene->GetSFX(&pSFX);

    if (!pSFX)
        return;

    int nSel = m_ListBoxSound.GetCurSel();

    if (nSel == CB_ERR)
        return;

    TCHAR szFileName[MAX_PATH];

    float fFrame = pSFX->GetCurrentFrame();
    DWORD Mode = 0;

    HRESULT hr = pSFX->QuerySoundKey(
        szFileName,
        Mode,
        m_fVolume,
        fFrame, 
        m_fMinDis,
        m_fMaxDis,
        m_bLoop,
        m_bContinue,
        m_nProbability,
        nSel
        );

    m_EditFileName = szFileName;
    m_Mode.SetCurSel(Mode);

    UpdateData(FALSE);
}

BOOL KGSFXSoundPanel::OnInitDialog()
{
    BOOL r = CDialog::OnInitDialog();

    return r;
}

BOOL KGSFXSoundPanel::OnSetActive()
{
    GET_SFX_EDITOR_RET(false);
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    pFrame->GetKeysFrame()->SetTimeLine(
        "音效关键帧", pSFX->GetSoundTimeLine(), SFX_TLDT_SOUND
        );

    UpdateUI();
    return TRUE;
}

void KGSFXSoundPanel::OnCbnSelchangeComboMode()
{
    Modify();
    UpdateInfo();
}

void KGSFXSoundPanel::OnBnClickedCheckLoop()
{
    Modify();
    UpdateInfo();
}

void KGSFXSoundPanel::OnBnClickedCheckContinue()
{
    Modify();
    UpdateInfo();
}

void KGSFXSoundPanel::OnLbnSelchangeListSound()
{
   UpdateInfo();
}

void KGSFXSoundPanel::OnLbnDblclkListSound()
{
    TCHAR szDef[MAX_PATH];
    g_GetFullPath(szDef, "\\data\\sound\\");

    CFileDialog Dlg(true);
    Dlg.GetOFN().lpstrInitialDir = szDef;

    if (Dlg.DoModal() == IDOK)
    {
        TCHAR szRetPath[MAX_PATH];
        m_EditFileName = Dlg.GetPathName();
        g_GetFilePathFromFullPath(szRetPath, m_EditFileName.GetBuffer());
        m_EditFileName = szRetPath;
        UpdateData(false);
        Modify();
        UpdateUI();
    }
}
