#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneObjectEditorPanelView.h"
#include "KObjectEditorDoc.h"
#include "KGInIFileEditDlg.h"
#include "KGMemDateEditDlg.h"
#include "Resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(KSceneObjectEditorPanelView, CFormView)

KSceneObjectEditorPanelView::KSceneObjectEditorPanelView()
: CFormView(KSceneObjectEditorPanelView::IDD)
{
	m_pObjectDlg = NULL;
	m_pNpcDlg    = NULL;
	m_pPropDlg   = NULL;
	m_pMagicDlg  = NULL;
	m_pEffectDlg = NULL;
	m_pSkillDlg  = NULL;
	m_pAniDlg    = NULL;
	m_pEquipDlg  = NULL;
	m_pPlayerAinDlg = NULL;
	m_pTaseDlg = NULL;

	//m_pPropertySheet = FALSE;
}

KSceneObjectEditorPanelView::~KSceneObjectEditorPanelView()
{
	//SAFE_DELETE(m_pPropertySheet);
}

void KSceneObjectEditorPanelView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(KSceneObjectEditorPanelView, CFormView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_NPC, OnBnClickedNpc)
	ON_BN_CLICKED(IDC_OBJECT, OnBnObjectClick)
	ON_BN_CLICKED(IDC_PROP, OnBnClickedProp)
	ON_BN_CLICKED(IDC_MAGIC, OnBnClickedMagic)
	ON_BN_CLICKED(IDC_EFFECT, OnBnClickedEffect)
	ON_BN_CLICKED(IDC_SKILL, OnBnClickedSkill)
	ON_BN_CLICKED(IDC_EQUIP, OnBnClickedEquip)
	ON_BN_CLICKED(IDC_PLAYER_ANIMATIOM, OnBnClickedPlayeranimatiom)
	ON_BN_CLICKED(IDC_NPCANIMATIOM, OnBnClickedNpcanimatiom)

	ON_BN_CLICKED(IDC_PLAYER_TASE, OnBnClickedTase)
END_MESSAGE_MAP()

void KSceneObjectEditorPanelView::OnBnObjectClick()
{

	if (!m_pObjectDlg)
	{
		m_pObjectDlg = new KG3DObjectEditor(this);
		if (m_pObjectDlg)
		{
			m_pObjectDlg->Create(IDD_OBJECT_EDITER, GetDesktopWindow());
			m_pObjectDlg->ShowWindow(SW_SHOW);
			m_pObjectDlg->ShowWindow(SW_SHOWMAXIMIZED);
		}
	}
}

#ifdef _DEBUG
void KSceneObjectEditorPanelView::AssertValid() const
{
	CFormView::AssertValid();
}

void KSceneObjectEditorPanelView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

int KSceneObjectEditorPanelView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void KSceneObjectEditorPanelView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	CWnd* pwndPropSheetHolder = GetDlgItem(IDC_OBJECTEDITOR_TAB);
	/*
	m_pPropertySheet = new KSceneObjectEditorPropertySheet(pwndPropSheetHolder);
	if (!m_pPropertySheet->Create(pwndPropSheetHolder,
		WS_CHILD | WS_VISIBLE, 0))
	{
		delete m_pPropertySheet;
		m_pPropertySheet = NULL;
		return;
	}

	CRect rectPropSheet;
	pwndPropSheetHolder->GetWindowRect(rectPropSheet);
	m_pPropertySheet->SetWindowPos(NULL,0, 0,
		rectPropSheet.Width(), rectPropSheet.Height(),
		SWP_NOZORDER | SWP_NOACTIVATE);

	CTabCtrl* pTab = m_pPropertySheet->GetTabControl();


	TC_ITEM	ti;
	ti.mask = TCIF_TEXT;

	ti.pszText = _T(" 模 型 ");
	pTab->SetItem(0, &ti);

	ti.pszText = _T(" 障 碍 ");
	pTab->SetItem(1, &ti);

	pTab->SetCurSel(0);
	pTab->SetCurSel(1);
	pTab->SetCurSel(0);

    ti.pszText = _T(" 物件管理器 ");
    pTab->SetItem(2, &ti);

    pTab->SetCurSel(0);


    KSceneObjectEditorDoc* pDc = (KSceneObjectEditorDoc*) GetDocument();
	m_pPropertySheet->SetSceneObjectEditorDoc(pDc);
	*/
}

void KSceneObjectEditorPanelView::OnBnClickedNpc()
{
	//if (!m_pNpcDlg)
	//{
	//	HWND hwndDlg = ::CreateDialog(
	//		AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_OBJECT_PROPERTY), NULL, NULL
	//	);
	//	m_pNpcDlg = new KGObjectPropertyEditDlg(PROPERTY_EDIT_TYPE_NPC, this);
	//	m_pNpcDlg->SubclassWindow(hwndDlg);
	//	KSceneObjectEditorDoc* pDc = (KSceneObjectEditorDoc*)GetDocument();
	//	m_pNpcDlg->SetSceneObjectEditorDoc(pDc);
	//	m_pNpcDlg->Init();
	//	m_pNpcDlg->ShowWindow(SW_SHOW);
	//}
   /* vector<CString> vecString;
    CString strTextP = "C:\\TestP.ini";
    CString strTextT = "C:\\TestT.ini";
    vecString.push_back(strTextP);
    KGInIFileEditDlg dlg(vecString, strTextT);
    dlg.DoModal();*/

    //////////////////////////////////////////////////////////////////////////
    // Sample : 编辑内存中的数据结构

    // 测试结构
    //struct TestStruct
    //{
    //    char szPath[MAX_PATH];
    //    char szString[128];

    //    int  nDate1;
    //    int  nDate2;

    //    float fDate1;
    //    float fDate2;

    //    DWORD dwColor;
    //    DWORD dwDate;

    //    bool  bDate1;
    //    bool  bDate2;

    //    TestStruct()
    //    {
    //        ZeroMemory(this, sizeof(TestStruct));
    //    }
    //};

    //// 定义一个和结构相应的元素声明
    //// 编移量为 ELEMENT_AUTO 时，为自己计算编移量，前提是
    //// 元素声明的顺序必须和结构的成员顺序一致
    //KGMEMDATEELEMENT element[] =
    //{
    //    // 偏移量       数据大小        数据类型        列表中显示名字

    //    ELEMENT_TITLE("字符数据"),      // 添加标题
    //    {ELEMENT_AUTO,  MAX_PATH,       MEM_TYPE_PATH,    _T("文件名")},
    //    {ELEMENT_AUTO,  128,            MEM_TYPE_STRING,  _T("字符串")},

    //    ELEMENT_TITLE("整形数据"),
    //    {ELEMENT_AUTO, 4,               MEM_TYPE_INT,     _T("数据1")},
    //    {ELEMENT_AUTO, 4,               MEM_TYPE_INT,     _T("数据2")},

    //    ELEMENT_TITLE("浮点数据"),
    //    {ELEMENT_AUTO, 4,               MEM_TYPE_FLOAT,   _T("数据1")},
    //    {ELEMENT_AUTO, 4,               MEM_TYPE_FLOAT,   _T("数据2")},

    //    ELEMENT_TITLE("DWORD数据"),
    //    {ELEMENT_AUTO, 4,               MEM_TYPE_DWORD,   _T("数据1")},
    //    {ELEMENT_AUTO, 4,               MEM_TYPE_COLOR,   _T("数据2")},

    //    ELEMENT_TITLE("BOOL数据"),
    //    {ELEMENT_AUTO, 1,               MEM_TYPE_BOOL,    _T("数据1")},
    //    {ELEMENT_AUTO, 1,               MEM_TYPE_BOOL,    _T("数据2")},

    //    ELEMENT_END()
    //};

    //static TestStruct test;
    //KGMemDateEditDlg dlg(reinterpret_cast<BYTE*>(&test), element);
    //dlg.DoModal();


	//////////////////////////////////////////////////////////////////////////
	// test the ini operation

	//m_pIniFile = g_OpenIniFile("D:\\test.ini", false, true);
	//if (m_pIniFile)
	//{
	//	// do read operation
	//	for (int r = 0; r < 1000; r++)
	//	{
	//		char szIndex[8];
	//		int  nDate = 0;
	//		itoa(r, szIndex, 10);
	//		m_pIniFile->GetInteger("main", szIndex, nDate, &nDate);
	//	}

	//	// do write operation
	//	for (int w = 0; w < 1000; w++)
	//	{
	//		char szDate[8];
	//		itoa(w, szDate, 10);
	//		m_pIniFile->WriteInteger("main", szDate, w);
	//	}

	//	m_pIniFile->Save("D:\\test.ini");
	//}

	//////////////////////////////////////////////////////////////////////////
	// test tab file operation

	ITabFile* pTabFile = g_OpenTabFile("D:\\test.tab", false, true);
	SAFE_RELEASE(pTabFile);
}

void KSceneObjectEditorPanelView::OnBnClickedProp()
{
	if (!m_pPropDlg)
	{
		HWND hwndDlg = ::CreateDialog(
			AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_OBJECT_PROPERTY), NULL, NULL
		);
		m_pPropDlg = new KGObjectPropertyEditDlg(PROPERTY_EDIT_TYPE_PROP, this);
		m_pPropDlg->SubclassWindow(hwndDlg);
		KSceneObjectEditorDoc* pDc = (KSceneObjectEditorDoc*)GetDocument();
		m_pPropDlg->SetSceneObjectEditorDoc(pDc);
		m_pPropDlg->Init();
		m_pPropDlg->ShowWindow(SW_SHOW);
	}
}

void KSceneObjectEditorPanelView::OnBnClickedMagic()
{
	if (!m_pMagicDlg)
	{
		HWND hwndDlg = ::CreateDialog(
			AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_OBJECT_PROPERTY), NULL, NULL
		);
		m_pMagicDlg = new KGObjectPropertyEditDlg(PROPERTY_EDIT_TYPE_MAGIC, this);
		m_pMagicDlg->SubclassWindow(hwndDlg);
		KSceneObjectEditorDoc* pDc = (KSceneObjectEditorDoc*)GetDocument();
		m_pMagicDlg->SetSceneObjectEditorDoc(pDc);
		m_pMagicDlg->Init();
		m_pMagicDlg->ShowWindow(SW_SHOW);
	}
}

void KSceneObjectEditorPanelView::OnBnClickedEffect()
{
	if (!m_pEffectDlg)
	{
		HWND hwndDlg = ::CreateDialog(
			AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_OBJECT_PROPERTY), NULL, NULL
		);
		m_pEffectDlg = new KGObjectPropertyEditDlg(PROPERTY_EDIT_TYPE_EFFECT, this);
		m_pEffectDlg->SubclassWindow(hwndDlg);
		KSceneObjectEditorDoc* pDc = (KSceneObjectEditorDoc*)GetDocument();
		m_pEffectDlg->SetSceneObjectEditorDoc(pDc);
		m_pEffectDlg->Init();
		m_pEffectDlg->ShowWindow(SW_SHOW);
	}
}

void KSceneObjectEditorPanelView::OnBnClickedSkill()
{
	if (!m_pSkillDlg)
	{
		HWND hwndDlg = ::CreateDialog(
			AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_OBJECT_PROPERTY), NULL, NULL
		);
		m_pSkillDlg = new KGObjectPropertyEditDlg(PROPERTY_EDIT_TYPE_SKILL, this);
		m_pSkillDlg->SubclassWindow(hwndDlg);
		KSceneObjectEditorDoc* pDc = (KSceneObjectEditorDoc*)GetDocument();
		m_pSkillDlg->SetSceneObjectEditorDoc(pDc);
		m_pSkillDlg->Init();
		m_pSkillDlg->ShowWindow(SW_SHOW);
	}
}

void KSceneObjectEditorPanelView::OnBnClickedPlayeranimatiom()
{
	if (!m_pPlayerAinDlg)
	{
		//COLOR_PROJECT color[2] =
		//{
		//	{RGB(223, 223, 223), RGB(122, 122, 122), RGB(68, 68, 68)},
		//	{RGB(242, 242, 242), RGB(122, 122, 122), RGB(68, 68, 68)},
		//};
		HWND hwndDlg = ::CreateDialog(
			AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_OBJECT_PROPERTY), NULL, NULL
		);
		m_pPlayerAinDlg =
			new KGObjectPropertyEditDlg(PROPERTY_EDIT_TYPE_PLAYER_ANI, this);
		m_pPlayerAinDlg->SubclassWindow(hwndDlg);
		KSceneObjectEditorDoc* pDc = (KSceneObjectEditorDoc*)GetDocument();
		m_pPlayerAinDlg->SetSceneObjectEditorDoc(pDc);
		m_pPlayerAinDlg->Init();
		//m_pPlayerAinDlg->m_listProperty.BuildColorProject(
		//	color, RGB( 96,  96, 160), RGB(255, 255, 255),
		//	RGB( 96,  96, 160), RGB(255, 255, 255),
		//	RGB(122, 178, 255), 2
		//);
		m_pPlayerAinDlg->ShowWindow(SW_SHOW);
	}
}

void KSceneObjectEditorPanelView::OnBnClickedEquip()
{
	if (!m_pEquipDlg)
	{
		HWND hwndDlg = ::CreateDialog(
			AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_OBJECT_PROPERTY), NULL, NULL
		);
		m_pEquipDlg = new KGObjectPropertyEditDlg(PROPERTY_EDIT_TYPE_EQUIP, this);
		m_pEquipDlg->SubclassWindow(hwndDlg);
		KSceneObjectEditorDoc* pDc = (KSceneObjectEditorDoc*)GetDocument();
		m_pEquipDlg->SetSceneObjectEditorDoc(pDc);
		m_pEquipDlg->Init();
		m_pEquipDlg->ShowWindow(SW_SHOW);
	}
}

void KSceneObjectEditorPanelView::OnBnClickedTase()
{
	if (!m_pTaseDlg)
	{
		m_pTaseDlg = new KGObjectPropertyEditTabDlg(PROPERTY_EDIT_TYPE_TASE, this);
		m_pTaseDlg->Create(IDD_OBJECT_PROPERTY, GetDesktopWindow());
		KSceneObjectEditorDoc* pDc = (KSceneObjectEditorDoc*)GetDocument();
		m_pTaseDlg->SetSceneObjectEditorDoc(pDc);
		m_pTaseDlg->ShowWindow(SW_SHOW);
	}
	//ITabFile* pTabFile = g_OpenTabFile(
	//	"C:\\JXIII\\blackhole\\tools\\sceneeditor\\debug\\settingsA\\QuestEdit\\aaaaaaaaaaaaaaaaaaaaaaaaaaaaa.tab",
	//	false, true
	//);
	//ITabFile* pTabFile = g_CreateTabFile();
	/*pTabFile->Release();*/
	/*delete pTabFile;*/
}

void KSceneObjectEditorPanelView::OnBnClickedNpcanimatiom()
{
	if (!m_pAniDlg)
	{
		HWND hwndDlg = ::CreateDialog(
			AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_OBJECT_PROPERTY), NULL, NULL
		);
		m_pAniDlg = new KGObjectPropertyEditDlg(PROPERTY_EDIT_TYPE_NPC_ANI, this);
		m_pAniDlg->SubclassWindow(hwndDlg);
		KSceneObjectEditorDoc* pDc = (KSceneObjectEditorDoc*)GetDocument();
		m_pAniDlg->SetSceneObjectEditorDoc(pDc);
		m_pAniDlg->Init();
		m_pAniDlg->ShowWindow(SW_SHOW);
	}
}
