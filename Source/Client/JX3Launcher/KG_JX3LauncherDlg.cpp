// KG_JX3LauncherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KG_JX3LauncherApp.h"
#include "KG_JX3LauncherDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
const int DIALOG_WIDTH                      = 262;

const int LAUNCH_BUTTON_WIDTH               = 183;
const int LAUNCH_BUTTON_HEIGHT              = 33;
const int LAUNCH_BUTTON_TOP_GAP_HEIGHT      = 52;
const int LAUNCH_BBUTTON_MIDDLE_GAP_HEIGHT  = 26;
const int LAUNCH_BBUTTON_BOTTOM_GAP_HEIGHT  = 190;

const int QUIT_BUTTON_WIDTH                 = 13;
const int QUIT_BUTTON_HEIGHT                = 14;
const int QUIT_BUTTON_LEFT_GAP_WIDTH        = 234;  // x position
const int QUIT_BUTTON_TOP_GAP_HEIGHT        = 8;    // y position

const int CLUE_TEXT_WIDTH                   = 204;
const int CLUE_TEXT_HEIGHT                  = 70;
const int CLUE_TEXT_LEFT_GAP_WIDTH          = 32;   // x position, Clue text is not centered.
const int CLUE_TEXT_BOTTOM_GAP_HEIGHT       = 84;
const TCHAR IDLE_CLUE_TEXT[]    = _T("服务器类型选择\n\n您可以在这里选择想要进入的服务器。");

////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG   // Launch debug exe file if JX3Launcher is debug mode.
#define GAME_UPDATER_FILE_NAME      _T("gameupdaterD.exe")
#define GAME_CONFIG_FILE_NAME       _T("GameConfigD.exe")
#else
#define GAME_UPDATER_FILE_NAME      _T("gameupdater.exe")
#define GAME_CONFIG_FILE_NAME       _T("GameConfig.exe")
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
KG_JX3LauncherDlg::KG_JX3LauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KG_JX3LauncherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void KG_JX3LauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_QUIT, m_QuitButton);
    DDX_Control(pDX, IDC_STATIC_CLUE, m_ClueText);
}

BEGIN_MESSAGE_MAP(KG_JX3LauncherDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_NCHITTEST()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDD_LAUNCHER_BUTTON,    OnBnClickedButton)
    ON_BN_CLICKED(IDC_BUTTON_QUIT,        OnCancel)
    ON_MESSAGE(MY_MSG_MOUSE_IN_BUTTON,    OnMouseInLaunchButton)
    ON_MESSAGE(MY_MSG_MOUSE_LEAVE_BUTTON, OnMouseLeaveLaunchButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// KG_JX3LauncherDlg message handlers

BOOL KG_JX3LauncherDlg::OnInitDialog()
{

	int nResult  = false;
	int nRetCode = false;

    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    SetWindowText(_T("剑侠情缘网络版叁"));

    nRetCode = ModifyStyle(WS_CAPTION, WS_MINIMIZEBOX, SWP_DRAWFRAME);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = _LoadConfigParameters();
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadConfigParameters()");

    nRetCode = _InitBackgroundBitmap();
    KGLOG_PROCESS_ERROR(nRetCode && "_InitBackgroundBitmap()");

    nRetCode = _InitDialogRegion();
    KGLOG_PROCESS_ERROR(nRetCode && "_InitDialogRegion()");

    nRetCode = _InitLaunchButtons();
    KGLOG_PROCESS_ERROR(nRetCode && "_InitLaunchButtons()");

    nRetCode = _InitQuitButton();
    KGLOG_PROCESS_ERROR(nRetCode && "_InitQuitButton()");

    nRetCode = _InitClueRegion();
    KGLOG_PROCESS_ERROR(nRetCode && "qing_InitClueRegion()");

	nResult = true;
Exit0:
    if (!nResult)
    {
        MessageBox(_T("加载器启动失败。"), _T("错误"), NULL);
        EndDialog(IDOK);
    }
    return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void KG_JX3LauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR KG_JX3LauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT KG_JX3LauncherDlg::OnNcHitTest(CPoint point)
{
    return HTCAPTION;
}

BOOL KG_JX3LauncherDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect RectDialog;
    CDC cDC;

    if (m_BackgroundBitmap.GetSafeHandle() == NULL)
        return CDialog::OnEraseBkgnd(pDC);

    GetClientRect(&RectDialog);

    cDC.CreateCompatibleDC(pDC);

    cDC.SelectObject(&m_BackgroundBitmap);
    pDC->BitBlt(0, 0, RectDialog.Width(), RectDialog.Height(), &cDC, 0, 0, SRCCOPY);

    cDC.DeleteDC();

    return TRUE;
}

void KG_JX3LauncherDlg::OnDestroy()
{
    _LAUNCH_ITEM_LIST::iterator it;
    for (it = m_LaunchItemList.begin(); it != m_LaunchItemList.end(); ++it)
        KG_DELETE(it->pButton);
}

void KG_JX3LauncherDlg::OnOK()
{
    // Do nothing.
}

void KG_JX3LauncherDlg::OnBnClickedButton()
{
    _LAUNCH_ITEM_LIST::iterator it;

    for (it = m_LaunchItemList.begin(); it != m_LaunchItemList.end(); ++it)
    {
        if (it->pButton == GetFocus())
        {
            _LoadItemExe(it->strPath.c_str());
            EndDialog(IDOK);
        }
    }
}

LRESULT KG_JX3LauncherDlg::OnMouseInLaunchButton(WPARAM wParam, LPARAM lParam)
{
    tstring strClueText;
    _LAUNCH_ITEM_LIST::iterator it;

    for (it = m_LaunchItemList.begin(); it != m_LaunchItemList.end(); ++it)
    {
        if (it->pButton == (KG_BitmapButton *)lParam)
        {
            strClueText  = it->strName + _T("\n\n") + it->strClue;
            m_ClueText.SetWindowText(strClueText.c_str());
            break;
        }
    }
    return NULL;
}

LRESULT KG_JX3LauncherDlg::OnMouseLeaveLaunchButton(WPARAM wParam, LPARAM lParam)
{
    m_ClueText.SetWindowText(IDLE_CLUE_TEXT);
    return NULL;
}

int KG_JX3LauncherDlg::_LoadConfigParameters()
{
	int nResult  = false;
	int nRetCode = false;
    TCHAR szApplicationName[MAX_PATH];
    tstring strIniFileName;
    int nIndex = 0;
    TCHAR szIndex[32];
    TCHAR szBuffer[MAX_PATH];
    _LAUNCH_ITEM LaunchItem;

    nRetCode = GetModuleFileName(NULL, szApplicationName, _countof(szApplicationName));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szApplicationName));

    strIniFileName = szApplicationName;
    nRetCode = (int)strIniFileName.find_last_of(_T('\\'));
    KGLOG_PROCESS_ERROR(nRetCode != (int)tstring::npos);
    strIniFileName.erase(nRetCode + 1);
    strIniFileName += _T("JX3Launcher.ini");

    m_LaunchItemList.clear();
    nIndex = 0;
    while(true)
    {
        nRetCode = _sntprintf(szIndex, _countof(szIndex), _T("%d"), nIndex);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < _countof(szIndex));

        nRetCode = GetPrivateProfileString(
            szIndex, _T("Path"), NULL, szBuffer, _countof(szBuffer), strIniFileName.c_str()
        );
        if (nRetCode == 0)  // Path is necessary, empty means no more items.
            break;

        LaunchItem.strPath = szBuffer;

        nRetCode = GetPrivateProfileString(
            szIndex, _T("Name"), NULL, szBuffer, _countof(szBuffer), strIniFileName.c_str()
        );
        if (nRetCode)
            LaunchItem.strName = szBuffer;
        else
            LaunchItem.strName.clear();

        nRetCode = GetPrivateProfileString(
            szIndex, _T("Clue"), NULL, szBuffer, _countof(szBuffer), strIniFileName.c_str()
        );
        if (nRetCode)
            LaunchItem.strClue = szBuffer;
        else
            LaunchItem.strClue.clear();

        LaunchItem.pButton = NULL;

        m_LaunchItemList.push_back(LaunchItem);

        nIndex++;
    }

    nRetCode = (int)m_LaunchItemList.size();
    KGLOG_PROCESS_ERROR(nRetCode > 0);  // No launch item indicate JX3Launcher.ini may be corrupt.

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3LauncherDlg::_InitBackgroundBitmap()
{
	int nResult  = false;
	int nRetCode = false;
    CDC *pDC = NULL;
    CBitmap TopBitmap;
    CBitmap MiddleBitmap;
    CBitmap BottomBitmap;
    BITMAP bmpTop;
    BITMAP bmpMiddle;
    BITMAP bmpBottom;
    int nBackgroundBitmapTotalHeight = 0;
    int nButtonAndGapTotalHeight     = 0;
    int nDialogHeight                = 0;
    CDC dcBackground;
    CDC dcTop;
    CDC dcMiddle;
    CDC dcBottom;
    CBitmap *pOldBackground = NULL;
    CBitmap *pOldTop        = NULL;
    CBitmap *pOldMiddle     = NULL;
    CBitmap *pOldBottom     = NULL;

    pDC = GetDC();
    KGLOG_PROCESS_ERROR(pDC);

    nRetCode = TopBitmap.LoadBitmap(IDB_BACKGROUND_TOP);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    nRetCode = TopBitmap.GetBitmap(&bmpTop);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = MiddleBitmap.LoadBitmap(IDB_BACKGROUND_MIDDLE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = MiddleBitmap.GetBitmap(&bmpMiddle);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = BottomBitmap.LoadBitmap(IDB_BACKGROUND_BOTTOM);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = BottomBitmap.GetBitmap(&bmpBottom);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    nBackgroundBitmapTotalHeight = bmpTop.bmHeight + bmpMiddle.bmHeight + bmpBottom.bmHeight;
    nButtonAndGapTotalHeight  = LAUNCH_BUTTON_TOP_GAP_HEIGHT + LAUNCH_BBUTTON_BOTTOM_GAP_HEIGHT;
    nButtonAndGapTotalHeight += LAUNCH_BUTTON_HEIGHT * (int)m_LaunchItemList.size();
    if (m_LaunchItemList.size() > 0)
        nButtonAndGapTotalHeight += LAUNCH_BBUTTON_MIDDLE_GAP_HEIGHT * ((int)m_LaunchItemList.size() - 1);
    nDialogHeight = max(nBackgroundBitmapTotalHeight, nButtonAndGapTotalHeight);

    m_BackgroundBitmap.DeleteObject();
    nRetCode = m_BackgroundBitmap.CreateCompatibleBitmap(pDC, DIALOG_WIDTH, nDialogHeight);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    nRetCode = dcBackground.CreateCompatibleDC(pDC);
    KGLOG_PROCESS_ERROR(nRetCode);

    pOldBackground = dcBackground.SelectObject(&m_BackgroundBitmap);
    KGLOG_PROCESS_ERROR(pOldBackground);

    // Stretch top background bitmap only by width.
    nRetCode = dcTop.CreateCompatibleDC(pDC);
    KGLOG_PROCESS_ERROR(nRetCode);

    pOldTop = dcTop.SelectObject(&TopBitmap);
    KGLOG_PROCESS_ERROR(pOldTop);

    nRetCode = dcBackground.StretchBlt(
        0, 0, DIALOG_WIDTH, bmpTop.bmHeight, &dcTop, 0, 0, bmpTop.bmWidth, bmpTop.bmHeight, SRCCOPY
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    // Stretch middle background bitmap both by width and height.
    nRetCode = dcMiddle.CreateCompatibleDC(pDC);
    KGLOG_PROCESS_ERROR(nRetCode);

    pOldMiddle = dcMiddle.SelectObject(&MiddleBitmap);
    KGLOG_PROCESS_ERROR(pOldMiddle);

    nRetCode = dcBackground.StretchBlt(
        0, bmpTop.bmHeight, DIALOG_WIDTH, nDialogHeight - bmpTop.bmHeight - bmpBottom.bmHeight,
        &dcMiddle, 0, 0, bmpMiddle.bmWidth, bmpMiddle.bmHeight, SRCCOPY
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    // Stretch bottom background bitmap only by width.
    nRetCode = dcBottom.CreateCompatibleDC(pDC);
    KGLOG_PROCESS_ERROR(nRetCode);

    pOldBottom = dcBottom.SelectObject(&BottomBitmap);
    KGLOG_PROCESS_ERROR(pOldBottom);

    nRetCode = dcBackground.StretchBlt(
        0, nDialogHeight - bmpBottom.bmHeight, DIALOG_WIDTH, bmpBottom.bmHeight,
        &dcBottom, 0, 0, bmpBottom.bmWidth, bmpBottom.bmHeight, SRCCOPY
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
 Exit0:
    if (pOldBottom)
        dcBottom.SelectObject(pOldBottom);
    dcBottom.DeleteDC();
    if (pOldMiddle)
        dcMiddle.SelectObject(pOldMiddle);
    dcMiddle.DeleteDC();
    if (pOldTop)
        dcTop.SelectObject(pOldTop);
    dcTop.DeleteDC();
    if (pOldBackground)
        dcBackground.SelectObject(pOldBackground);
    dcBackground.DeleteDC();
    if (pDC)
    {
        ReleaseDC(pDC);
        pDC = NULL;
    }
	return nResult;
}

int KG_JX3LauncherDlg::_InitDialogRegion()
{
	int nResult  = false;
	int nRetCode = false;
    RECT DialogRect;
    BITMAP bmp;
    CDC dcBackground;
    CBitmap *pOldBitmap = NULL;
    CRgn DialogRgn;
    CRgn TempRgn;
    int nY = 0;
    int nX = 0;
    int nXLeft = 0;
    int nXRigth = 0;
    const COLORREF TRANSPARENT_COLOR = RGB(255, 87, 226);

    // Set the width and height of dialog as same as background bitmap.
    nRetCode = m_BackgroundBitmap.GetBitmap(&bmp);
    KGLOG_PROCESS_ERROR(nRetCode);

    DialogRect.left   = 0;
    DialogRect.top    = 0;
    DialogRect.right  = bmp.bmWidth;
    DialogRect.bottom = bmp.bmHeight;
    MoveWindow(&DialogRect);
    CenterWindow();

    nRetCode = dcBackground.CreateCompatibleDC(NULL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pOldBitmap = dcBackground.SelectObject(&m_BackgroundBitmap);
    KGLOG_PROCESS_ERROR(pOldBitmap);

    DialogRgn.DeleteObject();
    nRetCode = DialogRgn.CreateRectRgn(0, 0, 0, 0);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (nY = 0; nY <= bmp.bmHeight; nY++)
    {
        nX = 0;
        while (true)
        {
            while (nX <= bmp.bmWidth && dcBackground.GetPixel(nX, nY) == TRANSPARENT_COLOR)
                nX++;
            nXLeft = nX;

            while (nX <= bmp.bmWidth && dcBackground.GetPixel(nX, nY) != TRANSPARENT_COLOR)
                nX++;
            nXRigth = nX;

            TempRgn.DeleteObject();
            nRetCode = TempRgn.CreateRectRgn(nXLeft, nY, nXRigth, nY + 1);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = DialogRgn.CombineRgn(&DialogRgn, &TempRgn, RGN_OR);
            KGLOG_PROCESS_ERROR(nRetCode);

            if (nX >= bmp.bmWidth)
                break;
        }
    }

    nRetCode = SetWindowRgn(DialogRgn, TRUE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = SetForegroundWindow();
    // KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    TempRgn.DeleteObject();
    DialogRgn.DeleteObject();
    if (pOldBitmap)
        dcBackground.SelectObject(pOldBitmap);
    dcBackground.DeleteDC();
	return nResult;
}

int KG_JX3LauncherDlg::_InitLaunchButtons()
{
	int nResult  = false;
	int nRetCode = false;
    RECT ButtonRect;
    _LAUNCH_ITEM_LIST::iterator it;

    m_LaunchButtonTextFont.DeleteObject();
    nRetCode = m_LaunchButtonTextFont.CreateFont(
        15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    ButtonRect.top    = LAUNCH_BUTTON_TOP_GAP_HEIGHT;
    ButtonRect.bottom = ButtonRect.top + LAUNCH_BUTTON_HEIGHT;
    ButtonRect.left   = (DIALOG_WIDTH - LAUNCH_BUTTON_WIDTH) / 2;
    ButtonRect.right  = ButtonRect.left + LAUNCH_BUTTON_WIDTH;
    for (it = m_LaunchItemList.begin(); it != m_LaunchItemList.end(); ++it)
    {
        ASSERT(it->pButton == NULL);
        it->pButton = new KG_BitmapButton();
        KGLOG_PROCESS_ERROR(it->pButton);

        nRetCode = it->pButton->Create(
            it->strName.c_str(), WS_VISIBLE | BS_OWNERDRAW, ButtonRect, this, IDD_LAUNCHER_BUTTON
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = it->pButton->LoadBitmaps(
            IDB_LAUNCH_BUTTON_NORMAL, IDB_LAUNCH_BUTTON_DOWN, IDB_LAUNCH_BUTTON_OVER
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        it->pButton->SetTextColor(RGB(255, 255, 255));
        it->pButton->SetFont(&m_LaunchButtonTextFont);
        it->pButton->SetWindowText(it->strName.c_str());

        ButtonRect.top    += LAUNCH_BUTTON_HEIGHT + LAUNCH_BBUTTON_MIDDLE_GAP_HEIGHT;
        ButtonRect.bottom += LAUNCH_BUTTON_HEIGHT + LAUNCH_BBUTTON_MIDDLE_GAP_HEIGHT;
    }    

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3LauncherDlg::_InitQuitButton()
{
    int nResult  = false;
    int nRetCode = false;

    m_QuitButton.MoveWindow(
        QUIT_BUTTON_LEFT_GAP_WIDTH, QUIT_BUTTON_TOP_GAP_HEIGHT, QUIT_BUTTON_WIDTH, QUIT_BUTTON_HEIGHT
    );

    nRetCode = m_QuitButton.LoadBitmaps(
        IDB_QUIT_BUTTON_NORMAL, IDB_QUIT_BUTTON_DOWN, IDB_QUIT_BUTTON_OVER
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KG_JX3LauncherDlg::_InitClueRegion()
{
	int nResult  = false;
    RECT ClueTextRect;
    RECT DialogRect;
    _LAUNCH_ITEM_LIST::iterator it;

    GetClientRect(&DialogRect);

    ClueTextRect.top    = DialogRect.bottom - CLUE_TEXT_BOTTOM_GAP_HEIGHT - CLUE_TEXT_HEIGHT;
    ClueTextRect.bottom = DialogRect.bottom - CLUE_TEXT_BOTTOM_GAP_HEIGHT;
    ClueTextRect.left   = CLUE_TEXT_LEFT_GAP_WIDTH;
    ClueTextRect.right  = ClueTextRect.left + CLUE_TEXT_WIDTH;
    m_ClueText.MoveWindow(&ClueTextRect, TRUE);

    m_ClueText.SetWindowText(IDLE_CLUE_TEXT);

	nResult = true;
// Exit0:
	return nResult;
}

int KG_JX3LauncherDlg::_LoadItemExe(const TCHAR cszExePath[])
{
    int nResult  = false;
    int nRetCode = false;
    tstring strExeFileName;
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcessInfo = {0};

    ASSERT(cszExePath);
    ASSERT(cszExePath[0]);

    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);

    strExeFileName = cszExePath;
    // Append '\\' if need.
    nRetCode = (int)strExeFileName.length();
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    if (strExeFileName[nRetCode - 1] != _T('/') && strExeFileName[nRetCode - 1] != _T('\\'))
        strExeFileName += _T('\\');

    nRetCode = _tcsicmp(AfxGetApp()->m_lpCmdLine, _T("-c"));
    if (nRetCode == 0)
        strExeFileName += GAME_CONFIG_FILE_NAME;
    else
        strExeFileName += GAME_UPDATER_FILE_NAME;

    nRetCode = CreateProcess(
        NULL, (LPTSTR)(strExeFileName.c_str()), NULL, NULL, false, 0, NULL, NULL,
        &StartupInfo, &ProcessInfo
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (ProcessInfo.hThread)
    {
        CloseHandle(ProcessInfo.hThread);
        ProcessInfo.hThread = NULL;
    }
    if (ProcessInfo.hProcess)
    {
        CloseHandle(ProcessInfo.hProcess);
        ProcessInfo.hProcess = NULL;
    }    
    return nResult;
}
