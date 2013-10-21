#include "KGMapResSearchDlg.h"

KGMapResSearch KGMapResSearchDlg::m_MapResSearch;
HINSTANCE KGMapResSearchDlg::m_hInstance;

KGMapResSearchDlg::KGMapResSearchDlg(void)
{
}

KGMapResSearchDlg::~KGMapResSearchDlg(void)
{
}

int KGMapResSearchDlg::Init(HINSTANCE hInstance, IKG3DEngineManager* pEngineMgr, const TCHAR cszClientPath[])
{

	int nResult  = false;
	int nRetCode = false;
	INT_PTR pnDlg = 0;

	KG_ASSERT_EXIT(hInstance);
	KG_ASSERT_EXIT(pEngineMgr);

	m_hInstance = hInstance;

	nRetCode = m_MapResSearch.Init(m_hInstance, pEngineMgr, cszClientPath);
	KGLOG_PROCESS_ERROR(nRetCode);
	
	//其它的初始化必须在此之前，因为创建对话框后会跑自己的消息循环
	pnDlg = DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)DialogProc);
	KGLOG_PROCESS_ERROR(pnDlg != -1);

	nResult = true;
Exit0:
	return nResult;

}

LRESULT CALLBACK KGMapResSearchDlg::DialogProc(HWND hDlg,
											   UINT uMsg,
											   WPARAM wParam,
											   LPARAM lParam)
{
	int nRetCode = false;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		nRetCode = m_MapResSearch.InitUI(hDlg);
		KGLOG_PROCESS_ERROR(nRetCode);
		break;
	case WM_NOTIFY:
		switch(LOWORD(wParam))
		{
		case IDC_LISTVIEW:
			nRetCode = m_MapResSearch.CatchListViewMessage(wParam, lParam);
			KGLOG_PROCESS_ERROR(nRetCode);
			nRetCode = m_MapResSearch.ListViewUpdate(hDlg, lParam);
			KGLOG_PROCESS_ERROR(nRetCode);
			break;
		}
		break;
	case WM_KEYDOWN:
		if (LOWORD(wParam) == VK_RETURN)
		{
			nRetCode = m_MapResSearch.Search();
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_RADIO_MULTI:
			nRetCode = m_MapResSearch.EnableMultiSearch();
			KGLOG_PROCESS_ERROR(nRetCode);
			break;
		case IDC_RADIO_SINGLE:
			nRetCode = m_MapResSearch.EnableSingleSearch();
			KGLOG_PROCESS_ERROR(nRetCode);
			break;
		case IDC_BUTTON_SEARCH:
			nRetCode = m_MapResSearch.Search();
			KGLOG_PROCESS_ERROR(nRetCode);
			break;
		case IDC_BUTTON_OPENMAP:
			nRetCode = m_MapResSearch.OpenMap();
			KGLOG_PROCESS_ERROR(nRetCode);
			break;
		case ID_POPMENU_COPYPOS:
		case ID_POPMENU_COPYMAP:
			m_MapResSearch.CopyResult(wParam);
			break;
		};
		break;
	case WM_CLOSE:
		m_MapResSearch.UnInit();
		nRetCode = EndDialog(hDlg, 0);
		KGLOG_PROCESS_ERROR(nRetCode);
		break;
	};

Exit0:
	return 0;
}