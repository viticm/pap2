// KDlgResourceList.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgResourceList.h"

static int s_nTopSpan = 10;
static int s_nBottomSpan = 40;
static int s_nSideSpan = 10;

static const TCHAR *s_TagTypes[] = 
{
	"SFX",    //TAGTYPE_SFX,
	"Sound",  //TAGTYPE_SOUND,
	"Motion", //TAGTYPE_MOTION,
	"TANI",  //TANI
};

// KDlgResourceList dialog

IMPLEMENT_DYNAMIC(KDlgResourceList, CDialog)

KDlgResourceList::KDlgResourceList(CWnd* pParent /*=NULL*/)
	: CDialog(KDlgResourceList::IDD, pParent)
{
	Create(IDD, AfxGetMainWnd());
	ShowWindow(SW_SHOW);
}

KDlgResourceList::~KDlgResourceList()
{
}

void KDlgResourceList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TAGLIST, m_treeResource);
	DDX_Control(pDX, IDC_BUTTON_LOG, m_buttonGenerateLog);
}


BEGIN_MESSAGE_MAP(KDlgResourceList, CDialog)
	ON_WM_SIZING()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_TAGLIST, &KDlgResourceList::OnNMDblclkTreeTaglist)
	ON_BN_CLICKED(IDC_BUTTON_LOG, &KDlgResourceList::OnBnClickedButtonLog)
END_MESSAGE_MAP()


// KDlgResourceList message handlers
void KDlgResourceList::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);
	RECT rectClient;
	RECT rectButton;
	GetClientRect(&rectClient);
	rectClient.left += s_nSideSpan;
	rectClient.right -= s_nSideSpan;
	rectClient.top += s_nTopSpan;
	rectClient.bottom -= s_nBottomSpan;
	m_treeResource.MoveWindow(&rectClient, TRUE);

	m_buttonGenerateLog.GetClientRect(&rectButton);
	rectClient.top = rectClient.bottom + 5;
	rectClient.bottom = rectClient.top + rectButton.bottom;
	rectClient.right = rectClient.left + rectButton.right;
	m_buttonGenerateLog.MoveWindow(&rectClient, TRUE);
}

void KDlgResourceList::OnNMDblclkTreeTaglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = m_treeResource.GetSelectedItem();
	DWORD dwIndex = static_cast<DWORD>(m_treeResource.GetItemData(hItem));
	HGLOBAL hData = NULL;
	void *pDest = NULL;
	BOOL bClose = FALSE;
	
	KG_PROCESS_ERROR(::OpenClipboard(this->GetSafeHwnd()));

	bClose = TRUE;
	::EmptyClipboard();
	size_t uSize = m_vecFileName[dwIndex].size() + 1; 
	hData = ::GlobalAlloc(GMEM_MOVEABLE, uSize * sizeof(TCHAR));	//应该是不用释放的!?
	KG_PROCESS_ERROR(hData);

	pDest = (LPWSTR)::GlobalLock(hData);
	KG_PROCESS_ERROR(pDest);

	memcpy(pDest, m_vecFileName[dwIndex].c_str(), sizeof(TCHAR) * uSize);

	::GlobalUnlock(hData);
	::SetClipboardData(CF_TEXT, hData);
Exit0:
	if (bClose)
	{
		::CloseClipboard();
	}
	*pResult = 0;
}

HTREEITEM KDlgResourceList::AddBrach(LPCSTR strBrach)
{
	HTREEITEM hReturn = m_treeResource.InsertItem(strBrach, 
		0, 
		1);
	KG_PROCESS_ERROR(hReturn);
	m_vecFileName.push_back(strBrach);
	m_treeResource.SetItemData(hReturn, static_cast<DWORD>(m_vecFileName.size() - 1));
Exit0:
	return hReturn;
}

void KDlgResourceList::AddLeaf(HTREEITEM hRoot, 
							   LPCSTR strName, 
							   DWORD dwFrame, 
							   DWORD dwIndex, 
							   DWORD dwType)
{
	TCHAR strLeafText[MAX_PATH << 1];
	sprintf_s(strLeafText,
		MAX_PATH << 1,
		"Frame %d, Index %d, Type %s, File %s",
		dwFrame, 
		dwIndex,
		s_TagTypes[dwType],
		strName);
	HTREEITEM hLeaf = m_treeResource.InsertItem(strLeafText,
		0,
		1,
		hRoot);
	m_vecFileName.push_back(strName);
	m_treeResource.SetItemData(hLeaf, static_cast<DWORD>(m_vecFileName.size() - 1));
}

void KDlgResourceList::OnBnClickedButtonLog()
{
	HTREEITEM hRoot = m_treeResource.GetRootItem();
	FILE* fpFile = fopen("tagandsource.log", "w");
	KG_PROCESS_ERROR(fpFile);
	
	while (hRoot)
	{
		fprintf(fpFile, "%s\n", m_treeResource.GetItemText(hRoot));
		HTREEITEM hChild = m_treeResource.GetChildItem(hRoot);
		while (hChild)
		{
			fprintf(fpFile, "    %s\n", m_treeResource.GetItemText(hChild));
			hChild = m_treeResource.GetNextSiblingItem(hChild);
		}
		hRoot = m_treeResource.GetNextSiblingItem(hRoot);
	}
Exit0:
	if (fpFile)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
}


void KDlgResourceList::Reset()
{
	m_treeResource.DeleteAllItems();
	m_vecFileName.clear();
}