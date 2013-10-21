////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertyTreeCtrl.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-2-13 17:24:34
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGPropertyTreeCtrl.h"
#include "KGPropertyEditTabDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


KGPropertyTreeCtrl::KGPropertyTreeCtrl()
{
	m_ppQuestIniFile       = NULL;
	m_ppTabFile		       = NULL;
	m_ppszClassifyFields   = NULL;
	m_nClassifyFieldsSize  = 0;
	m_pszCaptionField	   = NULL;
	m_pszPrimaryKesField   = NULL;
}

KGPropertyTreeCtrl::~KGPropertyTreeCtrl()
{
}

int KGPropertyTreeCtrl::BandData(
	ITabFile** ppTabFile, IIniFile** ppQuestIniFile, IIniFile** ppIniFile,
	KGPrimaryKeyManager* pKeyManager, int pnTypeRow[], int nTypeRowSize, int nTitleRow, int nDataRow,
	CImageList* pImageList, Funtion GetImageIndex
)
{
	m_pPrimaryManager = pKeyManager;
	m_ppQuestIniFile  = ppQuestIniFile;
	m_ppTabFile		  = ppTabFile;
	m_ppIniFile       = ppIniFile;

	if (pImageList)
		SetImageList(pImageList, TVSIL_NORMAL);
	m_fnGetImageIndex = GetImageIndex;

	return true;
}

int KGPropertyTreeCtrl::BandData(
	ITabFile** ppTabFile, IIniFile** ppQuestIniFile, IIniFile** ppIniFile,
	KGPrimaryKeyManager* nKeyManager, char** ppszClassifyFields,
	int nClassifyFieldsSize, char* szCaptionField, char* szPrimaryKeyField,
	CImageList* pImageList, Funtion GetImageIndex
)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(nKeyManager);
	KG_PROCESS_ERROR(ppQuestIniFile);
	KG_PROCESS_ERROR(ppTabFile);
	KG_PROCESS_ERROR(ppIniFile);

	KG_PROCESS_ERROR(ppszClassifyFields);
	KG_PROCESS_ERROR(nClassifyFieldsSize >= 1);
	KG_PROCESS_ERROR(szCaptionField);
	KG_PROCESS_ERROR(szPrimaryKeyField);

	m_pPrimaryManager	  = nKeyManager;
	m_ppQuestIniFile	  = ppQuestIniFile;
	m_ppTabFile           = ppTabFile;
	m_ppIniFile		      = ppIniFile;

	m_ppszClassifyFields  = ppszClassifyFields;
	m_nClassifyFieldsSize = nClassifyFieldsSize;
	m_pszCaptionField     = szCaptionField;
	m_pszPrimaryKesField  = szPrimaryKeyField;

	if (pImageList)
		SetImageList(pImageList, TVSIL_NORMAL);
	m_fnGetImageIndex = GetImageIndex;

	nResult = true;
Exit0:
	return nResult;
}

HTREEITEM KGPropertyTreeCtrl::GetItemByPrimaryKey(
	HTREEITEM hStartItem, DWORD_PTR nPrimaryKey
)
{
	HTREEITEM hResultItem  = NULL;
	DWORD_PTR nTreeItemDate = 0;

	KG_PROCESS_ERROR(hStartItem);

	nTreeItemDate = GetItemData(hStartItem);
	if (nTreeItemDate == nPrimaryKey)
	{
		hResultItem = hStartItem;
	}
	KG_PROCESS_SUCCESS(hResultItem);

	hResultItem = GetItemByPrimaryKey(GetChildItem(hStartItem), nPrimaryKey);
	KG_PROCESS_SUCCESS(hResultItem);

	hResultItem = GetItemByPrimaryKey(
		GetNextSiblingItem(hStartItem), nPrimaryKey
	);
	KG_PROCESS_SUCCESS(hResultItem);

Exit1:
Exit0:
	return hResultItem;
}


int KGPropertyTreeCtrl::CopyOneRow(int nSourRow, int nDestRow)
{
	int nResult  = false;
	int nRetCode = false;

	int nColsNumber = 0;
	int nRowsNumber = 0;
	int i = 0;
	int nData = 0;

	char szValue[1024];
	*szValue = '\0';

	KG_PROCESS_ERROR(nSourRow >= 1);
	KG_PROCESS_ERROR(nDestRow >= 1);
	KG_PROCESS_ERROR(m_ppTabFile);
	KG_PROCESS_ERROR(*m_ppTabFile);

	nRowsNumber = (*m_ppTabFile)->GetHeight();
	nColsNumber = (*m_ppTabFile)->GetWidth();

	KG_PROCESS_ERROR(nSourRow <= nRowsNumber);
	KG_PROCESS_ERROR(nDestRow <= nRowsNumber);

	for (i = 1; i <= nColsNumber; i++)
	{
		nRetCode = (*m_ppTabFile)->GetString(
			nSourRow, i, "", szValue, sizeof(szValue)
		);
		nRetCode = (*m_ppTabFile)->WriteString(nDestRow, i, szValue);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::RefrushTypeInfo(HTREEITEM hItem)
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hTempItem = hItem;
	int  nRow = 0;
	int  i	  = 0;
	int  nItemData = 0;
	char szSectionName[256];
	*szSectionName = '\0';
	int  nKeyValue = 0;

	CString* pszTitleText = new CString[m_nClassifyFieldsSize];
	for (i = 0; i < m_nClassifyFieldsSize; i++)
	{
		pszTitleText[i] = "";
	}

	CString szItemTitle = "";
	CString szItemData  = "";
	CString szTempValue = "";

	KG_PROCESS_ERROR(hItem);

	szItemTitle = GetItemText(hItem);

	nRow = GetItemPos(hItem);
	KG_PROCESS_ERROR(nRow >= 1);

	hTempItem = GetParentItem(hTempItem);
	KG_PROCESS_ERROR(hTempItem);
	nItemData = (int)GetItemData(hTempItem);
	if (nItemData == SUBSET_TREE_DATA)
	{
		while ((*m_ppQuestIniFile)->GetNextSection(szSectionName, szSectionName))
		{
			(*m_ppQuestIniFile)->GetInteger(
				szSectionName, _T("TreeHandle"), 0, &nKeyValue
			);
			if (nKeyValue == (INT_PTR)hTempItem)
			{
				break;
			}
		}
		if (nKeyValue == (INT_PTR)hTempItem)
		{
			char szValue[256];
			*szValue = '\0';
			nItemData = (int)GetItemData(hItem);
			szItemData.Format("%d", nItemData);
			nRetCode  = FindInSubset(*m_ppQuestIniFile, szItemData, szTempValue);
			if (szTempValue == "")
			{
				CString szTempSection =
					CString(szSectionName) + CString(_T("-")) + szItemData;
				(*m_ppQuestIniFile)->GetString(
					szSectionName, _T("Name"), "", szValue, sizeof(szValue)
				);
				(*m_ppQuestIniFile)->WriteString(
					szTempSection.GetBuffer(), _T("Name"), szValue
				);
				(*m_ppQuestIniFile)->GetString(
					szSectionName, _T("TreeHandle"), "", szValue, sizeof(szValue)
				);
				(*m_ppQuestIniFile)->WriteString(
					szTempSection.GetBuffer(), _T("TreeHandle"), szValue
				);
				(*m_ppQuestIniFile)->EraseSection(szSectionName);
			}
		}
		else
		{
			CString szTempSection;
			nItemData = (int)GetItemData(hItem);
			szTempSection.Format("%d", nItemData);
			(*m_ppQuestIniFile)->WriteString(
				szTempSection.GetBuffer(), _T("Name"),
				GetItemText(hTempItem).GetBuffer()
			);
			(*m_ppQuestIniFile)->WriteInteger(
				szTempSection.GetBuffer(), _T("TreeHandle"), (int)(INT_PTR)(hTempItem)
			);
		}

		hTempItem = GetParentItem(hTempItem);
	}

	i = m_nClassifyFieldsSize - 1;
	while (hTempItem && i >= 0)
	{
		pszTitleText[i--] = GetItemText(hTempItem);
		hTempItem = GetParentItem(hTempItem);
	}

	for (i = 0; i < m_nClassifyFieldsSize; i++)
	{
		nRetCode  = (*m_ppTabFile)->WriteString(
			nRow, m_ppszClassifyFields[i], pszTitleText[i].GetBuffer()
		);
		KG_PROCESS_ERROR(nRetCode);
	}

	nRetCode = (*m_ppTabFile)->WriteString(
		nRow, m_pszCaptionField, szItemTitle.GetBuffer()
	);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	for (i = 0; i < m_nClassifyFieldsSize; i++)
	{
		pszTitleText[i].ReleaseBuffer();
	}
	pszTitleText->ReleaseBuffer();
	return nResult;
}

int KGPropertyTreeCtrl::UpdateInfoRelating(HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hParentItem = NULL;
	HTREEITEM hPrevItem   = NULL;
	HTREEITEM hChildItem  = NULL;
	HTREEITEM hTempItem   = NULL;

	int nItemData  = 0;
	int nItemPos   = 0;

	char szTempValue[1024];
	*szTempValue = '\0';

	KG_PROCESS_ERROR(m_ppTabFile);
	KG_PROCESS_ERROR(*m_ppTabFile);
	KG_PROCESS_ERROR(hTreeItem);

	nItemData = (int)GetItemData(hTreeItem);
	KG_PROCESS_ERROR(nItemData != SUBSET_TREE_DATA);
	KG_PROCESS_ERROR(nItemData >= 1);

	nItemPos = GetItemPos(hTreeItem);
	KG_PROCESS_ERROR(nItemPos > 2);

	(*m_ppTabFile)->WriteString(nItemPos, _T("PrequestID1"), _T(""));
	(*m_ppTabFile)->WriteString(nItemPos, _T("PrequestID2"), _T(""));
	(*m_ppTabFile)->WriteString(nItemPos, _T("PrequestID3"), _T(""));
	(*m_ppTabFile)->WriteString(nItemPos, _T("PrequestID4"), _T(""));

	hParentItem = GetParentItem(hTreeItem);

	if (ItemIsInSubset(hTreeItem))
	{
		hPrevItem = GetPrevDataItem(hParentItem);
		if (ItemIsInSubset(hPrevItem))
		{
			hTempItem = GetParentItem(hPrevItem);
		}
		else
		{
			hTempItem = hPrevItem;
		}
		if (GetParentItem(hParentItem) != GetParentItem(hTempItem))
		{
			hPrevItem = NULL;
		}
	}
	else
	{
		hPrevItem = GetPrevDataItem(hTreeItem);
		if (ItemIsInSubset(hPrevItem))
		{
			hTempItem = GetParentItem(hPrevItem);
		}
		else
		{
			hTempItem = hPrevItem;
		}
		if (GetParentItem(hTreeItem) != GetParentItem(hTempItem))
		{
			hPrevItem = NULL;
		}
	}

	KG_PROCESS_SUCCESS(!hPrevItem);

	nRetCode = IsDataItem(hPrevItem);
	KG_PROCESS_SUCCESS(!nRetCode);

	if (ItemIsInSubset(hPrevItem))
	{
		CString szRelatingData  = _T("");
		char szRelatingLogc[2];
		int  nRelatingNumber     = 0;

		hChildItem = GetChildItem(GetParentItem(hPrevItem));
		ASSERT(hChildItem);

		while (hChildItem && nRelatingNumber < RELATING_DATA_COUNT)
		{
			int nPrevItemPos = GetItemPos(hChildItem);
			ASSERT(nPrevItemPos >= -1);
			szRelatingData.Format(_T("PrequestID%d"), nRelatingNumber + 1);

			nRetCode = (*m_ppTabFile)->GetString(
				nPrevItemPos, _T("QuestID"), "", szTempValue, sizeof(szTempValue)
			);
			KG_PROCESS_ERROR(nRetCode);

			nRetCode = (*m_ppTabFile)->WriteString(
				nItemPos, szRelatingData.GetBuffer(), szTempValue
			);
			KG_PROCESS_ERROR(nRetCode);

			hChildItem = GetNextSiblingItem(hChildItem);
			nRelatingNumber++;
		}
		(*m_ppTabFile)->GetString(
			nItemPos, _T("PrequestLogic"), _T(""), szRelatingLogc, sizeof(szRelatingLogc)
		);
		if (CString(szRelatingLogc) ==  _T(""))
		{
			(*m_ppTabFile)->WriteString(nItemPos, _T("PrequestLogic"), _T("1"));
		}
	}
	else
	{
		int nPrevItemPos = GetItemPos(hPrevItem);
		ASSERT(nPrevItemPos >= -1);
		nRetCode = (*m_ppTabFile)->GetString(
			nPrevItemPos, _T("QuestID"), "", szTempValue, sizeof(szTempValue)
		);
		KG_PROCESS_ERROR(nRetCode);
		(*m_ppTabFile)->WriteString(nItemPos, _T("PrequestID1"), szTempValue);
	}

Exit1:
	nResult = true;
Exit0:
	Invalidate();
	return nResult;
}

int KGPropertyTreeCtrl::FindInsertPosInFile(
	const CString pcszClassifyField[], int nFieldCount
)
{
	int nResult  = -1;
	//int nRetCode = false;

	//char szTempValue[256];
	//*szTempValue = '\0';

	//int nRowsNumber = 0;
	//int nCurRow	    = 0;
	//int nCurField   = 0;
	//int nTempField  = 0;
	//int nTempRow	= 0;

	//KG_PROCESS_ERROR(nFieldCount <= m_nClassifyFieldsSize);
	//KG_PROCESS_ERROR(m_ppTabFile);
	//KG_PROCESS_ERROR(*m_ppTabFile);
	//KG_PROCESS_ERROR(pcszClassifyField);
	//KG_PROCESS_ERROR(nFieldCount >= 1);

	//nRowsNumber = (*m_ppTabFile)->GetHeight();
	//KG_PROCESS_ERROR(nRowsNumber >= 2);

	//for (nCurRow = 3; nCurRow <= nRowsNumber; nCurRow++)
	//{
	//	for (nCurField = 0; nCurField < nFieldCount; nCurField++)
	//	{
	//		//int nCols = m_pnTypeRow[nCurField];
	//		nRetCode = (*m_ppTabFile)->GetString(
	//			nCurRow, m_ppszClassifyFields[nCurField], "", szTempValue, sizeof(szTempValue)
	//		);
	//		if (CString(szTempValue) != pcszClassifyField[nCurField])
	//			break;
	//	}
	//	if (nCurField > nTempField)
	//	{
	//		nTempField = nCurField;
	//		nTempRow   = nCurRow;
	//	}
	//}

	//if (!nTempRow)
	//{
	//	nResult = nRowsNumber;
	//	KG_PROCESS_SUCCESS(true);
	//}
	//
	//for (nCurRow = nTempRow + 1; nCurRow <= nRowsNumber; nCurRow++)
	//{
	//	for (nCurField = 0; nCurField < nTempField; nCurField++)
	//	{
	//		//int nCols = m_pnTypeRow[nCurField];
	//		nRetCode = (*m_ppTabFile)->GetString(
	//			nCurRow, m_ppszClassifyFields[nCurField], "", szTempValue, sizeof(szTempValue)
	//		);
	//		if (CString(szTempValue) != pcszClassifyField[nCurField])
	//		{
	//			break;
	//		}
	//	}
	//	if (nCurField < nTempField)
	//	{
	//		break;
	//	}
	//}

	//nResult = nCurRow - 1;
	nResult = (*m_ppTabFile)->GetHeight();

//Exit1:
//Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::GetCollectivityFirstPos(HTREEITEM hItem)
{
	int nResult  = -1;
	int nRetCode = false;

	char szTempValue[1024];
	*szTempValue = '\0';

	int  i = 0;
	int  j = 0;

	int  nNumber      = 0;
	int  nItemData    = 0;
	int  nRowsNumber  = 0;

	HTREEITEM hTempItem = hItem;

	CString* pszTitleText = NULL;

	KG_PROCESS_ERROR(m_ppTabFile);
	KG_PROCESS_ERROR(*m_ppTabFile);
	KG_PROCESS_ERROR(hItem);

	nItemData = (int)GetItemData(hItem);
	if (nItemData >= 1)
	{
		return GetItemPos(hItem);
	}

	nNumber = nItemData * -1 + 1;
	KG_PROCESS_ERROR(nNumber >= 1);

	pszTitleText = new CString[nNumber];
	for (i = 0; i < nNumber; i++)
	{
		pszTitleText[i] = "";
	}

	hTempItem = hItem;
	i = nNumber - 1;
	while (hTempItem && i >= 0)
	{
		pszTitleText[i--] = GetItemText(hTempItem);
		hTempItem = GetParentItem(hTempItem);
	}

	nRowsNumber = (*m_ppTabFile)->GetHeight();
	KG_PROCESS_ERROR(nRowsNumber > 2);

	for (i = 3; i <= nRowsNumber; i++)
	{
		for (j = 0; j < nNumber; j++)
		{
			//int nCols = m_pnTypeRow[j];
			nRetCode = (*m_ppTabFile)->GetString(
				i, m_ppszClassifyFields[j], "", szTempValue, sizeof(szTempValue)
			);
			if (CString(szTempValue) != pszTitleText[j])
			{
				break;
			}
		}
		if (j >= nNumber)
		{
			break;
		}
	}
	KG_PROCESS_ERROR(i <= nRowsNumber);

	nResult = i;
Exit0:
	pszTitleText->ReleaseBuffer();
	return nResult;
}

int KGPropertyTreeCtrl::GetCollectivityLastPos(HTREEITEM hItem)
{
	int nResult  = -1;
	int nRetCode = false;

	char szTempValue[1024];
	*szTempValue = '\0';

	int  i = 0;
	int  j = 0;

	int  nNumber      = 0;
	int  nItemData    = 0;
	int  nRowsNumber  = 0;

	HTREEITEM hTempItem = hItem;

	CString* pszTitleText = NULL;

	KG_PROCESS_ERROR(m_ppTabFile);
	KG_PROCESS_ERROR(*m_ppTabFile);
	KG_PROCESS_ERROR(hItem);

	nItemData = (int)GetItemData(hItem);
	if (nItemData >= 1)
	{
		return GetItemPos(hItem);
	}

	nNumber = nItemData * -1 + 1;
	KG_PROCESS_ERROR(nNumber >= 1);

	pszTitleText = new CString[nNumber];
	for (i = 0; i < nNumber; i++)
	{
		pszTitleText[i] = "";
	}

	hTempItem = hItem;
	i = nNumber - 1;
	while (hTempItem && i >= 0)
	{
		pszTitleText[i--] = GetItemText(hTempItem);
		hTempItem = GetParentItem(hTempItem);
	}

	nRowsNumber = (*m_ppTabFile)->GetHeight();
	KG_PROCESS_ERROR(nRowsNumber > 2);

	i = GetCollectivityFirstPos(hItem);
	KG_PROCESS_ERROR(i >= 1);

	for (i = i + 1; i <= nRowsNumber; i++)
	{
		for (j = 0; j < nNumber; j++)
		{
			//int nCols = m_pnTypeRow[j];
			nRetCode = (*m_ppTabFile)->GetString(
				i, m_ppszClassifyFields[j], "", szTempValue, sizeof(szTempValue)
			);
			if (CString(szTempValue) != pszTitleText[j])
			{
				break;
			}
		}
		if (j < nNumber)
		{
			break;
		}
	}

	nResult = i - 1;
Exit0:
	pszTitleText->ReleaseBuffer();
	return nResult;
}

int KGPropertyTreeCtrl::BeginStick(
	HTREEITEM hSour, HTREEITEM hDest, MOVE_TYPE eCoypType
)
{
	int nResult  = false;
	int nRetCode = false;

	int nDestData  = 0;
	int nSourData  = 0;
	int nDestRow   = 0;
	int nSourRow   = 0;
	int i = 0;
	int nShowErrorMessageFlag = true;

	HTREEITEM hChildItem = NULL;
	HTREEITEM hTempItem  = hDest;

	KG_PROCESS_ERROR(hSour);
	KG_PROCESS_ERROR(hDest);

	nDestData  = (int)GetItemData(hDest);
	nSourData  = (int)GetItemData(hSour);

	if (nDestData < 1)
	{
		KG_PROCESS_ERROR(nDestData == nSourData);
	}
	else
	{
		KG_PROCESS_ERROR(nSourData >= 1);
	}

	if (ItemIsInSubset(hDest) && IsSubsetItem(hSour))
	{
		KG_PROCESS_ERROR(false);
	}

	hTempItem = GetParentItem(hDest);
	if (hTempItem)
	{
		if (GetItemData(hTempItem) == SUBSET_TREE_DATA)
		{
			if (eCoypType == MOVE_TYPE_COPY)
				KG_PROCESS_ERROR(GetSibingCount(hDest) < 4);
			else
			{
				if (GetParentItem(hDest) == GetParentItem(hSour))
					KG_PROCESS_ERROR(GetSibingCount(hDest) < 5);
				else
					KG_PROCESS_ERROR(GetSibingCount(hDest) < 4);
			}
		}
	}

	/*if (nDestData < 1)
	{
		KGValueEditGetValueDlg dlg(this, _T("请输入新名字"), 32);
		while (dlg.DoModal() == IDOK)
		{
			m_stringName = dlg.GetValue();
			hTempItem	 = hDest;
			while (hTempItem)
			{
				CString stringName = GetItemText(hTempItem);
				if (stringName == m_stringName)
				{
					MessageBox(
						_T("名字已存在,请出入另外一个名字."), _T("Error!!"), MB_OK | MB_ICONWARNING
					);
					break;
				}
				hTempItem = GetPrevSiblingItem(hTempItem);
			}
			while (hTempItem)
			{
				CString stringName = GetItemText(hTempItem);
				if (stringName == m_stringName)
				{
					MessageBox(
						_T("名字已存在,请出入另外一个名字."), _T("Error!!"), MB_OK | MB_ICONWARNING
						);
					break;
				}
				hTempItem = GetPrevSiblingItem(hTempItem);
			}

		}
		nShowErrorMessageFlag = false;
		KG_PROCESS_ERROR(false);
	}*/

	if (GetParent())
	{
		GetParent()->SendMessage(WM_USER_BACKUP_TAB_FILE);
		((KGObjectPropertyEditTabDlg*)GetParent())->m_nBeSavedFlag = false;
	}

	nResult = true;
Exit0:
	if (!nResult && nShowErrorMessageFlag)
	{
		::MessageBox(
			this->m_hWnd, _T("====不能粘贴===="), _T("粘贴出错!!"), MB_OK | MB_ICONERROR
		);
	}
	return nResult;
}

int KGPropertyTreeCtrl::AfterStick(HTREEITEM hNewItem, MOVE_TYPE eCopyType)
{
	int nResult  = false;
	int nRetCode = false;

	//SetItemText(hNewItem, _T("==请输入新的名字=="));
	int nItemData = (int)GetItemData(hNewItem);

	if (nItemData < 1)
	{
		EditLabel(hNewItem);
	}

	nResult = true;
//Exit0:
	return nResult;
}
/*
int KGPropertyTreeCtrl::FindInSubset(CString szPrimaryKey, CString& szReturnSectionName)
{
	int nResult  = false;
	int nRetCode = false;

	char szSectionName[SECTION_NAME_SIZE];
	*szSectionName = '\0';

	KG_PROCESS_ERROR(m_ppQuestIniFile);
	KG_PROCESS_ERROR(*m_ppQuestIniFile);

	nRetCode = (*m_ppQuestIniFile)->GetNextSection(szSectionName, szSectionName);
	while (nRetCode)
	{
		CString szTempString = szSectionName;
		CString szTokenizeFinded;
		int nPos = 0;
		szTokenizeFinded = szTempString.Tokenize(_T("-"), nPos);
		while (szTokenizeFinded != _T(""))
		{
			if (szTokenizeFinded == szPrimaryKey)
			{
				break;
			}
			szTokenizeFinded = szTempString.Tokenize(_T("-"), nPos);
		}
		if (szTokenizeFinded != _T(""))
		{
			break;
		}
		nRetCode = (*m_ppQuestIniFile)->GetNextSection(szSectionName, szSectionName);
	}
	KG_PROCESS_ERROR(nRetCode);
	szReturnSectionName = szSectionName;

	nResult = true;
Exit0:
	return nResult;
}
*/

int KGPropertyTreeCtrl::DelRowInTabFile(HTREEITEM hItem)
{
	int nResult  = false;
	int nRetCode = false;

	int nItemData   = 0;
	int nItemPos    = 0;
	int nPrimaryKey = 0;
	CString szSectionName;
	CString szItemData;
	KG_PROCESS_ERROR(hItem);

	nItemData = (int)GetItemData(hItem);
	szItemData.Format("%d", nItemData);

	nItemData = (int)GetItemData(hItem);

	if (nItemData >= 1 && nItemData != SUBSET_TREE_DATA)
	{
		nItemPos = GetItemPos(hItem);
		(*m_ppTabFile)->GetInteger(
			nItemPos, _T("QuestID"), 0, &nPrimaryKey
		);
		m_pPrimaryManager->AddKey(nPrimaryKey);
		(*m_ppTabFile)->Remove(nItemPos);
		FindInSubset(*m_ppQuestIniFile, szItemData, szSectionName);
		if (szSectionName != "")
		{
			EraseItemInSubset(hItem);
		}
	}
	DelRowInTabFile(GetChildItem(hItem));
	DelRowInTabFile(GetNextSiblingItem(hItem));

	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::EraseItemInSubset(HTREEITEM hTreeItem)
{
	int nResult	 = false;
	int nRetCode = false;

	CString szNewSectionName = _T("");
	CString szSectionName    = _T("");
	CString szItemDate       = _T("");
	CString szTempString     = _T("");

	char szTempValue[256];
	*szTempValue   = '\0';
	int  nItemData = 0;

	KG_PROCESS_ERROR(m_ppQuestIniFile);
	KG_PROCESS_ERROR(*m_ppQuestIniFile);
	KG_PROCESS_ERROR(hTreeItem);

	nItemData = (int)GetItemData(hTreeItem);
	szItemDate.Format(_T("%d"), nItemData);

	FindInSubset(*m_ppQuestIniFile, szItemDate, szSectionName);
	KG_PROCESS_ERROR(szSectionName != _T(""));

	szNewSectionName = szSectionName;

	szItemDate.Format(_T("-%d-"), nItemData);
	nRetCode = szNewSectionName.Find(szItemDate);
	if (nRetCode == -1)
	{
		szItemDate.Format(_T("%d-"), nItemData);
		szTempString = szNewSectionName.Left(szItemDate.GetLength());
		if (szTempString != szItemDate)
		{
			szItemDate.Format(_T("-%d"), nItemData);
			szTempString = szNewSectionName.Right(szItemDate.GetLength());
			if (szTempString != szItemDate)
			{
				szItemDate = _T("");
			}
		}
	}
	else
	{
		szItemDate.Format(_T("%d-"), nItemData);
	}

	KG_PROCESS_ERROR(szItemDate != _T(""));
	szNewSectionName.Replace(szItemDate, _T(""));

	if (szNewSectionName != _T(""))
	{
		(*m_ppQuestIniFile)->GetString(
			szSectionName, _T("Name"), _T(""), szTempValue, sizeof(szTempValue)
		);
		(*m_ppQuestIniFile)->WriteString(
			szNewSectionName.GetBuffer(), _T("Name"), szTempValue
		);
		(*m_ppQuestIniFile)->GetString(
			szSectionName, _T("TreeHandle"), "", szTempValue, sizeof(szTempValue)
		);
		(*m_ppQuestIniFile)->WriteString(
			szNewSectionName.GetBuffer(), _T("TreeHandle"), szTempValue
		);
	}
	(*m_ppQuestIniFile)->EraseSection(szSectionName);

	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::AfterDelItem(HTREEITEM hNextItem)
{
	int nResult  = false;
	int nRetCode = false;

	if (m_hNextItem)
	{
		//nRetCode = UpdateInfoRelating(m_hNextItem);
		//KG_PROCESS_ERROR(nRetCode);
		nRetCode = ItemIsInSubset(m_hNextItem);
		if (nRetCode)
		{
			while (m_hNextItem)
			{
				nRetCode = UpdateInfoRelating(m_hNextItem);
				KG_PROCESS_ERROR(nRetCode);
				m_hNextItem = GetNextSiblingItem(m_hNextItem);
			}
		}
		else
		{
			nRetCode = UpdateInfoRelating(m_hNextItem);
			KG_PROCESS_ERROR(nRetCode);
		}
	}

	if (GetParent())
	{
		GetParent()->SendMessage(WM_USER_BACKUP_TAB_FILE);
		((KGObjectPropertyEditTabDlg*)GetParent())->m_nBeSavedFlag = false;
		((KGObjectPropertyEditTabDlg*)GetParent())->UpdateStateBar();
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::BeginDelItem(HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hTempItem   = NULL;

	CString szSectionName = _T("");
	CString szItemData    = _T("");

	int nItemData = 0;
	int nItemPos  = 0;

	KG_PROCESS_ERROR(hTreeItem);

	nItemData = (int)GetItemData(hTreeItem);
	szItemData.Format(_T("%d"), nItemData);

	if (nItemData >= 1 && nItemData != SUBSET_TREE_DATA)
	{
		nItemPos = GetItemPos(hTreeItem);
		m_pPrimaryManager->AddKey(nItemData);
		(*m_ppTabFile)->Remove(nItemPos);

		FindInSubset(*m_ppQuestIniFile, szItemData, szSectionName);
		if (szSectionName != _T(""))
		{
			hTempItem = GetNextDataItem(GetLastSibingItem(hTreeItem));
			EraseItemInSubset(hTreeItem);
		}
		else
		{
			hTempItem = GetNextDataItem(hTreeItem);
		}
	}
	else
	{
		DelRowInTabFile(GetChildItem(hTreeItem));
		hTempItem = GetNextDataItem(GetInnermostChildItem(hTreeItem, false));
	}

	m_hNextItem = hTempItem;
	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::IsSubsetItem(HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	int nItemData = 0;

	KG_PROCESS_ERROR(hTreeItem);

	nItemData = (int)GetItemData(hTreeItem);

	if (nItemData == SUBSET_TREE_DATA)
	{
		KG_PROCESS_SUCCESS(true);
	}
	else
	{
		KG_PROCESS_ERROR(false);
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::IsDataItem(HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	int nItemData = 0;

	KG_PROCESS_ERROR(hTreeItem);

	nItemData = (int)GetItemData(hTreeItem);

	if (nItemData >= 1 &&nItemData != SUBSET_TREE_DATA)
	{
		KG_PROCESS_SUCCESS(true);
	}
	else
	{
		KG_PROCESS_ERROR(false);
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::IsClassItem(HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	int nItemData = 0;

	KG_PROCESS_ERROR(hTreeItem);

	nItemData = (int)GetItemData(hTreeItem);

	if (nItemData < 1)
	{
		KG_PROCESS_SUCCESS(true);
	}
	else
	{
		KG_PROCESS_ERROR(false);
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::ItemIsInSubset(HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hTempItem = NULL;
	int nItemData = 0;

	KG_PROCESS_ERROR(hTreeItem);
	hTempItem = GetParentItem(hTreeItem);
	KG_PROCESS_ERROR(hTempItem);

	nItemData = (int)GetItemData(hTempItem);

	if (nItemData == SUBSET_TREE_DATA)
	{
		KG_PROCESS_SUCCESS(true);
	}
	else
	{
		KG_PROCESS_ERROR(false);
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::GetInnermostChildItem_Assistant(
	HTREEITEM* phTreeItem, int* pnCounter, HTREEITEM hTreeItem,
	int nLayer, int nCallFlag, int nFirstFlag
)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(phTreeItem);
	KG_PROCESS_ERROR(pnCounter);
	KG_PROCESS_ERROR(hTreeItem);

	if (nCallFlag && !ItemIsInSubset(hTreeItem))
	{
		nLayer++;
	}

	if (nFirstFlag)
	{
		if (nLayer > *pnCounter  && !IsSubsetItem(hTreeItem))
		{
			*pnCounter  = nLayer;
			*phTreeItem = hTreeItem;
		}
	}
	else
	{
		if (nLayer >= *pnCounter && !IsSubsetItem(hTreeItem))
		{
			*pnCounter  = nLayer;
			*phTreeItem = hTreeItem;
		}
	}

	GetInnermostChildItem_Assistant(
		phTreeItem, pnCounter, GetChildItem(hTreeItem), nLayer,
		true, nFirstFlag
	);
	GetInnermostChildItem_Assistant(
		phTreeItem, pnCounter, GetNextSiblingItem(hTreeItem),
		nLayer, false, nFirstFlag
	);

	nResult = true;
Exit0:
	return nResult;
}

HTREEITEM KGPropertyTreeCtrl::GetInnermostChildItem(
	HTREEITEM hTreeItem, int nFirstFlag
)
{
	HTREEITEM hResultItem = NULL;
	HTREEITEM hChildItem  = NULL;
	int nCounter = 0;

	KG_PROCESS_ERROR(hTreeItem);
	hChildItem = GetChildItem(hTreeItem);

	if (hChildItem)
	{
		GetInnermostChildItem_Assistant(
			&hResultItem, &nCounter, hChildItem, 1, false, nFirstFlag
		);
	}

	if (!hResultItem)
	{
		hResultItem = hTreeItem;
	}

Exit0:
	return hResultItem;
}

HTREEITEM KGPropertyTreeCtrl::GetNextDataItem(HTREEITEM hTreeItem)
{
	HTREEITEM hResultItem = NULL;
	HTREEITEM hNextItem   = NULL;
	HTREEITEM hParenItem  = NULL;
	HTREEITEM hTempItem   = NULL;
	int nItemDate         = 0;

	KG_PROCESS_ERROR(hTreeItem);

	hParenItem = hTreeItem;

	if (IsClassItem(hParenItem) || IsSubsetItem(hParenItem))
	{
		hNextItem = GetInnermostChildItem(hParenItem);
	}
	KG_PROCESS_SUCCESS(IsDataItem(hNextItem));

	while (hParenItem)
	{
		hNextItem = GetNextSiblingItem(hParenItem);
		while (hNextItem)
		{
			hTempItem = GetInnermostChildItem(hNextItem, true);
			CString string = GetItemText(hTempItem);
			if (IsDataItem(hTempItem))
			{
				hNextItem = hTempItem;
				KG_PROCESS_SUCCESS(true);
			}
			hNextItem = GetNextSiblingItem(hNextItem);
		}
		hParenItem = GetParentItem(hParenItem);
	}

Exit1:
	hResultItem = hNextItem;
Exit0:
	return hResultItem;
}

HTREEITEM KGPropertyTreeCtrl::GetPrevDataItem(HTREEITEM hTreeItem)
{
	HTREEITEM hResultItem = NULL;
	HTREEITEM hPrevItem   = NULL;
	HTREEITEM hParenItem  = NULL;
	HTREEITEM hTempItem   = NULL;
	int nItemDate         = 0;

	KG_PROCESS_ERROR(hTreeItem);

	hParenItem = hTreeItem;

	while (hParenItem)
	{
		hPrevItem = GetPrevSiblingItem(hParenItem);
		while (hPrevItem)
		{
			hTempItem = GetInnermostChildItem(hPrevItem, false);
			if (IsDataItem(hTempItem))
			{
				hPrevItem = hTempItem;
				KG_PROCESS_SUCCESS(true);
			}
			hPrevItem = GetPrevSiblingItem(hPrevItem);
		}
		hParenItem = GetParentItem(hParenItem);
	}

Exit1:
	hResultItem = hPrevItem;
Exit0:
	return hResultItem;
}

int KGPropertyTreeCtrl::AfterCopyItem(
	HTREEITEM hCopyItem, HTREEITEM hNewItem, HTREEITEM hParentItem,
	HTREEITEM hAfterItem
)
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hPrevDataItem = NULL;
	HTREEITEM hNextDataItem = NULL;

	int nItemData    = 0;
	int nSourItemPos = 0;
	int nDestItemPos = 0;
	int nRowNumber   = 0;

	KG_PROCESS_ERROR(m_ppTabFile);
	KG_PROCESS_ERROR(*m_ppTabFile);

	KG_PROCESS_ERROR(hCopyItem);
	KG_PROCESS_ERROR(hNewItem);

	nRowNumber = (*m_ppTabFile)->GetHeight();
	KG_PROCESS_ERROR(nRowNumber >= 2);

	nItemData = (int)GetItemData(hCopyItem);
	KG_PROCESS_SUCCESS(nItemData  < 1);
	KG_PROCESS_SUCCESS(nItemData == SUBSET_TREE_DATA);

	nItemData = (int)GetItemData(hNewItem);
	KG_PROCESS_SUCCESS(nItemData  < 1);
	KG_PROCESS_SUCCESS(nItemData == SUBSET_TREE_DATA);

	nSourItemPos = GetItemPos(hCopyItem);

	ASSERT(hParentItem);

	hPrevDataItem = GetPrevDataItem(hNewItem);
	//hNextDataItem = GetNextDataItem(hNewItem);
	nRetCode = ItemIsInSubset(hNewItem);
	if (nRetCode)
	{
		hNextDataItem = GetNextDataItem(GetLastSibingItem(hNewItem));
	}
	else
	{
		hNextDataItem = GetNextDataItem(hNewItem);
	}

	if (hPrevDataItem)
	{
		nDestItemPos = GetItemPos(hPrevDataItem);
	}
	else
	{
		nDestItemPos = 2;
	}

	if (nDestItemPos < nSourItemPos)
	{
		nSourItemPos++;
	}
	(*m_ppTabFile)->InsertAfter(nDestItemPos++);
	nRetCode = CopyOneRow(nSourItemPos, nDestItemPos);
	KG_PROCESS_ERROR(nRetCode);

	//nItemData = FindId(_T("QuestID"));

	m_pPrimaryManager->GetKey(&nItemData);

	nRetCode  = SetItemData(hNewItem, nItemData);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode  = (*m_ppTabFile)->WriteInteger(
		nDestItemPos, _T("QuestID"), nItemData
	);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = RefrushTypeInfo(hNewItem);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateInfoRelating(hNewItem);
	KG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_SUCCESS(!hNextDataItem);
	nRetCode = ItemIsInSubset(hNextDataItem);

	if (nRetCode)
	{
		while (hNextDataItem)
		{
			nRetCode = UpdateInfoRelating(hNextDataItem);
			KG_PROCESS_ERROR(nRetCode);
			hNextDataItem = GetNextSiblingItem(hNextDataItem);
		}
	}
	else
	{
		nRetCode = UpdateInfoRelating(hNextDataItem);
		KG_PROCESS_ERROR(nRetCode);
	}

	((KGObjectPropertyEditTabDlg*)GetParent())->UpdateStateBar();
	((KGObjectPropertyEditTabDlg*)GetParent())->m_nBeSavedFlag = false;

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

// Update 2006.3.6.7.25
int KGPropertyTreeCtrl::GetItemPos(HTREEITEM hItem)
{
	int nResult  = -1;
	int nRetCode = false;

	int nItemData   = 0;
	int nRowsNumber = 0;
	int nValue      = 0;
	int nPos        = 0;

	KG_PROCESS_ERROR(hItem);

	nRowsNumber = (*m_ppTabFile)->GetHeight();
	nItemData   = (int)GetItemData(hItem);

	KG_PROCESS_ERROR(nItemData >= 1);

	if (nItemData == SUBSET_TREE_DATA)
	{
		HTREEITEM hChildItem = GetChildItem(hItem);
		if (hChildItem && GetItemPos(hChildItem) > 2)
		{
			while (GetNextSiblingItem(hChildItem))
			{
				hChildItem = GetNextSiblingItem(hChildItem);
			}
			nPos = GetItemPos(hChildItem);
		}
		else
		{
			nPos = GetItemPos(GetPrevSiblingItem(hItem));
		}
	}
	else
	{
		for (nPos = 3; nPos <= nRowsNumber; nPos++ )
		{
			nRetCode = (*m_ppTabFile)->GetInteger(nPos, m_pszPrimaryKesField, 0, &nValue);
			KG_PROCESS_ERROR(nRetCode);
			if (nItemData == nValue)
			{
				break;
			}
		}
	}
	KG_PROCESS_ERROR(nPos <= nRowsNumber);


	nResult = nPos;
Exit0:
	return nResult;
}

int KGPropertyTreeCtrl::Update(int* nLoadNumber)
{
    int nResult  = false;
	int nRetCode = false;

	int nIndex   = 0;

	HTREEITEM* ppClassItems = new HTREEITEM[m_nClassifyFieldsSize];
	char** pszItemText = new char*[m_nClassifyFieldsSize];

	for (nIndex = 0; nIndex < m_nClassifyFieldsSize; nIndex++)
	{
		pszItemText[nIndex]  = new char[64];
		ppClassItems[nIndex] = 0;
	}

	char szCaption[64] = _T("");

	int  nRowsCount  = 0;
	int  nColsCount  = 0;
	int  nClassRow   = 0;
	int  nCurRow     = 0;

	HTREEITEM pTreeItem = TVI_ROOT;

	KG_PROCESS_ERROR(m_ppTabFile);
	KG_PROCESS_ERROR(*m_ppTabFile);
	KG_PROCESS_ERROR(m_ppIniFile);
	KG_PROCESS_ERROR(*m_ppIniFile);
	//KG_PROCESS_ERROR(m_nTypeRowSize);
	//KG_PROCESS_ERROR(m_nDataRow);

	nRetCode = (*m_ppTabFile)->FindColumn(m_pszPrimaryKesField);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = (*m_ppTabFile)->FindColumn(m_pszCaptionField);
	KG_PROCESS_ERROR(nRetCode);

	for (nIndex = 0; nIndex < m_nClassifyFieldsSize; nIndex++)
	{
		nRetCode = (*m_ppTabFile)->FindColumn(m_ppszClassifyFields[nIndex]);
		KG_PROCESS_ERROR(nRetCode);
	}

	DeleteAllItems();

	nRowsCount = (*m_ppTabFile)->GetHeight();
	nColsCount = (*m_ppTabFile)->GetWidth();

	KG_PROCESS_SUCCESS(nRowsCount == 2);
	KG_PROCESS_ERROR(nRowsCount >= 2);

	for (nCurRow = 3; nCurRow <= nRowsCount; nCurRow++)
	{
		CString szSubSectionName  = _T("");
		CString szItemData		  = _T("");
		CString szItemText        = _T("");

		HTREEITEM hParentTreeItem = NULL;

		int nInsertIndex = 0;
		int nItemData    = 0;

		nRetCode  = (*m_ppTabFile)->GetString(
			nCurRow, m_pszCaptionField, _T(""), szCaption, sizeof (szCaption)
		);
		KG_PROCESS_ERROR(nRetCode);

		nRetCode  = (*m_ppTabFile)->GetInteger(
			nCurRow, m_pszPrimaryKesField, 0, &nItemData
		);
		KG_PROCESS_ERROR(nRetCode);

		for (nIndex = 0; nIndex < m_nClassifyFieldsSize; nIndex++)
		{
			(*m_ppTabFile)->GetString(
				nCurRow, m_ppszClassifyFields[nIndex], _T(""),
				pszItemText[nIndex], 64
			);
		}

		for (nIndex = 0; nIndex < m_nClassifyFieldsSize; nIndex++)
		{
			if (nIndex)
			{
				ppClassItems[nIndex] = GetChildItem(ppClassItems[nIndex - 1]);
			}
			else
			{
				ppClassItems[nIndex] = GetRootItem();
			}

			while (ppClassItems[nIndex])
			{
				szItemText = GetItemText(ppClassItems[nIndex]);
				if (szItemText == pszItemText[nIndex])
				{
					break;
				}
				ppClassItems[nIndex] = GetNextSiblingItem(ppClassItems[nIndex]);
			}

			if (!ppClassItems[nIndex])
			{
				break;
			}
		}

		for (nInsertIndex = 0; nInsertIndex < m_nClassifyFieldsSize; nInsertIndex++)
		{
			if (!ppClassItems[nInsertIndex])
			{
				break;
			}
		}

		for (nIndex = nInsertIndex; nIndex < m_nClassifyFieldsSize; nIndex++)
		{
			if (nIndex)
			{
				pTreeItem = InsertItem(
					pszItemText[nIndex], nIndex, nIndex,
					ppClassItems[nIndex - 1]
				);
				ppClassItems[nIndex] = pTreeItem;
				SetItemData(pTreeItem, nIndex * -1);
			}
			else
			{
				pTreeItem = InsertItem(
					pszItemText[nIndex], nIndex, nIndex
				);
				ppClassItems[nIndex] = pTreeItem;
				SetItemData(pTreeItem, nIndex);
			}
		}

		szItemData.Format("%d", nItemData);

		if (
			FindInSubset(*m_ppQuestIniFile, szItemData, szSubSectionName)
		)
		{
			int nTreeHandle = -1;
			(*m_ppQuestIniFile)->GetInteger(
				szSubSectionName.GetBuffer(), _T("TreeHandle"), -1, &nTreeHandle
			);
			if (nTreeHandle != -1)
			{
				hParentTreeItem = (HTREEITEM)(DWORD_PTR)nTreeHandle;
			}
			else
			{
				char szSubsetName[256];
				*szSubsetName = '\0';
				int nTreeHandle = -1;

				(*m_ppQuestIniFile)->GetString(
					szSubSectionName.GetBuffer(), _T("Name"),
					_T("任务子集"),	szSubsetName, sizeof(szSubsetName)
				);
				hParentTreeItem = InsertItem(
					szSubsetName, m_nClassifyFieldsSize, m_nClassifyFieldsSize,
					ppClassItems[m_nClassifyFieldsSize - 1]
				);
				SetItemData(hParentTreeItem, SUBSET_TREE_DATA);
				(*m_ppQuestIniFile)->WriteInteger(
					szSubSectionName.GetBuffer(), _T("TreeHandle"),
					(int)(DWORD_PTR)hParentTreeItem
				);
			}
		}
		else
		{
			hParentTreeItem = ppClassItems[m_nClassifyFieldsSize - 1];
		}
		pTreeItem = InsertItem(
			szCaption, m_fnGetImageIndex(*m_ppTabFile, nCurRow),
		    m_fnGetImageIndex(*m_ppTabFile, nCurRow),
			hParentTreeItem//ppClassItems[m_nTypeRowSize - 1]
		);

		SetItemData(pTreeItem, nItemData);

		if (nLoadNumber)
		{
			(*nLoadNumber)++;
		}
	}

Exit1:
    nResult = true;
Exit0:
	for (int nIndex = 0; nIndex < m_nClassifyFieldsSize; nIndex++)
	{
		SAFE_DELETE_ARRAY(pszItemText[nIndex]);
	}
	SAFE_DELETE(pszItemText);
	SAFE_DELETE(ppClassItems);
    return nResult;
}

BEGIN_MESSAGE_MAP(KGPropertyTreeCtrl, KGTreeCtrl)
	ON_WM_KEYUP()
	//ON_WM_DRAWITEM()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &KGPropertyTreeCtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &KGPropertyTreeCtrl::OnTvnEndlabeledit)
END_MESSAGE_MAP()

void KGPropertyTreeCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HTREEITEM hItem = (HTREEITEM)lpDrawItemStruct->hwndItem;
	int nSelFlag = lpDrawItemStruct->itemState & ODS_SELECTED;
	CString szTemp = "";
	char szText[256];
	*szText = '\0';
	int nIndex = 1;
	CDC dc;
	CRect rect = lpDrawItemStruct->rcItem;
	dc.FromHandle(lpDrawItemStruct->hDC);
	HTREEITEM hTempItem = hItem;

	while (GetPrevSiblingItem(hTempItem))
	{
		nIndex++;
		hTempItem = GetPrevSiblingItem(hTempItem);
	}
	szTemp.Format("%d", nIndex);
	dc.SetTextColor(RGB(255, 2, 2));
	dc.TextOut(rect.left, rect.top, szTemp);
	dc.SetTextColor(RGB(2, 255, 2));
	rect.left += 15;
	::GetWindowText(lpDrawItemStruct->hwndItem, szText, sizeof(szText));
	dc.TextOut(rect.left, rect.top, szText);
}

void KGPropertyTreeCtrl::OnPaint()
{
	CTreeCtrl::OnPaint();

	HTREEITEM hItem = GetSelectedItem();
	CString szItemText = GetItemText(hItem);
	CFont font;
	CRect rectClient;
	RECT  rect;
	CClientDC dc(this);
	CString szTemp = "";
	int nIndex = 1;

	font.CreateFont(
		15,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial")                // lpszFacename
		);
	dc.SelectObject(&font);
	dc.SetBkColor(/*RGB(2, 2, 255));//*/GetBkColor());
	dc.SetTextColor(RGB(2, 255, 2));
	GetItemRect(hItem, &rect, true);
	rect.left += 2;
	//
	HTREEITEM hTempItem = hItem;
	while (GetPrevSiblingItem(hTempItem))
	{
		nIndex++;
		hTempItem = GetPrevSiblingItem(hTempItem);
	}
	szTemp.Format("%d", nIndex);
	dc.SetTextColor(RGB(255, 2, 2));
	dc.TextOut(rect.left, rect.top, szTemp);
	dc.SetTextColor(RGB(2, 255, 2));
	rect.left += 15;
	dc.TextOut(rect.left, rect.top, szItemText);

	GetClientRect(&rectClient);

	if (::GetFocus() == this->m_hWnd)
	{
		//dc.Draw3dRect(&rectClient, RGB(255, 2, 2), RGB(255, 2, 2));
	}
	else
	{
		//dc.Draw3dRect(&rectClient, GetBkColor(), GetBkColor());
	}

}

void KGPropertyTreeCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nResult  = false;
	int nRetCode = false;

	switch (nChar)
	{
	case 'A' :
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				CString szstring = "";
				HTREEITEM hItem = GetPrevDataItem(GetSelectedItem());
				szstring = GetItemText(hItem);
				::MessageBox(this->m_hWnd, szstring.GetBuffer(), "LOVE", MB_OK);
			}
		}
		break;
	case 'S' :
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				CString szstring = "";
				HTREEITEM hItem = GetNextDataItem(GetSelectedItem());
				szstring = GetItemText(hItem);
				::MessageBox(this->m_hWnd, szstring.GetBuffer(), "LOVE", MB_OK);
			}
		}
		break;
	case 'B' :
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
			}
		}
		break;
	default:
		break;
	}

	nResult = true;
//Exit0:
	KGTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void KGPropertyTreeCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	static CRect rcItem;
	static int nItemState;
	HTREEITEM  hItem;
	HTREEITEM  hTempItem;
	int     nIndex = 1;
	RECT    rect;
	RECT	rtTp;
	char szItemText[256];
	CString szAftItemText;
	char    szAftText[256];
	*szAftText = '\0';
	int     nPos  = 0;
	int     nRow  = 0;
	int     dwItemData = 0;
	int		nQuestSwitchFlag = 0;

	int     nPreMarkFlag  = 0;
	int		nAftRowNumFag = 0;
	int		nMarkNameFlag = 0;

	char    szItemDate[32];
	*szItemDate = '\0';

	CClientDC dc(this);
	dc.SelectObject(GetFont());

	SIZE    nSize;
	LPNMTVCUSTOMDRAW pCustomDraw = (LPNMTVCUSTOMDRAW)pNMHDR;

	KG_PROCESS_ERROR(m_ppQuestIniFile);
	KG_PROCESS_ERROR(m_ppTabFile);
	KG_PROCESS_ERROR(m_ppIniFile);
	KG_PROCESS_ERROR(*m_ppQuestIniFile);
	KG_PROCESS_ERROR(*m_ppTabFile);
	KG_PROCESS_ERROR(*m_ppIniFile);

	// Update 2006-4-22
	int nStartupFlag = 0;
	(*m_ppIniFile)->GetInteger(
		_T("Setting"), _T("AppendWordFlag"), 0, &nStartupFlag
	);
	KG_PROCESS_SUCCESS(!nStartupFlag);
	//

	switch (pCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		nItemState = pCustomDraw->nmcd.uItemState;
		pCustomDraw->nmcd.uItemState &= ~CDIS_FOCUS;
		*pResult = CDRF_NOTIFYPOSTPAINT;
		break;
	case CDDS_ITEMPOSTPAINT:
		hItem = (HTREEITEM)pCustomDraw->nmcd.dwItemSpec;
		GetItemRect(hItem, &rect, true);
		rtTp.left   = rect.left - 2;
		rtTp.top    = rect.top  - 2;
		rtTp.right  = 800;
		rtTp.bottom = rect.bottom + 2;
		/*::FillRect(pCustomDraw->nmcd.hdc, &rtTp, ::CreateSolidBrush(GetBkColor()));
		::SetBkColor(pCustomDraw->nmcd.hdc, GetBkColor());*/
		dc.FillRect(&rtTp, &CBrush(GetBkColor()));
		dc.SetBkColor(GetBkColor());
		hTempItem = hItem;


		//////////////////////////////////////////////////////////////////////////
		//
		(*m_ppIniFile)->GetInteger(
			_T("Setting"), _T("PreNumber"), 0, &nPreMarkFlag
		);
		(*m_ppIniFile)->GetInteger(
			_T("Setting"), _T("AftRowNumber"), 0, &nAftRowNumFag
		);
		(*m_ppIniFile)->GetInteger(
			_T("Setting"), _T("ShowName"), 0, &nMarkNameFlag
		);
		//
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//
		if (nPreMarkFlag)
		{
			nIndex    = 1;
			while (GetPrevSiblingItem(hTempItem))
			{
				nIndex++;
				hTempItem = GetPrevSiblingItem(hTempItem);
			}
			sprintf_s(szItemText, "(%d)", nIndex);
			if (hItem == GetSelectedItem())
			{
				dc.SetTextColor(RGB(2, 255, 2));
			}
			else
			{
				dc.SetTextColor(RGB(125, 255, 255));
			}
			/*::TextOut(
				pCustomDraw->nmcd.hdc,rect.left, rect.top,
				szItemText, (int)strlen(szItemText)
			);
			::GetTextExtentPoint(
				pCustomDraw->nmcd.hdc, szItemText, (int)strlen(szItemText), &nSize
			);*/
			dc.TextOut(
				rect.left, rect.top, szItemText, (int)strlen(szItemText)
			);
			::GetTextExtentPoint(
				dc.m_hDC, szItemText, (int)strlen(szItemText), &nSize
			);
			rect.left += nSize.cx + 2;
		}
		//
		//////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////
		//
		/*nRow = GetItemPos(hItem);*/
		dwItemData = (int)GetItemData(hItem);
		itoa(dwItemData, szItemDate, 10);
		(*m_ppTabFile)->GetInteger(szItemDate, _T("IsAvailable"), 0, &nQuestSwitchFlag);
		/*(*m_ppTabFile)->GetInteger(
			nRow, _T("IsAvailable"), 0, &nQuestSwitchFlag
		);*/
		if (hItem == GetSelectedItem())
		{
			dc.SetTextColor(RGB(2, 255, 2));
		}
		else
		{
			if (!nQuestSwitchFlag && dwItemData >= 1 && dwItemData != SUBSET_TREE_DATA)
				dc.SetTextColor(RGB(255, 50, 50));
			else
				dc.SetTextColor(RGB(255, 255, 255));
		}
		strncpy(szItemText, GetItemText(hItem).GetBuffer(), sizeof(szItemText));
		/*::TextOut(
			pCustomDraw->nmcd.hdc,rect.left, rect.top,
			szItemText, (int)strlen(szItemText)
		);

		::GetTextExtentPoint(
			pCustomDraw->nmcd.hdc, szItemText, (int)strlen(szItemText), &nSize
		);*/
		dc.TextOut(
			rect.left, rect.top, szItemText, (int)strlen(szItemText)
		);
		::GetTextExtentPoint(
			dc.m_hDC, szItemText, (int)strlen(szItemText), &nSize
		);
		rect.left += nSize.cx + 4;
		//
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//
		/*nRow = GetItemPos(hItem);*/
		/*dwItemData = (int)GetItemData(hItem);*/
		if (dwItemData >= 1 && dwItemData != SUBSET_TREE_DATA)
		{
			int nHaveMarkFlag = false;
			int nLeft  = rect.left - 10;
			int nRight = 0;

			if (nAftRowNumFag)
			{
				nRow = GetItemPos(hItem);
				sprintf_s(szItemText, "(%d)", nRow);
				if (hItem == GetSelectedItem())
				{
					dc.SetTextColor(RGB(2, 255, 2));
				}
				else
				{
					dc.SetTextColor(RGB(125, 255, 255));
				}
				/*::TextOut(
					pCustomDraw->nmcd.hdc, rect.left, rect.top,
					szItemText, (int)strlen(szItemText)
				);
				nHaveMarkFlag = true;
				::GetTextExtentPoint(
					pCustomDraw->nmcd.hdc, szItemText, (int)strlen(szItemText), &nSize
				);*/
				dc.TextOut(
					rect.left, rect.top, szItemText, (int)strlen(szItemText)
				);
				nHaveMarkFlag = true;
				::GetTextExtentPoint(
					dc.m_hDC, szItemText, (int)strlen(szItemText), &nSize
				);
				rect.left += nSize.cx ;
			}

			CString szTempText;
			CString szTemp;

			(*m_ppIniFile)->GetString(
				_T("Setting"), _T("After_TreeItemText"), "",
				szAftText, sizeof(szAftText)
			);

			szTempText = szAftText;

			szTemp = szTempText.Tokenize(_T(","), nPos);
			while (szTemp != "")
			{
				if (nMarkNameFlag)
				{
					(*m_ppIniFile)->GetString(
						szTemp.GetBuffer(), _T("Name"), "",
						szItemText, sizeof(szItemText)
					);
					if (CString(szAftText) == "")
					{
						szTemp = szTempText.Tokenize(_T(","), nPos);
						continue;
					}
					szAftItemText += CString(szItemText) + _T(" : ");
					if (hItem == GetSelectedItem())
					{
						dc.SetTextColor(RGB(2, 255, 2));
					}
					else
					{
						dc.SetTextColor(RGB(125, 255, 255));
					}
					strncpy_s(
						szItemText, szAftItemText.GetBuffer(), sizeof(szItemText)
					);
					/*::TextOut(
						pCustomDraw->nmcd.hdc,rect.left, rect.top,
						szItemText, (int)strlen(szItemText)
					);
					::GetTextExtentPoint(
						pCustomDraw->nmcd.hdc, szItemText, (int)strlen(szItemText), &nSize
					);*/
					dc.TextOut(
						rect.left, rect.top, szItemText, (int)strlen(szItemText)
					);
					::GetTextExtentPoint(
						dc.m_hDC, szItemText, (int)strlen(szItemText), &nSize
					);
					rect.left += nSize.cx;
				}

			/*	(*m_ppTabFile)->GetString(
					nRow, szTemp.GetBuffer(), "",
					szItemText, sizeof(szItemText)
				);*/
				(*m_ppTabFile)->GetString(
					szItemDate, szTemp.GetBuffer(), "",
					szItemText, sizeof(szItemText)
				);

				if (hItem == GetSelectedItem())
				{
					dc.SetTextColor(RGB(2, 255, 2));
				}
				else
				{
					dc.SetTextColor(RGB(125, 255, 255));
				}

				szTemp = szTempText.Tokenize(_T(","), nPos);

				if (szTemp != "")
				{
					strncat_s(szItemText, _T(", "), sizeof(szItemText));
				}

				/*::TextOut(
					pCustomDraw->nmcd.hdc,rect.left, rect.top,
					szItemText, (int)strlen(szItemText)
				);
				nHaveMarkFlag = true;
				::GetTextExtentPoint(
					pCustomDraw->nmcd.hdc, szItemText, (int)strlen(szItemText), &nSize
				);*/
				dc.TextOut(
					rect.left, rect.top, szItemText, (int)strlen(szItemText)
				);
				nHaveMarkFlag = true;
				::GetTextExtentPoint(
					dc.m_hDC, szItemText, (int)strlen(szItemText), &nSize
				);
				rect.left += nSize.cx + 2;
				szAftItemText = "";
			}
			if (!nQuestSwitchFlag)
			{
				RECT rectSwitch;
				CPen pen(PS_SOLID, 2, RGB(255, 2, 2));
				dc.SelectObject(&pen);
				GetItemRect(hItem, &rectSwitch, true);
				rectSwitch.left   -= 18;
				rectSwitch.bottom -= 1;
				rectSwitch.right =
					rectSwitch.left + (rectSwitch.bottom - rectSwitch.top);
				dc.MoveTo(rectSwitch.left, rect.top);
				dc.LineTo(rectSwitch.right, rectSwitch.bottom);
				dc.MoveTo(rectSwitch.right, rect.top);
				dc.LineTo(rectSwitch.left, rectSwitch.bottom);
			}
			/*
			if (nHaveMarkFlag)
			{
				nRight = rect.left;
				if (hItem == GetSelectedItem())
				{
					::SetTextColor(pCustomDraw->nmcd.hdc, RGB(2, 255, 2));
				}
				else
				{
					::SetTextColor(pCustomDraw->nmcd.hdc, RGB(2, 255, 255));
				}
				::TextOut(
					pCustomDraw->nmcd.hdc, nLeft, rect.top,
					_T("("), sizeof(_T("("))
				);
				::TextOut(
					pCustomDraw->nmcd.hdc, nRight, rect.top,
					_T(")"), sizeof(_T(")"))
				);
			}*/
			/*CRect rectClient;
			GetClientRect(&rectClient);
			CClientDC dc(this);
			if (::GetFocus() == this->m_hWnd)
			{
				dc.Draw3dRect(&rect, RGB(255, 2, 2), RGB(255, 2, 2));
			}
			else
			{
				dc.Draw3dRect(&rect, GetBkColor(), GetBkColor());
			}*/
		}
		break;
	default:
		*pResult = CDRF_DODEFAULT;
	}
Exit1:
Exit0:
	return;
}

void KGPropertyTreeCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nResult  = false;
	int nRetCode = false;

	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	HTREEITEM hSelItem = pTVDispInfo->item.hItem;
	CString stringText = _T("");
	int nItemData = (int)GetItemData(hSelItem);
	int nRow = GetItemPos(hSelItem);

	if (pTVDispInfo->item.pszText)
	{
		stringText = pTVDispInfo->item.pszText;
	}
	else
	{
		stringText = GetItemText(pTVDispInfo->item.hItem);
	}
	stringText.TrimRight();
	stringText.TrimLeft();

	if (stringText == _T(""))
	{
		MessageBox(_T("名字不能为空!!"), _T("Error!!"), MB_OK | MB_ICONWARNING);
		EditLabel(pTVDispInfo->item.hItem);
		KG_PROCESS_ERROR(false);
	}

	if (nItemData < 1)
	{
		HTREEITEM hTempItem = GetPrevSiblingItem(pTVDispInfo->item.hItem);
		while (hTempItem)
		{
			CString stringTemp = GetItemText(hTempItem);
			if (stringTemp == stringText)
			{
				MessageBox(_T("此名字已存在!!"), _T("Error!!"), MB_OK | MB_ICONWARNING);
				EditLabel(pTVDispInfo->item.hItem);
				KG_PROCESS_ERROR(false);
			}
			hTempItem = GetPrevSiblingItem(hTempItem);
		}
		hTempItem = GetNextSiblingItem(pTVDispInfo->item.hItem);
		while (hTempItem)
		{
			CString stringTemp = GetItemText(hTempItem);
			if (stringTemp == stringText)
			{
				MessageBox(_T("此名字已存在!!"), _T("Error!!"), MB_OK | MB_ICONWARNING);
				EditLabel(pTVDispInfo->item.hItem);
				KG_PROCESS_ERROR(false);
			}
			hTempItem = GetNextSiblingItem(hTempItem);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if (stringText.GetLength() > 32)
	{
		::MessageBox(
			m_hWnd, _T("名字过长!!"), _T("Error"), MB_OK | MB_ICONERROR
		);
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	if (nItemData < 1)
	{
		int  nIndex = nItemData * - 1;
		//int  nCols  = m_pnTypeRow[nIndex];
		int  nFirstRow = GetCollectivityFirstPos(hSelItem);
		int  nLastRow  = GetCollectivityLastPos(hSelItem);
		int  i = 0;
		for (i = nFirstRow; i <= nLastRow; i++)
		{
			nRetCode = (*m_ppTabFile)->WriteString(
				i, m_ppszClassifyFields[nIndex], stringText.GetBuffer() //pTVDispInfo->item.pszText
			);
		}

	}
	else if (nItemData == SUBSET_TREE_DATA)
	{
		char szSectionName[256];
		int  nKeyValue = 0;
		*szSectionName = '\0';
		while ((*m_ppQuestIniFile)->GetNextSection(szSectionName, szSectionName))
		{
			(*m_ppQuestIniFile)->GetInteger(
				szSectionName, _T("TreeHandle"), 0, &nKeyValue
			);
			if (nKeyValue == (int)(DWORD_PTR)hSelItem)
			{
				break;
			}
		}
		if (*szSectionName != '\0')
		{
			nRetCode = (*m_ppQuestIniFile)->WriteString(
				szSectionName, _T("Name"), stringText.GetBuffer() //pTVDispInfo->item.pszText
			);
			KG_PROCESS_ERROR(nRetCode);
		}
	}
	else if (nItemData >= 1)
	{
		nRetCode = (*m_ppTabFile)->WriteString(
			nRow, m_pszCaptionField, stringText.GetBuffer() //pTVDispInfo->item.pszText
		);
		KG_PROCESS_ERROR(nRetCode);
		((KGObjectPropertyEditTabDlg*)GetParent())->m_listProperty.UpdateProperty(nRow);
		((KGObjectPropertyEditTabDlg*)GetParent())->m_nBeSavedFlag = false;
	}
	else
	{
	}

	SetItemText(hSelItem, stringText.GetBuffer()/* pTVDispInfo->item.pszText*/);
	nResult = true;
Exit0:
	*pResult = 0;
}
