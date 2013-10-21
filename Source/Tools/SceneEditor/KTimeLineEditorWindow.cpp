#include "stdafx.h"
//#include "ktimelineeditorwindow.h"
//#include "KTimeLineEditorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//
//
//KTimeLineEditorWindow::KTimeLineEditorWindow(void)
//{
//	m_pTimeLineListBox = NULL;
//	m_nNumTimeLine = 0;
//	m_nAction = 0;
//	m_nActionSave = 0;
//	m_bInsertPoint = FALSE;
//	m_pGrid = NULL;
//	m_ptOrg.x = 10;
//	m_ptOrg.y = 370;
//	m_fHScale = 1.0f;
//	m_fVScale = 1.0f;
//	memset(&m_rectSelectedRegion, -1, sizeof(RECT));
//}
//
//KTimeLineEditorWindow::~KTimeLineEditorWindow(void)
//{
//
//	m_SelectTimeLines.clear();
//	delete m_pGrid;
//
//}
//
//
//BOOL KTimeLineEditorWindow::Create(RECT& rectWindow, CWnd* pParent)
//{
//	BOOL bReturn;
//	ASSERT(pParent);
//	KTimeLineEditorDialog* pWindow = reinterpret_cast<KTimeLineEditorDialog*>(pParent);
//	m_pTimeLines = pWindow->GetTimeLines();
//	//Setting up the orgin and scales
//	map<string, KTimeLine4Editor<float, float>*>::iterator it = m_pTimeLines->begin();
//	for ( ; it != m_pTimeLines->end(); it++ )
//	{
//		it->second->SetOrg(m_ptOrg);
//		it->second->SetScale(m_fVScale, ENUM_SCALE_VERTICAL);
//		it->second->SetScale(m_fHScale, ENUM_SCALE_HORIZONTAL);
//	}
//	m_pTimeLineListBox = reinterpret_cast<CListBox*>(pWindow->GetDlgItem(IDC_LIST_TIMELINES));
//	bReturn = CWnd::Create(NULL, "TimeLineEditorWindow", WS_VISIBLE|WS_CHILD, rectWindow, pParent, IDD_WINDOW_TIMELINEEDITOR);
//	m_pGrid = new KTimeLineGrid(abs(rectWindow.right - rectWindow.left), abs(rectWindow.bottom - rectWindow.top));
//	m_pGrid->SetOrigin(m_ptOrg);
//	m_pGrid->SetScale(m_fVScale, ENUM_SCALE_VERTICAL);
//	m_pGrid->SetScale(m_fHScale, ENUM_SCALE_HORIZONTAL);
//	BOOL bReturn1 = m_Font.CreateFont(10, 5, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
//		              FF_DONTCARE| DEFAULT_PITCH, "Arial" );
//	SetFocus();
//
//	return bReturn;
//}
//
//
//LRESULT KTimeLineEditorWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	TCHAR strTimeLineName[MAX_TIMELINE_NAME];
//	INT nIndices[MAX_SELECTION];
//	int i;
//	switch(message)
//	{
//		case WM_STATUS_CHANGE:
//			{
//				m_nAction = static_cast<int>(wParam);
//			}
//			break;
//		case WM_TIMELINE_CHANGE:
//			{
//				vector<string> NewSelectList;
//				int nNumActual = m_pTimeLineListBox->GetSelItems(MAX_SELECTION, nIndices);
//				for ( i = 0; i < nNumActual; i++ )
//				{
//					m_pTimeLineListBox->GetText(nIndices[i], strTimeLineName);
//					NewSelectList.push_back(strTimeLineName);
//					m_SelectTimeLines.erase(strTimeLineName);
//				}
//				set<string>::iterator it = m_SelectTimeLines.begin();
//				for ( ; it != m_SelectTimeLines.end(); it++ )
//				{
//					(*m_pTimeLines)[*it]->ClearSelection();
//				}
//				m_SelectTimeLines.clear();
//				for( i = 0; i < nNumActual; i++ )
//				{
//					m_SelectTimeLines.insert(NewSelectList[i]);
//				}
//				Invalidate();
//			}
//			break;
//		case WM_KEYDOWN:
//			{
//				map<string, KTimeLine4Editor<float, float>*>::iterator it = m_pTimeLines->begin();
//				switch(wParam)
//				{
//				case VK_DELETE://Delete all selected key points
//                    {
//						set<string>::iterator it = m_SelectTimeLines.begin();
//						for ( ; it != m_SelectTimeLines.end(); it++ )
//						{
//							(*m_pTimeLines)[*it]->Remove();
//						}
//						Invalidate();
//					}
//					break;
//				case VK_RETURN://Update value
//					{
//						KTimeLineEditorDialog* pParent = static_cast<KTimeLineEditorDialog*>(GetParent());
//						if ( pParent && m_SelectTimeLines.size() )//如果有选中显示的Timeline
//						{
//							float fTime = pParent->m_fTime;
//							float fValue = pParent->m_fValue;
//							set<string>::iterator it = m_SelectTimeLines.begin();
//							for ( ; it != m_SelectTimeLines.end(); it++ )
//							{
//								if ((*m_pTimeLines)[*it]->GetNumSelectedKeys())
//								{
//									if( fTime != (*m_pTimeLines)[*it]->GetOperateKey() )
//									{
//										while ( (*m_pTimeLines)[*it]->ValidateKey(fTime))
//											fTime += 0.00001f;//防止出现覆盖的情况
//									}
//									(*m_pTimeLines)[*it]->MoveFrame((*m_pTimeLines)[*it]->GetOperateKey(), fTime);
//									(*m_pTimeLines)[*it]->SetFrameValue(fTime, fValue);
//									(*m_pTimeLines)[*it]->SetOperateKey(fTime);
//								}
//							}
//							Invalidate();
//						}
//					return 0;
//					}
//					break;
//				case VK_LEFT://Move all timeline's Origin right
//					{
//						m_ptOrg.x -= 20;
//						while ( it != m_pTimeLines->end() )
//						{
//							it->second->SetOrg(m_ptOrg);
//							it++;
//						}
//						POINT ptOrg = m_pGrid->GetOrigin();
//						ptOrg.x -= 20;
//						m_pGrid->SetOrigin(m_ptOrg);
//						Invalidate();
//					}
//					break;
//				case VK_RIGHT://Move all timeline's Origin left
//					{
//						m_ptOrg.x += 20;
//						while ( it != m_pTimeLines->end() )
//						{
//							it->second->SetOrg(m_ptOrg);
//							it++;
//						}
//						m_pGrid->SetOrigin(m_ptOrg);
//						Invalidate();
//
//					}
//					break;
//				case VK_UP://Move all timeline's Origin down
//					{
//						m_ptOrg.y += 20;
//						while ( it != m_pTimeLines->end() )
//						{
//							it->second->SetOrg(m_ptOrg);
//							it++;
//						}
//						m_pGrid->SetOrigin(m_ptOrg);
//						Invalidate();
//					}
//					break;
//				case VK_DOWN://Move all timeline's Origin up
//					{
//						//设置Timeline
//						m_ptOrg.y -= 20;
//						while ( it != m_pTimeLines->end() )
//						{
//							it->second->SetOrg(m_ptOrg);
//							it++;
//						}
//						//设置Grid
//						m_pGrid->SetOrigin(m_ptOrg);
//						Invalidate();
//					}
//					break;
//				case 72://H, reset position
//					{
//						m_ptOrg.x = 10;
//						m_ptOrg.y = 370;
//						m_fHScale = 1.0f;
//						m_fVScale = 1.0f;
//						while ( it != m_pTimeLines->end() )
//						{
//							it->second->SetOrg(m_ptOrg);
//							it->second->SetScale(m_fHScale, ENUM_SCALE_HORIZONTAL);
//							it->second->SetScale(m_fVScale, ENUM_SCALE_VERTICAL);
//							it++;
//						}
//						//设置Grid
//						m_pGrid->SetScale(m_fHScale, ENUM_SCALE_HORIZONTAL);
//						m_pGrid->SetScale(m_fVScale, ENUM_SCALE_VERTICAL);
//						m_pGrid->SetOrigin(m_ptOrg);
//						Invalidate();
//					}
//				}
//				break;
//			}
//			break;
//	}
//	return CWnd::WindowProc(message, wParam, lParam);
//}
//
//BEGIN_MESSAGE_MAP(KTimeLineEditorWindow, CWnd)
//	ON_WM_PAINT()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONUP()
//	ON_WM_MOUSEWHEEL()
//	ON_WM_MBUTTONDOWN()
//	ON_WM_MBUTTONUP()
//END_MESSAGE_MAP()
//
//void KTimeLineEditorWindow::OnPaint()
//{
//	CPaintDC dc(this);
//	CPen PenDot(PS_DOT, 1, RGB(0, 0, 0));
//	CPen Pen(PS_SOLID, 1, RGB(0, 0, 0));
//
//	dc.SelectObject(&m_Font);
//
//	//显示网格
//	m_pGrid->DrawGrid(&dc);
//
//	//显示Timeline
//	set<string>::iterator it = m_SelectTimeLines.begin();
//	for( ; it != m_SelectTimeLines.end(); it++ )
//		DrawTimeLine(&dc, *it);
//
//
//	if ( m_bInsertPoint )
//	{
//		CPen* pOldPen = dc.SelectObject(&Pen);
//		dc.MoveTo(m_ptInsertPoint.x, 0);
//		dc.LineTo(m_ptInsertPoint.x, 2046);
//	}
//
//	//显示选择区域
//
//	dc.SelectObject(&PenDot);
//	HGDIOBJ hOldBrush = dc.SelectObject(GetStockObject(HOLLOW_BRUSH));
//	if ( m_rectSelectedRegion.right != -1)
//		dc.Rectangle(&m_rectSelectedRegion);
//	dc.SelectObject(hOldBrush);
//
//
//
//
//}
//
//inline COLORREF KTimeLineEditorWindow::GetTimeLineColor(const string& strTimeLineName)
//{
//	COLORREF crReturn;
//	int nIndex = (m_pTimeLineListBox->FindString(0, strTimeLineName.c_str())) % 3;
//	crReturn = (0xff<<(nIndex<<3)) & 0x00ffffff;
//	return crReturn;
//}
//
//void KTimeLineEditorWindow::DrawTimeLine(CDC* pDC, const string& strTimeLineName)
//{
//	KTimeLine4Editor<float, float>* pTimeLine = (*m_pTimeLines)[strTimeLineName];
//	COLORREF crColor = GetTimeLineColor(strTimeLineName);
//	pTimeLine->DrawTimeLineCurve(pDC, crColor);
//
//}
//
//void KTimeLineEditorWindow::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	set<string>::iterator it = m_SelectTimeLines.begin();
//	//Select operation
//	m_rectSelectedRegion.left = point.x;
//	m_rectSelectedRegion.top = point.y;
//	m_nSelected = 0;
//	for ( ; it != m_SelectTimeLines.end(); it++ )
//	{
//		(*m_pTimeLines)[*it]->ClickSelection(point);
//	}
//	Invalidate();
//    CWnd::OnLButtonDown(nFlags, point);
//}
//
//void KTimeLineEditorWindow::OnMouseMove(UINT nFlags, CPoint point)
//{
//	KTimeLineEditorDialog* pParent = static_cast<KTimeLineEditorDialog*>(GetParent());
//	set<string>::iterator it = m_SelectTimeLines.begin();
//	if ( nFlags & MK_LBUTTON )
//	{
//		//Draw selection region box
//		m_rectSelectedRegion.bottom = point.y;
//		m_rectSelectedRegion.right = point.x;
//		float fTime;
//		KTimeLine<float, float>::enumInterpolate Type = KTimeLine<float, float>::ENUM_INTERPOLATE_LINEAR;
//		BOOL bGotType = FALSE;
//		for ( ; it != m_SelectTimeLines.end(); it++ )
//		{
//			(*m_pTimeLines)[*it]->ClearSelection();
//			if ( (*m_pTimeLines)[*it]->SelectPointInRegion(m_rectSelectedRegion, fTime) && !bGotType )
//			{
//				Type = (*m_pTimeLines)[*it]->GetInterpolateType();
//				bGotType = TRUE;
//			}
//		}
//		if ( bGotType )
//		{
//			KTimeLineEditorDialog* pParent = static_cast<KTimeLineEditorDialog*>(GetParent());
//			if (pParent)
//			{
//				if ( Type == KTimeLine<float, float>::ENUM_INTERPOLATE_NONE )
//					static_cast<CButton*>(pParent->GetDlgItem(IDC_CHECK_INP))->SetCheck(BST_UNCHECKED);
//				else
//					static_cast<CButton*>(pParent->GetDlgItem(IDC_CHECK_INP))->SetCheck(BST_CHECKED);
//			}
//		}
//		Invalidate(TRUE);
//	}
//	else if ( nFlags & MK_MBUTTON )//Operations here
//	{
//
//		//for ( i = 0; i < nNumTimeLine; i++ )
//		for ( ; it != m_SelectTimeLines.end(); it++ )
//		{
//			//if ( (*m_pTimeLines)[m_vecSelectTimeLines[i]]->IsActive())//激活的
//			if ( (*m_pTimeLines)[*it]->IsActive())//激活的
//			{
//				//if ( (*m_pTimeLines)[m_vecSelectTimeLines[i]]->GetOperateKey() != -9999999.9f )//有操作点
//				if ( (*m_pTimeLines)[*it]->GetNumSelectedKeys() )//有操作点
//				{
//					switch(m_nAction)
//					{
//					case 0://Move
//						{
//							//(*m_pTimeLines)[m_vecSelectTimeLines[i]]->MoveTo(point);
//							(*m_pTimeLines)[*it]->MoveTo(point);
//							if (pParent)
//							{
//								float fTime = (*m_pTimeLines)[*it]->GetOperateKey();
//								float* const pValue = (*m_pTimeLines)[*it]->GetSingleKeyFrame(fTime);
//								LPARAM lParam = reinterpret_cast<LPARAM>(pValue);
//								WPARAM wParam = reinterpret_cast<WPARAM>(&fTime);
//								::SendMessage(pParent->GetSafeHwnd(), WM_UPDATEVALUE, wParam, lParam);
//							}
//						}
//						break;
//					case 1://Insert
//						{
//							(*m_pTimeLines)[*it]->DrawInsertPoint(point);
//							m_ptInsertPoint = point;
//							m_bInsertPoint = TRUE;
//
//						}
//						break;
//					case 2://Add
//						{
//
//						}
//						break;
//					}
//				}
//			}
//		}
//		Invalidate(TRUE);
//	}
//	CWnd::OnMouseMove(nFlags, point);
//}
//
//
//
//void KTimeLineEditorWindow::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	int nDebug = 0;
//
//	memset(&m_rectSelectedRegion, -1, sizeof(RECT));
//	CWnd* pParent = GetParent();
//	if ( pParent && m_SelectTimeLines.size() )
//	{
//		set<string>::iterator it = m_SelectTimeLines.begin();
//		while ( it != m_SelectTimeLines.end() && !(*m_pTimeLines)[*it]->IsActive() && !(*m_pTimeLines)[*it]->GetNumSelectedKeys() ) it++, nDebug++;
//		if ( it != m_SelectTimeLines.end() )
//		{
//			if ( (*m_pTimeLines)[*it]->GetNumSelectedKeys() )
//			{
//				float fTime = (*m_pTimeLines)[*it]->GetOperateKey();
//				float fValue = *(*m_pTimeLines)[*it]->GetSingleKeyFrame(fTime);
//				::SendMessage(pParent->GetSafeHwnd(), WM_UPDATEVALUE, reinterpret_cast<WPARAM>(&fTime), reinterpret_cast<LPARAM>(&fValue));
//			}
//		}
//	}
//	Invalidate();
//	CWnd::OnLButtonUp(nFlags, point);
//}
//
//void KTimeLineEditorWindow::Invalidate(BOOL bErase)
//{
//	CWnd* pParent = GetParent();
//	if ( pParent )
//	{
//		RECT rect;
//		GetWindowRect(&rect);
//		pParent->ScreenToClient(&rect);
//		pParent->InvalidateRect(&rect, bErase);
//	}
//	CWnd::Invalidate(bErase);
//}
//
//BOOL KTimeLineEditorWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
//{
//	//If ZDelta is negative, Zoom out
//	//set<string>::iterator it = m_SelectTimeLines.begin();
//	map<string, KTimeLine4Editor<float, float>*>::iterator it = (*m_pTimeLines).begin();
//	float fStep = zDelta / WHEEL_DELTA * 0.8f;
//	if ( zDelta > 0 )
//	{
//		m_fHScale *= fStep;
//		m_fVScale *= fStep;
//	}
//	else
//	{
//		m_fHScale /= -fStep;
//		m_fVScale /= -fStep;
//	}
//	for ( ; it != (*m_pTimeLines).end(); it++ )
//	{
//		it->second->SetScale(m_fHScale, ENUM_SCALE_HORIZONTAL);
//		it->second->SetScale(m_fVScale, ENUM_SCALE_VERTICAL);
//
//	}
//	m_pGrid->SetScale(m_fHScale, ENUM_SCALE_HORIZONTAL);
//	m_pGrid->SetScale(m_fVScale, ENUM_SCALE_VERTICAL);
//	Invalidate();
//	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
//}
//
//BOOL KTimeLineEditorWindow::SaveToFile(const TCHAR* szFileName, int nIndex /*= 0*/)
//{
//	BOOL bReturn = E_FAIL;
//
//	if ( static_cast<unsigned int>(nIndex) < m_SelectTimeLines.size() )
//	{
//		if ( (*m_pTimeLines)[*m_SelectTimeLines.begin()]->SaveToFile(szFileName) == S_OK )
//		{
//			bReturn = S_OK;
//		}
//	}
//	return bReturn;
//}
//
//BOOL KTimeLineEditorWindow::LoadFromFile(const TCHAR* szFileName, int nIndex /*= 0*/ )
//{
//	BOOL bReturn = E_FAIL;
//	if ( static_cast<unsigned int>(nIndex) < m_SelectTimeLines.size() )
//	{
//		(*m_pTimeLines)[*m_SelectTimeLines.begin()]->Clear();
//		if ( (*m_pTimeLines)[*m_SelectTimeLines.begin()]->LoadFromFile(szFileName) == S_OK )
//		{
//			bReturn = S_OK;
//		}
//	}
//	return bReturn;
//}
//
//void KTimeLineEditorWindow::OnMButtonDown(UINT nFlags, CPoint point)
//{
//	set<string>::iterator it = m_SelectTimeLines.begin();
//	float fSelectedTime;
//	switch(m_nAction)
//	{
//	case 0://Move
//		{
//			for ( ; it != m_SelectTimeLines.end(); it++ )
//			{
//				if ((*m_pTimeLines)[*it]->IsActive())
//				{
//					//nActiveLines++;
//					if ( (*m_pTimeLines)[*it]->SelectSinglePoint(point, fSelectedTime, FALSE) )//是否有关键点被选中
//					{
//						if ((*m_pTimeLines)[*it]->IsTimePointSelected(fSelectedTime))//这个点是否在已选中的关键点列表中
//						{
//							(*m_pTimeLines)[*it]->SetOperateKey(fSelectedTime);//设定这个点为操作点
//						}
//					}
//				}
//			}
//		}
//		break;
//	case 1://Insert
//		{
//			m_nActionSave = 1;
//			m_bInsertPoint = TRUE;
//			for ( ; it != m_SelectTimeLines.end(); it++ )
//			{
//				if ((*m_pTimeLines)[*it]->IsActive())
//				{
//					(*m_pTimeLines)[*it]->DrawInsertPoint(point);
//					m_ptInsertPoint = point;
//				}
//			}
//			Invalidate();
//		}
//		break;
//	case 2://Add
//		{
//			m_nActionSave = m_nAction;
//			for ( ; it != m_SelectTimeLines.end(); it++ )
//			{
//				if ((*m_pTimeLines)[*it]->IsActive())
//				{
//					(*m_pTimeLines)[*it]->Add(point);
//					m_nAction = 0;//变化操作为移动
//				}
//			}
//			Invalidate();
//		}
//		break;
//	}
//	CWnd::OnMButtonDown(nFlags, point);
//}
//
//
//void KTimeLineEditorWindow::OnMButtonUp(UINT nFlags, CPoint point)
//{
//	set<string>::iterator it = m_SelectTimeLines.begin();
//	if ( m_nAction == 1 )//Insert will be done when release the middle button
//	{
//		it = m_SelectTimeLines.begin();
//		for ( ; it != m_SelectTimeLines.end(); it++ )
//		{
//			if ((*m_pTimeLines)[*it]->IsActive())
//			{
//				(*m_pTimeLines)[*it]->Insert(point);
//			}
//		}
//		m_bInsertPoint = FALSE;
//		Invalidate();
//	}
//	if ( m_nActionSave )
//	{
//		m_nAction = m_nActionSave;
//		m_nActionSave = 0;
//	}
//	CWnd::OnMButtonUp(nFlags, point);
//}
//
//void KTimeLineEditorWindow::GetSelectedTimeLines(KTimeLine4Editor<float, float>** &pSelected, int& nNum)
//{
//	int i = 0;
//	delete[] pSelected;
//	set<string>::iterator itBegin = m_SelectTimeLines.begin();
//	nNum = m_SelectTimeLines.size();
//	pSelected = new KTimeLine4Editor<float, float>*[nNum];
//
//	if ( pSelected )
//	{
//		while ( itBegin != m_SelectTimeLines.end() )
//		{
//			pSelected[i] = (*m_pTimeLines)[*itBegin];
//			i++;
//			itBegin++;
//		}
//	}
//}