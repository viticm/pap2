#pragma once
//#include "KTimeLine4Editor.h"
//#pragma warning(disable:4244)
//class KTimeLineGrid
//{
//public:
//	KTimeLineGrid(const POINT& WindowSize) { m_ptSize = WindowSize; m_ptOrg.x = 10;	m_ptOrg.y = 200; m_fHScale = 1.0f; m_fVScale = 1.0f; }
//	KTimeLineGrid(int nWidth, int nHeight) { m_ptSize.x = nWidth; m_ptSize.y = nHeight; m_ptOrg.x = 10;	m_ptOrg.y = 200; m_fHScale = 1.0f; m_fVScale = 1.0f; }
//	~KTimeLineGrid(void);
//	
//	/*
//	*@brief 显示网格
//	*@param 使用的DC
//	*@param 需要画的区域, NULL为全部重画
//	*/
//	void DrawGrid(CDC* pDC, RECT* pRect = NULL);
//		
//	/*
//	*@brief 设定当前的缩放比列
//	*@param nScale 比列
//	*/
//	void SetScale(float fScale, enumScale Type) { Type == ENUM_SCALE_HORIZONTAL ? m_fHScale = fScale : m_fVScale = fScale; }
//
//	/*
//	*@brief 得到当前的缩放比列
//	*/
//	float GetScale(enumScale Type) { return (Type == ENUM_SCALE_HORIZONTAL ? m_fHScale : m_fVScale); }
//
//	/*
//	*@brief 得到当前的原点
//	*/
//	POINT GetOrigin() { return m_ptOrg;	}
//
//	/*
//	*@brief 设定原点
//	*@param ptOrg新的原点
//	*/
//	void SetOrigin(POINT& ptOrg){ m_ptOrg = ptOrg; }
//
//private:
//	//////////////////////////////////////////////////////////////////////////
//	//私有数据
//	POINT m_ptOrg;//窗口左下角在逻辑空间中的位置
//	float m_fHSpan;//水平方向上的间距
//	float m_fVSpan;//垂直方向上的间距，实际显示时间距为m_nXSpan * m_nScale
//	float m_fHScale;//水平方向上的缩放系数
//	float m_fVScale;//竖直方向上的缩放系数
//	bool m_bRedraw;//标示是否需要重画
//	POINT m_ptSize;//窗口的尺寸
//};
