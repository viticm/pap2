////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFX.h"
#include "KG3DEngineManager.h"
#include "KG3DModelTable.h"
#include "KG3DSceneSFXEditor.h"
#include "KG3DAnimationController.h"
#include "IEEditor.h"
#include "KG3DTagSFXAnimationController.h"
#include "MathTool.h"
#include "..\..\..\Include\IKG3DSoundShell.h"
#include "dsound.h"
#include "KG3DGraphicsTool.h"
#include "KG3DTextureTable.h"
#include "KG3DSFXSelectTexture.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DRecorder.h"
#include "KG3DGraphicsTool.h"
#include "KG3DMultiThreadLoader.h"
#include "IEKG3DAnimationTag.h"
#include "KG3DAnimationTagContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

extern BOOL g_bOpenSelectTexture;

    int g_holdrand = (int)time(0);
    
    inline int random() 
    { 
        return (((g_holdrand = g_holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
    }

    inline float GetRandom(float fMin, float fMax)
    {
        float fRandNum = (float)(((g_holdrand = g_holdrand * 214013L + 2531011L) >> 16) & 0x7fff) / RAND_MAX;
        return (fMin + (fMax - fMin) * fRandNum);
    }

    inline int GetRandomSgn()
    {
	    return ((((g_holdrand = g_holdrand * 214013L + 2531011L) >> 16) & 0x7fff) > RAND_MAX / 2) ? 1 : -1;
    }


    inline D3DXVECTOR2 GetRandVec2D()
    {
	    D3DXVECTOR2 vResult;
	    vResult.x = GetRandom(-1.0f, 1.0f);
	    vResult.y = GetRandom(-1.0f, 1.0f);

	    D3DXVec2Normalize(&vResult, &vResult);

	    return vResult;
    }

    inline D3DXVECTOR3 GetRandVec3D()
    {
	    D3DXVECTOR3 vResult;
	    vResult.x = GetRandom(-1.0f, 1.0f);
	    vResult.y = GetRandom(-1.0f, 1.0f);
	    vResult.z = GetRandom(-1.0f, 1.0f);

	    D3DXVec3Normalize(&vResult, &vResult);

	    return vResult;
    }

    inline void RandomVec3D(D3DXVECTOR3& v, const float& fact)
    {
        v.x = GetRandom(-fact, fact);
        v.y = GetRandom(-fact, fact);
        v.z = GetRandom(-fact, fact);
    }

namespace {

    struct ScreenQuad
    {
        D3DXVECTOR4 p; 
        D3DXVECTOR2 t; 

        enum {FVF = D3DFVF_XYZRHW | D3DFVF_TEX1};
    };

    const char* SafeGetRetPath(const char _filePath[]) 
    {
        char rootPath[MAX_PATH];
        char filePath[MAX_PATH];

        g_GetRootPath(rootPath);
        strcpy(filePath, _filePath);

        strlwr(filePath);
        strlwr(rootPath);

        char* pStr = strstr(const_cast<char*>(filePath), rootPath);
        size_t offset = 0;

        if (pStr == filePath) 
        {
            offset = strlen(rootPath);            
            if (filePath[offset] == '\\')
                ++offset;
        }

        return filePath + offset;
    }

    /*void DrawScreenQuad(const D3DXVECTOR2& center, float fRadius)
    {
        KG3DRenderState RenderState;

        ScreenQuad v[4];

        v[0].p = D3DXVECTOR4(center.x - fRadius - 0.5f, center.y - fRadius - 0.5f, 0.f, 1.f);
        v[0].t = D3DXVECTOR2(0.f, 0.f);
        v[1].p = D3DXVECTOR4(center.x + fRadius - 0.5f, center.y - fRadius - 0.5f, 0.f, 1.f);
        v[1].t = D3DXVECTOR2(1.f, 0.f);
        v[2].p = D3DXVECTOR4(center.x - fRadius - 0.5f, center.y + fRadius - 0.5f, 0.f, 1.f);
        v[2].t = D3DXVECTOR2(0.f, 1.f);
        v[3].p = D3DXVECTOR4(center.x + fRadius - 0.5f, center.y + fRadius - 0.5f, 0.f, 1.f);
        v[3].t = D3DXVECTOR2(1.f, 1.f);

        RenderState.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
        RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);

        RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        RenderState.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

        g_pd3dDevice->SetFVF(ScreenQuad::FVF);
        g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(ScreenQuad));

    }*/
}

KG3DSFX::ProcessBlockFunction KG3DSFX::ms_pfnProcessBlock[SFX_FEID_SIZE] = 
{
    &ReadGeneralInformationBlock,   // SFX_FEID_GENERAL_INFORMATION,
    &ReadParticleKindBlock,         // SFX_FEID_PARTICLE_KIND,
    &ReadLauncherBlock,             // SFX_FEID_LAUNCHER,
    &ReadForceFieldBlock,           // SFX_FEID_FORCE_FIELD,
    &ReadTextureBlock,              // SFX_FEID_TEXTURE,
    &ReadBillboardBlock,            // SFX_FEID_BILLBOARD,
    &ReadTrackBlock,                // SFX_FEID_TRACK,
    &ReadModelBlock,                // SFX_FEID_MODEL,
    &ReadMeteorBlock,               // SFX_FEID_METEOR,
    &ReadMagicParpetBlock,          // SFX_FEID_MAGIC_PARPET,
    &ReadChannelBlock,              // SFX_FEID_CHANNEL
    &ReadSoundBlock,                // SFX_FEID_SOUND
	&ReadSelectMarkBlock,           // SFX_FEID_FOOTPRINT           
    &ReadExpBlock,                  // SFX_FEID_EXP
    &ReadBoxProjBlock,              // SFX_FEID_BOX_PROJ
    NULL                            // SFX_FEID_PARICLE_KIND_EXT,
};

KG3DSFX::KG3DSFX(void)
{
    m_fCurrentFrame			= 0;
	m_fPrevFrame			= 0;
    m_dwTotalFrameNum		= 100;
    m_dwFrameInterval		= 30;
    m_dwLastTime			= 0;
	m_fDelayTime			= 0.f;
    m_eSFXAnimationPlay		= SFX_AP_LOOP;
    m_eSFXAnimationPlaySave = SFX_AP_LOOP;
    m_pParentModel			= NULL;
    m_bSortBillboard		= TRUE;
    m_fSizeScale			= 1.0f;
    m_fAlpha				= 1.0f;
    m_pChannel				= NULL;
	m_fModelAniStartFrame	= 50;
    m_dwModelPlayMode		= ENU_ANIMATIONPLAY_CIRCLE;
    m_nIsSockWave			= FALSE;
    m_pSFXAnimationController = NULL;
    m_nPlayFinishFlag		= FALSE;
    m_fpCallBackRender		= NULL;
    m_pUserData				= NULL;
    m_pExpEffect            = NULL;
    m_dwRenderTarIndex		= (DWORD)-1;
    m_nBillBoardMdlNum      = 0;
    m_dwFixedSize           = false;
    m_dwLockYAlex           = false;
	m_bModelAnimationStarted = FALSE;
    m_nEditor               = FALSE;    
    m_nTexLoadIndex         = -1;
    m_nMdlLoadIndex         = -1;
    m_dwMaxParticleNum      =  0;
    m_fShockWavePower       = 1.f;
    m_fLastParticlFrame     = -1;
    m_fFixedFrame           = 50.f;
    m_fSpeed                = 1.f;
	m_dwLastTimeUpdateFootprint = 0;
	m_vecTexture.push_back(NULL);
	m_vecCuttingNum.push_back(1);
    m_ParticleContainer.SetTextureNum((UINT)m_vecTexture.size());
	m_vecModelsInfo.clear();
	m_pSelectMark = NULL;
	m_dwLoadOption = 0;
	m_dwUserData = 0;
}


KG3DSFX::~KG3DSFX(void)
{
    ClearUp(TRUE);
}


HRESULT KG3DSFX::_LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
	
	HRESULT hrResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	DWORD            dwSize   = 0;
	SFX_FILE_HEADER* pHeader  = NULL;
	DWORD            fileLen  = 0;
	BYTE*            pBuffer  = NULL;
	DWORD            dwCurPos = 0;
	DWORD            i        = 0;
	IFile*           pFile    = NULL;

	ASSERT(cszFileName);

	ClearUp(FALSE);

	m_scName = cszFileName;

    if (uOption & MLO_SFX_EDITING)
        m_nEditor = true;

	m_dwLoadOption = uOption;
	
	pFile = g_OpenFile(cszFileName);
	KG_PROCESS_ERROR(pFile);

	fileLen = pFile->Size();
	ASSERT(fileLen > 0);
	KG_PROCESS_ERROR(fileLen);

	pBuffer = new BYTE[fileLen];
	KG_PROCESS_ERROR(pBuffer);

	dwSize = pFile->Read(pBuffer, fileLen);
	KG_PROCESS_ERROR(dwSize == fileLen);

	pHeader = reinterpret_cast<SFX_FILE_HEADER*>(pBuffer);
	KG_PROCESS_ERROR(pHeader->dwID == SFX_ID);
	KG_PROCESS_ERROR(pHeader->dwFileLength == fileLen);
	KG_PROCESS_ERROR(LOWORD(pHeader->dwVersion) >= 2);

    m_nVerson = (int)LOWORD(pHeader->dwVersion);

	dwCurPos += sizeof(SFX_FILE_HEADER);
	for (i = 0; i < pHeader->dwElementNum; ++i, dwCurPos += sizeof(SFX_ELEMENT))
	{
		SFX_ELEMENT *pElem = reinterpret_cast<SFX_ELEMENT*>(&pBuffer[dwCurPos]);
		ASSERT(dwCurPos < fileLen);

		KG_PROCESS_ERROR(
			(pElem->dwElementID >= SFX_FEID_GENERAL_INFORMATION) &&
			(pElem->dwElementID < SFX_FEID_SIZE)
			);

		ASSERT(ms_pfnProcessBlock[pElem->dwElementID]);
		KG_PROCESS_ERROR(ms_pfnProcessBlock[pElem->dwElementID]);

		ASSERT(pElem->dwElementOffset <= fileLen);
		hRetCode = (this->*ms_pfnProcessBlock[pElem->dwElementID])(pBuffer, pElem->dwElementOffset);
		KG_COM_PROC_ERR_RET_ERR(hRetCode);
	}

	// Make sure there is one texture at least
	if (m_vecTexture.size() == 0)
	{
		m_vecTexture.push_back(NULL);
		m_vecCuttingNum.push_back(1);
        m_ParticleContainer.SetTextureNum((UINT)m_vecTexture.size());
	}

    ASSERT(m_vecTexture.size() == m_vecCuttingNum.size());

    for (size_t i = 0; i < m_vecParticleLifeTimeLine.size(); ++i)
        clip(m_vecParticleLifeTimeLine[i]->m_uTextureIndex, (UINT)0, (UINT)m_vecTexture.size() - 1);

    // 
    if (LOWORD(pHeader->dwVersion) <= 2)
    {
        size_t uForceNum = m_vecSFXParticleForceField.size();
        size_t uLauNum   = m_vecSFXParticleLauncher.size();

        for (size_t i = 0; i < uLauNum; i++)
        {
            for (size_t a = 0; a < uForceNum; a++)
            {
                m_vecSFXParticleLauncher[i]->m_vecEffForcesIDs.push_back((DWORD)a);
                m_vecSFXParticleLauncher[i]->m_vecEffForces.push_back(m_vecSFXParticleForceField[a]);
                m_vecSFXParticleForceField[a]->AddLauncherRef(m_vecSFXParticleLauncher[i]);
                
            }
        }
    }
    else
    {
         size_t uLauNum   = m_vecSFXParticleLauncher.size();
         for (size_t i = 0; i < uLauNum; i++)
         {
            size_t uForceNum = m_vecSFXParticleLauncher[i]->m_vecEffForcesIDs.size();

            for (size_t a = 0; a < uForceNum; a++)
            {
                UINT FID = m_vecSFXParticleLauncher[i]->m_vecEffForcesIDs[a];
                IEKG3DSFXParticleForceField* pForce = m_vecSFXParticleForceField[FID];
                m_vecSFXParticleLauncher[i]->m_vecEffForces.push_back(pForce);
                pForce->AddLauncherRef(m_vecSFXParticleLauncher[i]);
            }
         }


    }

	CalculateAABB();
	//ComputeBBox();

	for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
	{
		if (!m_vecModelsInfo[s].pSFXModel) 
		{
			ASSERT(FALSE);
			continue;
		}

		size_t ainNum = m_vecModelsInfo[s].pSFXModel->m_vecAnimationLoaded.size();
		if (ainNum)
		{
			//AnimationItem clipInfo;
			IEKG3DClip* piClip;
			piClip = m_vecModelsInfo[s].pSFXModel->m_vecAnimationLoaded[0];
			piClip->GetNumFrames(&(m_vecModelsInfo[s].dwNumFrame));
			piClip->GetFrameLength(&(m_vecModelsInfo[s].fFrameTime));
			m_vecModelsInfo[s].nValidFlag = TRUE;
		}
	}

	hrResult = S_OK;
	m_bMeshPostLoadProcessed = TRUE;
Exit0:
	if (FAILED(hrResult))
	{
		KGLogPrintf(KGLOG_ERR, "Load SFX File %s failed.\n", cszFileName);
		ClearUp(FALSE);
	}

	SAFE_DELETE_ARRAY(pBuffer);
	KG_COM_RELEASE(pFile);
	
	return hrResult;
}

HRESULT KG3DSFX::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
	HRESULT hr = E_FAIL;

	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && !g_cRecorder.IsLock())
	{		
		TCHAR strFile[MAX_PATH];
		TCHAR strDrive[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strExt[MAX_PATH];

		_splitpath_s(cszFileName, 
			strDrive, 
			MAX_PATH,
			strPath, 
			MAX_PATH, 
			strFile, 
			MAX_PATH, 
			strExt,
			MAX_PATH);
		if (strcmp(strFile,"title") != 0)
		{
			m_dwRecordID =  KG3DRecorder::GetMasterID();
			KG3DRecorder::Parameters MyParameters;
			MyParameters.nNum = 4;
			MyParameters.size_tDataSize = sizeof(TrueType) + 
				MAX_PATH * sizeof(char) + sizeof(uFileNameHash) + sizeof(uOption);
			char* pData = new char[MyParameters.size_tDataSize];
			MyParameters.pData = pData;
			MyParameters.nFrom[0] = 0;		
			*(TrueType*)(&pData[MyParameters.nFrom[0]]) = TT_SFX;
			MyParameters.nFrom[1] = sizeof(TrueType);
			strcpy(&(pData[MyParameters.nFrom[1]]),cszFileName);
			MyParameters.nFrom[2] = sizeof(TrueType) + MAX_PATH * sizeof(char);
			*(unsigned*)(&pData[MyParameters.nFrom[2]]) = uFileNameHash;
			MyParameters.nFrom[3] = sizeof(TrueType) + MAX_PATH * sizeof(char) + sizeof(uFileNameHash);
			*(unsigned*)(&pData[MyParameters.nFrom[3]]) = uOption;
			g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_LOADFROMFILE,GetCurrentTime(),MyParameters);
			bIsPassRecord = TRUE;
			g_cRecorder.Lock();	
		}
	}
	m_uOption = uOption;
	m_scName  = cszFileName;

	DWORD dwThreadID = ::GetCurrentThreadId();
	if(dwThreadID != g_dwMainThreadID)
	{
		uOption |= MLO_MESH_MULTI;
	}


	if((uOption & MLO_MESH_MULTI) && (g_eEngineOption.bUseMultiThreadLoad))
	{
		g_cMultiThreadLoader.TryMultiThreadLoad_Model(this);
		hr = S_OK;
	}
	else
	{
		hr =_LoadFromFile(cszFileName,uFileNameHash,uOption);
	}

	if (bIsPassRecord)
		g_cRecorder.UnLock();

	return hr;
}

HRESULT KG3DSFX::LoadFromFileMultiThread()
{
	_LoadFromFile(m_scName.c_str(),0,m_uOption);
	return S_OK;
}

HRESULT KG3DSFX::LoadPostProcess()
{
    if (m_bMeshPostLoadProcessed)
    {
        for (size_t i = 0; i < m_vecModelsInfo.size(); ++i)
        {
            m_vecModelsInfo[i].pSFXModel->LoadPostProcess();
        }
    }

    return S_OK;
}

HRESULT KG3DSFX::SaveToFile(const char cszFileName[], unsigned uOption)
{
	//Safely save to file backup before overwrite the old file and delete the back after save successfully.
	HRESULT hResult     = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	
	TCHAR strBackupFileName[MAX_PATH];
	BOOL bSucceeded = TRUE;
	BOOL bNeedBackup = PathFileExists(cszFileName);
	BOOL bExists = bNeedBackup;
	DWORD dwIndex = 0;
	while (bExists)
	{
		sprintf_s(strBackupFileName, 
			MAX_PATH,
			"%s.bak%d", 
			cszFileName, 
			dwIndex);
		
		bExists = PathFileExists(strBackupFileName);
		dwIndex++;
	}

	if (bNeedBackup)
	{
		bSucceeded = CopyFile(cszFileName, strBackupFileName, TRUE);
		if (!bSucceeded)
		{
			KGLogPrintf(KGLOG_ERR, "Backup SFX file %s failed.", cszFileName);
		}
	}
	
	hRetCode = _SaveToFile(cszFileName, uOption);
	KG_COM_PROCESS_ERROR(hRetCode);
	
	
	hResult = S_OK;
Exit0:
	if (SUCCEEDED(hResult) && bSucceeded)
	{
		//Delete backup when succeed.
		DeleteFile(strBackupFileName);
	}
	return hResult;
}

HRESULT KG3DSFX::_SaveToFile( const char cszFileName[], unsigned uOption )
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	SFX_FILE_HEADER Header;
	SFX_ELEMENT     ElemGeneral;
	SFX_ELEMENT*    pElemParticleKind   = NULL;
	SFX_ELEMENT*    pElemLauncher       = NULL;
	SFX_ELEMENT*    pElemForceField     = NULL;
	SFX_ELEMENT*    pElemTexture        = NULL;
	SFX_ELEMENT*    pElemBillboard      = NULL;
	SFX_ELEMENT*    pElemTrack          = NULL;
	SFX_ELEMENT*    pElemModel          = NULL;
	SFX_ELEMENT*    pElemMeteor         = NULL;
	SFX_ELEMENT*    pElemMagicParpet    = NULL;
	SFX_ELEMENT*    pElemChannel        = NULL;
    SFX_ELEMENT*    pElemSound          = NULL;
	SFX_ELEMENT*    pElemSelectMark     = NULL;
    SFX_ELEMENT*    pElemExp            = NULL;
    SFX_ELEMENT*    pElemBoxProj        = NULL;
	size_t nRet     = 0;
	DWORD  i        = 0;
	FILE*  fpFile   = NULL;
	DWORD  dwWriteNum = 0;

	hRetCode = UpdateModelAniInfo();
	KG_COM_PROCESS_ERROR(hRetCode);

	fpFile = ::fopen(cszFileName, "wb");
	KG_PROCESS_ERROR(fpFile);

    int nPraticleKindSize = 0;
    for (i = 0; i < m_vecParticleLifeTimeLine.size(); ++i)
    {
        size_t j;
        for (j = 0; j < i; ++j)
            if (m_vecParticleLifeTimeLine[j]->equal(m_vecParticleLifeTimeLine[i]))
                break;
        if (j < i)
            continue;
        nPraticleKindSize++;
    }

	Header.dwID         = SFX_ID;
	Header.dwVersion    = SFX_VERSION;
	Header.dwFileLength = 0;
	Header.dwElementNum =
		1 +                                           // General Information
		(DWORD)nPraticleKindSize                 +    // Particle Kind
		(DWORD)m_vecSFXParticleLauncher.size()   +    // Particle Launcher
		(DWORD)m_vecSFXParticleForceField.size() +    // Force Field
		(DWORD)m_vecTexture.size()               +    // Texture
		(DWORD)m_vecBillboard.size()             +    // Billboard
		(DWORD)m_vecTrack.size()                 +    // Track
		(DWORD)m_vecMeteor.size()                +    // Meteor
		(DWORD)m_vecModelsInfo.size()            +    // Models
        (DWORD)m_tlSoundKey.size()              +
        //(DWORD)m_vecSoundKey.size()              +    // Sound
        (DWORD)m_vecMagicParpet.size()           +    // MagicParpet
        (DWORD)m_vecBoxProj.size()                    // Box Projection 
        ;

	if (m_pChannel)                                   // Channel 
		Header.dwElementNum++;

    if (ExpEffectValid())                             // Exp Effect
        Header.dwElementNum++;          

	if (m_pSelectMark)                                //ProjectionTexture
		Header.dwElementNum++;

	ASSERT(m_vecTexture.size() != 0);

	if (nPraticleKindSize > 0)
	{
		pElemParticleKind = new SFX_ELEMENT[nPraticleKindSize];
		KG_PROCESS_ERROR(pElemParticleKind);
	}

	if (m_vecSFXParticleLauncher.size() > 0)
	{
		pElemLauncher = new SFX_ELEMENT[m_vecSFXParticleLauncher.size()];
		KG_PROCESS_ERROR(pElemLauncher);
	}

	if (m_vecSFXParticleForceField.size() > 0)
	{
		pElemForceField = new SFX_ELEMENT[m_vecSFXParticleForceField.size()];
		KG_PROCESS_ERROR(pElemForceField);
	}

	if (m_vecTexture.size() > 0)
	{
		pElemTexture = new SFX_ELEMENT[m_vecTexture.size()];
		KG_PROCESS_ERROR(pElemTexture);
	}

	if (m_vecBillboard.size() > 0)
	{
		pElemBillboard = new SFX_ELEMENT[m_vecBillboard.size()];
		KG_PROCESS_ERROR(pElemBillboard);
	}

    if (m_vecBoxProj.size() > 0)
    {
        pElemBoxProj = new SFX_ELEMENT[m_vecBoxProj.size()];
        KG_PROCESS_ERROR(pElemBoxProj);
    }

	if (m_vecTrack.size() > 0)
	{
		pElemTrack = new SFX_ELEMENT[m_vecTrack.size()];
		KG_PROCESS_ERROR(pElemTrack);
	}

	if (m_vecModelsInfo.size())
	{
		pElemModel = new SFX_ELEMENT[m_vecModelsInfo.size()];
		KG_PROCESS_ERROR(pElemModel);
	}

	if (m_vecMeteor.size() > 0)
	{
		pElemMeteor = new SFX_ELEMENT[m_vecMeteor.size()];
		KG_PROCESS_ERROR(pElemMeteor);
	}

	if (m_vecMagicParpet.size() > 0)
	{
		pElemMagicParpet = new SFX_ELEMENT[m_vecMagicParpet.size()];
		KG_PROCESS_ERROR(pElemMagicParpet);
	}

	if (m_pChannel)
	{
		pElemChannel = new SFX_ELEMENT;
		KG_PROCESS_ERROR(pElemChannel);
	}

    if (m_tlSoundKey.size() > 0)
    {
        pElemSound = new SFX_ELEMENT[m_tlSoundKey.size()];
        KG_PROCESS_ERROR(pElemSound);
    }

    /*if (m_vecSoundKey.size() > 0)
    {
        pElemSound = new SFX_ELEMENT[m_vecSoundKey.size()];
        KG_PROCESS_ERROR(pElemSound);
    }*/

	if (m_pSelectMark)
	{
		pElemSelectMark = new SFX_ELEMENT;
		KG_PROCESS_ERROR(pElemSelectMark);
    }

    if (ExpEffectValid())
    {
        pElemExp = new SFX_ELEMENT;
        KG_PROCESS_ERROR(pElemExp);
    }

	nRet = ::fseek(
		fpFile, sizeof(SFX_FILE_HEADER) + sizeof(SFX_ELEMENT) * Header.dwElementNum, SEEK_CUR
		);
	KG_PROCESS_ERROR(nRet == 0);

	ElemGeneral.dwElementID     = SFX_FEID_GENERAL_INFORMATION;
	ElemGeneral.dwElementOffset = ::ftell(fpFile);
	dwWriteNum                 += WriteGeneralInformationBlock(fpFile);
    
    int n = 0;
	for (i = 0; i < (DWORD)m_vecParticleLifeTimeLine.size(); ++i)
	{
        size_t j;
        for (j = 0; j < i; ++j)
            if (m_vecParticleLifeTimeLine[j]->equal(m_vecParticleLifeTimeLine[i]))
                break;
        if (j < i)
            continue;

		pElemParticleKind[n].dwElementID = SFX_FEID_PARTICLE_KIND;
		pElemParticleKind[n].dwElementOffset = ::ftell(fpFile);
		dwWriteNum += WriteParticleKindBlock(fpFile, i);
        n++;
	}

    for (i = 0; i < m_vecSFXParticleLauncher.size(); ++i)
    {
        pElemLauncher[i].dwElementID = SFX_FEID_LAUNCHER;
        pElemLauncher[i].dwElementOffset = ::ftell(fpFile);
        dwWriteNum += WriteLauncherBlock(fpFile, i);
    }

	for (i = 0; i < m_vecTrack.size(); ++i)
	{
		pElemTrack[i].dwElementID = SFX_FEID_TRACK;
		pElemTrack[i].dwElementOffset = ::ftell(fpFile);
		dwWriteNum += WriteTrackBlock(fpFile, i);
	}

	for (i = 0; i < m_vecSFXParticleForceField.size(); ++i)
	{
		pElemForceField[i].dwElementID = SFX_FEID_FORCE_FIELD;
		pElemForceField[i].dwElementOffset = ::ftell(fpFile);
		dwWriteNum += WriteForceFieldBlock(fpFile, i);
	}

	for (i = 0; i < m_vecTexture.size(); ++i)
	{
		pElemTexture[i].dwElementID = SFX_FEID_TEXTURE;
		pElemTexture[i].dwElementOffset = ::ftell(fpFile);
		dwWriteNum += WriteTextureBlock(fpFile, i);
	}

	for (i = 0; i < m_vecBillboard.size(); ++i)
	{
		pElemBillboard[i].dwElementID = SFX_FEID_BILLBOARD;
		pElemBillboard[i].dwElementOffset = ::ftell(fpFile);
		dwWriteNum += WriteBillboardBlock(fpFile, i);
	}

    for (i = 0; i < m_vecBoxProj.size(); ++i)
    {
        pElemBoxProj[i].dwElementID = SFX_FEID_BOX_PROJ;
        pElemBoxProj[i].dwElementOffset = ::ftell(fpFile);
        dwWriteNum += WriteBoxProjBlock(fpFile, i);
    }

	for (i = 0; i < m_vecModelsInfo.size(); i++)
	{
		pElemModel[i].dwElementID = SFX_FEID_MODEL;
		pElemModel[i].dwElementOffset = ::ftell(fpFile);
		dwWriteNum += WriteModelBlock(fpFile, i);
	}

	for (i = 0; i < m_vecMeteor.size(); ++i)
	{
		pElemMeteor[i].dwElementID = SFX_FEID_METEOR;
		pElemMeteor[i].dwElementOffset = ::ftell(fpFile);
		dwWriteNum += WriteMeteorBlock(fpFile, i);
	}

    for (i = 0; i < m_vecMagicParpet.size(); ++i)
    {
        pElemMagicParpet[i].dwElementID = SFX_FEID_MAGIC_PARPET;
        pElemMagicParpet[i].dwElementOffset = ::ftell(fpFile);
        dwWriteNum += WriteMagicParpetBlock(fpFile, i);
    }

	if (pElemChannel)
	{
		pElemChannel->dwElementID = SFX_FEID_CHANNEL;
		pElemChannel->dwElementOffset = ::ftell(fpFile);
		dwWriteNum += WriteChannelBlock(fpFile);
	}

    for (i = 0; i < m_tlSoundKey.size(); ++i)
    {
        pElemSound[i].dwElementID = SFX_FEID_SOUND;
        pElemSound[i].dwElementOffset = ::ftell(fpFile);
        dwWriteNum += WriteSoundBlock(fpFile, i);
    }

    //for (i = 0; i < m_vecSoundKey.size(); ++i)
    //{
    //    pElemSound[i].dwElementID = SFX_FEID_SOUND;
    //    pElemSound[i].dwElementOffset = ::ftell(fpFile);
    //    dwWriteNum += WriteSoundBlock(fpFile, i);
    //}

	if (pElemSelectMark)
	{
		pElemSelectMark->dwElementID = SFX_FEID_SELECTMARK;
		pElemSelectMark->dwElementOffset = ::ftell(fpFile);
		dwWriteNum += WriteSelectMarkBlock(fpFile);
	}

    if (pElemExp)
    {
        pElemExp->dwElementID = SFX_FEID_EXP;
        pElemExp->dwElementOffset = ::ftell(fpFile);
        dwWriteNum += WriteExpBlock(fpFile);
    }

	nRet = ::fseek(fpFile, sizeof(SFX_FILE_HEADER), SEEK_SET);
	KG_PROCESS_ERROR(nRet == 0);

	nRet = ::fwrite(&ElemGeneral, 1, sizeof(SFX_ELEMENT), fpFile);
	KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT));
	dwWriteNum += sizeof(SFX_ELEMENT);

	if (pElemParticleKind)
	{
		nRet = ::fwrite(pElemParticleKind, 1, sizeof(SFX_ELEMENT) * nPraticleKindSize, fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * nPraticleKindSize);
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)nPraticleKindSize;
	}

	if (pElemTrack)
	{
		nRet = ::fwrite(pElemTrack, 1, sizeof(SFX_ELEMENT) * m_vecTrack.size(), fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_vecTrack.size());
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)m_vecTrack.size();
	}

	if (pElemLauncher)
	{
		nRet = ::fwrite(pElemLauncher, 1, sizeof(SFX_ELEMENT) * m_vecSFXParticleLauncher.size(), fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_vecSFXParticleLauncher.size());
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)m_vecSFXParticleLauncher.size();
	}

	if (pElemForceField)
	{
		nRet = ::fwrite(pElemForceField, 1, sizeof(SFX_ELEMENT) * m_vecSFXParticleForceField.size(), fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_vecSFXParticleForceField.size());
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)m_vecSFXParticleForceField.size();
	}

	if (pElemTexture)
	{
		nRet = ::fwrite(pElemTexture, 1, sizeof(SFX_ELEMENT) * m_vecTexture.size(), fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_vecTexture.size());
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)m_vecTexture.size();
	}

	if (pElemBillboard)
	{
		nRet = ::fwrite(pElemBillboard, 1, sizeof(SFX_ELEMENT) * m_vecBillboard.size(), fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_vecBillboard.size());
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)m_vecBillboard.size();
	}

    if (pElemBoxProj)
    {
        nRet = ::fwrite(pElemBoxProj, 1, sizeof(SFX_ELEMENT) * m_vecBoxProj.size(), fpFile);
        KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_vecBoxProj.size());
        dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)m_vecBoxProj.size();
    }

	if (pElemModel)
	{
		nRet = ::fwrite(pElemModel, 1, sizeof(SFX_ELEMENT) * m_vecModelsInfo.size(), fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_vecModelsInfo.size());
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * m_vecModelsInfo.size();
	}

	if (pElemMeteor)
	{
		nRet = ::fwrite(pElemMeteor, 1, sizeof(SFX_ELEMENT) * m_vecMeteor.size(), fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_vecMeteor.size());
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)m_vecMeteor.size();
	}

	if (pElemMagicParpet)
	{
		nRet = ::fwrite(pElemMagicParpet, 1, sizeof(SFX_ELEMENT) * m_vecMagicParpet.size(), fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_vecMagicParpet.size());
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)m_vecMagicParpet.size();
	}

	if (pElemChannel)
	{
		nRet = ::fwrite(pElemChannel, 1, sizeof(SFX_ELEMENT), fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT));
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT);
	}

    if (pElemSound)
    {
        nRet = ::fwrite(pElemSound, 1, sizeof(SFX_ELEMENT) *  m_tlSoundKey.size(), fpFile);
        KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_tlSoundKey.size());
        dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)m_tlSoundKey.size();
    }

    //if (pElemSound)
    //{
    //    nRet = ::fwrite(pElemSound, 1, sizeof(SFX_ELEMENT) *  m_vecSoundKey.size(), fpFile);
    //    KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT) * m_vecSoundKey.size());
    //    dwWriteNum += (DWORD)sizeof(SFX_ELEMENT) * (DWORD)m_vecSoundKey.size();
    //}
	
    if (pElemSelectMark)
	{
		nRet = ::fwrite(pElemSelectMark, 1, sizeof(SFX_ELEMENT), fpFile);
		KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT));
		dwWriteNum += (DWORD)sizeof(SFX_ELEMENT);
	}

    if (pElemExp)
    {
        nRet = ::fwrite(pElemExp, 1, sizeof(SFX_ELEMENT), fpFile);
        KG_PROCESS_ERROR(nRet == sizeof(SFX_ELEMENT));
        dwWriteNum += (DWORD)sizeof(SFX_ELEMENT);
    }

	Header.dwFileLength = dwWriteNum + sizeof(SFX_FILE_HEADER);
	nRet = ::fseek(fpFile, 0, SEEK_SET);
	KG_PROCESS_ERROR(nRet == 0);

	nRet = ::fwrite(&Header, 1, sizeof(SFX_FILE_HEADER), fpFile);
	KG_PROCESS_ERROR(nRet == sizeof(SFX_FILE_HEADER));

	hResult = S_OK;
Exit0 :
	if (fpFile)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	SAFE_DELETE_ARRAY(pElemModel);
	SAFE_DELETE_ARRAY(pElemMagicParpet);
	SAFE_DELETE_ARRAY(pElemTrack);
	SAFE_DELETE_ARRAY(pElemMeteor);
	SAFE_DELETE_ARRAY(pElemBillboard);
    SAFE_DELETE_ARRAY(pElemBoxProj);
	SAFE_DELETE_ARRAY(pElemTexture);
	SAFE_DELETE_ARRAY(pElemForceField);
	SAFE_DELETE_ARRAY(pElemLauncher);
	SAFE_DELETE_ARRAY(pElemParticleKind);
    SAFE_DELETE(pElemChannel);
    SAFE_DELETE_ARRAY(pElemSound);
	SAFE_DELETE(pElemSelectMark);
    SAFE_DELETE(pElemExp);

	return hResult;
}


HRESULT KG3DSFX::Reload()
{
	return LoadFromFile(m_scName.c_str(), 0, 0);
}


HRESULT KG3DSFX::Init()
{
	return KG3DModel::Init();
}

void KG3DSFX::ReSet()
{
    m_ParticleContainer.Reset();
}

BOOL KG3DSFX::IsLoaded()
{
	if(m_bInMultiThreadLoadList)
		return FALSE;

	if (!m_bMeshPostLoadProcessed)
		return FALSE;

    for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
        if (!m_vecModelsInfo[s].pSFXModel->IsLoaded())
            return FALSE;

    return TRUE;
}

BOOL KG3DSFX::IsModelLoad()
{
	for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
		if (!m_vecModelsInfo[s].pSFXModel->IsResourceReady(TRUE))
			return FALSE;
	return TRUE;
}

HRESULT GetAniResource(LPCSTR strAnimationName,
                       enuModelPlayAnimationType&  AniType,
                       KG3DAnimationTagContainer*& pTagInput,
                       KG3DClip*& pClipInput,
                       BOOL IsFirstAni);
BOOL IsAniLoaded(enuModelPlayAnimationType  AniType,
                 KG3DAnimationTagContainer* pTagInput,
                 KG3DClip*& pClipInput);

BOOL KG3DSFX::IsAniLoad()
{
    for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
    {
        enuModelPlayAnimationType  AniType = MPAT_INVALIDATE;
        KG3DAnimationTagContainer* pTagInput = NULL;
        KG3DClip* pClipInput = NULL;
        LPCTSTR strAnimationName = NULL;

        if (!m_vecModelsInfo[s].pSFXModel || m_vecModelsInfo[s].pSFXModel->m_vecAnimationLoaded.size() <= 0)
            continue;
        
         m_vecModelsInfo[s].pSFXModel->m_vecAnimationLoaded.front()->GetPathName(&strAnimationName);

        if (FAILED(GetAniResource(strAnimationName,AniType,pTagInput,pClipInput,FALSE)))
		{
			SAFE_RELEASE(pTagInput);
			SAFE_RELEASE(pClipInput);
			return FALSE;
		}

        if (!IsAniLoaded(AniType,pTagInput,pClipInput))
		{
			SAFE_RELEASE(pTagInput);
			SAFE_RELEASE(pClipInput);
			return FALSE;
		}

		SAFE_RELEASE(pTagInput);
		SAFE_RELEASE(pClipInput);
    }

    return TRUE;
}

void KG3DSFX::ClearUp(BOOL bUnBind)
{
    size_t i = 0;

	if(bUnBind)
		UnBind();

    for (i = 0; i < m_vecSFXParticleLauncher.size(); i++)
        SAFE_DELETE(m_vecSFXParticleLauncher[i]);
    m_vecSFXParticleLauncher.clear();

    for(i = 0; i < m_vecParticleLifeTimeLine.size(); i++)
        SAFE_DELETE(m_vecParticleLifeTimeLine[i]);
    m_vecParticleLifeTimeLine.clear();

    for(i = 0; i < m_vecSFXParticleForceField.size(); i++)
        SAFE_DELETE(m_vecSFXParticleForceField[i]);
    m_vecSFXParticleForceField.clear();

    for (i = 0; i < m_vecTexture.size(); ++i)
        SAFE_RELEASE(m_vecTexture[i]);
    m_vecTexture.clear();
    m_vecCuttingNum.clear();

    for (i = 0; i < m_vecBillboard.size(); ++i)
        SAFE_DELETE(m_vecBillboard[i]);
    m_vecBillboard.clear();

    for (i = 0; i < m_vecTrack.size(); ++i)
        SAFE_DELETE(m_vecTrack[i]);
    m_vecTrack.clear();

    for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
    {
        if (m_vecModelsInfo[s].pSFXModel)
        {
            m_vecModelsInfo[s].pSFXModel->SetParentObjMatrix(NULL);
        }
        SAFE_RELEASE(m_vecModelsInfo[s].pSFXModel);
    }
    m_vecModelsInfo.clear();

    for (i = 0; i < m_vecSubsetColor.size(); ++i)
    {
        for (size_t j = 0; j < m_vecSubsetColor[i]->size(); j++)
            SAFE_DELETE(*(m_vecSubsetColor[i]->begin() + j));
        SAFE_DELETE(m_vecSubsetColor[i]);
    }
    m_vecSubsetColor.clear();

    for (i = 0; i < m_timelineModelAniBound.size(); ++i)
        SAFE_DELETE(m_timelineModelAniBound[i]);
    m_timelineModelAniBound.clear();
    
    for (i = 0; i < m_vecMeteor.size(); ++i)
        SAFE_DELETE(m_vecMeteor[i]);
    m_vecMeteor.clear();

	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwThreadID = ::GetCurrentThreadId();
		for (i = 0; i < m_vecMagicParpet.size(); ++i)
		{
			g_ProjTextureRender.DelProjTextureSource(m_vecMagicParpet[i]);
			SAFE_DELETE(m_vecMagicParpet[i]);

			ASSERT(dwThreadID == g_dwMainThreadID);
		}
		m_vecMagicParpet.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	
 	SAFE_DELETE(m_pSelectMark);
    SAFE_DELETE(m_pChannel);

	if(bUnBind)
		SAFE_DELETE(m_pSFXAnimationController);

    SAFE_DELETE(m_pExpEffect);

	for (i = 0; i < m_vecBoxProj.size(); ++i)
		SAFE_DELETE(m_vecBoxProj[i]);
	m_vecBoxProj.clear();

	IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
	if (pSoundShell)
	{
        for (size_t i = 0; i < m_tlSoundKey.size(); ++i)
        {
            int nFrame = 0;
            _SoundInfo* info = m_tlSoundKey.GetDataByIndex((DWORD)i, nFrame);

            if (!info)
                continue;

            for (size_t s = 0; s < info->vSubSound.size(); ++s)
            {
                SFX_SOUND_BLOCK& block = info->vSubSound[s];

                if (!block.dwSound)
                    continue;

                if (block.bContinue)
                {
                    if (block.bLoop)
                        pSoundShell->Stop3DSoundAfterThisTime(block.dwSound);
                }
                else
                {
                    pSoundShell->Stop3DSound(block.dwSound);
                }
            }

            info->vSubSound.clear();
        }

		/*for (DWORD i = 0; i <  m_vecSoundKey.GetSize(); ++i)
		{
			int nFrame = 0;
			SFX_SOUND_BLOCK* pSound = m_vecSoundKey.GetDataByIndex(i, nFrame);
			if (!pSound || !pSound->dwSound)
				continue;

			if (pSound->bContinue)
			{
				if (pSound->bLoop)
					pSoundShell->Stop3DSoundAfterThisTime(pSound->dwSound);
			}
			else
				pSoundShell->Stop3DSound(pSound->dwSound);
		}*/
	}

    //m_vecSoundKey.Clear();
    m_nTexLoadIndex = -1;
    m_nMdlLoadIndex = -1;

    m_Srt.vTrans = D3DXVECTOR3(0.f, 0.f, 0.f);
    D3DXMatrixIdentity(&m_Srt.matRot);
    m_Srt.vScanl = D3DXVECTOR3(1.f, 1.f, 1.f);

}


HRESULT KG3DSFX::UpdateParticleSystem()
{
    HRESULT hResult  = E_FAIL;

    ASSERT(false);


    KG_PROCESS_SUCCESS(m_eSFXAnimationPlay == SFX_AP_PAUSE);

    if (m_fLastParticlFrame == -1.f || m_fLastParticlFrame > m_fCurrentFrame) 
    {
        for (size_t i = 0; i < m_vecSFXParticleLauncher.size(); i++)
            m_vecSFXParticleLauncher[i]->LaunchParticle(m_fCurrentFrame);
        m_ParticleContainer.UpdateParticle(m_fCurrentFrame);
    }
    else
    {
        float step = m_fFixedFrame / GetCurrInterval();//(float)m_dwFrameInterval;  
        float cur = m_fLastParticlFrame + step;

        while (cur <= m_fCurrentFrame)
        {
            for (size_t i = 0; i < m_vecSFXParticleLauncher.size(); i++)
                m_vecSFXParticleLauncher[i]->LaunchParticle(cur);
            m_ParticleContainer.UpdateParticle(cur);
            
            cur += step;
        }

        if (cur - step != m_fCurrentFrame)
        {
            for (size_t i = 0; i < m_vecSFXParticleLauncher.size(); i++)
                m_vecSFXParticleLauncher[i]->LaunchParticle(m_fCurrentFrame);
            m_ParticleContainer.UpdateParticle(m_fCurrentFrame);
        }
    }

Exit1 :
    hResult  = S_OK;
    m_fLastParticlFrame = m_fCurrentFrame;
    return hResult;
}


HRESULT KG3DSFX::Render(unsigned uOption, void* pExtInfo)
{
	KG3DRenderState RenderState;
	D3DXMATRIX		matIdentity;

	if (!m_bMeshPostLoadProcessed)
		return S_OK;

	if (m_fAlpha == 0.0f)
		return S_OK;
			
	D3DXMatrixIdentity(&matIdentity);
	UpdateMatrix();


	/* 
     * launches particles
	 */

    if (m_eSFXAnimationPlay != SFX_AP_PAUSE)
	{
		for (size_t i = 0; i < m_vecSFXParticleLauncher.size(); i++)
			m_vecSFXParticleLauncher[i]->LaunchParticle(m_fCurrentFrame);
	}

	/* 
     * render and framemove models
     */

	if (IsModelLoad())
	{
		for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
		{
			D3DXMATRIX  matSfxScanl;
			D3DXVECTOR3 vSfxScanl;

			m_Srt.ScanlMat(&matSfxScanl);

			if (m_vecModelsInfo[s].pSFXModel)
			{
				D3DXMATRIX  matModelScanl;
				D3DXVECTOR3 vModelScanl;

				m_vecModelsInfo[s].pSFXModel->GetScaling(&vModelScanl);
				D3DXMatrixScaling(&matModelScanl, vModelScanl.x, vModelScanl.y, vModelScanl.z);    

				KG3DMaterial *pMtl = static_cast<KG3DMaterial *>(m_vecModelsInfo[s].pSFXModel->GetMaterial());

				if (!pMtl)
					continue;

				//for (int i = 0; i < (int)pMtl->m_dwNumMaterials; ++i)

				DWORD dwSubNum = min(pMtl->m_dwNumMaterials, (DWORD)m_vecSubsetColor[s]->size());

				for (DWORD i = 0; i < dwSubNum; ++i)
				{
					D3DXCOLOR color;
					(*(m_vecSubsetColor[s]->begin() + i))->GetData(&color, m_fCurrentFrame);
					m_vecModelsInfo[s].pSFXModel->SetModelColor(i, (D3DCOLORVALUE)color);
				}

				D3DXMATRIX matTrack;
				if (m_vecModelsInfo[s].nTrackIndex != -1)
				{
					KG3DSFXTrack *pTrack = NULL;

					if (SUCCEEDED(GetTrack(m_vecModelsInfo[s].nTrackIndex , &pTrack)))
					{
						pTrack->GetTransform(m_fCurrentFrame, &matTrack);
						//m_vecModelsInfo[s].pSFXModel->SetTransformationMatrix(&matTrack);
					}
					else
						m_vecModelsInfo[s].nTrackIndex = -1;
				}

				KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

				if (!pCamera)
					return S_OK;

				if (m_vecModelsInfo[s].dwIsBillBoard)
				{
					D3DXMATRIX matWorld;

					D3DXVECTOR3 vFace  = pCamera->GetCameraFront();
					D3DXVECTOR3 vFront = D3DXVECTOR3(0.f, 0.f, 1.f);
					vFace.y = 0.f;

					D3DXVec3Normalize(&vFace, &vFace);

					D3DXVECTOR3 vCross;
					D3DXVec3Cross(&vCross, &vFront, &vFace);
					float fRot = acosf(D3DXVec3Dot(&vFace, &vFront));

					if (vCross.y < 0.f)
						fRot *= -1.f;

					D3DXMatrixRotationY(&matWorld, fRot);

					matWorld._41 = m_matWorld._41;
					matWorld._42 = m_matWorld._42;
					matWorld._43 = m_matWorld._43;

					if (m_vecModelsInfo[s].nTrackIndex != -1)
					{
						matWorld._41 += matTrack._41;
						matWorld._42 += matTrack._42;
						matWorld._43 += matTrack._43;    
					}

					matWorld = matModelScanl * matSfxScanl * matWorld;

					m_vecModelsInfo[s].pSFXModel->SetParentObjMatrix(NULL);
					m_vecModelsInfo[s].pSFXModel->SetTransformationMatrix(&matWorld);

				}
				else
				{
					D3DXMATRIX matWorld;
					D3DXMatrixIdentity(&matWorld);

					//m_vecModelsInfo[s].pSFXModel->GetTransformationMatrix(&matWorld);

					m_vecModelsInfo[s].pSFXModel->SetParentObjMatrix(&m_matWorld);

					matWorld *= matSfxScanl;

					if (m_vecModelsInfo[s].nTrackIndex != -1)
					{
						matWorld = matWorld * matTrack;
						m_vecModelsInfo[s].pSFXModel->SetTransformationMatrix(&matWorld);
					}
					/*m_vecModelsInfo[s].pSFXModel->SetTransformationMatrix(&matTrack);*/


					//m_vecModelsInfo[s].pSFXModel->SetTransformationMatrix(&matWorld);
				}

				m_vecModelsInfo[s].pSFXModel->Render(RENDER_NONE, NULL);
			}
		}
	}


	/* 
     * render particles
     */

	RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	RenderState.SetRenderState(D3DRS_ZENABLE, TRUE);
	RenderState.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	RenderState.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RenderState.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	RenderState.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	RenderState.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	RenderState.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	RenderState.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    RenderState.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    RenderState.SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture1);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);
	m_ParticleContainer.RenderParticle(this, m_fCurrentFrame);


	/* 
     * render billboards
     */

	if (m_fpCallBackRender && m_dwRenderTarIndex != -1)
		m_fpCallBackRender(m_dwRenderTarIndex, m_pUserData);

	RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	RenderState.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	RenderState.SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	RenderState.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	RenderState.SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	RenderState.SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

	RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
	RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
	RenderState.SetSamplerState(0, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
	RenderState.SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);

	g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture2);

	vector<KG3DSFXBillboard*>::iterator iter = m_vecBillboard.begin();
	while (iter != m_vecBillboard.end())
	{
		if ((*iter)->GetBlendMode() == SFX_BM_SRCALPHA_INVSRCALPHA)
			(*iter)->Render(m_fCurrentFrame);

		++iter;
	}

	RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	iter = m_vecBillboard.begin();
	while (iter != m_vecBillboard.end())
	{
		if ((*iter)->GetBlendMode() == SFX_BM_SRCALPHA_ONE)
			(*iter)->Render(m_fCurrentFrame);

		++iter;
	}

    DWORD dwColorOpSave = 0;
    DWORD dwArg1Save = 0;
    DWORD dwArg2Save = 0;

    DWORD dwColorOpSave1 = 0;
    DWORD dwArg1Save1 = 0;
    DWORD dwArg2Save1 = 0;

    DWORD dwSrcBlendSave = 0;
    DWORD dwDstBlendSave = 0;

    g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLOROP,   &dwColorOpSave);
    g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &dwArg1Save);
    g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &dwArg2Save);

    g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLOROP,   &dwColorOpSave1);
    g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLORARG1, &dwArg1Save1);
    g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLORARG2, &dwArg2Save1);

    RenderState.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    RenderState.SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    RenderState.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    g_pd3dDevice->GetRenderState(D3DRS_SRCBLEND,  &dwSrcBlendSave );
    g_pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &dwDstBlendSave );

    g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
    g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );

    iter = m_vecBillboard.begin();
    while (iter != m_vecBillboard.end())
    {
        if ((*iter)->GetBlendMode() == SFX_BM_SRCALPHA_INVSRCCOLOR)
            (*iter)->Render(m_fCurrentFrame);

        ++iter;
    }

    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   dwColorOpSave);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, dwArg1Save);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, dwArg2Save);

    g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   dwColorOpSave1);
    g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, dwArg1Save1);
    g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, dwArg2Save1);

    g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,  dwSrcBlendSave );
    g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, dwDstBlendSave );



	/* 
     * render meteors
     */

	RenderState.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	RenderState.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	RenderState.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	RenderState.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	RenderState.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	vector<KG3DSFXMeteor*>::iterator iterMeteor = m_vecMeteor.begin();
	while (iterMeteor != m_vecMeteor.end())
	{
		if ((*iterMeteor)->GetBlendMode() == SFX_BM_SRCALPHA_ONE)
		{
			(*iterMeteor)->Update(m_fCurrentFrame);
			(*iterMeteor)->Render();
		}

		++iterMeteor;
	}

	RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	iterMeteor = m_vecMeteor.begin();
	while (iterMeteor != m_vecMeteor.end())
	{
		if ((*iterMeteor)->GetBlendMode() == SFX_BM_SRCALPHA_INVSRCALPHA)
		{
			(*iterMeteor)->Update(m_fCurrentFrame);
			(*iterMeteor)->Render();
		}

		++iterMeteor;
	}


	/* 
     * render magic parpet (AOE)
     */

	for (size_t i = 0; i < m_vecMagicParpet.size(); ++i)
	{
        if (!m_vecMagicParpet[i])
            continue;

		m_vecMagicParpet[i]->Update(m_fCurrentFrame);
		m_vecMagicParpet[i]->Render();
	}


	/* 
     * render channel
	 */

	if (m_pChannel)
		m_pChannel->Render(0);

    /*
     * render box project 
     */

    for (size_t i = 0; i < m_vecBoxProj.size(); ++i)
        m_vecBoxProj[i]->Render();

	return S_OK;
}


HRESULT KG3DSFX::FrameMove()
{
	HRESULT hResult  = E_FAIL;
    BOOL bPlayAnimationFinished = FALSE; 
	
	Helper_UpdateTransformation(TRUE);

	m_bSelfDestructFramemoveFlag = FALSE;
	if (!m_bMeshPostLoadProcessed)
		return S_OK;

	ProcessCommands();

	KG_PROCESS_ERROR(TRUE);//避免warning Exit0没有用
	if (m_eSFXAnimationPlay != SFX_AP_PAUSE)
	{
		float fSeek = 0;

		/* whent call function PlayAnimation m_dwLastTime be set 0 */
		if (m_bModelAnimationStarted == 0)
		{
			m_dwLastTime = g_cGraphicsTool.GetNowTime();

			for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
			{
				if (m_vecModelsInfo[s].pSFXModel)
				{   
					m_vecModelsInfo[s].pSFXModel->PlayAnimation(
                        m_vecModelsInfo[s].dwPlayMode, m_vecModelsInfo[s].fPlaySpeed * m_fSpeed, 0, NULL, NULL, ANICTL_PRIMARY);                      
				}
			}

            /* update model animation information in begin play */
			UpdateModelAniInfo();
			SeekModelAniBySFXFrame(m_fCurrentFrame, &fSeek);
			
            m_bModelAnimationStarted = TRUE;
		}

		DWORD dwCurrentTime = g_cGraphicsTool.GetNowTime();

		for (size_t i = 0; i < m_vecMagicParpet.size(); i++)
		{
			KG3DSFXMagicParpet *pParpet = m_vecMagicParpet[i];
			if (pParpet)
			{
				pParpet->FrameMove();
			}
		}
        /* the time scaling effect not affected itself */
        if (HaveTimeScalingEffect())
            dwCurrentTime = timeGetTime();
		
        /* calculate current sfx frame */
        m_fDelayTime        = (float)(dwCurrentTime - m_dwLastTime);
		float fElapseFrame  = m_fDelayTime / GetCurrInterval();
		m_dwLastTime        = dwCurrentTime;
		m_fCurrentFrame    += fElapseFrame;

        ASSERT(m_dwTotalFrameNum > 1);
        DWORD dwMaxFrame = m_dwTotalFrameNum - 1;

        if (m_fCurrentFrame > (float)(dwMaxFrame))
        {
            if (m_eSFXAnimationPlay == SFX_AP_LOOP)
                m_fCurrentFrame -= ((DWORD)(m_fCurrentFrame) / dwMaxFrame) * dwMaxFrame;
            else if (m_eSFXAnimationPlay == SFX_AP_ONE_TIME)
            {
                m_fCurrentFrame = (float)m_dwTotalFrameNum;
                int nFinish = m_nPlayFinishFlag;
                m_nPlayFinishFlag = TRUE;

                if (!nFinish)
                {
                    if (m_pParentSFXSuede)
                        ((KG3DSFXSuedeAnimationController*)(m_pParentSFXSuede->GetAnimationController(ANICTL_PRIMARY)))->OnFinishPlay();
                    else
                        bPlayAnimationFinished = TRUE;
                }
            }
        }

        /* update sub models */
		for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
		{
			if (!m_vecModelsInfo[s].pSFXModel)
				continue;

            m_vecModelsInfo[s].pSFXModel->FrameMove();

			if (m_vecModelsInfo[s].nCycleFlag)
			{
				if (m_fCurrentFrame < m_fPrevFrame)
					SeekModelAniBySFXFrame(m_fCurrentFrame, &fSeek);

				if (m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->IsPaused())
				{
					m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(FALSE);
					SeekModelAniBySFXFrame(m_fCurrentFrame, &fSeek);
				}
			}
			else
			{
				if (m_fCurrentFrame >= m_vecModelsInfo[s].nLeftFrame && m_fCurrentFrame <= m_vecModelsInfo[s].nRighFrame)
				{
					if (m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->IsPaused())
					{
						m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(FALSE);
						SeekModelAniBySFXFrame(m_fCurrentFrame, &fSeek);
					}
				}
				else
				{
					if (!m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->IsPaused())
					{
						m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
						m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(ANIMATION_SEEK_SET, 0);
					}
				}
			}

		}

		if (m_nPlayFinishFlag != TRUE || bPlayAnimationFinished)
		{	
			if (m_pChannel)
				m_pChannel->FrameMove();

            if (m_pExpEffect)
                m_pExpEffect->FrameMove(m_fCurrentFrame);

            for (size_t i = 0; i < m_vecTrack.size(); ++i)
                m_vecTrack[i]->FrameMove();

            for (size_t i = 0; i < m_vecBoxProj.size(); ++i)
                m_vecBoxProj[i]->FrameMove();

            
            /* the shound key frame */
            //DWORD size = (DWORD)m_vecSoundKey.GetSize();
            IKG3DSoundShell* pSoundShell = g_cEngineManager.GetSound();
			D3DXVECTOR3 vPos = D3DXVECTOR3(m_matWorld._41 / 100.f, m_matWorld._42 / 100.f, m_matWorld._43 / 100.f);
            D3DXVECTOR3 vVel(0.0f, 0.0f, 0.0f);
			if (m_pParentModel)
			{
				vPos = D3DXVECTOR3(m_pParentModel->m_matWorld._41 / 100.f, 
					m_pParentModel->m_matWorld._42 / 100.f, m_pParentModel->m_matWorld._43/ 100.f);
			}
            if (pSoundShell)
            {
                for (size_t i = 0; i < m_tlSoundKey.size(); ++i)
                {
                    int nFrame = 0;
                    _SoundInfo* info = m_tlSoundKey.GetDataByIndex((DWORD)i, nFrame);
                    bool bNeedPlay = false;

                    if (!info)
                        continue;

                    if (m_fCurrentFrame < m_fPrevFrame)
                    {
                        if (nFrame >= m_fPrevFrame || nFrame <= m_fCurrentFrame)
                        {
                            bNeedPlay = true;
                        }
                    }
                    else
                    {
                        if (nFrame >= m_fPrevFrame && nFrame < m_fCurrentFrame)
                        {
                            bNeedPlay = true;
                        }
                    }

                    if (bNeedPlay)
                    {       
                        int nRand = rand() % info->nProbabilitySum;
                        int nProbabilityCount = 0;

                        size_t playIndex = 0;

                        for (playIndex = 0; playIndex < info->vSubSound.size(); ++playIndex)
                        {
                            SFX_SOUND_BLOCK& block = info->vSubSound[playIndex];                            
                            
							nProbabilityCount += block.nProbability;

                            if (nProbabilityCount >= nRand)
                                break;                        
                            
                        }

                        if (playIndex >= info->vSubSound.size())
                            playIndex = (int)info->vSubSound.size() - 1;

                        PlaySound(*info, playIndex);

                        if (playIndex != -1 && pSoundShell->IsPlaying3DSound(info->vSubSound[playIndex].dwSound))
                        {
                            pSoundShell->Set3DSoundPosition(info->vSubSound[playIndex].dwSound, &vPos, &vVel);
                        }
                    }
                }

                /*for (DWORD i = 0; i < size; ++i) 
                {
                    int nFrame = 0;
                
                    SFX_SOUND_BLOCK* pSound = m_vecSoundKey.GetDataByIndex(i, nFrame);
                    
                    if (!pSound)
                        continue;

                    if (m_fCurrentFrame < m_fPrevFrame)
                    {
                        if ((float)nFrame >= m_fPrevFrame || (float)nFrame <= m_fCurrentFrame)
                        {
                            PlaySound(*pSound);
                        }
                    }
                    else
                    {
                        if ((float)nFrame >= m_fPrevFrame && (float)nFrame < m_fCurrentFrame)
                        {
                            PlaySound(*pSound);
                        }
                    }

                    if (pSoundShell->IsPlaying3DSound(pSound->dwSound))
					{
						pSoundShell->Set3DSoundPosition(pSound->dwSound, &vPos, &vVel);
					}
                }*/
            }
		}
        else
        {
           
            for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
            {
                if (m_vecModelsInfo[s].pSFXModel && m_vecModelsInfo[s].pSFXModel->IsLoaded() )
                {
                    m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(ANIMATION_SEEK_SET, 0);
                    m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
                }
            }
        }

       

	}
    else
    {
        if (m_eSFXAnimationPlaySave != m_eSFXAnimationPlay)
        {
			IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();

            for (size_t i = 0; i < m_tlSoundKey.size(); ++i)
            {
                int nFrame = 0;
                _SoundInfo* info = m_tlSoundKey.GetDataByIndex((DWORD)i, nFrame);

                if (!info)
                    continue;

                for (size_t s = 0; s < info->vSubSound.size(); ++s)
                {
                    SFX_SOUND_BLOCK& block = info->vSubSound[s];

                    if (!block.dwSound)
                        continue;

                    if (block.bContinue)
                    {
                        if (block.bLoop)
                        {
                            if (pSoundShell)
                                pSoundShell->Stop3DSoundAfterThisTime(block.dwSound);
                        }
                    }
                    else
                    {
                        if (pSoundShell)
                            pSoundShell->Stop3DSound(block.dwSound);
                    }
                }
            }

            /*
            for (DWORD i = 0; i <  m_vecSoundKey.GetSize(); ++i)
            {
                int nFrame = 0;
                SFX_SOUND_BLOCK* pSound = m_vecSoundKey.GetDataByIndex(i, nFrame);
                if (!pSound || !pSound->dwSound)
                    continue;

                if (pSound->bContinue)
                {
                    if (pSound->bLoop)
					{
						if (pSoundShell)
							pSoundShell->Stop3DSoundAfterThisTime(pSound->dwSound);
					}
                }
                else
				{
					if (pSoundShell)
						pSoundShell->Stop3DSound(pSound->dwSound);
				}
            } */
        }
    }

    for (size_t i = 0; i < m_vecSFXParticleLauncher.size(); i++)
        m_vecSFXParticleLauncher[i]->FrameMove();

    m_fPrevFrame = m_fCurrentFrame;
    m_eSFXAnimationPlaySave = m_eSFXAnimationPlay;

	hResult = S_OK;
Exit0 :
    if (bPlayAnimationFinished)
    {
        IKG3DAnimationController* pController = GetAnimationController(ANICTL_PRIMARY);

        if (pController)
        {
            IKG3DAnimationEventHandler* pEventHandler = pController->GetEventHandler();
            if (pEventHandler)
            {
                pEventHandler->OnPlayAnimationFinish(pController);
            }
        }
    }

	return hResult;
}

HRESULT KG3DSFX::PlaySound(_SoundInfo& soundInfo, size_t index)
{
    ASSERT(index < soundInfo.vSubSound.size());

	if (((int)index) < 0 || index >= soundInfo.vSubSound.size())
		return E_FAIL;

    D3DXVECTOR3 vPos = D3DXVECTOR3(m_matWorld._41 / 100.f, m_matWorld._42 / 100.f, m_matWorld._43 / 100.f);
    D3DXVECTOR3 vVel(0.0f, 0.0f, 0.0f);

    IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();

    if (!pSoundShell)
        return E_FAIL;

    SFX_SOUND_BLOCK& playSound = soundInfo.vSubSound[index];

    switch (playSound.dwMode)
    {
    case SFX_SOUND_MODE_INT :
        {
            for (size_t s = 0; s < soundInfo.vSubSound.size(); ++s)
            {
                pSoundShell->Stop3DSound(soundInfo.vSubSound[s].dwSound);
            }

            break; 
        }

    case SFX_SOUND_MODE_WAIT :
        {
            for (size_t s = 0; s < soundInfo.vSubSound.size(); ++s)
            {
                if (pSoundShell->IsPlaying3DSound(soundInfo.vSubSound[s].dwSound))
                    return S_OK;
            }
        }

        break;

    case SFX_SOUND_MODE_INT_ALL :
        {
            for (size_t i = 0; i < m_tlSoundKey.size(); ++i)
            {
                int nFrame = 0;
                _SoundInfo* info = m_tlSoundKey.GetDataByIndex((DWORD)i, nFrame);

                if (!info)
                    continue;

                for (size_t s = 0; s < info->vSubSound.size(); ++s)
                {
                    SFX_SOUND_BLOCK& block = info->vSubSound[s];

                    if (!block.dwSound)
                        continue;

                    pSoundShell->Stop3DSound(block.dwSound);
                }
            }
        }
        break;

    default :
        break;
    }

    if (playSound.bLoop)
    {
        if (pSoundShell->IsPlaying3DSound(playSound.dwSound))
            return S_OK;
    }

    // null sound key 
    if (!(*playSound.szFileName))
        return S_OK;

    HRESULT hr = pSoundShell->Play3DSound(
        CHARACTER_SOUND, 
        playSound.szFileName,
        1.0f, //Sound.fVolume / 100.f, 
        playSound.bLoop,
        &vPos, 
        &vVel, 
        g_cEngineOption.fMinSoundRange, //Sound.fMinDis,
        DS3D_DEFAULTMAXDISTANCE, //Sound.fMaxDis,
        &playSound.dwSound
        );

    return hr;

}
//
//HRESULT KG3DSFX::PlaySound(SFX_SOUND_BLOCK& Sound)
//{
//    D3DXVECTOR3 vPos = D3DXVECTOR3(m_matWorld._41 / 100.f, m_matWorld._42 / 100.f, m_matWorld._43 / 100.f);
//    D3DXVECTOR3 vVel(0.0f, 0.0f, 0.0f);
//
//	IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
//
//	if (!pSoundShell)
//		return E_FAIL;
//    
//    switch (Sound.dwMode)
//    {
//    case SFX_SOUND_MODE_INT :
//		if (pSoundShell)
//			pSoundShell->Stop3DSound(Sound.dwSound);
//        break;  
//
//    case SFX_SOUND_MODE_WAIT :
//        if (pSoundShell->IsPlaying3DSound(Sound.dwSound))
//            return S_OK;
//        break;
//
//    case SFX_SOUND_MODE_INT_ALL :
//        for (DWORD i = 0; i <  m_vecSoundKey.GetSize(); ++i)
//        {
//            int nFrame = 0;
//            
//            SFX_SOUND_BLOCK* pSound = m_vecSoundKey.GetDataByIndex(i, nFrame);
//
//            if (!pSound)
//                continue;
//
//			pSoundShell->Stop3DSound(pSound->dwSound);
//        }
//        break;
//
//    default :
//        break;
//    }
//
//    if (Sound.bLoop)
//    {
//        if (pSoundShell->IsPlaying3DSound(Sound.dwSound))
//            return S_OK;
//    }
//
//    HRESULT hr = pSoundShell->Play3DSound(
//        CHARACTER_SOUND, 
//        Sound.szFileName,
//        1.0f, //Sound.fVolume / 100.f, 
//        Sound.bLoop,
//        &vPos, 
//        &vVel, 
//        g_cEngineOption.fMinSoundRange, //Sound.fMinDis,
//        DS3D_DEFAULTMAXDISTANCE, //Sound.fMaxDis,
//        &Sound.dwSound
//        );
//    return hr;
//}

HRESULT KG3DSFX::OnLostDevice()
{
    KG3DModel::OnLostDevice();
    
    if (m_pChannel)
        m_pChannel->OnLostDevice();

	if (m_pSelectMark)
		m_pSelectMark->OnLostDevice();

    for (size_t i = 0; i < m_vecMeteor.size(); i++)
        m_vecMeteor[i]->OnLostDevice();

    for (size_t i = 0; i < m_vecBillboard.size(); i++)
        m_vecBillboard[i]->OnLostDevice();

    for (size_t i = 0; i < m_vecMagicParpet.size(); i++)
        m_vecMagicParpet[i]->OnLostDevice();

    for (size_t i = 0; i < m_vecBoxProj.size(); ++i)
        m_vecBoxProj[i]->OnLostDevice();

    return S_OK;
}

HRESULT KG3DSFX::OnResetDevice()
{
    KG3DModel::OnResetDevice();

    if (m_pChannel)
        m_pChannel->OnResetDevice();

	if (m_pSelectMark)
		m_pSelectMark->OnResetDevice();

    for (size_t i = 0; i < m_vecMeteor.size(); i++)
        m_vecMeteor[i]->OnResetDevice();

    for (size_t i = 0; i < m_vecBillboard.size(); i++)
        m_vecBillboard[i]->OnResetDevice();

    for (size_t i = 0; i < m_vecMagicParpet.size(); i++)
        m_vecMagicParpet[i]->OnResetDevice();

    for (size_t i = 0; i < m_vecBoxProj.size(); ++i)
        m_vecBoxProj[i]->OnResetDevice();

    return S_OK;
}

void KG3DSFX::ScanlAllTimeLine(float fScanl)
{
    size_t i = 0;

    for (i = 0; i < GetParticleLifeTimeLineNum(); ++i)
    {
        KG3DSFXParticleLifeTimeLine* pParticleKind = NULL;
        GetParticleLifeTimeLine(static_cast<DWORD>(i), (IEKG3DSFXParticleLifeTimeLine**)&pParticleKind);
        ASSERT(pParticleKind);
        pParticleKind->ScanlTimeLine(fScanl);
    }

    for (i = 0; i < GetForceFieldNum(); ++i)
    {
        KG3DSFXParticleForceField* pForce = NULL;
       GetForceField(static_cast<DWORD>(i), (IEKG3DSFXParticleForceField**)&pForce);
        ASSERT(pForce);
        pForce->ScanlTimeLine(fScanl);
    }

    for (i = 0; i < GetBillboardNum(); ++i)
    {
        KG3DSFXBillboard *pBillboard = NULL;
        GetBillboard(static_cast<DWORD>(i), &pBillboard);
        ASSERT(pBillboard);
        pBillboard->ScanlTimeLine(fScanl);
    }

    for (i = 0; i < GetParticleLauncherNum(); ++i)
    {   
        KG3DSFXParticleLauncher *pLauncher = NULL;
        GetParticleLauncher(static_cast<DWORD>(i), &pLauncher);
        ASSERT(pLauncher);
        pLauncher->ScanlTimeLine(fScanl);
    }


    for (i = 0; i < m_vecSubsetColor.size(); ++i)
    {
        KG3DSFX::MDL_COLOR_LINE_T* pColorLine = m_vecSubsetColor[i];
        ASSERT(pColorLine);
        for (size_t i = 0; i < pColorLine->size(); ++i)
        {
            (*pColorLine)[i]->Scanl(fScanl);
        }     
    }

   SetTotalFrameNum(static_cast<DWORD>((static_cast<float>(GetTotalFrameNum()) * fScanl) + 0.5f));

}

HRESULT KG3DSFX::RenderSelectMark(DWORD color /* = 0xFFFFFFFF */)
{
    ComputeBBox();
	g_cGraphicsTool.DrawAABBox(&m_BBox, color);

	return S_OK;
}


void KG3DSFX::TransformationFrameMove()
{
	for (int i = 0; i < (int)m_vecSFXParticleLauncher.size(); ++i)
	{
		m_vecSFXParticleLauncher[i]->LaunchParticle(m_fCurrentFrame);
	}
}


HRESULT KG3DSFX::SetCurrentFrame(float fFrame)
{
	m_fCurrentFrame = fFrame;

	float fSeek = 0.0f;
	for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
	{
		if (m_vecModelsInfo[s].nCycleFlag)
			SeekModelAniBySFXFrame(fFrame, &fSeek, s);
		else
		{
			if (
				m_fCurrentFrame >= m_vecModelsInfo[s].nLeftFrame && 
				m_fCurrentFrame <= m_vecModelsInfo[s].nRighFrame
				)
			{
				SeekModelAniBySFXFrame(fFrame, &fSeek, s);
			}
			else
			{
				m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(
					ANIMATION_SEEK_SET, 0
					);
			}
		}

	}

	return S_OK;
}


float KG3DSFX::GetCurrentFrame() const
{
	return m_fCurrentFrame;
}


HRESULT KG3DSFX::PlayAnimation(SFX_ANIMATION_PLAY sfxAnimationPlay, float fSpeed, int nOffsetTime)
{
    if(IsLoaded() && IsAniLoad())
    {
        return PlayAnimationImpl(sfxAnimationPlay, fSpeed, nOffsetTime);
    }
    else
    {
        return m_Commands.AddCommand(new KG3DSfxCommand_PlayAnimation(sfxAnimationPlay, fSpeed, nOffsetTime), TRUE);
    }

    return S_OK;
}

HRESULT KG3DSFX::PlayAnimationImpl(SFX_ANIMATION_PLAY sfxAnimationPlay, float fSpeed, int nOffsetTime)
{
    m_fPrevFrame = -1;
    m_fLastParticlFrame = -1;
    m_fSpeed = fSpeed;

    if (sfxAnimationPlay == SFX_AP_PAUSE)
    {
        for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
        {
            if (m_vecModelsInfo[s].pSFXModel)
                m_vecModelsInfo[s].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
        }
        m_dwLastTime = 0;
        vector<KG3DSFXMeteor*>::iterator iterMeteor = m_vecMeteor.begin();
        while (iterMeteor != m_vecMeteor.end())
        {
            if ((*iterMeteor)->m_nTrackIndex != -1)
            {
                (*iterMeteor)->SetSampling(FALSE);
            }

            ++iterMeteor;
        }
    }
    else
    {
        m_dwLastTime = g_cGraphicsTool.GetNowTime();
        m_nPlayFinishFlag   = FALSE;
        m_bModelAnimationStarted = FALSE;
        m_dwStartTime = g_cGraphicsTool.GetNowTime();
        vector<KG3DSFXMeteor*>::iterator iterMeteor = m_vecMeteor.begin();
        while (iterMeteor != m_vecMeteor.end())
        {
            if ((*iterMeteor)->m_nTrackIndex != -1)
            {
                (*iterMeteor)->SetSampling(TRUE);
            }
            ++iterMeteor;
        }
    }

    m_eSFXAnimationPlay = sfxAnimationPlay;
    m_fCurrentFrame		= (float)nOffsetTime;

    //GetAnimationController(ANICTL_PRIMARY)->SetMotionExtraInfo(IKG3DAnimationController::TYPE_SFX);
    GetAnimationController(ANICTL_PRIMARY)->SetSpeed(m_fSpeed);

    return S_OK;
}


HRESULT KG3DSFX::PlayAnimation(DWORD  dwPlayType, 
                               LPCSTR cszAnimationName, 
                               FLOAT  fSpeed, 
                               int    nOffsetTime, 
                               PVOID  pReserved, 
                               PVOID pUserdata, 
                               enuAnimationControllerPriority Priority)
{
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && !g_cRecorder.IsLock()  && m_dwRecordID != 0 )
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 4;
		MyParameters.size_tDataSize = MAX_PATH * sizeof(char) +
			sizeof(dwPlayType) + sizeof(fSpeed) + sizeof(nOffsetTime);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(DWORD*)(&pData[MyParameters.nFrom[0]]) = dwPlayType;
		MyParameters.nFrom[1] = sizeof(dwPlayType);
        if (cszAnimationName == NULL)
            pData[MyParameters.nFrom[1]] = NULL;
        else
		    strcpy(&(pData[MyParameters.nFrom[1]]),cszAnimationName);
		MyParameters.nFrom[2] = MAX_PATH * sizeof(char) + sizeof(dwPlayType);
		*(FLOAT*)(&pData[MyParameters.nFrom[2]]) = fSpeed;
		MyParameters.nFrom[3] = MAX_PATH * sizeof(char) + sizeof(dwPlayType) + sizeof(fSpeed);
		*(int*)(&pData[MyParameters.nFrom[3]]) = nOffsetTime;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_PLAYANIMATION,GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}

    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    m_dwModelPlayMode = dwPlayType;

	switch (dwPlayType)
    {
    case ENU_ANIMATIONPLAY_CIRCLE	  :
	case ENU_ANIMATIONPLAY_ADDCURRENT :
        {
            hRetCode = PlayAnimation(SFX_AP_LOOP, fSpeed, nOffsetTime);
            KG_COM_PROCESS_ERROR(hRetCode);
        } 
        break;

    case ENU_ANIMATIONPLAY_ONCE			   :
	case ENU_ANIMATIONPLAY_ADDCURRENT_ONCE :
        {
            hRetCode = PlayAnimation(SFX_AP_ONE_TIME, fSpeed, nOffsetTime);
            KG_COM_PROCESS_ERROR(hRetCode);
        }
        break;

    default:
		KG_PROCESS_ERROR(FALSE);
        break;
    }

    hResult = S_OK;
Exit0 :
	if (bIsPassRecord)
		g_cRecorder.UnLock();
    return hResult;
}

SFX_ANIMATION_PLAY KG3DSFX::GetPlayAnimation() const
{
	return m_eSFXAnimationPlay;
}


HRESULT KG3DSFX::SetFrameInterval(DWORD dwInterval)
{
	m_dwFrameInterval = dwInterval;

	return S_OK;
}


DWORD KG3DSFX::GetFrameInterval() const
{
	return m_dwFrameInterval;
}

float KG3DSFX::GetCurrInterval() const
{
    return static_cast<float>(m_dwFrameInterval) / m_fSpeed;
}


DWORD KG3DSFX::GetTotalFrameNum() const
{
	return m_dwTotalFrameNum;
}

DWORD KG3DSFX::GetParticleNum() const 
{
    return m_ParticleContainer.m_dwActiveNumber / 2;
}

DWORD KG3DSFX::GetMaxParticleNum() const 
{
    return m_dwMaxParticleNum;
}

void KG3DSFX::SetMaxParticleNum(DWORD dwNum)
{
    m_dwMaxParticleNum = dwNum;
}

int KG3DSFX::GetPlayTime() const 
{
    return g_cGraphicsTool.GetNowTime() - m_dwStartTime;
}

BOOL KG3DSFX::OnProcessCommands(KG3DModelCommandBase* pCmd)
{
    switch(pCmd->m_eType)
    {
    case CMD_PlayAnimation:
        {
            KG3DSfxCommand_PlayAnimation* pSfxAni = static_cast<KG3DSfxCommand_PlayAnimation*>(pCmd);

            if (IsLoaded() && IsAniLoad())
            {
                PlayAnimationImpl(pSfxAni->dwPlayType, pSfxAni->fSpeed, pSfxAni->nOffsetTime);
                return TRUE;
            }
        }
        break;
    default :
        return KG3DModel::OnProcessCommands(pCmd);
        break;
    }

    return FALSE;
}

HRESULT KG3DSFX::SetTotalFrameNum(DWORD dwTotalFrame)
{
	ASSERT( dwTotalFrame > 0 );
	m_dwTotalFrameNum = dwTotalFrame;

	return S_OK;
}

void KG3DSFX::_SetPhysicsScene(KG3DPhysiscScene *pScene)
{
	KG3DModel::_SetPhysicsScene(pScene);

	for (size_t i = 0; i < m_vecModelsInfo.size(); i++)
	{
		KG3DModel *pSFXModel = m_vecModelsInfo[i].pSFXModel;
		if (pSFXModel)
		{
			pSFXModel->SetPhysicsScene(pScene);
		}
	}
}

HRESULT KG3DSFX::_BindToSocket(KG3DModel *pModel, const char cszSocketName[])
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    hRetCode = KG3DModel::_BindToSocket(pModel, cszSocketName);
    KG_COM_PROCESS_ERROR(hRetCode);

	SetPhysicsScene(pModel->m_lpPhysicsScene);

    if (m_pExpEffect)
        m_pExpEffect->OnSfxBind(pModel);

    hResult = S_OK;
Exit0 :
    return hResult;
}

HRESULT KG3DSFX::_BindToBone(KG3DModel* pModel, const char* cszSocketName)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    hRetCode = KG3DModel::_BindToBone(pModel, cszSocketName);
    KG_COM_PROCESS_ERROR(hRetCode);

	SetPhysicsScene(pModel->m_lpPhysicsScene);
		
    if (m_pExpEffect)
        m_pExpEffect->OnSfxBind(pModel);

    hResult = S_OK;
Exit0 :
    return hResult;
}

HRESULT KG3DSFX::UnBindFromOther()
{
    if (FAILED(KG3DModel::UnBindFromOther()))
        return E_FAIL;

	SetPhysicsScene(NULL);

    if (m_pExpEffect)
        m_pExpEffect->OnSfxUnbind();

    return S_OK;

}

HRESULT KG3DSFX::BindToModel(KG3DModel *pModel)
{
	ASSERT(pModel);

	m_pParentModel = pModel;
	m_pParentModel  = pModel;
	m_pParentMatrix = NULL;
	return S_OK;
}


HRESULT KG3DSFX::UnBind()
{
	UnBindFromOther();
    m_pParentModel = NULL;

	return S_OK;
}


D3DXMATRIX *KG3DSFX::GetCurrentWorldMatrix()
{
	return &m_matWorld;
}

BOOL KG3DSFX::IsBBoxChanged()
{
    for (size_t i = 0; i < m_vecModelsInfo.size(); i++)
    {
        if (m_vecModelsInfo[i].pSFXModel->IsBBoxChanged())
        {
            return TRUE;
        }
    }

    return FALSE;
}

void KG3DSFX::SetBBoxChanged(BOOL bBBoxChanged)
{
    size_t uModelsCount = m_vecModelsInfo.size();
    if (uModelsCount > 0)
    {
        for (size_t i = 0; i < uModelsCount; i++)
        {
            m_vecModelsInfo[i].pSFXModel->SetBBoxChanged(bBBoxChanged);
        }
        ComputeBBox();
    }
}

HRESULT KG3DSFX::ComputeBBox()
{
    D3DXVECTOR3 BoxPos[8];
    D3DXMATRIX  MatTrans;

    D3DXVECTOR3 vScanl;
    D3DXVECTOR3 vTrans;
    D3DXMATRIX  matRot;
    D3DXMATRIX  matScal;
    D3DXMATRIX  matInverse;

	if(!IsLoaded())
		return S_OK;

    m_BBox.Clear();
    m_BBoxLocal.Clear();

    UpdateMatrix();

    size_t i = 0;
    
    MatrixExract(vScanl, vTrans, matRot, &m_matWorld);
    D3DXMatrixIdentity(&MatTrans);
    
    MatTrans._11 = vScanl.x;
    MatTrans._22 = vScanl.y;
    MatTrans._33 = vScanl.z;

    MatTrans._41 = vTrans.x;
    MatTrans._42 = vTrans.y;
    MatTrans._43 = vTrans.z;


    if (m_staticAABB.bInit) 
    {
        m_staticAABB.TransformCoord(BoxPos, &m_matWorld);
        m_BBox.AddPosition(BoxPos, 8);
    }

    for (i = 0; i < m_vecSFXParticleLauncher.size(); ++i)
    {
        AABBOX  aabbox;
        m_vecSFXParticleLauncher[i]->GetAABBox(&aabbox);
        aabbox.TransformCoord(BoxPos, &m_matWorld);
        m_BBox.AddPosition(BoxPos, 8);
    }

    for (i = 0; i < m_vecBillboard.size(); ++i)
    {
        AABBOX  aabbox;
        m_vecBillboard[i]->GetAABBox(&aabbox);
        aabbox.TransformCoord(BoxPos, &MatTrans);
        m_BBox.AddPosition(BoxPos, 8);
    }

    for (i = 0; i < m_vecBoxProj.size(); ++i)
    {
        AABBOX  aabbox;
        m_vecBoxProj[i]->GetAABBox(&aabbox);
        m_BBox.AddPosition(aabbox.A);
        m_BBox.AddPosition(aabbox.B);
    }

	if (m_pChannel)
	{
		m_BBox.AddPosition(m_pChannel->m_vSourPos);
		m_BBox.AddPosition(m_pChannel->m_vDestPos);
	}

    for (int i = 0; i < (int)m_vecModelsInfo.size(); i++)
    {
        D3DXVECTOR3 vA;
        D3DXVECTOR3 vB;
    
        m_vecModelsInfo[i].pSFXModel->m_matWorld._41 = m_matWorld._41;
        m_vecModelsInfo[i].pSFXModel->m_matWorld._42 = m_matWorld._42;
        m_vecModelsInfo[i].pSFXModel->m_matWorld._43 = m_matWorld._43;
        
        m_vecModelsInfo[i].pSFXModel->ComputeBBox();
        m_vecModelsInfo[i].pSFXModel->GetBBox(vA, vB);

        m_BBox.AddPosition(vA);
        m_BBox.AddPosition(vB);
    }

    D3DXMatrixInverse(&matInverse, NULL, &m_matWorld);
    m_BBox.TransformCoord(BoxPos, &matInverse);
    m_BBoxLocal.AddPosition(BoxPos, 8);

	return S_OK;
}


BOOL KG3DSFX::IsRayIntersect(D3DXVECTOR3 &Inter, D3DXVECTOR3 Origin, D3DXVECTOR3 Direction)
{
	UpdateMatrix();

	D3DXVECTOR3 vM0   = (m_BBox.A + m_BBox.B) * 0.5f;
	D3DXVECTOR3 vM1   = Origin + Direction * 100;
	D3DXVECTOR3 vM1M0 = vM0 - vM1;

	D3DXVec3Cross(&vM1, &Direction, &vM1M0);
	float fDirection = D3DXVec3Length(&Direction);
	float fS = D3DXVec3Length(&vM1);
	fS = fDirection / fS;
	float fRad = D3DXVec3Length(&(vM0 - m_BBox.A));

	if (fS < fRad)
	{
		Inter = D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43);
		return TRUE;
	}
	else
		return FALSE;
}


HRESULT KG3DSFX::SetAlpha(float fAlpha, BOOL bUseSpecial)
{
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 2;
		MyParameters.size_tDataSize = sizeof(fAlpha) + sizeof(bUseSpecial);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(float*)(&pData[MyParameters.nFrom[0]]) = fAlpha;
		MyParameters.nFrom[1] = sizeof(fAlpha);
		*(BOOL*)(&pData[MyParameters.nFrom[1]]) = bUseSpecial;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_SETALPHA,
			GetCurrentTime(),MyParameters);
	}
	m_fAlpha = fAlpha;
	return S_OK;
}


void KG3DSFX::SetScaleSize(float f)
{
	m_fSizeScale = f;
}


float KG3DSFX::GetScaleSize()
{
	return m_fSizeScale;
}



int KG3DSFX::GetIsShockWave()
{
	return m_nIsSockWave;
}


int KG3DSFX::GetIsCloseFloor()
{
	return m_nCloseFloorFlag;
}


HRESULT KG3DSFX::SetIsShockWave(int nValue)
{
	m_nIsSockWave = nValue;
	return S_OK;
}


HRESULT KG3DSFX::AddTexture(const char szTextureName[])
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	KG3DTexture* pTexture = NULL;

    KG_PROCESS_ERROR(szTextureName);

	if (szTextureName[0])
	{
		hRetCode = g_cTextureTable.LoadResourceFromFile(
			szTextureName, 
			0, 
			/*TEXOP_DISABLEDTX|*/TEXOP_RELOAD, 
			(IKG3DResourceBase**)&pTexture
			);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
	}

	hResult = S_OK;

Exit0 :

    m_vecTexture.push_back(pTexture);
    m_vecCuttingNum.push_back(1);
    m_ParticleContainer.SetTextureNum((UINT)m_vecTexture.size());

	return hResult;
}


HRESULT KG3DSFX::RemoveTexture(UINT uTextureIndex)
{
	ASSERT(uTextureIndex < m_vecTexture.size());

	if (m_vecTexture.size() == 1)
    {
        SAFE_RELEASE(m_vecTexture[0]);
		return E_FAIL;
    }

	if (uTextureIndex < m_vecTexture.size())
	{
		SAFE_RELEASE(m_vecTexture[uTextureIndex]);
		m_ParticleContainer.Reset();
		m_vecTexture.erase(m_vecTexture.begin() + uTextureIndex);
		m_vecCuttingNum.erase(m_vecCuttingNum.begin() + uTextureIndex);
		m_ParticleContainer.SetTextureNum((UINT)m_vecTexture.size());
		for (UINT i = 0; i < m_vecParticleLifeTimeLine.size(); ++i)
		{
			if (
				m_vecParticleLifeTimeLine[i]->m_uTextureIndex >= uTextureIndex &&
				m_vecParticleLifeTimeLine[i]->m_uTextureIndex > 0
				)
				m_vecParticleLifeTimeLine[i]->m_uTextureIndex--;
		}

		return S_OK;
	}

	return E_FAIL;
}


DWORD KG3DSFX::GetTextureNum() const
{
	return (DWORD)m_vecTexture.size();
}


const UINT KG3DSFX::GetRandTexFrame(UINT uTexIndex) const
{
	ASSERT(uTexIndex < m_vecTexture.size());
	UINT uNumber = 0;
	GetTextureCutNum(uTexIndex, &uNumber);
	uNumber = uNumber * uNumber - 1;
	if ((int)uNumber < 1)
		uNumber = 1;
	return rand() % uNumber; 
}


HRESULT KG3DSFX::GetTexture(UINT uTextureIndex, KG3DTexture **ppTexture) const
{
	ASSERT(ppTexture);
	ASSERT(uTextureIndex < m_vecTexture.size());

	if (uTextureIndex < m_vecTexture.size())
	{
		(*ppTexture) = m_vecTexture[uTextureIndex];
		return S_OK;
	}

	(*ppTexture) = NULL;
	return E_FAIL;
}


HRESULT KG3DSFX::GetIKG3DTexture(UINT uTextureIndex, IKG3DTexture **ppTexture) const
{
	ASSERT(ppTexture);
	ASSERT(uTextureIndex < m_vecTexture.size());

	if (uTextureIndex < m_vecTexture.size())
	{
		(*ppTexture) = m_vecTexture[uTextureIndex];
		return S_OK;
	}

	(*ppTexture) = NULL;
	return E_FAIL;
}


HRESULT KG3DSFX::SetTexture(UINT uTextureIndex, const char szTextureName[])
{
	ASSERT(szTextureName);
	ASSERT(uTextureIndex < m_vecTexture.size());

	if (uTextureIndex < m_vecTexture.size())
	{

		KG3DTexture *pTexture = NULL;
		HRESULT hr;
		if (szTextureName[0] == '\0')
			hr = S_OK;
		else
		{
			//DWORD loadFlag = TEXOP_DISABLEDTX | TEXOP_RELOAD;
			//if (!g_bClient)
			DWORD loadFlag = TEXOP_DISABLEDTX;
			hr = g_cTextureTable.LoadResourceFromFile(szTextureName, 0, loadFlag, 
				(IKG3DResourceBase **)&pTexture);
		}

		if (SUCCEEDED(hr))
		{
			SAFE_RELEASE(m_vecTexture[uTextureIndex]);
			m_vecTexture[uTextureIndex] = pTexture;
		}

		return hr;
	}

	KGLogPrintf(KGLOG_DEBUG, TEXT("can not open texture : %s, %s"), szTextureName, KG_FUNCTION);

	return E_FAIL;
}


HRESULT KG3DSFX::SetTextureCutNum(UINT uTextureIndex, UINT uCuttingNum)
{
	ASSERT(uTextureIndex < m_vecCuttingNum.size());

	if (uTextureIndex < m_vecCuttingNum.size())
	{
		m_vecCuttingNum[uTextureIndex] = uCuttingNum;
		for (int i = 0; i < (int)m_vecParticleLifeTimeLine.size(); ++i)
		{
			KG3DSFXParticleLifeTimeLine *pLifeTime = 
				m_vecParticleLifeTimeLine[i];
			if (pLifeTime->m_uTextureIndex == uTextureIndex)
			{
				bool bReset = false;
				for (int j = 0; j < (int)pLifeTime->m_nTextureFrameTimeLine.GetSize(); ++j)
				{
					int nFrame;
					int nData;
					if (FAILED(pLifeTime->m_nTextureFrameTimeLine.GetKeyFrameByIndex(&nFrame, &nData, j)))
                        continue;
					if (nData > (int)(uCuttingNum * uCuttingNum - 1) && nData != INT_MAX)
					{
						bReset = true;
						break;
					}
				}

				if (bReset)
				{
					pLifeTime->m_nTextureFrameTimeLine.Clear();
					pLifeTime->m_nTextureFrameTimeLine.InsertKeyFrame(0, 0);
				}
			}
		}

		return S_OK;
	}

	return E_FAIL;
}


HRESULT KG3DSFX::GetTextureCutNum(UINT uTextureIndex, UINT *pCutNum) const
{
	ASSERT(pCutNum);
	ASSERT(uTextureIndex < m_vecCuttingNum.size());

	if (uTextureIndex < m_vecCuttingNum.size())
	{
		(*pCutNum) = m_vecCuttingNum[uTextureIndex];
		return S_OK;
	}

	return E_FAIL;
}


HRESULT KG3DSFX::GetTextureCoord(FRECT* pTextureCoord, UINT uTextureIndex, UINT uFrameIndex)
{
	ASSERT(pTextureCoord);
	ASSERT(uTextureIndex < m_vecCuttingNum.size());

	if (uTextureIndex < m_vecCuttingNum.size())
	{
		UINT uBlocksNum = m_vecCuttingNum[uTextureIndex] * 
			m_vecCuttingNum[uTextureIndex];

		ASSERT(uBlocksNum > uFrameIndex);

		if (uBlocksNum > uFrameIndex)
		{
			UINT uY = uFrameIndex / m_vecCuttingNum[uTextureIndex];
			UINT uX = uFrameIndex - uY * m_vecCuttingNum[uTextureIndex];

			float offset = 1.0f / (float)m_vecCuttingNum[uTextureIndex];
			pTextureCoord->Left = uX * offset;
			pTextureCoord->Width = offset;
			pTextureCoord->Bottom = ( uY + 1 ) * offset;
			pTextureCoord->Height = offset;

			return S_OK;
		}
	}

	return E_FAIL;
}


HRESULT KG3DSFX::GetRenderTargetTextureCount(OUT DWORD* pTexCount)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(pTexCount);
	*pTexCount = 0;
	for (DWORD i = 0; i < m_vecBillboard.size(); i++)
	{
		if (m_vecBillboard[i]->m_dwRenderTargetTexFlag)
		{
			if (m_vecBillboard[i]->GetTexture())
				(*pTexCount)++;
			if (m_vecBillboard[i]->GetTexture2())
				(*pTexCount)++;
		}
	}

	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFX::GetRenderTargetTexture(OUT DWORD* pResourceId, IN DWORD dwTexIndex /* = 0 */)
{
	HRESULT hResult  = E_FAIL;

	KG3DTexture* pTexture = NULL;


	int nRenterTexIndex = -1;

	KGLOG_PROCESS_ERROR(dwTexIndex < m_vecBillboard.size() * 2);
	KGLOG_PROCESS_ERROR(dwTexIndex >=0);
	KGLOG_PROCESS_ERROR(pResourceId);

	for (DWORD i = 0; i < m_vecBillboard.size(); i++)
	{
		if (m_vecBillboard[i]->m_dwRenderTargetTexFlag)
		{
			pTexture = (KG3DTexture*)m_vecBillboard[i]->GetTexture();
			if (pTexture)
				nRenterTexIndex++;
			if (nRenterTexIndex == (int)dwTexIndex)
				break;
			pTexture = (KG3DTexture*)m_vecBillboard[i]->GetTexture2();
			if (pTexture)
				nRenterTexIndex++;
			if (nRenterTexIndex == (int)dwTexIndex)
				break;    
		}
	}

	KGLOG_PROCESS_ERROR(nRenterTexIndex == (int)dwTexIndex);
	KGLOG_PROCESS_ERROR(pTexture);

	pTexture->GetID(pResourceId);

	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFX::SetCallbackRender(CallbackRender fpCallbackRender, void* pUserData)
{
	//if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	//{
	//	g_cRecorder.DelByID(m_dwRecordID);
	//	m_dwRecordID = 0;
	//}
	//if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	//{
	//	KG3DRecorder::Parameters MyParameters;
	//	g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_SETCALLBACKRENDER,
	//		GetCurrentTime(),MyParameters);
	//}

	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	KG3DTexture* pTexture = NULL;

	KG_PROCESS_ERROR(GetBillboardNum() > 0);

	hRetCode = GetRenderTargetTexture(&m_dwRenderTarIndex);
	KG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = g_cTextureTable.GetResource(&pTexture, m_dwRenderTarIndex);
	KG_COM_PROCESS_ERROR(hRetCode);
	_ASSERTE(pTexture);

	pTexture->Release();

	KG_PROCESS_ERROR(pTexture->m_uOption & TEXOP_RENDERTARGET);

	KG_PROCESS_ERROR(fpCallbackRender);
	m_fpCallBackRender = fpCallbackRender;
	m_pUserData = pUserData;

	hResult = S_OK;
Exit0:
	if (FAILED(hResult))
	{
		m_fpCallBackRender = NULL;
		m_pUserData = NULL;
		m_dwRenderTarIndex  = (DWORD)-1;
	}
	return hResult;
}


HRESULT KG3DSFX::ReflushTexture()
{
	// particle

	size_t size = m_vecTexture.size();
	for (size_t i = 0; i < size; i++)
	{
		if (m_vecTexture[i])
			m_vecTexture[i]->ForceReload();
	}

	// billboard

	size = m_vecBillboard.size();
	for (size_t i = 0; i < size; i++)
	{
		KG3DTexture* pTexture = (KG3DTexture*)m_vecBillboard[i]->GetTexture();
		if (pTexture)       
			pTexture->ForceReload();
		pTexture = (KG3DTexture*)m_vecBillboard[i]->GetTexture2();
		if (pTexture)
			pTexture->ForceReload();
	}

	// aoe

    for (size_t i = 0; i < m_vecMagicParpet.size(); ++i)
    {
        if (!m_vecMagicParpet[i])
            continue;

        KG3DTexture* pTexture = static_cast<KG3DTexture*>(m_vecMagicParpet[i]->GetTexture());
     
        if (pTexture)
            pTexture->ForceReload();

    }

	// channel

	if (m_pChannel && m_pChannel->GetTexture())
	{
		m_pChannel->GetTexture()->ForceReload();
	}

	return S_OK;
}


HRESULT KG3DSFX::AddParticleLifeTimeLine(IEKG3DSFXParticleLifeTimeLine **ppLifeTimeLine)
{
	KG3DSFXParticleLifeTimeLine *pLifeTimeLine = new KG3DSFXParticleLifeTimeLine;

	if (pLifeTimeLine == NULL)
		return E_OUTOFMEMORY;

	(*ppLifeTimeLine) = pLifeTimeLine;
	m_vecParticleLifeTimeLine.push_back(pLifeTimeLine);

	return S_OK;
}


HRESULT KG3DSFX::GetParticleLifeTimeLine(
	DWORD dwIndex, IEKG3DSFXParticleLifeTimeLine **ppLifeTimeLine
)
{
	ASSERT(ppLifeTimeLine);
	if (dwIndex < m_vecParticleLifeTimeLine.size())
	{
		(*ppLifeTimeLine) = m_vecParticleLifeTimeLine[dwIndex];
		return S_OK;
	}

	return E_FAIL;
}


HRESULT KG3DSFX::RemoveParticleLifeTimeLine(DWORD dwIndex)
{
	if (dwIndex < m_vecParticleLifeTimeLine.size())
	{
		KG3DSFXParticleLifeTimeLine *pParticleTimeLine = m_vecParticleLifeTimeLine[dwIndex];
		for (int i = 0; i < (int)m_vecSFXParticleLauncher.size(); ++i)
		{
			if (m_vecSFXParticleLauncher[i]->GetParticleLifeTime() == pParticleTimeLine)
				m_vecSFXParticleLauncher[i]->SetParticleLifeTimeLine(NULL);
		}

		m_ParticleContainer.Reset();

		SAFE_DELETE(pParticleTimeLine);
		m_vecParticleLifeTimeLine.erase(m_vecParticleLifeTimeLine.begin() + dwIndex);

		return S_OK;
	}

	return E_FAIL;
}


DWORD KG3DSFX::GetParticleLifeTimeLineNum() const
{
	return (DWORD)m_vecParticleLifeTimeLine.size();
}

HRESULT KG3DSFX::AddParticleLauncher(
	SFX_PARTICLE_TYPE		  eParticleType,
	SFX_LAUNCHER_SHAPE		  eLauncherShape,
	SFX_PARTICLE_UPDATE_MODE  eParticleUpdateMode,
	KG3DSFXParticleLauncher** ppParticleLauncher
)
{
	ASSERT(ppParticleLauncher);
	KG3DSFXParticleLauncher *pParticleLauncher = new KG3DSFXParticleLauncher(
		this, 
		&m_ParticleContainer, 
		eParticleType, 
		eLauncherShape, 
		eParticleUpdateMode
		);
	if(pParticleLauncher == NULL)
		return E_OUTOFMEMORY;

	if (ppParticleLauncher)
		(*ppParticleLauncher) = pParticleLauncher;

	m_vecSFXParticleLauncher.push_back(pParticleLauncher);

	return S_OK;
}


HRESULT KG3DSFX::GetParticleLauncher(
	DWORD dwIndex, KG3DSFXParticleLauncher** ppParticleLauncher
)
{
	ASSERT(ppParticleLauncher);
	if (dwIndex < m_vecSFXParticleLauncher.size())
	{
		(*ppParticleLauncher) = m_vecSFXParticleLauncher[dwIndex];
		return S_OK;
	}

	return E_FAIL;
}


HRESULT KG3DSFX::GetParticleLauncher(
	DWORD dwIndex, IEKG3DSFXParticleLauncher** ppParticleLauncher)
{
	return GetParticleLauncher(dwIndex,(KG3DSFXParticleLauncher**)ppParticleLauncher);
}


HRESULT KG3DSFX::RemoveParticleLauncher(DWORD dwIndex )
{
	if( dwIndex < m_vecSFXParticleLauncher.size() )
	{
        KG3DSFXParticleLauncher* pLauncher =  m_vecSFXParticleLauncher[dwIndex];
        size_t uEffForceNum = pLauncher->m_vecEffForces.size();
        for (size_t i = 0; i < uEffForceNum; i++)
        {
#if defined(_DEBUG) | defined(DEBUG)
			HRESULT hr = 
#endif
				pLauncher->m_vecEffForces[i]->DelLauncherRef(pLauncher);
            ASSERT(SUCCEEDED(hr));
        }


		SAFE_DELETE( m_vecSFXParticleLauncher[dwIndex] );
		m_vecSFXParticleLauncher.erase(m_vecSFXParticleLauncher.begin() + dwIndex);



		return S_OK;
	}

	return E_FAIL;
}


DWORD KG3DSFX::GetParticleLauncherNum() const
{
	return (DWORD)m_vecSFXParticleLauncher.size();
}

BOOL KG3DSFX::IsLauncherBeing(KG3DSFXParticleLauncher* pLauncer)
{
    for (size_t i = 0; i < m_vecSFXParticleLauncher.size(); i++)
    {
        if (pLauncer == m_vecSFXParticleLauncher[i])
            return TRUE;
    }
    return FALSE;
}


HRESULT KG3DSFX::AddForceField(SFX_FORCE_FIELD_TYPE eForceFieldType, IEKG3DSFXParticleForceField** ppForceField)
{
	KG3DSFXParticleForceField *pForceField = new KG3DSFXParticleForceField;
	if (pForceField == NULL)
		return E_OUTOFMEMORY;

    pForceField->SetType(eForceFieldType);
	if (ppForceField)
		(*ppForceField) = pForceField;

	m_vecSFXParticleForceField.push_back(pForceField);

	return S_OK;
}


HRESULT KG3DSFX::GetForceField(DWORD dwIndex, IEKG3DSFXParticleForceField **ppForceField
)
{
	ASSERT(ppForceField);
	if (dwIndex < m_vecSFXParticleForceField.size())
	{
		(*ppForceField) = m_vecSFXParticleForceField[dwIndex];
		return S_OK;
	}
	return E_FAIL;
}


HRESULT KG3DSFX::RemoveForceField(DWORD dwIndex)
{
	if (dwIndex < m_vecSFXParticleForceField.size())
	{
        KG3DSFXParticleForceField* pForce = m_vecSFXParticleForceField[dwIndex];
        size_t uEffLauNum = pForce->m_vecLauncherRef.size();
        for (size_t i = 0; i < uEffLauNum; i++)
            pForce->m_vecLauncherRef[i]->DelEffForce(pForce);

		SAFE_DELETE( m_vecSFXParticleForceField[dwIndex] );
		m_vecSFXParticleForceField.erase(m_vecSFXParticleForceField.begin() + dwIndex);
		return S_OK;
	}
	return E_FAIL;
}


DWORD KG3DSFX::GetForceFieldNum() const
{
	return (DWORD)m_vecSFXParticleForceField.size();
}


HRESULT KG3DSFX::AddTrack(const char cszFileName[], KG3DSFXTrack** ppTrack)
{
	ASSERT(cszFileName);
	ASSERT(cszFileName[0] != '\0');
	ASSERT(ppTrack);

	KG3DSFXTrack *pTrack = new KG3DSFXTrack(this);
	if (pTrack == NULL)
		return E_OUTOFMEMORY;

    //TCHAR szFullPath[MAX_PATH];
    //g_GetFullPath(szFullPath, cszFileName);

	HRESULT hr = pTrack->LoadFromFile(cszFileName);
	if (SUCCEEDED(hr))
	{
		m_vecTrack.push_back(pTrack);
		*ppTrack = pTrack;
	}
    else
    {
        SAFE_DELETE(pTrack);
    }
	return hr;
}


HRESULT KG3DSFX::GetTrack(DWORD dwIndex, KG3DSFXTrack **ppTrack)
{
	ASSERT(ppTrack);
	if (dwIndex < m_vecTrack.size())
	{
		(*ppTrack) = m_vecTrack[dwIndex];
		return S_OK;
	}
	return E_FAIL;
}


HRESULT KG3DSFX::GetTrack(DWORD dwIndex, IEKG3DSFXTrack **ppTrack)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;

	KG3DSFXTrack *pTrack = NULL;

	hrRetCode = GetTrack(dwIndex, &pTrack);
	KGLOG_PROCESS_ERROR_RET_COM_CODE(SUCCEEDED(hrRetCode), hrRetCode);

	*ppTrack = pTrack;

	hrResult = S_OK;
Exit0:
	return hrResult;
}


HRESULT KG3DSFX::RemoveTrack(DWORD dwIndex)
{
	if (dwIndex < m_vecTrack.size())
	{
		SAFE_DELETE(m_vecTrack[dwIndex]);
		m_vecTrack.erase(m_vecTrack.begin() + dwIndex);

		for (int i = 0; i < (int)m_vecSFXParticleLauncher.size(); ++i)
		{
			int nBindTrackIndex = m_vecSFXParticleLauncher[i]->GetBindTrackIndex();
			if (nBindTrackIndex == (int)dwIndex)
				m_vecSFXParticleLauncher[i]->UnBindToTrack();
			else
			{
				if (nBindTrackIndex > (int)dwIndex)
					m_vecSFXParticleLauncher[i]->BindToTrack(nBindTrackIndex - 1);
			}
		}

		for (size_t s = 0; s < m_vecBillboard.size(); s++)
		{
			int nBindTrackIndex = m_vecBillboard[s]->GetBindTrackIndex();
			if (nBindTrackIndex == (int)dwIndex)
				m_vecBillboard[s]->UnBindToTrack();
			else
			{
				if (nBindTrackIndex > (int)dwIndex)
					m_vecBillboard[s]->BindToTrack(nBindTrackIndex - 1);
			}
		}

        for (size_t s = 0; s < m_vecMeteor.size(); s++)
        {
            if (!m_vecMeteor[s])
                continue;

            int nBindTrackIndex = m_vecMeteor[s]->GetBindTrackIdx();
            if (nBindTrackIndex == (int)dwIndex)
                m_vecMeteor[s]->SetBindTrackIdx(-1);
            else
            {
                if (nBindTrackIndex > (int)dwIndex)
                    m_vecMeteor[s]->SetBindTrackIdx(nBindTrackIndex - 1);
            }
        }

        for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
        {
            if (!m_vecModelsInfo[s].pSFXModel)
                continue;

            int nBindTrackIndex = m_vecModelsInfo[s].nTrackIndex;
            if (nBindTrackIndex == (int)dwIndex)
                m_vecModelsInfo[s].nTrackIndex = -1;
            else
            {
                if (nBindTrackIndex > (int)dwIndex)
                    m_vecModelsInfo[s].nTrackIndex = nBindTrackIndex - 1;
            }

        }

		return S_OK;
	}
	return E_FAIL;
}


HRESULT KG3DSFX::ChangeTrack(DWORD dwIndex, const char cszNewFileName[])
{
	ASSERT(cszNewFileName);
	ASSERT(cszNewFileName[0] != '\0');
	ASSERT(dwIndex < m_vecTrack.size());

	KG3DSFXTrack *pTrack = NULL;
	if (dwIndex < m_vecTrack.size())
	{
		pTrack = m_vecTrack[dwIndex];
	}
	else
		return E_FAIL;

	//if (pTrack->GetName() == cszNewFileName)
	//	return S_OK;

	return pTrack->LoadFromFile(cszNewFileName);
}


DWORD KG3DSFX::GetTrackNum() const
{
	return (DWORD)m_vecTrack.size();
}


HRESULT KG3DSFX::AddMeteor(KG3DSFXMeteor **ppMeteor)
{
	ASSERT(ppMeteor);
	KG3DSFXMeteor *pMeteor = new KG3DSFXMeteor(this);
	if (!pMeteor)
		return E_FAIL;

	m_vecMeteor.push_back(pMeteor);

	if (ppMeteor)
		*ppMeteor = pMeteor;

	return S_OK;
}


HRESULT KG3DSFX::GetMeteor(DWORD dwIndex, IEKG3DSFXMeteor **ppMeteor)
{
	if (dwIndex < m_vecMeteor.size())
	{
		*ppMeteor = m_vecMeteor[dwIndex];
		return S_OK;
	}
	return E_FAIL;
}


DWORD KG3DSFX::GetMeteorNum() const
{
	return (DWORD)m_vecMeteor.size();
}


HRESULT KG3DSFX::RemoveMeteor(DWORD dwIndex)
{
	if (dwIndex < m_vecMeteor.size())
	{
		SAFE_DELETE(m_vecMeteor[dwIndex]);
		m_vecMeteor.erase(m_vecMeteor.begin() + dwIndex);
		return S_OK;
	}
	return E_FAIL;
}


HRESULT KG3DSFX::SetMeteorInterpolate(float fInterpolate)
{
	for (DWORD i = 0; i < m_vecMeteor.size(); i++)
		m_vecMeteor[i]->SetMeteorInterpolate(fInterpolate);
	return S_OK;
}


float KG3DSFX::GetMeteorInterpolate()
{
	if (m_vecMeteor.size())
		return m_vecMeteor[0]->GetMeteorInterpolate();
	else
		return -1;
}


DWORD KG3DSFX::GetMeteorBindTrackIndex(DWORD dwIndex)
{
    if (dwIndex < m_vecMeteor.size())
    {
        return m_vecMeteor[dwIndex]->GetBindTrackIdx();
    }
    return static_cast<DWORD>(-1);
}

void KG3DSFX::ClearMeteorListNode()
{
	for (DWORD i = 0; i < m_vecMeteor.size();i++)
	{
		m_vecMeteor[i]->ClearListNode();
	}
}

void KG3DSFX::SetMeteorRunState(BOOL bSampling, KG3DAnimationController *pController)
{
	for (DWORD i = 0; i < m_vecMeteor.size();i++)
	{
		m_vecMeteor[i]->SetSampling(bSampling);
		m_vecMeteor[i]->SetPrecisionAnimationController(pController);
	}
}

void KG3DSFX::SetMeteorSampling(BOOL val)
{
	for (DWORD i = 0; i < m_vecMeteor.size();i++)
	{
		m_vecMeteor[i]->SetSampling(val);
	}
}

BOOL KG3DSFX::ExpEffectValid()
{
    if (!m_pExpEffect)
        return FALSE;

    return m_pExpEffect->IsValid();
}

BOOL KG3DSFX::HaveTimeScalingEffect()
{
    if (!m_pExpEffect)
        return FALSE;

    return m_pExpEffect->HaveTimeScaling();
}

void KG3DSFX::SortBillboard()
{
	KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
	ASSERT(pCamera);
	D3DXVECTOR3 vCameraPos;
	pCamera->GetPosition(&vCameraPos);
	int i, j;
	for (i = 0; i < (int)m_vecBillboard.size(); ++i)
	{
		for (j = 0; j < i; ++j)
		{
			D3DXVECTOR3 vPos1, vPos2;
			m_vecBillboard[i]->m_vPositionLine.GetData(&vPos1, m_fCurrentFrame);
			m_vecBillboard[j]->m_vPositionLine.GetData(&vPos2, m_fCurrentFrame);
			float fLength1 = D3DXVec3LengthSq(&D3DXVECTOR3(vPos1 - vCameraPos));
			float fLength2 = D3DXVec3LengthSq(&D3DXVECTOR3(vPos2 - vCameraPos));
			if (fLength1 < fLength2)
			{
				KG3DSFXBillboard *pTemp = m_vecBillboard[i];
				m_vecBillboard[i] = m_vecBillboard[j];
				m_vecBillboard[j] = pTemp;
			}
		}
	}
}


HRESULT KG3DSFX::AddBillboard(KG3DSFXBillboard **ppBillboard)
{
	ASSERT(ppBillboard);
	KG3DSFXBillboard *pBillboard = new KG3DSFXBillboard(this);
    
    if (pBillboard == NULL)
        return E_OUTOFMEMORY;

    if (FAILED(pBillboard->Init()))
    {
        SAFE_DELETE(pBillboard);
        return E_FAIL;
    }

	if (ppBillboard)
		(*ppBillboard) = pBillboard;

	m_vecBillboard.push_back(pBillboard);

	return S_OK;
}


HRESULT KG3DSFX::GetBillboard(DWORD dwIndex, KG3DSFXBillboard **ppBillboard)
{
	ASSERT(ppBillboard);
	if (dwIndex < m_vecBillboard.size())
	{
		*ppBillboard = m_vecBillboard[dwIndex];
		return S_OK;
	}
	return E_FAIL;
}


HRESULT KG3DSFX::GetBillboard(DWORD dwIndex, IEKG3DSFXBillboard **ppiBillboard)   
{
	HRESULT hResult = E_FAIL;

	ASSERT(ppiBillboard);
	if (dwIndex < m_vecBillboard.size())
	{
		*ppiBillboard = dynamic_cast<IEKG3DSFXBillboard *>(m_vecBillboard[dwIndex]);
		KGLOG_PROCESS_ERROR(*ppiBillboard);
	}
	hResult = S_OK;
Exit0:
	return hResult;
}


DWORD KG3DSFX::GetBillboardNum() const
{
	return (DWORD)m_vecBillboard.size();
}


HRESULT KG3DSFX::RemoveBillboard(DWORD dwIndex)
{
	if (dwIndex <m_vecBillboard.size())
	{
		SAFE_DELETE(m_vecBillboard[dwIndex]);
		m_vecBillboard.erase(m_vecBillboard.begin() + dwIndex);
		return S_OK;
	}
	return E_FAIL;
}


HRESULT KG3DSFX::EnableBillboardSort(BOOL bEnable)
{
	m_bSortBillboard = bEnable;
	return S_OK;
}


HRESULT KG3DSFX::AddMagicParpet(IEKG3DSFXMagicParpet **ppMagicParpet, DWORD dwType)
{
	ASSERT(ppMagicParpet);

    KG3DSFXMagicParpet* pMagicParpet = new KG3DSFXMagicParpet(this, dwType);

	if (!pMagicParpet)
		return E_OUTOFMEMORY;

	*ppMagicParpet = pMagicParpet;
    m_vecMagicParpet.push_back(pMagicParpet);

	return S_OK;
}


HRESULT KG3DSFX::RemoveMagicParpet(int index)
{
    if (index < 0 || index >= (int)m_vecMagicParpet.size())
        return E_FAIL;
    
    vector<KG3DSFXMagicParpet*>::iterator idel = m_vecMagicParpet.begin();
    advance(idel, index);
    KG3DSFXMagicParpet* pAoe = *idel;
    m_vecMagicParpet.erase(idel);
    SAFE_DELETE(pAoe);

    return S_OK;
}

HRESULT KG3DSFX::RemoveMagicParpet(IEKG3DSFXMagicParpet* aoe)
{
    KG3DSFXMagicParpet* pAoe = (KG3DSFXMagicParpet*)aoe;

    if (!pAoe)
        return E_FAIL;

    vector<KG3DSFXMagicParpet*>::iterator idel = find(m_vecMagicParpet.begin(), m_vecMagicParpet.end(), pAoe);

    if (idel == m_vecMagicParpet.end())
        return E_FAIL;

    m_vecMagicParpet.erase(idel);
    SAFE_DELETE(pAoe);

    return S_OK;
}

HRESULT KG3DSFX::RenderFootprint()
{
	if (m_pSelectMark)
	{
		D3DXMATRIX		matIdentity;
		D3DXMATRIX      matSave;
		D3DXMatrixIdentity(&matIdentity);
		g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);
	
		m_pSelectMark->RenderTexture();
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);
	}
	return S_OK;
}
HRESULT KG3DSFX::AddSelectMark(IEKG3DSFXSelectMark** ppSelectMark)
{
	ASSERT(ppSelectMark);
	if (!m_pSelectMark)
		m_pSelectMark = new KG3DSFXSelectMark(this);

	if (!m_pSelectMark)
		return E_OUTOFMEMORY;
	
	*ppSelectMark = m_pSelectMark;
	return S_OK;
}
HRESULT KG3DSFX::RemoveSelectMark()
{
	if(m_pSelectMark)
		SAFE_DELETE(m_pSelectMark);
	return S_OK;
}
IEKG3DSFXMagicParpet* KG3DSFX::GetMagicParpet(int index)
{
    if (index < 0 || index >= (int)m_vecMagicParpet.size())
        return NULL;

	return (IEKG3DSFXMagicParpet*)m_vecMagicParpet[index];
}
IEKG3DSFXSelectMark* KG3DSFX::GetSelectMark()
{
	return (IEKG3DSFXSelectMark*)m_pSelectMark;
}

HRESULT KG3DSFX::UpdateChannelEffectPos(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	if (m_pChannel && m_eSFXAnimationPlay != SFX_AP_PAUSE)
		m_pChannel->UpdateEffectPos(v1, v2);
	return S_OK;
}

HRESULT KG3DSFX::AddBoxProjection(IKG3DSfxBoxProjection** ppBoxProjection)
{
    ASSERT(ppBoxProjection);
    KG3DSfxBoxProjection* bp = new KG3DSfxBoxProjection(this);
    *ppBoxProjection = bp;
    m_vecBoxProj.push_back(bp);

    if (!g_bClient)
    {
        bp->FrameMove();
        bp->Create();
    }

    return S_OK;
}

HRESULT KG3DSFX::RemoveBoxProjection(IKG3DSfxBoxProjection* pBoxProjection)
{
    KG3DSfxBoxProjection* bp = (KG3DSfxBoxProjection*)pBoxProjection;
    vector<KG3DSfxBoxProjection*>::iterator idel = find(m_vecBoxProj.begin(), m_vecBoxProj.end(), bp);

    if (idel == m_vecBoxProj.end())
        return E_FAIL;

    m_vecBoxProj.erase(idel);
    SAFE_DELETE(bp);

    return S_OK;
}

HRESULT KG3DSFX::RemoveBoxProjection(int index)
{
    if (index < 0 || index >= (int)m_vecBoxProj.size())
        return E_FAIL;

    vector<KG3DSfxBoxProjection*>::iterator idel = m_vecBoxProj.begin();
    advance(idel, index);
	KG3DSfxBoxProjection* bp = *idel;
    m_vecBoxProj.erase(idel);
	SAFE_DELETE(bp);

    return S_OK;
}

IKG3DSfxBoxProjection* KG3DSFX::GetBoxProjection(int index)
{
    if (index < 0 || index >= (int)m_vecBoxProj.size())
        return NULL;

    return m_vecBoxProj[index];
}

HRESULT KG3DSFX::AddChannel(IEKG3DSFXChannel** ppChannel)
{
	ASSERT(ppChannel);
	if (!m_pChannel)
		m_pChannel = new KG3DSFXChannel(this);

	if (!m_pChannel)
		return E_OUTOFMEMORY;

	*ppChannel = m_pChannel;
	return S_OK;
}


HRESULT KG3DSFX::RemoveChannel()
{
	SAFE_DELETE(m_pChannel);
	return S_OK;
}


KG3DSFXChannel* KG3DSFX::GetChannel()
{
	return m_pChannel;
}


HRESULT KG3DSFX::GetChannel(IEKG3DSFXChannel ** pChannel)
{
	_ASSERTE(pChannel);
	if (pChannel)
	{
		*pChannel = GetChannel();
		return S_OK;
	}
	return E_FAIL;
}


KG3DModel* KG3DSFX::GetModel(size_t index)
{
	HRESULT hResult  = E_FAIL;

	KG3DModel* pModel = NULL;
	KG_PROCESS_ERROR(index < m_vecModelsInfo.size());

	pModel = m_vecModelsInfo[index].pSFXModel;

	hResult = S_OK;
Exit0:
	return pModel;
}


HRESULT KG3DSFX::SetModel(const char cszFileName[], KG3DModel** ppModel, size_t index)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	KG3DModel* pModel = NULL;

	KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index < m_vecModelsInfo.size());

	KG_PROCESS_ERROR(cszFileName);
	KG_PROCESS_ERROR(ppModel);

	hRetCode = g_cModelTable.LoadResourceFromFile(
		cszFileName, 
		0,
		MLO_MULTI, 
		(IKG3DResourceBase **)&pModel
		);
	KG_COM_PROCESS_ERROR(hRetCode);
	KG_PROCESS_ERROR(pModel);

	*ppModel = pModel;
	SAFE_RELEASE(m_vecModelsInfo[index].pSFXModel);
	m_vecModelsInfo[index].pSFXModel = pModel;
    m_vecModelsInfo[index].pSFXModel->SetParentObjMatrix(&m_matWorld);

	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFX::GetModel(size_t index, IEKG3DModel **ppModel)
{
	HRESULT hResult  = E_FAIL;

	KG_PROCESS_ERROR(index < m_vecModelsInfo.size());

	*ppModel = m_vecModelsInfo[index].pSFXModel;

	hResult = S_OK;
Exit0:
	return hResult;

}


HRESULT KG3DSFX::AddModel(const char cszFileName[], KG3DModel** ppModel)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	_SfxModelInfo modelInfo;
	KG3DModel*    pModel = NULL;

	KG_PROCESS_ERROR(cszFileName);
	KG_PROCESS_ERROR(ppModel);

	hRetCode = g_cModelTable.LoadResourceFromFile(
		cszFileName, 
		0,
		m_dwLoadOption, 
		(IKG3DResourceBase **)&pModel
		);
	KG_COM_PROCESS_ERROR(hRetCode);
	KG_PROCESS_ERROR(pModel);

	modelInfo.pSFXModel = pModel;
	*ppModel = pModel;
	modelInfo.pSFXModel->SetParentObjMatrix(&m_matWorld);
	m_vecModelsInfo.push_back(modelInfo);

	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFX::RemoveModel(size_t index)
{
	HRESULT hResult  = E_FAIL;

	KG_PROCESS_ERROR(index < m_vecModelsInfo.size());
	KG_PROCESS_ERROR(index >= 0);

	SAFE_RELEASE(m_vecModelsInfo[index].pSFXModel);
	m_vecModelsInfo.erase(m_vecModelsInfo.begin() + index);

	for (size_t s = 0; s < m_vecSubsetColor[index]->size(); s++)
		SAFE_DELETE(*(m_vecSubsetColor[index]->begin() + s));
    SAFE_DELETE(m_vecSubsetColor[index]);
	m_vecSubsetColor.erase(m_vecSubsetColor.begin() + index);

	SAFE_DELETE(m_timelineModelAniBound[index]);
	m_timelineModelAniBound.erase(m_timelineModelAniBound.begin() + index);

	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFX::GetModelBindTrackIndex(size_t index, int* pTrackIndex)
{
	HRESULT hResult  = E_FAIL;

	KG_PROCESS_ERROR(index < m_vecModelsInfo.size());

	*pTrackIndex = m_vecModelsInfo[index].nTrackIndex;

	hResult = S_OK;
Exit0:
	return hResult;

}


HRESULT KG3DSFX::GetModelAnimationSpeed(float *pValue, size_t index)
{
	HRESULT hResult  = E_FAIL;

	size_t s = m_vecModelsInfo.size();

	KG_PROCESS_ERROR(pValue);
	//KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index <  s);

	*pValue = m_vecModelsInfo[index].fPlaySpeed;

	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFX::GetModelAnimationStart(float *pValue, size_t index)
{
	HRESULT hResult  = E_FAIL;

	size_t s = m_vecModelsInfo.size();

	KG_PROCESS_ERROR(pValue);
	//KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index <  s);

	*pValue = m_vecModelsInfo[index].fPlayStart;

	hResult = S_OK;
Exit0:
	return hResult;

	return S_OK;
}

HRESULT KG3DSFX::GetModelAnimationMode(DWORD *pValue, size_t index)
{
	HRESULT hResult  = E_FAIL;

	size_t s = m_vecModelsInfo.size();

	KG_PROCESS_ERROR(pValue);
	//KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index <  s);

	*pValue = m_vecModelsInfo[index].dwPlayMode;

	hResult = S_OK;
Exit0:
	return hResult;

}


HRESULT KG3DSFX::GetModelAnimationFrameCount(DWORD* pValue, size_t index)
{
	HRESULT hResult  = E_FAIL;

	size_t s = m_vecModelsInfo.size();

	KG_PROCESS_ERROR(pValue);
	//KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index <  s);

	*pValue = m_vecModelsInfo[index].dwNumFrame;

	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFX::GetModelAnimationFrameTime(float* pValue, size_t index)
{
	HRESULT hResult  = E_FAIL;

	size_t s = m_vecModelsInfo.size();

	KG_PROCESS_ERROR(pValue);
	//KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index <  s);

	*pValue = m_vecModelsInfo[index].fFrameTime;

	hResult = S_OK;
Exit0:
	return hResult;
}


void KG3DSFX::SetModelAnimationFrameTime(float fValue, size_t index)
{
	HRESULT hResult  = E_FAIL;

	size_t s = m_vecModelsInfo.size();

	KG_PROCESS_ERROR(fValue);
	//KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index <  s);

	m_vecModelsInfo[index].fFrameTime = fValue;

	hResult = S_OK;
Exit0:
	return;
}


void KG3DSFX::SetModelAnimationSpeed(float fValue, size_t index)
{
	HRESULT hResult  = E_FAIL;

	size_t s = m_vecModelsInfo.size();

	KG_PROCESS_ERROR(fValue);
	//KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index <  s);

	m_vecModelsInfo[index].fPlaySpeed = fValue;
	m_vecModelsInfo[index].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->SetSpeed(fValue);

	hResult = S_OK;
Exit0:
	return;
}


void KG3DSFX::SetModelAnimationStart(float fValue, size_t index)
{
	HRESULT hResult  = E_FAIL;

	size_t s = m_vecModelsInfo.size();

	KG_PROCESS_ERROR(fValue);
	//KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index <  s);

	m_vecModelsInfo[index].fPlayStart = fValue;

	hResult = S_OK;
Exit0:
	return;
}


void KG3DSFX::SetModelAnimationMode(DWORD dwValue, size_t index)
{
	HRESULT hResult  = E_FAIL;

	size_t s = m_vecModelsInfo.size();

	//KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index <  s);

	m_vecModelsInfo[index].dwPlayMode = dwValue;

	hResult = S_OK;
Exit0:
	return;
}


void KG3DSFX::SetModelAnimationFrameCount(DWORD Value, size_t index)
{
	HRESULT hResult  = E_FAIL;

	size_t s = m_vecModelsInfo.size();

	//KG_PROCESS_ERROR(index >= 0);
	KG_PROCESS_ERROR(index <  s);

	m_vecModelsInfo[index].dwNumFrame = Value;

	hResult = S_OK;
Exit0:
	return;
}


HRESULT KG3DSFX::GetMeshFileName(LPSTR pFileName)
{
	wsprintf(pFileName, "%s", m_scName.c_str());
	return S_OK;
}


HRESULT KG3DSFX::GetMaterialFileName(LPSTR pFileName)
{
	wsprintf(pFileName, "");
	return S_OK;
}


HRESULT KG3DSFX::GetAnimationFileName(LPSTR pFileName)
{
	wsprintf(pFileName,"");
	return S_OK;
}


DWORD KG3DSFX::GetNumModels() const
{
	return (DWORD)m_vecModelsInfo.size();
}


void KG3DSFX::SetModelStartFrame(float fStartFrame)
{
	m_fModelAniStartFrame = fStartFrame;
}


float KG3DSFX::GetModelStartFrame()
{
	return m_fModelAniStartFrame;
}


int KG3DSFX::IsMdlAniConnect(size_t index)
{
	if (index >= 0 && index < m_vecModelsInfo.size())
		return m_vecModelsInfo[index].nCycleFlag;
	else
		return false;
}


DWORD KG3DSFX::GetMdlNumFrame(size_t index)
{
	if (index >= 0 && index < m_vecModelsInfo.size())
		return m_vecModelsInfo[index].dwNumFrame;
	else
		return 0;
}


HRESULT KG3DSFX::EnagleMdlBillBoard(int nEnagle, size_t index)
{
    if (index >= 0 && index < m_vecModelsInfo.size())
    {
        m_vecModelsInfo[index].dwIsBillBoard = nEnagle;

        m_nBillBoardMdlNum = 0;
        for (int i = 0; i < (int)m_vecModelsInfo.size(); i++)
        {
            if (m_vecModelsInfo[i].dwIsBillBoard)
                m_nBillBoardMdlNum++;
        }

        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}


BOOL KG3DSFX::IsMdlBillBoard(size_t index)
{
    if (index >= 0 && index < m_vecModelsInfo.size())
    {
        return (BOOL)m_vecModelsInfo[index].dwIsBillBoard;
    }
    else
    {
        return false;
    }
}


FLOAT KG3DSFX::GetNdlFrameTime(size_t index)
{
	if (index >= 0 && index < m_vecModelsInfo.size())
		return m_vecModelsInfo[index].fFrameTime;
	else
		return -1;
}


HRESULT KG3DSFX::EnagleMdlAniConnect(int nEnagle, size_t index)
{
	if (index >= 0 && index < m_vecModelsInfo.size())
		m_vecModelsInfo[index].nCycleFlag = nEnagle;
	return S_OK;
}


HRESULT KG3DSFX::UpdateModelAniInfo()
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
	{
		hRetCode = UpdateModelAniInfo(s);
		KG_COM_PROCESS_ERROR(hRetCode);
	}

	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFX::UpdateModelAniInfo(size_t index)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	KG3DAnimationController* pMdlAniController = NULL;
	IEKG3DClip*   pIEMdlClip = NULL;
	KG3DClip*     pMdlClip = NULL;
	KG3DModel*			     pModel            = NULL;

	int	    nDateL  = 0;
	int	    nDateR  = 0;

	pModel = m_vecModelsInfo[index].pSFXModel;
	KG_PROCESS_ERROR(pModel);

	pMdlAniController = static_cast<KG3DAnimationController*>(pModel->GetAnimationController(ANICTL_PRIMARY));
	KG_PROCESS_ERROR(pMdlAniController);
	
	hRetCode = pMdlAniController->GetCurAnimation(&pIEMdlClip);
	KG_PROCESS_SUCCESS(FAILED(hRetCode));
	KG_PROCESS_SUCCESS(!pIEMdlClip);

	pMdlClip = reinterpret_cast<KG3DClip*>(pIEMdlClip);

	ASSERT(index < m_timelineModelAniBound.size());
	m_timelineModelAniBound[index]->GetKeyFrameByIndex(&m_vecModelsInfo[index].nLeftFrame, &nDateL, 0);
	m_timelineModelAniBound[index]->GetKeyFrameByIndex(&m_vecModelsInfo[index].nRighFrame, &nDateR, 1);
	
	if (pMdlClip)
	{
		hRetCode = pMdlClip->GetNumFrames(&(m_vecModelsInfo[index].dwNumFrame));
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		hRetCode = pMdlClip->GetFrameLength(&(m_vecModelsInfo[index].fFrameTime));
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		float fPassTime = (float)m_vecModelsInfo[index].nLeftFrame * GetCurrInterval();//m_dwFrameInterval;
		float fAnimTime = 
			(m_vecModelsInfo[index].dwNumFrame - 1) * 
			(m_vecModelsInfo[index].fFrameTime / m_vecModelsInfo[index].fPlaySpeed);

        KG_PROCESS_ERROR(fabs(fAnimTime) > FLT_EPSILON);

		fPassTime = fmod(fPassTime, fAnimTime);
		m_vecModelsInfo[index].fPlayStart = fAnimTime - fPassTime;
		m_vecModelsInfo[index].fPlayStart =
			fmod(m_vecModelsInfo[index].fPlayStart, fAnimTime);
	}
Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFX::SeekModelAniBySFXFrame(float fFrame, float* pfSeek, size_t index)
{
	float	fPassTime = fFrame * GetCurrInterval();//m_dwFrameInterval;	
	float	fMdlSeek  = fPassTime + m_vecModelsInfo[index].fPlayStart;
	
	if (m_vecModelsInfo[index].pSFXModel)
		m_vecModelsInfo[index].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(ANIMATION_SEEK_SET, (int)fMdlSeek);
	if (pfSeek)
		*pfSeek = fMdlSeek;

	return S_OK;
}


HRESULT KG3DSFX::SeekModelAniBySFXFrame(float fFrame, float* pfSeek)
{
	HRESULT hResult  = E_FAIL; 

	for (size_t s = 0; s < m_vecModelsInfo.size(); s++)
		SeekModelAniBySFXFrame(fFrame, pfSeek, s);

	hResult = S_OK;
	return hResult;
}


DWORD  KG3DSFX::GetModelPlayMode()
{
	return m_dwModelPlayMode;
}


HRESULT KG3DSFX::AddSubsetColorDiffuseLine(DWORD dwIndex, D3DCOLORVALUE initColor)
{
	KG3DSFX::COLOR_LINE_T *pDiffuseLine = new KG3DSFX::COLOR_LINE_T;
	pDiffuseLine->InsertKeyFrame(0, initColor);
	m_vecSubsetColor[dwIndex]->push_back(pDiffuseLine);
	return S_OK;
}


HRESULT KG3DSFX::DeleteSubsetColorDiffuseLine(DWORD dwIndex)
{
	for (int i = 0; i < (int)m_vecSubsetColor[dwIndex]->size(); ++i)
	{
		SAFE_DELETE(*(m_vecSubsetColor[dwIndex]->begin() + i));
	}
	m_vecSubsetColor[dwIndex]->clear();
	return S_OK;
}


HRESULT KG3DSFX::AddMutiSubsetColorDiffuseLine(DWORD dwIndex, IEKG3DMaterial *piMat)
{
	HRESULT hResult = E_FAIL;
	KG3DSFX::MDL_COLOR_LINE_T* pMdlColorT = new KG3DSFX::MDL_COLOR_LINE_T;
	DWORD dwNumMaterial = 0;

	KG3DMaterial *pMat = NULL;
	pMat = dynamic_cast<KG3DMaterial *>(piMat);
	KGLOG_PROCESS_ERROR(pMat);

	dwNumMaterial = pMat->m_dwNumMaterials;

	for (UINT i = 0; i < dwNumMaterial; i++)
	{
		KG3DSFX::COLOR_LINE_T* pColorLine = new KG3DSFX::COLOR_LINE_T();
		pMdlColorT->push_back(pColorLine);
	}
	//SAFE_DELETE(m_vecSubsetColor[dwIndex]); //主分支好像没有这个
	m_vecSubsetColor.push_back(pMdlColorT);


	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFX::AddTimeLineModelAniBound()
{
	KG3DSFX::MDL_ANI_BOUND_T*  pMdlAniT   = new KG3DSFX::MDL_ANI_BOUND_T;
	m_timelineModelAniBound.push_back(pMdlAniT);

	return S_OK;
}


TKG3DTimeLinebase* KG3DSFX::GetSubsetColorLine(DWORD dwSubset,DWORD dwIndex)
{
	TKG3DTimeLinebase *pLine = *(m_vecSubsetColor[dwSubset]->begin() + dwIndex);
	return pLine;
}


int KG3DSFX::GetSubsetColorLineSize(DWORD dwSubset)
{
	return (int) m_vecSubsetColor[dwSubset]->size();
}


int KG3DSFX::GetTimeLineModelAniBoundSize()
{
	return (int) m_timelineModelAniBound.size();
}


TKG3DTimeLinebase* KG3DSFX::GetModelAniBoundTimeLine(DWORD dwIndex)
{
	return m_timelineModelAniBound[dwIndex];
}


HRESULT KG3DSFX::InsertSoundKey(LPCTSTR szFileName,     
                                DWORD  dwMode,
                                FLOAT  fVolume,
                                FLOAT  fSFXKey,
                                FLOAT  fMinDis,
                                FLOAT  fMaxDis,
                                BOOL   bLoop,
                                BOOL   bContinue,
                                int    nProbability,
                                int    nSubIndex
                               )
{
    if (fSFXKey < 0.f || fSFXKey >= (FLOAT)m_dwTotalFrameNum)
        return E_FAIL;

    SFX_SOUND_BLOCK SoundKey;
    ZeroMemory(&SoundKey, sizeof(SoundKey));
    
    SoundKey.dwMode  = dwMode;
    SoundKey.dwSound = 0;
    SoundKey.fMaxDis = fMaxDis;
    SoundKey.fMinDis = fMinDis;
    SoundKey.fVolume = fVolume;
    SoundKey.bLoop   = bLoop;
    SoundKey.bContinue = bContinue;
    SoundKey.nProbability = nProbability;

    if (szFileName)
        _tcsncpy(SoundKey.szFileName, szFileName, sizeof(SoundKey.szFileName) / sizeof(SoundKey.szFileName[0]));
    
    _SoundInfo* pInfo = m_tlSoundKey.find((int)fSFXKey);

    if (!pInfo)
    {
        m_tlSoundKey.InsertKeyFrame((int)fSFXKey, _SoundInfo());
        pInfo = m_tlSoundKey.find((int)fSFXKey);
        ASSERT(pInfo);
    }

    pInfo->vSubSound.push_back(SoundKey);

    pInfo->nProbabilitySum = 0;
    for (size_t i = 0; i < pInfo->vSubSound.size(); ++i)
        pInfo->nProbabilitySum += pInfo->vSubSound[i].nProbability;
    ASSERT(pInfo->nProbabilitySum > 0);

   /* if (!szFileName ||)
        return E_INVALIDARG;

    RemoveSoundKey(fSFXKey);

    SFX_SOUND_BLOCK SoundKey;

    ZeroMemory(&SoundKey, sizeof(SoundKey));

    SoundKey.dwMode  = dwMode;
    SoundKey.dwSound = 0xffffffff;
    SoundKey.fMaxDis = fMaxDis;
    SoundKey.fMinDis = fMinDis;
    SoundKey.fVolume = fVolume;
    SoundKey.bLoop   = bLoop;
    SoundKey.bContinue = bContinue;
    _tcsncpy(SoundKey.szFileName, szFileName, sizeof(SoundKey.szFileName) / sizeof(SoundKey.szFileName[0]));

    m_vecSoundKey.InsertKeyFrame((int)fSFXKey, SoundKey);*/

    return S_OK;

}

HRESULT KG3DSFX::RemoveSoundKey(FLOAT fSFXKey, int nSubIndex)
{
    _SoundInfo* pInfo = m_tlSoundKey.find((int)fSFXKey);

    if (!pInfo)
        return E_FAIL;

    if (nSubIndex >= (int)pInfo->vSubSound.size())
        return E_FAIL;

    vector<SFX_SOUND_BLOCK>::iterator idel = pInfo->vSubSound.begin();
    advance(idel, nSubIndex);
    pInfo->vSubSound.erase(idel);

    if (pInfo->vSubSound.size() > 0)
    {
        pInfo->nProbabilitySum = 0;
        for (size_t i = 0; i < pInfo->vSubSound.size(); ++i)
            pInfo->nProbabilitySum += pInfo->vSubSound[i].nProbability;
        ASSERT(pInfo->nProbabilitySum > 0);
    }
    else
    {
        m_tlSoundKey.RemoveKeyFrame((int)fSFXKey);
    }

    return S_OK;
}

HRESULT KG3DSFX::ModifySoundKey(LPCTSTR szFileName,
                               DWORD  dwMode,
                               FLOAT  fVolume,
                               FLOAT  fSFXKey,
                               FLOAT  fMinDis,
                               FLOAT  fMaxDis,
                               BOOL   bLoop,
                               BOOL   bContinue,
                               int    nProbability,
                               int    nSubIndex
                               )
{
    _SoundInfo* pInfo = m_tlSoundKey.find((int)fSFXKey);

    if (!pInfo)
        return E_FAIL;

    if (nSubIndex >= (int)pInfo->vSubSound.size())
        return E_FAIL;

    SFX_SOUND_BLOCK& block = pInfo->vSubSound[nSubIndex];

    block.dwMode  = dwMode;
    block.fMaxDis = fMaxDis;
    block.fMinDis = fMinDis;
    block.fVolume = fVolume;
    block.bLoop   = bLoop;
    block.bContinue = bContinue;
    block.nProbability = nProbability;
	block.dwSound = 0;
    _tcsncpy(block.szFileName, szFileName, sizeof(block.szFileName) / sizeof(block.szFileName[0]));

    pInfo->nProbabilitySum = 0;
    for (size_t i = 0; i < pInfo->vSubSound.size(); ++i)
        pInfo->nProbabilitySum += pInfo->vSubSound[i].nProbability;
    ASSERT(pInfo->nProbabilitySum > 0);

   /* DWORD size = (DWORD)m_vecSoundKey.GetSize();

    for (DWORD i = 0; i < size; ++i)
    {
        int nFrame = 0;
        SFX_SOUND_BLOCK* pSound = m_vecSoundKey.GetDataByIndex(i, nFrame);
        if (pSound && nFrame == (int)fSFXKey)
        {
            pSound->dwMode  = dwMode;
            pSound->fMaxDis = fMaxDis;
            pSound->fMinDis = fMinDis;
            pSound->fVolume = fVolume;
            pSound->bLoop   = bLoop;
            pSound->bContinue = bContinue;
            _tcsncpy(pSound->szFileName, szFileName, sizeof(pSound->szFileName) / sizeof(pSound->szFileName[0]));

            return S_OK;
        }
    }*/

    return S_OK;

}

HRESULT KG3DSFX::QuerySoundKey(LPTSTR  szFileName,
                              DWORD&  dwMode,
                              FLOAT&  fVolume,
                              FLOAT   fSFXKey,
                              FLOAT&  fMinDis,
                              FLOAT&  fMaxDis,
                              BOOL&   bLoop,
                              BOOL&   bContinue,
                              int&    nProbability,
                              int     nSubIndex
                              )
{

    _SoundInfo* pInfo = m_tlSoundKey.find((int)fSFXKey);

    if (!pInfo)
        return E_FAIL;

    if (nSubIndex >= (int)pInfo->vSubSound.size())
        return E_FAIL;

    const SFX_SOUND_BLOCK& block = pInfo->vSubSound[nSubIndex];

    _tcsncpy(szFileName, block.szFileName, MAX_PATH);
    dwMode  = block.dwMode;
    fMaxDis = block.fMaxDis;
    fMinDis = block.fMinDis;
    fVolume = block.fVolume;
    bLoop   = block.bLoop;
    bContinue = block.bContinue;
    nProbability = block.nProbability;

    /*DWORD size = (DWORD)m_vecSoundKey.GetSize();

    for (DWORD i = 0; i < size; ++i)
    {
        int nFrame = 0;
        SFX_SOUND_BLOCK* pSound = m_vecSoundKey.GetDataByIndex(i, nFrame);
        if (pSound && nFrame == (int)fSFXKey)
        {
            _tcsncpy(szFileName, pSound->szFileName, MAX_PATH);
            dwMode  = pSound->dwMode;
            fMaxDis = pSound->fMaxDis;
            fMinDis = pSound->fMinDis;
            fVolume = pSound->fVolume;
            bLoop   = pSound->bLoop;
            bContinue = pSound->bContinue;
            return S_OK;
        }
    }*/

    return S_OK;
}

int KG3DSFX::GetSoundKeyNum(FLOAT fSFXKey)
{
    _SoundInfo* pInfo = m_tlSoundKey.find((int)fSFXKey);

    if (!pInfo)
        return 0;

    return (int)pInfo->vSubSound.size();
}

HRESULT KG3DSFX::CalculateAABB()
{
    size_t i = 0;

    m_staticAABB.Clear();

    for (i = 0; i < m_vecTrack.size(); ++i)
    {
        AABBOX aabbox;

        m_vecTrack[i]->GetAABBox(&aabbox);
        m_staticAABB.AddPosition(aabbox.A);
        m_staticAABB.AddPosition(aabbox.B);
    }

    for (i = 0; i < m_vecMeteor.size(); ++i)
    {
        AABBOX aabbox;

        m_vecMeteor[i]->GetAABBox(&aabbox);
        m_staticAABB.AddPosition(aabbox.A);
        m_staticAABB.AddPosition(aabbox.B);
    }

	for (i = 0; i < m_vecMagicParpet.size(); ++i)
	{
        if (!m_vecMagicParpet[i])
            continue;

		float fRadius  = static_cast<float>(m_vecMagicParpet[i]->GetRadius()) / sinf(D3DX_PI / 4.f);
		D3DXVECTOR3 vA = D3DXVECTOR3(-fRadius, 0.f, -fRadius);
		D3DXVECTOR3 vB = D3DXVECTOR3( fRadius, 0.f,  fRadius);

		m_staticAABB.AddPosition(vA);
		m_staticAABB.AddPosition(vB);
	}
	
    if (m_pSelectMark)
	{
		float fWidth = m_pSelectMark->GetTextureWidth()* 0.5f;
		float fHeight = m_pSelectMark->GetTextureHeight()*0.5f;
		D3DXVECTOR3 vA = D3DXVECTOR3(-fWidth, 0.f, -fHeight);
		D3DXVECTOR3 vB = D3DXVECTOR3( fWidth, 0.f,  fHeight);

		m_staticAABB.AddPosition(vA);
		m_staticAABB.AddPosition(vB);
	}

    if (!m_staticAABB.bInit)
    {
        D3DXVECTOR3 v[] = {
            D3DXVECTOR3(-1.f, -1.f, -1.f),
            D3DXVECTOR3( 1.f,  1.f,  1.f)
        };
        m_staticAABB.AddPosition(v, sizeof(v) / sizeof(D3DXVECTOR3));
    }


    return S_OK;
}


HRESULT KG3DSFX::ReadGeneralInformationBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);

    SFX_GENERAL_INFORMATION_BLOCK *pBlock = (SFX_GENERAL_INFORMATION_BLOCK*)&pBuffer[dwOffset];
    m_dwFrameInterval  = pBlock->dwFrameInterval;
    m_dwTotalFrameNum  = pBlock->dwTotalFrameNum;
    m_nIsSockWave      = (int)pBlock->dwSoveWaveFlag;
	m_nCloseFloorFlag  = (int)pBlock->dwCloseFloorFlag;
    m_dwFixedSize      = pBlock->dwScanlByParentFlag;
    m_dwLockYAlex      = pBlock->dwLockYAlex;
    m_dwShockMode      = pBlock->dwShockMode;
    m_dwMaxParticleNum = pBlock->dwMaxParticle;

    return S_OK;
}


HRESULT KG3DSFX::ReadParticleKindBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);
    DWORD i;
    SFX_PARTICLE_KIND_BLOCK *pBlock = (SFX_PARTICLE_KIND_BLOCK*)&pBuffer[dwOffset];
    DWORD dwPos = dwOffset + sizeof(SFX_PARTICLE_KIND_BLOCK);
    KG3DSFXParticleLifeTimeLine *pKind = NULL;
    HRESULT hr = AddParticleLifeTimeLine((IEKG3DSFXParticleLifeTimeLine**)&pKind);

    if (FAILED(hr))
        return E_FAIL;

    pKind->m_uTextureIndex = pBlock->dwTextureIndex;
    pKind->m_fRotationFactor = *(float*)&pBlock->dwRotationFactor;

    pKind->m_wType = pBlock->wType;
    pKind->m_wFlag = pBlock->wFlag;
    pKind->m_dwSmooth = pBlock->dwSmooth;

    // verson 5
    if (m_nVerson < 5)
    {
        pKind->m_fRotationTimeLineMin.InsertKeyFrame(0, pKind->m_fRotationFactor * m_dwFrameInterval / 19);
        pKind->m_fRotationTimeLineMax.InsertKeyFrame(0, pKind->m_fRotationFactor * m_dwFrameInterval / 19);
    }

    for (i = 0; i < pBlock->dwColorFrameNumL; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        D3DXCOLOR Color(pKeyFrame->dwValue);
        pKind->m_ColorTimeLineL.InsertKeyFrame(nFrame, Color);
        if (m_nVerson <= 4)
            pKind->m_ColorTimeLineR.InsertKeyFrame(nFrame, Color);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    if (m_nVerson > 4)
    {
        for (i = 0; i < pBlock->dwColorFrameNumR; ++i)
        {
            SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            D3DXCOLOR Color(pKeyFrame->dwValue);
            pKind->m_ColorTimeLineR.InsertKeyFrame(nFrame, Color);
            dwPos += sizeof(SFX_KEY_FRAME);
        }
    }

    for (i = 0; i < pBlock->dwWidthFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fWidth = *(float*)&pKeyFrame->dwValue;
        pKind->m_fWidthTimeLine.InsertKeyFrame(nFrame, fWidth);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwHeightFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fHeight = *(float*)&pKeyFrame->dwValue;
        pKind->m_fHeightTimeLine.InsertKeyFrame(nFrame, fHeight);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwTextureFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        int nTextureFrame = (int)pKeyFrame->dwValue;
        pKind->m_nTextureFrameTimeLine.InsertKeyFrame(nFrame, nTextureFrame);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

	// add
    if (pBlock->dwSelfRotationNumMin > 10000000 || pBlock->dwSelfRotationNumMax > 10000000)
        return E_FAIL;

    if (m_nVerson <= 4)
    {
        TKG3DTimeLine<float> tempTimeLine;
        for (i = 0; i < pBlock->dwSelfRotationNumMin; ++i)
        {
            SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            float fRotation = (float)pKeyFrame->dwValue;
            tempTimeLine.InsertKeyFrame(nFrame, fRotation);
            dwPos += sizeof(SFX_KEY_FRAME);
        }

        if (tempTimeLine.GetSize() >= 1)
        {
            for (i = 0; i < tempTimeLine.GetSize() - 1; ++i) 
            {
                float fRotL;
                float fRotR;
                int nKeyL;
                int nKeyR;

                tempTimeLine.GetKeyFrameByIndex(&nKeyL, &fRotL, i);
                tempTimeLine.GetKeyFrameByIndex(&nKeyR, &fRotR, i + 1);

                float fRot = (fRotR - fRotL) / (nKeyR - nKeyL);
                pKind->m_fRotationTimeLineMin.InsertKeyFrame(nKeyL, fRot);
                pKind->m_fRotationTimeLineMin.InsertKeyFrame(nKeyR - 1, fRot);
                pKind->m_fRotationTimeLineMax.InsertKeyFrame(nKeyL, fRot);
                pKind->m_fRotationTimeLineMax.InsertKeyFrame(nKeyR - 1, fRot);

            }
        }

    }
    else
    {
        for (i = 0; i < pBlock->dwSelfRotationNumMin; ++i)
        {
            SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            float fRotation = *(float*)&pKeyFrame->dwValue;
            pKind->m_fRotationTimeLineMin.InsertKeyFrame(nFrame, fRotation);
            dwPos += sizeof(SFX_KEY_FRAME);
        }

        for (i = 0; i < pBlock->dwSelfRotationNumMax; ++i)
        {
            SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            float fRotation = *(float*)&pKeyFrame->dwValue;
            pKind->m_fRotationTimeLineMax.InsertKeyFrame(nFrame, fRotation);
            dwPos += sizeof(SFX_KEY_FRAME);
        }
    }


    return S_OK;
}


HRESULT KG3DSFX::ReadLauncherBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);
    DWORD i;
    SFX_PARTICLE_LAUNCHER_BLOCK *pBlock =
        (SFX_PARTICLE_LAUNCHER_BLOCK*)&pBuffer[dwOffset];
    DWORD dwPos = dwOffset + sizeof(SFX_PARTICLE_LAUNCHER_BLOCK);
    KG3DSFXParticleLauncher *pLauncher = NULL;
    HRESULT hr = AddParticleLauncher(
        (SFX_PARTICLE_TYPE)pBlock->dwParticleType,
        (SFX_LAUNCHER_SHAPE)pBlock->dwLauncherShape,
        (SFX_PARTICLE_UPDATE_MODE)pBlock->dwParticleUpdateMode,
        &pLauncher
    );

    if (FAILED(hr))
        return E_FAIL;

    pLauncher->SetShapeParam1(pBlock->fLauncherShapeParam1);
    pLauncher->SetShapeParam2(pBlock->fLauncherShapeParam2);
    pLauncher->EnableSpeedInherit((BOOL)pBlock->dwIsSpeedInherit);
    //pLauncher->SetScalingCenter(&pBlock->vScalingCenter);
    //pLauncher->SetRotationCenter(&pBlock->vRotationCenter);
    pLauncher->SetRandColDelay(pBlock->dwColorDelay);
    pLauncher->SetBlendMode((SFX_BLEND_MODE)pBlock->dwBlendMode);
    pLauncher->m_dwMotionType = pBlock->dwMotionType;
    pLauncher->m_dwForceOption = pBlock->dwForceOption;
    pLauncher->SetRandRotDelay(pBlock->dwRoatnDelay);
    pLauncher->SetRandAlpDelay(pBlock->dwAlphaDelay);
    pLauncher->SetRandForceRotDelay(pBlock->dwFcRotDelay);


    KG3DSFXParticleLifeTimeLine *pParticleKind = NULL;
    GetParticleLifeTimeLine(pBlock->dwParticleKindIndex, (IEKG3DSFXParticleLifeTimeLine**)&pParticleKind);

    /*
    if (m_nEditor) 
        for (i = 0; i < m_vecSFXParticleLauncher.size() - 1; ++i)
            if (m_vecSFXParticleLauncher[i]->GetParticleLifeTimeDirect() && 
                pParticleKind->equal(m_vecSFXParticleLauncher[i]->GetParticleLifeTimeDirect())
                )
            {
                pParticleKind = m_vecSFXParticleLauncher[i]->GetParticleLifeTimeDirect()->clone();
                m_vecParticleLifeTimeLine.push_back(pParticleKind);
   
                break;
            }
            */
    pLauncher->SetParticleLifeTimeLine(pParticleKind);
    pLauncher->SetName((const char*)pBlock->byName);

    if (pBlock->dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK)
        pLauncher->BindToTrack(pBlock->dwBindTrackIndex);

    pLauncher->SetBindToTrackMode((SFX_LAUNCHER_BIND_TO_TRACK_MODE)pBlock->dwBindTrackMode);
	pLauncher->SetRandTexDelay(pBlock->dwRandTexDelay);

    for (i = 0; i < pBlock->dwParticleNumFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fNum = *(float*)&pKeyFrame->dwValue;
        pLauncher->m_fParticleNumLine.InsertKeyFrame(nFrame, fNum);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwLifeMaxFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fLife = *(float*)&pKeyFrame->dwValue;
        pLauncher->m_fParticleLifeMaxLine.InsertKeyFrame(nFrame, fLife);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwLifeMinFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fLife = *(float*)&pKeyFrame->dwValue;
        pLauncher->m_fParticleLifeMinLine.InsertKeyFrame(nFrame, fLife);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwSpeedMaxFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fSpeed = *(float*)&pKeyFrame->dwValue;
        pLauncher->m_fParticleSpeedMaxLine.InsertKeyFrame(nFrame, fSpeed);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwSpeedMinFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fSpeed = *(float*)&pKeyFrame->dwValue;
        pLauncher->m_fParticleSpeedMinLine.InsertKeyFrame(nFrame, fSpeed);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwScatterMinFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fScatter = *(float*)&pKeyFrame->dwValue;
        pLauncher->m_fParticleScatterMin.InsertKeyFrame(nFrame, fScatter);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwScatterMaxFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fScatter = *(float*)&pKeyFrame->dwValue;
        pLauncher->m_fParticleScatterMax.InsertKeyFrame(nFrame, fScatter);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwTranslationFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3 *pKeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        pLauncher->m_RtsTimeLine.m_TranslationTimeLine.InsertKeyFrame(nFrame, pKeyFrame->Vector3);
        dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
    }

    for (i = 0; i < pBlock->dwRotationFrameNum; ++i)
    {
        if (m_nVerson <= 3)
        {
            SFX_KEY_FRAME_VECTOR3 *pKeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            D3DXQUATERNION q;
            D3DXQuaternionRotationYawPitchRoll(&q, pKeyFrame->Vector3.x, pKeyFrame->Vector3.y, pKeyFrame->Vector3.z);
            pLauncher->m_RtsTimeLine.m_RotationTimeLine.InsertKeyFrame(nFrame, q);
            dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
        }
        else 
        {
            SFX_KEY_FRAME_VECTOR4 *pKeyFrame = (SFX_KEY_FRAME_VECTOR4*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            pLauncher->m_RtsTimeLine.m_RotationTimeLine.InsertKeyFrame(nFrame, pKeyFrame->q);
            dwPos += sizeof(SFX_KEY_FRAME_VECTOR4);
        }

    }

    for (i = 0; i < pBlock->dwScalingFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3 *pKeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        pLauncher->m_RtsTimeLine.m_ScalingTimeLine.InsertKeyFrame(nFrame, pKeyFrame->Vector3);
        dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);

        if (m_nVerson > 3) 
        {
            SFX_KEY_FRAME_VECTOR4 *pKeyFrame = (SFX_KEY_FRAME_VECTOR4*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            pLauncher->m_RtsTimeLine.m_ScalRotationTimLine.InsertKeyFrame(nFrame, pKeyFrame->q);
            dwPos += sizeof(SFX_KEY_FRAME_VECTOR4);
        }
    }

    for (i = 0; i < 32; i++)
    {
        if (HasForceBit(pBlock->dwEffForceBit, i))
            pLauncher->m_vecEffForcesIDs.push_back(i);
    }

    return S_OK;
}


HRESULT KG3DSFX::ReadForceFieldBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);

    DWORD i;
    SFX_FORCE_FIELD_BLOCK *pBlock = (SFX_FORCE_FIELD_BLOCK*)&pBuffer[dwOffset];
    DWORD dwPos = dwOffset + sizeof(SFX_FORCE_FIELD_BLOCK);
    KG3DSFXParticleForceField *pForce = NULL;
    SFX_FORCE_FIELD_TYPE eForceType = (SFX_FORCE_FIELD_TYPE)pBlock->dwForceType;
    HRESULT hr = AddForceField(eForceType, (IEKG3DSFXParticleForceField**)&pForce);
    if (FAILED(hr))
        return E_FAIL;

    //pForce->SetRandomFactor(*(float*)&pBlock->dwRandom);

    if (m_nVerson <= 3)
    {
        for (i = 0; i < pBlock->dwForceFrameNum; ++i)
        {
            SFX_KEY_FRAME_VECTOR3 *pKeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            D3DXVECTOR3 v = pKeyFrame->Vector3;
            FLOAT5 f5 = FLOAT5(v.x, v.y, v.z, *(float*)&pBlock->dwRandom, *(float*)&pBlock->dwRandom);
            pForce->m_ForceTimeLine.InsertKeyFrame(nFrame, f5);
            dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
        }
    }
    else
    {
        if (m_nVerson < 5) 
        {
            for (i = 0; i < pBlock->dwForceFrameNum; ++i)
            {
                SFX_KEY_FRAME_VECTOR4 *pKeyFrame = (SFX_KEY_FRAME_VECTOR4*)&pBuffer[dwPos];
                int nFrame = (int)pKeyFrame->dwFrame;
                D3DXVECTOR4 v = (D3DXVECTOR4)pKeyFrame->q;
                FLOAT5 f5 = FLOAT5(v.x, v.y, v.z, v.w, v.w);
                pForce->m_ForceTimeLine.InsertKeyFrame(nFrame, f5);
                dwPos += sizeof(SFX_KEY_FRAME_VECTOR4);
            }
        }
        else 
        {
            for (i = 0; i < pBlock->dwForceFrameNum; ++i)
            {
                SFX_KEY_FRAME_VECTOR5 *pKeyFrame = (SFX_KEY_FRAME_VECTOR5*)&pBuffer[dwPos];
                int nFrame = (int)pKeyFrame->dwFrame;
                FLOAT5 v = (FLOAT5)pKeyFrame->v;
                pForce->m_ForceTimeLine.InsertKeyFrame(nFrame, v);
                dwPos += sizeof(SFX_KEY_FRAME_VECTOR5);
            }
        }
        //for (i = 0; i < pBlock->dwRoataionFrameNum; ++i)
        //{
        //    SFX_KEY_FRAME_VECTOR3 *pKeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
        //    int nFrame = (int)pKeyFrame->dwFrame;
        //    D3DXVECTOR3 v = pKeyFrame->Vector3;
        //    pForce->m_ForcePosTimeLine.InsertKeyFrame(nFrame, v);
        //    dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
        //}
        //for (i = 0; i < pBlock->dwRoataionFrameNum; ++i)
        //{
        //    SFX_KEY_FRAME_VECTOR3 *pKeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
        //    int nFrame = (int)pKeyFrame->dwFrame;
        //    D3DXVECTOR3 v = pKeyFrame->Vector3;
        //    pForce->m_ForcePosTimeLine.InsertKeyFrame(nFrame, v);
        //    dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
        //}
        for (i = 0; i < pBlock->dwRoataionFrameNum; ++i)
        {
            SFX_KEY_FRAME_VECTOR3 *pKeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            D3DXVECTOR3 v = (D3DXVECTOR3)pKeyFrame->Vector3;
            pForce->m_ForcePosTimeLine.InsertKeyFrame(nFrame, v);
            dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
        }
        for (i = 0; i < pBlock->dwGFrameNum; ++i)
        {
            SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            FLOAT g = *(FLOAT*)&pKeyFrame->dwValue;
            pForce->m_ForceGTimeLine.InsertKeyFrame(nFrame, g);
            dwPos += sizeof(SFX_KEY_FRAME);
        }
    }

    return S_OK;
}

namespace {
    inline bool IsAbsolutePath(char path[]) {
        if (path[0] >= 'A' && 
            path[0] <= 'z' &&
            path[1] == ':' &&
            path[2] == '\\')
            return true;
        return false;
    }
}

HRESULT KG3DSFX::ReadTextureBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);

    HRESULT hRetCode = E_FAIL;

    ++m_nTexLoadIndex;

    SFX_TEXTURE_BLOCK *pBlock = (SFX_TEXTURE_BLOCK*)&pBuffer[dwOffset];

    char* pStr = reinterpret_cast<char*>(pBlock->byTextureName);
    if (IsAbsolutePath(pStr))
    {
        pStr = strstr(pStr, "data\\");

        if (g_AddAlertMessage)
        {
            char strMessage[256];
            sprintf(strMessage,
                "warning : there are absolute file path (index %02d)     %s",
                m_nTexLoadIndex, 
                reinterpret_cast<char*>(pBlock->byTextureName)
                );
            g_AddAlertMessage(strMessage);
            if (pStr)
                g_AddAlertMessage("          editor automatically adjust success :)"); 
            else
                g_AddAlertMessage("          editor automatically adjust fail :("); 
        }
    }

    hRetCode = AddTexture(pStr);

    if (FAILED(hRetCode))
	{
        if (!pStr)
            pStr = (LPTSTR)pBlock->byTextureName;

        if (g_AddAlertMessage)
        {
            char strMessage[128];
            sprintf(strMessage, "error : can not find texture (index %02d)     %s",  m_nTexLoadIndex, pStr);
            g_AddAlertMessage(strMessage);
            return S_OK;
        }
        else 
        {
		    KGLogPrintf(     
			    KGLOG_DEBUG,    
			    TEXT("load texture : %s error!!"), 
			    (LPCSTR)pBlock->byTextureName, 
			    KG_FUNCTION  
		    );                
            return E_FAIL;
        }
	}

    SetTextureCutNum(GetTextureNum() - 1, pBlock->dwCutNum);

    return S_OK;
}


HRESULT KG3DSFX::ReadBillboardBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);

    DWORD i;
    HRESULT hr = E_FAIL;
    SFX_BILLBOARD_BLOCK *pBlock =
        (SFX_BILLBOARD_BLOCK*)&pBuffer[dwOffset];
    DWORD dwPos = dwOffset + sizeof(SFX_BILLBOARD_BLOCK);
    KG3DSFXBillboard *pBillboard = NULL;

    hr = AddBillboard(&pBillboard);
    KG_COM_PROCESS_ERROR(hr);
 
    pBillboard->m_dwRenderTargetTexFlag = pBlock->dwRenterTarget;
    if (FAILED(pBillboard->LoadTexture((const char*)pBlock->byTextureName)))
    {
        if (g_AddAlertMessage)
        {
            char strMessage[128];
            sprintf(strMessage, "error : can not find buillboard texture     %s", (const char*)pBlock->byTextureName);
            g_AddAlertMessage(strMessage);
        }
    }

    hr = pBillboard->SetTextureCutNum(pBlock->dwCutNum);
    KG_COM_PROCESS_ERROR(hr);

    if (FAILED(pBillboard->LoadTexture2((const char*)pBlock->byTexture2Name)))
    {
        if (g_AddAlertMessage)
        {
            char strMessage[128];
            sprintf(strMessage, "error : can not find buillboard texture     %s", (const char*)pBlock->byTexture2Name);
            g_AddAlertMessage(strMessage);
        }
    }

    hr = pBillboard->SetTexture2CutNum(pBlock->dwCutNum2);
    KG_COM_PROCESS_ERROR(hr);

    pBillboard->m_dwBillBoradType = pBlock->dwBillBoardType;
    //pBillboard->EnableRotation(pBlock->dwIsRotation);
    //pBillboard->EnableLockAxis(pBlock->dwIsLockAxis);

    pBillboard->SetBlendMode((SFX_BLEND_MODE)pBlock->dwBlendMode);

    if (pBlock->byBoneName[0])
        pBillboard->BindToBone((const char*)pBlock->byBoneName);

    if (pBlock->dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK)
        pBillboard->BindToTrack(pBlock->dwBindTrackIndex);

    for (i = 0; i < pBlock->dwPositionFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3 *pKeyFrameVec3 = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrameVec3->dwFrame;
        D3DXVECTOR3 vPos = pKeyFrameVec3->Vector3;
        pBillboard->m_vPositionLine.InsertKeyFrame(nFrame, vPos);
        dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
    }

    for (i = 0; i < pBlock->dwWidthFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fWidth = *(float*)&pKeyFrame->dwValue;
        pBillboard->m_fWidthTimeLine.InsertKeyFrame(nFrame, fWidth);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwHeightFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fHeight = *(float*)&pKeyFrame->dwValue;
        pBillboard->m_fHeightTimeLine.InsertKeyFrame(nFrame, fHeight);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwDiffuseFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        D3DXCOLOR color = pKeyFrame->dwValue;
        pBillboard->m_DiffuseTimeLine.InsertKeyFrame(nFrame, color);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwTextureFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        int nIndex = (int)pKeyFrame->dwValue;
        pBillboard->m_nTextureFrameIndex.InsertKeyFrame(nFrame, nIndex);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwTexture2FrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        int nIndex = (int)pKeyFrame->dwValue;
        pBillboard->m_nTexture2FrameIndex.InsertKeyFrame(nFrame, nIndex);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwUVFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR2 *pKeyFrame = (SFX_KEY_FRAME_VECTOR2*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        D3DXVECTOR2 vUV = pKeyFrame->Vector2;
        pBillboard->m_vUVTimeLine.InsertKeyFrame(nFrame, vUV);
        dwPos += sizeof(SFX_KEY_FRAME_VECTOR2);
    }

    for (i = 0; i < pBlock->dwRotationFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        float fRotation = *(float*)&pKeyFrame->dwValue;
        pBillboard->m_fRotationTimeLine.InsertKeyFrame(nFrame, fRotation);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    hr = S_OK;

Exit0:

    return hr;
}

HRESULT KG3DSFX::ReadBoxProjBlock(BYTE* pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);

    HRESULT hr = E_FAIL;
    SFX_BOX_PROJ* pBlock = (SFX_BOX_PROJ*)&pBuffer[dwOffset];
    DWORD dwPos = dwOffset + sizeof(SFX_BOX_PROJ);
    DWORD i = 0;

    KG3DSfxBoxProjection* pBoxProj = NULL;

    hr = AddBoxProjection((IKG3DSfxBoxProjection**)&pBoxProj);
    KG_COM_PROCESS_ERROR(hr);

    pBoxProj->SetTranslation(&pBlock->vTranslation);
    pBoxProj->SetScalingCenter(&pBlock->vScalingCenter);
    pBoxProj->SetScalingRotation(&pBlock->qScalingRotation);
    pBoxProj->SetScaling(&pBlock->vScaling);
    pBoxProj->SetRotationCenter(&pBlock->vRotationCenter);
    pBoxProj->SetRotation(&pBlock->qRotation);
    pBoxProj->SetTexture((char*)pBlock->byTextureFileName);
    pBoxProj->SetTexClip(pBlock->nClipU, pBlock->nClipV);
    pBoxProj->SetUVFrameSpeed(pBlock->fFrameSpeed);
    pBoxProj->SetProjType(pBlock->dwProjMode);
    pBoxProj->SetBlendMode((SFX_BLEND_MODE)pBlock->dwBlend);

    for (i = 0; i < pBlock->dwColorFrameNum; ++i)
    {
        SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        D3DXCOLOR color = pKeyFrame->dwValue;
        pBoxProj->m_tlColor.InsertKeyFrame(nFrame, color);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (i = 0; i < pBlock->dwUVFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR4 *pKeyFrame = (SFX_KEY_FRAME_VECTOR4*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        D3DXVECTOR4 v = pKeyFrame->q;
        pBoxProj->m_tlUV.InsertKeyFrame(nFrame, v);
        dwPos += sizeof(SFX_KEY_FRAME_VECTOR4);
    }


    hr = S_OK;
Exit0 :
    return hr;
}

HRESULT KG3DSFX::ReadTrackBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);

    HRESULT hr = E_FAIL;
    SFX_TRACK_BLOCK *pBlock = (SFX_TRACK_BLOCK*)&pBuffer[dwOffset];
    //DWORD dwPos = dwOffset + sizeof(SFX_TRACK_BLOCK);
    KG3DSFXTrack *pTrack = NULL;

    hr = AddTrack((const char *)pBlock->byTrackFileName, &pTrack);
    KG_COM_PROCESS_ERROR(hr);

    pTrack->SetTranslation(&pBlock->vTranslation);
    pTrack->SetScalingCenter(&pBlock->vScalingCenter);
    pTrack->SetScalingRotation(&pBlock->qScalingRotation);
    pTrack->SetScaling(&pBlock->vScaling);
    pTrack->SetRotationCenter(&pBlock->vRotationCenter);
    pTrack->SetRotation(&pBlock->qRotation);

    pTrack->SetInterval(pBlock->dwInterval);

    hr = S_OK;

Exit0:
    return hr;
}

HRESULT KG3DSFX::ReadSoundBlock(BYTE* pBuffer, DWORD dwOffset)
{
    HRESULT hResult  = E_FAIL;

    ASSERT(pBuffer);

    if (m_nVerson < 6)
    {
        _SoundInfo info;

        SFX_SOUND_BLOCK* pBlock = (SFX_SOUND_BLOCK*)&pBuffer[dwOffset];
        pBlock->dwSound = 0;
        pBlock->nProbability = 1;

        info.vSubSound.push_back(*pBlock);
        info.nProbabilitySum = 1;

        m_tlSoundKey.InsertKeyFrame((int)pBlock->fUnUse, info);

    }
    else
    {
        DWORD dwSubNum = *(DWORD*)&pBuffer[dwOffset];
        dwOffset += sizeof(DWORD);

        if (dwSubNum <= 0)
            return S_OK;

        _SoundInfo Info;
        Info.nProbabilitySum = 0;

        for (DWORD i = 0; i < dwSubNum; ++i)
        {
            SFX_SOUND_BLOCK* pBlock = (SFX_SOUND_BLOCK*)&pBuffer[dwOffset];
            pBlock->dwSound = 0;
            Info.vSubSound.push_back(*pBlock);
            Info.nProbabilitySum += pBlock->nProbability;

            dwOffset += sizeof(SFX_SOUND_BLOCK);
        }

        m_tlSoundKey.InsertKeyFrame((int)(Info.vSubSound.back().fUnUse), Info);

    }


    //if (pBuffer)
    //{
    //    SFX_SOUND_BLOCK* pBlock = (SFX_SOUND_BLOCK*)&pBuffer[dwOffset];
    //    //DWORD dwPos = dwOffset + sizeof(SFX_SOUND_BLOCK);
    //    pBlock->dwSound = 0;
    //    m_vecSoundKey.InsertKeyFrame((int)pBlock->fUnUse, *pBlock);
    //}   
    //else
    //{
    //    KG_PROCESS_ERROR(false);
    //}

    hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT KG3DSFX::ReadChannelBlock(BYTE *pBuffer, DWORD dwOffset)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    ASSERT(pBuffer);
    if (pBuffer)
    {
        SFX_CHANNEL_BLOCK *pBlock = (SFX_CHANNEL_BLOCK*)&pBuffer[dwOffset];
        //DWORD dwPos = dwOffset + sizeof(SFX_CHANNEL_BLOCK);

        IEKG3DSFXChannel* pChannel = NULL;
        hRetCode = AddChannel(&pChannel);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = pChannel->SetPieceCount(pBlock->nPieceCount);
        KG_COM_PROCESS_ERROR(hRetCode);
        hRetCode = pChannel->SetPieceLength(pBlock->fLength);
        KG_COM_PROCESS_ERROR(hRetCode);
        hRetCode = pChannel->SetPieceWidth(pBlock->fWidth);
        KG_COM_PROCESS_ERROR(hRetCode);
        hRetCode = pChannel->SetUSpeed(pBlock->fUSpeed);
        KG_COM_PROCESS_ERROR(hRetCode);
        hRetCode = pChannel->SetVPieceCount(pBlock->nVCount);
        KG_COM_PROCESS_ERROR(hRetCode);
        hRetCode = pChannel->SetVUpdateFcy(pBlock->fVUpdateFcy);
        KG_COM_PROCESS_ERROR(hRetCode);
        hRetCode = pChannel->SetTexture((char*)pBlock->byTextureFileName);
        KG_COM_PROCESS_ERROR(hRetCode);
        hRetCode = pChannel->SetBlendMode(pBlock->nBlendMode);
        KG_COM_PROCESS_ERROR(hRetCode);
    }
	else
	{
		KG_PROCESS_ERROR(FALSE);
	}

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT KG3DSFX::ReadSelectMarkBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);
    HRESULT hr = E_FAIL;
    SFX_SELECTMARK_BLOCK *pBlock = (SFX_SELECTMARK_BLOCK*)&pBuffer[dwOffset];
    KG3DSFXMagicParpet *pParpet = NULL;

    hr = AddMagicParpet((IEKG3DSFXMagicParpet**)&pParpet, AOE_PROJECT);
    KG_COM_PROCESS_ERROR(hr);

    int radius = (int)sqrt((float)pBlock->dwWidth * (float)pBlock->dwWidth + (float)pBlock->dwHeight * (float)pBlock->dwHeight);
    pParpet->SetRadius(radius, true);
    pParpet->SetOffset(0.f);
    pParpet->SetHeightLimit(100);
    pParpet->SetFaceType(AOE_FACE_NONE);
    pParpet->SetLife(0.f);
    pParpet->SetTexture((char*)pBlock->byTextureFileName);

	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DSFX::ReadExpBlock(BYTE* pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);
    ASSERT(m_pExpEffect == NULL);

    SFX_EXP_BLOCK *pBlock = (SFX_EXP_BLOCK*)&pBuffer[dwOffset];
    DWORD dwPos = dwOffset + sizeof(SFX_EXP_BLOCK);

    m_pExpEffect = new KG3DSfxExp();
    KG_PROCESS_ERROR(m_pExpEffect);

    for (DWORD i = 0; i < pBlock->dwTimeScalFrameNum; ++i)
    {
        SFX_KEY_FRAME* KeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
        int nFrame  = (int)KeyFrame->dwFrame;
        float fScal = *(float*)&KeyFrame->dwValue;
        m_pExpEffect->m_tlTimeScal.InsertKeyFrame(nFrame, fScal);
        dwPos += sizeof(SFX_KEY_FRAME);
    }

    for (DWORD i = 0; i < pBlock->dwSwingFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3* KeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
        int nFrame  = (int)KeyFrame->dwFrame;
        D3DXVECTOR3 vSwing = KeyFrame->Vector3;
        m_pExpEffect->m_tlCameraSwing.InsertKeyFrame(nFrame, vSwing);
        dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
    }
    
    for (DWORD i = 0; i < pBlock->dwFrequencyFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3* KeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
        int nFrame  = (int)KeyFrame->dwFrame;
        D3DXVECTOR3 vSwing = KeyFrame->Vector3;
        m_pExpEffect->m_tlCameraFrequency.InsertKeyFrame(nFrame, vSwing);
        dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
    }

    for (DWORD i = 0; i < pBlock->dwModelScanlFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3* KeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
        int nFrame  = (int)KeyFrame->dwFrame;
        D3DXVECTOR3 vScanl = KeyFrame->Vector3;
        m_pExpEffect->m_tlModelScanl.InsertKeyFrame(nFrame, vScanl);
        dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
    }

    return S_OK;
Exit0 :
    return E_FAIL;
}


HRESULT KG3DSFX::ReadMagicParpetBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);
    HRESULT hr = E_FAIL;

    SFX_MAGIC_PARPET_BLOCK *pBlock = (SFX_MAGIC_PARPET_BLOCK*)&pBuffer[dwOffset];
    KG3DSFXMagicParpet *pParpet = NULL;
    DWORD dwPos = dwOffset + sizeof(SFX_MAGIC_PARPET_BLOCK);

    hr = AddMagicParpet((IEKG3DSFXMagicParpet**)&pParpet, pBlock->dwType);
    KG_COM_PROCESS_ERROR(hr);

	pParpet->SetRadius((int)*(float*)&pBlock->dwRadius, true);
    pParpet->SetOffset(*(float*)&pBlock->dwOffset);
    pParpet->SetHeightLimit(*(float*)&pBlock->dwHeightLimit);
	if ((int)pBlock->dwIsFootprint)
        pParpet->SetType(AOE_FOOTPRINT, false);
    pParpet->SetFaceType(pBlock->dwFaceType);
    if ((int)pBlock->dwFaceCameraFlag)
        pParpet->SetFaceType(AOE_FACE_EYES);
	pParpet->SetLife((float)pBlock->dwLife);
    hr = pParpet->SetTexture((char*)pBlock->byTextureFileName);

    if (pBlock->dwExpFlag)
    {
        SFX_MAGIC_PARPET_EXP_BLOCK* pBlockExp = (SFX_MAGIC_PARPET_EXP_BLOCK*)&pBuffer[dwPos];
        dwPos +=  sizeof(SFX_MAGIC_PARPET_EXP_BLOCK);

        pParpet->SetTexCull(pBlockExp->nTexCullX, pBlockExp->nTexCullY);
        pParpet->SetTexSpeed(pBlockExp->fTexSpeed);

        for (int i = 0; i < (int)pBlockExp->dwRadiusNum; ++i)
        {
            SFX_KEY_FRAME* pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            int nValue = (int)pKeyFrame->dwValue;
            pParpet->m_tlRadius.InsertKeyFrame(nFrame, nValue);
            dwPos += sizeof(SFX_KEY_FRAME);
        }

        for (int i = 0; i < (int)pBlockExp->dwColorNum; ++i)
        {
            SFX_KEY_FRAME* pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            D3DXCOLOR color = (D3DXCOLOR)pKeyFrame->dwValue;
            pParpet->m_tlColor.InsertKeyFrame(nFrame, color);
            dwPos += sizeof(SFX_KEY_FRAME);
        }
    }
    else
    {
        pParpet->SetTexCull(1, 1);
        pParpet->SetTexSpeed(0.f);
    }


    hr = S_OK;
Exit0:
    return hr;
}


HRESULT KG3DSFX::ReadMeteorBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);

    HRESULT hr = E_FAIL;
    SFX_METEOR_BLOCK *pBlock = (SFX_METEOR_BLOCK*)&pBuffer[dwOffset];
    KG3DSFXMeteor *pMeteor = NULL;
    DWORD i = 0;
    DWORD dwPos = dwOffset + sizeof(SFX_METEOR_BLOCK);

    hr = AddMeteor(&pMeteor);
    KG_COM_PROCESS_ERROR(hr);

    //pMeteor->SetTranslation(&pBlock->vTranslation);
    //pMeteor->SetScalingCenter(&pBlock->vScalingCenter);
    //pMeteor->SetScalingRotation(&pBlock->qScalingRotation);
    //pMeteor->SetScaling(&pBlock->vScaling);
    //pMeteor->SetRotationCenter(&pBlock->vRotationCenter);
    //pMeteor->SetRotation(&pBlock->qRotation);

    pMeteor->SetBindTrackIdx(pBlock->nTrackIdx);
    pMeteor->SetEmitMinInterval(pBlock->fEmitInterval);
    pMeteor->SetLife(pBlock->fLife);
    pMeteor->SetWidth(pBlock->fWidth);
    pMeteor->SetTextureIdx(pBlock->dwTextureIdx);
    pMeteor->SetDuration(pBlock->fDuration);

    pMeteor->SetOffset(pBlock->vOffset);
    pMeteor->EnableRandomOffset(pBlock->bEnableRandomOffset);
    pMeteor->SetBlendMode((SFX_BLEND_MODE)pBlock->BlendMode);
    pMeteor->SetRallyingScale(pBlock->fRallyingScale);
    pMeteor->SetMeteorInterpolate(pBlock->fInterpolateLength);
    pMeteor->SetSampling((BOOL)pBlock->dwSampling);

    for (i = 0; i < (DWORD)pBlock->fTranslationFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3 *pKeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        pMeteor->m_RtsTimeLine.m_TranslationTimeLine.InsertKeyFrame(nFrame, pKeyFrame->Vector3);
        dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
    }

    for (i = 0; i < (DWORD)pBlock->fRotationFrameNum; ++i)
    {
        if (m_nVerson <= 3)
        {
            SFX_KEY_FRAME_VECTOR3 *pKeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            D3DXQUATERNION q;
            D3DXQuaternionRotationYawPitchRoll(&q, pKeyFrame->Vector3.x, pKeyFrame->Vector3.y, pKeyFrame->Vector3.z);
            pMeteor->m_RtsTimeLine.m_RotationTimeLine.InsertKeyFrame(nFrame, q);
            dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);
        }
        else 
        {
            SFX_KEY_FRAME_VECTOR4 *pKeyFrame = (SFX_KEY_FRAME_VECTOR4*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            pMeteor->m_RtsTimeLine.m_RotationTimeLine.InsertKeyFrame(nFrame, pKeyFrame->q);
            dwPos += sizeof(SFX_KEY_FRAME_VECTOR4);
        }

    }

    for (i = 0; i < (DWORD)pBlock->fScalingFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3 *pKeyFrame = (SFX_KEY_FRAME_VECTOR3*)&pBuffer[dwPos];
        int nFrame = (int)pKeyFrame->dwFrame;
        pMeteor->m_RtsTimeLine.m_ScalingTimeLine.InsertKeyFrame(nFrame, pKeyFrame->Vector3);
        dwPos += sizeof(SFX_KEY_FRAME_VECTOR3);

        if (m_nVerson > 3) 
        {
            SFX_KEY_FRAME_VECTOR4 *pKeyFrame = (SFX_KEY_FRAME_VECTOR4*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            pMeteor->m_RtsTimeLine.m_ScalRotationTimLine.InsertKeyFrame(nFrame, pKeyFrame->q);
            dwPos += sizeof(SFX_KEY_FRAME_VECTOR4);
        }
    }



    hr = S_OK;

Exit0:
    return hr;
}


HRESULT KG3DSFX::ReadModelBlock(BYTE *pBuffer, DWORD dwOffset)
{
    ASSERT(pBuffer);

    DWORD i;
    HRESULT hr = E_FAIL;
    SFX_MODEL_BLOCK *pBlock = (SFX_MODEL_BLOCK*)&pBuffer[dwOffset];
    DWORD dwPos = dwOffset + sizeof(SFX_MODEL_BLOCK);
    size_t index = 0;

    ++m_nMdlLoadIndex;

    KG3DModel *pModel = NULL;
    hr = AddModel((const char*)pBlock->byMeshFileName, &pModel);
    if (FAILED(hr))
    {
        if (g_AddAlertMessage)
        {
            char strMessage[128];
            sprintf(strMessage, "error : can not find model (index %02d)     %s",  m_nMdlLoadIndex, (LPCSTR)pBlock->byMeshFileName);
            g_AddAlertMessage(strMessage);
            return S_OK;
        }
        else
            return hr;
    }

    index = m_vecModelsInfo.size() - 1;
    ASSERT(m_vecModelsInfo[index].pSFXModel == pModel);

    if (pBlock->byMaterialFileName[0])
    {
        //TCHAR szFullPath[MAX_PATH];
        //g_GetFullPath(szFullPath, (LPCSTR)pBlock->byMaterialFileName);
        hr = m_vecModelsInfo[index].pSFXModel->LoadMaterialFromFile((LPCSTR)pBlock->byMaterialFileName, m_uOption);
        KG_COM_PROCESS_ERROR(hr);
    }

    if (pBlock->byAnimationFileName[0])
    {
        //TCHAR szFullPath[MAX_PATH];
        //g_GetFullPath(szFullPath, (LPCSTR)pBlock->byAnimationFileName);
        hr = m_vecModelsInfo[index].pSFXModel->LoadAnimationFromFile((LPCSTR)pBlock->byAnimationFileName, m_uOption);
        KG_COM_PROCESS_ERROR(hr);
    }

    m_vecModelsInfo[index].fPlayStart	 = *(float*)&pBlock->dwPlayStart;
    m_vecModelsInfo[index].fPlaySpeed	 = *(float*)&pBlock->dwPlaySpeed;
    m_vecModelsInfo[index].dwPlayMode	 = pBlock->dwPlayMode;
	m_vecModelsInfo[index].dwIsBillBoard = pBlock->dwIsBillBoard;

    if (pBlock->dwIsBillBoard)
        m_nBillBoardMdlNum++;

    m_vecModelsInfo[index].pSFXModel->SetScalingCenter(&pBlock->vScalingCenter);
    m_vecModelsInfo[index].pSFXModel->SetScalingRotation(&pBlock->qScalingRotation);
    m_vecModelsInfo[index].pSFXModel->SetScaling(&pBlock->vScaling);
    m_vecModelsInfo[index].pSFXModel->SetRotationCenter(&pBlock->vRotationCenter);
    m_vecModelsInfo[index].pSFXModel->SetRotation(&pBlock->qRotation);
    m_vecModelsInfo[index].pSFXModel->SetTranslation(&pBlock->vTranslation);
    m_vecModelsInfo[index].pSFXModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);


    MDL_COLOR_LINE_T* pMdlColorLine = new MDL_COLOR_LINE_T;
    for (i = 0; i < pBlock->dwSubsetNum; ++i)
    {
        COLOR_LINE_T *pColorLine = new COLOR_LINE_T;
        //m_vecSubsetColor.push_back(pColorLine);
        pMdlColorLine->push_back(pColorLine);
        SFX_MODEL_SUBSET *pSubset = (SFX_MODEL_SUBSET*)&pBuffer[dwPos];
        dwPos += sizeof(SFX_MODEL_SUBSET);
        for (DWORD j = 0; j < pSubset->dwDiffuseColorFrame; ++j)
        {
            SFX_KEY_FRAME *pKeyFrame = (SFX_KEY_FRAME*)&pBuffer[dwPos];
            int nFrame = (int)pKeyFrame->dwFrame;
            D3DXCOLOR color = pKeyFrame->dwValue;
            pColorLine->InsertKeyFrame(nFrame, color);
            dwPos += sizeof(SFX_KEY_FRAME);
        }
    }
    m_vecSubsetColor.push_back(pMdlColorLine);

	EnagleMdlAniConnect(static_cast<int>(pBlock->dwPlayConnect), index);
    MDL_ANI_BOUND_T* pAniBoundLine = new MDL_ANI_BOUND_T;
    DWORD dwStartFrame = 0;
    DWORD dwFinisFrame = m_dwTotalFrameNum - 1;
    if (pBlock->dwStartFrame >= 0 && pBlock->dwStartFrame < m_dwTotalFrameNum)
        dwStartFrame = pBlock->dwStartFrame;
    if (pBlock->dwFinisFrame > 0 && pBlock->dwFinisFrame < m_dwTotalFrameNum)
        dwFinisFrame = pBlock->dwFinisFrame;
	pAniBoundLine->InsertKeyFrame(dwStartFrame, dwStartFrame);
	pAniBoundLine->InsertKeyFrame(dwFinisFrame, dwFinisFrame);
    m_timelineModelAniBound.push_back(pAniBoundLine);

    m_vecModelsInfo[index].nTrackIndex = pBlock->dwBindTrackIndex;

    ASSERT(m_timelineModelAniBound.size() == m_vecModelsInfo.size());

    hr = S_OK;

Exit0:
    return hr;
}


DWORD KG3DSFX::WriteGeneralInformationBlock(FILE *pFile)
{
    DWORD dwWriteNum = 0;
    SFX_GENERAL_INFORMATION_BLOCK Block;

    memset(&Block, 0, sizeof(Block));
    Block.fShockWavePower       = m_fShockWavePower;
    Block.dwFrameInterval       = m_dwFrameInterval;
    Block.dwTotalFrameNum       = m_dwTotalFrameNum;
    Block.dwSoveWaveFlag        = (DWORD)m_nIsSockWave;
	Block.dwCloseFloorFlag      = (DWORD)m_nCloseFloorFlag;
    Block.dwScanlByParentFlag   = m_dwFixedSize;
    Block.dwLockYAlex           = m_dwLockYAlex;
    Block.dwShockMode           = m_dwShockMode;
    Block.dwMaxParticle         = m_dwMaxParticleNum;
    dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_GENERAL_INFORMATION_BLOCK), pFile);

    return dwWriteNum;
}


DWORD KG3DSFX::WriteParticleKindBlock(FILE *pFile, DWORD dwIndex)
{
    int           dwWriteNum = 0;
    DWORD         i = 0;
    SFX_KEY_FRAME KeyFrame;
    int           nFrame;
    SFX_PARTICLE_KIND_BLOCK Block;

    memset(&Block, 0, sizeof(Block));
    KG3DSFXParticleLifeTimeLine *pLife = m_vecParticleLifeTimeLine[dwIndex];
    Block.dwColorFrameNumL     = (DWORD)pLife->m_ColorTimeLineL.GetSize();
    Block.dwColorFrameNumR     = (DWORD)pLife->m_ColorTimeLineR.GetSize();
    Block.dwWidthFrameNum      = (DWORD)pLife->m_fWidthTimeLine.GetSize();
    Block.dwHeightFrameNum     = (DWORD)pLife->m_fHeightTimeLine. GetSize();
    Block.dwTextureFrameNum    = (DWORD)pLife->m_nTextureFrameTimeLine.GetSize();
    Block.dwTextureIndex       = (DWORD)pLife->m_uTextureIndex;
    Block.dwRotationFactor     = *(DWORD*)&pLife->m_fRotationFactor;
	Block.dwSelfRotationNumMin = (DWORD)pLife->m_fRotationTimeLineMin.GetSize();
    Block.dwSelfRotationNumMax = (DWORD)pLife->m_fRotationTimeLineMax.GetSize();
    Block.wType                = pLife->m_wType;
    Block.wFlag                = pLife->m_wFlag;
    Block.dwSmooth             = pLife->m_dwSmooth;
    dwWriteNum                += (DWORD)::fwrite(&Block, 1, sizeof(SFX_PARTICLE_KIND_BLOCK), pFile);

    for (i = 0; i < Block.dwColorFrameNumL; ++i)
    {
        D3DXCOLOR Color;
        pLife->m_ColorTimeLineL.GetKeyFrameByIndex(&nFrame, &Color, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = (DWORD)Color;
        dwWriteNum      += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwColorFrameNumR; ++i)
    {
        D3DXCOLOR Color;
        pLife->m_ColorTimeLineR.GetKeyFrameByIndex(&nFrame, &Color, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = (DWORD)Color;
        dwWriteNum      += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwWidthFrameNum; ++i)
    {
        float fWidth;
        pLife->m_fWidthTimeLine.GetKeyFrameByIndex(&nFrame, &fWidth, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fWidth;
        dwWriteNum      += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwHeightFrameNum; ++i)
    {
        float fHeight;
        pLife->m_fHeightTimeLine.GetKeyFrameByIndex(&nFrame, &fHeight, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fHeight;
        dwWriteNum      += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwTextureFrameNum; ++i)
    {
        int nTexture;
        pLife->m_nTextureFrameTimeLine.GetKeyFrameByIndex(&nFrame, &nTexture, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = (DWORD)nTexture;
        dwWriteNum      += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

	for (i = 0; i < Block.dwSelfRotationNumMin; ++i)
	{
		float fRotation = 0;
		pLife->m_fRotationTimeLineMin.GetKeyFrameByIndex(&nFrame, &fRotation, i);
		KeyFrame.dwFrame = (DWORD)nFrame;
		KeyFrame.dwValue = *(DWORD*)&fRotation;
		dwWriteNum      += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
	}

    for (i = 0; i < Block.dwSelfRotationNumMax; ++i)
    {
        float fRotation = 0;
        pLife->m_fRotationTimeLineMax.GetKeyFrameByIndex(&nFrame, &fRotation, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fRotation;
        dwWriteNum      += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }


    return dwWriteNum;
}


DWORD KG3DSFX::WriteLauncherBlock(FILE *pFile, DWORD dwIndex)
{
    DWORD i = 0;
    DWORD dwWriteNum = 0;
    int nFrame;
    SFX_KEY_FRAME KeyFrame;
    SFX_PARTICLE_LAUNCHER_BLOCK Block;

    memset(&Block, 0, sizeof(Block));
    KG3DSFXParticleLauncher *pLauncher = m_vecSFXParticleLauncher[dwIndex];
    Block.dwElementNum = 0;
    Block.dwIsSpeedInherit      = (DWORD)pLauncher->IsSpeedInherit();
    Block.dwLauncherShape       = (DWORD)pLauncher->GetLauncherShape();
    Block.dwLifeMaxFrameNum     = (DWORD)pLauncher->m_fParticleLifeMaxLine.GetSize();
    Block.dwLifeMinFrameNum     = (DWORD)pLauncher->m_fParticleLifeMinLine.GetSize();
    Block.dwParticleNumFrameNum = (DWORD)pLauncher->m_fParticleNumLine.GetSize();
    Block.dwParticleType        = (DWORD)pLauncher->GetParticleType();
    Block.dwParticleUpdateMode  = (DWORD)pLauncher->GetParticleUpdateMode();
    Block.dwScatterMinFrameNum  = (DWORD)pLauncher->m_fParticleScatterMin.GetSize();
    Block.dwScatterMaxFrameNum  = (DWORD)pLauncher->m_fParticleScatterMax.GetSize();
    Block.dwSpeedMaxFrameNum    = (DWORD)pLauncher->m_fParticleSpeedMaxLine.GetSize();
    Block.dwSpeedMinFrameNum    = (DWORD)pLauncher->m_fParticleSpeedMinLine.GetSize();
    Block.dwTranslationFrameNum = (DWORD)pLauncher->m_RtsTimeLine.m_TranslationTimeLine.GetSize();
    Block.dwRotationFrameNum    = (DWORD)pLauncher->m_RtsTimeLine.m_RotationTimeLine.GetSize();
    Block.dwScalingFrameNum     = (DWORD)pLauncher->m_RtsTimeLine.m_ScalingTimeLine.GetSize();
    Block.dwBlendMode           = (DWORD)pLauncher->GetBlendMode();
    Block.dwColorDelay          = pLauncher->GetRandTexDelay();
    //pLauncher->GetScalingCenter(&Block.vScalingCenter);
    //pLauncher->GetRotationCenter(&Block.vRotationCenter);
    Block.dwBindTrackMode       = (DWORD)pLauncher->GetBindToTrackMode();
	Block.dwRandTexDelay        = pLauncher->GetRandTexDelay();
    Block.dwMotionType          = pLauncher->m_dwMotionType;
    Block.dwForceOption         = pLauncher->m_dwForceOption;
    Block.dwColorDelay          = pLauncher->GetRandColDelay();
    Block.dwRoatnDelay          = pLauncher->GetRandRotDelay();
    Block.dwAlphaDelay          = pLauncher->GetRandAlpDelay();
    Block.dwFcRotDelay          = pLauncher->GetRandForceRotDelay();

    size_t uForceNum = m_vecSFXParticleLauncher[dwIndex]->m_vecEffForces.size();
    size_t uForceAllNum = m_vecSFXParticleForceField.size();

    ASSERT(uForceAllNum >= uForceNum);

    for (i = 0; i < uForceNum; i++)
    {
        for (UINT a = 0; a < uForceAllNum; a++)
        {
            if (m_vecSFXParticleForceField[a] == m_vecSFXParticleLauncher[dwIndex]->m_vecEffForces[i])
            {
                SetForceBit(Block.dwEffForceBit, a);
                break;
            }
        }
    }


    for (i = 0; i < m_vecParticleLifeTimeLine.size(); ++i)
        if (pLauncher->GetParticleLifeTimeDirect() && m_vecParticleLifeTimeLine[i]->equal(pLauncher->GetParticleLifeTimeDirect()))
            break;    
    Block.dwParticleKindIndex = i;


    Block.fLauncherShapeParam1 = pLauncher->GetShapeParam1();
    Block.fLauncherShapeParam2 = pLauncher->GetShapeParam2();


    std::string str = pLauncher->GetName() + '\0';
    str._Copy_s((char*)Block.byName, SFX_NAME_LENGTH, SFX_NAME_LENGTH);

    Block.dwBindTrackIndex = pLauncher->GetBindTrackIndex();

    dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_PARTICLE_LAUNCHER_BLOCK), pFile);

    for (i = 0; i < Block.dwParticleNumFrameNum; ++i)
    {
        float fNum;
        pLauncher->m_fParticleNumLine.GetKeyFrameByIndex(&nFrame, &fNum, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fNum;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwLifeMaxFrameNum; ++i)
    {
        float fLife;
        pLauncher->m_fParticleLifeMaxLine.GetKeyFrameByIndex(&nFrame, &fLife, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fLife;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwLifeMinFrameNum; ++i)
    {
        float fLife;
        pLauncher->m_fParticleLifeMinLine.GetKeyFrameByIndex(&nFrame, &fLife, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fLife;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwSpeedMaxFrameNum; ++i)
    {
        float fSpeed;
        pLauncher->m_fParticleSpeedMaxLine.GetKeyFrameByIndex(&nFrame, &fSpeed, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fSpeed;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwSpeedMinFrameNum; ++i)
    {
        float fSpeed;
        pLauncher->m_fParticleSpeedMinLine.GetKeyFrameByIndex(&nFrame, &fSpeed, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fSpeed;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwScatterMinFrameNum; ++i)
    {
        float fScatter;
        pLauncher->m_fParticleScatterMin.GetKeyFrameByIndex(&nFrame, &fScatter, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fScatter;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwScatterMaxFrameNum; ++i)
    {
        float fScatter;
        pLauncher->m_fParticleScatterMax.GetKeyFrameByIndex(&nFrame, &fScatter, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fScatter;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < Block.dwTranslationFrameNum; ++i)
    {
        D3DXVECTOR3 vTrans;
        pLauncher->m_RtsTimeLine.m_TranslationTimeLine.GetKeyFrameByIndex(&nFrame, &vTrans, i);
        SFX_KEY_FRAME_VECTOR3   TransFrame;
        TransFrame.dwFrame = (DWORD)nFrame;
        TransFrame.Vector3 = vTrans;

        dwWriteNum += (DWORD)::fwrite(&TransFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR3), pFile);
    }

    for (i = 0; i < Block.dwRotationFrameNum; ++i)
    {
        D3DXQUATERNION vRot;
        pLauncher->m_RtsTimeLine.m_RotationTimeLine.GetKeyFrameByIndex(&nFrame, &vRot, i);
        SFX_KEY_FRAME_VECTOR4   RotFrame;
        RotFrame.dwFrame = (DWORD)nFrame;
        RotFrame.q = vRot;

        dwWriteNum += (DWORD)::fwrite(&RotFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR4), pFile);
    }

    for (i = 0; i < Block.dwScalingFrameNum; ++i)
    {
        D3DXVECTOR3 vScale;
        pLauncher->m_RtsTimeLine.m_ScalingTimeLine.GetKeyFrameByIndex(&nFrame, &vScale, i);
        SFX_KEY_FRAME_VECTOR3   ScaleFrame;
        ScaleFrame.dwFrame = (DWORD)nFrame;
        ScaleFrame.Vector3 = vScale;

        dwWriteNum += (DWORD)::fwrite(&ScaleFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR3), pFile);

        // verson 4

        D3DXQUATERNION qScanlRotation;
        pLauncher->m_RtsTimeLine.m_ScalRotationTimLine.GetKeyFrameByIndex(&nFrame, &qScanlRotation, i);
        SFX_KEY_FRAME_VECTOR4  ScaleRotationFrame;
        ScaleRotationFrame.dwFrame = (DWORD)nFrame;
        ScaleRotationFrame.q = qScanlRotation;
        dwWriteNum += (DWORD)::fwrite(&ScaleRotationFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR4), pFile);

    }

    return dwWriteNum;
}


DWORD KG3DSFX::WriteForceFieldBlock(FILE *pFile, DWORD dwIndex)
{
    DWORD dwWriteNum = 0;
    SFX_FORCE_FIELD_BLOCK Block;

    memset(&Block, 0, sizeof(Block));
    KG3DSFXParticleForceField *pForce = m_vecSFXParticleForceField[dwIndex];

    Block.dwElementNum = 0;
    Block.dwForceFrameNum = (DWORD)pForce->m_ForceTimeLine.GetSize();
    Block.dwRoataionFrameNum = (DWORD)pForce->m_ForcePosTimeLine.GetSize();
    Block.dwGFrameNum = (DWORD)pForce->m_ForceGTimeLine.GetSize();
    Block.dwForceType = (DWORD)pForce->GetType();
    /*float fRandom = pForce->GetRandomFactor();
    Block.dwRandom = *(DWORD*)&fRandom;*/
    dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_FORCE_FIELD_BLOCK), pFile);
    
    int i = 0;
    for (i = 0; i < (int)Block.dwForceFrameNum; ++i)
    {
        // old versor
        /*
        D3DXVECTOR3 vec;
        int nFrame;
        pForce->m_ForceTimeLine.GetKeyFrameByIndex(&nFrame, &vec, i);
        SFX_KEY_FRAME_VECTOR3 KeyFrame;
        KeyFrame.dwFrame = nFrame;
        KeyFrame.Vector3 = vec;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR3), pFile);
        */
        FLOAT5 vec;
        int nFrame;
        pForce->m_ForceTimeLine.GetKeyFrameByIndex(&nFrame, &vec, i);
        SFX_KEY_FRAME_VECTOR5 KeyFrame;
        KeyFrame.dwFrame = nFrame;
        KeyFrame.v = (FLOAT5)vec;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR5), pFile);
    }

    for (i = 0; i < (int)Block.dwRoataionFrameNum; ++i)
    {
        D3DXVECTOR3 vec;
        int nFrame;
        pForce->m_ForcePosTimeLine.GetKeyFrameByIndex(&nFrame, &vec, i);
        SFX_KEY_FRAME_VECTOR3 KeyFrame;
        KeyFrame.dwFrame = nFrame;
        KeyFrame.Vector3 = vec;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR3), pFile);
    }

    for (i = 0; i < (int)Block.dwGFrameNum; ++i)
    {
        FLOAT g;
        int nFrame;
        pForce->m_ForceGTimeLine.GetKeyFrameByIndex(&nFrame, &g, i);
        SFX_KEY_FRAME KeyFrame;
        KeyFrame.dwFrame = nFrame;
        KeyFrame.dwValue = *(DWORD*)&g;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    return dwWriteNum;
}


DWORD KG3DSFX::WriteTextureBlock(FILE *pFile, DWORD dwIndex)
{
    KG3DTexture *pTexture = m_vecTexture[dwIndex];
    DWORD dwWriteNum = 0;
    SFX_TEXTURE_BLOCK Block;

    memset(&Block, 0, sizeof(Block));

    if (pTexture)
        strcpy_s((char*)Block.byTextureName, SFX_TEXTURE_FILE_NAME_LENGTH, SafeGetRetPath(pTexture->m_scName.c_str()));
    else
        Block.byTextureName[0] = '\0';

    Block.dwCutNum = m_vecCuttingNum[dwIndex];
    Block.dwElementNum = 0;

    dwWriteNum += (int)::fwrite(&Block, 1, sizeof(SFX_TEXTURE_BLOCK), pFile);

    return dwWriteNum;
}


DWORD KG3DSFX::WriteBillboardBlock(FILE *pFile, DWORD dwIndex)
{ 
    SFX_KEY_FRAME         KeyFrame;
    SFX_KEY_FRAME_VECTOR3 KeyFrameVec3;
    SFX_KEY_FRAME_VECTOR2 KeyFrameVec2;
    SFX_BILLBOARD_BLOCK   Block;
    
    DWORD   i;
    DWORD   dwWriteNum = 0;
    int     nFrame;

    memset(&Block, 0, sizeof(Block));

    KG3DSFXBillboard *pBillboard = m_vecBillboard[dwIndex];
    Block.dwElementNum = 0;

    Block.dwPositionFrameNum    = (DWORD)pBillboard->m_vPositionLine.GetSize();
    Block.dwWidthFrameNum       = (DWORD)pBillboard->m_fWidthTimeLine.GetSize();
    Block.dwHeightFrameNum      = (DWORD)pBillboard->m_fHeightTimeLine.GetSize();
    Block.dwDiffuseFrameNum     = (DWORD)pBillboard->m_DiffuseTimeLine.GetSize();
    Block.dwTextureFrameNum     = (DWORD)pBillboard->m_nTextureFrameIndex.GetSize();
    Block.dwTexture2FrameNum    = (DWORD)pBillboard->m_nTexture2FrameIndex.GetSize();
    Block.dwUVFrameNum          = (DWORD)pBillboard->m_vUVTimeLine.GetSize();
    Block.dwRotationFrameNum    = (DWORD)pBillboard->m_fRotationTimeLine.GetSize();

    if (pBillboard->GetTexture() == NULL)
        Block.byTextureName[0] = '\0';
    else
        strncpy((char*)Block.byTextureName,
            SafeGetRetPath(((KG3DTexture*)pBillboard->GetTexture())->m_scName.c_str()),
            min(SFX_TEXTURE_FILE_NAME_LENGTH, MAX_PATH)
            );

    if (pBillboard->GetTexture2() == NULL)
        Block.byTexture2Name[0] = '\0';
    else
        strncpy((char*)Block.byTexture2Name,
            SafeGetRetPath(((KG3DTexture*)pBillboard->GetTexture2())->m_scName.c_str()),
            min(SFX_TEXTURE_FILE_NAME_LENGTH, MAX_PATH)
        );

    Block.dwCutNum  = (DWORD)pBillboard->GetTextureCutNum();
    Block.dwCutNum2 = (DWORD)pBillboard->GetTexture2CutNum();

    Block.dwIsRotation      = 0;
    Block.dwBillBoardType   = pBillboard->m_dwBillBoradType;

    Block.dwBlendMode       = pBillboard->GetBlendMode();
    Block.dwRenterTarget    = pBillboard->m_dwRenderTargetTexFlag;

    if (pBillboard->IsBindToBone())
        pBillboard->GetBindName()._Copy_s((char*)Block.byBoneName, SFX_NAME_LENGTH, SFX_NAME_LENGTH);
    else
        Block.byBoneName[0] = 0;

    Block.dwBindTrackIndex = pBillboard->GetBindTrackIndex();

    dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_BILLBOARD_BLOCK), pFile);

    for (i = 0; i < (int)Block.dwPositionFrameNum; ++i)
    {
        D3DXVECTOR3 vPos;
        pBillboard->m_vPositionLine.GetKeyFrameByIndex(&nFrame, &vPos, i);
        KeyFrameVec3.dwFrame = (DWORD)nFrame;
        KeyFrameVec3.Vector3 = vPos;
        dwWriteNum += (DWORD)::fwrite(&KeyFrameVec3, 1, sizeof(SFX_KEY_FRAME_VECTOR3), pFile);
    }

    for (i = 0; i < (int)Block.dwWidthFrameNum; ++i)
    {
        float fWidth;
        pBillboard->m_fWidthTimeLine.GetKeyFrameByIndex(&nFrame, &fWidth, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fWidth;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < (int)Block.dwHeightFrameNum; ++i)
    {
        float fHeight;
        pBillboard->m_fHeightTimeLine.GetKeyFrameByIndex(&nFrame, &fHeight, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fHeight;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < (int)Block.dwDiffuseFrameNum; ++i)
    {
        D3DXCOLOR Color;
        pBillboard->m_DiffuseTimeLine.GetKeyFrameByIndex(&nFrame, &Color, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = (DWORD)Color;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < (int)Block.dwTextureFrameNum; ++i)
    {
        int nTexture;
        pBillboard->m_nTextureFrameIndex.GetKeyFrameByIndex(&nFrame, &nTexture, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = (DWORD)nTexture;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < (int)Block.dwTexture2FrameNum; ++i)
    {
        int nTexture2;
        pBillboard->m_nTexture2FrameIndex.GetKeyFrameByIndex(&nFrame, &nTexture2, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = (DWORD)nTexture2;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < (int)Block.dwUVFrameNum; ++i)
    {
        D3DXVECTOR2 vUV;
        pBillboard->m_vUVTimeLine.GetKeyFrameByIndex(&nFrame, &vUV, i);
        KeyFrameVec2.dwFrame = (DWORD)nFrame;
        KeyFrameVec2.Vector2 = vUV;
        dwWriteNum += (DWORD)::fwrite(&KeyFrameVec2, 1, sizeof(SFX_KEY_FRAME_VECTOR2), pFile);
    }

    for (i = 0; i < (int)Block.dwRotationFrameNum; ++i)
    {
        float fRotation;
        pBillboard->m_fRotationTimeLine.GetKeyFrameByIndex(&nFrame, &fRotation, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fRotation;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    return dwWriteNum;
}

DWORD KG3DSFX::WriteBoxProjBlock(FILE* pFile, DWORD dwIndex)
{
    DWORD dwWriteNum = 0;
    SFX_BOX_PROJ Block;
    memset(&Block, 0, sizeof(Block));

    KG3DSfxBoxProjection* pBoxProj = m_vecBoxProj[dwIndex];
    ASSERT(pBoxProj);

    char filePath[MAX_PATH];
    int i = 0;
    int nFrame = 0;

    pBoxProj->GetScalingCenter(&Block.vScalingCenter);
    pBoxProj->GetScalingRotation(&Block.qScalingRotation);
    pBoxProj->GetScaling(&Block.vScaling);
    pBoxProj->GetRotationCenter(&Block.vRotationCenter);
    pBoxProj->GetRotation(&Block.qRotation);
    pBoxProj->GetTranslation(&Block.vTranslation);
    pBoxProj->GetTexture(filePath);
    strcpy_s((char*)Block.byTextureFileName, SFX_TEXTURE_FILE_NAME_LENGTH, SafeGetRetPath(filePath));
    pBoxProj->GetTexClip(Block.nClipU, Block.nClipV);
    Block.fFrameSpeed = pBoxProj->GetUVFrameSpeed();
    Block.dwProjMode = pBoxProj->GetProjType();
    Block.dwBlend = pBoxProj->GetBlendMode();
    Block.dwColorFrameNum = (DWORD)pBoxProj->m_tlColor.GetSize();
    Block.dwUVFrameNum = (DWORD)pBoxProj->m_tlUV.GetSize();
    
    dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_BOX_PROJ), pFile);

    for (i = 0; i < (int)Block.dwColorFrameNum; ++i)
    {
        SFX_KEY_FRAME KeyFrame;
        D3DXCOLOR Color;
        pBoxProj->m_tlColor.GetKeyFrameByIndex(&nFrame, &Color, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = (DWORD)Color;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < (int)Block.dwUVFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR4 KeyFrame;
        D3DXVECTOR4 v;
        pBoxProj->m_tlUV.GetKeyFrameByIndex(&nFrame, &v, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.q = (D3DXQUATERNION)v;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR4), pFile);
    }

    return dwWriteNum;
}

DWORD KG3DSFX::WriteTrackBlock(FILE *pFile, DWORD dwIndex)
{
    DWORD dwWriteNum = 0;
    SFX_TRACK_BLOCK Block;
    memset(&Block, 0, sizeof(Block));

    IEKG3DSFXTrack *pTrack = m_vecTrack[dwIndex];
    Block.dwElementNum = 0;
    pTrack->GetScalingCenter(&Block.vScalingCenter);
    pTrack->GetScalingRotation(&Block.qScalingRotation);
    pTrack->GetScaling(&Block.vScaling);
    pTrack->GetRotationCenter(&Block.vRotationCenter);
    pTrack->GetRotation(&Block.qRotation);
    pTrack->GetTranslation(&Block.vTranslation);
    
    strcpy_s((char*)Block.byTrackFileName, SFX_TEXTURE_FILE_NAME_LENGTH, SafeGetRetPath(pTrack->GetName()));


    Block.dwInterval = pTrack->GetInterval();
    dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_TRACK_BLOCK), pFile);

    return dwWriteNum;
}


DWORD KG3DSFX::WriteMagicParpetBlock(FILE *pFile, DWORD dwIndex)
{
    SFX_MAGIC_PARPET_BLOCK Block;
    DWORD dwWriteNum = 0;
    KG3DSFXMagicParpet* pMagicParpet = m_vecMagicParpet[dwIndex];

    memset(&Block, 0, sizeof(Block));

    float fVal = (float)pMagicParpet->GetRadius();

    Block.dwRadius          = *(DWORD*)&fVal;
    fVal = pMagicParpet->GetHeightLimit();
    Block.dwHeightLimit     = *(DWORD*)&fVal;
    fVal = pMagicParpet->GetOffset();
    Block.dwOffset          = *(DWORD*)&fVal;
	Block.dwLife            = (DWORD)pMagicParpet->GetLife();
    Block.dwType            = pMagicParpet->GetType();
    Block.dwFaceType        = pMagicParpet->GetFaceType();
    Block.dwExpFlag         = TRUE;

    KG3DTexture *pTexture = NULL;

    if (pMagicParpet->GetTexture() == NULL)
        Block.byTextureFileName[0] = '\0';
    else
    {
        pTexture = dynamic_cast<KG3DTexture*>(pMagicParpet->GetTexture());
        if (pTexture)
            strncpy(
                (char*)Block.byTextureFileName, 
                SafeGetRetPath(pTexture->m_scName.c_str()), 
                min(SFX_TEXTURE_FILE_NAME_LENGTH, MAX_PATH)
                );
        else
            Block.byTextureFileName[0] = '\0';
    }

    dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_MAGIC_PARPET_BLOCK), pFile);


    // exp

    SFX_KEY_FRAME KeyFrame;
    int nFrame;
    int i;

    SFX_MAGIC_PARPET_EXP_BLOCK BlockExp;
    memset(&BlockExp, 0, sizeof(BlockExp));

    BlockExp.dwRadiusNum = (DWORD)pMagicParpet->m_tlRadius.size();
    BlockExp.dwColorNum = (DWORD)pMagicParpet->m_tlColor.size();
    pMagicParpet->GetTexCull(&BlockExp.nTexCullX, &BlockExp.nTexCullY);
    BlockExp.fTexSpeed = pMagicParpet->GetTexSpeed();

    dwWriteNum += (DWORD)::fwrite(&BlockExp, 1, sizeof(SFX_MAGIC_PARPET_EXP_BLOCK), pFile);

    for (i = 0; i < (int)BlockExp.dwRadiusNum; ++i)
    {
        int radius;
        pMagicParpet->m_tlRadius.GetKeyFrameByIndex(&nFrame, &radius, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = (DWORD)radius;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (i = 0; i < (int)BlockExp.dwColorNum; ++i)
    {
        D3DXCOLOR color;
        pMagicParpet->m_tlColor.GetKeyFrameByIndex(&nFrame, &color, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = (DWORD)color;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }


    return dwWriteNum;
}


DWORD KG3DSFX::WriteExpBlock(FILE* pFile)
{
    SFX_EXP_BLOCK Block;
    DWORD dwWriteNum = 0;
    memset(&Block, 0, sizeof(Block));

    Block.dwTimeScalFrameNum   = (DWORD)m_pExpEffect->m_tlTimeScal.size();
    Block.dwSwingFrameNum      = (DWORD)m_pExpEffect->m_tlCameraSwing.size();
    Block.dwFrequencyFrameNum  = (DWORD)m_pExpEffect->m_tlCameraFrequency.size();
    Block.dwModelScanlFrameNum = (DWORD)m_pExpEffect->m_tlModelScanl.size();

    dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_EXP_BLOCK), pFile);

    for (int i = 0; i < (int)Block.dwTimeScalFrameNum; ++i)
    {
        SFX_KEY_FRAME KeyFrame;
        int   nFrame;
        float fScal = 0;

        m_pExpEffect->m_tlTimeScal.GetKeyFrameByIndex(&nFrame, &fScal, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.dwValue = *(DWORD*)&fScal;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
    }

    for (int i = 0; i < (int)Block.dwSwingFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3 KeyFrame;
        int   nFrame;
        D3DXVECTOR3 vSwing;

        m_pExpEffect->m_tlCameraSwing.GetKeyFrameByIndex(&nFrame, &vSwing, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.Vector3 = vSwing;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR3), pFile);
    }

    for (int i = 0; i < (int)Block.dwFrequencyFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3 KeyFrame;
        int   nFrame;
        D3DXVECTOR3 vFrequ;

        m_pExpEffect->m_tlCameraFrequency.GetKeyFrameByIndex(&nFrame, &vFrequ, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.Vector3 = vFrequ;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR3), pFile);
    }

    for (int i = 0; i < (int)Block.dwModelScanlFrameNum; ++i)
    {
        SFX_KEY_FRAME_VECTOR3 KeyFrame;
        int   nFrame;
        D3DXVECTOR3 vScanl;

        m_pExpEffect->m_tlModelScanl.GetKeyFrameByIndex(&nFrame, &vScanl, i);
        KeyFrame.dwFrame = (DWORD)nFrame;
        KeyFrame.Vector3 = vScanl;
        dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR3), pFile);
    }


    return dwWriteNum;
}

DWORD KG3DSFX::WriteSelectMarkBlock(FILE *pFile)
{
	DWORD dwWriteNum = 0;
	SFX_SELECTMARK_BLOCK Block;

	memset(&Block, 0, sizeof(Block));
	Block.dwWidth  = (DWORD)m_pSelectMark->GetTextureWidth();
	Block.dwHeight = (DWORD)m_pSelectMark->GetTextureHeight();
	/*Block.dwFoot   = (DWORD)m_pSelectMark->GetenFoot();
	std::string scFootpringtTpye = m_pSelectMark->GetFootprintType();
	strncpy((char*)Block.byFootprintType,scFootpringtTpye.c_str(),min(SFX_TEXTURE_FILE_NAME_LENGTH, MAX_PATH));*/

	KG3DTexture *pTexture = NULL;
	TCHAR szRelName[MAX_PATH];
		pTexture = dynamic_cast<KG3DTexture*>(m_pSelectMark->GetTexture());
	if (pTexture)
	{
		g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, pTexture->m_scName.c_str(), szRelName);
			strncpy(
				(char*)Block.byTextureFileName, 
				szRelName, 
				min(SFX_TEXTURE_FILE_NAME_LENGTH, MAX_PATH)
				);
		KGLogPrintf(KGLOG_ERR,"SFXFootprint Write Texture: %s", (char*)Block.byTextureFileName);
	}
	else
	{
		Block.byTextureFileName[0] = '\0';
	}

	dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_SELECTMARK_BLOCK), pFile);

	return dwWriteNum;
}

DWORD KG3DSFX::WriteChannelBlock(FILE *pFile)
{
    DWORD dwWriteNum = 0;
    SFX_CHANNEL_BLOCK Block;
    memset(&Block, 0, sizeof(Block));

    Block.fLength     = m_pChannel->GetPieceLength();
    Block.fUSpeed     = m_pChannel->GetUSpeed();
    Block.fVUpdateFcy = m_pChannel->GetVUpdateFcy();
    Block.fWidth      = m_pChannel->GetPieceWidth();
    Block.nPieceCount = m_pChannel->GetPieceCount();
    Block.nVCount     = m_pChannel->GetVPieceCount();
    Block.nBlendMode  = m_pChannel->GetBlendMode();

    if (m_pChannel->GetTexture() == NULL)
        Block.byTextureFileName[0] = '\0';
    else
    {
        strncpy(
            (char*)Block.byTextureFileName, 
            SafeGetRetPath( m_pChannel->GetTexture()->m_scName.c_str()), 
            min(SFX_TEXTURE_FILE_NAME_LENGTH, MAX_PATH)
        );
    }

    dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_CHANNEL_BLOCK), pFile);

    return dwWriteNum;
}


DWORD KG3DSFX::WriteSoundBlock(FILE* pFile, DWORD dwIndex)
{
    DWORD dwWriteNum = 0;
    
    int nFrame = 0;
    _SoundInfo* pInfo = m_tlSoundKey.GetDataByIndex(dwIndex, nFrame);

    if (!pInfo)
        return 0;

    vector<SFX_SOUND_BLOCK>& vBlock = pInfo->vSubSound;
    DWORD dwSubNum = (DWORD)vBlock.size();

    dwWriteNum += (DWORD)::fwrite(&dwSubNum, 1, sizeof(DWORD), pFile);

    for (DWORD i = 0; i < dwSubNum; ++i)
    {
        SFX_SOUND_BLOCK& block = vBlock[i];
        block.fUnUse = (float)nFrame;
        dwWriteNum += (DWORD)::fwrite(&block, 1, sizeof(SFX_SOUND_BLOCK), pFile);
    }

    

   /* DWORD dwWriteNum = 0;
    int nFrame = 0;
    SFX_SOUND_BLOCK* pSound = m_vecSoundKey.GetDataByIndex(dwIndex, nFrame);

    if (!pSound)
        return 0;

    pSound->fUnUse = (float)nFrame;

    dwWriteNum += (DWORD)::fwrite(pSound, 1, sizeof(SFX_SOUND_BLOCK), pFile);*/

    return dwWriteNum;
}


DWORD KG3DSFX::WriteMeteorBlock(FILE *pFile, DWORD dwIndex)
{
    SFX_METEOR_BLOCK Block;
    DWORD dwWriteNum = 0;
    KG3DSFXMeteor* pMeteor = m_vecMeteor[dwIndex];
    DWORD i = 0;
    int nFrame = 0;

    memset(&Block, 0, sizeof(Block));

    ASSERT(pMeteor);

    Block.dwTextureIdx  = pMeteor->GetTextureIdx();
    Block.fEmitInterval = pMeteor->GetEmitMinInterval();
    Block.fLife         = pMeteor->GetLife();
    Block.fWidth        = pMeteor->GetWidth();
    Block.nTrackIdx     = pMeteor->GetBindTrackIdx();

    Block.fTranslationFrameNum = (float)pMeteor->m_RtsTimeLine.m_TranslationTimeLine.GetSize();
    Block.fRotationFrameNum    = (float)pMeteor->m_RtsTimeLine.m_RotationTimeLine.GetSize();
    Block.fScalingFrameNum     = (float)pMeteor->m_RtsTimeLine.m_ScalingTimeLine.GetSize();

//    pMeteor->GetScalingCenter(&Block.vScalingCenter);
    pMeteor->GetScalingRotation(&Block.qScalingRotation);
    pMeteor->GetScaling(&Block.vScaling);
    pMeteor->GetRotationCenter(&Block.vRotationCenter);
    pMeteor->GetRotation(&Block.qRotation);
    pMeteor->GetTranslation(&Block.vTranslation);

    Block.bEnableRandomOffset = pMeteor->IsEnableRandomOffset();
    Block.vOffset             = pMeteor->GetOffset();
    Block.BlendMode           = (DWORD)pMeteor->GetBlendMode();
    Block.fRallyingScale      = pMeteor->GetRallyingScale();
    Block.fInterpolateLength  = pMeteor->GetMeteorInterpolate();
    Block.fDuration           = pMeteor->GetDuration();
	Block.dwSampling          = (DWORD)pMeteor->GetSampling();

    dwWriteNum += (int)::fwrite(&Block, 1, sizeof(SFX_METEOR_BLOCK), pFile);


    for (i = 0; i < (DWORD)Block.fTranslationFrameNum; ++i)
    {
        D3DXVECTOR3 vTrans;
        pMeteor->m_RtsTimeLine.m_TranslationTimeLine.GetKeyFrameByIndex(&nFrame, &vTrans, i);
        SFX_KEY_FRAME_VECTOR3   TransFrame;
        TransFrame.dwFrame = (DWORD)nFrame;
        TransFrame.Vector3 = vTrans;

        dwWriteNum += (DWORD)::fwrite(&TransFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR3), pFile);
    }

    for (i = 0; i < (DWORD)Block.fRotationFrameNum; ++i)
    {
        D3DXQUATERNION vRot;
        pMeteor->m_RtsTimeLine.m_RotationTimeLine.GetKeyFrameByIndex(&nFrame, &vRot, i);
        SFX_KEY_FRAME_VECTOR4   RotFrame;
        RotFrame.dwFrame = (DWORD)nFrame;
        RotFrame.q = vRot;

        dwWriteNum += (DWORD)::fwrite(&RotFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR4), pFile);
    }

    for (i = 0; i < (DWORD)Block.fScalingFrameNum; ++i)
    {
        D3DXVECTOR3 vScale;
        pMeteor->m_RtsTimeLine.m_ScalingTimeLine.GetKeyFrameByIndex(&nFrame, &vScale, i);
        SFX_KEY_FRAME_VECTOR3   ScaleFrame;
        ScaleFrame.dwFrame = (DWORD)nFrame;
        ScaleFrame.Vector3 = vScale;

        dwWriteNum += (DWORD)::fwrite(&ScaleFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR3), pFile);

        // verson 4

        D3DXQUATERNION qScanlRotation;
        pMeteor->m_RtsTimeLine.m_ScalRotationTimLine.GetKeyFrameByIndex(&nFrame, &qScanlRotation, i);
        SFX_KEY_FRAME_VECTOR4  ScaleRotationFrame;
        ScaleRotationFrame.dwFrame = (DWORD)nFrame;
        ScaleRotationFrame.q = qScanlRotation;
        dwWriteNum += (DWORD)::fwrite(&ScaleRotationFrame, 1, sizeof(SFX_KEY_FRAME_VECTOR4), pFile);

    }





    return dwWriteNum;
}


DWORD KG3DSFX::WriteModelBlock(FILE *pFile, DWORD dwIndex)
{
    SFX_MODEL_BLOCK Block;
    DWORD dwWriteNum = 0;
    int i = 0;
    _SfxModelInfo modelInfo = m_vecModelsInfo[dwIndex];

    memset(&Block, 0, sizeof(Block));

    ASSERT(modelInfo.pSFXModel);

    Block.dwElementNum = 0;

    Block.dwPlayMode    = modelInfo.dwPlayMode;
    Block.dwPlaySpeed   = *(DWORD*)&(modelInfo.fPlaySpeed);
    Block.dwPlayStart   = *(DWORD*)&(modelInfo.fPlayStart);
	Block.dwIsBillBoard	= modelInfo.dwIsBillBoard;	
    
    modelInfo.pSFXModel->GetScalingCenter(&Block.vScalingCenter);
    modelInfo.pSFXModel->GetScalingRotation(&Block.qScalingRotation);
    modelInfo.pSFXModel->GetScaling(&Block.vScaling);
    modelInfo.pSFXModel->GetRotationCenter(&Block.vRotationCenter);
    modelInfo.pSFXModel->GetRotation(&Block.qRotation);
    modelInfo.pSFXModel->GetTranslation(&Block.vTranslation);

    KG3DMaterial *pMaterial = modelInfo.pSFXModel->GetMaterial();
    
    Block.dwSubsetNum = pMaterial->m_dwNumMaterials;
    ASSERT(m_vecSubsetColor[dwIndex]->size() == Block.dwSubsetNum);
	Block.dwPlayConnect = static_cast<DWORD>(modelInfo.nCycleFlag);
	Block.dwStartFrame  = static_cast<DWORD>(modelInfo.nLeftFrame);
	Block.dwFinisFrame  = static_cast<DWORD>(modelInfo.nRighFrame);
    Block.dwBindTrackIndex = (modelInfo.nTrackIndex);

    TCHAR szFileName[MAX_PATH];
    TCHAR szRelName[MAX_PATH];
    modelInfo.pSFXModel->GetMeshFileName(szFileName);
    g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, szFileName, szRelName);
    strncpy((char*)Block.byMeshFileName, szRelName, min(SFX_MODEL_FILE_NAME_LENGTH, MAX_PATH));

    modelInfo.pSFXModel->GetMaterialFileName(szFileName);
    g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, szFileName, szRelName);
    strncpy((char*)Block.byMaterialFileName, szRelName, min(SFX_MODEL_FILE_NAME_LENGTH, MAX_PATH));

    modelInfo.pSFXModel->GetAnimationFileName(szFileName, MAX_PATH);
    g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, szFileName, szRelName);
    strncpy((char*)Block.byAnimationFileName, szRelName, min(SFX_MODEL_FILE_NAME_LENGTH, MAX_PATH));

    Block.byMeshFileName[SFX_MODEL_FILE_NAME_LENGTH - 1] = '\0';
    Block.byMaterialFileName[SFX_MODEL_FILE_NAME_LENGTH - 1] = '\0';
    Block.byAnimationFileName[SFX_MODEL_FILE_NAME_LENGTH - 1] = '\0';

    dwWriteNum += (DWORD)::fwrite(&Block, 1, sizeof(SFX_MODEL_BLOCK), pFile);

    MDL_COLOR_LINE_T* pMdlColorLine = m_vecSubsetColor[dwIndex];
    for (i = 0; i < (int)Block.dwSubsetNum; ++i)
    {
        COLOR_LINE_T *pColorLine = *(pMdlColorLine->begin() + i);//m_vecSubsetColor[i];
        SFX_MODEL_SUBSET Subset;
        memset(&Subset, 0, sizeof(Subset));

        Subset.dwDiffuseColorFrame = (DWORD)pColorLine->GetSize();
        Subset.dwElementNum = 0;
        dwWriteNum += (DWORD)::fwrite(&Subset, 1, sizeof(SFX_MODEL_SUBSET), pFile);
        for (int j = 0; j < (int)Subset.dwDiffuseColorFrame; ++j)
        {
            int nFrame;
            D3DXCOLOR Color;
            SFX_KEY_FRAME KeyFrame;
            pColorLine->GetKeyFrameByIndex(&nFrame, &Color, j);
            KeyFrame.dwFrame = (DWORD)nFrame;
            KeyFrame.dwValue = (DWORD)Color;
            dwWriteNum += (DWORD)::fwrite(&KeyFrame, 1, sizeof(SFX_KEY_FRAME), pFile);
        }
    }

    return dwWriteNum;
}

IKG3DAnimationController* KG3DSFX::GetAnimationController(enuAnimationControllerPriority nPriority)
{
    IKG3DAnimationController* pReturn = NULL;
	KG_PROCESS_ERROR(nPriority == ANICTL_PRIMARY);

    if (m_pSFXAnimationController == NULL)
    {
        m_pSFXAnimationController = new KG3DTagSFXAnimationController(this);
        KGLOG_PROCESS_ERROR(m_pSFXAnimationController);
    }
    pReturn = m_pSFXAnimationController;
Exit0:
    return pReturn;
}

void KG3DSFX::SetTagUsingInfo()
{
	m_pParentSFXSuede = NULL;
	KG3DTagSFXAnimationController* pNewContronler = new KG3DTagSFXAnimationController(this);
	KGLOG_PROCESS_ERROR(pNewContronler);
	if (m_pSFXAnimationController)
	{
		pNewContronler->Clone(m_pSFXAnimationController);
		delete m_pSFXAnimationController;
	}
	
	m_pSFXAnimationController = pNewContronler;
Exit0:
	;
}

HRESULT KG3DSFX::UpdateMatrix()
{
    HRESULT hResult = S_OK;
    D3DXMATRIX matParent;

	if (m_pParentMatrix)
		m_matWorldSave = *(m_pParentMatrix);
	else
		D3DXMatrixIdentity(&m_matWorldSave);

    switch(m_BindType)
    {
    case ENUM_BIND_NONE:
        {
			D3DXMATRIX matBindModel;
			D3DXMatrixIdentity(&matBindModel);
			if (m_pParentModel)
				matBindModel = m_pParentModel->m_matWorld;
            else
                matBindModel = m_matWorldSave;

            // remove this scanl
            if (m_dwFixedSize) 
            {
                D3DXVECTOR3 vScanl;
                D3DXVECTOR3 vTrans;
                D3DXMATRIX  matRot;
                D3DXMATRIX  matScal;

                MatrixExract(vScanl, vTrans, matRot, &matBindModel);
                D3DXMatrixScaling(&matScal, 1.f / vScanl.x, 1.f / vScanl.y, 1.f / vScanl.z);

                matBindModel = matScal * matBindModel;
            }

            if (m_dwLockYAlex)
                LockAxisY(matBindModel);

            m_matWorld = m_Matrix * /*m_matWorldSave*/ matBindModel;
            ASSERT_FLOAT_IS_VALIED(m_matWorld._11);
            break;
        }
    case ENUM_BIND_BONE:
        {
            ASSERT(m_pParentModel);
            hResult = S_OK;

            if (m_nBindIndex == -1)
                matParent = m_pParentModel->m_matWorld;
            else
                hResult = m_pParentModel->GetBoneMatrix(m_nBindIndex, &matParent, NULL);

            // remove this scanl

            if (m_dwFixedSize && SUCCEEDED(hResult)) 
            {
                D3DXVECTOR3 vScanl;
                D3DXVECTOR3 vTrans;
                D3DXMATRIX  matRot;
                D3DXMATRIX  matScal;

                MatrixExract(vScanl, vTrans, matRot, &matParent);
                D3DXMatrixScaling(&matScal, 1.f / vScanl.x, 1.f / vScanl.y, 1.f / vScanl.z);

                matParent = matScal * matParent;
            }

            if (m_dwLockYAlex && SUCCEEDED(hResult))
                LockAxisY(matParent);

            if (SUCCEEDED(hResult))
            {
                m_matWorld = m_Matrix * matParent * m_matWorldSave;
                ASSERT_FLOAT_IS_VALIED(m_matWorld._11);
            }
            else
            {
                m_matWorld = m_Matrix * m_matWorldSave;
                ASSERT_FLOAT_IS_VALIED(m_matWorld._11);

                return S_FALSE;
            }
            break;
        }
    case ENUM_BIND_SOCKET:
        {
            ASSERT(m_pParentModel);
            hResult = S_OK;
            hResult = m_pParentModel->GetSocketMatrix(m_nBindIndex, matParent);
            
            // remove this scanl

            if (m_dwFixedSize && SUCCEEDED(hResult)) 
            {
                D3DXVECTOR3 vScanl;
                D3DXVECTOR3 vTrans;
                D3DXMATRIX  matRot;
                D3DXMATRIX  matScal;

                MatrixExract(vScanl, vTrans, matRot, &matParent);
                D3DXMatrixScaling(&matScal, 1.f / vScanl.x, 1.f / vScanl.y, 1.f / vScanl.z);

                matParent = matScal * matParent;
            }

            if (m_dwLockYAlex && SUCCEEDED(hResult))
                LockAxisY(matParent);

            if (SUCCEEDED(hResult))
            {
                m_matWorld = m_Matrix * matParent * m_matWorldSave;
                ASSERT_FLOAT_IS_VALIED(m_matWorld._11);
            }
            else
            {
                m_matWorld = m_Matrix * m_matWorldSave;
                ASSERT_FLOAT_IS_VALIED(m_matWorld._11);
                hResult = S_FALSE;
            }
            break;
        }
	default :
		break;
    }

	if (m_nCloseFloorFlag)
	{
		IKG3DScene* pCurScene = g_cGraphicsTool.GetScene();
		if (pCurScene)
		{
			D3DXVECTOR3 vFloor = D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43);
			/*pCurScene->GetFloor(
				&vFloor, &D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43)
				);*/
			pCurScene->GetFloor(&vFloor, &vFloor.y);
			m_matWorld._42 = vFloor.y;
		}
	}

    //if (vAlexY != D3DXVECTOR3(0.f, 1.f, 0.f))
    //{
      /*  _SRT srt;
        MatrixExract(srt.vScanl, srt.vTrans, srt.matRot, &m_matWorld);

        D3DXMATRIX matScanl;
        D3DXMATRIX matTrans;
        D3DXMATRIX matRotat;

        D3DXMatrixTranslation(&matTrans, srt.vTrans.x, srt.vTrans.y, srt.vTrans.z);
        D3DXMatrixScaling(&matScanl, srt.vScanl.x,  srt.vScanl.y, srt.vScanl.z);

        D3DXVECTOR3 vAlexY = D3DXVECTOR3(srt.matRot._21, srt.matRot._22, srt.matRot._23);
        
        matRotat = srt.matRot;

        if (vAlexY != D3DXVECTOR3(0.f, 1.f, 0.f))
        {
            D3DXVec3Normalize(&vAlexY, &vAlexY);

            D3DXVECTOR3 vWordY = D3DXVECTOR3(0.f, 1.f, 0.f);
            D3DXVECTOR3 vCross;
            D3DXVec3Cross(&vCross, &vAlexY, &vWordY);
            D3DXVec3Normalize(&vCross, &vCross);

            FLOAT       fDot   = D3DXVec3Dot(&vAlexY, &vWordY);
            FLOAT       fAngel = acosf(fDot);
            D3DXMATRIX  matRot;

            D3DXMatrixRotationAxis(&matRot, &vCross, fAngel);

            matRotat = srt.matRot * matRot;

        }

        m_matWorld = matScanl * matRotat * matTrans;*/


    
  /*  D3DXVec3Normalize(&vAlexY, &vAlexY);

    D3DXVECTOR3 vWordY = D3DXVECTOR3(0.f, 1.f, 0.f);
    D3DXVECTOR3 vCross;
    D3DXVec3Cross(&vCross, &vAlexY, &vWordY);
    D3DXVec3Normalize(&vCross, &vCross);
    
    FLOAT       fDot   = D3DXVec3Dot(&vAlexY, &vWordY);
    FLOAT       fAngel = acosf(fDot);
    D3DXMATRIX  matRot;

    D3DXMatrixRotationAxis(&matRot, &vCross, fAngel);

    m_matWorld = matRot * m_matWorld;*/

    //}


    


    MatrixExract(m_Srt.vScanl, m_Srt.vTrans, m_Srt.matRot, &m_matWorld);

    //D3DXMATRIX matInv;
    //D3DXMatrixInverse(&matInv, NULL, &m_Srt.matRot);
    //m_matWorld = matInv * m_matWorld;

    return hResult;
}


HRESULT KG3DSFX::Playing()
{
	if(m_vecMagicParpet.size() > 0 )
		UpdateMatrix();
    for (size_t i = 0; i < m_vecMagicParpet.size(); ++i)
	{
		DWORD dwCurTime = timeGetTime();
		if(m_vecMagicParpet[i]->GetType() == AOE_FOOTPRINT && dwCurTime - m_dwLastTimeUpdateFootprint > 500)
		{
			m_vecMagicParpet[i]->Update(m_fCurrentFrame);
			m_vecMagicParpet[i]->NewOneFootprint();
			m_dwLastTimeUpdateFootprint = dwCurTime;
		}
		
	}
	return S_OK;
}

KG3DSFXSuede::KG3DSFXSuede()
{
    m_pParentModel = NULL;
    m_pSuedeAnimationController = NULL;
}


KG3DSFXSuede::~KG3DSFXSuede()
{
   UnInit();
}

HRESULT KG3DSFXSuede::LoadFromFile(IFile* pFile)
{
    HRESULT hr = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    SFX_BIND_HEADER header;
    SFX_BIND_BLOCK  block;

    int nModelCount = 0;
    unsigned long uSize = 0;

    KG_PROCESS_ERROR(pFile);

    uSize = pFile->Read(&header, sizeof(SFX_BIND_HEADER));
    KGLOG_PROCESS_ERROR(uSize == sizeof(SFX_BIND_HEADER));

    nModelCount = header.dwSFXCnt;

    while (nModelCount--)
    {
        _BindInfo bindInfo;
        KG3DModel* pModle = NULL;

        uSize = pFile->Read(&block, sizeof(SFX_BIND_BLOCK));
        KGLOG_PROCESS_ERROR(uSize == sizeof(SFX_BIND_BLOCK));

        hRetCode = g_cModelTable.LoadResourceFromFile(block.cszSFXFileName, 0, MLO_MULTI, (IKG3DResourceBase**)&pModle);
        KG_COM_PROCESS_ERROR(hRetCode);

        pModle->m_pParentSFXSuede = this;
        pModle->SetTranslation(&(block.vTransforation));
        pModle->SetScaling(&(block.vScaling));
        pModle->SetRotation(&(block.qRotation));
        if (pModle->GetType() == MESHTYPE_SFX)
            dynamic_cast<KG3DSFX*>(pModle)->SetScaleSize(block.SizeScale); 

        bindInfo.dwBindType = block.dwTargetType;
        bindInfo.strTarName = block.cszBoneName;
		bindInfo.FileName   = block.cszSFXFileName;
        bindInfo.pModel     = pModle;
		bindInfo.vTransforation = block.vTransforation;
		bindInfo.vScaling       = block.vScaling;
		bindInfo.qRotation      = block.qRotation;
		bindInfo.SizeScale      = block.SizeScale;
        m_vecBindInfo.push_back(bindInfo);
    }
    m_ParentModels.resize(m_vecBindInfo.size(), NULL);
    //m_scFullName = "";
    m_scName = "";
    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DSFXSuede::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    SFX_BIND_HEADER header;
    SFX_BIND_BLOCK  block;

    int nModelCount  = 0;
    size_t size      = 0;
    IFile* pFile     = NULL;

    pFile = g_OpenFile(cszFileName);
    KGLOG_PROCESS_ERROR(pFile);

    size = pFile->Read(&header, sizeof(SFX_BIND_HEADER));
    KG_PROCESS_ERROR(size == sizeof(SFX_BIND_HEADER));
    nModelCount = header.dwSFXCnt;

    while (nModelCount--)
    {
        _BindInfo bindInfo;
        KG3DModel* pModle = NULL;

        size = pFile->Read(&block, sizeof(SFX_BIND_BLOCK));
        KG_PROCESS_ERROR(size == sizeof(SFX_BIND_BLOCK));

        hRetCode = g_cModelTable.LoadResourceFromFile(block.cszSFXFileName, 0, MLO_MULTI, (IKG3DResourceBase**)&pModle);
        KG_COM_PROCESS_ERROR(hRetCode);

		pModle->m_pParentSFXSuede = this;
        pModle->SetTranslation(&(block.vTransforation));
        pModle->SetScaling(&(block.vScaling));
        pModle->SetRotation(&(block.qRotation));
        if (pModle->GetType() == MESHTYPE_SFX)
            dynamic_cast<KG3DSFX*>(pModle)->SetScaleSize(block.SizeScale); 

        bindInfo.dwBindType = block.dwTargetType;
        bindInfo.strTarName = block.cszBoneName;
		bindInfo.FileName   = block.cszSFXFileName;
        bindInfo.pModel     = pModle;
		bindInfo.vTransforation = block.vTransforation;
		bindInfo.vScaling       = block.vScaling;
		bindInfo.qRotation      = block.qRotation;
		bindInfo.SizeScale      = block.SizeScale;

        m_vecBindInfo.push_back(bindInfo);
    }
	m_ParentModels.resize(m_vecBindInfo.size(), NULL);
    m_scName = cszFileName;
    //m_scFullName = cszFileName;
    hResult = S_OK;
Exit0:
    KG_COM_RELEASE(pFile);
	if(FAILED(hResult))
	{
		KGLogPrintf(KGLOG_ERR, "KG3DSFXSuede::LoadFromFile %s failed.\n", cszFileName);
	}
    return hResult;
}


HRESULT KG3DSFXSuede::SaveToFile(const char cszFileName[])
{
    HRESULT hResult  = E_FAIL;

    SFX_BIND_HEADER header;
    FILE* pFile = NULL;
    size_t size = 0;

    Trim();

    header.dwSFXCnt = (DWORD)m_vecBindInfo.size();
    DWORD dwSFXCnt = header.dwSFXCnt;

    fopen_s(&pFile, cszFileName, "wb");
    KG_PROCESS_ERROR(pFile);

    size = fwrite(&header, 1, sizeof(SFX_BIND_HEADER), pFile);
    KG_PROCESS_ERROR(size == sizeof(SFX_BIND_HEADER));

    while (dwSFXCnt--)
    {
        KG3DModel* pModel = (KG3DModel*)m_vecBindInfo[dwSFXCnt].pModel;
        ASSERT(pModel);
        TCHAR szRelName[MAX_PATH];
        g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, pModel->m_scName.c_str(), szRelName);
        SFX_BIND_BLOCK block;
        memset(&block, 0, sizeof(block));
        strcpy_s(block.cszSFXFileName, 128, szRelName);
        block.Transformation = pModel->m_Matrix;

        if (pModel->GetType() == MESHTYPE_SFX)
            block.SizeScale = dynamic_cast<KG3DSFX*>(pModel)->GetScaleSize();
        else
            block.SizeScale = 1;

        block.PlayMode = (DWORD)pModel->m_dwPlayType;
        string strBone;     
        DWORD dwBindType = m_vecBindInfo[dwSFXCnt].dwBindType;
		pModel->GetTranslation(&(block.vTransforation));
		pModel->GetScaling(&(block.vScaling));
		pModel->GetRotation(&(block.qRotation));

		if (pModel->GetBindType() == ENUM_BIND_BONE)//这个因为可能保存的时候已经unbind了， 所以要检查实际的绑定情况来确定是不是要乘2.54
			block.vScaling *= 2.54f;

		block.dwTargetType = dwBindType;
		if (dwBindType == BIND_TYPE_BONE || dwBindType == BIND_TYPE_SOCKET)
		{
			strcpy_s(block.cszBoneName, 32, m_vecBindInfo[dwSFXCnt].strTarName.c_str());
		}
		else 
		{
			strBone._Copy_s(TEXT(""), 32, 32);
		}
        size = fwrite(&block, 1, sizeof(SFX_BIND_BLOCK), pFile);
        KG_PROCESS_ERROR(size == sizeof(SFX_BIND_BLOCK));
    }

    hResult = S_OK;
Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    return hResult;
}

HRESULT KG3DSFXSuede::SaveToMemory(void** ppData, DWORD *pdwSize)
{
    HRESULT hResult  = E_FAIL;
    SFX_BIND_HEADER* pHeader = NULL;
    SFX_BIND_BLOCK* pBindBlock = NULL;
    KG_PROCESS_ERROR(ppData);
    KG_PROCESS_ERROR(pdwSize);

    Trim();
    *pdwSize = sizeof(SFX_BIND_HEADER) + sizeof(SFX_BIND_BLOCK) * static_cast<DWORD>(m_vecBindInfo.size());
    
    *ppData = new BYTE[*pdwSize];
    KG_PROCESS_ERROR(*ppData);

    pHeader = reinterpret_cast<SFX_BIND_HEADER*>(*ppData);
    ASSERT(pHeader);

    pHeader->dwSFXCnt = static_cast<DWORD>(m_vecBindInfo.size());

    pBindBlock = reinterpret_cast<SFX_BIND_BLOCK*>(pHeader + 1);
    ASSERT(pBindBlock);

    for (DWORD i = 0; i < pHeader->dwSFXCnt; i++)
    {
        KG3DModel* pModel = (KG3DModel*)m_vecBindInfo[i].pModel;
        ASSERT(pModel);
        TCHAR szRelName[MAX_PATH];
        g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, pModel->m_scName.c_str(), szRelName);
        memset(pBindBlock, 0, sizeof(SFX_BIND_BLOCK));
        strcpy_s(pBindBlock->cszSFXFileName, 128, szRelName);
        pBindBlock->Transformation = pModel->m_Matrix;

        if (pModel->GetType() == MESHTYPE_SFX)
            pBindBlock->SizeScale = dynamic_cast<KG3DSFX*>(pModel)->GetScaleSize();
        else
            pBindBlock->SizeScale = 1;

        pBindBlock->PlayMode = (DWORD)pModel->m_dwPlayType;
        string strBone;     
        DWORD dwBindType = m_vecBindInfo[i].dwBindType;
        pModel->GetTranslation(&(pBindBlock->vTransforation));
        pModel->GetScaling(&(pBindBlock->vScaling));
        pModel->GetRotation(&(pBindBlock->qRotation));

        if (pModel->GetBindType() == ENUM_BIND_BONE)//这个因为可能保存的时候已经unbind了， 所以要检查实际的绑定情况来确定是不是要乘2.54
            pBindBlock->vScaling *= 2.54f;

        pBindBlock->dwTargetType = dwBindType;
        if (dwBindType == BIND_TYPE_BONE || dwBindType == BIND_TYPE_SOCKET)
        {
            strcpy_s(pBindBlock->cszBoneName, 32, m_vecBindInfo[i].strTarName.c_str());
        }
        else 
        {
            strBone._Copy_s(TEXT(""), 32, 32);
        }
        pBindBlock++;
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

BOOL KG3DSFXSuede::IsLoaded()
{
    if (m_bInMultiThreadLoadList)
        return FALSE;

    for (size_t i = 0; i < m_vecBindInfo.size(); ++i)
        if (m_vecBindInfo[i].pModel && !m_vecBindInfo[i].pModel->IsLoaded())
            return FALSE;
    return TRUE;
}


HRESULT KG3DSFXSuede::BindToSocket(IKG3DModel *pModel, const char* cszSocketName)
{
    return Bind(pModel);
}


HRESULT KG3DSFXSuede::BindToBone(IKG3DModel *prModel, const char* strBoneName)
{
    return Bind(prModel);
}


HRESULT KG3DSFXSuede::Bind(IKG3DModel* pModel)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    hRetCode = UnBindFromOther();
    KG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pModel);

    m_pParentModel = static_cast<KG3DModel*>(pModel);
	m_ParentModels.clear();
    for (DWORD i = 0; i < m_vecBindInfo.size(); i++)
    {
        m_ParentModels.push_back((KG3DModel*)pModel);
		if (!m_vecBindInfo[i].pModel)
			continue;
		switch (m_vecBindInfo[i].dwBindType)
        {
        case BIND_TYPE_BONE :
            hRetCode = m_vecBindInfo[i].pModel->BindToBone(
               m_pParentModel, m_vecBindInfo[i].strTarName.c_str()
            );
            KG_COM_PROCESS_ERROR(hRetCode);
            break;
        case BIND_TYPE_ORG :
            hRetCode = m_vecBindInfo[i].pModel->BindToBone(
                m_pParentModel, TEXT("")
            );
            KG_COM_PROCESS_ERROR(hRetCode);
            break;
        case BIND_TYPE_SOCKET :
            hRetCode = m_vecBindInfo[i].pModel->BindToSocket(
                m_pParentModel, m_vecBindInfo[i].strTarName.c_str()
            );
            KG_COM_PROCESS_ERROR(hRetCode);
            break;
        default :
			ASSERT(0);
            break;
        }
		
    }  

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSFXSuede::ApplyBind(IKG3DModel* pParent, UINT uIndex, BOOL bWorldBind /*= FALSE*/)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	KG3DModel* pParentModel = static_cast<KG3DModel*>(pParent);
	hRetCode = UnBindFromOther(uIndex);
	KG_COM_PROCESS_ERROR(hRetCode);
	KG_PROCESS_ERROR(pParent);
	KG_PROCESS_ERROR(uIndex < m_vecBindInfo.size());
	m_pParentModel = NULL;
	m_ParentModels[uIndex] = pParentModel;
	KG_PROCESS_ERROR(m_vecBindInfo[uIndex].pModel);
	switch (m_vecBindInfo[uIndex].dwBindType)
	{
	case BIND_TYPE_BONE :
		hRetCode = m_vecBindInfo[uIndex].pModel->BindToBone(
			pParentModel, m_vecBindInfo[uIndex].strTarName.c_str()
			);
		KG_COM_PROCESS_ERROR(hRetCode);
		break;
	case BIND_TYPE_ORG :
		hRetCode = m_vecBindInfo[uIndex].pModel->BindToBone(
			pParentModel, TEXT("")
			);
		KG_COM_PROCESS_ERROR(hRetCode);
		break;
	case BIND_TYPE_SOCKET :
		hRetCode = m_vecBindInfo[uIndex].pModel->BindToSocket(
			pParentModel, m_vecBindInfo[uIndex].strTarName.c_str()
			);
		KG_COM_PROCESS_ERROR(hRetCode);
		break;
	default :
		break;
	}

	if (bWorldBind)
	{
		D3DXMATRIX* pmatWorld;
		D3DXMATRIX matWorld;
		hRetCode = m_vecBindInfo[uIndex].pModel->GetMatWorld(&pmatWorld);
		KG_COM_PROCESS_ERROR(hRetCode);

		matWorld = *pmatWorld;
		m_vecBindInfo[uIndex].pModel->UnBindFromOther();
		m_vecBindInfo[uIndex].pModel->UpdateByMatrix(matWorld);
	}
	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSFXSuede::UnBindFromOther()
{
    HRESULT hResult  = E_FAIL;

    for (DWORD i = 0; i < m_vecBindInfo.size(); i++)
    {
        if (!m_vecBindInfo[i].pModel)
            continue;
        m_vecBindInfo[i].pModel->UnBindFromOther();
    }  

	m_pParentModel = NULL;
    hResult = S_OK;
    return hResult;
}

HRESULT KG3DSFXSuede::UnBindFromOther(UINT uIndex)
{
	HRESULT hrResult  = E_FAIL;

	KG_PROCESS_ERROR(uIndex < m_vecBindInfo.size());
	KG_PROCESS_ERROR(m_vecBindInfo[uIndex].pModel);
	m_vecBindInfo[uIndex].pModel->UnBindFromOther();
    KG_PROCESS_ERROR(uIndex < m_ParentModels.size());
    m_ParentModels[uIndex] = NULL;

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DSFXSuede::PlayAnimation(DWORD dwType)
{
	((KG3DSFXSuedeAnimationController*)GetAnimationController(ANICTL_PRIMARY))->m_nPlayCount = 0;
	((KG3DSFXSuedeAnimationController*)GetAnimationController(ANICTL_PRIMARY))->m_nFinishCnt = 0;

    for (DWORD i = 0; i < m_vecBindInfo.size(); i++)
    {
        if (m_vecBindInfo[i].pModel)
		{
            m_vecBindInfo[i].pModel->PlayAnimation(dwType, NULL, 0, 0, NULL, NULL, ANICTL_PRIMARY);
			((KG3DSFXSuedeAnimationController*)GetAnimationController(ANICTL_PRIMARY))->m_nPlayCount++;
		}
    }

	//GetAnimationController(ANICTL_PRIMARY)->SetMotionExtraInfo(IKG3DAnimationController::TYPE_SFX);
    return S_OK;
}


HRESULT KG3DSFXSuede::PlayAnimation(DWORD dwPlayType, LPCSTR strAnimationName, FLOAT fSpeed, int nOffsetTime, PVOID pReserved, PVOID pUserdata, enuAnimationControllerPriority Priority)
{
	((KG3DSFXSuedeAnimationController*)GetAnimationController(ANICTL_PRIMARY))->m_nPlayCount = 0;
	((KG3DSFXSuedeAnimationController*)GetAnimationController(ANICTL_PRIMARY))->m_nFinishCnt = 0;

    for (DWORD i = 0; i < m_vecBindInfo.size(); i++) 
    {
        if (m_vecBindInfo[i].pModel)
		{
            m_vecBindInfo[i].pModel->PlayAnimation(dwPlayType, strAnimationName, fSpeed, nOffsetTime, pReserved, pUserdata, ANICTL_PRIMARY);
			((KG3DSFXSuedeAnimationController*)GetAnimationController(ANICTL_PRIMARY))->m_nPlayCount++;
		}   
	}

	//GetAnimationController(ANICTL_PRIMARY)->SetMotionExtraInfo(IKG3DAnimationController::TYPE_SFX);

    return S_OK;
}


HRESULT KG3DSFXSuede::PlayAnimation(DWORD dwPlayType, FLOAT fSpeed, int nOffsetTime, PVOID pReserved, PVOID pUserdata, enuAnimationControllerPriority Priority)
{
	((KG3DSFXSuedeAnimationController*)GetAnimationController(ANICTL_PRIMARY))->m_nPlayCount = 0;
	((KG3DSFXSuedeAnimationController*)GetAnimationController(ANICTL_PRIMARY))->m_nFinishCnt = 0;

    for (DWORD i = 0; i < m_vecBindInfo.size(); i++) 
    {
        if (m_vecBindInfo[i].pModel)
		{
            m_vecBindInfo[i].pModel->PlayAnimation(dwPlayType, fSpeed, nOffsetTime, pReserved, pUserdata, ANICTL_PRIMARY);
			((KG3DSFXSuedeAnimationController*)GetAnimationController(ANICTL_PRIMARY))->m_nPlayCount++;
		}
    }

	//GetAnimationController(ANICTL_PRIMARY)->SetMotionExtraInfo(IKG3DAnimationController::TYPE_SFX);
    return S_OK;
}


DWORD KG3DSFXSuede::GetModelCount()
{
    return (DWORD)m_vecBindInfo.size();
}


IKG3DModel* KG3DSFXSuede::GetModel(DWORD dwIndex)
{
    if (dwIndex < m_vecBindInfo.size())
        return (IKG3DModel*)m_vecBindInfo[dwIndex].pModel;
    return NULL;
}

LPCSTR KG3DSFXSuede::GetModelName(DWORD dwIndex)
{
	LPCSTR strReturn = NULL;
	IKG3DModel* pModel = GetModel(dwIndex);
	KG_PROCESS_ERROR(pModel);
	strReturn = static_cast<KG3DModel*>(pModel)->m_scName.c_str();
Exit0:
	return strReturn;
}


KG3DModel* KG3DSFXSuede::GetBindModel()
{
    KG3DModel* pReturn = NULL;
	KG_PROCESS_ERROR(m_ParentModels.size());
	pReturn = m_ParentModels[0];
Exit0:
	return pReturn;
}

KG3DModel* KG3DSFXSuede::GetBindModel(UINT uIndex)
{
	KG3DModel* pReturn = NULL;
	KG_PROCESS_ERROR(m_ParentModels.size() > uIndex);
	pReturn = m_ParentModels[uIndex];
Exit0:
	return pReturn;
}

HRESULT KG3DSFXSuede::Render()
{
    size_t size = m_vecBindInfo.size();
    for (size_t s = 0; s < size; s++)
        m_vecBindInfo[s].pModel->Render(0, NULL);
    return S_OK;
}


HRESULT KG3DSFXSuede::FrameMove()
{
    size_t size = m_vecBindInfo.size();
    for (size_t s = 0; s < size; s++)
        m_vecBindInfo[s].pModel->FrameMove();
    return S_OK;
}


HRESULT KG3DSFXSuede::AddModel(KG3DModel* pModel)
{
    if (!pModel)
        return E_FAIL;
    _BindInfo bindInfo;
    string strTarName;
    bindInfo.pModel = pModel;
    switch (pModel->GetBindType())
    {
    case ENUM_BIND_BONE :
        pModel->GetBindBoneName(strTarName);
        bindInfo.dwBindType = BIND_TYPE_BONE;
        bindInfo.strTarName = strTarName;
        break;
    case ENUM_BIND_SOCKET:
        pModel->GetBindBoneName(strTarName);
        bindInfo.dwBindType = BIND_TYPE_SOCKET;
        bindInfo.strTarName = strTarName;
        break;
    default :
        bindInfo.dwBindType = BIND_TYPE_ORG;
        break;
    }
    m_vecBindInfo.push_back(bindInfo);
	m_ParentModels.push_back(NULL);
    return S_OK;
}


HRESULT KG3DSFXSuede::DelModel(DWORD dwIndex)
{
    if (dwIndex < m_vecBindInfo.size())
    {
        SAFE_RELEASE(m_vecBindInfo[dwIndex].pModel);
        m_vecBindInfo.erase(m_vecBindInfo.begin() + dwIndex);
		m_ParentModels.erase(m_ParentModels.begin() + dwIndex);
        return S_OK;
    }
    return E_FAIL;
}


HRESULT KG3DSFXSuede::Trim()
{
    for (int i = (int)m_vecBindInfo.size() - 1; i >= 0; i--)
    {
        if (!m_vecBindInfo[i].pModel)
		{
			m_vecBindInfo.erase(m_vecBindInfo.begin() + i);
			m_ParentModels.erase(m_ParentModels.begin() + i);
		}
    }
    return S_OK;
}


HRESULT KG3DSFXSuede::CopyBindInfo(int nSrcIndex, int nDstIndex /* = -1 */)
{
    HRESULT hResult  = E_FAIL;

    int nSubCnt = (int)m_vecBindInfo.size();

    _BindInfo Info;
	KG3DModel *pParentModel = NULL;
    KG_PROCESS_ERROR(nSrcIndex != nDstIndex);
    KG_PROCESS_ERROR(nSrcIndex >=  0);
    KG_PROCESS_ERROR(nSrcIndex < nSubCnt);
    KG_PROCESS_ERROR(nDstIndex < nSubCnt);

    Info.dwBindType = m_vecBindInfo[nSrcIndex].dwBindType;
    Info.pModel     = m_vecBindInfo[nSrcIndex].pModel;
    Info.strTarName = m_vecBindInfo[nSrcIndex].strTarName;
	Info.FileName   = m_vecBindInfo[nSrcIndex].FileName;
    Info.pModel->AddRef();

    if (nDstIndex == -1)
    {
        m_vecBindInfo.push_back(Info);
    }
    else
    {
        SAFE_RELEASE(m_vecBindInfo[nDstIndex].pModel);
        m_vecBindInfo[nDstIndex].dwBindType = Info.dwBindType;
        m_vecBindInfo[nDstIndex].pModel     = Info.pModel;
        m_vecBindInfo[nDstIndex].strTarName = Info.strTarName; 
		m_vecBindInfo[nDstIndex].FileName   = Info.FileName;
    }
	
	pParentModel = m_ParentModels[nSrcIndex];
	m_ParentModels.push_back(pParentModel);
	if (pParentModel)
	{
		pParentModel->AddRef();
	}

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT KG3DSFXSuede::ModifyInfo(int nIndex, KG3DModel* pModel, DWORD dwBindType, string TarName)
{
    HRESULT hResult  = E_FAIL;

    int nSubCnt = (int)m_vecBindInfo.size();

    IKG3DModel* pDepend = NULL;

    D3DXVECTOR3     vTrans = D3DXVECTOR3(0.f, 0.f, 0.f);
    D3DXVECTOR3     vScanl = D3DXVECTOR3(1.f, 1.f, 1.f);
    D3DXQUATERNION  qRoton;

    D3DXQuaternionIdentity(&qRoton);

    KG_PROCESS_ERROR(pModel);
    KG_PROCESS_ERROR(nIndex >= 0);
    KG_PROCESS_ERROR(nIndex <  nSubCnt);

    pModel->AddRef();

    if (m_vecBindInfo[nIndex].pModel)
    {
        m_vecBindInfo[nIndex].pModel->GetParentModel(&pDepend);
        UnBindFromOther(nIndex);

        m_vecBindInfo[nIndex].pModel->GetTranslation(&vTrans);
        m_vecBindInfo[nIndex].pModel->GetRotation(&qRoton);
        m_vecBindInfo[nIndex].pModel->GetScaling(&vScanl);
        
    }

    SAFE_RELEASE(m_vecBindInfo[nIndex].pModel);

    m_vecBindInfo[nIndex].pModel     = pModel;
    m_vecBindInfo[nIndex].dwBindType = dwBindType;
    m_vecBindInfo[nIndex].strTarName = TarName;
	m_vecBindInfo[nIndex].vTransforation = vTrans;
	m_vecBindInfo[nIndex].vScaling = vScanl;
	m_vecBindInfo[nIndex].qRotation = qRoton;

    m_vecBindInfo[nIndex].pModel->SetTranslation(&vTrans);
    m_vecBindInfo[nIndex].pModel->SetRotation(&qRoton);
    m_vecBindInfo[nIndex].pModel->SetScaling(&vScanl);


    if (pDepend)
        ApplyBind(pDepend, nIndex);

    hResult  = S_OK;
Exit0:
    return hResult;
}


//KG3DModel* KG3DSFXSuede::UpdateModel(KG3DModel* pModel, DWORD dwIndex)
//{
//    HRESULT hRetCode = E_FAIL;
//
//    KG3DModel* pModelold = NULL;
//    ASSERT(dwIndex < m_vecBindInfo.size());
//    ASSERT(pModel);
//      
//    KG_PROCESS_ERROR(dwIndex < m_vecBindInfo.size());
//    KG_PROCESS_ERROR(pModel);
//
//    pModelold = m_vecBindInfo[dwIndex].pModel;
//    if (pModelold && pModelold->GetBindType() == ENUM_BIND_BONE)
//    {
//        KG3DModel* pBindModel = NULL;
//        string strBoneName;
//        pModelold->GetParentModel((IKG3DModel**)&pBindModel);
//        ASSERT(pBindModel);
//        hRetCode = pModelold->GetBindBoneName(strBoneName);
//        ASSERT(hRetCode == S_OK);
//        pModel->BindToBone(pBindModel, strBoneName.c_str());
//        pModel->m_Matrix = pModelold->m_Matrix;
//    }
//    m_vecBindInfo[dwIndex].pModel = pModel;
//
//Exit0:
//    return pModelold;
//}

IKG3DAnimationController* KG3DSFXSuede::GetAnimationController(enuAnimationControllerPriority nPriority)
{
    KG_USE_ARGUMENT(nPriority);

    IKG3DAnimationController* pReturn = NULL;
    if (!m_pSuedeAnimationController)
    {
        m_pSuedeAnimationController = new KG3DSFXSuedeAnimationController(this);
        KGLOG_PROCESS_ERROR(m_pSuedeAnimationController);
    }
    pReturn = m_pSuedeAnimationController;
Exit0:
    return pReturn;
}


HRESULT KG3DSFXSuede::SetCallbackRender(CallbackRender fpCallbackRender, void* pUserData)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    IKG3DModel*  pModel   = NULL;

    KG_PROCESS_ERROR(GetModelCount() > 0);

    pModel = GetModel(0);
    KG_PROCESS_ERROR(pModel);

    hRetCode = pModel->SetCallbackRender(fpCallbackRender, pUserData);
    KG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT KG3DSFXSuede::SetAlpha(float fAlpha, BOOL bUseSpecial)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    for (DWORD i = 0; i < m_vecBindInfo.size(); i++) 
    {
        if (m_vecBindInfo[i].pModel)
        {
            hRetCode = m_vecBindInfo[i].pModel->SetAlpha(fAlpha, bUseSpecial);
            KG_COM_PROCESS_ERROR(hRetCode);
        }
    }

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT KG3DSFXSuede::GetRenderTargetTextureCount(OUT DWORD* pTexCount)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pTexCount);
    *pTexCount = 0;

    for (DWORD d = 0; d < m_vecBindInfo.size(); d++)
    {
        if (!m_vecBindInfo[d].pModel)
            continue;

        if (m_vecBindInfo[d].pModel->GetType() != MESHTYPE_SFX)
            continue;

        KG3DSFX* pSFX = (KG3DSFX*)m_vecBindInfo[d].pModel;

        for (DWORD i = 0; i < pSFX->m_vecBillboard.size(); i++)
        {
            if (pSFX->m_vecBillboard[i]->m_dwRenderTargetTexFlag)
            {
                if (pSFX->m_vecBillboard[i]->GetTexture())
                    (*pTexCount)++;
                if (pSFX->m_vecBillboard[i]->GetTexture2())
                    (*pTexCount)++;
            }
        }
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSFXSuede::UnInit()
{
    for (DWORD i = 0; i < m_vecBindInfo.size(); i++)
        SAFE_RELEASE(m_vecBindInfo[i].pModel);
    m_vecBindInfo.clear();
    m_ParentModels.clear();
    SAFE_DELETE(m_pSuedeAnimationController);

	KG3DModel::UnInit();

    return S_OK;
}

HRESULT KG3DSFXSuede::GetRenderTargetTexture(OUT DWORD* pResourceId, IN DWORD dwTexIndex)
{
    HRESULT hResult  = E_FAIL;

    KG3DTexture* pTexture = NULL;


    int nRenterTexIndex = -1;

    KGLOG_PROCESS_ERROR(dwTexIndex >=0);
    KGLOG_PROCESS_ERROR(pResourceId);

    for (DWORD d = 0; d < m_vecBindInfo.size(); d++)
    {
        if (m_vecBindInfo[d].pModel->GetType() != MESHTYPE_SFX)
            continue;
        KG3DSFX* pSFX = (KG3DSFX*)m_vecBindInfo[d].pModel;

        DWORD i = 0;
        for (i = 0; i < pSFX->m_vecBillboard.size(); i++)
        {
            if (pSFX->m_vecBillboard[i]->m_dwRenderTargetTexFlag)
            {
                pTexture = (KG3DTexture*)pSFX->m_vecBillboard[i]->GetTexture();
                if (pTexture)
                    nRenterTexIndex++;
                if (nRenterTexIndex == (int)dwTexIndex)
                    break;
                pTexture = (KG3DTexture*)pSFX->m_vecBillboard[i]->GetTexture2();
                if (pTexture)
                    nRenterTexIndex++;
                if (nRenterTexIndex == (int)dwTexIndex)
                    break;    
            }
        }

        if (i != pSFX->m_vecBillboard.size())
            break;
    }

    KGLOG_PROCESS_ERROR(nRenterTexIndex == (int)dwTexIndex);
    KGLOG_PROCESS_ERROR(pTexture);

    pTexture->GetID(pResourceId);

    hResult = S_OK;
Exit0:
    return hResult;
}



IKG3DAnimationController* KG3DSFX::GetSplitAnimationContorller(DWORD dwPartIndex, DWORD dwControllerIndex)
{
    return NULL;
}


IKG3DAnimationController* KG3DSFXSuede::GetSplitAnimationContorller(DWORD dwPartIndex, DWORD dwControllerIndex)
{
    return NULL;
}

HRESULT KG3DSFXSuede::Clone(KG3DSFXSuede* pSrc)
{
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(pSrc);

	for (size_t i=0;i<pSrc->m_vecBindInfo.size();i++)
	{
		_BindInfo& SrcInfo = pSrc->m_vecBindInfo[i];
		KG3DModel* pModel = NULL;
		_BindInfo  NewInfo;
		
		hr = g_cModelTable.LoadResourceFromFile(SrcInfo.FileName.c_str(), 0, MLO_MULTI, (IKG3DResourceBase**)&pModel);
		KG_COM_PROCESS_ERROR(hr);

		pModel->m_pParentSFXSuede = this;
		pModel->SetTranslation(&(SrcInfo.vTransforation));
		pModel->SetScaling(&(SrcInfo.vScaling));
		pModel->SetRotation(&(SrcInfo.qRotation));

		if (pModel->GetType() == MESHTYPE_SFX)
			dynamic_cast<KG3DSFX*>(pModel)->SetScaleSize(SrcInfo.SizeScale); 

		NewInfo.dwBindType = SrcInfo.dwBindType;
		NewInfo.strTarName = SrcInfo.strTarName;
		NewInfo.FileName   = SrcInfo.FileName;
		NewInfo.pModel     = pModel;
		NewInfo.vTransforation = SrcInfo.vTransforation;
		NewInfo.vScaling       = SrcInfo.vScaling;
		NewInfo.qRotation      = SrcInfo.qRotation;
		NewInfo.SizeScale      = SrcInfo.SizeScale;

		m_vecBindInfo.push_back(NewInfo);
	}
	m_ParentModels.resize(m_vecBindInfo.size(), NULL);

	hr = S_OK;
Exit0:
	return hr;
}


BOOL KG3DSFX::IsSortAsSFX()
{
	return TRUE;
}


BOOL KG3DSFX::IsResourceReady(BOOL bTestText)
{
	return IsLoaded();
}


HRESULT KG3DSFXSuede::ChangeModel(UINT uIndex, KG3DModel *pModel)
{
	HRESULT hResult = E_FAIL;
	KG_PROCESS_ERROR(uIndex < m_vecBindInfo.size());

	SAFE_RELEASE(m_vecBindInfo[uIndex].pModel);
	m_vecBindInfo[uIndex].pModel = pModel;
	pModel->AddRef();
	hResult = S_OK;
Exit0:
	return hResult;
}

DWORD KG3DSFX::GetSourceReference(TagSourceInfo *pInfo, DWORD dwSize)
{
	DWORD dwReturn = 0;
	
	//暂时只处理Sound的
	DWORD dwKeySize = static_cast<DWORD>(m_tlSoundKey.size());
	KG_PROCESS_ERROR(dwSize >= dwKeySize);

	for (DWORD i = 0; i < dwKeySize; i++)
	{
		int nFrame = 0;
		_SoundInfo* pSoundBlock = m_tlSoundKey.GetDataByIndex(i, nFrame);
		(pInfo+dwReturn)->dwFrame = static_cast<DWORD>(nFrame);
		(pInfo+dwReturn)->dwIndex = 0;
		(pInfo+dwReturn)->dwType = 0;
		(pInfo+dwReturn)->strFilePath = pSoundBlock->vSubSound.back().szFileName;
		TCHAR strFileName[MAX_PATH];
		TCHAR strFileExt[MAX_PATH];
		_splitpath_s(pSoundBlock->vSubSound.back().szFileName,
			NULL,
			0,
			NULL,
			0,
			strFileName,
			MAX_PATH,
			strFileExt,
			MAX_PATH);
		sprintf_s((pInfo+dwReturn)->strFileName,
			MAX_PATH,
			"%s%s",
			strFileName,
			strFileExt);
		(pInfo+dwReturn)->dwHash = g_FileNameHash(pSoundBlock->vSubSound.back().szFileName);
		dwReturn++;
	}
	
Exit0:
	return dwReturn;
}

HRESULT KG3DSFX::ChangeResource(UINT uFrame, UINT uIndex, LPCSTR strNew)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	_SoundInfo *pSoundBlock = NULL;
	
	hRetCode = m_tlSoundKey.QueryByFrame(uFrame, &pSoundBlock);
	KG_COM_PROCESS_ERROR(hRetCode);

	strcpy_s(pSoundBlock->vSubSound.back().szFileName, 128, strNew);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSFX::DeleteResource(UINT uFrame, UINT uIndex)
{
	m_tlSoundKey.Remove(uFrame);
	return S_OK;
}
HRESULT KG3DSFX::AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault)
{
    _SoundInfo Info;
	SFX_SOUND_BLOCK NewSoundBlock;
	ZeroMemory(&NewSoundBlock, sizeof(SFX_SOUND_BLOCK));
	NewSoundBlock.dwSound = 0xffffffff;
	if (!pDefault)
	{
		NewSoundBlock.dwMode  = 0;
		NewSoundBlock.fVolume = 100.0f;
		NewSoundBlock.bLoop   = FALSE;
		NewSoundBlock.bContinue = TRUE;
	}
	else
	{
		SoundDataInfoVersion2 *pNew = static_cast<SoundDataInfoVersion2*>(pDefault);
		NewSoundBlock.fVolume = pNew->SoundCandidates[0].fVolume;
		NewSoundBlock.bLoop = pNew->bLoop;
		NewSoundBlock.dwMode = (pNew->InterruptMode == SPI_ENABLE) ? SFX_SOUND_MODE_INT : SFX_SOUND_MODE_WAIT;
		NewSoundBlock.bContinue = (pNew->MotionChangeState == SMCS_KEEP);
	}
	Info.vSubSound.push_back(NewSoundBlock);
	NewSoundBlock.nProbability = 1;
	Info.nProbabilitySum = 1;
	strcpy_s(NewSoundBlock.szFileName, 128, strAdd);
	return m_tlSoundKey.InsertKeyFrame(uFrame, Info);
}

//HRESULT KG3DSFX::AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault)
//{
//    _SoundInfo Info;
//	SFX_SOUND_BLOCK NewSoundBlock;
//	strcpy_s(NewSoundBlock.szFileName, MAX_PATH, strAdd);
//    Info.vSubSound.push_back(NewSoundBlock);
//    NewSoundBlock.nProbability = 1;
//    Info.nProbabilitySum = 1;
//	return m_tlSoundKey.InsertKeyFrame(uFrame, Info);
//}

HRESULT KG3DSFX::DeleteAllSound()
{
	//m_vecSoundKey.Clear();
	return S_OK;
}

HRESULT KG3DSFX::ChangePlayType(UINT uFrame, UINT uIndex, void *pDefault)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	SoundDataInfoVersion2 *pNew = NULL;
	SFX_SOUND_BLOCK *pSoundBlock = NULL;
	KG_PROCESS_ERROR(pDefault);

	//hRetCode = m_vecSoundKey.QueryByFrame(uFrame, &pSoundBlock);
	KG_COM_PROCESS_ERROR(hRetCode);

	pNew = static_cast<SoundDataInfoVersion2*>(pDefault);
	pSoundBlock->fVolume = pNew->SoundCandidates[0].fVolume;
	pSoundBlock->bLoop = pNew->bLoop;
	pSoundBlock->dwMode = (pNew->InterruptMode == SPI_ENABLE) ? SFX_SOUND_MODE_INT : SFX_SOUND_MODE_WAIT;
	pSoundBlock->bContinue = (pNew->MotionChangeState == SMCS_KEEP);

	hResult = S_OK;
Exit0:
	return hResult;
}


void KG3DSFXSuede::Reset()
{
	for (size_t i = 0; i < m_vecBindInfo.size(); i++)
	{
		_BindInfo& Bind = m_vecBindInfo[i];
		Bind.pModel->SetTranslation(&Bind.vTransforation);
		Bind.pModel->SetScaling(&Bind.vScaling);
		Bind.pModel->SetRotation(&Bind.qRotation);
	}
}

HRESULT KG3DSFX::OnReferenceZero()
{
	//ATLTRACE("Release sfx, %s\n", m_scName.c_str());
	return g_cModelTable.ReleaseResource(this, RELEASE_ATONCE);
}