#pragma once
//#include "KTimeLine4Editor.h"
//#include "KTimeLineGrid.h"
//#include <map>
//using namespace std;
//
//#define IDD_WINDOW_TIMELINEEDITOR 8760
//#define WM_STATUS_CHANGE WM_USER + 1000 //状态变化消息
//#define WM_TIMELINE_CHANGE WM_USER + 10001 //Timeline选择变化消息
//#define WM_UPDATEVALUE WM_USER + 1002
//
//class KTimeLineEditorWindow:public CWnd
//{
//public:
//	KTimeLineEditorWindow(void);
//	~KTimeLineEditorWindow(void);
//	BOOL Create(RECT& rectWindow, CWnd* pParent);
//	POINT GetOrg() { return m_ptOrg; }
//	float GetScale(enumScale Type) { return (Type == ENUM_SCALE_HORIZONTAL ? m_fHScale : m_fVScale); }
//
//protected:
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
//private:
//	inline COLORREF GetTimeLineColor(const string& strTimeLineName);
//	void DrawTimeLine(CDC* pDC, const string& strTimeLineName);
//	//void TimePointAction(m_nAction, point);
//	int m_nAction;
//	KTimeLineGrid* m_pGrid;
//	CListBox* m_pTimeLineListBox;
//	map<string, KTimeLine4Editor<float, float>*>* m_pTimeLines;
//	//vector<string> m_vecSelectTimeLines;
//	set<string> m_SelectTimeLines;
//	int m_nNumTimeLine;
//	RECT m_rectSelectedRegion;
//	int m_nSelected;//是否当前操作是否选择了关键点
//	int m_nActionSave;
//	BOOL m_bInsertPoint;	
//	//////////////////////////////////////////////////////////////////////////
//	//Operations 
//	POINT m_ptInsertPoint;
//	CFont m_Font;
//	POINT m_ptOrg;
//	float m_fVScale;
//	float m_fHScale;
//public:
//	BOOL SaveToFile(const TCHAR* szFileName, int nIndex = 0);
//	BOOL LoadFromFile(const TCHAR* szFileName, int nIndex = 0);
//	void GetSelectedTimeLines(KTimeLine4Editor<float, float>** &pSelected, int& nNum);
//	void Invalidate(BOOL bErase=TRUE);
//	DECLARE_MESSAGE_MAP()
//	afx_msg void OnPaint();
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
//	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
//
//};
