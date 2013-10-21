// KSceneEditorRoadSetup.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorRoadSetup.h"
#include "IEEditor.h"
#include "IEKG3DTerrain.h"

// KSceneEditorRoadSetup dialog

IMPLEMENT_DYNAMIC(KSceneEditorRoadSetup, CDialog)

KSceneEditorRoadSetup::KSceneEditorRoadSetup(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorRoadSetup::IDD, pParent)
	, m_fRoadWidth(1000)
	, m_bShowLine(false)
	, m_nModulusK(8)
	, m_nTexDensity(1000)
	, m_fViewDistance(30000)
	, m_fTransWidth(1000)
	, m_nEdge(25)
	, m_nNodeSize(10)
	, m_bShowNode(TRUE)
	, m_nSegmentLength(200)
{
	m_lpEditor = NULL;
	m_lpOutDoorSpaceMgr = NULL;
	m_nLastEdge = m_nEdge;
	m_nLastModulusK = m_nModulusK;
	m_lpRoadMgr = NULL;
}

KSceneEditorRoadSetup::~KSceneEditorRoadSetup()
{
}

void KSceneEditorRoadSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_EDIT1, m_fRoadWidth);
	DDX_Text(pDX, IDC_EDIT_TEX_DENSITY, m_nTexDensity);
	DDX_Text(pDX, IDC_EDIT_VIEW_DISTANCE, m_fViewDistance);
	DDX_Text(pDX, IDC_EDIT_TRANS_WIDTH, m_fTransWidth);
	DDX_Check(pDX,IDC_CHECK_SHOWLINE, m_bShowLine);
	DDX_Slider(pDX, IDC_SLIDER_K, m_nModulusK);
	DDX_Control(pDX, IDC_SLIDER_K, m_SliderK);
	DDX_Text(pDX, IDC_STATIC_K, m_CStringK);
	DDX_Control(pDX, IDC_EDIT_RENAME, m_EditReName);
	DDX_Text(pDX, IDC_EDIT_RENAME, m_strName);
	DDX_Slider(pDX, IDC_SLIDER_EDGE,m_nEdge);
	DDX_Text(pDX, IDC_STATIC_EDGEM, m_CStringEdge);
	DDX_Control(pDX, IDC_SLIDER_NODESIZE, m_SliderNode);
	DDX_Slider(pDX, IDC_SLIDER_NODESIZE,m_nNodeSize);	
	DDX_Text(pDX,IDC_STATIC_NODESIZE,m_CStringNodeSize);
	DDX_Check(pDX,IDC_CHECK_SHOWNODE, m_bShowNode);
	DDX_Text(pDX,IDC_STATIC_SL,m_CStringSL);
	DDX_Slider(pDX,IDC_SLIDER_SL,m_nSegmentLength);
	DDX_Control(pDX, IDC_SLIDER_SL, m_SliderSL);
	DDX_Text(pDX, IDC_EDIT_ID, m_nID);
}


BEGIN_MESSAGE_MAP(KSceneEditorRoadSetup, CDialog)
	ON_BN_CLICKED(IDC_NEWROAD, &KSceneEditorRoadSetup::OnBnClickedNewroad)
	ON_BN_CLICKED(IDC_NEWNODE, &KSceneEditorRoadSetup::OnBnClickedNewnode)
	ON_LBN_SELCHANGE(IDC_LIST1, &KSceneEditorRoadSetup::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_REFRESH, &KSceneEditorRoadSetup::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_FLATROAD, &KSceneEditorRoadSetup::OnBnClickedFlatroad)
	ON_BN_CLICKED(IDC_HEIGHT_TERRAINTOROAD, &KSceneEditorRoadSetup::OnBnClickedHeightTerraintoroad)
	ON_BN_CLICKED(IDC_DELETEROAD, &KSceneEditorRoadSetup::OnBnClickedDeleteroad)
	ON_BN_CLICKED(IDC_BUN_REFRESH, &KSceneEditorRoadSetup::OnBnClickedBunRefresh)
	ON_BN_CLICKED(IDC_CHECK_SHOWLINE, &KSceneEditorRoadSetup::OnBnClickedCheckShowline)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, &KSceneEditorRoadSetup::OnBnClickedButtonTexture)

	ON_EN_KILLFOCUS(IDC_EDIT_RENAME, &KSceneEditorRoadSetup::OnEnKillfocusEditRename)
	ON_BN_CLICKED(IDC_BUTTON_PRE, &KSceneEditorRoadSetup::OnBnClickedButtonPre)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &KSceneEditorRoadSetup::OnBnClickedButtonNext)
	ON_EN_KILLFOCUS(IDC_EDIT_TEX_DENSITY, &KSceneEditorRoadSetup::OnEnKillfocusEditTexDensity)
	ON_EN_KILLFOCUS(IDC_EDIT_VIEW_DISTANCE, &KSceneEditorRoadSetup::OnEnKillfocusEditViewDistance)
	ON_EN_KILLFOCUS(IDC_EDIT_TRANS_WIDTH, &KSceneEditorRoadSetup::OnEnKillfocusEditTransWidth)
	ON_EN_KILLFOCUS(IDC_EDIT1, &KSceneEditorRoadSetup::OnEnKillfocusEdit1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_EDGE, &KSceneEditorRoadSetup::OnNMCustomdrawSliderEdge)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_K, &KSceneEditorRoadSetup::OnNMCustomdrawSliderK)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NODESIZE, &KSceneEditorRoadSetup::OnNMCustomdrawSliderNodesize)
	ON_BN_CLICKED(IDC_CHECK_SHOWNODE, &KSceneEditorRoadSetup::OnBnClickedCheckShownode)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_K, &KSceneEditorRoadSetup::OnNMReleasedcaptureSliderK)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_EDGE, &KSceneEditorRoadSetup::OnNMReleasedcaptureSliderEdge)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SL, &KSceneEditorRoadSetup::OnNMReleasedcaptureSliderSl)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SL, &KSceneEditorRoadSetup::OnNMCustomdrawSliderSl)
	ON_EN_KILLFOCUS(IDC_EDIT_ID, &KSceneEditorRoadSetup::OnEnKillfocusEditId)
	ON_BN_CLICKED(IDC_REFRESH_LIST, &KSceneEditorRoadSetup::OnBnClickedRefreshList)
END_MESSAGE_MAP()


// KSceneEditorRoadSetup message handlers
void KSceneEditorRoadSetup::SetEditor(IEKG3DSceneSceneEditor* pEditor,IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr)
{
	m_lpEditor = pEditor;
	m_lpOutDoorSpaceMgr = pOutDoorMgr;
	if (m_lpOutDoorSpaceMgr)
		m_lpOutDoorSpaceMgr->GetTerrainRoadMgr(&m_lpRoadMgr);
}
void KSceneEditorRoadSetup::OnBnClickedNewroad()
{
	// TODO: Add your control notification handler code here
	if (!m_lpRoadMgr)
		return;
	m_lpRoadMgr->AddNewOne();
	FillList();
	int nIndex = m_List.GetCount()-1;
	m_List.SetCurSel(nIndex);
	OnLbnSelchangeList1();
}


void KSceneEditorRoadSetup::OnBnClickedNewnode()
{
	if (!m_lpRoadMgr)
		return;
	if (!m_lpEditor)
		return;

	m_lpEditor->AddNodeToScene(NODETYPE_ROAD); // #define NODETYPE_ROAD 1
	// TODO: Add your control notification handler code here
}

void KSceneEditorRoadSetup::FillList()
{
	if (!m_lpRoadMgr)
		return;
	{
		int nCount = m_List.GetCount();
		for (int i=0;i<nCount;i++)
		{
			m_List.DeleteString(0);
		}
	}
	DWORD dwCount = m_lpRoadMgr->GetCount();
	vector<std::string> vecName;
	m_lpRoadMgr->GetAllName(vecName);
	for (int i=0;i<(int)vecName.size();i++)
	{
		//TCHAR name[MAX_PATH];
		//wsprintf(name,"Road %d",i);
		m_List.AddString(vecName[i].c_str());
	}

}


void KSceneEditorRoadSetup::OnLbnSelchangeList1()
{
	if (!m_lpRoadMgr)
		return;
	//m_lpTerrain->SetCurrentRoadShowLine(FALSE);
	int nIndex = m_List.GetCurSel();
	m_lpRoadMgr->SetCurSerpentine(nIndex);

	//m_lpRoadMgr->GetCurrentRoadShowLine(m_bShowLine);
	m_fRoadWidth = m_lpRoadMgr->GetCurSerWidth();
	m_fLastRoadWidth = m_fRoadWidth;
	m_lpRoadMgr->GetCurrentRoadID(m_nID);
	std::string strname;
	m_lpRoadMgr->GetCurSerName(strname);
	m_strLastName = m_strName = strname.c_str();

	float fTexDensity;
	m_lpRoadMgr->GetCurrentRoadTexDensity(fTexDensity);
	m_nLastTexDensity = m_nTexDensity = (int)((1 / fTexDensity) + 0.5f);
	m_lpRoadMgr->GetCurrentRoadBlendLength(m_fTransWidth);
	m_fLastTransWidth = m_fTransWidth;
	float fEdge;
	m_lpRoadMgr->GetCurrentRoadEdgeModulus(fEdge);
	m_nLastEdge = m_nEdge = (int)((1 - fEdge) * 100);
	m_CStringEdge.Format("边缘过度系数：%d",m_nEdge);
	
	float fK;
	m_lpRoadMgr->GetCurSerModulusK(fK);
	m_nLastModulusK = m_nModulusK = (int)(fK * 10);
	m_CStringK.Format("弯曲系数: %d",m_nModulusK);
//	float fNodeSize;
	/*m_lpRoadMgr->GetCurrentRoadNodeSize(fNodeSize);
	m_nNodeSize = (int)fNodeSize;
	m_CStringNodeSize.Format("节点大小：%d",m_nNodeSize);*/
	
	int nSegmentLength;
	m_lpRoadMgr->GetCurSerSegmentLength(nSegmentLength);
	m_nLastSegmentLength = m_nSegmentLength = nSegmentLength/50; 
	m_CStringSL.Format("分段长度：%d",nSegmentLength);

	m_bShowLine = m_lpRoadMgr->GetCurSerDrawSelectMark();
	UpdateData(FALSE);
}

void KSceneEditorRoadSetup::OnBnClickedRefresh() // 更新当前道路对应的地表高度
{
	if (!m_lpRoadMgr)
		return;
	m_lpRoadMgr->RefreshHeight(TRUE);

}

void KSceneEditorRoadSetup::OnBnClickedFlatroad()
{
	// TODO: Add your control notification handler code here
	if (!m_lpRoadMgr)
		return;
	if (!m_lpEditor)
		return;
	m_lpEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	//m_lpRoadMgr->SetTerrainState(ROAD_FLAT);
}

void KSceneEditorRoadSetup::OnBnClickedHeightTerraintoroad()
{
	// TODO: Add your control notification handler code here
	if (!m_lpRoadMgr)
		return;
	m_lpRoadMgr->RefreshHeight(FALSE);
}

void KSceneEditorRoadSetup::OnBnClickedDeleteroad()
{
	if (!m_lpRoadMgr)
		return;
	if (!m_lpEditor)
		return;
	m_lpEditor->DeleteNodeFromScene(NODETYPE_ROAD);
	int nIndex = m_List.GetCurSel();
	m_lpRoadMgr->DeleteOne(nIndex);
	FillList();
	if(nIndex >= m_List.GetCount() && m_List.GetCount()>0)
		nIndex = m_List.GetCount()-1;
	if (m_List.GetCount()>0)
	{
		m_List.SetCurSel(nIndex);
		OnLbnSelchangeList1();
	}

}

void KSceneEditorRoadSetup::OnBnClickedBunRefresh()
{
	// TODO: Add your control notification handler code here
	if (!m_lpRoadMgr)
		return;
	m_lpRoadMgr->RefreshCurSerpentine();
}

void KSceneEditorRoadSetup::OnBnClickedCheckShowline()
{
	// TODO: Add your control notification handler code here
	if (!m_lpRoadMgr)
		return;
	UpdateData(TRUE);
	m_lpRoadMgr->SetCurSerDrawSelectMark(m_bShowLine);

}

void KSceneEditorRoadSetup::OnNMCustomdrawSliderK(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateData(TRUE);

	m_CStringK.Format("弯曲系数: %d",m_nModulusK);
	UpdateData(FALSE);
}

BOOL KSceneEditorRoadSetup::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_SliderK.SetRange(1,20);
	m_SliderNode.SetRange(0,20);
	m_SliderSL.SetRange(1,50);
	if(m_lpEditor)
	{
		m_lpEditor->Advise(this);
		m_bShowNode = m_lpEditor->GetRenderNode();
	}
	FillList();
	return TRUE;
}
void KSceneEditorRoadSetup::OnBnClickedButtonTexture()
{
	// TODO: Add your control notification handler code here
	if (!m_lpRoadMgr)
		return;

	HRESULT hr = E_FAIL;
	TCHAR strFilter[] = "Texture Files |*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS;";
	CString fileName;
	CFileDialog DlgLoad(TRUE, ".dds", "test.dds", OFN_HIDEREADONLY , strFilter);

	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);
	if(DlgLoad.DoModal() == IDOK)
	{
		fileName = DlgLoad.GetPathName();
		m_lpRoadMgr->ChangeCurrentRoadTexture(fileName);
	}
	::SetCurrentDirectory(szDir);

	
	//if (FAILED(hr))
	//{
	//	MessageBox(fileName, "Error");
	//}

}

void KSceneEditorRoadSetup::OnEnKillfocusEditRename()
{
	// TODO: Add your control notification handler code here
	//m_strSaveName = m_strName;
	UpdateData(TRUE);
	if (!m_lpRoadMgr)
		return;
	if(strcmpi(m_strName,m_strLastName) == 0)
		return ;
	if(m_lpRoadMgr->FindSerpentineByName(m_strName))		
	{
		char strText[64];
		sprintf(strText,"道路名称'%s'已存在！！",m_strName);
		m_strName = m_strLastName;
		UpdateData(FALSE);
		MessageBox(strText, _T("提示"), MB_OK);
		return ;
	}
	m_lpRoadMgr->SetCurSerName(m_strName);
	m_strLastName = m_strName;
	int i = m_List.GetCurSel();
	FillList();
	m_List.SetCurSel(i);
}

void KSceneEditorRoadSetup::OnBnClickedButtonPre()
{
	// TODO: Add your control notification handler code here
	if (!m_lpRoadMgr)
		return;
	int i = m_List.GetCurSel();
	m_lpRoadMgr->MoveCurSerpentinePre();
	FillList();
	if(--i < 0 )
		i = 0;
	m_List.SetCurSel(i);
}

void KSceneEditorRoadSetup::OnBnClickedButtonNext()
{
	// TODO: Add your control notification handler code here
	if (!m_lpRoadMgr)
		return;
	int i = m_List.GetCurSel();
	m_lpRoadMgr->MoveCurSerpentineNext();
	FillList();
	if(++i >= m_List.GetCount() )
		i = m_List.GetCount() - 1;
	m_List.SetCurSel(i);
}



void KSceneEditorRoadSetup::OnEnKillfocusEditTexDensity()
{
	UpdateData(TRUE);
	if (!m_lpRoadMgr)
		return;
	if(m_nLastTexDensity == m_nTexDensity)
		return;
	m_nLastTexDensity = m_nTexDensity;
	m_lpRoadMgr->SetCurrentRoadTexDensity(1.0f/m_nTexDensity);
	m_lpRoadMgr->RefreshCurSerpentine();
}

void KSceneEditorRoadSetup::OnEnKillfocusEditViewDistance()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (!m_lpRoadMgr)
		return;
	float fDis = m_fViewDistance * m_fViewDistance;
	//m_lpRoadMgr->SetRoadPartVisableDistance(fDis);
}

void KSceneEditorRoadSetup::OnEnKillfocusEditTransWidth()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (!m_lpRoadMgr)
		return;
	if(m_fLastTransWidth == m_fTransWidth)
		return;
	m_fLastTransWidth = m_fTransWidth;
	m_lpRoadMgr->SetCurrentRoadBlendLength(m_fTransWidth);
	m_lpRoadMgr->RefreshCurSerpentine();
}

void KSceneEditorRoadSetup::OnEnKillfocusEdit1()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_fRoadWidth > 5000)
	{
		m_fRoadWidth = 5000;
		UpdateData(FALSE);
	}
	else if (m_fRoadWidth < 500)
	{
		m_fRoadWidth = 200;
		UpdateData(FALSE);
	}
	if (!m_lpRoadMgr)
		return;
	if(m_fLastRoadWidth == m_fRoadWidth)
		return;
	m_fLastRoadWidth = m_fRoadWidth;
	m_lpRoadMgr->SetCurSerWidth(m_fRoadWidth);
	m_lpRoadMgr->RefreshCurSerpentine();
}



void KSceneEditorRoadSetup::OnNMCustomdrawSliderEdge(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateData(TRUE);
	m_CStringEdge.Format("边缘过度系数：%d",m_nEdge);
	UpdateData(FALSE);
}


void KSceneEditorRoadSetup::OnNMCustomdrawSliderNodesize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateData(TRUE);

	if (!m_lpRoadMgr)
		return;

	//m_lpRoadMgr->SetCurrentRoadNodeSize((float)m_nNodeSize);

	m_CStringNodeSize.Format("节点大小：%d",m_nNodeSize);
	UpdateData(FALSE);
}




void KSceneEditorRoadSetup::OnBnClickedCheckShownode()
{
	// TODO: Add your control notification handler code here
	if (!m_lpEditor)
		return;
	UpdateData(TRUE);
	m_lpEditor->SetRenderNode(m_bShowNode);
}


void KSceneEditorRoadSetup::SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
{
	if(nSinkID==EVENT_ROADNODE_SELECTED)
	{
		m_List.SetCurSel(dwValue);
		UpdateData(FALSE);
		OnLbnSelchangeList1();
	}
}
void KSceneEditorRoadSetup::OnNMReleasedcaptureSliderK(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	float K = m_nModulusK * 0.1f;
	if (!m_lpRoadMgr)
		return;
	if(m_nLastModulusK == m_nModulusK)
		return ;
	m_nLastModulusK = m_nModulusK;
	m_lpRoadMgr->SetCurSerModulusK(K);
	m_lpRoadMgr->RefreshCurSerpentine();	
}

void KSceneEditorRoadSetup::OnNMReleasedcaptureSliderEdge(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (!m_lpRoadMgr)
		return;
	if(m_nLastEdge == m_nEdge)
		return ;
	m_nLastEdge = m_nEdge;
	float fEdgeModulus =(100 - m_nEdge) * 0.01f;
	m_lpRoadMgr->SetCurrentRoadEdgeModulus(fEdgeModulus);
	m_lpRoadMgr->RefreshCurSerpentine();
}

void KSceneEditorRoadSetup::OnNMReleasedcaptureSliderSl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (!m_lpRoadMgr)
		return;
	if(m_nLastSegmentLength == m_nSegmentLength)
		return ;
	m_nLastSegmentLength = m_nSegmentLength;
	int nSegmentLength = m_nSegmentLength * 50;
	m_lpRoadMgr->SetCurSerSegmentLength(nSegmentLength);
	m_lpRoadMgr->RefreshCurSerpentine();
}

void KSceneEditorRoadSetup::OnNMCustomdrawSliderSl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateData(TRUE);
	
	m_CStringSL.Format("分段长度：%d cm",m_nSegmentLength* 50);
	UpdateData(FALSE);
}

void KSceneEditorRoadSetup::OnEnKillfocusEditId()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_lpRoadMgr->SetCurrentRoadID(m_nID);
}

void KSceneEditorRoadSetup::OnBnClickedRefreshList()
{
	FillList();
}
