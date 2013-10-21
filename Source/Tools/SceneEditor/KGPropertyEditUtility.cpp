////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertyEditUtilityFunctions.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-3-17 17:21:26
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGPropertyEditUtility.h"
#include "KGPropertyEditTabDlg.h"
#include "algorithm"
#include "shlwapi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KGPrimaryKeyManager::KGPrimaryKeyManager()
{
	ASSERT(Refrush(1, 2));
}

KGPrimaryKeyManager::KGPrimaryKeyManager(int nMinKey, int nMaxKey)
{
	ASSERT(Refrush(nMinKey, nMaxKey));
}

KGPrimaryKeyManager::~KGPrimaryKeyManager()
{
	m_vectorKeys.clear();
}

int KGPrimaryKeyManager::Refrush(
	int nMinKey, int nMaxKey, CProgressCtrl* pProgress, CStatusBar* pStatusBar
)
{
	int nResult    = false;
	int nRetCode   = false;
	int nKeysCount = 0;
	int nKey       = 0;
	int nPos	   = 0;
	int nStep      = 0;
	int nCount	   = 0;

	KG_PROCESS_ERROR(nMaxKey > nMinKey);
	KG_PROCESS_ERROR(nMaxKey < MAX_KEY);
	KG_PROCESS_ERROR(nMinKey > MIN_KEY);

	m_nMaxKey = nMaxKey;
	m_nMinKey = nMinKey;

	nKeysCount = nMaxKey - nMinKey + 1;

	m_vectorKeys.clear();
	m_vectorKeys.resize(nKeysCount + FREE_SIZE);

	if (pProgress)
	{
		pProgress->SetRange32(1, PROESS_SETP);
		pProgress->SetPos(1);
		nStep = (int)((nKeysCount / (float)PROESS_SETP) + 0.5f);
		if (!nStep)
			nStep = 1;
	}
	/*nS = timeGetTime();*/
	for (nKey = nMaxKey; nKey >= nMinKey; nKey--, nCount++)
	{
		m_vectorKeys.push_back(nKey);
		if (pProgress && !(nCount % nStep))
		{
			pProgress->SetPos(++nPos);
		}
		/*if (pStatusBar)
		{
			char szText[128];
			sprintf(szText, "%s%d", _T("正在构造任务ID管理器->ID: "), nPos);
			pStatusBar->SetPaneText(0, szText);
		}*/
	}
	//nE = timeGetTime();
	//nT = (nE - nS) / 1000;
	//{
	//	char ms[32];
	//	sprintf(ms, "%d", nT);
	//		::MessageBox(::GetActiveWindow(), ms, _T("message"), MB_OK);
	//}

	if (pProgress)
	{
		pProgress->SetPos(PROESS_SETP);
		pProgress->SetPos(1);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGPrimaryKeyManager::GetKey(int *pnKey)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(pnKey);
	if (m_vectorKeys.empty())
	{
		::MessageBox(::GetActiveWindow(), _T("Not Have Usable PrimaryKey!!"), _T("Error!!"), MB_OK);
		KG_PROCESS_ERROR(false);
	}

	*pnKey = m_vectorKeys.back();

	m_vectorKeys.pop_back();

	nResult = true;
Exit0:
	return nResult;
}

int KGPrimaryKeyManager::AddKey(int nKey, int nCheckFlag)
{
	int nResult  = false;
	int nRetCode = false;

	vector<int>::iterator itorKey;

	KG_PROCESS_ERROR(nKey <= m_nMaxKey);
	KG_PROCESS_ERROR(nKey >= m_nMinKey);

	if (nCheckFlag)
	{
		/*itorKey = find(m_vectorKeys.begin(), m_vectorKeys.end(), nKey);
		KG_PROCESS_ERROR(itorKey == m_vectorKeys.end());*/
		int nPos = m_vectorKeys.find(nKey);
		KG_PROCESS_ERROR(nPos == -1);
	}

	m_vectorKeys.push_back(nKey);

	nResult = true;
Exit0:
	return nResult;
}

int KGPrimaryKeyManager::DelKey(int nKey)
{
	int nResult  = false;
	int nRetCode = false;

	/*vector<int>::iterator itorKey;

	KG_PROCESS_ERROR(nKey <= m_nMaxKey);
	KG_PROCESS_ERROR(nKey >= m_nMinKey);

	itorKey = find(m_vectorKeys.begin(), m_vectorKeys.end(), nKey);
	KG_PROCESS_ERROR(itorKey != m_vectorKeys.end());

	m_vectorKeys.erase(itorKey);*/

	//
	KG_PROCESS_ERROR(nKey <= m_nMaxKey);
	KG_PROCESS_ERROR(nKey >= m_nMinKey);

	nRetCode = m_vectorKeys.erase(nKey);
	KG_PROCESS_ERROR(nRetCode);

	//

	nResult = true;
Exit0:
	return nResult;
}

int g_IniPrimaryKesManager(
	KGPrimaryKeyManager* pKeyManager, ITabFile* ppTabFile[], int nFileCount,
	const char pcszManagerKey[], CProgressCtrl* pPrpgress
)
{
	int nResult  = false;
	int nRetCode = false;

	int nRowsCount  = 0;
	int nColsCount  = 0;
	int nCurRow     = 0;
	int nPriamryKey = 0;
	int nIndex      = 0;
	int nStep       = 0;
	int nCount	    = 0;
	int nPos        = 0;

	KG_PROCESS_ERROR(pKeyManager);
	KG_PROCESS_ERROR(ppTabFile);

	for (nIndex = 0; nIndex < nFileCount; nIndex++)
	{
		KG_PROCESS_ERROR(ppTabFile[nIndex]);
	}

	for (nIndex = 0; nIndex < nFileCount; nIndex++)
	{
		nRowsCount = ppTabFile[nIndex]->GetHeight();
		nColsCount = ppTabFile[nIndex]->GetWidth();

		nRetCode = ppTabFile[nIndex]->FindColumn(pcszManagerKey);
		KG_PROCESS_ERROR(nRetCode);


		if (pPrpgress)
		{
			/*pPrpgress->SetRange32(1, nRowsCount);
			pPrpgress->SetPos(1);*/
			pPrpgress->SetRange32(1, PROESS_SETP);
			pPrpgress->SetPos(1);
			nStep = (int)(((nRowsCount - 2) / (float)PROESS_SETP) + 0.5f);
			if (!nStep)
				nStep = 1;
		}

		if (nRowsCount <= 2)
		{
			continue;
		}

		for (nCurRow = 3; nCurRow <= nRowsCount; nCurRow++)
		{
			ppTabFile[nIndex]->GetInteger(
				nCurRow, pcszManagerKey, 0, &nPriamryKey
			);
			pKeyManager->DelKey(nPriamryKey);
			if (pPrpgress)
			{
				/*pPrpgress->SetPos(nCurRow);*/
				if (pPrpgress && !((nCurRow - 2) % nStep))
				{
					pPrpgress->SetPos(++nPos);
				}
			}
		}

		if (pPrpgress)
		{
			pPrpgress->SetPos(PROESS_SETP);
			pPrpgress->SetPos(1);
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int g_UniteTabFile(ITabFile** ppUnitFile, ITabFile** pImportFile, int nUniteCount)
{
	int nResult  = false;
	int nRetCode = false;

	char szFieldName[64];
	*szFieldName = '\0';

	int nRowsCount = 0;
	int nCurRow	   = 0;
	int nColsCount = 0;
	int nCurCols   = 0;
	int nIndex     = 0;


	KGLOG_PROCESS_ERROR(ppUnitFile);
	SAFE_RELEASE(*ppUnitFile);
	(*ppUnitFile) = g_CreateTabFile();
	KGLOG_PROCESS_ERROR(*ppUnitFile);

	for (nIndex = 0; nIndex < nUniteCount; nIndex++)
	{
		KGLOG_PROCESS_ERROR(pImportFile[nIndex]);
	}

	nRowsCount = (*pImportFile)->GetHeight();

	for (nIndex = 0; nIndex < nUniteCount; nIndex++)
	{
		nColsCount = pImportFile[nIndex]->GetWidth();
		for (nCurCols = 1; nCurCols <= nColsCount; nCurCols++)
		{
			strncpy(
				szFieldName, pImportFile[nIndex]->GetColName(nCurCols),
				sizeof(szFieldName)
			);
			KGLOG_PROCESS_ERROR(*szFieldName != '\0');
			nRetCode = (*ppUnitFile)->FindColumn(szFieldName);
			if (nRetCode == -1)
				(*ppUnitFile)->InsertNewCol(szFieldName);

		}
	}

	nColsCount   = (*ppUnitFile)->GetWidth();
	*szFieldName = '\0';
	for (nCurCols = 1; nCurCols <= nColsCount; nCurCols++)
	{
		strncpy(
			szFieldName, (*ppUnitFile)->GetColName(nCurCols),
			sizeof(szFieldName)
		);
		for (nIndex = 0; nIndex < nUniteCount; nIndex++)
		{
			nRetCode = pImportFile[nIndex]->FindColumn(szFieldName);
			if (nRetCode == -1)
				continue;

			nRowsCount = pImportFile[nIndex]->GetHeight();
			for (nCurRow = 1; nCurRow <= nRowsCount; nCurRow++)
			{
				char szFieldValue[1024];
				*szFieldValue = '\0';

				pImportFile[nIndex]->GetString(
					nCurRow, szFieldName, _T(""), szFieldValue, sizeof(szFieldValue)
				);
				(*ppUnitFile)->WriteString(nCurRow, szFieldName, szFieldValue);
			}
			break;
		}
	}

	nResult = true;
Exit0:
	return nResult;

}

int g_UnUniteTabFile(ITabFile* pUnitFile, ITabFile** ppExportFile, int nExportCount)
{
	int nResult  = false;
	int nRetCode = false;

	int nFileRowsCount = 0;
	int nFileCurRow    = 0;
	int nFileColsCount = 0;
	int nFileCurCol    = 0;
	int nExportFileRow = 0;
	int nIndex	       = 0;

	char szFieldValue[1024];
	char szFieldName[64];
	*szFieldValue = '\0';
	*szFieldName  = '\0';

	KGLOG_PROCESS_ERROR(ppExportFile);
	KGLOG_PROCESS_ERROR(pUnitFile);
	for (nIndex = 0; nIndex < nExportCount; nIndex++)
	{
		KGLOG_PROCESS_ERROR(ppExportFile[nIndex]);
	}

	for (nIndex = 0; nIndex < nExportCount; nIndex++)
	{
		while (ppExportFile[nIndex]->GetHeight() > 2)
		{
			ppExportFile[nIndex]->Remove(3);
		}
	}

	nFileRowsCount = pUnitFile->GetHeight();
	nFileColsCount = pUnitFile->GetWidth();

	for (nFileCurCol = 1; nFileCurCol <= nFileColsCount; nFileCurCol++)
	{
		strncpy(
			szFieldName, pUnitFile->GetColName(nFileCurCol), sizeof(szFieldName)
		);
		for (nFileCurRow = 2; nFileCurRow <= nFileRowsCount; nFileCurRow++)
		{
			pUnitFile->GetString(
				nFileCurRow, szFieldName, _T(""), szFieldValue, sizeof(szFieldValue)
			);
			for (nIndex = 0; nIndex < nExportCount; nIndex++)
			{
				nRetCode = ppExportFile[nIndex]->FindColumn(szFieldName);
				if (nRetCode < 1)
					continue;
				ppExportFile[nIndex]->WriteString(nFileCurRow, szFieldName, szFieldValue);
			}
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int g_GetPosByPrimaryKey(ITabFile* pTabFile, const char szPrimaryKeyField[], int nKey, int nFlag)
{
	int nResult  = false;
	int nRetCode = false;

	int nRowsCount  = 0;
	int nRow        = 0;
	int nPrimaryKey = 0;

	KGLOG_PROCESS_ERROR(pTabFile);
	KGLOG_PROCESS_ERROR(szPrimaryKeyField);

	nRowsCount = pTabFile->GetHeight();

	for (nRow = 2; nRow <= nRowsCount; nRow++)
	{
		nRetCode = pTabFile->GetInteger(nRow, szPrimaryKeyField, 0, &nPrimaryKey);
		if (nPrimaryKey == nKey)
		{
			nResult = nRow;
			KG_PROCESS_SUCCESS(true);
		}
	}

	KG_PROCESS_SUCCESS(nFlag);
	for (nRow = 2; nRow <= nRowsCount; nRow++)
	{
		nRetCode = pTabFile->GetInteger(nRow, szPrimaryKeyField, 0, &nPrimaryKey);
		if (nPrimaryKey > nKey)
		{
			nResult = nRow;
			KG_PROCESS_SUCCESS(true);
		}
	}

	nResult = nRowsCount + 1;

Exit1:
Exit0:
	return nResult;
}

int g_CopyOneRow(ITabFile* pTabFIle, int nSourRow, int nDestRow)
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
	KG_PROCESS_ERROR(pTabFIle);

	nRowsNumber = pTabFIle->GetHeight();
	nColsNumber = pTabFIle->GetWidth();

	KG_PROCESS_ERROR(nSourRow <= nRowsNumber);
	KG_PROCESS_ERROR(nDestRow <= nRowsNumber + 1);

	for (i = 1; i <= nColsNumber; i++)
	{
		nRetCode = pTabFIle->GetString(
			nSourRow, i, "", szValue, sizeof(szValue)
		);
		nRetCode = pTabFIle->WriteString(nDestRow, i, szValue);
	}

	nResult = true;
Exit0:
	return nResult;
}

int BusyCursor(CWnd* pWnd)
{
	//HCURSOR hCr = LoadCursor(NULL, IDC_SIZE);
	//SetClassLongPtr(pWnd->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)hCr);
	ShowCursor(FALSE);
	return true;
}

int FreeCursor(CWnd* pWnd)
{
	//HCURSOR hCr = LoadCursor(NULL, IDC_ARROW);
	//SetClassLongPtr(pWnd->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)hCr);
	ShowCursor(TRUE);
	return true;
}

int operation_tree_item(
	CTreeCtrl* pTree, HTREEITEM hTreeItem, TreeOpFun operator_fun, int nFirstCallFlag
)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(pTree);
	KG_PROCESS_ERROR(hTreeItem);

	operator_fun(pTree, hTreeItem);

	operation_tree_item(pTree, pTree->GetChildItem(hTreeItem), operator_fun, false);
	KG_PROCESS_SUCCESS(nFirstCallFlag);
	operation_tree_item(pTree, pTree->GetNextSiblingItem(hTreeItem), operator_fun, false);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int FindInSubset(IIniFile* pIniFile, CString szPrimaryKey, CString& szReturnSectionName)
{
	int nResult  = false;
	int nRetCode = false;

	char szSectionName[SECTION_NAME_SIZE];
	*szSectionName = '\0';

	KG_PROCESS_ERROR(pIniFile);

	nRetCode = pIniFile->GetNextSection(szSectionName, szSectionName);
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
		nRetCode = pIniFile->GetNextSection(szSectionName, szSectionName);
	}

	KG_PROCESS_ERROR(nRetCode);

	szReturnSectionName = szSectionName;

	nResult = true;
Exit0:
	return nResult;
}

int BuildPrepertyFile(
	ITabFile* pTabFile, IIniFile* pIniFile, const char pcszSectionName[],
	int* pnInsertNumber, CProgressCtrl* pProCtrl
)
{
	int nResult  = false;
	int nRetCode = false;

//	char szPropertyValue[128];
	char szKeyValue[128];
	char szKeyName[128];
	char szDependValue[128];
	char szDependName[128];

	*szKeyValue     = '\0';
	*szKeyName   	= '\0';
	*szDependValue  = '\0';
	*szDependName   = '\0';

	KG_PROCESS_ERROR(pcszSectionName);
	KG_PROCESS_ERROR(pTabFile);
	KG_PROCESS_ERROR(pIniFile);

	if (KGPropertyListCtrl::IsClassifySection(pIniFile, pcszSectionName))
	{

		while (
			pIniFile->GetNextKey(pcszSectionName, szKeyName, szKeyName)
		)
		{
			pIniFile->GetString(
				pcszSectionName, szKeyName, _T(""), szKeyValue, sizeof(szKeyValue)
			);
			if (
				(CString(szKeyName) != _T("Depend_Name")) &&
				(CString(szKeyName) != _T("Depend_Value"))
			)
			{
				BuildPrepertyFile(
					pTabFile, pIniFile, szKeyValue, pnInsertNumber
				);
			}
		}
	}
	else
	{
		char szPropertyValueType[128];
		char szTempValue[128];
		ITEM_VILUE_TYPE eItemValueType;

		*szPropertyValueType ='\0';
		*szTempValue = '\0';

		nRetCode = pIniFile->GetString(
			pcszSectionName, _T("Type"), _T(""), szPropertyValueType,
			sizeof(szPropertyValueType)
		);

		eItemValueType = KGPropertyListCtrl::GetKeyType(szPropertyValueType);

		switch (eItemValueType)
		{
		case VALUE_STRING :
		case VALUE_FLOAT  :
		case VALUE_COLOR  :
		case VALUE_INT    :
		case VALUE_BOOL   :
		case VALUE_TYPE   :
		case VALUE_FORCE_ID :
		case VALUE_COOLDOWN_ID :
		case VALUE_STATE_ID :
		case VALUE_NPC_ID :
		case VALUE_NPC_TEMPLATE :
			nRetCode = pTabFile->InsertNewCol(pcszSectionName);
			KG_PROCESS_ERROR(nRetCode);
			nRetCode = pIniFile->GetString(
				pcszSectionName,_T("Def"), _T(""), szTempValue, sizeof(szTempValue)
			);
			if (nRetCode)
			{
				nRetCode = pTabFile->WriteString(2, pcszSectionName, szTempValue);
				KG_PROCESS_ERROR(nRetCode);
			}
			*pnInsertNumber++;
			break;
		case VALUE_QUEST_MAPNPC :
		case VALUE_MIN_MAX      :
		case VALUE_DITEM      :
		case VALUE_ITEM       :
		case VALUE_FORCE_IV   :
		case VALUE_STATE_IV   :
		case VALUE_LIVE_IV    :
		case VALUE_SKILL_IV   :
		case VALUE_NPC_ID_NUM :
		case VALUE_ITEM_TID_IID :
		case VALUE_ITEM_TID_IID_NUM :
		case VALUE_GOSSIP :
		case VALUE_PREQUEDT_ID :
		case VALUE_SPRING_DATE :
			nRetCode = pIniFile->GetString(
				pcszSectionName,_T("PARAM_1"), _T(""), szTempValue, sizeof(szTempValue)
			);
			nRetCode = pTabFile->InsertNewCol(szTempValue);
			KG_PROCESS_ERROR(nRetCode);

			nRetCode = pIniFile->GetString(
				pcszSectionName,_T("PARAM_2"), _T(""), szTempValue, sizeof(szTempValue)
			);
			nRetCode = pTabFile->InsertNewCol(szTempValue);
			KG_PROCESS_ERROR(nRetCode);

			*pnInsertNumber += 2;

			if (eItemValueType == VALUE_ITEM_TID_IID_NUM)
			{
				nRetCode = pIniFile->GetString(
					pcszSectionName,_T("PARAM_3"), _T(""),
					szTempValue, sizeof(szTempValue)
				);
				nRetCode = pTabFile->InsertNewCol(szTempValue);
				KG_PROCESS_ERROR(nRetCode);
				*pnInsertNumber++;
			}
			if (eItemValueType == VALUE_GOSSIP || eItemValueType == VALUE_PREQUEDT_ID || eItemValueType == VALUE_SPRING_DATE)
			{
				nRetCode = pIniFile->GetString(
					pcszSectionName,_T("PARAM_3"), _T(""),
					szTempValue, sizeof(szTempValue)
				);
				nRetCode = pTabFile->InsertNewCol(szTempValue);
				KG_PROCESS_ERROR(nRetCode);
				nRetCode = pIniFile->GetString(
					pcszSectionName,_T("PARAM_4"), _T(""),
					szTempValue, sizeof(szTempValue)
				);
				nRetCode = pTabFile->InsertNewCol(szTempValue);
				KG_PROCESS_ERROR(nRetCode);
				nRetCode = pIniFile->GetString(
					pcszSectionName,_T("PARAM_5"), _T(""),
					szTempValue, sizeof(szTempValue)
				);
				nRetCode = pTabFile->InsertNewCol(szTempValue);
				KG_PROCESS_ERROR(nRetCode);
				*pnInsertNumber += 3;
			}
			if (eItemValueType == VALUE_SPRING_DATE)
			{
				nRetCode = pIniFile->GetString(
					pcszSectionName,_T("PARAM_6"), _T(""),
					szTempValue, sizeof(szTempValue)
				);
				nRetCode = pTabFile->InsertNewCol(szTempValue);
				KG_PROCESS_ERROR(nRetCode);
				*pnInsertNumber += 1;
			}
			break;
		default :
			nRetCode = pTabFile->InsertNewCol(pcszSectionName);
			KG_PROCESS_ERROR(nRetCode);
			nRetCode = pIniFile->GetString(
				pcszSectionName,_T("Def"), _T(""), szTempValue, sizeof(szTempValue)
				);
			if (nRetCode)
			{
				nRetCode = pTabFile->WriteString(2, pcszSectionName, szTempValue);
				KG_PROCESS_ERROR(nRetCode);
			}
			*pnInsertNumber++;
			break;
		}
		if (pProCtrl)
		{
			pProCtrl->SetPos(*pnInsertNumber);
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int AutoFillTable(
	ITabFile* pTabFile, int nRow, IIniFile* pRuleTable, LPCSTR szLeadPath, LPCSTR szPrefix
)
{
	int nResult  = false;
	int nRetCode = false;

	char szFullPath[MAX_PATH];
	char szRelaPath[MAX_PATH];
	char szKeyName[128];
	char szKeyDate[128];

	*szFullPath = '\0';
	*szRelaPath = '\0';
	*szKeyName  = '\0';
	*szKeyDate  = '\0';

	KG_PROCESS_ERROR(pTabFile);
	KG_PROCESS_ERROR(pRuleTable);

	nRetCode = ::PathFileExists(szLeadPath);
	KG_PROCESS_ERROR(nRetCode);
	KG_PROCESS_ERROR(*szPrefix != '\0');
	KG_PROCESS_ERROR(nRow >= 2);

	while (pRuleTable->GetNextKey(TEXT("Rule"), szKeyName, szKeyName))
	{
		pRuleTable->GetString(TEXT("Rule"), szKeyName, TEXT(""), szKeyDate, sizeof(szKeyDate));
		if (*szKeyDate != '\0')
		{
			sprintf(szFullPath, TEXT("%s%s%s"), szLeadPath, szPrefix, szKeyDate);
			if (!::PathFileExists(szFullPath))
				continue;
			g_GetFilePathFromFullPath(szRelaPath, szFullPath);
			pTabFile->WriteString(nRow, szKeyName, szRelaPath);
		}
	}

	nResult = true;
Exit0:
	return nResult;
}