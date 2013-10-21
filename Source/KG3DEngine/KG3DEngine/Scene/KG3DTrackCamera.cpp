////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DTrackCamera.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-20 16:35:52
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DEngineManager.h"
#include "KG3DTrackCamera.h"
#include "KG3DScene.h"
#include "KG3DGraphicsTool.h"
#include "KG3DRepresentNPC.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DIntersection.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 粗略地两个方向向量是否是同一方向
__forceinline BOOL rude_same_dir(D3DXVECTOR3 v1, D3DXVECTOR3 v2, float fAngel = D3DX_PI / 180)
{
    float fDot = D3DXVec3Dot(&v1, &v2);
    if (fDot >  1.0f)
        fDot =  1.0f;
    if (fDot < -1.0f)
        fDot = -1.0f;
    float fAng = acosf(fDot);

    if (fAng <= fAngel)
        return TRUE;
    else
        return FALSE;
}

// 粗略地判断两个点是否在同一位置
__forceinline BOOL rude_same_pos(D3DXVECTOR3 v1, D3DXVECTOR3 v2, FLOAT precision = 0.5f)
{
    if (D3DXVec3Length(&(v2 - v1)) <= precision)
        return TRUE;
    else
        return FALSE;
}

// 得到相机前截面的四个角
__forceinline void GetFourSquare(KG3DCamera*  pCamera,
                                 D3DXVECTOR3& vCurPosition,
                                 float        fzNear,
                                 float        fFovY,
                                 float        fAspect,
                                 D3DXVECTOR3& vLeftUp, 
                                 D3DXVECTOR3& vRightUp,
                                 D3DXVECTOR3& vRightBottom,
                                 D3DXVECTOR3& vLeftBottom 
                                 )
{
    ASSERT(pCamera);

    D3DXVECTOR3 vFront  = pCamera->GetCameraFront();
    D3DXVECTOR3 vUp     = pCamera->GetCameraUp();
    D3DXVECTOR3 vRight  = pCamera->GetCameraRight();
    float   fAngelHalf  = fFovY / 2.f;

    D3DXVec3Normalize(&vFront, &vFront);
    D3DXVec3Normalize(&vUp,    &vUp);
    D3DXVec3Normalize(&vRight, &vRight);

    D3DXVECTOR3 vCenter = vCurPosition + vFront * fzNear;
    float fVelDis       = fabs(tanf(fAngelHalf) * fzNear);
    float fHovDis       = fabs(fVelDis * fAspect);

    vLeftUp      = vCenter + vUp * fVelDis - vRight * fHovDis;
    vRightUp     = vCenter + vUp * fVelDis + vRight * fHovDis;
    vLeftBottom  = vCenter - vUp * fVelDis - vRight * fHovDis;
    vRightBottom = vCenter - vUp * fVelDis + vRight * fHovDis;

}

KG3DTrackCamera::KG3DTrackCamera(void) 
{
/*	if (g_cRecorder.IsRecord())
	{	
		m_dwRecordID =  KG3DRecorder::GetMasterID();
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 0;
		MyParameters.size_tDataSize = 0;
		MyParameters.pData = NULL;
		g_cRecorder.RecordEvent(m_dwRecordID,"KG3DTrackCamera(void)",GetCurrentTime(),MyParameters);
	}  */ 
	m_lpCurScene        = NULL;
    
    m_vecCurPostion     = D3DXVECTOR3(0.0f, 500.0f, 1000.0f);
    m_vecPerfectPos     = D3DXVECTOR3(0.0f, 500.0f, 1000.0f);
    m_vecTagPostion     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vecPrevTagPos     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vecLookAtPos      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vecCurVelocity    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    //
    m_vecPositionSave   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);       
    m_vecAxesPosSave    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    //

    m_trackInfo.fFlexCoefficient  = 1.5f;
    m_trackInfo.fDampCoefficient  = sqrtf(1.5f) * 2;

    m_fDisPreTarger     = 0.0f;
    //m_trackInfo.fFlexRate  = 500.0f;
    m_fFlexOffset       = 0.0f;
    m_trackInfo.nUseObstructdAvert = true;      
    m_trackInfo.nUseFlexibilitySys = true;
    m_nUseFlexFlag      = false;
    m_nUpdateFlag       = true;         
    m_nObstrucFlag      = false;
    m_dwState           = CAMERA_IMMOBILEING;
	m_fStartVelocity    = 100.f;

    //
	m_vecTagDirectn     = D3DXVECTOR3(0.0f, 0.0f, 0.5f);
    m_fDistance         = 1000.0f;
    m_fAngleHor         = 0.0f;
    m_fAngleVel         = D3DX_PI / 8;
    m_fUpOffset	        = 0.0f;
	m_fAnglVelocity     = 0.0f;

	m_nRegressFlag      = true;
	m_nObstrucFlag      = false;
	m_nObTerrainFg      = false;

    m_fAnVarHor         = 0.0f;
    m_fAnVarVel         = 0.0f;
    m_fVelGlide         = m_fAngleVel;
    m_fZNearSave        = 10.f;

    m_cameraStation.pCamera = this;

	m_StationHelper.Init(&m_cameraStation);//by dengzhihui Refactor 2006年11月6日
    //

}

KG3DTrackCamera::~KG3DTrackCamera(void)
{
#if defined(DEBUG) | defined(_DEBUG)
	m_bPerspective = 0x12345678;//用来检验释放的代码，不要删除
#endif
}

HRESULT KG3DTrackCamera::BindScene(KG3DScene* lpCurScene)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(lpCurScene);
    m_lpCurScene = lpCurScene;

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT KG3DTrackCamera::FrameMove()
{
	if (!m_trackInfo.bEnableTrack)
		return S_OK;

    HRESULT hResult  = E_FAIL;

    D3DXVECTOR3 vPrevTagToCurPos  = m_vecPrevTagPos - m_vecCurPostion;     // 上一帧从位置到目标
    D3DXVECTOR3 vTagToPerfectPos  = m_vecTagPostion - m_vecPerfectPos;     // 当前帧从位置到目标
    D3DXVECTOR3 vecPrevPos;

    D3DXVECTOR3 vecSaveCurPos = m_vecCurPostion;                       // 备份上一政的位置

    float fPrevCurTag = D3DXVec3Length(&vPrevTagToCurPos) + m_fFlexOffset;

    float fDelayTime = g_cGraphicsTool.GetTinyTime() / 1000.0f;
    static float fSaveDelay = fDelayTime;
    if (fDelayTime == 0.0f) 
        fDelayTime = fSaveDelay; 
    else
        fSaveDelay = fDelayTime;

    m_dwState = m_dwState & ~CAMERA_FLEX_MOVING;
    m_dwState = m_dwState & ~CAMERA_LINE_MOVING;
    m_dwState = m_dwState |  CAMERA_IMMOBILEING;

//    KG_PROCESS_SUCCESS(m_vecPerfectPos == m_vecCurPostion);   // 录像模式镜头会出问题，具体原因质询黄蕾
    KG_PROCESS_SUCCESS(!m_nUpdateFlag);

    D3DXVec3Normalize(&vTagToPerfectPos, &vTagToPerfectPos);
    m_vecCurPostion = m_vecTagPostion - vTagToPerfectPos * fPrevCurTag;

    m_fFlexOffset = 0.0f;
   

    // -- 移动Camera到期待的位置
    if (m_nUseFlexFlag && m_trackInfo.nUseFlexibilitySys)
    {
        // 弹性系统
        D3DXVECTOR3 vecFlexIAccel;  // 弦加速度
        D3DXVECTOR3 vecDirPreCurr;  // 线偏移值
        D3DXVECTOR3 vecUpdatePos ;
        D3DXVECTOR3 vecDirPreUpde;


        vecDirPreCurr  = m_vecCurPostion - m_vecPerfectPos;


        // 弦加速度 = (-弹性系数 * 线偏移值) - (阻尼系数 * 当前速度)
        vecFlexIAccel  = 
            (-m_trackInfo.fFlexCoefficient * vecDirPreCurr) - (m_trackInfo.fDampCoefficient * m_vecCurVelocity);
       
        m_vecCurVelocity = m_vecCurVelocity + vecFlexIAccel * fDelayTime;

        vecUpdatePos     = m_vecCurPostion + m_vecCurVelocity * fDelayTime;

        vecDirPreUpde    = vecUpdatePos - m_vecPerfectPos;

        D3DXVec3Normalize(&vecDirPreCurr, &vecDirPreCurr);
        D3DXVec3Normalize(&vecDirPreUpde, &vecDirPreUpde);

        if (rude_same_dir(vecDirPreCurr, vecDirPreUpde, 0.05f))
        {
            m_vecCurPostion = vecUpdatePos;
        }
        else
        {
            m_vecCurPostion = m_vecPerfectPos;
            m_nUseFlexFlag  = false;
            m_nUpdateFlag   = false;
        }

        m_dwState = m_dwState |  CAMERA_FLEX_MOVING;
        m_dwState = m_dwState & ~CAMERA_LINE_MOVING;
        m_dwState = m_dwState & ~CAMERA_IMMOBILEING;
        m_dwState = m_dwState & ~CAMERA_OBSTRUCTDED;
    }
    else
    {
        m_vecCurPostion = m_vecPerfectPos;
        m_nUpdateFlag   = false;
        
        m_dwState = m_dwState |  CAMERA_LINE_MOVING;
        m_dwState = m_dwState & ~CAMERA_FLEX_MOVING;
        m_dwState = m_dwState & ~CAMERA_IMMOBILEING;
        m_dwState = m_dwState & ~CAMERA_OBSTRUCTDED;
    }

    // -- 遮档检测
    if (m_trackInfo.nUseObstructdAvert)
    {
        D3DXVECTOR3 vecSaved = m_vecPerfectPos;
        D3DXVECTOR3 vecCheck = m_vecPerfectPos;

        ObstructdAvert(&vecCheck, &m_vecTagPostion);

        int nObs = false;

        if (vecSaved != vecCheck)
        {
            D3DXVECTOR3 vecDirCheck = m_vecTagPostion - vecCheck;
            D3DXVECTOR3 vecCurToTag = m_vecTagPostion - m_vecCurPostion;

            if (D3DXVec3LengthSq(&vecDirCheck) <= D3DXVec3LengthSq(&vecCurToTag))
            {
                nObs = true;
            }
            else //if (!rude_same_pos(m_vecPrevTagPos, m_vecTagPostion))
            {
                float fCmpDis = 50.f;

                if (m_nObstrucFlag)
                    fCmpDis = 100.f;

                float fDis = D3DXVec3LengthSq(&(vecSaveCurPos - vecCheck));

                if (fDis < fCmpDis * fCmpDis)
                    nObs = true;
                else
                    nObs = false;
              
            }
            /*else
            {
                nObs = false;
            }*/

            D3DXVec3Normalize(&vecDirCheck, &vecDirCheck);
            vecCheck = vecCheck + vecDirCheck * 18.f;

        }

        if (nObs)
        {
            m_vecCurPostion  = vecCheck;
            m_vecCurVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            m_nUseFlexFlag   = true;
            m_nObstrucFlag   = true;
            m_nUpdateFlag    = false;

            m_dwState = m_dwState | CAMERA_IMMOBILEING;
            m_dwState = m_dwState | CAMERA_OBSTRUCTDED;
        }
        else
        {
            m_nObstrucFlag   = false; 
            m_dwState = m_dwState & ~CAMERA_OBSTRUCTDED;
        }
    }

    m_vecPrevTagPos = m_vecTagPostion; 

    // m_fUpOffset 模拟客户端用 : Camera 往上抬的长度
    SetLookAtPosition(m_vecLookAtPos + D3DXVECTOR3(0.0f, m_fUpOffset, 0.0f));
    SetUpDirection(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
    GetPosition(&vecPrevPos);

    // 避免抖动
    //if (!rude_same_pos(vecPrevPos, m_vecCurPostion, 5.f))
        SetPosition(m_vecCurPostion);

Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT KG3DTrackCamera::UpdateCamera(  
    D3DXVECTOR3 vecPosition, D3DXVECTOR3 vecTargetPos, float fLookAtOffset /*= 100.0f*/, float fTargetMoveSpeed
)
{
  
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 4;
		MyParameters.size_tDataSize = sizeof(D3DXVECTOR3) * 2 + sizeof(float) * 2; 			
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(D3DXVECTOR3*)(pData) = vecPosition;
		MyParameters.nFrom[1] = sizeof(D3DXVECTOR3);
		*(D3DXVECTOR3*)(&pData[MyParameters.nFrom[1]]) = vecTargetPos;
		MyParameters.nFrom[2] = sizeof(D3DXVECTOR3) * 2;
		*(float*)(&pData[MyParameters.nFrom[2]]) = fLookAtOffset;
		MyParameters.nFrom[3] = sizeof(D3DXVECTOR3) * 2 + sizeof(float);
		*(float*)(&pData[MyParameters.nFrom[3]]) = fTargetMoveSpeed;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DCAMERA_UPDATECAMERA,GetCurrentTime(),MyParameters);
	}
	HRESULT hResult  = E_FAIL;

    D3DXVECTOR3 vecDirPosTag;                       // 从理想位置指向轴心位置的方向
    float       fDisPreTarger;                      // 从理想位置指向轴心位置的长度  
    D3DXVECTOR3 vCameraFront;
    D3DXVECTOR3 vCameraSpeed;

    //
    if (m_vecPositionSave == vecPosition && m_vecAxesPosSave == vecTargetPos)
    {
        KG_PROCESS_SUCCESS(TRUE);
    }
    m_vecPositionSave = vecPosition;
    m_vecAxesPosSave  = vecTargetPos;
    //

    vCameraFront = GetCameraFront();
    D3DXVec3Normalize(&vCameraFront, &vCameraFront);

    if (vecPosition != vecTargetPos)
    { 
        vecDirPosTag  = vecTargetPos - vecPosition;
        fDisPreTarger = D3DXVec3Length(&vecDirPosTag);
        D3DXVec3Normalize(&vecDirPosTag, &vecDirPosTag);
    }
    else
    {
        vecDirPosTag = vCameraFront;
        fDisPreTarger = 0.0f;
    }

    if (m_fDisPreTarger == 0.0f)
        m_fDisPreTarger = fDisPreTarger;
    
    //m_fFlexOffset = m_fFlexOffset + fDisPreTarger - m_fDisPreTarger;   
    m_fFlexOffset = fDisPreTarger - m_fDisPreTarger;
    
    if (!static_cast<int>(m_fFlexOffset))
        m_fFlexOffset = 0.0f;
    m_fDisPreTarger  = fDisPreTarger;

    if (D3DXVec3Dot(&vCameraFront, &vecDirPosTag) < -0.98f)
        vecDirPosTag *= -1.0f;

    //D3DXVec3Normalize(&vecDirPosTag, &vecDirPosTag);

    //
    /*if (m_fTimePervUpdate == 0.f)
        m_fTimePervUpdate = g_cGraphicsTool.GetNowTime();
    fCameraSpeed = g_cGraphicsTool.GetNowTime() - m_fTimePervUpdate;
    vCameraSpeed = vecPosition - m_vecPerfectPos;
    m_fStartVelocity = D3DXVec3Length(&vCameraSpeed) * 1000.f / fCameraSpeed;
    m_fStartVelocity = max(m_fStartVelocity, 100.f);    
    m_fTimePervUpdate = g_cGraphicsTool.GetNowTime();*/
    //


    m_vecLookAtPos   = vecTargetPos  + vecDirPosTag * fLookAtOffset;  // 视点
    //m_vecPerfectPos  = vecPosition + vecDirPosTag * m_fFlexOffset;  // 期待位置
    m_vecPerfectPos  = vecPosition;
    m_vecTagPostion  = vecTargetPos;                                  // 轴心

    if (rude_same_pos(m_vecTagPostion, m_vecPrevTagPos))
        m_fStartVelocity = 100.f;
    else
        m_fStartVelocity = fTargetMoveSpeed;



    m_nUpdateFlag    = true;    // Camera 需要更新                                      

Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}


HRESULT KG3DTrackCamera::ObstructdAvert(D3DXVECTOR3* pVecCameraPos, D3DXVECTOR3* pVecTargerPos)
{
	HRESULT hResult  = E_FAIL;

    D3DXVECTOR3 vecDirRight;                            // 当前的位置的右方向
    D3DXVECTOR3 vecDirUp;                               // 当前的位置的上方向
    D3DXVECTOR3 vecUsablePos;
    
    static D3DXVECTOR3 vSrc[5];
    static D3DXVECTOR3 vDst[5];

    float fVel   = tanf(m_PerspectiveData.fFovy * 0.5f) * m_PerspectiveData.fzNear + 1.f;
    float fHov   = fVel * m_PerspectiveData.fAspect;   

    vecDirRight = GetCameraRight();
    vecDirUp    = GetCameraUp();

 
    // 五射线检测

    vSrc[0] = *pVecCameraPos;
    vSrc[1] = *pVecCameraPos + vecDirUp * fVel - vecDirRight * fHov;
    vSrc[2] = *pVecCameraPos + vecDirUp * fVel + vecDirRight * fHov;
    vSrc[3] = *pVecCameraPos - vecDirUp * fVel - vecDirRight * fHov;
    vSrc[4] = *pVecCameraPos - vecDirUp * fVel + vecDirRight * fHov;

    vDst[0] = *pVecTargerPos;
    vDst[1] = *pVecTargerPos + vecDirUp * fVel - vecDirRight * fHov;
    vDst[2] = *pVecTargerPos + vecDirUp * fVel + vecDirRight * fHov;
    vDst[3] = *pVecTargerPos - vecDirUp * fVel - vecDirRight * fHov;
    vDst[4] = *pVecTargerPos - vecDirUp * fVel + vecDirRight * fHov;

	//GetCrossPostion的意义统一成为从vSrc发射向vDes
    if (GetCrossPosition(vDst, vSrc, 5, &vecUsablePos))
		*pVecCameraPos = vecUsablePos;
        

	hResult = S_OK;
	return hResult;
}
HRESULT KG3DTrackCamera::SetTrackInfo(TrackCameraInfo trackInfo)
{
	if (g_cRecorder.IsRecord() &&  m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(TrackCameraInfo); 			
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(TrackCameraInfo*)(pData) = trackInfo;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DCAMERA_SETTRACKINFO,GetCurrentTime(),MyParameters);
	}   
	m_trackInfo = trackInfo;
    return S_OK;
}

HRESULT KG3DTrackCamera::GetTrackInfo(TrackCameraInfo* pTrackInof)
{
    if (!pTrackInof)
        return E_FAIL;

    *pTrackInof = m_trackInfo;
    return S_OK;
}

DWORD KG3DTrackCamera::GetCurrentState()
{
    return m_dwState;
}


BOOL KG3DTrackCamera::GetCrossPosition(
    const D3DXVECTOR3 vSrc[], const D3DXVECTOR3 vDst[], int nCount, D3DXVECTOR3* pVecCrossPos
    )
{
#if 0
	return FALSE;
#else
	FLOAT fTestRange = 0;

	for (int i = 0; i < nCount; ++i)
	{
		D3DXVECTOR3 vDir = vDst[i] - vSrc[i];

		FLOAT length = D3DXVec3Length(&vDir);
		fTestRange = max(length, fTestRange);
	}

	BOOL bRet = FALSE;
	FLOAT fDistRet = FLT_MAX;
	D3DXVECTOR3 vRet(0,0,0);

	const FLOAT CrossPosDistBias = 0.f;

	for (int i = 0; i < nCount; ++i)
	{
		D3DXVECTOR3 vDir = vDst[i] - vSrc[i];
		D3DXVec3Normalize(&vDir, &vDir);
		FLOAT fTempRet = FLT_MAX;
		BOOL bTempRet = this->RayIntersectionCamera(vSrc[i], vDir, fTestRange, &fTempRet);
		if(bTempRet && fTempRet < fDistRet)
		{
			fDistRet = fTempRet;
			_ASSERTE(fTempRet > -FLT_EPSILON);
			
		}
		bRet |= bTempRet;
	}

	if(bRet && NULL != pVecCrossPos)
	{
		D3DXVECTOR3 vDir0 = vDst[0] - vSrc[0];
		D3DXVec3Normalize(&vDir0, &vDir0);
		if(fDistRet > CrossPosDistBias)
			vRet = vSrc[0] + vDir0 * (fDistRet - CrossPosDistBias);
		else
			vRet = vSrc[0];
	}

	if(NULL != pVecCrossPos)
		*pVecCrossPos = vRet;

	return bRet;
#endif
}

int KG3DTrackCamera::_Station::IniByConfigFile(const char pcszFileName[])
{
	int nResult  = false;

	IIniFile* pIniFile = NULL;

	pIniFile = g_OpenIniFile(pcszFileName);
	KG_PROCESS_ERROR(pIniFile);

	pIniFile->GetInteger(TEXT("Camera"), TEXT("nChaseType"), nChaseType, &nChaseType);
	pIniFile->GetInteger(TEXT("Camera"), TEXT("bObstructdAvert"), bObstructdAvert, &bObstructdAvert);
    pIniFile->GetFloat(TEXT("Camera"),   TEXT("fChaseRate"), fChaseRate, &fChaseRate);
    pIniFile->GetFloat(TEXT("Camera"),   TEXT("fMaxDistance"), fMaxDistance, &fMaxDistance);
	pIniFile->GetFloat(TEXT("Camera"),   TEXT("fMinDistance"), fMinDistance, &fMinDistance);
	pIniFile->GetFloat(TEXT("Camera"),   TEXT("fMaxAngelVel"), fMaxAngelVel, &fMaxAngelVel);
	pIniFile->GetFloat(TEXT("Camera"),   TEXT("fMinAngelVel"), fMinAngelVel, &fMinAngelVel);
	pIniFile->GetFloat(TEXT("Camera"),   TEXT("fAngelRateHor"), fAngelRateHor, &fAngelRateHor);
	pIniFile->GetFloat(TEXT("Camera"),   TEXT("fAngelRateVel"), fAngelRateVel, &fAngelRateVel);
	pIniFile->GetFloat(TEXT("Camera"),   TEXT("fDisZoomRate"), fDisZoomRate, &fDisZoomRate);
	pIniFile->GetFloat(TEXT("Camera"),   TEXT("fFlexCoefficient"), fFlexCoefficient, &fFlexCoefficient);
    pIniFile->GetFloat(TEXT("Camera"),   TEXT("fDampCoefficient"), fDampCoefficient, &fDampCoefficient);
    pIniFile->GetInteger(TEXT("Camera"), TEXT("bUseFlexibilitySys"), bUseFlexibilitySys, &bUseFlexibilitySys);
    pIniFile->GetFloat(TEXT("Camera"), TEXT("fFlexRate"), fFlexRate, &fFlexRate);

    if (pCamera)
    {
        pCamera->m_trackInfo.fFlexCoefficient   = fFlexCoefficient;
        pCamera->m_trackInfo.fDampCoefficient   = fDampCoefficient;
        pCamera->m_trackInfo.nUseObstructdAvert = bObstructdAvert;
        pCamera->m_trackInfo.nUseFlexibilitySys = bUseFlexibilitySys;
        //pCamera->m_trackInfo.fFlexRate          = fFlexRate;
    }

	nResult = true;
Exit0:
	KG_COM_RELEASE(pIniFile);
	return nResult;
}

int KG3DTrackCamera::_Station::SaveToConfigFile(const char pcszFileName[])
{
	int nResult  = false;

	IIniFile* pIniFile = NULL;
	
	pIniFile = g_OpenIniFile(pcszFileName, false, true);
	KG_PROCESS_ERROR(pIniFile);

	pIniFile->WriteInteger(TEXT("Camera"), TEXT("nChaseType"), nChaseType);
	pIniFile->WriteInteger(TEXT("Camera"), TEXT("bObstructdAvert"), bObstructdAvert);
	pIniFile->WriteFloat(TEXT("Camera"), TEXT("fMaxDistance"), fMaxDistance);
	pIniFile->WriteFloat(TEXT("Camera"), TEXT("fMinDistance"), fMinDistance);
	pIniFile->WriteFloat(TEXT("Camera"), TEXT("fMaxAngelVel"), fMaxAngelVel);
	pIniFile->WriteFloat(TEXT("Camera"), TEXT("fMinAngelVel"), fMinAngelVel);
	pIniFile->WriteFloat(TEXT("Camera"), TEXT("fAngelRateHor"), fAngelRateHor);
	pIniFile->WriteFloat(TEXT("Camera"), TEXT("fAngelRateVel"), fAngelRateVel);
	pIniFile->WriteFloat(TEXT("Camera"), TEXT("fDisZoomRate"), fDisZoomRate);
	pIniFile->WriteFloat(TEXT("Camera"), TEXT("fFlexCoefficient"), fFlexCoefficient);
    pIniFile->WriteFloat(TEXT("Camera"), TEXT("fDampCoefficient"), fDampCoefficient);
	pIniFile->WriteFloat(TEXT("Camera"), TEXT("fChaseRate"), fChaseRate);
	pIniFile->WriteInteger(TEXT("Camera"), TEXT("bUseFlexibilitySys"), bUseFlexibilitySys);	
    pIniFile->WriteFloat(TEXT("Camera"), TEXT("fFlexRate"), fFlexRate);

    if (pCamera)
    {
        pCamera->m_trackInfo.fFlexCoefficient   = fFlexCoefficient;
        pCamera->m_trackInfo.fDampCoefficient   = fDampCoefficient;
        pCamera->m_trackInfo.nUseObstructdAvert = bObstructdAvert;
        pCamera->m_trackInfo.nUseFlexibilitySys = bUseFlexibilitySys;
        //pCamera->m_trackInfo.fFlexRate          = fFlexRate;
    }

	pIniFile->Save(pcszFileName);

	nResult = true;
Exit0:
	KG_COM_RELEASE(pIniFile);
	return nResult;
}

KG3DTrackCamera::_Station::_Station()
{
	pCamera             = NULL;

	nChaseType          = 2;
	bObstructdAvert     = true;
	bUseFlexibilitySys  = true;

	fChaseRate          = D3DX_PI;

	fMaxDistance        = 4000.0f;
	fMinDistance        = 0.0f;

	fMaxAngelVel        =  D3DX_PI / 2;
	fMinAngelVel        = -D3DX_PI / 180 * 50;

	fAngelRateHor       = D3DX_PI / 500;
	fAngelRateVel       = D3DX_PI / 500;

	fFlexCoefficient    = 1.5f;
	fDampCoefficient    = sqrtf(1.5f) * 2;
	fDisZoomRate        = 50.0f;
	fFlexRate           = 500.0f;
}

KG3DTrackCamera::_Station& KG3DTrackCamera::_Station::operator=( KG3DTrackCamera::_Station& temp )
{
	KG3DTrackCamera* p = this->pCamera;
	memcpy_s(this, sizeof(_Station), &temp, sizeof(_Station));
	this->pCamera = p;
	/*
	this->nChaseType          = temp.nChaseType;
	this->bObstructdAvert     = temp.bObstructdAvert;
	this->bUseFlexibilitySys  = temp.bUseFlexibilitySys;

	this->fChaseRate          = temp.fChaseRate;

	this->fMaxDistance        = temp.fMaxDistance;
	this->fMinDistance        = temp.fMinDistance;

	this->fMaxAngelVel        = temp.fMaxAngelVel;
	this->fMinAngelVel        = temp.fMinAngelVel;

	this->fAngelRateHor       = temp.fAngelRateHor;
	this->fAngelRateVel       = temp.fAngelRateVel;

	this->fFlexCoefficient    = temp.fFlexCoefficient;
	this->fDampCoefficient    = temp.fDampCoefficient;
	this->fDisZoomRate        = temp.fDisZoomRate;
	this->fFlexRate           = temp.fFlexRate;*/

	return *this;
}
HRESULT KG3DTrackCamera::SetDistance(float fDistance)
{
	HRESULT hResult  = E_FAIL;

	KG_PROCESS_ERROR(fDistance >= m_cameraStation.fMinDistance);
	KG_PROCESS_ERROR(fDistance <= m_cameraStation.fMaxDistance);

	m_fDistance = fDistance;

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DTrackCamera::SetAngleHor(float fAngleHor)
{
	m_fAngleHor = fAngleHor;
	return S_OK;
}

HRESULT KG3DTrackCamera::SetAngleVel(float fAngleVel)
{
    HRESULT hResult = E_FAIL;

    KG_PROCESS_ERROR(fAngleVel >= m_cameraStation.fMinAngelVel);
    KG_PROCESS_ERROR(fAngleVel <= m_cameraStation.fMaxAngelVel);

	m_fAngleVel = fAngleVel;

    hResult = S_OK;
Exit0 :
	return hResult;
}

float KG3DTrackCamera::GetDistance()
{
	return m_fDistance;
}

float KG3DTrackCamera::GetAngleHor()
{
	return m_fAngleHor;
}

float KG3DTrackCamera::GetAngleVel()
{
	return m_fAngleVel;
}

HRESULT KG3DTrackCamera::ZoomDistance(float fMultiple)
{
	HRESULT hResult  = E_FAIL;

	float fTempDistance = m_fDistance * fMultiple;

	KG_PROCESS_ERROR(fTempDistance >= m_cameraStation.fMinDistance);
	KG_PROCESS_ERROR(fTempDistance <= m_cameraStation.fMaxDistance);

	m_fDistance = fTempDistance;

Exit0:
	hResult = S_OK;
	return hResult;
}

HRESULT KG3DTrackCamera::AddDistance(float fDistance)
{
	HRESULT hResult  = E_FAIL;
	
	float fTempDistance = m_fDistance + fDistance;

	KG_PROCESS_ERROR(fTempDistance >= m_cameraStation.fMinDistance);
	KG_PROCESS_ERROR(fTempDistance <= m_cameraStation.fMaxDistance);

	m_fDistance = fTempDistance;

Exit0:
	hResult = S_OK;
	return hResult;
}

HRESULT KG3DTrackCamera::AddAngleHor(float fAngleHor) 
{
	m_fAngleHor += fAngleHor; 
	return S_OK;
}

HRESULT KG3DTrackCamera::AddAngleVel(float fAngleVel)
{
	HRESULT hResult  = E_FAIL;

	float fTempAngel = m_fAngleVel + fAngleVel;

	KG_PROCESS_ERROR(fTempAngel >= m_cameraStation.fMinAngelVel);
	KG_PROCESS_ERROR(fTempAngel <= m_cameraStation.fMaxAngelVel);
	
	m_fAngleVel = fTempAngel;

	hResult = S_OK;
Exit0:
	return hResult;
}

LRESULT KG3DTrackCamera::ProcessMouseMove(POINT& prevPoint, POINT& currPoint)
{
	int	nResult  = false;

    KG3DScene*	lpCurScene = NULL;	
    D3DXVECTOR3 vecTerrain;

    lpCurScene   = m_lpCurScene;
    KG_PROCESS_ERROR(lpCurScene);

	float fTempAngel = 0;
    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_RBUTTON) & 0x8000))
    {
		if  (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			m_fAnVarHor = (currPoint.x - prevPoint.x) * m_cameraStation.fAngelRateHor;
			m_nRegressFlag = false;
		}

		m_fAnVarVel = (currPoint.y - prevPoint.y) * m_cameraStation.fAngelRateVel;
		m_fAngleHor = m_fAngleHor + m_fAnVarHor;
        //if (m_fVelGlide + m_fAnVarVel > - D3DX_PI / 2)
        m_fVelGlide = m_fVelGlide + m_fAnVarVel;
		fTempAngel  = m_fAngleVel + m_fAnVarVel;

        //lpCurScene->GetFloor(&vecTerrain, &m_vecCurPostion);
		vecTerrain = m_vecCurPostion;
		lpCurScene->GetFloor(&m_vecCurPostion, &vecTerrain.y);
        if (m_vecCurPostion.y >= vecTerrain.y + 5)
        {
            if (
                (fTempAngel <= m_cameraStation.fMaxAngelVel) &&
                m_fUpOffset <= 0.0f
            )
            {
                m_fAngleVel = fTempAngel;
            }
        }
        else
		{
			if (
                (fTempAngel <= m_cameraStation.fMaxAngelVel) &&
                (fTempAngel >= m_cameraStation.fMinAngelVel) &&
                m_fUpOffset <= 0.0f
            )
			{
				m_fAngleVel = fTempAngel;
			}
            if (fTempAngel  < m_cameraStation.fMinAngelVel) 
                m_fAngleVel = m_cameraStation.fMinAngelVel;
            if (fTempAngel  > m_cameraStation.fMaxAngelVel) 
                m_fAngleVel = m_cameraStation.fMaxAngelVel;

		}
        //currPoint = prevPoint;
        //::SetCursorPos(currPoint.x, currPoint.y);
	}
	else
	{
		m_fAnVarHor = 0.0f;
		m_fAnVarVel = 0.0f;
	}

    nResult = true;
Exit0:
    return nResult;
}

LRESULT KG3DTrackCamera::ProcessMouseWeel(WPARAM wParam, LPARAM lParam)
{	
	int	nResult  = false;

	short nDelta		= HIWORD(wParam); 
	float fZoom         = 1.0f;

	if (nDelta > 0)
	{
		fZoom -= m_cameraStation.fDisZoomRate;
	}
	else  
	{
		fZoom += m_cameraStation.fDisZoomRate;
	}

	FLOAT fTempDistance = m_fDistance + fZoom;

	KG_PROCESS_ERROR(fTempDistance >= m_cameraStation.fMinDistance);
	KG_PROCESS_ERROR(fTempDistance <= m_cameraStation.fMaxDistance);

	m_fDistance = fTempDistance;         

	nResult = true;
Exit0:
	return nResult;
	
}

/*
HRESULT KG3DTrackCamera::ProcessLButtonDn(WPARAM wParam, LPARAM lParam)
{
    //while (ShowCursor(FALSE) >= 0);
    return S_OK;
}*/


/*
HRESULT KG3DTrackCamera::ProcessLButtonUp(WPARAM wParam, LPARAM lParam)
{
    //while (ShowCursor(TRUE) < 0);
    return S_OK;
}*/


HRESULT KG3DTrackCamera::UpdateAsClient(
	D3DXVECTOR3 vecTargetPos, D3DXVECTOR3 vecTargetDir, float fTargerSpeed, DWORD dwTargerMoveType
)
{
	HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

	D3DXVECTOR3  vecCurPos;					// 当前摄象机的位置
	D3DXVECTOR3  vecPerPos;					// 理想摄象机的位置

	D3DXVECTOR3  vecTemp_1;
	D3DXVECTOR3  vecTemp_2;

    D3DXVECTOR3  vecTarDir;                 // 目标的方向      
    D3DXVECTOR3  vecCameLA;                 // 相机观看点(Targer是旋转的中心,LookAt是观看点)

    D3DXVECTOR3  vecDirPerTar;              // 从理想位置指向目标位置
    D3DXVECTOR3  vecDirCurTar;              // 从当前位置指向目标位置

    D3DXVECTOR3  vecDirCurPer;              // 从当前位置指向理想位置
    D3DXVECTOR3  vecDisplacet;              // 位移

	float  fDelayTime = 0.0f;               // 
    //float  fDisCurTar = 0.0f;               // 当前位置到目标位置的长度
    //float  fDisCurPer = 0.0f;               // 当前位置到理想位置的长度
    //float  fDisPerTar = 0.0f;               // 理想位置到目标位置的长度
    //float  fPrevCurTr = 0.0f;               // 上一帧和目标的长度

	int    nTargerMoveFlag = false;			// 目标是否发生运动(位置和方向)
    int    nTargerPosMoved = false;         // 目标是否发生运动(位置)
	int	   nRegressFlag	   = false;			// 是否回到理想角度
    int    nMouseMoved     = false;         

	D3DXMATRIX matRocation;		

    static float scfVelGlide  = 0.0f;
    static int   nUseFlexFlag = false;
    static POINT scMousePoint;
    POINT mousePoint;

    fDelayTime = g_cGraphicsTool.GetTinyTime() / 1000.0f;
    KG_PROCESS_ERROR(fDelayTime != 0.0f);
	
	vecCurPos = m_vecCurPostion;
	nTargerMoveFlag = 
		(m_vecTagPostion != vecTargetPos) || (m_vecTagDirectn != vecTargetDir);
    nTargerPosMoved = m_vecTagPostion != vecTargetPos;

    //
    // 鼠标运动消息
    //

	KG_PROCESS_ERROR(NULL != m_lpCurScene);

    IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
    hRetCode = m_lpCurScene->GetCurOutputWnd(&piSceneCurOutputWnd);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    GetCursorPos(&mousePoint);
    if (::GetFocus() == static_cast<KG3DSceneOutputWnd *>(piSceneCurOutputWnd)->m_hWnd)
    {
        if ((scMousePoint.x != mousePoint.x) || scMousePoint.y != mousePoint.y)
        {
            ProcessMouseMove(scMousePoint, mousePoint);
            nMouseMoved = true;
        }
    }
    scMousePoint = mousePoint;

    //
    // 如果仰视的角度小于设置的最小角度则抬起LookAt点
    //

    if (m_fVelGlide < m_fAngleVel)
    {
        D3DXVECTOR3 vecDirCamTar = vecTargetPos - vecCurPos;
        vecDirCamTar.y = 0.0f;
        float fLen = D3DXVec3Length(&vecDirCamTar);
        float fHig = tanf(D3DX_PI / 2.5) * fLen;
        m_fUpOffset = (m_fAngleVel - m_fVelGlide) * fHig;
    }
    else
    {
        m_fUpOffset = 0.0f;   
        m_fVelGlide = m_fAngleVel;
    }

    if (nTargerPosMoved)
    {
        if (m_fVelGlide < m_fAngleVel)
            m_fVelGlide += D3DX_PI / 4 * fDelayTime;
        else
            m_fVelGlide = m_fAngleVel;
    }

	//
	// 如果目标运动,则让摄象机慢慢回到理想角度(如果之前角度发生偏转) : 追尾
	//

	if (
        (nTargerMoveFlag && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000) && m_cameraStation.nChaseType == 2) || // 智能
        (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000) && m_cameraStation.nChaseType == 1)    // 总是
    )
	{
        // 计算合理的偏转角度
        if (fabs(m_fAngleHor) > D3DX_PI * 2)
            m_fAngleHor = fmodf(m_fAngleHor, D3DX_PI * 2);
        if (m_fAngleHor >  D3DX_PI)
            m_fAngleHor = m_fAngleHor - D3DX_PI * 2;
        if (m_fAngleHor < -D3DX_PI)       
            m_fAngleHor = D3DX_PI * 2 + m_fAngleHor;

        // 求偏转方向
		if (m_fAngleHor > 0.0f)
			nRegressFlag =  1;
		else if (m_fAngleHor < 0.0f)
			nRegressFlag = -1;
		else
			nRegressFlag =  0;

        // 当前帧偏转角度
		m_fAngleHor += nRegressFlag * (-m_cameraStation.fChaseRate) * fDelayTime;

        // 已经回到理想角度
		if (m_fAngleHor * nRegressFlag <= 0.0f || !nRegressFlag)
			m_fAngleHor = 0.0f;
	}

    vecTarDir  = vecTargetDir;

    // 旋转水平角度
	if (m_fAngleHor != 0.0f)
	{
		D3DXMatrixRotationY(&matRocation, m_fAngleHor);
		D3DXVec3TransformCoord(&vecTarDir, &vecTarDir, &matRocation);
	}
	
	//
	// 根据目标位置和方向以及相关设置计算出摄象机的理想位置 vecPerPos
	//

	vecTemp_1 = -vecTarDir;
	D3DXMatrixRotationY(&matRocation, -D3DX_PI / 2);
	D3DXVec3TransformCoord(&vecTemp_2,   &vecTemp_1, &matRocation);
	D3DXMatrixRotationAxis(&matRocation, &vecTemp_2, m_fAngleVel);
	D3DXVec3TransformCoord(&vecTemp_1,   &vecTemp_1, &matRocation);
	D3DXVec3Normalize(&vecTemp_1, &vecTemp_1);
	vecPerPos = vecTargetPos + m_fDistance * vecTemp_1;

    //
    // 更新位置
    //
    
    if (!rude_same_pos(vecPerPos, m_vecPerfectPos) || scfVelGlide != m_fVelGlide)
        UpdateCamera(vecPerPos, vecTargetPos, 100.f, fTargerSpeed);

	//add by huangjinshou 2008-1-30/////////////
	m_fHeightDistance =m_vecCurPostion.y - vecTargetPos.y;
	///////////////////////////////////////////
    scfVelGlide = m_fVelGlide;
	m_vecTagDirectn = vecTargetDir;
	
	hResult = S_OK;
Exit0:
	return hResult;
}

IKG3DCamera* KG3DTrackCamera::GetNew()
{
	return new KG3DTrackCamera;
}


HRESULT KG3DTrackCamera::Copy(IKG3DCamera* pCamera)
{
	KG3DTrackCamera* p3DCamera = static_cast<KG3DTrackCamera*>(pCamera);
	if (!p3DCamera)
		return E_FAIL;
	*p3DCamera = *this;
	return S_OK;	
}

//
//////////////////////////////////////////////////////////////////////////
//(by dengzhihui Refactor 2006年11月6日
HRESULT KG3DTrackCamera::GetStation(IEKG3DCameraStation** pStation)
{
	*pStation = &m_StationHelper;
	return S_OK;
}


HRESULT KG3DTrackCamera::CStation::IniByConfigFile(LPCTSTR pcszFileName)
{
	m_station->IniByConfigFile(pcszFileName);
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::SaveToConfigFile(LPCTSTR pcszFileName)
{
	m_station->SaveToConfigFile(pcszFileName);
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::GetChaseType(int* pChaseType)
{
	*pChaseType = m_station->nChaseType;
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::SetChaseType(int nChaseType)
{
	m_station->nChaseType = nChaseType;
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::GetChaseRate(float* pChaseRate)
{
	*pChaseRate = m_station->fChaseRate;
	return S_OK;
}
HRESULT KG3DTrackCamera::CStation::SetChaseRate(float fChaseRate)
{
	m_station->fChaseRate = fChaseRate;
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::GetObstructdAvert(int* pObstructdAvert)
{
	*pObstructdAvert = m_station->bObstructdAvert;
	return S_OK;
}
HRESULT KG3DTrackCamera::CStation::SetObstructdAvert(int ObstructdAvert)
{
	m_station->bObstructdAvert = ObstructdAvert;
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::GetFlexCoefficient(float* FlexCoefficient)
{
	*FlexCoefficient = m_station->fFlexCoefficient;
	return S_OK;
}
HRESULT KG3DTrackCamera::CStation::SetFlexCoefficient(float FlexCoefficient)
{
	m_station->fFlexCoefficient = FlexCoefficient;
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::GetDampCoefficient(float* DampCoefficient)
{
	*DampCoefficient = m_station->fDampCoefficient;
	return S_OK;
}
HRESULT KG3DTrackCamera::CStation::SetDampCoefficient(float DampCoefficient)
{
	m_station->fDampCoefficient = DampCoefficient;
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::GetMaxAngelVel(float* MaxAngelVel)
{
	*MaxAngelVel = m_station->fMaxAngelVel;
	return S_OK;
}
HRESULT KG3DTrackCamera::CStation::SetMaxAngelVel(float MaxAngelVel)
{
	m_station->fMaxAngelVel = MaxAngelVel;
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::GetMinAngelVel(float* MinAngelVel)
{
	*MinAngelVel = m_station->fMinAngelVel;
	return S_OK;
}
HRESULT KG3DTrackCamera::CStation::SetMinAngelVel(float MinAngelVel)
{
	m_station->fMinAngelVel = MinAngelVel;
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::GetAngelRateVel(float* AngelRateVel)
{	
	*AngelRateVel = m_station->fAngelRateVel;
	return S_OK;
}
HRESULT KG3DTrackCamera::CStation::SetAngelRateVel(float AngelRateVel)
{
	m_station->fAngelRateVel = AngelRateVel;
	return S_OK;
}


HRESULT KG3DTrackCamera::CStation::GetAngelRateHor(float* AngelRateHor)
{
	*AngelRateHor = m_station->fAngelRateHor;
	return S_OK;
}
HRESULT KG3DTrackCamera::CStation::SetAngelRateHor(float AngelRateHor)
{
	m_station->fAngelRateHor = AngelRateHor;
	return S_OK;
}

HRESULT KG3DTrackCamera::CStation::GetMaxDistance(float* MaxDistance)
{
	*MaxDistance = m_station->fMaxDistance;
	return S_OK;
}
HRESULT KG3DTrackCamera::CStation::SetMaxDistance(float MaxDistance)
{
	m_station->fMaxDistance = MaxDistance;
	return S_OK;
}
HRESULT KG3DTrackCamera::CStation::GetDisZoomRate(float *fRate)
{
    *fRate = m_station->fDisZoomRate;
    return S_OK;
}
HRESULT KG3DTrackCamera::CStation::SetDisZoomRate(float fRate)
{
    m_station->fDisZoomRate = fRate;
    return S_OK;
}
//)