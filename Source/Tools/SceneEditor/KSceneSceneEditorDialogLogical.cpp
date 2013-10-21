// KSceneSceneEditorDialogLogical.cpp : implementation file
//

#include "stdafx.h"
#include "comutil.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogLogical.h"
#include ".\kscenesceneeditordialoglogical.h"
#include "KLogicInfoRenameDlg.h"
#include "KSceneEditorDocLogical.h"
//#include "KG3DSceneSceneEditor.h"
#include "IEEditor.h"
#include "SceneEditorDoc.h"
#include "KLogicObstacleGenerator.h"
#include "IEKG3DTerrain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TAB1	0
#define TAB2	1

#define COLUMN_WIDTH1	90
#define COLUMN_WIDTH2	60	
#define COLUMN_WIDTH3	60
////////////////////////////////////////////////////////////////////////////////



// KSceneSceneEditorDialogLogical dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogLogical, CDialog)
KSceneSceneEditorDialogLogical::KSceneSceneEditorDialogLogical(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogLogical::IDD, pParent), m_bObjInit(FALSE)
{
#ifndef _WIN32_WCE
	//EnableActiveAccessibility();
#endif
	m_lpSceneEditor = NULL;
	m_lpDoc = NULL;
	m_bInit = FALSE;
	m_bShowOKCANCEL = FALSE;
	m_bRectPen = TRUE;
	m_eBrushShape = cbRectangle;
	m_nBrushWidth = 5;
	m_nShowWidth = 1;
	m_nCellHeight = 1;
}

KSceneSceneEditorDialogLogical::~KSceneSceneEditorDialogLogical()
{
	//m_formobject.DestroyWindow();
	DestroyWindow();
}

void KSceneSceneEditorDialogLogical::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	/*if (m_bObjInit)
		DDX_Control(pDX, IDC_S3LOGICFORM, m_formobject);*/
	DDX_Control(pDX, IDC_TAB, m_TabCtrl);
	DDX_Control(pDX, IDC_CHECK_MASKREST, m_Check);
	DDX_Control(pDX, IDC_LIST_LOGICDATA, m_ListLogicData);
}


void KSceneSceneEditorDialogLogical::SetSceneEditorDoc(KSceneEditorDoc* pDoc)
{
	m_lpDoc = pDoc;
	m_lpDoc->m_lpLogicalform = this;
}

BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogLogical, CDialog)
	ON_WM_SIZE()
//	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_CHECK_OBSTACLE, &KSceneSceneEditorDialogLogical::OnBnClickedLogicInfo)
	ON_BN_CLICKED(IDC_CHECK_CANPUTDOODAD, &KSceneSceneEditorDialogLogical::OnBnClickedLogicInfo)
	ON_BN_CLICKED(IDC_CHECK_STALLAGE, &KSceneSceneEditorDialogLogical::OnBnClickedLogicInfo)
	ON_BN_CLICKED(IDC_CHECK_INDOORREST, &KSceneSceneEditorDialogLogical::OnBnClickedLogicInfo)
	ON_BN_CLICKED(IDC_CHECK_REST, &KSceneSceneEditorDialogLogical::OnBnClickedLogicInfo)
	ON_BN_CLICKED(IDC_CHECK_DISMOUNT, &KSceneSceneEditorDialogLogical::OnBnClickedLogicInfo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PENSIZE, &KSceneSceneEditorDialogLogical::OnCustomDrawSliderPenSize)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PENSIZE, &KSceneSceneEditorDialogLogical::OnSpinClickedPenSize)
	ON_EN_CHANGE(IDC_EDIT_PENSIZE, &KSceneSceneEditorDialogLogical::OnEnChangePenSize)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_AREA, &KSceneSceneEditorDialogLogical::OnCustomDrawSliderArea)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_AREA, &KSceneSceneEditorDialogLogical::OnSpinClickedArea)
	ON_EN_CHANGE(IDC_EDIT_AREA, &KSceneSceneEditorDialogLogical::OnEnChangeArea)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CELLHEIGHT, &KSceneSceneEditorDialogLogical::OnCustomDrawSliderCellHeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CELLHEIGHT, &KSceneSceneEditorDialogLogical::OnSpinClickedCellHeight)
	ON_EN_CHANGE(IDC_EDIT_CELLHEIGHT, &KSceneSceneEditorDialogLogical::OnEnChangeCellHeight)

	//ON_WM_SETFOCUS(IDC_EDIT_COLOR, &KSceneSceneEditorDialogLogical::OnBnClickedEditColor)
	ON_BN_CLICKED(IDC_CHECK, &KSceneSceneEditorDialogLogical::OnBnClickedCheck)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LOGICDATA, &KSceneSceneEditorDialogLogical::OnTcnSelchangeList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LOGICDATA, &KSceneSceneEditorDialogLogical::OnListItemRename)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &KSceneSceneEditorDialogLogical::OnTcnSelchangeTab)
	ON_BN_CLICKED(IDC_BN_ADD, &KSceneSceneEditorDialogLogical::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BN_DEL, &KSceneSceneEditorDialogLogical::OnBnClickedDel)
	ON_BN_CLICKED(IDC_BN_OK, &KSceneSceneEditorDialogLogical::OnBnClickedOK)
	ON_BN_CLICKED(IDC_BN_CANCEL,  &KSceneSceneEditorDialogLogical::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BN_BROWSESCRIPT, &KSceneSceneEditorDialogLogical::OnBnClickedBrowseScript)
	ON_BN_CLICKED(IDC_BN_DELETESCRIPT, &KSceneSceneEditorDialogLogical::OnBnClickedDeleteScript)
	ON_BN_CLICKED(IDC_CHECK_SHOWLOGICALOBSTACLE, &KSceneSceneEditorDialogLogical::OnModifyDisplaySpeLogicInfo)
	ON_BN_CLICKED(IDC_CHECK_SHOWSURFACEOBSTACLE, &KSceneSceneEditorDialogLogical::OnModifyDisplaySpeLogicInfo)
	ON_BN_CLICKED(IDC_CHECK_SHOWDOODADOBSTACLE, &KSceneSceneEditorDialogLogical::OnModifyDisplaySpeLogicInfo)
	ON_BN_CLICKED(IDC_CHECK_SHOWSELECTDOODADOBSTACLE, &KSceneSceneEditorDialogLogical::OnModifyDisplaySpeLogicInfo)
	ON_BN_CLICKED(IDC_CHECK_MASKREST, &KSceneSceneEditorDialogLogical::OnModifyDisplaySpeLogicInfo)
	ON_BN_CLICKED(IDC_RADIO_RECTPEN, &KSceneSceneEditorDialogLogical::OnModifyBrushInfo)
	ON_BN_CLICKED(IDC_RADIO_ROUNDPEN, &KSceneSceneEditorDialogLogical::OnModifyBrushInfo)
    ON_BN_CLICKED(IDC_EXPORT_MESH_DATA, &KSceneSceneEditorDialogLogical::OnBnClickedExportMeshData)
END_MESSAGE_MAP()

void KSceneSceneEditorDialogLogical::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rt;
	GetDlgItem(IDC_EDIT_COLOR)->GetWindowRect(&rt);
	ScreenToClient(&rt);

	if (point.x > rt.left && point.x < rt.right && point.y > rt.top && point.y < rt.bottom)
	{
		CColorDialog dlg;
		static bool bShowDlg = true;
		if (bShowDlg)
		{
			bShowDlg = false;
			if (dlg.DoModal() == IDOK)
			{
				m_color= dlg.GetColor();
				RedrawColorWindow();
				OnBnClickedLogicInfo();
			}
			bShowDlg = true;
		}
	}
}

void KSceneSceneEditorDialogLogical::OnListItemRename(NMHDR *pNMHDR, LRESULT *pResult)
{
	KLogicInfoRenameDlg dlg;
	int nIndex = m_ListLogicData.GetSelectionMark();
	if (nIndex == -1)
		return;

	dlg.m_strEditText = m_ListLogicData.GetItemText(nIndex, 0);

	LogicDataItem* pItem = (LogicDataItem*)m_ListLogicData.GetItemData(nIndex);
	if (dlg.DoModal() == IDOK)
	{
		if (pItem)
		{
			pItem->SetName(dlg.m_strEditText.GetBuffer());
		}
		m_ListLogicData.SetItemText(nIndex, 0, dlg.m_strEditText.GetBuffer());
	}
}

void KSceneSceneEditorDialogLogical::OnDestroy()
{
	if (m_ListLogicData)
	{
		while(m_ListLogicData.GetItemCount())
		{
			LogicDataItem* pItem = (LogicDataItem*)m_ListLogicData.GetItemData(0);
			SAFE_DELETE(pItem);
			m_ListLogicData.DeleteItem(0);
		}
	}
	CDialog::OnClose();
}

void KSceneSceneEditorDialogLogical::OnModifyBrushInfo()
{
	BOOL bChecked = ((CButton*)GetDlgItem(IDC_RADIO_RECTPEN))->GetCheck();
	if (bChecked)
	{
		m_eBrushShape = cbRectangle;
	}
	else
	{
		m_eBrushShape = cbCircle;
	}
	OnEnChangePenSize();
}

void KSceneSceneEditorDialogLogical::OnBnClickedLogicInfo()
{
	if (m_ListLogicData.GetSelectionMark() != -1 &&
		m_bShowOKCANCEL == FALSE)
	{
		m_bShowOKCANCEL = TRUE;
		GetDlgItem(IDC_BN_OK)->ShowWindow(TRUE);
		GetDlgItem(IDC_BN_CANCEL)->ShowWindow(TRUE);
	}
}

void KSceneSceneEditorDialogLogical::OnBnClickedDeleteScript()
{
	GetDlgItem(IDC_EDIT_SCRIPT)->SetWindowText("");
	m_szFileName[0] = '\0';
	m_szFilePath[0] = '\0';
	OnBnClickedLogicInfo();
}

void KSceneSceneEditorDialogLogical::OnBnClickedBrowseScript()
{
	CFileDialog dlg(true, "Lua", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D脚本文件(*.Lua)|*.Lua||");
	if (dlg.DoModal() == IDOK)
	{
		strcpy(m_szFileName, dlg.GetFileName());
		strcpy(m_szFilePath, dlg.GetPathName());

		GetDlgItem(IDC_EDIT_SCRIPT)->SetWindowText(m_szFileName);

		string strFilePath = m_szFilePath;
		int nIndex = strFilePath.find("scripts\\");
		if (nIndex == -1)
		{
			nIndex = 0;
		}
		strcpy(m_szFilePath, strFilePath.c_str() + nIndex);
		OnBnClickedLogicInfo();
	}
}

//void KSceneSceneEditorDialogLogical::OnCancel()
//{
//	//DestroyWindow();
//}
//
////void KSceneSceneEditorDialogLogical::OnPaint()
////{
////	//CDialog::OnPaint();
////
////}
//
//void KSceneSceneEditorDialogLogical::OnOK()
//{
//
//}

void KSceneSceneEditorDialogLogical::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_bInit)
	{
		CRect r;
		GetClientRect(&r);

		static const short nSep = 2;
		r.left += nSep;
		r.right -= nSep;
		r.top += nSep;
		r.bottom -= nSep;

		SetChildWindowPos(TAB1, r);
	}
}

//(by dengzhihui 2006年8月2日 17:00:48
//BOOL RegisterDLL()
//{
//	TCHAR szBuf[MAX_PATH + 1];
//	GetModuleFileName(NULL, szBuf, MAX_PATH);
//	CString s(szBuf);
//	int nFind = s.ReverseFind('\\');
//	if (nFind < 0)
//		return FALSE;
//	s = s.Mid(0, nFind);
//	s += _T("\\S3AxFormProj.ocx");
//
//	LPCTSTR pszDllName= s;
//	HINSTANCE hLib = LoadLibrary(pszDllName);
//	if (hLib == NULL || hLib == (HINSTANCE)HINSTANCE_ERROR)
//	{
//		return FALSE;
//	}
//	FARPROC lpDllEntryPoint;
//	lpDllEntryPoint = GetProcAddress
//		(hLib,_T("DllRegisterServer"));
//	//调用注册函数DllRegisterServer
//	if(lpDllEntryPoint!=NULL)
//	{
//		if(FAILED((*lpDllEntryPoint)()))
//		{
//			FreeLibrary(hLib);
//			return FALSE;
//		};
//		return TRUE;
//	}
//	else
//		return FALSE;
//}
static DWORD dwBitInfo[][2] = {
	{MODIFY_BLOCKCHARACTER, 1}, //障碍
	{MODIFY_PUTOBJECT, 2},		//可摆放物件
	{MODIFY_STALL, 4},			//可摆摊
	{MODIFY_INDOOR, 8},			//室内区域
	{MODIFY_REST, 16},			//休息区域
	{MODIFY_RIDEHORSE, 32},		//下马区域
	{MODIFY_SCRIPTFILE, 64},	//索引文件ID
	{MODIFY_NONE, 0},			//结束
};

//将控件传过来的逻辑信息格式转为engine中用的格式(modifyflag)
//凡是传到引擎engine中的 logicinfo 都需要先转换一下
DWORD TransToEngineFormat(long lLogicInfo)
{
	DWORD dwRet = 0;
	for(int i = 0; dwBitInfo[i][0] != MODIFY_NONE; i++)
	{
		dwRet |= lLogicInfo & dwBitInfo[i][1] ? dwBitInfo[i][0] : 0;
	}
	return dwRet;
}

void KSceneSceneEditorDialogLogical::OnCustomDrawSliderPenSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nPos = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_PENSIZE))->GetPos();
	char szPos[20];
	itoa(nPos, szPos, 10);
	GetDlgItem(IDC_EDIT_PENSIZE)->SetWindowText(szPos);
}

void KSceneSceneEditorDialogLogical::OnSpinClickedPenSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nPos = ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_PENSIZE))->GetPos();
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_PENSIZE))->SetPos(nPos);
}

void KSceneSceneEditorDialogLogical::OnEnChangePenSize()
{
	CString strText;
	int nPenSize = 0;
	CWnd* pWnd = GetDlgItem(IDC_EDIT_PENSIZE);
	if (!pWnd || pWnd->GetWindowTextLength() > 9)
		return;
	pWnd->GetWindowText(strText);
	nPenSize = atoi(strText.GetBuffer());
	if(m_lpSceneEditor)
		m_lpSceneEditor->SetBrushInfo(nPenSize, (eCellBrushShape)m_eBrushShape);
	m_nBrushWidth = nPenSize;
}

void KSceneSceneEditorDialogLogical::OnCustomDrawSliderArea(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nPos = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AREA))->GetPos();
	char szPos[20];
	itoa(nPos, szPos, 10);
	GetDlgItem(IDC_EDIT_AREA)->SetWindowText(szPos);
}

void KSceneSceneEditorDialogLogical::OnSpinClickedArea(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nPos = ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_AREA))->GetPos();
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AREA))->SetPos(nPos);
}

void KSceneSceneEditorDialogLogical::OnEnChangeArea()
{
	CString strText;
	int nAreaSize = 0;
	CWnd* pWnd = GetDlgItem(IDC_EDIT_AREA);
	if (pWnd->GetWindowTextLength() > 9)
		return;
	pWnd->GetWindowText(strText);
	nAreaSize = atoi(strText.GetBuffer());
	pWnd = GetDlgItem(IDC_SPIN_CELLHEIGHT);
	if (pWnd)
	{
		int nCellHeight = ((CSpinButtonCtrl*)pWnd)->GetPos();
		if(m_lpSceneEditor)
			m_lpSceneEditor->SetDisplayParam(nAreaSize, nCellHeight);
		m_nShowWidth = nAreaSize;
	}
}

void KSceneSceneEditorDialogLogical::OnCustomDrawSliderCellHeight(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nPos = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CELLHEIGHT))->GetPos();
	char szPos[20];
	itoa(nPos, szPos, 10);
	GetDlgItem(IDC_EDIT_CELLHEIGHT)->SetWindowText(szPos);
}

void KSceneSceneEditorDialogLogical::OnSpinClickedCellHeight(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nPos = ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_CELLHEIGHT))->GetPos();
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CELLHEIGHT))->SetPos(nPos);
}

void KSceneSceneEditorDialogLogical::OnEnChangeCellHeight()
{
	CString strText;
	int nCellHeight = 0;
	CWnd* pWnd = GetDlgItem(IDC_EDIT_CELLHEIGHT);
	if (pWnd->GetWindowTextLength() > 9)
		return;
	pWnd->GetWindowText(strText);
	nCellHeight = atoi(strText.GetBuffer());
	pWnd = GetDlgItem(IDC_SPIN_AREA);
	if (pWnd)
	{
		int nAreaSize = ((CSpinButtonCtrl*)pWnd)->GetPos();
		if(m_lpSceneEditor)
			m_lpSceneEditor->SetDisplayParam(nAreaSize, nCellHeight);
		m_nCellHeight = nCellHeight;
	}
}

void KSceneSceneEditorDialogLogical::SetEngineModifyStateByLogicInfo(BOOL bSelected, long lLogicInfo, LPCTSTR lpstrScriptFileName)
{
	if(!m_lpSceneEditor)
		return;

	if (bSelected)
	{
		m_lpSceneEditor->SetCurrentScriptFileName(lpstrScriptFileName);
		for(int i = 0; dwBitInfo[i][0] != MODIFY_NONE; i++)
		{
			m_lpSceneEditor->SetModifyState(eCellModifyState(dwBitInfo[i][0]),
				lLogicInfo & dwBitInfo[i][1]);
			m_lpSceneEditor->SetShowState(eCellModifyState(dwBitInfo[i][0]),
				lLogicInfo & dwBitInfo[i][1]);
		}
	}
	else
	{
		m_lpSceneEditor->SetCurrentScriptFileName(NULL);
		m_lpSceneEditor->SetModifyState(DWORD_FORCE, FALSE);
		m_lpSceneEditor->SetShowState(DWORD_FORCE, FALSE);
	}
	m_lpSceneEditor->RecalcShowCell(); //刷新当前显示的格子集合
}


void KSceneSceneEditorDialogLogical::SetSceneEditor(IEKG3DSceneSceneEditor* pEditor)
{
	pEditor->GetIELogicScene(&m_lpSceneEditor);
}
void KSceneSceneEditorDialogLogical::SetDocument(CDocument* pDoc, BOOL bSave)
{
	put_MapFileName(pDoc->GetPathName());
	if (bSave)
		Save();
	else
		Load();
}

void KSceneSceneEditorDialogLogical::OnModifyDisplaySpeLogicInfo()
{
	if(!m_lpSceneEditor) return;
	BOOL bChecked;
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_SHOWLOGICALOBSTACLE))->GetCheck();
	m_lpSceneEditor->SetShowLogicalCell(bChecked);

	SetLogicCheck();

	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_SHOWSURFACEOBSTACLE))->GetCheck();
	m_lpSceneEditor->SetShowLogicalTerrenCell(bChecked);

	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_SHOWDOODADOBSTACLE))->GetCheck();
	m_lpSceneEditor->SetShowLogicalItemCell(bChecked);

	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_SHOWSELECTDOODADOBSTACLE))->GetCheck();
	m_lpSceneEditor->SetShowLogicalSelectedCell(bChecked);

	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_MASKREST))->GetCheck();
	m_lpSceneEditor->SetShowRest(!bChecked);
}

void KSceneSceneEditorDialogLogical::OnBnClickedClearalltrap()
{
	if(m_lpDoc)
	{
		m_lpDoc->ClearAllTrap();
	}
}

void KSceneSceneEditorDialogLogical::OnBnClickedCheck()
{
	
}

BOOL KSceneSceneEditorDialogLogical::OnInitDialog()
{
	BOOL bResult = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(bResult);
	//TCITEM
	//m_TabCtrl.Create(WS_VISIBLE, rc, this, IDC_TAB);
	m_color = RGB(255, 255, 255);
	//m_brush.CreateSolidBrush(m_color);
	m_TabCtrl.InsertItem(0, "逻辑信息");
	m_TabCtrl.InsertItem(1, "高级设置");
	
	m_ListLogicData.InsertColumn(0, "名称", LVCFMT_CENTER, COLUMN_WIDTH1);
	m_ListLogicData.InsertColumn(1, "颜色", LVCFMT_CENTER, COLUMN_WIDTH2);
	m_ListLogicData.InsertColumn(2, "只读", LVCFMT_CENTER, COLUMN_WIDTH3);

	//笔刷大小
	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PENSIZE);
	pSliderCtrl->SetRange(1, 100);
	pSliderCtrl->SetPos(m_nBrushWidth);
	CSpinButtonCtrl* pSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_PENSIZE);
	pSpinCtrl->SetBase(10);
	pSpinCtrl->SetRange(1, 100);
	pSpinCtrl->SetPos(m_nBrushWidth);
	
	//显示区域大小
	pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_AREA);
	pSliderCtrl->SetRange(1, 64);
	pSliderCtrl->SetPos(m_nShowWidth);
	pSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_AREA);
	pSpinCtrl->SetBase(10);
	pSpinCtrl->SetRange(1, 64);
	pSpinCtrl->SetPos(m_nShowWidth);

	//格子高度
	pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CELLHEIGHT);
	pSliderCtrl->SetRange(1, 20);
	pSliderCtrl->SetPos(m_nCellHeight);
	pSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_CELLHEIGHT);
	pSpinCtrl->SetBase(10);
	pSpinCtrl->SetRange(1, 20);
	pSpinCtrl->SetPos(m_nCellHeight);

	((CButton*)GetDlgItem(IDC_RADIO_RECTPEN))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_CHECK_MASKREST))->SetCheck(TRUE);
	SetLogicCheck();
	SetShowWindow(0);
	GetDlgItem(IDC_EDIT_COLOR)->EnableWindow(FALSE);
	
	m_bInit = TRUE;
Exit0:
	return bResult;
}

void KSceneSceneEditorDialogLogical::SetLogicCheck()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_SHOWLOGICALOBSTACLE))->GetCheck();

	GetDlgItem(IDC_CHECK_SHOWSURFACEOBSTACLE)->EnableWindow(bCheck);
	GetDlgItem(IDC_CHECK_SHOWDOODADOBSTACLE)->EnableWindow(bCheck);
	GetDlgItem(IDC_CHECK_SHOWSELECTDOODADOBSTACLE)->EnableWindow(bCheck);
	
	if (!bCheck)
	{
		((CButton*)GetDlgItem(IDC_CHECK_SHOWSURFACEOBSTACLE))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_SHOWDOODADOBSTACLE))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_SHOWSELECTDOODADOBSTACLE))->SetCheck(FALSE);
	}
}

void KSceneSceneEditorDialogLogical::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nCurSel = m_TabCtrl.GetCurSel();
	SetShowWindow(nCurSel);
}

void KSceneSceneEditorDialogLogical::Save()
{
	char szName[64];
	COLORREF color;
	DWORD dwMask;
	char szFileName[MAX_PATH];
	char szPathName[MAX_PATH];
	CRect rect;
	char* pszSection = "Section";
	char szSectionName[64];

	char szText[MAX_PATH];
	if (m_eBrushShape == cbRectangle)
	{
		itoa(1, szText, 10);
	}
	else
	{
		itoa(2, szText, 10);
	}
	WritePrivateProfileString("Settings", "笔刷形状", szText, m_szLogicFileName);
	itoa(m_nBrushWidth, szText, 10);
	WritePrivateProfileString("Settings", "笔刷大小", szText, m_szLogicFileName);
	itoa(m_nShowWidth, szText, 10);
	WritePrivateProfileString("Settings", "显示范围", szText, m_szLogicFileName);
	itoa(m_nCellHeight, szText, 10);
	WritePrivateProfileString("Settings", "格子高度", szText, m_szLogicFileName);

	for (int i = 0; i < m_ListLogicData.GetItemCount(); i++)
	{
		LogicDataItem* pItem = (LogicDataItem*)m_ListLogicData.GetItemData(i);
		if (pItem)
		{
			pItem->GetName(szName);
			pItem->GetItemInfo(&color, &dwMask, szFileName, szPathName, &rect);
			sprintf(szSectionName, "%s%d", pszSection, i);
			
			if (dwMask & dwBitInfo[0][1])
			{
				itoa(1, szText, 10);
			}
			else
			{
				itoa(0, szText, 10);
			}
			WritePrivateProfileString(szSectionName, "障碍", szText, m_szLogicFileName);

			if (dwMask & dwBitInfo[1][1])
			{
				itoa(1, szText, 10);
			}
			else
			{
				itoa(0, szText, 10);
			}
			WritePrivateProfileString(szSectionName, "不可摆放物件", szText, m_szLogicFileName);
			if (dwMask & dwBitInfo[2][1])
			{
				itoa(1, szText, 10);
			}
			else
			{
				itoa(0, szText, 10);
			}
			WritePrivateProfileString(szSectionName, "可摆摊", szText, m_szLogicFileName);
			if (dwMask & dwBitInfo[3][1])
			{
				itoa(1, szText, 10);
			}
			else
			{
				itoa(0, szText, 10);
			}
			WritePrivateProfileString(szSectionName, "室内区域", szText, m_szLogicFileName);
			if (dwMask & dwBitInfo[4][1])
			{
				itoa(1, szText, 10);
			}
			else
			{
				itoa(0, szText, 10);
			}
			WritePrivateProfileString(szSectionName, "休息区域", szText, m_szLogicFileName);
			if (dwMask & dwBitInfo[5][1])
			{
				itoa(1, szText, 10);
			}
			else
			{
				itoa(0, szText, 10);
			}
			WritePrivateProfileString(szSectionName, "下马区域", szText, m_szLogicFileName);
			ultoa((unsigned long)color, szText, 10);
			WritePrivateProfileString(szSectionName, "颜色", szText, m_szLogicFileName);
			WritePrivateProfileString(szSectionName, "名称", szName, m_szLogicFileName);
			WritePrivateProfileString(szSectionName, "脚本", szPathName, m_szLogicFileName); 
		}
		else
		{
			MessageBox("在列表中有些项目没有绑定有效的数据，将可能导致保存异常");
		}
	}
}

void KSceneSceneEditorDialogLogical::Load()
{
	int nRet = GetPrivateProfileInt("Settings", "笔刷形状", 1, m_szLogicFileName);
	if (nRet == 1)
	{
		m_eBrushShape = cbRectangle;
		((CButton*)GetDlgItem(IDC_RADIO_RECTPEN))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_ROUNDPEN))->SetCheck(FALSE);
	}
	else
	{
		m_eBrushShape = cbCircle;
		((CButton*)GetDlgItem(IDC_RADIO_RECTPEN))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ROUNDPEN))->SetCheck(TRUE);
	}
	m_nBrushWidth = GetPrivateProfileInt("Settings", "笔刷大小", 5, m_szLogicFileName);
	m_nShowWidth = GetPrivateProfileInt("Settings", "显示范围", 1, m_szLogicFileName);
	m_nCellHeight = GetPrivateProfileInt("Settings", "格子高度", 1, m_szLogicFileName);

	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PENSIZE);
	pSliderCtrl->SetPos(m_nBrushWidth);
	CSpinButtonCtrl* pSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_PENSIZE);
	pSpinCtrl->SetPos(m_nBrushWidth);

	//显示区域大小
	pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_AREA);
	pSliderCtrl->SetPos(m_nShowWidth);
	pSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_AREA);
	pSpinCtrl->SetPos(m_nShowWidth);

	//格子高度
	pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CELLHEIGHT);
	pSliderCtrl->SetPos(m_nCellHeight);
	pSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_CELLHEIGHT);
	pSpinCtrl->SetPos(m_nCellHeight);
	LoadLogicInfo();
}

void KSceneSceneEditorDialogLogical::LoadLogicInfo()
{
	char szSectionName[64];
	char szSection[64];
	strcpy(szSection, "Section");
	for (int i = 0;; i++)
	{
		sprintf(szSectionName, "%s%d", szSection, i);
		char szName[64];
		GetPrivateProfileString(szSectionName, "名称", "", szName, 64, m_szLogicFileName);
		if (strlen(szName) == 0)
		{
			break;
		}
		m_ListLogicData.InsertItem(LVIF_TEXT|LVIF_STATE, i, "", LVIS_SELECTED, LVIS_SELECTED, 0, 0);
		m_ListLogicData.SetItemText(i, 0, szName);

		LogicDataItem* pItem = new LogicDataItem;
		m_ListLogicData.SetItemData(i, (DWORD_PTR)pItem);
		DWORD dwMask = 0;
		int nRet;
		nRet = GetPrivateProfileInt(szSectionName, "障碍", 0, m_szLogicFileName);
		if (nRet)
		{
			dwMask = dwMask | dwBitInfo[0][1];
		}
		nRet = GetPrivateProfileInt(szSectionName, "不可摆放物件", 0, m_szLogicFileName);
		if (nRet)
		{
			dwMask = dwMask | dwBitInfo[1][1];
		}
		nRet = GetPrivateProfileInt(szSectionName, "可摆摊", 0, m_szLogicFileName);
		if (nRet)
		{
			dwMask = dwMask | dwBitInfo[2][1];
		}
		nRet = GetPrivateProfileInt(szSectionName, "室内区域", 0, m_szLogicFileName);
		if (nRet)
		{
			dwMask = dwMask | dwBitInfo[3][1];
		}
		nRet = GetPrivateProfileInt(szSectionName, "休息区域", 0, m_szLogicFileName);
		if (nRet)
		{
			dwMask = dwMask | dwBitInfo[4][1];
		}
		nRet = GetPrivateProfileInt(szSectionName, "下马区域", 0, m_szLogicFileName);
		if (nRet)
		{
			dwMask = dwMask | dwBitInfo[5][1];
		}
		COLORREF color = GetPrivateProfileInt(szSectionName, "颜色", (INT)RGB(255, 255, 255), m_szLogicFileName);
		char szScript[MAX_PATH];
		GetPrivateProfileString(szSectionName, "脚本", "", szScript, MAX_PATH, m_szLogicFileName);

		string strFilePath = szScript;
		int nIndex = strFilePath.find("scripts\\");
		if (nIndex == -1)
		{
			nIndex = 0;
		}
		strcpy(szScript, strFilePath.c_str() + nIndex);

		int nLen = strlen(szScript);
		for (int i = nLen - 1; i > 0; i--)
		{
			if (szScript[i] == '\\' || szScript[i] == '/')
			{
				nLen = i + 1;
				break;
			}
		}

		pItem->SetItemInfo(color, FALSE, dwMask, szScript + nLen, szScript, CRect(0, 0, 0, 0));
		pItem->SetName(szName);
	}
	RefreshItemRect();
	m_ListLogicData.RedrawItems(0, m_ListLogicData.GetItemCount() - 1);
}

void KSceneSceneEditorDialogLogical::put_MapFileName(CString cstrPathName)
{
	ASSERT(cstrPathName.GetLength() < MAX_PATH);
	strcpy(m_szLogicFileName, cstrPathName);
	m_szLogicFileName[strlen(m_szLogicFileName) - 3] = '\0';
	if (strlen(m_szLogicFileName) + strlen("Logic.ini") < MAX_PATH)
	{
		strcat(m_szLogicFileName, "Logic.ini");
	}
	else
	{
		MessageBox("文件路径错误，逻辑信息设置参数将不可以保存");
	}
}

void KSceneSceneEditorDialogLogical::GetItem(long Index, BSTR * ItemName, long * LogicInfo, BSTR * ScriptFileName)
{
	LogicDataItem* pItem = (LogicDataItem*)m_ListLogicData.GetItemData(Index);
	if (!pItem)
		return;
	COLORREF color;
	char szName[64];
	char szFileName[MAX_PATH];
	char szPathName[MAX_PATH];
	CRect rect;
	pItem->GetName(szName);
	pItem->GetItemInfo(&color, (DWORD*)LogicInfo, szFileName, szPathName, &rect);

	*ItemName = _com_util::ConvertStringToBSTR(szName);
	*ScriptFileName = _com_util::ConvertStringToBSTR(szPathName);
}	

long KSceneSceneEditorDialogLogical::get_Count()
{
	return m_ListLogicData.GetItemCount();
}

void KSceneSceneEditorDialogLogical::OnBnClickedEditColor()
{
	CColorDialog dlg;
	static bool bShowDlg = true;
	if (bShowDlg)
	{
		bShowDlg = false;
		if (dlg.DoModal() == IDOK)
		{
			m_color= dlg.GetColor();
			OnBnClickedLogicInfo();
		}
		bShowDlg = true;
	}
}

//HBRUSH KSceneSceneEditorDialogLogical::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//	HBRUSH hOldBrush = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
//	if (nCtlColor == CTLCOLOR_EDIT &&
//		pWnd->GetDlgCtrlID() == IDC_EDIT_COLOR)
//	{	
//		pDC->SetDCBrushColor(m_color);
////			->SetBkColor(m_color);
//	}
//	return hOldBrush;
//}

void KSceneSceneEditorDialogLogical::SetShowWindow(int nCurSel)
{
	GetDlgItem(IDC_LIST_LOGICDATA)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_BN_ADD)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_BN_DEL)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_BN_OK)->ShowWindow(TAB1 == nCurSel && m_bShowOKCANCEL);
	GetDlgItem(IDC_BN_CANCEL)->ShowWindow(TAB1 == nCurSel && m_bShowOKCANCEL);
	GetDlgItem(IDC_BN_SHOW)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_STATIC_LOGICALSET)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_CHECK_OBSTACLE)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_CHECK_CANPUTDOODAD)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_CHECK_STALLAGE)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_CHECK_INDOORREST)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_CHECK_REST)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_CHECK_DISMOUNT)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_BN_BROWSESCRIPT)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_BN_DELETESCRIPT)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_EDIT_COLOR)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_EDIT_SCRIPT)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_STATIC_SCRIPT)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_STATIC_COLOR)->ShowWindow(TAB1 == nCurSel);
	GetDlgItem(IDC_CLEARALLTRAP)->ShowWindow(TAB1 == nCurSel);

	GetDlgItem(IDC_RADIO_RECTPEN)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_RADIO_ROUNDPEN)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_STATIC_PEN)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_SLIDER_PENSIZE)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_SPIN_PENSIZE)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_EDIT_PENSIZE)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_STATIC_AREA)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_SLIDER_AREA)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_EDIT_AREA)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_SPIN_AREA)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_STATIC_CELLHEIGHT)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_SLIDER_CELLHEIGHT)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_EDIT_CELLHEIGHT)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_SPIN_CELLHEIGHT)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_STATIC_SURFACETYPE)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_STATIC_SLIDEDIRECTION)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_STATIC_SLIDESPEED)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_COMBO_SURFACETYPE)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_COMBO_SLIDEDIRECTION)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_EDIT_SLIDESPEED)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_SPIN_SLIDESPEED)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_CHECK_SHOWLOGICALOBSTACLE)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_CHECK_SHOWSURFACEOBSTACLE)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_CHECK_SHOWDOODADOBSTACLE)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_CHECK_SHOWSELECTDOODADOBSTACLE)->ShowWindow(TAB2 == nCurSel);
	GetDlgItem(IDC_CHECK_MASKREST)->ShowWindow(TAB2 == nCurSel);
}

void KSceneSceneEditorDialogLogical::SetChildWindowPos(int nCurSel, CRect rect)
{
	//list
	CWnd* pWnd = GetDlgItem(IDC_LIST_LOGICDATA);
	CRect r;
	pWnd->GetWindowRect(&r);
	ScreenToClient(&r);
	pWnd->MoveWindow(r.left, 25, rect.Width(), rect.Height() - 260, TRUE);
	
	// 公共偏移  高度190
	int nX = 8;
	int nY = rect.Height() - 230;
	MoveWindowByOffset(nX, nY, IDC_BN_ADD);
	MoveWindowByOffset(nX, nY, IDC_BN_DEL);
	MoveWindowByOffset(nX, nY, IDC_BN_OK);
	MoveWindowByOffset(nX, nY, IDC_BN_CANCEL);
	MoveWindowByOffset(nX, nY, IDC_BN_SHOW);
	MoveWindowByOffset(nX, nY, IDC_STATIC_LOGICALSET);
	MoveWindowByOffset(nX, nY, IDC_CHECK_OBSTACLE);
	MoveWindowByOffset(nX, nY, IDC_CHECK_CANPUTDOODAD);
	MoveWindowByOffset(nX, nY, IDC_CHECK_STALLAGE);
	MoveWindowByOffset(nX, nY, IDC_CHECK_INDOORREST);
	MoveWindowByOffset(nX, nY, IDC_CHECK_REST);
	MoveWindowByOffset(nX, nY, IDC_CHECK_DISMOUNT);
	MoveWindowByOffset(nX, nY, IDC_EDIT_SCRIPT);
	MoveWindowByOffset(nX, nY, IDC_BN_BROWSESCRIPT);
	MoveWindowByOffset(nX, nY, IDC_BN_DELETESCRIPT);
	MoveWindowByOffset(nX, nY, IDC_EDIT_COLOR);
	MoveWindowByOffset(nX, nY, IDC_STATIC_SCRIPT);
	MoveWindowByOffset(nX, nY, IDC_STATIC_COLOR);
	MoveWindowByOffset(nX, nY, IDC_CLEARALLTRAP);
    MoveWindowByOffset(nX, nY, IDC_EXPORT_MESH_DATA);
}

void KSceneSceneEditorDialogLogical::MoveWindowByOffset(int nX, int nY, DWORD dwWndID)
{
	CRect r;
	CWnd* pWnd = GetDlgItem(dwWndID);
	static CRect RectOffset;

	pWnd->GetWindowRect(&r);
	ScreenToClient(&r);
	//记录第一个控件位置
	if (dwWndID == IDC_BN_ADD)
	{
		RectOffset = r;
	}
	pWnd->MoveWindow(r.left - RectOffset.left + nX, r.top - RectOffset.top + nY, r.Width(), r.Height(), TRUE);
}

void KSceneSceneEditorDialogLogical::OnBnClickedAdd()
{
	m_bShowOKCANCEL = TRUE;
	GetDlgItem(IDC_BN_OK)->ShowWindow(TRUE);
	GetDlgItem(IDC_BN_CANCEL)->ShowWindow(TRUE);
	int nIndex =  m_ListLogicData.GetItemCount();
	char szItemText[128];
	char szTemp[128];
	
	strcpy(szItemText, "新项目");
	itoa(nIndex, szTemp, 10);
	strcat(szItemText, szTemp);
	m_ListLogicData.InsertItem(LVIF_TEXT|LVIF_STATE, nIndex, "", LVIS_SELECTED, LVIS_SELECTED, 0, 0);
	m_ListLogicData.SetItemText(nIndex, 0, szItemText);
	m_ListLogicData.SetSelectionMark(nIndex);

	m_color = RGB(255, 255, 255);
	m_szFileName[0] = '\0';
	m_szFilePath[0] = '\0';
	((CButton*)GetDlgItem(IDC_CHECK_OBSTACLE))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_CANPUTDOODAD))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_STALLAGE))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_INDOORREST))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_REST))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_DISMOUNT))->SetCheck(FALSE);
	GetDlgItem(IDC_EDIT_SCRIPT)->SetWindowText("");
	RedrawColorWindow();
	SetEngineModifyStateByLogicInfo(FALSE, 0, NULL);
	if (m_lpSceneEditor)
		m_lpSceneEditor->SetCurrentEditColor(m_color);
}

void KSceneSceneEditorDialogLogical::OnBnClickedDel()
{
	int nIndex = m_ListLogicData.GetSelectionMark();
	if (nIndex < 0)
		return;
	LogicDataItem* pItem = (LogicDataItem*)m_ListLogicData.GetItemData(nIndex);
	if (pItem && m_lpSceneEditor)
	{
		m_lpSceneEditor->DeleteCurrentCellInfo();//TransToEngineFormat(LogicInfo), ScriptFileName);
		m_lpSceneEditor->RecalcShowCell(); //刷新当前显示的格子集合
	}
	SAFE_DELETE(pItem);
	m_ListLogicData.DeleteItem(nIndex);
	RefreshItemRect();
}

void KSceneSceneEditorDialogLogical::RefreshItemRect()
{
	for (int i = 0; i < m_ListLogicData.GetItemCount(); i++)
	{
		LogicDataItem* pItem = (LogicDataItem*)m_ListLogicData.GetItemData(i);
		if (pItem == NULL)
		{
			continue;
		}
		CRect r;
		m_ListLogicData.GetSubItemRect(i, 1, LVIR_BOUNDS, r);
		r.left += 2;
		r.right -= 2;
		r.top += 2;
		r.bottom -= 2;
		pItem->SetRect(r);
	}
}

void KSceneSceneEditorDialogLogical::OnBnClickedOK()
{
	int nIndex = m_ListLogicData.GetSelectionMark();
	if (nIndex == -1)
		return;
	m_bShowOKCANCEL = FALSE;
	GetDlgItem(IDC_BN_OK)->ShowWindow(FALSE);
	GetDlgItem(IDC_BN_CANCEL)->ShowWindow(FALSE);
	char szOldFileName[MAX_PATH];
	char szOldFilePath[MAX_PATH];
	BOOL bModify = FALSE;
	COLORREF OldColor;
	DWORD dwOldMask;
	CRect OldRect;
	LogicDataItem* pItem = (LogicDataItem*)m_ListLogicData.GetItemData(nIndex);
	if (!pItem)
	{
		pItem = new LogicDataItem;
		m_ListLogicData.SetItemData(nIndex, (DWORD_PTR)pItem);
	}
	else 
	{
		bModify = TRUE;
		pItem->GetItemInfo(&OldColor, &dwOldMask, szOldFileName, szOldFilePath, &OldRect);
	}
	CRect RectColor;
	DWORD dwMask = MODIFY_NONE;
	
	m_ListLogicData.GetSubItemRect(nIndex, 1, LVIR_BOUNDS, RectColor);
	RectColor.left += 2;
	RectColor.right -= 2;
	RectColor.top += 2;
	RectColor.bottom -= 2;

	if (((CButton*)GetDlgItem(IDC_CHECK_OBSTACLE))->GetCheck())
	{
		dwMask = dwMask | dwBitInfo[0][1];
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_CANPUTDOODAD))->GetCheck())
	{
		dwMask = dwMask | dwBitInfo[1][1];
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_STALLAGE))->GetCheck())
	{
		dwMask = dwMask | dwBitInfo[2][1];
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_INDOORREST))->GetCheck())
	{
		dwMask = dwMask | dwBitInfo[3][1];
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_REST))->GetCheck())
	{
		dwMask = dwMask | dwBitInfo[4][1];
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_DISMOUNT))->GetCheck())
	{
		dwMask = dwMask | dwBitInfo[5][1];
	}

	pItem->SetItemInfo(m_color, FALSE, dwMask, m_szFileName, m_szFilePath, RectColor);

	for (int i = 0; i < m_ListLogicData.GetItemCount(); i++)
	{
		if (i == nIndex)
		{
			continue;
		}
		char szScriptN[MAX_PATH];
		char szScriptP[MAX_PATH];
		DWORD Mask;
		COLORREF color;
		CRect rt;
		LogicDataItem* pLogicDataItem = (LogicDataItem*)m_ListLogicData.GetItemData(i);
		if (pLogicDataItem)
		{
			pLogicDataItem->GetItemInfo(&color, &Mask, szScriptN, szScriptP, &rt);
			if (Mask == dwMask && strcmp(szScriptP, m_szFilePath) == 0)
			{
				MessageBox("列表中已有相同逻辑信息的项,请重新设置该项！");
				SAFE_DELETE(pItem);
				m_ListLogicData.SetItemData(nIndex, NULL);
				return;
			}
		}
	}

	CString strName = m_ListLogicData.GetItemText(nIndex, 0);
	pItem->SetName(strName.GetBuffer());
	if (m_lpSceneEditor && bModify)
	{
		/*char szOldFile[MAX_PATH * 2];
		char szNewFile[MAX_PATH * 2];
		strcpy(szOldFile, szOldFilePath);
		strcpy(szNewFile, m_strFilePath);*/
		m_lpSceneEditor->ModifyCurrentCellInfo(TransToEngineFormat(dwOldMask), TransToEngineFormat(dwMask), szOldFilePath, m_szFilePath);
		SetEngineModifyStateByLogicInfo((dwMask || strlen(m_szFilePath) > 0), dwMask, m_szFilePath);
		m_lpSceneEditor->SetCurrentEditColor(m_color);
	}
	else if (m_lpSceneEditor)
	{
		SetEngineModifyStateByLogicInfo(TRUE, dwMask, m_szFilePath);
		if (m_lpSceneEditor)
			m_lpSceneEditor->SetCurrentEditColor(m_color);
	}
	m_ListLogicData.RedrawItems(0, m_ListLogicData.GetItemCount() - 1);

	//处理Poly
	list<IEKG3DRepresentPoly*>::iterator i = m_lpDoc->m_lpLogicalData->m_listLogicalPoly.begin();
	list<IEKG3DRepresentPoly*>::iterator iend = m_lpDoc->m_lpLogicalData->m_listLogicalPoly.end();
	while (i!=iend)
	{
		IEKG3DRepresentPoly* pPoly = *i;
		if (strcmp(pPoly->GetLogicalSetName(), strName.GetBuffer()) == 0)
		{
			pPoly->SetModifyState(TransToEngineFormat(dwMask));
			pPoly->SetScriptFileName(m_szFilePath);
		}
		++i;
	}

}

void KSceneSceneEditorDialogLogical::OnBnClickedCancel()
{
	m_bShowOKCANCEL = FALSE;
	GetDlgItem(IDC_BN_OK)->ShowWindow(FALSE);
	GetDlgItem(IDC_BN_CANCEL)->ShowWindow(FALSE);
}

void KSceneSceneEditorDialogLogical::OnTcnSelchangeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nIndex = m_ListLogicData.GetHotItem();
	if (nIndex < 0)
		return;
	LogicDataItem* pItem = (LogicDataItem*)m_ListLogicData.GetItemData(nIndex);
	if (pItem)
	{
		DWORD dwMask;
		CRect rect;
		pItem->GetItemInfo(&m_color, &dwMask, m_szFileName, m_szFilePath, &rect);
		((CButton*)GetDlgItem(IDC_CHECK_OBSTACLE))->SetCheck(dwMask & dwBitInfo[0][1]);
		((CButton*)GetDlgItem(IDC_CHECK_CANPUTDOODAD))->SetCheck(dwMask & dwBitInfo[1][1]);
		((CButton*)GetDlgItem(IDC_CHECK_STALLAGE))->SetCheck(dwMask & dwBitInfo[2][1]);
		((CButton*)GetDlgItem(IDC_CHECK_INDOORREST))->SetCheck(dwMask & dwBitInfo[3][1]);
		((CButton*)GetDlgItem(IDC_CHECK_REST))->SetCheck(dwMask & dwBitInfo[4][1]);
		((CButton*)GetDlgItem(IDC_CHECK_DISMOUNT))->SetCheck(dwMask & dwBitInfo[5][1]);
		GetDlgItem(IDC_EDIT_SCRIPT)->SetWindowText(m_szFileName);
		RedrawColorWindow();
		char szFile[MAX_PATH];
		strcpy_s(szFile,MAX_PATH,m_szFilePath);
		SetEngineModifyStateByLogicInfo(TRUE, dwMask, szFile);
		if (m_lpSceneEditor)
			m_lpSceneEditor->SetCurrentEditColor(m_color);
	}
	else
	{
		m_color = RGB(255, 255, 255);
		m_szFileName[0] = '\0';
		m_szFilePath[0] = '\0';
		((CButton*)GetDlgItem(IDC_CHECK_OBSTACLE))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_CANPUTDOODAD))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_STALLAGE))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_INDOORREST))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_REST))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_DISMOUNT))->SetCheck(FALSE);
		GetDlgItem(IDC_EDIT_SCRIPT)->SetWindowText("");
		RedrawColorWindow();
		SetEngineModifyStateByLogicInfo(FALSE, 0, NULL);
		if (m_lpSceneEditor)
			m_lpSceneEditor->SetCurrentEditColor(m_color);
	}
}

void KSceneSceneEditorDialogLogical::RedrawColorWindow()
{
	CRect r;
	CBrush NewBrush,*pOldBrush;
	CDC* pDC = GetDlgItem(IDC_EDIT_COLOR)->GetDC();

	NewBrush.CreateSolidBrush(m_color);
	pOldBrush = pDC->SelectObject(&NewBrush);
	GetDlgItem(IDC_EDIT_COLOR)->GetWindowRect(&r);
	GetDlgItem(IDC_EDIT_COLOR)->ScreenToClient(&r);

	r.left += 1;
	r.right -= 1;
	r.top += 1;
	r.bottom -= 1;
	KG_PROCESS_ERROR(pDC->Rectangle(r));
	pDC->SelectObject(pOldBrush);
	NewBrush.DeleteObject();

	GetDlgItem(IDC_EDIT_COLOR)->ReleaseDC(pDC);
Exit0:
	;
}

LogicDataItem::LogicDataItem()
{
	m_szName[0] = '\0';
	m_color = RGB(255, 255, 255);
	m_bReadOnly = FALSE;
	m_LogicMask = 0;
	m_szScriptFileName[0] = '\0';
	m_szScriptFilePath[0] = '\0';
	m_ClolorRect.bottom = 0;
	m_ClolorRect.top = 0;
	m_ClolorRect.left = 0;
	m_ClolorRect.right = 0;
}

void LogicDataItem::GetColor(COLORREF *pColor)
{
	*pColor = m_color;
}

void LogicDataItem::GetItemInfo(COLORREF* pColor, DWORD* pLogicMask, char* pszFileName, char* pszFilePath, CRect* pRect)
{
	*pColor = m_color;
	*pLogicMask = m_LogicMask;
	strcpy_s(pszFileName,MAX_PATH, m_szScriptFileName);
	strcpy_s(pszFilePath,MAX_PATH, m_szScriptFilePath);
	*pRect = m_ClolorRect;
}

void LogicDataItem::GetRect(CRect *pRect)
{
	*pRect = m_ClolorRect;
}

void LogicDataItem::SetItemInfo(COLORREF color, BOOL ReadOnly, DWORD LogicMask, const char* pszFileName, const char* pszFilePath, CRect rect)
{
	ASSERT(pszFileName);
	ASSERT(pszFilePath);
	ASSERT(strlen(pszFileName) < MAX_PATH - 1);
	ASSERT(strlen(pszFilePath) < MAX_PATH - 1);
	m_color = color;
	m_bReadOnly = ReadOnly;
	m_LogicMask = LogicMask;
	strcpy(m_szScriptFileName, pszFileName);
	strcpy(m_szScriptFilePath, pszFilePath);
	m_ClolorRect = rect;
}

void LogicDataItem::SetRect(CRect rect)
{
	m_ClolorRect = rect;
}

void LogicDataItem::SetName(const char* pszName)
{
	ASSERT(pszName);
	strcpy(m_szName, pszName);
}

void LogicDataItem::GetName(char* pszNameBuf)
{
	ASSERT(pszNameBuf);
	strcpy(pszNameBuf, m_szName);
}

typedef std::vector<D3DXMATRIX> KMatrixTable;
typedef std::map<LPD3DXMESH, KMatrixTable> KMeshTable;

KMeshTable g_MeshTable;

BOOL ExportEntityMeshCallback(
    float fXStart, float fYStart, float fZStart, 
    LPD3DXMESH piMesh, int nNumSubset,BOOL bSubsetNoCell[],
    const D3DXMATRIX& crTransformationMatrix,
    int nDynamicObstacleGroupIndex,LPSTR pMeshName
)
{
    KMatrixTable& MatrixTable = g_MeshTable[piMesh];
    
    for (int i = 0; i < (int)MatrixTable.size(); i++)
    {
        if (MatrixTable[i] == crTransformationMatrix)
        {
            return true;
        }
    }

    MatrixTable.push_back(crTransformationMatrix);

    return true;
}

#pragma pack(1)
struct KTerrainHeader
{
    int   nVertexCountX;
    int   nVertexCountZ;
};

struct KMeshHeader
{
    int nVertexCount;
    int nTriangleCount;
};

struct KVertex 
{
    float x;
    float y;
    float z;
};

struct KTriangle
{
    int nA;
    int nB;
    int nC;
};
#pragma pack()

BOOL ProcessMesh(FILE* pFile, LPD3DXMESH piMesh, const D3DXMATRIX& crMatrix, float fBaseX, float fBaseZ)
{
    BOOL            bResult             = false;
    HRESULT         hRetCode            = E_FAIL;
    D3DXVECTOR3*    pPos                = NULL;
    BYTE*           pVertices           = NULL;
	WORD*           pwIndices           = NULL;
    DWORD           dwVerticesStride    = 0;
    DWORD           dwNumFaces          = 0;
    DWORD           dwNumVertices       = 0;
    size_t          uWrite              = 0;
    KMeshHeader     Header;

    hRetCode = piMesh->LockVertexBuffer(0, (void**)&pVertices);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piMesh->LockIndexBuffer(0, (void**)&pwIndices);
    KGLOG_COM_PROCESS_ERROR(hRetCode);        	

    dwVerticesStride    = piMesh->GetNumBytesPerVertex();
    dwNumVertices       = piMesh->GetNumVertices();
    dwNumFaces          = piMesh->GetNumFaces();

    KGLOG_PROCESS_ERROR(dwNumVertices > 0);
    KGLOG_PROCESS_ERROR(dwNumVertices < USHRT_MAX);
    KGLOG_PROCESS_ERROR(dwNumFaces > 0);

    Header.nVertexCount     = dwNumVertices;
    Header.nTriangleCount   = dwNumFaces;

    uWrite = fwrite(&Header, sizeof(Header), 1, pFile); 
    KGLOG_PROCESS_ERROR(uWrite == 1);

    for (DWORD l = 0; l < dwNumVertices; l++)
    {
        D3DXVECTOR3 vPos = *(D3DXVECTOR3*)(pVertices + dwVerticesStride *l);
        D3DXVECTOR3 vOut;
        KVertex     Vertex;

        D3DXVec3TransformCoord(&vOut, &vPos, &crMatrix);

        Vertex.x = vOut.x - fBaseX;
        Vertex.y = vOut.y;
        Vertex.z = vOut.z - fBaseZ;

        uWrite = fwrite(&Vertex, sizeof(Vertex), 1, pFile); 
        KGLOG_PROCESS_ERROR(uWrite == 1);
    }

    for (DWORD l = 0; l < dwNumFaces; l++)
    {
        KTriangle   Triangle;

        Triangle.nA = pwIndices[l * 3 + 0];
        Triangle.nB = pwIndices[l * 3 + 1];
        Triangle.nC = pwIndices[l * 3 + 2];

        uWrite = fwrite(&Triangle, sizeof(Triangle), 1, pFile); 
        KGLOG_PROCESS_ERROR(uWrite == 1);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        MessageBox(NULL, "Process mesh error !", "error", MB_OK);
    }
    return bResult;
}

void KSceneSceneEditorDialogLogical::OnBnClickedExportMeshData()
{
    HRESULT                 hResult                 = E_FAIL;
    int                     nRetCode                = false;
    IEKG3DScene*            pi3DScene               = NULL;
    float                   fBaseX                  = 0.0f;
    float                   fBaseZ                  = 0.0f;
    int                     nVertexStartX           = 0;
    int                     nVertexStartZ           = 0;
    int                     nVertexCountX           = 0;
    int                     nVertexCountZ           = 0;
    FILE*                   pFile                   = NULL;
    float*                  pfHeight                = NULL;
	IEKG3DSceneSceneEditor* pEditor                 = NULL;
    KTerrainHeader          Header;
    D3DXVECTOR3             vOut;

    pi3DScene = m_lpDoc->GetScene();
    KGLOG_PROCESS_ERROR(pi3DScene);

	pi3DScene->GetSceneSceneEditor(&pEditor);
    KGLOG_PROCESS_ERROR(pEditor);

    nRetCode = pEditor->ForceLoadAllEntity();
    KGLOG_COM_PROCESS_ERROR(nRetCode);

	pi3DScene->GetLogicalSceneSize(&nVertexStartX, &nVertexStartX, &nVertexCountX, &nVertexCountZ);

    fBaseX = TERRIANCELLWIDTH * nVertexStartX;
    fBaseZ = TERRIANCELLWIDTH * nVertexStartZ;

    pfHeight = new float[nVertexCountX * nVertexCountZ];
    KGLOG_PROCESS_ERROR(pfHeight);

    pFile = fopen("D:\\Mesh.dat", "wb");
    KGLOG_PROCESS_ERROR(pFile);

    Header.nVertexCountX = nVertexCountX;
    Header.nVertexCountZ = nVertexCountZ;

    nRetCode = fwrite(&Header, sizeof(Header), 1, pFile);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    for (int nZ = 0; nZ < nVertexCountZ; nZ++)
    {
        for (int nX = 0; nX < nVertexCountX; nX++)
        {
            float fPosX     = fBaseX + TERRIANCELLWIDTH * nX;
            float fPosZ     = fBaseZ + TERRIANCELLWIDTH * nZ;
            int   nIndex    = nVertexCountX * nZ + nX;

            pi3DScene->GetHeight(&vOut,&D3DXVECTOR3(fPosX, 0, fPosZ));

            pfHeight[nIndex] = vOut.y;
        }
    }

    nRetCode = fwrite(pfHeight, sizeof(float) * nVertexCountX * nVertexCountZ, 1, pFile);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

	int nRegionCountX = (int)((nVertexCountX - 1) * TERRIANCELLWIDTH  / LOGICAL_CELL_CM_LENGTH / REGION_GRID_WIDTH + 1);
    int nRegionCountZ = (int)((nVertexCountZ - 1) * TERRIANCELLWIDTH  / LOGICAL_CELL_CM_LENGTH / REGION_GRID_HEIGHT + 1);

    // 本来直接遍历所有Entity就可以了,但是那段代码太乱了...
    // 暂时先遍历所有Region拼起来吧
    for (int nZ = 0; nZ < nRegionCountZ; nZ++)
    {
        for (int nX = 0; nX < nRegionCountX; nX++)
        {
            pi3DScene->TraverseEntityMeshForGenerateLogicObstacle(
                TERRIANCELLWIDTH * nVertexStartX, TERRIANCELLWIDTH * nVertexStartZ, 
                nX, nZ, ExportEntityMeshCallback
            );
        }
    }

    for (KMeshTable::iterator it = g_MeshTable.begin(); it != g_MeshTable.end(); ++it)
    {
        KMatrixTable& MatrixTable = it->second;
        
        for (int i = 0; i < (int)MatrixTable.size(); i++)
        {
            ProcessMesh(pFile, it->first, MatrixTable[i], fBaseX, fBaseZ);
        }
    }

Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    KG_DELETE_ARRAY(pfHeight);
    g_MeshTable.clear();
    return;
}
