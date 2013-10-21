

//
// ZhaoXinYu 2006-8-25
//

#pragma once
#include <list>

#define WM_SFX_SLIDER_OPERATE_TICS WM_USER + 200

#define SFX_SLIDER_TICS_COPY 1
#define SFX_SLIDER_TICS_MOVE 2
#define SFX_SLIDER_TICS_DEL  3

typedef struct tagSLIDER_OPERATE_TICS
{
    int *pTicsList;
    int nCnt;
    int nOffset;
    int nOperate;
}SLIDER_OPERATE_TICS;

class KSFXSliderCtrl : public CStatic
{
    DECLARE_DYNAMIC(KSFXSliderCtrl)
public:
    int SetTic(int nTic);
    int SetRange(int nMin, int nMax);
    int SetPos(int nPos);

    int GetSliderRect(RECT *pRc);

    int ClearTics();
    int GetCurPos();
    int GetRange(int &nMin, int &nMax);

    int Init();

private:

    int  DrawFrame(CDC* pDC, RECT rect, COLORREF color, int nFlag = FALSE);
    BOOL IsTic(int nPos);

private:
    int     m_nMinPos;
    int     m_nMaxPos;
    int     m_nCurPos;
    int     m_nPage;

    RECT    m_rcChannel;
    RECT    m_rcSlider;
    float   m_fStep;

    BOOL m_bShowTic;

    enum SLIDER_OPERATOR_STATE
    {
        SOS_NONE,
        SOS_DRAG_SLIDER,
        SOS_SELECT_TIC,
        SOS_COPY_SELECTED_TIC,
        SOS_MOVE_SELECTED_TIC,
    } m_SliderOpState;

    POINT   m_StartPos;
    POINT   m_EndPos;

    int m_nStartPos;

    void UpdateSelectedTic();
    void UpdateMovingTic(int nOffset);

    vector<int>   m_listTic;
    vector<int>   m_listSelectTic;
    vector<int>   m_listMovingTic;

    //
    CFont         m_font;

public:
    KSFXSliderCtrl(void);
    ~KSFXSliderCtrl(void);

    DECLARE_MESSAGE_MAP()

public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void PreSubclassWindow();
    virtual BOOL OnEraseBkgnd(CDC* pDC);
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
