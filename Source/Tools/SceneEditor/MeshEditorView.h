#pragma once

#include "KSceneModelEditorSword2CheckTool.h"
#include "KSceneModelEditorSFXSpeedTreeDialog.h"
//(by dengzhihui 2006年9月15日 11:54:59
//#include "InputProxy.h"
#include "BaseView.h"
//)
// KSceneModelEditorView 视图
class KModelEditorPlayerToolsDialog;
class KDlgMDLCheckTool;
class KDlgPlayerCheckTool;
class KDlgFilterFileView;
class IEKG3DSceneModelEditor; //by dengzhihui Refactor 2006年11月24日
class KSceneModelEditorLoadAni;



//(by dengzhihui 2006年11月1日 17:06:29
class CModelSampleSink : public IEKG3DSimpleSink
{
public:
	CModelSampleSink();
	void Init(KSceneModelEditorSFXSpeedTreeDialog* pDialogSpeedTree)
	{
		m_pDialogSpeedTree = pDialogSpeedTree;
	}
	virtual void SinkFunc(INT nSinkID);
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint){}
private:
	KSceneModelEditorSFXSpeedTreeDialog* m_pDialogSpeedTree;
};
//)

static TCHAR *s_strDefaultBindSocketType[] =
{
	"\\f1\\",
	"\\f2\\",
	"\\f3\\",
	"\\m1\\",
	"\\m2\\",
	"\\m3\\",
	"\\npc_source\\",
	"Default"
};

static const size_t s_NumTypes = sizeof(s_strDefaultBindSocketType) / sizeof(TCHAR*);

class KSceneModelEditorView : public CBaseView
{
	DECLARE_DYNCREATE(KSceneModelEditorView)

protected:
	
	int	   m_nOutputWindowID;
	KSceneModelEditorView();           // 动态创建所使用的受保护的构造函数
	virtual ~KSceneModelEditorView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制此视图
	int&		GetOutputWindowID() { return m_nOutputWindowID; }
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//{{AFX_VIRTUAL(KSceneEditorView)
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(KSceneEditorView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnModelopenadd();
	afx_msg void OnSFXNew();
	afx_msg void OnSelect();
	afx_msg void OnPlayAll();
	afx_msg void OnLoadModel();
	afx_msg void OnSkillAnimationPanel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	KSceneModelEditorSFXSpeedTreeDialog* m_pDialogSpeedTree;
	KSceneModelEditorSword2CheckTool* m_lpDialogSword2Check;
	KModelEditorPlayerToolsDialog* m_lpDialogPlayerTools;
public:
	void OnFilterOpenFile(LPCSTR strFileName);
	KSceneModelEditorLoadAni *m_lpDialogSkillAnimation;
	afx_msg void OnPlayerTools();
	afx_msg void OnDestroy();
    afx_msg void OnShowbone();
	afx_msg void OnShowsocket();
	afx_msg void OnShowgrid();
	afx_msg void OnUpdateShowgrid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShowbone(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShowsocket(CCmdUI *pCmdUI);
	afx_msg void OnSword2check();
	afx_msg void OnUpdatePersfectcamer(CCmdUI *pCmdUI);
	afx_msg void OnEditproperty();
	afx_msg void OnUpdateEditproperty(CCmdUI *pCmdUI);
	afx_msg void OnSeteffect();
    afx_msg void OnSceneSetting();
	afx_msg void OnMDLCheckTool();
	afx_msg void OnPlayerCheckTool();
	afx_msg void OnChangeMovingMode();
	afx_msg void OnEnableHairLOD();
	afx_msg void OnUpdateHairLOD(CCmdUI *pCmdUI);

	//(by dengzhihui 2006年9月15日 11:54:30
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnInitialUpdate();
	afx_msg void OnPopMenuMove();
	afx_msg void OnPopMenuRotate();
	afx_msg void OnPopMenuScale();
	afx_msg void OnActionUp();	
	afx_msg void OnActionDown();	
	afx_msg void OnActionLeft();	
	afx_msg void OnActionRight();
	afx_msg void OnDeleteSelectedObject();
	afx_msg void OnIsolate();
	afx_msg void OnRefreshshader();
	afx_msg void OnFilterOpen();
	afx_msg void OnShowGrid();
	afx_msg void OnUpdateShowGrid(CCmdUI *pCmdUI);
	afx_msg void OnConvertSpeedTree();

	IEKG3DSceneModelEditor* GetDocScene();
public:
	afx_msg void OnSpeedtreePanel();
    LRESULT OnSFXEditorCloseInvoke(WPARAM wParam, LPARAM lParam);
	void Zoom();
private:
	struct BindPair
	{
		std::string strSocket;
		std::string strMesh;
		IKG3DModel *pModel;
	};

	void InitDefaultBindSocket(DWORD dwType);
	KDlgMDLCheckTool *m_pMDLCheckTool;
	KDlgPlayerCheckTool *m_pPlayerCheckTool;
	KDlgFilterFileView *m_pFilterFileDlg;
	BOOL m_bBindAll;
	std::vector<BindPair> m_DefaultBindMesh[s_NumTypes];
	bool m_bInited[s_NumTypes];
public:
	afx_msg void OnBillcloud();
	afx_msg void OnBindAll();
	afx_msg void OnPlayernormaltools();
	afx_msg void OnMotionProcess();
	HRESULT LoadBatchToolConfig(IEKG3DAnimationTagContainer *pTagContainer);
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};



