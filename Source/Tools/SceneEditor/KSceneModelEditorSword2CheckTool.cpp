// KSceneModelEditorSword2CheckTool.cpp : implementation file
// 
#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorSword2CheckTool.h"
#include "Engine.h"
#include "IEEditor.h"
#include "IEKG3DModelTable.h"
//#include "SceneEditorCommon.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSword2CheckTool dialog

KSceneModelEditorSword2CheckTool::KSceneModelEditorSword2CheckTool(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorSword2CheckTool::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneModelEditorSword2CheckTool)
	m_nMode = -1;
	//}}AFX_DATA_INIT
	//m_lpModelEditor = NULL;
	//m_lpCurModelBody = NULL;
}

void KSceneModelEditorSword2CheckTool::SetModelEditor(IEKG3DSceneModelEditor* pModelEditor)
{
	m_pCurModelEditor = pModelEditor;
}

void KSceneModelEditorSword2CheckTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorSword2CheckTool)
	DDX_Control(pDX, IDC_COMBO_SOCKET, m_Combo_Socket);
	DDX_Control(pDX, IDC_LIST_WEAPEN, m_ListBox_Weapon);
	DDX_Control(pDX, IDC_COMBO_ANINAME, m_Combo_Ani);
	DDX_Control(pDX, IDC_LIST_UP, m_ListBox_Up);
	DDX_Control(pDX, IDC_LIST_HEAD, m_ListBox_Head);
	DDX_Control(pDX, IDC_LIST_DOWN, m_ListBox_Down);
	DDX_CBIndex(pDX, IDC_COMBO_MODE, m_nMode);

	DDX_Control(pDX, IDC_COMBO_SOCKET2, m_Combo_Socket1);
	DDX_Control(pDX, IDC_LIST_WEAPEN2, m_ListBox_Weapon1);
	DDX_Control(pDX, IDC_COMBO_ANINAME2, m_Combo_Ani1);
	DDX_Control(pDX, IDC_LIST_UP2, m_ListBox_Up1);
	DDX_Control(pDX, IDC_LIST_HEAD2, m_ListBox_Head1);
	DDX_Control(pDX, IDC_LIST_DOWN2, m_ListBox_Down1);
	DDX_CBIndex(pDX, IDC_COMBO_MODE2, m_nMode1);

	//DDX_Check(pDX, IDC_CHECK_SYN, m_bSynAniChange);
	//}}AFX_DATA_MAP

	/*CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorMeshBindDialog)
	DDX_Control(pDX, IDC_LIST1, m_ListBoxModel);
	//}}AFX_DATA_MAP*/
}


BEGIN_MESSAGE_MAP(KSceneModelEditorSword2CheckTool, CDialog)
	//{{AFX_MSG_MAP(KSceneModelEditorSword2CheckTool)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ADDHEAD, OnBnClickedButtonAddhead)
	ON_BN_CLICKED(IDC_BUTTON_ADDHEAD2, OnBnClickedButtonAddhead2)

	ON_LBN_SELCHANGE(IDC_LIST_HEAD, OnLbnSelchangeListHead)
//	ON_LBN_SELCHANGE(IDC_LIST_HEAD2, OnLbnSelchangeListHead2)
	
	ON_BN_CLICKED(IDC_BUTTON_ADDUP, OnBnClickedButtonAddup)
	ON_BN_CLICKED(IDC_BUTTON_ADDUP2, OnBnClickedButtonAddup2)
	
	ON_BN_CLICKED(IDC_BUTTON_ADDDOWN, OnBnClickedButtonAdddown)
	ON_BN_CLICKED(IDC_BUTTON_ADDDOWN2, OnBnClickedButtonAdddown2)

	ON_LBN_SELCHANGE(IDC_LIST_UP, OnLbnSelchangeListUp)
	ON_LBN_SELCHANGE(IDC_LIST_UP2, OnLbnSelchangeListUp2)

	ON_LBN_SELCHANGE(IDC_LIST_DOWN, OnLbnSelchangeListDown)
	ON_LBN_SELCHANGE(IDC_LIST_DOWN2, OnLbnSelchangeListDown2)
	
	ON_BN_CLICKED(IDC_BUTTON_ANIOPEN, OnBnClickedButtonAniopen)
	ON_BN_CLICKED(IDC_BUTTON_ANIOPEN2, OnBnClickedButtonAniopen2)
	
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_PLAY2, OnBnClickedButtonPlay2)

	ON_CBN_SELCHANGE(IDC_COMBO_ANINAME, OnCbnSelchangeComboAniname)
	ON_CBN_SELCHANGE(IDC_COMBO_ANINAME2, OnCbnSelchangeComboAniname2)
	
	ON_BN_CLICKED(IDC_BUTTON_ADDHEADTAB, OnBnClickedButtonAddheadtab)
	ON_BN_CLICKED(IDC_BUTTON_ADDHEADTAB2, OnBnClickedButtonAddheadtab2)
	
	ON_BN_CLICKED(IDC_BUTTON_ADDUPTAB, OnBnClickedButtonAdduptab)
	ON_BN_CLICKED(IDC_BUTTON_ADDUPTAB2, OnBnClickedButtonAdduptab2)

	ON_BN_CLICKED(IDC_BUTTON_ADDDOWNTAB, OnBnClickedButtonAdddowntab)
	ON_BN_CLICKED(IDC_BUTTON_ADDDOWNTAB3, OnBnClickedButtonAdddowntab3)

	ON_CBN_SELCHANGE(IDC_COMBO_MODE, OnCbnSelchangeComboMode)
	ON_CBN_SELCHANGE(IDC_COMBO_MODE2, OnCbnSelchangeComboMode2)
	
	ON_BN_CLICKED(IDC_BUTTON_ADDWEAPEN, OnBnClickedButtonAddweapen)
	ON_BN_CLICKED(IDC_BUTTON_ADDWEAPEN2, OnBnClickedButtonAddweapen2)

	ON_LBN_SELCHANGE(IDC_LIST_WEAPEN, OnLbnSelchangeListWeapen)
	ON_LBN_SELCHANGE(IDC_LIST_WEAPEN2, OnLbnSelchangeListWeapen2)

	ON_CBN_SELCHANGE(IDC_COMBO_SOCKET, OnCbnSelchangeComboSocket)
	ON_CBN_SELCHANGE(IDC_COMBO_SOCKET2, OnCbnSelchangeComboSocket2)
	
	ON_BN_CLICKED(IDC_CHECK_EDITUP, OnBnClickedCheckEditup)
	ON_BN_CLICKED(IDC_CHECK_EDITUP2, OnBnClickedCheckEditup2)
		
	ON_BN_CLICKED(IDC_BUTTON_HEADDEL, OnBnClickedButtonHeaddel)
	ON_BN_CLICKED(IDC_BUTTON_HEADDEL2, OnBnClickedButtonHeaddel2)
	
	ON_BN_CLICKED(IDC_BUTTON_UPDEL, OnBnClickedButtonUpdel)
	ON_BN_CLICKED(IDC_BUTTON_UPDEL2, OnBnClickedButtonUpdel2)

	ON_BN_CLICKED(IDC_BUTTON_DOWNDEL, OnBnClickedButtonDowndel)
	ON_BN_CLICKED(IDC_BUTTON_DOWNDEL2, OnBnClickedButtonDowndel2)

	ON_BN_CLICKED(IDC_BUTTON_WEAPONDEL, OnBnClickedButtonWeapondel)
	ON_BN_CLICKED(IDC_BUTTON_WEAPONDEL2, OnBnClickedButtonWeapondel2)

	ON_BN_CLICKED(IDC_BUTTON_ADDWEAPENTAB, OnBnClickedButtonAddweapentab)
	ON_BN_CLICKED(IDC_BUTTON_ADDWEAPENTAB2, OnBnClickedButtonAddweapentab2)
	
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_LBN_SELCHANGE(IDC_LIST_HEAD2, OnLbnSelchangeListHead2)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSword2CheckTool message handlers

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAddhead()
{
	HRESULT hr = E_FAIL;
    TCHAR strFileNamesBuffer[800] = "";
	//TCHAR strFilePath[MAX_PATH];
	TCHAR strDriver[MAX_PATH];
	TCHAR strDir[MAX_PATH];
	TCHAR strFileName[MAX_PATH];
	TCHAR strExt[MAX_PATH];
	TCHAR strFullFileName[MAX_PATH];
	TCHAR szFilter[] ="Mesh Files (*.Mesh)|*.Mesh||";
	ModelInfo Info;

	TCHAR strOutputInfo[MAX_PATH << 1];
	IEKG3DModel* pCurModel = NULL;
    IEKG3DModelTable *pModelTable = NULL;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	CFileDialog dlgMeshOpen(TRUE,
		NULL, 
		NULL,
		OFN_HIDEREADONLY,
		szFilter
		);

	if(!m_pCurModelEditor)
		goto Exit0;
	
	
	dlgMeshOpen.m_ofn.lpstrFile = strFileNamesBuffer;
	dlgMeshOpen.m_ofn.nMaxFile = sizeof(strFileNamesBuffer);
	if (dlgMeshOpen.DoModal() != IDOK)
		goto Exit0;

	wsprintf(strFileNamesBuffer, "%s", dlgMeshOpen.GetPathName());
	_splitpath(strFileNamesBuffer, strDriver, strDir, strFileName, strExt);

	sprintf(strFullFileName, "%s%s%s.Mesh", strDriver, strDir, strFileName);
    
    pManager->GetIEKG3DModelTable(&pModelTable);

    hr = pModelTable->IELoadResourceFromFile(strFullFileName, 0, 0, 
		(IKG3DResourceBase **)&pCurModel);
	if(FAILED(hr))
	{
		sprintf(strOutputInfo, "打开模型文件%s失败.", strFullFileName);
		MessageBox(strOutputInfo, "错误");
		goto Exit0;
	}
	
	Info.pModel = pCurModel;
	Info.strModelName = strFileName;
	m_listModelHead.push_back(Info);
	FillListBoxHead();

Exit0:
	return;
}

void KSceneModelEditorSword2CheckTool::FillListBoxHead(int nIndex /* = 0*/)
{
	//TCHAR strFilePath[MAX_PATH];
	//TCHAR strDrive[MAX_PATH];
	//TCHAR strDir[MAX_PATH];
	//TCHAR strFileName[MAX_PATH];
	//TCHAR strExt[MAX_PATH];

	CListBox* pCurListBox = &m_ListBox_Head;
	_ModelList* pCurModelList = &m_listModelHead;
	int nNewItemIndex = -1;
	if (nIndex == 1)
	{
		pCurListBox = &m_ListBox_Head1;
		pCurModelList = &m_listModelHead1;
	}
	pCurListBox->SetCurSel(-1);
	int Count = pCurListBox->GetCount();
	for(int index =Count+1;index>=0;index--)
	{
		pCurListBox->DeleteString(index);
	}

	_ModelList::iterator i = pCurModelList->begin();
	while(i != pCurModelList-> end())
	{
		ModelInfo& Info = (*i);
		if(Info.pModel)
		{
			nNewItemIndex = pCurListBox->AddString(Info.strModelName.c_str());
			pCurListBox->SetItemData(nNewItemIndex, (DWORD_PTR)Info.pModel);
		}
		i++;
	}
}

void KSceneModelEditorSword2CheckTool::FillListBoxWeapon(int nIndex)
{
	/*CListBox* pCurListBox = &m_ListBox_Weapon;
	MODELLIST* pCurModelList = &m_listModelWeapon;
	if (nIndex == 1)
	{
		pCurListBox = &m_ListBox_Weapon1;
		pCurModelList = &m_listModelWeapon1;
	}
	
	pCurListBox->SetCurSel(-1);
	int Count = pCurListBox->GetCount();
	for(int index = Count + 1; index>=0; index--)
	{
		pCurListBox->DeleteString(index);
	}

	MODELLIST::iterator i = pCurModelList->begin();
	while(i != pCurModelList->end())
	{
		ModelInfo Info = (*i);
		LPMODEL pModel = Info.pModel;

		if(pModel)
		{
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",pModel->m_szMeshFileName);
			_splitpath(path_buffer, drive, dir, fname, ext);

			int Index = pCurListBox->AddString(Info.Name.c_str());
			pCurListBox->SetItemData(Index,(DWORD)pModel);
		}
		i++;
	}*/
}

void KSceneModelEditorSword2CheckTool::FillListBoxUpBody(int nIndex)
{
	/*CListBox* pCurListBox = &m_ListBox_Up;
	MODELLIST* pCurModelList = &m_listModelUpBody;
	if (nIndex == 1)
	{
		pCurListBox = &m_ListBox_Up1;
		pCurModelList = &m_listModelUpBody1;
	}

	m_ListBox_Up.SetCurSel(-1);
	int Count = m_ListBox_Up.GetCount();
	for(int index = Count + 1; index >= 0; index--)
	{
		pCurListBox->DeleteString(index);
	}
	
	MODELLIST::iterator i = pCurModelList->begin();
	while(i != pCurModelList->end())
	{
		ModelInfo Info = (*i);
		LPMODEL pModel = Info.pModel;

		if(pModel)
		{
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",pModel->m_szMeshFileName);
			_splitpath(path_buffer, drive, dir, fname, ext);

			int Index = pCurListBox->AddString(Info.Name.c_str());
			pCurListBox->SetItemData(Index,(DWORD)pModel);
		}
		i++;
	}*/
}

void KSceneModelEditorSword2CheckTool::FillListBoxDownBody(int nIndex)
{
	/*CListBox* pCurListBox = &m_ListBox_Down;
	MODELLIST* pCurModelList = &m_listModelDownBody;
	if (nIndex == 1)
	{
		pCurListBox = &m_ListBox_Down1;
		pCurModelList = &m_listModelDownBody1;
	}

	pCurListBox->SetCurSel(-1);
	int Count = pCurListBox->GetCount();
	for(int index = Count + 1; index >= 0; index--)
	{
		pCurListBox->DeleteString(index);
	}

	MODELLIST::iterator i = pCurModelList->begin();
	while(i != pCurModelList->end())
	{
		ModelInfo Info = (*i);
		LPMODEL pModel = Info.pModel;

		if(pModel)
		{
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",pModel->m_szMeshFileName);
			_splitpath(path_buffer, drive, dir, fname, ext);

			int Index = pCurListBox->AddString(Info.Name.c_str());
			pCurListBox->SetItemData(Index,(DWORD)pModel);
		}
		i++;
	}*/
}

void KSceneModelEditorSword2CheckTool::OnLbnSelchangeListHead()
{
	// TODO: Add your control notification handler code here
	UpdataModels();
}

void KSceneModelEditorSword2CheckTool::UpdataModels()
{
	IEKG3DModel* pModelBody = NULL;
	IEKG3DModel* pModelBody1 = NULL;
	int nCurSel = 0;
	IEKG3DModel* pModel = NULL;
	if(!m_pCurModelEditor)
		return;

	m_pCurModelEditor->ClearModel();

////////////////////////////////////////////////////////////////////////////
////Head
	nCurSel = m_ListBox_Head.GetCurSel();
	if (nCurSel >= 0)
	{
		pModel = reinterpret_cast<IEKG3DModel*>(m_ListBox_Head.GetItemData(nCurSel));
		if(pModel)
		{
			m_pCurModelEditor->AddModel(pModel);
		}
	}

//	//Second
//	Index = m_ListBox_Head1.GetCurSel();
//	if (Index >= 0)
//	{
//		LPMODEL pModel = (LPMODEL) m_ListBox_Head1.GetItemData(Index);
//		if(pModel)
//		{
//			m_lpModelEditor->AddRenderMesh(pModel);
//		}
//	}
////Head end
////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////
////Upper body
//	
//	Index = m_ListBox_Up.GetCurSel();
//	if(Index>=0)
//	{
//		LPMODEL pModel = (LPMODEL) m_ListBox_Up.GetItemData(Index);
//		if(pModel)
//		{
//			pModelBody = pModel;
//
//			m_lpModelEditor->AddRenderMesh(pModel);
//
//			if(m_lpCurModelBody!=pModel)
//			{
//				m_lpCurModelBody = pModel;
//				FillComboSocket(pModel);
//				if(m_nMode==1)
//				{
//					FillComboAnimation(pModel);
//				}
//			}
//		}
//	}
//	
//	//Second
//	Index = m_ListBox_Up1.GetCurSel();
//	if(Index >= 0)
//	{
//		LPMODEL pModel = (LPMODEL) m_ListBox_Up1.GetItemData(Index);
//		if(pModel)
//		{
//			pModelBody1 = pModel;
//
//			m_lpModelEditor->AddRenderMesh(pModel);
//
//			if(m_lpCurModelBody != pModel)
//			{
//				m_lpCurModelBody = pModel;
//				FillComboSocket(pModel, 1);
//				if(m_nMode1 == 1)
//				{
//					FillComboAnimation(pModel, 1);
//				}
//			}
//		}
//	}
////Upper body
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
////Lower body	
//	Index = m_ListBox_Down.GetCurSel();
//	if(Index>=0)
//	{
//		LPMODEL pModel = (LPMODEL) m_ListBox_Down.GetItemData(Index);
//		if(pModel)
//		{
//			m_lpModelEditor->AddRenderMesh(pModel);
//		}
//	}
//	
//	//Second
//	Index = m_ListBox_Down1.GetCurSel();
//	if(Index >= 0)
//	{
//		LPMODEL pModel = (LPMODEL) m_ListBox_Down1.GetItemData(Index);
//		if(pModel)
//		{
//			m_lpModelEditor->AddRenderMesh(pModel);
//		}
//	}
////Lower body
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
////Weapon
//
//	Index = m_ListBox_Weapon.GetCurSel();
//	if(Index>=0)
//	{
//		LPMODEL pModel = (LPMODEL) m_ListBox_Weapon.GetItemData(Index);
//		if(pModel)
//		{
//			m_lpModelEditor->AddRenderMesh(pModel);
//
//			if(pModelBody)
//			{
//				int Index = m_Combo_Socket.GetCurSel();
//				if(Index>=0)
//				{
//					DWORD SocketIndex = m_Combo_Socket.GetItemData(Index);
//					pModel->Bind(pModelBody,
//						(LPSTR)pModelBody->m_lpSockets[SocketIndex].scName.c_str());
//				}
//				else
//				{
//					int k = 0;
//				}
//			}
//		}
//	}
//
//	//Second
//	Index = m_ListBox_Weapon1.GetCurSel();
//	if(Index >= 0)
//	{
//		LPMODEL pModel = (LPMODEL) m_ListBox_Weapon1.GetItemData(Index);
//		if(pModel)
//		{
//			m_lpModelEditor->AddRenderMesh(pModel);
//
//			if(pModelBody1)
//			{
//				int Index = m_Combo_Socket1.GetCurSel();
//				if(Index>=0)
//				{
//					DWORD SocketIndex = m_Combo_Socket1.GetItemData(Index);
//					pModel->Bind(pModelBody1,
//						(LPSTR)pModelBody1->m_lpSockets[SocketIndex].scName.c_str());
//				}
//				else
//				{
//					int k = 0;
//				}
//			}
//		}
//	}

//Weapon
//////////////////////////////////////////////////////////////////////////
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAddup()
{
	//// TODO: Add your control notification handler code here
	//// TODO: Add your control notification handler code here
	//if(!m_lpModelEditor)
	//	return;
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (dlg.DoModal() == IDOK)
	//{
	//	TCHAR path_buffer[256];
	//	TCHAR drive[256];
	//	TCHAR dir[256];
	//	TCHAR fname[256];
	//	TCHAR ext[256];

	//	wsprintf(path_buffer,"%s",dlg.GetPathName());
	//	_splitpath(path_buffer, drive, dir, fname, ext);

	//	TCHAR filename[256];

	//	wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);

	//	LPMODEL pModel = NULL;

	//	if(FAILED(g_cMeshTable.Get1NewXMesh(&pModel)))
	//		return;

	//	if(FAILED(pModel->LoadMesh(filename)))
	//		return;

	//	//////////////////////////////////////////////////////////////////////////
	//	{
	//		char bigBuff[800] = "";  // maximum common dialog buffer size
	//		char szFilter[] ="材质文件 (*.Mtl)|*.Mtl||";

	//		CFileDialog dlg(true, NULL, NULL,
	//			OFN_HIDEREADONLY , szFilter);
	//		// Modify OPENFILENAME members directly to point to bigBuff
	//		dlg.m_ofn.lpstrFile = bigBuff;
	//		dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//		if (dlg.DoModal() == IDOK)
	//		{
	//			wsprintf(filename,"%s",dlg.GetPathName());
	//			if(FAILED(pModel->LoadMaterial(filename)))
	//				return;
	//		}
	//	}

	//	if(m_nMode==1)
	//	{
	//		//////////////////////////////////////////////////////////////////////////
	//		{
	//			char bigBuff[800] = "";  // maximum common dialog buffer size
	//			char szFilter[] ="动作文件 (*.Ani)|*.Ani||";

	//			CFileDialog dlg(true, NULL, NULL,
	//				OFN_HIDEREADONLY , szFilter);
	//			// Modify OPENFILENAME members directly to point to bigBuff
	//			dlg.m_ofn.lpstrFile = bigBuff;
	//			dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//			if (dlg.DoModal() == IDOK)
	//			{
	//				wsprintf(filename,"%s",dlg.GetPathName());
	//				if(FAILED(pModel->LoadAnimation(filename)))
	//					return;
	//			}
	//		}
	//	}


	//	ModelInfo Info(pModel,fname);
	//	m_listModelUpBody.push_back(Info);

	//	FillListBoxUpBody();
	//}
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAdddown()
{
	//if(!m_lpModelEditor)
	//	return;
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (dlg.DoModal() == IDOK)
	//{
	//	TCHAR path_buffer[256];
	//	TCHAR drive[256];
	//	TCHAR dir[256];
	//	TCHAR fname[256];
	//	TCHAR ext[256];

	//	wsprintf(path_buffer,"%s",dlg.GetPathName());
	//	_splitpath(path_buffer, drive, dir, fname, ext);

	//	TCHAR filename[256];

	//	wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);

	//	LPMODEL pModel = NULL;

	//	if(FAILED(g_cMeshTable.Get1NewXMesh(&pModel)))
	//		return;

	//	if(FAILED(pModel->LoadMesh(filename)))
	//		return;

	//	//////////////////////////////////////////////////////////////////////////
	//	{
	//		char bigBuff[800] = "";  // maximum common dialog buffer size
	//		char szFilter[] ="材质文件 (*.Mtl)|*.Mtl||";

	//		CFileDialog dlg(true, NULL, NULL,
	//			OFN_HIDEREADONLY , szFilter);
	//		// Modify OPENFILENAME members directly to point to bigBuff
	//		dlg.m_ofn.lpstrFile = bigBuff;
	//		dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//		if (dlg.DoModal() == IDOK)
	//		{
	//			wsprintf(filename,"%s",dlg.GetPathName());
	//			if(FAILED(pModel->LoadMaterial(filename)))
	//				return;
	//		}
	//	}
		/*//////////////////////////////////////////////////////////////////////////
		{
			char bigBuff[800] = "";  // maximum common dialog buffer size
			char szFilter[] ="动作文件 (*.Ani)|*.Ani||";

			CFileDialog dlg(true, NULL, NULL,
				OFN_HIDEREADONLY , szFilter);
			// Modify OPENFILENAME members directly to point to bigBuff
			dlg.m_ofn.lpstrFile = bigBuff;
			dlg.m_ofn.nMaxFile = sizeof(bigBuff);
			if (dlg.DoModal() == IDOK)
			{
				wsprintf(filename,"%s",dlg.GetPathName());
				if(FAILED(pModel->LoadAnimation(filename)))
					return;
			}
		}*/

		/*ModelInfo Info(pModel,fname);
		m_listModelDownBody.push_back(Info);

		FillListBoxDownBody();
	}*/
}

void KSceneModelEditorSword2CheckTool::OnLbnSelchangeListUp()
{
	// TODO: Add your control notification handler code here
	/*UpdataModels();

	if ((static_cast<CButton*>(GetDlgItem(IDC_CHECK_EDITUP)))->GetCheck())
	{
		int Index = m_ListBox_Up.GetCurSel();
		if(Index>=0)
		{
			LPMODEL pModel = (LPMODEL) m_ListBox_Up.GetItemData(Index);
			if(pModel)
			{
				m_lpModelEditor->m_SelectableList.clear();
				m_lpModelEditor->m_SelectedList.clear();

				m_lpModelEditor->AddSelectableMesh(pModel);
				m_lpModelEditor->AddSelectedMesh(pModel);

				m_lpModelEditor->SetCurMesh(pModel);
			}
		}
	}
	else
	{
		m_lpModelEditor->m_SelectableList.clear();
		m_lpModelEditor->m_SelectedList.clear();
		m_lpModelEditor->SetCurMesh(NULL);
	}*/
}

void KSceneModelEditorSword2CheckTool::OnLbnSelchangeListDown()
{
	// TODO: Add your control notification handler code here
	UpdataModels();

}

BOOL KSceneModelEditorSword2CheckTool::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//// TODO: Add extra initialization here
	//m_nMode = 0;
	//m_nMode1 = 0;
	//m_bSynAniChange = FALSE;
	//UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAniopen()
{
	// TODO: Add your control notification handler code here

	//TCHAR filename[256];
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="动作文件 (*.Ani)|*.Ani||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (dlg.DoModal() == IDOK)
	//{
	//	wsprintf(filename,"%s",dlg.GetPathName());

	//	BOOL K = 0;

	//	MODELLIST::iterator i = m_listModelHead.begin();
	//	while(i!=m_listModelHead.end())
	//	{
	//		ModelInfo Info = (*i);
	//		LPMODEL pModel = Info.pModel;

	//		//pModel->ClearnAnimations();
	//		if(FAILED(pModel->LoadAnimation(filename)))
	//			return;
	//		if(K==0)
	//		{
	//			K++;
	//			FillComboAnimation(pModel);
	//		}
	//		i++;
	//	}
	//	
	//	i = m_listModelUpBody.begin();
	//	while(i!=m_listModelUpBody.end())
	//	{
	//		ModelInfo Info = (*i);
	//		LPMODEL pModel = Info.pModel;

	//		//pModel->ClearnAnimations();
	//		if(FAILED(pModel->LoadAnimation(filename)))
	//			return;
	//		if(K==0)
	//		{
	//			K++;
	//			FillComboAnimation(pModel);
	//		}
	//		i++;
	//	}	
	//	
	//	i = m_listModelDownBody.begin();
	//	while(i!=m_listModelDownBody.end())
	//	{
	//		ModelInfo Info = (*i);
	//		LPMODEL pModel = Info.pModel;

	//		//pModel->ClearnAnimations();
	//		if(FAILED(pModel->LoadAnimation(filename)))
	//			return;
	//		if(K==0)
	//		{
	//			K++;
	//			FillComboAnimation(pModel);
	//		}
	//		i++;
	//	}
	//}
}

void KSceneModelEditorSword2CheckTool::PlayAnimation(int AniIndex, DWORD PlayMethod, float Speed, int nIndex)
{
	/*MODELLIST* pHead = &m_listModelHead;
	MODELLIST* pUpBody = &m_listModelUpBody;
	MODELLIST* pLowBody = &m_listModelDownBody;

	if (nIndex == 1)
	{
		pHead = &m_listModelHead1;
		pUpBody = &m_listModelUpBody1;
		pLowBody = &m_listModelDownBody1;

	}

	MODELLIST::iterator i = pHead->begin();
	while(i != pHead->end())
	{
		ModelInfo Info = (*i);
		LPMODEL pModel = Info.pModel;
		pModel->PlayAnimation(AniIndex,PlayMethod,1.0f);
		i++;
	}

	i = pUpBody->begin();
	while(i != pUpBody->end())
	{
		ModelInfo Info = (*i);
		LPMODEL pModel = Info.pModel;
		pModel->PlayAnimation(AniIndex,PlayMethod,1.0f);
		i++;
	}	

	i = pLowBody->begin();
	while(i != pLowBody->end())
	{
		ModelInfo Info = (*i);
		LPMODEL pModel = Info.pModel;
		pModel->PlayAnimation(AniIndex,PlayMethod,1.0f);
		i++;
	}*/
}
void KSceneModelEditorSword2CheckTool::OnBnClickedButtonPlay()
{
	// TODO: Add your control notification handler code here
	/*int Index = m_Combo_Ani.GetCurSel();
	if(Index < 0)
		return;
	DWORD AniIndex = m_Combo_Ani.GetItemData(Index);

	DWORD Method = PLAY_CIRCLE;

	PlayAnimation(AniIndex,Method,1.0f);*/
}

//void KSceneModelEditorSword2CheckTool::FillComboAnimation(LPMODEL pModel, int nIndex)
//{
	/*CComboBox* pCurBox = &m_Combo_Ani;
	if (nIndex == 1)
	{
		pCurBox = &m_Combo_Ani1;
	}
	
	pCurBox->SetCurSel(-1);
	int Count = pCurBox->GetCount();
	for(int i = Count + 1; i >= 0 ; i--)
	{
		pCurBox->DeleteString(i);
	}

	pCurBox->Clear();
	
	if(!pModel)
		return;

	for(DWORD i = 0; i < pModel->m_dNumAnimation; i++)
	{
		int Index = pCurBox->AddString(pModel->m_lpAnimations[i]->Name);
		pCurBox->SetItemData(Index,i);
	}
	pCurBox->SetCurSel(0);*/
//}

//void KSceneModelEditorSword2CheckTool::FillComboSocket(LPMODEL pModel, int nIndex)
//{
	/*CComboBox* pCurBox = &m_Combo_Socket;
	if (nIndex == 1)
	{
		pCurBox = &m_Combo_Socket1;
	}

	pCurBox->SetCurSel(-1);
	int Count = pCurBox->GetCount();
	for(int i = Count + 1; i >= 0; i--)
	{
		pCurBox->DeleteString(i);
	}

	pCurBox->Clear();

	if(!pModel)
		return;

	for(DWORD i=0;i<pModel->m_dNumSocket;i++)
	{
		int Index = pCurBox->AddString(pModel->m_lpSockets[i].scName.c_str());
		pCurBox->SetItemData(Index,i);
	}
//	pCurBox->SetCurSel(0);*/
//}

void KSceneModelEditorSword2CheckTool::OnCbnSelchangeComboAniname()
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	int Index = m_Combo_Ani.GetCurSel();
	if(Index<0)
		return;
	DWORD AniIndex = m_Combo_Ani.GetItemData(Index);

	DWORD Method = PLAY_CIRCLE;

	PlayAnimation(AniIndex,Method,1.0f);
	if (m_bSynAniChange && Index < m_Combo_Ani1.GetCount())
	{
		m_Combo_Ani1.SetCurSel(Index);
		AniIndex = m_Combo_Ani1.GetItemData(Index);
		PlayAnimation(AniIndex, Method, 1.0f, 1);
	}*/

}

//void KSceneModelEditorSword2CheckTool::OpenFromTabFile(MODELLIST* pList,BOOL bAnimation, int nIndex)
//{
//	// TODO: Add your control notification handler code here
//	//TCHAR filename[256];
//	//char bigBuff[800] = "";  // maximum common dialog buffer size
//	//char szFilter[] ="配置文件 (*.txt)|*.txt||";
//
//	//CFileDialog dlg(true, NULL, NULL,
//	//	OFN_HIDEREADONLY , szFilter);
//	//// Modify OPENFILENAME members directly to point to bigBuff
//	//dlg.m_ofn.lpstrFile = bigBuff;
//	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
//	//if (dlg.DoModal() == IDOK)
//	//{
//	//	wsprintf(filename,"%s",dlg.GetPathName());
//	//	ITabFile* TabFile = g_OpenTabFile(filename);
//	//	if (!TabFile)
//	//		return;
//
//	//	int Row = TabFile->GetHeight();
//
//	//	TCHAR MeshName[256];
//	//	TCHAR MtlName[256];
//	//	TCHAR AniName[256];
//	//	TCHAR DescName[256];
//	//	TCHAR Name[256];
//
//	//	for(int i=1;i<Row;i++)
//	//	{
//	//		TabFile->GetString(i+1,1,"",DescName,256);
//	//		TabFile->GetString(i+1,2,"",MeshName,256);
//	//		TabFile->GetString(i+1,3,"",MtlName ,256);
//	//		TabFile->GetString(i+1,4,"",AniName ,256);
//
//	//		TCHAR AAA[256];
//	//		wsprintf(AAA,"正在打开%s %d OF %d",DescName,i,Row);
//	//		this->SetWindowText(AAA);
//
//	//		if(bAnimation)
//	//		{
//	//			TabFile->GetString(i+1,4,"",AniName ,256);
//	//		}
//
//	//		if(FAILED(g_cFileFinder.FindFile(Name,MeshName)))
//	//		{
//	//			//MessageBox(MeshName,"找不到文件");
//	//			continue;
//	//		}
//
//	//		LPMODEL pModel = NULL;
//	//		if(FAILED(g_cMeshTable.Get1NewXMesh(&pModel)))
//	//			continue;
//	//		if(FAILED(pModel->LoadMesh(Name)))
//	//			continue;
//
//	//		//////////////////////////////////////////////////////////////////////////
//	//		if(FAILED(g_cFileFinder.FindFile(Name,MtlName)))
//	//		{
//	//			//MessageBox(MtlName,"找不到文件");
//	//		}
//	//		else
//	//		{
//	//			pModel->LoadMaterial(Name);
//
//	//		}
//
//	//		//////////////////////////////////////////////////////////////////////////
//	//		if(bAnimation)
//	//		{
//	//			if(FAILED(g_cFileFinder.FindFile(Name,AniName)))
//	//			{
//	//				//MessageBox(AniName,"找不到文件");
//	//			}
//	//			else
//	//			{
//	//				pModel->LoadAnimation(Name);
//	//			}
//	//		}
//
//	//		ModelInfo Info(pModel,DescName);
//	//		pList->push_back(Info);
//	//	}
//
//	//	TabFile->Release();
//	//}
//
//	//SetWindowText("模型批量检查工具");
//}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAddheadtab()
{
	/*OpenFromTabFile(&m_listModelHead,FALSE);

	FillListBoxHead();*/
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAdduptab()
{
	/*if(m_nMode==1)
	{
		OpenFromTabFile(&m_listModelUpBody,TRUE);
	}
	else
	{
		OpenFromTabFile(&m_listModelUpBody,FALSE);
	}

	FillListBoxUpBody();*/
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAdddowntab()
{
	/*OpenFromTabFile(&m_listModelDownBody,FALSE);

	FillListBoxDownBody();*/
}

void KSceneModelEditorSword2CheckTool::OnCbnSelchangeComboMode()
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);

	if(m_nMode==1)
	{
		m_ListBox_Down.EnableWindow(FALSE);
		m_ListBox_Head.EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_ADDHEAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ADDHEADTAB)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_HEADDEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_HEAD)->EnableWindow(FALSE);

		GetDlgItem(IDC_STATIC_UP)->SetWindowText("NPC模型");

		GetDlgItem(IDC_BUTTON_ADDDOWN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ADDDOWNTAB)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWNDEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_DOWN)->EnableWindow(FALSE);
	}
	else
	{
		m_ListBox_Down.EnableWindow(TRUE);
		m_ListBox_Head.EnableWindow(TRUE);
 
		GetDlgItem(IDC_BUTTON_ADDHEAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADDHEADTAB)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_HEADDEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_HEAD)->EnableWindow(TRUE);

		GetDlgItem(IDC_STATIC_UP)->SetWindowText("上身");

		GetDlgItem(IDC_BUTTON_ADDDOWN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADDDOWNTAB)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DOWNDEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_DOWN)->EnableWindow(TRUE);
	}*/
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAddweapen()
{
	//// TODO: Add your control notification handler code here
	//if(!m_lpModelEditor)
	//	return;
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (dlg.DoModal() == IDOK)
	//{
	//	TCHAR path_buffer[256];
	//	TCHAR drive[256];
	//	TCHAR dir[256];
	//	TCHAR fname[256];
	//	TCHAR ext[256];

	//	wsprintf(path_buffer,"%s",dlg.GetPathName());
	//	_splitpath(path_buffer, drive, dir, fname, ext);

	//	TCHAR filename[256];

	//	wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);

	//	LPMODEL pModel = NULL;

	//	if(FAILED(g_cMeshTable.Get1NewXMesh(&pModel)))
	//		return;

	//	if(FAILED(pModel->LoadMesh(filename)))
	//		return;

	//	//////////////////////////////////////////////////////////////////////////
	//	{
	//		char bigBuff[800] = "";  // maximum common dialog buffer size
	//		char szFilter[] ="材质文件 (*.Mtl)|*.Mtl||";

	//		CFileDialog dlg(true, NULL, NULL,
	//			OFN_HIDEREADONLY , szFilter);
	//		// Modify OPENFILENAME members directly to point to bigBuff
	//		dlg.m_ofn.lpstrFile = bigBuff;
	//		dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//		if (dlg.DoModal() == IDOK)
	//		{
	//			wsprintf(filename,"%s",dlg.GetPathName());
	//			if(FAILED(pModel->LoadMaterial(filename)))
	//				return;
	//		}
	//	}

	//	ModelInfo Info(pModel,fname);
	//	m_listModelWeapon.push_back(Info);

	//	FillListBoxWeapon();
	//}
}

void KSceneModelEditorSword2CheckTool::OnLbnSelchangeListWeapen()
{
	// TODO: Add your control notification handler code here
	UpdataModels();
}

void KSceneModelEditorSword2CheckTool::OnCbnSelchangeComboSocket()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	UpdataModels();
}

void KSceneModelEditorSword2CheckTool::OnBnClickedCheckEditup()
{
	// TODO: Add your control notification handler code here
	/*if((static_cast<CButton*>(GetDlgItem(IDC_CHECK_EDITUP)))->GetCheck())
	{
		int Index = m_ListBox_Up.GetCurSel();
		if(Index>=0)
		{
			LPMODEL pModel = (LPMODEL) m_ListBox_Up.GetItemData(Index);
			if(pModel)
			{
				m_lpModelEditor->m_SelectableList.clear();
				m_lpModelEditor->m_SelectedList.clear();

				m_lpModelEditor->AddSelectableMesh(pModel);
				m_lpModelEditor->AddSelectedMesh(pModel);

				m_lpModelEditor->SetCurMesh(pModel);
			}
		}
	}
	else
	{
		m_lpModelEditor->m_SelectableList.clear();
		m_lpModelEditor->m_SelectedList.clear();
		m_lpModelEditor->SetCurMesh(NULL);
	}*/
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAddhead2()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpModelEditor)
	//	return;
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (dlg.DoModal() == IDOK)
	//{
	//	TCHAR path_buffer[256];
	//	TCHAR drive[256];
	//	TCHAR dir[256];
	//	TCHAR fname[256];
	//	TCHAR ext[256];

	//	wsprintf(path_buffer,"%s",dlg.GetPathName());
	//	_splitpath(path_buffer, drive, dir, fname, ext);

	//	TCHAR filename[256];

	//	wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);

	//	LPMODEL pModel = NULL;

	//	if(FAILED(g_cMeshTable.Get1NewXMesh(&pModel)))
	//		return;

	//	if(FAILED(pModel->LoadMesh(filename)))
	//		return;

	//	//////////////////////////////////////////////////////////////////////////
	//	{
	//		char bigBuff[800] = "";  // maximum common dialog buffer size
	//		char szFilter[] ="材质文件 (*.Mtl)|*.Mtl||";

	//		CFileDialog dlg(true, NULL, NULL,
	//			OFN_HIDEREADONLY , szFilter);
	//		// Modify OPENFILENAME members directly to point to bigBuff
	//		dlg.m_ofn.lpstrFile = bigBuff;
	//		dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//		if (dlg.DoModal() == IDOK)
	//		{
	//			wsprintf(filename,"%s",dlg.GetPathName());
	//			if(FAILED(pModel->LoadMaterial(filename)))
	//				return;
	//		}
	//	}
	//
	//	ModelInfo Info(pModel,fname);
	//	m_listModelHead1.push_back(Info);

	//	FillListBoxHead(1);
	//}
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAddheadtab2()
{
	// TODO: Add your control notification handler code here
	//OpenFromTabFile(&m_listModelHead1,FALSE);

	FillListBoxHead(1);
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonHeaddel2()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAddup2()
{
	//if(!m_lpModelEditor)
	//	return;
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (dlg.DoModal() == IDOK)
	//{
	//	TCHAR path_buffer[256];
	//	TCHAR drive[256];
	//	TCHAR dir[256];
	//	TCHAR fname[256];
	//	TCHAR ext[256];

	//	wsprintf(path_buffer,"%s",dlg.GetPathName());
	//	_splitpath(path_buffer, drive, dir, fname, ext);

	//	TCHAR filename[256];

	//	wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);

	//	LPMODEL pModel = NULL;

	//	if(FAILED(g_cMeshTable.Get1NewXMesh(&pModel)))
	//		return;

	//	if(FAILED(pModel->LoadMesh(filename)))
	//		return;

	//	//////////////////////////////////////////////////////////////////////////
	//	{
	//		char bigBuff[800] = "";  // maximum common dialog buffer size
	//		char szFilter[] ="材质文件 (*.Mtl)|*.Mtl||";

	//		CFileDialog dlg(true, NULL, NULL,
	//			OFN_HIDEREADONLY , szFilter);
	//		// Modify OPENFILENAME members directly to point to bigBuff
	//		dlg.m_ofn.lpstrFile = bigBuff;
	//		dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//		if (dlg.DoModal() == IDOK)
	//		{
	//			wsprintf(filename,"%s",dlg.GetPathName());
	//			if(FAILED(pModel->LoadMaterial(filename)))
	//				return;
	//		}
	//	}

	//	if(m_nMode1 == 1)
	//	{
	//		//////////////////////////////////////////////////////////////////////////
	//		{
	//			char bigBuff[800] = "";  // maximum common dialog buffer size
	//			char szFilter[] ="动作文件 (*.Ani)|*.Ani||";

	//			CFileDialog dlg(true, NULL, NULL,
	//				OFN_HIDEREADONLY , szFilter);
	//			// Modify OPENFILENAME members directly to point to bigBuff
	//			dlg.m_ofn.lpstrFile = bigBuff;
	//			dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//			if (dlg.DoModal() == IDOK)
	//			{
	//				wsprintf(filename,"%s",dlg.GetPathName());
	//				if(FAILED(pModel->LoadAnimation(filename)))
	//					return;
	//			}
	//		}
	//	}


	//	ModelInfo Info(pModel,fname);
	//	m_listModelUpBody1.push_back(Info);

	//	FillListBoxUpBody(1);
	//}
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAdduptab2()
{
	// TODO: Add your control notification handler code here
	/*if(m_nMode==1)
	{
		OpenFromTabFile(&m_listModelUpBody1,TRUE);
	}
	else
	{
		OpenFromTabFile(&m_listModelUpBody1,FALSE);
	}*/

	FillListBoxUpBody(1);
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonUpdel2()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSword2CheckTool::OnBnClickedCheckEditup2()
{
	// TODO: Add your control notification handler code here
	/*if((static_cast<CButton*>(GetDlgItem(IDC_CHECK_EDITUP2)))->GetCheck())
	{
		int Index = m_ListBox_Up1.GetCurSel();
		if(Index>=0)
		{
			LPMODEL pModel = (LPMODEL) m_ListBox_Up1.GetItemData(Index);
			if(pModel)
			{
				m_lpModelEditor->m_SelectableList.clear();
				m_lpModelEditor->m_SelectedList.clear();

				m_lpModelEditor->AddSelectableMesh(pModel);
				m_lpModelEditor->AddSelectedMesh(pModel);

				m_lpModelEditor->SetCurMesh(pModel);
			}
		}
	}
	else
	{
		m_lpModelEditor->m_SelectableList.clear();
		m_lpModelEditor->m_SelectedList.clear();
		m_lpModelEditor->SetCurMesh(NULL);
	}*/
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAdddown2()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpModelEditor)
	//	return;
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (dlg.DoModal() == IDOK)
	//{
	//	TCHAR path_buffer[256];
	//	TCHAR drive[256];
	//	TCHAR dir[256];
	//	TCHAR fname[256];
	//	TCHAR ext[256];

	//	wsprintf(path_buffer,"%s",dlg.GetPathName());
	//	_splitpath(path_buffer, drive, dir, fname, ext);

	//	TCHAR filename[256];

	//	wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);

	//	LPMODEL pModel = NULL;

	//	if(FAILED(g_cMeshTable.Get1NewXMesh(&pModel)))
	//		return;

	//	if(FAILED(pModel->LoadMesh(filename)))
	//		return;

	//	//////////////////////////////////////////////////////////////////////////
	//	{
	//		char bigBuff[800] = "";  // maximum common dialog buffer size
	//		char szFilter[] ="材质文件 (*.Mtl)|*.Mtl||";

	//		CFileDialog dlg(true, NULL, NULL,
	//			OFN_HIDEREADONLY , szFilter);
	//		// Modify OPENFILENAME members directly to point to bigBuff
	//		dlg.m_ofn.lpstrFile = bigBuff;
	//		dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//		if (dlg.DoModal() == IDOK)
	//		{
	//			wsprintf(filename,"%s",dlg.GetPathName());
	//			if(FAILED(pModel->LoadMaterial(filename)))
	//				return;
	//		}
	//	}
	//	ModelInfo Info(pModel,fname);
	//	m_listModelDownBody1.push_back(Info);

	//	FillListBoxDownBody(1);
	//}
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAdddowntab3()
{
	// TODO: Add your control notification handler code here
	//OpenFromTabFile(&m_listModelDownBody1,FALSE);

	FillListBoxDownBody(1);
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonDowndel2()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAddweapen2()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpModelEditor)
	//	return;
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (dlg.DoModal() == IDOK)
	//{
	//	TCHAR path_buffer[256];
	//	TCHAR drive[256];
	//	TCHAR dir[256];
	//	TCHAR fname[256];
	//	TCHAR ext[256];

	//	wsprintf(path_buffer,"%s",dlg.GetPathName());
	//	_splitpath(path_buffer, drive, dir, fname, ext);

	//	TCHAR filename[256];

	//	wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);

	//	LPMODEL pModel = NULL;

	//	if(FAILED(g_cMeshTable.Get1NewXMesh(&pModel)))
	//		return;

	//	if(FAILED(pModel->LoadMesh(filename)))
	//		return;

	//	//////////////////////////////////////////////////////////////////////////
	//	{
	//		char bigBuff[800] = "";  // maximum common dialog buffer size
	//		char szFilter[] ="材质文件 (*.Mtl)|*.Mtl||";

	//		CFileDialog dlg(true, NULL, NULL,
	//			OFN_HIDEREADONLY , szFilter);
	//		// Modify OPENFILENAME members directly to point to bigBuff
	//		dlg.m_ofn.lpstrFile = bigBuff;
	//		dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//		if (dlg.DoModal() == IDOK)
	//		{
	//			wsprintf(filename,"%s",dlg.GetPathName());
	//			if(FAILED(pModel->LoadMaterial(filename)))
	//				return;
	//		}
	//	}

	//	ModelInfo Info(pModel,fname);
	//	m_listModelWeapon1.push_back(Info);

	//	FillListBoxWeapon(1);
	//}
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAddweapentab2()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonWeapondel2()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonPlay2()
{
	// TODO: Add your control notification handler code here
	//int Index = m_Combo_Ani1.GetCurSel();
	//if(Index < 0)
	//	return;
	//DWORD AniIndex = m_Combo_Ani1.GetItemData(Index);

	//DWORD Method = PLAY_CIRCLE;

	//PlayAnimation(AniIndex,Method,1.0f, 1);
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAniopen2()
{
	// TODO: Add your control notification handler code here
	//TCHAR filename[256];
	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="动作文件 (*.Ani)|*.Ani||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if (dlg.DoModal() == IDOK)
	//{
	//	wsprintf(filename,"%s",dlg.GetPathName());

	//	BOOL K = 0;

	//	MODELLIST::iterator i = m_listModelHead1.begin();
	//	while(i!=m_listModelHead1.end())
	//	{
	//		ModelInfo Info = (*i);
	//		LPMODEL pModel = Info.pModel;

	//		//pModel->ClearnAnimations();
	//		if(FAILED(pModel->LoadAnimation(filename)))
	//			return;
	//		if(K==0)
	//		{
	//			K++;
	//			FillComboAnimation(pModel, 1);
	//		}
	//		i++;
	//	}

	//	i = m_listModelUpBody1.begin();
	//	while(i!=m_listModelUpBody1.end())
	//	{
	//		ModelInfo Info = (*i);
	//		LPMODEL pModel = Info.pModel;

	//		//pModel->ClearnAnimations();
	//		if(FAILED(pModel->LoadAnimation(filename)))
	//			return;
	//		if(K==0)
	//		{
	//			K++;
	//			FillComboAnimation(pModel, 1);
	//		}
	//		i++;
	//	}	

	//	i = m_listModelDownBody1.begin();
	//	while(i!=m_listModelDownBody1.end())
	//	{
	//		ModelInfo Info = (*i);
	//		LPMODEL pModel = Info.pModel;

	//		//pModel->ClearnAnimations();
	//		if(FAILED(pModel->LoadAnimation(filename)))
	//			return;
	//		if(K==0)
	//		{
	//			K++;
	//			FillComboAnimation(pModel, 1);
	//		}
	//		i++;
	//	}
	//}
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonHeaddel()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonUpdel()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonDowndel()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonWeapondel()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButtonAddweapentab()
{
	// TODO: Add your control notification handler code here
}

//void KSceneModelEditorSword2CheckTool::OnLbnSelchangeListHead2()
//{
//	// TODO: Add your control notification handler code here
//}

void KSceneModelEditorSword2CheckTool::OnLbnSelchangeListUp2()
{
	/*UpdataModels();

	if (static_cast<CButton*>(GetDlgItem(IDC_CHECK_EDITUP))->GetCheck())
	{
		int Index = m_ListBox_Up1.GetCurSel();
		if(Index >= 0)
		{
			LPMODEL pModel = (LPMODEL) m_ListBox_Up1.GetItemData(Index);
			if(pModel)
			{
				m_lpModelEditor->m_SelectableList.clear();
				m_lpModelEditor->m_SelectedList.clear();

				m_lpModelEditor->AddSelectableMesh(pModel);
				m_lpModelEditor->AddSelectedMesh(pModel);

				m_lpModelEditor->SetCurMesh(pModel);
			}
		}
	}
	else
	{
		m_lpModelEditor->m_SelectableList.clear();
		m_lpModelEditor->m_SelectedList.clear();
		m_lpModelEditor->SetCurMesh(NULL);
	}*/
}

void KSceneModelEditorSword2CheckTool::OnLbnSelchangeListDown2()
{
	// TODO: Add your control notification handler code here
	UpdataModels();
}

void KSceneModelEditorSword2CheckTool::OnLbnSelchangeListWeapen2()
{
	// TODO: Add your control notification handler code here
	UpdataModels();
}

void KSceneModelEditorSword2CheckTool::OnCbnSelchangeComboAniname2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//int Index = m_Combo_Ani1.GetCurSel();
	//if(Index<0)
	//	return;
	//DWORD AniIndex = m_Combo_Ani1.GetItemData(Index);

	//DWORD Method = PLAY_CIRCLE;

	//PlayAnimation(AniIndex,Method,1.0f, 1);
	//if (m_bSynAniChange && Index < m_Combo_Ani.GetCount())
	//{
	//	m_Combo_Ani.SetCurSel(Index);
	//	AniIndex = m_Combo_Ani.GetItemData(Index);
	//	PlayAnimation(AniIndex, Method, 1.0f);
	//}
}

void KSceneModelEditorSword2CheckTool::OnCbnSelchangeComboSocket2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	UpdataModels();
}

void KSceneModelEditorSword2CheckTool::OnCbnSelchangeComboMode2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(m_nMode1 == 1)
	{
		m_ListBox_Down1.EnableWindow(FALSE);
		m_ListBox_Head1.EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_ADDHEAD2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ADDHEADTAB2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_HEADDEL2)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_HEAD2)->EnableWindow(FALSE);

		GetDlgItem(IDC_STATIC_UP2)->SetWindowText("NPC模型");

		GetDlgItem(IDC_BUTTON_ADDDOWN2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ADDDOWNTAB2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWNDEL2)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_DOWN2)->EnableWindow(FALSE);
	}
	else
	{
		m_ListBox_Down1.EnableWindow(TRUE);
		m_ListBox_Head1.EnableWindow(TRUE);

		GetDlgItem(IDC_BUTTON_ADDHEAD2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADDHEADTAB2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_HEADDEL2)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_HEAD2)->EnableWindow(TRUE);

		GetDlgItem(IDC_STATIC_UP2)->SetWindowText("上身");

		GetDlgItem(IDC_BUTTON_ADDDOWN2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADDDOWNTAB2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DOWNDEL2)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_DOWN2)->EnableWindow(TRUE);
	}
}

void KSceneModelEditorSword2CheckTool::OnBnClickedButton1()
{
	/*int Index = m_Combo_Ani.GetCurSel();
	if(Index<0)
		return;
	DWORD AniIndex = m_Combo_Ani.GetItemData(Index);

	DWORD Method = PLAY_CIRCLE;

	PlayAnimation(AniIndex,Method,1.0f);
	
	if (m_Combo_Ani1.GetCount() > Index)
		AniIndex = m_Combo_Ani1.GetItemData(Index);
	PlayAnimation(AniIndex, Method, 1.0f, 1);*/
}

void KSceneModelEditorSword2CheckTool::OnLbnSelchangeListHead2()
{
	// TODO: Add your control notification handler code here
	UpdataModels();
}

