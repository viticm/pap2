////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorFrame.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:12:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORFRAME_H_
#define _INCLUDE_KSCENESFXEDITORFRAME_H_

////////////////////////////////////////////////////////////////////////////////
#include "afxwin.h"
#include "KSceneSFXEditorPanel.h"
#include "KSceneSFXEditorSelectDlg.h"
#include "KSceneSFXEditorTextureDlg.h"
#include "KSceneSFXEditorKeyFrameDlg.h"
#include "KSceneSFXEditorView.h"

//(by dengzhihui 2006年7月12日 18:16:55
#include "ChildFrm.h"
//)

class KSceneSFXEditorFrame :
	public MFCFramework::CChildFrame
{
	DECLARE_DYNCREATE(KSceneSFXEditorFrame)
protected:

    
	CSplitterWnd    m_wndSplitter;
    BOOL            m_bCreated;

	KSceneSFXEditorFrame(void);
public:
    void UpdateUI();
    KSceneSFXEditorPanel*       GetPanelView();
    KSceneSFXEditorKeyFrameDlg* GetKeysFrame();
    KGSFXMainPaneWindow*        GetRenderPane();

    KSceneSFXEditorSelectDlg    m_dlgSelect;
   // KGSfxDopeSheetDlg           m_dopeSheet;
    //KSceneSFXEditorTextureDlg   m_dlgTexture;
    //KSceneSFXEditorKeyFrameDlg  m_dlgKeyFrame;

    IEKG3DSceneSFXEditor* GetSfxScene();

	virtual ~KSceneSFXEditorFrame(void);
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	void ActivateFrame(int nCmdShow);
	void OnSetFocus(CWnd* pOldWnd);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
    afx_msg void OnUpdateSfxViewSelect(CCmdUI *pCmdUI);
public:
    afx_msg void OnSfxViewSelect();
public:
    afx_msg void OnSfxViewTexture();
public:
    afx_msg void OnUpdateSfxViewTexture(CCmdUI *pCmdUI);
public:
    afx_msg void OnSfxViewKeyframe();
public:
    afx_msg void OnUpdateSfxViewKeyframe(CCmdUI *pCmdUI);
public:
    afx_msg void OnImportSuede();
    afx_msg void OnExportSuede();
    afx_msg void OnImportSfx();
    afx_msg void OnOpenDopeSheet();
    afx_msg void OnSetCoord();
    afx_msg void OnSetLink();
};


inline IEKG3DSceneSFXEditor* GetSfxEditor() {

    CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
    KSceneSFXEditorFrame *pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    KSceneSFXEditorDoc* pDoc = dynamic_cast<KSceneSFXEditorDoc*>(pFrame->GetActiveDocument());
    if (!pDoc)
        return NULL;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    return pScene;
}

inline IEKG3DSFX* GetSfx() {

    CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
    KSceneSFXEditorFrame *pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    KSceneSFXEditorDoc* pDoc = dynamic_cast<KSceneSFXEditorDoc*>(pFrame->GetActiveDocument());
    if (!pDoc)
        return NULL;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return NULL;
    IEKG3DSFX* pSfx = NULL;
    pScene->GetSFX(&pSfx);
    return pSfx;
}

inline int GetCurrKeyFrame() {

    CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
    KSceneSFXEditorFrame *pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    return pFrame->GetKeysFrame()->m_Slider.GetCurPos();
}

inline IKG3DTimeLine* GetCurrTimeLine() {

    CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
    KSceneSFXEditorFrame *pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    return pFrame->GetKeysFrame()->GetTimeLine();
}

inline void SetCurrTimeLine(IKG3DTimeLine* tl, const char name[], SFX_TIME_LINE_DATA_TYPE type) {

    CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
    KSceneSFXEditorFrame *pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    return pFrame->GetKeysFrame()->SetTimeLine(name, (TKG3DTimeLinebase*)tl, type);
}

inline void UpdateKeyFrameUI()
{
    CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
    KSceneSFXEditorFrame *pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    return pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

#define GET_EDIT_INFO() \
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd(); \
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame(); \
    KSceneSFXEditorDoc* pDoc = dynamic_cast<KSceneSFXEditorDoc*>(pFrame->GetActiveDocument()); \
    IEKG3DSceneSFXEditor *pScene = NULL; \
    IEKG3DSFX* pSfx = NULL; \
    if (pDoc) \
    { \
        pDoc->GetSceneSFXEditor(&pScene); \
    } \
    if (pScene) \
    { \
        pScene->GetSFX(&pSfx); \
    } \


extern void SFXUINeedUpdate();
//(by dengzhihui 2006年7月5日 16:08:01
#define GET_SFX_EDITOR_RET(n)\
CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();\
KSceneSFXEditorFrame *pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();\
KSceneSFXEditorDoc* pDoc = dynamic_cast<KSceneSFXEditorDoc*>(pFrame->GetActiveDocument());\
if (!pDoc)\
    return n;\
IEKG3DSceneSFXEditor *pScene = NULL;\
pDoc->GetSceneSFXEditor(&pScene);\
if (pScene == NULL)\
    return n

#define GET_SFX_EDITOR()\
CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();\
KSceneSFXEditorFrame *pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();\
KSceneSFXEditorDoc* pDoc = dynamic_cast<KSceneSFXEditorDoc*>(pFrame->GetActiveDocument());\
if (!pDoc)\
    return;\
IEKG3DSceneSFXEditor *pScene = NULL;\
pDoc->GetSceneSFXEditor(&pScene);\
if (pScene == NULL)\
    return

#define GETSCENE(pSFXEditor)\
	IEKG3DScene* p3DScene = NULL;\
	pSFXEditor->GetScene(&p3DScene);\
	_ASSERTE(p3DScene);\
	if (!p3DScene) return


#define GETSELECT(pSceneEditorBase) \
	IEKG3DSceneSelectBase* pSelector = NULL;\
	pSceneEditorBase->GetSceneSelector(&pSelector);\
	_ASSERTE(pSelector);\
	if (!pSelector) return

inline DWORD GetEditState(IEKG3DSceneEditorBase* pBase){\
	DWORD dwState = 0;\
	pBase->GetEditState(&dwState);\
	return dwState;\
}
//)
#endif //_INCLUDE_KSCENESFXEDITORFRAME_H_
