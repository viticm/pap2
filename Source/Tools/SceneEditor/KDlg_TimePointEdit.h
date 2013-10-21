#pragma once

//#include "timepoint.h"
//
//namespace PropertyBase
//{
//
//// KDlg_TimePointEdit dialog
//
//class KDlg_TimePointEdit : public CDialog
//{
//	DECLARE_DYNAMIC(KDlg_TimePointEdit)
//
//public:
//	KDlg_TimePointEdit(ModelSFX::KTimePoint* pTP, CWnd* pParent = NULL);   // standard constructor
//	virtual ~KDlg_TimePointEdit();
//
//	// Dialog Data
//	enum { IDD = IDD_TIMEPOINT_EDIT };
//	
//	struct Grid
//	{
//		CPoint ptOrigin;//逻辑上的原点在屏幕上的位置
//		CPoint ptMax;//最大值点在屏幕上的位置
//		float fMapX;//逻辑上的点和屏幕上的点的在X方向映射关系
//		float fMapY;//逻辑上的点和屏幕上的点的在Y方向映射关系
//	};
//	
//	//void SetStore(PropertyBase::_VarProperty* pProperty)
//	//{
//	//	m_lpProperty = pProperty;
//	//}
//	//
//	//void RefreshData()
//	//{
//	//	//if(m_lpProperty)
//	//	{
//	//		//m_lpStore->pStr = m_Value;
//	//		/*if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_STRING)
//	//		{
//	//		LPSTR pStr = (LPSTR) m_lpProperty->Address;
//	//		wsprintf(pStr,"%s",m_Value);
//	//		}
//	//		else if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_STLSTRING)
//	//		{
//	//		string* pStr = (string*) m_lpProperty->Address;
//	//		(*pStr) = m_Value;
//	//		}*/
//	//	}
//	//}
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	//PropertyBase::_VarProperty* m_lpProperty;
//	DECLARE_MESSAGE_MAP()
//	virtual BOOL OnInitDialog(void);
//	int IsSelectPoint(const POINT& pt);
//	void DrawLine(CDC* pdc);
//	void AddPoint(const POINT& pt);
//	void AddPoint(const CPoint& pt);
//	void DrawSelectionBox(CDC* pdc);
//	void NormalizePoint(CPoint& pt, const RECT& rect);
//	bool SelectRegion(RECT& Region, UINT nFlags);
//	inline bool IsPointInRect(RECT& rect, CPoint& point);
//	void DrawRect(CDC* pdc, RECT& rect);
//	void DrawGrid(CDC* pDc);
//	void ToTimePoint(void);
//
//private:
//	vector<CPoint> m_vecPointList;
//	int m_iValidateReg;//选择的时候有效的距离
//	int m_iPointCount;
//	int m_iCurrentNewPoint;
//	vector<int> m_vecSelectPointIndex;
//	//bool m_bSelectStatus;
//	CPoint m_ptLeftOrg;//保存鼠标按键未放开时的位移
//	CPoint m_ptRightOrg;
//	Grid m_GridCoordinate;
//	int m_iZoom;
//	RECT m_rectSelReg;
//	RECT m_rectClient;
//	RECT m_rectBound;
//	float m_fTime;
//	float m_fValue;
//	ModelSFX::KTimePoint* m_pTP;
//public:
//
//	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnPaint();
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnMove(int x, int y);
//	afx_msg void OnBnClickedButtonReset();
//
//	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnBnClickedButtonTotimepoint();
//	void DrawPointBox(CDC* pDc);
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//
//
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
//protected:
//	void ToPoint(void);
////	virtual void OnCancel();
////	virtual void PostNcDestroy();
//};
//
//};