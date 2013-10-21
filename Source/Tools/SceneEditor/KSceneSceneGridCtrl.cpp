#include "StdAfx.h"
#include "KSceneSceneGridCtrl.h"
#include <math.h>
#include <limits>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(KSceneSceneGridCtrl, CGridCtrl)

KSceneSceneGridCtrl::KSceneSceneGridCtrl(void)
:m_nLastHighLightRow(-1)
,m_nLastHighLightColumn(-1)
,m_bEnableHighLightFixed(false)
,m_pScrollBarHorz(NULL)
,m_dwState(0)
,m_pEventHandler(NULL)
{
}

KSceneSceneGridCtrl::~KSceneSceneGridCtrl(void)
{
	m_pEventHandler = NULL;
}

BEGIN_MESSAGE_MAP(KSceneSceneGridCtrl, CGridCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()		
END_MESSAGE_MAP()

HRESULT KSceneSceneGridCtrl::SetEventHandler( KSceneSceneGridCtrlEventHandler* pEventHandler )
{
	m_pEventHandler = pEventHandler;
	return S_OK;
}

KSceneSceneGridCtrl::InitAppearanceStruct::InitAppearanceStruct()
{
	ZeroMemory(this, sizeof(InitAppearanceStruct) - sizeof(CString));///最后一个自己初始化
	m_bEditEnable = TRUE;
}

BOOL KSceneSceneGridCtrl::InitAppearanceStruct::IsValid()
{
	if (m_nFixedColumnCount >= 0 
		&& m_nFixedRowCount >= 0
		&& m_nRowCount >= m_nFixedRowCount
		&& m_nColumnCount >= m_nFixedColumnCount
		&& m_nDefColumnWidth > 0
		&& m_nFixedColumnWidth > 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL KSceneSceneGridCtrl::InitAppearance( InitAppearanceStruct* pStruct )
{
	_ASSERTE(pStruct);
	if (!pStruct->IsValid())
	{
		return FALSE;
	}

	SetEditable(pStruct->m_bEditEnable);
	EnableTitleTips(FALSE);

	SetRowResize(pStruct->m_bEnableRowResize);
	SetColumnResize(pStruct->m_bEnableColResize);

	SetSingleRowSelection(pStruct->m_bSingleRowSelection);
	SetSingleColSelection(pStruct->m_bSingleColomnSelection);

	SetFixedColumnSelection(TRUE);
	SetFixedRowSelection(TRUE);

	///不设一下字体的话，在有些机器默认字体不对的情况下会显示得很怪
	{
		CGridCellBase* pCellBase = GetDefaultCell(FALSE, FALSE);
		LOGFONT* pFont = pCellBase->GetFont();
		_tcscpy(pFont->lfFaceName, _T("Arial"));
		pFont->lfHeight =pStruct->m_nDefRowHeight -2;
		pCellBase->SetFont(pFont);

		pCellBase = GetDefaultCell(FALSE, TRUE);
		pFont = pCellBase->GetFont();
		_tcscpy(pFont->lfFaceName, _T("Arial"));
		pFont->lfHeight =pStruct->m_nDefRowHeight -2;
		pCellBase->SetFont(pFont);

		pCellBase = GetDefaultCell(TRUE, FALSE);
		pFont = pCellBase->GetFont();
		_tcscpy(pFont->lfFaceName, _T("Arial"));
		pFont->lfHeight =pStruct->m_nDefRowHeight -2;
		pCellBase->SetFont(pFont);

		pCellBase = GetDefaultCell(TRUE, TRUE);
		pFont = pCellBase->GetFont();
		_tcscpy(pFont->lfFaceName, _T("Arial"));
		pFont->lfHeight =pStruct->m_nDefRowHeight -3;
		pCellBase->SetFont(pFont);
	}

	_ASSERTE(pStruct->IsValid());
	SetDefCellWidth(pStruct->m_nDefColumnWidth);
	SetDefCellHeight(pStruct->m_nDefRowHeight);
	SetFixedRowCount(pStruct->m_nFixedRowCount);
	SetFixedColumnCount(pStruct->m_nFixedColumnCount);
	SetRowCount(pStruct->m_nRowCount);
	SetColumnCount(pStruct->m_nColumnCount);

	m_bEnableHighLightFixed = pStruct->m_bEnableHightLighFixed;



	if (pStruct->m_bInitBasicTitles)
	{
		///初始化标题数据
		TCHAR tczTemp[32];
		int nColumnCount = GetColumnCount();
		for(int i= pStruct->m_nFixedColumnCount; i < nColumnCount; i++)
		{
			int nRet = _itot_s(i, tczTemp, 10);
			if (nRet == 0)
			{
				SetItemText(0, i, tczTemp);
			}
		}
		int nRowCount = GetRowCount();
		for (int i = pStruct->m_nFixedRowCount; i < nRowCount; i++)
		{
			int nRet = _itot_s(i, tczTemp, 10);
			if (nRet == 0)
			{
				SetItemText(i, 0, tczTemp);
				SetItemData(i, 0, (LPARAM)i);
			}
		}
		if (nRowCount > 0 && nColumnCount > 0)
		{
			SetItemText(0, 0, (LPCTSTR)pStruct->m_csCornerDescription);
			for (int i = 0; i < GetFixedColumnCount(); i++)
			{
				SetColumnWidth(i, pStruct->m_nFixedColumnWidth);
			}
		}
	}//end if


	Refresh();
	if(IsUseExternalScrollBar())
		this->ShowScrollBar(SB_BOTH, FALSE);//<ScrollBar>
	return TRUE;
}
INT KSceneSceneGridCtrl::GetSelectedMinRow()
{
	if (GetSelectedCount() > 0)
	{
		CCellRange tempRange = GetSelectedCellRange();
		return tempRange.GetMinRow();
	}
	return -1;
}
HRESULT KSceneSceneGridCtrl::GetSelectedMinRowHeadData(LPARAM* pData)
{
	INT nMinRowSelected = GetSelectedMinRow();
	KG_PROCESS_ERROR(pData);
	KG_PROCESS_ERROR(nMinRowSelected > 0 && GetColumnCount() > 0);	
	*pData = GetItemData(nMinRowSelected, 0);
	return S_OK;
Exit0:
	return E_FAIL;
}
BOOL KSceneSceneGridCtrl::AddRow(LPCTSTR tczHead, BOOL bSetHeadData, LPARAM lparamHead, INT nRow /*= -2*/)///如果取默认值就自动得到当前选择的
{
	int nSelectedMinRow = nRow;
	if (nSelectedMinRow == -2)
	{
		nSelectedMinRow = GetSelectedMinRow();
	}
	CString csTemp = _T("");
	if (tczHead)
	{
		csTemp = tczHead;
	}
	int nRet = InsertRow(csTemp, nSelectedMinRow);
	if (nRet >= 0 && GetColumnCount() > 0)
	{
		SetItemData(nRet, 0, lparamHead);
	}
	Refresh();
	return nRet >= 0 ?TRUE : FALSE;
}
BOOL KSceneSceneGridCtrl::AddRow(INT nRow /*= -2*/)///如果取默认值就自动得到当前选择的
{
	int nSelectedMinRow = nRow;
	if (nSelectedMinRow == -2)
	{
		nSelectedMinRow = GetSelectedMinRow();
	}
	int nRet = InsertRow(_T(""), nSelectedMinRow);	
	Refresh();
	return nRet >= 0 ?TRUE : FALSE;
}
BOOL KSceneSceneGridCtrl::DelRow(INT nRow/* = -2*/)
{
	int nSelectedMinRow = nRow;
	if (nSelectedMinRow == -2)
	{
		nSelectedMinRow = GetSelectedMinRow();
	}
	BOOL bRet = DeleteRow(nSelectedMinRow);
	Refresh();
	return bRet;
}
BOOL KSceneSceneGridCtrl::ClearRow( INT nRow /*= -2*/, BOOL ClearFixed /*FALSE*/ )
{
	int nSelectedMinRow = nRow;
	if (nSelectedMinRow == -2)
	{
		nSelectedMinRow = GetSelectedMinRow();
	}
	if (nSelectedMinRow < 0 || nSelectedMinRow >= GetRowCount())
	{
		return FALSE;
	}
	int nColumnCount = GetColumnCount();
	int nClearBegin = ClearFixed ? 0 : GetFixedColumnCount();
	for (int i = nClearBegin; i < nColumnCount; i++)
	{
		SetItemText(nRow, i, _T(""));
		SetItemData(nRow, i, NULL);
		MarkCell(nRow, i, FALSE);
	}
	return TRUE;
}
BOOL KSceneSceneGridCtrl::MoveRow(BOOL bUpOrDown, INT nRow/* = -2*/)
{
	int nSelectedMinRow = nRow;
	if (nSelectedMinRow == -2)
	{
		nSelectedMinRow = GetSelectedMinRow();
	}
	///排除边界Row，设定差增量
	CCellRange tempRange = GetSelectedCellRange();
	int nRowDiff = 0;

	if (nSelectedMinRow < 0 || nSelectedMinRow >= GetRowCount())
	{
		return FALSE;
	}
	if(bUpOrDown)///Up
	{
		if (nSelectedMinRow <= GetFixedRowCount())///选到了Fixed下面那个
		{
			return TRUE;
		}
		nRowDiff = -1;
	}
	else///Down
	{
		if (nSelectedMinRow >= GetRowCount() - 1)
		{
			return TRUE;
		}
		nRowDiff = 1;
	}

	///插入一个临时的行用于数据交换
	int nFirstUnFixedColumn = GetFixedColumnCount();
	int nRowCount = GetRowCount();
	InsertRow(_T(""), -1);
	int nRowCoutAfterTempInsert = GetRowCount();


	if (nRowCoutAfterTempInsert == nRowCount + 1)///插入成功
	{
		int nColumnCount = GetColumnCount();
		for (int i = nFirstUnFixedColumn; i < nColumnCount; i++)
		{
			CGridCellBase* pTempSelCell = GetCell(nSelectedMinRow, i);
			CGridCellBase* pTempUpCell = GetCell(nSelectedMinRow + nRowDiff, i);
			CGridCellBase* pTempCell = GetCell(nRowCoutAfterTempInsert - 1, i);
			_ASSERTE(pTempCell && pTempSelCell && pTempUpCell);
			*pTempCell = *pTempUpCell;
			*pTempUpCell = *pTempSelCell;
			*pTempSelCell = *pTempCell;
		}		
		DeleteRow(nRowCoutAfterTempInsert - 1);
		SetSelectedRange(tempRange.GetMinRow() + nRowDiff
			, tempRange.GetMinCol()
			, tempRange.GetMaxRow()  + nRowDiff
			, tempRange.GetMaxCol());
		PostMessage(WM_VSCROLL, bUpOrDown ? SB_LINEUP : SB_LINEDOWN, NULL);
		Refresh();
		return TRUE;
	}
	return FALSE;
}
BOOL KSceneSceneGridCtrl::UpRow(INT nRow/* = -2*/)
{
	return MoveRow(TRUE, nRow);
}
BOOL KSceneSceneGridCtrl::DownRow(INT nRow/* = -2*/)
{
	return MoveRow(FALSE, nRow);
}

BOOL KSceneSceneGridCtrl::ValidateEdit( int nRow, int nCol, LPCTSTR str)
{
	KG_PROCESS_ERROR(str);
	{
		CPoint TempPoint(nRow, nCol);
		//KG_PROCESS_ERROR(this->CallbackCall<CALLBACK_VALIDATE_EDIT>(TempPoint, (DWORD_PTR)str));

		if(NULL != m_pEventHandler)
		{
			BOOL bRet = m_pEventHandler->IsEditStringValid(*this, TempPoint, str);
			KG_PROCESS_ERROR(bRet);
		}
		return CGridCtrl::ValidateEdit(nRow, nCol, str);
	}
Exit0:
	return FALSE;
}

void KSceneSceneGridCtrl::OnEditPaste()
{
	CCellRange Selection = GetSelectedCellRange();
	/*if(CallbackCall<CALLBACK_PASTE>(CPoint(Selection.GetMinRow(), Selection.GetMinCol()), NULL))
	CGridCtrl::OnEditPaste();	*/

	if(NULL != m_pEventHandler)
	{
		BOOL bRet = m_pEventHandler->IsPasteValid(*this, CPoint(Selection.GetMinRow(), Selection.GetMinCol()));
		if(! bRet)
			return;
	}

	CGridCtrl::OnEditPaste();
}
void KSceneSceneGridCtrl::OnEditCut()
{
	CCellRange Selection = GetSelectedCellRange();
	/*if(CallbackCall<CALLBACK_CUT>(CPoint(Selection.GetMinRow(), Selection.GetMinCol()), NULL))
	CGridCtrl::OnEditCut();	*/

	if(NULL != m_pEventHandler)
	{
		BOOL bRet = m_pEventHandler->IsCutValid(*this, CPoint(Selection.GetMinRow(), Selection.GetMinCol()));
		if(! bRet)
			return;
	}
	CGridCtrl::OnEditCut();
}
void KSceneSceneGridCtrl::OnEditCopy()
{
	CCellRange Selection = GetSelectedCellRange();
	/*if(CallbackCall<CALLBACK_COPY>(CPoint(Selection.GetMinRow(), Selection.GetMinCol()), NULL))
	CGridCtrl::OnEditCopy();*/

	if(NULL != m_pEventHandler)
	{
		BOOL bRet = m_pEventHandler->IsCopyValid(*this, CPoint(Selection.GetMinRow(), Selection.GetMinCol()));
		if(! bRet)
			return;
	}

	CGridCtrl::OnEditCopy();
}
void KSceneSceneGridCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CCellID CellID = this->GetCellFromPt(point);
	if (this->IsValid(CellID) && NULL != m_pEventHandler)
	{
		/*DWORD_PTR dwRet = this->CallbackCall<CALLBACK_DL_CLICKED>(CPoint(CellID.row, CellID.col), NULL);
		if (!dwRet)
		{
		return;
		}*/

		BOOL bRet = m_pEventHandler->IsLButtomDoubleClickValid(*this, CPoint(CellID.row, CellID.col));
		if(! bRet)
			return;
	}
	CGridCtrl::OnLButtonDblClk(nFlags, point);
}
void KSceneSceneGridCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	CCellID CellID = this->GetCellFromPt(point);
	if (this->IsValid(CellID) && NULL != m_pEventHandler)
	{
		/*DWORD_PTR dwRet = this->CallbackCall<CALLBACK_L_DOWN>(CPoint(CellID.row, CellID.col), NULL);
		if (!dwRet)
		{
		return;
		}*/

		BOOL bRet = m_pEventHandler->IsLButtonDownValid(*this, CPoint(CellID.row, CellID.col));
		if(! bRet)
			return;
	}
	CGridCtrl::OnLButtonDown(nFlags, point);
}
void KSceneSceneGridCtrl::OnEditCell( int nRow, int nCol, CPoint point, UINT nChar )
{
	//StructOnEdit TempOnEditStruct;
	//TempOnEditStruct.m_CPoint = point;
	//TempOnEditStruct.m_uChar = nChar;
	//DWORD_PTR dwRet = this->CallbackCall<CALLBACK_ON_EDIT>(CPoint(nRow, nCol), (DWORD_PTR)&TempOnEditStruct);
	/*if(!dwRet)
	return;*/

	if (NULL != m_pEventHandler)
	{
		BOOL bRet = m_pEventHandler->IsEditCharValid(*this, CPoint(nRow, nCol), nChar);
		if (! bRet)
		{
			return;
		}
	}
	return CGridCtrl::OnEditCell(nRow, nCol, point, nChar);
}
void KSceneSceneGridCtrl::HighLightRow( INT nRow, BOOL bHighLight /*= TRUE*/)
{
	INT nRowCount = GetRowCount();
	INT nColumnCount = GetColumnCount();
	INT nRowBegin = m_bEnableHighLightFixed ? 0 : GetFixedRowCount();
	INT nHightLightBegin = m_bEnableHighLightFixed? 0 : GetFixedColumnCount();
	if (nRow < nRowBegin || nRow >= nRowCount)
	{
		return;
	}
	if (bHighLight)
	{
		UnHighLightAllRow();

		for (int i = nHightLightBegin; i < nColumnCount; i++)
		{
			HighLightCell(nRow, i);
		}
		m_nLastHighLightRow = nRow;
		RedrawRow(nRow);
		return;
	}
	else
	{
		for (int i = nHightLightBegin; i< nColumnCount; i++)
		{
			HighLightCell(nRow, i, FALSE);
		}
		RedrawRow(nRow);
		if (nRow == m_nLastHighLightRow)
		{
			m_nLastHighLightRow = -1;
		}

	}
}

void KSceneSceneGridCtrl::HighLightColumn( INT nColumn, BOOL bHighLight /*= TRUE*/)
{
	INT nColumnCount = GetColumnCount();
	INT nHightLightBegin = m_bEnableHighLightFixed? 0 : GetFixedRowCount();
	INT nColumnBegin = m_bEnableHighLightFixed? 0 : GetFixedColumnCount();
	INT nRowCount = GetRowCount();
	if (nColumn < nColumnBegin || nColumn >= nColumnCount)
	{
		return;
	}

	if (bHighLight)
	{
		UnHighLightAllColumn();

		for (int i = nHightLightBegin; i < nRowCount; i++)
		{
			HighLightCell(i, nColumn);
		}
		m_nLastHighLightColumn = nColumn;
		RedrawColumn(nColumn);
		return;
	}
	else
	{
		for (int i = nHightLightBegin; i< nRowCount; i++)
		{
			HighLightCell(i, nColumn, FALSE);

		}
		RedrawColumn(nColumn);
		if (nColumn == m_nLastHighLightColumn)
		{
			m_nLastHighLightColumn = -1;
		}	
	}
}

void KSceneSceneGridCtrl::MarkCell( INT nRow, INT nColumn, BOOL bEnable /*= TRUE*/ )
{
	COLORREF color = GetItemBkColour(nRow, nColumn);
	if (bEnable)
	{
		if (color == GetMarkColor() || color == GetHighLightAndMarkColor())
		{
			return;
		}
		if (color == GetHighLightColor())
		{
			SetItemBkColour(nRow, nColumn, GetHighLightAndMarkColor());
			return;
		}
		SetItemBkColour(nRow, nColumn, GetMarkColor());
	}
	else
	{
		if (color == GetHighLightAndMarkColor())
		{
			SetItemBkColour(nRow, nColumn, GetHighLightColor());
			return;
		}
		if (color == GetHighLightColor())
		{
			return;
		}
		if (color == GetMarkColor())
		{
			SetItemBkColour(nRow, nColumn, CLR_DEFAULT);
			return;
		}
		//SetItemBkColour(nRow, nColumn, CLR_DEFAULT);
	}
}

void KSceneSceneGridCtrl::HighLightCell( INT nRow, INT nColumn, BOOL bEnable /*= TRUE*/ )
{
	///如果不这样的话，很容易会因为溢出等原因造成颜色错误，所以就写得喔挫一点避免溢出检查
	COLORREF color = GetItemBkColour(nRow, nColumn);
	if (bEnable)
	{
		if (color == CLR_DEFAULT)
		{
			color = GetHighLightColor();
		}
		else if (color == GetMarkColor())
		{
			color = GetHighLightAndMarkColor();
		}
	}
	else
	{
		if (color == GetHighLightColor())
		{
			color = CLR_DEFAULT;
		}
		else if (color == GetHighLightAndMarkColor())
		{
			color = GetMarkColor();
		}
	}
	SetItemBkColour(nRow, nColumn, color);
}

void KSceneSceneGridCtrl::UnHighLightAllRow()
{
	HighLightRow(m_nLastHighLightRow, FALSE);
}

void KSceneSceneGridCtrl::UnHighLightAllColumn()
{
	HighLightColumn(m_nLastHighLightColumn, FALSE);
}
INT KSceneSceneGridCtrl::FindRowWhichsDataEqualsGiven(LPARAM lData
													  , INT nBegin/* = -1*/
													  , BOOL FindFixed/* = FALSE*/
													  , INT nWhichColumn /*= 0*/
													  , INT nWhichFinded /*= 1*/)
{
	INT nStartPos = nBegin;
	if(nStartPos < 0)
		nStartPos = 0;
	if(!FindFixed)
		nStartPos = (nStartPos < GetFixedRowCount()) ? GetFixedRowCount() : nStartPos;
	INT nMatchedRet = 0;
	for (int i = nStartPos; i < GetRowCount(); i++)
	{
		if(lData == GetItemData(i, nWhichColumn))
		{
			if((++nMatchedRet) == nWhichFinded)
				return i;
		}
	}
	return -1;
}
//<ScrollBar>
void KSceneSceneGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if(IsUseExternalScrollBar())
	{
		switch(nSBCode)
		{
		case SB_LINEUP:
		case SB_LINEDOWN:
		case SB_PAGEDOWN:
		case SB_PAGEUP:
			{
				///CGridCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
				break;
			}
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			{
				SCROLLINFO tempInfoThis;
				tempInfoThis.cbSize = sizeof(SCROLLINFO);
				tempInfoThis.nTrackPos = nPos;
				tempInfoThis.nPos = nPos;
				tempInfoThis.fMask = SIF_POS | SIF_TRACKPOS;
				this->SetScrollInfo(SB_VERT, &tempInfoThis, FALSE);
			}
			break;
		}

		CGridCtrl::OnVScroll(nSBCode, nPos, NULL);

		this->ShowScrollBar(SB_VERT, FALSE);
	}
	else
	{
		CGridCtrl::OnVScroll(nSBCode, nPos, NULL);
	}

}

void KSceneSceneGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (IsUseExternalScrollBar())
	{
		switch(nSBCode)
		{
		case SB_LINELEFT:
		case SB_LINERIGHT:
		case SB_PAGELEFT:
		case SB_PAGERIGHT:
		case SB_THUMBTRACK:
			{
				SCROLLINFO tempInfoThis;
				tempInfoThis.cbSize = sizeof(SCROLLINFO);
				tempInfoThis.nTrackPos = nPos;
				tempInfoThis.nPos = nPos;
				tempInfoThis.fMask = SIF_POS | SIF_TRACKPOS;
				this->SetScrollInfo(SB_HORZ, &tempInfoThis, FALSE);
			}
			break;
		}

		CGridCtrl::OnHScroll(nSBCode, nPos, NULL);
		this->ShowScrollBar(SB_HORZ, FALSE);
	}
	else
	{
		CGridCtrl::OnHScroll(nSBCode, nPos, NULL);
	}

}

void KSceneSceneGridCtrl::ResetScrollBars()
{
	CGridCtrl::ResetScrollBars();
	if (IsUseExternalScrollBar())
	{
		if (m_pScrollBarHorz)
		{
			SCROLLINFO SI;
			this->GetScrollInfo(SB_HORZ, &SI);
			m_pScrollBarHorz->SetScrollInfo(&SI);
		}
		this->ShowScrollBar(SB_VERT, FALSE);
		this->ShowScrollBar(SB_HORZ, FALSE);
	}
	return;
}

BOOL KSceneSceneGridCtrl::BindHScrollBar( CScrollBar* pScrollBar )
{
	if (IsUseExternalScrollBar())
	{
		if (!m_pScrollBarHorz && pScrollBar)
		{
			m_pScrollBarHorz = pScrollBar;
			SCROLLINFO SI;
			this->GetScrollInfo(SB_VERT, &SI);
			m_pScrollBarHorz->SetScrollInfo(&SI);
			return TRUE;
		}
		else if (m_pScrollBarHorz && !pScrollBar)
		{
			m_pScrollBarHorz = NULL;
			return TRUE;
		}
	}
	return FALSE;
}
//</ScrollBar>


