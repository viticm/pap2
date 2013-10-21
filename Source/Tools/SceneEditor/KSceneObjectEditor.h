// KSceneObjectEditor.h: interface for the KSceneObjectEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KSCENEOBJECTEDITOR_H__C66B5DA4_EF71_4EAA_8FFD_DA31B1BF6485__INCLUDED_)
#define AFX_KSCENEOBJECTEDITOR_H__C66B5DA4_EF71_4EAA_8FFD_DA31B1BF6485__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneEditBase.h"

class KSceneObjectEditor : public KSceneEditBase  
{
public:
	LPOBJECT   m_lpObject;
	BOOL       m_bShowPart;//整体一起显示还是各个部分分别显示；

	BOOL       m_bShowHeight_Up;
	BOOL       m_bShowHeight_Down;
	BOOL       m_bShowBarrier;
	BOOL       m_bShowObject;
	DWORD      m_dwBarrier;
	float      m_fHeightValue;
public:
	HRESULT LoadbjectFromFile(LPSTR pFileName);

	void ONKEYDOWN(WPARAM wParam, LPARAM lParam);
	void ONLBUTTONDOWN(WPARAM wParam, LPARAM lParam);
	void ONMOUSEMOVE(WPARAM wParam, LPARAM lParam);

	HRESULT OnNewObject();


	KSceneObjectEditor();
	virtual ~KSceneObjectEditor();

	HRESULT  Render();
	HRESULT  FrameMove();
	virtual	 HRESULT OnEditMoving();
	virtual	 HRESULT OnEditRotating();
	virtual  HRESULT OnEditScaling();
};

#endif // !defined(AFX_KSCENEOBJECTEDITOR_H__C66B5DA4_EF71_4EAA_8FFD_DA31B1BF6485__INCLUDED_)
