////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGObjectEditerTreeCtrl.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-4-20 18:37:05
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KGObjectEditerTreeCtrl.h"
#include "KG3DObjectEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


KGObjectEditerTreeCtrl::KGObjectEditerTreeCtrl()
{
	m_pSubEditer = NULL;
}

KGObjectEditerTreeCtrl::~KGObjectEditerTreeCtrl()
{
}

int KGObjectEditerTreeCtrl::BeginStick(HTREEITEM hSour, HTREEITEM hDest, MOVE_TYPE eCoypType)
{
	int nResult  = false;
	int nRetCode = false;

	int nDestData  = 0;
	int nSourData  = 0;

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

	nResult = true;
Exit0:
	if (!nResult)
	{
		::MessageBox(
			this->m_hWnd, _T("不能粘贴\t\t"), _T("粘贴出错!!"),
			MB_OK | MB_ICONERROR
		);
	}
	return nResult;
}

HTREEITEM KGObjectEditerTreeCtrl::GetItemByPrimaryKey(
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

int KGObjectEditerTreeCtrl::AfterCopyItem(
	HTREEITEM hCopyItem, HTREEITEM hNewItem, HTREEITEM hParentItem, HTREEITEM hAfterItem
)
{
	int nResult  = false;
	int nRetCode = false;

	char szPrimaryKeyField[64];
	*szPrimaryKeyField = '\0';

	int nItemData    = 0;
	int nSourItemPos = 0;
	int nDestItemPos = 0;
	int nRowNumber   = 0;

	KG_PROCESS_ERROR(hCopyItem);
	KG_PROCESS_ERROR(hNewItem);

	nRowNumber = m_pSubEditer->m_pGameLogicTabFile->GetHeight();
	KG_PROCESS_ERROR(nRowNumber >= 2);

	nRetCode = m_pSubEditer->m_pGameLogicIniFile->GetString(
		_T("Main"), _T("PrimaryKey_Field"), _T(""),
		szPrimaryKeyField, sizeof(szPrimaryKeyField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nItemData = (int)GetItemData(hCopyItem);
	KG_PROCESS_SUCCESS(nItemData  < 1);

	nSourItemPos = g_GetPosByPrimaryKey(
		m_pSubEditer->m_pGameLogicTabFile, szPrimaryKeyField, nItemData
	);

	nRetCode = m_pSubEditer->m_primaryKeyManager_L.GetKey(&nItemData);
	KGLOG_PROCESS_ERROR(nRetCode);

	nDestItemPos = g_GetPosByPrimaryKey(
		m_pSubEditer->m_pGameLogicTabFile, szPrimaryKeyField, nItemData,
		false
	);

	if (nDestItemPos <= nSourItemPos)
	{
		nSourItemPos++;
	}

	m_pSubEditer->m_pGameLogicTabFile->InsertAfter(nDestItemPos - 1);
	nRetCode = g_CopyOneRow(m_pSubEditer->m_pGameLogicTabFile, nSourItemPos, nDestItemPos);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pSubEditer->m_pGameLogicTabFile->WriteInteger(
		nDestItemPos, szPrimaryKeyField, nItemData
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode  = SetItemData(hNewItem, nItemData);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = RefrushClassInfo(hNewItem);
	KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGObjectEditerTreeCtrl::BeginDelItem(HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hTempItem   = NULL;

	char szPrimaryKeyField[64];
	char szItemData[64];
	*szPrimaryKeyField = '\0';
	*szItemData = '\0';

	int nRepresentId = 0;
	int nRowsCount   = 0;
	int nCurRow		 = 0;
	int nItemData    = 0;
	int nItemPos_L   = 0;
	int nItemPos_R	 = 0;

	KG_PROCESS_ERROR(hTreeItem);

	nItemData = (int)GetItemData(hTreeItem);
	itoa(nItemData, szItemData, 10);

	nRetCode = m_pSubEditer->m_pGameLogicIniFile->GetString(
		_T("Main"), _T("PrimaryKey_Field"), _T(""),
		szPrimaryKeyField, sizeof(szPrimaryKeyField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRowsCount = m_pSubEditer->m_pGameLogicTabFile->GetHeight();
	if (nItemData >= 1)
	{
		nItemPos_L = g_GetPosByPrimaryKey(
			m_pSubEditer->m_pGameLogicTabFile, szPrimaryKeyField, nItemData
		);

		// 2006-4-20 19:16 //////////////////////////////////////
		nRetCode = m_pSubEditer->m_pGameLogicTabFile->GetInteger(
			nItemPos_L, _T("RepresentID"), 0, &nRepresentId
		);

		for (nCurRow = 3; nCurRow <= nRowsCount; nCurRow++)
		{
			int nIdTemp = 0;
			m_pSubEditer->m_pGameLogicTabFile->GetInteger(
				nCurRow, _T("RepresentID"), 0, &nIdTemp
			);
			if (nIdTemp == nRepresentId && nCurRow != nItemPos_L)
				break;
		}

		if (nCurRow > nRowsCount)
		{
			CString stringWaring = _T("");
			char szName[64];
			m_pSubEditer->m_pGameLogicTabFile->GetString(
				nItemPos_L, _T("Name"), _T(""), szName, sizeof(szName)
			);
			stringWaring.Format(
				_T("是否同时删除 [%s] 关联的表现逻辑表中的项 [RepresentID:%d] ?1"),
				szName, nRepresentId
			);
			nRetCode = MessageBox(
				stringWaring, _T("Alert"),MB_YESNO | MB_ICONWARNING
			);
			if (nRetCode == IDYES)
			{
				nItemPos_R = g_GetPosByPrimaryKey(
					m_pSubEditer->m_pRepresentTabFile,_T("RepresentID"), nRepresentId
				);
				m_pSubEditer->m_primaryKeyManager_R.AddKey(nItemData);
				m_pSubEditer->m_pRepresentTabFile->Remove(nItemPos_R);
			}
		}

		//////////////////////////////////////////////////////////////

		m_pSubEditer->m_primaryKeyManager_L.AddKey(nItemData);
		m_pSubEditer->m_pGameLogicTabFile->Remove(nItemPos_L);
		m_pSubEditer->m_pClassifyFile->EraseSection(szItemData);
	}
	else
	{
		DelRowInTabFile(GetChildItem(hTreeItem));
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGObjectEditerTreeCtrl::DelRowInTabFile(HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	char szPrimaryKeyField[64];
	char szItemData[64];
	*szPrimaryKeyField = '\0';
	*szItemData = '\0';

	int nRepresentId = 0;
	int nRowsCount   = 0;
	int nCurRow		 = 0;
	int nItemData    = 0;
	int nItemPos_L   = 0;
	int nItemPos_R	 = 0;

	KG_PROCESS_ERROR(hTreeItem);

	nItemData = (int)GetItemData(hTreeItem);
	itoa(nItemData, szItemData, 10);

	nRetCode = m_pSubEditer->m_pGameLogicIniFile->GetString(
		_T("Main"), _T("PrimaryKey_Field"), _T(""),
		szPrimaryKeyField, sizeof(szPrimaryKeyField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	if (nItemData >= 1)
	{
		nItemPos_L = g_GetPosByPrimaryKey(
			m_pSubEditer->m_pGameLogicTabFile, szPrimaryKeyField, nItemData
		);

		// 2006-4-20 19:16 //////////////////////////////////////
		nRetCode = m_pSubEditer->m_pGameLogicTabFile->GetInteger(
			nItemPos_L, _T("RepresentID"), 0, &nRepresentId
		);

		for (nCurRow = 3; nCurRow <= nRowsCount; nCurRow++)
		{
			int nIdTemp = 0;
			m_pSubEditer->m_pGameLogicTabFile->GetInteger(
				nCurRow, _T("RepresentID"), 0, &nIdTemp
			);
			if (nIdTemp == nRepresentId && nCurRow != nItemPos_L)
				break;
		}

		if (nCurRow > nRowsCount)
		{
			CString stringWaring = _T("");
			char szName[64];
			m_pSubEditer->m_pGameLogicTabFile->GetString(
				nItemPos_L, _T("Name"), _T(""), szName, sizeof(szName)
			);
			stringWaring.Format(
				_T("是否同时删除 [%s] 关联的表现逻辑表中的项 [RepresentID:%d] ?1"),
				szName, nRepresentId
			);
			nRetCode = MessageBox(
				stringWaring, _T("Alert"),MB_YESNO | MB_ICONWARNING
			);
			if (nRetCode == IDYES)
			{
				nItemPos_R = g_GetPosByPrimaryKey(
					m_pSubEditer->m_pRepresentTabFile,_T("RepresentID"), nRepresentId
				);
				m_pSubEditer->m_primaryKeyManager_R.AddKey(nItemData);
				m_pSubEditer->m_pRepresentTabFile->Remove(nItemPos_R);
			}
		}

		//////////////////////////////////////////////////////////////

		m_pSubEditer->m_primaryKeyManager_L.AddKey(nItemData);
		m_pSubEditer->m_pGameLogicTabFile->Remove(nItemPos_L);
		m_pSubEditer->m_pClassifyFile->EraseSection(szItemData);
	}
	DelRowInTabFile(GetChildItem(hTreeItem));
	DelRowInTabFile(GetNextSiblingItem(hTreeItem));

	nResult = true;
Exit0:
	return nResult;
}

int KGObjectEditerTreeCtrl::RefrushClassInfo(HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	CString szItemPath = _T("");
	char szItemData[64];
	*szItemData = '\0';
	int nItemData = 0;

	KGLOG_PROCESS_ERROR(hTreeItem);

	nItemData = (int)GetItemData(hTreeItem);
	//itoa(dwItemData, szItemData, 10);
	sprintf_s(szItemData, "%d", nItemData);
	hTreeItem   = GetParentItem(hTreeItem);
	while(hTreeItem)
	{
		if (szItemPath != _T(""))
			szItemPath = GetItemText(hTreeItem) + _T("-") + szItemPath;
		else
			szItemPath = GetItemText(hTreeItem);
		hTreeItem   = GetParentItem(hTreeItem);
	}

	nRetCode = m_pSubEditer->m_pClassifyFile->WriteString(
		szItemData, _T("Class"), szItemPath
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

HTREEITEM KGObjectEditerTreeCtrl::GetInsertNode(HTREEITEM hStartItem, CString szClassPath)
{
	HTREEITEM hResult   = NULL;
	HTREEITEM hRetCode  = NULL;

	CString szNodeText  = _T("");
	CString szItemText  = _T("");
	CString szNextPath  = _T("");

	int nItemData = 0;
	int nPos  = 0;

	szNodeText = szClassPath.Tokenize(_T("-"), nPos);
	KG_PROCESS_ERROR(nPos != -1);
	szNextPath = szClassPath.Right(szClassPath.GetLength() - nPos);

	hResult  = hStartItem;
	hRetCode = hStartItem;

	if (hResult)
	{
		nItemData = (int)GetItemData(hResult);
		if (nItemData >= 1)
		{
			hRetCode = GetParentItem(hStartItem);
			hResult  = NULL;
		}
	}

	if (hResult)
		hRetCode = GetParentItem(hRetCode);
	while (hResult)
	{
		szItemText = GetItemText(hResult);
		if (szNodeText == szItemText)
			break;
		hResult = GetNextSiblingItem(hResult);
	}

	if (!hResult)
	{
		hResult = hRetCode;
		while (nPos != -1)
		{
			if (hResult)
				nItemData = (int)GetItemData(hResult) - 1;
			else
				nItemData = 0;
			hResult   = InsertItem(szNodeText, 0, 0, hResult);
			SetItemData(hResult, nItemData);
			szNodeText = szClassPath.Tokenize(_T("-"), nPos);
		}
	}
	else if (szNextPath != _T(""))
	{
		hResult = GetInsertNode(GetChildItem(hResult), szNextPath);
	}
	else
	{
		KG_PROCESS_SUCCESS(true);
	}

Exit1:
Exit0:
	return hResult;
}

int  KGObjectEditerTreeCtrl::RefrushClassInfoEx(CTreeCtrl* pTree, HTREEITEM hTreeItem)
{
	int nResult   = false;
	int nRetCode  = false;

	int nItemData = 0;

	KG_PROCESS_ERROR(hTreeItem);
	KG_PROCESS_ERROR(pTree);

	nItemData = (int)pTree->GetItemData(hTreeItem);
	KG_PROCESS_ERROR(nItemData >= 1);

	((KGObjectEditerTreeCtrl*)pTree)->RefrushClassInfo(hTreeItem);

	nResult = true;
Exit0:
	return nResult;
}

void KGObjectEditerTreeCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nResult  = false;
	int nRetCode = false;

	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	HTREEITEM hSelItem = pTVDispInfo->item.hItem;
	int nItemData      = 0;
	int nItemPos       = 0;

	char szPrimaryKeyField[64];
	char szCaptionKeyField[64];

	*szPrimaryKeyField = '\0';
	*szCaptionKeyField = '\0';

	KG_PROCESS_ERROR(pTVDispInfo->item.pszText);
	KG_PROCESS_ERROR(*pTVDispInfo->item.pszText != '\0');

	nItemData = (int)GetItemData(hSelItem);

	nRetCode = m_pSubEditer->m_pGameLogicIniFile->GetString(
		_T("Main"), _T("PrimaryKey_Field"), _T(""),
		szPrimaryKeyField, sizeof(szPrimaryKeyField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pSubEditer->m_pGameLogicIniFile->GetString(
		_T("Main"), _T("Title_Field"), _T(""),
		szCaptionKeyField, sizeof(szCaptionKeyField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nItemPos = g_GetPosByPrimaryKey(
		m_pSubEditer->m_pGameLogicTabFile, szPrimaryKeyField, nItemData
	);

	if (strlen(pTVDispInfo->item.pszText) > 64)
	{
		MessageBox(
			_T("名字过长!!"), _T("Error"), MB_OK | MB_ICONERROR
		);
		KGLOG_PROCESS_ERROR(false);
	}

	SetItemText(hSelItem, pTVDispInfo->item.pszText);

	if (nItemData >= 1)
	{
		nRetCode = m_pSubEditer->m_pGameLogicTabFile->WriteString(
			nItemPos, szCaptionKeyField, pTVDispInfo->item.pszText
		);
		KG_PROCESS_ERROR(nRetCode);

		//((KG3DObjectEditor*)(GetParent()->GetParent()))->m_pPropertyList->UpdateProperty(nItemPos);
	}
	else //(nItemData < 1)
	{
		operation_tree_item(this, hSelItem, RefrushClassInfoEx);
	}

	nResult = true;
Exit0:
	*pResult = 0;
}

int KGObjectEditerTreeCtrl::Update(KGSubEditer* pSubEditer)
{
	int nResult  = false;
	int nRetCode = false;

	char szPrimaryKeyField[64];
	char szTitleField[64];

	*szPrimaryKeyField = '\0';
	*szTitleField      = '\0';

	int  nTabFileRowsCount = 0;
	int  nTabFileCurRow    = 0;

	DeleteAllItems();

	KGLOG_PROCESS_ERROR(pSubEditer);
	m_pSubEditer = pSubEditer;

	nRetCode = m_pSubEditer->m_pGameLogicIniFile->GetString(
		_T("Main"), _T("PrimaryKey_Field"), _T(""),
		szPrimaryKeyField, sizeof(szPrimaryKeyField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pSubEditer->m_pGameLogicIniFile->GetString(
		_T("Main"), _T("Title_Field"), _T(""), szTitleField, sizeof(szTitleField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nTabFileRowsCount = m_pSubEditer->m_pGameLogicTabFile->GetHeight();
	for (nTabFileCurRow = 3; nTabFileCurRow <= nTabFileRowsCount; nTabFileCurRow++)
	{
		HTREEITEM hAfterItem  = NULL;
		HTREEITEM hInsertItem = NULL;

		char szClassPath[MAX_PATH];
		char szTitleText[64];
		char szPrimaryKey[16];

		*szClassPath = '\0';
		*szTitleText = '\0';
		*szPrimaryKey = '\0';

		int  nPrimaryKey = 0;

		nRetCode = m_pSubEditer->m_pGameLogicTabFile->GetInteger(
			nTabFileCurRow, szPrimaryKeyField, 0, &nPrimaryKey
		);
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = m_pSubEditer->m_pGameLogicTabFile->GetString(
			nTabFileCurRow, szTitleField, _T("=No Name="), szTitleText, sizeof(szTitleText)
		);
		itoa(nPrimaryKey, szPrimaryKey, 10);
		nRetCode = m_pSubEditer->m_pClassifyFile->GetString(
			szPrimaryKey, _T("Class"), _T(""), szClassPath, sizeof(szClassPath)
		);
		hAfterItem  = GetInsertNode(GetRootItem(), szClassPath);
		hInsertItem = InsertItem(szTitleText, 2, 2, hAfterItem);
		KGLOG_PROCESS_ERROR(hInsertItem);
		SetItemData(hInsertItem, nPrimaryKey);
	}

	nResult = true;
Exit0:
	return nResult;
}

BEGIN_MESSAGE_MAP(KGObjectEditerTreeCtrl, KGTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &KGObjectEditerTreeCtrl::OnTvnEndlabeledit)
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void KGObjectEditerTreeCtrl::OnPaint()
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hSelItem = GetSelectedItem();
	CClientDC dc(this);
	CRect rectItem;
	CPen pen(PS_SOLID, 2, RGB(255, 2, 2));
	CPen* penOld = NULL;
	KGTreeCtrl::OnPaint();

	KG_PROCESS_ERROR(hSelItem);
	GetItemRect(hSelItem, &rectItem, TRUE);

	penOld = dc.SelectObject(&pen);
	dc.MoveTo(rectItem.left, rectItem.bottom);
	dc.LineTo(rectItem.right, rectItem.bottom);

	dc.SelectObject(penOld);

	pen.DeleteObject();

Exit0:
	return;
}
