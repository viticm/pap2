// KGTrafficPropertyEditerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGTrafficPropertyEditerDlg.h"
#include "IEEditor.h"
#include "KSO3World.h"
#include "KSceneHelperFuncs.h"

// KGTrafficPropertyEditerDlg dialog

IMPLEMENT_DYNAMIC(KGTrafficPropertyEditerDlg, CDialog)

KGTrafficPropertyEditerDlg::KGTrafficPropertyEditerDlg(
	IEKG3DScene* lpEditScene,KSceneEditorDocLogical* pDocLogical,CWnd* pParent /*=NULL*/)
	: CDialog(KGTrafficPropertyEditerDlg::IDD, pParent)
{
	m_lpEditScene      = lpEditScene;
	m_lpDocLogical     = pDocLogical;
	m_dwNodeID         = 0;
}

KGTrafficPropertyEditerDlg::~KGTrafficPropertyEditerDlg()
{
}

BOOL KGTrafficPropertyEditerDlg::OnInitDialog()
{
	HRESULT hRetCode = E_FAIL;
	SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
	IEKG3DSceneEntity* pEntity = NULL;
	int nResult  = false;
	int nRetCode = false;

	IEKG3DSceneSceneEditor* lpEditScene = NULL;
	char  szText[100];
	nRetCode = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	m_lpEditScene->GetSceneSceneEditor(&lpEditScene);
	KG_PROCESS_ERROR(lpEditScene);
	//lpEditScene->GetSelectedEntity(0, &pEntity);
	pEntity = KSF::GetFirstSelectedEntity(lpEditScene);
	KG_PROCESS_ERROR(NULL != pEntity);

	hRetCode = pEntity->GetType(&EntityType);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	if(EntityType == SCENEENTITY_OBJECT)
	{
		IEKG3DRepresentObject* pObject = NULL;
		hRetCode = pEntity->GetPointer((VOID **)&pObject);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		if(pObject->GetType()==REPRESENTOBJECT_TRAFFICPOINT)
		{
			KSceneEditorDocLogical::TrafficPoint* pTrafficPoint = NULL;
			IEKG3DRepresentTrafficPoint* pRepTrafficPoint = lpEditScene->GetIEKG3DRepresentTrafficPoint(pObject);
			m_dwNodeID = pRepTrafficPoint->GetSetID();
			if (m_lpDocLogical->m_mapTrafficPoint.find(m_dwNodeID) != m_lpDocLogical->m_mapTrafficPoint.end())				
			{
				pTrafficPoint = &(m_lpDocLogical->m_mapTrafficPoint[m_dwNodeID]);
				itoa(pTrafficPoint->m_dwNodeID, szText, 10);
				editNodeID.SetWindowText(szText);
				itoa(pTrafficPoint->m_dwMapID,szText,10);
				editMapID.SetWindowText(szText);
				itoa(pTrafficPoint->m_dwCityID,szText,10);
				editCityID.SetWindowText(szText);
				itoa(pTrafficPoint->m_dwForceID,szText,10);
				editForceID.SetWindowText(szText);
				itoa(pTrafficPoint->m_dwTongID,szText,10);
				editTongID.SetWindowText(szText);
				editDescription.SetWindowText(pTrafficPoint->m_strDescription.c_str());
				editScriptFile.SetWindowText(pTrafficPoint->m_strScriptFile.c_str());
				checkNeedOpen.SetCheck(pTrafficPoint->m_bNeedOpen);
				if (pTrafficPoint->m_nLinkID == -2)
				{
					buttonChange.EnableWindow(FALSE);
					buttonDelete.EnableWindow(FALSE);
					buttonEdit.EnableWindow(FALSE);
					editCostMoney.EnableWindow(FALSE);
					editVelocity.EnableWindow(FALSE);
					editTrackID.EnableWindow(FALSE);
					editByWhat.EnableWindow(FALSE);
					m_ListMapPoints.EnableWindow(FALSE);
					listTrack.EnableWindow(FALSE);
					buttonIsOverMap.SetCheck(0);
				}
				else if (pTrafficPoint->m_nLinkID == -1)
				{
					buttonEdit.EnableWindow(FALSE);
					editCostMoney.EnableWindow(FALSE);
					editVelocity.EnableWindow(FALSE);
					editTrackID.EnableWindow(FALSE);
					editByWhat.EnableWindow(FALSE);
					buttonDelete.EnableWindow(FALSE);
					buttonIsOverMap.SetCheck(1);
					FillOverMapList(m_dwNodeID);
				}				
			}
		}
		
	}
	nResult = true;
Exit0:
	return nResult;
}

void KGTrafficPropertyEditerDlg::FillOverMapList(DWORD dwID)
{
	char  szText[100];
	m_ListMapPoints.ResetContent();
	map<DWORD,string>::iterator it = m_lpDocLogical->m_mapLinkAbleList.begin();
	while (it != m_lpDocLogical->m_mapLinkAbleList.end())
	{
		int nIndex = m_ListMapPoints.AddString((it->second).c_str());	
		m_ListMapPoints.SetItemData(nIndex,(DWORD_PTR)(&(it->first)));
		it++;
	}
	m_ListMapPoints.SetCurSel(0);
	listTrack.ResetContent();
	list<KSceneEditorDocLogical::TrafficPointSet*>::iterator iSet = m_lpDocLogical->m_listOverMapPointSet.begin();
	while (iSet != m_lpDocLogical->m_listOverMapPointSet.end())
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = *iSet;						
		if (pSet->m_dwFromID == dwID || pSet->m_dwToID == dwID)
		{
			wsprintf(szText,"%s 到 %s",m_lpDocLogical->m_mapLinkAbleList[pSet->m_dwFromID].c_str(),
				m_lpDocLogical->m_mapLinkAbleList[pSet->m_dwToID].c_str());
			int nIndex = listTrack.AddString(szText);
			listTrack.SetItemData(nIndex,(DWORD_PTR)pSet);
		}
		iSet++;	
	}				
}

void KGTrafficPropertyEditerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DESCRIPTION, editDescription);
	DDX_Control(pDX, IDC_NODEID, editNodeID);
	DDX_Control(pDX, IDC_NEEDOPNE, checkNeedOpen);
	DDX_Control(pDX, IDC_MAPID, editMapID);
	DDX_Control(pDX, IDC_CITYID, editCityID);
	DDX_Control(pDX, IDC_FORCEID, editForceID);
	DDX_Control(pDX, IDC_TONGID, editTongID);
	DDX_Control(pDX, IDC_SCRIPTFILE, editScriptFile);
	DDX_Control(pDX, IDC_COMBO1, m_ListMapPoints);
	DDX_Control(pDX, IDC_BUTTON1, buttonChange);
	DDX_Control(pDX, IDC_CHECKOVERMAP, buttonIsOverMap);
	DDX_Control(pDX, IDC_LISTTRACK, listTrack);
	DDX_Control(pDX, IDC_BUTTONDELETE, buttonDelete);
	DDX_Control(pDX, IDC_EDITTRACKID, editTrackID);
	DDX_Control(pDX, IDC_EDITCOSTMONEY, editCostMoney);
	DDX_Control(pDX, IDC_EDITVELOCITY, editVelocity);
	DDX_Control(pDX, IDC_BUTTONEDIT, buttonEdit);
	DDX_Control(pDX, IDC_EDITBYWHAT, editByWhat);
}


BEGIN_MESSAGE_MAP(KGTrafficPropertyEditerDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON3, &KGTrafficPropertyEditerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDOK, &KGTrafficPropertyEditerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_NEEDOPNE, &KGTrafficPropertyEditerDlg::OnBnClickedNeedopne)
	ON_BN_CLICKED(IDC_BUTTON1, &KGTrafficPropertyEditerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECKOVERMAP, &KGTrafficPropertyEditerDlg::OnBnClickedCheckovermap)
	ON_BN_CLICKED(IDC_BUTTONDELETE, &KGTrafficPropertyEditerDlg::OnBnClickedButtondelete)
	ON_BN_CLICKED(IDC_BUTTONEDIT, &KGTrafficPropertyEditerDlg::OnBnClickedButtonedit)
	ON_LBN_SELCHANGE(IDC_LISTTRACK, &KGTrafficPropertyEditerDlg::OnChangedListtrack)
END_MESSAGE_MAP()


// KGTrafficPropertyEditerDlg message handlers

void KGTrafficPropertyEditerDlg::OnBnClickedButton3()
{
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	TCHAR szAppDir[MAX_PATH] = { 0 };
	CFileDialog dlg(true, "Lua", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D脚本文件(*.Lua)|*.Lua||");
	if(dlg.DoModal() == IDOK)
	{
		TCHAR FileName[MAX_PATH];
		TCHAR RetFileName[MAX_PATH];
		wsprintf(FileName,"%s",dlg.GetPathName());
		//pManager->GetRelativePath(g_szDefWorkDirectory,FileName,RetFileName);
		TCHAR* strFind = NULL;

		strFind = FileName + strlen(g_szDefWorkDirectory);
		strcpy(RetFileName, strFind);
		editScriptFile.SetWindowText(RetFileName);
	}
}

void KGTrafficPropertyEditerDlg::OnBnClickedOk()
{
	KSceneEditorDocLogical::TrafficPoint* pTrafficPoint = NULL;
	CString  szText;
	if (m_lpDocLogical->m_mapTrafficPoint.find(m_dwNodeID) != m_lpDocLogical->m_mapTrafficPoint.end())				
	{
		pTrafficPoint = &(m_lpDocLogical->m_mapTrafficPoint[m_dwNodeID]);
		editMapID.GetWindowText(szText);
		pTrafficPoint->m_dwMapID = (DWORD)(atof(szText.GetBuffer()));
		editCityID.GetWindowText(szText);
		pTrafficPoint->m_dwCityID = (DWORD)(atof(szText.GetBuffer()));
		editForceID.GetWindowText(szText);
		pTrafficPoint->m_dwForceID = (DWORD)(atof(szText.GetBuffer()));
		editTongID.GetWindowText(szText);
		pTrafficPoint->m_dwTongID = (DWORD)(atof(szText.GetBuffer()));
		editDescription.GetWindowText(szText);
		pTrafficPoint->m_strDescription = szText.GetBuffer();
		editScriptFile.GetWindowText(szText);
		pTrafficPoint->m_strScriptFile = szText.GetBuffer();
		pTrafficPoint->m_bNeedOpen = checkNeedOpen.GetCheck();	
		if (m_lpDocLogical->m_mapLinkAbleList.find(m_dwNodeID) != m_lpDocLogical->m_mapLinkAbleList.end())
			m_lpDocLogical->m_mapLinkAbleList[m_dwNodeID] = pTrafficPoint->m_strDescription;
	}
//Exit0:
	OnOK();
}

void KGTrafficPropertyEditerDlg::OnBnClickedNeedopne()
{

	return;
}

void KGTrafficPropertyEditerDlg::OnBnClickedButton1()
{
	KSceneEditorDocLogical::TrafficPoint* pTrafficPoint = NULL;
	if (m_lpDocLogical->m_mapTrafficPoint.find(m_dwNodeID) != m_lpDocLogical->m_mapTrafficPoint.end())				
	{
		int nIndex = m_ListMapPoints.GetCurSel();
		KG_PROCESS_ERROR(nIndex != LB_ERR);
		DWORD_PTR pPoint = m_ListMapPoints.GetItemData(nIndex);
		if (pPoint != NULL)
		{
			DWORD dwToID = *((DWORD*)pPoint);
			list<KSceneEditorDocLogical::TrafficPointSet*>::iterator iSet = m_lpDocLogical->m_listOverMapPointSet.begin();
			while (iSet != m_lpDocLogical->m_listOverMapPointSet.end())
			{
				KSceneEditorDocLogical::TrafficPointSet* pSet = *iSet;						
				if ((pSet->m_dwFromID == m_dwNodeID && pSet->m_dwToID == dwToID)|| 
					(pSet->m_dwFromID == dwToID && pSet->m_dwToID == m_dwNodeID))
				{
					break;
				}
				iSet++;	
			}
			if (iSet == m_lpDocLogical->m_listOverMapPointSet.end())
			{	
				int nID = 0;
				g_pSO3World->m_RoadManager.AddTrack(&nID,m_dwNodeID,dwToID);

				KSceneEditorDocLogical::TrafficPointSet* pTrafficPointSet = new KSceneEditorDocLogical::TrafficPointSet;
				KGLOG_PROCESS_ERROR(pTrafficPointSet);	
				m_lpDocLogical->m_listOverMapPointSet.push_back(pTrafficPointSet);
				pTrafficPointSet->m_dwCostMoney = 1;
				pTrafficPointSet->m_dwVelocity = 48;
				pTrafficPointSet->m_dwID = nID;
				pTrafficPointSet->m_dwFromID = m_dwNodeID;
				pTrafficPointSet->m_dwToID = dwToID;
				m_lpDocLogical->m_mapTrackByWhat[nID] = "";
				FillOverMapList(m_dwNodeID);
			}
		}
	}
		
Exit0:
	return;	
}


void KGTrafficPropertyEditerDlg::OnBnClickedCheckovermap()
{
	int nIsCheck = 0;
	KSceneEditorDocLogical::TrafficPoint* pTrafficPoint = NULL;
	if (m_lpDocLogical->m_mapTrafficPoint.find(m_dwNodeID) != m_lpDocLogical->m_mapTrafficPoint.end())	
	{
		pTrafficPoint = &(m_lpDocLogical->m_mapTrafficPoint[m_dwNodeID]);
		nIsCheck = buttonIsOverMap.GetCheck();
		if (nIsCheck && pTrafficPoint->m_nLinkID == -2)
		{
			m_lpDocLogical->m_mapLinkAbleList[m_dwNodeID] = pTrafficPoint->m_strDescription;
			pTrafficPoint->m_nLinkID = -1;
			buttonChange.EnableWindow(TRUE);
			m_ListMapPoints.EnableWindow(TRUE);
			listTrack.EnableWindow(TRUE);
		}
		else if ((!nIsCheck) && pTrafficPoint->m_nLinkID == -1)
		{
			list<KSceneEditorDocLogical::TrafficPointSet*>::iterator iSet = m_lpDocLogical->m_listOverMapPointSet.begin();
			while (iSet != m_lpDocLogical->m_listOverMapPointSet.end())
			{
				KSceneEditorDocLogical::TrafficPointSet* pSet = *iSet;						
				if (pSet->m_dwFromID == m_dwNodeID || pSet->m_dwToID == m_dwNodeID)
				{
					KTRACK* pKTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pSet->m_dwFromID,pSet->m_dwToID);
					if (pKTrack != NULL)
						g_pSO3World->m_RoadManager.DelTrack(pKTrack->nTrackID);	
					iSet++;
					map<DWORD,string>::iterator itTrackByWhat = m_lpDocLogical->m_mapTrackByWhat.find(pSet->m_dwID);
					if (itTrackByWhat != m_lpDocLogical->m_mapTrackByWhat.end())
						m_lpDocLogical->m_mapTrackByWhat.erase(itTrackByWhat);
					m_lpDocLogical->m_listOverMapPointSet.remove(pSet);
					SAFE_DELETE(pSet);
					continue;
				}
				iSet++;	
			}	
			map<DWORD,string>::iterator iLinkAbleList = m_lpDocLogical->m_mapLinkAbleList.find(m_dwNodeID);
			if (iLinkAbleList != m_lpDocLogical->m_mapLinkAbleList.end())
				m_lpDocLogical->m_mapLinkAbleList.erase(iLinkAbleList);
			pTrafficPoint->m_nLinkID = -2;
			buttonChange.EnableWindow(FALSE);
			buttonDelete.EnableWindow(FALSE);
			buttonEdit.EnableWindow(FALSE);
			editCostMoney.EnableWindow(FALSE);
			editVelocity.EnableWindow(FALSE);
			editTrackID.EnableWindow(FALSE);
			editByWhat.EnableWindow(FALSE);
			m_ListMapPoints.EnableWindow(FALSE);
			listTrack.EnableWindow(FALSE);
		}
		FillOverMapList(m_dwNodeID);
	}
}

void KGTrafficPropertyEditerDlg::OnBnClickedButtondelete()
{
	int nIndex = listTrack.GetCurSel();
	map<DWORD,string>::iterator itTrackByWhat;
	KG_PROCESS_ERROR(nIndex != LB_ERR);
	DWORD_PTR pPtr = listTrack.GetItemData(nIndex);
	KSceneEditorDocLogical::TrafficPointSet* pTrafficPointSet = (KSceneEditorDocLogical::TrafficPointSet*)pPtr;
	KG_PROCESS_ERROR(pTrafficPointSet != NULL);

	g_pSO3World->m_RoadManager.DelTrack(pTrafficPointSet->m_dwID);
	itTrackByWhat = m_lpDocLogical->m_mapTrackByWhat.find(pTrafficPointSet->m_dwID);
	if (itTrackByWhat != m_lpDocLogical->m_mapTrackByWhat.end())
		m_lpDocLogical->m_mapTrackByWhat.erase(itTrackByWhat);
	m_lpDocLogical->m_listOverMapPointSet.remove(pTrafficPointSet);
	SAFE_DELETE(pTrafficPointSet);
	FillOverMapList(m_dwNodeID);
Exit0:
	return;
}

void KGTrafficPropertyEditerDlg::OnBnClickedButtonedit()
{
	CString szText;
	map<DWORD,string>::iterator itTrackByWhat;
	int nIndex = listTrack.GetCurSel();
	KG_PROCESS_ERROR(nIndex != LB_ERR);
	DWORD_PTR pPtr = listTrack.GetItemData(nIndex);
	KSceneEditorDocLogical::TrafficPointSet* pTrafficPointSet = (KSceneEditorDocLogical::TrafficPointSet*)pPtr;
	KG_PROCESS_ERROR(pTrafficPointSet != NULL);
	editVelocity.GetWindowText(szText);
	pTrafficPointSet->m_dwVelocity = (DWORD)(atof(szText.GetBuffer()));
	editCostMoney.GetWindowText(szText);
	pTrafficPointSet->m_dwCostMoney = (DWORD)(atof(szText.GetBuffer()));
	editByWhat.GetWindowText(szText);
	itTrackByWhat = m_lpDocLogical->m_mapTrackByWhat.find(pTrafficPointSet->m_dwID);
	if (itTrackByWhat != m_lpDocLogical->m_mapTrackByWhat.end())
		m_lpDocLogical->m_mapTrackByWhat[pTrafficPointSet->m_dwID] = szText.GetBuffer();
Exit0:
	return;
}

void KGTrafficPropertyEditerDlg::OnChangedListtrack()
{
	char  szText[100];
	int nIndex = listTrack.GetCurSel();
	DWORD_PTR pPtr = 0;
	KSceneEditorDocLogical::TrafficPointSet* pTrafficPointSet = NULL;

	if (nIndex == LB_ERR)
	{
		buttonEdit.EnableWindow(FALSE);
		buttonDelete.EnableWindow(FALSE);
		editCostMoney.EnableWindow(FALSE);
		editVelocity.EnableWindow(FALSE);
		editTrackID.EnableWindow(FALSE);
		editByWhat.EnableWindow(FALSE);
		return;
	}

	pPtr = listTrack.GetItemData(nIndex);
	pTrafficPointSet = (KSceneEditorDocLogical::TrafficPointSet*)pPtr;
	KG_PROCESS_ERROR(pTrafficPointSet != NULL);

	buttonEdit.EnableWindow(TRUE);
	editCostMoney.EnableWindow(TRUE);
	editVelocity.EnableWindow(TRUE);
	buttonEdit.EnableWindow(TRUE);
	buttonDelete.EnableWindow(TRUE);
	editByWhat.EnableWindow(TRUE);
	itoa(pTrafficPointSet->m_dwID,szText,10);
	editTrackID.SetWindowText(szText);
	itoa(pTrafficPointSet->m_dwVelocity,szText,10);
	editVelocity.SetWindowText(szText);
	itoa(pTrafficPointSet->m_dwCostMoney,szText,10);
	editCostMoney.SetWindowText(szText);
	editByWhat.SetWindowText(m_lpDocLogical->m_mapTrackByWhat[pTrafficPointSet->m_dwID].c_str());
Exit0:
	return;
}
