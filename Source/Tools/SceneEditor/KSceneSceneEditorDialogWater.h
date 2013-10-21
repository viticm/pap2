#pragma once
//#include "KG3DSceneSceneEditor.h"
interface IEKG3DSceneSceneEditor;//by dengzhihui Refactor 2006Äê11ÔÂ27ÈÕ
 
// KSceneSceneEditorDialogWater dialog

class KSceneSceneEditorDialogWater : public CDialog/*, public IEKG3DSimpleSink*/
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogWater)

	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
public:
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}

 
	float m_fBumpNear;
	float m_fBumpFar;
public:
	KSceneSceneEditorDialogWater(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogWater();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_WATER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	//virtual void SinkFunc(INT nSinkID){}
	//virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedCheckShowreftext();
	afx_msg void OnEnChangeEditBumpNear();
	afx_msg void OnEnChangeEditBumpFar();

	afx_msg void OnBnClickedCheckWaterUp();
	afx_msg void OnBnClickedCheckWaterDown();
	afx_msg void OnBnClickedCheckWaterFlat();
	afx_msg void OnRefreshFx();

	afx_msg void OnBnClickedButtonLoad();
public:
	afx_msg void OnBnClickedSetdefheight();
public:
	afx_msg void OnBnClickedCheckWaterget();
public:
	afx_msg void OnBnClickedCheckWaterset();
public:
	afx_msg void OnBnClickedCheckWaterfollew();
public:
	float m_fHeight;
public:
	float m_fAlpha;
public:
	afx_msg void OnEnChangeEdit1();
public:
	afx_msg void OnEnChangeEdit2();
public:
	afx_msg void OnBnClickedCheckShowwater();
public:
	afx_msg void OnBnClickedCheckWaterreract();
public:
	afx_msg void OnBnClickedSetdefheight2();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCheckAlwayrender();
	afx_msg void OnBnClickedCheckWateredge();
};

