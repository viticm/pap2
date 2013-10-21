// KTimeLineEditorDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "KTimeLineEditorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//
//
//// KTimeLineEditorDialog dialog
//
//IMPLEMENT_DYNAMIC(KTimeLineEditorDialog, CDialog)
//
//KTimeLineEditorDialog::KTimeLineEditorDialog(CWnd* pParent /*=NULL*/)
//	: CDialog(KTimeLineEditorDialog::IDD, pParent)
//	, m_nAction(0)
//{
//	Create(IDD);
//	ShowWindow(SW_SHOW);
//	m_bInterpolated = TRUE;
//}
//
//KTimeLineEditorDialog::~KTimeLineEditorDialog()
//{
//}
//
//void KTimeLineEditorDialog::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_EDIT_TIME, m_fTime);
//	DDX_Text(pDX, IDC_EDIT_VALUE, m_fValue);
//}
//
//
//BEGIN_MESSAGE_MAP(KTimeLineEditorDialog, CDialog)
//	ON_BN_CLICKED(IDC_RADIO_INSERT, OnBnClickedRadioInsert)
//	ON_BN_CLICKED(IDC_RADIO_ADD, OnBnClickedRadioAdd)
//	ON_BN_CLICKED(IDC_RADIO_MOVE, OnBnClickedRadioMove)
//	ON_LBN_SELCHANGE(IDC_LIST_TIMELINES, OnLbnSelchangeListTimelines)
//	ON_WM_MOUSEWHEEL()
//	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
//	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
//	ON_BN_CLICKED(IDC_CHECK_INP, OnBnClickedCheckInp)
//END_MESSAGE_MAP()
//
//
//// KTimeLineEditorDialog message handlers
//BOOL KTimeLineEditorDialog::OnInitDialog()
//{
//	BOOL bReturn = TRUE;
//	//int i;
//	CDialog::OnInitDialog();
//
//	static_cast<CButton*>(GetDlgItem(IDC_CHECK_INP))->SetCheck(BST_CHECKED);
//	//////////////////////////////////////////////////////////////////////////
//	//Create TimeLineEditor Window here
//	RECT rectEditor;
//	CWnd* pPlaceHolder = GetDlgItem(IDC_STATIC_EDITOR);
//	ASSERT(pPlaceHolder);
//	pPlaceHolder->GetWindowRect(&rectEditor);
//	ScreenToClient(&rectEditor);
//	//pPlaceHolder->ShowWindow(SW_HIDE);
//	m_pwndEditor = new KTimeLineEditorWindow;
//	m_pwndEditor->Create(rectEditor, this);
//	//////////////////////////////////////////////////////////////////////////
//
//	//////////////////////////////////////////////////////////////////////////
//	//Not deleted
//	/*KTimeLine4Editor<float, float>* pTimeLines[40];
//	float fX = 10.0f;
//	for ( i = 0; i < 10; i++ )
//	{
//		pTimeLines[i] = new KTimeLine4Editor<float, float>;
//		pTimeLines[i]->AddFrame(10.0f, i * fX);
//		pTimeLines[i]->AddFrame(20.0f, i * fX + 20);
//		pTimeLines[i]->AddFrame(30.0f, i * fX);
//		AddTimeLine(NULL, pTimeLines[i]);
//	}
//	pTimeLines[i] = new KTimeLine4Editor<float, float>;
//	pTimeLines[i]->AddFrame(10.0f, i * fX);
//	pTimeLines[i]->AddFrame(40.0f, i * fX + 20);
//	pTimeLines[i]->AddFrame(60.0f, i * fX);
//	AddTimeLine("test", pTimeLines[i]);*/
//	//////////////////////////////////////////////////////////////////////////
//
//	CButton* btMove = static_cast<CButton*>(GetDlgItem(IDC_RADIO_MOVE));
//	btMove->SetCheck(BST_CHECKED);
//	GetDlgItem(IDC_EDIT_VALUE)->SetWindowText("");
//	GetDlgItem(IDC_EDIT_TIME)->SetWindowText("");
//	CWnd* pOld = m_pwndEditor->SetFocus();
//	return TRUE;  // return TRUE unless you set the focus to a control
//	// EXCEPTION: OCX Property Pages should return FALSE
//}
//
//void KTimeLineEditorDialog::OnOK()
//{
//
//}
//
//void KTimeLineEditorDialog::PostNcDestroy()
//{
//	//delete this;
//}
//
//void KTimeLineEditorDialog::OnCancel()
//{
//	DestroyWindow();
//}
//
//void KTimeLineEditorDialog::OnBnClickedRadioInsert()
//{
//	m_nAction = STATUS_INSERT;
//	OnActionStatusChange(m_nAction);
//}
//
//void KTimeLineEditorDialog::OnBnClickedRadioAdd()
//{
//	m_nAction = STATUS_ADD;
//	OnActionStatusChange(m_nAction);
//}
//
//void KTimeLineEditorDialog::OnBnClickedRadioMove()
//{
//	m_nAction = STATUS_MOVE;
//	OnActionStatusChange(m_nAction);
//}
//
//
///*
//*@brief 向左边的列表中添加TimeLine，并制定名字
//*@param TimeLine的名字
//*@param TimeLine的指针
//*/
//void KTimeLineEditorDialog::AddTimeLine(LPCSTR strName, KTimeLine4Editor<float, float>* pTimeLine)
//{
//	TCHAR strTimeLineDefaultName[MAX_TIMELINE_NAME];
//	int i = 0;
//	size_t nCount;
//
//	ASSERT(pTimeLine);
//	CListBox* pTimeLineList = static_cast<CListBox*>(GetDlgItem(IDC_LIST_TIMELINES));
//	POINT ptOrg = m_pwndEditor->GetOrg();
//	float fVScale = m_pwndEditor->GetScale(ENUM_SCALE_VERTICAL);
//	float fHScale = m_pwndEditor->GetScale(ENUM_SCALE_HORIZONTAL);
//	pTimeLine->SetScale(fVScale, ENUM_SCALE_VERTICAL);
//	pTimeLine->SetScale(fHScale, ENUM_SCALE_HORIZONTAL);
//	pTimeLine->SetOrg(ptOrg);
//	if ( !pTimeLine->GetNumFrames())
//	{
//		pTimeLine->AddFrame(0.0f, 0.0f);
//		POINT pt = m_pwndEditor->GetOrg();
//		float fReturnTime;
//		pTimeLine->SelectSinglePoint(pt, fReturnTime);
//	}
//
//	if ( !strName || !strcmp(strName, "") )
//	{
//		//Make sure the Time line name is valid, if not, set it to NonameX
//		do
//		{
//			sprintf(strTimeLineDefaultName, "Noname%d", i++);
//			nCount = m_TimeLines.count(strTimeLineDefaultName);
//		}while ( nCount );
//		m_TimeLines[strTimeLineDefaultName] = pTimeLine;
//		pTimeLineList->GetDC()->SetTextColor(RGB(0, 255, 0));
//		pTimeLineList->AddString(strTimeLineDefaultName);
//	}
//	else
//	{
//		m_TimeLines[strName] = pTimeLine;
//		pTimeLineList->AddString(strName);
//	}
//}
//
///*
//*@brief 在选择状态改变的时候， 通知TimeLineEditor窗口
//*@param nStatus 状态
//*/
//void KTimeLineEditorDialog::OnActionStatusChange(int nStatus)
//{
//	HWND hwndEditor = m_pwndEditor->GetSafeHwnd();
//	ASSERT(::IsWindow(hwndEditor));
//	::SendMessage(hwndEditor, WM_STATUS_CHANGE, nStatus, 0);
//}
//
///*
//*@brief 在选中的Timeline改变的时候， 通知TimeLineEditor窗口
//*@param nStatus 状态
//*/
//void KTimeLineEditorDialog::OnLbnSelchangeListTimelines()
//{
//	HWND hwndEditor = m_pwndEditor->GetSafeHwnd();
//	ASSERT(::IsWindow(hwndEditor));
//	::SendMessage(hwndEditor, WM_TIMELINE_CHANGE, 0, 0);
//}
//
//BOOL KTimeLineEditorDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
//{
//	// TODO: Add your message handler code here and/or call default
//	//MessageBox("");
//	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
//}
//
//
//
//BOOL KTimeLineEditorDialog::PreTranslateMessage(MSG* pMsg)
//{
//	switch(pMsg->message)
//	{
//	case WM_KEYDOWN:
//		switch(pMsg->wParam)
//		{
//		case VK_RETURN:
//			{
//				UpdateData();
//			}
//		case VK_LEFT:
//		case VK_RIGHT:
//		case VK_UP:
//		case VK_DOWN:
//		case VK_DELETE:
//		case 72:
//			break;
//		default:
//			return CDialog::PreTranslateMessage(pMsg);
//		}
//	case WM_MOUSEWHEEL:
//		::SendMessage(m_pwndEditor->GetSafeHwnd(), pMsg->message, pMsg->wParam, pMsg->lParam);
//		return TRUE;
//		break;
//
//	}
//
//	return CDialog::PreTranslateMessage(pMsg);
//}
//
//LRESULT KTimeLineEditorDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	// TODO: Add your specialized code here and/or call the base class
//	switch(message)
//	{
//	case WM_UPDATEVALUE:
//		{
//			m_fTime = *reinterpret_cast<float*>(wParam);
//			m_fValue = *reinterpret_cast<float*>(lParam);
//			UpdateData(FALSE);
//		}
//		break;
//	}
//	return CDialog::WindowProc(message, wParam, lParam);
//}
//
//void KTimeLineEditorDialog::OnBnClickedButtonSave()
//{
//	KTimeLine4Editor<float, float>** pSelected = NULL;
//	int nNumSelected;
//	m_pwndEditor->GetSelectedTimeLines(pSelected, nNumSelected);
//	if ( nNumSelected )
//	{
//		CFileDialog dlg(FALSE, "tl");
//		if( dlg.DoModal() == IDOK )
//		{
//			CString strSaveFileName = dlg.GetPathName();
//			//m_pwndEditor->SaveToFile(strSaveFileName);
//			m_pwndEditor->SaveToFile(strSaveFileName);
//		}
//	}
//}
//
//void KTimeLineEditorDialog::OnBnClickedButtonLoad()
//{
//	KTimeLine4Editor<float, float>** pSelected = NULL;
//	int nNumSelected = 0;
//	m_pwndEditor->GetSelectedTimeLines(pSelected, nNumSelected);
//	if ( nNumSelected )
//	{
//		CFileDialog dlg(TRUE, "tl");
//		if( dlg.DoModal() == IDOK )
//		{
//			CString strSaveFileName = dlg.GetPathName();
//			//m_pwndEditor->SaveToFile(strSaveFileName);
//			m_pwndEditor->LoadFromFile(strSaveFileName);
//		}
//		if ( pSelected[0]->GetInterpolateType() != KTimeLine<float, float>::ENUM_INTERPOLATE_NONE )
//		{
//			static_cast<CButton*>(GetDlgItem(IDC_CHECK_INP))->SetCheck(BST_CHECKED);
//		}
//		else
//		{
//			static_cast<CButton*>(GetDlgItem(IDC_CHECK_INP))->SetCheck(BST_UNCHECKED);
//		}
//		delete[] pSelected;
//		Invalidate();
//	}
//}
//
//void KTimeLineEditorDialog::OnBnClickedCheckInp()
//{
//	int i;
//	m_bInterpolated = ( static_cast<CButton*>(GetDlgItem(IDC_CHECK_INP))->GetCheck() & BST_CHECKED );
//	KTimeLine<float, float>::enumInterpolate Type = KTimeLine<float, float>::ENUM_INTERPOLATE_LINEAR;
//	if ( !m_bInterpolated )
//		Type = KTimeLine<float, float>::ENUM_INTERPOLATE_NONE;
//
//	KTimeLine4Editor<float, float>** pSelected = NULL;
//	int nNumSelected;
//	m_pwndEditor->GetSelectedTimeLines(pSelected, nNumSelected);
//	for ( i = 0; i < nNumSelected; i++ )
//	{
//		pSelected[i]->SetInterpolateType(Type);
//	}
//	delete[] pSelected;
//	m_pwndEditor->Invalidate();
//}
