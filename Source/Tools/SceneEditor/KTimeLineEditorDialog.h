#pragma once
//#include <map>
//#include <string>
//#include "KTimeLine4Editor.h"
//#include "ktimelineeditorwindow.h"
//
//
//using namespace std;
//
//#define MAX_SELECTION 40
//#define MAX_TIMELINE_NAME 255
//
//// KTimeLineEditorDialog dialog
//class KTimeLineEditorDialog : public CDialog
//{
//	DECLARE_DYNAMIC(KTimeLineEditorDialog)
//	enum 
//	{
//		STATUS_BEGIN = 0,
//		STATUS_MOVE = 0,
//		STATUS_INSERT,
//		STATUS_ADD,
//		//Add more here
//		STATUS_COUNT,
//	};
//public:
//	KTimeLineEditorDialog(CWnd* pParent = NULL);   // standard constructor
//	virtual ~KTimeLineEditorDialog();
//	map<string, KTimeLine4Editor<float, float>*>* GetTimeLines() { return &m_TimeLines; };
//	void AddTimeLine(LPCSTR strName, KTimeLine4Editor<float, float>* pTimeLine);
//// Dialog Data
//	enum { IDD = IDD_DIALOG_TIMELINE };
//private:
//	void OnActionStatusChange(int nStatus);
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	virtual void OnOK();
//	virtual void PostNcDestroy();
//	virtual void OnCancel();
//	DECLARE_MESSAGE_MAP()
//private:
//	int m_nAction;
//	BOOL m_bInterpolated;
//	map<string, KTimeLine4Editor<float, float>*> m_TimeLines;
//	KTimeLineEditorWindow* m_pwndEditor;
//public:
//	float m_fTime;
//	float m_fValue;
//	virtual BOOL OnInitDialog();
//	afx_msg void OnBnClickedRadioInsert();
//	afx_msg void OnBnClickedRadioAdd();
//	afx_msg void OnBnClickedRadioMove();
//	afx_msg void OnLbnSelchangeListTimelines();
//	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
//protected:
//	
//public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
//protected:
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
//public:
//	afx_msg void OnBnClickedButtonSave();
//	afx_msg void OnBnClickedButtonLoad();
//	afx_msg void OnBnClickedCheckInp();
//};
