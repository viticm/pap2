// KSceneEditorRiver.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorRiver.h"
#include "IEEditor.h"
#include "IEKG3DTerrain.h"

// KSceneEditorRiver dialog
#define IS_EQUAL(ValA,ValB) \
	if (ValA == ValB)   \
		return;
IMPLEMENT_DYNAMIC(KSceneEditorRiver, CDialog)

KSceneEditorRiver::KSceneEditorRiver(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorRiver::IDD, pParent)
{
	m_lpEditor = NULL;
	m_lpOutDoorSpaceMgr = NULL;
	m_lpRiverMgr = NULL;
	m_strName = "";
	m_strLastName ="";
	m_nRiverWidth = 0;
	m_nLastRiverWidth = 0;
	m_nSegmentLength = 0; 
	m_nLastSegmentLength = 0;
	m_nModulusK = 0;
	m_nLastModulusK = 0;
	m_nRiverDepth= 0 ;
	m_nLastRiverDepth= 0 ;
	m_bShowSelectMark = TRUE;
	m_bShowLineEx = FALSE;
}

KSceneEditorRiver::~KSceneEditorRiver()
{
}

void KSceneEditorRiver::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_EDIT_RENAME, m_strName);
	DDX_Slider(pDX,IDC_SLIDER_SL,m_nSegmentLength);
	DDX_Control(pDX, IDC_SLIDER_SL, m_SliderSL);
	DDX_Text(pDX,IDC_STATIC_SL,m_CStringSL);
	DDX_Slider(pDX, IDC_SLIDER_K, m_nModulusK);
	DDX_Control(pDX, IDC_SLIDER_K, m_SliderK);
	DDX_Text(pDX, IDC_STATIC_K, m_CStringK);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_nRiverWidth);
	DDX_Text(pDX, IDC_EDIT_DEPTH, m_nRiverDepth);
	DDX_Check(pDX,IDC_CHECK_SHOWLINE,m_bShowSelectMark);
	DDX_Check(pDX,IDC_CHECK_SHOWLINEEX,m_bShowLineEx);
}


BEGIN_MESSAGE_MAP(KSceneEditorRiver, CDialog)
	ON_BN_CLICKED(IDC_NEWRIVER, &KSceneEditorRiver::OnBnClickedNewriver)
	ON_LBN_SELCHANGE(IDC_LIST1, &KSceneEditorRiver::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_ADD_NODE, &KSceneEditorRiver::OnBnClickedAddNode)
	ON_BN_CLICKED(IDC_MAKE_RIVER, &KSceneEditorRiver::OnBnClickedMakeRiver)
	ON_BN_CLICKED(IDC_DELRIVER, &KSceneEditorRiver::OnBnClickedDelriver)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_K, &KSceneEditorRiver::OnNMCustomdrawSliderK)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SL, &KSceneEditorRiver::OnNMCustomdrawSliderSl)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_K, &KSceneEditorRiver::OnNMReleasedcaptureSliderK)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SL, &KSceneEditorRiver::OnNMReleasedcaptureSliderSl)
	ON_EN_KILLFOCUS(IDC_EDIT_DEPTH, &KSceneEditorRiver::OnEnKillfocusEditDepth)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, &KSceneEditorRiver::OnEnKillfocusEditWidth)
	ON_EN_KILLFOCUS(IDC_EDIT_RENAME, &KSceneEditorRiver::OnEnKillfocusEditRename)
	ON_BN_CLICKED(IDC_CHECK_SHOWLINE, &KSceneEditorRiver::OnBnClickedCheckShowline)
	ON_BN_CLICKED(IDC_UPDATE_TERRAINHEIGHT, &KSceneEditorRiver::OnBnClickedUpdateTerrainheight)
	ON_BN_CLICKED(IDC_CHECK_SHOWLINEEX, &KSceneEditorRiver::OnBnClickedCheckShowlineex)
	ON_BN_CLICKED(IDC_COMPUTE_ALPHA, &KSceneEditorRiver::OnBnClickedComputeAlpha)
	ON_BN_CLICKED(IDC_REFRESH_LIST, &KSceneEditorRiver::OnBnClickedRefreshList)
	ON_BN_CLICKED(IDC_REFRESH_SHADER, &KSceneEditorRiver::OnBnClickedRefreshShader)
END_MESSAGE_MAP()


// KSceneEditorRiver message handlers
void KSceneEditorRiver::SetEditor(IEKG3DSceneSceneEditor* pEditor,IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr)
{
	m_lpEditor = pEditor;
	m_lpOutDoorSpaceMgr = pOutDoorMgr;
	if (m_lpOutDoorSpaceMgr)
		m_lpOutDoorSpaceMgr->GetTerrainRiverMgr(&m_lpRiverMgr);
}
BOOL KSceneEditorRiver::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_SliderK.SetRange(1,20);
	m_SliderSL.SetRange(1,50);
	if(m_lpEditor)
	{
		m_lpEditor->Advise(this);
	}
	FillList();
	return TRUE;
}
void KSceneEditorRiver::OnBnClickedNewriver()
{
	// TODO: Add your control notification handler code here
	if (!m_lpRiverMgr)
		return;
	m_lpRiverMgr->AddNewOne();
	FillList();
	int nIndex = m_List.GetCount()-1;
	m_List.SetCurSel(nIndex);
	OnLbnSelchangeList1();
}
void KSceneEditorRiver::FillList()
{
	if (!m_lpRiverMgr)
		return;
	{
		int nCount = m_List.GetCount();
		for (int i=0;i<nCount;i++)
		{
			m_List.DeleteString(0);
		}
	}
	DWORD dwCount = m_lpRiverMgr->GetCount();
	vector<std::string> vecName;
	m_lpRiverMgr->GetAllName(vecName);
	for (int i=0;i<(int)vecName.size();i++)
	{
		//TCHAR name[MAX_PATH];
		//wsprintf(name,"Road %d",i);
		m_List.AddString(vecName[i].c_str());
	}

}

void KSceneEditorRiver::OnLbnSelchangeList1()
{
	if (!m_lpRiverMgr)
		return;
	//m_lpTerrain->SetCurrentRoadShowLine(FALSE);
	int nIndex = m_List.GetCurSel();
	m_lpRiverMgr->SetCurSerpentine(nIndex);

	m_nLastRiverWidth = m_nRiverWidth =(int) m_lpRiverMgr->GetCurSerWidth();
	m_nLastRiverDepth =m_nRiverDepth= (int)m_lpRiverMgr->GetCurrentRiverDepth();
	std::string strname;
	m_lpRiverMgr->GetCurSerName(strname);
	m_strLastName = m_strName = strname.c_str();

	float fK;
	m_lpRiverMgr->GetCurSerModulusK(fK);
	m_nLastModulusK = m_nModulusK = (int)(fK * 10);
	m_CStringK.Format("弯曲系数: %d",m_nModulusK);
	//float fNodeSize;
	/*m_lpRoadMgr->GetCurrentRoadNodeSize(fNodeSize);
	m_nNodeSize = (int)fNodeSize;
	m_CStringNodeSize.Format("节点大小：%d",m_nNodeSize);*/

	int nSegmentLength;
	m_lpRiverMgr->GetCurSerSegmentLength(nSegmentLength);
	m_nLastSegmentLength = m_nSegmentLength = nSegmentLength/50; 
	m_CStringSL.Format("分段长度：%d dm",nSegmentLength);

	m_bShowSelectMark = m_lpRiverMgr->GetCurSerDrawSelectMark();
	m_bShowLineEx = m_lpRiverMgr->GetCurSerDrawExtendLine();
	UpdateData(FALSE);
}




void KSceneEditorRiver::OnBnClickedAddNode()
{
	// TODO: Add your control notification handler code here
	if (!m_lpEditor)
		return;

	m_lpEditor->AddNodeToScene(NODETYPE_RIVER);//#define NODETYPE_RIVER 2
}

void KSceneEditorRiver::OnBnClickedMakeRiver()
{
	if(m_lpRiverMgr)
		m_lpRiverMgr->RefreshCurSerpentine();
}

void KSceneEditorRiver::OnBnClickedDelriver()
{
	if(!m_lpEditor)
		return;

	m_lpEditor->DeleteNodeFromScene(NODETYPE_RIVER);
	int nIndex = m_List.GetCurSel();
	m_lpRiverMgr->DeleteOne(nIndex);
	FillList();
	if(nIndex >= m_List.GetCount() && m_List.GetCount()>0)
		nIndex = m_List.GetCount()-1;
	if (m_List.GetCount()>0)
	{
		m_List.SetCurSel(nIndex);
		OnLbnSelchangeList1();
	}
}

void KSceneEditorRiver::OnNMCustomdrawSliderK(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateData(TRUE);
	m_CStringK.Format("弯曲系数: %d",m_nModulusK);
	UpdateData(FALSE);
}

void KSceneEditorRiver::OnNMCustomdrawSliderSl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateData(TRUE);
	m_CStringSL.Format("分段长度：%d cm",m_nSegmentLength* 50);
	UpdateData(FALSE);
}

void KSceneEditorRiver::OnNMReleasedcaptureSliderK(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	
	if (!m_lpRiverMgr)
		return;
	IS_EQUAL(m_nLastModulusK,m_nModulusK);
	m_nLastModulusK = m_nModulusK;
	float K = m_nModulusK * 0.1f;
	m_lpRiverMgr->SetCurSerModulusK(K);
	//m_lpRiverMgr->RefreshCurrentRoad();	
}

void KSceneEditorRiver::OnNMReleasedcaptureSliderSl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (!m_lpRiverMgr)
		return;
	if (m_nLastSegmentLength == m_nSegmentLength)
		return ;
	m_nLastSegmentLength = m_nSegmentLength;
	int nSegmentLength = m_nSegmentLength * 50;
	m_lpRiverMgr->SetCurSerSegmentLength(nSegmentLength);
	//m_lpRiverMgr->RefreshCurrentRoad();
}
void KSceneEditorRiver::SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
{
	if(nSinkID==EVENT_RIVERNODE_SELECTED)
	{
		m_List.SetCurSel(dwValue);
		UpdateData(FALSE);
		OnLbnSelchangeList1();
	}
}
void KSceneEditorRiver::OnEnKillfocusEditDepth()
{
	if(!m_lpRiverMgr)
		return;
	UpdateData(TRUE);
	IS_EQUAL(m_nLastRiverDepth,m_nRiverDepth);
	m_nLastRiverDepth = m_nRiverDepth;
	m_lpRiverMgr->SetCurrentRiverDepth((float)m_nRiverDepth);
	
}

void KSceneEditorRiver::OnEnKillfocusEditWidth()
{
	if(!m_lpRiverMgr)
		return;
	UpdateData(TRUE);
	if(m_nRiverWidth > 50000)
	{
		m_nRiverWidth = 50000;
		UpdateData(FALSE);
	}
	IS_EQUAL(m_nLastRiverWidth,m_nRiverWidth);
	m_nLastRiverWidth = m_nRiverWidth;
	m_lpRiverMgr->SetCurSerWidth((float)m_nRiverWidth);
}

void KSceneEditorRiver::OnEnKillfocusEditRename()
{
	if (!m_lpRiverMgr)
		return;
	UpdateData(TRUE);
	if(strcmpi(m_strName,m_strLastName) == 0)
		return ;
	if(m_lpRiverMgr->FindSerpentineByName(m_strName))		
	{
		char strText[64];
		sprintf(strText,"河流名称'%s'已存在！！",m_strName);
		m_strName = m_strLastName;
		UpdateData(FALSE);
		MessageBox(strText, _T("提示"), MB_OK);
		return ;
	}
	m_strLastName = m_strName;
	m_lpRiverMgr->SetCurSerName(m_strName);
	int i = m_List.GetCurSel();
	FillList();
	m_List.SetCurSel(i);
}

void KSceneEditorRiver::OnBnClickedCheckShowline()
{
	if(!m_lpRiverMgr)
		return;
	UpdateData(TRUE);
	m_lpRiverMgr->SetCurSerDrawSelectMark(m_bShowSelectMark);
}

void KSceneEditorRiver::OnBnClickedUpdateTerrainheight()
{
	if(m_lpRiverMgr)
		m_lpRiverMgr->RefreshHeight(TRUE);
}

void KSceneEditorRiver::OnBnClickedCheckShowlineex()
{
	if(!m_lpRiverMgr)
		return;
	UpdateData(TRUE);
	m_lpRiverMgr->SetCurSerDrawExtendLine(m_bShowLineEx);
}

void KSceneEditorRiver::OnBnClickedComputeAlpha()
{
	if(!m_lpRiverMgr)
		return;
	m_lpRiverMgr->ComputeCurrentRiverAlpha();
}

void KSceneEditorRiver::OnBnClickedRefreshList()
{
	FillList();
}

void KSceneEditorRiver::OnBnClickedRefreshShader()
{
	if (m_lpEditor)
		m_lpEditor->RefreshWaterEffect();
}
