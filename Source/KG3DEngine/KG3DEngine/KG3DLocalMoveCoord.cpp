////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMoveCoordinate.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-11-7 19:13:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DLocalMoveCoord.h"
#include "KG3DSFXParticleLauncher.h"
#include "KG3DGraphicsTool.h"
#include "..\EditInterface\IEKG3DSceneSFXEditor.h"
#include "KG3DSceneEditorBase.h"

#include "KG3DSceneFunctions.h"
#include "KG3DCollocator.h"//g_Switch

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//KG3DMoveCoordinate::KG3DMoveCoordinate()
//{
//    m_pCoordMesh = NULL;
//    m_pAttachScene      = NULL;
//    m_dwNumMatrials     = 0;
//    m_dwCurrSelCoord    = 0xffffffff;
//    m_fZoom             = 2;
//    m_nMoveFlag         = FALSE;
//    m_dwMode            = KG3DCOORD_WORLD;
//
//    m_EntityList.Clear();
//    KG3DD3DXLoadMeshFromX(MOVE_COORD_MESH_PATH, D3DXMESH_MANAGED, g_pd3dDevice, 
//        NULL, NULL, NULL, &m_dwNumMatrials, &m_pCoordMesh);
//
//}

KG3DMoveCoordinate::KG3DMoveCoordinate( KG3DScene& scene )
:m_pAttachScene(&scene)
{
	m_pCoordMesh = NULL;
	//m_pAttachScene      = NULL;
	m_dwNumMatrials     = 0;
	m_dwCurrSelCoord    = 0xffffffff;
	m_fZoom             = 2;
	m_nMoveFlag         = FALSE;
	m_dwMode            = KG3DCOORD_WORLD;

	//m_EntityList.Clear();
	KG3DD3DXLoadMeshFromX(MOVE_COORD_MESH_PATH, D3DXMESH_MANAGED, g_pd3dDevice, 
		NULL, NULL, NULL, &m_dwNumMatrials, &m_pCoordMesh);
}
KG3DMoveCoordinate::~KG3DMoveCoordinate()
{
    SAFE_RELEASE(m_pCoordMesh);
}

//KG3DMoveCoordinate* KG3DMoveCoordinate::GetCoordinate()
//{
//    static KG3DMoveCoordinate MoveCoordinate;
//    return &MoveCoordinate;
//}

HRESULT KG3DMoveCoordinate::SetMode(DWORD dwMode)
{
    m_dwMode = dwMode;
    return S_OK;
}

HRESULT KG3DMoveCoordinate::Zoom(FLOAT fScanl)
{
    if (m_fZoom + fScanl > 0.0f)
        m_fZoom += fScanl;
    return S_OK;
}

//HRESULT KG3DMoveCoordinate::AttachEntity(KG3DScene* pScene, std::list<KG3DSceneEntity> listEntitys)
//{
//    m_EntityList = listEntitys;
//
//    KG_PROCESS_ERROR(pScene);
//    m_pAttachScene = pScene;
//
//Exit0:
//    return S_OK;
//}

//HRESULT KG3DMoveCoordinate::AttachEntity(KG3DScene* pScene, KG3DSceneEntity Entity)
//{
//    m_EntityList.Clear();
//    m_EntityList.AddEntity(Entity);
//
//    KG_PROCESS_ERROR(pScene);
//    m_pAttachScene = pScene;
//    
//Exit0:
//    return S_OK;
//}

//HRESULT KG3DMoveCoordinate::ReleaseAttach()
//{
//    m_EntityList.Clear();
//    return S_OK;
//}

HRESULT KG3DMoveCoordinate::MoveBegin()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    if (m_dwCurrSelCoord == 0xFFFFFFFF)
        return E_FAIL;

    D3DXVECTOR3  vCrossXZ;
    D3DXVECTOR3  vCrossYZ;
    D3DXVECTOR3  vCrossXY;

    D3DXPLANE    planeXZ;
    D3DXPLANE    planeXY;
    D3DXPLANE    planeYZ;

    D3DXVECTOR3  vPoint = D3DXVECTOR3(m_matCoord._41, m_matCoord._42, m_matCoord._43);
    D3DXVECTOR3  vNorXZ = D3DXVECTOR3(m_matCoord._21, m_matCoord._22, m_matCoord._23);
    D3DXVECTOR3  vNorXY = D3DXVECTOR3(m_matCoord._31, m_matCoord._32, m_matCoord._33) * -1.f;
    D3DXVECTOR3  vNorYZ = D3DXVECTOR3(m_matCoord._11, m_matCoord._12, m_matCoord._13);

    D3DXPlaneFromPointNormal(
        &planeXZ,
        &vPoint,
        &vNorXZ
        );
    D3DXPlaneFromPointNormal(
        &planeXY,
        &vPoint,
        &vNorXY
        );
    D3DXPlaneFromPointNormal(
        &planeYZ,
        &vPoint,
        &vNorYZ
        );

    D3DXPlaneNormalize(&planeYZ, &planeYZ);
    D3DXPlaneNormalize(&planeXZ, &planeXZ);
    D3DXPlaneNormalize(&planeXY, &planeXY);

    D3DXVECTOR3 vLook;
    D3DXVECTOR3 vCamera;
    D3DXVECTOR3 vOrg;
    D3DXVECTOR3 vDir;

    IEKG3DSceneOutputWnd *piCurOutputWnd = NULL;
    KG3DSceneOutputWnd *pCurOutputWnd = NULL;
    hRetCode = m_pAttachScene->GetCurOutputWnd(&piCurOutputWnd);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
	{
		pCurOutputWnd = static_cast<KG3DSceneOutputWnd *>(piCurOutputWnd);

		pCurOutputWnd->GetCamera().GetPosition(&vCamera);
		pCurOutputWnd->GetPickRay(&vOrg, &vDir, NULL);
		pCurOutputWnd->GetCamera().GetLookAtPosition(&vLook);


		D3DXPlaneIntersectLine(
			&vCrossXZ, 
			&planeXZ,
			&vOrg,
			&(vOrg + vDir * 10000000.0f)
			);
		D3DXPlaneIntersectLine(
			&vCrossXY,
			&planeXY,
			&vOrg,
			&(vOrg + vDir * 10000000.0f)
			);
		D3DXPlaneIntersectLine(
			&vCrossYZ, 
			&planeYZ,
			&vOrg,
			&(vOrg + vDir * 10000000.0f)
			);


		D3DXVECTOR3 vLoodFor;
		D3DXVec3Normalize(&vLoodFor, &(vCamera - vLook));

		FLOAT fDotToXY = D3DXPlaneDotNormal(&planeXY, &vLoodFor);
		FLOAT fDotToYZ = D3DXPlaneDotNormal(&planeYZ, &vLoodFor); 
		FLOAT fDotToXZ = D3DXPlaneDotNormal(&planeXZ, &vLoodFor);

		m_matCoordPane = m_matCoord;


		switch (m_dwCurrSelCoord)
		{
		case 0 :    // y
        case 6 :
			//KG_PROCESS_ERROR(fLenToXY < INT_MAX || fLenToYZ < INT_MAX);
			if (fDotToXY >= fDotToYZ)
			{
				m_currSelPane = planeXY;
				m_vPrevCross = vCrossXY;
			}
			else
			{
				m_currSelPane = planeYZ;
				m_vPrevCross = vCrossYZ;
			}
			break;
		case 1 :    // z
        case 7 :
			//KG_PROCESS_ERROR(fLenToXZ < INT_MAX || fLenToYZ < INT_MAX);
			if (fDotToXZ >= fDotToYZ)
			{
				m_currSelPane = planeXZ;
				m_vPrevCross = vCrossXZ;
			}
			else
			{
				m_currSelPane = planeYZ;
				m_vPrevCross = vCrossYZ;
			}
			break;
		case 2 :    // x
        case 8 :
			//KG_PROCESS_ERROR(fLenToXY < INT_MAX || fLenToXZ < INT_MAX);
			if (fDotToXY >= fDotToXZ)
			{
				m_currSelPane = planeXY;
				m_vPrevCross = vCrossXY;
			}
			else
			{
				m_currSelPane = planeXZ;
				m_vPrevCross = vCrossXZ;
			}
			break;
		case 3 :    // yz
			//KG_PROCESS_ERROR(fLenToYZ < INT_MAX);
			m_currSelPane = planeYZ;
			m_vPrevCross = vCrossYZ;
			break;
		case 4 :    // zx
			//KG_PROCESS_ERROR(fLenToXZ < INT_MAX);
			m_currSelPane = planeXZ;
			m_vPrevCross = vCrossXZ;
			break;
		case 5 :    // xy
			//KG_PROCESS_ERROR(fLenToXY < INT_MAX);
			m_currSelPane = planeXY;
			m_vPrevCross = vCrossXY;
			break;
		default :
			KG_PROCESS_ERROR(false);
			break;
		}

		m_nMoveFlag = TRUE;
	}
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DMoveCoordinate::MoveEnd()
{
    HRESULT hResult  = E_FAIL;

    m_nMoveFlag = FALSE;

    hResult = S_OK;
//Exit0:
    return hResult;
}

float KG3DMoveCoordinate::GetScalingFactor()
{
    IEKG3DSceneOutputWnd *piCurOutputWnd = NULL;
    m_pAttachScene->GetCurOutputWnd(&piCurOutputWnd);
    ASSERT(piCurOutputWnd);
    
    KG3DCamera* pCamera = (KG3DCamera*)piCurOutputWnd->GetIECamera();
    ASSERT(pCamera);

    D3DXVECTOR3 vCenter = D3DXVECTOR3(m_matCoord._41, m_matCoord._42, m_matCoord._43);
    D3DXVECTOR3 vCamera = pCamera->GetPosition();

    float fDis = D3DXVec3Length(&(vCenter - vCamera));
    if (pCamera->IsPerspective())
        return fDis / 700.f;
    return pCamera->GetOrthogonalData().fWidth / 400.f ;

}

HRESULT KG3DMoveCoordinate::FrameMove()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IEKG3DSceneOutputWnd *piCurOutputWnd = NULL;
    
    D3DXVECTOR3 vCoordX;
    D3DXVECTOR3 vCoordY;
    D3DXVECTOR3 vCoordZ;

    switch (m_dwMode) 
    {
    case KG3DCOORD_WORLD :
        {
            /*D3DXMATRIX matEntity = m_EntityList.GetWorldMatrix();
            D3DXMatrixIdentity(&m_matCoord);
            m_matCoord._41 = matEntity._41;
            m_matCoord._42 = matEntity._42;
            m_matCoord._43 = matEntity._43;*/
			D3DXVECTOR3 vCenter(0,0,0);
			KSF::GetSelectionCenter(m_pAttachScene->GetSelectionTool(), vCenter);
			D3DXMatrixTranslation(&m_matCoord, vCenter.x, vCenter.y, vCenter.z);
        }
        break;
    case KG3DCOORD_LOCAL :
        {
            //m_matCoord = m_EntityList.GetWorldMatrix();
			D3DXVECTOR3 vCenter(0,0,0);
			KSF::GetSelectionCenter(m_pAttachScene->GetSelectionTool(), vCenter);
			D3DXMatrixTranslation(&m_matCoord, vCenter.x, vCenter.y, vCenter.z);

           D3DXVECTOR3 t;
            D3DXVECTOR3 s;
            D3DXMATRIX  q;
            D3DXMATRIX  m;
            MatrixExract(s, t, q, &m_matCoord);
            D3DXMatrixTranslation(&m, t.x, t.y, t.z);
            m_matCoord = q * m;
        }
        break;
    default :
        ASSERT(false);
        break;
    }

    vCoordX = D3DXVECTOR3(m_matCoord._11, m_matCoord._12, m_matCoord._13);
    vCoordY = D3DXVECTOR3(m_matCoord._21, m_matCoord._22, m_matCoord._23);
    vCoordZ = D3DXVECTOR3(m_matCoord._31, m_matCoord._32, m_matCoord._33);

    if (!m_nMoveFlag && m_pAttachScene && m_pCoordMesh && /*m_EntityList.GetSize()*/ 0 != m_pAttachScene->GetSelectionTool().GetSelectionCount())
    {
        D3DXMATRIX  matWorldInv;
        D3DXVECTOR3 vOrg;
        D3DXVECTOR3 vDir;
        BOOL  bHit   = FALSE;
        DWORD dwFace = 0;

        hRetCode = m_pAttachScene->GetCurOutputWnd(&piCurOutputWnd);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        piCurOutputWnd->GetPickRay(&vOrg, &vDir, NULL);

        D3DXMATRIX matScanl;
        float fScal = m_fZoom * GetScalingFactor();
        D3DXMatrixScaling(&matScanl, fScal, fScal, fScal);
        D3DXMatrixIdentity(&matWorldInv);
        D3DXMatrixInverse(&matWorldInv, NULL, &(matScanl * m_matCoord));
        D3DXVec3TransformCoord(&vOrg, &vOrg, &matWorldInv);
        D3DXVec3TransformNormal(&vDir, &vDir, &matWorldInv);

        m_dwCurrSelCoord = 0xFFFFFFFF;
        for (DWORD i = 0; i < m_dwNumMatrials; i++)
        {
            D3DXIntersectSubset(
                m_pCoordMesh,
                i,
                &vOrg,
                &vDir,
                &bHit,
                &dwFace,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL
                );
            if (bHit)
            {
                m_dwCurrSelCoord = i;
                break;
            }
        }
    }

    if (m_nMoveFlag && m_pAttachScene && m_pCoordMesh && /*m_EntityList.GetSize()*/0 != m_pAttachScene->GetSelectionTool().GetSelectionCount())
    {
        D3DXVECTOR3 vOrg;
        D3DXVECTOR3 vDir;
        D3DXVECTOR3 vCurrCross;
        D3DXVECTOR3 vMove;
        
        hRetCode = m_pAttachScene->GetCurOutputWnd(&piCurOutputWnd);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        piCurOutputWnd->GetPickRay(&vOrg, &vDir, NULL);
        
        KG_PROCESS_ERROR( 
            D3DXPlaneIntersectLine(
                &vCurrCross,
                &m_currSelPane,
                &vOrg,
                &(vOrg + vDir * 10000000.0f)
                )
            );
		
        vMove = vCurrCross - m_vPrevCross;
		
        D3DXVECTOR3 vTemp = vMove;
        
        switch (m_dwCurrSelCoord)
        {
        case 0 :    // y
        case 6 :
            vMove = vCoordY * D3DXVec3Dot(&vTemp, &vCoordY);
            break;
        case 1 :    // z
        case 7 :
            vMove = vCoordZ * D3DXVec3Dot(&vTemp, &vCoordZ);
            break;
        case 2 :    // x
        case 8 :
            vMove = vCoordX * D3DXVec3Dot(&vTemp, &vCoordX);
            break;
        case 3 :    // yz
            vMove = vCoordY * D3DXVec3Dot(&vTemp, &vCoordY) + vCoordZ * D3DXVec3Dot(&vTemp, &vCoordZ);
            break;
        case 4 :    // zx
            vMove = vCoordZ * D3DXVec3Dot(&vTemp, &vCoordZ) + vCoordX * D3DXVec3Dot(&vTemp, &vCoordX);
            break;
        case 5 :    // xy
            vMove = vCoordX * D3DXVec3Dot(&vTemp, &vCoordX) + vCoordY * D3DXVec3Dot(&vTemp, &vCoordY);
            break;
        default :
            break;
        }

       /* m_EntityList.Translation(vMove);
        m_EntityList.Update();*/
		KSF::SelectionTranslationEntity(m_pAttachScene->GetSelectionTool(), vMove);

        m_vPrevCross = vCurrCross;

    }

    IEKG3DSceneEditorBase* pEditorBase = NULL;
    m_pAttachScene->GetSceneEditorBase(&pEditorBase);
    if (pEditorBase)
        pEditorBase->SetRtsActive(m_dwCurrSelCoord != 0xffffffff);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DMoveCoordinate::Render()
{
    HRESULT hResult  = E_FAIL;
    
    D3DXMATRIX matWorldSave;
    D3DXMATRIX matParent;
    D3DXMATRIX matLocalIv;
    D3DXMATRIX matScanl;
    KG3DRenderState R;

    D3DXMATRIX  matLine;
    D3DXVECTOR3 vSrc;
    D3DXVECTOR3 vDst;
    FLOAT       fLen = 40.f * m_fZoom * GetScalingFactor();

    D3DXMATRIX matWorldMatrix;
    D3DXMATRIX matMatrix;

    IEKG3DSceneOutputWnd* pCurWnd = NULL;
    IEKG3DSceneOutputWnd* pRedWnd = NULL;

	_ASSERTE(NULL != m_pAttachScene);
    m_pAttachScene->GetCurRenderWnd(&pRedWnd);
    m_pAttachScene->GetCurOutputWnd(&pCurWnd);

    if (pRedWnd != pCurWnd)
        return S_OK;

	if(g_Switch.bNewSelection)
		return S_OK;

    D3DXMatrixIdentity(&matLine);

    KG_PROCESS_ERROR(/*m_EntityList.GetSize()*/0 != m_pAttachScene->GetSelectionTool().GetSelectionCount());

    FrameMove();

    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
    R.SetRenderState(D3DRS_AMBIENT, 0xffffffff);
    R.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    R.SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    R.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    g_pd3dDevice->SetTexture(0, NULL);

    float fScal = m_fZoom * GetScalingFactor();
    D3DXMatrixScaling(&matScanl, fScal, fScal, fScal);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &(matScanl * m_matCoord));

    vSrc = D3DXVECTOR3(m_matCoord._41, m_matCoord._42, m_matCoord._43);

    for (DWORD i = 0; i < m_dwNumMatrials; i++)
    {
        D3DMATERIAL9 malNor;
        ZeroMemory(&malNor, sizeof(malNor));
        switch (i)
        {
        case 0 :    // y
            malNor.Diffuse.r = 0.78f;
            malNor.Diffuse.g = 0.0f;
            malNor.Diffuse.b = 0.0f;
            malNor.Diffuse.a = 1.0f;
            malNor.Ambient = malNor.Diffuse;
            break;
        case 1 :    // z
            malNor.Diffuse.r = 0.0f;
            malNor.Diffuse.g = 0.7f;
            malNor.Diffuse.b = 0.0f;
            malNor.Diffuse.a = 1.0f;
            malNor.Ambient = malNor.Diffuse;
            break;
        case 2 :    // x
            malNor.Diffuse.r = 0.0f;
            malNor.Diffuse.g = 0.0f;
            malNor.Diffuse.b = 0.75f;
            malNor.Diffuse.a = 1.0f;
            malNor.Ambient = malNor.Diffuse;
            break;
        case 3 :    // yz     
            R.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            if (i == m_dwCurrSelCoord) {
                malNor.Diffuse.r = 1.0f;
                malNor.Diffuse.g = 1.0f;
                malNor.Diffuse.b = 0.0f;
                malNor.Diffuse.a = 0.2f;
                malNor.Ambient = malNor.Diffuse;
            } else {
                malNor.Diffuse.r = 0.0f;
                malNor.Diffuse.g = 0.0f;
                malNor.Diffuse.b = 0.0f;
                malNor.Diffuse.a = 0.2f;
                malNor.Ambient = malNor.Diffuse;
            }
            break;
        case 4 :    // zx
            R.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            if (i == m_dwCurrSelCoord) {
                malNor.Diffuse.r = 1.0f;
                malNor.Diffuse.g = 1.0f;
                malNor.Diffuse.b = 0.0f;
                malNor.Diffuse.a = 0.2f;
                malNor.Ambient = malNor.Diffuse;
            }  else {
                malNor.Diffuse.r = 0.0f;
                malNor.Diffuse.g = 0.0f;
                malNor.Diffuse.b = 0.0f;
                malNor.Diffuse.a = 0.2f;
                malNor.Ambient = malNor.Diffuse;
            }
            break;
        case 5 :    // xy
            R.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            if (i == m_dwCurrSelCoord) {
                malNor.Diffuse.r = 1.0f;
                malNor.Diffuse.g = 1.0f;
                malNor.Diffuse.b = 0.0f;
                malNor.Diffuse.a = 0.2f;
                malNor.Ambient = malNor.Diffuse;
            } else {
                malNor.Diffuse.r = 0.0f;
                malNor.Diffuse.g = 0.0f;
                malNor.Diffuse.b = 0.0f;
                malNor.Diffuse.a = 0.2f;
                malNor.Ambient = malNor.Diffuse;
            }
            break;
        case 6 :    // y
            g_pd3dDevice->SetTransform(D3DTS_WORLD, &matLine);
            vDst = vSrc + D3DXVECTOR3(m_matCoord._21, m_matCoord._22, m_matCoord._23) * fLen;
            if (6 == m_dwCurrSelCoord || 0 == m_dwCurrSelCoord)
                g_cGraphicsTool.DrawLine(&vSrc, &vDst, 0xffffff00, 0xffffff00);
            else
                g_cGraphicsTool.DrawLine(&vSrc, &vDst, 0xffff0000, 0xffc80000);
            break;
        case 7 : // z
            g_pd3dDevice->SetTransform(D3DTS_WORLD, &matLine);
            vDst = vSrc - D3DXVECTOR3(m_matCoord._31, m_matCoord._32, m_matCoord._33) * fLen;
            if (7 == m_dwCurrSelCoord || 1 == m_dwCurrSelCoord)
                g_cGraphicsTool.DrawLine(&vSrc, &vDst, 0xffffff00, 0xffffff00);
            else
                g_cGraphicsTool.DrawLine(&vSrc, &vDst, 0xff00ff00, 0xff009c00);
            break;
        case 8 : // x
            g_pd3dDevice->SetTransform(D3DTS_WORLD, &matLine);
            vDst = vSrc + D3DXVECTOR3(m_matCoord._11, m_matCoord._12, m_matCoord._13) * fLen;
            if (8 == m_dwCurrSelCoord || 2 == m_dwCurrSelCoord )
                g_cGraphicsTool.DrawLine(&vSrc, &vDst, 0xffffff00, 0xffffff00);
            else
                g_cGraphicsTool.DrawLine(&vSrc, &vDst, 0xff0000ff, 0xff0000c3);
            break;
        default :   // df
            malNor.Diffuse.r = 1.0f;
            malNor.Diffuse.g = 0.0f;
            malNor.Diffuse.b = 1.0f;
            malNor.Diffuse.a = 0.0f;
            malNor.Ambient = malNor.Diffuse;
            break;
        }
        if (i > 5)
            continue;

        D3DMATERIAL9 malSave;
        g_pd3dDevice->GetMaterial(&malSave);
        g_pd3dDevice->SetMaterial(&malNor);
        m_pCoordMesh->DrawSubset(i);
        g_pd3dDevice->SetMaterial(&malSave);
    }

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave); 

    hResult = S_OK;
Exit0:
    return hResult;
}
