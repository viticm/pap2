#pragma once

//namespace ModelSFX
//{
//	class KTimePointColor;
//}
//
//#define GetAlpha(color) color>>24
//#define GetRed(color)   (color << 8) >> 24
//#define GetGreen(color) (color << 16) >> 24
//#define GetBlue(color) (color << 24 ) >> 24
//#define LIFE_SCREEN_POINT_SIZE 355
//// KDlg_TimePoint_Color dialog
//
//#define IDC_BUTTON_START 9999
//
//namespace PropertyBase
//{
//	class KDlg_TimePoint_Color;
//	class KTimePointButton : public CButton
//	{
//		DECLARE_DYNAMIC(KTimePointButton)
//		friend class KDlg_TimePoint_Color;
//	public:
//		KTimePointButton();
//		virtual ~KTimePointButton();
//
//	protected:
//		DECLARE_MESSAGE_MAP()
//	public:
//		afx_msg void OnPaint();
//
//	public:
//		BOOL Create(CPoint pt, CWnd *pParentWnd, UINT nID);
//		void SetStatus(BOOL bStatus) { m_bSelected = bStatus;Invalidate();}
//	private:
//		BOOL m_bSelected;
//		static const int m_iRegion = 5;
//		CPoint m_ptLeftOrg;
//		RECT m_rectPos;
//		void DrawButton(CDC* pdc);
//	public:
//		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	protected:
//		
//	public:
//		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//		afx_msg void OnBnClicked();
//		afx_msg void OnBnDoubleclicked();
//
//		virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
//	};
//}
//
//namespace PropertyBase
//{
//
//
//class KDlg_TimePoint_Color : public CDialog
//{
//	DECLARE_DYNAMIC(KDlg_TimePoint_Color)
//
//	friend class KTimePointButton;
//public:
//	struct TimePoint
//	{
//		COLORREF crColor;
//		int iOffset;
//	};
//	//void SetStore(PropertyBase::_VarProperty* pProperty)
//	//{
//	//	m_lpProperty = pProperty;
//	//}
//	//void SetStore( ModelSFX::KTimePointColor* pTPColor )
//	//{
//	//	m_pData = pTPColor;
//	//}
//	KDlg_TimePoint_Color(ModelSFX::KTimePointColor* pData, CWnd* pParent = NULL);   // standard constructor
//	virtual ~KDlg_TimePoint_Color();
//
//// Dialog Data
//	enum { IDD = IDD_TIMEPOINT_COLOR };
//private:
//	HCURSOR m_curAdd;
//	HCURSOR m_curNormal;
//	COLORREF m_crColorSel;
//	DWORD m_dwColor;
//	CButton m_ctlBtNext;
//	CButton m_ctlBtPrev;
//	CButton m_ctlBtDel;
//	CButton m_ctlBtNew;
//	
//	ModelSFX::KTimePointColor* m_pData;
//
//	CSliderCtrl m_ctlSliderRed;
//	CSliderCtrl m_ctlSliderGreen;
//	CSliderCtrl m_ctlSliderBlue;
//	CSliderCtrl m_ctlSliderAlpha;
//
//	float m_fPosition;
//	BYTE m_iRed;
//	BYTE m_iGreen;
//	BYTE m_iBlue;
//	BYTE m_iAlpha;
//	DWORD* m_pTPBarBuffer;
//	RECT m_rectSelColor;
//	RECT m_rectColorTP;
//	RECT m_rectEffReg;
//	RECT m_rectDropReg;
//	struct _VarProperty* m_lpProperty;
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	vector<TimePoint> m_vecTP;
//	vector<KTimePointButton*> m_vecButton;
//	int m_iIndex;
//	DECLARE_MESSAGE_MAP()
//	
//private:
//	void DrawTimePointColor(CDC* pdc);
//	COLORREF Conversion(DWORD color);
//	void ChangeSelColor(void);
//	void ConvertToTimePointColor(void);
//	bool PointInRect(CPoint point, RECT& rect);
//	void AddButton(CPoint pt, COLORREF color = RGB(255, 255, 255));
//
//
//public:
//	virtual BOOL OnInitDialog();
//	afx_msg void OnBnClickedButtonPrev();
//	afx_msg void OnBnClickedButtonNext();
//	afx_msg void OnBnClickedButtonDel();
//	//afx_msg void OnBnClickedButtonNew();
//	afx_msg void OnBnClickedButtonPick();
//	void DrawSelectColor(CDC* pdc);
//	afx_msg void OnPaint();
//	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//	void ChangeActiveButton(int iIndex);
//	void RenderToBmp(CDC* pdc);
//	afx_msg void OnBnClickedButtonApply();
//	afx_msg void OnEnChangeEditPosition();
//	void MoveTimePoint(int iIndex, int iPos);
//	afx_msg void OnEnChangeEditRed();
//	afx_msg void OnEnChangeEditGreen();
//	afx_msg void OnEnChangeEditBlue();
//	afx_msg void OnEnChangeEditAlpha();
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	
//	
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//protected:
//	virtual void PostNcDestroy();
//	virtual void OnCancel();
//	virtual void OnOK();
//};
//
//}
