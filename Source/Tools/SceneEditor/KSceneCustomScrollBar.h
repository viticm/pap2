////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneCustomScrollBar.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-25 15:07:36
//  Comment     : 用于控制其他窗体。用消息反射，反射了WM_VSCROLL和WM_HSCROLL
/*然后再次向绑定的CWnd发送消息。发送的时候会根据对方的ScrollBar的比例调整
滚动大小。如果对方的ScrollBar也是动态出现的话，也需要对方绑定这个ScrollBar
这样才可以动态调整大小
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENECUSTOMSCROLLBAR_H_
#define _INCLUDE_KSCENECUSTOMSCROLLBAR_H_

////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>

// KSceneCustomScrollBar
class KSceneCustomScrollBar : public CScrollBar
{
	DECLARE_DYNAMIC(KSceneCustomScrollBar)

public:
	KSceneCustomScrollBar();
	virtual ~KSceneCustomScrollBar();
	afx_msg void VScroll(UINT nSBCode, UINT nPos);
	afx_msg void HScroll(UINT nSBCode, UINT nPos);
private:
	inline void OnScroll32(UINT message, UINT nSBCode, UINT nPos);
	void ReCalculateScrollScales();
protected:
	DECLARE_MESSAGE_MAP()
private:
	std::vector<CWnd*> m_vecWnd;
	std::vector<float>	m_vecScrollScale;
	INT m_nBarType;
	bool m_bLockMessagePocess;
public:
	template<UINT _BindIndex>
	BOOL BindAnWnd(CWnd* pCWnd, CWnd** ppOld = NULL)
	{ 
		_ASSERTE(pCWnd);
		if (_BindIndex == m_vecWnd.size())
		{
			m_vecWnd.push_back(pCWnd);
			m_vecScrollScale.push_back(0);
			return TRUE;
		}
		else if (_BindIndex > m_vecWnd.size())
		{
			return FALSE;
		}
		if (ppOld)
			*ppOld = m_vecWnd[_BindIndex];
		m_vecWnd[_BindIndex] = pCWnd;
		return TRUE;
	}
	void ReAlignBindedWindows();
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};




#endif //_INCLUDE_KSCENECUSTOMSCROLLBAR_H_
