#pragma once


#include "KGLilisSlipPanel.h"
#include "IEEditor.h"

#define GET_HEIGHT_EVENT 100

interface IEKG3DSceneSceneEditor;

class CColorWnd : public CWnd
{
    DECLARE_DYNAMIC(CColorWnd)
public :
    CColorWnd();
    void SetColor(DWORD dwColor);

public :
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg LRESULT OnColorChange(WPARAM wParam, LPARAM lParam);

protected :
    DWORD m_dwColor;
};

class KSceneSceneEditorWaterPanel : public KGLilisSlipPanel<CDialog>
{
	DECLARE_DYNAMIC(KSceneSceneEditorWaterPanel)

public:
	KSceneSceneEditorWaterPanel(CWnd* pParent = NULL);
	virtual ~KSceneSceneEditorWaterPanel();

    IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
    IEKG3DWaterEffect* m_pWaterEffect;

public:
    void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
    {
        m_lpSceneSceneEditor = pSceneEdit;
        
        if (m_lpSceneSceneEditor)
            m_pWaterEffect = m_lpSceneSceneEditor->GetWaterEffect();
        else
            m_pWaterEffect = NULL;
    }

    void UpdateCtrl(int nDetail);

    BOOL OnInitDialog();
	enum { IDD = IDD_SCENEEDITOR_WATER_NEW };

    CString m_strSurfaceTextureName;
    CString m_strBumpTextureName;
    CComboBox m_ComboDetail;

    CColorWnd m_ColorWnd1;
    CColorWnd m_ColorWnd2;
    CColorWnd m_ColorWnd3;

    int   m_nHeight;
    float m_fFogStart;
    float m_fFogEnd;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonSurface();
    afx_msg void OnBnClickedButtonBump();
    afx_msg void OnStnClickedColorDif();
    afx_msg void OnStnClickedColorSpe();
    afx_msg void OnStnClickedColorFog();
    afx_msg void OnCbnSelchangeWaterDetail();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor); 
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedButtonReloadShader();
    afx_msg void OnBnClickedCheckHide();
    afx_msg void OnBnClickedCheckEdge();
    afx_msg void OnBnClickedCheckSet();
    afx_msg void OnBnClickedCheckGet();
    afx_msg void OnEnChangeEditHeight();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnEnChangeEditFotStart();
    afx_msg void OnEnChangeEditFogEnd();
    afx_msg void OnBnClickedButtonDefHeight();
};
