// KDlgFilterFileView.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgFilterFileView.h"


// KDlgFilterFileView dialog

IMPLEMENT_DYNAMIC(KDlgFilterFileView, CDialog)

KDlgFilterFileView::KDlgFilterFileView(CWnd* pParent /*=NULL*/)
	: CDialog(KDlgFilterFileView::IDD, pParent)
	, m_strInitDir(_T(""))
	, m_strFilter(_T(""))
	, m_pfCallBack(NULL)
	, m_strFullPath(_T(""))
{

}

KDlgFilterFileView::~KDlgFilterFileView()
{
}

void KDlgFilterFileView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES, m_lstFiles);
	DDX_Text(pDX, IDC_EDIT_INITDIR, m_strInitDir);
	DDX_Text(pDX, IDC_EDIT_FILTER, m_strFilter);
	DDX_Text(pDX, IDC_EDIT_FILEPATH, m_strFullPath);
}


BEGIN_MESSAGE_MAP(KDlgFilterFileView, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KDlgFilterFileView::OnBnClickedButtonRefresh)
	ON_LBN_DBLCLK(IDC_LIST_FILES, &KDlgFilterFileView::OnLbnDblclkListFiles)
	ON_LBN_SELCHANGE(IDC_LIST_FILES, &KDlgFilterFileView::OnLbnSelchangeListFiles)
END_MESSAGE_MAP()


// KDlgFilterFileView message handlers
void KDlgFilterFileView::OnOK()
{
	FilterStringToKeys();
	FillListByFilter();
}

void KDlgFilterFileView::FillListByFilter()
{
	KG_PROCESS_SUCCESS(!m_strFilesInDir.size());
	
	m_lstFiles.ResetContent();
	
	for (size_t i = 0; i < m_strFileNames.size(); i++)
	{
		BOOL bChecked = TRUE;
		const TCHAR* strStart = m_strFileNames[i];
		for (size_t j = 0; j < m_Keywords.size(); j++)
		{
			const TCHAR *pFind = strstr(strStart, m_Keywords[j].c_str());
			if (!pFind)
			{
				bChecked = FALSE;
				break;
			}
			strStart = pFind + m_Keywords[j].size();
		}
		if (bChecked)
		{
			int nIndex = m_lstFiles.AddString(m_strFileNames[i]);
			if (nIndex == LB_ERR || nIndex == LB_ERRSPACE)
				continue;
			m_lstFiles.SetItemData(nIndex, i);
		}
	}
Exit1:
	;
}
void KDlgFilterFileView::OnBnClickedButtonRefresh()
{
	UpdateData();
	OnRefreshInitDir();
	OnOK();
}

void KDlgFilterFileView::OnRefreshInitDir()
{
	CFileFind Find;
	m_strFilesInDir.clear();
	m_strFileNames.clear();
	m_lstFiles.ResetContent();
	BOOL bFind = Find.FindFile(m_strInitDir);
	while (bFind)
	{
		bFind = Find.FindNextFile();
		m_strFilesInDir.push_back(Find.GetFilePath().MakeLower());
		TCHAR strFile[MAX_PATH];
		TCHAR strExt[MAX_PATH];
		TCHAR strFileName[MAX_PATH];
		_splitpath_s(m_strFilesInDir[m_strFilesInDir.size() - 1],
			NULL,
			0,
			NULL,
			0,
			strFile,
			MAX_PATH,
			strExt,
			MAX_PATH);
		sprintf_s(strFileName, 
			MAX_PATH,
			"%s%s",
			strFile,
			strExt);
		m_strFileNames.push_back(strFileName);
	}
}

void KDlgFilterFileView::OnLbnDblclkListFiles()
{
	int nCurSel = m_lstFiles.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);

	size_t uIndex = static_cast<size_t>(m_lstFiles.GetItemData(nCurSel));
	KG_PROCESS_ERROR(uIndex < m_strFilesInDir.size());

	if (m_pfCallBack)
	{
		m_pfCallBack(m_strFilesInDir[uIndex], (void*)GetParent());
	}
Exit0:
	;
}

void KDlgFilterFileView::OnLbnSelchangeListFiles()
{
	int nCurSel = m_lstFiles.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);

	size_t uIndex = static_cast<size_t>(m_lstFiles.GetItemData(nCurSel));
	KG_PROCESS_ERROR(uIndex < m_strFilesInDir.size());

	m_strFullPath = m_strFilesInDir[uIndex];

	UpdateData(FALSE);
Exit0:
	;
}

void KDlgFilterFileView::FilterStringToKeys()
{
	UpdateData();
	TCHAR strLower[MAX_PATH];
	strcpy_s(strLower, MAX_PATH, m_strFilter.MakeLower());
	m_Keywords.clear();
	TCHAR *pEnd = strLower + strlen(strLower);
	TCHAR *pFind = strchr(strLower, '*');
	TCHAR *pStart = strLower;
	while (pFind && pStart < pEnd)
	{
		*pFind = '\0';
		if (pStart[0] != '\0')
		{
			m_Keywords.push_back(pStart);
		}
		if (pFind + 1 < pEnd)
		{
			pStart = pFind + 1;
		}
		pFind = strchr(pStart, '*');
	}
	if (pStart[0] != '\0')
	{
		m_Keywords.push_back(pStart);
	}
}