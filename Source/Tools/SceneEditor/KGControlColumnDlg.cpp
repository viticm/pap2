
#include "stdafx.h"
#include "KGControlColumnDlg.h"
#include "KG3DObjectEditor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGControlColumnDlg, CDialog)

KGControlColumnDlg::KGControlColumnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KGControlColumnDlg::IDD, pParent)
{
	m_pParentWindow = (KG3DObjectEditor*)pParent;
	m_eShowMode = SHOW_MODEL_BIND;
	m_ppIniFile = NULL;
	m_ppTabFile = NULL;
	m_strFrameTime = "";
}

KGControlColumnDlg::~KGControlColumnDlg()
{
}

void KGControlColumnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MOD_LIST, m_comboxModList);
	DDX_Control(pDX, IDC_ANI_LIST, m_comboxAniList);
	DDX_Control(pDX, IDC_SLIDER, m_sliderSpeed);
	DDX_Text(pDX, IDC_FRAME, m_strFrameTime);
}

BEGIN_MESSAGE_MAP(KGControlColumnDlg, CDialog)
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_SPEED_MINUS, OnMunisAniSpeed)
	ON_BN_CLICKED(IDC_BUTTON_SPEED_ADD, OnAddAniSpeed)
	ON_BN_CLICKED(IDC_BUTTON_STOP,  OnAniStop)
	ON_BN_CLICKED(IDC_BUTTON_PALY,  OnAniPlay)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnAniPause)
	ON_BN_CLICKED(IDC_BUTTON_EDIT,  OnEdit)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_ANI_LIST, OnCbnAniSelect)
	ON_CBN_SELCHANGE(IDC_MOD_LIST, OnCbnModSelect)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &KGControlColumnDlg::OnBnClickedButtonSave)
	ON_EN_CHANGE(IDC_FRAME, &KGControlColumnDlg::OnEnChangeFrame)
END_MESSAGE_MAP()

int KGControlColumnDlg::SetShowMode(SHOW_MODEL eShowMode)
{
	m_eShowMode = eShowMode;
	return true;
}

SHOW_MODEL KGControlColumnDlg::GetShowMode()
{
	return m_eShowMode;
}

int  KGControlColumnDlg::BandData(ITabFile** ppTabFile, IIniFile** ppIniFile)
{
	m_ppTabFile = ppTabFile;
	m_ppIniFile = ppIniFile;

	return true;
}

int KGControlColumnDlg::Update(int nCurrentRow)
{
	int nResult  = false;
	int nRetCode = false;

	char szFieldName[64];
	char szFieldPate[64];

	*szFieldName  = '\0';
	*szFieldPate  = '\0';

	int nColsCount = 0;
	int nCurCol	   = 0;
	int nIndex     = 0;

    char szPresentID[12];

	KGLOG_PROCESS_ERROR(nCurrentRow > 2);
	KGLOG_PROCESS_ERROR(m_ppTabFile);
	KGLOG_PROCESS_ERROR(*m_ppTabFile);

    (*m_ppTabFile)->GetString(
        nCurrentRow, 
        TEXT("RepresentID"), 
        "", 
        szPresentID,
        sizeof(szPresentID)
        );
    KGLOG_PROCESS_ERROR(*szPresentID != '\0');

	nColsCount = (*m_ppTabFile)->GetWidth();

	while (m_comboxAniList.GetCount() >= 1)
	{
		m_comboxAniList.DeleteString(0);
	}

	while (m_comboxModList.GetCount() >= 1)
	{
		m_comboxModList.DeleteString(0);
	}

	for (int i = 0; i < ANI_COUNT_MAX; i++)
	{
		*(m_szAinFieldName[i]) = '\0';
		*(m_szModFieldName[i]) = '\0';
	}

	nIndex = 0;
	//for (nCurCol = 1; nCurCol <= nColsCount; nCurCol++)
	{
        TCHAR szFullPath[MAX_PATH];
        g_GetFullPath(szFullPath, TEXT("\\Represent\\data\\skill\\animation_npc.txt"));
        ITabFile* pAinTabFile = g_OpenTabFile(szFullPath);
        if (pAinTabFile)
        {
            int nRows = pAinTabFile->GetHeight();
            for (int i = 1; i <= nRows; i++)
            {
                char szID[12];
                pAinTabFile->GetString(
                    i, 
                    TEXT("RepresentID"), 
                    "", 
                    szID,
                    sizeof(szID)
                    );
                if (!stricmp(szID, szPresentID))
	{
                    char szAinPath[MAX_PATH];
                    char szAinID[12];

                    pAinTabFile->GetString(
                        i, 
                        TEXT("AnimationID"), 
                        "", 
                        szAinID,
                        sizeof(szAinID)
                        );

                    pAinTabFile->GetString(
                        i, 
                        TEXT("AnimationFile"), 
                        "", 
                        szAinPath,
                        sizeof(szAinPath)
                        );


                    strncpy_s(m_szAinFieldName[nIndex], szAinPath, sizeof(m_szAinFieldName[nIndex]));
                    nIndex++;
                    m_comboxAniList.AddString(szAinID);
                    
                }
            }
        }
        SAFE_RELEASE(pAinTabFile);


		/*strncpy_s(
			szFieldName, (*m_ppTabFile)->GetColName(nCurCol), sizeof(szFieldName)
		);
		if (!strstr(szFieldName, _T("AniFile")))
			continue;

		strncpy_s(m_szAinFieldName[nIndex], szFieldName, sizeof(m_szAinFieldName[nIndex]));
		nIndex++;
		(*m_ppIniFile)->GetString(
			szFieldName, _T("Name"), _T(""), szFieldPate, sizeof(szFieldPate)
		);
		m_comboxAniList.AddString(szFieldPate);*/
	}

	nIndex = 0;
	for (nCurCol = 1; nCurCol <= nColsCount; nCurCol++)
	{
		strncpy_s(
			szFieldName, (*m_ppTabFile)->GetColName(nCurCol), sizeof(szFieldName)
		);
		if (!strstr(szFieldName, _T("ModelFile")))
			continue;

		strncpy_s(m_szModFieldName[nIndex], szFieldName, sizeof(m_szModFieldName[nIndex]));
		nIndex++;
		(*m_ppIniFile)->GetString(
			szFieldName, _T("Name"), _T(""), szFieldPate, sizeof(szFieldPate)
		);
		m_comboxModList.AddString(szFieldPate);
	}

	m_nCurrentRow = nCurrentRow;

	nResult = true;
Exit0:
	return nResult;
}

void KGControlColumnDlg::OnSize(UINT nType, int cx, int cy)
{
	CRect rect1;
	CRect rect2;
	GetClientRect(&rect1);
	m_sliderSpeed.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	rect2.right = rect1.right - 4;

	m_sliderSpeed.MoveWindow(&rect2);
}


void KGControlColumnDlg::OnCbnModSelect()
{
	int nResult  = false;
	int nRetCode = false;

	int nSelIndex = -1;

	KGLOG_PROCESS_ERROR(m_ppTabFile);
	KGLOG_PROCESS_ERROR(*m_ppTabFile);

	nSelIndex = m_comboxModList.GetCurSel();
	KGLOG_PROCESS_ERROR(nSelIndex != -1);

	KGLOG_PROCESS_ERROR(*(m_szModFieldName[nSelIndex]) != '\0');

	m_pParentWindow->m_pObjectScene->SetCurModel(m_szModFieldName[nSelIndex]);
	m_pParentWindow->m_pObjectScene->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, m_fAniSpeed);

	OnCbnAniSelect();

	nResult = true;
Exit0:
	return;
}

void KGControlColumnDlg::OnCbnAniSelect()
{
	int nResult  = false;
	int nRetCode = false;

	int  nSelIndex = 0;

	KGLOG_PROCESS_ERROR(m_ppTabFile);
	KGLOG_PROCESS_ERROR(*m_ppTabFile);

	nSelIndex = m_comboxAniList.GetCurSel();
	KGLOG_PROCESS_ERROR(nSelIndex != -1);

	KGLOG_PROCESS_ERROR(*(m_szAinFieldName[nSelIndex]) != '\0');

	m_pParentWindow->m_pObjectScene->SetCurAnimt(m_szAinFieldName[nSelIndex]);
	m_ePlayState = PLAY_STATE_PALY;

	GetDlgItem(IDC_BUTTON_PALY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);

	if (!GetDlgItem(IDC_SLIDER)->IsWindowEnabled())
		GetDlgItem(IDC_SLIDER)->EnableWindow(TRUE);

	GetDlgItem(IDC_BUTTON_SPEED_MINUS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SPEED_ADD)->EnableWindow(TRUE);
	if (m_fAniSpeed <= 0.0f)
		GetDlgItem(IDC_BUTTON_SPEED_MINUS)->EnableWindow(FALSE);
	if (m_fAniSpeed >= 10.0f)
		GetDlgItem(IDC_BUTTON_SPEED_ADD)->EnableWindow(FALSE);

	// show current animation frame time
	if (m_pParentWindow && m_pParentWindow->m_pObjectScene)
	{
		float fCurAniFrameTime;
		m_pParentWindow->m_pObjectScene->GetCurAniFrameTime(&fCurAniFrameTime);
		int nFrameTime = static_cast<int>(fCurAniFrameTime);//m_fCurAniFrameTime);
		m_strFrameTime.Format(TEXT("%d"), nFrameTime);
		UpdateData(FALSE);
	}

	nResult = true;
Exit0:
	return;
}

void KGControlColumnDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nHPos = 0;

	if (pScrollBar == (CWnd*)&m_sliderSpeed /*&& nPos != 0*/)
	{
		nHPos = m_sliderSpeed.GetPos();
		m_fAniSpeed = nHPos / 10.0f;
		m_pParentWindow->m_pObjectScene->SetAnimationSpeed(m_fAniSpeed);

		GetDlgItem(IDC_BUTTON_SPEED_MINUS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SPEED_ADD)->EnableWindow(TRUE);
		if (m_fAniSpeed <= 0.0f)
			GetDlgItem(IDC_BUTTON_SPEED_MINUS)->EnableWindow(FALSE);
		if (m_fAniSpeed >= 10.0f)
			GetDlgItem(IDC_BUTTON_SPEED_ADD)->EnableWindow(FALSE);
	}
}

void KGControlColumnDlg::OnAddAniSpeed()
{
	if (m_fAniSpeed < 10.0f)
	{
		m_fAniSpeed += 0.1f;
		m_pParentWindow->m_pObjectScene->SetAnimationSpeed(m_fAniSpeed);
		m_sliderSpeed.SetPos((int)(m_fAniSpeed * 10));

		GetDlgItem(IDC_BUTTON_SPEED_MINUS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SPEED_ADD)->EnableWindow(TRUE);
		if (m_fAniSpeed <= 0.0f)
			GetDlgItem(IDC_BUTTON_SPEED_MINUS)->EnableWindow(FALSE);
		if (m_fAniSpeed >= 10.0f)
			GetDlgItem(IDC_BUTTON_SPEED_ADD)->EnableWindow(FALSE);
	}
}

void KGControlColumnDlg::OnMunisAniSpeed()
{
	if (m_fAniSpeed > 0.0f)
	{
		m_fAniSpeed -= 0.1f;
		m_pParentWindow->m_pObjectScene->SetAnimationSpeed(m_fAniSpeed);
		m_sliderSpeed.SetPos((int)(m_fAniSpeed * 10));

		GetDlgItem(IDC_BUTTON_SPEED_MINUS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SPEED_ADD)->EnableWindow(TRUE);
		if (m_fAniSpeed <= 0.0f)
			GetDlgItem(IDC_BUTTON_SPEED_MINUS)->EnableWindow(FALSE);
		if (m_fAniSpeed >= 10.0f)
			GetDlgItem(IDC_BUTTON_SPEED_ADD)->EnableWindow(FALSE);
	}
}


void KGControlColumnDlg::OnAniStop()
{
	int nSelIndex = -1;

	KGLOG_PROCESS_ERROR(m_ppTabFile);
	KGLOG_PROCESS_ERROR(*m_ppTabFile);

	nSelIndex = m_comboxModList.GetCurSel();
	KGLOG_PROCESS_ERROR(nSelIndex != -1);

	KGLOG_PROCESS_ERROR(*(m_szModFieldName[nSelIndex]) != '\0');

	m_pParentWindow->m_pObjectScene->SetCurModel(m_szModFieldName[nSelIndex]);
	m_pParentWindow->m_pObjectScene->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, m_fAniSpeed);

	m_ePlayState = PLAY_STATE_STOP;

	GetDlgItem(IDC_BUTTON_PALY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);

Exit0:
	return;
}

void KGControlColumnDlg::OnAniPause()
{
	m_pParentWindow->m_pObjectScene->PauseAnimation(true);
	m_ePlayState = PLAY_STATE_PAUSE;

	GetDlgItem(IDC_BUTTON_PALY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
}

void KGControlColumnDlg::OnAniPlay()
{
	if (m_ePlayState == PLAY_STATE_PAUSE)
		m_pParentWindow->m_pObjectScene->PauseAnimation(false);
	else
		OnCbnAniSelect();
	m_ePlayState = PLAY_STATE_PALY;

	GetDlgItem(IDC_BUTTON_PALY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
}

void KGControlColumnDlg::OnEdit()
{
	int  nSelIndex = -1;
	char szModelFileFullPath[MAX_PATH];
	char szModelFileRelaPath[MAX_PATH];

	*szModelFileRelaPath = '\0';
	*szModelFileFullPath = '\0';

	KGLOG_PROCESS_ERROR(m_ppTabFile);
	KGLOG_PROCESS_ERROR(*m_ppTabFile);

	nSelIndex = m_comboxModList.GetCurSel();
	KGLOG_PROCESS_ERROR(nSelIndex != -1);

	KGLOG_PROCESS_ERROR(*(m_szModFieldName[nSelIndex]) != '\0');

	(*m_ppTabFile)->GetString(
		m_nCurrentRow, m_szModFieldName[nSelIndex], "", szModelFileRelaPath,
		sizeof(szModelFileRelaPath)
	);

	if (*szModelFileRelaPath != '\0')
	{
		g_GetFullPath(szModelFileFullPath, szModelFileRelaPath);
		AfxGetApp()->OpenDocumentFile(szModelFileFullPath);
	}

Exit0:
	return;
}

void KGControlColumnDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if (m_eShowMode == SHOW_MODEL_BIND)
	{
		m_eShowMode = SHOW_MODEL_FLOAT;
	}
	else if (m_eShowMode == SHOW_MODEL_FLOAT)
	{
		m_eShowMode = SHOW_MODEL_BIND;
	}
	else
	{
		m_eShowMode = SHOW_MODEL_HIDE;
	}
	m_pParentWindow->UpdateInterface();
	CDialog::OnNcLButtonDblClk(nHitTest, point);
}

void KGControlColumnDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_eShowMode == SHOW_MODEL_BIND)
	{
		m_eShowMode = SHOW_MODEL_FLOAT;
	}
	else if (m_eShowMode == SHOW_MODEL_FLOAT)
	{
		m_eShowMode = SHOW_MODEL_BIND;
	}
	else
	{
		m_eShowMode = SHOW_MODEL_HIDE;
	}
	m_pParentWindow->UpdateInterface();
	CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL KGControlColumnDlg::OnInitDialog()
{
	int nResult  = FALSE;
	int nRetCode = FALSE;

	nRetCode = CDialog::OnInitDialog();
	KGLOG_PROCESS_ERROR(nRetCode);

	m_ePlayState = PLAY_STATE_STOP;
	m_fAniSpeed = 1.0f;
	m_sliderSpeed.SetRange(0, 100);
	m_sliderSpeed.SetTicFreq(10);
	m_sliderSpeed.SetPos(10);

	GetDlgItem(IDC_BUTTON_SPEED_MINUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SPEED_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_SLIDER)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PALY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);

	m_toolTipCtrl.Create(this);
	m_toolTipCtrl.AddTool(GetDlgItem(IDC_FRAME), TEXT("当前动画帧间隔"));
	m_toolTipCtrl.AddTool(GetDlgItem(IDC_BUTTON_SAVE), TEXT("保存动画帧间隔"));
	//m_toolTipCtrl.SetDelayTime(1000);
	m_toolTipCtrl.SetTipBkColor(RGB(50, 50, 50));
	m_toolTipCtrl.SetTipTextColor(RGB(255, 255, 255));
	m_toolTipCtrl.Activate(TRUE);

	nResult = TRUE;
Exit0:
	return nResult;
}

LRESULT KGControlColumnDlg::OnNcHitTest(CPoint point)
{
	LRESULT lHitTest = CDialog::OnNcHitTest(point);

	if (lHitTest == HTCAPTION && m_eShowMode == SHOW_MODEL_BIND)
	{
		lHitTest =  HTCLIENT;
	}
	return lHitTest;
}


HBRUSH KGControlColumnDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void KGControlColumnDlg::OnCancel()
{
	m_eShowMode = SHOW_MODEL_HIDE;
	ShowWindow(SW_HIDE);
	m_pParentWindow->UpdateInterface();
	return;
}

void KGControlColumnDlg::OnOK()
{
	return;
}

BOOL KGControlColumnDlg::PreTranslateMessage(MSG* pMsg)
{
	m_toolTipCtrl.RelayEvent(pMsg);
	/*if (pMsg->message == WM_MOUSEMOVE)
	{
		m_toolTipCtrl.UpdateTipText(TEXT("当前动画帧间隔"), GetDlgItem(IDC_FRAME));
		m_toolTipCtrl.UpdateTipText(TEXT("保存动画帧间隔"), GetDlgItem(IDC_BUTTON_SAVE));
	}*/
	return CDialog::PreTranslateMessage(pMsg);
}


void KGControlColumnDlg::OnBnClickedButtonSave()
{
	if (m_pParentWindow && m_pParentWindow->m_pObjectScene)
	{
		if (SUCCEEDED(m_pParentWindow->m_pObjectScene->SaveCurAniFrameTime()))
			::MessageBox(
				m_hWnd, TEXT("On Yesh ~ Save Success!!"), TEXT("Save Success"),
				MB_OK
			);
	}
}

void KGControlColumnDlg::OnEnChangeFrame()
{
	UpdateData(TRUE);
	int nFrameTime = max(atoi(m_strFrameTime.GetBuffer()), 1);
	if (m_pParentWindow && m_pParentWindow->m_pObjectScene)
	{
		m_pParentWindow->m_pObjectScene->SetCurAniFrameTime(static_cast<float>(nFrameTime));
	}
	m_strFrameTime.Format(TEXT("%d"), nFrameTime);
	UpdateData(FALSE);
}
