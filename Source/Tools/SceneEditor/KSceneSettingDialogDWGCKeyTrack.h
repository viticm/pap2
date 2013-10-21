////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingDialogDWGCKeyTrack.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-2-9 10:22:16
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _INCLUDE_KSCENESETTINGDIALOGDWGCKEYTRACK_H_
#define _INCLUDE_KSCENESETTINGDIALOGDWGCKEYTRACK_H_
#include "IEKG3DEnvironment.h"
#include "IEKG3DEnvEffDynamicWeather.h"
#include "KSceneCustomScrollBar.h"
#include "KSceneSceneTreeCtrl.h"
#include "KSceneSceneGridCtrl.h"
#include <bitset>
#include <vector>
#include "afxwin.h"
#include "afxcmn.h"
////////////////////////////////////////////////////////////////////////////////



// KSceneSettingDialogDWGCKeyTrack dialog
//class KSceneSettingDialogKeyTable;

class IEKG3DScene;
class IEKG3DEnvironment;
class IEKG3DEnvEffDynamicWeather;

//这个面板原来是CPropertyPage，现在要单独显示，改成CDialog，但原来的写法不变，所以用这个来暂时代替一下
class KDummyPropertyPage : public CDialog	
{
	DECLARE_DYNAMIC(KDummyPropertyPage)
public:	
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnSetActive(){return TRUE;}
	virtual BOOL OnKillActive(){return TRUE;}
	KDummyPropertyPage(UINT ID):CDialog(ID){}
	virtual ~KDummyPropertyPage(){}
	afx_msg void OnShowWindow(
		BOOL bShow,
		UINT nStatus 
		);
};
class KSceneSettingDialogDWGCKeyTrack : public KDummyPropertyPage
	, public KSceneSceneGridCtrlEventHandler//public CPropertyPage
	, public KSceneSceneTreeCtrlEventHandler
{
	typedef KDummyPropertyPage TypeBase;
	DECLARE_DYNAMIC(KSceneSettingDialogDWGCKeyTrack)
	enum
	{
		MAX_KEY_TABLE = 7,
	};
	
public:
	KSceneSettingDialogDWGCKeyTrack();   // standard constructor
	virtual ~KSceneSettingDialogDWGCKeyTrack();

// Dialog Data
	enum { IDD = IDD_SCENESETTING_DWGC_KEY_TRACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	int m_nFlag;
private:
	std::vector<KG3DENVEFF_HANDLE>	m_vecEnvEffHandle;
	
	CImageList	m_ImageList;
	
	KSceneSceneTreeCtrl* m_pTree;
	KSceneSceneGridCtrl* m_pGridCtrl;
	KSceneCustomScrollBar m_ScrollBarVert;
	
	bool m_bIsDWPlaying;
private:
	VOID ReloadAllEnvEff();
	VOID SetGridKey(INT Row, INT Column, size_t Info);
	
private:
	VOID BindCallBacks(BOOL bBind);

	VOID BindGridCallbacks( BOOL bBind );
	VOID BindTreeCallbacks(BOOL bBind);
	
	VOID ClearContents();
	IEKG3DEnvEffDynamicWeather*	GetDynamicWeather();
	
	VOID SetGridKey(int nRow, int nColumn, INT nValue, LPCTSTR pText = NULL);
	VOID UpdatePage(DWORD dwType);	
private://用于拷贝的数据
	struct KeyTrackCopyStruct 
	{
		bool m_bIsValid;
		bool m_bCopyOrCut;
		KG3DENVEFF_HANDLE m_HandleSrc;
		KG3DENVEFF_DWGCKEY_HANDLE m_KeySrc;
		KeyTrackCopyStruct():m_bIsValid(false),m_bCopyOrCut(true){}
	};
	KeyTrackCopyStruct m_KeyTrackCopyStruct;

	KeyTrackCopyStruct& GetCopyStruct(){return m_KeyTrackCopyStruct;}
	

	struct KeyTrackStateStruct
	{
		BOOL m_bIsValid;
		KG3DENVEFF_HANDLE	m_Handle;
		KG3DENVEFF_DWGCKEY_HANDLE	m_KeyPos;
		KeyTrackStateStruct():m_bIsValid(FALSE){}
	};
	KeyTrackStateStruct m_KeyTrackStateStruct;
protected:
	void GridReloadKey(INT nRow, INT nColumn);	//把同一行的Grid都刷新一次
	BOOL GridDelKey(INT Row, INT Column);
	BOOL IsBlendingNeededToBeShown(){return this->IsDlgButtonChecked(IDC_CHECK1) == BST_CHECKED;}
	
	///Grid的Callback
	virtual BOOL IsEditStringValid(KSceneSceneGridCtrl& ctrl, CPoint point, LPCTSTR pStrEdit);
	virtual BOOL IsEditCharValid(KSceneSceneGridCtrl& ctrl, CPoint point, UINT uChar);
	virtual BOOL IsLButtomDoubleClickValid(KSceneSceneGridCtrl& ctrl, CPoint point);	
	virtual BOOL IsLButtonDownValid(KSceneSceneGridCtrl& ctrl, CPoint point);
	virtual BOOL IsCopyValid(KSceneSceneGridCtrl& ctrl, CPoint point);	
	virtual BOOL IsCutValid(KSceneSceneGridCtrl& ctrl, CPoint point);
	virtual BOOL IsPasteValid(KSceneSceneGridCtrl& ctrl, CPoint point);

	void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	
	///Tree的CallBack
	virtual BOOL OnItemExpanded(KSceneSceneTreeCtrl& tree, HTREEITEM hItemExpander);
	virtual BOOL OnItemDelete(KSceneSceneTreeCtrl& tree, HTREEITEM hItemDelete);
	virtual BOOL OnItemInserted(KSceneSceneTreeCtrl& tree, HTREEITEM hItemInserted);
	VOID OnEnumTree(HTREEITEM hItem);
	virtual BOOL OnItemSelectChange(KSceneSceneTreeCtrl& tree, HTREEITEM hItemInserted, HTREEITEM hItemLastSelect);
	virtual BOOL OnItemExpanding(KSceneSceneTreeCtrl& tree, HTREEITEM hItemExpanding);
	
private:
	INT m_nCopyMode;
	CEdit m_edRegion;
	CEdit m_edHour;
	CButton m_cbPlay;
	CSliderCtrl m_Slider;
	HWND	m_hWndToPropertySheet;
public:
	int Update();
	VOID SetAssociateHWnd(HWND hWnd);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnBnClickedBnDwgcRowAdd();
	afx_msg void OnBnClickedBnDwgcRowDel();
	afx_msg void OnBnClickedBnDwgcRowMod();
	afx_msg void OnBnClickedBnDwgcRowDown();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedSsBnSetKey();
	afx_msg void OnBnClickedBnDwgcPlay();
	afx_msg void OnBnClickedSsBnSetAllKey();
	afx_msg void OnBnClickedSsBnCopyPrevKey();
	afx_msg void OnBnClickedSsBnDelAllKey();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedSsBnHelp();
};


#endif //_INCLUDE_KSCENESETTINGDIALOGDWGCKEYTRACK_H_
