
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRepresentNPC.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-9-27 13:11:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DRepresentNPC.h"
//#include "FileType.h"
#include "KG3DModelTable.h"
#include "KG3DClip.h"
#include "KG3DGraphicsTool.h"
#include "KG3DScene.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DRepresentObjectSet.h"
#include "KG3DRepresentObjectVisibleEntitySet.h"
#include "KG3DRepresentObjectSoundBall.h"
#include "KG3DRepresentObjectPVS.h"
#include "KG3DModelSpeedTree.h"
#include "KG3DRepresentObjectTable.h"
#include "IEEditor.h"
#include "KG3DSceneFunctions.h"//GetLogicalHeight
#include "KG3DCollocator.h"	//g_Switch
#include "MathTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define APERTURE TEXT("\\Data\\source\\other\\特效\\aperture.Sfx")

BOOL g_bRenderRepresentPoly = TRUE;
BOOL g_bRenderAlertRange = FALSE;
BOOL g_bRenderAttackRange = FALSE;
DWORD g_dwSmallRa = 100;
DWORD g_dwBigRa = 500;

//static D3DXQUATERNION s_quatFloor;
extern BOOL g_bDesign;



KG3DRepresentNPC::KG3DRepresentNPC(void)
:m_vNewDir(1,0,0)
,m_vAxis(1,0,0)
,m_vecCurFloor(0,0,0)
,m_vLogicalFloor_CellOrder(0,0,0,0)
{
    m_dwTemplateID = 0;
    //m_nTemplateIndex = 0;
	m_nAlertRange = 0;
	m_nAttackRange = 0;
	
    m_pModelFace     = NULL;
    m_vecPrePosition = D3DXVECTOR3(0.0f, 0.0f,  0.0f);
    m_vecCurPosition = D3DXVECTOR3(0.0f, 0.0f,  0.0f);
    m_vecDirection   = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
    m_vecPreDirecn   = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
    m_fAngleHor      = 0.0f;
    m_fAngelRateHor  = D3DX_PI / 1800;
    m_dwNpcType	     = NPC_TYPE_COMPUTER;
    m_fMoveSpeed	 = 800.0f;
    m_dwMovementType = (DWORD)NPC_MOVEMENT_TYPE_STAND;
    //m_lpScene        = NULL;//<SetScene Refactor>
	//m_nDisableAutoTurn = 0;
    //m_pAperture = NULL;
    //(by dengzhihui 2006年9月15日 16:50:17
    m_eNPCState = nmsNone;
    m_fLegLen = 100.f;

	m_dwRepresentID = 0;
    //)
	
	m_bAttackRange = true; //by huangjinshou 2007年7月13日

	m_dwReliveID = 0;
	m_dwRandomID = 0;
	m_dwAISetID  = 0;
}

KG3DRepresentNPC::~KG3DRepresentNPC(void)
{
	UnInit();
}

//HRESULT KG3DRepresentNPC::BandScene(IKG3DScene* lpScene)//<SetScene Refactor>
//{
//    HRESULT hResult  = E_FAIL;
//
//    KG_PROCESS_ERROR(lpScene);
//    m_lpScene = dynamic_cast<KG3DScene*>(lpScene);
//
//    hResult = S_OK;
//Exit0:
//    return hResult;
//}

HRESULT  KG3DRepresentNPC::SetNPCTranslation(D3DXVECTOR3* pValue)
{
    KG3DTransformation::SetTranslation(pValue);
    return S_OK;
}
HRESULT  KG3DRepresentNPC::SetNPCRotation(D3DXQUATERNION* pValue)
{
    KG3DTransformation::SetRotation(pValue);
    return S_OK;
}

HRESULT KG3DRepresentNPC::Init()
{
    return KG3DRepresentObject::Init();
}

HRESULT KG3DRepresentNPC::UnInit()
{
    /*if ( NULL != m_pAperture ) 
    {
    g_cModelTable.ReleaseResource( m_pAperture, RELEASE_ATONCE );
    m_pAperture = NULL;
    }*/

    if(m_pModelFace)
    {
        m_pModelFace->UnBindFromOther();
    }
    SAFE_RELEASE(m_pModelFace);
    return KG3DRepresentObject::UnInit();
}

HRESULT KG3DRepresentNPC::LoadByLogicId(DWORD dwNpcId)
{
    HRESULT hResult  = E_FAIL;

    hResult = S_OK;
    return hResult;
}

HRESULT KG3DRepresentNPC::LoadFromIniFileEx(LPSTR pFileName,LPSTR pSectionName,DWORD dwOption)
{
	HRESULT hr = E_FAIL;
	IIniFile* pFile = NULL;

	pFile = g_OpenIniFile(pFileName);
	KGLOG_PROCESS_ERROR(pFile);

	hr = LoadFromIniSection(pFile,pSectionName,dwOption);

Exit0:
	SAFE_RELEASE(pFile);
	return hr;
}

HRESULT KG3DRepresentNPC::LoadFromIniSection(IIniFile* pFile,LPSTR pSectionName,DWORD dwOption)
{
	HRESULT hr = E_FAIL;

	TCHAR szSceneRunnerModelName[MAX_PATH];
	TCHAR szSceneRunnerMtlName[MAX_PATH];
	TCHAR szSceneRunnerAniName[MAX_PATH];
	TCHAR szSceneRunnerFaceModelName[MAX_PATH];
	TCHAR szSceneRunnerFaceMtlName[MAX_PATH];

	pFile->GetString(pSectionName,"RUNNERMODEL","Data\\public\\scenerunner\\scenerunner.Mesh",szSceneRunnerModelName,MAX_PATH);
	pFile->GetString(pSectionName,"RUNNERMTL","Data\\public\\scenerunner\\scenerunner.Mtl",szSceneRunnerMtlName,MAX_PATH);
	pFile->GetString(pSectionName,"RUNNERANI","Data\\public\\scenerunner\\scenerunner.Ani",szSceneRunnerAniName,MAX_PATH);
	pFile->GetString(pSectionName,"RUNNERFACEMODEL","",szSceneRunnerFaceModelName,MAX_PATH);
	pFile->GetString(pSectionName,"RUNNERFACEMTL","",szSceneRunnerFaceMtlName,MAX_PATH);	

	{
		hr = SetModel(0, szSceneRunnerModelName, 
			NULL,
			szSceneRunnerAniName, FALSE);
		if (FAILED(hr))
		{
			KGLogPrintf(KGLOG_ERR, _T("P模式的SceneRunner载入失败,Model：%s, Ani: %s")
				, szSceneRunnerModelName, szSceneRunnerAniName);
		}

		if(_strcmpi(szSceneRunnerFaceModelName,"")!=0)
		{
			SetModelFace(szSceneRunnerFaceModelName,"","","S_Face");
		}

		SetNpcFrontDir(D3DXVECTOR3(0.0f,0.0f, -1.0f));
		SetNpcType(NPC_TYPE_PROTAGONIST);

		KG3DModel *pNPCBody = NULL;
		GetModel(pNPCBody, 0,TRUE);
		if(pNPCBody)
		{
			pNPCBody->SetClientAdd(TRUE);
		}
	}
	return S_OK;
}


HRESULT KG3DRepresentNPC::LoadFromFile(LPCTSTR cszFileName, unsigned uFileNameHash, unsigned uOption)
{
    HRESULT hr = SetModel(0, cszFileName, "", "",TRUE);
	if (FAILED(hr))
	{
		SetModel(0, _T("Data\\public\\scenerunner\\scenerunner.Mesh"), "", "",TRUE);
	}
	for (size_t i=0;i<m_vecModel.size();i++)
	{
		if(m_vecModel[i])
		{
			m_vecModel[i]->SetClientAdd(TRUE);
		}
	}
    return S_OK;
}

HRESULT KG3DRepresentNPC::Copy(KG3DRepresentObject* pSrcObject)
{
    KG3DRepresentNPC* pSrcNPC = static_cast<KG3DRepresentNPC*>(pSrcObject);

    m_scNPCName = pSrcNPC->m_scNPCName;
    m_dwTemplateID = pSrcNPC->m_dwTemplateID;

    m_nKind = pSrcNPC->m_nKind;
    m_nLevel = pSrcNPC->m_nLevel;
    //m_nAIKind = pSrcNPC->m_nAIKind;
    m_dwRepresentID = pSrcNPC->m_dwRepresentID;
	m_nAlertRange = pSrcNPC->m_nAlertRange;
	m_nAttackRange = pSrcNPC->m_nAttackRange;
    //m_nDisableAutoTurn = pSrcNPC->m_nDisableAutoTurn;

	m_dwReliveID = pSrcNPC->m_dwReliveID;
	m_dwRandomID = pSrcNPC->m_dwRandomID;
	m_dwAISetID  = pSrcNPC->m_dwAISetID;

    KG3DRepresentObject::Copy(pSrcNPC);

    TCHAR szFaceMesh[MAX_PATH];
    TCHAR szFaceMtl[MAX_PATH];
    if(pSrcNPC->m_pModelFace)
    {
        pSrcNPC->m_pModelFace->GetMeshFileName(szFaceMesh);
        pSrcNPC->m_pModelFace->GetMaterialFileName(szFaceMtl);

        SetModelFace(szFaceMesh,szFaceMtl,"","S_Face");
    }
	FrameMove();
    return S_OK;
}

HRESULT KG3DRepresentNPC::SetModelFace(LPCTSTR pMeshFile,LPCTSTR pMtlFile,LPCTSTR pAniFile,LPCTSTR pSocketName)
{
    SAFE_RELEASE(m_pModelFace);

    if (!pMeshFile)
        return E_FAIL;

    if(SUCCEEDED(g_cModelTable.LoadResourceFromFile(pMeshFile, 0, MLO_MULTI, 
        (IKG3DResourceBase **)&m_pModelFace)))
    {
		m_pModelFace->SetClientAdd(TRUE);

        if(_strcmpi(pAniFile,"")!=0)
        {
            if(m_pModelFace)
            {
                if(SUCCEEDED(m_pModelFace->LoadAnimationFromFile(pAniFile,0)))
                {
                    m_pModelFace->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, pAniFile, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
                    //m_pModelFace->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
                }
            }
        }
        if((pSocketName)&&(_strcmpi(pSocketName,"")!=0))
        {
            KG3DModel* pModel = NULL;
            if(SUCCEEDED(GetModel(pModel,0,FALSE)))
            {
                if(pModel)
                {
                    m_pModelFace->BindToSocket(pModel,pSocketName);
                    //m_pModelFace->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
                }
            }
        }
    }
    return S_OK;
}

HRESULT KG3DRepresentNPC::RenderHierarchy(BOOL bCull)
{
    return KG3DRepresentObject::Render(0);
}



HRESULT KG3DRepresentDoodad::Render(DWORD dwOption)
{
	if(GetDistance()<=25000000*4)
	{
		KG3DRepresentObject::Render(dwOption);
	}

	BOOL bSelected = FALSE;
	if (g_bDesign)
	{
		KG3DScene *pScene = NULL;
		KG3DSceneSceneEditor *pEditor = NULL;

		pScene = g_cGraphicsTool.GetCurScene();
		KG_PROCESS_ERROR(pScene);

		pEditor = dynamic_cast<KG3DSceneSceneEditor*>(pScene);
		KG_PROCESS_ERROR(pEditor);

		bSelected = pEditor->GetSelectionTool().IsSelected(*this);
	}

	if(! g_bClient && (g_cEngineManager.GetRenderOption(RENDER_INFO) || bSelected))
	{
		TCHAR Name[256];
		sprintf(Name,"%s(%d)",m_scDoodadName.c_str(),m_dwReliveID);
		D3DXVECTOR3 pos = m_Translation+D3DXVECTOR3(0,200,0);
		g_cGraphicsTool.DrawScreenText3D(pos,Name,0xFF00FF30);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentNPC::DrawVisibleSphere(D3DCOLORVALUE color,float radius,float hight )
{
	GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetTexture(1,NULL);
	//DWORD dwCullMode;
	//g_pd3dDevice->GetRenderState(D3DRS_CULLMODE,&dwCullMode);
	//g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	g_cGraphicsTool.DrawCylinderEx(&m_Matrix,color,radius,hight,TRUE);
	g_cGraphicsTool.DrawSphereEx(&m_Matrix,color,radius,TRUE);

	//g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,dwCullMode);

	return S_OK;
}

HRESULT KG3DRepresentNPC::PauseAnimation(BOOL bPause)
{
	for (DWORD i = 0; i < GetModelCount(); i++)
	{
		KG3DModel* pModel = GetModel(i);
		if(NULL != pModel)
            pModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(bPause);
    }

    if(m_pModelFace)
    {
        m_pModelFace->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(bPause);
    }
    return S_OK;
}

HRESULT KG3DRepresentNPC::PlayAnimation(LPCSTR szAniPath)
{
    if (!szAniPath)
        return E_FAIL;

	for (DWORD i = 0; i < GetModelCount(); i++)
	{
		KG3DModel* pModel = GetModel(i);
		if(NULL != pModel)
        {
            pModel->PlayAnimation(
                ENU_ANIMATIONPLAY_CIRCLE, szAniPath, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
        }
    }
    return S_OK;
}

HRESULT KG3DRepresentNPC::SetAlpha(float fAlpha)
{
	for (DWORD i = 0; i < GetModelCount(); i++)
	{
		KG3DModel* pModel = GetModel(i);
		if(NULL != pModel)
           pModel->SetAlpha(fAlpha, TRUE);
    }

    if(m_pModelFace)
    {
        m_pModelFace->SetAlpha(fAlpha, TRUE);
    }
    return S_OK;
}


HRESULT KG3DRepresentNPC::GetPrePosition(D3DXVECTOR3* pVecPosition)
{
    *pVecPosition = m_vecPrePosition;
    return S_OK;
}

HRESULT KG3DRepresentNPC::GetCurPosition(D3DXVECTOR3* pVecPosition)
{
    *pVecPosition = m_vecCurPosition;
    return S_OK;
}

HRESULT KG3DRepresentNPC::SetCurPosition(D3DXVECTOR3 vecPosition, KG3DScene& sceneCur, int nSnapToGround)
{
    HRESULT hResult  = E_FAIL;

    if (nSnapToGround)
    {
		D3DXVECTOR3 vCheckPos = vecPosition;
		vCheckPos.y += 200.f;//加上身高来测	
		D3DXVECTOR3 vFloorRet = vecPosition;
		HRESULT hr = sceneCur.GetFloor(&vCheckPos, &vFloorRet.y);
      
		if (vecPosition.y - m_fLegLen <= vFloorRet.y)
		{
			vecPosition.y  = vFloorRet.y;		
            m_vecPrePosition  = vecPosition;
			m_fUpSpeed = 0.0f;
		}

		if (SUCCEEDED(hr))
		{
			m_vecCurPosition = vecPosition;	 
		}
		else
		{
			m_vecPrePosition  = m_vecCurPosition;
			FLOAT fOldY = m_vecCurPosition.y;
			m_vecCurPosition = vecPosition;	 
			m_vecCurPosition.y = fOldY;
		}
	}
    else
    {
        m_vecCurPosition = vecPosition;	
    }

    SetTranslation(&m_vecCurPosition);

    //Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}


HRESULT KG3DRepresentNPC::SetNpcFrontDir(D3DXVECTOR3 vecFront)
{
    m_vecDirection  = vecFront;
    m_vecPreDirecn  = vecFront;

    return S_OK;
}

HRESULT KG3DRepresentNPC::SetNpcType(DWORD dwNpcType)
{
    m_dwNpcType = dwNpcType;
    return S_OK;
}

HRESULT KG3DRepresentNPC::SetDirection(D3DXVECTOR3 vecDirection)
{
    D3DXVECTOR3 vecCross;
    float fLen = D3DXVec3Length(&vecDirection) * D3DXVec3Length(&m_vecDirection);
    float fDot = D3DXVec3Dot(&vecDirection, &m_vecDirection);
    float fCos = fDot / fLen;
    if (fCos >  1.0f)
        fCos =  1.0f;
    if (fCos < -1.0f)
        fCos = -1.0f;
    float fAng = acosf(fCos);
    D3DXVec3Cross(&vecCross, &vecDirection, &m_vecDirection);
    if (vecCross.y > 0.0f)
        TurnLef(fAng);
    else
        TurnRig(fAng);

    return S_OK;
}

HRESULT KG3DRepresentNPC::GetDirection(D3DXVECTOR3* pVecDirection)
{
    *pVecDirection = m_vecPreDirecn;
    return S_OK;
}

HRESULT KG3DRepresentNPC::GoAhead(float fSpeed, KG3DScene& sceneCur)
{
    D3DXVECTOR3 vecPosition = m_vecCurPosition;
    vecPosition += m_vecDirection * fSpeed;
    SetCurPosition(vecPosition, sceneCur, true);

    m_dwMovementType = (DWORD)NPC_MOVEMENT_TYPE_AHEAD;

    return S_OK;
}

HRESULT KG3DRepresentNPC::BackOff(float fSpeed, KG3DScene& sceneCur)
{
    D3DXVECTOR3 vecPosition = m_vecCurPosition;
    vecPosition -= m_vecDirection * fSpeed;
    SetCurPosition(vecPosition, sceneCur, true);

    m_dwMovementType = (DWORD)NPC_MOVEMENT_TYPE_BACK;

    return S_OK;
}

HRESULT KG3DRepresentNPC::MoveLef(float fSpeed, KG3DScene& sceneCur)
{
    D3DXVECTOR3 vecPosition = m_vecCurPosition;
    D3DXVECTOR3 vecDirLeft;
    D3DXMATRIX	matRotation;

    D3DXMatrixRotationY(&matRotation, -D3DX_PI / 2);
    D3DXVec3TransformCoord(&vecDirLeft, &m_vecDirection, &matRotation);
    vecPosition += vecDirLeft * fSpeed;
    SetCurPosition(vecPosition, sceneCur, true);

    m_dwMovementType = (DWORD)NPC_MOVEMENT_TYPE_LEFT;

    return S_OK;
}

HRESULT KG3DRepresentNPC::MoveRig(float fSpeed, KG3DScene& sceneCur) 
{
    D3DXVECTOR3 vecPosition = m_vecCurPosition;
    D3DXVECTOR3 vecDirLeft;
    D3DXMATRIX	matRotation;

    D3DXMatrixRotationY(&matRotation, D3DX_PI / 2);
    D3DXVec3TransformCoord(&vecDirLeft, &m_vecDirection, &matRotation);
    vecPosition += vecDirLeft * fSpeed;
    SetCurPosition(vecPosition, sceneCur, true);

    m_dwMovementType = (DWORD)NPC_MOVEMENT_TYPE_RIGHT;

    return S_OK;
}


HRESULT KG3DRepresentNPC::TurnLef(float fAngel, int nUpdateDirFlag /* = true */)
{
    D3DXQUATERNION queRocation;
    D3DXMATRIX	   matRocation;

    m_fAngleHor += -fAngel; 
    D3DXQuaternionRotationYawPitchRoll(&queRocation, m_fAngleHor, 0.0f, 0.0f);
    D3DXMatrixRotationY(&matRocation, -fAngel);
    D3DXVec3TransformCoord(&m_vecDirection, &m_vecDirection, &matRocation);
    D3DXVec3Normalize(&m_vecDirection, &m_vecDirection);
    SetRotation(&queRocation);
    m_dwMovementType = (DWORD)NPC_MOVEMENT_TYPE_TURN_L;

    if (nUpdateDirFlag)
        m_vecPreDirecn = m_vecDirection;
    return S_OK;
}

HRESULT KG3DRepresentNPC::TurnRig(float fAngel, int nUpdateDirFlag /* = true */)
{
    D3DXQUATERNION queRocation;
    D3DXMATRIX	   matRocation;

    m_fAngleHor += fAngel; 
    D3DXQuaternionRotationYawPitchRoll(&queRocation, m_fAngleHor, 0.0f, 0.0f);
    D3DXMatrixRotationY(&matRocation, fAngel);
    D3DXVec3TransformCoord(&m_vecDirection, &m_vecDirection, &matRocation);
    D3DXVec3Normalize(&m_vecDirection, &m_vecDirection);
    SetRotation(&queRocation);
    m_dwMovementType = (DWORD)NPC_MOVEMENT_TYPE_TURN_R;

    if (nUpdateDirFlag)
        m_vecPreDirecn = m_vecDirection;

    return S_OK;
}

HRESULT KG3DRepresentNPC::Jump(float fUSpeed)
{
    if (m_vecCurPosition == m_vecPrePosition)
    {
        m_fUpSpeed = fUSpeed;
        m_vecCurPosition.y += 100.f + m_fLegLen;
        m_dwMovementType = (DWORD)NPC_MOVEMENT_TYPE_JUMP;
    }
    return S_OK;
}

HRESULT KG3DRepresentNPC::ProcessRButtonDn(WPARAM wParam, LPARAM lParam)
{
    //while (ShowCursor(FALSE) >= 0);
    return S_OK;
}

HRESULT KG3DRepresentNPC::ProcessRButtonUp(WPARAM wParam, LPARAM lParam)
{
    //while (ShowCursor(TRUE) < 0);
    //
    /*  KG3DScene* lpCurScene = g_cGraphicsTool.GetScene();
    KG_PROCESS_ERROR(lpCurScene);

    D3DXVECTOR3 vP;
    D3DXVECTOR3 vL;
    D3DXVECTOR3 vD;
    lpCurScene->m_lpCurOutputWnd->GetCamera().GetPosition(&vP);
    lpCurScene->m_lpCurOutputWnd->GetCamera().GetLookAtPosition(&vL);
    vD = vL - vP;
    vD.y = 0;
    D3DXVec3Normalize(&vD, &vD);
    SetDirection(vD);*/
    //D3DXVec3Normalize(&m_vecDirection, &m_vecDirection);

    /*    float fDot = D3DXVec3Dot(&vD, &m_vecDirection);
    if (fDot >  1.0f)
    fDot =  1.0f;
    if (fDot < -1.0f)
    fDot = -1.0f;
    float fAng = acosf(fDot);

    TurnLef(fAng)*/; 
//
//Exit0:
return S_OK;
}

static bool bOutput = false;



HRESULT KG3DRepresentNPC::Render(DWORD dwOption)
{
	D3DXMATRIX  matSaveTrans;
	D3DXMATRIX  matTransform;

	if(GetDistance() <= 25000000*4)
	{
		for (DWORD i = 0; i < GetModelCount(); i++)
		{
			KG3DModel* pModel = GetModel(i);
			if(NULL != pModel)
				pModel->Render(dwOption, NULL);
		}
	}

	if (g_bRenderAttackRange && m_bAttackRange)
	{
		D3DCOLORVALUE Attackcolor;
		Attackcolor.a = 0.2f;Attackcolor.r=0.8f;Attackcolor.g = 0.8f;Attackcolor.b = 0.0f;
		DrawVisibleSphere(Attackcolor,m_nAttackRange,10.0f);
	}
	if(g_bRenderAlertRange)
	{
		D3DCOLORVALUE Alertcolor;
		Alertcolor.a = 0.17f;Alertcolor.r=0.1f;Alertcolor.g = 0.8f;Alertcolor.b = 0.8f;
		DrawVisibleSphere(Alertcolor,m_nAlertRange,5.0f);	
	}

	BOOL bSelected = FALSE;
	if (g_bDesign)
	{
		KG3DScene *pScene = NULL;
		KG3DSceneSceneEditor *pEditor = NULL;

		pScene = g_cGraphicsTool.GetCurScene();
		KG_PROCESS_ERROR(pScene);

		pEditor = dynamic_cast<KG3DSceneSceneEditor*>(pScene);
		KG_PROCESS_ERROR(pEditor);

		bSelected = pEditor->GetSelectionTool().IsSelected(*this);
	}

	if(g_eEngineOption.bUseLogical && !g_bClient && (g_cEngineManager.GetRenderOption(RENDER_INFO) || bSelected))
	{
		D3DXVECTOR3 vecPos = m_vecCurPosition;
		vecPos.y += 200.0f;
		g_cGraphicsTool.DrawLine(&vecPos, &(vecPos + m_vNewDir * 200.0f), 0xffff0000, 0xff00ff00);
		g_cGraphicsTool.DrawLine(&vecPos, &(vecPos + m_vAxis * 200.0f), 0xffffffff, 0xffffffff);

		D3DVIEWPORT9 viewport;
		D3DXMATRIX matProj,matView,matWorld;
		g_pd3dDevice->GetViewport(&viewport);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
		g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
		g_pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);

		TCHAR strName[MAX_PATH] = _T("");
		sprintf(strName,"%s(%d,%d,%d)",m_scNPCName.c_str(),m_dwReliveID,m_dwRandomID,m_dwAISetID);

		D3DXVECTOR3 vCenter = m_Translation + D3DXVECTOR3(0,200,0);
		D3DXVECTOR3 vScreen;
		D3DXVec3Project(&vScreen,&vCenter,&viewport,&matProj,&matView,&matWorld);

		g_cGraphicsTool.DrawScreenText3D(vCenter,strName,0xFF0000FF);
		RenderFloorChecker(vScreen, g_cGraphicsTool.GetFont());
				
	}
	return S_OK;
Exit0:
	return E_FAIL;
}


void KG3DRepresentNPC::SetNpcMoveBits(enumNpcMoveState eState, BOOL bSet)
{
    if (bSet)
        m_eNPCState |= WORD(eState);
    else
        m_eNPCState &= ~WORD(eState);
}

float KG3DRepresentNPC::GetMoveSpeed()
{
    return m_fMoveSpeed;
}

HRESULT KG3DRepresentNPC::SetMoveSpeed(float fMoveSpeed)
{
    m_fMoveSpeed = fMoveSpeed;
	KG3DModel* pModel = NULL;
	if(SUCCEEDED(GetModel(pModel,0,FALSE)))
	{
		/*pModel->GetAnimationController(ANICTL_PRIMARY);
		if(pModel->m_pAnimationController)
			pModel->m_pAnimationController->m_fSpeed = fMoveSpeed;*/
	}
    return S_OK;
}

HRESULT KG3DRepresentNPC::GetName(LPTSTR pName, size_t BufferSize)
{
	_stprintf_s(pName, BufferSize,"%s",m_scNPCName.c_str());
    return S_OK;
}

HRESULT KG3DRepresentNPC::GetNPCTranslation(D3DXVECTOR3* pValue)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pValue);

    KG3DTransformation::GetTranslation(pValue);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DRepresentNPC::GetNPCRotation(D3DXQUATERNION* pValue)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pValue);

    KG3DTransformation::GetRotation(pValue);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DRepresentNPC::GetKind(INT *pnRetKind)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pnRetKind);

    *pnRetKind = m_nKind;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

//HRESULT KG3DRepresentNPC::GetAIKind(INT *pnRetAIKind)
//{
//    HRESULT hrResult = E_FAIL;
//
//    KGLOG_PROCESS_ERROR(pnRetAIKind);
//
//    *pnRetAIKind = m_nAIKind;
//
//    hrResult = S_OK;
//Exit0:
//    return hrResult;
//}

HRESULT KG3DRepresentNPC::GetLevel(INT *pnRetLevel)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pnRetLevel);

    *pnRetLevel = m_nLevel;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DRepresentNPC::GetTemplateID(DWORD *pdwRetID)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetID);

    *pdwRetID = m_dwTemplateID;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DRepresentNPC::GetRepresentID(DWORD *pdwRetID)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetID);

    *pdwRetID = m_dwRepresentID;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DRepresentNPC::GetAlertRange(float *pnAlertRange)
{
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pnAlertRange);

	*pnAlertRange = m_nAlertRange;

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DRepresentNPC::GetAttackRange(float *pnAttackRange)
{
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pnAttackRange);

	*pnAttackRange = m_nAttackRange;

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DRepresentNPC::Load( LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam )
{

	KG_PROCESS_ERROR(wParam != NULL);

	{
		IEKG3DRepresentNPC_BuildStruct* pBuildStruct = reinterpret_cast<IEKG3DRepresentNPC_BuildStruct*>(wParam);
		
		_ASSERTE(pBuildStruct->nSize == sizeof(IEKG3DRepresentNPC_BuildStruct));
		KG_PROCESS_ERROR(pBuildStruct->nSize == sizeof(IEKG3DRepresentNPC_BuildStruct));

		SetRepresentID(pBuildStruct->dwRepresentID);
		SetTemplateID(pBuildStruct->dwNPCTempleteID);
		SetNPCName(pBuildStruct->strNPCName);
		SetKind(pBuildStruct->nType);
		SetLevel(pBuildStruct->nLevel);
		//SetAIKind(pBuildStruct->nAIType);
		SetScaling(&D3DXVECTOR3(pBuildStruct->fScale,pBuildStruct->fScale,pBuildStruct->fScale));
		SetAlertRange(pBuildStruct->fAlertRange);
		SetAttackRange(pBuildStruct->fAttackRange);	
		SetReliveID(pBuildStruct->dwReliveID);
		SetRandomID(pBuildStruct->dwRandomID);
		SetAISetID (pBuildStruct->dwAISet);

		HRESULT hr = LoadFromFile(pBuildStruct->strMdlName,0,0);
		KGLOG_COM_PROCESS_ERROR(hr);

		if(pBuildStruct->strFaceMesh != NULL)
			SetModelFace(pBuildStruct->strFaceMesh,pBuildStruct->strFaceMtl,"","S_FACE");

		if(pBuildStruct->strAniName != NULL)
			PlayAnimation(pBuildStruct->strAniName);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

//HRESULT KG3DRepresentNPC::GetIsDisableAutoTurn( int* val )
//{
//	if (val)
//		*val = m_nDisableAutoTurn;
//	return S_OK;
//}

void KG3DRepresentNPC::SetNPCName( LPCTSTR pName )
{
	if(pName != NULL)
		m_scNPCName = pName;
}

HRESULT KG3DRepresentNPC::SaveToInformationBuffer( KG3DMemoryFile* pFile )
{
	ASSERT(0);
	return E_NOTIMPL;
}

HRESULT KG3DRepresentNPC::LoadFromInformationBuffer( const KG3DMemoryFile* pFile,DWORD dwOption )
{
	ASSERT(0);
	return E_NOTIMPL;
}

void KG3DRepresentNPC::SaveTheFloorHeight( KG3DScene& sceneCur, IELogicalSceneConnection* pLogicalConnection)
{
	if(! g_Switch.bFloorChecker)
		return;

	enum
	{
		em_bias = 10,
		em_logical_bias = 0,
	};
	m_vecCurFloor = m_vecCurPosition;
	FLOAT fHeight = 0;
	sceneCur.GetFloor(&(m_vecCurFloor + D3DXVECTOR3(0,em_bias,0)), &fHeight);
	m_vecCurFloor.y = fHeight;

	if (NULL != pLogicalConnection)
	{
		if(m_vecCurPosition.x > 0 && m_vecCurPosition.z > 0)
		{
			INT nX = 0, nY = 0, nZ = 0;
			KSF::SceneToLogicalDirect(m_vecCurPosition + D3DXVECTOR3(0, em_logical_bias, 0), &nX, &nY, &nZ);
			m_vLogicalFloor_CellOrder = D3DXVECTOR4((FLOAT)nX, (FLOAT)nY, (FLOAT)nZ, 0);
			D3DXVECTOR3 vTemp(m_vLogicalFloor_CellOrder.x
				,m_vLogicalFloor_CellOrder.y,m_vLogicalFloor_CellOrder.z);
			size_t uOrderOfCell = 0;
			FLOAT fLogicalZ = KSF::GetLogicalHeight(*pLogicalConnection, vTemp, &uOrderOfCell);

			if (fLogicalZ > -FLT_MAX)
			{
				m_vLogicalFloor_CellOrder.z = fLogicalZ;
				m_vLogicalFloor_CellOrder.w = (FLOAT)uOrderOfCell;
			}
		}
	}
}

void KG3DRepresentNPC::RenderFloorChecker( D3DXVECTOR3 &vScreen, LPD3DXFONT pFont )
{
	if(! g_Switch.bFloorChecker)
		return;

	RECT destRect1;
	SetRect(&destRect1, (int)vScreen.x, (int)vScreen.y, 0, 80);

	if(m_vecCurFloor != D3DXVECTOR3(0,0,0))//没有更新这个的就不用画Floor，这个是SceneRunner 的，有机会要把SceneRunner分出来
	{
		RECT rect2 = destRect1;
		rect2.top = destRect1.top -20;
		rect2.bottom = destRect1.bottom -20;

		TCHAR strFloor[MAX_PATH] = _T("");
		_stprintf_s(strFloor, _T("SceneFloor: %f,%f,%f"), m_vecCurFloor.x, m_vecCurFloor.y, m_vecCurFloor.z);

		pFont->DrawTextA(NULL, strFloor, -1, &rect2, DT_NOCLIP, 0xff0000ff);
	}

	if (m_vLogicalFloor_CellOrder != D3DXVECTOR4(0,0,0,0))
	{
		{
			RECT rect2 = destRect1;
			rect2.top = destRect1.top -40;
			rect2.bottom = destRect1.bottom -40;

			TCHAR strFloor[MAX_PATH] = _T("");
			_stprintf_s(strFloor, _T("LogicFloor: %f,%f,%f, %u"), m_vLogicalFloor_CellOrder.x, m_vLogicalFloor_CellOrder.y, m_vLogicalFloor_CellOrder.z, size_t(m_vLogicalFloor_CellOrder.w));

			pFont->DrawTextA(NULL, strFloor, -1, &rect2, DT_NOCLIP, 0xff0000ff);

		}

		//再画出Logical重新转换为Scene坐标之后的值
		{
			RECT rect3 = destRect1;
			rect3.top = destRect1.top -60;
			rect3.bottom = destRect1.bottom -60;


			D3DXVECTOR3 vLogicalFloorToScene(0,0,0);
			D3DXVECTOR3 vLogicalCur(m_vLogicalFloor_CellOrder.x
					, m_vLogicalFloor_CellOrder.y, m_vLogicalFloor_CellOrder.z);
			KSF::LogicalToSceneDirect(vLogicalCur, vLogicalFloorToScene);

			TCHAR strFloorScene[MAX_PATH] = _T("");
			_stprintf_s(strFloorScene, _T("LFlr2Scene: %f,%f,%f"), vLogicalFloorToScene.x, vLogicalFloorToScene.y, vLogicalFloorToScene.z);

			D3DCOLOR colorToUsed = 0xff0000ff;
			if (vLogicalFloorToScene.y < m_vecCurFloor.y)
			{
				colorToUsed = 0xffff0000;	//逻辑的坐标应该一直比Scene的坐标高才对
			}
			pFont->DrawTextA(NULL, strFloorScene, -1, &rect3, DT_NOCLIP, colorToUsed);
		}
	}
}
KG3DRepresentDoodad::KG3DRepresentDoodad()
{
    m_dwTemplateID = 0;
}
KG3DRepresentDoodad::~KG3DRepresentDoodad()
{

}

HRESULT KG3DRepresentDoodad::Copy(KG3DRepresentObject* pSrcObject)
{
    KG3DRepresentDoodad* pSrcDoodad = static_cast<KG3DRepresentDoodad*>(pSrcObject);

    m_scDoodadName = pSrcDoodad->m_scDoodadName;
    m_dwTemplateID = pSrcDoodad->m_dwTemplateID;
	m_dwReliveID   = pSrcDoodad->m_dwReliveID;
	m_bUseDefaultObstacle = pSrcDoodad->m_bUseDefaultObstacle;
	m_bGenerateObstacle = pSrcDoodad->m_bGenerateObstacle;

    KG3DRepresentObject::Copy(pSrcObject);
    return S_OK;
}

HRESULT KG3DRepresentDoodad::LoadFromFile(
    const char cszFileName[], unsigned uFileNameHash, unsigned uOption
    )
{
    SetModel(0, _T("Data\\public\\scenerunner\\scenerunner.Mesh"), "", _T("Data\\public\\scenerunner\\scenerunner.Ani"),TRUE);

    return S_OK;
}

HRESULT KG3DRepresentDoodad::GetTranslation(D3DXVECTOR3* pValue)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pValue);

    KG3DTransformation::GetTranslation(pValue);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT  KG3DRepresentDoodad::SetDoodadTranslation(D3DXVECTOR3* pValue)
{
    KG3DTransformation::SetTranslation(pValue);
    return S_OK;
}
HRESULT  KG3DRepresentDoodad::SetRotation(D3DXQUATERNION* pValue)
{
    KG3DTransformation::SetRotation(pValue);
    return S_OK;
}

HRESULT KG3DRepresentDoodad::GetRotation(D3DXQUATERNION* pValue)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pValue);

    KG3DTransformation::GetRotation(pValue);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DRepresentDoodad::GetTemplateID(DWORD *pdwRetID)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetID);

    *pdwRetID = m_dwTemplateID;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DRepresentDoodad::GetName( LPTSTR pName, size_t BufferSize )
{
	_stprintf_s(pName, BufferSize,"%s",m_scDoodadName.c_str());
	return S_OK;
}

void KG3DRepresentDoodad::SetDoodadName( const std::string &name )
{
	m_scDoodadName = name;
}

HRESULT KG3DRepresentDoodad::Load( LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam )
{
	IEKG3DRepresentDoodad_BuildStruct* pBuildStruct = reinterpret_cast<IEKG3DRepresentDoodad_BuildStruct*>(wParam);
	KG_PROCESS_ERROR(NULL != pBuildStruct);
	{
		_ASSERTE(pBuildStruct->nSize == sizeof(IEKG3DRepresentDoodad_BuildStruct));
		KG_PROCESS_ERROR(pBuildStruct->nSize == sizeof(IEKG3DRepresentDoodad_BuildStruct));

		HRESULT hr = SetModel(0,pBuildStruct->pMeshName,pBuildStruct->pMtlName,pBuildStruct->pAniName,TRUE);
		KG_COM_PROCESS_ERROR(hr);
		SetTemplateID(pBuildStruct->dwDoodadTempleteID);
		SetDoodadName(pBuildStruct->pDoodadName);
		SetReliveID(pBuildStruct->dwReliveID);
		SetUseDefaultObstacle(pBuildStruct->bUseDefaultObstacle);
		SetGenerateObstacle(pBuildStruct->bGenerateObstacle);
		
		return S_OK;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentDoodad::SaveToInformationBuffer( KG3DMemoryFile* pFile )
{
	ASSERT(0);
	return E_NOTIMPL;
}

HRESULT KG3DRepresentDoodad::LoadFromInformationBuffer( const KG3DMemoryFile* pFile ,DWORD dwOption)
{
	ASSERT(0);
	return E_NOTIMPL;
}
KG3DRepresentWayPoint::KG3DRepresentWayPoint()
{
    m_nWayPointType = 0;
}

KG3DRepresentWayPoint::~KG3DRepresentWayPoint()
{

}

HRESULT KG3DRepresentWayPoint::Copy(KG3DRepresentObject* pSrcObject)
{
    KG3DRepresentWayPoint* pSrcDoodad = static_cast<KG3DRepresentWayPoint*>(pSrcObject);

    m_scWayPointName = pSrcDoodad->m_scWayPointName;
    m_dwSetID = pSrcDoodad->m_dwSetID;
    KG3DRepresentObject::Copy(static_cast<KG3DRepresentObject*>(pSrcObject));
    return S_OK;
}

/*

HRESULT KG3DRepresentWayPoint::LoadFromFile( 
	const char cszFileName[], unsigned uFileNameHash, unsigned uOption
	)
{
	SetModel(0, _T("Data\\public\\Logical\\巡逻起点.Mesh"), _T("Data\\public\\Logical\\巡逻起点.Mtl"), "");

	return S_OK;
}
*/
HRESULT KG3DRepresentWayPoint::Load( LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam )
{
	if (wParam)
	{
		IEKG3DRepresentWayPoint_BuildStruct* pBuildStuct = reinterpret_cast<IEKG3DRepresentWayPoint_BuildStruct*>(wParam);
		
		_ASSERTE(pBuildStuct->nSize == sizeof(IEKG3DRepresentWayPoint_BuildStruct));
		KG_PROCESS_ERROR(pBuildStuct->nSize == sizeof(IEKG3DRepresentWayPoint_BuildStruct));

		SetSetID(pBuildStuct->dwSetID);
		SetWayPointName(pBuildStuct->tcsName);

		SetModel(0, _T("Data\\public\\Logical\\巡逻起点.Mesh"), _T("Data\\public\\Logical\\巡逻起点.Mtl"), "",FALSE);
		return S_OK;
	}
Exit0:
	return E_FAIL;	

}

HRESULT KG3DRepresentWayPoint::SetWayPointType(int nType)
{
    if (nType != m_nWayPointType)
    {
        switch (nType)
        {
        case 0:
            SetModel(0, _T("Data\\public\\Logical\\巡逻起点.Mesh"), _T("Data\\public\\Logical\\巡逻起点.Mtl"), "",TRUE);
            break;
        case 1:
            SetModel(0, _T("Data\\public\\Logical\\巡逻路径.Mesh"), _T("Data\\public\\Logical\\巡逻路径.Mtl"),"",TRUE);
            break;
        case 2:
            SetModel(0, _T("Data\\public\\Logical\\巡逻终点.Mesh"), _T("Data\\public\\Logical\\巡逻终点.Mtl"), "",TRUE);
            break;
        }
        m_nWayPointType = nType;
    }
    return S_OK;
}
HRESULT KG3DRepresentWayPoint::GetName(LPTSTR pName, size_t BufferSize)
{
    return KG3DRepresentObject::GetName(pName, BufferSize);
}

HRESULT KG3DRepresentWayPoint::GetTranslation(D3DXVECTOR3* pValue)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pValue);

    KG3DTransformation::GetTranslation(pValue);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DRepresentWayPoint::GetRotation(D3DXQUATERNION* pValue)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pValue);

    KG3DTransformation::GetRotation(pValue);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DRepresentWayPoint::SaveToInformationBuffer( KG3DMemoryFile* pFile )
{
	ASSERT(0);
	return E_NOTIMPL;
}

HRESULT KG3DRepresentWayPoint::LoadFromInformationBuffer( const KG3DMemoryFile* pFile ,DWORD dwOption)
{
	ASSERT(0);
	return E_NOTIMPL;
}
//HRESULT KG3DRepresentTrafficPoint::Render(DWORD dwOption)
//{
//	/*if(!g_bRenderSoundBall)
//		return S_OK;*/
//
//	//if(!m_lpSoundBallData)
//	//	return S_OK;
//
//	g_pd3dDevice->SetTexture(0,NULL);
//
//	D3DCOLORVALUE color;
//	color.a = 0.3f;color.r=0.8f;color.g = 0.2f;color.b = 1.0f;
//
//	GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,FALSE,D3DBLEND_INVSRCCOLOR,D3DBLEND_SRCCOLOR);
//	GraphicsStruct::RenderState Fog(D3DRS_FOGENABLE, FALSE);
//	GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_CCW);
//	if (m_dwType == 0)
//		g_cGraphicsTool.DrawSphereEx(&m_Matrix,color,g_dwBigRa,TRUE);
//	else
//		g_cGraphicsTool.DrawSphereEx(&m_Matrix,color,g_dwSmallRa,TRUE);
//	return S_OK;
//}

HRESULT KG3DRepresentTrafficPoint::Copy(KG3DRepresentObject* pSrcObject)
{
	KG3DRepresentTrafficPoint* pSrcDoodad = static_cast<KG3DRepresentTrafficPoint*>(pSrcObject);

	//m_scWayPointName = pSrcDoodad->m_scWayPointName;
	m_dwSetID = pSrcDoodad->m_dwSetID;
	m_TrafficPointKind = pSrcDoodad->m_TrafficPointKind;
	KG3DRepresentObject::Copy(static_cast<KG3DRepresentObject*>(pSrcObject));

	return S_OK;
}

HRESULT KG3DRepresentTrafficPoint::GetTranslation(D3DXVECTOR3* pValue)
{
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pValue);

	KG3DTransformation::GetTranslation(pValue);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

/*
HRESULT KG3DRepresentTrafficPoint::LoadFromFile( 
					LPCTSTR cszFileName, unsigned uFileNameHash, unsigned uOption)
{
	SetModel(0, _T("Data\\public\\Logical\\结点标志.Mesh"), _T("Data\\public\\Logical\\结点标志.Mtl"), "");
	return S_OK;
}*/


HRESULT KG3DRepresentTrafficPoint::SetKind(DWORD dwType)
{
	//if (dwType != m_dwType)
	//{
		switch (dwType)
		{
		case 0:
			SetModel(0, _T("Data\\public\\Logical\\结点标志.Mesh"), _T("Data\\public\\Logical\\结点标志.Mtl"), "",TRUE);
			break;
		case 1:
			SetModel(0, _T("Data\\public\\Logical\\路径标志.Mesh"), _T("Data\\public\\Logical\\路径标志.Mtl"),"",TRUE);
			break;
		}
		m_TrafficPointKind = dwType;
	//}
	return S_OK;
}

HRESULT KG3DRepresentTrafficPoint::Load( LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam )
{
	IEKG3DRepresentTrafficPoint_BuildStruct* pBuildStruct 
		= reinterpret_cast<IEKG3DRepresentTrafficPoint_BuildStruct*>(wParam);
	KG_PROCESS_ERROR(NULL != pBuildStruct);

	_ASSERTE(pBuildStruct->nSize == sizeof(IEKG3DRepresentTrafficPoint_BuildStruct));
	KG_PROCESS_ERROR(pBuildStruct->nSize == sizeof(IEKG3DRepresentTrafficPoint_BuildStruct));

	SetKind(pBuildStruct->dwType);
	SetSetID(pBuildStruct->dwSetID);

	return S_OK;
Exit0:
	return E_FAIL;

}

HRESULT KG3DRepresentTrafficPoint::SaveToInformationBuffer( KG3DMemoryFile* pFile )
{
	ASSERT(0);
	return E_NOTIMPL;
}

HRESULT KG3DRepresentTrafficPoint::LoadFromInformationBuffer( const KG3DMemoryFile* pFile ,DWORD dwOption)
{
	ASSERT(0);
	return E_NOTIMPL;
}
//HRESULT  KG3DRepresentTrafficPoint::ComputeDynamicBBox()
//{
//	return KG3DRepresentTrafficPoint::ComputeBBox();
//}
//
//BOOL KG3DRepresentTrafficPoint::IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
//{
//
//	float K = PointDistanceLine(&m_Translation,&Origin,&(Origin+Direction));
//	float fRadius = 0;
//	if (m_dwType == 0)
//		fRadius = g_dwBigRa;
//	else
//		fRadius = g_dwSmallRa;
//	if(K < fRadius) // * m_Scaling.y)
//	{
//		Inter = m_Translation;
//		return TRUE;
//	}
//	return FALSE;
//}
//HRESULT  KG3DRepresentTrafficPoint::ComputeBBox()
//{
//	D3DXVECTOR3 A,C,Center;
//	HRESULT hr = E_FAIL;
//	GetTranslation(&Center);
//	m_BBox.Clear();
//	if (m_dwType == 0)
//	{
//		A.x = Center.x - g_dwBigRa;
//		A.y = Center.y - g_dwBigRa;
//		A.z = Center.z - g_dwBigRa;
//		C.x = Center.x + g_dwBigRa;
//		C.y = Center.y + g_dwBigRa;
//		C.z = Center.z + g_dwBigRa;
//	}
//	else
//	{
//		A.x = Center.x - g_dwSmallRa;
//		A.y = Center.y - g_dwSmallRa;
//		A.z = Center.z - g_dwSmallRa;
//		C.x = Center.x + g_dwSmallRa;
//		C.y = Center.y + g_dwSmallRa;
//		C.z = Center.z + g_dwSmallRa;
//	}
//	m_BBox.AddPosition(A);
//	m_BBox.AddPosition(C);
//	hr = S_OK;
//Exit0:
//	return hr;
//}

KG3DRepresentRefreshPoint::KG3DRepresentRefreshPoint()
{
	
}

KG3DRepresentRefreshPoint::~KG3DRepresentRefreshPoint()
{

}

HRESULT KG3DRepresentRefreshPoint::Copy(KG3DRepresentObject* pSrcObject)
{
    KG3DRepresentRefreshPoint* pSrcPoint = static_cast<KG3DRepresentRefreshPoint*>(pSrcObject);

    m_scRefreshPointName = pSrcPoint->m_scRefreshPointName;
    m_dwSetID = pSrcPoint->m_dwSetID;
    m_dwRefreshPointType = pSrcPoint->m_dwRefreshPointType;
    KG3DRepresentObject::Copy(static_cast<KG3DRepresentObject*>(pSrcObject));
    return S_OK;
}

/*
HRESULT KG3DRepresentRefreshPoint::LoadFromFile( 
    LPCTSTR cszFileName, unsigned uFileNameHash, unsigned uOption
    )
{
    SetModel(0, _T("Data\\public\\Logical\\刷新点.Mesh"), "","");

    return S_OK;
}*/


HRESULT KG3DRepresentRefreshPoint::GetName(LPTSTR pName, size_t BufferSize)
{
    return KG3DRepresentObject::GetName(pName, BufferSize);
}

HRESULT KG3DRepresentRefreshPoint::GetTranslation(D3DXVECTOR3* pValue)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pValue);

    KG3DTransformation::GetTranslation(pValue);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DRepresentRefreshPoint::GetRotation(D3DXQUATERNION* pValue)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pValue);

    KG3DTransformation::GetRotation(pValue);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DRepresentRefreshPoint::Load( LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam )
{
	SetModel(0, _T("Data\\public\\Logical\\刷新点.Mesh"), "","",FALSE);
	KG_PROCESS_ERROR(wParam);
	{
		IEKG3DRepresentRefreshPoint_BuildStruct* pBuildStruct = reinterpret_cast<IEKG3DRepresentRefreshPoint_BuildStruct*>(wParam);
		KG_PROCESS_ERROR(NULL != pBuildStruct);

		_ASSERTE(pBuildStruct->nSize == sizeof(IEKG3DRepresentRefreshPoint_BuildStruct));
		KG_PROCESS_ERROR(pBuildStruct->nSize == sizeof(IEKG3DRepresentRefreshPoint_BuildStruct));

		SetRefreshPointType(pBuildStruct->dwType);
		SetSetID(pBuildStruct->dwID);
		SetRefreshPointName(pBuildStruct->tcsName);

		return S_OK;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentRefreshPoint::SaveToInformationBuffer( KG3DMemoryFile* pFile )
{
	ASSERT(0);
	return E_NOTIMPL;
}

KG3DRepresentPoly::KG3DRepresentPoly()
{
	m_lpPolyMesh = NULL;
	m_nLogicalSetID = -1;
}
KG3DRepresentPoly::~KG3DRepresentPoly()
{
	SAFE_RELEASE(m_lpPolyMesh);
}
HRESULT KG3DRepresentRefreshPoint::LoadFromInformationBuffer( const KG3DMemoryFile* pFile ,DWORD dwOption)
{
	ASSERT(0);
	return E_NOTIMPL;

}
HRESULT KG3DRepresentPoly::CreateFromScenePoly(KG3DPoly* pPoly)
{
    m_Poly = *pPoly;
    m_Poly.ComputeBBox();

    m_Translation = m_Poly.m_BBox.GetCenter();
	SetTranslation(&m_Translation);
    m_PreviousTranslation = m_Translation;//
    m_BBox = m_Poly.m_BBox;
    m_dwColor = 0x40FF8000;

	int nNumVertex = (int)m_Poly.m_vecVertex.size() + 2;
	SAFE_RELEASE(m_lpPolyMesh);
	if( FAILED( g_pd3dDevice->CreateVertexBuffer(nNumVertex*sizeof( VERTEXPOLY ),D3DUSAGE_DYNAMIC ,
													FVF_POLY,D3DPOOL_DEFAULT, &m_lpPolyMesh,NULL ) ) )
	{
		return E_FAIL;
	}
	
	VERTEXPOLY * pVer = NULL;
	if(FAILED( m_lpPolyMesh->Lock( 0, nNumVertex*sizeof( VERTEXPOLY ), (void**)&pVer, D3DLOCK_DISCARD )))
		return E_FAIL;
	D3DXVECTOR3 vCenter(0,0,0);
	for (int i = 0 ; i<(int)m_Poly.m_vecVertex.size();i++)
	{
		pVer[i+1].pos = m_Poly.m_vecVertex[i];// - m_Translation ;
		pVer[i+1].color = m_dwColor;
		vCenter += m_Poly.m_vecVertex[i]; 
	}
	pVer[0].pos = vCenter / ((float)nNumVertex - 2);
	pVer[0].color = m_dwColor;
	pVer[nNumVertex-1].pos = pVer[1].pos;
	pVer[nNumVertex-1].color = m_dwColor;
	m_lpPolyMesh->Unlock();
    return S_OK;
}

HRESULT KG3DRepresentPoly::ComputeBBox()
{
    m_BBox = m_Poly.m_BBox;
    return S_OK;
}

extern BOOL IsRayIntersectSphere(D3DXVECTOR3* pIntersection,
								 D3DXVECTOR3* pS_Center,float S_Radius,
								 D3DXVECTOR3* pLineA,D3DXVECTOR3* pDir);

BOOL KG3DRepresentPoly::IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	D3DXVECTOR3 vCenter = m_BBox.GetCenter();
	vCenter.y = m_BBox.B.y;

	return IsRayIntersectSphere(&Inter,&vCenter,200,&Origin,&Direction);
	//return m_BBox.IsRayIntersect3D(Inter,Origin,Direction);
}

BOOL KG3DRepresentPoly::RayIntersection( const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir 
										  , FLOAT* pfRet, BOOL bIntersectBoxOnly, KG3DIntersectionFilter* pFilter /*= NULL*/ )
{
	BOOL bHit = FALSE;
	FLOAT fDistance = FLT_MAX;

	if (NULL != pFilter)
	{
		HRESULT hr = pFilter->Accept(*this);
		if (FAILED(hr))
			return FALSE;
	}

	D3DXVECTOR3 vCenter = m_BBox.GetCenter();
	vCenter.y = m_BBox.B.y;

	D3DXVECTOR3 Inter;
	bHit = IsRayIntersectSphere(&Inter,&vCenter,200,(D3DXVECTOR3*)&vSrc,(D3DXVECTOR3*)&vNormalizedDir);

	if (bHit)
	{
		fDistance = D3DXVec3Length(&(vSrc - Inter));

		_ASSERTE(fDistance < FLT_MAX);
		if (NULL != pfRet)
		{
			*pfRet = fDistance;
		}
	}
	return bHit;
}


HRESULT KG3DRepresentPoly::DrawPoly()
{
	DWORD dwNumFace = (DWORD)m_Poly.m_vecVertex.size() /*- 2*/;

	D3DXVECTOR3 vNext(0,0,0);
	D3DXVECTOR3 vH(0,m_Poly.m_fHeight,0);

	for (size_t i=0;i<m_Poly.m_vecVertex.size();i++)
	{
		D3DXVECTOR3& vPos = m_Poly.m_vecVertex[i];
		if(i < m_Poly.m_vecVertex.size() -1)
			vNext = m_Poly.m_vecVertex[i+1];
		else
			vNext = m_Poly.m_vecVertex[0];
		g_cGraphicsTool.DrawRect(&(vPos+vH),&(vNext +vH),&vPos,m_dwColor,NULL);
	}
	{
		//GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_CCW);
		D3DXMATRIX matWorldSave,matTrans1,matTrans2;
		D3DXMatrixIdentity(&matTrans1);
		//D3DXMatrixTranslation(&matTrans1, m_Translation.x, m_Translation.y, m_Translation.z);
		D3DXMatrixTranslation(&matTrans2,0, m_Poly.m_fHeight, 0);
		g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matTrans1);
		g_pd3dDevice->SetStreamSource( 0, m_lpPolyMesh,0, sizeof( VERTEXPOLY) );
		g_pd3dDevice->SetFVF( FVF_POLY );
		g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLEFAN , 0 , dwNumFace);

		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matTrans2);
		g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLEFAN , 0 , dwNumFace);

		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
	}
	
	//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 vCenter = m_BBox.GetCenter();
	vCenter.y = m_BBox.B.y;
	D3DXMATRIX mat;
	D3DXMatrixTranslation(&mat,vCenter.x,vCenter.y,vCenter.z);

	D3DCOLORVALUE color;
	color.r = 1;
	color.g = 0.8F;
	color.b = 0;
	color.a = 0.5;

	{
		GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_ONE);
		GraphicsStruct::RenderState Light(D3DRS_LIGHTING,TRUE);
		g_cGraphicsTool.DrawSphereEx(&mat,color,200,FALSE);

		TCHAR szName[MAX_PATH];
		wsprintf(szName,"%s -- %s",m_scName.c_str(),m_strLogicalSetName.c_str());
		g_cGraphicsTool.DrawScreenText3D(vCenter,szName,0xFFA000FF);
	}

	return S_OK;
}
HRESULT KG3DRepresentPoly::Render(DWORD dwOption)
{
    D3DXVECTOR3 V = m_Translation - m_PreviousTranslation;
    if(D3DXVec3LengthSq(&V)>100)
    {
        OnSetTranslation(&V);
        m_PreviousTranslation = m_Translation;

    }


    if(g_bRenderRepresentPoly)
    {
		GraphicsStruct::RenderState light(D3DRS_LIGHTING,FALSE);
		GraphicsStruct::RenderState Fog(D3DRS_FOGENABLE, FALSE);
		GraphicsStruct::RenderStateAlpha Alpha(0,FALSE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_ONE);
		GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);
		g_pd3dDevice->SetTexture(0,NULL);
		g_pd3dDevice->SetTexture(1,NULL);

		D3DXVECTOR3 vStart(0,0,0);
        D3DXVECTOR3 vLast(0,0,0);
        D3DXVECTOR3 vH(0,m_Poly.m_fHeight,0);
		DWORD dwColor = 0xff0080ff;
        for (size_t i=0;i<m_Poly.m_vecVertex.size();i++)
        {
            D3DXVECTOR3& vPos = m_Poly.m_vecVertex[i];
		
            if(i==0)
            {
                vStart = vPos;
                vLast = vPos;
            }

            if(i >= m_Poly.m_vecVertex.size() -1)
            {
                g_cGraphicsTool.DrawLine(&vPos,&vStart,dwColor,dwColor);
                g_cGraphicsTool.DrawLine(&(vPos+vH),&(vStart+vH),dwColor,dwColor);
            }
			
			g_cGraphicsTool.DrawLine(&vPos,&vLast,dwColor,dwColor);
            g_cGraphicsTool.DrawLine(&(vPos+vH),&(vLast+vH),dwColor,dwColor);
            g_cGraphicsTool.DrawLine(&vPos,&(vPos+vH),dwColor,dwColor);

            vLast = vPos;
        }
		{
			GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

			DrawPoly();
		}

    }
    return S_OK;
}

HRESULT KG3DRepresentPoly::OnSetTranslation(D3DXVECTOR3* pValue)
{
	if(!m_lpPolyMesh)
		return E_FAIL;

	VERTEXPOLY * pVer = NULL;
	DWORD dwNumVertex = (DWORD)m_Poly.m_vecVertex.size();

	if(FAILED( m_lpPolyMesh->Lock( 0, dwNumVertex*sizeof( VERTEXPOLY ), (void**)&pVer, D3DLOCK_DISCARD )))
		return E_FAIL;
	
	D3DXVECTOR3 vCenter(0,0,0);
	for (DWORD i = 0 ; i<dwNumVertex;i++)
	{
		D3DXVECTOR3& vPos = m_Poly.m_vecVertex[i];
		vPos += *pValue;
		pVer[i+1].pos = vPos;
		vCenter += vPos; 
	}
	pVer[0].pos = vCenter / ((float)dwNumVertex);;
	pVer[dwNumVertex+1].pos = pVer[1].pos;
	m_lpPolyMesh->Unlock();
   
    m_Poly.ComputeBBox();
    ComputeBBox();

    return S_OK;
}

HRESULT KG3DRepresentPoly::RenderSelectMark(DWORD dwColor /* = 0xFFFF0000 */)
{
    g_cGraphicsTool.DrawAABBox(&m_BBox,dwColor);

	//GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_ONE);
	//DrawPoly();
    return S_OK;
}


HRESULT KG3DRepresentPoly::GetName(LPTSTR pName, size_t BufferSize)
{
	_stprintf_s(pName, BufferSize, "多边形%s",m_scName.c_str());
    return S_OK;
}

FLOAT KG3DRepresentPoly::GetVertexHeight()
{
    return m_Poly.m_fHeight;
}

INT   KG3DRepresentPoly::GetVertexCount()
{
    return (int)m_Poly.m_vecVertex.size();
}

D3DXVECTOR3 KG3DRepresentPoly::GetVertex(int nIndex)
{
    if((nIndex>=0)&&(nIndex<(int)m_Poly.m_vecVertex.size()))
    {
        return m_Poly.m_vecVertex[nIndex];
    }
    return D3DXVECTOR3(0,0,0);
}

HRESULT KG3DRepresentPoly::SetVertexHeight(FLOAT fHeight)
{
    m_Poly.m_fHeight = fHeight;
	ComputeBBox();
    return S_OK;
}

HRESULT KG3DRepresentPoly::SetVertexCount(INT nCount)
{
    m_Poly.m_vecVertex.resize(nCount);
    return S_OK;
}

HRESULT KG3DRepresentPoly::SetVertex(int nIndex,D3DXVECTOR3 vPos)
{
    if((nIndex>=0)&&(nIndex<(int)m_Poly.m_vecVertex.size()))
    {
        m_Poly.m_vecVertex[nIndex] = vPos;
    }
    return S_OK;
}

DWORD KG3DRepresentPoly::GetColor()
{
    return m_dwColor;
}

INT KG3DRepresentPoly::GetLogicalSetID()
{
    return m_nLogicalSetID;
}

HRESULT KG3DRepresentPoly::SetColor(DWORD dwColor)
{
    //m_dwColor = dwColor;
    return S_OK;
}

HRESULT KG3DRepresentPoly::SetLogicalSetID(INT nID)
{
    m_nLogicalSetID = nID;
    return S_OK;
}
LPCTSTR KG3DRepresentPoly::GetLogicalSetName()
{
	return m_strLogicalSetName.c_str();
}
HRESULT KG3DRepresentPoly::SetLogicalSetName(string strName)
{
	m_strLogicalSetName = strName;
	return S_OK;
}

BOOL KG3DRepresentPoly::IsPointIn(D3DXVECTOR3 vPos,BOOL bTestHeight)
{
	return m_Poly.IsPointIn(vPos,bTestHeight);
}

HRESULT KG3DRepresentPoly::SetModifyState(DWORD dwState)
{
	m_dwModifyState = dwState;
	return S_OK;
}

DWORD KG3DRepresentPoly::GetModifyState()
{
	return m_dwModifyState;
}

LPCTSTR KG3DRepresentPoly::GetScriptFileName()
{
	return m_szScriptFileName.c_str();
}
HRESULT KG3DRepresentPoly::SetScriptFileName(LPCTSTR pFileName)
{
	TCHAR path_buffer[256];
	size_t Pos = string::npos; 
	wsprintf(path_buffer,"%s",pFileName);
	_strlwr_s(path_buffer, 256);
	std::string ScriptName = path_buffer;	
	if(!(ScriptName == ""))
	{
		Pos = ScriptName.find("scripts\\");
		if(Pos != 0)
		{
			wsprintf(path_buffer,"多边形%s 绑定的脚本: %s不是正确的路径",m_scName.c_str(),ScriptName.c_str());
			MessageBox(NULL,path_buffer,"警告",MB_OK);
			if( Pos != string::npos)
			   ScriptName = ScriptName.substr(Pos);
		}
	}
	
	m_szScriptFileName = ScriptName;
	return S_OK;
}

LPCTSTR KG3DRepresentPoly::GetPolyName()
{
	return m_scName.c_str();
}

HRESULT KG3DRepresentPoly::SetPolyName(LPCTSTR pName)
{
	m_scName = pName;
	return S_OK;
}

HRESULT KG3DRepresentPoly::ComputeDynamicBBox()
{
	ComputeBBox();
	return S_OK;
}


HRESULT KG3DRepresentPoly::Copy(KG3DRepresentObject* pSrcObject)
{
	KG3DRepresentPoly* pSrcPoly = static_cast<KG3DRepresentPoly*>(pSrcObject);
	KG_PROCESS_ERROR(pSrcPoly);
	CreateFromScenePoly(&(pSrcPoly->m_Poly));
	this->SetPolyName(pSrcPoly->GetPolyName());
	this->SetLogicalSetName(pSrcPoly->GetLogicalSetName());
	this->SetScriptFileName(pSrcPoly->GetScriptFileName());
	this->SetLogicalSetID(pSrcPoly->m_nLogicalSetID);
	this->SetPolyUseage(pSrcPoly->GetPolyUseage());
	this->SetModifyState(pSrcPoly->GetModifyState());
	this->m_PreviousTranslation = pSrcPoly->m_PreviousTranslation;
	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DRepresentTerrainBlock::Render(DWORD dwOption)
{
	HRESULT hr = E_FAIL;
	//KG_PROCESS_ERROR(m_pTerrain);
	//KG_PROCESS_ERROR(m_pTerrain->m_lpCurrentTerrainEx);
	//m_pTerrain->m_lpCurrentTerrainEx->RenderTerrainCopyBrush();
	KG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);
	m_lpSceneOutDoorSpaceMgr->RenderTerrainCopyBrush();
	for (size_t i = 0; i < m_vecCopyObjects.size(); ++i)
	{
		//m_vecCopyObjects[i]->Render(0);
		//Scene.RemoveRenderEntity(SCENEENTITY_OBJECT, m_vecCopyObjects[i]);

		//Scene.RemoveRepresentObject(m_vecCopyObjects[i]);

		//if(bClearPasteBoard)
		//{
		//	SAFE_RELEASE(m_vecCopyObjects[i]);
		//}
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DRepresentTerrainBlock::SetTerrainPoint(KG3DSceneOutDoorSpaceMgr* pSceneOutDoorSpaceMgr)
{
	m_lpSceneOutDoorSpaceMgr = pSceneOutDoorSpaceMgr;
	return S_OK;
}

HRESULT KG3DRepresentTerrainBlock::ComputeBBox()
{
	D3DXVECTOR2 A,C;
	D3DXMATRIX matSave,mat,matT,matR,matS,matInvese;
	AABBOX bbox;
	D3DXVECTOR3 BosPos[8];
	D3DXVECTOR3 vecPos;

	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);
	//KG_PROCESS_ERROR(m_pTerrain->m_lpCurrentTerrainEx);

	m_BBox.Clear();
	GetTranslation(&vecPos);
	D3DXMatrixTranslation(&matT,vecPos.x,vecPos.y,vecPos.z);
	D3DXMatrixRotationY(&matR,m_lpSceneOutDoorSpaceMgr->m_fTerrainCopyRotation);
	D3DXMatrixScaling(&matS,m_lpSceneOutDoorSpaceMgr->m_vTerrainCopyScale.x,
		m_lpSceneOutDoorSpaceMgr->m_vTerrainCopyScale.y,
		m_lpSceneOutDoorSpaceMgr->m_vTerrainCopyScale.z);
	//mat = matS * matR * matT ;
	//D3DXMatrixInverse(&matInvese,NULL,&mat);

	float cell = 100;

	A.x = 0 -  m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyWidth * cell*0.5F;
	A.y = 0 -  m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyHeight * cell*0.5F;
	C.x = 0 +  m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyWidth * cell*0.5F;
	C.y = 0 +  m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyHeight * cell*0.5F;

	bbox.AddPosition(D3DXVECTOR3(A.x,0,A.y));
	bbox.AddPosition(D3DXVECTOR3(C.x,0,C.y));
	bbox.TransformCoord(BosPos,&m_Matrix);
	m_BBox.AddPosition(BosPos,8);
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DRepresentTerrainBlock::RenderSelectMark(DWORD dwColor)
{
	D3DXVECTOR2 A,C;
	AABBOX bbox;
	D3DXVECTOR3 BoxPos[8];
	D3DXVECTOR3 vecPos;

	ComputeBBox();
	g_cGraphicsTool.DrawAABBox(&m_BBox, 0xff808080);
	float cell = 100;
	A.x = 0 -  m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyWidth * cell*0.5F;
	A.y = 0 -  m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyHeight * cell*0.5F;
	C.x = 0 +  m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyWidth * cell*0.5F;
	C.y = 0 +  m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyHeight * cell*0.5F;

	bbox.AddPosition(D3DXVECTOR3(A.x,0,A.y));
	bbox.AddPosition(D3DXVECTOR3(C.x,0,C.y));
	bbox.TransformCoord(BoxPos,&m_Matrix);
	g_cGraphicsTool.DrawOBBox(BoxPos,dwColor);
	return S_OK;
}

HRESULT KG3DRepresentTerrainBlock::ComputeDynamicBBox()
{
	HRESULT hr = KG3DRepresentTerrainBlock::ComputeBBox();
	return hr;
}

BOOL KG3DRepresentTerrainBlock::IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	D3DXVECTOR3 vInter;
	if (m_BBox.IsRayIntersect3D(vInter,Origin,Direction,NULL,NULL))
	{
		Inter = vInter;
		return TRUE;
	}
	return FALSE;
}

void KG3DRepresentTerrainBlock::UpdateTransformation()
{
	D3DXVECTOR3 vec3;
	D3DXQUATERNION quaRotion;
	D3DXMATRIX matNow;
	KG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);

	D3DXMatrixTransformation(&m_Matrix,
		NULL,
		NULL,
		NULL,
		&m_RotationCenter,
		&m_Rotation,
		&m_Translation);	//和原来不一样的是，Scaling不起作用
    ASSERT_FLOAT_IS_VALIED(m_Matrix._11);

	m_lpSceneOutDoorSpaceMgr->m_matCopyBrush = m_Matrix;

	_ASSERTE(m_vecCopyObjects.size() == m_vecTrans.size());

	for (size_t i = 0;i < m_vecCopyObjects.size();i++)
	{				
		m_vecTrans[i].GetRotation(&quaRotion);
		quaRotion = quaRotion * m_Rotation;
		m_vecCopyObjects[i]->SetRotation(&quaRotion);

		m_vecTrans[i].GetRotationCenter(&vec3);
		m_vecCopyObjects[i]->SetRotationCenter(&vec3);

		D3DXMatrixMultiply(&matNow,&(m_vecTrans[i].m_Matrix), &m_Matrix);
		D3DXVec3TransformCoord(&vec3,&D3DXVECTOR3(0,0,0),&matNow);
		m_vecCopyObjects[i]->SetTranslation(&vec3);
	}
Exit0:
	return;
}

/*
void KG3DRepresentTerrainBlock::SetObjectList(vector<KG3DRepresentObject*>* pObjectList,vector<KG3DTransformation>* pPos)
{
//m_pObjectList = pObjectList;
m_pvecPos = pPos; 
}*/



HRESULT KG3DRepresentTerrainBlock::SaveBlockToFile(const char cszFileName[])
{
	BlockFileHead Head;	
	FILE*  fpFile     = NULL;
	DWORD* pOffset = NULL;
	KG3DTransformation Pos;
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);
	//KG_PROCESS_ERROR(m_pTerrain->m_lpCurrentTerrainEx);
	//KG_PROCESS_ERROR(m_pObjectList);
	//KG_PROCESS_ERROR(m_pvecPos);
	_ASSERTE(m_vecCopyObjects.size() == m_vecTrans.size());
	fpFile = ::fopen(cszFileName, "wb");
	KG_PROCESS_ERROR(fpFile);
	Head.m_fTerrainCopyHeightMax = m_lpSceneOutDoorSpaceMgr->m_fTerrainCopyHeightMax;
	Head.m_fTerrainCopyHeightMin = m_lpSceneOutDoorSpaceMgr->m_fTerrainCopyHeightMin;
	Head.m_fTerrainCopyHeightAve = m_lpSceneOutDoorSpaceMgr->m_fTerrainCopyHeightAve;
	Head.m_nTerrainCopyHeight = m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyHeight;
	Head.m_nTerrainCopyWidth = m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyWidth;
	Head.m_TerrainCopyTextureInfo = m_lpSceneOutDoorSpaceMgr->m_TerrainCopyTextureInfo;
	Head.m_nDetailTextureNum = m_lpSceneOutDoorSpaceMgr->m_nNumTerrainCopyTexture;
	Head.m_nSize = m_vecCopyObjects.size();
    Head.m_vecCopyPos = m_lpSceneOutDoorSpaceMgr->m_vecCopyPos;
	pOffset = new DWORD[Head.m_nSize];
	::fwrite(&Head,sizeof(BlockFileHead),1, fpFile);
	::fwrite(pOffset,sizeof(DWORD),Head.m_nSize,fpFile);
	::fwrite(m_lpSceneOutDoorSpaceMgr->m_lpTerrainCopyHeightData,
		sizeof(float),Head.m_nTerrainCopyHeight * Head.m_nTerrainCopyWidth,fpFile);
	::fwrite(m_lpSceneOutDoorSpaceMgr->m_lpTerrainCopyBlendData,
		sizeof(BYTE),Head.m_nTerrainCopyHeight * Head.m_nTerrainCopyWidth,fpFile);
	for (int i = 0;i < Head.m_nDetailTextureNum;i++)
	{
		::fwrite(m_lpSceneOutDoorSpaceMgr->m_ppTerrainCopyTexturePath[i],sizeof(char),MAX_PATH,fpFile);
	}
	for (size_t i = 0;i < m_vecCopyObjects.size();i++)
	{
		Pos = *((KG3DTransformation*)(m_vecCopyObjects[i]));
		*((KG3DTransformation*)(m_vecCopyObjects[i])) = m_vecTrans[i];
		pOffset[i] = ftell(fpFile);
		SaveObject(fpFile,m_vecCopyObjects[i]);
		*((KG3DTransformation*)(m_vecCopyObjects[i])) = Pos;
	}
	::fseek(fpFile,0,SEEK_SET);
	::fwrite(&Head,sizeof(BlockFileHead),1, fpFile);
	::fwrite(pOffset,sizeof(DWORD),Head.m_nSize,fpFile);
	hr = S_OK;

Exit0 :
	if (fpFile)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	SAFE_DELETE_ARRAY(pOffset);
	return hr;
}


HRESULT KG3DRepresentTerrainBlock::SaveObject(FILE* pFile,KG3DRepresentObject* pObject)
{
	HRESULT hr = E_FAIL;
	size_t SizeWrite = 0;
	EntitySave_ObjectBaseInfo vObjectBaseInfo;
	KG_PROCESS_ERROR(pObject);
	KG_PROCESS_ERROR(pFile);
	DWORD ModelCnt = pObject->GetModelCount(); 
	DWORD Type = pObject->GetType();
	KG3DModel* pModel = NULL;
	DWORD dwVersion = 0;
	ZeroMemory(&vObjectBaseInfo,sizeof(EntitySave_ObjectBaseInfo));
	vObjectBaseInfo.dwType = pObject->GetType();
	vObjectBaseInfo.dwModelCount = pObject->GetModelCount();
	vObjectBaseInfo.vScaling = pObject->m_Scaling;
	vObjectBaseInfo.vScalingCenter = pObject->m_ScalingCenter;
	vObjectBaseInfo.vScalingRotation = pObject->m_ScalingRotation;
	vObjectBaseInfo.vRotation = pObject->m_Rotation;
	vObjectBaseInfo.vRotaionCenter = pObject->m_RotationCenter;
	vObjectBaseInfo.vTranslation = pObject->m_Translation;

	SizeWrite = fwrite(&dwVersion, 1, sizeof(DWORD), pFile);
	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

	SizeWrite = fwrite(&vObjectBaseInfo, 1, sizeof(EntitySave_ObjectBaseInfo), pFile);
	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(EntitySave_ObjectBaseInfo));
	//////////////////////////////////////////////////////////////////////////
	switch(Type)
	{
	case REPRESENTOBJECT_DEFAULT:
		{
			char MeshName[MAX_PATH];
			char MtlName [MAX_PATH];
			char AniName [MAX_PATH];

			for (DWORD i = 0; i < ModelCnt; i++)
			{
				DWORD dwVersion = 0;
				EntitySave_ModelInfoBase vModelInfoBase;
				//////////////////////////////////////////////////////////////////////////
				ZeroMemory(&vModelInfoBase,sizeof(EntitySave_ModelInfoBase));
				vModelInfoBase.nLightmapTextureIndex = -1;

				hr = pObject->GetModelFileName(i, MeshName, MtlName, AniName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, MeshName, vModelInfoBase.MeshName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, MtlName, vModelInfoBase.MtlName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);	

				hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, AniName, vModelInfoBase.AniName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				hr = pObject->GetModel(pModel, i,FALSE);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				vModelInfoBase.dwModelType = pModel->GetType();

				if (vModelInfoBase.dwModelType == MESHTYPE_SFX)
				{
					KG3DSFX *psfx = (KG3DSFX*)pModel;
					vModelInfoBase.fSFXScaling = psfx->GetScaleSize();
				}
				else
					vModelInfoBase.fSFXScaling = 1.0F;
				//////////////////////////////////////////////////////////////////////////
				SizeWrite = fwrite(&dwVersion, 1, sizeof(DWORD), pFile);
				KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

				SizeWrite = fwrite(&vModelInfoBase, 1, sizeof(EntitySave_ModelInfoBase), pFile);
				KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(EntitySave_ModelInfoBase));
				//////////////////////////////////////////////////////////////////////////
				if (ModelCnt > 1)
				{
					DWORD dwVersion = 0;
					EntitySave_ModelTranslation vModelTranlation;
					//////////////////////////////////////////////////////////////////////////
					ZeroMemory(&vModelTranlation,sizeof(EntitySave_ModelTranslation));
					vModelTranlation.vScaling = pModel->m_Scaling;
					vModelTranlation.vScalingCenter = pModel->m_ScalingCenter;
					vModelTranlation.vScalingRotation = pModel->m_ScalingRotation;
					vModelTranlation.vRotation = pModel->m_Rotation;
					vModelTranlation.vRotaionCenter = pModel->m_RotationCenter;
					vModelTranlation.vTranslation = pModel->m_Translation;
					//////////////////////////////////////////////////////////////////////////
					SizeWrite = fwrite(&dwVersion, 1, sizeof(DWORD), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

					SizeWrite = fwrite(&vModelTranlation, 1, sizeof(EntitySave_ModelTranslation), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(EntitySave_ModelTranslation));
				}
			}
		}
		break;
	case REPRESENTOBJECT_SET:
		{
			char ModelSetName[256];

			DWORD dwVersion = 0;
			EntitySave_ObjectSetInfo vObjectSetInfo;
			//////////////////////////////////////////////////////////////////////////
			ZeroMemory(&vObjectSetInfo,sizeof(EntitySave_ObjectSetInfo));
			KG3DRepresentObjectSet* pSet = static_cast<KG3DRepresentObjectSet*>(pObject);
			KGLOG_PROCESS_ERROR_CONSOLE(pSet);

			hr = pSet->GetModelSetIniFileName(ModelSetName);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
			hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, ModelSetName,vObjectSetInfo.ModelSetName);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			vObjectSetInfo.nCurModel = pSet->GetCurrentModel();
			KG3DModel* pModel = NULL;
			hr = pSet->GetModel(pModel, 0,FALSE);

			vObjectSetInfo.nLightmapTextureIndex = -1;
			SizeWrite = fwrite(&dwVersion, 1, sizeof(DWORD), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

			SizeWrite = fwrite(&vObjectSetInfo, 1, sizeof(EntitySave_ObjectSetInfo), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(EntitySave_ObjectSetInfo));
		}
		break;	
	case REPRESENTOBJECT_VISIBLESET:
		{
			char MeshName[MAX_PATH];
			char MtlName [MAX_PATH];
			char AniName [MAX_PATH];

			DWORD dwVersion = 0;
			EntitySave_VisibleSetInfo vVisibleSetInfo;
			//////////////////////////////////////////////////////////////////////////
			ZeroMemory(&vVisibleSetInfo,sizeof(EntitySave_VisibleSetInfo));
			KG3DRepresentObjectVisibleEntitySet* pSet = static_cast<KG3DRepresentObjectVisibleEntitySet*>(pObject);
			KGLOG_PROCESS_ERROR_CONSOLE(pSet);

			hr = pSet->GetModelFileName(0,MeshName,MtlName,AniName);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, MeshName, vVisibleSetInfo.EdgeMeshName);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			vVisibleSetInfo.dwDepth = pSet->m_dwSetDepth;
			vVisibleSetInfo.dwInSideLayer = pSet->m_dwInsideLayerID;
			vVisibleSetInfo.dwOutSideLayer = pSet->m_dwOutsideLayerID;
			//////////////////////////////////////////////////////////////////////////
			SizeWrite = fwrite(&dwVersion, 1, sizeof(DWORD), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

			SizeWrite = fwrite(&vVisibleSetInfo, 1, sizeof(EntitySave_VisibleSetInfo), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(EntitySave_VisibleSetInfo));
		}
		break;
	case REPRESENTOBJECT_SOUNDBALL:
		{
			char FileName[MAX_PATH];

			KG3DRepresentObjectSoundBall* pSound = static_cast<KG3DRepresentObjectSoundBall*>(pObject);
			KGLOG_PROCESS_ERROR(pSound);

			g_pFileFinder->GetRelativePath(g_Def_WorkDirectory,pSound->m_szSoundBallDataFileName.c_str(),FileName);
			//////////////////////////////////////////////////////////////////////////
			SizeWrite = fwrite(&dwVersion, 1, sizeof(DWORD), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

			SizeWrite = fwrite(FileName, 1,MAX_PATH*sizeof(char), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == MAX_PATH * sizeof(char));
		}
		break;
	case REPRESENTOBJECT_ANTIPORTAL:
		{
			char MeshName[MAX_PATH];
			char MtlName [MAX_PATH];
			char AniName [MAX_PATH];


			for (DWORD i = 0; i < ModelCnt; i++)
			{
				DWORD dwVersion = 0;
				EntitySave_ModelInfoBase vModelInfoBase;
				//////////////////////////////////////////////////////////////////////////
				ZeroMemory(&vModelInfoBase,sizeof(EntitySave_ModelInfoBase));
				vModelInfoBase.nLightmapTextureIndex = -1;

				hr = pObject->GetModelFileName(i, MeshName, MtlName, AniName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, MeshName, vModelInfoBase.MeshName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, MtlName, vModelInfoBase.MtlName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);	

				hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, AniName, vModelInfoBase.AniName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				hr = pObject->GetModel(pModel, i,FALSE);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				vModelInfoBase.dwModelType = pModel->GetType();

				if (vModelInfoBase.dwModelType == MESHTYPE_SFX)
				{
					KG3DSFX *psfx = (KG3DSFX*)pModel;
					vModelInfoBase.fSFXScaling = psfx->GetScaleSize();
				}
				else
					vModelInfoBase.fSFXScaling = 1.0F;
				//////////////////////////////////////////////////////////////////////////
				SizeWrite = fwrite(&dwVersion, 1, sizeof(DWORD), pFile);
				KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

				SizeWrite = fwrite(&vModelInfoBase, 1, sizeof(EntitySave_ModelInfoBase), pFile);
				KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(EntitySave_ModelInfoBase));
				//////////////////////////////////////////////////////////////////////////
				if (ModelCnt > 1)
				{
					DWORD dwVersion = 0;
					EntitySave_ModelTranslation vModelTranlation;
					//////////////////////////////////////////////////////////////////////////
					ZeroMemory(&vModelTranlation,sizeof(EntitySave_ModelTranslation));
					vModelTranlation.vScaling = pModel->m_Scaling;
					vModelTranlation.vScalingCenter = pModel->m_ScalingCenter;
					vModelTranlation.vScalingRotation = pModel->m_ScalingRotation;
					vModelTranlation.vRotation = pModel->m_Rotation;
					vModelTranlation.vRotaionCenter = pModel->m_RotationCenter;
					vModelTranlation.vTranslation = pModel->m_Translation;
					//////////////////////////////////////////////////////////////////////////
					SizeWrite = fwrite(&dwVersion, 1, sizeof(DWORD), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

					SizeWrite = fwrite(&vModelTranlation, 1, sizeof(EntitySave_ModelTranslation), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(EntitySave_ModelTranslation));
				}
			}

			//////////////////////////////////////////////////////////////////////////
			KG3DRepresentObjectAntiPortal* pAntiPortal = static_cast<KG3DRepresentObjectAntiPortal*>(pObject);

			SizeWrite = fwrite(&dwVersion, 1, sizeof(DWORD), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

			SizeWrite = fwrite(&pAntiPortal->m_dwAntiPortalType, 1, sizeof(DWORD), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

			SizeWrite = fwrite(&pAntiPortal->m_fWidthBottom, 1, sizeof(float), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(float));
			SizeWrite = fwrite(&pAntiPortal->m_fWidthTop, 1, sizeof(float), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(float));
			SizeWrite = fwrite(&pAntiPortal->m_fHeight, 1, sizeof(float), pFile);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(float));
		}
		break;
	default:
		break;
	}
	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DRepresentTerrainBlock::ReadObject(IFile* pFile, KG3DRepresentObject** ppObject)
{
	HRESULT hr = E_FAIL;

	size_t SizeRead = 0;
	KG3DModel* pModel = NULL;

	DWORD dwVersion = 0;
	EntitySave_ObjectBaseInfo vObjectBaseInfo;
	if (!pFile)
		return E_FAIL;
	if (!ppObject)
		return E_FAIL;
	ZeroMemory(&vObjectBaseInfo,sizeof(EntitySave_ObjectBaseInfo));
	//////////////////////////////////////////////////////////////////////////

	SizeRead = pFile->Read(&dwVersion, sizeof(DWORD));
	KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));

	SizeRead = pFile->Read(&vObjectBaseInfo, sizeof(EntitySave_ObjectBaseInfo));
	KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(EntitySave_ObjectBaseInfo));
	//////////////////////////////////////////////////////////////////////////
	KG3DRepresentObject* pObject = NULL;
	KGLOG_COM_PROCESS_ERROR(g_GetObjectTable().NewOneResourse(vObjectBaseInfo.dwType,(IKG3DResourceBase**)(&pObject)));
	*ppObject = pObject;

	//pObject->SetScene(pEditor);
	//pObject->Advise(this);

	pObject->SetScaling(&vObjectBaseInfo.vScaling);
	pObject->SetScalingCenter(&vObjectBaseInfo.vScalingCenter);
	pObject->SetScalingRotation(&vObjectBaseInfo.vScalingRotation);
	pObject->SetRotation(&vObjectBaseInfo.vRotation);
	pObject->SetRotationCenter(&vObjectBaseInfo.vRotaionCenter);
	pObject->SetTranslation(&vObjectBaseInfo.vTranslation);

	//////////////////////////////////////////////////////////////////////////
	switch(vObjectBaseInfo.dwType)
	{
	case REPRESENTOBJECT_DEFAULT:
		{
			for (DWORD i = 0; i < vObjectBaseInfo.dwModelCount; i++)
			{
				DWORD dwVersion = 0;
				EntitySave_ModelInfoBase vModelInfoBase;
				//////////////////////////////////////////////////////////////////////////
				ZeroMemory(&vModelInfoBase,sizeof(EntitySave_ModelInfoBase));
				vModelInfoBase.nLightmapTextureIndex = -1;
				//////////////////////////////////////////////////////////////////////////
				SizeRead = pFile->Read(&dwVersion, sizeof(DWORD));
				KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));

				SizeRead = pFile->Read(&vModelInfoBase, sizeof(EntitySave_ModelInfoBase));
				KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(EntitySave_ModelInfoBase));
				//////////////////////////////////////////////////////////////////////////
				hr = pObject->SetModel(i, vModelInfoBase.MeshName, vModelInfoBase.MtlName, vModelInfoBase.AniName,TRUE);
				if(FAILED(hr))
				{
					KGLogPrintf(KGLOG_ERR,"Load Model Failed %s", vModelInfoBase.MeshName);
				}
				KGLOG_COM_PROCESS_ERROR(hr);

				hr = pObject->GetModel(pModel, i,FALSE);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);


				if (vModelInfoBase.dwModelType == MESHTYPE_SFX)
				{
					KG3DSFX *psfx = (KG3DSFX*)pModel;
					psfx->SetScaleSize(vModelInfoBase.fSFXScaling);
				}
				if (pModel->GetType() == MESHTYPE_SPEEDTREE)
				{
					KG3DModelSpeedTree* pSpdTree = static_cast<KG3DModelSpeedTree*>(pModel);
					pSpdTree->SetLeafScaling(&vObjectBaseInfo.vScaling);
				}
				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				if (vObjectBaseInfo.dwModelCount > 1)
				{
					DWORD dwVersion = 0;
					EntitySave_ModelTranslation vModelTranlation;
					//////////////////////////////////////////////////////////////////////////
					ZeroMemory(&vModelTranlation,sizeof(EntitySave_ModelTranslation));

					SizeRead = pFile->Read(&dwVersion, sizeof(DWORD));
					KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));

					SizeRead = pFile->Read(&vModelTranlation, sizeof(EntitySave_ModelTranslation));
					KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(EntitySave_ModelTranslation));

					//////////////////////////////////////////////////////////////////////////
					pModel->SetScaling(&vModelTranlation.vScaling);
					pModel->SetScalingCenter(&vModelTranlation.vScalingCenter);
					pModel->SetScalingRotation(&vModelTranlation.vScalingRotation);
					pModel->SetRotation(&vModelTranlation.vRotation);
					pModel->SetRotationCenter(&vModelTranlation.vRotaionCenter);
					pModel->SetTranslation(&vModelTranlation.vTranslation);
				}
			}
		}
		break;
	case REPRESENTOBJECT_SET:
		{
			DWORD dwVersion = 0;
			EntitySave_ObjectSetInfo vObjectSetInfo;
			//////////////////////////////////////////////////////////////////////////
			ZeroMemory(&vObjectSetInfo,sizeof(EntitySave_ObjectSetInfo));

			SizeRead = pFile->Read(&dwVersion, sizeof(DWORD));
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));

			SizeRead = pFile->Read(&vObjectSetInfo, sizeof(EntitySave_ObjectSetInfo));
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(EntitySave_ObjectSetInfo));

			//////////////////////////////////////////////////////////////////////////
			KG3DRepresentObjectSet* pSet = static_cast<KG3DRepresentObjectSet*>(pObject);
			KGLOG_PROCESS_ERROR_CONSOLE(pSet);

			hr = pSet->LoadFromIniFile(vObjectSetInfo.ModelSetName, 0);
			if(FAILED(hr))
			{
				KGLogPrintf(KGLOG_ERR,"Load ModelSet Failed %s", vObjectSetInfo.ModelSetName);
			}
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = pSet->SetCurrentModel(vObjectSetInfo.nCurModel,TRUE);

			KG3DModel* pModel = NULL;
			hr = pSet->GetModel(pModel, 0,FALSE);
			KG_COM_PROCESS_ERROR(hr);

			if(pModel)
			{
				if (pModel->GetType() == MESHTYPE_SPEEDTREE)
				{
					KG3DModelSpeedTree *pSpdTree = static_cast<KG3DModelSpeedTree*>(pModel);
					pSpdTree->SetLeafScaling(&vObjectBaseInfo.vScaling);
				}
			}
			//////////////////////////////////////////////////////////////////////////

		}
		break;	
	case REPRESENTOBJECT_VISIBLESET:
		{
			DWORD dwVersion = 0;
			EntitySave_VisibleSetInfo vVisibleSetInfo;
			//////////////////////////////////////////////////////////////////////////
			ZeroMemory(&vVisibleSetInfo,sizeof(EntitySave_VisibleSetInfo));

			SizeRead = pFile->Read(&dwVersion, sizeof(DWORD));
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));

			SizeRead = pFile->Read(&vVisibleSetInfo, sizeof(EntitySave_VisibleSetInfo));
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(EntitySave_VisibleSetInfo));
			//////////////////////////////////////////////////////////////////////////
			KG3DRepresentObjectVisibleEntitySet* pSet = static_cast<KG3DRepresentObjectVisibleEntitySet*>(pObject);
			KGLOG_PROCESS_ERROR_CONSOLE(pSet);

			hr = pSet->LoadEdgeModel(vVisibleSetInfo.EdgeMeshName);
			if(FAILED(hr))
			{
				KGLogPrintf(KGLOG_ERR,"Load EdgeModel Failed %s", vVisibleSetInfo.EdgeMeshName);
			}
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			pSet->m_dwSetDepth = vVisibleSetInfo.dwDepth;
			pSet->m_dwInsideLayerID = vVisibleSetInfo.dwInSideLayer;
			pSet->m_dwOutsideLayerID = vVisibleSetInfo.dwOutSideLayer;
			//////////////////////////////////////////////////////////////////////////

		}
	case REPRESENTOBJECT_SOUNDBALL:
		{
			char RetFileName[MAX_PATH];

			KG3DRepresentObjectSoundBall* pSound = static_cast<KG3DRepresentObjectSoundBall*>(pObject);
			KGLOG_PROCESS_ERROR(pSound);

			SizeRead = pFile->Read(&dwVersion, sizeof(DWORD));
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));

			SizeRead = pFile->Read(RetFileName, sizeof(char) * MAX_PATH);
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == MAX_PATH * sizeof(char));

			pSound->LoadFromSoundBallDataFile(RetFileName);
		}
		break;
	case REPRESENTOBJECT_ANTIPORTAL:
		{
			for (DWORD i = 0; i < vObjectBaseInfo.dwModelCount; i++)
			{
				DWORD dwVersion = 0;
				EntitySave_ModelInfoBase vModelInfoBase;
				//////////////////////////////////////////////////////////////////////////
				ZeroMemory(&vModelInfoBase,sizeof(EntitySave_ModelInfoBase));
				vModelInfoBase.nLightmapTextureIndex = -1;
				//////////////////////////////////////////////////////////////////////////
				SizeRead = pFile->Read(&dwVersion, sizeof(DWORD));
				KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));

				SizeRead = pFile->Read(&vModelInfoBase, sizeof(EntitySave_ModelInfoBase));
				KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(EntitySave_ModelInfoBase));
				//////////////////////////////////////////////////////////////////////////

				hr = pObject->SetModel(i, vModelInfoBase.MeshName, vModelInfoBase.MtlName, vModelInfoBase.AniName,TRUE);
				if(FAILED(hr))
				{
					KGLogPrintf(KGLOG_ERR,"Load Model Failed %s", vModelInfoBase.MeshName);
				}
				KGLOG_COM_PROCESS_ERROR(hr);

				hr = pObject->GetModel(pModel, i,FALSE);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				if (vModelInfoBase.dwModelType == MESHTYPE_SFX)
				{
					KG3DSFX *psfx = (KG3DSFX*)pModel;
					psfx->SetScaleSize(vModelInfoBase.fSFXScaling);
				}
				if (pModel->GetType() == MESHTYPE_SPEEDTREE)
				{
					KG3DModelSpeedTree* pSpdTree = static_cast<KG3DModelSpeedTree*>(pModel);
					pSpdTree->SetLeafScaling(&vObjectBaseInfo.vScaling);
				}
				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				if (vObjectBaseInfo.dwModelCount > 1)
				{
					DWORD dwVersion = 0;
					EntitySave_ModelTranslation vModelTranlation;
					//////////////////////////////////////////////////////////////////////////
					ZeroMemory(&vModelTranlation,sizeof(EntitySave_ModelTranslation));

					SizeRead = pFile->Read(&dwVersion, sizeof(DWORD));
					KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));

					SizeRead = pFile->Read(&vModelTranlation, sizeof(EntitySave_ModelTranslation));
					KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(EntitySave_ModelTranslation));

					//////////////////////////////////////////////////////////////////////////
					pModel->SetScaling(&vModelTranlation.vScaling);
					pModel->SetScalingCenter(&vModelTranlation.vScalingCenter);
					pModel->SetScalingRotation(&vModelTranlation.vScalingRotation);
					pModel->SetRotation(&vModelTranlation.vRotation);
					pModel->SetRotationCenter(&vModelTranlation.vRotaionCenter);
					pModel->SetTranslation(&vModelTranlation.vTranslation);
				}
			}
			//////////////////////////////////////////////////////////////////////////
			KG3DRepresentObjectAntiPortal* pAntiPortal = static_cast<KG3DRepresentObjectAntiPortal*>(pObject);

			SizeRead = pFile->Read(&dwVersion, sizeof(DWORD));
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));

			SizeRead = pFile->Read(&pAntiPortal->m_dwAntiPortalType, sizeof(DWORD));
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));

			SizeRead = pFile->Read(&pAntiPortal->m_fWidthBottom, sizeof(float));
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(float));

			SizeRead = pFile->Read(&pAntiPortal->m_fWidthTop, sizeof(float));
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(float));

			SizeRead = pFile->Read(&pAntiPortal->m_fHeight, sizeof(float));
			KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(float));
		}
		break;
	default:
		break;
	}
	return S_OK;
Exit0:
	SAFE_DELETE(*ppObject);
	return E_FAIL;
}
HRESULT KG3DRepresentTerrainBlock::ReadBlockFromFile(const char cszFileName[])
{
	int nRetCode = false;
	BlockFileHead Header;
	IFile* pFile = NULL;
	DWORD* pOffset = NULL;
	KG3DRepresentObject* pObject = NULL;
	map<BYTE,std::string> mapDetailTex;

	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);
	//KG_PROCESS_ERROR(m_pTerrain->m_lpCurrentTerrainEx);


	SAFE_DELETE_ARRAY(m_lpSceneOutDoorSpaceMgr->m_pRealTextureIndex);
	for (int i = 0; i < m_lpSceneOutDoorSpaceMgr->m_nNumTerrainCopyTexture; i++)
	{
		SAFE_DELETE_ARRAY(m_lpSceneOutDoorSpaceMgr->m_ppTerrainCopyTexturePath[i]);
	}
	SAFE_DELETE_ARRAY(m_lpSceneOutDoorSpaceMgr->m_ppTerrainCopyTexturePath);

	pFile = g_OpenFile(cszFileName);
	KGLOG_PROCESS_ERROR(pFile);

	nRetCode = (int)pFile->Read(&Header, sizeof(BlockFileHead));
	KGLOG_PROCESS_ERROR(nRetCode == (int)sizeof(BlockFileHead));


	m_lpSceneOutDoorSpaceMgr->m_fTerrainCopyHeightMax = Header.m_fTerrainCopyHeightMax;
	m_lpSceneOutDoorSpaceMgr->m_fTerrainCopyHeightMin = Header.m_fTerrainCopyHeightMin;
	m_lpSceneOutDoorSpaceMgr->m_fTerrainCopyHeightAve = Header.m_fTerrainCopyHeightAve;
	m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyHeight = Header.m_nTerrainCopyHeight;
	m_lpSceneOutDoorSpaceMgr->m_nTerrainCopyWidth = Header.m_nTerrainCopyWidth;
	m_lpSceneOutDoorSpaceMgr->m_TerrainCopyTextureInfo = Header.m_TerrainCopyTextureInfo;
	m_lpSceneOutDoorSpaceMgr->m_nNumTerrainCopyTexture = Header.m_nDetailTextureNum;
	m_lpSceneOutDoorSpaceMgr->m_vecCopyPos = Header.m_vecCopyPos; // by Likan

	pOffset = new DWORD[Header.m_nSize];
	KGLOG_PROCESS_ERROR(pOffset);

	nRetCode = (int)pFile->Read(pOffset, sizeof(DWORD) * (unsigned long)(Header.m_nSize));
	KGLOG_PROCESS_ERROR(nRetCode == (int)(sizeof(DWORD) * Header.m_nSize));

	SAFE_DELETE_ARRAY(m_lpSceneOutDoorSpaceMgr->m_lpTerrainCopyHeightData);
	SAFE_DELETE_ARRAY(m_lpSceneOutDoorSpaceMgr->m_lpTerrainCopyBlendData);

	m_lpSceneOutDoorSpaceMgr->m_lpTerrainCopyHeightData = 
		new float[Header.m_nTerrainCopyWidth * Header.m_nTerrainCopyHeight];
	KGLOG_PROCESS_ERROR( m_lpSceneOutDoorSpaceMgr->m_lpTerrainCopyHeightData);

	nRetCode = (int)pFile->Read(m_lpSceneOutDoorSpaceMgr->m_lpTerrainCopyHeightData,
		sizeof(float) * Header.m_nTerrainCopyWidth * Header.m_nTerrainCopyHeight);
	KGLOG_PROCESS_ERROR(nRetCode == (int)(sizeof(float) * Header.m_nTerrainCopyWidth * Header.m_nTerrainCopyHeight));

	m_lpSceneOutDoorSpaceMgr->m_lpTerrainCopyBlendData = 
		new BYTE[Header.m_nTerrainCopyWidth * Header.m_nTerrainCopyHeight];
	KGLOG_PROCESS_ERROR( m_lpSceneOutDoorSpaceMgr->m_lpTerrainCopyBlendData);

	nRetCode = (int)pFile->Read(m_lpSceneOutDoorSpaceMgr->m_lpTerrainCopyBlendData,
		sizeof(BYTE) * Header.m_nTerrainCopyWidth * Header.m_nTerrainCopyHeight);
	KGLOG_PROCESS_ERROR(nRetCode == (int)(sizeof(BYTE) * Header.m_nTerrainCopyWidth * Header.m_nTerrainCopyHeight));

	m_lpSceneOutDoorSpaceMgr->m_pRealTextureIndex = new BYTE[256];
	ZeroMemory(m_lpSceneOutDoorSpaceMgr->m_pRealTextureIndex,sizeof(BYTE) * 256);
	m_lpSceneOutDoorSpaceMgr->m_ppTerrainCopyTexturePath = new char*[256];
	ZeroMemory(m_lpSceneOutDoorSpaceMgr->m_ppTerrainCopyTexturePath,sizeof(char*) * 256);

	for (int i = 0;i < Header.m_nDetailTextureNum;i++)
	{
		m_lpSceneOutDoorSpaceMgr->m_ppTerrainCopyTexturePath[i] = new char[MAX_PATH];
		nRetCode = (int)pFile->Read(m_lpSceneOutDoorSpaceMgr->m_ppTerrainCopyTexturePath[i],
			sizeof(char) * MAX_PATH);
		KGLOG_PROCESS_ERROR(nRetCode == (int)(sizeof(char) * MAX_PATH));
	}
	if (SUCCEEDED(m_lpSceneOutDoorSpaceMgr->GetAllDetailTextureFromMaterialMgr(&mapDetailTex)))
	{		
		for (int i = 0;i < Header.m_nDetailTextureNum;i++)
		{
			map<BYTE,std::string>::iterator it = mapDetailTex.begin();
			while (it != mapDetailTex.end())
			{
				if (strcmp(m_lpSceneOutDoorSpaceMgr->m_ppTerrainCopyTexturePath[i],(it->second).c_str()) == 0)
				{
					break;
				}
				it++;
			}
			if (it != mapDetailTex.end())
			{
				m_lpSceneOutDoorSpaceMgr->m_pRealTextureIndex[i] = it->first;
			}
			else
			{
				BYTE byteTextureIndex = 0;
				m_lpSceneOutDoorSpaceMgr->AddDetailTextureToMaterialMgr(byteTextureIndex,m_lpSceneOutDoorSpaceMgr->m_ppTerrainCopyTexturePath[i]);
				m_lpSceneOutDoorSpaceMgr->m_pRealTextureIndex[i] = byteTextureIndex;
			}
		}
	}
	for (size_t i = 0;i < Header.m_nSize;i++)
	{
		nRetCode = pFile->Seek(pOffset[i], SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode != -1);

		hr = ReadObject(pFile, &pObject);
		KGLOG_COM_PROCESS_ERROR(hr);

		m_vecCopyObjects.push_back(pObject);
		m_vecTrans.push_back(static_cast<const KG3DTransformation&>(*pObject));
	}

	m_lpSceneOutDoorSpaceMgr->InitTerrainCopyBrush();

	KG_COM_RELEASE(pFile);
	SAFE_DELETE_ARRAY(pOffset);
	return S_OK;
Exit0:

	{
		for (size_t i = 0;i < m_vecCopyObjects.size();i++)
			SAFE_RELEASE(m_vecCopyObjects[i]);
		m_vecCopyObjects.clear();
		m_vecTrans.clear();
		//m_pvecPos = NULL;
	}

	KG_COM_RELEASE(pFile);
	SAFE_DELETE_ARRAY(pOffset);
	return E_FAIL;
}

KG3DRepresentTerrainBlock::KG3DRepresentTerrainBlock( void )
{
	//m_pObjectList = NULL;
	m_lpSceneOutDoorSpaceMgr = NULL;
	//m_pvecPos = NULL;

	KG_CUSTOM_HELPERS::TContainerRelease(m_vecCopyObjects);
}

VOID KG3DRepresentTerrainBlock::RemoveAllCopyObjectsInScene( KG3DSceneSceneEditor& Scene, BOOL bClearPasteBoard )
{
	for (size_t i = 0; i < m_vecCopyObjects.size(); ++i)
	{
		//Scene.RemoveRenderEntity(SCENEENTITY_OBJECT, m_vecCopyObjects[i]);

		Scene.RemoveRepresentObject(m_vecCopyObjects[i]);

		//if(bClearPasteBoard)
		//{
		//	SAFE_RELEASE(m_vecCopyObjects[i]);
		//}
	}
	if(bClearPasteBoard)
	{
		m_vecCopyObjects.clear();
		m_vecTrans.clear();
	}
}

VOID KG3DRepresentTerrainBlock::AddAllCopyObjectsToScene( KG3DSceneSceneEditor& Scene )
{
	for (size_t i = 0; i < m_vecCopyObjects.size(); ++i)
	{
		//Scene.AddRepresentObject(m_vecCopyObjects[i],FALSE,FALSE,FALSE);
		Scene.AddRepresentObject(m_vecCopyObjects[i], 0, NULL);
	}
}

KG3DRepresentTerrainBlock::~KG3DRepresentTerrainBlock()
{
	KG3DSceneSceneEditor* pScene = NULL;
	if (m_lpSceneOutDoorSpaceMgr)
		m_lpSceneOutDoorSpaceMgr->GetSceneEditor(&pScene);
	if (pScene)
	{
		for (size_t i = 0; i < m_vecCopyObjects.size(); ++i)
		{
			pScene->RemoveRepresentObject(m_vecCopyObjects[i]);
		}
	}
	m_vecCopyObjects.clear();
	m_vecTrans.clear();
}

HRESULT KG3DRepresentTerrainBlock::AddCopyObject( KG3DRepresentObject* p )
{
	if (NULL != p)
	{
		m_vecCopyObjects.push_back(p);

		m_vecTrans.push_back(static_cast<const KG3DTransformation&>(*p));

		return S_OK;
	}
	return E_FAIL;

}
//HRESULT  KG3DRepresentTerrainBlock::PasteTerrain(DWORD dwOption)
//{
//	if (m_pTerrain)
//		m_pTerrain->m_lpCurrentTerrainEx->PasteSelectedTerrain(D3DXVECTOR3(0,0,0),dwOption,0);
//	return S_OK;
//}

HRESULT KG3DRepresentNPC::FrameMove()
{
	DWORD dwGroundType = 0xffffffff;
	KG3DScene *pScene = NULL;
	KG3DSceneSceneEditor *pEditor = NULL;
	HRESULT hResult = KG3DRepresentObject::FrameMove();
	D3DXVECTOR3 vec2Pos(m_matWorld._41, m_matWorld._42, m_matWorld._43);

	pScene = g_cGraphicsTool.GetCurScene();
	KG_PROCESS_ERROR(pScene);

	pEditor = dynamic_cast<KG3DSceneSceneEditor*>(pScene);
	KG_PROCESS_ERROR(pEditor);

	dwGroundType = pEditor->GetGroundType(vec2Pos);
Exit0:
	return hResult;
}