#include "stdafx.h"
#include ".\ktimepointcolor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//namespace ModelSFX
//{
////#define GetAlpha(color) color>>24
////#define GetRed(color)   (color << 8) >> 24
////#define GetGreen(color) (color << 16) >> 24
////#define GetBlue(color) (color << 24 ) >> 24
//
//KTimePointColor::KTimePointColor(void)
//{
//
//	TimePointColor tpc1 = { 0xffff0000, 0.0f};
//	TimePointColor tpc2 = { 0xff00ff00, 0.7f};
//	TimePointColor tpc3 = { 0x7f0000ff, 0.5f};
//	TimePointColor tpc4 = { 0x4f00ff00, 1.0f};
//	m_vecTP.push_back(tpc1);
//	m_vecTP.push_back(tpc3);
//	//m_vecTP.push_back(tpc2);
//	m_vecTP.push_back(tpc4);
//}
//
//KTimePointColor::~KTimePointColor(void)
//{
//	m_vecTP.clear();
//}
//
//COLORREF KTimePointColor::GetValue(float fTime)
//{
//	if ( fTime < 0.0f || fTime > 1.0f )
//		return 0xffffffff;
//	int iSize = Size() - 1;
//	if (fTime == 0.0f)
//		return m_vecTP[0].crColor;
//	if (fTime == 1.0f)
//		return m_vecTP[iSize].crColor;
//	int i = 0;
//	while ( i < iSize && m_vecTP[i].fTime < fTime )
//		i++;
//	BYTE *c1 = (BYTE*)&m_vecTP[i-1].crColor;
//	BYTE *c2 = (BYTE*)&m_vecTP[i].crColor;
//	COLORREF color;
//	BYTE* c3 = (BYTE*)&color;
//	float fRation = (fTime - m_vecTP[i-1].fTime) / (m_vecTP[i].fTime - m_vecTP[i-1].fTime);
//	//float fReverse = 1.0f - fRation;
//	c3[0] = (BYTE)(fRation * (c2[0] - c1[0]) + c1[0]);
//	c3[1] = (BYTE)(fRation * (c2[1] - c1[1]) + c1[1]);
//	c3[2] = (BYTE)(fRation * (c2[2] - c1[2]) + c1[2]);
//	c3[3] = (BYTE)(fRation * (c2[3] - c1[3]) + c1[3]);
//	return color;
//}
//
//
//
//void KTimePointColor::Insert(TimePointColor& tpColor)
//{
//	vector<TimePointColor>::iterator it = m_vecTP.begin();
//	while ( it != m_vecTP.end() && it->fTime < tpColor.fTime) it++;
//	if ( it == m_vecTP.end() )
//	{
//		m_vecTP.push_back(tpColor);
//	}
//	else if ( it->fTime == tpColor.fTime )
//	{
//		*it = tpColor;
//	}
//	else
//	{
//		m_vecTP.insert(it, tpColor);
//	}
//}
//
//
//
//void KTimePointColor::Remove(int iIndex)
//{
//	if ( iIndex > Size() - 1 )
//		return;
//	vector<TimePointColor>::iterator it = m_vecTP.begin();
//	advance(it, iIndex);
//	m_vecTP.erase(it);
//}
//
//
//
//void KTimePointColor::Reset(void)
//{
//	m_vecTP.clear();
//	KTimePointColor();
//}
//
//}