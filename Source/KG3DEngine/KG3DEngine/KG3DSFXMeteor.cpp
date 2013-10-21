////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXMeteor.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-2-8 10:14:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXMeteor.h"
#include "KG3DSFX.h"
#include "KG3DGraphicsTool.h"
#include "KG3DAnimationController.h"
#include "KG3DModelST.h"
#include "KG3DBip.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


KG3DSFXMeteor::KG3DSFXMeteor(KG3DSFX* pSFX)
{
    ASSERT(pSFX);

    m_pSFX = pSFX;
    m_lpVertexBuffer = NULL;
    m_uVertexCnt = 0;

    m_fLastFrame = 0;
    m_fLastEmitFrame = 0;

    m_fLife = 20.0f;
    m_fEmitMinInterval = 1.0f;
    m_nTrackIndex = -1;

    m_fWidth             = 20.0f;

    m_dwTextureIdx = 0;

    m_bRandomOffset = FALSE;
    m_vOffset = D3DXVECTOR3(0, 0, 0);

    m_BlendMode          = SFX_BM_SRCALPHA_INVSRCALPHA;
    m_nLineNumber        = 0;
    m_fInterpolateLength = -1;
    m_fPrevFrame         = -1;
    m_fRallyingScale     = 0.5f;
	m_bSampling = FALSE;
    m_fPrevTrackFrame = 0.f;
    m_fDuration = 0.f;
	//m_MeteorList.push_back(m_listNode);

    m_RtsTimeLine.m_TranslationTimeLine.InsertKeyFrame(0, D3DXVECTOR3(0.f, 0.f, 0.f));
    m_RtsTimeLine.m_RotationTimeLine.InsertKeyFrame(0, KG3DTransformation::IdentiQuaternion);
    m_RtsTimeLine.m_ScalingTimeLine.InsertKeyFrame(0, D3DXVECTOR3(1.f, 1.f, 1.f));
    m_RtsTimeLine.m_ScalRotationTimLine.InsertKeyFrame(0, KG3DTransformation::IdentiQuaternion);

}

KG3DSFXMeteor::~KG3DSFXMeteor(void)
{
    SAFE_RELEASE(m_lpVertexBuffer);
	//KGMETEORLIST::iterator it = m_MeteorList.begin();
	//list<METEOR_NODE>::iterator i;
	//while(it != m_MeteorList.end())
	//{
	//	i =(*it)->begin();
	//	while(i != (*it)->end())
	//	{
	//		i->clear();
	//		i++;
	//	}
	//	it++;
	//}
	m_MeteorList.clear();
}

HRESULT KG3DSFXMeteor::SetRallyingScale(float fScanl)
{
    if (fScanl > 0.0f && fScanl < 1.0f)
        m_fRallyingScale = fScanl;
    return S_OK;
}

float KG3DSFXMeteor::GetRallyingScale()
{
    return m_fRallyingScale;
}


HRESULT KG3DSFXMeteor::SetMeteorInterpolate(float fInterval)
{
    m_fInterpolateLength = fInterval;
    return S_OK;
}

float KG3DSFXMeteor::GetMeteorInterpolate()
{
    return m_fInterpolateLength;
}

void KG3DSFXMeteor::InterpolateAppend(METEOR_NODE& node)
{
   /* if (!m_listNode.size())
        return;
    if (m_listNode.back().TanBegin == node.TanBegin)
        return;

    METEOR_NODE backNode = m_listNode.back();
    FLOAT vLen1 = D3DXVec3Length(&(backNode.PosBegin - node.PosBegin));
    FLOAT vLen2 = D3DXVec3Length(&(backNode.PosEnd - node.PosEnd));
    FLOAT fDistance = vLen1 > vLen2 ? vLen1 : vLen2;
    int   nIntCount = (int)(fDistance / m_fInterpolateLength);*/
	if (!m_MeteorList.size())
		return;
	if (!m_MeteorList.back().size())
		return;
	if (m_MeteorList.back().back().TanBegin == node.TanBegin)
		return;

	METEOR_NODE backNode = m_MeteorList.back().back();
	FLOAT vLen1 = D3DXVec3Length(&(backNode.PosBegin - node.PosBegin));
	FLOAT vLen2 = D3DXVec3Length(&(backNode.PosEnd - node.PosEnd));
	FLOAT fDistance = vLen1 > vLen2 ? vLen1 : vLen2;
	int   nIntCount = (int)(fDistance / m_fInterpolateLength);
    //
  /*  D3DXVECTOR3 dirB;
    D3DXVECTOR3 dirE;
    D3DXVec3Normalize(&dirB, &(node.PosBegin - backNode.PosBegin));
    D3DXVec3Normalize(&dirE, &(node.PosEnd - backNode.PosEnd));
    float fDotB = D3DXVec3Dot(&node.TanBegin, &dirB);
    float fDotE = D3DXVec3Dot(&node.TanEnd, &dirE);
    if (fDotB < 0.0f)
        node.TanBegin *= -1;
    if (fDotE < 0.0f)
        node.TanEnd *= -1;*/
    //

    if (nIntCount > 1)
    {
        float fLifeDist   = node.fLife - backNode.fLife;
        float fStride     = 1.0f / (float)nIntCount;
        float fListStride = fLifeDist / (float)nIntCount;
        for (int i = 1; i < nIntCount; i++)
        {
            D3DXVECTOR3 vDirection;
            D3DXVECTOR3 vCenter;
            METEOR_NODE addNode;
            Hermite(
                &addNode.PosBegin, 
                &backNode.PosBegin,
                &backNode.TanBegin,
                &node.PosBegin,
                &node.TanBegin,
                i * fStride
            );
            Hermite(
                &addNode.PosEnd, 
                &backNode.PosEnd,
                &backNode.TanEnd,
                &node.PosEnd,
                &node.TanEnd,
                i * fStride
            );
            /*vCenter = (addNode.PosBegin + addNode.PosEnd) * 0.5f;
            D3DXVec3Normalize(&vDirection, &(addNode.PosBegin - addNode.PosEnd));
            addNode.PosBegin = vCenter + vDirection * m_fWidth;
            addNode.PosEnd   = vCenter - vDirection * m_fWidth;*/
            
            addNode.fLife = i * fListStride + backNode.fLife;
            if (m_bRandomOffset)
            {
                addNode.vVelocity = D3DXVECTOR3(
                    GetRandom(-m_vOffset.x, m_vOffset.x),
                    GetRandom(-m_vOffset.x, m_vOffset.x),
                    GetRandom(-m_vOffset.x, m_vOffset.x)
                );
            }
            else
                addNode.vVelocity = m_vOffset;
            m_MeteorList.back().push_back(addNode);
        }
    }
}

BOOL D3DXVec3HazySameDirection(D3DXVECTOR3& v1, D3DXVECTOR3&v2)
{
    if (
        fabs(v1.x - v2.x) < 0.005f &&
        fabs(v1.y - v2.y) < 0.005f &&
        fabs(v1.z - v2.z) < 0.005f
        )
        return TRUE;
    else
        return FALSE;
}

void KG3DSFXMeteor::_BindInfo::clear()
{
    eBindType    = ENUM_BIND_FORCE_DWORD;
    pDepdModelST = NULL;
    pDependModel = NULL;
    pSFXDepModel = NULL;
    pBip         = NULL;
    pClip        = NULL;
    pImmDepModel = NULL;
    nBoneIndex   = -1;
    nSockIndex   = -1;
    strBindName  = TEXT("");
    D3DXMatrixIdentity(&matSockOffset);
    pAinmationController = NULL;
    vecSkeletons.clear();
}

void KG3DSFXMeteor::SetPrecisionAnimationController(KG3DAnimationController *pController)
{
	m_PrecisionBindInfo.pAinmationController = pController;
}

HRESULT KG3DSFXMeteor::UpdateBindInfo()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    enuModelBindType eBindType = ENUM_BIND_FORCE_DWORD;

  //  KG3DModelST*  pDepdModelST = NULL;
    KG3DModel*    pDependModel = NULL;
    KG3DModel*    pSFXDepModel = NULL;
    string        strBindName;
    BindExtraInfo info;
    D3DXMATRIX    matSockOffset;

    int           nBoneIndex = -1;
    int           nSockIndex = -1;

	KG3DClip* pClip = NULL;

    m_pSFX->GetParentModel((IKG3DModel**)&pSFXDepModel);
    KG_PROCESS_ERROR(pSFXDepModel);

    pSFXDepModel->GetParentModel((IKG3DModel**)&pDependModel);
    KG_PROCESS_ERROR(pDependModel);

    /*
    pDepdModelST = dynamic_cast<KG3DModelST*>(pDependModel);
    KG_PROCESS_ERROR(pDepdModelST);
    */

    eBindType = pSFXDepModel->GetBindType();

    hRetCode = pSFXDepModel->GetBindBoneName(strBindName);
    KG_COM_PROCESS_ERROR(hRetCode);

    if (
        (pSFXDepModel != m_PrecisionBindInfo.pSFXDepModel) ||
        (pDependModel != m_PrecisionBindInfo.pDependModel) ||
      //  (pDepdModelST != m_PrecisionBindInfo.pDepdModelST) ||
        (eBindType    != m_PrecisionBindInfo.eBindType)    ||
        (strBindName  != m_PrecisionBindInfo.strBindName)
        )
    {
        m_PrecisionBindInfo.pSFXDepModel = pSFXDepModel;
        m_PrecisionBindInfo.pDependModel = pDependModel;
      //  m_PrecisionBindInfo.pDepdModelST = pDepdModelST;
        m_PrecisionBindInfo.eBindType    = eBindType;
        m_PrecisionBindInfo.strBindName  = strBindName;

        if (eBindType == ENUM_BIND_BONE)
        {
            nBoneIndex = pDependModel->FindBone(strBindName.c_str(), &info);
            KG_PROCESS_ERROR(nBoneIndex != -1);
            KG_PROCESS_ERROR(info.pModel);
            if (info.pModel->m_Mesh2Bip.size())
                nBoneIndex = info.pModel->m_Mesh2Bip[nBoneIndex];
        }
        else
        {
            nSockIndex = pDependModel->FindSocket(strBindName.c_str(), &info);
            KG_PROCESS_ERROR(nSockIndex != -1);
            KG_PROCESS_ERROR(info.pModel);
            KG_PROCESS_ERROR(info.pModel->GetMesh());
            nBoneIndex = 
                info.pModel->GetMesh()->m_pSockets[nSockIndex].dwParentBoneIndex;
            KG_PROCESS_ERROR(nBoneIndex != -1);
            if (info.pModel->m_Mesh2Bip.size())
                nBoneIndex = info.pModel->m_Mesh2Bip[nBoneIndex];
            matSockOffset = 
                info.pModel->GetMesh()->m_pSockets[nSockIndex].matMatrixOffset;
        }
        
        m_PrecisionBindInfo.matSockOffset = matSockOffset; 
        m_PrecisionBindInfo.nBoneIndex    = nBoneIndex;
        m_PrecisionBindInfo.nSockIndex    = nSockIndex;
        m_PrecisionBindInfo.pImmDepModel  = info.pModel;

        m_PrecisionBindInfo.pBip  = pDependModel->m_pBip;//const_cast<KG3DBip*>(pDepdModelST->GetBip());
        KG_PROCESS_ERROR(m_PrecisionBindInfo.pBip);

        m_PrecisionBindInfo.pBip->GetSkeleton(m_PrecisionBindInfo.vecSkeletons, nBoneIndex);

        ASSERT(m_PrecisionBindInfo.vecSkeletons.size());
        KG_PROCESS_ERROR(m_PrecisionBindInfo.vecSkeletons.size());
    }

	KG_PROCESS_ERROR(m_PrecisionBindInfo.pAinmationController);
	pClip = m_PrecisionBindInfo.pAinmationController->GetCurAnimation();
	KG_PROCESS_ERROR(pClip);
	ASSERT(pClip);

	//pClipInfo = *(pAinmation->m_ClipList.begin());

	//ASSERT(pClipInfo);
	//KG_PROCESS_ERROR(pClipInfo);

	//ASSERT(pClipInfo->piClip);
	//KG_PROCESS_ERROR(pClipInfo->piClip);

	m_PrecisionBindInfo.pClip = dynamic_cast<KG3DClip *>(pClip);
	KG_PROCESS_ERROR(m_PrecisionBindInfo.pClip);


	hResult = S_OK;
Exit0:
    if (hResult != S_OK)
        m_PrecisionBindInfo.clear();
    return hResult;
}

//vector<D3DXMATRIX> GetIniMatrixRange(FLOAT fBeginFrame, FLOAT  fEndFrame, FLOAT  fPrecision)
int GetAniMatrixRange(vector<D3DXMATRIX>& vecMatrixs, FLOAT fBeginFrame, FLOAT  fEndFrame, FLOAT  fPrecision)
{
    int nResult  = false;

    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity(&matIdentity);

    //vector<D3DXMATRIX> vecMatrixs;

    ASSERT(fPrecision > 0.0f);

    FLOAT fIter = fBeginFrame + fPrecision;

    if (fIter > fEndFrame)
        fIter = fEndFrame;

    while (fIter <= fEndFrame)
    {
        vecMatrixs.push_back(matIdentity);
        fIter += fPrecision;
    }

    if (fIter > fEndFrame && fIter < fEndFrame + fPrecision)
        vecMatrixs.push_back(matIdentity);

    nResult = true;
//Exit0:
    return (int)vecMatrixs.size();
}

HRESULT KG3DSFXMeteor::UpdatePrecisionMatrix(vector<D3DXMATRIX>& vecMatrixs, FLOAT fPrecision, D3DXMATRIX matTrack)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    vector<D3DXMATRIX> vecMatTemp;
    
	FLOAT fWeight      = 0.0f;
    DWORD dwFrameTo    = 0;
    DWORD dwFrame      = 0;
    FLOAT fCurrFrame   = 0.0f;
    INT   nNumberFrame = 0;

    KG_PROCESS_ERROR(m_pSFX);
	KG_PROCESS_ERROR(fPrecision > 0.f);

    if (fPrecision < (FLOAT)KG3DSFX_MAXMAL_PRECISION)
        fPrecision = (FLOAT)KG3DSFX_MAXMAL_PRECISION;

    hRetCode = UpdateBindInfo();
    KG_COM_PROCESS_ERROR(hRetCode);

    KG_PROCESS_ERROR(m_PrecisionBindInfo.pAinmationController);
	if (m_PrecisionBindInfo.pAinmationController->IsPaused())
    {
        m_fPrevFrame = -1;
        KG_PROCESS_SUCCESS(TRUE);
    }
    
    //
    DWORD dwAniPlayTime = m_PrecisionBindInfo.pAinmationController->GetPlayTime();
    DWORD dwPlayType = m_PrecisionBindInfo.pAinmationController->GetPlayType();
    float fAniSpeed = m_PrecisionBindInfo.pAinmationController->GetSpeed();
    m_PrecisionBindInfo.pClip->GetCurFrame(
        (DWORD)(dwAniPlayTime * fAniSpeed), dwPlayType,
        &dwFrame, &dwFrameTo, &fWeight
    );
    fCurrFrame = dwFrame + fWeight;

    if (m_fPrevFrame == -1 || m_fPrevFrame >= fCurrFrame)
    {
        m_fPrevFrame = fCurrFrame;
        KG_PROCESS_ERROR(false);
    }

    nNumberFrame = static_cast<INT>(ceil((fCurrFrame - m_fPrevFrame) / fPrecision));

    ASSERT(nNumberFrame);
    KG_PROCESS_ERROR(nNumberFrame);

    vecMatrixs.clear();
    GetAniMatrixRange(vecMatrixs, m_fPrevFrame, fCurrFrame, fPrecision);

	for (int i = (int)m_PrecisionBindInfo.vecSkeletons.size() - 1; i >= 0 ; i--)
    {
         vecMatTemp.clear(); 
         m_PrecisionBindInfo.pClip->GetMatrixsRange(
             vecMatTemp,
             m_PrecisionBindInfo.vecSkeletons[i], 
             m_fPrevFrame, 
             fCurrFrame, 
             fPrecision
             );

        ASSERT(vecMatTemp.size() == vecMatrixs.size());

        for (DWORD j = 0; j < vecMatrixs.size(); j++)
            vecMatrixs[j] = vecMatTemp[j] * vecMatrixs[j];
    }

#if 0
    KGLogPrintf(KGLOG_DEBUG, "matrix number : %u\n", vecMatrixs.size());
#endif

    if (m_PrecisionBindInfo.eBindType == ENUM_BIND_SOCKET)
    {
        for (DWORD j = 0; j < vecMatrixs.size(); j++)
            vecMatrixs[j] = m_PrecisionBindInfo.matSockOffset * vecMatrixs[j];
    } 

    for (DWORD j = 0; j < vecMatrixs.size(); j++)
    {
        D3DXMATRIX matComb3_1 = m_PrecisionBindInfo.pSFXDepModel->m_Matrix * (vecMatrixs[j] * m_PrecisionBindInfo.pImmDepModel->m_matWorld);
        D3DXMATRIX matComb2_1 = m_pSFX->m_Matrix * /* m_pSFX->m_matWorldSave */ matComb3_1;
        D3DXMATRIX matCombAll = m_Matrix * matTrack * matComb2_1;
        vecMatrixs[j] = matCombAll;
    }

    m_fPrevFrame = fCurrFrame;

Exit1 :
    hResult = S_OK;
Exit0 :
    return hResult;
}

HRESULT KG3DSFXMeteor::Update(float fCurrentFrame)
{
    HRESULT hResult = E_FAIL;

    KG_PROCESS_ERROR(m_pSFX);

    if (fCurrentFrame != m_fLastFrame)
    {
        m_RtsTimeLine.m_TranslationTimeLine.GetData(&m_Translation, fCurrentFrame);
        m_RtsTimeLine.m_RotationTimeLine.GetData(&m_Rotation, fCurrentFrame);
        m_RtsTimeLine.m_ScalingTimeLine.GetData(&m_Scaling, fCurrentFrame);
        m_RtsTimeLine.m_ScalRotationTimLine.GetData(&m_ScalingRotation, fCurrentFrame);
        UpdateTransformation();
    }

    if (fCurrentFrame < m_fLastFrame)
    {
        m_fLastFrame = fCurrentFrame;
        m_fLastEmitFrame = 0;
        KG_PROCESS_SUCCESS(TRUE);
    }
    else if (fCurrentFrame == m_fLastFrame)
        KG_PROCESS_SUCCESS(TRUE);

    float fElapsed = fCurrentFrame - m_fLastFrame;
    m_fLastFrame = fCurrentFrame;

	if (!m_MeteorList.size())
	{
		list<METEOR_NODE> plistNode;		
		m_MeteorList.push_back(plistNode);
	}	

    {
        KGMETEORLIST::iterator it = m_MeteorList.begin();

        while(it != m_MeteorList.end())
        {
            list<METEOR_NODE>::iterator i = it->begin();

            while (i != it->end())
            {
                i->fLife    -= fElapsed;
                i->PosBegin += (i->vVelocity * fElapsed * (float)m_pSFX->GetFrameInterval());
                i->PosEnd   += (i->vVelocity * fElapsed * (float)m_pSFX->GetFrameInterval());
                ++i;
            }

            while (it->size()) 
            {
                METEOR_NODE& node = it->front();
                if (node.fLife < 0)
                    it->pop_front();
                else
                    break;
            };

            it++;
        }
    }

    if (m_MeteorList.back().size() > KG3DSFX_METEOR_LINE_MAX_SIZE)
        KG_PROCESS_SUCCESS(TRUE);

    if (m_fDuration > 0.f && (m_pSFX->GetPlayTime() / 1000.f) > m_fDuration)
        KG_PROCESS_SUCCESS(TRUE);

    if (m_bSampling)
    {
        METEOR_NODE node;

        node.fLife    = m_fLife;
        node.PosBegin = D3DXVECTOR3(-m_fWidth, 0, 0);
        node.PosEnd   = D3DXVECTOR3( m_fWidth, 0, 0);
        node.TanBegin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        node.TanEnd   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        
        if (m_bRandomOffset)
        {
            node.vVelocity = D3DXVECTOR3(
                GetRandom(-m_vOffset.x, m_vOffset.x),
                GetRandom(-m_vOffset.x, m_vOffset.x),
                GetRandom(-m_vOffset.x, m_vOffset.x)
                );
        }
        else
            node.vVelocity = m_vOffset;

        D3DXMATRIX matTrack;
        D3DXMatrixIdentity(&matTrack);
        
        if (m_nTrackIndex != -1)
        {
            KG3DSFXTrack *pTrack = NULL;

            if (SUCCEEDED(m_pSFX->GetTrack(m_nTrackIndex, &pTrack)))
            {
                float fFrame = 0;
                pTrack->GetTransform(fCurrentFrame, &matTrack, &fFrame);

                if (fFrame < m_fPrevFrame)
                {
                    m_MeteorList.clear();
                    m_fPrevFrame = fFrame;
                    KG_PROCESS_SUCCESS(TRUE);
                }

                m_fPrevFrame = fFrame;
            }
        }

        vector<D3DXMATRIX> vecMatrixs;
        vecMatrixs.push_back(m_Matrix * matTrack * (*m_pSFX->GetCurrentWorldMatrix()));

        if (false && g_cEngineOption.bEnableBladesSmoot)
            UpdatePrecisionMatrix(vecMatrixs, m_fInterpolateLength, matTrack);

        ASSERT(vecMatrixs.size() > 0);
        KG_PROCESS_ERROR(vecMatrixs.size() > 0);

        if (m_fInterpolateLength != -1)
        {
            for (DWORD i = 0; i < vecMatrixs.size(); i++)
            {
                METEOR_NODE nd = node;
                D3DXVec3TransformCoord(&nd.PosBegin,  &nd.PosBegin, &vecMatrixs[i]);
                D3DXVec3TransformCoord(&nd.PosEnd,    &nd.PosEnd,   &vecMatrixs[i]);
                m_MeteorList.back().push_back(nd);
            }
            m_fLastEmitFrame = fCurrentFrame;
        }
        else
        {
            if (m_MeteorList.back().size() && (fCurrentFrame - m_fLastEmitFrame) < m_fEmitMinInterval)
            {
                METEOR_NODE& backNode = m_MeteorList.back().back();
                D3DXVec3TransformCoord(&node.PosBegin,  &node.PosBegin, &vecMatrixs[0]);
                D3DXVec3TransformCoord(&node.PosEnd,    &node.PosEnd,   &vecMatrixs[0]);
                backNode = node;
            }
            else
            {
                D3DXVec3TransformCoord(&node.PosBegin,  &node.PosBegin, &vecMatrixs[0]);
                D3DXVec3TransformCoord(&node.PosEnd,    &node.PosEnd,   &vecMatrixs[0]);
                m_MeteorList.back().push_back(node);
                m_fLastEmitFrame = fCurrentFrame;
            }
        }
    }

    {
        KGMETEORLIST::iterator itor = m_MeteorList.begin();

        while (itor != m_MeteorList.end() )
        {
            if ((!itor->size()))
            {
                KGMETEORLIST::iterator itNext = itor;
                itNext++;
                m_MeteorList.erase(itor);
                itor = itNext;
            }
            else
                itor++;
        }      
    }

Exit1 :
    hResult = S_OK;
Exit0 :
    return hResult;
}

void KG3DSFXMeteor::AddListNode()
{
	list<METEOR_NODE> plistNode;		
	m_MeteorList.push_back(plistNode);
}

HRESULT KG3DSFXMeteor::OnLostDevice()
{
    SAFE_RELEASE(m_lpVertexBuffer);

    return S_OK;
}

HRESULT KG3DSFXMeteor::OnResetDevice()
{
    return S_OK;
}

HRESULT KG3DSFXMeteor::Render()
{
	HRESULT hRetCode = E_FAIL;

	if (!m_MeteorList.size())
		return S_OK;
	KGMETEORLIST::iterator it = m_MeteorList.begin();
	while (it != m_MeteorList.end())
	{
		if (it->size() < 2)
		{
			it++;
			continue;
		}
		D3DXMATRIX matWorldSave;
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);
		g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

		if (
			!m_lpVertexBuffer                      || 
			m_nLineNumber < (int)it->size() || 
			m_nLineNumber > (int)it->size() - 50
			)
		{
			m_nLineNumber = (int)it->size();
			SAFE_RELEASE(m_lpVertexBuffer);

			DWORD dwThreadID = GetCurrentThreadId();
			ASSERT(dwThreadID == g_dwMainThreadID);

			hRetCode = g_pd3dDevice->CreateVertexBuffer(
				sizeof(VFormat::_Faces_Diffuse_Texture1) * m_nLineNumber * 2,
				D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
				D3DFVF_Faces_Diffuse_Texture1, 
				D3DPOOL_DEFAULT, 
				&m_lpVertexBuffer, 
				NULL
				);
			if (FAILED(hRetCode))
				return E_FAIL;
		}

		VFormat::_Faces_Diffuse_Texture1 *pData = NULL;
		hRetCode = m_lpVertexBuffer->Lock(0, 0, (VOID**)&pData, D3DLOCK_DISCARD);
		if (FAILED(hRetCode))
			return E_FAIL;

		int nRallyingIndex = static_cast<int>(it->size() * m_fRallyingScale) - 1;
		if (nRallyingIndex < 0)
			nRallyingIndex = 0;
		float size = (float)it->size();
		if (m_fRallyingScale != 0.5f && nRallyingIndex)
		{
			list<METEOR_NODE>::iterator i = it->begin();
			int nodeIdx = 0;
			while (i != it->end())
			{
				pData[nodeIdx * 2].p = i->PosBegin;
				pData[nodeIdx * 2 + 1].p = i->PosEnd;

				pData[nodeIdx * 2].tu1 = 0.0f;
				pData[nodeIdx * 2 + 1].tu1 = 1.0f;

				if (nodeIdx <= nRallyingIndex)
				{
					pData[nodeIdx * 2].tv1 = 1.0f - 0.5f * nodeIdx / nRallyingIndex; 
					pData[nodeIdx * 2 + 1].tv1 = 1.0f - 0.5f * nodeIdx / nRallyingIndex; 
				}
				else
				{
					pData[nodeIdx * 2].tv1 = 
						0.5f * (size - 1) / ((size - 1) - nRallyingIndex) -
						nodeIdx * 0.5f / ((size - 1) - nRallyingIndex);
					pData[nodeIdx * 2 + 1].tv1 = 
						0.5f * (size - 1) / ((size - 1) - nRallyingIndex) -
						nodeIdx * 0.5f / ((size - 1) - nRallyingIndex);
				}

				DWORD alpha = (DWORD)(i->fLife / m_fLife * 255.0f);
				pData[nodeIdx * 2].diffuse = D3DCOLOR_ARGB(alpha, 255, 255, 255);
				pData[nodeIdx * 2 + 1].diffuse = D3DCOLOR_ARGB(alpha, 255, 255, 255);

				++nodeIdx;
				++i;
			}
		}
		else
		{
			list<METEOR_NODE>::iterator i = it->begin();
			int nodeIdx = 0;
			while (i != it->end())
			{
				pData[nodeIdx * 2].p = i->PosBegin;
				pData[nodeIdx * 2 + 1].p = i->PosEnd;

				pData[nodeIdx * 2].tu1 = 0.0f;
				pData[nodeIdx * 2 + 1].tu1 = 1.0f;

				pData[nodeIdx * 2].tv1 = 1.0f - nodeIdx / (size - 1); 
				pData[nodeIdx * 2 + 1].tv1 = 1.0f - nodeIdx / (size - 1); 

				DWORD alpha = (DWORD)(i->fLife / m_fLife * 255.0f);
				pData[nodeIdx * 2].diffuse = D3DCOLOR_ARGB(alpha, 255, 255, 255);
				pData[nodeIdx * 2 + 1].diffuse = D3DCOLOR_ARGB(alpha, 255, 255, 255);

				++nodeIdx;
				++i;
			}
		}

		hRetCode = m_lpVertexBuffer->Unlock();
		if (FAILED(hRetCode))
			return E_FAIL;

		BOOL bTextureReady = FALSE;

		g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture1);
		g_pd3dDevice->SetStreamSource(0, m_lpVertexBuffer, 0, sizeof(VFormat::_Faces_Diffuse_Texture1));
		KG3DTexture *pTexture = NULL;
		m_pSFX->GetTexture(m_dwTextureIdx, &pTexture);
		if (pTexture)
		{
			bTextureReady = pTexture->IsResourceReady();
			g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
		}
		else
			g_pd3dDevice->SetTexture(0, NULL);

		if (bTextureReady && it->size() > 1)
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, ((UINT)it->size() - 1) * 2);

		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
		it++;
	}
	
	return S_OK;
}

//BOOL KG3DSFXMeteor::IsRayIntersect(D3DXVECTOR3 &Inter, D3DXVECTOR3 Origin, D3DXVECTOR3 Direction)
//{
//    // this function only be called when ray is through its AABB.
//    // so always return TRUE.
//    Inter = Origin;
//    return TRUE;
//}

HRESULT KG3DSFXMeteor::Hermite(
                D3DXVECTOR3 *pOut,
                CONST D3DXVECTOR3 *pV1,
                CONST D3DXVECTOR3 *pT1,
                CONST D3DXVECTOR3 *pV2,
                CONST D3DXVECTOR3 *pT2,
                FLOAT s
                )
{
    FLOAT fLenSD = D3DXVec3Length(&(*pV2 - *pV1));
    D3DXVECTOR3 T1;
    D3DXVECTOR3 T2;
    D3DXVec3Normalize(&T1, pT1);
    D3DXVec3Normalize(&T2, pT2);
    T1 *= fLenSD;
    T2 *= fLenSD;
    D3DXVec3Hermite(pOut, pV1, &T1, pV2, &T2, s);
    return S_OK;
}
HRESULT KG3DSFXMeteor::EditorRender()
{
    D3DXVECTOR3 BeginPos(-m_fWidth, 0, 0);
    D3DXVECTOR3 EndPos(m_fWidth, 0, 0);

    D3DXVec3TransformCoord(&BeginPos, &BeginPos, &m_Matrix);
    D3DXVec3TransformCoord(&EndPos, &EndPos, &m_Matrix);

    g_cGraphicsTool.DrawLine(&BeginPos, &EndPos, 0xFFFFFFFF, 0xFFFFFFFF);
    return S_OK;
}

HRESULT KG3DSFXMeteor::RenderSelectMark()
{
    D3DXVECTOR3 BeginPos(-m_fWidth, 0, 0);
    D3DXVECTOR3 EndPos(m_fWidth, 0, 0);

    //if (m_MeteorList.size() && m_MeteorList.back().size())
    //{
    //    BeginPos = m_MeteorList.back().back().PosBegin;
    //    EndPos = m_MeteorList.back().back().PosEnd;
    //}

    D3DXVec3TransformCoord(&BeginPos, &BeginPos, &m_Matrix);
    D3DXVec3TransformCoord(&EndPos, &EndPos, &m_Matrix);

    g_cGraphicsTool.DrawLine(&BeginPos, &EndPos, 0xFF00FF00, 0xFF00FF00);
    return S_OK;
}

HRESULT KG3DSFXMeteor::GetAABBox(AABBOX *pBox)
{
    D3DXVECTOR3 a(-m_fWidth, 5, 5);
    D3DXVECTOR3 b(m_fWidth, -5, -5);
    UpdateTransformation();
    pBox->AddPosition(a, b, &m_Matrix);
    return S_OK;
}

void KG3DSFXMeteor::SetLife(float fLife)
{
    if (fLife > 0.0f)
        m_fLife = fLife;
}

float KG3DSFXMeteor::GetLife()
{
    return m_fLife;
}

void KG3DSFXMeteor::SetEmitMinInterval(float fInterval)
{
    m_fEmitMinInterval = fInterval;
}

float KG3DSFXMeteor::GetEmitMinInterval()
{
    return m_fEmitMinInterval;
}

void KG3DSFXMeteor::SetWidth(float fWidth)
{
    m_fWidth = fWidth;
}

float KG3DSFXMeteor::GetWidth()
{
    return m_fWidth;
}

void KG3DSFXMeteor::SetBindTrackIdx(int nTrackIdx)
{
    m_nTrackIndex = nTrackIdx;
}

int KG3DSFXMeteor::GetBindTrackIdx()
{
    return m_nTrackIndex;
}

void KG3DSFXMeteor::SetTextureIdx(DWORD idx)
{
    m_dwTextureIdx = idx;
}

DWORD KG3DSFXMeteor::GetTextureIdx()
{
    return m_dwTextureIdx;
}

void KG3DSFXMeteor::EnableRandomOffset(BOOL bEnable)
{
    m_bRandomOffset = bEnable;
}

BOOL KG3DSFXMeteor::IsEnableRandomOffset()
{
    return m_bRandomOffset;
}

void KG3DSFXMeteor::SetOffset(D3DXVECTOR3 vOffset)
{
    m_vOffset = vOffset;
}

D3DXVECTOR3 KG3DSFXMeteor::GetOffset()
{
    return m_vOffset;
}

SFX_BLEND_MODE KG3DSFXMeteor::GetBlendMode()
{
    return m_BlendMode;
}

void KG3DSFXMeteor::SetBlendMode(SFX_BLEND_MODE BlendMode)
{
    m_BlendMode = BlendMode;
}

void KG3DSFXMeteor::ClearListNode()
{
	//m_listNode.clear();
}

void KG3DSFXMeteor::SetSampling(BOOL bSampling)
{
	m_bSampling = bSampling;
}

BOOL KG3DSFXMeteor::GetSampling()
{
	return m_bSampling;
}