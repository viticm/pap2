#pragma once
#include "IEKG3DScene.h"
//#include "KG3DSceneClient.h"
#include "IEKG3DSceneSFXEditor.h"

enum
{
    RESOURCE_TYPE_TEXTURE,
    RESOURCE_TYPE_MODEL,
    RESOURCE_TYPE_UNKNOW
};

interface IEKG3DTexture;
interface IEKG3DModel;

class KGResourseMiniViewWnd : public CWnd, public RenderCallBack
{
    DECLARE_DYNAMIC(KGResourseMiniViewWnd)

public :
    int Create(RECT rectWnd, CWnd* pDependWnd, IEKG3DSceneSFXEditor* pDependScene);
    int Render(IEKG3DSceneOutputWnd* pOWnd);
    int Draw(LPSTR pszFileName);
    int Refluse();

public :
    KGResourseMiniViewWnd(void);
    ~KGResourseMiniViewWnd(void);

public :
    IEKG3DModel*      m_pModel;
    IEKG3DTexture*    m_pTexture;
    //
    static DWORD GID;
    DWORD m_wndID;
    int   m_nRenderFlag;
    //

    int m_nOutputWindowID;
    IEKG3DScene* m_pDependScene;

    DWORD m_resourceType;
    float m_fModelScanl;

protected :
    DECLARE_MESSAGE_MAP()
    BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
