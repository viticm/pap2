#include "StdAfx.h"
#include "KG3DModel.h"
#include "KG3DBip.h"
#include "KG3DModelST.h"
#include "KG3DAnimationWarper.h"
#include "KG3DRTSInfoCollector.h"
#include "KG3DGraphicsTool.h"
#include "KG3DAnimationTagContainer.h"
#include "KG3DEngineManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern D3DXMATRIX g_stMatIdentity;
KG3DAnimationWarperTable g_cAnimationWarperTable;

KG3DAnimationWarper::KG3DAnimationWarper(void)
{
    m_pModel = NULL;
    //m_pResult = NULL;
    m_bEnabled = FALSE;
	for (DWORD i = 0; i < s_dwSecondaryControllerCnt; i++)
	{
		m_ControllerPriority[i] = ANICTL_PRIMARY;
		m_SecondaryControllers[i].m_Priority = ANICTL_PRIMARY;
		m_SecondaryControllers[i].SetMotionExtraInfo(i + 1);
		m_SecondaryTagContainers[i] = NULL;
	}
}

void KG3DAnimationWarper::UnInit()
{
	for (size_t i = 0; i < m_vecTagContainers.size(); i++)
	{
		SAFE_RELEASE(m_vecTagContainers[i]);
	}
	m_vecTagContainers.clear();

	for (size_t i = 0; i < m_vecAnimations.size(); i++)
	{
		SAFE_DELETE(m_vecAnimations[i].pCollector);
	}
	m_vecAnimations.clear();

	for (size_t i = 0; i < m_Data.size(); i++)
	{
		SAFE_DELETE(m_Data[i].Composer);
	}
	m_Data.clear();

	for (DWORD i = 0; i < s_dwSecondaryControllerCnt; i++)
	{
		if (m_SecondaryTagContainers[i])
		{
			m_SecondaryControllers[i].RemoveAnimationControllerUpdateNotifier(m_SecondaryTagContainers[i]);
		}
		SAFE_RELEASE(m_SecondaryTagContainers[i]);
	}
}

KG3DAnimationWarper::~KG3DAnimationWarper(void)
{
	UnInit();
}

HRESULT KG3DAnimationWarper::GetWeight(int nComposer, int nBoneIndex, FLOAT *pfRetWeight)
{
    HRESULT hrResult = E_FAIL;
    KG_PROCESS_ERROR(nComposer < static_cast<int>(m_Data.size()));
    KG_PROCESS_ERROR(nBoneIndex < static_cast<int>(m_Data[nComposer].BoneWeight.size()));
    KGLOG_PROCESS_ERROR(pfRetWeight);

    *pfRetWeight = m_Data[nComposer].BoneWeight[nBoneIndex] ? 1.0f : 0.0f;
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DAnimationWarper::SetWeight(int nComposer,
                                    int nBoneIndex,
                                    float fWeight,
                                    BOOL bInherit = TRUE)
{
    HRESULT hrResult = E_FAIL;
    KG_PROCESS_ERROR(nComposer < static_cast<int>(m_Data.size()));

    KG_PROCESS_ERROR(nBoneIndex < static_cast<int>(m_Data[nComposer].BoneWeight.size()));
	BOOL bWeigth = (fWeight == 1.0f) ? TRUE : FALSE;
	m_Data[nComposer].BoneWeight[nBoneIndex] = bWeigth;

    if (bInherit)
    {
        SetWeightInherit(nComposer,
            nBoneIndex,
            fWeight);
    }
    hrResult = S_OK;
Exit0:
    return hrResult;
}

void KG3DAnimationWarper::SetWeightInherit(int nComposer,
                                           int nBone,
                                           float fWeight)
{
	BOOL bWeigth = (fWeight == 1.0f) ? TRUE : FALSE;
	m_Data[nComposer].BoneWeight[nBone] = bWeigth;
	
	for (int i = 0; i < m_Skeleton.GetNumChildren(nBone); i++)
    {
        int nChild = m_Skeleton.GetChild(nBone, i);
        m_Data[nComposer].BoneWeight[nChild] = bWeigth;
        SetWeightInherit(nComposer, nChild, fWeight);
    }
}

void KG3DAnimationComposer::CreateTweenCollector(KG3DModel *pModel)
{
	if (!m_pCollector)
	{
		m_pCollector = new KG3DRTSInfoCollector;
	}
	KG_PROCESS_ERROR(m_pCollector);

	m_pCollector->CreateStore(pModel);
    m_pCollector->Clear();
Exit0:
	return;
}

HRESULT KG3DAnimationWarper::AddComposer()
{
    HRESULT hrResult = E_FAIL;
    WaperInfo New;
	INT nNumBones = 0;
    KG_PROCESS_ERROR(m_pModel);
    New.Composer = new KG3DAnimationComposer;
    KGLOG_PROCESS_ERROR(New.Composer);
	
	New.Composer->Init(&m_Skeleton, m_pModel);
    m_pModel->GetNumBones(&nNumBones);
    New.BoneWeight.resize(nNumBones, TRUE);
    m_Data.push_back(New);

    New.Composer->m_pSkeleton = &m_Skeleton;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

KG3DSkeleton::SerializeData::SerializeData()
{
    dwMask = cdwMask;
    dwNumBones = 0;
}

HRESULT KG3DAnimationWarper::GetComposerCount(DWORD *pnRetComposerCount)
{
    HRESULT hrResult = E_FAIL;
    KGLOG_PROCESS_ERROR(pnRetComposerCount);

    *pnRetComposerCount = static_cast<DWORD>(m_Data.size());

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DAnimationWarper::FrameMove(D3DXMATRIX *pResult)
{
    HRESULT hrResult = E_FAIL;
    int nNumBones = 0;
    DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
    KG3DAnimationController *pControllerTop = NULL;
    KG3DAnimationController *pControllerBottom = NULL;

    KG_PROCESS_ERROR(m_pModel);
    KG_ASSERT_EXIT(pResult);

    KG3DAnimationController* pModelController = static_cast<KG3DAnimationController*>(m_pModel->GetAnimationController(ANICTL_PRIMARY));
    IKG3DAnimationEventHandler *pHandler = pModelController->GetEventHandler();

    ASSERT(m_Data.size() == 2);
    D3DXMATRIX *pMat = m_Data[1].Composer->GetResultByIndex(0);

    pControllerTop = m_Data[0].Composer->GetAnimationController(JOINT_TOPINFO_ANI_TOP_INDEX);
    pControllerBottom = m_Data[1].Composer->GetAnimationController(JOINT_BOTTOMINFO_ANI_BOTTOM_INDEX);
    pControllerTop->SetSpeed(pControllerBottom->GetSpeed());

    for (int i = 1; i >= 0; i--)
    {
        KG3DAnimationComposer *pComposer = m_Data[i].Composer;
        ASSERT(pComposer);
        KG3DAnimationComposer::TweenTimeInfo *pTweenInfo = &pComposer->m_TweenTimeInfo[1 - i];
        ASSERT(pTweenInfo);
        KG3DAnimationController *pCurrentController = pComposer->GetAnimationController(1 - i);
        ASSERT(pCurrentController);

        if (dwNowTime >= pTweenInfo->dwStartTime + pTweenInfo->dwSpan)
        {
            if (pComposer->m_bInTween)
            {
                pCurrentController->PauseAnimation(FALSE);
                pComposer->m_bInTween = FALSE;
            }
            pCurrentController->SetModel(m_pModel);
            pCurrentController->RegistereEventHandler(pHandler);
            pComposer->FrameMove(pCurrentController, i, 1 - i, pMat, FALSE);
            pCurrentController->m_Priority = m_ControllerPriority[i];
            pCurrentController->UnregisterEventHandler(pHandler);
            pCurrentController->SetModel(NULL);
        }
        else
        {
            //在动作混合区间
            pComposer->FrameMove(pCurrentController, i, 1 - i, pMat, TRUE);
        }
    }

	nNumBones = m_Skeleton.GetNumBones();
	for (int j = 0; j < nNumBones; j++)
	{
		if (m_Data[0].BoneWeight[j])
			pResult[j] = m_Data[0].Composer->GetResultWorld(j);
		else
			pResult[j] = m_Data[1].Composer->GetResultWorld(j);
	}

    //更新2号动作控制器
    for (DWORD i = 0; i < s_dwSecondaryControllerCnt; i++)
    {
        if (m_SecondaryControllers[i].HasAnimation())
        {
            m_SecondaryControllers[i].RegistereEventHandler(pHandler);
            m_SecondaryControllers[i].UpdateAnimation(NULL, FALSE, TRUE);
            m_SecondaryControllers[i].UnregisterEventHandler(pHandler);
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DAnimationWarper::AddAnimation(IEKG3DClip *pClip, 
                                         int nComposerIndex)
{
    HRESULT hrResult = E_FAIL; 
    KG3DRTSInfoCollector *pCollector = NULL;
    KG_PROCESS_ERROR(m_pModel);
    KG_PROCESS_ERROR(pClip);
    KG_PROCESS_ERROR(nComposerIndex < static_cast<int>(m_Data.size()));

    DWORD i = 0;
    for (i = 0; i < m_vecAnimations.size(); i++)
    {
        if (m_vecAnimations[i].pClip == pClip)
        {
            pCollector = m_vecAnimations[i].pCollector;
            break;
        }
    }
	
    //not found
    if (i == m_vecAnimations.size())
    {
        AnimationInfo New;
        New.pClip = pClip;
        New.pCollector = new KG3DRTSInfoCollector;
        ASSERT(New.pCollector);
        New.pCollector->CreateStore(m_pModel);
        New.pCollector->Clear();
        m_vecAnimations.push_back(New);
        pCollector = New.pCollector;
    }
    ASSERT(pCollector);
    m_Data[nComposerIndex].Composer->AddAnimation(pCollector, i);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

KG3DAnimationComposer::BoneInfoItem::BoneInfoItem()
{
    pCollector = NULL;
    fWeight = 1.0f;
    dwType = NORMAL_RTS;
}

void KG3DAnimationComposer::SetSkeleton(KG3DSkeleton* pSkeleton)
{
    HRESULT hrResult = E_FAIL;
    DWORD dwBoneNum = 0;

    m_SkeletonInfo.clear();
    m_pSkeleton = pSkeleton;
    KG_PROCESS_SUCCESS(!m_pSkeleton);

    dwBoneNum = (DWORD)m_pSkeleton->GetNumBones();

    m_SkeletonInfo.resize(dwBoneNum);
	
    KG_DELETE_ARRAY(m_pWorldResult);
    m_pWorldResult = new D3DXMATRIX[dwBoneNum];
    KG_PROCESS_ERROR(m_pWorldResult);

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(m_pWorldResult);
    }
}

void* KG3DAnimationComposer::GetData(int nIndex)
{
    ASSERT(nIndex < static_cast<int>(m_SkeletonInfo.size()));
    return &(m_SkeletonInfo[nIndex].rts);
}


HRESULT KG3DAnimationComposer::SetBoneInfo(unsigned int uAnmation,
                                        unsigned int uBone,
                                        DWORD dwType,
                                        float fWeight)
{
    HRESULT hrResult = E_FAIL;
    KG_PROCESS_ERROR(uBone < m_SkeletonInfo.size());
    KG_PROCESS_ERROR(uAnmation < m_SkeletonInfo[uBone].Info.size());

    m_SkeletonInfo[uBone].Info[uAnmation].dwType = dwType;
    m_SkeletonInfo[uBone].Info[uAnmation].fWeight = fWeight;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DAnimationComposer::GetBoneInfo(unsigned int uAnmation,
                                        unsigned int uBone,
                                        DWORD& dwType,
                                        float& fWeight)
{
    HRESULT hrResult = E_FAIL;
    KG_PROCESS_ERROR(uBone < m_SkeletonInfo.size());
    KG_PROCESS_ERROR(uAnmation < m_SkeletonInfo[uBone].Info.size());

    dwType = m_SkeletonInfo[uBone].Info[uAnmation].dwType;
    fWeight = m_SkeletonInfo[uBone].Info[uAnmation].fWeight;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

void KG3DAnimationComposer::AddAnimation(KG3DAnimationUpdateExtraInfoCollector *pCollector, 
                                         DWORD dwIndex)
{
    size_t i = 0;
	KG_PROCESS_ERROR(m_SkeletonInfo.size());
	KG_PROCESS_ERROR(m_pModel);
    
	for (i = 0; i < m_SkeletonInfo[0].Info.size(); i++)
    {
        if (pCollector == m_SkeletonInfo[0].Info[i].pCollector)
            break;
    }
    
	//如果找到了存在的Colletor就不用创建新的了
	KG_PROCESS_SUCCESS(i != m_SkeletonInfo[0].Info.size());
    
	for (i = 0; i < m_SkeletonInfo.size(); i++)
    {
        BoneInfoItem Item;
        Item.pCollector = pCollector;
        Item.dwIndex = dwIndex;
        m_SkeletonInfo[i].Info.push_back(Item);
    }
    m_Controller.resize(m_Controller.size() + 1);
	m_TweenTimeInfo.resize(m_TweenTimeInfo.size() + 1);
	m_TweenTimeInfo[m_TweenTimeInfo.size() - 1].Init(m_pSkeleton->GetNumBones());
Exit1:
Exit0:
    return;
}

void KG3DAnimationComposer::RemoveAnimation(DWORD dwIndex)
{
	size_t i = 0;
	DWORD dwController2Erase = 0xfffffff;
	std::vector<KG3DAnimationController>::iterator itControll2Erase = m_Controller.begin();
	std::vector<TweenTimeInfo>::iterator itTweenTimeInfo2Erase = m_TweenTimeInfo.begin();

	KG_PROCESS_ERROR(dwIndex < m_SkeletonInfo[0].Info.size());
	for (i = 0; i < m_SkeletonInfo.size(); i++)
	{
		std::vector<BoneInfoItem>::iterator j = m_SkeletonInfo[i].Info.begin();
		advance(j, dwIndex);
		dwController2Erase = j->dwIndex;
		m_SkeletonInfo[i].Info.erase(j);
	}

	for (i = 0; i < m_SkeletonInfo.size(); i++)
	{
		for (size_t j = 0; j < m_SkeletonInfo[i].Info.size(); j++)
		{
			//如果还有使用相同Index的Controller和TweenTimeInfo就不删除
			KG_PROCESS_SUCCESS(m_SkeletonInfo[i].Info[j].dwIndex == dwController2Erase);
		}
	}
	
	//把将被删除的Index之后的索引都减一,得到删除之后正确的索引
	for (i = 0; i < m_SkeletonInfo.size(); i++)
	{
		for (size_t j = 0; j < m_SkeletonInfo[i].Info.size(); j++)
		{
			if (m_SkeletonInfo[i].Info[j].dwIndex > dwController2Erase)
			{
				m_SkeletonInfo[i].Info[j].dwIndex--;
			}
		}
	}

	advance(itControll2Erase, dwController2Erase);
	advance(itTweenTimeInfo2Erase, dwController2Erase);

	itTweenTimeInfo2Erase->UnInit();
	m_Controller.erase(itControll2Erase);
	m_TweenTimeInfo.erase(itTweenTimeInfo2Erase);
Exit1:
Exit0:
	return;
}

HRESULT KG3DAnimationComposer::GetAnimationCount(DWORD *pdwRetAnimationCount)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetAnimationCount);

    *pdwRetAnimationCount = (DWORD)m_Controller.size();

    hrResult = S_OK;
Exit0:
    return hrResult;
}

void KG3DAnimationComposer::SaveTweenTimeInfo(
    DWORD dwAnimationIndex,
    KG3DAnimationController *pStart,
    DWORD dwTweenTime
)
{
    KG3DAnimationController StartController;
    KG3DAnimationController EndController;

	KG_PROCESS_ERROR(m_pCollector);
	KG_PROCESS_ERROR(dwAnimationIndex < m_TweenTimeInfo.size());

    TweenTimeInfo& Info = m_TweenTimeInfo[dwAnimationIndex];

	KG_PROCESS_SUCCESS(!dwTweenTime);

    DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
    if (dwNowTime < Info.dwStartTime + Info.dwSpan)
	{
        ASSERT(Info.dwSpan > 0);
        float fWeight = static_cast<float>(dwNowTime - Info.dwStartTime) / Info.dwSpan;
        for (DWORD i = 0; i < Info.dwNumBones; i++)
        {
            KG3DClip::InterpolateRTSKeyFrame(
                Info.pTweenStartInfo[i],
                Info.pTweenStartInfo[i],
                Info.pTweenEndInfo[i],
                fWeight
            );
        }
	}
	else
    {
        ASSERT(pStart);
        pStart->Clone(StartController);
        StartController.UpdateAnimation(m_pCollector, TRUE, FALSE);
        memcpy(Info.pTweenStartInfo, m_pCollector->GetData(), Info.dwNumBones * sizeof(RTS));
	}

    m_Controller[dwAnimationIndex].Clone(EndController);
    EndController.UpdateAnimation(m_pCollector, TRUE, FALSE);
    memcpy(Info.pTweenEndInfo, m_pCollector->GetData(), Info.dwNumBones * sizeof(RTS));

    m_Controller[dwAnimationIndex].PauseAnimation(TRUE);
    m_bInTween = TRUE;
Exit1:
	Info.dwStartTime = g_cGraphicsTool.GetNowTime();
	Info.dwSpan = dwTweenTime;
Exit0:
	;
}

void KG3DAnimationComposer::UpdateCollectorByTweenInfo(DWORD dwCollectorIndex)
{
    RTS rts;
    TweenTimeInfo& TweenInfo = m_TweenTimeInfo[dwCollectorIndex];
    DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
    ASSERT(dwNowTime >= TweenInfo.dwStartTime);
    ASSERT(TweenInfo.dwSpan > 0);
    float fWeight = static_cast<float>(dwNowTime - TweenInfo.dwStartTime) / TweenInfo.dwSpan;
    for (DWORD i = 0; i < TweenInfo.dwNumBones; i++)
    {
        KG3DClip::InterpolateRTSKeyFrame(
            rts,
            TweenInfo.pTweenStartInfo[i],
            TweenInfo.pTweenEndInfo[i],
            fWeight
        );

        KG3DClip::RTS2Matrix(m_pWorldResult[i], rts);
    }
//Exit0:
    return;
}

void KG3DAnimationComposer::FrameMove(
    KG3DAnimationController *pController, int nComposer, int nAnimationIndex, const D3DXMATRIX *pcDownRootMat, BOOL bInTween
)
{
    BOOL bVisible = TRUE;

    ASSERT(pController);

    if (bInTween)
    {
        UpdateCollectorByTweenInfo(nAnimationIndex);
    }
    else
    {
        pController->SetUpdateMatrix(m_pWorldResult);
        pController->UpdateAnimation(NULL, TRUE, TRUE);
    }

    if (nComposer == 0)
    {
        m_pWorldResult[0] = *pcDownRootMat;
    }

    if (g_bClient)
    {
        ASSERT(m_pModel);
        bVisible = m_pModel->IsVisible();
    }

	if (bVisible)
	{
		if (m_bBoneTransformEnable)
		{
			std::map<int, float>::iterator it = m_BoneTransforms.begin();
			std::map<int, float>::iterator itEnd = m_BoneTransforms.end();
			while (it != itEnd)
			{
				int nBone = (*it).first;
				float fAngle = (*it).second;
				D3DXMATRIX matRotationX;
				D3DXMatrixRotationX(&matRotationX, fAngle);
            	D3DXMatrixMultiply(&m_pWorldResult[nBone], &matRotationX, &m_pWorldResult[nBone]);
				it++;
			}
            m_bBoneTransformEnable = false;
		}

		for (int i = 0; i < m_pSkeleton->GetNumBaseBone(); i++)
		{
			int nIndex = m_pSkeleton->GetBaseBone(i);
			UpdateRelativeBone(nIndex, g_stMatIdentity);
		}

	}

    return;
}

void KG3DAnimationComposer::FrameMove()
{
	RTS* pRTS[MAX_COMPOSE];
	DWORD pFlag[MAX_COMPOSE];
	float pWeight[MAX_COMPOSE];
	for (DWORD i = 0; i < static_cast<DWORD>(m_SkeletonInfo.size()); i++)
	{
		BoneInfo& Bone = m_SkeletonInfo[i];
		for (DWORD j = 0; j < static_cast<DWORD>(Bone.Info.size()); j++)
		{
			pRTS[j] = static_cast<RTS*>(Bone.Info[j].pCollector->GetData(i));
			pFlag[j] = Bone.Info[j].dwType;
			pWeight[j] = Bone.Info[j].fWeight;
		}

		KG3DClip::ComposeRTS(Bone.rts,
			pRTS,
			pFlag,
			static_cast<DWORD>(Bone.Info.size()));

		KG3DClip::RTS2Matrix(m_pWorldResult[i], Bone.rts);
	}

	if (m_bBoneTransformEnable)
	{
		std::map<int, float>::iterator it = m_BoneTransforms.begin();
		while (it != m_BoneTransforms.end())
		{
			int nBone = (*it).first;
			float fAngle = (*it).second;
			static D3DXVECTOR3 vec3Up(1.0f, 0.0f, 0.0f);
			D3DXMATRIX matRotationY;
			D3DXMatrixRotationAxis(&matRotationY, &vec3Up, fAngle);
			D3DXMatrixMultiply(&m_pWorldResult[nBone], &matRotationY, &m_pWorldResult[nBone]);
			it++;
		}
	}

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);

	for (int i = 0; i < m_pSkeleton->GetNumBaseBone(); i++)
	{
		int nIndex = m_pSkeleton->GetBaseBone(i);
		UpdateRelativeBone(nIndex, matIdentity);
	}
}

void KG3DAnimationComposer::UpdateRelativeBone(int nIndex, const D3DXMATRIX& BaseMatrice)
{
    int nNumChildren = m_pSkeleton->GetNumChildren(nIndex);
    ASSERT(nNumChildren >= 0);

    D3DXMatrixMultiply(&m_pWorldResult[nIndex], &m_pWorldResult[nIndex], &BaseMatrice);
    for (int i = 0; i < nNumChildren; i++)
    {
        UpdateRelativeBone(m_pSkeleton->GetChild(nIndex, i), m_pWorldResult[nIndex]);
    }
}

//////////////////////////////////////////////////////////////////////////
int KG3DSkeleton::GetParnet(int nIndex)
{
    ASSERT(nIndex < static_cast<int>(m_Skeleton.size()));
    return m_Skeleton[nIndex].nParent;
}

void KG3DSkeleton::Create(KG3DModel *pModel)
{
    INT nNumBones = 0;
	KG_PROCESS_ERROR(pModel);
	pModel->GetNumBones(&nNumBones);
    KG_PROCESS_ERROR(nNumBones);
    KG_PROCESS_ERROR(pModel->GetType() == MESHTYPE_MODELST);
    
	CreateModelST(pModel);
Exit0:
    return;
}

LPCSTR KG3DSkeleton::GetBoneName(int nIndex)
{
    LPCSTR strReturn = NULL;
    KG_PROCESS_ERROR(nIndex < static_cast<int>(m_Skeleton.size()));

    strReturn = m_Skeleton[nIndex].strBoneName;
Exit0:
    return strReturn;
}

void KG3DSkeleton::CreateModelST(KG3DModel *pModel)
{
    HRESULT hr = E_FAIL;
    const KG3DBip* pBip = NULL;
    int nNumBones = 0;
    int nBoneIndex = 0;
    int nNumChildBones = 0;
    int nChildBoneIndex = 0;
    int nChildBoneID = 0;
    KG3DModelST *pModelST = static_cast<KG3DModelST*>(pModel);
    KG_PROCESS_ERROR(pModelST);

    pBip = pModelST->GetBip();
    KG_PROCESS_ERROR(pBip);


    nNumBones = pBip->GetNumBones();
    m_Skeleton.clear();
    m_Skeleton.resize(nNumBones);

    for (nBoneIndex = 0; nBoneIndex < nNumBones; nBoneIndex++)
    {
        Bone& Info = m_Skeleton[nBoneIndex];
        Info.nParent = pBip->GetParentIndex(nBoneIndex);
        Info.nBone = nBoneIndex;
        strcpy_s(Info.strBoneName, 
            MAX_BONE_NAME_LENGTH, 
            pBip->GetBoneName(nBoneIndex));

        nNumChildBones = pBip->GetNumChildren(nBoneIndex);
        for (nChildBoneIndex = 0; 
            nChildBoneIndex < nNumChildBones;
            nChildBoneIndex++)
        {
            nChildBoneID = pBip->GetChildIndex(nBoneIndex, nChildBoneIndex);
            Info.Children.push_back(nChildBoneID);
        }
    }

    m_BaseBones.clear();

    int nNumBaseBone = pBip->GetNumBaseBone();
    if (nNumBaseBone)
    {
        m_BaseBones.resize(nNumBaseBone);
        for (int i = 0; i < nNumBaseBone; i++)
        {
            m_BaseBones[i] = pBip->GetBaseBone(i);
        }
    }
    hr = S_OK;
Exit0:
    return ;	
}

int KG3DSkeleton::GetNumBaseBone()
{
    return static_cast<int>(m_BaseBones.size());
}

int KG3DSkeleton::GetBaseBone(int nIndex)
{
    return m_BaseBones[nIndex];
}

BOOL KG3DAnimationWarper::GetEnable()
{
    return m_bEnabled;
}

HRESULT KG3DAnimationWarper::Enable(BOOL bEnable)
{
    DWORD dwNumAni = 0;
    KG_PROCESS_SUCCESS(!bEnable);
    KG_PROCESS_SUCCESS(m_bEnabled == bEnable);
    for (size_t i = 0; i < m_Data.size(); i++)
    {
        m_Data[i].Composer->GetAnimationCount(&dwNumAni);
        for (DWORD j = 0; j < dwNumAni; j++)
        {
            KG3DAnimationController *pCurController = m_Data[i].Composer->GetAnimationController(j);
            pCurController->StartAnimation(pCurController->GetPlayType(), 0, pCurController->GetSpeed());
        }
    }
Exit1:
    m_bEnabled = bEnable;
    return S_OK;
}

void KG3DAnimationWarper::SetModel(KG3DModel *pModel)
{
    m_pModel = pModel;
    KG_PROCESS_SUCCESS(!pModel);
    m_Skeleton.Create(pModel);
    INT nNumBone = 0;
	pModel->GetNumBones(&nNumBone);
    KG_PROCESS_ERROR(nNumBone);
    //m_pResult = new D3DXMATRIX[nNumBone];
    //KGLOG_PROCESS_ERROR(m_pResult);
Exit1:
Exit0:
    ;
}

HRESULT KG3DAnimationWarper::GetComposer(int nIndex, IEKG3DAnimationComposer **ppiRetComposer)
{
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_ERROR(nIndex < static_cast<int>(m_Data.size()));
    KGLOG_PROCESS_ERROR(ppiRetComposer);

    *ppiRetComposer = m_Data[nIndex].Composer;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

const D3DXMATRIX& KG3DAnimationComposer::GetResultWorld(int nIndex)
{
    return m_pWorldResult[nIndex];
}

D3DXMATRIX* KG3DAnimationComposer::GetResultByIndex(int nIndex)
{
    return &m_pWorldResult[nIndex];
}

KG3DAnimationComposer::KG3DAnimationComposer()
{
    m_pWorldResult = NULL;
	m_bBoneTransformEnable = FALSE;
	m_pCollector = NULL;
	m_pModel = NULL;
	m_dwPauseTime = 0;
    m_bInTween = FALSE;
}

HRESULT KG3DAnimationComposer::Init(KG3DSkeleton *pSkeleton, KG3DModel *pModel)
{
	HRESULT hRetCode = E_FAIL;
	
	KG_PROCESS_ERROR(pSkeleton);
	KG_PROCESS_ERROR(pModel);

	SetSkeleton(pSkeleton);
	CreateTweenCollector(pModel);
	m_dwPauseTime = 0;
	m_pModel = pModel;
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

KG3DAnimationComposer::~KG3DAnimationComposer()
{
    for(size_t i = 0; i < m_TweenTimeInfo.size(); i++)
	{
		m_TweenTimeInfo[i].UnInit();
	}
	SAFE_DELETE(m_pCollector);
	KG_DELETE_ARRAY(m_pWorldResult);
}

void KG3DAnimationWarper::CheckSychornize(KG3DClip *pNewClip, 
										  DWORD& dwTweenTime)
{
	KG3DClip *pBottomClip = NULL;
	KG3DAnimationComposer *pTopComposer = NULL;
	KG3DAnimationComposer *pBottomComposer = NULL;
	KG3DAnimationController *pTopController = NULL;
	KG3DAnimationController *pBottomController = NULL;
	KG_PROCESS_ERROR(pNewClip);

	GetComposer(JOINT_BOTTOMINFO_COMPOSERINDEX, (IEKG3DAnimationComposer **)&pBottomComposer);
	_ASSERTE(pBottomComposer);

	KG_PROCESS_SUCCESS(!pBottomComposer->m_TweenTimeInfo[0].dwStartTime);

	pBottomController = pBottomComposer->GetAnimationController(JOINT_BOTTOMINFO_ANI_BOTTOM_INDEX);
	_ASSERTE(pBottomController);

	GetComposer(JOINT_TOPINFO_COMPOSERINDEX, (IEKG3DAnimationComposer **)&pTopComposer);
	_ASSERTE(pTopComposer);

	pTopController = pTopComposer->GetAnimationController(JOINT_TOPINFO_ANI_TOP_INDEX);
	_ASSERTE(pTopController);

	pBottomClip = pBottomController->GetCurAnimation();
	if (pBottomClip)
	{
		if (pNewClip->GetID() == pBottomClip->GetID())
		{
			//如果上下半身使用的是同一个动作， 那么做同步处理
            pTopController->SetSpeed(pBottomController->GetSpeed());
            pTopController->SeekAnimation(ANIMATION_SEEK_SET, pBottomController->GetPlayTime());
            pBottomController->SeekAnimation(ANIMATION_SEEK_SET, pBottomController->GetPlayTime());
		}
	}
Exit1:
Exit0:
	;
}

HRESULT KG3DAnimationWarper::ChangeAnimation(unsigned int uComposer,
                                          unsigned int uAnimation,
										  IEKG3DClip* pClip,
										  IKG3DAnimationController* pStart,
                                          DWORD dwTweenTime)
{
    //一定要在设置了Controller之后才调用change, 不然SaveTweenTimeInfo会出错
	HRESULT hrResult = E_FAIL;
    KG_PROCESS_ERROR(uComposer < m_Data.size());

	KG3DRTSInfoCollector *pCollector = static_cast<KG3DRTSInfoCollector*>(m_Data[uComposer].Composer->GetCollector(uAnimation));
	KG3DAnimationController* pControlller = m_Data[uComposer].Composer->GetAnimationController(uAnimation);
	pControlller->SetAnimation(pClip);
	dwTweenTime = (pStart && pStart->HasAnimation()) ? dwTweenTime : 0;
	
	if (uComposer == JOINT_TOPINFO_COMPOSERINDEX && uAnimation == JOINT_TOPINFO_ANI_TOP_INDEX)
	{
		CheckSychornize(static_cast<KG3DClip*>(pClip), dwTweenTime);
	}

    m_Data[uComposer].Composer->SaveTweenTimeInfo(uAnimation, static_cast<KG3DAnimationController*>(pStart), dwTweenTime);
	
	for (size_t i = 0; i < m_vecAnimations.size(); i++)
	{
		if (m_vecAnimations[i].pCollector == pCollector)
		{
			m_vecAnimations[i].pClip = pClip;
			break;
		}
	}
    hrResult = S_OK;
Exit0:
    return hrResult;
}

KG3DAnimationUpdateExtraInfoCollector* KG3DAnimationComposer::GetCollector(unsigned int uIndex)
{
    KG3DAnimationUpdateExtraInfoCollector *pReturn = NULL;
    KG_PROCESS_ERROR(m_SkeletonInfo.size());
    KG_PROCESS_ERROR(uIndex < m_SkeletonInfo[0].Info.size());

    pReturn = m_SkeletonInfo[0].Info[uIndex].pCollector;
Exit0:
    return pReturn;
}

HRESULT KG3DAnimationWarper::Helper_SaveBipExtraInfo(FILE *fpFile)
{
    HRESULT hr = E_FAIL;
    KG3DSkeleton::SerializeData Data;
    size_t uSize = 0;
    KG_PROCESS_ERROR(fpFile);
    Data.dwNumBones = static_cast<int>(m_Skeleton.GetNumBones());
    KG_PROCESS_ERROR(Data.dwNumBones);
    uSize = fwrite(&Data, sizeof(KG3DSkeleton::SerializeData), 1, fpFile);
    KG_PROCESS_ERROR(uSize == 1);
    for (int i = 0; i < static_cast<int>(Data.dwNumBones); i++)
    {
        uSize = fwrite(m_Skeleton.GetBoneName(i), sizeof(TCHAR) * MAX_BONE_NAME_LENGTH, 1, fpFile);
        KG_PROCESS_ERROR(uSize == 1);
    }
    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DAnimationWarper::Save(const TCHAR *strFileName)
{
    size_t uSize = 0;
    HRESULT hr = E_FAIL;
    SerializeData Data;
    FILE* fpFile = NULL;
    float *pWeights = NULL;
    KG3DAnimationUpdateExtraInfoCollector** ppCollectors = NULL;
    KG_PROCESS_ERROR(m_vecAnimations.size());
    KG_PROCESS_ERROR(m_Data.size());
    KG_PROCESS_ERROR(strFileName);
    fpFile = fopen(strFileName, "wb");
    KGLOG_PROCESS_ERROR(fpFile);

    //Save animation collector information
    DWORD dwNumCollector = 0;
    ppCollectors = new KG3DAnimationUpdateExtraInfoCollector*[m_vecAnimations.size()];
    KGLOG_PROCESS_ERROR(ppCollectors);

    for (size_t i = 0; i < m_vecAnimations.size(); i++)
    {
        ppCollectors[dwNumCollector] = m_vecAnimations[i].pCollector;
        dwNumCollector++;
    }
    Data.dwNumAnimation = static_cast<DWORD>(m_vecAnimations.size());
    Data.dwNumBone = static_cast<DWORD>(m_Data[0].BoneWeight.size());
    Data.dwNumAnimationComposer = static_cast<DWORD>(m_Data.size());

    uSize = fwrite(&Data, sizeof(SerializeData), 1, fpFile);
    KGLOG_PROCESS_ERROR(uSize == 1);
    //Save bone information
    hr = Helper_SaveBipExtraInfo(fpFile);
    KGLOG_COM_PROCESS_ERROR(hr);
    //Save composer information
    for (DWORD i = 0; i < Data.dwNumAnimationComposer; i++)
    {
        KG3DAnimationComposer::SerializeData* pData = NULL;
        BYTE* pbData = NULL;
        hr = m_Data[i].Composer->SaveBlock(&pData, ppCollectors, dwNumCollector);
        if (FAILED(hr))
        {
            KGLogPrintf(KGLOG_ERR, "Warper Info file %s : Save composer %d failed.", strFileName, i);
            continue;
        }
        size_t uSaveSize = sizeof(KG3DAnimationComposer::SerializeData) + pData->dwBlockLength;
        uSize = fwrite(pData, uSaveSize, 1, fpFile);
        pbData = reinterpret_cast<BYTE*>(pData);
        SAFE_DELETE_ARRAY(pbData);
        KGLOG_PROCESS_ERROR(uSize == 1);
    }
    //Save composer bone weight information
    size_t uIndex = 0;
    size_t uNumWeight = Data.dwNumAnimationComposer * Data.dwNumBone;
    pWeights = new float[uNumWeight];
    KGLOG_PROCESS_ERROR(pWeights);
    for (size_t i = 0; i < m_Data.size(); i++)
    {
        for (size_t j = 0; j < m_Data[i].BoneWeight.size(); j++)
        {
			pWeights[uIndex] = (m_Data[i].BoneWeight[j]) ? 1.0f : 0.0f;
            uIndex++;
        }
    }
    uSize = fwrite(pWeights, sizeof(float) * uNumWeight, 1, fpFile);
    KG_PROCESS_ERROR(uSize == 1);
    hr = S_OK;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    SAFE_DELETE_ARRAY(pWeights);
    SAFE_DELETE_ARRAY(ppCollectors);
    return hr;
}


void KG3DAnimationWarper::Clear()
{
    //Clear will only clear animation information, not the skeleton.
    for (size_t i = 0; i < m_vecAnimations.size(); i++)
    {
        //AnimationIt->pAnimation did not invoke AddRef(), so no need to invoke Release() here
        SAFE_DELETE(m_vecAnimations[i].pCollector);
    }
    m_vecAnimations.clear();	
    for (size_t i = 0; i < m_Data.size(); i++)
    {
        SAFE_DELETE(m_Data[i].Composer);
    }
    m_Data.clear();
}

HRESULT KG3DAnimationWarper::Helper_LoadBipExtraInfo(IFile *pFile)
{
    HRESULT hr = E_FAIL;
    unsigned long uSize = 0;
    KG3DSkeleton::SerializeData Data;
    m_BoneMatchTable.clear();

    KG_PROCESS_ERROR(pFile);

    uSize = pFile->Read(&Data, sizeof(KG3DSkeleton::SerializeData));
    KGLOG_PROCESS_ERROR(uSize == sizeof(KG3DSkeleton::SerializeData));

    KG_PROCESS_ERROR(Data.dwMask == KG3DSkeleton::SerializeData::cdwMask);
    KG_PROCESS_ERROR(Data.dwNumBones == static_cast<DWORD>(m_Skeleton.GetNumBones()));

    m_BoneMatchTable.resize(Data.dwNumBones);
    for (size_t i = 0; i < m_BoneMatchTable.size(); i++)
    {
        TCHAR strBoneName[MAX_BONE_NAME_LENGTH];

        uSize = pFile->Read(strBoneName, sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
        KGLOG_PROCESS_ERROR(uSize == sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
		_strlwr_s(strBoneName, MAX_BONE_NAME_LENGTH);
        int nIndex = m_Skeleton.FindBone(strBoneName);
        m_BoneMatchTable[i] = nIndex;
    }
    hr = S_OK;
Exit0:
    return hr;
}

int KG3DSkeleton::FindBone(LPCSTR strBoneName)
{
    int nReturn = -1;
    KG_PROCESS_ERROR(strBoneName);
    for (size_t i = 0; i < m_Skeleton.size(); i++)
    {
        if (!strcmp(m_Skeleton[i].strBoneName, strBoneName))
        {
            nReturn = static_cast<int>(i);
            break;
        }
    }
Exit0:
    return nReturn;
}

HRESULT KG3DAnimationWarper::Load(const TCHAR *strFileName)
{
    HRESULT hr = E_FAIL;
    IFile *pFile = NULL;
    unsigned long uSize = 0;
    SerializeData Data;
    float *pWeights = NULL;
    KG3DAnimationUpdateExtraInfoCollector** ppCollectors = NULL;
	DWORD dwStartTime = timeGetTime();

    KGLOG_PROCESS_ERROR(strFileName);
    KG_PROCESS_ERROR(m_pModel);
    Clear();

    pFile = g_OpenFile(strFileName);
    if (!pFile)
    {
        KGLogPrintf(KGLOG_ERR, "KG3DAnimationWarper open file %s failed.", strFileName);
        goto Exit0;
    }

    uSize = pFile->Read(&Data, sizeof(SerializeData));
    KGLOG_PROCESS_ERROR(uSize == sizeof(SerializeData));

    KG_PROCESS_ERROR(Data.cdwMask == SerializeData::cdwMask);

    switch(Data.dwVersion)
    {
    case SerializeData::cdwVersion:
        {
            //read bone name information here
            hr = Helper_LoadBipExtraInfo(pFile);
            KGLOG_COM_PROCESS_ERROR(hr);

            m_Data.resize(Data.dwNumAnimationComposer);

            ppCollectors = new KG3DAnimationUpdateExtraInfoCollector*[Data.dwNumAnimation];
            KGLOG_PROCESS_ERROR(ppCollectors);

            for (DWORD i = 0; i < Data.dwNumAnimation; i++)
            {
                AnimationInfo New;
                New.pClip = NULL;

                New.pCollector = new KG3DRTSInfoCollector;
                KGLOG_PROCESS_ERROR(New.pCollector);

                ppCollectors[i] = New.pCollector;
                New.pCollector->CreateStore(m_pModel);
                New.pCollector->Clear();
				m_vecAnimations.push_back(New);
                m_vecTagContainers.push_back(NULL);
            }

            for (DWORD i = 0; i < Data.dwNumAnimationComposer; i++)
            {
                m_Data[i].Composer = new KG3DAnimationComposer;
                KGLOG_PROCESS_ERROR(m_Data[i].Composer);

				m_Data[i].Composer->Init(&m_Skeleton, m_pModel);
                m_Data[i].Composer->ReadBlock(pFile, ppCollectors, Data.dwNumAnimation);
            }
            break;
        }
    default:
        assert(0);
    }

    //Load composer weight information here
    size_t uIndex = 0;
    size_t uNumWeight = Data.dwNumBone * Data.dwNumAnimationComposer;
    pWeights = new float[uNumWeight];
    KGLOG_PROCESS_ERROR(pWeights);

    uSize = pFile->Read(pWeights, sizeof(float) * (unsigned long)uNumWeight);
    KGLOG_PROCESS_ERROR(uSize == sizeof(float) * uNumWeight);

    for (size_t i = 0; i < m_Data.size(); i++)
    {
        m_Data[i].BoneWeight.resize(Data.dwNumBone);
        for (size_t j = 0; j < m_Data[i].BoneWeight.size(); j++)
        {
            assert(m_BoneMatchTable[j] != -1 && m_BoneMatchTable[j] < static_cast<int>(m_Data[i].BoneWeight.size()));
			BOOL bWeight = (pWeights[uIndex] == 1.0f) ? TRUE : FALSE;
			m_Data[i].BoneWeight[m_BoneMatchTable[j]] = bWeight;
            uIndex++;
        }
    }
    hr = S_OK;
Exit0:
    SAFE_DELETE_ARRAY(pWeights);
    SAFE_DELETE_ARRAY(ppCollectors);

    KG_COM_RELEASE(pFile);

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DAnimationWarper::Load cost %d %s",dwCost,strFileName);
	}
    return hr;
}


HRESULT KG3DAnimationComposer::SaveBlock(SerializeData** ppData,
                                         KG3DAnimationUpdateExtraInfoCollector **ppCollectors,
                                         DWORD dwNumCollector)
{
    HRESULT hr = E_FAIL;
    SerializeData* pData = NULL;
    BoneInfoSerializeData* pBoneInfo = NULL;
    std::vector<DWORD> vecCollectorIndices;
    size_t uBufferSize = 0;
    KG_PROCESS_ERROR(ppData);
    KG_PROCESS_ERROR(ppCollectors);
    KG_PROCESS_ERROR(dwNumCollector);

    KG_PROCESS_ERROR(m_SkeletonInfo.size());

    DWORD dwNumItemPerBone = static_cast<DWORD>(m_SkeletonInfo[0].Info.size());
    DWORD dwNumBones = static_cast<DWORD>(m_SkeletonInfo.size());

    uBufferSize = sizeof(SerializeData) + sizeof(BoneInfoSerializeData) * dwNumBones * dwNumItemPerBone;
    pData = reinterpret_cast<SerializeData*>(new BYTE[uBufferSize]);
    KGLOG_PROCESS_ERROR(pData);

    pData->Init();
    pData->dwNumBones = dwNumBones;
    pData->dwNumItemPerBone = dwNumItemPerBone;
    pBoneInfo = reinterpret_cast<BoneInfoSerializeData*>(reinterpret_cast<BYTE*>(pData) + sizeof(SerializeData));

    vecCollectorIndices.resize(dwNumItemPerBone);
    //prepare indices
    for (size_t i = 0; i < m_SkeletonInfo[0].Info.size(); i++)
    {
        vecCollectorIndices[i] = 0xffffffff;
        for (DWORD j = 0; j < dwNumCollector; j++)
        {
            if (m_SkeletonInfo[0].Info[i].pCollector == ppCollectors[j])
            {
                vecCollectorIndices[i] = j;
                break;
            }
        }
    }

    for (DWORD i = 0; i < pData->dwNumBones; i++)
    {
        for (DWORD j = 0; j < pData->dwNumItemPerBone; j++)
        {
            pBoneInfo[i * pData->dwNumItemPerBone + j].dwCollectorIndex = vecCollectorIndices[j];
            pBoneInfo[i * pData->dwNumItemPerBone + j].dwType = m_SkeletonInfo[i].Info[j].dwType;
            pBoneInfo[i * pData->dwNumItemPerBone + j].fWeight = m_SkeletonInfo[i].Info[j].fWeight;
        }
    }

    pData->dwBlockLength = sizeof(BoneInfoSerializeData) * pData->dwNumBones * pData->dwNumItemPerBone;
    *ppData = pData;
    hr = S_OK;
Exit0:
    return hr;
}


KG3DAnimationComposer::TweenTimeInfo::TweenTimeInfo()
{
	pTweenStartInfo = NULL;
	pTweenEndInfo = NULL;
	dwStartTime = 0;
	dwNumBones = 0;
	dwSpan = 0;
}

HRESULT KG3DAnimationComposer::TweenTimeInfo::Init(DWORD dwNumBones)
{
	HRESULT hrResult = E_FAIL;
	KG_PROCESS_ERROR(dwNumBones);

    pTweenStartInfo = new RTS[dwNumBones];
    KGLOG_PROCESS_ERROR(pTweenStartInfo);

	pTweenEndInfo = new RTS[dwNumBones];
	KGLOG_PROCESS_ERROR(pTweenEndInfo);

    this->dwNumBones = dwNumBones;

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(pTweenEndInfo);
        KG_DELETE_ARRAY(pTweenStartInfo);
    }
	return hrResult;
}

void KG3DAnimationComposer::TweenTimeInfo::UnInit()
{
	SAFE_DELETE_ARRAY(pTweenEndInfo);
	SAFE_DELETE_ARRAY(pTweenStartInfo);

	dwNumBones = 0;
}

HRESULT KG3DAnimationComposer::ReadBlock(IFile *pFile, 
                                         KG3DAnimationUpdateExtraInfoCollector **ppCollectors, 
                                         DWORD dwNumCollectors)
{
    HRESULT hr = E_FAIL;
    unsigned long uReadSize = 0;
    SerializeData ReadInData;
    m_SkeletonInfo.clear();

    BoneInfoSerializeData* pBoneInfoData = NULL;

    KG_PROCESS_ERROR(pFile);
    KG_PROCESS_ERROR(ppCollectors);
    KG_PROCESS_ERROR(dwNumCollectors);

    uReadSize = pFile->Read(&ReadInData, sizeof(SerializeData));
    KG_PROCESS_ERROR(uReadSize == sizeof(SerializeData));

    KG_PROCESS_ERROR(ReadInData.dwMask == SerializeData::cdwMask);

    switch (ReadInData.dwVersion)
    {
    case 0:
        {
			pBoneInfoData = new BoneInfoSerializeData[ReadInData.dwNumBones * ReadInData.dwNumItemPerBone];
            KG_PROCESS_ERROR(pBoneInfoData);

            uReadSize = pFile->Read(pBoneInfoData, 
                sizeof(BoneInfoSerializeData) * ReadInData.dwNumBones * ReadInData.dwNumItemPerBone);
            KG_PROCESS_ERROR(uReadSize == sizeof(BoneInfoSerializeData) * ReadInData.dwNumBones * ReadInData.dwNumItemPerBone);

            m_SkeletonInfo.resize(ReadInData.dwNumBones);
            for (size_t i = 0; i < m_SkeletonInfo.size(); i++)
            {
                m_SkeletonInfo[i].Info.resize(ReadInData.dwNumItemPerBone);
                for (size_t j = 0; j < m_SkeletonInfo[i].Info.size(); j++)
                {
                    m_SkeletonInfo[i].Info[j].dwType = pBoneInfoData[i * ReadInData.dwNumItemPerBone + j].dwType;
                    m_SkeletonInfo[i].Info[j].fWeight = pBoneInfoData[i * ReadInData.dwNumItemPerBone + j].fWeight;
                    if (dwNumCollectors > pBoneInfoData[i * ReadInData.dwNumItemPerBone + j].dwCollectorIndex)
                    {
                        m_SkeletonInfo[i].Info[j].pCollector = ppCollectors[pBoneInfoData[i * ReadInData.dwNumItemPerBone + j].dwCollectorIndex];
                        m_SkeletonInfo[i].Info[j].dwIndex = pBoneInfoData[i * ReadInData.dwNumItemPerBone + j].dwCollectorIndex;
                    }
                    else
                    {
                        KGLogPrintf(KGLOG_ERR, "KG3DAnimationComposer::ReadBlock - Invalidate collector index on Bone %d Composer %d", 
                            i,
                            j);
                    }
                }
            }
            m_Controller.resize(ReadInData.dwNumItemPerBone);
			m_TweenTimeInfo.resize(ReadInData.dwNumItemPerBone);
			
			for (size_t i = 0; i < m_TweenTimeInfo.size(); i++)
			{
				m_TweenTimeInfo[i].Init(ReadInData.dwNumBones);
			}
        }
        break;
    default:
        break;
    }
    hr = S_OK;
Exit0:
    SAFE_DELETE_ARRAY(pBoneInfoData);
    return hr;
}

KG3DAnimationComposer::SerializeData::SerializeData()
{
    Init();
}

KG3DAnimationComposer::SerializeData::~SerializeData()
{
    dwMask = 0;
    dwVersion = 0;
    dwNumBones = 0;
}

void KG3DAnimationComposer::SerializeData::Init()
{
    dwMask = cdwMask;
    dwVersion = cdwVersion;
    dwNumBones = 0;
}

KG3DAnimationWarper::SerializeData::SerializeData()
{
    dwMask = cdwMask;
    dwVersion = cdwVersion;
}

KG3DAnimationWarper::SerializeData::~SerializeData()
{

}

void KG3DAnimationWarper::CloneAnimationController(unsigned int uComposerIndex,
                                                   unsigned int uContrllerIndex, 
                                                   const KG3DAnimationController& Src)
{
    KG_PROCESS_ERROR(uComposerIndex < m_Data.size());
    KG_PROCESS_ERROR(uContrllerIndex < m_Data.size());
    m_Data[uComposerIndex].Composer->CloneAnimationController(uContrllerIndex, Src);
Exit0:
    ;
}

void KG3DAnimationComposer::CloneAnimationController(unsigned int uControllerIndex,
                                                     const KG3DAnimationController& Src)
{
    KG_PROCESS_ERROR(uControllerIndex < m_Controller.size());
    Src.Clone(m_Controller[uControllerIndex]);
Exit0:
    ;
}
HRESULT KG3DAnimationComposer::SetBoneTransform(int nBoneIndex,
						 const BoneTransformationInputData& Data)
{
	m_BoneTransforms[nBoneIndex] = *static_cast<const float*>(Data.Data);
	return S_OK;
}

HRESULT KG3DAnimationWarper::SetBoneTransformation(LPCSTR strBoneName,
							  const BoneTransformationInputData& Data)
{
	HRESULT hr = E_FAIL;
	int nBoneIndex = m_Skeleton.FindBone(strBoneName);
	KG_PROCESS_ERROR(nBoneIndex != -1);
	for (size_t i = 0; i < m_Data.size(); i++)
	{
		m_Data[i].Composer->SetBoneTransform(nBoneIndex, Data);
		m_Data[i].Composer->EnableBoneTransformEnable(TRUE);
	}
	hr = S_OK;
Exit0:
	return hr;
}

void KG3DAnimationWarper::EnableBoneTransformEnable(BOOL bEnable)
{
	for (size_t i = 0; i < m_Data.size(); i++)
	{
		m_Data[i].Composer->EnableBoneTransformEnable(bEnable);
	}
}

BOOL KG3DAnimationWarper::IsBoneTransformEnable()
{
	BOOL bReturn = FALSE;
	KG_PROCESS_ERROR(m_Data.size());
	bReturn = m_Data[0].Composer->IsBoneTransformEnable();
Exit0:
	return bReturn;
}

void KG3DAnimationWarper::Pause(BOOL bPause)
{
    KG3DAnimationController *pController = NULL;
    unsigned uSize = (unsigned)m_Data.size();
    ASSERT(uSize == 2);
    for (unsigned i = 0; i < uSize; i++)
    {
        pController = m_Data[i].Composer->GetAnimationController(1 - i);
        ASSERT(pController);
        pController->PauseAnimation(bPause);
    }

	for (size_t i = 0; i < ANICTL_COUNT; i++)
	{
		m_SecondaryControllers[i].PauseAnimation(bPause);
	}
}

HRESULT KG3DAnimationWarperTable::UnInit()
{
	map<UINT,KG3DAnimationWarper*>::iterator i = m_mapAnimationWarper.begin();
	map<UINT,KG3DAnimationWarper*>::iterator iend = m_mapAnimationWarper.end();
	while (i!=iend)
	{
		KG3DAnimationWarper* pWarper = i->second;
		SAFE_DELETE(pWarper);
		i++;
	}
	m_mapAnimationWarper.clear();
	return S_OK;
}

HRESULT KG3DAnimationWarper::Clone(KG3DAnimationWarper* pDest)
{
	HRESULT hr = E_FAIL;
	DWORD dwStartTime = timeGetTime();

	KGLOG_PROCESS_ERROR(pDest);

	pDest->m_BoneMatchTable = m_BoneMatchTable;
	pDest->m_Skeleton = m_Skeleton;
	
	pDest->m_Data.resize(m_Data.size());

	//ppCollectors = new KG3DAnimationUpdateExtraInfoCollector*[m_Data.size()];
	//KGLOG_PROCESS_ERROR(ppCollectors);

	//////////////////////////////////////////////////////////////////////////

	for (size_t i=0;i<m_vecAnimations.size();i++)
	{
		AnimationInfo New;
		New.pClip = NULL;

		New.pCollector = new KG3DRTSInfoCollector;
		KGLOG_PROCESS_ERROR(New.pCollector);

		New.pCollector->CreateStore(pDest->m_pModel);
		New.pCollector->Clear();

		pDest->m_vecAnimations.push_back(New);
		pDest->m_vecTagContainers.push_back(NULL);
	}

	for (size_t i=0;i<m_Data.size();i++)
	{
		pDest->m_Data[i].Composer = new KG3DAnimationComposer;
		KGLOG_PROCESS_ERROR(pDest->m_Data[i].Composer);

		pDest->m_Data[i].Composer->Init(&pDest->m_Skeleton, pDest->m_pModel);
		m_Data[i].Composer->Clone(pDest->m_Data[i].Composer);

		pDest->m_Data[i].BoneWeight = m_Data[i].BoneWeight;
	}

	hr = S_OK;
Exit0:
	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DAnimationWarper::Clone cost %d",dwCost);
	}
	return hr;
}

HRESULT KG3DAnimationWarperTable::LoadWarper(KG3DAnimationWarper* pDest,LPSTR pFileName)
{
	HRESULT hr = E_FAIL;
	map<UINT,KG3DAnimationWarper*>::iterator i;
	KG3DAnimationWarper* pSrc = NULL;
	KGLOG_PROCESS_ERROR(pFileName);
	KGLOG_PROCESS_ERROR(pDest);

	UINT uHash = g_FileNameHash(pFileName);

	i = m_mapAnimationWarper.find(uHash);
	if(i==m_mapAnimationWarper.end())
	{
		pSrc = new KG3DAnimationWarper;
		KGLOG_PROCESS_ERROR(pSrc);
		pSrc->SetModel(pDest->m_pModel);

		pSrc->Load(pFileName);

		m_mapAnimationWarper[uHash] = pSrc;
	}
	else
	{
		pSrc = i->second;
	}

	KGLOG_PROCESS_ERROR(pSrc);

	pSrc->Clone(pDest);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DAnimationComposer::Clone(KG3DAnimationComposer* pDest)
{
	HRESULT hr = E_FAIL;

	pDest->m_SkeletonInfo = m_SkeletonInfo;
	pDest->m_Controller.resize(m_Controller.size());
	pDest->m_TweenTimeInfo.resize(m_TweenTimeInfo.size());

	for(size_t i = 0; i < pDest->m_TweenTimeInfo.size(); i++)
	{
		pDest->m_TweenTimeInfo[i].Init( m_TweenTimeInfo[i].dwNumBones );
	}

	hr = S_OK;
//Exit0:;
	return hr;
}
