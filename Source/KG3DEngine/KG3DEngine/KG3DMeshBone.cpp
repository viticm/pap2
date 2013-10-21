////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMeshBone.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-26 9:48:48
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DMeshBone.h"
#include "KG3DGraphicsTool.h"
#include "KG3DModelST.h"
#include "KG3DMesh.h"
#include "KG3DBip.h"
#include "KG3DStructs.h"
#include "KG3DScene.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DMeshBone::KG3DMeshBone(void)
{
    m_dwPrevTime   = timeGetTime();
    m_pAttachScene = NULL;
    m_pAttachModel = NULL;
    m_pSysMesh     = NULL;
    m_nSelSubIndex = -1;
    m_nSeledIndex  = -1;
    m_nBindIndex   = -1;
    D3DXMatrixIdentity(&m_matWorld);
}

KG3DMeshBone::~KG3DMeshBone(void)
{
    SAFE_RELEASE(m_pSysMesh);
}

HRESULT KG3DMeshBone::SetSelectedBone(const TCHAR cszName[])
{
    HRESULT hResult  = E_FAIL;
    m_nSelSubIndex  = -1;
    for (DWORD i = 0; i < m_vecSubMeshInfo.size(); i ++)
    {
        if (!stricmp(cszName, m_vecSubMeshInfo[i].szBoneName))
        {
            m_nSeledIndex = i;
            break;
        }
    }

    hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT KG3DMeshBone::GetSelBoneName(TCHAR szName[], INT nLen, DWORD *pdwType)
{
    HRESULT hResult  = E_FAIL;

    KG_PROCESS_ERROR(szName);
    if (m_nSelSubIndex != -1)
    {
        strncpy(szName, m_vecSubMeshInfo[m_nSelSubIndex].szBoneName, nLen);
        *pdwType = m_vecSubMeshInfo[m_nSelSubIndex].dwType;
    }
    else
    {
        strncpy(szName, TEXT(""), nLen);
        *pdwType = 1;
    }
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DMeshBone::UpdateSelBone(BOOL bSelectedUpdate /* = FALSE */)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;

    D3DXMATRIX  matWorldInv;
    D3DXVECTOR3 vOrg;
    D3DXVECTOR3 vDir;
    BOOL  bHit   = FALSE;
    DWORD dwFace = 0;

    KG_PROCESS_ERROR(m_pAttachScene);
    KG_PROCESS_ERROR(m_pAttachModel);

    hRetCode = m_pAttachScene->GetCurOutputWnd(&piSceneCurOutputWnd);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(piSceneCurOutputWnd);
    KG_PROCESS_ERROR(m_pSysMesh); 
    piSceneCurOutputWnd->GetPickRay(&vOrg, &vDir, NULL);

    D3DXMatrixIdentity(&matWorldInv);
    D3DXMatrixInverse(&matWorldInv, NULL, &m_pAttachModel->m_matWorld/*m_matWorld*/);
    D3DXVec3TransformCoord(&vOrg, &vOrg, &matWorldInv);
    D3DXVec3TransformNormal(&vDir, &vDir, &matWorldInv);

    D3DXIntersect(
        m_pSysMesh,
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

    KG_PROCESS_ERROR(bHit);

    m_nSelSubIndex = static_cast<int>(dwFace) / 8;
    if (bSelectedUpdate)
        m_nSeledIndex = m_nSelSubIndex;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DMeshBone::Render()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    
    D3DMATERIAL9 malNor;
    D3DMATERIAL9 malSelSock;
    D3DMATERIAL9 malMov;
    D3DMATERIAL9 malSel;
    D3DMATERIAL9 malBinded;
    D3DMATERIAL9 malWaitBind;

    D3DXMATRIX   matWorld;
    D3DXMATRIX   matSave;

    DWORD dwCurrTime = timeGetTime();

    KG3DModel *pModel = NULL;

    KG_PROCESS_ERROR(m_pAttachScene);
    KG_PROCESS_ERROR(m_pSysMesh);

    if (dwCurrTime - m_dwPrevTime > 100.0f)
    {
        m_nSelSubIndex = -1;
        UpdateSelBone();
        m_dwPrevTime = dwCurrTime;
    }

    ZeroMemory(&malNor, sizeof(malNor));
    ZeroMemory(&malSelSock, sizeof(malSelSock));
    ZeroMemory(&malSel, sizeof(malSel));
    ZeroMemory(&malMov, sizeof(malMov));
    ZeroMemory(&malBinded, sizeof(malBinded));
    ZeroMemory(&malWaitBind, sizeof(malWaitBind));

    malNor.Diffuse.r = 0.57f;
    malNor.Diffuse.g = 0.6f;
    malNor.Diffuse.b = 0.67f;
    malNor.Diffuse.a = 1.0f;
    malNor.Ambient = malNor.Diffuse;

    malSelSock.Diffuse.r = 1.0f;
    malSelSock.Diffuse.g = 1.0f;
    malSelSock.Diffuse.b = 0.0f;
    malSelSock.Diffuse.a = 1.0f;
    malSelSock.Ambient = malSelSock.Diffuse;

    malSel.Diffuse.r = 1.0f;
    malSel.Diffuse.g = 0.0f;
    malSel.Diffuse.b = 0.0f;
    malSel.Diffuse.a = 1.0f;
    malSel.Ambient = malSel.Diffuse;

    malMov.Diffuse.r = 0.7f;
    malMov.Diffuse.g = 0.7f;
    malMov.Diffuse.b = 1.0f;
    malMov.Diffuse.a = 1.0f;
    malMov.Ambient = malMov.Diffuse;

    malBinded.Diffuse.r = 0.7f;
    malBinded.Diffuse.g = 0.7f;
    malBinded.Diffuse.b = 0.0f;
    malBinded.Diffuse.a = 1.0f;
    malBinded.Ambient = malBinded.Diffuse;

    malWaitBind.Diffuse.r = 0.0f;
    malWaitBind.Diffuse.g = 0.0f;
    malWaitBind.Diffuse.b = 1.0f;
    malWaitBind.Diffuse.a = 1.0f;
    malWaitBind.Ambient = malWaitBind.Diffuse;

    D3DXMatrixIdentity(&matWorld);
    g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_pAttachModel->m_matWorld);
    //g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    g_pd3dDevice->SetTexture(0, 0);

    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

    for (DWORD i = 0; i < m_vecSubMeshInfo.size(); i++)
    {
        if ((int)i == m_nSeledIndex)
        {
            g_pd3dDevice->SetMaterial(&malSel);
        }
        else if ((int)i == m_nSelSubIndex)
        {
            if (m_vecSubMeshInfo[i].dwType == 2)
                g_pd3dDevice->SetMaterial(&malSelSock);
            else
                g_pd3dDevice->SetMaterial(&malMov);
        }
        else if (m_vecSubMeshInfo[i].dwType != 2)
        {
            g_pd3dDevice->SetMaterial(&malMov);
        }
        else
        {
            KG3DModelST* pModelST = dynamic_cast<KG3DModelST*>(m_pAttachModel);
            int nHaveBind = false;
            if (pModelST)
            {
                int nCount = pModelST->GetNumModel();
                for (int d = 0; d < nCount; d++)
                {
                    IKG3DModel *piModel = NULL;
                    pModelST->GetModel(d, &piModel);
                    pModel = dynamic_cast<KG3DModel *>(piModel);
                    KGLOG_PROCESS_ERROR(pModel);
                    for (
                        std::set<KG3DModelLod>::iterator n = pModel->m_ChildModels.begin();
                        n != pModel->m_ChildModels.end();
                        n++
                    )
                    {
                        string strBoneName;
						KG3DModelLod& Lod = *n;

                        Lod.pModelCurrent->GetBindBoneName(strBoneName);
                        if (strBoneName == m_vecSubMeshInfo[i].szBoneName)
                        {
                            nHaveBind = true;
                            break;
                        }
                    }
                    if (nHaveBind)
                        break;
                }
                if (nHaveBind)
                    g_pd3dDevice->SetMaterial(&malBinded);
                else
                {
                    g_pd3dDevice->SetMaterial(&malNor);
                }
            }
            else
            {
                g_pd3dDevice->SetMaterial(&malNor);
            }
        }
        hRetCode = m_pSysMesh->DrawSubset(i);
    }

    //g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DMeshBone::_CreateMesh()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    DWORD dwNumFaces    = (DWORD)m_vecSubMeshInfo.size() * 8;
    DWORD dwNumVertices = (DWORD)m_vecSubMeshInfo.size() * 6;

    VFormat::FACES_NORMAL_DIFFUSE_TEXTURE0* pVBuffer = NULL;
    WORD*  pIBuffer = NULL;
    DWORD* pABuffer = NULL;

    hRetCode = D3DXCreateMeshFVF(
        dwNumFaces,
        dwNumVertices,
        D3DXMESH_MANAGED,
        D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE,
        g_pd3dDevice,
        &m_pSysMesh
    );
    KG_COM_PROCESS_ERROR(hRetCode);

    // vertex
    hRetCode = m_pSysMesh->LockVertexBuffer(0, (void**)&pVBuffer);
    KG_COM_PROCESS_ERROR(hRetCode);

    for (DWORD i = 0; i < m_vecSubMeshInfo.size(); i++)
    {
        pVBuffer[i * 6].p     = m_vecSubMeshInfo[i].vPosition[0];
        pVBuffer[i * 6].diffuse = 0xFFFFFFFF;
        pVBuffer[i * 6 + 1].p = m_vecSubMeshInfo[i].vPosition[1];
        pVBuffer[i * 6 + 1].diffuse = 0xFFFF0000;
        pVBuffer[i * 6 + 2].p = m_vecSubMeshInfo[i].vPosition[2];
        pVBuffer[i * 6 + 2].diffuse = 0xFF0000FF;
        pVBuffer[i * 6 + 3].p = m_vecSubMeshInfo[i].vPosition[3];
        pVBuffer[i * 6 + 3].diffuse = 0xFF00FF00;
        pVBuffer[i * 6 + 4].p = m_vecSubMeshInfo[i].vPosition[4];
        pVBuffer[i * 6 + 4].diffuse = 0xFFFFFF00;
        pVBuffer[i * 6 + 5].p = m_vecSubMeshInfo[i].vPosition[5];
        pVBuffer[i * 6 + 5].diffuse = 0xFFFFFFFF;
    }

    hRetCode = m_pSysMesh->UnlockVertexBuffer();
    KG_COM_PROCESS_ERROR(hRetCode);


    // index
    hRetCode = m_pSysMesh->LockIndexBuffer(0, (void**)&pIBuffer);
    KG_COM_PROCESS_ERROR(hRetCode);

    for (DWORD i = 0; i < m_vecSubMeshInfo.size(); i++)
    {
        for (DWORD j = 0; j < 24; j++)
        {
            pIBuffer[i * 24 + j] = (WORD)(m_vecSubMeshInfo[i].dwIndex[j] + 6 * i);
        }
    }

    hRetCode = m_pSysMesh->UnlockIndexBuffer();
    KG_COM_PROCESS_ERROR(hRetCode);


    // index
    hRetCode = m_pSysMesh->LockAttributeBuffer(0, (DWORD**)&pABuffer);
    KG_COM_PROCESS_ERROR(hRetCode);

    for (DWORD i = 0; i < m_vecSubMeshInfo.size(); i++)
    {
        for (DWORD j = 0; j < 8; j++)
        {
            pABuffer[i * 8 + j] = i;
        }
    }

    hRetCode = m_pSysMesh->UnlockAttributeBuffer();
    KG_COM_PROCESS_ERROR(hRetCode);

    D3DXComputeNormals(m_pSysMesh, NULL);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DMeshBone::AddSubMesh(D3DXVECTOR3 v1, D3DXVECTOR3 v2, TCHAR* szBoneName, DWORD dwType)
{
    HRESULT hResult  = E_FAIL;

    _SubMeshInfo subMeshInfo;
    D3DXVECTOR3  vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXVECTOR3  vDir = v2 - v1;
    D3DXVECTOR3  vVer;
    D3DXVECTOR3  vHor;
    D3DXMATRIX   matRotation;
    FLOAT        fAngel = 0;
    FLOAT        fLen   = 0;

    ASSERT(szBoneName);

    fLen = D3DXVec3Length(&vDir);
    D3DXVec3Normalize(&vDir, &vDir);
    fAngel = D3DXVec3Dot(&vUp, &vDir);

    if (abs(acosf(fAngel)) <= 0.05f)
    {
        vVer = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
        vHor = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    }
    else
    {
        D3DXMatrixRotationY(&matRotation, D3DX_PI /2);
        D3DXVec3TransformNormal(&vVer, &vDir, &matRotation);

        D3DXVec3Cross(&vHor, &vDir, &vVer);
        D3DXVec3Cross(&vVer, &vDir, &vHor);
    }

    D3DXVec3Normalize(&vVer, &vVer);
    D3DXVec3Normalize(&vHor, &vHor);

    switch (dwType)
    {
    case 1 :
        vVer *= fLen / 2;
        vHor *= fLen / 2;
        subMeshInfo.vPosition[0] = v2;
        subMeshInfo.vPosition[1] = v1 + (vDir * fLen) - vHor - vVer;
        subMeshInfo.vPosition[2] = v1 + (vDir * fLen) + vHor - vVer;
        subMeshInfo.vPosition[3] = v1 + (vDir * fLen) + vHor + vVer;
        subMeshInfo.vPosition[4] = v1 + (vDir * fLen) - vHor + vVer;
        subMeshInfo.vPosition[5] = v1;
        break;
    case 0 :
        vVer *= fLen / 15;
        vHor *= fLen / 15;
        subMeshInfo.vPosition[0] = v2;
        subMeshInfo.vPosition[1] = v1 + (vDir * fLen / 15) - vHor - vVer;
        subMeshInfo.vPosition[2] = v1 + (vDir * fLen / 15) + vHor - vVer;
        subMeshInfo.vPosition[3] = v1 + (vDir * fLen / 15) + vHor + vVer;
        subMeshInfo.vPosition[4] = v1 + (vDir * fLen / 15) - vHor + vVer;
        subMeshInfo.vPosition[5] = v1;
        break;
    case 2 :
        vVer *= fLen / 15;
        vHor *= fLen / 15;
        subMeshInfo.vPosition[0] = v2;
        subMeshInfo.vPosition[1] = v1 + (vDir * fLen / 2) - vHor - vVer;
        subMeshInfo.vPosition[2] = v1 + (vDir * fLen / 2) + vHor - vVer;
        subMeshInfo.vPosition[3] = v1 + (vDir * fLen / 2) + vHor + vVer;
        subMeshInfo.vPosition[4] = v1 + (vDir * fLen / 2) - vHor + vVer;
        subMeshInfo.vPosition[5] = v1;
        break;
    default :
        KG_PROCESS_ERROR(0);
        break;
    }

    subMeshInfo.dwIndex[0]   = 0;
    subMeshInfo.dwIndex[1]   = 3;
    subMeshInfo.dwIndex[2]   = 4;

    subMeshInfo.dwIndex[3]   = 0;
    subMeshInfo.dwIndex[4]   = 2;
    subMeshInfo.dwIndex[5]   = 3;

    subMeshInfo.dwIndex[6]   = 0;
    subMeshInfo.dwIndex[7]   = 1;
    subMeshInfo.dwIndex[8]   = 2;

    subMeshInfo.dwIndex[9]   = 0;
    subMeshInfo.dwIndex[10]  = 4;
    subMeshInfo.dwIndex[11]  = 1;

   /* subMeshInfo.dwIndex[12]  = 1;
    subMeshInfo.dwIndex[13]  = 4;
    subMeshInfo.dwIndex[14]  = 2;

    subMeshInfo.dwIndex[15]  = 2;
    subMeshInfo.dwIndex[16]  = 4;
    subMeshInfo.dwIndex[17]  = 3;*/

    subMeshInfo.dwIndex[12]   = 5;
    subMeshInfo.dwIndex[13]   = 4;
    subMeshInfo.dwIndex[14]   = 3;

    subMeshInfo.dwIndex[15]   = 5;
    subMeshInfo.dwIndex[16]   = 3;
    subMeshInfo.dwIndex[17]   = 2;

    subMeshInfo.dwIndex[18]   = 5;
    subMeshInfo.dwIndex[19]   = 2;
    subMeshInfo.dwIndex[20]   = 1;

    subMeshInfo.dwIndex[21]   = 5;
    subMeshInfo.dwIndex[22]   = 1;
    subMeshInfo.dwIndex[23]   = 4;

    subMeshInfo.dwType = dwType;
    strncpy(subMeshInfo.szBoneName, szBoneName, sizeof(subMeshInfo.szBoneName));
    m_vecSubMeshInfo.push_back(subMeshInfo);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DMeshBone::Attach(KG3DModel* pModel, KG3DScene* pScene)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG3DModelST* pModelST  = NULL;
    KG3DModel *pModelInVector = NULL;
    
    KG_PROCESS_ERROR(pModel);
    KG_PROCESS_ERROR(pScene);


    m_pAttachScene = pScene;
    m_pAttachModel = pModel;

    m_matWorld = m_pAttachModel->m_matWorld;
    m_vecSubMeshInfo.clear();

    SAFE_RELEASE(m_pSysMesh);
    pModelST = dynamic_cast<KG3DModelST*>(pModel);
    if (pModelST)
    {
        KG3DBip* pBip = const_cast<KG3DBip*>(pModelST->GetBip());
        int nNumBonew = 0;
       
        KG_PROCESS_ERROR(pBip);
        nNumBonew = pBip->GetNumBones();
        for (int i = 0; i < nNumBonew; i++)
        {
            int nParentIndex = 0;
            D3DXVECTOR3 v1;
            D3DXVECTOR3 v2;
            D3DXMATRIX matObj = pBip->GetBoneObjMatrix(i);
            v1.x = matObj._41;
            v1.y = matObj._42;
            v1.z = matObj._43;
            nParentIndex = pBip->GetParentIndex(i);
            matObj = pBip->GetBoneObjMatrix(nParentIndex);
            v2.x = matObj._41;
            v2.y = matObj._42;
            v2.z = matObj._43;

            if (v1 == v2)
                continue;

            AddSubMesh(v2, v1, const_cast<TCHAR*>(pBip->GetBoneName(i)), 0);
        }

        int nCount = pModelST->GetNumModel();

        for (int s = 0; s < nCount; s++)
        {
            IKG3DModel *piModel = NULL;
            pModelST->GetModel(s, &piModel);

            pModelInVector = dynamic_cast<KG3DModel *>(piModel);
            KGLOG_PROCESS_ERROR(pModelInVector);
            
            KG3DMesh *pMesh = pModelInVector->GetMesh();
            if (NULL == pMesh)
                continue;
            DWORD dwNumSocket = 0;
            dwNumSocket = pMesh->m_dwNumSocket;
            for (DWORD i = 0; i < dwNumSocket; i++)
            {
                D3DXVECTOR3 v1 = D3DXVECTOR3(-15.0f, 0.0f, 0.0f);
                D3DXVECTOR3 v2 = D3DXVECTOR3( 15.0f, 0.0f, 0.0f);
                //D3DXMATRIX* pMatSocket = pMesh->GetSocketMatrix(i);
                D3DXMATRIX matSocket;
                pModelInVector->GetSocketMatrix(i, matSocket);
                D3DXVec3TransformCoord(&v1, &v1, &matSocket);
                D3DXVec3TransformCoord(&v2, &v2, &matSocket);

                AddSubMesh(
                    v2, v1,
                    pMesh->m_pSockets[i].strSocketName,
                    2
                );
            }
            //pMesh->Release();//原来GetMesh是加引用计数的，现在不加
        }

        {
            D3DXVECTOR3 v1 = D3DXVECTOR3(0.0f,  10.0f, 0.0f);
            D3DXVECTOR3 v2 = D3DXVECTOR3(0.0f,   0.0f, 0.0f);
            //D3DXVec3TransformCoord(&v1, &v1, &pModelST->m_matWorld);
            //D3DXVec3TransformCoord(&v2, &v2, &pModelST->m_matWorld);

            AddSubMesh(v2, v1, TEXT("Origin"), 0);
       
        }
    }
    else
    {
        KG3DMesh* pMesh = pModel->GetMesh();
		KG_PROCESS_ERROR(NULL != pMesh);

        int nNumBonew = 0;

        nNumBonew = pMesh->GetNumBones();
        for (int i = 0; i < nNumBonew; i++)
        {
            D3DXVECTOR3 v1;
            D3DXVECTOR3 v2;
            D3DXMATRIX matObj = pModel->m_pBoneMatricesForRender[i];
            v1.x = matObj._41;
            v1.y = matObj._42;
            v1.z = matObj._43;

            if (pMesh->m_pBoneInfo[i].dwNumChild > 0)
            {
                for (DWORD s = 0; s < pMesh->m_pBoneInfo[i].dwNumChild; ++s)
                {
                    DWORD childIndex = pMesh->m_pBoneInfo[i].dwChildIndex[s];
                    matObj = pModel->m_pBoneMatricesForRender[childIndex];
                    v2.x = matObj._41;
                    v2.y = matObj._42;
                    v2.z = matObj._43;

                    if (v1 == v2)
                        continue;

                    AddSubMesh(v1, v2, const_cast<TCHAR*>(pMesh->GetBoneName(i)), 0);
                }
            }
            else
            {
                D3DXVECTOR3 v = D3DXVECTOR3(matObj._11, matObj._12, matObj._13);
                v2 = v1 + v * 10.f;
                AddSubMesh(v1, v2, const_cast<TCHAR*>(pMesh->GetBoneName(i)), 0);
            }

            /*
            int nParentIndex = 0;
            D3DXVECTOR3 v1;
            D3DXVECTOR3 v2;
            D3DXMATRIX matObj = pModel->m_pBoneMatricesForRender[i];// pBip->GetBoneObjMatrix(i);
            v1.x = matObj._41;
            v1.y = matObj._42;
            v1.z = matObj._43;
            nParentIndex = static_cast<int>(pMesh->m_pBoneInfo[i].dwParentIndex);
            matObj = pModel->m_pBoneMatricesForRender[nParentIndex];
            v2.x = matObj._41;
            v2.y = matObj._42;
            v2.z = matObj._43;

            if (v1 == v2)
                continue;

            AddSubMesh(v2, v1, const_cast<TCHAR*>(pMesh->GetBoneName(i)), 0);
            */
        }       
        
        DWORD dwNumSocket = 0;
        dwNumSocket = pMesh->m_dwNumSocket;
        for (DWORD i = 0; i < dwNumSocket; i++)
        {
            D3DXVECTOR3 v1 = D3DXVECTOR3(-15.0f, 0.0f, 0.0f);
            D3DXVECTOR3 v2 = D3DXVECTOR3( 15.0f, 0.0f, 0.0f);
            D3DXMATRIX* pMatSocket = pMesh->GetSocketMatrix(i);
            D3DXVec3TransformCoord(&v1, &v1, pMatSocket);
            D3DXVec3TransformCoord(&v2, &v2, pMatSocket);

            AddSubMesh(
                v2, v1,
                pMesh->m_pSockets[i].strSocketName,
                2
                );
        }
    }

    hRetCode = _CreateMesh();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

void KG3DMeshBone::SetAttachScene(KG3DScene* pScene)
{
	m_pAttachScene = pScene;
}