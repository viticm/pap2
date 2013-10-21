////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorTimeView.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:16:11
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORTIMEVIEW_H_
#define _INCLUDE_KSCENESFXEDITORTIMEVIEW_H_

////////////////////////////////////////////////////////////////////////////////
#include "KSceneSFXEditorParticlePage.h"
#include "KSceneSFXEditorBillboardPage.h"
#include "KSceneSFXEditorModel.h"

enum ETimeLineType
{
    TLT_PARTICLE_COLOR,
    TLT_PARTICLE_WIDTH,
    TLT_PARTICLE_HEIGHT,
    TLT_PARTICLE_TEXTURE_FRAME_INDEX,

    TLT_LAUNCHER_EMIT_NUM,
    TLT_LAUNCHER_SPEED_MIN,
    TLT_LAUNCHER_SPEED_MAX,
    TLT_LAUNCHER_LIFE_MIN,
    TLT_LAUNCHER_LIFE_MAX,
    TLT_LAUNCHER_SCATTER_MIN,
    TLT_LAUNCHER_SCATTER_MAX,
    TLT_LAUNCHER_TRANSFORMATION,

    TLT_FORCE_FIELD_FORCE,

    TLT_BILLBOARD_TRANSFORMATION,
    TLT_BILLBOARD_COLOR,
    TLT_BILLBOARD_WIDTH,
    TLT_BILLBOARD_HEIGHT,
    TLT_BILLBOARD_TEXTURE_FRAME_INDEX_1,
    TLT_BILLBOARD_TEXTURE_FRAME_INDEX_2,
    TLT_BILLBOARD_ROTATION,
    TLT_BILLBOARD_UV,
};
// KSceneSFXEditorTimeView form view

class KSceneSFXEditorTimeView : public CFormView
{
	DECLARE_DYNCREATE(KSceneSFXEditorTimeView)

    ETimeLineType   m_eCurTimeLineType;

protected:
	KSceneSFXEditorTimeView();           // protected constructor used by dynamic creation
	virtual ~KSceneSFXEditorTimeView();

    KSceneSFXEditorParticlePage*    m_pParticlePage;
    KSceneSFXEditorBillboardPage*   m_pBillboardPage;
    KSceneSFXEditorModel*           m_pModelPage;

    void UpdateUIForParticlePage();
    void UpdateUIForBillboardPage();
    void UpdateUIForModelPage();

    void DeleteKeyFrameParticlePage();
    void DeleteKeyFrameBillboardPage();
    void DeleteKeyFrameModelPage();

    void MoveKeyFrameLeftParticlePage();
    void MoveKeyFrameLeftBillboardPage();
    void MoveKeyFrameLeftModelPage();

    void MoveKeyFrameRightParticlePage();
    void MoveKeyFrameRightBillboardPage();
    void MoveKeyFrameRightModelPage();


    void OnSetTransformationX();
    void OnSetTransformationY();
    void OnSetTransformationZ();

	void OnSetTranslation();
	void OnSetRotation();
	void OnSetScale();

public:
	enum { IDD = IDD_SFXEDITOR_TIMELINE };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

    CStatic     m_staticKeyFrameNum;
    CStatic     m_staticCurrentKeyFrame;

    CSliderCtrl m_sliderTimeLine;

    CButton     m_checkPlaySFX;

    CButton     m_buttonMoveStart;
    CButton     m_buttonMoveEnd;

    CButton     m_buttonPreFrame;
    CButton     m_buttonNextFrame;

    BOOL        m_bAuto0;

    CStatic     m_staticTransStatus;
    KEnterEdit  m_editTransX;
    KEnterEdit  m_editTransY;
    KEnterEdit  m_editTransZ;

    BOOL        m_bAutoKey;
    CButton     m_buttonSetKeyFrame;
    BOOL        m_bAbs;


    void UpdateUI();

    void SetParticlePage( KSceneSFXEditorParticlePage *pPage )
    {
        ASSERT( pPage );
        m_pParticlePage = pPage;
    }

    void UpdateEditStatusUI();
    void UpdateSelectObjUI();
    void SetTransformation(BOOL bForceSet);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual void OnInitialUpdate();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnBnClickedButtonDelKeyFrame();
    afx_msg void OnBnClickedButtonMoveKeyFrameLeft();
    afx_msg void OnBnClickedButtonMoveKeyFrameRight();
    afx_msg void OnBnClickedRadioPlaySfx();
    afx_msg void OnBnClickedCheckPlaySfx();
    afx_msg void OnBnClickedButtonMoveStart();
    afx_msg void OnBnClickedButtonMoveEnd();
    afx_msg void OnBnClickedCheckAuto0();
    afx_msg void OnBnClickedCheckAutokey();
    afx_msg void OnBnClickedButtonSetKeyframe();
protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};




#endif //_INCLUDE_KSCENESFXEDITORTIMEVIEW_H_
