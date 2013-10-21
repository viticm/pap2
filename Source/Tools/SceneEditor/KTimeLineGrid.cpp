#include "StdAfx.h"
#include "ktimelinegrid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//
//
//
//KTimeLineGrid::~KTimeLineGrid(void)
//{
//}
//
///*
//*@brief 显示网格
//*@param pDC 窗口的DC
//*@param pRect 需要重画的区域
//*/
//void KTimeLineGrid::DrawGrid(CDC* pDC, RECT* pRect /* = NULL*/)
//{
//	TCHAR strValue[255];
//	CPen Pen(PS_SOLID, 1, RGB(0, 0, 0));
//    CPen PenGray(PS_SOLID, 1, RGB(192, 192, 192));
//	pDC->SetBkColor(pDC->GetPixel(2, 2));
//	float fScaleX = 1.0f;
//	float fScaleY = 1.0f;
//	float fStepX = 10.0f * m_fHScale;
//	float fStepY = 10.0f * m_fVScale;
//	while ( fStepX < 10.0f)
//	{
//		fStepX *= 10.0f;
//		fScaleX *= 10.0f;
//	}
//	while( fStepY < 10.0f )
//	{
//		fStepY *= 10.0f;
//		fScaleY *= 10.0f;
//	}
//	float i;
//
//	HGDIOBJ pOldPen = pDC->SelectObject(&Pen);
//	pDC->MoveTo(0, m_ptOrg.y);
//	pDC->LineTo(m_ptSize.x, m_ptOrg.y);
//	pDC->MoveTo(m_ptOrg.x, 0);
//	pDC->LineTo(m_ptOrg.x, m_ptSize.y);
//
//	//Draw horizontal lines
//	pDC->SelectObject(&PenGray);
//	//int nStepY = static_cast<int>(fStepY);
//	//int nStepX = static_cast<int>(fStepX);
//	for( i = m_ptOrg.y - fStepY; i > 0; i -= fStepY )
//	{
//		pDC->MoveTo(0, i);
//		pDC->LineTo(m_ptSize.x, i);
//		sprintf(strValue, "%.0f", (m_ptOrg.y - i) / fStepY * 10 * fScaleY);
//		pDC->TextOut(m_ptOrg.x + 2, i + 2, strValue);
//	}
//
//	for( i = m_ptOrg.y + fStepY; i < m_ptSize.y; i += fStepY )
//	{
//		pDC->MoveTo(0, i);
//		pDC->LineTo(m_ptSize.x, i);
//		sprintf(strValue, "%.0f", (m_ptOrg.y - i) / fStepY * 10 * fScaleY);
//		pDC->TextOut(m_ptOrg.x + 2, i + 2, strValue);
//	}
//
//	//Draw vertical lines
//	for( i = m_ptOrg.x - fStepX; i > 0; i -= fStepX )
//	{
//		pDC->MoveTo(i, 0);
//		pDC->LineTo(i, m_ptSize.y);
//		sprintf(strValue, "%.0f", (i - m_ptOrg.x) / fStepX * 10 * fScaleX);
//		pDC->TextOut(i + 2, m_ptOrg.y + 2, strValue);
//	}
//
//	for( i = m_ptOrg.x + fStepX; i < m_ptSize.x; i += fStepX )
//	{
//		pDC->MoveTo(i, 0);
//		pDC->LineTo(i, m_ptSize.y);
//		sprintf(strValue, "%.0f", (i - m_ptOrg.x) / fStepX * 10 * fScaleX);
//		pDC->TextOut(i + 2, m_ptOrg.y + 2, strValue);
//	}
//
//
//	pDC->SelectObject(pOldPen);
//}
