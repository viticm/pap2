#pragma once

interface IEKG3DTerrain;

class KSceneSceneEditorDialogGroundTextureUV : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogGroundTextureUV)

	IEKG3DTerrain* m_pTerrain;
	int m_nLayer,m_nIndex;
public:
	void SetGroundTexture(IEKG3DTerrain* pTerrain,int nLayer,int nIndex)
	{
		m_pTerrain = pTerrain;
		m_nLayer = nLayer;
		m_nIndex = nIndex;
	}

	KSceneSceneEditorDialogGroundTextureUV(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogGroundTextureUV();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_GROUNDTEXTUREUV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	float m_fXZAngle;
	float m_fYAngle;
	float m_fScaleU;
	float m_fScaleV;

public:
	afx_msg void OnEnChangeEditXzangle();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnChangeEditYangle();
	afx_msg void OnEnChangeEditScaleu();
	afx_msg void OnEnChangeEditScalev();
};
