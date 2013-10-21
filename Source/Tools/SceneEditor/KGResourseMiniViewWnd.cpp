#include "stdafx.h"
#include "KGResourseMiniViewWnd.h"
//#include "KG3DGraphicsTool.h"
#include "IEKG3DModelTable.h"
#include "../EditInterface/IEOther.h" //by dengzhihui Refactor 2006年11月2日
#include "IEKG3DSceneOutputWnd.h"
//#include "SceneEditorCommon.h"//by dengzhihui Refactor 2006年12月5日
#include "IEEditor.h"
#include "IEKG3DTexture.h"
#include "IEKG3DTextureTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGResourseMiniViewWnd, CWnd)
KGResourseMiniViewWnd::KGResourseMiniViewWnd(void)
{
    m_nOutputWindowID = -1;
    m_wndID = GID++;
    m_pDependScene = NULL;
    m_resourceType = RESOURCE_TYPE_UNKNOW;
    m_pModel = NULL;
    m_pTexture = NULL;
    m_fModelScanl = 1.0f;
    m_nRenderFlag = true;
}

KGResourseMiniViewWnd::~KGResourseMiniViewWnd(void)
{
    SAFE_RELEASE(m_pModel);
    SAFE_RELEASE(m_pTexture);
}

BEGIN_MESSAGE_MAP(KGResourseMiniViewWnd, CWnd)
    ON_WM_SIZE()
    //ON_WM_PAINT()
END_MESSAGE_MAP()

void KGResourseMiniViewWnd::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    Refluse();
}

int KGResourseMiniViewWnd::Refluse()
{
    if (m_pDependScene)
        m_pDependScene->OnResizeWindow(NULL);
    return TRUE;
}

BOOL KGResourseMiniViewWnd::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_MOUSEWHEEL)
    {
        short zDelay = HIWORD(pMsg->wParam);
        m_fModelScanl += zDelay / 1000.0f;

    }
    return CWnd::PreTranslateMessage(pMsg);
}

DWORD KGResourseMiniViewWnd::GID = 3302;
int KGResourseMiniViewWnd::Create(RECT rectWnd, CWnd* pDependWnd, IEKG3DSceneSFXEditor* pDependScene)
{
    int nResult  = false;
    int nRetCode = false;
    HRESULT hRetCode = E_FAIL;

    KG_PROCESS_ERROR(pDependScene);
    KG_PROCESS_ERROR(pDependWnd);

    hRetCode = pDependScene->GetScene(&m_pDependScene);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    nRetCode = CWnd::Create(
        "BUTTON",
        NULL,
        WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        rectWnd,
        pDependWnd,
        m_wndID
    );
    KG_PROCESS_ERROR(nRetCode);
    SetParent(pDependWnd);
    pDependScene->RegistCalkBackRender(this);


    m_pDependScene->AddOutputWindow(
        "Resource Mini View",
        m_hWnd,
        OUTPUTWND_RESOURCE_MINIVIEW,
        0,
        &m_nOutputWindowID
     );

    wndID = m_nOutputWindowID;

    nResult = true;
Exit0:
    return nResult;
}

int KGResourseMiniViewWnd::Render(IEKG3DSceneOutputWnd* pOWnd)
{
    int nResult  = false;
    int nRetCode = false;

    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

    if (!g_pd3dDevice)
    {
        //Get3DEngineInterface((void **)(&pManager));
        pManager->GetD3DDevice(&g_pd3dDevice);
    }

    KG_PROCESS_SUCCESS(!m_nRenderFlag);
    m_pDependScene->RenderWindowBegin(pOWnd,TRUE);

    g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    DWORD dwViewPortWidth  = 0;
    DWORD dwViewPortHeight = 0;
    pOWnd->GetViewPortWidthHeight(&dwViewPortWidth, &dwViewPortHeight);
    float fWidth = (float)dwViewPortWidth;
    float fHeight = (float)dwViewPortHeight;
    if (m_resourceType == RESOURCE_TYPE_MODEL && m_pModel)
    {
        IEKG3DCamera* pCamera = g_pGraphicsTool->GetCamera();	//by dengzhihui Refactor 2006年11月2日
        D3DXVECTOR3 vScanl = D3DXVECTOR3(m_fModelScanl, m_fModelScanl, m_fModelScanl);
        D3DXVECTOR3 vModel;

        D3DXVECTOR3 vCameraPosSave;
        D3DXVECTOR3 vLookAtSave;
        
        pCamera->GetPosition(&vCameraPosSave);
        pCamera->GetLookAtPosition(&vLookAtSave);

        KG_PROCESS_ERROR(pCamera);
        m_pModel->GetTranslation(&vModel);
        pCamera->SetPosition(D3DXVECTOR3(0.0f, 2000.0f, 1500.0f));
        pCamera->SetLookAtPosition(vModel);
        m_pModel->SetScaling(&vScanl);
        m_pModel->FrameMove();
        m_pModel->Render(0, NULL);

        pCamera->SetPosition(vCameraPosSave);
        pCamera->SetLookAtPosition(vLookAtSave);
    }
    else if (m_resourceType == RESOURCE_TYPE_TEXTURE && m_pTexture)
    {


        DWORD dwID = 0;
        m_pTexture->GetID(&dwID);
        g_pGraphicsTool->DrawScreenRect(
            &D3DXVECTOR2(0, 0),
            &D3DXVECTOR2(fWidth, fHeight),
            0.5f,
            1.0f,
            1.0f,
            0xFFFFFFFF,
            dwID
       );
    }
    else
    {
        //ATLTRACE2(TEXT("UNKNOW"));
    }

    D3DCOLOR color;
    if (::GetFocus() == m_hWnd)
        color = D3DCOLOR_ARGB(255, 255, 0, 0);
    else
        color = D3DCOLOR_ARGB(255, 50, 57, 57);

    g_pGraphicsTool->DrawFocusRect(
        *pOWnd->GetViewPort(),
        4,
        color
        );

    m_pDependScene->RenderWindowEnd(pOWnd);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KGResourseMiniViewWnd::Draw(LPSTR pszFileName)
{
    int nResult  = false;
    int nRetCode = false;
    IEKG3DTextureTable *piTable = NULL;
    IEKG3DModelTable *pModelTable = NULL;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    char* pExt = NULL;
    KG_PROCESS_ERROR(pszFileName);

    //Get3DEngineInterface((void **)(&pManager));

    pExt = strrchr(pszFileName, '.');
    KG_PROCESS_ERROR(pExt);

    if (
        (!strcmpi(pExt, TEXT(".mdl")))  ||
        (!strcmpi(pExt, TEXT(".mesh"))) ||
        (!strcmpi(pExt, TEXT(".sfx")))
     )
    {
        SAFE_RELEASE(m_pModel);
        pManager->GetIEKG3DModelTable(&pModelTable);
        HRESULT hR = pModelTable->IELoadResourceFromFile(
            pszFileName, 0, 0, (IKG3DResourceBase **)&m_pModel
        );
        KG_COM_PROCESS_ERROR(hR);
        KG_PROCESS_ERROR(m_pModel);

        m_resourceType = RESOURCE_TYPE_MODEL;
    }
    else if (
        (!strcmpi(pExt, TEXT(".tga"))) ||
        (!strcmpi(pExt, TEXT(".bmp"))) ||
        (!strcmpi(pExt, TEXT(".jpg"))) ||
        (!strcmpi(pExt, TEXT(".png"))) ||
        (!strcmpi(pExt, TEXT(".dds")))
    )
    {
        SAFE_RELEASE(m_pTexture);
        pManager->GetIEKG3DTextureTable(&piTable);
        HRESULT hR = piTable->IELoadResourceFromFile(
            pszFileName, 0, 0, (IKG3DResourceBase **)&m_pTexture
        );
        KG_COM_PROCESS_ERROR(hR);
        KG_PROCESS_ERROR(m_pTexture);
        m_resourceType = RESOURCE_TYPE_TEXTURE;
    }
    else
    {
        m_resourceType = RESOURCE_TYPE_UNKNOW;
    }


    nResult = true;
Exit0:
    return nResult;
}