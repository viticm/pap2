#if !defined(AFX_KSCENEMODELEDITORANIMATIONPANEL_H__D57AEDC9_B2F0_457B_8911_57A47527D7B6__INCLUDED_)
#define AFX_KSCENEMODELEDITORANIMATIONPANEL_H__D57AEDC9_B2F0_457B_8911_57A47527D7B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorAnimationPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorAnimationPanel form view

#ifndef __AFXEXT_H__
#include <afxext.h>

#endif
#include "KG3DInterface.h"
#include "animationseekbar1.h"
#include "afxwin.h"
#include "KDlgPlayerCheckTool.h"
#include "KDlgSoundTag.h"

class IEKG3DAnimationTagContainer;
class IEKG3DAnimationTag;
struct MotionTagDataRuntime;

#define WM_ANIMATION_SEEK WM_USER + 0x1
#define WM_ANIMATION_SEEK_CALLBACK WM_USER + 0x2

#define GETID(a, b) ((a & 0x0000ffff) << 16 | (b &0x0000ffff))
class CPoistionButton : public CButton
{
public:
    enum LockState
    {
        LOCK_NONE = 0x0,
        LOCK_VERTICAL = 0x1,
        LOCK_HORIZONTAL = (0x1 << 1),
    };
    CPoistionButton();
    ~CPoistionButton();
    void SetLockState(LockState State) { m_LockState = State; }
    void SetLink(CWnd* pChild);
    int GetCurrentPosition();
    
    DECLARE_MESSAGE_MAP()
    LRESULT OnAnimationSeekCallBack(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    int m_nMaxFrame;
    int m_nMin;//最小的位置
    int m_nMax;//根据最大帧数算出来的最大位移
	int m_nBasePos;//相对偏移
	int m_nCurBasePos;//本次鼠标移动的相对偏移
	UINT m_uCurFrame;
private:
    LockState m_LockState;
    BOOL m_bSetCapture;
	BOOL m_bMoved;
    CPoint m_ptCaptureStartPoint;
    typedef std::set<CWnd*> ChildWndTable;
    ChildWndTable m_Children;
};

struct IKG3DSoundShell;
class KSceneModelEditorAnimationPanel : public CFormView
{
protected:
	KSceneModelEditorAnimationPanel();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(KSceneModelEditorAnimationPanel)
    enum KeyFrameType
    {
       KFT_SFX_FRAME,
       KFT_SOUND_FRAME,
       KFT_MOTION_FRAME,
       KFT_COUNT,
    };
// Form Data
public:
	//{{AFX_DATA(KSceneModelEditorAnimationPanel)
	enum { IDD = IDD_MODELEDITOR_ANIMATIONVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes 
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorAnimationPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~KSceneModelEditorAnimationPanel();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorAnimationPanel)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//TimeLine m_DialogTimeLine;
	afx_msg void OnPaint();
    void OnSFXEditorClose();
    DECLARE_EVENTSINK_MAP()
public:
	void OnMotionTagLoadAnimationseekbar1(long nID, long* lType);
    void OnMotionTagSaveAnimationseekbar1(long nID, long lType);
    void OnNewMotionTagAnimationseekbar1(long nFrame);
    void OnNewSFXTagAnimationseekbar1(long nFrame);
    void OnNewSoundTagAnimationseekbar1(long nFrame);
    void OnQueryExistTagTypeAnimationseekbar1(long lFrame, long* bSFXTag, long* bSoundTag, long* bMotionTag);
    void OnSFXSelectAnimationseekbar1(long uID);
    void OnSFXTagCopyAnimationseekbar1(long lSrc, long lDest);
    void OnSFXTagDeleteAnimationseekbar1(long nID);
    void OnSFXTagFillTagListAnimationseekbar1(long nFrame, long nCount, BSTR* strTagName, long* nID, long* nReturn);
    //void OnSFXTagLoadAnimationseekbar1(long nID, long* nPlayType, float* fPlaySpeed, long* nAutoAdaptSpeed, long* nInterruptable, long* nKeepMotionFinish, long* nNotify, long* nBindIndex, BSTR* strSFXFileName, BSTR* strTagName);
    void OnSFXTagNewAnimationseekbar1(long* nID, long nFrame);
    //void OnSFXTagSaveAnimationseekbar1(long nTagID, long nPlayType, float fPlaySpeed, long nAutoAdaptSpeed, long nInterruptable, long nKeepMotionFinish, long nNotify, LPCTSTR strTagName, long nBindID);
    void OnSoundFileSelectionAnimationseekbar1(LPCTSTR strFilePath);
    void OnSoundTagCopyAnimationseekbar1(long lSrc, long lDest);
    void OnSoundTagDeleteAnimationseekbar1(long nID);
    void OnSoundTagLoadAnimationseekbar1(long lID, long* bLoop, long* bKeep, long* bInterruptable, long* lNumCanidate);
	void OnSoundTagSaveAnimationseekbar1(long lID, long lLoop, long lKeep, long lInterruptable, long lNumCandidate);
    void OnLoadTagAnimation(LPCSTR strFileName);
	void OnMotionTagCopyAnimationseekbar1(long lSrc, long lDest);
    void OnMotionTagDeleteAnimationseekbar1(long nID);
    void OnMotionTagSaveAnimationseekbar(UINT uFrame, const MotionTagDataRuntime* &Data);
    void OnMotionTagLoadAnimationseekbar(UINT uFrame, MotionTagDataRuntime* &Data);
    LRESULT OnAnimationSeek(WPARAM wParam, LPARAM lParam);
    virtual void OnInitialUpdate();

	void InitSound();
private:
	
	IEKG3DAnimationTag* GetTag(enuTagType Type);
    int m_nLastSFXTagDialog;
    CString m_strSFXStartPath;
    std::set<int> m_SelectedSFXFrame;
    std::set<int> m_SelectedSoundFrame;
    std::set<int> m_SelectedMotionFrame;
public:
    CAnimationseekbar1 m_AnimationSeekBar;
	IEKG3DAnimationTagContainer* m_pContainer;
    afx_msg void OnBnClickedButtonPlay();
    afx_msg void OnBnClickedButtonSave();
    afx_msg void OnBnClickedButtonLoad();
    void UpdateUI();
    void SetAnimation(LPCSTR strFileName);
    void OnMotionDialogDoModalAnimationseekbar1(long nFrame);
private:
    void GetAnimationDefaultPath(LPSTR strAnimationPath);
    CButton m_ctlCursorRight;
    CPoistionButton m_ctlCursorPosition;
    CButton m_ctlCursorLeft;
	int m_nBasePosition;
public:
    afx_msg void OnBnClickedButtonLeft();
    afx_msg void OnBnClickedButtonRight();
    void OnKeyFrameCopyAnimationseekbar1(long lType, long lID, long lDest);
    void OnKeyFrameMoveAnimationseekbar1(long lType, long lID, long FrameDest);
	void OnPlaySoundFileAnimationseekbar1(LPCTSTR strSoundFilePath, float fVolume);
	DWORD m_PatternID;
	void OnLoadSoundTagCandidateAnimationseekbar1(long lFrame, long lIndex, BSTR* strSoundFileName, float* fHitRate, float* fVolume, float* fMinDis, float* fMaxDis);
	void OnSaveSoundTagCandidateAnimationseekbar1(long lFrame, long lIndex, LPCTSTR strSoundFileName, float fHitRate, float fVolume, float fMinDis, float fMaxDis);
	void OnDeleteSoundTagCandidateAnimationseekbar1(long lFrame, long lCandidate);
	afx_msg void OnBnClickedTagExport();

	CString m_strCurTaniName;
	void OnSFXTagLoadAnimationseekbar1(long nID, long* nPlayType, float* fPlaySpeed, long* nAutoAdaptSpeed, long* nInterruptable, long* nKeepMotionFinish, long* nNotify, long* nBindIndex, BSTR* strSFXFileName, BSTR* strTagName, 
		long* nWorldPos);
	void OnSFXTagSaveAnimationseekbar1(long nTagID, long nPlayType, float fPlaySpeed, long nAutoAdaptSpeed, long nInterruptable, long nKeepMotionFinish, long nNotify, LPCTSTR strTagName, long nBindID, long nWorldPos);
	void OnSoundDialogDoModalAnimationseekbar1(long nFrame);
	void OnSfxDialogDoModalAnimationseekbar1(long nFrame);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORANIMATIONPANEL_H__D57AEDC9_B2F0_457B_8911_57A47527D7B6__INCLUDED_)
