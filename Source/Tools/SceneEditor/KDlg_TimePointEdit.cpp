// KDlg_TimePointEdit.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//#include "SceneEditor.h"
//#include "KDlg_TimePointEdit.h"
//#include "timepoint.h"
//
//
//namespace PropertyBase
//{
//
//// KDlg_TimePointEdit dialog
//
//IMPLEMENT_DYNAMIC(KDlg_TimePointEdit, CDialog)
//
//KDlg_TimePointEdit::KDlg_TimePointEdit(ModelSFX::KTimePoint* pTP, CWnd* pParent /*=NULL*/)
//: CDialog(KDlg_TimePointEdit::IDD, pParent)
//{
//	ASSERT(pTP);
//	m_pTP = pTP;
//	Create(IDD, pParent);
//}
//
//KDlg_TimePointEdit::~KDlg_TimePointEdit()
//{
//	m_vecPointList.clear();
//	m_vecSelectPointIndex.clear();
//}
//
//void KDlg_TimePointEdit::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_STATIC_TIME, m_fTime);
//	DDX_Text(pDX, IDC_STATIC_VALUE, m_fValue);
//}
//
//BOOL KDlg_TimePointEdit::OnInitDialog(void)
//{
//	RECT rectThis;
//	m_iValidateReg = 4;
//	memset(&m_ptRightOrg, 0, sizeof(POINT));
//	memset(&m_ptLeftOrg, 0, sizeof(POINT));
//	memset(&m_rectSelReg, -1, sizeof(RECT));
//	GetClientRect(&m_rectClient);
//	rectThis.bottom = 600;
//	rectThis.left = 100;
//	rectThis.right = 600;
//	rectThis.top = 10;
//	MoveWindow(&rectThis);
//	GetClientRect(&m_rectClient);
//	m_GridCoordinate.ptOrigin.y = m_rectClient.bottom - 40;
//	m_GridCoordinate.ptOrigin.x = m_rectClient.left + 40;
//	m_GridCoordinate.ptMax.x = m_rectClient.right - 40;
//	m_GridCoordinate.ptMax.y = m_rectClient.top + 40;
//	m_GridCoordinate.fMapX = 1.0f / abs(m_GridCoordinate.ptOrigin.x - m_GridCoordinate.ptMax.x);
//	m_GridCoordinate.fMapY = 10.0f ;
//	m_iPointCount = 0;
//	ToPoint();
//	return TRUE;
//}
//
//
//// KDlg_TimePointEdit message handlers
//BEGIN_MESSAGE_MAP(KDlg_TimePointEdit, CDialog)
//	ON_WM_PAINT()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONUP()
//	ON_WM_RBUTTONDOWN()
//	ON_WM_MBUTTONDOWN()
//	ON_WM_MBUTTONUP()
//	ON_WM_MOVE()
//	ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
//	ON_WM_SIZE()
//	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonTotimepoint)
//	ON_WM_KEYDOWN()
//END_MESSAGE_MAP()
//
//
//
//int KDlg_TimePointEdit::IsSelectPoint(const POINT& pt)
//{
//	int i;
//	for ( i = 0; i < m_iPointCount; i++ )
//	{
//		if ( (m_vecPointList[i].x - m_iValidateReg < pt.x) && (m_vecPointList[i].x + m_iValidateReg > pt.x))
//		{
//			if ( (m_vecPointList[i].y - m_iValidateReg < pt.y ) && (m_vecPointList[i].y + m_iValidateReg > pt.y))
//				return i;
//		}
//	}
//	return -1;
//}
//
//void KDlg_TimePointEdit::DrawLine(CDC* pdc)
//{
//	if ( m_iPointCount < 2 )
//		return;
//	int i;
//	pdc->MoveTo(m_vecPointList[0]);
//    for ( i = 1; i < m_iPointCount; i++ )
//		pdc->LineTo(m_vecPointList[i]);
//}
//
//void KDlg_TimePointEdit::AddPoint(const POINT& pt)
//{
//	m_iPointCount++;
//	if ( m_iPointCount == 1)
//	{
//		m_vecPointList.push_back(pt);
//		m_iCurrentNewPoint = static_cast<int>(m_vecPointList.size());
//		return;
//	}
//	vector<CPoint>::iterator it = m_vecPointList.begin();
//	m_iCurrentNewPoint = 0;
//	while( it->x < pt.x && it!= m_vecPointList.end())
//	{
//		it++;
//		m_iCurrentNewPoint++;
//	}
//	if ( it != m_vecPointList.end() )
//	{
//		m_vecPointList.insert(it, pt);
//	}
//	else
//	{
//		m_vecPointList.push_back(pt);
//	}
//}
//
//void KDlg_TimePointEdit::AddPoint(const CPoint& pt)
//{
//	m_iPointCount++;
//	if ( m_iPointCount == 1)
//	{
//		m_vecPointList.push_back(pt);
//		m_iCurrentNewPoint = static_cast<int>(m_vecPointList.size());
//		return;
//	}
//	vector<CPoint>::iterator it = m_vecPointList.begin();
//	m_iCurrentNewPoint = 0;
//	while( it->x < pt.x && it!= m_vecPointList.end())
//	{
//		it++;
//		m_iCurrentNewPoint++;
//	}
//	if ( it != m_vecPointList.end() )
//	{
//		m_vecPointList.insert(it, pt);
//	}
//	else
//	{
//		m_vecPointList.push_back(pt);
//	}
//}
//
//void KDlg_TimePointEdit::DrawSelectionBox(CDC* pdc)
//{
//	int iLen = static_cast<int>(m_vecSelectPointIndex.size());
//	const int iSize = 5;
//	CPen Red(PS_SOLID, 0, RGB(255, 0, 0));
//	CPen *pSave;
//	int i;
//	CPoint save = pdc->GetCurrentPosition();
//	pSave = pdc->SelectObject(&Red);
//	for ( i = 0; i < iLen; i++ )
//	{
//		int iIndex = m_vecSelectPointIndex[i];
//		CPoint leftup(m_vecPointList[iIndex].x - iSize, m_vecPointList[iIndex].y - iSize);
//		CPoint rightbt(m_vecPointList[iIndex].x + iSize, m_vecPointList[iIndex].y + iSize);
//		CPoint tmp1(rightbt.x, leftup.y);
//		CPoint tmp2(leftup.x, rightbt.y);
//		pdc->MoveTo(leftup);
//		pdc->LineTo(tmp1);
//		pdc->LineTo(rightbt);
//		pdc->LineTo(tmp2);
//		pdc->LineTo(leftup);
//	}
//	pdc->SelectObject(pSave);
//}
//
//
//void KDlg_TimePointEdit::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//	DrawLine(&dc);
//	DrawPointBox(&dc);
//	DrawSelectionBox(&dc);
//	DrawRect(&dc, m_rectSelReg);
//	DrawGrid(&dc);
//}
//
//void KDlg_TimePointEdit::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	SetCapture();
//	SetFocus();
//	int iIndex = IsSelectPoint(point);
//	m_ptLeftOrg = point;
//	//if ( iIndex != -1 )
//	//{
//	//	if ( nFlags & MK_CONTROL)//按下Ｃｔｒｌ才可以多选
//	//	{
//	//		if ( find(m_vecSelectPointIndex.begin(), m_vecSelectPointIndex.end(), iIndex) == m_vecSelectPointIndex.end())
//	//		{
//	//			m_vecSelectPointIndex.push_back(iIndex);
//	//			sort(m_vecSelectPointIndex.begin(), m_vecSelectPointIndex.end());
//	//		}
//	//	}
//	//	else
//	//	{
//	//		m_vecSelectPointIndex.clear();
//	//		m_vecSelectPointIndex.push_back(iIndex);
//	//	}
//	//}
//	//Invalidate();
//	CDialog::OnLButtonDown(nFlags, point);
//}
//
//void KDlg_TimePointEdit::OnMouseMove(UINT nFlags, CPoint point)
//{
//	bool bSelReg = true;
//	m_fTime = (point.x - m_GridCoordinate.ptOrigin.x) /414.0f ;
//	m_fValue = (m_GridCoordinate.ptOrigin.y - point.y) / 10.0f;
//	m_rectBound.bottom = m_GridCoordinate.ptOrigin.y;
//	m_rectBound.top = m_GridCoordinate.ptMax.y;
//	m_rectBound.left = m_GridCoordinate.ptOrigin.x;
//	m_rectBound.right = m_GridCoordinate.ptMax.x;
//	if ( nFlags & MK_LBUTTON)//没选中点，所以处理为对区域的选取
//	{
//		m_rectSelReg.left = min(point.x, m_ptLeftOrg.x);
//		m_rectSelReg.right = max(point.x, m_ptLeftOrg.x);
//		m_rectSelReg.bottom = max(point.y, m_ptLeftOrg.y);
//		m_rectSelReg.top = min(point.y, m_ptLeftOrg.y);
//		bSelReg = SelectRegion(m_rectSelReg, nFlags);
//		Invalidate();
//	}
//	else if ( nFlags & MK_RBUTTON )//处理移动
//	{
//		CPoint offset = point - m_ptRightOrg;
//		int iSize = static_cast<int>(m_vecSelectPointIndex.size());
//		int i;
//		if ( offset.x < 0 )
//		{
//			for ( i = 0; i < iSize; i++ )
//			{
//				int iIndex = m_vecSelectPointIndex[i];
//				m_vecPointList[iIndex] += offset;
//				if ( iIndex > 0 )
//				{
//					if (m_vecPointList[iIndex].x < m_vecPointList[iIndex - 1].x)
//					{
//						m_vecPointList[iIndex].x = m_vecPointList[iIndex - 1].x;
//					}
//				}
//				NormalizePoint(m_vecPointList[iIndex], m_rectBound);
//			}
//		}
//		else
//		{
//			for ( i = iSize - 1; i >= 0; i-- )
//			{
//				int iIndex = m_vecSelectPointIndex[i];
//				m_vecPointList[iIndex] += offset;
//				if ( iIndex < m_iPointCount - 1 )
//				{
//					if (m_vecPointList[iIndex].x > m_vecPointList[iIndex + 1].x)
//					{
//						m_vecPointList[iIndex].x = m_vecPointList[iIndex + 1].x;
//					}
//				}
//				NormalizePoint(m_vecPointList[iIndex], m_rectBound);
//			}
//		}
//		m_ptRightOrg = point;
//		Invalidate();
//	}
//	else if ( nFlags & MK_MBUTTON )
//	{
//		if ( m_iCurrentNewPoint != -1 )
//		{
//			m_vecPointList[m_iCurrentNewPoint] = point;
//			Invalidate();
//		}
//
//	}
//	UpdateData(FALSE);
//	CDialog::OnMouseMove(nFlags, point);
//}
//
//void KDlg_TimePointEdit::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	int isp = IsSelectPoint(point);
//	if ( isp == -1 && m_rectSelReg.bottom == -1 && !(nFlags & MK_CONTROL))
//	{
//		m_vecSelectPointIndex.clear();
//	}
//	memset(&m_rectSelReg, -1, sizeof(RECT));
//	ReleaseCapture();
//	Invalidate();
//	CDialog::OnLButtonUp(nFlags, point);
//}
//
//bool KDlg_TimePointEdit::SelectRegion(RECT& Region, UINT nFlags)
//{
//	int i;
//	bool bGetPoint = false;
//	//if ( nFlags & MK_CONTROL )//多选模式
//	//{
//	//	for ( i = 0; i < m_iPointCount; i++ )
//	//	{
//	//		if ( IsPointInRect(Region, m_vecPointList[i]) )
//	//		{
//	//			bGetPoint = true;
//	//			if ( find(m_vecSelectPointIndex.begin(), m_vecSelectPointIndex.end(), i) == m_vecSelectPointIndex.end())
//	//			{
//	//				//不在选择列表中
//	//				m_vecSelectPointIndex.push_back(i);
//	//				sort(m_vecSelectPointIndex.begin(), m_vecSelectPointIndex.end());
//	//			}
//	//		}
//	//	}
//	//}
//	//else
//	//{
//	//	//这种情况不需要排序，因为PointList是排了序的
//	//	m_vecSelectPointIndex.clear();
//	//	for ( i = 0; i < m_iPointCount; i++ )
//	//	{
//	//		if ( IsPointInRect(Region, m_vecPointList[i]) )
//	//		{
//	//			m_vecSelectPointIndex.push_back(i);
//	//			bGetPoint = true;
//	//		}
//	//	}
//	//}
//
//	return bGetPoint;
//}
//
//inline bool KDlg_TimePointEdit::IsPointInRect(RECT& rect, CPoint& point)
//{
//	int left = min(rect.left, rect.right);
//	int right = max(rect.right, rect.left);
//	int top = min(rect.top, rect.bottom);
//	int bt = max(rect.top, rect.bottom);
//	if ( point.x < right && point.x > left && point.y < bt && point.y > top )
//		return true;
//	return false;
//}
//
//void KDlg_TimePointEdit::DrawRect(CDC* pdc, RECT& rect)
//{
//	CPen Blue(PS_SOLID, 0, RGB(0, 0, 255));
//	CPen *pSave;
//	CPoint save = pdc->GetCurrentPosition();
//	pSave = pdc->SelectObject(&Blue);
//	CPoint p1(rect.left, rect.top);
//	CPoint p2(rect.right, rect.top);
//	CPoint p3(rect.right, rect.bottom);
//	CPoint p4(rect.left, rect.bottom);
//	pdc->MoveTo(p1);
//	pdc->LineTo(p2);
//	pdc->LineTo(p3);
//	pdc->LineTo(p4);
//	pdc->LineTo(p1);
//	pdc->SelectObject(pSave);
//}
//
//void KDlg_TimePointEdit::OnRButtonDown(UINT nFlags, CPoint point)
//{
//	m_ptRightOrg = point;
//	CDialog::OnRButtonDown(nFlags, point);
//}
//
//void KDlg_TimePointEdit::OnMButtonDown(UINT nFlags, CPoint point)
//{
//	SetFocus();
//	if ( point.x > m_GridCoordinate.ptOrigin.x && point.y < m_GridCoordinate.ptOrigin.y )
//	{
//		AddPoint(point);
//	}
//	m_vecSelectPointIndex.clear();
//	CDialog::OnMButtonDown(nFlags, point);
//	Invalidate();
//}
//
//
//
//void KDlg_TimePointEdit::OnMButtonUp(UINT nFlags, CPoint point)
//{
//	m_iCurrentNewPoint = -1;
//	CDialog::OnMButtonUp(nFlags, point);
//}
//
//void KDlg_TimePointEdit::NormalizePoint(CPoint& pt, const RECT& rect)
//{
//	if ( pt.x < rect.left ) pt.x = rect.left;
//		else if ( pt.x > rect.right ) pt.x = rect.right;
//	if ( pt.y <  rect.top ) pt.y = rect.top;
//		else if ( pt.y > rect.bottom ) pt.y = rect.bottom;
//}
//
//void KDlg_TimePointEdit::OnMove(int x, int y)
//{
//	CDialog::OnMove(x, y);
//	GetClientRect(&m_rectClient);
//}
//
//void KDlg_TimePointEdit::OnBnClickedButtonReset()
//{
//	m_vecPointList.clear();
//
//	CPoint ptStart(m_GridCoordinate.ptOrigin.x, m_GridCoordinate.ptOrigin.y - 10);
//	CPoint ptEnd(m_GridCoordinate.ptMax.x, m_GridCoordinate.ptOrigin.y - 10);
//	m_vecPointList.push_back(ptStart);
//	m_vecPointList.push_back(ptEnd);
//	m_vecSelectPointIndex.clear();
//	m_iPointCount = 2;
//	Invalidate();
//}
//
//void KDlg_TimePointEdit::DrawGrid(CDC* pDc)
//{
//	CPen PenCoord(PS_SOLID, 2, RGB(0, 0, 0));
//	CPen PenGrid(PS_SOLID, 1, RGB(128, 128, 128));
//	COLORREF crBack = RGB(192, 192, 192);
//	CPoint p1;
//	CPoint p2;
//	CPen* pPenSave;
//	CPoint ptSave;
//	char strCood[128];
//	pPenSave = pDc->SelectObject(&PenGrid);
//	ptSave = pDc->GetCurrentPosition();
//	pDc->SetBkColor(crBack);
//	int i, j;
//	int iMax = (m_GridCoordinate.ptMax.x - m_GridCoordinate.ptOrigin.x) / 10;
//	int jMax = (m_GridCoordinate.ptOrigin.y - m_GridCoordinate.ptMax.y) / 50;
//	for ( i = 1; i < 11/*iMax*/; i++ )
//	{
//		p1.x = i * iMax + m_GridCoordinate.ptOrigin.x;
//		p1.y = m_GridCoordinate.ptMax.y;
//		p2.x = p1.x;
//		p2.y = m_GridCoordinate.ptOrigin.y;
//		pDc->MoveTo(p1);
//		pDc->LineTo(p2);
//		sprintf(strCood, "%.1f", i / 10.0f );
//		pDc->TextOut(p1.x, m_GridCoordinate.ptOrigin.y, strCood);
//	}
//
//	RECT rect;
//	GetClientRect(&rect);
//
//	for ( j = 0 ; j < jMax; j++ )
//	{
//
//		p1.x = m_GridCoordinate.ptOrigin.x;
//		p1.y = - j * 50 + m_GridCoordinate.ptOrigin.y ;
//		p2.x = m_GridCoordinate.ptMax.x;
//		p2.y = p1.y;
//		pDc->MoveTo(p1);
//		pDc->LineTo(p2);
//		sprintf(strCood, "%d", j * 5);
//		pDc->TextOut(m_GridCoordinate.ptOrigin.x - 10, p1.y, strCood);
//	}
//
//	pDc->SelectObject(&PenCoord);
//	//坐标轴Y
//	if( m_GridCoordinate.ptOrigin.x > m_rectClient.left &&
//		m_GridCoordinate.ptOrigin.x < m_rectClient.right )
//	{
//
//		p1.x = m_GridCoordinate.ptOrigin.x;
//		p2.x = m_GridCoordinate.ptOrigin.x;
//		p1.y = m_rectClient.top;
//		p2.y = m_rectClient.bottom;
//		pDc->MoveTo(p1);
//		pDc->LineTo(p2);
//	}
//
//	//坐标轴X
//	if ( m_GridCoordinate.ptOrigin.y > m_rectClient.top &&
//		m_GridCoordinate.ptOrigin.y < m_rectClient.bottom )
//	{
//		p1.x = m_rectClient.left;
//		p2.x = m_rectClient.right;
//		p1.y = m_GridCoordinate.ptOrigin.y;
//		p2.y = m_GridCoordinate.ptOrigin.y;
//		pDc->MoveTo(p1);
//		pDc->LineTo(p2);
//
//	}
//	pDc->SelectObject(&pPenSave);
//	pDc->MoveTo(ptSave);
//
//}
//
//void KDlg_TimePointEdit::OnSize(UINT nType, int cx, int cy)
//{
//	CDialog::OnSize(nType, cx, cy);
//	GetClientRect(&m_rectClient);
//}
//
//void KDlg_TimePointEdit::ToTimePoint(void)
//{
//	if ( !m_iPointCount )
//		return;
//	if ( m_vecPointList[0].x != m_GridCoordinate.ptOrigin.x )
//	{
//		CPoint pt( m_GridCoordinate.ptOrigin.x,m_GridCoordinate.ptOrigin.y);
//		AddPoint(pt);
//	}
//	ModelSFX::KTimePoint* pTimePoint = m_pTP;//reinterpret_cast<ModelSFX::KTimePoint*>(m_lpProperty->Address);
//	pTimePoint->Reset();
//	float fMaxX = float(m_vecPointList[m_iPointCount-1].x - m_GridCoordinate.ptOrigin.x);
//	int i;
//	ModelSFX::TimePoint tmp;
//	int iLast = 0;
//	int iCur = 0;
//	for( i = 0; i <= m_iPointCount; i++)
//	{
//		while ( iCur <= m_iPointCount && m_vecPointList[iCur].x == m_vecPointList[iLast].x )//两个点的x坐标相同,表示在同一时刻出现了值跳变的情况
//			iCur++;
//		tmp.fTime = float(m_vecPointList[iLast].x - m_GridCoordinate.ptOrigin.x) * m_GridCoordinate.fMapX;
//		tmp.fLeftVal = (m_GridCoordinate.ptOrigin.y - m_vecPointList[iLast].y) / m_GridCoordinate.fMapY;
//		tmp.fRightVal = (m_GridCoordinate.ptOrigin.y - m_vecPointList[iCur-1].y) / m_GridCoordinate.fMapY;
//		pTimePoint->Insert(&tmp);
//		i = iCur;
//		iLast = i;
//	}
//	//pTimePoint->Output();
//}
//
//void KDlg_TimePointEdit::OnBnClickedButtonTotimepoint()
//{
//	ToTimePoint();
//}
//
//void KDlg_TimePointEdit::DrawPointBox(CDC* pDc)
//{
//	int iLen = static_cast<int>(m_vecPointList.size());
//	const int iSize = 5;
//	CPen Green(PS_SOLID, 0, RGB(0, 255, 0));
//	CPen *pSave;
//	int iIndex;
//	CPoint save = pDc->GetCurrentPosition();
//	pSave = pDc->SelectObject(&Green);
//	for ( iIndex = 0; iIndex < iLen; iIndex++ )
//	{
//		CPoint leftup(m_vecPointList[iIndex].x - iSize, m_vecPointList[iIndex].y - iSize);
//		CPoint rightbt(m_vecPointList[iIndex].x + iSize, m_vecPointList[iIndex].y + iSize);
//		CPoint tmp1(rightbt.x, leftup.y);
//		CPoint tmp2(leftup.x, rightbt.y);
//		pDc->MoveTo(leftup);
//		pDc->LineTo(tmp1);
//		pDc->LineTo(rightbt);
//		pDc->LineTo(tmp2);
//		pDc->LineTo(leftup);
//	}
//	pDc->SelectObject(pSave);
//}
//
//void KDlg_TimePointEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	switch (nChar)
//	{
//	case VK_DELETE:
//		{
//			int iSize = static_cast<int>(m_vecSelectPointIndex.size());
//            if (iSize)
//			{
//				int i;
//				vector<CPoint>::iterator it;
//				for ( i = iSize - 1; i >= 0  ; i-- )
//				{
//					it = m_vecPointList.begin();
//					advance(it, m_vecSelectPointIndex[i]);
//					m_vecPointList.erase(it);
//
//				}
//				m_iPointCount -= iSize;
//				m_vecSelectPointIndex.clear();
//			}
//			Invalidate();
//			break;
//		}
//	}
//
//	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
//}
//
//
//
//BOOL KDlg_TimePointEdit::PreTranslateMessage(MSG* pMsg)
//{
//	if ( pMsg->message == WM_KEYDOWN )
//	{
//		HWND w1 =this->GetSafeHwnd();
//		WindowProc(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
//		return true;
//	}
//	return CDialog::PreTranslateMessage(pMsg);
//}
//
//
//void KDlg_TimePointEdit::ToPoint(void)
//{
//	ModelSFX::KTimePoint* pTP = m_pTP;//reinterpret_cast<ModelSFX::KTimePoint*>(m_lpProperty->Address);
//	if ( m_iPointCount )
//	{
//		m_vecPointList.clear();
//		m_vecSelectPointIndex.clear();
//		m_iPointCount = 0;
//	}
//	int iSize = pTP->Size();
//	int i;
//	for ( i = 0; i < iSize; i++ )
//	{
//		ModelSFX::TimePoint& tp = pTP->m_vecTP[i];
//        if ( pTP->m_vecTP[i].fLeftVal == pTP->m_vecTP[i].fRightVal )
//		{
//			CPoint pt((int)(pTP->m_vecTP[i].fTime / m_GridCoordinate.fMapX + m_GridCoordinate.ptOrigin.x),
//				      (int)(m_GridCoordinate.ptOrigin.y - pTP->m_vecTP[i].fLeftVal * m_GridCoordinate.fMapY));
//			m_vecPointList.push_back(pt);
//			m_iPointCount++;
//		}
//		else
//		{
//			CPoint pt((int)(pTP->m_vecTP[i].fTime / m_GridCoordinate.fMapX + m_GridCoordinate.ptOrigin.x),
//				(int)(m_GridCoordinate.ptOrigin.y - pTP->m_vecTP[i].fLeftVal * m_GridCoordinate.fMapY));
//			m_vecPointList.push_back(pt);
//			pt.y = (int)(m_GridCoordinate.ptOrigin.y - pTP->m_vecTP[i].fRightVal * m_GridCoordinate.fMapY);
//			m_vecPointList.push_back(pt);
//			m_iPointCount += 2;
//		}
//	}
//
//}
//
//};
//
//
//
