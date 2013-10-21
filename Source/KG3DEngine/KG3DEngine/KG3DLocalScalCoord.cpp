////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DScalCoordinate.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-11-7 19:13:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DLocalScalCoord.h"
#include "KG3DSFXParticleLauncher.h"
#include "KG3DGraphicsTool.h"
#include "..\EditInterface\IEKG3DSceneSFXEditor.h"
#include "KG3DModel.h"
#include "KG3DScene.h"
#include "KG3DCollocator.h"//g_Switch


//<Old Selection>
#include "KG3DSceneFunctions.h"
#include "KG3DSceneSelectionTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//KG3DScalCoordinate::KG3DScalCoordinate()
//{
//    m_pCoordMesh        = NULL;
//    m_pAttachScene      = NULL;
//    m_dwNumMatrials     = 0;
//    m_dwCurrSelCoord    = 0xffffffff;
//    m_fZoom             = 0.75f;
//    m_nMoveFlag         = FALSE;
//    m_dwMode            = KG3DCOORD_WORLD;
//
//    //m_EntityList.Clear();
//    KG3DD3DXLoadMeshFromX(SCAL_COORD_MESH_PATH, D3DXMESH_MANAGED, g_pd3dDevice, 
//        NULL, NULL, NULL, &m_dwNumMatrials, &m_pCoordMesh);
//
//}

KG3DScalCoordinate::KG3DScalCoordinate( KG3DScene& scene )
:m_pAttachScene(&scene)
{
	m_pCoordMesh        = NULL;
	m_dwNumMatrials     = 0;
	m_dwCurrSelCoord    = 0xffffffff;
	m_fZoom             = 0.75f;
	m_nMoveFlag         = FALSE;
	m_dwMode            = KG3DCOORD_WORLD;

	//m_EntityList.Clear();
	KG3DD3DXLoadMeshFromX(SCAL_COORD_MESH_PATH, D3DXMESH_MANAGED, g_pd3dDevice, 
		NULL, NULL, NULL, &m_dwNumMatrials, &m_pCoordMesh);

}
KG3DScalCoordinate::~KG3DScalCoordinate()
{
    SAFE_RELEASE(m_pCoordMesh);
}

//KG3DScalCoordinate* KG3DScalCoordinate::GetCoordinate()
//{
//    static KG3DScalCoordinate ScalCoordinate;
//    return &ScalCoordinate;
//}

HRESULT KG3DScalCoordinate::SetMode(DWORD dwMode)
{
	_ASSERTE(dwMode >= KG3DCOORD_SYSTEM_MIN && dwMode <= KG3DCOORD_SYSTEM_MAX);
	if(dwMode >= KG3DCOORD_SYSTEM_MIN && dwMode <= KG3DCOORD_SYSTEM_MAX)
	{
		m_dwMode = static_cast<KG3DCOORD>(dwMode);
		return S_OK;
	}
	return E_FAIL;   
}

HRESULT KG3DScalCoordinate::Zoom(FLOAT fScanl)
{
    if (m_fZoom + fScanl > 0.0f)
        m_fZoom += fScanl;
    return S_OK;
}

//HRESULT KG3DScalCoordinate::AttachEntity(KG3DScene* pScene, std::list<KG3DSceneEntity> listEntitys)
//{
//    m_EntityList = listEntitys;
//
//    KG_PROCESS_ERROR(pScene);
//    m_pAttachScene = pScene;
//
//Exit0:
//    return S_OK;
//}

//HRESULT KG3DScalCoordinate::AttachEntity(KG3DScene* pScene, KG3DSceneEntity Entity)
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

//HRESULT KG3DScalCoordinate::ReleaseAttach()
//{
//     m_EntityList.Clear();
//     return S_OK;
//}

HRESULT KG3DScalCoordinate::ScalBegin()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    if (m_dwCurrSelCoord == 0xFFFFFFFF)
        return E_FAIL;

    D3DXVECTOR3  vCrossXZ;
    D3DXVECTOR3  vCrossYZ;
    D3DXVECTOR3  vCrossXY;
    D3DXVECTOR3  vCrossXYZ;

    D3DXPLANE    planeXZ;
    D3DXPLANE    planeXY;
    D3DXPLANE    planeYZ;
    D3DXPLANE    planeXYZ;

    D3DXVECTOR3  vPoint  = D3DXVECTOR3(m_matCoord._41, m_matCoord._42, m_matCoord._43);
    D3DXVECTOR3  vNorXZ  = D3DXVECTOR3(m_matCoord._21, m_matCoord._22, m_matCoord._23);
    D3DXVECTOR3  vNorXY  = D3DXVECTOR3(m_matCoord._31, m_matCoord._32, m_matCoord._33) * -1.f;
    D3DXVECTOR3  vNorYZ  = D3DXVECTOR3(m_matCoord._11, m_matCoord._12, m_matCoord._13);
    D3DXVECTOR3  vNorXYZ = vNorXZ;//vNorXZ + vNorXY + vNorYZ;
    D3DXVec3Normalize(&vNorXYZ, &vNorXYZ);

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
    D3DXPlaneFromPointNormal(
        &planeXYZ,
        &vPoint,
        &vNorXYZ
        );

    D3DXPlaneNormalize(&planeYZ, &planeYZ);
    D3DXPlaneNormalize(&planeXZ, &planeXZ);
    D3DXPlaneNormalize(&planeXY, &planeXY);
    D3DXPlaneNormalize(&planeXYZ, &planeXYZ);

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
		D3DXPlaneIntersectLine(
			&vCrossXYZ, 
			&planeXYZ,
			&vOrg,
			&(vOrg + vDir * 10000000.0f)
			);

		D3DXVECTOR3 vLoodFor;
		D3DXVec3Normalize(&vLoodFor, &(vCamera - vLook));

		FLOAT fDotToXY  = D3DXPlaneDotNormal(&planeXY, &vLoodFor);
		FLOAT fDotToYZ  = D3DXPlaneDotNormal(&planeYZ, &vLoodFor); 
		FLOAT fDotToXZ  = D3DXPlaneDotNormal(&planeXZ, &vLoodFor);

		m_matCoordPane = m_matCoord;

		switch (m_dwCurrSelCoord)
		{
		case 0 :    // y
        case 7 :
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
        case 8 :
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
        case 9 :
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
		case 6 :
			//m_currSelPane = planeXYZ;
			//m_vPrevCross = vCrossXYZ;
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

HRESULT KG3DScalCoordinate::ScalEnd()
{
    HRESULT hResult  = E_FAIL;

    m_nMoveFlag = FALSE;

    hResult = S_OK;
//Exit0:
    return hResult;
}

float KG3DScalCoordinate::GetScalingFactor()
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

HRESULT KG3DScalCoordinate::FrameMove()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IEKG3DSceneOutputWnd *piCurOutputWnd = NULL;

    D3DXVECTOR3 vCoordX;
    D3DXVECTOR3 vCoordY;
    D3DXVECTOR3 vCoordZ;

	_ASSERTE(NULL != m_pAttachScene);
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

    if (!m_nMoveFlag && m_pAttachScene && m_pCoordMesh && /*m_EntityList.GetSize()*/0 != m_pAttachScene->GetSelectionTool().GetSelectionCount())
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
        D3DXVECTOR3 vScal;
        D3DXVECTOR3 vLocalScal;
        D3DXMATRIX  matEntity;

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

        D3DXVECTOR3 vTemp = vScal;
        D3DXVECTOR3 tCoord;
        D3DXVECTOR3 vSrc;
        D3DXVECTOR3 vDst;

        switch (m_dwCurrSelCoord)
        {
        case 0 :    // y
        case 7 :
            vSrc = vCoordY *  D3DXVec3Dot(&m_vPrevCross, &vCoordY);
            vDst = vCoordY *  D3DXVec3Dot(&vCurrCross, &vCoordY);
            vScal = vDst - vSrc;
            break;
        case 1 :    // z
        case 8 :
            vSrc = vCoordZ *  D3DXVec3Dot(&m_vPrevCross, &vCoordZ);
            vDst = vCoordZ *  D3DXVec3Dot(&vCurrCross, &vCoordZ);
            vScal = vSrc - vDst;
            break;
        case 2 :    // x
        case 9 :
            vSrc = vCoordX *  D3DXVec3Dot(&m_vPrevCross, &vCoordX);
            vDst = vCoordX *  D3DXVec3Dot(&vCurrCross, &vCoordX);
            vScal = vDst - vSrc;
            break;
        case 3 :    // yz
            tCoord = vCoordY - vCoordZ;
            D3DXVec3Normalize(&tCoord, &tCoord);
            vSrc = tCoord *  D3DXVec3Dot(&m_vPrevCross, &tCoord);
            vDst = tCoord *  D3DXVec3Dot(&vCurrCross, &tCoord);
            vScal = vDst - vSrc;
            vScal.z = -vScal.z;
            break;
        case 4 :    // xy
            tCoord = vCoordX + vCoordY;
            D3DXVec3Normalize(&tCoord, &tCoord);
            vSrc = tCoord *  D3DXVec3Dot(&m_vPrevCross, &tCoord);
            vDst = tCoord *  D3DXVec3Dot(&vCurrCross, &tCoord);
            vScal = vDst - vSrc;
            break;
        case 5 :    // zx
            tCoord = vCoordZ - vCoordX;
            D3DXVec3Normalize(&tCoord, &tCoord);
            vSrc = tCoord *  D3DXVec3Dot(&m_vPrevCross, &tCoord);
            vDst = tCoord *  D3DXVec3Dot(&vCurrCross, &tCoord);
            vScal = vDst - vSrc;
            vScal.z = -vScal.z;
            break;
        case 6 :    // xyz
            vSrc = vCoordY *  D3DXVec3Dot(&m_vPrevCross, &vCoordY);
            vDst = vCoordY *  D3DXVec3Dot(&vCurrCross, &vCoordY);
            vScal = vDst - vSrc;
            vScal.x = vScal.y;
            vScal.z = vScal.y;
            break;
        default :
            break;
        }

        //m_EntityList.Scale(vScal, m_dwMode);
		
		KSF::SelectionScaleEntity(m_pAttachScene->GetSelectionTool(), vScal, m_dwMode);

        //m_EntityList.Update();

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

HRESULT KG3DScalCoordinate::Render()
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
    FLOAT       fLen = 125.f * m_fZoom * GetScalingFactor();

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
    //KG_PROCESS_ERROR(m_EntityList.GetSize());
	
	KG_PROCESS_ERROR(0 != m_pAttachScene->GetSelectionTool().GetSelectionCount());

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
        case 6 :    // xyz
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
                malNor.Diffuse.a = 0.0f;
                malNor.Ambient = malNor.Diffuse;
            }
            break;
        case 7 :    // y
            g_pd3dDevice->SetTransform(D3DTS_WORLD, &matLine);
            vDst = vSrc + D3DXVECTOR3(m_matCoord._21, m_matCoord._22, m_matCoord._23) * fLen;
            if (7 == m_dwCurrSelCoord || 0 == m_dwCurrSelCoord)
                g_cGraphicsTool.DrawLine(&vSrc, &vDst, 0xffffff00, 0xffffff00);
            else
                g_cGraphicsTool.DrawLine(&vSrc, &vDst, 0xffff0000, 0xffc80000);
            break;
        case 8 : // z
            g_pd3dDevice->SetTransform(D3DTS_WORLD, &matLine);
            vDst = vSrc - D3DXVECTOR3(m_matCoord._31, m_matCoord._32, m_matCoord._33) * fLen;
            if (8 == m_dwCurrSelCoord || 1 == m_dwCurrSelCoord)
                g_cGraphicsTool.DrawLine(&vSrc, &vDst, 0xffffff00, 0xffffff00);
            else
                g_cGraphicsTool.DrawLine(&vSrc, &vDst, 0xff00ff00, 0xff009c00);
            break;
        case 9 : // x
            g_pd3dDevice->SetTransform(D3DTS_WORLD, &matLine);
            vDst = vSrc + D3DXVECTOR3(m_matCoord._11, m_matCoord._12, m_matCoord._13) * fLen;
            if (9 == m_dwCurrSelCoord || 2 == m_dwCurrSelCoord )
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
          if (i > 6)
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



//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DLocalTranslationCoord.h"

#include "KG3DBaseCoordImp.h"
#include "KG3DFrameDrawer.h"
#include "KG3DMeshHelpers.h"

#include "IEEditorTypes.h"

class KG3DScaleCoordinateSolid : public KG3DBaseCoordImp
{
	friend KG3DBaseCoord* BuildKG3DScaleCoordinateSolid( LPCTSTR, DWORD_PTR pScene, DWORD_PTR );

	STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_COORD_SCALING;}
	STDMETHOD(Init)();
	virtual	ULONG GetActivationState(){return SCENESTATE_SCALE;}
	STDMETHOD(Render)();
	virtual D3DXMATRIX GetTrans();
	KG3DScaleCoordinateSolid(KG3DMessageBroadcasterGroup& Sc):KG3DBaseCoordImp(Sc){}
};


HRESULT STDMETHODCALLTYPE KG3DScaleCoordinateSolid::Init()
{
	RepresentData	rpData;
	_ASSERTE(NULL == m_RepresentData.MeshForAxis[0] && _T("重复的初始化"));

	HRESULT hr = E_FAIL;
	//创建第一个轴Mesh，沿X轴，位置在原点
	{
		hr = D3DXCreateCylinder(g_pd3dDevice, em_mesh_inner_radius, em_mesh_inner_radius
			, em_mesh_range, em_mesh_sides, 1, &rpData.MeshForAxis[0], NULL);
		KG_COM_PROCESS_ERROR(hr);

		D3DXMATRIX matTransform;
		{
			D3DXMATRIX matTranslation;
			D3DXMatrixTranslation(&matTranslation, em_mesh_range/2, 0, 0);	//因为D3DX默认创建出来的那个圆柱是Z轴向的，中点在原点的，所以要转到Y轴向，并推上去，推到底部在原点
			D3DXMATRIX matRotation;
			D3DXMatrixRotationAxis(&matRotation, &D3DXVECTOR3(0,1,0), D3DX_PI / 2);
			matTransform = matRotation * matTranslation;
		}

		hr = D3DXMeshTransformation(rpData.MeshForAxis[0], &matTransform, NULL);
		KG_COM_PROCESS_ERROR(hr);
	}

	//创建第一个面Mesh，法线为X轴
	{
		FLOAT fNear = (FLOAT)em_selplane_inner_edge;
		FLOAT fFar = (FLOAT)em_selplane_outter_edge;
		D3DVECTOR Vertices[] = {
			{0, 0, fNear},
			{0, fNear, 0},
			{0, 0, fFar},
			{0, fFar, 0},
		};
		DWORD Indices[] = {0,1,2,1,3,2};
		hr = D3DXMeshCreateByVerticesPos(D3DFVF_XYZ, Vertices, _countof(Vertices), Indices
			, _countof(Indices), g_pd3dDevice, D3DXMESH_MANAGED, &rpData.MeshForPlane[0]);
		KG_COM_PROCESS_ERROR(hr);
	}
	//复制出另外两个其它两个面，复制出另外两个轴Mesh
	{
		_ASSERTE(NULL != rpData.MeshForAxis[0] && NULL != rpData.MeshForPlane[0]);
		for (size_t i = 1; i < _countof(rpData.MeshForAxis); ++i)
		{
			hr = D3DXMeshClone(rpData.MeshForAxis[0], &rpData.MeshForAxis[i]);
			KG_COM_PROCESS_ERROR(hr);

			const D3DXMATRIX& matRotation = D3DXMatrixGetRotationMatrixOfAxis(i);
			hr = D3DXMeshTransformation(rpData.MeshForAxis[i], &matRotation, NULL);
			KG_COM_PROCESS_ERROR(hr);
		}

		{
			hr = D3DXMeshClone(rpData.MeshForPlane[0], &rpData.MeshForPlane[1]);
			KG_COM_PROCESS_ERROR(hr);

			D3DXMATRIX matRotation;
			D3DXMatrixRotationAxis(&matRotation, &D3DXVECTOR3(0,0,1), -D3DX_PI / 2);

			hr = D3DXMeshTransformation(rpData.MeshForPlane[1], &matRotation, NULL);
			KG_COM_PROCESS_ERROR(hr);

			hr = D3DXMeshClone(rpData.MeshForPlane[0], &rpData.MeshForPlane[2]);
			KG_COM_PROCESS_ERROR(hr);

			D3DXMatrixRotationAxis(&matRotation, &D3DXVECTOR3(0,1,0), D3DX_PI / 2);
			hr = D3DXMeshTransformation(rpData.MeshForPlane[2], &matRotation, NULL);
			KG_COM_PROCESS_ERROR(hr);
		}
	}
	//创建中间那个三角形
	{
		FLOAT BeginPos = (FLOAT)em_selplane_inner_edge;
		D3DVECTOR Vertices[] = {
			{BeginPos, 0, 0},
			{0, BeginPos, 0},
			{0, 0, BeginPos},
		};
		DWORD Indices[] = {
			0,1,2
		};
		hr = D3DXMeshCreateByVerticesPos(D3DFVF_XYZ, Vertices, _countof(Vertices), Indices
			, _countof(Indices), g_pd3dDevice, D3DXMESH_MANAGED, &rpData.MeshForIntegration);
		KG_COM_PROCESS_ERROR(hr);
	}

	m_RepresentData = rpData;
	ZeroMemory(&rpData, sizeof(rpData));

	hr = KG3DBaseCoordImp::Init();
	KG_COM_PROCESS_ERROR(hr);

	//因为创建的时候，是按照Y轴，Z轴，X轴的顺序创建的，
	return S_OK;
Exit0:
	rpData.ReleaseMeshes();
	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE KG3DScaleCoordinateSolid::Render()
{
	HRESULT hr = E_FAIL;
	if(this->GetCurStateID() == EM_EDITOR_TOOL_STATE_NOTATIVATED)
		return S_OK;

	const D3DCOLORVALUE selectedColor = Color2Value(0xfff2d900);
	const D3DCOLORVALUE axisColor[] = {Color2Value(0xffc80000), Color2Value(0xff0000c3), Color2Value(0xff00cc00)};
	const D3DCOLORVALUE selectedColorAlpha = Color2Value(0x50f2d900);

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
	D3DXMATRIX matCustomProj = matProj;
	{
		const FLOAT fProjZScale = 0.0001f;
		matCustomProj._33 *= fProjZScale;
		matCustomProj._43 *= fProjZScale;
	}

#if 0//渲染碰撞圈，用于检查是否正确碰撞
	RenderIntersectMesh(m_RepresentData, matView, matProj);
#endif

#if 1
	D3DXMATRIX matWorld;
	{
		D3DXMatrixScaling(&matWorld, RadiusOfSelector,RadiusOfSelector,RadiusOfSelector);
		D3DXMatrixGetTranslationPart(matWorld) = m_RepresentData.vecPos;
	}
	KG3DFrameDrawerBasic& Drawer = g_GetFrameDrawerBasic();
	hr = Drawer.Begin(&matWorld, &matView, &matCustomProj);
	if (SUCCEEDED(hr))
	{
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		GraphicsStruct::RenderState rsZFunc(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		_ASSERTE(KG3DCOORD_AXIS_COUNT <= 3 && KG3DCOORD_PLANE_COUNT <= 3 && _T("否则会内存越界"));
		_ASSERTE(KG3DFrameDrawer::em_fvf == D3DFVF_XYZ);


		struct AxisLine 
		{
			D3DXVECTOR3 Start;
			D3DXVECTOR3 End;
		};
		const FLOAT fAxisBegin = 0.25f;

		static AxisLine axisLineBuffer[KG3DCOORD_AXIS_COUNT] = 
		{
			{D3DXVECTOR3(fAxisBegin, 0, 0), D3DXVec3GetNormalOfPlane(0)},
			{D3DXVECTOR3(0, fAxisBegin, 0), D3DXVec3GetNormalOfPlane(1)},
			{D3DXVECTOR3(0, 0, fAxisBegin), D3DXVec3GetNormalOfPlane(2)},
		};

		//画3条线
		for(int i = 0; i < KG3DCOORD_AXIS_COUNT; ++i)
		{
			Drawer.SetColor(axisColor[i]);

			AxisLine& lineBuffer = axisLineBuffer[i];
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &lineBuffer, sizeof(D3DXVECTOR3));
		}

		const FLOAT fInnerRange = (FLOAT)em_selplane_inner_edge / (FLOAT)em_mesh_range;
		const FLOAT fOutterRange = (FLOAT)em_selplane_outter_edge / (FLOAT)em_mesh_range;
		//画边面
		{
			struct SideFace
			{
				D3DVECTOR p0, p1, p2, p3, p4;
			};

			static SideFace SideFaces[KG3DCOORD_PLANE_COUNT] = {
				{{0, fOutterRange, 0}, {0, fInnerRange, 0}, {0, 0, fInnerRange}, {0, 0, fOutterRange}, {0, fOutterRange, 0}},
				{{0, 0, fOutterRange}, {0, 0, fInnerRange}, {fInnerRange, 0, 0}, {fOutterRange, 0, 0}, {0, 0, fOutterRange}},
				{{fOutterRange, 0, 0}, {fInnerRange, 0, 0}, {0, fInnerRange, 0}, {0, fOutterRange, 0}, {fOutterRange, 0, 0}},
			};

			for (int i = 0; i < KG3DCOORD_PLANE_COUNT; ++i)
			{
				if (m_ResultData.IntersectedCoord - KG3DCOORD_FIRST_PLANE == i)
				{
					Drawer.SetColor(selectedColorAlpha);
					g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &SideFaces[i], sizeof(D3DXVECTOR3));
					Drawer.SetColor(selectedColor);
					g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &SideFaces[i], sizeof(D3DXVECTOR3));
				}
				else
				{
					Drawer.SetColor(axisColor[i]);
					g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &SideFaces[i], sizeof(D3DXVECTOR3));
				}
			}
		}
		if (m_ResultData.IntersectedCoord >= KG3DCOORD_FIRST_AXIS && m_ResultData.IntersectedCoord < KG3DCOORD_FIRST_AXIS + KG3DCOORD_AXIS_COUNT)
		{
			UINT iIndex= m_ResultData.IntersectedCoord - KG3DCOORD_FIRST_AXIS;
			Drawer.SetColor(selectedColor);
			AxisLine& lineBuffer = axisLineBuffer[iIndex];
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &lineBuffer, sizeof(D3DXVECTOR3));
		}
		//画选中面对应两选择线
		{			
			if (m_ResultData.IntersectedCoord >= KG3DCOORD_FIRST_PLANE && m_ResultData.IntersectedCoord < KG3DCOORD_FIRST_PLANE + KG3DCOORD_PLANE_COUNT)
			{
				UINT iIndexOfPlane = m_ResultData.IntersectedCoord - KG3DCOORD_FIRST_PLANE;

				Drawer.SetColor(selectedColor);

				UINT iPreIndex = (iIndexOfPlane - 1 + KG3DCOORD_PLANE_COUNT) % KG3DCOORD_PLANE_COUNT;
				UINT iNextIndex = (iIndexOfPlane + 1) % KG3DCOORD_PLANE_COUNT;
				_ASSERTE(iPreIndex < _countof(axisLineBuffer) && iNextIndex < _countof(axisLineBuffer));
				AxisLine& preAxisLine = axisLineBuffer[iPreIndex];
				AxisLine& nextAxisLine = axisLineBuffer[iNextIndex];


				g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &preAxisLine, sizeof(D3DXVECTOR3));
				g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &nextAxisLine, sizeof(D3DXVECTOR3));
			}
		}
		//如果选中的是中间的大三角，那么要特殊处理
		if (m_ResultData.IntersectedCoord == KG3DCOORD_INTEGRATION)
		{
			static D3DVECTOR points[] = {
				{fInnerRange, 0,0}, {0, fInnerRange, 0}, {0, 0,fInnerRange}, {fInnerRange, 0,0},
				{fOutterRange, 0,0}, {0, fOutterRange, 0}, {0, 0, fOutterRange}, {fOutterRange, 0,0},
			};

			Drawer.SetColor(selectedColorAlpha);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, points, sizeof(D3DXVECTOR3));
			Drawer.SetColor(selectedColor);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, points, sizeof(D3DXVECTOR3));
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, &points[4], sizeof(D3DXVECTOR3));
		}

		Drawer.End();
	}

	//画出轴线上的几个方块
	{
		KG3DFrameDrawerBox& DrawerBox = g_GetFrameDrawerBox();
		DrawerBox.SetParameters(TRUE, FALSE, 0xffffffff, 0xffffffff);
		DrawerBox.SetAxis(D3DXVECTOR3(1,0,0), D3DXVECTOR3(0,0,1));
		const FLOAT fBoxRange = 0.05f;
		DrawerBox.SetRange(D3DXVECTOR3(fBoxRange, fBoxRange, fBoxRange));
		hr = DrawerBox.Begin(&matWorld, &matView, &matCustomProj);
		if (SUCCEEDED(hr))
		{
			for (size_t i = 0; i < KG3DCOORD_AXIS_COUNT; ++i)
			{
				const D3DXVECTOR3& vNormal = D3DXVec3GetNormalOfPlane(i);

				D3DXVECTOR3 vCenter = vNormal * 1;

				DrawerBox.SetColor(axisColor[i]);
				DrawerBox.DrawBox(vCenter);
			}
			DrawerBox.End();
		}
	}
#endif
	return S_OK;
}

namespace
{
	FLOAT CalScaleComponent(FLOAT fEnd, FLOAT fStart)
	{
		//_ASSERTE(fStart > -0.1f && _T("不允许有负值"));
		if(fStart < 0)
			fStart = 0;

		if(fStart < FLT_EPSILON)	//因为下面要除，所以如果Start很小，那么一除就会很大，这样是不行的
			return 1;

		if(fEnd < 0)
			fEnd = 0;

		FLOAT fRet = fEnd / fStart;
		_ASSERTE(fRet > -FLT_EPSILON);

		//对放缩进行信号延滞处理，不然线性放缩的话，放缩的结果就很敏感，这里主要用log曲线和开方曲线进行延滞
		if(fRet > 1)
			fRet = log(fRet + 1.0f) / log(2.f);	//这里就是y = log2(fRet + 1);因为没有log2函数，所以用自然对数变换一下
		else
			fRet = (sqrt(fRet) * 0.8f + 0.2f); //不能直接用，因为如果fEnd太小，会得到一个0的Scale，这样东西就被完全压扁鸟

		return fRet;
	}
}

D3DXMATRIX KG3DScaleCoordinateSolid::GetTrans()
{
	D3DXVECTOR3 vEnd = (m_ResultData.EndPos - m_RepresentData.vecPos);
	D3DXVECTOR3 vStart = (m_ResultData.StartPos - m_RepresentData.vecPos); 

	D3DXMATRIX matTrans;
	D3DXMatrixTranslation(&matTrans, m_RepresentData.vecPos.x, m_RepresentData.vecPos.y, m_RepresentData.vecPos.z);

	D3DXMATRIX matRet;
	if (m_ResultData.IntersectedCoord != KG3DCOORD_INTEGRATION)
	{		
		D3DXVECTOR3 vRet(1,1,1);
		vRet.x = CalScaleComponent(vEnd.x, vStart.x);
		vRet.y = CalScaleComponent(vEnd.y, vStart.y);
		vRet.z = CalScaleComponent(vEnd.z, vStart.z);
		
		D3DXMatrixScaling(&matRet, vRet.x, vRet.y, vRet.z);		
	}
	else
	{
		FLOAT fScaleFactor = 1;

		//用它们到原点的长度来比较，但是参考Max的放缩框，放缩是只考虑上下的分量的
#if defined(DEBUG) | defined(_DEBUG)
		FLOAT fEndFactor = D3DXVec3Length(&D3DXVECTOR3(vStart.x, vEnd.y, vStart.z));	
		FLOAT fStartFactor = D3DXVec3Length(&vStart);

		_ASSERTE(fEndFactor != 0 && fStartFactor != 0);
#endif
		if (vEnd.y > vStart.y)
		{
			fScaleFactor = 1.f + (vEnd.y - vStart.y)/GetSelectorRingRadius(m_RepresentData);
		}
		else
		{
			fScaleFactor = 1.f / (1 + (vStart.y - vEnd.y)/GetSelectorRingRadius(m_RepresentData));
		}
		
		D3DXMatrixScaling(&matRet, fScaleFactor, fScaleFactor, fScaleFactor);		
	}

	D3DXMATRIX matInv;
	D3DXMatrixInverse(&matInv, NULL, &matTrans);

	return matInv * matRet * matTrans;
}

KG3DBaseCoord* BuildKG3DScaleCoordinateSolid( LPCTSTR, DWORD_PTR pKG3DMessageBroadcasterGroup, DWORD_PTR )
{
	KG3DBaseCoord* p = NULL;
	HRESULT hr = E_FAIL;
	KG3DMessageBroadcasterGroup* pBC = reinterpret_cast<KG3DMessageBroadcasterGroup*>(pKG3DMessageBroadcasterGroup);
	_ASSERTE(_tcsstr(typeid(*pBC).name(), _T("KG3DMessageBroadcasterGroup")) && _T("传入的对象不是这种类型的"));
	_ASSERTE(NULL != pBC && _T("需要wParam赋值"));
	p = new KG3DScaleCoordinateSolid(*pBC);
	KG_PROCESS_ERROR(NULL != p);
	hr = p->Init();
	KG_COM_PROCESS_ERROR(hr);
	return p;
Exit0:
	SAFE_RELEASE(p);
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

