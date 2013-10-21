
#ifndef _INCLUDE_KSCENESFXEDITORKEYFRAMEDLG_H_
#define _INCLUDE_KSCENESFXEDITORKEYFRAMEDLG_H_

#include "KSFXSliderCtrl.h"
#include "KEnterEdit.h"
#include "KG3DKeyFrameDataBase.h"//这个文件要弄成公用的
#include "KGLilisPanel.h"

enum SFX_TIME_LINE_DATA_TYPE
{
    SFX_TLDT_NONE,
    SFX_TLDT_INT,
    SFX_TLDT_FLOAT,
    SFX_TLDT_VECTOR2,
    SFX_TLDT_VECTOR3,
    SFX_TLDT_D3DXCOLOR,
    SFX_TLDT_VECTOR4,
    SFX_TLDT_SOUND,
    SFX_TLDT_VECTOR5
};

class KSceneSFXEditorKeyFrameDlg : public CDialog
{
    DECLARE_DYNAMIC(KSceneSFXEditorKeyFrameDlg)

public:
    KSceneSFXEditorKeyFrameDlg(CWnd* pParent = NULL);  
    virtual ~KSceneSFXEditorKeyFrameDlg();

    DWORD    m_dwWndState;
 
    BOOL     m_bAutoKey;
    BOOL     m_bSetKey;
    BOOL     m_bLoop;
    CString  m_strSelPath;
    CString  m_strAinPath;

    KSFXSliderCtrl m_Slider;

    KGLilisButton m_buttonMoveReset;
    KGLilisButton m_buttonRotaReset;
    KGLilisButton m_buttonMoveStart;
    KGLilisButton m_buttonMoveLeft;
    KGLilisButton m_checkPlay;
    KGLilisButton m_buttonMoveRight;
    KGLilisButton m_buttonMoveEnd;
    KGLilisButton m_buttonScalReset;

    KGLilisButton m_buttonBindSocket;
    KGLilisButton m_checkAutoKey;
    CBitmapButton m_buttonSet;

    CButton     m_buttonLoopOrOne;

    CStatic     m_staticEditState;
    KEnterEdit  m_editTransX;
    KEnterEdit  m_editTransY;
    KEnterEdit  m_editTransZ;

    CStatic     m_staticCurFrame;
    KEnterEdit  m_editTotalKeyFrameNum;

    KEnterEdit  m_editKeyFrameInterval;

    CStatic     m_staticParticleNum;

    CStatic     m_staticX;
    CStatic     m_staticY;
    CStatic     m_staticZ;

    int         m_nUpdateSliderDelay;

    CComboBox   m_comb;


    enum { IDD = IDD_SFXEDITOR_KEYFRAME };

    void UpdateUI();
    void UpdateCurFrameUI(bool bIsOnTimer = false);
    void UpdateTransformUI();

    void UpdateSliderCtrl();

    void OnSetTotalFrameNum();
    void OnSetFrameInterval();

    void OnSetTransX();
    void OnSetTransY();
    void OnSetTransZ();

    void OnSetTranslation();
    void OnSetRotation();
    void OnSetScale();

    void OnSetTransformation(BOOL bForce);

    template<class T>
    void UpdateSliderTics();
    
    template<class T>
    size_t GetTimeLineSize();

    template<class T>
    void OperateTics(int *pTics, int nCnt, int nOffset, int nOperate);

	void  ShowModelFrameBound();

    int SetDependWnd(CView* pDependWnd);
    CView*  m_pDependWnd;

    DWORD UpdateWndState();
    int  FillComb();

    TKG3DTimeLinebase* GetTimeLine() { return m_pTimeLine; }
    

public:
	void UpdateModelAniBoundLine();
    void SetTimeLine(
        const char cszTimeLineName[],
        TKG3DTimeLinebase* pTimeLine,
        SFX_TIME_LINE_DATA_TYPE t
    );

private:
    TKG3DTimeLinebase*       m_pTimeLine;
    SFX_TIME_LINE_DATA_TYPE  m_eTimeLineDataType;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual void OnOK();
    void OnCancel();



    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedButtonPlaySfx();
    afx_msg void OnBnClickedButtonMoveLeft();
    afx_msg void OnBnClickedButtonMoveStart();
    afx_msg void OnBnClickedButtonMoveRight();
    afx_msg void OnBnClickedButtonMoveEnd();
    afx_msg void OnBnClickedCheckAutokey();
    afx_msg void OnBnClickedCheckSetkey();
    afx_msg void OnBnClickedButtonSetKeyframe();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedLoop();
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnButtonClickSocketBind();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnCbnSelchangeComboAni();
    afx_msg void OnEnChangeEditTransY();
    afx_msg void OnEnChangeEditTransX();
    afx_msg void OnBnClickedMoveReset();
    afx_msg void OnBnClickedRotaReset();
    afx_msg void OnBnClickedScalReset();
public:
    afx_msg void OnCbnSetfocusAni();
};

#endif //_INCLUDE_KSCENESFXEDITORKEYFRAMEDLG_H_
