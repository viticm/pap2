#include "StdAfx.h"
#include "KG3DClip.h"
#include "KG3DModel.h"
#include "KG3DClipTable.h"
#include "KG3DGraphicsTool.h"
#include "KG3DAnimationController.h"
#include "KG3DTexture.h"
#include "KG3DEngineManager.h"
//#include "KG3DMultiThreadLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const DWORD ANI_FILE_MASK = 0x414E494D;
const DWORD ANI_FILE_END_FLAG = 0xFFFFFFFF;

extern BOOL g_bMultiThreadLoadAnimation;
extern int  g_nAniEnableInterpolate;
extern int g_nInterpolateFPS;

void KG3DClip::InterpolateRTSKeyFrame(RTS& FrameResult, 
							const RTS& FrameA, 
							const RTS& FrameB, 
							float fWeight)
{
	D3DXQUATERNION sRot, Rot;
	D3DXVECTOR3 Scale, Translation;
	D3DXQuaternionSlerp(&FrameResult.SRotation, &FrameA.SRotation, &FrameB.SRotation, fWeight);
	D3DXQuaternionSlerp(&FrameResult.Rotation, &FrameA.Rotation, &FrameB.Rotation, fWeight);
	D3DXVec3Lerp(&FrameResult.Scale, &FrameA.Scale, &FrameB.Scale, fWeight);
	D3DXVec3Lerp(&FrameResult.Translation, &FrameA.Translation, &FrameB.Translation, fWeight);
	FrameResult.Sign = FrameA.Sign;
}

void KG3DClip::ComposeRTS(RTS &rtsResult,
						  RTS **ppRTS,
						  DWORD *pFlags,
						  DWORD dwNumRTS)
{
	for (DWORD i = 0; i < dwNumRTS; i++)
	{
		DWORD dwCurFlag = pFlags[i] & Bone_Scale_Mask;
		
		switch (dwCurFlag)
		{
		case Bone_Scale:
			{
				rtsResult.Scale = ppRTS[i]->Scale;
				rtsResult.Sign = ppRTS[i]->Sign;
				rtsResult.SRotation = ppRTS[i]->SRotation;
			}
			break;
		case Bone_Scale_Reverse:
		case Bone_Scale_None:
			break;			
		}
		
		dwCurFlag = pFlags[i] & Bone_Rotation_Mask;
		
		switch (dwCurFlag)
		{
		case Bone_Rotation:
			{
				rtsResult.Rotation = ppRTS[i]->Rotation;
			}
			break;
		case Bone_Rotation_Reverse:
		case Bone_Rotation_None:
			break;
		}
		
		dwCurFlag = pFlags[i] & Bone_Translation_Mask;
		switch (dwCurFlag)
		{
		case Bone_Translation:
			{
				rtsResult.Translation = ppRTS[i]->Translation;
			}
			break;
		case Bone_Translation_None:
		case Bone_Translation_Reverse:
			break;
		}
	}
}

void KG3DClip::RTS2Matrix(D3DXMATRIX& matResult, const RTS& RtsSource)
{
    D3DXMATRIX matSRot, matSRotInv, matRot, matScale, matScaleSignAndTrans;

    D3DXMatrixRotationQuaternion(&matSRot, &RtsSource.SRotation);
    D3DXMatrixTranspose(&matSRotInv, &matSRot);
    D3DXMatrixRotationQuaternion(&matRot, &RtsSource.Rotation);
    D3DXMatrixTranspose(&matRot, &matRot);
    D3DXMatrixScaling(&matScale, RtsSource.Scale.x, RtsSource.Scale.y, RtsSource.Scale.z);

    // construct matScaleSign * matTrans
    matScaleSignAndTrans._12 = matScaleSignAndTrans._13 = matScaleSignAndTrans._14 = 
    matScaleSignAndTrans._21 = matScaleSignAndTrans._23 = matScaleSignAndTrans._24 = 
    matScaleSignAndTrans._31 = matScaleSignAndTrans._32 = matScaleSignAndTrans._34 = 0.0f;
    matScaleSignAndTrans._11 = matScaleSignAndTrans._22 = matScaleSignAndTrans._33 = RtsSource.Sign;
    matScaleSignAndTrans._41 = RtsSource.Translation.x;
    matScaleSignAndTrans._42 = RtsSource.Translation.y;
    matScaleSignAndTrans._43 = RtsSource.Translation.z;
    matScaleSignAndTrans._44 = 1.0f;

    D3DXMatrixMultiply(&matResult, &matSRot, &matScale);
    D3DXMatrixMultiply(&matResult, &matResult, &matSRotInv);
    D3DXMatrixMultiply(&matResult, &matResult, &matRot);
    D3DXMatrixMultiply(&matResult, &matResult, &matScaleSignAndTrans);
}

KG3DClip::KG3DClip(void)
{
    m_pbyFileData = NULL;
    m_uFileDataSize = 0;
	m_dwType = 0; 
	m_dwID   = 0;
    m_ulRefCount = 1;
    m_dwOptions = 0;

	m_dwNumFrames = 0;
    m_fFrameLength = 0.0f;
    m_dwAniLen = 0;
    m_dwNumVertices = 0;
	m_dwNumAnimatedVertice = 0;
	m_pAnimatedVerticesIndex = NULL;
	m_pAnimatedVerticesPosition = NULL;
	m_pAnimationUV = NULL;
    m_dwNumAnimatedBone = 0;

    m_bPreInterpolateFlag = false;
    m_dwInterpolateFrames = 0;
    m_dwExpectFrames = 0;
	
	m_bInMultiThreadLoadList = FALSE;
    m_eDynamicLoadState = STATE_NONE;
    m_dwLastUseTime = 0;
    m_uTotalMemSize = 0;

    m_ppszBoneName = NULL;
    m_ppBoneRTS = NULL;
    m_ppBoneMatrixPreCompute = NULL;
    m_ppBoneRTSPreCompute = NULL;
}

KG3DClip::~KG3DClip(void)
{
	Clear();
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DClip::GetFrameLength(FLOAT *fRetLength)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(fRetLength);

    *fRetLength = m_fFrameLength;
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DClip::SetFrameLength(FLOAT fFrameLength)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(fFrameLength > 0.0f); 

    m_fFrameLength = fFrameLength;
    m_dwAniLen = (DWORD)(fFrameLength * (m_dwNumFrames - 1));
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DClip::GetNumFrames(DWORD *pdwRetNum)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetNum);

    *pdwRetNum = m_dwNumFrames;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DClip::GetName(LPCSTR *ppszName)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppszName);

    *ppszName = m_strClipName.c_str();

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DClip::Rename(const TCHAR* strNewName)
{
    if (!strNewName)
    {
        m_strClipName = "";
    }
    else
    {
        m_strClipName = strNewName;
    }
    return S_OK;
}

HRESULT KG3DClip::GetPathName(LPCTSTR *ppszPath)
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(ppszPath);

	*ppszPath = m_scName.c_str();

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DClip::SetPathName(const TCHAR *strPath)
{
	if (!strPath)
	{
		m_scName = "";
	}
	else
	{
		m_scName = strPath;
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////

HRESULT KG3DClip::Clear()
{
    KG_DELETE_ARRAY(m_ppBoneRTS);
    KG_DELETE_ARRAY(m_ppszBoneName);

    if (m_ppBoneMatrixPreCompute)
    {
        for (DWORD i = 0; i < m_dwExpectFrames; i++)
        {
            KG_DELETE_ARRAY(m_ppBoneMatrixPreCompute[i]);
        }
        KG_DELETE_ARRAY(m_ppBoneMatrixPreCompute);
    }

    if (m_ppBoneRTSPreCompute)
    {
        for (DWORD i = 0; i < m_dwExpectFrames; i++)
        {
            KG_DELETE_ARRAY(m_ppBoneRTSPreCompute[i]);
        }
        KG_DELETE_ARRAY(m_ppBoneRTSPreCompute);
    }

    m_bPreInterpolateFlag = false;
    KG_DELETE_ARRAY(m_pbyFileData);
    m_eDynamicLoadState = STATE_NONE;
    return S_OK;
}

HRESULT KG3DClip::LoadFromFileMultiThread()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    unsigned uSizeRead = 0;
    unsigned uFileRemainSize = 0;
    _ANI_FILE_HEADER *pAniFileHeader = NULL;
    IFile *piFile = NULL;
    unsigned uFileSize = 0;

    piFile = g_OpenFile(m_scName.c_str());
    KGLOG_PROCESS_ERROR(piFile);

    uFileSize = piFile->Size();
    KGLOG_PROCESS_ERROR(uFileSize > sizeof(_ANI_FILE_HEADER));

    m_uFileDataSize = uFileSize;

    m_pbyFileData = new BYTE[uFileSize];
    KGLOG_PROCESS_ERROR(m_pbyFileData);

    m_uTotalMemSize += m_uFileDataSize;

    uSizeRead = piFile->Read(m_pbyFileData, uFileSize);
    KGLOG_PROCESS_ERROR(uSizeRead == uFileSize);

    pAniFileHeader = (_ANI_FILE_HEADER *)m_pbyFileData;
    ASSERT(pAniFileHeader);

    KGLOG_PROCESS_ERROR(pAniFileHeader->dwMask == ANI_FILE_MASK);

    m_strClipName = pAniFileHeader->strDesc;

    uFileRemainSize = uFileSize - sizeof(_ANI_FILE_HEADER);

    m_dwType = pAniFileHeader->dwType;
    switch (m_dwType)
    {
    case ANIMATION_BONE_RTS:
        {
            _BONE_ANI *pBoneAni = (_BONE_ANI *)(pAniFileHeader + 1);
            ASSERT(pBoneAni);

            m_dwNumAnimatedBone = pBoneAni->dwNumBones;
            m_dwNumFrames = pBoneAni->dwNumFrames;
            m_fFrameLength = pBoneAni->fFrameLength;
            m_dwAniLen = (DWORD)(m_fFrameLength * (m_dwNumFrames - 1));

            uFileRemainSize -= sizeof(_BONE_ANI);

            KGLOG_PROCESS_ERROR(uFileRemainSize > sizeof(char) * m_dwNumAnimatedBone * MAX_BONENAME_LEN);
            char *pszBoneNameBegin = (char *)(pBoneAni + 1);
            ASSERT(pszBoneNameBegin);
            m_ppszBoneName = new char*[m_dwNumAnimatedBone];
            for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
            {
                m_ppszBoneName[i] = pszBoneNameBegin;
                strlwr(m_ppszBoneName[i]);
                pszBoneNameBegin += MAX_BONENAME_LEN;
            }

            uFileRemainSize -= sizeof(char) * m_dwNumAnimatedBone * MAX_BONENAME_LEN;

            KGLOG_PROCESS_ERROR(uFileRemainSize >= sizeof(RTS) * m_dwNumAnimatedBone * m_dwNumFrames);
            RTS *pBoneRTSBegin = (RTS *)pszBoneNameBegin;
            ASSERT(pBoneRTSBegin);
            m_ppBoneRTS = new RTS*[m_dwNumAnimatedBone];
            for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
            {
                m_ppBoneRTS[i] = pBoneRTSBegin;
                pBoneRTSBegin += m_dwNumFrames;
            }

            m_bPreInterpolateFlag = (g_bClient && g_nAniEnableInterpolate);
            if (m_bPreInterpolateFlag)
            {
                ASSERT(g_nInterpolateFPS > 0);
                float fInterpolateFrameLength = (float)(1000.0 / g_nInterpolateFPS);
                hrRetCode = AllocateMemforPreInterpolate(fInterpolateFrameLength);
                KGLOG_COM_PROCESS_ERROR(hrRetCode);

                //if (m_bInMultiThreadLoadList)
                //{
                //    hrRetCode = PreInterpolate();
                //    KGLOG_COM_PROCESS_ERROR(hrRetCode);
                //}
            }
        }
        break;
    case ANIMATION_VERTICES:
        {
            _VERTEX_ANI *pVertexAni = (_VERTEX_ANI *)(pAniFileHeader + 1);
            ASSERT(pVertexAni);

            m_dwNumVertices = pVertexAni->dwNumVertices;
            m_dwNumAnimatedVertice = pVertexAni->dwNumAnimatedVertices;
            m_dwNumFrames = pVertexAni->dwNumFrames;
            m_fFrameLength = pVertexAni->fFrameLength;
            m_dwAniLen = (DWORD)(m_fFrameLength * (m_dwNumFrames - 1));

            uFileRemainSize -= sizeof(_VERTEX_ANI);

            KGLOG_PROCESS_ERROR(uFileRemainSize > sizeof(DWORD) * m_dwNumAnimatedVertice);
            m_pAnimatedVerticesIndex = (DWORD *)(pVertexAni + 1);
            ASSERT(m_pAnimatedVerticesIndex);

            uFileRemainSize -= sizeof(DWORD) * m_dwNumAnimatedVertice;

            KGLOG_PROCESS_ERROR(uFileRemainSize >= sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames);
            m_pAnimatedVerticesPosition = (D3DXVECTOR3 *)(m_pAnimatedVerticesIndex + m_dwNumAnimatedVertice);
            ASSERT(m_pAnimatedVerticesPosition);

            uFileRemainSize -= sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames;

            if (uFileRemainSize >= sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames)
            {
                m_pAnimationUV = m_pAnimatedVerticesPosition + m_dwNumAnimatedVertice * m_dwNumFrames;
                ASSERT(m_pAnimationUV);

                m_dwOptions |= ENUM_CLIPOP_UVANI;
            }
        }
        break;
    default:
        KGLogPrintf(KGLOG_DEBUG, "Invalid Animation Type %u", m_dwType);
        KG_PROCESS_ERROR(false);
    }

    m_eDynamicLoadState = STATE_READY;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(m_ppBoneRTS);
        KG_DELETE_ARRAY(m_ppszBoneName);
        KG_DELETE_ARRAY(m_pbyFileData);
		m_uTotalMemSize = 0;
        KGLogPrintf(KGLOG_ERR, "Ani file load failed \"%s\"", m_scName.c_str());
    }
    KG_COM_RELEASE(piFile);
    return hrResult;
}

HRESULT KG3DClip::LoadFromFile(const char cszFileName[], unsigned uOption)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwStartTime = timeGetTime();
    //BOOL bMulti = false;

    KGLOG_PROCESS_ERROR(cszFileName);

	m_scName = cszFileName;

	//bMulti = g_bMultiThreadLoadAnimation;//uOption & MLO_ANI_MULTI;

    m_eDynamicLoadState = STATE_NONE;

	if (uOption == MLO_ANI_MULTI)
	{
        //g_cMultiThreadLoader.TryMultiThreadLoad_Clip(this);
		
        hrRetCode = g_ClipPrecompute.InsertClip(this);
        KG_COM_PROCESS_ERROR(hrRetCode);

        m_bInMultiThreadLoadList = TRUE;
	}
	else
	{
        hrRetCode = LoadFromFileMultiThread();
        KG_COM_PROCESS_ERROR(hrRetCode);
	}

	hrResult = S_OK;
Exit0:
	DWORD dwCost = timeGetTime() - dwStartTime;
	if (g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %u KG3DClip::LoadFromFile cost %s", dwCost, cszFileName);
	}
	return hrResult;
}

HRESULT KG3DClip::SaveToFile(const char cszFileName[], unsigned uOption)
{
	HRESULT hrResult = E_FAIL;
	FILE* fpFile = NULL;
	size_t uSize = 0;
    DWORD dwEndFileFlag = ANI_FILE_END_FLAG;
    _ANI_FILE_HEADER AniFileHeader;
	
    KGLOG_PROCESS_ERROR(cszFileName);
    KGLOG_PROCESS_ERROR(!m_bPreInterpolateFlag); // confirm call this function not in client

	fpFile = fopen(cszFileName, "wb");
	KGLOG_PROCESS_ERROR(fpFile);

    AniFileHeader.dwBlockLength = 0;
    AniFileHeader.dwMask = ANI_FILE_MASK;
    AniFileHeader.dwNumAnimations = 1;
    AniFileHeader.dwType = m_dwType;
    memcpy(&AniFileHeader.strDesc, m_strClipName.c_str(), MAX_CLIPDESC_LEN - 1);
    AniFileHeader.strDesc[MAX_CLIPDESC_LEN - 1] = '\0';

    uSize = fwrite(&AniFileHeader, sizeof(_ANI_FILE_HEADER), 1, fpFile);
    KGLOG_PROCESS_ERROR(uSize == 1);

	switch(m_dwType)
	{
	case ANIMATION_BONE_RTS:
		{
            _BONE_ANI BoneRTSAni;
            BoneRTSAni.dwNumBones = m_dwNumAnimatedBone;
            BoneRTSAni.dwNumFrames = m_dwNumFrames;
            BoneRTSAni.fFrameLength = m_fFrameLength;
            uSize = fwrite(&BoneRTSAni, sizeof(_BONE_ANI), 1, fpFile);
            KGLOG_PROCESS_ERROR(uSize == 1);

            for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
            {
                uSize = fwrite(m_ppszBoneName[i], sizeof(char) * MAX_BONENAME_LEN, 1, fpFile);
                KGLOG_PROCESS_ERROR(uSize == 1);
            }

            for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
            {
                uSize = fwrite(m_ppBoneRTS[i], sizeof(RTS) * m_dwNumFrames, 1, fpFile);
                KGLOG_PROCESS_ERROR(uSize == 1);
            }

			break;
		}
	case ANIMATION_VERTICES:
		{
            _VERTEX_ANI VertexAni;
            VertexAni.dwNumVertices = m_dwNumVertices;
            VertexAni.dwNumAnimatedVertices = m_dwNumAnimatedVertice;
            VertexAni.dwNumFrames = m_dwNumFrames;
            VertexAni.fFrameLength = m_fFrameLength;
            uSize = fwrite(&VertexAni, sizeof(_VERTEX_ANI), 1, fpFile);
            KGLOG_PROCESS_ERROR(uSize == 1);

            uSize = fwrite(m_pAnimatedVerticesIndex, sizeof(DWORD) * m_dwNumAnimatedVertice, 1, fpFile);
            KGLOG_PROCESS_ERROR(uSize == 1);

            uSize = fwrite(m_pAnimatedVerticesPosition, sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames, 1, fpFile);
            KGLOG_PROCESS_ERROR(uSize == 1);

			if (m_dwOptions & ENUM_CLIPOP_UVANI)
			{
				//Save uv animation
				DWORD dwUVAnimationSize = sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames;
				uSize = fwrite(m_pAnimationUV, dwUVAnimationSize, 1, fpFile);
                KGLOG_PROCESS_ERROR(uSize == 1);
			}
			break;
		}
	}

	uSize = fwrite(&dwEndFileFlag, sizeof(DWORD), 1, fpFile);
    KGLOG_PROCESS_ERROR(uSize == 1);

	hrResult = S_OK;
Exit0:
	if (fpFile)
	{
		fclose(fpFile);
        fpFile = NULL;
	}
	return hrResult;
}

ULONG STDMETHODCALLTYPE KG3DClip::AddRef()
{
	return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KG3DClip::Release()
{
    ULONG ulNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (ulNewRefCount == 0)
        delete this;
    return ulNewRefCount;
}

HRESULT STDMETHODCALLTYPE KG3DClip::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return S_OK;
}

HRESULT KG3DClip::UpdateRTSRealTime(
    UpdateAnimationInfo *pAnimationInfo, 
    DWORD dwFrame, DWORD dwFrameTo, float fWeight, 
    KG3DAnimationUpdateExtraInfoCollector *pExtraInfo
)
{
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_ERROR(pAnimationInfo);

    if (!pExtraInfo)
    {
        ASSERT(pAnimationInfo->pMatrix);
        for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
        {
            RTS rts;
            InterpolateRTSKeyFrame(
                rts, 
                m_ppBoneRTS[i][dwFrame], 
                m_ppBoneRTS[i][dwFrameTo], 
                fWeight
            );
            RTS2Matrix(pAnimationInfo->pMatrix[i], rts);
        }
    }
    else
    {
        for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
        {
            RTS rts;
            InterpolateRTSKeyFrame(
                rts, 
                m_ppBoneRTS[i][dwFrame], 
                m_ppBoneRTS[i][dwFrameTo], 
                fWeight
            );
            pExtraInfo->SaveData(i, &rts);
        }
    }

    pAnimationInfo->bNeedUpdateToWorld = TRUE;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DClip::UpdateRTSPre(
    UpdateAnimationInfo *pAnimationInfo, 
    DWORD dwFrame, DWORD dwFrameTo, float fWeight, 
    KG3DAnimationUpdateExtraInfoCollector *pExtraInfo
)
{
    HRESULT hrResult = E_FAIL;
    int nNewMatrixFlag = false;
    int nNewRTSFlag = false;
    DWORD dwPreComputeFrame = 0;
	
	ASSERT(
		(dwFrameTo == dwFrame + 1) ||
		(dwFrameTo == dwFrame) 	   ||
		((dwFrame == m_dwNumFrames - 1) && (dwFrameTo == 0))
	);

    KG_PROCESS_ERROR(pAnimationInfo);

    dwPreComputeFrame = dwFrame;
    if (m_dwInterpolateFrames > 0)
    {
        if (dwFrame == dwFrameTo)
            dwPreComputeFrame = dwFrame * m_dwInterpolateFrames;
        else if ((dwFrame == m_dwNumFrames - 1) && (fWeight > 0.5))
            dwPreComputeFrame = 0;
        else
            dwPreComputeFrame = dwFrame * m_dwInterpolateFrames + (DWORD)(fWeight * m_dwInterpolateFrames);
    }

    if (!pExtraInfo)
    {
        if (!m_ppBoneMatrixPreCompute[dwPreComputeFrame])
        {
            m_ppBoneMatrixPreCompute[dwPreComputeFrame] = new D3DXMATRIX[m_dwNumAnimatedBone];
            KGLOG_PROCESS_ERROR(m_ppBoneMatrixPreCompute[dwPreComputeFrame]);
            nNewMatrixFlag = true;

            m_uTotalMemSize += sizeof(D3DXMATRIX) * m_dwNumAnimatedBone;

            for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
            {
                RTS rts;
                InterpolateRTSKeyFrame(
                    rts, 
                    m_ppBoneRTS[i][dwFrame], 
                    m_ppBoneRTS[i][dwFrameTo], 
                    fWeight
                );
                RTS2Matrix(m_ppBoneMatrixPreCompute[dwPreComputeFrame][i], rts);
            }
        }
        memcpy(pAnimationInfo->pMatrix, m_ppBoneMatrixPreCompute[dwPreComputeFrame], sizeof(D3DXMATRIX) * m_dwNumAnimatedBone);
    }
    else
    {
        if (!m_ppBoneRTSPreCompute)
        {
            for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
            {
                pExtraInfo->SaveData(i, &m_ppBoneRTS[i][dwPreComputeFrame]);
            }
        }
        else 
        {
            if (!m_ppBoneRTSPreCompute[dwPreComputeFrame])
            {
                m_ppBoneRTSPreCompute[dwPreComputeFrame] = new RTS[m_dwNumAnimatedBone];
                KGLOG_PROCESS_ERROR(m_ppBoneRTSPreCompute[dwPreComputeFrame]);
                nNewRTSFlag = true;

                m_uTotalMemSize += sizeof(RTS) * m_dwNumAnimatedBone;

                for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
                {
                    InterpolateRTSKeyFrame(
                        m_ppBoneRTSPreCompute[dwPreComputeFrame][i], 
                        m_ppBoneRTS[i][dwFrame], 
                        m_ppBoneRTS[i][dwFrameTo], 
                        fWeight
                    );
                }
            }
            pExtraInfo->SetData(m_ppBoneRTSPreCompute[dwPreComputeFrame], m_dwNumAnimatedBone);
        }
    }

    pAnimationInfo->bNeedUpdateToWorld = TRUE;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nNewRTSFlag)
        {
            KG_DELETE_ARRAY(m_ppBoneRTSPreCompute[dwPreComputeFrame]);
            nNewRTSFlag = false;
        }
        if (nNewMatrixFlag)
        {
            KG_DELETE_ARRAY(m_ppBoneMatrixPreCompute[dwPreComputeFrame]);
            nNewMatrixFlag = false;
        }
    }
    return hrResult;
}

HRESULT KG3DClip::UpdateVertices(
    UpdateAnimationInfo *pAnimationInfo, DWORD dwFrame, DWORD dwFrameTo, float fWeight
)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	VFormat::FACES_NORMAL_TEXTURE1* pVers = NULL;
	VFormat::FACES_NORMAL_TEXTURE1* pVersSrc = NULL;
	VFormat::FACES_NORMAL_TEXTURE2* pVersSrc1 = NULL;
	VFormat::FACES_NORMAL_TEXTURE2* pVers1 = NULL;
	float fReverse = 1.0f - fWeight;
	DWORD dwFVF = 0;
    DWORD dwNumVertice = 0;
	LPD3DXMESH pSrcMesh = NULL;
    BOOL bLockMeshSrcVertexBuffer = FALSE;
    BOOL bLockMeshVertexBuffer = FALSE;

    KG_PROCESS_ERROR(pAnimationInfo);

    pSrcMesh = pAnimationInfo->pMeshSrc;
    KG_PROCESS_ERROR(pSrcMesh);

    dwNumVertice = pSrcMesh->GetNumVertices();
	KG_PROCESS_ERROR(dwNumVertice == m_dwNumAnimatedVertice);

	dwFVF = pSrcMesh->GetFVF();

	if (!pAnimationInfo->pMesh)
	{
		DWORD dwOption = pSrcMesh->GetOptions();

		hrRetCode = pSrcMesh->CloneMeshFVF(dwOption & (~D3DXMESH_MANAGED) | D3DXMESH_VB_DYNAMIC, 
			dwFVF,
			g_pd3dDevice, 
			&(pAnimationInfo->pMesh));
		KG_COM_PROCESS_ERROR(hrRetCode);
	}

	if (dwFVF & D3DFVF_TEX2)
	{
		hrRetCode = pAnimationInfo->pMeshSrc->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVersSrc1);
		KG_COM_PROCESS_ERROR(hrRetCode);
		
        bLockMeshSrcVertexBuffer = TRUE;

		hrRetCode = pAnimationInfo->pMesh->LockVertexBuffer(D3DLOCK_DISCARD, (void**)&pVers1);
		KG_COM_PROCESS_ERROR(hrRetCode);

        bLockMeshVertexBuffer = TRUE;

		//Update vertices position data
		for (DWORD i = 0; i < m_dwNumAnimatedVertice; i++)
		{
			DWORD dwIndex = m_pAnimatedVerticesIndex[i];
			D3DXVECTOR3& PosA = m_pAnimatedVerticesPosition[i * m_dwNumFrames + dwFrame];
			D3DXVECTOR3& PosB = m_pAnimatedVerticesPosition[i * m_dwNumFrames + dwFrameTo];
            D3DXVec3Lerp(&pVers1[dwIndex].p, &PosA, &PosB, fWeight);
			pVers1[dwIndex].Normal = pVersSrc1[dwIndex].Normal;
			pVers1[dwIndex].tu2 = pVersSrc1[dwIndex].tu2;
			pVers1[dwIndex].tv2 = pVersSrc1[dwIndex].tv2;
			pVers1[dwIndex].tu1 = pVersSrc1[dwIndex].tu1;
			pVers1[dwIndex].tv1 = pVersSrc1[dwIndex].tv1;
		}

		if(m_dwOptions & ENUM_CLIPOP_UVANI)
		{
			//Update vertices uv data
			for (DWORD i = 0; i < m_dwNumAnimatedVertice; i++)
			{
				DWORD dwIndex = m_pAnimatedVerticesIndex[i];
				D3DXVECTOR3& UVA = m_pAnimationUV[i * m_dwNumFrames + dwFrame];
				D3DXVECTOR3& UVB = m_pAnimationUV[i * m_dwNumFrames + dwFrameTo];

				pVers1[dwIndex].tu1 = UVA.x * fReverse + UVB.x * fWeight;
				pVers1[dwIndex].tv1 = UVA.y * fReverse + UVB.y * fWeight;
			}
		}
	}
	else
	{
		hrRetCode = pAnimationInfo->pMeshSrc->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVersSrc);
		KG_COM_PROCESS_ERROR(hrRetCode);

        bLockMeshSrcVertexBuffer = TRUE;

		hrRetCode = pAnimationInfo->pMesh->LockVertexBuffer(D3DLOCK_DISCARD, (void**)&pVers);
		KG_COM_PROCESS_ERROR(hrRetCode);

        bLockMeshVertexBuffer = TRUE;

		//Update vertices position data
		for (DWORD i = 0; i < m_dwNumAnimatedVertice; i++)
		{
			DWORD dwIndex = m_pAnimatedVerticesIndex[i];
			D3DXVECTOR3& PosA = m_pAnimatedVerticesPosition[i * m_dwNumFrames + dwFrame];
			D3DXVECTOR3& PosB = m_pAnimatedVerticesPosition[i * m_dwNumFrames + dwFrameTo];
            D3DXVec3Lerp(&pVers[dwIndex].p, &PosA, &PosB, fWeight);
			pVers[dwIndex].Normal = pVersSrc[dwIndex].Normal;
			pVers[dwIndex].tu1 = pVersSrc[dwIndex].tu1;
			pVers[dwIndex].tv1 = pVersSrc[dwIndex].tv1;
		}

		if (m_dwOptions & ENUM_CLIPOP_UVANI)
		{
			//Update vertices uv data
			for (DWORD i = 0; i < m_dwNumAnimatedVertice; i++)
			{
				DWORD dwIndex = m_pAnimatedVerticesIndex[i];
				D3DXVECTOR3& UVA = m_pAnimationUV[i * m_dwNumFrames + dwFrame];
				D3DXVECTOR3& UVB = m_pAnimationUV[i * m_dwNumFrames + dwFrameTo];

				pVers[dwIndex].tu1 = UVA.x * fReverse + UVB.x * fWeight;
				pVers[dwIndex].tv1 = UVA.y * fReverse + UVB.y * fWeight;
			}
		}
	}

    hrResult = S_OK;
Exit0:
	if (bLockMeshVertexBuffer)
	{
		pAnimationInfo->pMesh->UnlockVertexBuffer();
	}
	if (bLockMeshSrcVertexBuffer)
	{
		pAnimationInfo->pMeshSrc->UnlockVertexBuffer();
	}
	return hrResult;
}

HRESULT KG3DClip::UpdateClip(
    UpdateAnimationInfo *pAnimationInfo, 
    KG3DAnimationUpdateExtraInfoCollector *pExtraInfo, 
    DWORD dwFrame, 
    DWORD dwFrameTo, 
    float fWeight
)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    KG_PROCESS_ERROR(pAnimationInfo);

    switch (m_dwType)
    {
    case ANIMATION_VERTICES:
        hrRetCode = UpdateVertices(pAnimationInfo, dwFrame, dwFrameTo, fWeight);
        KG_COM_PROCESS_ERROR(hrRetCode);
        break;
    case ANIMATION_BONE_RTS:
        if (m_bPreInterpolateFlag)
        {
            hrRetCode = UpdateRTSPre(pAnimationInfo, dwFrame, dwFrameTo, fWeight, pExtraInfo);
            KG_COM_PROCESS_ERROR(hrRetCode);
        }
        else
        {
            hrRetCode = UpdateRTSRealTime(pAnimationInfo, dwFrame, dwFrameTo, fWeight, pExtraInfo);
            KG_COM_PROCESS_ERROR(hrRetCode);
        }
        break;
    default:
        ASSERT(!"Invalid Animation Type");
    }

    m_dwLastUseTime = g_cGraphicsTool.GetNowTime();

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DClip::GetCurFrame(
    DWORD dwAniPlayLen, DWORD dwPlayType, 
    DWORD *pdwFrame, DWORD *pdwFrameTo, float *pfWeight
)
{
    HRESULT hrResult = E_FAIL;
    DWORD dwFrame = 0;
    DWORD dwFrameTo = 0;
    float fWeight = 0;

    switch (dwPlayType)
    {
    case ENU_ANIMATIONPLAY_CIRCLE:
        {
            DWORD dwSpanTime = dwAniPlayLen % m_dwAniLen;
            dwFrame = (DWORD)(dwAniPlayLen % m_dwAniLen / m_fFrameLength);
            dwFrameTo = (dwFrame + 1 >= m_dwNumFrames) ? 0 : (dwFrame + 1);
            fWeight = (dwSpanTime -  dwFrame * m_fFrameLength) / m_fFrameLength;
            break;
        }
    case ENU_ANIMATIONPLAY_ONCE:
        {
            DWORD dwRepeatTimes = dwAniPlayLen / m_dwAniLen;
            if (dwRepeatTimes >= 1)
            {
                dwFrame = m_dwNumFrames - 1;
                dwFrameTo = dwFrame;
                fWeight = 1.0f;
            }
            else
            {
                dwFrame = (DWORD)(dwAniPlayLen / m_fFrameLength);
                dwFrameTo = dwFrame + 1;
                fWeight = (dwAniPlayLen -  dwFrame * m_fFrameLength) / m_fFrameLength;
            }
            break;
        }
    default:
        ASSERT(!"Invalid Ani Play Type");
        goto Exit0;
    }

    *pdwFrame = dwFrame;
    *pdwFrameTo = dwFrameTo;
    *pfWeight = fWeight;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KG3DClip::FindBone(const TCHAR* strBoneName)
{
	for (int i = 0; i < (int)m_dwNumAnimatedBone; i++)
	{
		if (!strcmp(m_ppszBoneName[i], strBoneName))
		{
			return i;
		}
	}
	return -1;
}

void KG3DClip::SwapBoneAnimationData(int nBone1, int nBone2)
{
    char *pszBoneNameTemp = NULL;
    RTS *pRtsTemp = NULL;
    D3DXMATRIX matTemp;
    RTS rtsTemp;

	KGLOG_PROCESS_ERROR(nBone1 != nBone2);
	
	switch (m_dwType)
	{
	case ANIMATION_BONE_RTS:
		{
            pRtsTemp = m_ppBoneRTS[nBone1];
            m_ppBoneRTS[nBone1] = m_ppBoneRTS[nBone2];
            m_ppBoneRTS[nBone2] = pRtsTemp;

            pszBoneNameTemp = m_ppszBoneName[nBone1];
            m_ppszBoneName[nBone1] = m_ppszBoneName[nBone2];
            m_ppszBoneName[nBone2] = pszBoneNameTemp;

            if (m_bPreInterpolateFlag)
            {
                for (DWORD i = 0; i < m_dwExpectFrames; i++)
                {
                    if (!m_ppBoneMatrixPreCompute[i])
                        continue;

                    matTemp = m_ppBoneMatrixPreCompute[i][nBone1];
                    m_ppBoneMatrixPreCompute[i][nBone1] = m_ppBoneMatrixPreCompute[i][nBone2];;
                    m_ppBoneMatrixPreCompute[i][nBone2] = matTemp;
                }

                if (m_ppBoneRTSPreCompute)
                {
                    for (DWORD i = 0; i < m_dwExpectFrames; i++)
                    {
                        if (!m_ppBoneRTSPreCompute[i])
                            continue;

                        rtsTemp = m_ppBoneRTSPreCompute[i][nBone1];
                        m_ppBoneRTSPreCompute[i][nBone1] = m_ppBoneRTSPreCompute[i][nBone2];;
                        m_ppBoneRTSPreCompute[i][nBone2] = rtsTemp;
                    }
                }
            }
		}
		break;
    default:
        ASSERT(!"Invalid Animation Type");
        goto Exit0;
	}

Exit0:
	return;
}

int KG3DClip::GetMatrixsRange(
    vector<D3DXMATRIX>& vecMatrixs,
    INT    nBoneIndex,
    FLOAT  fBeginFrame, 
    FLOAT  fEndFrame, 
    FLOAT  fPrecision
)
{
    HRESULT hResult  = E_FAIL;

    ASSERT(fPrecision >= 0.0f);

    FLOAT fIter = fBeginFrame + fPrecision;
  
    if (fIter > fEndFrame)
        fIter = fEndFrame;

    while (fIter <= fEndFrame)
    {
        INT   nFrame   = static_cast<INT>(fIter);
        INT   nFrameTo = nFrame + 1;
        FLOAT fWeight  = fIter - nFrame;

		if (nFrame >= static_cast<int>(m_dwNumFrames))
			nFrame = 0;

		if (nFrameTo >= static_cast<int>(m_dwNumFrames))
            nFrameTo = 0;

        D3DXMATRIX matrix;
        RTS rts;
     
        InterpolateRTSKeyFrame(
            rts, 
            m_ppBoneRTS[nBoneIndex][nFrame], 
            m_ppBoneRTS[nBoneIndex][nFrameTo], 
            fWeight
        );
        RTS2Matrix(matrix, rts);
        vecMatrixs.push_back(matrix);

        fIter += fPrecision;
    }

    if (fIter > fEndFrame && fIter < fEndFrame + fPrecision)
    {
        fIter = fEndFrame;

        INT   nFrame   = static_cast<INT>(fIter);
        INT   nFrameTo = nFrame + 1;
        FLOAT fWeight  = fIter - nFrame;

		if (nFrame >= static_cast<int>(m_dwNumFrames))
			nFrame = 0;

		if (nFrameTo >= static_cast<int>(m_dwNumFrames))
            nFrameTo = 0;

        D3DXMATRIX matrix;
        RTS rts;

        InterpolateRTSKeyFrame(
            rts, 
            m_ppBoneRTS[nBoneIndex][nFrame], 
            m_ppBoneRTS[nBoneIndex][nFrameTo], 
            fWeight
        );
        RTS2Matrix(matrix, rts);
        vecMatrixs.push_back(matrix);
    }
    hResult = S_OK;
//Exit0:
    return (int)vecMatrixs.size();
}

DWORD KG3DClip::ConvertFrameToTime(UINT uFrameIndex)
{
    DWORD dwTime = 0;
    KG_PROCESS_ERROR(uFrameIndex < m_dwNumFrames);

    dwTime = (DWORD)(m_fFrameLength * uFrameIndex);
Exit0:
    return dwTime;
}

HRESULT KG3DClip::AllocateMemforPreInterpolate(float fInterpolateFrameLength)
{
    HRESULT hrResult = E_FAIL;

    m_dwExpectFrames = m_dwNumFrames;
    if (m_fFrameLength > fInterpolateFrameLength)
    {
        m_dwInterpolateFrames = (DWORD)(m_fFrameLength / fInterpolateFrameLength) + 1;
        m_dwExpectFrames = (m_dwNumFrames - 1) * m_dwInterpolateFrames + 1;

        m_ppBoneRTSPreCompute = new RTS*[m_dwExpectFrames];
        KGLOG_PROCESS_ERROR(m_ppBoneRTSPreCompute);
        memset(m_ppBoneRTSPreCompute, 0, sizeof(RTS *) * m_dwExpectFrames);

        m_uTotalMemSize += sizeof(RTS *) * m_dwExpectFrames;
    }

    m_ppBoneMatrixPreCompute = new D3DXMATRIX*[m_dwExpectFrames];
    KGLOG_PROCESS_ERROR(m_ppBoneMatrixPreCompute);
    memset(m_ppBoneMatrixPreCompute, 0, sizeof(D3DXMATRIX *) * m_dwExpectFrames);

    m_uTotalMemSize += sizeof(D3DXMATRIX *) * m_dwExpectFrames;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(m_ppBoneRTSPreCompute);
        KG_DELETE_ARRAY(m_ppBoneMatrixPreCompute);
    }
    return hrResult;
}

HRESULT KG3DClip::PreInterpolate()
{
    HRESULT hrResult = E_FAIL;

    for (DWORD i = 0; i < m_dwExpectFrames; i++)
    {
        m_ppBoneMatrixPreCompute[i] = new D3DXMATRIX[m_dwNumAnimatedBone];
        KGLOG_PROCESS_ERROR(m_ppBoneMatrixPreCompute[i]);
    }

    if (m_ppBoneRTSPreCompute)
    {
        for (DWORD i = 0; i < m_dwExpectFrames; i++)
        {
            m_ppBoneRTSPreCompute[i] = new RTS[m_dwNumAnimatedBone];
            KGLOG_PROCESS_ERROR(m_ppBoneRTSPreCompute[i]);
        }

        float fWeight = (float)(1.0 / m_dwInterpolateFrames);
        DWORD t = 0;
        for (DWORD i = 0; i < m_dwNumFrames; i++)
        {
            for (DWORD j = 0; j < m_dwInterpolateFrames; j++)
            {
                if (t >= m_dwExpectFrames)
                    break;

                for (DWORD k = 0; k < m_dwNumAnimatedBone; k++)
                {
                    DWORD dwA = i;
                    DWORD dwB = (i + 1 >= m_dwNumFrames) ? 0 : (i + 1);

                    InterpolateRTSKeyFrame(
                        m_ppBoneRTSPreCompute[t][k], 
                        m_ppBoneRTS[k][dwA], 
                        m_ppBoneRTS[k][dwB],
                        j * fWeight
                    );

                    RTS2Matrix(m_ppBoneMatrixPreCompute[t][k], m_ppBoneRTSPreCompute[t][k]);
                }
                t++;
            }
        }
    }
    else
    {
        for (DWORD i = 0; i < m_dwNumFrames; i++)
        {
            for (DWORD j = 0; j < m_dwNumAnimatedBone; j++)
            {
                RTS2Matrix(m_ppBoneMatrixPreCompute[i][j], m_ppBoneRTS[j][i]);
            }
        }
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (m_ppBoneRTSPreCompute)
        {
            for (DWORD i = 0; i < m_dwExpectFrames; i++)
            {
                KG_DELETE_ARRAY(m_ppBoneRTSPreCompute[i]);
            }
        }
        for (DWORD i = 0; i < m_dwExpectFrames; i++)
        {
            KG_DELETE_ARRAY(m_ppBoneMatrixPreCompute[i]);
        }
    }
    return hrResult;
}
