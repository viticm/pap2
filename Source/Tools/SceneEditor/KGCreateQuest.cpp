////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGCreateQuestDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:27:18
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGCreateQuest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGCreateQuestDlg, CDialog)
KGCreateQuestDlg::KGCreateQuestDlg(
	CTreeCtrl* pTree, HTREEITEM hItem, ITabFile* pFile,
	CString szMap, CString szLink, CString szQuest,
	QUEST_TYPE eType, CWnd* pParent
) : CDialog(KGCreateQuestDlg::IDD, pParent)
{
	m_pTree	   = pTree;
	m_pTabFile = pFile;
	m_hItem    = hItem;
	m_eType	   = eType;

	m_szMapName		  = szMap;
	m_szQuestLinkName = szLink;
	m_szQuestName	  = szQuest;
}

KGCreateQuestDlg::~KGCreateQuestDlg()
{
}

void KGCreateQuestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME, m_szName);
}

BOOL KGCreateQuestDlg::OnInitDialog()
{
	/*
	CDialog::OnInitDialog();

	switch (m_eType)
	{
	case  QUEST_MAP:
		SetWindowText("请写入地图名 : ");
		break;
	case QUEST_LINK :
		SetWindowText("请写入任务链名 : ");
		break;
	case QUEST_GLEAN :
		m_szQuestType = "收集";
		SetWindowText("请写入任务名 : ");
		break;
	case QUEST_KILL  :
		m_szQuestType = "杀怪";
		SetWindowText("请写入任务名 : ");
		break;
	case QUEST_POS :
		m_szQuestType = "送信";
		SetWindowText("请写入任务名 : ");
		break;
	case QUEST_TALK :
		m_szQuestType = "对话";
		SetWindowText("请写入任务名 : ");
		break;
	case QUEST_OFFER :
		m_szQuestType = "贡献";
		SetWindowText("请写入任务名 : ");
		break;
	default:
		m_szQuestType = "其他";
		SetWindowText("请写入任务名 : ");
		break;
	}

	UpdateData(false);
*/
	return TRUE;
}

BEGIN_MESSAGE_MAP(KGCreateQuestDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void KGCreateQuestDlg::OnBnClickedOk()
{
	int nResult  = false;
	int nRetCode = false;
	/*
	m_nOkFlag    = false;
	UpdateData();

	if (m_szName == "")
	{
		::MessageBox(this->m_hWnd, "必须写入名字", "Error", MB_OK);
		KG_PROCESS_ERROR(false);
	}

	switch (m_eType)
	{
	case QUEST_MAP :
		{
			HTREEITEM hItem = m_pTree->GetRootItem();
			while (hItem)
			{
				if (m_pTree->GetItemText(hItem) == m_szName)
				{
					::MessageBox(this->m_hWnd, "地图已存在", "Error", MB_OK);
					KG_PROCESS_ERROR(false);
				}
				hItem = m_pTree->GetNextItem(hItem, TVGN_NEXT);
			}

			if (m_szMapName != "")
			{
				m_pTree->InsertItem(
					m_szName, m_eType, m_eType, TVI_ROOT, m_hItem
				);
			}
			else
			{
				m_pTree->InsertItem(m_szName, m_eType, m_eType);
			}
		}
		break;
	case QUEST_LINK :
		{
			HTREEITEM hItem = m_pTree->GetChildItem(m_hItem);
			while (hItem)
			{
				if (m_pTree->GetItemText(hItem) == m_szName)
				{
					::MessageBox(this->m_hWnd, "任务链已存在", "Error", MB_OK);
					KG_PROCESS_ERROR(false);
				}
				hItem = m_pTree->GetNextItem(hItem, TVGN_NEXT);
			}

			if (m_szQuestLinkName != "")
			{
				m_pTree->InsertItem(
					m_szName, m_eType, m_eType,
					m_pTree->GetParentItem(m_hItem), m_hItem
				);
			}
			else
			{
				m_pTree->InsertItem(m_szName, m_eType, m_eType, m_hItem);
			}
		}
		break;
	case QUEST_GLEAN :
	case QUEST_KILL  :
	case QUEST_POS   :
	case QUEST_TALK  :
	case QUEST_OFFER :
	case QUEST_OTHER :
		{
			char szMapName[256];
			char szLinName[256];
			char szQueName[256];
			int  nRowCount  = m_pTabFile->GetHeight();
			int	 nQuestID   = 0;
			HTREEITEM hItem;

			if (m_szQuestName != "")
			{
				hItem = m_hItem;
			}
			else
			{
				hItem = m_pTree->GetChildItem(m_hItem);
			}

			//
			// Insert Node Of The Tree
			//

			while (hItem)
			{
				if (m_pTree->GetItemText(hItem) == m_szName)
				{
					::MessageBox(this->m_hWnd, "任务存在", "Error", MB_OK);
					KG_PROCESS_ERROR(false);
				}
				hItem = m_pTree->GetNextItem(hItem, TVGN_PREVIOUS);
			}

			if (m_szQuestName != "")
			{
				hItem = m_pTree->InsertItem(
					m_szName, m_eType, m_eType,
					m_pTree->GetParentItem(m_hItem), m_hItem
				);
			}
			else
			{
				hItem = m_pTree->InsertItem(
					m_szName, m_eType, m_eType, m_hItem, TVI_FIRST
				);
			}

			//
			// Insert Quest to Tab File
			//


			int i = 2;
			while (i <= nRowCount && nRowCount > 2)
			{
				m_pTabFile->GetString(
					i, "MapName", "", szMapName, sizeof(szMapName)
				);
				m_pTabFile->GetString(
					i, "TaskLinkName", "", szLinName, sizeof(szLinName)
				);
				m_pTabFile->GetString(
					i, "QuestName", "", szQueName, sizeof(szQueName)
				);

				if (m_szQuestName != "")
				{
					if (
						m_szMapName       == CString(szMapName) &&
						m_szQuestLinkName == CString(szLinName) &&
						m_szQuestName     == CString(szQueName)
					)
					{
						break;
					}
				}
				else
				{
					if (
						m_szMapName       == CString(szMapName) &&
						m_szQuestLinkName == CString(szLinName)
					)
					{
						i--;
						break;
					}
				}
				i++;
			}

			if (i > nRowCount)
			{
				i--;
			}

			int n = 1;
			//int nIDSave = 0;
			while (n <= nRowCount)
			{
				int nID = 0;
				m_pTabFile->GetInteger(n, "QuestID", nID, &nID);
				if (nID > nQuestID)
				{
					nQuestID = nID;
				}
				//nIDSave = nID;
				n++;
			}
			nQuestID++;
			m_pTabFile->InsertAfter(i);
			m_pTabFile->WriteInteger(i + 1, "QuestID", nQuestID);
			m_pTabFile->WriteString(i + 1, "MapName", m_szMapName.GetBuffer());
			m_pTabFile->WriteString(
				i + 1, "TaskLinkName", m_szQuestLinkName.GetBuffer()
			);
			m_pTabFile->WriteString(i + 1, "QuestName", m_szName.GetBuffer());
			m_pTabFile->WriteString(i + 1, "QuestType", m_szQuestType);

			//
			// Id Relationship
			//

			m_pTabFile->GetString(
				i, "MapName", "", szMapName, sizeof(szMapName)
			);
			m_pTabFile->GetString(
				i, "TaskLinkName", "", szLinName, sizeof(szLinName)
			);
			if (
				CString(szMapName) == m_szMapName &&
				CString(szLinName) == m_szQuestLinkName
			)
			{
				int nId;
				m_pTabFile->GetInteger(i, "QuestID", -1, &nId);
				KG_PROCESS_ERROR(nId != -1);
				m_pTabFile->WriteInteger(i + 1, "PrequestID", nId);
			}

			m_pTabFile->GetString(
				i + 2, "MapName", "", szMapName, sizeof(szMapName)
			);
			m_pTabFile->GetString(
				i + 2, "TaskLinkName", "", szLinName, sizeof(szLinName)
			);
			if (
				CString(szMapName) == m_szMapName &&
				CString(szLinName) == m_szQuestLinkName
			)
			{
				int nId;
				m_pTabFile->GetInteger(i + 1, "QuestID", -1, &nId);
				KG_PROCESS_ERROR(nId != -1);
				m_pTabFile->WriteInteger(i + 2, "PrequestID", nId);
			}

			m_pTree->SetItemData(hItem, nQuestID);
			m_pTree->SelectItem(hItem);
		}
		break;
	default:;
	}

	nResult = true;
	m_nOkFlag = true;
	OnOK();*/
//Exit0:
	return;
}

