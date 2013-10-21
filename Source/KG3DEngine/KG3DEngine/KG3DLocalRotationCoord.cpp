////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRotationCoordinateOld.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-11-9 14:14:57
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DLocalRotationCoord.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSFXParticleLauncher.h"
#include "KGStateMachine.h"
#include "Render/KG3DFrameDrawer.h"
#include "KG3DScene.h"
#include "KG3DModel.h"

#include "KG3DCollocator.h"//g_Switch

//<Old Selection>
#include "KG3DSceneFunctions.h"
#include "KG3DSceneSelectionTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//KG3DRotationCoordinateOld::KG3DRotationCoordinateOld()
//{
//    m_pCoordMesh        = NULL;
//    m_pAttachScene      = NULL;
//    m_dwNumMatrials     = 0;
//    m_dwCurrSelCoord    = 0xffffffff;
//    m_fZoom             = 1;
//    m_fAngelX           = 0.0f;
//    m_fAngelY           = 0.0f;
//    m_fAngelZ           = 0.0f;
//    m_nMoveFlag         = FALSE;
//    m_dwMode            = KG3DCOORD_WORLD;
//
//    KG3DD3DXLoadMeshFromX(ROTATION_COORD_MESH_PATH, D3DXMESH_MANAGED, g_pd3dDevice, 
//        NULL, NULL, NULL, &m_dwNumMatrials, &m_pCoordMesh);
//    //m_EntityList.Clear();
//}

KG3DRotationCoordinateOld::KG3DRotationCoordinateOld(KG3DScene& scene)
:m_pAttachScene(&scene)
{
	m_pCoordMesh        = NULL;
	m_dwNumMatrials     = 0;
	m_dwCurrSelCoord    = 0xffffffff;
	m_fZoom             = 1;
	m_fAngelX           = 0.0f;
	m_fAngelY           = 0.0f;
	m_fAngelZ           = 0.0f;
	m_nMoveFlag         = FALSE;
	m_dwMode            = KG3DCOORD_WORLD;

	KG3DD3DXLoadMeshFromX(ROTATION_COORD_MESH_PATH, D3DXMESH_MANAGED, g_pd3dDevice, 
		NULL, NULL, NULL, &m_dwNumMatrials, &m_pCoordMesh);
	//m_EntityList.Clear();
}

KG3DRotationCoordinateOld::~KG3DRotationCoordinateOld()
{
    SAFE_RELEASE(m_pCoordMesh);
}

//KG3DRotationCoordinateOld* KG3DRotationCoordinateOld::GetCoordinate()
//{
//    static KG3DRotationCoordinateOld RotCoordinate;
//    return &RotCoordinate;
//}

HRESULT KG3DRotationCoordinateOld::SetMode(DWORD dwMode)
{
    m_dwMode = dwMode;

	_ASSERTE(NULL != m_pAttachScene);
    switch (m_dwMode) 
    {
    case KG3DCOORD_WORLD :
		{
            /*m_matEntityWorld = m_EntityList.GetWorldMatrix();
            D3DXMatrixIdentity(&m_matCoord);
            m_matCoord._41 = m_matEntityWorld._41;
            m_matCoord._42 = m_matEntityWorld._42;
            m_matCoord._43 = m_matEntityWorld._43;*/

			D3DXVECTOR3 vCenter(0,0,0);
			KSF::GetSelectionCenter(m_pAttachScene->GetSelectionTool(), vCenter);
			D3DXMatrixTranslation(&m_matCoord, vCenter.x, vCenter.y, vCenter.z);

			m_matEntityWorld = m_matCoord;
        }
        break;
    case KG3DCOORD_LOCAL :
        {
            /*m_matCoord = m_EntityList.GetWorldMatrix();*/
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

    return S_OK;
}

HRESULT KG3DRotationCoordinateOld::Zoom(FLOAT fScanl)
{
    if (m_fZoom + fScanl > 0.0f)
        m_fZoom += fScanl;
    
    return S_OK;
}

//HRESULT KG3DRotationCoordinateOld::AttachEntity(KG3DScene* pScene, std::list<KG3DSceneEntity> listEntitys)
//{
//    D3DXMATRIX matWorldMatrix;
//
//    m_EntityList = listEntitys;
//
//    KG_PROCESS_ERROR(pScene);
//    m_pAttachScene = pScene;
//
//    m_matEntityWorld = m_EntityList.GetWorldMatrix();
//
//    m_vBeginCross.x = m_matEntityWorld._41;
//    m_vBeginCross.y = m_matEntityWorld._42;
//    m_vBeginCross.z = m_matEntityWorld._43;
//
//    m_vPrevCross = m_vBeginCross;
//
//    switch (m_dwMode) 
//    {
//    case KG3DCOORD_WORLD :
//        {
//            D3DXMATRIX matEntity = m_EntityList.GetWorldMatrix();
//            D3DXMatrixIdentity(&m_matCoord);
//            m_matCoord._41 = matEntity._41;
//            m_matCoord._42 = matEntity._42;
//            m_matCoord._43 = matEntity._43;
//        }
//        break;
//    case KG3DCOORD_LOCAL :
//        {
//            m_matCoord = m_EntityList.GetWorldMatrix();
//            D3DXVECTOR3 t;
//            D3DXVECTOR3 s;
//            D3DXMATRIX  q;
//            D3DXMATRIX  m;
//            MatrixExract(s, t, q, &m_matCoord);
//            D3DXMatrixTranslation(&m, t.x, t.y, t.z);
//            m_matCoord = q * m;
//        }
//        break;
//    default :
//        ASSERT(false);
//        break;
//    }
//
//Exit0:
//    return S_OK;
//}

//HRESULT KG3DRotationCoordinateOld::AttachEntity(KG3DScene* pScene, KG3DSceneEntity Entity)
//{
//    D3DXMATRIX matWorldMatrix;
//
//    m_EntityList.Clear();
//    m_EntityList.AddEntity(Entity);
//
//    KG_PROCESS_ERROR(pScene);
//    m_pAttachScene = pScene;
//
//    matWorldMatrix = m_EntityList.GetWorldMatrix();
//
//    m_vBeginCross.x = matWorldMatrix._41;
//    m_vBeginCross.y = matWorldMatrix._42;
//    m_vBeginCross.z = matWorldMatrix._43;
//
//    m_vPrevCross = m_vBeginCross;
//
//Exit0:
//    return S_OK;
//}


//HRESULT KG3DRotationCoordinateOld::ReleaseAttach()
//{
//    m_EntityList.Clear();
//    m_pAttachScene = NULL;
//    return S_OK;
//}

HRESULT KG3DRotationCoordinateOld::RotateBegin()
{
 
	//m_matEntityWorld = m_EntityList.GetWorldMatrix();
	{
		D3DXVECTOR3 vCenter(0,0,0);
		KSF::GetSelectionCenter(m_pAttachScene->GetSelectionTool(), vCenter);
		D3DXMatrixTranslation(&m_matEntityWorld, vCenter.x, vCenter.y, vCenter.z);
	}

	m_vBeginCross.x = m_matEntityWorld._41;
	m_vBeginCross.y = m_matEntityWorld._42;
	m_vBeginCross.z = m_matEntityWorld._43;

	m_vPrevCross = m_vBeginCross;

    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    D3DXVECTOR3 vOrg;
    D3DXVECTOR3 vDir;

    D3DXMATRIX  matWorldInv;

    D3DXVECTOR3  vCrossXZ;
    D3DXVECTOR3  vCrossYZ;
    D3DXVECTOR3  vCrossXY;

    D3DXPLANE    planeXZ;
    D3DXPLANE    planeXY;
    D3DXPLANE    planeYZ;

    D3DXVECTOR3  vPoint = D3DXVECTOR3(m_matCoord._41, m_matCoord._42, m_matCoord._43);
    D3DXVECTOR3  vNorXZ = D3DXVECTOR3(m_matCoord._21, m_matCoord._22, m_matCoord._23);
    D3DXVECTOR3  vNorXY = D3DXVECTOR3(m_matCoord._31, m_matCoord._32, m_matCoord._33);
    D3DXVECTOR3  vNorYZ = D3DXVECTOR3(m_matCoord._11, m_matCoord._12, m_matCoord._13);

    D3DXVECTOR3 vCross;
    IEKG3DSceneOutputWnd *piCurOutputWnd = NULL;


    KG_PROCESS_ERROR(m_dwCurrSelCoord != 0xFFFFFFFF);
    //KG_PROCESS_ERROR(m_pAttachScene);
    //KG_PROCESS_ERROR(m_EntityList.GetSize());

	_ASSERTE(NULL != m_pAttachScene);
	KG_PROCESS_ERROR(0 != m_pAttachScene->GetSelectionTool().GetSelectionCount());

    hRetCode = m_pAttachScene->GetCurOutputWnd(&piCurOutputWnd);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    piCurOutputWnd->GetPickRay(&vOrg, &vDir, NULL);

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

    switch (m_dwCurrSelCoord)
    {
    case 0 :    // y
        KG_PROCESS_ERROR(
            D3DXPlaneIntersectLine(
                &vCross, 
                &planeXZ,
                &vOrg,
                &(vOrg + vDir * 10000000.0f)
                )
            );
        m_vPrevCross    = vCross;
        m_currSelPane   = planeXZ;
        m_currSelNormal = vNorXZ;
        break;
    case 1 :    // x
        KG_PROCESS_ERROR(
            D3DXPlaneIntersectLine(
                &vCross, 
                &planeYZ,
                &vOrg,
                &(vOrg + vDir * 10000000.0f)
                )
            );
        m_vPrevCross    = vCross;
        m_currSelPane   = planeYZ;
        m_currSelNormal = vNorYZ;
        break;
    case 2 :    // z
        KG_PROCESS_ERROR(
            D3DXPlaneIntersectLine(
                &vCross, 
                &planeXY,
                &vOrg,
                &(vOrg + vDir * 10000000.0f)
                )
            );
        m_vPrevCross    = vCross;
        m_currSelPane   = planeXY;
        m_currSelNormal = vNorXY;
        break;
    default :
        ASSERT(FALSE);
        break;
    }
    m_vBeginCross = m_vPrevCross;

  
    m_fAngelX = 0.0f;
    m_fAngelY = 0.0f;
    m_fAngelZ = 0.0f;

    m_nMoveFlag = TRUE;
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DRotationCoordinateOld::RotateEnd()
{
    HRESULT hResult  = E_FAIL;

    D3DXMATRIX matWorldMatrix = m_matEntityWorld;//m_EntityList.GetWorldMatrix();
    m_vBeginCross.x = matWorldMatrix._41;
    m_vBeginCross.y = matWorldMatrix._42;
    m_vBeginCross.z = matWorldMatrix._43;

    m_vPrevCross = m_vBeginCross;

    m_fAngelX = 0.0f;
    m_fAngelY = 0.0f;
    m_fAngelZ = 0.0f;

    m_nMoveFlag = FALSE;

    hResult = S_OK;
    return hResult;
}

float KG3DRotationCoordinateOld::GetScalingFactor()
{
    if (!m_pAttachScene)
        return 1.f;

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

DWORD KG3DRotationCoordinateOld::GetSelCoord(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDir)
{
    D3DXVECTOR3 vCoordX = D3DXVECTOR3(m_matCoord._11, m_matCoord._12, m_matCoord._13);
    D3DXVECTOR3 vCoordY = D3DXVECTOR3(m_matCoord._21, m_matCoord._22, m_matCoord._23);
    D3DXVECTOR3 vCoordZ = D3DXVECTOR3(m_matCoord._31, m_matCoord._32, m_matCoord._33);
    D3DXVECTOR3 vCenter = D3DXVECTOR3(m_matCoord._41, m_matCoord._42, m_matCoord._43);
    D3DXVECTOR3 vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);

    D3DXVECTOR3 src;
    D3DXVECTOR3 dir;

    BOOL  bHit   = FALSE;
    DWORD dwFace = 0;

    FLOAT fScal = m_fZoom * GetScalingFactor();

    D3DXMATRIX  matCoord;
    D3DXMATRIX  matMove;
    D3DXMATRIX  matRota;
    D3DXMATRIX  matScal;
    D3DXMATRIX  matCoordInv;

    D3DXVECTOR3 vRotaAxis;

    D3DXMatrixTranslation(&matMove, vCenter.x, vCenter.y, vCenter.z);
    D3DXMatrixScaling(&matScal, fScal, fScal, fScal);

    // y

    D3DXVec3Cross(&vRotaAxis, &vNormal, &vCoordY);
    D3DXMatrixRotationAxis(&matRota, &vRotaAxis, acosf(D3DXVec3Dot(&vNormal, &vCoordY)));
    matCoord = matScal * matRota * matMove;
    D3DXMatrixInverse(&matCoordInv, NULL, &matCoord);
    D3DXVec3TransformCoord(&src, &vSrc, &matCoordInv);
    D3DXVec3TransformNormal(&dir, &vDir, &matCoordInv);

    for (DWORD i = 0; i < m_dwNumMatrials; i++)
    {
        D3DXIntersectSubset(
            m_pCoordMesh,
            i,
            &src,
            &dir,
            &bHit,
            &dwFace,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
            );
        if (bHit)
            return 0;
    }


    // x

    D3DXVec3Cross(&vRotaAxis, &vNormal, &vCoordX);
    D3DXMatrixRotationAxis(&matRota, &vRotaAxis, acosf(D3DXVec3Dot(&vNormal, &vCoordX)));
    matCoord = matScal * matRota * matMove;
    D3DXMatrixInverse(&matCoordInv, NULL, &matCoord);
    D3DXVec3TransformCoord(&src, &vSrc, &matCoordInv);
    D3DXVec3TransformNormal(&dir, &vDir, &matCoordInv);

    for (DWORD i = 0; i < m_dwNumMatrials; i++)
    {
        D3DXIntersectSubset(
            m_pCoordMesh,
            i,
            &src,
            &dir,
            &bHit,
            &dwFace,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
            );
        if (bHit)
            return 1;
    }


    // z

    D3DXVec3Cross(&vRotaAxis, &vNormal, &vCoordZ);
    D3DXMatrixRotationAxis(&matRota, &vRotaAxis, acosf(D3DXVec3Dot(&vNormal, &vCoordZ)));
    matCoord = matScal * matRota * matMove;
    D3DXMatrixInverse(&matCoordInv, NULL, &matCoord);
    D3DXVec3TransformCoord(&src, &vSrc, &matCoordInv);
    D3DXVec3TransformNormal(&dir, &vDir, &matCoordInv);

    for (DWORD i = 0; i < m_dwNumMatrials; i++)
    {
        D3DXIntersectSubset(
            m_pCoordMesh,
            i,
            &src,
            &dir,
            &bHit,
            &dwFace,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
            );
        if (bHit)
            return 2;
    }

   return 0xffffffff;
}


HRESULT KG3DRotationCoordinateOld::FrameMove()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IEKG3DSceneOutputWnd *piCurOutputWnd = NULL;

    D3DXVECTOR3 vCoordX;
    D3DXVECTOR3 vCoordY;
    D3DXVECTOR3 vCoordZ;

    switch (m_dwMode) 
    {
    case KG3DCOORD_WORLD:
        {
           /* D3DXMATRIX matEntity = m_EntityList.GetWorldMatrix();
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


    if (!m_nMoveFlag && m_pAttachScene && m_pCoordMesh && /*m_EntityList.GetSize()*/0 != m_pAttachScene->GetSelectionTool().GetSelectionCount())
    {
        D3DXVECTOR3 vOrg;
        D3DXVECTOR3 vDir;

        hRetCode = m_pAttachScene->GetCurOutputWnd(&piCurOutputWnd);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        piCurOutputWnd->GetPickRay(&vOrg, &vDir, NULL);
        D3DXVec3Normalize(&vDir, &vDir);
        m_dwCurrSelCoord = GetSelCoord(vOrg, vDir);

    }

    if (m_nMoveFlag && m_pAttachScene && m_pCoordMesh && /*m_EntityList.GetSize()*/0 != m_pAttachScene->GetSelectionTool().GetSelectionCount())
    {
        D3DXVECTOR3 vModelPosition;
        D3DXVECTOR3 vOrg;
        D3DXVECTOR3 vDir;
        D3DXVECTOR3 vCurrOrig;
        D3DXMATRIX  matWorldMatrix;

        hRetCode = m_pAttachScene->GetCurOutputWnd(&piCurOutputWnd);
        KG_PROCESS_ERROR(piCurOutputWnd);
        piCurOutputWnd->GetPickRay(&vOrg, &vDir, NULL);

        matWorldMatrix = m_matEntityWorld;

        vModelPosition.x = matWorldMatrix._41;
        vModelPosition.y = matWorldMatrix._42;
        vModelPosition.z = matWorldMatrix._43;

        D3DXVECTOR3 vCurrCross;
        KG_PROCESS_ERROR(
            D3DXPlaneIntersectLine(
                &vCurrCross, 
                &m_currSelPane,
                &vOrg,
                &(vOrg + vDir * 10000000.0f)
                )
            );
      

        D3DXVECTOR3 vCurrDir;
        D3DXVECTOR3 vPrevDir;

        D3DXVec3Normalize(&vCurrDir, &(vCurrCross   - vModelPosition));
        D3DXVec3Normalize(&vPrevDir, &(m_vPrevCross - vModelPosition)); 

        D3DXVECTOR3 vCross;
        FLOAT fDot = D3DXVec3Dot(&vCurrDir, &vPrevDir);
        if (fDot > 1.0f)
            fDot = 1.0f;
        D3DXVec3Cross(&vCross, &vCurrDir, &vPrevDir);
        FLOAT fAngel = 0.0f;
        D3DXVec3Normalize(&vCross, &vCross);
        D3DXVec3Normalize(&m_currSelNormal, &m_currSelNormal);

        if (fabs(D3DXVec3Dot(&vCross, &m_currSelNormal) - 1) <= 0.05)
            fAngel = -acosf(fDot);
        else
            fAngel =  acosf(fDot);

        D3DXMATRIX mat;
        switch (m_dwCurrSelCoord)
        {
        case 0 :
            D3DXMatrixRotationAxis(&mat, &vCoordY, fAngel);
            m_fAngelY += fAngel;
            break;
        case 1 :
            D3DXMatrixRotationAxis(&mat, &vCoordX, fAngel);
            m_fAngelX += fAngel;
            break;
        case 2 :
            D3DXMatrixRotationAxis(&mat, &vCoordZ, fAngel);
            m_fAngelZ += fAngel;
            break;
        default :
            break;
        }

        //m_EntityList.Rotation(mat);
		KSF::SelectionRotationEntity(m_pAttachScene->GetSelectionTool(), mat);

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

HRESULT KG3DRotationCoordinateOld::Render()
{
    HRESULT hResult  = E_FAIL;

    /*if (!m_EntityList.GetSize())
        return E_FAIL;*/
	if (0 == m_pAttachScene->GetSelectionTool().GetSelectionCount())
	{
		return E_FAIL;
	}

    D3DXMATRIX matWorldSave;
    D3DXMATRIX matWorld;
    D3DXMATRIX matParent;
    D3DXMATRIX matLocalIv;
    D3DXMATRIX matWorldMatrix;
    D3DXMATRIX matMatrix;

    IEKG3DSceneOutputWnd* pCurWnd = NULL;
    IEKG3DSceneOutputWnd* pRedWnd = NULL;

    m_pAttachScene->GetCurRenderWnd(&pRedWnd);
    m_pAttachScene->GetCurOutputWnd(&pCurWnd);

    if (pRedWnd != pCurWnd || pRedWnd == NULL)
        return S_OK;

	if(g_Switch.bNewSelection)
		return S_OK;

    FrameMove();

    D3DXMatrixIdentity(&matWorld);
    KG3DRenderState R;
    R.SetRenderState(D3DRS_AMBIENT, 0xffffffff);
    R.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
    R.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    R.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    R.SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    g_pd3dDevice->SetTexture(0, NULL);

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    D3DXVECTOR3 vCenter = D3DXVECTOR3(m_matCoord._41, m_matCoord._42, m_matCoord._43);
    D3DXVECTOR3 vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
    FLOAT fRadiu = 59.f * m_fZoom * GetScalingFactor();
    D3DXVECTOR3 vCoordX = D3DXVECTOR3(m_matCoord._11, m_matCoord._12, m_matCoord._13);
    D3DXVECTOR3 vCoordY = D3DXVECTOR3(m_matCoord._21, m_matCoord._22, m_matCoord._23);
    D3DXVECTOR3 vCoordZ = D3DXVECTOR3(m_matCoord._31, m_matCoord._32, m_matCoord._33);

    D3DXMATRIX matProjSave;
    D3DXMATRIX matProj;
    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    ASSERT(pCamera);
    FLOAT fFovY = 0.f;
    FLOAT fAspe = 0.f;
    FLOAT fNear = 0.f;
    FLOAT fFar  = 0.f;
    pCamera->GetPerspective(&fFovY, &fAspe, &fNear, &fFar);
    fFar = D3DXVec3Length(&(pCamera->GetPosition() - vCenter));

    D3DXMatrixPerspectiveFovLH(&matProj, fFovY, fAspe, fNear, fFar);
    g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjSave);

    if (pCamera->IsPerspective())
        g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

    // y
    if (m_dwCurrSelCoord == 0)
        g_cGraphicsTool.DrawCircle(vCenter, vCoordY, fRadiu, 0xfff2d900);
    else 
        g_cGraphicsTool.DrawCircle(vCenter, vCoordY, fRadiu, 0xffc80000);

    // x
    if (m_dwCurrSelCoord == 1)
        g_cGraphicsTool.DrawCircle(vCenter, vCoordX, fRadiu, 0xfff2d900);
    else 
        g_cGraphicsTool.DrawCircle(vCenter, vCoordX, fRadiu, 0xff0000c3);

    // z
    if (m_dwCurrSelCoord == 2)
        g_cGraphicsTool.DrawCircle(vCenter, vCoordZ, fRadiu, 0xfff2d900);
    else 
        g_cGraphicsTool.DrawCircle(vCenter, vCoordZ, fRadiu, 0xff00cc00);

    g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjSave);

	//画出从开始到结束的三角扇形
    if (m_dwCurrSelCoord == 0)
        g_cGraphicsTool.DrawFan(vCenter, vCoordY, fRadiu - 1.f, m_vBeginCross, m_fAngelY, 0x50000000);
    else if (m_dwCurrSelCoord == 1)
        g_cGraphicsTool.DrawFan(vCenter, vCoordX, fRadiu - 1.f, m_vBeginCross, m_fAngelX, 0x50000000);
    else if (m_dwCurrSelCoord == 2)
        g_cGraphicsTool.DrawFan(vCenter, vCoordZ, fRadiu - 1.f, m_vBeginCross, m_fAngelZ, 0x50000000);
    else 
        ;

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave); 

    hResult = S_OK;
    return hResult;
}
//////////////////////////////////////////////////////////////////////////
#include "KG3DBaseCoordImp.h"

#include "KGStateMachine.h"
#include "KG3DFrameDrawer.h"
#include "KG3DMeshHelpers.h"

#include "IEEditorTypes.h"
class KG3DRotationCoordinateSolid : public KG3DBaseCoordImp
{
public:
	friend KG3DBaseCoord* BuildKG3DRotationCoordinateSolid( LPCTSTR, DWORD_PTR KG3DMessageBroadcasterGroup, DWORD_PTR );

	STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_COORD_ROTATION;}
	STDMETHOD(Init)();
	virtual	ULONG GetActivationState(){return SCENESTATE_ROTATE;}
	virtual D3DXMATRIX GetTrans();
	STDMETHOD(Render)();

	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );

	KG3DRotationCoordinateSolid(KG3DMessageBroadcasterGroup& Sc);

private:
	FLOAT m_fAngle;
	D3DXVECTOR3 m_vLastDir;
};

KG3DRotationCoordinateSolid::KG3DRotationCoordinateSolid( KG3DMessageBroadcasterGroup& Sc )
	:KG3DBaseCoordImp(Sc)
	,m_fAngle(0)
	,m_vLastDir(1,0,0)
{

}
HRESULT STDMETHODCALLTYPE KG3DRotationCoordinateSolid::Init()
{
	RepresentData rpData;
	
	HRESULT hr = D3DXCreateTorus(g_pd3dDevice, em_mesh_inner_radius, em_mesh_range, em_mesh_sides, em_mesh_rings
		, &rpData.MeshForPlane[2], NULL);	//创建出来的圈圈是Z轴正向的
	KG_COM_PROCESS_ERROR(hr);
	{
		//复制出另外两个圈圈

		D3DXMATRIX matRotation;
		D3DXMatrixRotationAxis(&matRotation, &D3DXVECTOR3(1,0,0), -D3DX_PI/2);	

		hr = D3DXMeshClone(rpData.MeshForPlane[2], &rpData.MeshForPlane[1]);
		KG_COM_PROCESS_ERROR(hr);
		hr = D3DXMeshTransformation(rpData.MeshForPlane[1], &matRotation, NULL);
		KG_COM_PROCESS_ERROR(hr);
		
		D3DXMatrixRotationAxis(&matRotation, &D3DXVECTOR3(0,1,0), D3DX_PI/2);

		hr = D3DXMeshClone(rpData.MeshForPlane[2], &rpData.MeshForPlane[0]);
		KG_COM_PROCESS_ERROR(hr);
		hr = D3DXMeshTransformation(rpData.MeshForPlane[0], &matRotation, NULL);
		KG_COM_PROCESS_ERROR(hr);
	}
	m_RepresentData = rpData;
	ZeroMemory(&rpData, sizeof(rpData));
	m_RepresentData.bFilterInter = em_filter_back;	//把后半的碰撞结果过滤掉

	hr = KG3DBaseCoordImp::Init();
	KG_COM_PROCESS_ERROR(hr);
	return S_OK;
Exit0:
	rpData.ReleaseMeshes();
	return E_FAIL;
}

D3DXMATRIX KG3DRotationCoordinateSolid::GetTrans()
{
	if(m_ResultData.IntersectedCoord >= KG3DCOORD_FIRST_PLANE && m_ResultData.IntersectedCoord < KG3DCOORD_FIRST_PLANE + KG3DCOORD_PLANE_COUNT)
	{
		D3DXVECTOR3 rotAxis = D3DXVec3GetNormalOfPlane(m_ResultData.IntersectedCoord - KG3DCOORD_FIRST_PLANE);

		D3DXMATRIX matTrans;
		D3DXMatrixTranslation(&matTrans, m_RepresentData.vecPos.x, m_RepresentData.vecPos.y, m_RepresentData.vecPos.z);

		D3DXMATRIX matRet;
		D3DXMatrixRotationAxis(&matRet, &rotAxis, m_fAngle);

		D3DXMATRIX matInv;
		D3DXMatrixInverse(&matInv, NULL, &matTrans);
		
		return matInv * matRet * matTrans;	//先还原移动，然后旋转，再次移动
	}
	return D3DXMatrixGetIdentity();
}

void __stdcall KG3DRotationCoordinateSolid::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	_ASSERTE(NULL != m_pStateMachine);
	DWORD oldState = this->GetCurStateID();

	KG3DBaseCoordImp::OnReceiveMessage(Message, pSender);

	if(Message.m_uMessage != KM_STATE_CHANGE && Message.m_uMessage != KM_EXECUTE_ACTION)
		return;

	DWORD curState = this->GetCurStateID();
	if(curState == EM_EDITOR_TOOL_STATE_NOTATIVATED)
	{
		m_fAngle = 0;
		return;
	}
	else if (curState == EM_EDITOR_TOOL_STATE_ATIVATED)
	{
		if(oldState == EM_EDITOR_TOOL_STATE_TRANSFORMING)
			m_fAngle = 0;
	}
	else if (curState == EM_EDITOR_TOOL_STATE_TRANSFORMING)
	{
		if (oldState == EM_EDITOR_TOOL_STATE_ATIVATED)
		{
			m_vLastDir = m_ResultData.StartPos - m_RepresentData.vecPos;
			D3DXVec3Normalize(&m_vLastDir, &m_vLastDir);
			m_fAngle = 0;
		}

		if(m_ResultData.IntersectedCoord >= KG3DCOORD_FIRST_PLANE && m_ResultData.IntersectedCoord < KG3DCOORD_FIRST_PLANE + KG3DCOORD_PLANE_COUNT)
		{
			D3DXVECTOR3 rotAxis = D3DXVec3GetNormalOfPlane(m_ResultData.IntersectedCoord - KG3DCOORD_FIRST_PLANE);

			D3DXVECTOR3 vCurDir = m_ResultData.EndPos - m_RepresentData.vecPos;
			D3DXVec3Normalize(&vCurDir, &vCurDir);

			FLOAT fCos = D3DXVec3Dot(&m_vLastDir, &vCurDir);
			MT_LIMIT_TO_RANGE(fCos, 1.f, -1.f);
			FLOAT fAnglePlus = acos(fCos);

			D3DXVECTOR3 vCross;
			D3DXVec3Cross(&vCross, &m_vLastDir, &vCurDir);

			BOOL bIsClockwise = D3DXVec3Dot(&rotAxis, &vCross) > 0;
			if(! bIsClockwise)
				fAnglePlus = -fAnglePlus;

			m_fAngle += fAnglePlus;

			//限制Angle到等价域，不然数值过大会出现浮点误差
			if (m_fAngle > D3DX_PI * 2)
			{
				m_fAngle -= D3DX_PI * 2;
			}
			else if (m_fAngle < -D3DX_PI*2)
			{
				m_fAngle += D3DX_PI * 2;
			}

			m_vLastDir = vCurDir;
		}		
	}
}
HRESULT STDMETHODCALLTYPE KG3DRotationCoordinateSolid::Render()
{
	_ASSERTE(NULL != m_pStateMachine);
	if(this->GetCurStateID() == EM_EDITOR_TOOL_STATE_NOTATIVATED)
		return S_OK;

	const D3DCOLOR seletedColor = 0xfff2d900;
	const D3DCOLOR circleColors[] = {0xffc80000, 0xff0000c3, 0xff00cc00};

	D3DXMATRIX matView; 
	D3DXMATRIX matProj;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	//每帧得到一下Camera位置
	{
		D3DXVECTOR3 camPos;
		if(NULL != D3DXMatrixGetPosOfView(&matView, &camPos))
		{
			m_RepresentData.CamPos = camPos;
		}
	}

	FLOAT RadiusOfSelector = GetSelectorRingRadius(m_RepresentData);

	//把投射矩阵的ZNear和ZFar压到选择圈两边，这样选择圈会在视角变动的时候保持大小且不会被遮挡
	const FLOAT fProjZScale = 0.0001f;

	D3DXMATRIX matCustomProj = matProj;
	matCustomProj._33 *= fProjZScale;
	matCustomProj._43 *= fProjZScale;

	KG3DFrameDrawerFan& Drawer = g_GetFrameDrawerFan();
	if (SUCCEEDED(Drawer.Begin(NULL, &matView, &matCustomProj)))
	{
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		GraphicsStruct::RenderState rsZFunc(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		Drawer.SetParameters(FALSE, TRUE, 0x00000000, 0x00000000);

		_ASSERTE(KG3DCOORD_PLANE_COUNT <= 3 && _T("否则circleColors[i]会内存越界"));

		/*先画出选择圈圈，和3DMax学习，选择圈圈面向屏幕这边是有色的,背向屏幕是灰色的
		为了实现这样的效果，要使用一下ZFar来Clip，先画灰色圈圈，然后设一下ZFar为圈圈中心
		到摄像机距离，再画一次彩色圈圈。为了避免和KG3DCamera耦合，直接从View矩阵和Perspective
		矩阵得到对应的数据，并直接操作矩阵来改ZFar。
		*/
		{

			//画第一次，灰色
			Drawer.SetLineColor(0xff505050);
			//画全局圆圈
			{
				D3DXVECTOR3 vNormalBoundCircle = m_RepresentData.CamPos - m_RepresentData.vecPos;
				if(D3DXVec3Length(&vNormalBoundCircle) > 1)
				{
					D3DXVec3Normalize(&vNormalBoundCircle, &vNormalBoundCircle);
					Drawer.DrawCircle(m_RepresentData.vecPos, vNormalBoundCircle, RadiusOfSelector);
				}
			}
			for(int i = 0; i < KG3DCOORD_PLANE_COUNT; ++i)
			{
				Drawer.DrawCircle(m_RepresentData.vecPos, D3DXVec3GetNormalOfPlane(i), RadiusOfSelector);
			}
			//计算改了ZFar之后的Proj矩阵
			D3DXMATRIX matPertNew = matProj;
			{
				D3DXVECTOR3 viewPointPos(0,0,0);
				D3DXVECTOR3 dirOfCam(0,0,1);
				if(NULL != D3DXMatrixGetPosOfView(&matView, &viewPointPos) && NULL != D3DXMatrixGetDirOfView(&matView, &dirOfCam))
				{
					D3DXVECTOR3 vDirOfCenter = m_RepresentData.vecPos - viewPointPos;
					D3DXVec3Normalize(&vDirOfCenter, &vDirOfCenter);
					FLOAT fProjScaleBias = D3DXVec3Dot(&vDirOfCenter, &dirOfCam);	//即夹角的Cos
					FLOAT fLength = D3DXVec3Length(&(viewPointPos - m_RepresentData.vecPos));
					D3DXMatrixProjResetZNearFar(matPertNew, fLength * fProjScaleBias - RadiusOfSelector, fLength);
				}
			}


			//改ZFar，画第二次，彩色，选中的话还要变个颜色
			Drawer.SetTransform(NULL, &matView, &matPertNew);


			for(int i = 0; i < KG3DCOORD_PLANE_COUNT; ++i)
			{
				m_ResultData.IntersectedCoord - KG3DCOORD_FIRST_PLANE == i 
					? Drawer.SetLineColor(seletedColor) 
					: Drawer.SetLineColor(circleColors[i]);
				Drawer.DrawCircle(m_RepresentData.vecPos, D3DXVec3GetNormalOfPlane(i), RadiusOfSelector);
			}

			//还原默认的Trans
			Drawer.SetTransform(NULL, &matView, &matCustomProj);
		}

		//如果开始旋转了，把表示旋转角度的扇形画出来
		if (this->GetCurStateID() == EM_EDITOR_TOOL_STATE_TRANSFORMING)
		{
			_ASSERTE(m_ResultData.IntersectedCoord >= KG3DCOORD_FIRST_PLANE 
				&& m_ResultData.IntersectedCoord < KG3DCOORD_FIRST_PLANE + KG3DCOORD_PLANE_COUNT);


			D3DCOLOR SelectionFanColor = circleColors[m_ResultData.IntersectedCoord - KG3DCOORD_FIRST_PLANE];
			SelectionFanColor &= 0x00ffffff;
			SelectionFanColor |= 0x50000000;//把Alpha设为50

			D3DXVECTOR3 vStart;
			D3DXVec3Normalize(&vStart, &(m_ResultData.StartPos - m_RepresentData.vecPos));

			D3DXVECTOR3 StartPoint = vStart * RadiusOfSelector + m_RepresentData.vecPos;

			Drawer.SetParameters(TRUE, FALSE, SelectionFanColor, 0xffffffff);
			Drawer.DrawFan( m_RepresentData.vecPos
				, D3DXVec3GetNormalOfPlane(m_ResultData.IntersectedCoord - KG3DCOORD_FIRST_PLANE)
				, StartPoint
				, m_fAngle);
		}

		Drawer.End();
	}

	return S_OK;
}

KG3DBaseCoord* BuildKG3DRotationCoordinateSolid( LPCTSTR, DWORD_PTR pKG3DMessageBroadcasterGroup, DWORD_PTR )
{
	KG3DRotationCoordinateSolid* p = NULL;
	HRESULT hr = E_FAIL;
	KG3DMessageBroadcasterGroup* pBC = reinterpret_cast<KG3DMessageBroadcasterGroup*>(pKG3DMessageBroadcasterGroup);
	_ASSERTE(_tcsstr(typeid(*pBC).name(), _T("KG3DMessageBroadcasterGroup")) && _T("传入的对象不是这种类型的"));
	_ASSERTE(NULL != pBC);
	p = new KG3DRotationCoordinateSolid(*pBC);
	KG_PROCESS_ERROR(NULL != p);
	hr = p->Init();
	KG_COM_PROCESS_ERROR(hr);
	return p;
Exit0:
	SAFE_RELEASE(p);
	return NULL;
}