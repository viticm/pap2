////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertyListCtrl.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-2-7 15:49:10
//  Comment     : 2006-2-24 Update To V2.0
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGPropertyListctrl.h"
#include "KGValueEditDlg.h"
#include "KGValueEditStringDlg.h"
#include "KGValueEditMinMaxDlg.h"
#include "KGPropertyEditTabDlg.h"
#include "KGValueEditBoolDlg.h"
#include "KGValueEditQuestTypeDlg.h"
#include "KGValueEditGossipDlg.h"
#include "KGValueEditForceDlg.h"
#include "KGProperytEditFinderDlg.h"
#include "KGValueEidtPrequestIDsDlg.h"
#include "KGValueEditFloatDlg.h"
#include "KColorDialog.h"
#include "KGValudEditDateTimeDlg.h"
#include "KGObjectEditerProjectListDlg.h"
#include "KG3DFileViewDialog.h"
#include "KGValueEdit4PairParamDlg.h"
#include "KGValueEdit4FilePathDlg.h"
#include "KGValueEditNpcSocketDlg.h"
#include "KGValueEditNpcNumDataDlg.h"
#include "KGValueEditNpcAniSoundDlg.h"
#include "KGValueEditNpcTemplate.h"
#include "KGMemDateEditDlg.h"
#include "KGValueEditerIDsDlg.h"
#include "KDlgNPCBindInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KGPropertyListCtrl::KGPropertyListCtrl(void)
{
	m_eEditType    = LIST_EDIT_TYPE_POPUP;
	m_lpCopyItem   = NULL;
	m_ppTabFile    = NULL;
	m_ppIniFile    = NULL;
	m_nEditFlag    = true;
	m_lpSaveItem   = NULL;
	m_eSaveVauleType = VALUE_UNKNOW;
	m_nBandDataFlag = false;
}

KGPropertyListCtrl::~KGPropertyListCtrl(void)
{
}
 
BEGIN_MESSAGE_MAP(KGPropertyListCtrl, KGListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYUP()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_USER_UPDATE_LIST_ITEM, OnUpdateListItem)
	ON_NOTIFY(UDN_DELTAPOS, ID_SP_SHOW, OnDeltaposSpin)
	ON_COMMAND(ID_BN_SHOW, OnBnClick)
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

int  KGPropertyListCtrl::EnableEdit(int nCanEditFlag)
{
	m_nEditFlag = nCanEditFlag;

	return true;
}

int  KGPropertyListCtrl::FindIniSection(CString szSectionName)
{
	int nResult  = false;
	int nRetCode = false;

	char szSection[SECTION_NAME_SIZE] = _T("");

	KG_PROCESS_ERROR(m_ppIniFile);
	KG_PROCESS_ERROR(*m_ppIniFile);

	while ((*m_ppIniFile)->GetNextSection(szSection, szSection))
	{
		KG_PROCESS_SUCCESS(CString(szSection) == szSectionName);
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

LPKGLISTITEM KGPropertyListCtrl::FindItemByPropertyName(
	LPKGLISTITEM pStartItem, CString szPropertyName
)
{
	LPKGLISTITEM   pResult = NULL;
	USER_ITEM_DATA itemDate;

	KG_PROCESS_ERROR(pStartItem);
	
	pStartItem->GetStructData(&itemDate, sizeof(itemDate));

	if (
		(CString(itemDate.szPropertyName) == szPropertyName) ||
		(CString(itemDate.szParamName_1)  == szPropertyName) ||
		(CString(itemDate.szParamName_2)  == szPropertyName) ||
		(CString(itemDate.szParamName_3)  == szPropertyName) ||
		(CString(itemDate.szParamName_4)  == szPropertyName) ||
		(CString(itemDate.szParamName_5)  == szPropertyName) 
	)
	{
		pResult = pStartItem;
	}

	if (!pResult)
	{
		pResult = FindItemByPropertyName(
			pStartItem->pFirstChildItem, szPropertyName
		);
	}

	if (!pResult)
	{
		pResult = FindItemByPropertyName(
			pStartItem->pNexSiblingItem, szPropertyName
		);
	}

Exit0:
	return pResult;

}

int  KGPropertyListCtrl::IsClassifySection(IIniFile* pIniFile, const char pcszSectionName[])
{
	int nResult  = false;
	int nRetCode = false;

	char szKeyName[128] = _T("");

	KG_PROCESS_ERROR(pcszSectionName);
	KG_PROCESS_ERROR(pIniFile);

	nRetCode = pIniFile->GetNextKey(pcszSectionName, szKeyName, szKeyName);
	KG_PROCESS_ERROR(nRetCode);
	szKeyName[5] = '\0';
	KG_PROCESS_ERROR(CString(szKeyName) == "Class");

	nResult = true;
Exit0:
	return nResult;
}

int  KGPropertyListCtrl::BandData(ITabFile** ppTabFile, IIniFile** ppIniFile, DWORD eEditType)
{
	m_eEditType = eEditType;
	m_ppTabFile = ppTabFile;
	m_ppIniFile = ppIniFile;
	
	return true;
}

int  KGPropertyListCtrl::InsertProperty(
	LPKGLISTITEM pDependedItem, char pcszSectionName[], int* pnInsertNumber, int nCurrentRow
)
{
	int  nResult  = false;
	int  nRetCode = false;

	char szPropertyValue[128];
	char szKeyValue[128];
	char szKeyName[128];
	char szDependName[128];
	char szDependValue[128];

	*szPropertyValue = '\0';
	*szKeyValue      = '\0';
	*szKeyName   	 = '\0';
	*szDependValue   = '\0';
	*szDependName    = '\0';

	nRetCode = (*m_ppIniFile)->GetString(
		pcszSectionName, _T("Depend_Name"), _T(""), szDependName, sizeof(szDependName)
	);
	if (nRetCode)
	{
		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName, _T("Depend_Value"), _T(""),szDependValue, sizeof(szDependValue)
		);
		if (nRetCode)
		{
			nRetCode = (*m_ppTabFile)->GetString(
				nCurrentRow, szDependName, _T(""), szPropertyValue, sizeof(szPropertyValue)
			);
			if (CString(szPropertyValue) != _T("其他"))
			{
				KG_PROCESS_ERROR(CString(szPropertyValue) == CString(szDependValue));
			}
		}

	}

	if (IsClassifySection(*m_ppIniFile, pcszSectionName)) 
	{
		LPKGLISTITEM pTitleItem = NULL;

		if (CString(pcszSectionName) != _T("_MainClass_"))
		{
			pTitleItem = new KGLISTITEM;
			ITEMTEXT	 itemText;

			if (!pDependedItem)
			{
				itemText.colorBack     = RGB(92, 92, 92);
				itemText.colorBackSel  = RGB(92, 92, 92);
				itemText.colorFrame    = RGB(192, 192, 192);
				itemText.colorFrameSel = RGB(192, 192, 192);
				itemText.colorText	   = RGB(255, 255, 255);
				itemText.colorTextSel  = RGB(2, 255, 2);
			}
			else
			{
				itemText.colorBack     = RGB(192, 192, 192);
				itemText.colorBackSel  = RGB(192, 192, 192);
				itemText.colorFrame    = RGB(92, 92, 92);
				itemText.colorFrameSel = RGB(92, 92, 92);
				itemText.colorText	   = RGB(68, 68, 68);
				itemText.colorTextSel  = RGB(2, 255, 2);
			}
			itemText.szItemText   = pcszSectionName ;
			pTitleItem->arryItemText.Add(itemText);
			InsertDepend(pDependedItem, pTitleItem);
		}

		while (
			(*m_ppIniFile)->GetNextKey(pcszSectionName, szKeyName, szKeyName)
		)
		{
			(*m_ppIniFile)->GetString(
				pcszSectionName, szKeyName, "", szKeyValue, sizeof(szKeyValue)
			);	
			if (
				(CString(szKeyName) != _T("Depend_Name")) &&
				(CString(szKeyName) != _T("Depend_Value"))
			)
			{
				InsertProperty(
					pTitleItem, szKeyValue, pnInsertNumber, nCurrentRow
				);
			}
		}
	}
	else
	{	
		LPKGLISTITEM    pInsertItem = new KGLISTITEM;
		USER_ITEM_DATA  itemData;
		ITEMTEXT	    itemText;
		CString			szProperytValueView;
		char		    szPropertyValueType[128];
		char			szTemp[128];

		*szPropertyValueType = '\0';
		*szTemp				 = '\0';

		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName, _T("Name"), _T(""), szKeyValue, sizeof(szKeyValue)
		);
		itemText.colorBack    = RGB(234, 234, 234);
		itemText.colorBackSel = RGB(234, 234, 234);
		itemText.szItemText   = szKeyValue;
		pInsertItem->arryItemText.Add(itemText);

		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName, _T("Type"), _T(""), szPropertyValueType,
			sizeof(szPropertyValueType)
		);

		strncpy_s(
			itemData.szPropertyName, pcszSectionName, sizeof(itemData.szPropertyName)
		);
		itemData.szPropertyName[sizeof(itemData.szPropertyName) - 1] = '\0';
		itemData.dwValueType = GetKeyType(szPropertyValueType);
		itemData.nRowsIndexInFile = nCurrentRow;

		nRetCode = (*m_ppIniFile)->GetInteger(
			pcszSectionName, _T("Min"), INT_MIN, &itemData.nMin
		);

		nRetCode = (*m_ppIniFile)->GetInteger(
			pcszSectionName, _T("Max"), INT_MAX, &itemData.nMax
		);

		nRetCode = (*m_ppIniFile)->GetInteger(
			pcszSectionName, _T("Length"), INT_MIN, &itemData.nLength
		);

		nRetCode = (*m_ppIniFile)->GetInteger(
			pcszSectionName, _T("Need"), 0, &itemData.nNeedFlag
		);
		
		nRetCode = (*m_ppIniFile)->GetInteger(
			pcszSectionName, _T("Edit"), 1, &itemData.nEditFlag
		);

		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName, _T("Tip"), _T(""), pInsertItem->szTipText,
			sizeof(pInsertItem->szTipText) 
		);

		itemText.colorBack    = RGB(255, 255, 255);
		itemText.colorBackSel = RGB(160, 255, 160);
		if (!itemData.nEditFlag)
		{
			itemText.colorBackSel = RGB(234, 234, 234);
		}
		else if (itemData.nNeedFlag)
		{
			itemText.colorBackSel = RGB(255, 160, 160);
		}
	
		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName,_T("PARAM_1"), _T(""), itemData.szParamName_1, 
			sizeof(itemData.szParamName_1)
		);
		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName,_T("PARAM_2"), _T(""), itemData.szParamName_2, 
			sizeof(itemData.szParamName_2)
		);
		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName,_T("PARAM_3"), _T(""), itemData.szParamName_3,
			sizeof(itemData.szParamName_3)
		);
		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName,_T("PARAM_4"), _T(""), itemData.szParamName_4, 
			sizeof(itemData.szParamName_4)
		);
		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName,_T("PARAM_5"), _T(""), itemData.szParamName_5,
			sizeof(itemData.szParamName_5)
		);
		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName,_T("PARAM_6"), _T(""), itemData.szParamName_6,
			sizeof(itemData.szParamName_6)
		);
		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName,_T("PARAM_7"), _T(""), itemData.szParamName_7, 
			sizeof(itemData.szParamName_7)
		);
		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName,_T("PARAM_8"), _T(""), itemData.szParamName_8, 
			sizeof(itemData.szParamName_8)
		);
		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName,_T("PARAM_9"), _T(""), itemData.szParamName_9, 
			sizeof(itemData.szParamName_9)
		);

		nRetCode = (*m_ppIniFile)->GetString(
			pcszSectionName,_T("FILE_FIETR"), _T(""), itemData.szFileFiter, 
			sizeof(itemData.szFileFiter)
		);

		itemText.szItemText = _T("......");
		pInsertItem->arryItemText.Add(itemText);
		pInsertItem->SetStructData(&itemData);
		InsertDepend(pDependedItem, pInsertItem);
		(*pnInsertNumber)++;
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyListCtrl::UpdateProperty(
	const char szPrimaryKeyName[], int nPrimaryKeyValue, int* pnLoadNumber /* = NULL */
)
{
	int nResult  = false;
	int nRetCode = false;

	int nPos = g_GetPosByPrimaryKey(
		*m_ppTabFile, szPrimaryKeyName, nPrimaryKeyValue
	);

	UpdateProperty(nPos);

	nResult = true;
//Exit0:
	return nResult;
}

int  KGPropertyListCtrl::UpdateProperty(int nCurrentRow, int* pnLoadNumber)
{
	int nResult  = false;
	int nRetCode = false;

	int nInsertNumber = 0;

	nRetCode = RemoveAll();
	KGLOG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_SUCCESS(nCurrentRow <= 2);
	
	m_nCurRowInTabFile = nCurrentRow;
	m_lpCopyItem       = NULL;

	if (m_editEdit.m_hWnd)
		m_editEdit.ShowWindow(SW_HIDE);
	if (m_bnEditShow.m_hWnd)
		m_bnEditShow.ShowWindow(SW_HIDE);
	if (m_spinCtrl.m_hWnd)
		m_spinCtrl.ShowWindow(SW_HIDE);
	if (m_comboxCtrl.m_hWnd)
		m_comboxCtrl.ShowWindow(SW_HIDE);

	InsertProperty(NULL, _T("_MainClass_"), &nInsertNumber, nCurrentRow);

	if (pnLoadNumber)
	{
		(*pnLoadNumber) = nInsertNumber;
	}

	m_eSaveVauleType = VALUE_UNKNOW;
	m_lpSaveItem     = NULL;

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGPropertyListCtrl::RetrievesItemData(LPKGLISTITEM pItem)
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;
	char szTrueText[128];
	char szFalseText[128];
	char szTemp[128];

	CString szTempValue = _T("");
	*szTemp		 = '\0';
	*szTrueText  = '\0';
	*szFalseText = '\0';

	KGLOG_PROCESS_ERROR(m_ppIniFile);
	KGLOG_PROCESS_ERROR(*m_ppIniFile);
	KGLOG_PROCESS_ERROR(m_ppTabFile);
	KGLOG_PROCESS_ERROR(*m_ppTabFile);

	KGLOG_PROCESS_ERROR(pItem->arryItemText.GetCount() >= 2);
	pItem->GetStructData(&itemData, sizeof(itemData));

	switch (itemData.dwValueType)
	{
	case VALUE_STRING :
	case VALUE_FLOAT  :
	case VALUE_COLOR  :
	case VALUE_INT    :
	case VALUE_TYPE   :
	case VALUE_COOLDOWN_ID :
	case VALUE_STATE_ID :
	case VALUE_NPC_ID   :
	//case VALUE_NPC_TEMPLATE :
	case VALUE_OBJ_TEMPLATE_ID :
	case VALUE_FILE :
	case VALUE_IDS :
		nRetCode = (*m_ppTabFile)->GetString(
			itemData.nRowsIndexInFile, itemData.szPropertyName, _T(""),
			itemData.szPropertyValue, sizeof(itemData.szPropertyValue)
		);
		/*if (itemData.dwValueType == VALUE_COLOR)
		{
			COLORREF  color;
			CClientDC dc(this);
			CRect     rect;
			(*m_ppTabFile)->GetInteger(
				itemData.nRowsIndexInFile, itemData.szPropertyName, 0, (int*)&color
			);
			*itemData.szPropertyValue = '\0';
			pItem->arryItemText.GetAt(1).colorBackSel = color;
			pItem->arryItemText.GetAt(1).colorBack    = color;
		}*/
		break;
	case VALUE_BOOL :
		nRetCode = (*m_ppTabFile)->GetString(
			itemData.nRowsIndexInFile, itemData.szPropertyName, _T(""),
			itemData.szPropertyValue, sizeof(itemData.szPropertyValue)
		);
		if (CString(itemData.szPropertyValue) == _T("1"))
		{
			nRetCode = (*m_ppIniFile)->GetString(
				itemData.szPropertyName, _T("True"), _T(""), itemData.szPropertyValue, 
				sizeof(itemData.szPropertyValue)
			);
		}
		else if (CString(itemData.szPropertyValue) == _T("0"))
		{
			nRetCode = (*m_ppIniFile)->GetString(
				itemData.szPropertyName, _T("False"), _T(""), itemData.szPropertyValue,
				sizeof(itemData.szPropertyValue)
			);
		}
		else
		{
			nRetCode = (*m_ppIniFile)->GetString(
				itemData.szPropertyName, _T("Uncase"), _T(""), itemData.szPropertyValue, 
				sizeof(itemData.szPropertyValue)
			);
		}
		break;
	case VALUE_QUEST_MAPNPC :	
	case VALUE_MIN_MAX :
	case VALUE_DITEM   :   
	case VALUE_ITEM    :
	case VALUE_FORCE_IV   :
	case VALUE_STATE_IV   :
	case VALUE_LIVE_IV    :
	case VALUE_SKILL_IV   :
	case VALUE_NPC_ID_NUM :
	case VALUE_ITEM_TID_IID :
	case VALUE_ITEM_TID_IID_NUM :
		nRetCode = (*m_ppTabFile)->GetString(
			itemData.nRowsIndexInFile, itemData.szParamName_1, _T(""),
			szTemp, sizeof(szTemp)
		);
		szTempValue = szTempValue + szTemp + _T(" ~ ");
		nRetCode = (*m_ppTabFile)->GetString(
			itemData.nRowsIndexInFile, itemData.szParamName_2, _T(""),
			szTemp, sizeof(szTemp)
		);
		szTempValue = szTempValue + szTemp;
		if (itemData.dwValueType == VALUE_ITEM_TID_IID_NUM)
		{
			nRetCode = (*m_ppTabFile)->GetString(
				itemData.nRowsIndexInFile, itemData.szParamName_3, _T(""), 
				szTemp, sizeof(szTemp)
			);
			szTempValue = szTempValue + _T(" ~ ") + szTemp;
		}
		strncpy(
			itemData.szPropertyValue, szTempValue.GetBuffer(),
			sizeof(itemData.szPropertyValue)
		);
		itemData.szPropertyValue[sizeof(itemData.szPropertyValue) - 1] = '\0';
		break;
	case VALUE_PREQUEDT_ID :
		(*m_ppTabFile)->GetString(
			itemData.nRowsIndexInFile, itemData.szParamName_1, "",
			szTemp, sizeof(szTemp)
		);
		if (CString(szTemp) == _T("0"))
		{
			szTempValue = _T("承继逻辑关系 : [或]");
		}
		else if (CString(szTemp) == _T("1"))
		{
			szTempValue = _T("承继逻辑关系 : [与]");
		}
		else
		{
			szTempValue == _T("");
		}
		(*m_ppTabFile)->GetString(
			itemData.nRowsIndexInFile, itemData.szParamName_2, _T(""),
			szTemp, sizeof(szTemp)
		);
		if (CString(szTemp) != "")
			szTempValue += _T("    承继ID1 : ") + CString(szTemp);
		(*m_ppTabFile)->GetString(
			itemData.nRowsIndexInFile, itemData.szParamName_3, _T(""),
			szTemp, sizeof(szTemp)
		);
		if (CString(szTemp) != "")
			szTempValue += _T("    承继ID2 : ") + CString(szTemp);
		(*m_ppTabFile)->GetString(
			itemData.nRowsIndexInFile, itemData.szParamName_4, _T(""),
			szTemp, sizeof(szTemp)
		);
		if (CString(szTemp) != "")
			szTempValue += _T("   承继ID3 : ") + CString(szTemp);
		(*m_ppTabFile)->GetString(
			itemData.nRowsIndexInFile, itemData.szParamName_5, _T(""),
			szTemp, sizeof(szTemp)
		);
		if (CString(szTemp) != _T(""))
			szTempValue += _T("   承继ID4 : ") + CString(szTemp);
		strncpy(
			itemData.szPropertyValue, szTempValue.GetBuffer(),
			sizeof(itemData.szPropertyValue)
		);
		itemData.szPropertyValue[sizeof(itemData.szPropertyValue) - 1] = '\0';
		break;
	case VALUE_GOSSIP :
		(*m_ppIniFile)->GetString(
			itemData.szParamName_1, _T("True"), _T(""),  szTrueText, 
			sizeof(szTrueText)
		);
		(*m_ppIniFile)->GetString(
			itemData.szParamName_1, _T("False"), _T(""), szFalseText,
			sizeof(szFalseText)
		);

		nRetCode = (*m_ppTabFile)->GetString(
			itemData.nRowsIndexInFile, itemData.szParamName_1, _T(""), 
			szTemp, sizeof(szTemp)
		);

		if (CString(szTemp) == _T("1"))
		{
			szTempValue = szTrueText;
		}
		else if (CString(szTemp) == _T("0"))
		{
			szTempValue = szFalseText;
		}
		else
		{
			szTempValue = _T("");
		}

		szTempValue = 
			_T("泡泡类型 : ") + szTempValue + _T(" 　泡泡内容　: ..........");

		strncpy(
			itemData.szPropertyValue, szTempValue.GetBuffer(),
			sizeof(itemData.szPropertyValue)
		);
		itemData.szPropertyValue[sizeof(itemData.szPropertyValue) - 1] = '\0';
		break;
	case VALUE_FORCE_ID :
		{
			char szKValue_1[256];
			char szKValue_2[256];
			char szKName_1[256];
			char szKName_2[256];

			*szKName_1  = '\0';
			*szKName_2  = '\0';
			*szKValue_1 = '\0';
			*szKValue_2 = '\0';

			nRetCode = (*m_ppTabFile)->GetString(
				itemData.nRowsIndexInFile, itemData.szPropertyName, _T("0"), 
				szTemp, sizeof(szTemp)
			);
			while ((*m_ppIniFile)->GetNextKey(_T("Force"), szKName_1, szKName_1))
			{
				(*m_ppIniFile)->GetString(
					_T("Force"), szKName_1, _T(""), szKName_2, sizeof(szKName_2)
				);
				(*m_ppIniFile)->GetString(
					szKName_2, _T("Id"), _T(""), szKValue_1, sizeof(szKValue_1)
				);
				if (CString(szKValue_1) == CString(szTemp))
				{
					(*m_ppIniFile)->GetString(
						szKName_2, _T("Name"), _T(""), szKValue_2, sizeof(szKValue_2)
					);
					break;
				}
			}
			strncpy(
				itemData.szPropertyValue, szKValue_2, sizeof(itemData.szPropertyValue)
			);
			itemData.szPropertyValue[sizeof(itemData.szPropertyValue) - 1] = '\0';
		}
		break;
	case VALUE_SPRING_DATE :
		{
			CString szTitle  = _T("");
			CString szText   = _T("");
			char szPropertyName[64];
			char szTempValue[64];
			*szPropertyName = '\0';
			*szTempValue = '\0';
			int i = 0;
			for (i = 1; i <= 6; i++)
			{
				szTitle.Format(_T("PARAM_%d"), i);
				(*m_ppIniFile)->GetString(
					itemData.szPropertyName, szTitle.GetBuffer(), _T(""),
					szPropertyName, sizeof(szPropertyName)
				);
				(*m_ppTabFile)->GetString(
					itemData.nRowsIndexInFile, szPropertyName, _T(""),
					szTempValue, sizeof(szTempValue)
				);
				szText += CString(szTempValue) + _T("  ");
			}
			strncpy_s(
				itemData.szPropertyValue, szText.GetBuffer(), 
				sizeof(itemData.szPropertyValue)
			);
		}
		break;
	case VALUE_4PAIR :
		{
			CString szTitle  = _T("");
			CString szText   = _T("");
			char szPropertyName[64];
			char szTempValue[64];
			*szPropertyName = '\0';
			*szTempValue = '\0';
			int i = 0;
			for (i = 1; i <= 8; i++)
			{
				szTitle.Format(_T("PARAM_%d"), i);
				(*m_ppIniFile)->GetString(
					itemData.szPropertyName, szTitle.GetBuffer(), _T(""),
					szPropertyName, sizeof(szPropertyName)
				);
				(*m_ppTabFile)->GetString(
					itemData.nRowsIndexInFile, szPropertyName, _T(""),
					szTempValue, sizeof(szTempValue)
				);
				szText += CString(szTempValue) + _T(" ");
			}
			strncpy_s(
				itemData.szPropertyValue, szText.GetBuffer(), 
				sizeof(itemData.szPropertyValue)
			);
		}
		break;
	case VALUE_4FILE :
		{
			CString szTitle  = _T("");
			CString szText   = _T("");
			char szPropertyName[64];
			char szTempValue[64];
			*szPropertyName = '\0';
			*szTempValue = '\0';
			int i = 0;
			for (i = 1; i <= 4; i++)
			{
				szTitle.Format(_T("PARAM_%d"), i);
				(*m_ppIniFile)->GetString(
					itemData.szPropertyName, szTitle.GetBuffer(), _T(""),
					szPropertyName, sizeof(szPropertyName)
					);
				(*m_ppTabFile)->GetString(
					itemData.nRowsIndexInFile, szPropertyName, _T(""),
					szTempValue, sizeof(szTempValue)
					);
				szText += CString(szTempValue) + _T(" ");
			}
			strncpy_s(
				itemData.szPropertyValue, szText.GetBuffer(), 
				sizeof(itemData.szPropertyValue)
				);
		}
		break;
	case VALUE_NPC_ANI_SOUND :
	case VALUE_NPC_SOCKET :
	case VALUE_NPC_NUM_DATA :
	case VALUE_NPC_SKILL :
	case VALUE_NPC_DROP :
		strncpy_s(
			itemData.szPropertyValue, _T("< 组合值 >"), sizeof(itemData.szPropertyValue)
		);
		break;
	case VALUE_NPC_TEMPLATE :
		{
			ITabFile* pNpcTemplate = NULL;
			char szFilePath[MAX_PATH];
			char szFullPath[MAX_PATH];
			char szTypeText[64];
			char szName[64];
			char szId[8];

			nRetCode = (*m_ppIniFile)->GetString(
				itemData.szPropertyName, _T("Type"), _T(""), szTypeText, sizeof(szTypeText)
			);
			KG_PROCESS_ERROR(nRetCode);

			nRetCode = (*m_ppIniFile)->GetString(
				szTypeText, _T("TAB"), _T(""), szFilePath, sizeof(szFilePath)
			);
			KG_PROCESS_ERROR(nRetCode);

			g_GetFullPath(szFullPath, szFilePath);
			pNpcTemplate = g_OpenTabFile(szFullPath);
			KG_PROCESS_ERROR(pNpcTemplate);
		
			nRetCode = (*m_ppTabFile)->GetString(
				itemData.nRowsIndexInFile, itemData.szPropertyName, _T(""), szId, sizeof(szId)
			);
			KG_PROCESS_ERROR(nRetCode);

			pNpcTemplate->GetString(szId, _T("Name"), _T(""), szName, sizeof(szName));

			sprintf_s(itemData.szPropertyValue, "名字 : %s  ID :%s", szName, szId);
			
			SAFE_RELEASE(pNpcTemplate);
		}
		break;
	default :
		strncpy_s(
			itemData.szPropertyValue, _T("Debug Message : Cand Not Update List Data"),
			sizeof(itemData.szPropertyValue)
		);
		break;
	}
	if (itemData.dwValueType != VALUE_COLOR)
		pItem->arryItemText.GetAt(1).szItemText = itemData.szPropertyValue;
	else
		pItem->arryItemText.GetAt(1).szItemText = _T("");
	pItem->SetStructData(&itemData);
	szTempValue.ReleaseBuffer();

	nResult = true;
Exit0:
	return nResult;

}

ITEM_VILUE_TYPE KGPropertyListCtrl::GetKeyType(CString szKeyType)
{
	ITEM_VILUE_TYPE eKetType;

	if (szKeyType == "STRING")
	{
		eKetType = VALUE_STRING;
	}
	else if (szKeyType == "INT")
	{
		eKetType = VALUE_INT;
	}
	else if (szKeyType == "FLOAT")
	{
		eKetType = VALUE_FLOAT;
	}
	else if (szKeyType == "BOOL")
	{
		eKetType = VALUE_BOOL;
	}
	else if (szKeyType == "COLOR")
	{
		eKetType = VALUE_COLOR;
	}
	else if (szKeyType == "FILE_PATH")
	{
		eKetType = VALUE_FILE;
	}
	else if (szKeyType == "QUEST_TYPE")
	{
		eKetType = VALUE_TYPE;
	}
	else if (szKeyType == "MAP_NPC")
	{
		eKetType = VALUE_QUEST_MAPNPC;
	}
	else if (szKeyType == "DITEM")
	{
		eKetType = VALUE_DITEM;
	}
	else if (szKeyType == "ITEM")
	{
		eKetType = VALUE_ITEM;
	}
	else if (szKeyType == "FORCE_ID")
	{
		eKetType = VALUE_FORCE_ID;
	}
	else if (szKeyType == "FORCE_IV")
	{
		eKetType = VALUE_FORCE_IV;
	}
	else if (szKeyType == "COOLDOWN_ID")
	{
		eKetType = VALUE_COOLDOWN_ID;
	}
	else if (szKeyType == "STATE_IV")
	{
		eKetType = VALUE_STATE_IV;
	}
	else if (szKeyType == "LIVE_IV")
	{
		eKetType = VALUE_LIVE_IV;
	}
	else if (szKeyType == "SKILL_IV")
	{
		eKetType = VALUE_SKILL_IV;
	}
	else if (szKeyType == "NPC_ID")
	{
		eKetType = VALUE_NPC_ID;
	}
	else if (szKeyType == "NPC_ID_NUM")
	{
		eKetType = VALUE_NPC_ID_NUM;
	}
	else if (szKeyType == "MIN_MAX")
	{
		eKetType = VALUE_MIN_MAX;
	}
	else if (szKeyType == "ITEM_TID_IID")
	{
		eKetType = VALUE_ITEM_TID_IID;
	}
	else if (szKeyType == "ITEM_TID_IID_NUM")
	{
		eKetType = VALUE_ITEM_TID_IID_NUM;
	}
	else if (szKeyType == "NPC_TEMPLATE_ID")
	{
		eKetType = VALUE_NPC_TEMPLATE;
	}
	else if (szKeyType == "GOSSIP")
	{
		eKetType = VALUE_GOSSIP;
	}
	else if (szKeyType == "PREQUEDT_ID")
	{
		eKetType = VALUE_PREQUEDT_ID;
	}
	else if (szKeyType == "OBJ_TEMPLATE_ID")
	{
		eKetType = VALUE_OBJ_TEMPLATE_ID;
	}
	else if (szKeyType == "SPRING_DATE")
	{
		eKetType = VALUE_SPRING_DATE;
	}
	else if (szKeyType == "4PAIR")
	{
		eKetType = VALUE_4PAIR;
	}
	else if (szKeyType == "4FILE")
	{
		eKetType = VALUE_4FILE;
	}
	else if (szKeyType == "NPC_ANI_SOUND")
	{
		eKetType = VALUE_NPC_ANI_SOUND;
	}
	else if (szKeyType == "NPC_SOCKET")
	{
		eKetType = VALUE_NPC_SOCKET;
	}
	else if (szKeyType == "NPC_NUM_DATA")
	{
		eKetType = VALUE_NPC_NUM_DATA;
	}
	else if (szKeyType == "NPC_SKILL")
	{
		eKetType = VALUE_NPC_SKILL;
	}
	else if (szKeyType == "NPC_DROP")
	{
		eKetType = VALUE_NPC_DROP;
	}
	else if (szKeyType == "ID")
	{
		eKetType = VALUE_IDS;
	}
	else
	{
		eKetType = VALUE_UNKNOW;
	}

	return eKetType;
}

void KGPropertyListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM   pItem = NULL;
	USER_ITEM_DATA itemData;

	POSITION       pos   = GetFirstSelectedItemPosition();
	int            nItem = GetNextSelectedItem(pos);	

	KG_PROCESS_SUCCESS(m_eEditType != LIST_EDIT_TYPE_POPUP);
	KG_PROCESS_SUCCESS(!m_nEditFlag);
	KG_PROCESS_ERROR(nItem != -1);

	pItem = (LPKGLISTITEM)GetItemData(nItem);
	KG_PROCESS_ERROR(pItem);

	KG_PROCESS_SUCCESS(m_listDataTree.IsTitle(pItem));
	pItem->GetStructData(&itemData, sizeof(itemData));
	
	if (!itemData.nEditFlag)
	{
		if (GetAsyncKeyState(VK_CONTROL) < 0)
		{
		}
		else
		{
			KG_PROCESS_SUCCESS(true);
		}
	}

	switch (itemData.dwValueType)
	{
	case VALUE_INT :
		{
			KGValueEditIntDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_FLOAT :
		{
			KGValueEditFloatDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, FT_PERCENTAGE, this);
			dlg.DoModal();
		}
		break;
	case VALUE_STRING :
		{
			KGValueEditStringDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_MIN_MAX :
		{
			KGValueEditMinMaxDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_BOOL :
		{
			KGValueEditBoolDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_TYPE :
		{
			KGValueEditQuestTypeDlg  dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_GOSSIP :
		{
			KGValueEditGossipDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_FORCE_ID :
		{
			KGValueEditForceDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();	
		}
		break;
	case VALUE_PREQUEDT_ID :
		{
			KGValueEidtPrequestIDsDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_SPRING_DATE :
		{
			KGValudEditDateTimeDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_4PAIR :
		{
			KGValueEdit4PairParamDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_4FILE :
		{
			KGValueEdit4FilePathDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_COLOR :
	case VALUE_FILE  :
		OnBnClick();
		break;
	case VALUE_NPC_ANI_SOUND :
		{
			KGValueEditNpcAniSoundDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_NPC_SOCKET :
		{
			KDlgNPCBindInfo* dlg = new KDlgNPCBindInfo(m_pScene, 
				*m_ppTabFile, 
				*m_ppIniFile, 
				pItem);

			dlg->Create(KDlgNPCBindInfo::IDD, this);
			//dlg.DoModal();
			dlg->ShowWindow(SW_SHOW);
			//KGValueEditNpcSocketDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			//dlg.DoModal();
		}
		break;
	case VALUE_NPC_NUM_DATA :
		{
			KGValueEditNpcNumDataDlg dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
		break;
	case VALUE_NPC_SKILL :
		{
			ModifyNpcSkill(*m_ppTabFile, *m_ppIniFile, pItem, this);
		}
		break;
	case VALUE_IDS :
		{
			KGValueEditerIDsDlg dlg(*m_ppTabFile, pItem, "NPC 表现ID", this);
			dlg.DoModal();
		}
		break;
	case VALUE_NPC_DROP :
		{
			ModifyNpcDrops(*m_ppTabFile, *m_ppIniFile, pItem, this);
		}
		break;
	case VALUE_NPC_TEMPLATE :
		{
			KGValueEditNpcTemplate dlg(*m_ppTabFile, *m_ppIniFile, pItem, this);
			dlg.DoModal();
		}
	default :
		break;
	}
	if (GetParent())
		::SendMessage(
			GetParent()->m_hWnd, WM_NOTIFY_ITEM_VALUE_UPDATE, (WPARAM)pItem, (LPARAM)pItem
		);	
Exit1:
	nResult = true;
Exit0:
	return KGListCtrl::OnLButtonDblClk(nFlags, point);
}

int  KGPropertyListCtrl::ModifyNpcSkill(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pItem, CWnd* pParent
)
{
	int nResult  = false;
	int nRetCode = false;

	// define the struct 
	USER_ITEM_DATA itemData;
	struct NPCSKILL
	{
		INT SkillID1;
		INT SkillLevel1;
        INT SkillInterval1;
        INT SkillType1;

        INT SkillID2;
        INT SkillLevel2;
        INT SkillInterval2;
        INT SkillType2;

        INT SkillID3;
        INT SkillLevel3;
        INT SkillInterval3;
        INT SkillType3;

        INT SkillID4;
        INT SkillLevel4;
        INT SkillInterval4;
        INT SkillType4;

		/*INT SkillID2;
		INT SkillLevel2;
		INT SkillID3;
		INT SkillLevel3;
		INT SkillID4;
		INT SkillLevel4;
		INT SkillID5;
		INT SkillLevel5;
		INT SkillID6;
		INT SkillLevel6;
		INT SkillID7;
		INT SkillLevel7;
		INT SkillID8;
		INT SkillLevel8;*/
	};
	NPCSKILL npcSkill;

	KGMEMDATEELEMENT element[] =   
	{
		ELEMENT_TITLE("技能1(Skill1)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能等级")},
        {ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("时间间隔")},
        {ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能类型")},


		ELEMENT_TITLE("技能2(Skill2)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能等级")},
        {ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("时间间隔")},
        {ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能类型")},

		ELEMENT_TITLE("技能3(Skill3)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能等级")},
        {ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("时间间隔")},
        {ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能类型")},

		ELEMENT_TITLE("技能4(Skill4)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能等级")},
        {ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("时间间隔")},
        {ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能类型")},

		/*ELEMENT_TITLE("技能5(Skill5)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能等级")},

		ELEMENT_TITLE("技能6(Skill6)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能等级")},

		ELEMENT_TITLE("技能7(Skill7)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能等级")},

		ELEMENT_TITLE("技能8(Skill8)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("技能等级")},*/

		ELEMENT_END()	
	};

	KG_PROCESS_ERROR(pTabFile);
	KG_PROCESS_ERROR(pIniFile);
	KG_PROCESS_ERROR(pItem);

	pItem->GetStructData(&itemData, sizeof(itemData));

	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillID1"), 0, &npcSkill.SkillID1);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel1"), 0, &npcSkill.SkillLevel1);
    pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillInterval1"), 0, &npcSkill.SkillInterval1);
    pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillType1"), 0, &npcSkill.SkillType1);

	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillID2"), 0, &npcSkill.SkillID2);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel2"), 0, &npcSkill.SkillLevel2);
    pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillInterval2"), 0, &npcSkill.SkillInterval2);
    pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillType2"), 0, &npcSkill.SkillType2);

	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillID3"), 0, &npcSkill.SkillID3);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel3"), 0, &npcSkill.SkillLevel3);
    pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillInterval3"), 0, &npcSkill.SkillInterval3);
    pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillType3"), 0, &npcSkill.SkillType3);

	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillID4"), 0, &npcSkill.SkillID4);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel4"), 0, &npcSkill.SkillLevel4);
    pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillInterval4"), 0, &npcSkill.SkillInterval4);
    pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillType4"), 0, &npcSkill.SkillType4);

	/*pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillID5"), 0, &npcSkill.SkillID5);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel5"), 0, &npcSkill.SkillLevel5);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillID6"), 0, &npcSkill.SkillID6);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel6"), 0, &npcSkill.SkillLevel6);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillID7"), 0, &npcSkill.SkillID7);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel7"), 0, &npcSkill.SkillLevel7);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillID8"), 0, &npcSkill.SkillID8);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel8"), 0, &npcSkill.SkillLevel8);*/

	{
		KGMemDateEditDlg memDateDlg((BYTE*)&npcSkill, element, 100, 800, pParent);
		KG_PROCESS_SUCCESS(memDateDlg.DoModal() != IDOK);
	}
	
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillID1"), npcSkill.SkillID1);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel1"), npcSkill.SkillLevel1);
    pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillInterval1"), npcSkill.SkillInterval1);
    pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillType1"), npcSkill.SkillType1);

	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillID2"), npcSkill.SkillID2);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel2"), npcSkill.SkillLevel2);
    pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillInterval2"), npcSkill.SkillInterval2);
    pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillType2"), npcSkill.SkillType2);

	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillID3"), npcSkill.SkillID3);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel3"), npcSkill.SkillLevel3);
    pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillInterval3"), npcSkill.SkillInterval3);
    pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillType3"), npcSkill.SkillType3);

	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillID4"), npcSkill.SkillID4);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel4"), npcSkill.SkillLevel4);
    pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillInterval4"), npcSkill.SkillInterval4);
    pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillType4"), npcSkill.SkillType4);
	/*pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillID5"), npcSkill.SkillID5);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel5"), npcSkill.SkillLevel5);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillID6"), npcSkill.SkillID6);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel6"), npcSkill.SkillLevel6);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillID7"), npcSkill.SkillID7);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel7"), npcSkill.SkillLevel7);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillID8"), npcSkill.SkillID8);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("SkillLevel8"), npcSkill.SkillLevel8);*/

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int  KGPropertyListCtrl::ModifyNpcDrops(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pItem, CWnd* pParent
)
{
	int nResult  = false;
	int nRetCode = false;

	// define the struct 
	USER_ITEM_DATA itemData;
	struct NPCDROP
	{
		INT Drop1;
		INT Count1;
		INT Drop2;
		INT Count2;
		INT Drop3;
		INT Count3;
		INT Drop4;
		INT Count4;
	};
	NPCDROP npcDrop;

	KGMEMDATEELEMENT element[] =   
	{
		ELEMENT_TITLE("掉落1(Drop1)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("掉落 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("掉落数量")},

		ELEMENT_TITLE("掉落2(Drop2)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("掉落 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("掉落数量")},

		ELEMENT_TITLE("掉落3(Drop3)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("掉落 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("掉落数量")},

		ELEMENT_TITLE("掉落4(Drop4)"),
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("掉落 ID")},
		{ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_INT,    _T("掉落数量")},

		ELEMENT_END()	
	};

	KG_PROCESS_ERROR(pTabFile);
	KG_PROCESS_ERROR(pIniFile);
	KG_PROCESS_ERROR(pItem);

	pItem->GetStructData(&itemData, sizeof(itemData));

	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("Drop1"), 0, &npcDrop.Drop1);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("Count1"), 0, &npcDrop.Count1);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("Drop2"), 0, &npcDrop.Drop2);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("Count2"), 0, &npcDrop.Count2);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("Drop3"), 0, &npcDrop.Drop3);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("Count3"), 0, &npcDrop.Count3);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("Drop4"), 0, &npcDrop.Drop4);
	pTabFile->GetInteger(itemData.nRowsIndexInFile, TEXT("Count1"), 0, &npcDrop.Count4);

	{
		KGMemDateEditDlg memDateDlg((BYTE*)&npcDrop, element, 100, 800, pParent);
		KG_PROCESS_SUCCESS(memDateDlg.DoModal() != IDOK);
	}

	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("Drop1"), npcDrop.Drop1);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("Count1"), npcDrop.Count1);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("Drop2"), npcDrop.Drop2);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("Count2"), npcDrop.Count2);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("Drop3"), npcDrop.Drop3);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("Count3"), npcDrop.Count3);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("Drop4"), npcDrop.Drop4);
	pTabFile->WriteInteger(itemData.nRowsIndexInFile, TEXT("Count4"), npcDrop.Count4);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

void KGPropertyListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nResult  = false;
	int nRetCode = false;

	POSITION pos		 = GetFirstSelectedItemPosition();
	int nItem			 = GetNextSelectedItem(pos);	
	LPKGLISTITEM pItem   = NULL;
	USER_ITEM_DATA sourItemDate;
	USER_ITEM_DATA destItemDate;
	POINT  point;
	CPoint HitPoint;
	int    nSourRow = 0;
	int	   nDestRow = 0;
	int	   nRepaintItemFlag = false;

	KG_PROCESS_ERROR(nItem != -1);
	::GetCursorPos(&point);
	::ScreenToClient(this->m_hWnd, &point);
	HitPoint.x = point.x;
	HitPoint.y = point.y;

	pItem = (LPKGLISTITEM)GetItemData(nItem);
	KG_PROCESS_ERROR(pItem);

	if (nChar == 'C' && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) 
	{
		m_lpCopyItem = pItem;
	}
	else if (nChar == 'V' && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		char szTempValue[1024];
		*szTempValue = '\0';
		KG_PROCESS_ERROR(m_lpCopyItem);
		nRepaintItemFlag = true;
		m_lpCopyItem->GetStructData(&sourItemDate, sizeof(sourItemDate));
		pItem->GetStructData(&destItemDate, sizeof(destItemDate));
			
		if (destItemDate.dwValueType == sourItemDate.dwValueType)
		{	
			nRetCode = (*m_ppTabFile)->GetString(
				m_nCurRowInTabFile, sourItemDate.szPropertyName, "", 
				szTempValue, sizeof(szTempValue)
			);
			if (nRetCode)
			{
				nRetCode = (*m_ppTabFile)->WriteString(
					m_nCurRowInTabFile, destItemDate.szPropertyName, szTempValue
				);
			}
			strncpy_s(
				destItemDate.szPropertyValue, sourItemDate.szPropertyValue,
				sizeof(destItemDate.szPropertyValue)
			);
			pItem->SetStructData(&destItemDate);

			nRetCode = (*m_ppTabFile)->GetString(
				m_nCurRowInTabFile, sourItemDate.szParamName_1, "", 
				szTempValue, sizeof(szTempValue)
			);
			KG_PROCESS_SUCCESS(!nRetCode);
			nRetCode = (*m_ppTabFile)->WriteString(
				m_nCurRowInTabFile, destItemDate.szParamName_1,szTempValue
			);
			KG_PROCESS_ERROR(nRetCode);

			nRetCode = (*m_ppTabFile)->GetString(
				m_nCurRowInTabFile, sourItemDate.szParamName_2, "", 
				szTempValue, sizeof(szTempValue)
			);
			KG_PROCESS_SUCCESS(!nRetCode);
			nRetCode = (*m_ppTabFile)->WriteString(
				m_nCurRowInTabFile, destItemDate.szParamName_2,szTempValue
			);
			KG_PROCESS_ERROR(nRetCode);

			nRetCode = (*m_ppTabFile)->GetString(
				m_nCurRowInTabFile, sourItemDate.szParamName_3, "", 
				szTempValue, sizeof(szTempValue)
			);
			KG_PROCESS_SUCCESS(!nRetCode);
			nRetCode = (*m_ppTabFile)->WriteString(
				m_nCurRowInTabFile, destItemDate.szParamName_3,szTempValue
			);
			KG_PROCESS_ERROR(nRetCode);

			nRetCode = (*m_ppTabFile)->GetString(
				m_nCurRowInTabFile, sourItemDate.szParamName_4, "", 
				szTempValue, sizeof(szTempValue)
			);
			KG_PROCESS_SUCCESS(!nRetCode);
			nRetCode = (*m_ppTabFile)->WriteString(
				m_nCurRowInTabFile, destItemDate.szParamName_4,szTempValue
			);
			KG_PROCESS_ERROR(nRetCode);

			nRetCode = (*m_ppTabFile)->GetString(
				m_nCurRowInTabFile, sourItemDate.szParamName_5, "", 
				szTempValue, sizeof(szTempValue)
			);
			KG_PROCESS_SUCCESS(!nRetCode);
			nRetCode = (*m_ppTabFile)->WriteString(
				m_nCurRowInTabFile, destItemDate.szParamName_5,szTempValue
			);
			KG_PROCESS_ERROR(nRetCode);
		}
		else
		{
			::MessageBox(
				this->m_hWnd, _T("==类型不相同，不能粘贴!!=="), _T("Error"),
				MB_OK | MB_ICONERROR
			);
		}

	}
	else if (nChar == 'F' && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
		KGProperytEditFinderDlg* pDlg = 
			new KGProperytEditFinderDlg(this, NULL);
		pDlg->Create(IDD_DIALOG26, this);
		pDlg->ShowWindow(SW_SHOW);
	}
	else if (nChar == VK_RETURN)
	{
		OnLButtonDblClk(nFlags, HitPoint);
	}
	else if (nChar == VK_DELETE)
	{
		nRepaintItemFlag = true;

		pItem->GetStructData(&destItemDate, sizeof(destItemDate));
		destItemDate.szPropertyValue[0] = '\0';

		nRetCode = (*m_ppTabFile)->FindColumn(destItemDate.szPropertyName);
		(*m_ppTabFile)->WriteString(
			m_nCurRowInTabFile, destItemDate.szPropertyName, _T("")
		);
		destItemDate.szPropertyValue[0] = '\0'; 
		pItem->SetStructData(&destItemDate);
		KG_PROCESS_SUCCESS(nRetCode);

		KG_PROCESS_SUCCESS(destItemDate.szParamName_1[0] == '\0');
		nRetCode = (*m_ppTabFile)->WriteString(
			m_nCurRowInTabFile, destItemDate.szParamName_1, _T("")
		);
		KG_PROCESS_ERROR(nRetCode);

		KG_PROCESS_SUCCESS(destItemDate.szParamName_2[0] == '\0');
		nRetCode = (*m_ppTabFile)->WriteString(
			m_nCurRowInTabFile, destItemDate.szParamName_2, _T("")
		);
		KG_PROCESS_ERROR(nRetCode);

		KG_PROCESS_SUCCESS(destItemDate.szParamName_3[0] == '\0');
		nRetCode = (*m_ppTabFile)->WriteString(
			m_nCurRowInTabFile, destItemDate.szParamName_3, _T("")
		);
		KG_PROCESS_ERROR(nRetCode);

		KG_PROCESS_SUCCESS(destItemDate.szParamName_4[0] == '\0');
		nRetCode = (*m_ppTabFile)->WriteString(
			m_nCurRowInTabFile, destItemDate.szParamName_4, _T("")
		);
		KG_PROCESS_ERROR(nRetCode);

		KG_PROCESS_SUCCESS(destItemDate.szParamName_5[0] == '\0');
		nRetCode = (*m_ppTabFile)->WriteString(
			m_nCurRowInTabFile, destItemDate.szParamName_5, _T("")
		);
		KG_PROCESS_ERROR(nRetCode);
	}

Exit1:
	if (nRepaintItemFlag)
	{
		Update(pItem);
	}
	nResult = true;
Exit0:
	return KGListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

LRESULT KGPropertyListCtrl::OnUpdateListItem(WPARAM wParam, LPARAM lParam)
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM   pListItem = (LPKGLISTITEM)wParam;
	USER_ITEM_DATA itemDate;
	KGLOG_PROCESS_ERROR(pListItem);
	
	pListItem->GetStructData(&itemDate, sizeof(itemDate));
	Update(pListItem);
	
	if (itemDate.dwValueType == VALUE_TYPE)
	{
		UpdateProperty(itemDate.nRowsIndexInFile);
		if (GetParent())
			GetParent()->SendMessage(WM_USER_UPDATE_TREE_ICON);
	}
	nResult = true;
Exit0:
	return nResult;
}

HBRUSH KGPropertyListCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd == &m_editEdit)
	{
		return CreateSolidBrush(RGB(255, 255, 255));
	}

	return CListCtrl::OnCtlColor(pDC, pWnd, nCtlColor);
}

void KGPropertyListCtrl::OnBnClick()
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM   pListItem = NULL;
	USER_ITEM_DATA itemData;
	CString szFileFiter = _T("");
	
	char* pszDefFilePath = NULL;
	char  szDefModelName[MAX_PATH];
	char  szDefAnimaName[MAX_PATH];
	char  szDefFilePath[MAX_PATH];

	char szColsText[1024];
	char szColsName[64];
	
	*szDefAnimaName = '\0';
	*szDefModelName = '\0';
	*szDefFilePath  = '\0';

	*szColsText = '\0';
	*szColsName = '\0';

	int nFileColsCount = 0;
	int nFileCurCols   = 0;

	nFileColsCount = (*m_ppTabFile)->GetWidth();

	POSITION pos   = GetFirstSelectedItemPosition();
	int      nItem = GetNextSelectedItem(pos);	

	if (nItem == -1) return;

	pListItem = (LPKGLISTITEM)GetItemData(nItem);
	pListItem->GetStructData(&itemData, sizeof(itemData));

	if (strstr(itemData.szPropertyName, _T("ModelFile")))
	{
		szFileFiter = _T("模型文件|*.mesh|所有文件|*.*|");
		g_GetFullPath(szDefModelName, itemData.szPropertyValue);
		g_GetFullPath(szDefFilePath, itemData.szPropertyValue);
	}
	else if (strstr(itemData.szPropertyName, _T("AniFile")))
	{
		szFileFiter = _T("动作文件|*.ani|所有文件|*.*|");
		g_GetFullPath(szDefAnimaName, itemData.szPropertyValue);
		g_GetFullPath(szDefFilePath, itemData.szPropertyValue);

		for (nFileCurCols = 1; nFileCurCols <= nFileColsCount; nFileCurCols++)
		{
			strncpy(
				szColsName, (*m_ppTabFile)->GetColName(nFileCurCols), sizeof(szColsName)
			);
			if (strstr(szColsName, _T("ModelFile")))
			{
				(*m_ppTabFile)->GetString(
					itemData.nRowsIndexInFile, nFileCurCols, _T(""),
					szColsText, sizeof(szColsText)
				);
				if (*szColsText != '\0')
				{
					g_GetFullPath(szDefModelName, szColsText);
					break;
				}
			}
		}
	}
	else if (strstr(itemData.szPropertyName, _T("MaterialFile")))
	{
		szFileFiter = _T("材质文件|*.mtl|所有文件|*.*|");
		g_GetFullPath(szDefFilePath, itemData.szPropertyValue);
	}
	else
	{
		szFileFiter = _T("所有文件|*.*|");
		g_GetFullPath(szDefFilePath, itemData.szPropertyValue);
	}

	if (*itemData.szPropertyValue != '\0')
		pszDefFilePath = szDefFilePath;
	else
		pszDefFilePath = NULL;

	KG3DFileViewDialog fileDlg(
		true, szDefModelName, szDefAnimaName, NULL, pszDefFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFileFiter.GetBuffer(),this
	);
	
	switch (itemData.dwValueType)
	{
	case VALUE_FILE :
		if (fileDlg.DoModal() == IDOK)
		{
			CString szFilePaht = fileDlg.GetPathName();
			char szRetPath[MAX_PATH];
			*szRetPath = '\0';

			nRetCode = g_GetFilePathFromFullPath(&szRetPath[0], szFilePaht.GetBuffer());
			if (!nRetCode)
			{
				::MessageBox(
					this->m_hWnd, _T("只能选择在工作目录中的文件!!\t\t"), _T("Error!!"),
					MB_OK | MB_ICONERROR
				);
				KGLOG_PROCESS_ERROR(false);
			}

			nRetCode = (*m_ppTabFile)->WriteString(
				itemData.nRowsIndexInFile, itemData.szPropertyName, szRetPath
			);
			KGLOG_PROCESS_ERROR(nRetCode);

			if (CString(itemData.szParamName_1) == TEXT("AUTOFILL"))
			{
				char szLeadPath[MAX_PATH];
				strncpy(szLeadPath, szFilePaht.GetBuffer(), sizeof(szLeadPath));
				char* pszPrefix = strrchr(szLeadPath, '.');
				if (pszPrefix)
				{	
					char szPrefix[128]; 
					*pszPrefix = '\0';
					pszPrefix = strrchr(szLeadPath, '\\');
					if (pszPrefix)
					{
						pszPrefix++;
						strncpy(szPrefix, pszPrefix, sizeof(szPrefix));
						*pszPrefix = '\0';
						AutoFillTable(
							*m_ppTabFile, itemData.nRowsIndexInFile, *m_ppIniFile, 
							szLeadPath, szPrefix
						);
					}

				}
			}


			Update(pListItem);
		}
		break;
	case VALUE_COLOR:
		{
			int nColor = 0;
			(*m_ppTabFile)->GetInteger(
				itemData.nRowsIndexInFile, itemData.szPropertyName, 0, &nColor
			);
			CColorDialog colorDlg((COLORREF)nColor, 0, this);
			if (colorDlg.DoModal() == IDOK)
			{
				nColor = colorDlg.GetColor();
				nRetCode = (*m_ppTabFile)->WriteInteger(
					itemData.nRowsIndexInFile, itemData.szPropertyName, nColor
				);
				KGLOG_PROCESS_ERROR(nRetCode);
				Update(pListItem);
			}

		}
		break; 
	default :
		break;
	}

	if (GetParent())
		::SendMessage(
		GetParent()->m_hWnd, WM_LIST_UPDATE_SCENE, (WPARAM)pListItem, (LPARAM)pListItem
		);	
	nResult = true;
Exit0:
	return;
}

void KGPropertyListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_editEdit.m_hWnd)
		m_editEdit.ShowWindow(SW_HIDE);
	if (m_bnEditShow.m_hWnd)
		m_bnEditShow.ShowWindow(SW_HIDE);
	if (m_spinCtrl.m_hWnd)
		m_spinCtrl.ShowWindow(SW_HIDE);
	if (m_comboxCtrl.m_hWnd)
		m_comboxCtrl.ShowWindow(SW_HIDE);

	return KGListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void KGPropertyListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	KG_PROCESS_SUCCESS(pScrollBar == (CWnd*)&m_spinCtrl);

	if (m_editEdit.m_hWnd)
		m_editEdit.ShowWindow(SW_HIDE);
	if (m_bnEditShow.m_hWnd)
		m_bnEditShow.ShowWindow(SW_HIDE);
	if (m_spinCtrl.m_hWnd)
		m_spinCtrl.ShowWindow(SW_HIDE);
	if (m_comboxCtrl.m_hWnd)
		m_comboxCtrl.ShowWindow(SW_HIDE);

Exit1:
	return KGListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL KGPropertyListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_editEdit.m_hWnd)
		m_editEdit.ShowWindow(SW_HIDE);
	if (m_bnEditShow.m_hWnd)
		m_bnEditShow.ShowWindow(SW_HIDE);
	if (m_spinCtrl.m_hWnd)
		m_spinCtrl.ShowWindow(SW_HIDE);
	if (m_comboxCtrl.m_hWnd)
		m_comboxCtrl.ShowWindow(SW_HIDE);

	return KGListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void KGPropertyListCtrl::OnSize(UINT nType, int cx, int cy)
{
	if (m_editEdit.m_hWnd)
		m_editEdit.ShowWindow(SW_HIDE);
	if (m_bnEditShow.m_hWnd)
		m_bnEditShow.ShowWindow(SW_HIDE);
	if (m_spinCtrl.m_hWnd)
		m_spinCtrl.ShowWindow(SW_HIDE);
	if (m_comboxCtrl.m_hWnd)
		m_comboxCtrl.ShowWindow(SW_HIDE);

	return KGListCtrl::OnSize(nType, cx, cy);
}

void KGPropertyListCtrl::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	USER_ITEM_DATA useData;
	CString szEditText = _T("");
	
	KGLOG_PROCESS_ERROR(m_lpSaveItem);

	m_lpSaveItem->GetStructData(&useData, sizeof(useData));

	float fValue = 0.0f;

	if (m_eSaveVauleType == VALUE_FLOAT)
	{
		m_editEdit.GetWindowText(szEditText);
		fValue = (float)atof(szEditText.GetBuffer()) + pNMUpDown->iDelta / 100.0f;
		if (fValue > 0.0f  && fValue < 10.0f)
			szEditText.Format("%.2f", fValue);
		strncpy_s(
			useData.szPropertyValue, szEditText.GetBuffer(), sizeof(useData.szPropertyValue)
		);
		m_editEdit.SetWindowText(szEditText);
	}
	
	m_lpSaveItem->SetStructData(&useData);
	(*m_ppTabFile)->WriteString(
		useData.nRowsIndexInFile, useData.szPropertyName, szEditText.GetBuffer()
	);

	/*
	::SendMessage(
		GetParent()->m_hWnd, WM_NOTIFY_ITEM_VALUE_UPDATE, (WPARAM)m_lpSaveItem, 0
	);*/

Exit0:
	*pResult = 0;
}

void KGPropertyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	LPKGLISTITEM pItem = (LPKGLISTITEM)GetItemData(lpDrawItemStruct->itemID);
	USER_ITEM_DATA itemData;

	KGListCtrl::DrawItem(lpDrawItemStruct);
	
	KG_PROCESS_ERROR(pItem);

	pItem->GetStructData(&itemData, sizeof(itemData));

	if (itemData.dwValueType == VALUE_COLOR)
	{
		COLORREF colorRef = (COLORREF)atoi(itemData.szPropertyValue);
		CRect rect;
		GetSubItemRect(
			lpDrawItemStruct->itemID, 1, LVIR_BOUNDS, rect
		);
		rect.left += 12;
		rect.top  += 2;
		rect.bottom -= 1;
		rect.right = rect.left + rect.Height();
		pDC->FillRect(&rect, &CBrush(colorRef));
		pDC->Draw3dRect(&rect, RGB(100, 100, 100), RGB(100, 100, 100));
	}

Exit0:
	return;
}

void KGPropertyListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	KGListCtrl::OnLButtonDown(nFlags, point);

	LPKGLISTITEM   pListItem = NULL;
	USER_ITEM_DATA itemData;
	
	char szUncaseText[256];
	char szTureText[256];
	char szFalseText[256];
	
	*szUncaseText = '\0';
	*szTureText   = '\0';
	*szFalseText  = '\0';

	CRect rectBnEdit;
	CRect rectClient;
	CRect rectEditer;

	POSITION pos   = GetFirstSelectedItemPosition();
	int      nItem = GetNextSelectedItem(pos);	

	KG_PROCESS_ERROR(nItem != -1);
	pListItem = (LPKGLISTITEM)GetItemData(nItem);
	KG_PROCESS_ERROR(pListItem);
	pListItem->GetStructData(&itemData, sizeof(itemData));
	KG_PROCESS_SUCCESS(m_listDataTree.IsTitle(pListItem));

	KG_PROCESS_SUCCESS(!itemData.nEditFlag);
	KG_PROCESS_SUCCESS(m_eEditType != LIST_EDIT_TYPE_BAND);
	KG_PROCESS_SUCCESS(!m_nEditFlag);

	if (!m_bnEditShow.m_hWnd)
	{
		m_bnEditShow.Create(_T("?"), WS_CHILD, rectBnEdit, this, ID_BN_SHOW);
	}
	if (!m_editEdit.m_hWnd)
	{
		m_editEdit.Create(ES_AUTOHSCROLL, rectEditer, this, ID_ED_SHOW);               
		m_editEdit.SetFont(GetFont());
	}
	if (!m_spinCtrl.m_hWnd)
	{
		m_spinCtrl.Create(WS_CHILD | UDS_SETBUDDYINT, rectBnEdit, this, ID_SP_SHOW);
	}
	if (!m_comboxCtrl.m_hWnd)
	{
		m_comboxCtrl.Create(WS_CHILD | CBS_DROPDOWNLIST, rectBnEdit, this, ID_CB_SHOW);
		m_comboxCtrl.SetFont(GetFont());
	}

	m_editEdit.ShowWindow(SW_HIDE);
	m_bnEditShow.ShowWindow(SW_HIDE);
	m_spinCtrl.ShowWindow(SW_HIDE);
	m_comboxCtrl.ShowWindow(SW_HIDE);

	//KG_PROCESS_ERROR(nItem != -1);
	//pListItem = (LPKGLISTITEM)GetItemData(nItem);
	//KG_PROCESS_ERROR(pListItem);
	//pListItem->GetStructData(&itemData, sizeof(itemData));
	//KG_PROCESS_SUCCESS(m_listDataTree.IsTitle(pListItem));

	GetSubItemRect(nItem, 1, LVIR_BOUNDS, rectBnEdit);
	GetWindowRect(rectClient);
	ScreenToClient(rectClient);

	rectEditer		 = rectBnEdit;
	rectBnEdit.right = rectClient.right - 18;
	rectBnEdit.left  = rectBnEdit.right - rectBnEdit.Height();

	rectEditer.left  = rectEditer.left + 1;
	rectEditer.top   = rectEditer.top  + 1;

	switch (m_eSaveVauleType)
	{
	case VALUE_INT    :
	case VALUE_STRING :
	case VALUE_FLOAT  :
		{
			CString szValue = _T("");
			USER_ITEM_DATA useData;
			KGLOG_PROCESS_ERROR(m_lpSaveItem);
			m_lpSaveItem->GetStructData(&useData, sizeof(useData));
			m_editEdit.GetWindowText(szValue);
			(*m_ppTabFile)->WriteString(
				useData.nRowsIndexInFile, useData.szPropertyName, szValue.GetBuffer()
			);
			m_editEdit.SetWindowText(_T(""));
			Update(m_lpSaveItem);
		}
		break;
	case VALUE_BOOL :
		{
			int nIndex = m_comboxCtrl.GetCurSel();
			CString szValue;
			USER_ITEM_DATA useData;
			KGLOG_PROCESS_ERROR(m_lpSaveItem);
			if (nIndex != -1)
			{
				m_lpSaveItem->GetStructData(&useData, sizeof(useData));
				szValue = m_pszState[nIndex];
				(*m_ppTabFile)->WriteString(
					useData.nRowsIndexInFile, useData.szPropertyName, szValue.GetBuffer()
				);
				Update(m_lpSaveItem);
			}
		}
		break;
	}

	::SendMessage(
		GetParent()->m_hWnd, WM_NOTIFY_ITEM_VALUE_UPDATE, (WPARAM)m_lpSaveItem, (LPARAM)pListItem
	);

	switch (itemData.dwValueType)
	{
	case VALUE_FILE  :
	case VALUE_COLOR :
		m_bnEditShow.ShowWindow(SW_SHOW);
		m_bnEditShow.MoveWindow(&rectBnEdit);
		break;
	case VALUE_FLOAT :
	case VALUE_INT   :
		if (itemData.dwValueType == VALUE_INT)
		{
			m_editEdit.ModifyStyle(0, ES_NUMBER);
			m_spinCtrl.SetBuddy(&m_editEdit);
		}
		else
		{
			m_spinCtrl.SetBuddy(NULL);
		}
		m_editEdit.SetWindowText(itemData.szPropertyValue);
		m_editEdit.ShowWindow(SW_SHOW);
		rectEditer.right = rectBnEdit.left;
		m_editEdit.MoveWindow(&rectEditer);		
		m_spinCtrl.ShowWindow(SW_SHOW);
		m_spinCtrl.MoveWindow(&rectBnEdit);
		//m_spinCtrl.SetBuddy(&m_editEdit);
		m_spinCtrl.SetRange32(itemData.nMin, itemData.nMax);
		m_editEdit.SetFocus();
		break;
	case VALUE_STRING :
		m_editEdit.ModifyStyle(ES_NUMBER, 0);
		m_editEdit.SetWindowText(itemData.szPropertyValue);
		m_editEdit.ShowWindow(SW_SHOW);
		m_editEdit.MoveWindow(&rectEditer);		
		m_editEdit.SetFocus();
		break;
	case VALUE_BOOL :
		m_comboxCtrl.ShowWindow(SW_SHOW);
		rectBnEdit.left   = rectEditer.left;
		rectBnEdit.bottom = rectBnEdit.top + 200;
		m_comboxCtrl.MoveWindow(&rectBnEdit);
		for (int i = 0; i <= m_comboxCtrl.GetCount(); i++)
			m_comboxCtrl.DeleteString(0);
		(*m_ppIniFile)->GetString(
			itemData.szPropertyName, "True", _T(""), szTureText, 
			sizeof(szTureText)
		);
		if (CString(szTureText) != "")
		{
			m_comboxCtrl.AddString(szTureText);
			m_pszState[0] = _T("1");
		}
		(*m_ppIniFile)->GetString(
			itemData.szPropertyName, "False", _T(""), szFalseText,
			sizeof(szFalseText)
		);
		if (CString(szFalseText) != "")
		{
			m_comboxCtrl.AddString(szFalseText);
			m_pszState[1] = _T("0");
		}
		(*m_ppIniFile)->GetString(
			itemData.szPropertyName, "Uncase", _T(""), szUncaseText, 
			sizeof(szUncaseText)
		);
		if (CString(szUncaseText) != "")
		{
			m_comboxCtrl.AddString(szUncaseText);
			m_pszState[2] = _T("-1");
		}
		/*if (CString(itemData.szPropertyValue) == _T("1"))
		{
			m_comboxCtrl.SelectString(0, szTureText);
		}
		else if (CString(itemData.szPropertyValue) == _T("0"))
		{
			m_comboxCtrl.SelectString(0, szFalseText);
		}
		else 
		{*/
		m_comboxCtrl.SelectString(0, itemData.szPropertyValue);
		break;
	default :
		m_editEdit.SetWindowText(_T("Debug Message : UnKnow Value Type"));
		break;
	}
	m_lpSaveItem     = pListItem;
	m_eSaveVauleType = (ITEM_VILUE_TYPE)itemData.dwValueType;

	//::SendMessage(GetParent()->m_hWnd, WM_LIST_UPDATE_SCENE, 0, 0);

Exit1:
Exit0:
	return;
}

void KGPropertyListCtrl::SetScene(LPVOID pScene)
{
	m_pScene = static_cast<IEKG3DScene*>(pScene);
}
