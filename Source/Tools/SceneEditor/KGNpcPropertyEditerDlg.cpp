////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGNpcPropertyEditerDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-25 12:48:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGNpcPropertyEditerDlg.h"
#include "SO3Represent/SO3RepresentInterface.h"
#include "IEEditor.h"
//#include "KG3DObjectPropertyDefine.h"
#include "IEKG3DSceneOutputWnd.h"
#include "KNpcTemplateList.h"
#include "KSO3World.h"
#include "KSceneEditorReviveDialog.h"
#include "KSceneEditorRandomGroupDialog.h"
#include "KSceneEditorAISetDialog.h"
#include "KSceneCommonParamDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CHECKSAME_INT	1
#define CHECKSAME_CHAR	2
#define PARENT_LISTITEMCOUNT	19
#define InitItem(_itemtext)						\
pItem = new KGLISTITEM();						\
itemText.colorBack  = RGB(128, 128, 192);		\
itemText.colorBackSel = itemText.colorBack;		\
itemText.szItemText = _T(_itemtext);			\
pItem->arryItemText.Add(itemText);				\
itemText.colorBack  = RGB(255, 255, 255);		\
itemText.colorBackSel = itemText.colorBack;

#define CheckNPCPropSameInt(_buf, _prop)																								\
{																																		\
	bAllPropAreTheSame = TRUE;																											\
	bFirstLoop = TRUE;																													\
	_buf = 0;																															\
	for (std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iter = m_listNpcData.begin(); iter != m_listNpcData.end(); iter++)		\
	{																																	\
		KSceneEditorDocLogical::NPCInfo* npc = *iter;																					\
		if (bFirstLoop)																													\
		{																																\
			_buf = npc->_prop;																											\
			bFirstLoop = FALSE;																											\
			continue;																													\
		}																																\
		if (_buf != npc->_prop)																											\
		{																																\
			bAllPropAreTheSame = FALSE;																									\
			break;																														\
		}																																\
	}																																	\
}

#define CheckNPCPropSameChar(_buf, _prop)																								\
{																																		\
	bAllPropAreTheSame = TRUE;																											\
	bFirstLoop = TRUE;																													\
	for (std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iter = m_listNpcData.begin(); iter != m_listNpcData.end(); iter++)		\
	{																																	\
		KSceneEditorDocLogical::NPCInfo* npc = *iter;																					\
		if (bFirstLoop)																													\
		{																																\
			strcpy(_buf, npc->_prop.c_str());																					\
			bFirstLoop = FALSE;																											\
			continue;																													\
		}																																\
		if (strcmp(_buf, npc->_prop.c_str()))																						\
		{																																\
			bAllPropAreTheSame = FALSE;																									\
			break;																														\
		}																																\
	}																																	\
}

//#define CheckDoodadPropSameInt(_buf, _prop)																									\
//{																																			\
//	bAllPropAreTheSame = TRUE;																												\
//	bFirstLoop = TRUE;																														\
//	_buf = 0;																																\
//	for (std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator iter = m_listDoodadData.begin(); iter != m_listDoodadData.end(); iter++)	\
//	{																																		\
//		KSceneEditorDocLogical::DoodadInfo* doodad = *iter;																					\
//		if (bFirstLoop)																														\
//		{																																	\
//			_buf = doodad->_prop;																											\
//			bFirstLoop = FALSE;																												\
//			continue;																														\
//		}																																	\
//		if (_buf != doodad->_prop)																											\
//		{																																	\
//			bAllPropAreTheSame = FALSE;																										\
//			break;																															\
//		}																																	\
//	}																																		\
//}
//
//#define CheckDoodadPropSameChar(_buf, _prop)																								\
//{																																			\
//	bAllPropAreTheSame = TRUE;																												\
//	bFirstLoop = TRUE;																														\
//	for (std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator iter = m_listDoodadData.begin(); iter != m_listDoodadData.end(); iter++)	\
//	{																																		\
//		KSceneEditorDocLogical::DoodadInfo* doodad = *iter;																					\
//		if (bFirstLoop)																														\
//		{																																	\
//			strcpy(_buf, doodad->_prop.c_str());																							\
//			bFirstLoop = FALSE;																												\
//			continue;																														\
//		}																																	\
//		if (strcmp(_buf, doodad->_prop.c_str()))																							\
//		{																																	\
//			bAllPropAreTheSame = FALSE;																										\
//			break;																															\
//		}																																	\
//	}																																		\
//}


extern IKSO3RepresentHandler* g_pRepresentHandler;

IMPLEMENT_DYNAMIC(KGNpcPropertyEditerDlg, CDialog)

KGNpcPropertyEditerDlg::KGNpcPropertyEditerDlg(
    IEKG3DScene* lpEditScene,KSceneEditorDocLogical* pDocLogical, CWnd* pParent, int nType
) : CDialog(KGNpcPropertyEditerDlg::IDD, pParent)
{
	m_lpEditScene			= lpEditScene;
	m_lpDocLogical			= pDocLogical;
    m_nType					= nType;
	m_nSelCount				= 0;
	m_pHandelObj			= NULL;
    m_plogicNpcData			= NULL;
	m_plogicNpcDataBak		= NULL;
	m_plogicDoodadData		= NULL;
	m_plogicDoodadDataBak	= NULL;
	m_pParentNpcData		= NULL;
	//m_pParentDoodadData		= NULL;
	m_hItemParent			= NULL;
}

KGNpcPropertyEditerDlg::~KGNpcPropertyEditerDlg()
{
	//SAFE_DELETE(m_pParentDoodadData);
	SAFE_DELETE(m_pParentNpcData);
}

CString KGNpcPropertyEditerDlg::KindIDToText(int nKind, int nType)
{
    CString strKing = _T("?");
    if (nType == PROPERTY_EDIT_TYPE_NPC)
    {
        switch (nKind)
        {
        case ckPlayer :
            strKing = _T("玩家");
            break;
        case ckFightNpc :
            strKing = _T("战斗NPC");
            break;
        case ckDialogNpc :
            strKing = _T("对话NPC");
            break;
        case ckClientNpc :
            strKing = _T("任务NPC");
            break;
        default :
            strKing = _T("无效NPC");
            break;
        }
    }
    else
    {
        switch (nKind)
        {
        case dkNormal :
            strKing = _T("普通");
            break;
        case dkCorpse :
            strKing = _T("尸体");
            break;
        case dkQuest :
            strKing = _T("任务相关");
            break;
        case dkTreasure :
            strKing = _T("宝箱");
            break;
        case dkOrnament :
            strKing = _T("装饰物");
            break;
        case dkClientOnly :
            strKing = _T("dkClientOnly");
            break;
        default :
            strKing = _T("无效Doodad");
            break;
        }
    }
    return strKing;
}

DWORD KGNpcPropertyEditerDlg::TextToKingID(CString strText, int nType)
{
    DWORD dwID = 0x0;
    
    if (nType == PROPERTY_EDIT_TYPE_NPC)
    {
        if (strText == _T("玩家"))
            dwID = ckPlayer;
        else if (strText == _T("战斗NPC"))
            dwID = ckFightNpc;
        else if (strText == _T("对话NPC"))
            dwID = ckDialogNpc;
        else if (strText == _T("任务NPC"))
            dwID = ckClientNpc;
    }
    else
    {
        if (strText == _T("普通"))
            dwID = dkNormal;
        else if (strText == _T("尸体"))
            dwID = dkQuest;
        else if (strText == _T("任务相关"))
            dwID = ckDialogNpc;
        else if (strText == _T("宝箱"))
            dwID = dkTreasure;
        else if (strText == _T("装饰物"))
            dwID = dkOrnament;
        else if (strText == _T("dkClientOnly"))
            dwID = dkClientOnly;
    }
    return dwID;
}

CString KGNpcPropertyEditerDlg::AITypeToText(int byAIType)
{
    CString strKing = _T("?");
    switch (byAIType)
    {
    case 0 :
        strKing = _T("0 无");
        break;
    case 1 :
        strKing = _T("1 发呆");
        break;
    case 2 :
        strKing = _T("2 闲逛");
        break;
    case 3 :
        strKing = _T("3 巡逻");
        break;
    }
    return strKing;
}

DWORD KGNpcPropertyEditerDlg::TextToAIType(CString strText)
{
    DWORD dwID = 0x0;
    if (strText == _T("0 无"))
        dwID = 0;
    else if (strText == _T("1 发呆"))
        dwID = 1;
    else if (strText == _T("2 闲逛"))
        dwID = 2;  
    else if (strText == _T("3 巡逻"))
        dwID = 3;
    else
        dwID = 0;

    return dwID;
}

void KGNpcPropertyEditerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_treeCtrl);
	DDX_Control(pDX, IDC_LIST, m_listCtrl);
}

int KGNpcPropertyEditerDlg::_HighlightItem(int nIndex)
{
    int nResult = false;
    LPKGLISTITEM pItem = NULL;

	pItem = (LPKGLISTITEM)m_listCtrl.GetItemData(nIndex);
    KG_PROCESS_ERROR(pItem);

	pItem->arryItemText.GetAt(1).colorBackSel = RGB(255, 2, 2);
	pItem->arryItemText.GetAt(1).colorBack = RGB(255, 2, 2);

    nResult = true;
Exit0:
    return nResult;
}

void KGNpcPropertyEditerDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 0 && m_lpEditScene)
    {
        m_lpEditScene->FrameMove();
        m_lpEditScene->Render(0);
    }
    return CDialog::OnTimer(nIDEvent);
}

void KGNpcPropertyEditerDlg::BackupNpcDoodadData()
{
	if (m_nType == PROPERTY_EDIT_TYPE_NPC)
	{
		// 清空bak数据
		for (std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iter = m_listNpcDataBak.begin(); iter != m_listNpcDataBak.end(); iter++)
		{
			KSceneEditorDocLogical::NPCInfo* bakNpcInfo = *iter;
			delete bakNpcInfo;
		}
		m_listNpcDataBak.clear();
		SAFE_DELETE(m_plogicNpcDataBak);

		// 备份打开面板时的npc数据们
		for (std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iter = m_listNpcData.begin(); iter != m_listNpcData.end(); iter++)
		{
			KSceneEditorDocLogical::NPCInfo* oldNpcInfo = *iter;
			KSceneEditorDocLogical::NPCInfo* newNpcInfo = new KSceneEditorDocLogical::NPCInfo;
			memcpy(newNpcInfo, oldNpcInfo, sizeof(KSceneEditorDocLogical::NPCInfo));
			m_listNpcDataBak.push_back(newNpcInfo);
		}
		if (m_plogicNpcData != NULL)
		{
			m_plogicNpcDataBak = new KSceneEditorDocLogical::NPCInfo;
			memcpy(m_plogicNpcDataBak, m_plogicNpcData, sizeof(KSceneEditorDocLogical::NPCInfo));
		}
	}
	//else if (m_nType == PROPERTY_EDIT_TYPE_DOODAD)
	{
		// 清空bak-doodad数据
		for (std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator iter = m_listDoodadDataBak.begin(); iter != m_listDoodadDataBak.end(); iter++)
		{
			KSceneEditorDocLogical::DoodadInfo* bakDoodadInfo = *iter;
			delete bakDoodadInfo;
		}
		m_listDoodadDataBak.clear();
		SAFE_DELETE(m_plogicDoodadDataBak);

		// 备份打开面板时的数据们
		for (std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator iter = m_listDoodadData.begin(); iter != m_listDoodadData.end(); iter++)
		{
			KSceneEditorDocLogical::DoodadInfo* oldDoodad = *iter;
			KSceneEditorDocLogical::DoodadInfo* newDoodad = new KSceneEditorDocLogical::DoodadInfo;
			memcpy(newDoodad, oldDoodad, sizeof(KSceneEditorDocLogical::DoodadInfo));
			m_listDoodadDataBak.push_back(newDoodad);
		}
		if (m_plogicDoodadData != NULL)
		{
			m_plogicDoodadDataBak = new KSceneEditorDocLogical::DoodadInfo;
			memcpy(m_plogicDoodadDataBak, m_plogicDoodadData, sizeof(KSceneEditorDocLogical::DoodadInfo));
		}
	}
}

BOOL KGNpcPropertyEditerDlg::OnInitDialog()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
	int nResult  = false;
	int nRetCode = false;
	
	IEKG3DSceneSceneEditor* lpEditScene = NULL;
	KScene*	lpLogcScene = NULL;
	DWORD dwStyle = 0x0;
	int	  nFirstLoop = true;

	nRetCode = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	m_lpEditScene->GetSceneSceneEditor(&lpEditScene);
	KG_PROCESS_ERROR(lpEditScene);


	hRetCode = lpEditScene->GetFirstSelectedObject(&m_pHandelObj);
    //hRetCode = lpEditScene->GetObjectInHandle(&m_pHandelObj);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	KGLOG_PROCESS_ERROR(m_pHandelObj);
	
	//
	// Init Contrals
	//

	m_imageListTree.Create(16, 16, ILC_COLOR24, 4, 4);
	m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_TALK));

	m_treeCtrl.SetImageList(&m_imageListTree, TVSIL_NORMAL);
	m_treeCtrl.SetTextColor(RGB(255, 255, 255));
	m_treeCtrl.SetLineColor(RGB(122, 122, 122)); 
	m_treeCtrl.SetBkColor(RGB(92,  92, 92));

	dwStyle = GetWindowLong(m_treeCtrl.m_hWnd, GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_treeCtrl.m_hWnd, GWL_STYLE, dwStyle);

	m_listCtrl.InsertColumn(0, _T("属性名"));
	m_listCtrl.InsertColumn(1, _T("属性值"));
	m_listCtrl.SetColumnWidth(0, 80);
	m_listCtrl.SetColumnWidth(1, 500);
	m_listCtrl.SetBkColor(RGB(255, 255, 255));

    m_listCtrl.m_lpEditScene = m_lpEditScene;
	

	//
	// Fill Contrals
	//
	IEKG3DSceneLogicalEditorBase* pLogicBase = NULL;
	lpEditScene->GetIELogicScene(&pLogicBase);
	KG_PROCESS_ERROR(pLogicBase);
	pLogicBase->GetLogicScene((LPVOID*)&lpLogcScene);
	KG_PROCESS_ERROR(lpLogcScene);

	//INT nCount = 0;
	m_listNpcData.clear();
	//lpEditScene->GetSelectedEntityCount(&m_nSelCount);

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	lpEditScene->GetSelectionToolInterface(&pSelTool);
	KG_PROCESS_ERROR(NULL != pSelTool);

	m_nSelCount = pSelTool->GetSelectionCount();
	m_listCtrl.m_nSelCount = m_nSelCount;

	IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
	pSelTool->GetAccessProxy(&pProxy);
	KG_PROCESS_ERROR(NULL != pProxy);
	
	if (m_nSelCount > 1 && m_nType == PROPERTY_EDIT_TYPE_NPC)
	{
		m_hItemParent = m_treeCtrl.InsertItem("公共属性");
		m_pParentNpcData = new KSceneEditorDocLogical::NPCInfo;
		m_pParentNpcData->pWayPointSet = NULL;
		m_treeCtrl.SetItemData(m_hItemParent, (DWORD_PTR)m_pParentNpcData);
		//else
		//{
		//	m_pParentDoodadData = new KSceneEditorDocLogical::DoodadInfo;
		//	m_treeCtrl.SetItemData(m_hItemParent, (DWORD_PTR)m_pParentDoodadData);
		//}
	}
	
	//for (int i = 0; i < m_nSelCount; i ++)
	int i = 0;
	for(IEKG3DSceneEntity* pEntity = pProxy->Reset(); NULL != pEntity; pEntity = pProxy->Next(), ++i)
	{
        if (m_nType == PROPERTY_EDIT_TYPE_NPC)
        {
			/*IEKG3DSceneEntity* pEntity = NULL;
			lpEditScene->GetSelectedEntity(i, &pEntity);*/
			
			hRetCode = pEntity->GetType(&EntityType);
			KGLOG_COM_PROCESS_ERROR(hRetCode);
		    if(EntityType==SCENEENTITY_OBJECT)
		    {
				IEKG3DRepresentObject* pObject = NULL;
				hRetCode = pEntity->GetPointer((VOID **)&pObject);
				KGLOG_COM_PROCESS_ERROR(hRetCode);
			    if (pObject->GetType()==REPRESENTOBJECT_NPC)
			    {
				    KSceneEditorDocLogical::NPCInfo* pNPCinfo = NULL;
				    if(SUCCEEDED(m_lpDocLogical->FindNPCInfoByEditorNPC(&pNPCinfo,pObject)))
				    {
					    HTREEITEM hitem = m_treeCtrl.InsertItem(pNPCinfo->szName.c_str(), m_hItemParent);
					    m_treeCtrl.SetItemData(hitem,DWORD_PTR(pObject));
					   	if( m_nSelCount == 1)
						{
							if (nFirstLoop)
							{
								m_plogicNpcData = pNPCinfo;
								//if (m_plogicNpcData->nDisableAutoTurn != 0 && m_plogicNpcData->nDisableAutoTurn != 1) 
								//	m_plogicNpcData->nDisableAutoTurn = 0;
								m_nType = PROPERTY_EDIT_TYPE_NPC;
								FillListCtrl();
								nFirstLoop = false;
							}
						}
						else
						{
							m_listNpcData.push_back(pNPCinfo);
						}
                    }
			    }
				else if (pObject->GetType()==REPRESENTOBJECT_DOODAD)
				{
					KSceneEditorDocLogical::DoodadInfo* pDoodadinfo = NULL;
					if(SUCCEEDED(m_lpDocLogical->FindDoodadInfoByEditorDoodad(&pDoodadinfo,pObject)))
					{
						HTREEITEM hitem = m_treeCtrl.InsertItem(pDoodadinfo->szName.c_str(), m_hItemParent);
						m_treeCtrl.SetItemData(hitem,DWORD_PTR(pObject));
						
						if (m_nSelCount == 1)
						{
							if (nFirstLoop)
							{
								m_plogicDoodadData = pDoodadinfo;
								m_nType = PROPERTY_EDIT_TYPE_DOODAD;
								FillListCtrl();
								nFirstLoop = false;
							}
						}
						else
						{
							m_listDoodadData.push_back(pDoodadinfo);
						}
					}
				}
		    }
        }
		else    // PROPERTY_EDIT_TYPE_DOODAD
		{
			//IEKG3DSceneEntity* pEntity = NULL;
			//lpEditScene->GetSelectedEntity(i, &pEntity);

			hRetCode = pEntity->GetType(&EntityType);
			KGLOG_COM_PROCESS_ERROR(hRetCode);

			if(EntityType == SCENEENTITY_OBJECT)
			{
				IEKG3DRepresentObject* pObject = NULL;
				hRetCode = pEntity->GetPointer((VOID **)&pObject);
				KGLOG_COM_PROCESS_ERROR(hRetCode);

				if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
                {
                    KSceneEditorDocLogical::DoodadInfo* pDoodadinfo = NULL;
                    if(SUCCEEDED(m_lpDocLogical->FindDoodadInfoByEditorDoodad(&pDoodadinfo,pObject)))
                    {
                        HTREEITEM hitem = m_treeCtrl.InsertItem(pDoodadinfo->szName.c_str(), m_hItemParent);
                        m_treeCtrl.SetItemData(hitem,DWORD_PTR(pObject));

						if (m_nSelCount == 1)
						{
							if (nFirstLoop)
							{
								m_plogicDoodadData = pDoodadinfo;
								m_nType = PROPERTY_EDIT_TYPE_DOODAD;
								FillListCtrl();
								nFirstLoop = false;
							}
						}
						else
						{
							m_listDoodadData.push_back(pDoodadinfo);
						}
                    }

                }			
            }
        }
	}

	//if( nFirstLoop && m_listNpcData.size() > 0 && i == m_nSelCount - 1)
	if (m_nType == PROPERTY_EDIT_TYPE_NPC && m_listNpcData.size() > 1)
	{
		FillListCtrlForUniformAttribute();
	}
	
	BackupNpcDoodadData();

	nResult = true;
Exit0:
	if (!lpLogcScene)
	{
		MessageBox(_T("没有绑定到游戏逻辑@_@"), _T("Error!!"), MB_OK);
		EndDialog(0);
	}
	return nResult;
}

BEGIN_MESSAGE_MAP(KGNpcPropertyEditerDlg, CDialog)
	ON_BN_CLICKED(ID_OK, &KGNpcPropertyEditerDlg::OnBnClickedOk)
	ON_BN_CLICKED(ID_CANCEL, &KGNpcPropertyEditerDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_REFRESH,&KGNpcPropertyEditerDlg::OnBnClickedRefresh)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &KGNpcPropertyEditerDlg::OnTvnSelchangedTree)
    ON_WM_TIMER()
END_MESSAGE_MAP()

void KGNpcPropertyEditerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    return CDialog::OnShowWindow(bShow, nStatus);
}

int KGNpcPropertyEditerDlg::FillListCtrlForUniformAttribute()
{
	//to do 去掉没有用的变量
	int nResult = FALSE;
	int nRetCode = FALSE;

	IIniFile*	 pIniFile = NULL;
	LPKGLISTITEM pItem = NULL;
	ITEMTEXT     itemText;

	char szFullPath[MAX_PATH];
	char szFilePath[MAX_PATH];
	char szText[128];
	char szScript[128];

	sprintf(szFilePath, SCTRPT_NAME_FILE);
	g_GetFullPath(szFullPath, szFilePath);
	pIniFile = g_OpenIniFile(szFullPath);

	itemText.colorExpandRectBack     = RGB(255, 255, 255);
	itemText.colorExpandRectBackSel  = RGB(255, 255, 255);
	itemText.colorExpandRectFrame    = RGB(168, 168, 168);
	itemText.colorExpandRectFrameSel = RGB(255, 2, 2);

	if (m_nType == PROPERTY_EDIT_TYPE_NPC)
	{
		
		m_listCtrl.BandNpcData(m_listNpcData, m_lpDocLogical, m_pParentNpcData, m_lpDocLogical->m_mapWayPointSet);
		m_listCtrl.Clear();
		m_listCtrl.m_nType = PROPERTY_EDIT_TYPE_NPC;
		if (m_listNpcData.size() < 1)
			return FALSE;

		// 1. nTempleteID 
		InitItem("NPC模版ID");		

		int nTempleteID = 0;
		BOOL bAllPropAreTheSame = TRUE;
		BOOL bFirstLoop = TRUE;
		CheckNPCPropSameInt(nTempleteID, nTempleteID);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->nTempleteID = nTempleteID;
			itoa(nTempleteID, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->nTempleteID = 0;
			itemText.szItemText = "";
		}
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 2. szName
		InitItem("NPC名字");

		char szName[128];
		CheckNPCPropSameChar(szName, szName);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->szName = szName;
			itemText.szItemText = szName;
		}
		else
		{
			m_pParentNpcData->szName = "";
			itemText.szItemText = "";
		}
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::EDITTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 3. szNickName
		InitItem("NPC别名");
		char szNickName[128];
		CheckNPCPropSameChar(szNickName, szNickName);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->szNickName = szNickName;
			itemText.szItemText = szNickName;
		}
		else
		{
			m_pParentNpcData->szNickName = "";
			itemText.szItemText = "";
		}
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::EDITTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 4. m_nLevel
		InitItem("NPC等级");
		int nLevel = 0;
		CheckNPCPropSameInt(nLevel, nLevel);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->nLevel = nLevel;
			itoa(nLevel, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->nLevel = 0;
			itemText.szItemText = "";
		}
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 5. m_nX
		InitItem("X-坐标");
		int nX = 0;
		CheckNPCPropSameInt(nX, nX);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->nX = nX;
			itoa(nX, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->nX = 0;
			itemText.szItemText = "";
		}
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 6. m_nY
		InitItem("Y-坐标");
		int nY = 0;
		CheckNPCPropSameInt(nY, nY);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->nY = nY;
			itoa(nY, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->nY = 0;
			itemText.szItemText = "";
		}
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 7. m_nZ
		InitItem("Z-坐标");
		int nZ = 0;
		CheckNPCPropSameInt(nZ, nZ);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->nZ = nZ;
			itoa(nZ, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->nZ = 0;
			itemText.szItemText = "";
		}
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 8. nKind
		InitItem("NPC类型");
		int nKind = 0;
		CheckNPCPropSameInt(nKind, nKind);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->nKind = nKind;
			itemText.szItemText = KindIDToText(nKind);
		}
		else
		{
			m_pParentNpcData->nKind = 0;
			itemText.szItemText = "";
		}
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 9. m_dwScriptID
		InitItem("脚本ID");
		DWORD dwScriptID = 0;
		CheckNPCPropSameInt(dwScriptID, dwScriptID);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->dwScriptID = dwScriptID;
			ultoa((int)dwScriptID, szText, 10);
			if (pIniFile)
			{
				nRetCode = pIniFile->GetString(szText, "Name", "", szScript, sizeof(szScript));
				if (nRetCode)
					itemText.szItemText = szScript;
				else
					itemText.szItemText = szText;
			}
			else
				itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->dwScriptID = 0;
			itemText.szItemText = "";
		}

		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::BNBROWSE;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 10. byStandDirection
		InitItem("NPC朝向");
		int nFaceDirection = 0;
		CheckNPCPropSameInt(nFaceDirection, nFaceDirection);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->nFaceDirection = nFaceDirection;
			itoa(nFaceDirection, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->nFaceDirection = 0;
			itemText.szItemText = "";
		}
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = _KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 11. byAIType
		InitItem("AI类型");
		//int nAIType = 0;

		//if (m_nType == PROPERTY_EDIT_TYPE_NPC)
		//{
		//	CheckNPCPropSameInt(nAIType, nAIType);
		//}
		//else //m_nType == PROPERTY_EDIT_TYPE_DOODAD
		//{
		//	CheckDoodadPropSameInt(nAIType, nAIType);
		//}

		//if (bAllPropAreTheSame)
		//{
		//	itoa(nAIType, szText, 10);
		//	itemText.szItemText = szText;
		//}
		//else
		//{
			itemText.szItemText = "";
		//}

		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 12. dwRepresentID
		InitItem("表现ID");
		DWORD dwRepresentID = 0;
		CheckNPCPropSameInt(dwRepresentID, dwRepresentID);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->dwRepresentID = dwRepresentID;
			ultoa(dwRepresentID, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->dwRepresentID = 0;
			itemText.szItemText = "";
		}
		
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 13. Patrol Way
		InitItem("巡逻路径");
		KSceneEditorDocLogical::WayPointSet* pWayPointSet = NULL;
 		bFirstLoop = TRUE;
		bAllPropAreTheSame = TRUE;
		for (std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iter = m_listNpcData.begin(); iter != m_listNpcData.end(); iter++)
		{
			KSceneEditorDocLogical::NPCInfo* npc = *iter;																				
			if (bFirstLoop)																												
			{		
				if (npc->pWayPointSet)
					pWayPointSet = npc->pWayPointSet;
				bFirstLoop = FALSE;																									
				continue;																												
			}
			if (pWayPointSet != npc->pWayPointSet)																					
			{																													
				bAllPropAreTheSame = FALSE;																								
				break;																													
			}				
		}			

		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->pWayPointSet = pWayPointSet;
			if (pWayPointSet)
				itemText.szItemText = pWayPointSet->szName.c_str();
			else
				itemText.szItemText = "";
		}
		else
		{
			m_pParentNpcData->pWayPointSet = NULL;
			itemText.szItemText = "";
		}
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 14. PatrolOrderIndex
		InitItem("PatrolOrderIndex");
		int nPatrolOrderIndex = 0, nPatrolOrderIndex_temp = 0;
		bFirstLoop = TRUE;
		bAllPropAreTheSame = TRUE;
		//for (std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iter = m_listNpcData.begin(); iter != m_listNpcData.end(); iter++)
		//{
		//	KSceneEditorDocLogical::NPCInfo* npc = *iter;																				
		//	if (bFirstLoop)																												
		//	{		
		//		m_lpDocLogical->GetAIParam(npc->m_cAIParameterSet,"PatrolOrderIndex",nPatrolOrderIndex);																		
		//		bFirstLoop = FALSE;																									
		//		continue;																												
		//	}
		//	m_lpDocLogical->GetAIParam(npc->m_cAIParameterSet,"PatrolOrderIndex",nPatrolOrderIndex_temp);
		//	if (nPatrolOrderIndex != nPatrolOrderIndex_temp)																					
		//	{																													
		//		bAllPropAreTheSame = FALSE;																								
		//		break;																													
		//	}																															
		//}			

		//if (bAllPropAreTheSame)
		//{
		//	m_pParentNpcData->m_cAIParameterSet = (*m_listNpcData.begin())->m_cAIParameterSet;
		//	itoa(nPatrolOrderIndex, szText, 10);
		//	itemText.szItemText = szText;
		//}
		//else
		//{
		//	m_pParentNpcData->m_cAIParameterSet.dwID = 0;
		//	m_pParentNpcData->m_cAIParameterSet.szName = "";
		//	m_lpDocLogical->SetAIParam(m_pParentNpcData->m_cAIParameterSet, "PatrolOrderIndex", 0);
			itemText.szItemText = "";
		//}
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		pItem->arryItemText.Add(itemText);
		m_listCtrl.InsertDepend(NULL, pItem);

		// 15. 重生组
		InitItem("重生组");
		DWORD dwNpcReliveID = 0;
		CheckNPCPropSameInt(dwNpcReliveID, dwNpcReliveID);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->dwNpcReliveID = dwNpcReliveID;
			ultoa(dwNpcReliveID, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->dwNpcReliveID = 0;
			itemText.szItemText = "";
		}

		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::BNBROWSE;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 16. 随机组
		InitItem("随机组");
		DWORD dwNpcRandomID = 0;
		CheckNPCPropSameInt(dwNpcRandomID, dwNpcRandomID);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->dwNpcRandomID = dwNpcRandomID;
			ultoa(dwNpcRandomID, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->dwNpcRandomID = 0;
			itemText.szItemText = "";
		}

		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::BNBROWSE;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 17. AI参数
		InitItem("AI参数");
		DWORD dwNpcAISet = 0;
		CheckNPCPropSameInt(dwNpcAISet, dwNpcAISet);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->dwNpcAISet = dwNpcAISet;
			ultoa(dwNpcAISet, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->dwNpcAISet = 0;
			itemText.szItemText = "";
		}

		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::BNBROWSE;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 18. 副本进度ID
		InitItem("副本进度ID");
		int nProgressID = 0;
		CheckNPCPropSameInt(nProgressID, nProgressID);
		if (bAllPropAreTheSame)
		{
			m_pParentNpcData->nProgressID = nProgressID;
			itoa(nProgressID, szText, 10);
			itemText.szItemText = szText;
		}
		else
		{
			m_pParentNpcData->nProgressID = 0;
			itemText.szItemText = "";
		}

		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 19. nSelCount 
		InitItem("被选NPC个数");

		itoa((int)m_listNpcData.size(), szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		m_listCtrl.InsertDepend(NULL, pItem);

		if ((int)m_listNpcData.size() == 0)
		{
			nResult = TRUE;
			return nResult;
		}

		nResult = TRUE;
	}
	//else
	//{
	//	m_listCtrl.BandDoodadData(m_listDoodadData, m_lpDocLogical, m_pParentDoodadData);
	//	m_listCtrl.Clear();
	//	m_listCtrl.m_nType = PROPERTY_EDIT_TYPE_DOODAD;
	//	if (m_listDoodadData.size() < 1)
	//		return FALSE;
	//	// 1. nTempleteID 
	//	InitItem("DOODAD模版ID");
	//	int nTempleteID = 0;
	//	BOOL bAllPropAreTheSame = TRUE;
	//	BOOL bFirstLoop = TRUE;
	//	CheckDoodadPropSameInt(nTempleteID, nTempleteID);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->nTempleteID = nTempleteID; 
	//		itoa(nTempleteID, szText, 10);
	//		itemText.szItemText = szText;
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->nTempleteID = 0;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::SPINTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 2. szName
	//	InitItem("DOODAD名字");
	//	char szName[128];
	//	CheckDoodadPropSameChar(szName, szName);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->szName = szName;
	//		itemText.szItemText = szName;
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->szName = "";
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::EDITTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 3. szNickName
	//	InitItem("DOODAD别名");
	//	char szNickName[128];
	//	CheckDoodadPropSameChar(szNickName, szNickName);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->szNickName = szNickName;
	//		itemText.szItemText = szNickName;
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->szNickName = "";
	//		itemText.szItemText = szNickName;
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::EDITTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 4. m_nLevel
	//	InitItem("DOODAD等级");
	//	int nLevel = 0;
	//	CheckDoodadPropSameInt(nLevel, nLevel);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->nLevel = nLevel;
	//		itoa(nLevel, szText, 10);
	//		itemText.szItemText = szText;
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->nLevel = 0;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::SPINTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 5. m_nX
	//	InitItem("X-坐标");
	//	int nX = 0;
	//	CheckDoodadPropSameInt(nX, nX);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->nX = nX;
	//		itoa(nX, szText, 10);
	//		itemText.szItemText = szText;
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->nX = 0;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::SPINTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 6. m_nY
	//	InitItem("Y-坐标");
	//	int nY = 0;
	//	CheckDoodadPropSameInt(nY, nY);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->nY = nY;
	//		itoa(nY, szText, 10);
	//		itemText.szItemText = szText;
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->nY = 0;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::SPINTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 7. m_nZ
	//	InitItem("Z-坐标");
	//	int nZ = 0;
	//	CheckDoodadPropSameInt(nZ, nZ);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->nZ = nZ;
	//		itoa(nX, szText, 10);
	//		itemText.szItemText = szText;
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->nZ = 0;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::SPINTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 8. nKind
	//	InitItem("DOODAD类型");
	//	int nKind = 0;
	//	CheckDoodadPropSameInt(nKind, nKind);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->nKind = nKind;
	//		itemText.szItemText = KindIDToText(nKind, PROPERTY_EDIT_TYPE_DOODAD);
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->nKind = 0;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::SPINTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 9. m_dwScriptID
	//	InitItem("脚本ID");
	//	DWORD dwScriptID = 0;
	//	CheckDoodadPropSameInt(dwScriptID, dwScriptID);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->dwScriptID = dwScriptID;
	//		ultoa(dwScriptID, szText, 10);
	//		if (pIniFile)
	//		{
	//			nRetCode = pIniFile->GetString(szText, "Name", "", szScript, sizeof(szScript));
	//			if (nRetCode)
	//				itemText.szItemText = szScript;
	//			else
	//				itemText.szItemText = szText;
	//		}
	//		else
	//			itemText.szItemText = szText;
	//	}
	//	else 
	//	{
	//		m_pParentDoodadData->dwScriptID = 0;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::BNBROWSE;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 10. nFaceDirection
	//	InitItem("DOODAD朝向");
	//	int nFaceDirection = 0;
	//	CheckDoodadPropSameInt(nFaceDirection, nFaceDirection);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->nFaceDirection = nFaceDirection;
	//		itoa(nFaceDirection, szText, 10);
	//		itemText.szItemText = szText;
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->nFaceDirection = 0;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::SPINTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 11. byAIType
	//	InitItem("AI类型");
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::COMBOX;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 12. 重生组
	//	InitItem("重生组");
	//	DWORD dwDoodadReliveID = 0;
	//	CheckDoodadPropSameInt(dwDoodadReliveID, dwDoodadReliveID);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->dwDoodadReliveID = dwDoodadReliveID;
	//		itoa(dwDoodadReliveID, szText, 10);
	//		itemText.szItemText = szText;
	//	}
	//	else
	//	{	
	//		m_pParentDoodadData->dwDoodadReliveID = 0;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::BNBROWSE;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 13. 是否读模板表的动态障碍
	//	InitItem("是否读模板表的动态障碍");
	//	BOOL bUseDefaultObstacle = FALSE;
	//	CheckDoodadPropSameInt(bUseDefaultObstacle, bUseDefaultObstacle);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->bUseDefaultObstacle = bUseDefaultObstacle;
	//		itemText.szItemText =bUseDefaultObstacle ? "1 是" : "0 否";
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->bUseDefaultObstacle = FALSE;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::COMBOX;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 14. 当不读模板表时 是否生成动态障碍
	//	InitItem("不读模板表时，是否生成动态障碍");
	//	BOOL bGenerateObstacle = FALSE;
	//	CheckDoodadPropSameInt(bGenerateObstacle, bGenerateObstacle);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->bGenerateObstacle = bGenerateObstacle;
	//		itemText.szItemText = bGenerateObstacle ? "1 是" : "0 否";
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->bGenerateObstacle = FALSE;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::COMBOX;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 15. 副本进度ID
	//	InitItem("副本进度ID");
	//	int nProgressID = 0;
	//	CheckDoodadPropSameInt(nProgressID, nProgressID);
	//	if (bAllPropAreTheSame)
	//	{
	//		m_pParentDoodadData->nProgressID = nProgressID;
	//		itoa(nProgressID, szText, 10);
	//		itemText.szItemText = szText;
	//	}
	//	else
	//	{
	//		m_pParentDoodadData->nProgressID = 0;
	//		itemText.szItemText = "";
	//	}
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::SPINTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);

	//	// 16. 选中个数
	//	InitItem("选中doodad个数");
	//	int nCount = (int)m_listDoodadData.size();
	//	itoa(nCount, szText, 10);
	//	itemText.szItemText = szText;
	//	pItem->arryItemText.Add(itemText);
	//	pItem->nItemType = KGLISTITEM::SPINTEXT;
	//	m_listCtrl.InsertDepend(NULL, pItem);
	//	nResult = TRUE;		
	//}
	return nResult;
}

int KGNpcPropertyEditerDlg::FillListCtrl()	
{
	int nResult  = false;
	int nRetCode = false;

	IIniFile*	 pIniFile = NULL;
	LPKGLISTITEM pItem = NULL;
	ITEMTEXT     itemText;
	
    char szFullPath[MAX_PATH];
	char szFilePath[MAX_PATH];
	char szText[128];
	char szScript[128];

	sprintf(szFilePath, SCTRPT_NAME_FILE);
	g_GetFullPath(szFullPath, szFilePath);
	pIniFile = g_OpenIniFile(szFullPath);

	itemText.colorExpandRectBack     = RGB(255, 255, 255);
	itemText.colorExpandRectBackSel  = RGB(255, 255, 255);
	itemText.colorExpandRectFrame    = RGB(168, 168, 168);
	itemText.colorExpandRectFrameSel = RGB(255, 2, 2);


	if (m_nType == PROPERTY_EDIT_TYPE_NPC)
	{
		m_listCtrl.BandNpcData(m_plogicNpcData,m_lpDocLogical->m_mapWayPointSet,m_lpDocLogical);
		m_listCtrl.Clear();
		m_listCtrl.m_nType = PROPERTY_EDIT_TYPE_NPC;


		// 1. nTempleteID 
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("NPC模版ID");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(240, 240, 240);
		itemText.colorBackSel = itemText.colorBack;
		itoa(m_plogicNpcData->nTempleteID, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 2. szName
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("NPC名字");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = m_plogicNpcData->szName.c_str();
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::EDITTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 3. szNickName
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("NPC别名");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = m_plogicNpcData->szNickName.c_str();
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::EDITTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 4. m_nLevel
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("NPC等级");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)m_plogicNpcData->nLevel, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 5. m_nX
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("X-坐标");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)m_plogicNpcData->nX, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 6. m_nY
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("Y-坐标");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)m_plogicNpcData->nY, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 7. m_nZ
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("Z-坐标");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)m_plogicNpcData->nZ, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 8. nKind
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("NPC类型");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = KindIDToText(m_plogicNpcData->nKind);
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 9. m_dwScriptID
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("脚本ID");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		ultoa((int)m_plogicNpcData->dwScriptID, szText, 10);
		if (pIniFile)
		{
			nRetCode = pIniFile->GetString(szText, "Name", "", szScript, sizeof(szScript));
			if (nRetCode)
				itemText.szItemText = szScript;
			else
				itemText.szItemText = szText;
		}
		else
			itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::BNBROWSE;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 10. byStandDirection
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("NPC朝向");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)m_plogicNpcData->nFaceDirection, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = _KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 11. byAIType
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("AI类型");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		//itemText.szItemText = AITypeToText(m_plogicNpcData->nAIMainState);
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 12. dwRepresentID
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("表现ID");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		ultoa(m_plogicNpcData->dwRepresentID, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 13. Patrol Way
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("巡逻路径");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		if (m_plogicNpcData->pWayPointSet)
			strcpy(szText,m_plogicNpcData->pWayPointSet->szName.c_str());
		else
			szText[0] ='\0';
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 14. byStandDirection
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("PatrolOrderIndex");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		int nPatrolOrderIndex = 0;
		m_lpDocLogical->GetAIParam(m_plogicNpcData->m_cAIParameterSet,"PatrolOrderIndex",nPatrolOrderIndex);
		itoa(nPatrolOrderIndex, szText, 10);
		itemText.szItemText = szText;
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		pItem->arryItemText.Add(itemText);
		m_listCtrl.InsertDepend(NULL, pItem);

		// 15. 重生组
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("重生组");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		ultoa(m_plogicNpcData->dwNpcReliveID, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::BNBROWSE;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 16. 随机组
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("随机组");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		ultoa(m_plogicNpcData->dwNpcRandomID, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::BNBROWSE;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 17. AI参数
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("AI参数");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		ultoa(m_plogicNpcData->dwNpcAISet, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::BNBROWSE;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 18. 副本进度ID
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("副本进度ID");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa(m_plogicNpcData->nProgressID, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

	}
	else // DOODAD
	{
		if (m_plogicDoodadData == NULL)
			return 0;

		m_listCtrl.BandDoodadData(m_plogicDoodadData, m_lpDocLogical);
		m_listCtrl.Clear();
		m_listCtrl.m_nType = PROPERTY_EDIT_TYPE_DOODAD;

		KSceneEditorDocLogical::DoodadInfo* pDoodad = m_plogicDoodadData;

		// 1. nTempleteID 
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("DOODAD模版ID");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(240, 240, 240);
		itemText.colorBackSel = itemText.colorBack;
		itoa(pDoodad->nTempleteID, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 2. szName
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("DOODAD名字");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = pDoodad->szName.c_str();
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::EDITTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 3. szNickName
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("DOODAD别名");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = pDoodad->szNickName.c_str();
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::EDITTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 4. m_nLevel
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("DOODAD等级");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)pDoodad->nLevel, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 5. m_nX
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("X-坐标");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)pDoodad->nX, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 6. m_nY
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("Y-坐标");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)pDoodad->nY, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 7. m_nX
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("Z-坐标");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)pDoodad->nZ, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 8. nKind
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("DOODAD类型");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = KindIDToText(pDoodad->nKind, PROPERTY_EDIT_TYPE_DOODAD);
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 9. m_dwScriptID
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("脚本ID");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		ultoa((int)pDoodad->dwScriptID, szText, 10);
		if (pIniFile)
		{
			nRetCode = pIniFile->GetString(szText, "Name", "", szScript, sizeof(szScript));
			if (nRetCode)
				itemText.szItemText = szScript;
			else
				itemText.szItemText = szText;
		}
		else
			itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::BNBROWSE;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 10. byStandDirection
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("DOODAD朝向");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)pDoodad->nFaceDirection, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 11. byAIType
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("AI类型");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		//itemText.szItemText = AITypeToText(pDoodad->nAIMainState);
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 12. 重生组
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("重生组");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)pDoodad->dwDoodadReliveID, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::BNBROWSE;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 13. 是否读模板表的动态障碍
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("是否读模板表的动态障碍");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = pDoodad->bUseDefaultObstacle ? "1 是" : "0 否";
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 14. 当不读模板表时 是否生成动态障碍
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("不读模板表时，是否生成动态障碍");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = pDoodad->bGenerateObstacle ? "1 是" : "0 否";
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::COMBOX;
		m_listCtrl.InsertDepend(NULL, pItem);

		// 15. 副本进度ID
		pItem = new KGLISTITEM();
		itemText.colorBack  = RGB(128, 128, 192);
		itemText.colorBackSel = itemText.colorBack;
		itemText.szItemText = _T("副本进度ID");
		pItem->arryItemText.Add(itemText);
		itemText.colorBack  = RGB(255, 255, 255);
		itemText.colorBackSel = itemText.colorBack;
		itoa((int)pDoodad->nProgressID, szText, 10);
		itemText.szItemText = szText;
		pItem->arryItemText.Add(itemText);
		pItem->nItemType = KGLISTITEM::SPINTEXT;
		m_listCtrl.InsertDepend(NULL, pItem);
	}

    
	nResult = true;
//Exit0:
	SAFE_RELEASE(pIniFile);
	return nResult;
}


KGListCtrlNPC::KGListCtrlNPC()
{
    m_lpEditScene = NULL;
	//m_pNpcData = NULL;
    //m_plogicDoodadData = NULL;
    m_nType = PROPERTY_EDIT_TYPE_NPC;
	m_nSelCount = 0;
	m_lpDocLogical = NULL;
}
KGListCtrlNPC::~KGListCtrlNPC()
{

}

int KGListCtrlNPC::BandNpcData(KSceneEditorDocLogical::NPCInfo* pNpcData,KSceneEditorDocLogical* pDoc)
{
	m_pNpcData = pNpcData;
    m_nType = PROPERTY_EDIT_TYPE_NPC;
	m_lpDocLogical = pDoc;
	return true;
}

int KGListCtrlNPC::BandDoodadData(KSceneEditorDocLogical::DoodadInfo* pDoodadData,KSceneEditorDocLogical* pDoc)
{
	m_pDoodadData = pDoodadData;
	m_nType = PROPERTY_EDIT_TYPE_DOODAD;
	m_lpDocLogical = pDoc;
	return true;
}

int KGListCtrlNPC::BandDoodadData(list<KSceneEditorDocLogical::DoodadInfo*>& pListDoodadData,KSceneEditorDocLogical* pDoc, KSceneEditorDocLogical::DoodadInfo* pParentDoodadData)
{
	m_pDoodadData = pParentDoodadData;
	m_listDoodadData = pListDoodadData;
	m_nType = PROPERTY_EDIT_TYPE_DOODAD;
	m_lpDocLogical = pDoc;
	return true;
}
int KGListCtrlNPC::BandNpcData(KSceneEditorDocLogical::NPCInfo* pNpcData,map<DWORD,KSceneEditorDocLogical::WayPointSet*> pmapWayPointSet,KSceneEditorDocLogical* pDoc)
{
	m_pNpcData = pNpcData;
	m_mapWayPointSet = pmapWayPointSet;
	m_nType = PROPERTY_EDIT_TYPE_NPC;
	m_lpDocLogical = pDoc;
	return true;
}
int KGListCtrlNPC::BandNpcData(list<KSceneEditorDocLogical::NPCInfo*>& pListNpcData,KSceneEditorDocLogical* pDoc, KSceneEditorDocLogical::NPCInfo* pNpcData, map<DWORD,KSceneEditorDocLogical::WayPointSet*> pmapWayPointSet)
{
	m_pNpcData = pNpcData;
	m_listNpcData = pListNpcData;
	m_mapWayPointSet = pmapWayPointSet;
	m_nType = PROPERTY_EDIT_TYPE_NPC;
	m_lpDocLogical = pDoc;
	return true;
}

int KGListCtrlNPC::Clear()
{
    if (::IsWindow(m_bnBrowse.m_hWnd))
        m_bnBrowse.ShowWindow(SW_HIDE);
    if (::IsWindow(m_comboxCtrl.m_hWnd))
        m_comboxCtrl.ShowWindow(SW_HIDE);
    if (::IsWindow(m_spinCtrl.m_hWnd))
        m_spinCtrl.ShowWindow(SW_HIDE);
    if (::IsWindow(m_EditText.m_hWnd))
        m_EditText.ShowWindow(SW_HIDE);
	RemoveAll();

    return TRUE;
}

BEGIN_MESSAGE_MAP(KGListCtrlNPC, KGListCtrl)
	ON_NOTIFY(UDN_DELTAPOS, ID_SP_SHOW, OnDeltaposSpin)
	ON_CBN_SELCHANGE(ID_CB_SHOW, OnCbnSelect)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
    ON_EN_CHANGE(ID_ED_SHOW, OnEnChangeEdit)
	ON_BN_CLICKED(CTL_BROWSE, OnBrowseClick)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void KGListCtrlNPC::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_DELETE)
    {        
        IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
        LPKGLISTITEM   pItem = (LPKGLISTITEM)GetItemData(m_nCurItem);
        IEKG3DSceneSceneEditor* lpEditScene = NULL;
        m_lpEditScene->GetSceneSceneEditor(&lpEditScene);

        if(m_nType == PROPERTY_EDIT_TYPE_NPC)
        {
            ITEMTEXT itemText;
            itemText = pItem->arryItemText.GetAt(0);

            DWORD dwScriptID = 0;
            string szFileNam = "";


            if(_strcmpi(itemText.szItemText,"脚本ID")==0)
            {
                pItem->arryItemText.GetAt(1).szItemText = "";
                Update(pItem);

                if (m_nType == PROPERTY_EDIT_TYPE_NPC)
                {
                    if (this->GetItemCount() == PARENT_LISTITEMCOUNT)
                    {
                        list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_listNpcData.begin();
                        while (i!=m_listNpcData.end())
                        {
                            KSceneEditorDocLogical::NPCInfo* pNpcData = *i;
                            pNpcData->dwScriptID = dwScriptID;
                            pNpcData->szScriptName = szFileNam;
                            i++;
                        }
                    }
                    else if (m_pNpcData)
                    {
                        m_pNpcData->dwScriptID = dwScriptID;
                        m_pNpcData->szScriptName = szFileNam;
                    }
                }
                else
                {
					if (m_nSelCount > 1)
					{
						list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_listDoodadData.begin();
						list<KSceneEditorDocLogical::DoodadInfo*>::iterator iend = m_listDoodadData.end();
						while (i!=iend)
						{
							KSceneEditorDocLogical::DoodadInfo* pDoodadData = *i;

							if (pDoodadData)
							{
								pDoodadData->dwScriptID = dwScriptID;
								pDoodadData->szScriptName = szFileNam;
							}

							i++;
						}				
					}
					else if (m_pDoodadData != NULL)
					{
						m_pDoodadData->dwScriptID = dwScriptID;
						m_pDoodadData->szScriptName = szFileNam;
					}
                }
            }
        }
    }

    return KGListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void KGListCtrlNPC::OnEnChangeEdit()
{
    CString strValue;
    int nValue;
    m_EditText.GetWindowText(strValue);
    sscanf(strValue.GetBuffer(), "%d", &nValue);

    if (m_nType == PROPERTY_EDIT_TYPE_NPC)
    {
		if (this->GetItemCount() != PARENT_LISTITEMCOUNT)
		{
			switch (m_nCurItem)
			{
			case 0 :
				m_pNpcData->nTempleteID = nValue;
				break;
			case 3 :
				m_pNpcData->nLevel = nValue;
				break;
			case 4 :
				m_pNpcData->nX = nValue;
				break;
			case 5 :
				m_pNpcData->nY = nValue;
				break;
			case 6 :
				m_pNpcData->nZ = nValue;
				break;
			//case 9 :
			//	m_pNpcData->nFaceDirection = (BYTE)nValue;
			//	break;
			case 1 :
				m_pNpcData->szName = strValue;
				break;
			case 2 :
				m_pNpcData->szNickName = strValue;
				break;
			case 13:
				{
					m_lpDocLogical->SetAIParam(m_pNpcData->m_cAIParameterSet,"PatrolOrderIndex",nValue);
				}
				break;
			/*case 17:
				m_pNpcData->nProgressID = nValue;
				break;*/
			default :
				return;
			}
		}//end of if (this->GetItemCount() != PARENT_LISTITEMCOUNT)
		else
		{
	//		switch (m_nCurItem)
	//		{
	//		case 17:
	//			for (std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iter = m_listNpcData.begin(); iter != m_listNpcData.end(); iter++)
	//			{
	//				KSceneEditorDocLogical::NPCInfo* npc = *iter;
	//				npc->nProgressID = nValue;
	//			}
	//			for (std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator iter = m_listDoodadData.begin(); iter != m_listDoodadData.end(); iter++)
	//			{
	//				KSceneEditorDocLogical::DoodadInfo* doodad = *iter;
	//				doodad->nProgressID = nValue;
	//			}
	//			break;
	//		case 9:
	//			for (std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iter = m_listNpcData.begin(); iter != m_listNpcData.end(); iter++)
	//			{
	//				KSceneEditorDocLogical::NPCInfo* npc = *iter;
	//				npc->nFaceDirection = (BYTE)nValue;
	//			}
	//			break;
	///*		case 13:
	//			for (std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iter = m_listNpcData.begin(); iter != m_listNpcData.end(); iter++)
	//			{
	//				KSceneEditorDocLogical::NPCInfo* npc = *iter;
	//				m_lpDocLogical->SetAIParam(npc->m_cAIParameterSet,"PatrolOrderIndex",nValue);
	//			}
	//			break;*/
	//		default:
	//			return;
	//		}
		}
    }
    else//doodad
    {
		if (m_nSelCount == 1)
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodad = m_pDoodadData;
			switch (m_nCurItem)
			{
			case 0 :
				pDoodad->nTempleteID = nValue;
				break;
			case 3 :
				pDoodad->nLevel = nValue;
				break;
			case 4 :
				pDoodad->nX = nValue;
				break;
			case 5 :
				pDoodad->nY = nValue;
				break;
			case 6 :
				pDoodad->nZ = nValue;
				break;
			case 9 :
				pDoodad->nFaceDirection = (BYTE)nValue;
				break;
			case 1 :
				pDoodad->szName = strValue;
				break;
			case 2 :
				pDoodad->szNickName = strValue;
				break;
			case 14:
				pDoodad->nProgressID = nValue;
				break;
			default :
				return;
			}
		}
		else
		{
			list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_listDoodadData.begin();
			list<KSceneEditorDocLogical::DoodadInfo*>::iterator iend = m_listDoodadData.end();
			while (i!=iend)
			{
				KSceneEditorDocLogical::DoodadInfo* pDoodad = *i;

				switch (m_nCurItem)
				{
				case 0 :
					pDoodad->nTempleteID = nValue;
					break;
				case 3 :
					pDoodad->nLevel = nValue;
					break;
				case 4 :
					pDoodad->nX = nValue;
					break;
				case 5 :
					pDoodad->nY = nValue;
					break;
				case 6 :
					pDoodad->nZ = nValue;
					break;
				case 9 :
					pDoodad->nFaceDirection = (BYTE)nValue;
					break;
				case 1 :
					pDoodad->szName = strValue;
					break;
				case 2 :
					pDoodad->szNickName = strValue;
					break;
				case 14:
					pDoodad->nProgressID = nValue;
					break;
				default :
					return;
				}

				i++;
			}
		}
    }

	LPKGLISTITEM   pItem = (LPKGLISTITEM)GetItemData(m_nCurItem);
    pItem->arryItemText.GetAt(1).szItemText = strValue;
}

BOOL KGListCtrlNPC::DlgModifyAISet(KSceneEditorDocLogical::NPCInfo* pNpc)
{
	KSceneCommonParamDialog ParamDlg;
	int nRetParamIndex[30];

	KSceneEditorDocLogical::AIParameterSet& aiset = pNpc->m_cAIParameterSet;

	for (size_t i=0;i<aiset.vecParameters.size();i++)
	{
		KSceneEditorDocLogical::AIParameter& para = aiset.vecParameters[i];

		nRetParamIndex[i] = ParamDlg.AddAnParam(para.strName.c_str(),para.strValue,FALSE);
	}

	if (IDOK != ParamDlg.DoModal())
		return FALSE;

	for (size_t i=0;i<aiset.vecParameters.size();i++)
	{
		KSceneEditorDocLogical::AIParameter& para = aiset.vecParameters[i];

		ParamDlg.GetData(nRetParamIndex[i], para.strValue);
	}

	pNpc->dwNpcAISet = m_lpDocLogical->FindAIParameterSet(pNpc->nTempleteID,aiset);

	return TRUE;
}

void KGListCtrlNPC::OnBrowseClick()
{
	int nResult  = false;
	int nRetCode = false;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	LPKGLISTITEM   pItem = (LPKGLISTITEM)GetItemData(m_nCurItem);
	IEKG3DSceneSceneEditor* lpEditScene = NULL;
	m_lpEditScene->GetSceneSceneEditor(&lpEditScene);

	if (m_nType == PROPERTY_EDIT_TYPE_NPC)
	{
		ITEMTEXT     itemText;

		itemText = pItem->arryItemText.GetAt(0);

		if(_strcmpi(itemText.szItemText,"重生组")==0)
		{
			KSceneEditorReviveDialog dlg;
			dlg.SetDocLogical(m_lpDocLogical,TRUE);

			if(dlg.DoModal()==IDOK)
			{
				if (this->GetItemCount() == PARENT_LISTITEMCOUNT)
				{
					list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_listNpcData.begin();
					while (i!=m_listNpcData.end())
					{
						KSceneEditorDocLogical::NPCInfo* pNpcData = *i;
						pNpcData->dwNpcReliveID = dlg.m_dwCurrentSelectedID;

						if(pNpcData->pPointer)
						{
							IEKG3DRepresentNPC* pRepNPC = lpEditScene->GetIEKG3DRepresentNPC(pNpcData->pPointer);
							if(pRepNPC)
								pRepNPC->SetReliveID(dlg.m_dwCurrentSelectedID);
						}
						i++;
					}
				}
				else if (m_pNpcData)
				{
					m_pNpcData->dwNpcReliveID = dlg.m_dwCurrentSelectedID;
					if(m_pNpcData->pPointer)
					{
						IEKG3DRepresentNPC* pRepNPC = lpEditScene->GetIEKG3DRepresentNPC(m_pNpcData->pPointer);
						if(pRepNPC)
							pRepNPC->SetReliveID(dlg.m_dwCurrentSelectedID);
					}
				}
			}
			return ;
		}
		else if(_strcmpi(itemText.szItemText,"随机组")==0)
		{
			KSceneEditorRandomGroupDialog dlg;
			dlg.SetSceneEditor(lpEditScene,m_lpDocLogical);

			if(dlg.DoModal()==IDOK)
			{
				if (this->GetItemCount() == PARENT_LISTITEMCOUNT)
				{
					list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_listNpcData.begin();
					while (i!=m_listNpcData.end())
					{
						KSceneEditorDocLogical::NPCInfo* pNpcData = *i;
						pNpcData->dwNpcRandomID = dlg.m_dwSelectedGroupID;

						if(pNpcData->pPointer)
						{
							IEKG3DRepresentNPC* pRepNPC = lpEditScene->GetIEKG3DRepresentNPC(pNpcData->pPointer);
							if(pRepNPC)
								pRepNPC->SetRandomID(dlg.m_dwSelectedGroupID);
						}
						i++;
					}
				}
				else if (m_pNpcData)
				{
					m_pNpcData->dwNpcRandomID = dlg.m_dwSelectedGroupID;
					if(m_pNpcData->pPointer)
					{
						IEKG3DRepresentNPC* pRepNPC = lpEditScene->GetIEKG3DRepresentNPC(m_pNpcData->pPointer);
						if(pRepNPC)
							pRepNPC->SetRandomID(dlg.m_dwSelectedGroupID);
					}
				}
			}
			return ;
		}
		else if (_strcmpi(itemText.szItemText,"AI参数")==0)
		{
			m_lpDocLogical->UpdataOneNpcAIParameters(m_pNpcData,m_lpDocLogical->m_bUseNpcWayPointSetPointer);
			
			KSceneEditorAISetDialog dlg;
			dlg.SetSceneEditor(lpEditScene,m_lpDocLogical,m_pNpcData->nTempleteID,m_pNpcData);
			dlg.m_listNpcToModify = m_listNpcData;

			if(dlg.DoModal()==IDOK)
			{
				if (this->GetItemCount() == PARENT_LISTITEMCOUNT)
				{
					dlg.ApplyModiyToNpcList(TRUE);
				}
				else
				{
					dlg.ApplyModiyToNpcList(FALSE);
				}
			}

			/*if(pNpc)
			{
				if(DlgModifyAISet(pNpc))
				{
					if (m_nSelCount>1)
					{
						list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_listNpcData.begin();
						while (i!=m_listNpcData.end())
						{
							KSceneEditorDocLogical::NPCInfo* pNpcData = *i;
							pNpcData->dwNpcAISet = pNpc->dwNpcAISet;
							pNpcData->m_cAIParameterSet = pNpc->m_cAIParameterSet;

							if(pNpcData->pPointer)
							{
								IEKG3DRepresentNPC* pRepNPC = lpEditScene->GetIEKG3DRepresentNPC(pNpcData->pPointer);
								if(pRepNPC)
									pRepNPC->SetAISetID(pNpc->dwNpcAISet);
							}
							i++;
						}
					}
					else if (m_pNpcData)
					{
						m_pNpcData->dwNpcAISet = pNpc->dwNpcAISet;
						m_pNpcData->m_cAIParameterSet = pNpc->m_cAIParameterSet;
						if(m_pNpcData->pPointer)
						{
							IEKG3DRepresentNPC* pRepNPC = lpEditScene->GetIEKG3DRepresentNPC(m_pNpcData->pPointer);
							if(pRepNPC)
								pRepNPC->SetAISetID(pNpc->dwNpcAISet);
						}
					}
				}
			}*/

			return ;
		}
	}
	else//Doodad
	{
		ITEMTEXT     itemText;

		itemText = pItem->arryItemText.GetAt(0);

		if (_strcmpi(itemText.szItemText, "重生组") == 0)
		{
			KSceneEditorReviveDialog dlg;
			dlg.SetDocLogical(m_lpDocLogical,FALSE);

			if(dlg.DoModal()==IDOK)
			{
				if (m_nSelCount == 1)
				{
					KSceneEditorDocLogical::DoodadInfo* pDoodadData = m_pDoodadData;

					if (pDoodadData)
					{
						pDoodadData->dwDoodadReliveID = dlg.m_dwCurrentSelectedID;
						if(pDoodadData->pPointer)
						{
							IEKG3DRepresentDoodad* pRepDoodad = lpEditScene->GetIEKG3DRepresentDoodad(pDoodadData->pPointer);
							if(pRepDoodad)
								pRepDoodad->SetReliveID(dlg.m_dwCurrentSelectedID);
						}
					}	
				}
				else
				{
					list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_listDoodadData.begin();
					list<KSceneEditorDocLogical::DoodadInfo*>::iterator iend = m_listDoodadData.end();
					while (i!=iend)
					{
						KSceneEditorDocLogical::DoodadInfo* pDoodadData = *i;

						if (pDoodadData)
						{
							pDoodadData->dwDoodadReliveID = dlg.m_dwCurrentSelectedID;
							if(pDoodadData->pPointer)
							{
								IEKG3DRepresentDoodad* pRepDoodad = lpEditScene->GetIEKG3DRepresentDoodad(pDoodadData->pPointer);
								if(pRepDoodad)
									pRepDoodad->SetReliveID(dlg.m_dwCurrentSelectedID);
							}
						}		

						i++;
					}
				}
			}
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	TCHAR szAppDir[MAX_PATH];
	IIniFile*	pIniFile = NULL;
	char szDefPath[MAX_PATH];
	char szFileNam[MAX_PATH];
	DWORD dwScriptID = 0x0;

	char szFullPath[MAX_PATH];
	char szFilePath[MAX_PATH];


	sprintf(szFilePath, SCTRPT_NAME_FILE);
	g_GetFullPath(szFullPath, szFilePath);
	pIniFile = g_OpenIniFile(szFullPath, false, true);

	g_GetFullPath(szDefPath, _T("Scripts\\"));
	CFileDialog fileDlg(
		true, szDefPath, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"脚本文件|*.lua|所有文件|*.*|",this
	);
	fileDlg.m_ofn.lStructSize = sizeof(fileDlg.m_ofn);
	fileDlg.m_ofn.lpstrTitle  = "选择脚本文件";
	fileDlg.m_ofn.lpstrInitialDir = szDefPath;

	KG_PROCESS_ERROR(pItem->nItemType == KGLISTITEM::BNBROWSE);
	if (fileDlg.DoModal() == IDOK)
	{
		CString strFullPath = fileDlg.GetPathName();
        g_pEngineManager->GetDefWorkDirectory(szAppDir, MAX_PATH);
		if (strFullPath.Find(szAppDir) == -1)
		{
            TCHAR szInfo[512];
            sprintf(
                szInfo, 
                TEXT("%s\n%s\n%s"),
                strFullPath.GetBuffer(),
                szAppDir,
                TEXT("只能选择在程序目录中的脚本文件!!")
                );
			::MessageBox(this->m_hWnd, szInfo, "Error", MB_OK);
			KG_PROCESS_ERROR(FALSE);
		}
		else
		{
			char szFullName[MAX_PATH];
			strncpy_s(
				szFullName, strFullPath.GetBuffer(), sizeof(szFileNam)
			);
			g_GetFilePathFromFullPath(szFileNam, szFullName);
		}

		if(_strcmpi(szFileNam,"")==0)
			dwScriptID = 0;
		else
			dwScriptID = g_FileNameHash(szFileNam);

		ultoa(dwScriptID, szDefPath, 10);
		pItem->arryItemText.GetAt(1).szItemText = szFileNam;
		Update(pItem);

		if (pIniFile)
		{
			char szFilePath[MAX_PATH];
			char szFullPath[MAX_PATH];
			strncpy_s(
				szFullPath, fileDlg.GetPathName().GetBuffer(), sizeof(szFullPath)
			);
			g_GetFilePathFromFullPath(szFilePath, szFullPath);
			pIniFile->WriteString(szDefPath, "Name", szFilePath);

			sprintf(szFilePath, SCTRPT_NAME_FILE);
			g_GetFullPath(szFullPath, szFilePath);
			pIniFile->Save(szFullPath);
		}

        if (m_nType == PROPERTY_EDIT_TYPE_NPC)
        {
			if (this->GetItemCount() == PARENT_LISTITEMCOUNT)
			{
				list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_listNpcData.begin();
				while (i!=m_listNpcData.end())
				{
					KSceneEditorDocLogical::NPCInfo* pNpcData = *i;
					pNpcData->dwScriptID = dwScriptID;
					pNpcData->szScriptName = szFileNam;
					i++;
				}
			}
		    else if (m_pNpcData)
			{
				m_pNpcData->dwScriptID = dwScriptID;
				m_pNpcData->szScriptName = szFileNam;
			}
        }
        else//doodad
        {
			if (m_nSelCount == 1)
			{
				KSceneEditorDocLogical::DoodadInfo* pDoodadData = m_pDoodadData;
				if (pDoodadData)
				{
					pDoodadData->dwScriptID = dwScriptID;
					pDoodadData->szScriptName = szFileNam;
				}
			}
			else
			{
				list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_listDoodadData.begin();
				list<KSceneEditorDocLogical::DoodadInfo*>::iterator iend = m_listDoodadData.end();
				while (i!=iend)
				{
					KSceneEditorDocLogical::DoodadInfo* pDoodadData = *i;

					if (pDoodadData)
					{
						pDoodadData->dwScriptID = dwScriptID;
						pDoodadData->szScriptName = szFileNam;
					}

					i++;
				}
			}
        }
	}

Exit0:
	SAFE_RELEASE(pIniFile);
	return;
}

void KGListCtrlNPC::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	LPKGLISTITEM   pItem = (LPKGLISTITEM)GetItemData(m_nCurItem);
	CString strText = pItem->arryItemText.GetAt(1).szItemText;
	char szText[128];
	int  nValue = (int)(atof(strText.GetBuffer()) + pNMUpDown->iDelta);
	itoa(nValue, szText, 10);
	pItem->arryItemText.GetAt(1).szItemText = szText;

    m_EditText.SetWindowText(szText);

    if (m_nType == PROPERTY_EDIT_TYPE_NPC)
    {
        switch (m_nCurItem)
        {
        case 0 :
            m_pNpcData->nTempleteID = nValue;
            break;
        case 3 :
            m_pNpcData->nLevel = nValue;
            break;
        case 4 :
            m_pNpcData->nX = nValue;
            break;
        case 5 :
            m_pNpcData->nY = nValue;
            break;
        case 6 :
            m_pNpcData->nZ = nValue;
            break;
        case 9 :
            m_pNpcData->nFaceDirection = nValue;
			if (this->GetItemCount() == PARENT_LISTITEMCOUNT)
			{
				list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_listNpcData.begin();
				list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_listNpcData.end();
				while (i!=iend)
				{
					KSceneEditorDocLogical::NPCInfo* pNPCData = *i;
					pNPCData->nFaceDirection = nValue;
					i++;
				}
			}
            break;
		case 17 :
			m_pNpcData->nProgressID = nValue;
			if (this->GetItemCount() == PARENT_LISTITEMCOUNT)
			{
				list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_listNpcData.begin();
				list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_listNpcData.end();
				while (i!=iend)
				{
					KSceneEditorDocLogical::NPCInfo* pNPCData = *i;
					pNPCData->nProgressID = nValue;
					i++;
				}
			}
			break;
        default :
            break;
        }
    }
    else//doodad
    {
		if (m_nSelCount == 1)
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodadData = m_pDoodadData;

			switch (m_nCurItem)
			{
			case 0 :
				pDoodadData->nTempleteID = nValue;
				break;
			case 3 :
				pDoodadData->nLevel = nValue;
				break;
			case 4 :
				pDoodadData->nX = nValue;
				break;
			case 5 :
				pDoodadData->nY = nValue;
				break;
			case 6 :
				pDoodadData->nZ = nValue;
				break;
			case 9 :
				pDoodadData->nFaceDirection = nValue;
				break;
			case 14 :
				pDoodadData->nProgressID = nValue;
				break;
			default :
				break;
			}
		}
		else
		{
			list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_listDoodadData.begin();
			list<KSceneEditorDocLogical::DoodadInfo*>::iterator iend = m_listDoodadData.end();
			while (i!=iend)
			{
				KSceneEditorDocLogical::DoodadInfo* pDoodadData = *i;

				switch (m_nCurItem)
				{
				case 0 :
					pDoodadData->nTempleteID = nValue;
					break;
				case 3 :
					pDoodadData->nLevel = nValue;
					break;
				case 4 :
					pDoodadData->nX = nValue;
					break;
				case 5 :
					pDoodadData->nY = nValue;
					break;
				case 6 :
					pDoodadData->nZ = nValue;
					break;
				case 9 :
					pDoodadData->nFaceDirection = nValue;
					break;
				case 14 :
					pDoodadData->nProgressID = nValue;
					break;
				default :
					break;
				}

				i++;
			}
		}
    }

//Exit0:
	*pResult = 0;
}

void KGListCtrlNPC::OnCbnSelect()
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM   pItem = (LPKGLISTITEM)GetItemData(m_nCurItem);
	CString strText;
	int nIndex  = 0;

	nIndex = m_comboxCtrl.GetCurSel();
	KGLOG_PROCESS_ERROR(nIndex != -1);
	m_comboxCtrl.GetLBText(nIndex, strText);

	pItem->arryItemText.GetAt(1).szItemText = strText;

    if (m_nType == PROPERTY_EDIT_TYPE_NPC)
    {
	    if (m_nCurItem == 7)
            m_pNpcData->nKind = KGNpcPropertyEditerDlg::TextToKingID(strText);
	    //if (m_nCurItem == 10)
        //    m_pNpcData->nAIMainState = KGNpcPropertyEditerDlg::TextToAIType(strText);
        if (m_nCurItem == 11)
        {
            DWORD dwRepresentID = (DWORD)atoi(strText);

            if (dwRepresentID != m_pNpcData->dwRepresentID)
            {
                KRepresentNPCEquipResource NPCRes;
                KRepresentNPCAnimationResource AniRes;

                g_pRepresentHandler->GetNPCEquipResource(dwRepresentID,&NPCRes);
                g_pRepresentHandler->GetNPCAnimationResource(dwRepresentID,24,&AniRes);

                LPCTSTR pMDLName = NPCRes.szMDLFilePath;
                LPCTSTR pAniName = AniRes.szAniFilePath;

                LPCTSTR pFaceMesh = NPCRes.Parts[RL_NPC_SOCKET_FACE].szMeshFilePath;
                LPCTSTR pFaceMtl = NPCRes.Parts[RL_NPC_SOCKET_FACE].szMtlFilePath;


                IEKG3DRepresentObject*     pNpc = NULL;
                pNpc = (IEKG3DRepresentObject*)(m_pNpcData->pPointer);
                KGLOG_PROCESS_ERROR(pNpc);

                pNpc->LoadFromFile(pMDLName,0,0);

                //if(pFaceMesh)
                pNpc->SetModelFace(pFaceMesh,pFaceMtl,"","S_FACE");

                //if(pAniName)
                pNpc->PlayAnimation(pAniName);     

                m_pNpcData->dwRepresentID = dwRepresentID;
            }
			
        }
		if (m_nCurItem == 12)
		{
			KSceneEditorDocLogical::WayPointSet* pSet =
				(KSceneEditorDocLogical::WayPointSet*) m_comboxCtrl.GetItemData(nIndex);
			m_pNpcData->pWayPointSet = pSet;
			//公共节点 更新公共节点以外的所有节点
			if (this->GetItemCount() == PARENT_LISTITEMCOUNT)
			{
				list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_listNpcData.begin();
				list<KSceneEditorDocLogical::NPCInfo*>::iterator iend = m_listNpcData.end();
				while (i!=iend)
				{
					KSceneEditorDocLogical::NPCInfo* pNPCData = *i;
					pNPCData->pWayPointSet = pSet;
					if(pNPCData->dwNpcAISet == 0)
					{
						m_lpDocLogical->GetNpcDefauleAIParameterSetByTempleteID(pNPCData->nTempleteID, &(pNPCData->m_cAIParameterSet));
					}

					m_lpDocLogical->UpdataOneNpcAIParameters(pNPCData,m_lpDocLogical->m_bUseNpcWayPointSetPointer);
					i++;
				}
			}
			else
			{
				if(m_pNpcData->dwNpcAISet == 0)
				{
					m_lpDocLogical->GetNpcDefauleAIParameterSetByTempleteID(m_pNpcData->nTempleteID, &(m_pNpcData->m_cAIParameterSet));
				}

				m_lpDocLogical->UpdataOneNpcAIParameters(m_pNpcData,m_lpDocLogical->m_bUseNpcWayPointSetPointer);
			}
		}
    }
    else // doodad
    {
		if (m_nSelCount == 1)
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodadData = m_pDoodadData;
			if (m_nCurItem == 7)
				pDoodadData->nKind = KGNpcPropertyEditerDlg::TextToKingID(strText, PROPERTY_EDIT_TYPE_DOODAD);
			//if (m_nCurItem == 10)
			//	pDoodadData->nAIMainState = KGNpcPropertyEditerDlg::TextToAIType(strText);
			if (m_nCurItem == 12)
				pDoodadData->bUseDefaultObstacle = (strText == "1 是");
			if (m_nCurItem == 13)
				pDoodadData->bGenerateObstacle = (strText == "1 是");
		}
		else
		{
			list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_listDoodadData.begin();
			list<KSceneEditorDocLogical::DoodadInfo*>::iterator iend = m_listDoodadData.end();
			while (i!=iend)
			{
				KSceneEditorDocLogical::DoodadInfo* pDoodadData = *i;

				if (m_nCurItem == 7)
					pDoodadData->nKind = KGNpcPropertyEditerDlg::TextToKingID(strText, PROPERTY_EDIT_TYPE_DOODAD);
				//if (m_nCurItem == 10)
				//	pDoodadData->nAIMainState = KGNpcPropertyEditerDlg::TextToAIType(strText);
				if (m_nCurItem == 12)
					pDoodadData->bUseDefaultObstacle = (strText == "1 是");
				if (m_nCurItem == 13)
					pDoodadData->bGenerateObstacle = (strText == "1 是");
				i++;
			}
		}
    }

	Update(pItem);

	nResult = true;
Exit0:
	return;
}

void KGListCtrlNPC::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pItem = NULL;
//	COLORREF colorRef;
	POINT point;
	CRect rectClient;
	CRect rectSub;
	int	  nItem = 0;
	int	  nSub  = 0;
	int	  nSubCount = 0;

	GetClientRect(&rectClient);
	::GetCursorPos(&point);
	ScreenToClient(&point);

	nItem = HitTest(point);
	KG_PROCESS_ERROR(nItem != -1);

	pItem = (LPKGLISTITEM)GetItemData(nItem);

    m_nCurItem = nItem;

    if ((nItem == 0)||
		(nItem == 4)||	//X
		(nItem == 5)||	//Y
		(nItem == 6)||	//Z
		(nItem == 18))	//被选npc个数
        return;


	if (!IsWindow(m_bnBrowse.m_hWnd))
	{
		m_bnBrowse.Create(
			"", WS_CHILD | BS_OWNERDRAW, CRect(0, 0, 16, 16), this, CTL_BROWSE
			);
		m_bnBrowse.LoadIcon(AfxGetApp()->LoadIcon(IDI_ICON_BR));
		m_bnBrowse.m_nSelFlag = false;
	}
	m_bnBrowse.ShowWindow(SW_HIDE);

	if (!IsWindow(m_comboxCtrl.m_hWnd))
	{
		m_comboxCtrl.Create(WS_CHILD | CBS_DROPDOWNLIST|WS_VSCROLL|WS_HSCROLL, CRect(0, 0, 16, 16), this, ID_CB_SHOW);
		m_comboxCtrl.SetFont(GetFont());
	}
	m_comboxCtrl.ShowWindow(SW_HIDE);

	if (!IsWindow(m_spinCtrl.m_hWnd))
	{
		m_spinCtrl.Create(WS_CHILD | UDS_SETBUDDYINT, CRect(0, 0, 16, 16), this, ID_SP_SHOW);
	}
	m_spinCtrl.ShowWindow(SW_HIDE);

    if (!IsWindow(m_EditText.m_hWnd))
    {
        m_EditText.Create(WS_CHILD, CRect(0, 0, 16, 16), this, ID_ED_SHOW);
        m_EditText.SetFont(GetFont());
    }
    m_EditText.ShowWindow(SW_HIDE);

	nSubCount = CListCtrl::GetHeaderCtrl()->GetItemCount();
	for (nSub = nSubCount - 1; nSub >= 0; nSub--)
	{
		GetSubItemRect(nItem, nSub, LVIR_BOUNDS, rectSub);
		if (rectSub.PtInRect(point) && nSub)
		{
			CString strValue;
			int nValue;
			if (m_nType == PROPERTY_EDIT_TYPE_NPC)
			{
				if(m_nSelCount == 1)
					if (nItem != 7 && nItem != 8 && nItem != 10 )
					{
						if (nItem == 0)
							nValue = (int)m_pNpcData->nTempleteID;
						else if (nItem == 3)
							nValue = (int)m_pNpcData->nLevel;
						else if (nItem == 4)
							nValue = (int)m_pNpcData->nX;
						else if (nItem == 5)
							nValue = (int)m_pNpcData->nY;
						else if (nItem == 6)
							nValue = (int)m_pNpcData->nZ;
						else if (nItem == 9)
							nValue = (int)m_pNpcData->nFaceDirection;
						else if (nItem == 17)
							nValue = (int)m_pNpcData->nProgressID;
						else
							nValue = 0;
						strValue.Format("%d", nValue);
						if (nItem == 1)
							m_EditText.SetWindowText(m_pNpcData->szName.c_str());
						else if (nItem == 2)
							m_EditText.SetWindowText(m_pNpcData->szNickName.c_str());
						else
							m_EditText.SetWindowText(strValue);
					}
			}
			else
			{
				if (m_nSelCount == 1)
				{
					KSceneEditorDocLogical::DoodadInfo* pDoodadData = m_pDoodadData;

					if (nItem != 7 && nItem != 8 && nItem != 10)
					{
						if (nItem == 0)
							nValue = (int)pDoodadData->nTempleteID;
						else if (nItem == 3)
							nValue = (int)pDoodadData->nLevel;
						else if (nItem == 4)
							nValue = (int)pDoodadData->nX;
						else if (nItem == 5)
							nValue = (int)pDoodadData->nY;
						else if (nItem == 6)
							nValue = (int)pDoodadData->nZ;
						else if (nItem == 9)
							nValue = (int)pDoodadData->nFaceDirection;
						else if (nItem == 14)
							nValue = (int)pDoodadData->nProgressID;
						else
							nValue = 0;
						strValue.Format("%d", nValue);
						if (nItem == 1)
							m_EditText.SetWindowText(pDoodadData->szName.c_str());
						else if (nItem == 2)
							m_EditText.SetWindowText(pDoodadData->szNickName.c_str());
						else
							m_EditText.SetWindowText(strValue);
					}
				}
				else
				{
					list<KSceneEditorDocLogical::DoodadInfo*>::iterator i = m_listDoodadData.begin();
					list<KSceneEditorDocLogical::DoodadInfo*>::iterator iend = m_listDoodadData.end();
					while (i!=iend)
					{
						KSceneEditorDocLogical::DoodadInfo* pDoodadData = *i;

						if (nItem != 7 && nItem != 8 && nItem != 10)
						{
							if (nItem == 0)
								nValue = (int)pDoodadData->nTempleteID;
							else if (nItem == 3)
								nValue = (int)pDoodadData->nLevel;
							else if (nItem == 4)
								nValue = (int)pDoodadData->nX;
							else if (nItem == 5)
								nValue = (int)pDoodadData->nY;
							else if (nItem == 6)
								nValue = (int)pDoodadData->nZ;
							else if (nItem == 9)
								nValue = (int)pDoodadData->nFaceDirection;
							else if (nItem == 14)
								nValue = (int)pDoodadData->nProgressID;
							else
								nValue = 0;
							strValue.Format("%d", nValue);
							if (nItem == 1)
								m_EditText.SetWindowText(pDoodadData->szName.c_str());
							else if (nItem == 2)
								m_EditText.SetWindowText(pDoodadData->szNickName.c_str());
							else
								m_EditText.SetWindowText(strValue);
						}

						i++;
					}
				}
			}

			CRect rectEdit;
			int nHeght;
			if (rectSub.right > rectClient.right)
				rectSub.right = rectClient.right;
			rectEdit = rectSub;
			rectEdit.left += 1;
			rectEdit.top += 1;

			rectSub.left = rectSub.right - rectSub.Height();
			nHeght = rectSub.Height();

			rectSub.top += 1;
			m_bnBrowse.MoveWindow(&rectSub);
			m_spinCtrl.MoveWindow(&rectSub);

			rectSub = rectEdit;
			rectSub.top -= 2;
			rectSub.right = rectSub.left + 200;
			rectSub.bottom = rectSub.top + 200;
			m_comboxCtrl.MoveWindow(&rectSub);

			switch (pItem->nItemType)
			{
			case KGLISTITEM::SPINTEXT:
				rectEdit.right -= nHeght;
				m_EditText.MoveWindow(&rectEdit);
				m_EditText.ShowWindow(SW_SHOW);
				m_spinCtrl.ShowWindow(SW_SHOW);
				break;
			case KGLISTITEM::EDITTEXT:
				m_EditText.MoveWindow(&rectEdit);
				m_EditText.ShowWindow(SW_SHOW);
				break;
			case KGLISTITEM::COMBOX:
				m_comboxCtrl.ShowWindow(SW_SHOW);
				break;
			case KGLISTITEM::BNBROWSE :
				m_bnBrowse.ShowWindow(SW_SHOW);
				break;
			default :
				break;
			}
			/*switch (nItem)
			{
			case 0 :
			case 3 :
			case 4 :
			case 5 :
			case 6 :
			case 9 :
			rectEdit.right -= nHeght;
			m_EditText.MoveWindow(&rectEdit);
			m_EditText.ShowWindow(SW_SHOW);
			m_spinCtrl.ShowWindow(SW_SHOW);
			break;
			case 1 :
			case 2 :
			m_EditText.MoveWindow(&rectEdit);
			m_EditText.ShowWindow(SW_SHOW);
			break;
			case 7 :
			case 10 :
			case 11 :
			case 12 :
			case 13 :
			m_comboxCtrl.ShowWindow(SW_SHOW);
			break;
			case 8 :
			m_bnBrowse.ShowWindow(SW_SHOW);
			break;
			default :
			break;
			}*/



			m_comboxCtrl.ResetContent();

			if (nItem == 7)
			{
				if (m_nType == PROPERTY_EDIT_TYPE_NPC)
				{
					m_comboxCtrl.InsertString(0, _T("战斗NPC"));
					m_comboxCtrl.InsertString(1, _T("对话NPC"));
					m_comboxCtrl.InsertString(2, _T("任务NPC"));
					m_comboxCtrl.InsertString(3, _T("玩家"));
				}
				else
				{
					m_comboxCtrl.InsertString(0, _T("普通"));
					m_comboxCtrl.InsertString(1, _T("尸体"));
					m_comboxCtrl.InsertString(2, _T("任务相关"));
					m_comboxCtrl.InsertString(3, _T("宝箱"));
					m_comboxCtrl.InsertString(4, _T("装饰物"));
					m_comboxCtrl.InsertString(5, _T("dkClientOnly"));
				}
			}

			if (nItem == 10)
			{
				m_comboxCtrl.InsertString(0, _T("0 无"));
				m_comboxCtrl.InsertString(1, _T("1 发呆"));
				m_comboxCtrl.InsertString(2, _T("2 闲逛"));
				m_comboxCtrl.InsertString(2, _T("3 巡逻"));
			}

			if (nItem == 11)
			{
				// 有点撮的做法，有空记得改
				if (m_nType == PROPERTY_EDIT_TYPE_NPC)
				{
					TCHAR szFullPath[MAX_PATH];
					TCHAR szScetion[128];
					TCHAR szTemplateID[8];
					INT   nRepresentId = 0;
					TCHAR szRepresentId[8];

					sprintf(szTemplateID, TEXT("%d"), m_pNpcData->nTempleteID);

					g_GetFullPath(szFullPath, NPC_TEMPLATE_TABLE_PATH);
					ITabFile* pNPCTemplate = g_OpenTabFile(szFullPath);
					if (pNPCTemplate)
					{
						for (int i = 1; i <= 10; i++)
						{
							sprintf(szScetion, TEXT("RepresentID%d"), i);
							pNPCTemplate->GetInteger(
								szTemplateID,
								szScetion,
								-1,
								&nRepresentId
								);
							if (nRepresentId > -1)
							{
								sprintf(szRepresentId, TEXT("%d"), nRepresentId);
								m_comboxCtrl.AddString(szRepresentId);
							}
						}              
					}
					sprintf(szRepresentId, TEXT("%d"), (int)m_pNpcData->dwRepresentID);
					m_comboxCtrl.SelectString(0, szRepresentId);
					SAFE_RELEASE(pNPCTemplate);
				}
			}

			if (nItem == 12)
			{
				int nCount = m_comboxCtrl.GetCount();
				for (int i=0;i<=nCount;i++)
				{
					m_comboxCtrl.DeleteString(0);
				}

				int nIndex = m_comboxCtrl.AddString("未设置");
				m_comboxCtrl.SetItemData(nIndex,(DWORD_PTR)NULL);

				map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i = m_mapWayPointSet.begin();
				while (i!=m_mapWayPointSet.end())
				{
					//DWORD dwID = i->first;
					KSceneEditorDocLogical::WayPointSet* pSet = i->second;

					TCHAR szName[MAX_PATH];
					wsprintf(szName,"id:%d %s",pSet->nIndex,pSet->szName.c_str());

					int nIndex = m_comboxCtrl.AddString(szName);
					m_comboxCtrl.SetItemData(nIndex,(DWORD_PTR)pSet);
					i++;
				}
			}

			if (nItem == 13 && m_nType == PROPERTY_EDIT_TYPE_NPC)
			{
				m_comboxCtrl.InsertString(0, _T("0 否"));
				m_comboxCtrl.InsertString(1, _T("1 是"));
			}

			if ((nItem == 12 || nItem == 13) && m_nType == PROPERTY_EDIT_TYPE_DOODAD)
			{
				m_comboxCtrl.InsertString(0, _T("0 否"));
				m_comboxCtrl.InsertString(1, _T("1 是"));
			}

			break;
		}
	}

Exit0:
	*pResult = 0;
}

void KGNpcPropertyEditerDlg::OnOK()
{
	m_listNpcData.clear();
    if (m_lpEditScene && m_plogicNpcData && m_plogicNpcData->pPointer)
    {
        m_lpEditScene->RemoveOutputWindow(m_nRenderWndID);
        //IEKG3DSceneSceneEditor* lpEditScene = NULL;
        //m_lpEditScene->GetSceneSceneEditor(&lpEditScene);
		//if (lpEditScene)
		//{
	//		lpEditScene->SetObjectInHandle(m_pHandelObj, FALSE);
	//	}
		
    }
    KillTimer(0);

    CDialog::OnOK();
	//ShowWindow(SW_HIDE);
}
   
void KGNpcPropertyEditerDlg::ReloadBackupNpcDoodadData()
{
	//if (m_nType == PROPERTY_EDIT_TYPE_DOODAD)
	{
		ASSERT(m_listDoodadData.size() == m_listDoodadDataBak.size());
		std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator iterDoodad = m_listDoodadData.begin();
		std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator iterDoodadBak = m_listDoodadDataBak.begin();

		while (iterDoodad != m_listDoodadData.end())
		{
			KSceneEditorDocLogical::DoodadInfo* doodad = *iterDoodad;
			KSceneEditorDocLogical::DoodadInfo* doodadBak = *iterDoodadBak;
			memcpy(doodad, doodadBak, sizeof(KSceneEditorDocLogical::DoodadInfo));

			iterDoodad++;
			iterDoodadBak++;
		}
	}

	if (m_nType == PROPERTY_EDIT_TYPE_NPC)
	{
		ASSERT(m_listNpcData.size() == m_listNpcDataBak.size());
		std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iterNpc = m_listNpcData.begin();
		std::list<KSceneEditorDocLogical::NPCInfo*>::iterator iterNpcBak = m_listNpcDataBak.begin();
		while (iterNpc != m_listNpcData.end())
		{
			KSceneEditorDocLogical::NPCInfo* npc = *iterNpc;
			KSceneEditorDocLogical::NPCInfo* npcBak = *iterNpcBak;
			memcpy(npc, npcBak, sizeof(KSceneEditorDocLogical::NPCInfo));

			iterNpc++;
			iterNpcBak++;
		}
		if (m_plogicNpcDataBak != NULL)
		{
			memcpy(m_plogicNpcData, m_plogicNpcDataBak, sizeof(KSceneEditorDocLogical::NPCInfo));
		}
	}
}

void KGNpcPropertyEditerDlg::OnCancel()
{
	ReloadBackupNpcDoodadData();

	m_listNpcData.clear();
    if (m_lpEditScene && m_plogicNpcData && m_plogicNpcData->pPointer)
    {
        m_lpEditScene->RemoveOutputWindow(m_nRenderWndID);
        //IEKG3DSceneSceneEditor* lpEditScene = NULL;
        //m_lpEditScene->GetSceneSceneEditor(&lpEditScene);
        //if (lpEditScene)
        //    lpEditScene->SetObjectInHandle(m_pHandelObj, FALSE);
    }
    KillTimer(0);
    
	CDialog::OnCancel();
	//ShowWindow(SW_HIDE);
}

void KGNpcPropertyEditerDlg::OnBnClickedOk()
{
    OnOK();
}

void KGNpcPropertyEditerDlg::OnBnClickedCancel()
{
	OnCancel();
}

void KGNpcPropertyEditerDlg::OnBnClickedRefresh()
{
	ITabFile* piTabFile = NULL;
	ITabFile* piAITabFile = NULL;
	char szFilePath[MAX_PATH];
	char szName[_NAME_LEN];
	char szText[128];
	char szTypeFilePath[MAX_PATH];
	char szAIName[200];
	int nRepresentID,nKind,nLevel,nAIType;


	SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
	IEKG3DSceneSceneEditor* lpEditScene = NULL;
	KScene*	lpLogcScene = NULL;

	HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
	if(!hitem && m_nSelCount > 1)
		return;
	
	if (hitem == m_hItemParent && hitem != NULL)
	{
		FillListCtrlForUniformAttribute();
	}
	else if (m_nType == PROPERTY_EDIT_TYPE_NPC)
	{
		HTREEITEM hRootitem = m_treeCtrl.GetRootItem();
		KGLOG_PROCESS_ERROR(hRootitem);

		snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, "NpcTemplate.tab");
		szFilePath[sizeof(szFilePath) - 1] = '\0';
		snprintf(szTypeFilePath, sizeof(szTypeFilePath), "%s/%s", SETTING_DIR, "AITemplate.tab");
		szTypeFilePath[sizeof(szTypeFilePath) - 1] = '\0';


		//打开文件
		piTabFile = g_OpenTabFile(szFilePath);
		KGLOG_PROCESS_ERROR(piTabFile);

		itoa(m_plogicNpcData->nTempleteID, szText, 10);
		BOOL bRetCode = FALSE;
		bRetCode = piTabFile->GetString(szText,"Name","",szName,_NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode);
		bRetCode = piTabFile->GetInteger(szText,"RepresentID1",0,&nRepresentID);
		KGLOG_PROCESS_ERROR(bRetCode);
		bRetCode = piTabFile->GetInteger(szText,"Level",1,&nLevel);
		KGLOG_PROCESS_ERROR(bRetCode);
		bRetCode = piTabFile->GetInteger(szText,"Kind",2,&nKind);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = piTabFile->GetInteger(szText,"AIParamTemplateID",1,&nAIType);
		KGLOG_PROCESS_ERROR(bRetCode);

		piAITabFile = g_OpenTabFile(szTypeFilePath);
		KGLOG_PROCESS_ERROR(piAITabFile);
		int nHeight = piAITabFile->GetHeight();
		int nAIID = 0;
		int i;
		for (i = 1; i <= nHeight; i++)
		{
			bRetCode = piAITabFile->GetInteger(i,"ID",-1,&nAIID);
			KGLOG_PROCESS_ERROR(bRetCode);
			if (nAIID == nAIType) 
              break;
		}
		KGLOG_PROCESS_ERROR(i != nHeight + 1);
		bRetCode = piAITabFile->GetString(i,"Name","",szAIName,200);
		//if (strstr(szAIName,"发呆"))
		//	m_plogicNpcData->nAIMainState = 1;
		//else if (strstr(szAIName,"巡逻"))
		//	m_plogicNpcData->nAIMainState = 3;
		//else if (strstr(szAIName,"游荡"))
		//	m_plogicNpcData->nAIMainState = 2;
		//else
		//	m_plogicNpcData->nAIMainState = 0;
        
		KGLOG_PROCESS_ERROR(m_plogicNpcData);
		m_plogicNpcData->szName = szName;
		m_plogicNpcData->nLevel = nLevel;
		m_plogicNpcData->nKind = nKind;

		KRepresentNPCEquipResource NPCRes;
		KRepresentNPCAnimationResource AniRes;
		HRESULT hr = E_FAIL;
		hr = g_pRepresentHandler->ResetActiveObject();
		if (hr != S_OK)
		{
			MessageBox("File Error!");
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		g_pRepresentHandler->GetNPCEquipResource(nRepresentID,&NPCRes);
		g_pRepresentHandler->GetNPCAnimationResource(nRepresentID,24,&AniRes);
		LPCTSTR pMDLName = NPCRes.szMDLFilePath;
		LPCTSTR pAniName = AniRes.szAniFilePath;
		LPCTSTR pFaceMesh = NPCRes.Parts[RL_NPC_SOCKET_FACE].szMeshFilePath;
		LPCTSTR pFaceMtl = NPCRes.Parts[RL_NPC_SOCKET_FACE].szMtlFilePath;
		IEKG3DRepresentObject*     pNpc = NULL;

		m_plogicNpcData->dwRepresentID = nRepresentID;
		pNpc = (IEKG3DRepresentObject*)(m_plogicNpcData->pPointer);
		KGLOG_PROCESS_ERROR(pNpc);

        pNpc->LoadFromFile(pMDLName,0,0);

        //if(pFaceMesh)
		pNpc->SetModelFace(pFaceMesh,pFaceMtl,"","S_FACE");
		//if(pAniName)
		pNpc->PlayAnimation(pAniName);     
	

		IEKG3DSceneEntity* pEntity = NULL;
		KGLOG_PROCESS_ERROR(m_lpEditScene);
        m_lpEditScene->GetFirstRenderEntity(&pEntity);
		while (pEntity != NULL)
		{
			pEntity->GetType(&EntityType);
			if(EntityType==SCENEENTITY_OBJECT)
			{
				IEKG3DRepresentObject* pObject = NULL;
				pEntity->GetPointer((VOID **)&pObject);
				if(pObject->GetType()==REPRESENTOBJECT_NPC)
				{
					KSceneEditorDocLogical::NPCInfo* pNPCinfo = NULL;
					if(SUCCEEDED(m_lpDocLogical->FindNPCInfoByEditorNPC(&pNPCinfo,pObject)))
					{
						if (pNPCinfo->nTempleteID == m_plogicNpcData->nTempleteID)
						{
							pNPCinfo->szName = m_plogicNpcData->szName;

							HTREEITEM hitem = m_treeCtrl.GetItemByPrimaryKey(hRootitem,DWORD_PTR(pObject));
							if (hitem != NULL)
								m_treeCtrl.SetItemText(hitem,szName);


							pNPCinfo->dwRepresentID = m_plogicNpcData->dwRepresentID;
							pNPCinfo->nLevel = m_plogicNpcData->nLevel;
							pNPCinfo->nKind = m_plogicNpcData->nKind;
							//pNPCinfo->nAIMainState = m_plogicNpcData->nAIMainState;
							pNpc = (IEKG3DRepresentObject*)(pNPCinfo->pPointer);
							KGLOG_PROCESS_ERROR(pNpc);

                            pNpc->LoadFromFile(pMDLName,0,0);


							//if(pFaceMesh)
							pNpc->SetModelFace(pFaceMesh,pFaceMtl,"","S_FACE");

							//if(pAniName)
							pNpc->PlayAnimation(pAniName);     
					
						}
					}
				}
			}
			m_lpEditScene->GetNextRenderEntity(&pEntity);
		}
		FillListCtrl();
		/*if (m_lpEditScene && m_plogicNpcData->pPointer)
		{
			IEKG3DSceneSceneEditor* lpEditScene = NULL;
			m_lpEditScene->GetSceneSceneEditor(&lpEditScene);
			if (lpEditScene)
				lpEditScene->SetObjectInHandle((IEKG3DRepresentObject*)m_plogicNpcData->pPointer, FALSE);
		}		*/
	}
Exit0:
	KG_COM_RELEASE(piAITabFile);
	KG_COM_RELEASE(piTabFile);
	return;

}

void KGNpcPropertyEditerDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
	
	if(!hitem)
		return;

	PVOID pPointer = (PVOID)m_treeCtrl.GetItemData(hitem);

	D3DXVECTOR3 vPositionObj(0, 0, 0);

    if (m_nType == PROPERTY_EDIT_TYPE_NPC)
    {
	    KSceneEditorDocLogical::NPCInfo* pNPCinfo = NULL;
		if (hitem == m_hItemParent && m_pParentNpcData != NULL)
		{
			m_plogicNpcData = m_pParentNpcData;
			FillListCtrlForUniformAttribute();
			return;
		}
	    if(SUCCEEDED(m_lpDocLogical->FindNPCInfoByEditorNPC(&pNPCinfo,pPointer)))
	    {
		    m_plogicNpcData = pNPCinfo;     
		    FillListCtrl();
			vPositionObj = pNPCinfo->vPosition;

            /*if (m_lpEditScene && m_plogicNpcData->pPointer)
            {
                IEKG3DSceneSceneEditor* lpEditScene = NULL;
                m_lpEditScene->GetSceneSceneEditor(&lpEditScene);
                if (lpEditScene)
                    lpEditScene->SetObjectInHandle((IEKG3DRepresentObject*)m_plogicNpcData->pPointer, FALSE);
				m_nSelCount = 1;
				m_listCtrl.m_nSelCount = 1;
            }*/
        }
    }
    else
    {
        KSceneEditorDocLogical::DoodadInfo* pDoodainfo = NULL;
		/*if (hitem == m_hItemParent&& m_pParentDoodadData != NULL)
		{
			m_plogicDoodadData = m_pParentDoodadData;
			FillListCtrlForUniformAttribute();
			return;
		}*/
        if(SUCCEEDED(m_lpDocLogical->FindDoodadInfoByEditorDoodad(&pDoodainfo, pPointer)))
        {
			m_plogicDoodadData = pDoodainfo;
            FillListCtrl();
			vPositionObj = pDoodainfo->vPosition;
        }
    }

	AtlVector3 oldpos, oldlookat, delta, newpos, newlookat;
	IEKG3DSceneSceneEditor* lpEditScene = NULL;
	m_lpEditScene->GetSceneSceneEditor(&lpEditScene);

	lpEditScene->GetCameraPosLookat(&oldpos, &oldlookat);
	delta.x = oldlookat.x - oldpos.x; delta.y = oldlookat.y - oldpos.y; delta.z = oldlookat.z - oldpos.z;
	newpos.x = vPositionObj.x - delta.x; newpos.y = vPositionObj.y - delta.y; newpos.z = vPositionObj.z - delta.z;
	newlookat.x = vPositionObj.x; newlookat.y = vPositionObj.y; newlookat.z = vPositionObj.z;
	D3DXVECTOR3 vz(0, 0, 0);
	if (MT_VECTOR_UNEQUAL(newlookat, vz, 0.001f))
	{
		lpEditScene->SetCameraPosLookat(&newpos, &newlookat);
	}
}
