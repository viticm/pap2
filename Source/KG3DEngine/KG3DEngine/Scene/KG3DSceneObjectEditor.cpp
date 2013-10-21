#include "StdAfx.h"
#include "kg3dsceneobjecteditor.h"
#include "kg3drepresentobjecttable.h"
#include "KG3DAnimationController.h"
#include "kg3dmodeltable.h"
#include "KG3DGraphicsTool.h"
#include "KG3DTextureTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR* ppSocketNames[] = { 
	"S_Face",
	"S_LH",
	"S_RH",
	"S_LP",
	"S_RP",
	"S_Spine2",
	"S_Hat"
};

KG3DSceneObjectEditor::KG3DSceneObjectEditor(void)
{
	m_lpRepresentObject = NULL;
	m_pObjectTemplate   = NULL;
	m_pLogicTable       = NULL;
	m_lpd3dFont			= NULL;
	m_dwAinPlayType     = ENU_ANIMATIONPLAY_CIRCLE;
	m_fAniPlaySpeed     = 1.0f;

	m_dwPrevTime = 0;
	m_dwCurrTime = 0;
	m_fMoveSpeed = 800.0f;
	m_fRotinRate = 90;
	m_fRotationY = 0;

	m_vNpcDirection = D3DXVECTOR3(0, 0, -1);
	m_vNpcPosition  = D3DXVECTOR3(0, 0, 0);

	m_pointPrevMoust.x = 0;
	m_pointPrevMoust.y = 0;
	m_fDelayCount	 = 1;

	m_pTexture = NULL;

}

KG3DSceneObjectEditor::~KG3DSceneObjectEditor(void)
{
	SAFE_RELEASE(m_lpRepresentObject);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_lpd3dFont);
}

HRESULT KG3DSceneObjectEditor::Init()
{
    KG3DSceneEditorBase::Init();

	/*///现在Fog变成了KG3DEnvEffFog，不会默认加载，需要用KG3DEnvironment来添加
	///这些参数在添加了Fog之后再用RequestModification来控制
	m_Fog.m_bFogEnable = FALSE;
	m_Fog.m_fFogStartValue = 1000;
	m_Fog.m_fFogEndValue = 8000;
	m_Fog.m_fFogDensity = 0.5F;
	KG3DScene::SetFogColor( 0xFFFFFFFF );*/

	//
	// To create a Windows friendly font using only a point size, an 
	// application must calculate the logical height of the font.
	// 
	// This is because functions like CreateFont() and CreateFontIndirect() 
	// only use logical units to specify height.
	//
	// Here's the formula to find the height in logical pixels:
	//
	//             -( point_size * LOGPIXELSY )
	//    height = ----------------------------
	//                          72
	//

	HRESULT hr;
	HDC hDC;
	int nHeight;
	int nPointSize = 9;

	hDC = GetDC( NULL );

	nHeight = -( MulDiv( nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );

	ReleaseDC( NULL, hDC );

	// Create a font for statistics and help output
	hr = D3DXCreateFont( 
		g_pd3dDevice, nHeight, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_lpd3dFont 
	);

	g_cTextureTable.LoadResourceFromFile(TEXT("\\Data\\source\\maps_source\\Texture\\ST_公堂地板_001.tga"),
		0, TEXOP_RELOAD, (IKG3DResourceBase**)&m_pTexture
	);
    return hr;
}

HRESULT KG3DSceneObjectEditor::UnInit()
{
	SAFE_RELEASE(m_lpRepresentObject);
	SAFE_RELEASE(m_lpd3dFont);

	for (int i = 0; i < BindInfo::COUNT; i++)
	{
		if (m_BindInfo.m_Models[i])
		{
			RemoveRenderEntity(SCENEENTITY_MODEL, m_BindInfo.m_Models[i]);	
			SAFE_RELEASE(m_BindInfo.m_Models[i]);
		}
	}
	KG3DSceneEditorBase::UnInit();
	return S_OK;
}

HRESULT KG3DSceneObjectEditor::FrameMove()
{
	HRESULT hResult  = E_FAIL;

	g_cGraphicsTool.SetCurScene(this);

	m_dwCurrTime = timeGetTime();
	if (!m_dwPrevTime)
		m_dwPrevTime = m_dwCurrTime;

	float fDelay = static_cast<float>(m_dwCurrTime - m_dwPrevTime) / 1000.0f;

	//m_TimeControl.FrameMove(FALSE);

	POINT pointMoust;
	::GetCursorPos(&pointMoust);
	if (m_pointPrevMoust.x == 0 && m_pointPrevMoust.y ==0)
		m_pointPrevMoust = pointMoust;

	D3DXMATRIX matRotation;
	D3DXMatrixIdentity(&matRotation);
	if (m_pointPrevMoust.x != pointMoust.x && GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		float fRot = static_cast<float>(pointMoust.x - m_pointPrevMoust.x) / 100;
		D3DXMatrixRotationY(&matRotation,  fRot);
		m_fRotationY += fRot;

	}
	m_pointPrevMoust = pointMoust;
	if (GetAsyncKeyState('W') & 0x8000)
		m_vNpcPosition += m_vNpcDirection * fDelay * m_fMoveSpeed;
	if (GetAsyncKeyState('S') & 0x8000)
		m_vNpcPosition-= m_vNpcDirection * fDelay * m_fMoveSpeed;
	if (GetAsyncKeyState('A') & 0x8000)
	{
		D3DXMatrixRotationY(&matRotation,  -fDelay * D3DXToRadian(m_fRotinRate));
		m_fRotationY -= fDelay * D3DXToRadian(m_fRotinRate);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		D3DXMatrixRotationY(&matRotation, fDelay * D3DXToRadian(m_fRotinRate));
		m_fRotationY += fDelay * D3DXToRadian(m_fRotinRate);
	}
	if (GetAsyncKeyState(VK_ADD) & 0x8000)
		m_fMoveSpeed++;
	if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000)
		m_fMoveSpeed--;


	D3DXQUATERNION quatRotation;
		 
	D3DXVec3TransformNormal(&m_vNpcDirection, &m_vNpcDirection, &matRotation);
	D3DXVec3Normalize(&m_vNpcDirection, &m_vNpcDirection);
	D3DXQuaternionRotationYawPitchRoll(&quatRotation, m_fRotationY, 0, 0);

	KG3DSceneEntityList::iterator iter = m_listRenderEntity.begin();
	while (iter != m_listRenderEntity.end())
	{
		if (iter->GetType() == SCENEENTITY_OBJECT)
		{
			iter->SetTranslation(&m_vNpcPosition);
			iter->SetRotation(&quatRotation, NULL);
		}
		iter++;
	}

	for_each(
		m_listRenderEntity.begin(),
		m_listRenderEntity.end(), 
		mem_fun_ref(&KG3DSceneEntity::FrameMove)
	);

	if (m_SceneWndManager->GetCurOutputWnd())
	{
		KG3DTrackCamera* pTrackCam = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetTrackCameraInterface();
		if (NULL != pTrackCam)
		{
			pTrackCam->UpdateAsClient(m_vNpcPosition, m_vNpcDirection, m_fMoveSpeed, 0);
			m_SceneWndManager->GetCurOutputWnd()->GetCamera().FrameMove();
		}
	}

	m_dwPrevTime = m_dwCurrTime;

//Exit0:
	return hResult;
}

HRESULT KG3DSceneObjectEditor::RenderForEachWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
	 switch(pWnd->GetType())
	 {
	 case OUTPUTWND_OBJECTEDITOR_MAIN: 
		 RenderForMainWnd(pWnd);
	 	break;
	 default:
		 break;
	 }
	 return S_OK;
 }

HRESULT KG3DSceneObjectEditor::RenderForMainWnd(KG3DSceneOutputWnd* pWnd)
{
	RenderWindowBegin(pWnd,TRUE);

	g_cGraphicsTool.ApplyLight(*this);//SunLight是用于0的    

	D3DXMATRIX matWord;
	D3DXMatrixTranslation(&matWord, 0, 0, 0);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWord);
	//g_cGraphicsTool.DrawGroundGrid(100, 0xFFAAAAAA);

	D3DXVECTOR3 vA = D3DXVECTOR3(-10000, 0,  10000);
	D3DXVECTOR3 vB = D3DXVECTOR3( 10000, 0,  10000);
	D3DXVECTOR3 vD = D3DXVECTOR3(-10000, 0, -10000);
	//DWORD dwTexID = 0;
	//if (m_pTexture)
	//	dwTexID = m_pTexture->GetID();
	g_cGraphicsTool.DrawRect(&vA, &vB, &vD, 0xFFFFFFFF, m_pTexture->GetTexture(), 50, 50);

	//////////////////////////////////////////////////////////////////////////
	char  szRepresentID[24];
	char  szLogicID[24];
	char  szTextName[64];
	char  szTextTemp[64];

	*szRepresentID = '\0';
	*szTextName    = '\0';
	*szTextTemp    = '\0';
	*szLogicID     = '\0';

	float fScalg = 1.0f;
	int   bTranl = false;

	RECT rectText;

	int nRepresentID = 0;

	if (m_pObjectTemplate && m_pLogicTable)
	{
		itoa(m_nLogicID, szLogicID, 10);
		m_pLogicTable->GetInteger(szLogicID, "RepresentID", 0, &nRepresentID);
		itoa(nRepresentID, szRepresentID, 10);

		m_pObjectTemplate->GetFloat(szRepresentID, "ModelScale", fScalg, &fScalg);
		SetScalingg(&D3DXVECTOR3(fScalg, fScalg, fScalg));

		m_pObjectTemplate->GetInteger(szRepresentID, "IsTransparent", bTranl, &bTranl);
		if (bTranl)
			SetAlpha(0.5f);
		else
			SetAlpha(1.0f);

		SetRect(&rectText, 2, 0, 0, 0);
		float fRet = g_cGraphicsTool.GetTinyTime();
		if (fRet == 0)
			fRet = 1;
		m_fDelayCount += fRet / 1000;
		if (m_fDelayCount >= 1)
		{
			m_fFPS = 1000 / fRet;
			m_fDelayCount = 0;
		}
		sprintf_s(
			szTextName, "FPS : %.2f\nMoving Speed : %.2f", 
			m_fFPS, m_fMoveSpeed, szTextTemp
		);
		m_lpd3dFont->DrawText(
			NULL, szTextName, -1, &rectText, DT_NOCLIP, 0xFFFFFFFF
		);
	}

	/*D3DXMATRIX matTrans;
	D3DXMATRIX matRotnY;
	D3DXMATRIX matWord;
	D3DXMatrixTranslation(&matTrans, m_fMoveX, -1000, m_fMoveZ);
	D3DXMatrixRotationY(&matRotnY, m_fRotationY);
	matWord = matRotnY * matTrans;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWord);*/
	for_each(
		m_listRenderEntity.begin(),
		m_listRenderEntity.end(), 
		mem_fun_ref(&KG3DSceneEntity::Render)
	);

	RenderWindowEnd(pWnd);

	 return S_OK;
 }

HRESULT KG3DSceneObjectEditor::BandObjectTemplate(
	ITabFile* pObjectTemplate, ITabFile* pLogicTable
)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(pObjectTemplate);
	KGLOG_PROCESS_ERROR(pLogicTable);
	m_pObjectTemplate = pObjectTemplate;
	m_pLogicTable = pLogicTable;

	hResult = S_OK;
Exit0:
	return hResult;
}

 HRESULT KG3DSceneObjectEditor::OpenObject(int nLogicID)
 {
	HRESULT hResult  = E_FAIL;

	m_nLogicID = nLogicID;

	hResult = S_OK;
//Exit0:
	return hResult;
 }

HRESULT KG3DSceneObjectEditor::SetCurModel(const char pcszModelField[])
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	char szModelRelaPath[512];
	char szRepresentID[64];
	char szLogicID[24];
	int  nRepresentID = 0;

	*szModelRelaPath = '\0';
	*szRepresentID   = '\0';
	*szLogicID       = '\0';

	KG_PROCESS_ERROR(m_pObjectTemplate);
	KG_PROCESS_ERROR(pcszModelField);

	for (int i = 0; i < m_BindInfo.COUNT; i++)
	{
		if (m_BindInfo.m_Models[i])
		{
			UnBind(i);
			RemoveRenderEntity(SCENEENTITY_MODEL, m_BindInfo.m_Models[i]);
		}
	}

	if (m_lpRepresentObject)
	{
		RemoveRenderEntity(m_lpRepresentObject, SCENEENTITY_OBJECT); 
		SAFE_RELEASE(m_lpRepresentObject);
	}

	g_GetObjectTable().NewOneResourse(0, (IKG3DResourceBase**)(&m_lpRepresentObject));
	KGLOG_PROCESS_ERROR(m_lpRepresentObject);

	itoa(m_nLogicID, szLogicID, 10);
	m_pLogicTable->GetInteger(szLogicID, "RepresentID", 0, &nRepresentID);
	itoa(nRepresentID, szRepresentID, 10);
	m_pObjectTemplate->GetString(
		szRepresentID, pcszModelField, "", szModelRelaPath, sizeof(szModelRelaPath)
	);
	
	hRetCode = m_lpRepresentObject->SetModel(0, szModelRelaPath, "", "",TRUE);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hRetCode);

	hRetCode = AddRenderEntity(SCENEENTITY_OBJECT, m_lpRepresentObject); 
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hRetCode);

	hResult = S_OK;
Exit0:
	return hResult;
}

 HRESULT KG3DSceneObjectEditor::SetCurAnimt(const char pcszAnimtField[]) 
 {
	 HRESULT hResult  = E_FAIL;
	 HRESULT hRetCode = E_FAIL;

	 char szRepresentID[64];
	 char szLogicID[24];

	 *szRepresentID   = '\0';
	 *szLogicID       = '\0';

	 DWORD nModelCount  = 0;
	 DWORD nModelIndex  = 0;

	 KGLOG_PROCESS_ERROR(m_lpRepresentObject);
	 KG_PROCESS_ERROR(m_pObjectTemplate);
	 KG_PROCESS_ERROR(pcszAnimtField);


	 nModelCount = m_lpRepresentObject->GetModelCount();
	 for (nModelIndex = 0; nModelIndex < nModelCount; nModelIndex++)
	 {
		 KG3DModel* pModel = m_lpRepresentObject->GetModel(nModelIndex);
		 if (pModel != NULL)
		 {
			 pModel->PlayAnimation(m_dwAinPlayType, pcszAnimtField, m_fAniPlaySpeed, 0, NULL, NULL, ANICTL_PRIMARY);
		 }
	 }

	if (nModelCount != 0 && m_lpRepresentObject->GetModel(0))
	{
		KG3DAnimationController* pControaller = 
			(KG3DAnimationController*)m_lpRepresentObject->GetModel(0)->GetAnimationController(ANICTL_PRIMARY);
		if (pControaller)
		{
			KG3DClip* pClip =NULL;
			hRetCode = pControaller->GetCurAnimation((IEKG3DClip**)(&pClip));
			KGLOG_COM_PROCESS_ERROR(hRetCode);
			if (pClip)
			{
				pClip->GetFrameLength(&m_fCurAniFrameTime);
				KGLOG_COM_PROCESS_ERROR(hRetCode);
			}
		}
	}

	 KGLOG_COM_PROCESS_ERROR_CONSOLE(hRetCode);

	 hResult = S_OK;
Exit0:
	 return hResult;
 }

 INT KG3DSceneObjectEditor::GetCurAniFrameTime()
 {
	 return static_cast<INT>(m_fCurAniFrameTime);
 }

 HRESULT KG3DSceneObjectEditor::SetCurAniFrameTime(int nTime)
 {
	 HRESULT hResult  = E_FAIL;
	 HRESULT hRetCode = E_FAIL;
	 IEKG3DClip *pIEClip = NULL;
	 KG3DClip *pClip = NULL;
	 m_fCurAniFrameTime = static_cast<float>(nTime);

	 KG_PROCESS_ERROR(m_lpRepresentObject);

	 int nModelCount = static_cast<int>(m_lpRepresentObject->GetModelCount());
	 if (nModelCount && m_lpRepresentObject->GetModel(0) != NULL)
	 {
		 KG3DAnimationController* pControaller = 
			 (KG3DAnimationController*)m_lpRepresentObject->GetModel(0)->GetAnimationController(ANICTL_PRIMARY);
		 if (pControaller)
		 {
			 
             hRetCode = pControaller->GetCurAnimation(&pIEClip);
			 KGLOG_COM_PROCESS_ERROR(hRetCode);

             if (pIEClip)
			 {
                 pClip = reinterpret_cast<KG3DClip *>(pIEClip);
				/* AnimationItem clipInfo;
				 pAnimation->GetClipData(0, clipInfo);
				 clipInfo.piClip->SetFrameLength(m_fCurAniFrameTime);*/
				 pClip->SetFrameLength(m_fCurAniFrameTime);
				/* pAnimation->SetClipData(0, )*/
			 }
		 }
	 }

	 hResult = S_OK;
Exit0:
	 return hResult;
 }

 HRESULT KG3DSceneObjectEditor::SaveCurAniFrameTime()
 {
	 HRESULT hResult  = E_FAIL;
	 HRESULT hRetCode = E_FAIL;
    
	 IEKG3DClip *pIEClip = NULL;
	 KG3DClip *pClip = NULL;
	 KG_PROCESS_ERROR(m_lpRepresentObject);

	 int nModelCount = static_cast<int>(m_lpRepresentObject->GetModelCount());
	 if (nModelCount && m_lpRepresentObject->GetModel(0))
	 {
		 KG3DAnimationController* pControaller = 
			 (KG3DAnimationController*)m_lpRepresentObject->GetModel(0)->GetAnimationController(ANICTL_PRIMARY);
		 if (pControaller)
		 {
             hRetCode = pControaller->GetCurAnimation(&pIEClip);
             KGLOG_COM_PROCESS_ERROR(hRetCode);

             pClip = reinterpret_cast<KG3DClip *>(pIEClip);

			 if (pClip && pClip->m_scName != "")
			 {
				pClip->SaveToFile(pClip->m_scName.c_str(), 0);
			 }
		 }
	 }
	 else
	 {
		 KG_PROCESS_ERROR(FALSE);
	 }
	 hResult = S_OK;
Exit0:
	 return hResult;
 }

HRESULT KG3DSceneObjectEditor::SetScalingg(D3DXVECTOR3* pScalVector)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(m_lpRepresentObject);

	for (DWORD i = 0; i < m_lpRepresentObject->GetModelCount(); i++)
	{
		KG3DModel* pModel = m_lpRepresentObject->GetModel(i);
		if(NULL != pModel)
			pModel->SetScaling(pScalVector);
	}

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSceneObjectEditor::OpenObject(LPSTR pFileName)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

    if (m_lpRepresentObject)
    {
		RemoveRenderEntity(m_lpRepresentObject, SCENEENTITY_OBJECT); 
	    SAFE_RELEASE(m_lpRepresentObject);
    }

	g_GetObjectTable().NewOneResourse(0, (IKG3DResourceBase**)(&m_lpRepresentObject));
	KGLOG_PROCESS_ERROR(m_lpRepresentObject);

	hRetCode = m_lpRepresentObject->SetModel(0, pFileName, "", "",TRUE);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hRetCode);

	hRetCode = AddRenderEntity(SCENEENTITY_OBJECT, m_lpRepresentObject); 
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hRetCode);

	hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneObjectEditor::PlayAnimation(const char pcszAnimtFile[])
{
	HRESULT hResult  = E_FAIL;

	KG_PROCESS_ERROR(m_lpRepresentObject);
	KG_PROCESS_ERROR(pcszAnimtFile);

	for (DWORD i = 0; i < m_lpRepresentObject->GetModelCount(); i++)
	{
		KG3DModel* pModel = m_lpRepresentObject->GetModel(i);
		if(NULL != pModel)
			pModel->PlayAnimation(m_dwAinPlayType, pcszAnimtFile
					, m_fAniPlaySpeed, 0, NULL, NULL, ANICTL_PRIMARY);
	}
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSceneObjectEditor::PlayAnimation(DWORD dwType, float fSpeed)
{
	HRESULT hResult  = E_FAIL;

	m_dwAinPlayType = dwType;
	m_fAniPlaySpeed = fSpeed;

	hResult = S_OK;
//Exit0:
	return hResult;
}

HRESULT KG3DSceneObjectEditor::SetAnimationSpeed(float fSpeed)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(m_lpRepresentObject);
	
	m_fAniPlaySpeed = fSpeed;

	for (DWORD i = 0; i < m_lpRepresentObject->GetModelCount(); i++)
	{
		KG3DModel* pModel = m_lpRepresentObject->GetModel(i);
		if(NULL != pModel)
			pModel->GetAnimationController(ANICTL_PRIMARY)->SetSpeed(fSpeed);
	}

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSceneObjectEditor::SetAnimationType(DWORD dwType)
{
	HRESULT hResult  = E_FAIL;

	m_dwAinPlayType = dwType;

	hResult = S_OK;
//Exit0:
	return hResult;
}


HRESULT KG3DSceneObjectEditor::SetAlpha(float fAlpha)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(m_lpRepresentObject);

	for (DWORD i = 0; i < m_lpRepresentObject->GetModelCount(); i++)
	{
		KG3DModel* pModel = m_lpRepresentObject->GetModel(i);
		if(NULL != pModel)
			pModel->SetAlpha(fAlpha, TRUE);
	}

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSceneObjectEditor::PauseAnimation(BOOL bPause)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(m_lpRepresentObject);

	for (DWORD i = 0; i < m_lpRepresentObject->GetModelCount(); i++)
	{
		KG3DModel* pModel = m_lpRepresentObject->GetModel(i);
		if(NULL != pModel)
		{
			pModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(bPause);
		}
	}

	hResult = S_OK;
Exit0:
	return hResult;
}


KG3DSceneObjectEditor::BindInfo::~BindInfo()
{
	for (int i = 0; i < COUNT; i++)
	{
		SAFE_RELEASE(m_Models[i]);
	}
}

KG3DSceneObjectEditor::BindInfo::BindInfo()
{
	memset(m_Models, 0, sizeof(KG3DModel*) * COUNT);
}



HRESULT KG3DSceneObjectEditor::Bind(DWORD dwPartIndex, 
											  LPCSTR strFileName, 
											  BOOL bUnBindOrg /* = FALSE */)
{
	HRESULT hr = E_FAIL;
	
	KGLOG_PROCESS_ERROR(m_lpRepresentObject);
	KGLOG_PROCESS_ERROR(dwPartIndex < BindInfo::COUNT);
	
	if (bUnBindOrg)
	{
		UnBind(dwPartIndex);
	}
	
	hr = g_cModelTable.LoadResourceFromFile(strFileName, 
		0, 
		MLO_MULTI, 
		(IKG3DResourceBase**)&m_BindInfo.m_Models[dwPartIndex]);
	KG_COM_PROCESS_ERROR(hr);

	{
		AddRenderEntity(KG3DSceneEntity(SCENEENTITY_MODEL, m_BindInfo.m_Models[dwPartIndex]));
	}

	hr = m_lpRepresentObject->BindModel(ppSocketNames[dwPartIndex], m_BindInfo.m_Models[dwPartIndex]);
	KG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneObjectEditor::UnBind(DWORD dwPartIndex) 
{
	HRESULT hr = E_FAIL;
	
	KGLOG_PROCESS_ERROR(m_lpRepresentObject);
	KGLOG_PROCESS_ERROR(dwPartIndex < BindInfo::COUNT);
	
	KG_PROCESS_ERROR(m_BindInfo.m_Models[dwPartIndex]);

	RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_MODEL, m_BindInfo.m_Models[dwPartIndex]));
	
	m_BindInfo.m_Models[dwPartIndex]->UnBindFromOther();
	SAFE_RELEASE(m_BindInfo.m_Models[dwPartIndex]);
	
	hr = S_OK;
Exit0:
	return hr;
}

//(by dengzhihui Refactor 2006年11月15日
HRESULT KG3DSceneObjectEditor::GetRepresentObject(IEKG3DRepresentObject** pObj)
{
	_ASSERTE(pObj);

	if (pObj)
	{
		*pObj = m_lpRepresentObject;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneObjectEditor::GetCurAniFrameTime(float* fCurAniFrameTime)
{
	_ASSERTE(fCurAniFrameTime);

	if (fCurAniFrameTime)
	{
		*fCurAniFrameTime = m_fCurAniFrameTime;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneObjectEditor::SetCurAniFrameTime(float fCurAniFrameTime)
{
	m_fCurAniFrameTime = fCurAniFrameTime;
	return S_OK;
}

HRESULT KG3DSceneObjectEditor::GetBaseScene(IKG3DScene** ppScene)
{
	_ASSERTE(ppScene);

	if (ppScene)
	{
		*ppScene = static_cast<IKG3DScene*>(this);

		return S_OK;
	}
	return E_FAIL;
}
//)
