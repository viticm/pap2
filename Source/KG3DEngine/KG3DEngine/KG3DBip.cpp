#include "StdAfx.h"
#include "KG3DGraphicsTool.h"
#include "KG3DClipTable.h"
#include "KG3DClipTools.h"
#include "KG3DBip.h"
#include "KG3DEngineManager.h"
#include "KG3DBipTable.h"
#include "KG3DMesh.h"
#include "KG3DModelST.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
inline int _GetLine(char **ppszBuffer, unsigned uLineBufferSize, char szRetLine[])
{
    int nResult  = false;
    char *pszLineBegin = NULL;
    char *pchLineEnd = NULL;
    unsigned uLineLength = 0;

    ASSERT(ppszBuffer);
    ASSERT(*ppszBuffer);
    ASSERT(uLineBufferSize > 0);
    ASSERT(szRetLine);
    KGLOG_PROCESS_ERROR(**ppszBuffer); // not a empty string

    pszLineBegin = *ppszBuffer;

    pchLineEnd = pszLineBegin;
    while (*pchLineEnd && (*pchLineEnd != '\r') && (*pchLineEnd != '\n'))
        ++pchLineEnd;

    uLineLength = (unsigned)(pchLineEnd - pszLineBegin);
    KGLOG_PROCESS_ERROR(uLineLength < uLineBufferSize);
    memcpy(szRetLine, pszLineBegin, uLineLength);
    szRetLine[uLineLength] = '\0';

    // seek to next line
    pszLineBegin = pchLineEnd;
    while (*pszLineBegin && (*pszLineBegin == '\r' || *pszLineBegin == '\n'))
        ++pszLineBegin;
    *ppszBuffer = pszLineBegin;

    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
KG3DBip::KG3DBip(void)
{
    m_ulRefCount = 1;
    m_pSkeleton = NULL;
    m_pObjMatrices = NULL;
	m_nNumBones = 0;
	m_dwID = static_cast<DWORD>(KG3DBipTable::INVALIDATE_ID);
    m_bEnableBoneTransform = FALSE;
	m_bEnableBoneScale = FALSE;
	m_pTweenTimeInfo = NULL;
	m_bInTween = FALSE;
	m_CurrentPriority = ANICTL_PRIMARY;
    m_pRTSCollector = NULL;
	m_lpOwenerModelST = NULL;
}

KG3DBip::~KG3DBip(void)
{
	CleanUp();
}

HRESULT KG3DBip::GetBoneInfoDataPoint(BoneInfoData **ppBoneInfo)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(ppBoneInfo);
    *ppBoneInfo = m_pSkeleton;
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DBip::GetBaseBoneCount(DWORD *pdwCount)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(pdwCount);

    *pdwCount = (DWORD)m_BaseBones.size();
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DBip::GetBaseBoneByPosition(DWORD dwPositon, INT *pnBaseBone)
{
    HRESULT hResult = E_FAIL;
    DWORD dwSize = 0;
    KGLOG_PROCESS_ERROR(pnBaseBone);
    
    dwSize = (DWORD)m_BaseBones.size();
    KGLOG_PROCESS_ERROR(dwPositon < dwSize);

    *pnBaseBone = m_BaseBones[dwPositon];
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DBip::LoadFromTextFile(const char cszFileName[])
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int nRetCode = false;
    IFile *piFile = NULL;
    unsigned uFileSize = 0;
    unsigned uReadSize = 0;
    char *pszFileBuffer = NULL;
    char *pszBuffer = NULL;
    char szLine[1024];
    int nBaseBonesNum = 0;
    _CHILD_BONE *pChildBones = NULL;

    ASSERT(!m_pSkeleton);

    KGLOG_PROCESS_ERROR(cszFileName);

    piFile = g_OpenFile(cszFileName);
    KGLOG_PROCESS_ERROR(piFile);

    uFileSize = piFile->Size();
    KGLOG_PROCESS_ERROR(uFileSize > 0);

    pszFileBuffer = new char[uFileSize + 1];
    KGLOG_PROCESS_ERROR(pszFileBuffer);

    uReadSize = piFile->Read(pszFileBuffer, uFileSize);
    KGLOG_PROCESS_ERROR(uReadSize == uFileSize);
    pszFileBuffer[uReadSize] = '\0';

    pszBuffer = pszFileBuffer;

    /// read bone num
    nRetCode = _GetLine(&pszBuffer, sizeof(szLine), szLine);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_nNumBones = atoi(szLine);
    KGLOG_PROCESS_ERROR(m_nNumBones > 0);

    m_pSkeleton = new BoneInfoData[m_nNumBones];
    KGLOG_PROCESS_ERROR(m_pSkeleton);

    pChildBones = new _CHILD_BONE[m_nNumBones];
    KGLOG_PROCESS_ERROR(pChildBones);

    /// read bones
    for (DWORD i = 0; i < (DWORD)m_nNumBones; i++)
    {
        nRetCode = _GetLine(&pszBuffer, sizeof(szLine), szLine);
        KGLOG_PROCESS_ERROR(nRetCode);

        hrRetCode = _LineToBoneInfo(szLine, &m_pSkeleton[i], &pChildBones[i]);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        m_pSkeleton[i].dwIndex = i;
        m_pSkeleton[i].dwParentIndex = i;
    }

    for (int i = 0; i < m_nNumBones; i++)
    {
        hrRetCode = _ConstructSkeleton(i, pChildBones[i]);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    /// read base bone index
    nRetCode = _GetLine(&pszBuffer, sizeof(szLine), szLine);
    KGLOG_PROCESS_ERROR(nRetCode);

    nBaseBonesNum = atoi(szLine);
    KGLOG_PROCESS_ERROR(nBaseBonesNum > 0);

    for (int i = 0; i < nBaseBonesNum; i++)
    {
        nRetCode = _GetLine(&pszBuffer, sizeof(szLine), szLine);
        KGLOG_PROCESS_ERROR(nRetCode);

        int nBaseBoneIndex = 0;
        nBaseBoneIndex = atoi(szLine);
        KGLOG_PROCESS_ERROR(nBaseBonesNum > 0);

        m_BaseBones.push_back(nBaseBoneIndex);
    }

    m_scName = cszFileName;

    m_pObjMatrices = new D3DXMATRIX[m_nNumBones];
    KGLOG_PROCESS_ERROR(m_pObjMatrices);

    hrResult = S_OK;
Exit0:
    KG_DELETE_ARRAY(pChildBones);
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(m_pObjMatrices);
        KG_DELETE_ARRAY(m_pSkeleton);
        KGLogPrintf(KGLOG_DEBUG, "bip load failed \"%s\"", cszFileName);
    }
    KG_DELETE_ARRAY(pszFileBuffer);
    KG_COM_RELEASE(piFile);
    return hrResult;
}

HRESULT KG3DBip::InitTweenData()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	int nTweenTimeInfoInitFlag = false;

    m_pRTSCollector = new KG3DRTSInfoCollector;
    KGLOG_PROCESS_ERROR(m_pRTSCollector);

    hrRetCode = m_pRTSCollector->CreateStore(this);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    m_pTweenTimeInfo = new KG3DAnimationComposer::TweenTimeInfo();
    KGLOG_PROCESS_ERROR(m_pTweenTimeInfo);

    hrRetCode = m_pTweenTimeInfo->Init((DWORD)m_nNumBones);
    KG_COM_PROCESS_ERROR(hrRetCode);
	nTweenTimeInfoInitFlag = true;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
		if (nTweenTimeInfoInitFlag)
		{
            m_pTweenTimeInfo->UnInit();
			nTweenTimeInfoInitFlag = false;
		}
		KG_DELETE(m_pTweenTimeInfo);
        KG_DELETE(m_pRTSCollector);
    }
    return hrResult;
}

HRESULT KG3DBip::UnInitTweenData()
{
    if (m_pTweenTimeInfo)
    {
        m_pTweenTimeInfo->UnInit();
    }
    KG_DELETE(m_pTweenTimeInfo);
    KG_DELETE(m_pRTSCollector);

    return S_OK;
}

void KG3DBip::Clone(const KG3DBip *pSrc)
{
    HRESULT hrResult = E_FAIL;

    m_bEnableBoneTransform = pSrc->m_bEnableBoneTransform;
    m_bEnableBoneScale = pSrc->m_bEnableBoneScale;
    m_nNumBones = pSrc->m_nNumBones;
    SAFE_DELETE_ARRAY(m_pSkeleton);
    m_pSkeleton = new BoneInfoData[m_nNumBones];
    KGLOG_PROCESS_ERROR(m_pSkeleton);

    memcpy_s(m_pSkeleton, sizeof(BoneInfoData) * m_nNumBones, pSrc->m_pSkeleton, sizeof(BoneInfoData) * m_nNumBones);

    m_BaseBones = pSrc->m_BaseBones;
    m_BoneTransforms = pSrc->m_BoneTransforms;
    m_BoneScale = pSrc->m_BoneScale;
    m_scName = pSrc->m_scName;
    m_dwID = pSrc->m_dwID;

    ASSERT(!m_pObjMatrices);
    m_pObjMatrices = new D3DXMATRIX[m_nNumBones];
    KGLOG_PROCESS_ERROR(m_pObjMatrices);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(m_pObjMatrices);
        SAFE_DELETE_ARRAY(m_pSkeleton);
    }

    return ;
}

void KG3DBip::CleanUp()
{
    UnInitTweenData();
	m_bEnableBoneTransform = FALSE;
	m_bEnableBoneScale = FALSE;
    KG_DELETE_ARRAY(m_pObjMatrices);
	SAFE_DELETE_ARRAY(m_pSkeleton);
	m_dwID = static_cast<DWORD>(KG3DBipTable::INVALIDATE_ID);
	
	m_CurrentPriority = ANICTL_PRIMARY;
	m_bInTween = FALSE;
}

HRESULT KG3DBip::_LineToBoneInfo(
    const char cszLineBuffer[], BoneInfoData *pRetBoneInfo, _CHILD_BONE *pRetChildBone
)
{
    HRESULT hrResult = E_FAIL;
    char *pszLine = NULL;
    char *pszFind = NULL;
    unsigned uBoneNameLength = 0;
    DWORD dwChildNum = 0;

    ASSERT(cszLineBuffer);
    ASSERT(pRetBoneInfo);
    ASSERT(pRetChildBone);

    pszLine = (char *)cszLineBuffer;

    pszFind = strstr(pszLine, "  ");
    if (pszFind)
        uBoneNameLength = (unsigned)(pszFind - pszLine);
    else
        uBoneNameLength = (unsigned)strlen(pszLine);

    KGLOG_PROCESS_ERROR(uBoneNameLength < sizeof(pRetBoneInfo->strBoneName));
    memcpy(pRetBoneInfo->strBoneName, pszLine, uBoneNameLength);
    pRetBoneInfo->strBoneName[uBoneNameLength] = '\0';

    strlwr(pRetBoneInfo->strBoneName);

    pRetBoneInfo->dwNumChildBones = 0;

    while (pszFind)
    {
        while (*pszFind == ' ') 
            pszFind++;

        if (*pszFind == '\0')
            break;

        char *pszEndOfChildName = strstr(pszFind, "  ");
        if (pszEndOfChildName)
            uBoneNameLength = (unsigned)(pszEndOfChildName - pszFind);
        else
            uBoneNameLength = (unsigned)strlen(pszFind);    //the last one

        KGLOG_PROCESS_ERROR(uBoneNameLength < sizeof(pRetChildBone->szChildName[dwChildNum]));
        KGLOG_PROCESS_ERROR(dwChildNum < MAX_NUM_CHILD_BONE);
        memcpy(pRetChildBone->szChildName[dwChildNum], pszFind, uBoneNameLength);
        pRetChildBone->szChildName[dwChildNum][uBoneNameLength] = '\0';
        strlwr(pRetChildBone->szChildName[dwChildNum]);
        ++dwChildNum;

        pszFind = pszEndOfChildName;
    }

    pRetBoneInfo->dwNumChildBones = dwChildNum;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DBip::_ConstructSkeleton(int nIndex, const _CHILD_BONE &crChildBone)
{
    HRESULT hrResult = E_FAIL;
    BoneInfoData *pInfo = NULL;
    DWORD dwNumChild = 0;
    int nBoneIndex = -1;

    ASSERT(nIndex < m_nNumBones);

    pInfo = &m_pSkeleton[nIndex];
    ASSERT(pInfo);

    dwNumChild = pInfo->dwNumChildBones;
    for (DWORD i = 0; i < dwNumChild; i++)
    {
        nBoneIndex = FindBone(crChildBone.szChildName[i]);
        KGLOG_PROCESS_ERROR(nBoneIndex != -1);

        pInfo->ChildIndices[i] = nBoneIndex;
        m_pSkeleton[nBoneIndex].dwParentIndex = (DWORD)nIndex;
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KG3DBip::FindBone(const TCHAR* strBoneName) const
{
	KG_PROCESS_ERROR(strBoneName);

	for (int i = 0; i < m_nNumBones; i++)
    {
        if (!strcmp(m_pSkeleton[i].strBoneName, strBoneName))
            return i;
    }

Exit0:
	return -1;
}

enuAnimationControllerPriority KG3DBip::GetCurrentPriority(KG3DAnimationController* pControllers)
{
	enuAnimationControllerPriority Priority = ANICTL_PRIMARY;
	for (int i = ANICTL_COUNT - 1; i > 0; i--)
	{
		if (pControllers[i].HasAnimation())
		{
			Priority = static_cast<enuAnimationControllerPriority>(i);
			break;
		}
	}
	return Priority;
}


HRESULT KG3DBip::_PlayAnimation(KG3DClip* pClip, 
							   DWORD PlayType, 
							   int nOffsetTime,
							   float fSpeed, 
							   TweenTimeSpan Span,
							   enuAnimationControllerPriority Priority,
							   KG3DAnimationController* pControllers)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	
	m_TweenSpan = Span;

	hrRetCode = GetClipTool().CheckClipByBip(pClip, this);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	if (!pControllers[m_CurrentPriority].HasAnimation() ||//如果当前的轨没有动画
		(Priority < m_CurrentPriority))//如果小于， 表示是低轨， 低轨的去掉tweentime
	{
		switch (PlayType)
		{
		case ENU_ANIMATIONPLAY_ADDCURRENT_ONCE:
			PlayType = ENU_ANIMATIONPLAY_ONCE;
			break;
		case ENU_ANIMATIONPLAY_ADDCURRENT:
			PlayType = ENU_ANIMATIONPLAY_CIRCLE;
			break;
		}
	}

	switch (PlayType)
	{
	case ENU_ANIMATIONPLAY_ONCE:
	case ENU_ANIMATIONPLAY_CIRCLE:
		{
			pControllers[Priority].Detach();
			pControllers[Priority].Attach(pClip);
			pControllers[Priority].StartAnimation(PlayType, nOffsetTime, fSpeed);
		}
		break;
	case ENU_ANIMATIONPLAY_ADDCURRENT:
		{
			ChangeAnimation(pClip, 
				fSpeed, 
				ENU_ANIMATIONPLAY_CIRCLE, 
				m_TweenSpan.dwTweenIn,
				m_TweenSpan.fTweenWeigth,
				m_CurrentPriority,
				Priority,
				pControllers);
		}
		break;
	case ENU_ANIMATIONPLAY_ADDCURRENT_ONCE:
		{
			ChangeAnimation(pClip, 
				fSpeed,
				ENU_ANIMATIONPLAY_ONCE,
				m_TweenSpan.dwTweenIn,
				m_TweenSpan.fTweenWeigth,
				m_CurrentPriority,
				Priority,
				pControllers);
		}
		break;
	default:
		break;
	}

	if (m_CurrentPriority < Priority)
	{
		m_CurrentPriority = Priority;
	}
	pControllers[Priority].SetMotionExtraInfo(IKG3DAnimationController::TYPE_ALLPART);
	pControllers[ANICTL_SECONDARY].RegistereEventHandler(pControllers[ANICTL_PRIMARY].GetEventHandler());

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DBip::PlayAnimation(LPCTSTR strFileName, 
                               DWORD PlayType, 
                               int nOffsetTime,
                               float fSpeed, 
                               void* pExtInfo,
							   enuAnimationControllerPriority Priority,
							   KG3DAnimationController* pControllers,
							   KG3DClip* pClip)
{
    HRESULT hrResult = E_FAIL;
	//HRESULT hrRetCode = E_FAIL;
	DWORD dwStartTime = timeGetTime();

	KGLOG_PROCESS_ERROR(strFileName);
    KGLOG_PROCESS_ERROR(pControllers);
	KGLOG_PROCESS_ERROR(pClip);
	if (pExtInfo)
	{
		m_TweenSpan = *(TweenTimeSpan*)pExtInfo;
	}
	else
	{
		m_TweenSpan.fTweenWeigth = 1.0f;
		m_TweenSpan.dwTweenIn = 0;
		m_TweenSpan.dwTweenOut = 0;
	}

	if(pClip->IsLoad())
	{
		hrResult = _PlayAnimation(pClip,PlayType,nOffsetTime,fSpeed,m_TweenSpan,Priority,pControllers);
	}
	else
	{
		KG3DModelCommand_Bip_PlayAnimation* pAni = new KG3DModelCommand_Bip_PlayAnimation(pClip,
			PlayType,nOffsetTime,fSpeed,m_TweenSpan,Priority,pControllers);
		m_Commands.AddCommand(pAni,TRUE);
		hrResult = S_OK;
	}

Exit0:

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DBip::PlayAnimation cost %d",dwCost);
	}
   // SAFE_RELEASE(pClip);
	return hrResult;
}

void KG3DBip::FrameMove(KG3DAnimationController* pControllers)
{
    KG3DAnimationController *pCurAniController = &pControllers[m_CurrentPriority];
    ASSERT(pCurAniController);

    ProcessCommands();

	if (m_bInTween)
	{
		DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
		ASSERT(dwNowTime >= m_pTweenTimeInfo->dwStartTime);
		DWORD dwTimeSpan = dwNowTime - m_pTweenTimeInfo->dwStartTime;
		if (dwTimeSpan < m_pTweenTimeInfo->dwSpan)
		{
			RTS rts;
			ASSERT(m_pTweenTimeInfo->dwSpan > 0);
			float fWeight = (float)dwTimeSpan / m_pTweenTimeInfo->dwSpan;
			for (DWORD i = 0; i < m_pTweenTimeInfo->dwNumBones; i++)
			{
				KG3DClip::InterpolateRTSKeyFrame(
					rts,
					m_pTweenTimeInfo->pTweenStartInfo[i],
					m_pTweenTimeInfo->pTweenEndInfo[i],
					fWeight
				);
                KG3DClip::RTS2Matrix(m_pObjMatrices[i], rts);
			}

            UpdateToObj();
		}
		else
		{
			if (pCurAniController->IsPaused())
				pCurAniController->PauseAnimation(FALSE);

			m_bInTween = FALSE;
		}
	}

    if (!m_bInTween && pCurAniController->HasAnimation())
    {
        pCurAniController->SetUpdateMatrix(m_pObjMatrices);
        pCurAniController->UpdateAnimation(NULL, TRUE, TRUE);

        UpdateToObj();
    }
}

void KG3DBip::Render()
{
    for (int i = 1; i < m_nNumBones; i++)
	{
		D3DXVECTOR3 A = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 B = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        D3DXVec3TransformCoord(&A, &A, &m_pObjMatrices[m_pSkeleton[i].dwParentIndex]);
        D3DXVec3TransformCoord(&B, &B, &m_pObjMatrices[i]);
		A.z += 100.0f;
		B.z += 100.0f;
		g_cGraphicsTool.DrawLine(&A, &B, 0xffff0000, 0xff00ffff);
	}
}

void KG3DBip::UpdateToObj()
{
	ASSERT(m_lpOwenerModelST);

    if (g_bClient)
    {
        KG_PROCESS_ERROR(m_lpOwenerModelST->IsVisible());
    }

	if (m_bEnableBoneScale)
	{
		std::map<int, D3DXVECTOR3>::iterator it = m_BoneScale.begin();
		std::map<int, D3DXVECTOR3>::iterator itend = m_BoneScale.end();
		while (it != itend)
		{
			int nBone = (*it).first;
			D3DXVECTOR3 vScale = (*it).second;
			D3DXMATRIX matScale;
			D3DXMatrixScaling(&matScale, vScale.x,vScale.y, vScale.z);
            D3DXMatrixMultiply(&m_pObjMatrices[nBone], &matScale, &m_pObjMatrices[nBone]);
			++it;
		}
	}    
	if (m_bEnableBoneTransform)
    {
		std::map<int, float>::iterator it = m_BoneTransforms.begin();
		std::map<int, float>::iterator itend = m_BoneTransforms.end();
        while (it != itend)
        {
            int nBone = (*it).first;
            float fAngle = (*it).second;
            D3DXMATRIX matRotationX;
            D3DXMatrixRotationX(&matRotationX, fAngle);
            D3DXMatrixMultiply(&m_pObjMatrices[nBone], &matRotationX, &m_pObjMatrices[nBone]);
            ++it;
        }
        m_bEnableBoneTransform = false;
    }

	for (size_t j = 0; j < m_BaseBones.size(); j++)
	{
		int nCurBaseBone = m_BaseBones[j];
		for(DWORD i = 0; i < m_pSkeleton[nCurBaseBone].dwNumChildBones; i++)
		{
            Helper_UpdateToObj(m_pSkeleton[nCurBaseBone].ChildIndices[i], m_pObjMatrices[nCurBaseBone]);
		}
	}
	
Exit0:
	return;
}

void KG3DBip::Helper_UpdateToObj(int nIndex, const D3DXMATRIX& matBase)
{
	KG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_nNumBones);
	BoneInfoData& Bone = m_pSkeleton[nIndex];
    D3DXMatrixMultiply(&m_pObjMatrices[nIndex], &m_pObjMatrices[nIndex], &matBase);
    for (DWORD i = 0; i < Bone.dwNumChildBones; i++)
	{
        Helper_UpdateToObj(Bone.ChildIndices[i], m_pObjMatrices[nIndex]);
	}
Exit0:
	;
}

ULONG KG3DBip::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DBip::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        delete this; 
    return uNewRefCount;
}

const D3DXMATRIX& KG3DBip::GetBoneObjMatrix(unsigned int uIndex)
{
	ASSERT (static_cast<int>(uIndex) < m_nNumBones);
    return m_pObjMatrices[uIndex];
}

int KG3DBip::GetNumBones() const
{
	return m_nNumBones;
}

const TCHAR* KG3DBip::GetBoneName(int nIndex) const
{
	KG_PROCESS_ERROR(nIndex < m_nNumBones && nIndex >= 0);
	return m_pSkeleton[nIndex].strBoneName;
Exit0:
	return NULL;
}

int KG3DBip::GetParentIndex(int nIndex) const
{
	KG_PROCESS_ERROR(nIndex < m_nNumBones && nIndex >= 0);
	return static_cast<int>(m_pSkeleton[nIndex].dwParentIndex);
Exit0:
	return -1;
}

int KG3DBip::GetNumChildren(int nIndex) const
{
	KG_PROCESS_ERROR(nIndex < m_nNumBones && nIndex >= 0);
	return static_cast<int>(m_pSkeleton[nIndex].dwNumChildBones);
Exit0:
	return -1;
}

int KG3DBip::GetChildIndex(int nBoneIndex, int nChildIndex) const
{
	KG_PROCESS_ERROR(nBoneIndex < m_nNumBones && nBoneIndex >= 0);
	KG_PROCESS_ERROR(nChildIndex < static_cast<int>(m_pSkeleton[nBoneIndex].dwNumChildBones));

	return static_cast<int>(m_pSkeleton[nBoneIndex].ChildIndices[nChildIndex]);
Exit0:
	return -1;
}

const std::vector<int>& KG3DBip::GetBaseBoneID() const
{
	return m_BaseBones;
}

HRESULT KG3DBip::GetSkeleton(vector<int>& vecSkeletons, int nBoneIndex)
{
    HRESULT hResult  = E_FAIL;

    vecSkeletons.clear();
   
    KG_PROCESS_ERROR(nBoneIndex < m_nNumBones);
    KG_PROCESS_ERROR(nBoneIndex >= 0);

	vecSkeletons.push_back(nBoneIndex);
	int nPrevIndex = nBoneIndex;

    while ((nBoneIndex = GetParentIndex(nBoneIndex)) != -1)
    {
        if (nPrevIndex == nBoneIndex)
            break;
        vecSkeletons.push_back(nBoneIndex);
        nPrevIndex = nBoneIndex;
    }
    KG_PROCESS_ERROR(vecSkeletons.size());

    hResult = S_OK;
Exit0:
    return hResult;
}
int KG3DBip::GetNumBaseBone() const
{
    return static_cast<int>(m_BaseBones.size());
}
int KG3DBip::GetBaseBone(int nIndex) const
{
    int nReturn = -1;
    KG_PROCESS_ERROR(nIndex < static_cast<int>(m_BaseBones.size()));

    nReturn = m_BaseBones[nIndex];
Exit0:
    return nReturn;
}

HRESULT KG3DBip::SetBoneTransform(LPCSTR strBoneName, const BoneTransformationInputData& Data)
{
    HRESULT hr = E_FAIL;
    KG_PROCESS_ERROR(strBoneName);
    int nBoneIndex = FindBone(strBoneName);
    KG_PROCESS_ERROR(nBoneIndex != -1);
	m_BoneTransforms[nBoneIndex] = *static_cast<const float*>(Data.Data);
    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DBip::SetBoneScale(LPCSTR strBoneName, D3DXVECTOR3& vScale)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(strBoneName);
	int nBoneIndex = FindBone(strBoneName);
	KG_PROCESS_ERROR(nBoneIndex != -1);
	m_BoneScale[nBoneIndex] = vScale;
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DBip::GetBoneScale(LPCSTR strBoneName, D3DXVECTOR3* pvScale)
{
	HRESULT hr = E_FAIL;
    map<int,D3DXVECTOR3>::iterator iFind;

	KG_PROCESS_ERROR(strBoneName);
	int nBoneIndex = FindBone(strBoneName);
	KG_PROCESS_ERROR(nBoneIndex != -1);

	iFind = m_BoneScale.find(nBoneIndex);
	if(iFind != m_BoneScale.end())
	{
		(*pvScale) = iFind->second;
	}
	else
	{
		(*pvScale) = D3DXVECTOR3(1,1,1);
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DBip::GenerateTweenInfo(KG3DAnimationComposer::TweenTimeInfo* pTweenTimeInfo,KG3DAnimationController* pAniController,BOOL bStart)
{
	KG3DAnimationController ControllerSave;
	RTS* pRTS = pTweenTimeInfo->pTweenStartInfo;

	pAniController->Clone(ControllerSave);

	if(!bStart)
		pRTS = pTweenTimeInfo->pTweenEndInfo;

	ControllerSave.UpdateAnimation(m_pRTSCollector, TRUE, FALSE);
	memcpy(
		pRTS, 
		m_pRTSCollector->GetData(), 
		pTweenTimeInfo->dwNumBones * sizeof(RTS)
	);

	return S_OK;
}

//从完整2号动画切回的处理
HRESULT KG3DBip::OnSecondaryAnimationFinish()
{	
	HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	KG3DAnimationController* pControllers = NULL;
    BOOL bInitTweenDataFlag = FALSE;

    if (!m_pTweenTimeInfo)
    {
        hrRetCode = InitTweenData();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
        bInitTweenDataFlag = TRUE;
    }

	if (m_lpOwenerModelST)
	{
		pControllers = m_lpOwenerModelST->m_Controllers;
	}
	KGLOG_PROCESS_ERROR(pControllers);

    KG_PROCESS_SUCCESS(!m_TweenSpan.dwTweenOut);

	GenerateTweenInfo(m_pTweenTimeInfo,&pControllers[ANICTL_SECONDARY],TRUE);

	GenerateTweenInfo(m_pTweenTimeInfo,&pControllers[ANICTL_PRIMARY],FALSE);

    m_pTweenTimeInfo->dwStartTime = g_cGraphicsTool.GetNowTime();
    m_pTweenTimeInfo->dwSpan = m_TweenSpan.dwTweenOut;

    pControllers[ANICTL_PRIMARY].PauseAnimation(TRUE);
    m_bInTween = TRUE;

Exit1:
	m_CurrentPriority = ANICTL_PRIMARY;
	pControllers[ANICTL_SECONDARY].Detach();

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bInitTweenDataFlag)
        {
            UnInitTweenData();
            bInitTweenDataFlag = false;
        }
    }
	return hrResult;
}

HRESULT KG3DBip::ChangeAnimation(IEKG3DClip* pClip,
								 float fSpeed,
								 DWORD dwType,
                                 DWORD dwTweenTime,
								 float fTweenWeight,
								 enuAnimationControllerPriority Src,
								 enuAnimationControllerPriority Dest,
								 KG3DAnimationController* pControllers)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	DWORD dwNowTime = 0;
    DWORD dwTimeSpan = 0;
    BOOL bInitTweenDataFlag = FALSE;

    if (!m_pTweenTimeInfo)
    {
        hrRetCode = InitTweenData();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
        bInitTweenDataFlag = TRUE;
    }

    if (dwTweenTime)
    {
        //生成开始帧
	    dwNowTime = g_cGraphicsTool.GetNowTime();
        ASSERT(dwNowTime >= m_pTweenTimeInfo->dwStartTime);
	    dwTimeSpan = dwNowTime - m_pTweenTimeInfo->dwStartTime;
	    if (dwTimeSpan < m_pTweenTimeInfo->dwSpan)
	    {
		    //如果本来就在tween中, 用当前的tweeninfo生成新的tweeninfo
            ASSERT(m_pTweenTimeInfo->dwSpan > 0);
		    float fWeight = static_cast<float>(dwTimeSpan) / m_pTweenTimeInfo->dwSpan;
		    for (DWORD i = 0; i < m_pTweenTimeInfo->dwNumBones; i++)
		    {
				KG3DClip::InterpolateRTSKeyFrame(m_pTweenTimeInfo->pTweenStartInfo[i],
				    m_pTweenTimeInfo->pTweenStartInfo[i],
				    m_pTweenTimeInfo->pTweenEndInfo[i],
				    fWeight);
		    }
	    }
	    else
        {
            pControllers[Src].PauseAnimation(TRUE);

			GenerateTweenInfo(m_pTweenTimeInfo,&pControllers[Src],TRUE);
	    }
		//////////////////////////////////////////////////////////////////////////
	    //生成结束帧
	    pControllers[Dest].SetAnimation(pClip);
	    pControllers[Dest].StartAnimation(dwType, 0, fSpeed);

		GenerateTweenInfo(m_pTweenTimeInfo,&pControllers[Dest],FALSE);

        pControllers[Dest].PauseAnimation(TRUE);

	    m_pTweenTimeInfo->dwStartTime = dwNowTime;
	    m_pTweenTimeInfo->dwSpan = dwTweenTime;
        m_bInTween = TRUE;
    }
    else
    {
        pControllers[Dest].SetAnimation(pClip);
        pControllers[Dest].StartAnimation(dwType, 0, fSpeed);
    }

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bInitTweenDataFlag)
        {
            UnInitTweenData();
            bInitTweenDataFlag = FALSE;
        }
    }
	return hrResult;
}

void KG3DBip::OnModelSTAttach(KG3DModel *pModel,KG3DAnimationController* pControllers)
{
	for (DWORD i = 0; i < ANICTL_COUNT; i++)
	{
		pControllers[i].SetModel(pModel);
        pControllers[i].m_Priority = static_cast<enuAnimationControllerPriority>(i);
	}
}

HRESULT KG3DBip::ProcessCommands()
{
	return m_Commands.ProcessCommandsForUser(this);
}

BOOL KG3DBip::OnProcessCommands(KG3DModelCommandBase* pCmd)
{
	BOOL bSucceed = FALSE;

	switch(pCmd->m_eType)
	{
	case CMD_BIP_PlayAnimation:
		{
			KG3DModelCommand_Bip_PlayAnimation* pAni = static_cast<KG3DModelCommand_Bip_PlayAnimation*>(pCmd);
			if(pAni->m_pClip->IsLoad())
			{
				_PlayAnimation(pAni->m_pClip,
					pAni->m_PlayType,
					pAni->m_nOffsetTime,
					pAni->m_fSpeed,
					pAni->m_Span,
					pAni->m_Priority,
					pAni->m_pControllers);

				bSucceed = TRUE;	
			}
			break;
		}
	default:
		ASSERT(0);
	}
	return bSucceed;
}
