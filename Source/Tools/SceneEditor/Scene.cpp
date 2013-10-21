// Scene.cpp: implementation of the K3DScene class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
//#include "Scene.h"
//#include ".\scene.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

K3DScene::K3DScene()
{
	m_Ambient = 0xFF808080;
	m_dType = 0;
	ID = 0;

	//////////////////////////////////////////////////////////////////////////	
	ZeroMemory( &m_SunLight, sizeof(D3DLIGHT9) );
	m_SunLight.Type       = D3DLIGHT_DIRECTIONAL;

	m_SunLight.Diffuse.r  = 1.0f;
	m_SunLight.Diffuse.g  = 1.0f;
	m_SunLight.Diffuse.b  = 1.0f;

	m_SunLight.Ambient.r  = 0.40f;
	m_SunLight.Ambient.g  = 0.40f;
	m_SunLight.Ambient.b  = 0.40f;

	if(g_cGraphicsTool.m_bSunSpecular)
	{
		m_SunLight.Specular.r  = 1.00f;
		m_SunLight.Specular.g  = 1.00f;
		m_SunLight.Specular.b  = 1.00f;
	}
	else
	{
		m_SunLight.Specular.r  = 0.00f;
		m_SunLight.Specular.g  = 0.00f;
		m_SunLight.Specular.b  = 0.00f;
	}

	D3DXVECTOR3 vecDir;
	g_cGraphicsTool.GetSunDirection(vecDir);

	D3DXVec3Normalize( (D3DXVECTOR3*)&m_SunLight.Direction, &vecDir );
	m_SunLight.Range       = 50000.0f;

	g_pd3dDevice->SetLight( 0, &m_SunLight );
	g_pd3dDevice->LightEnable( 0, TRUE );
}

K3DScene::~K3DScene()
{

}

HRESULT K3DScene::Initialize(PVOID pSceneTable)
{
	m_TimeLast = timeGetTime()*1.0f;

	KSceneTable * pTable = (KSceneTable*) pSceneTable;
	return S_OK;
}

HRESULT K3DScene::InvalidateDeviceObjects(void)
{
	return E_NOTIMPL;
}

HRESULT K3DScene::RestoreDeviceObjects(void)
{
	return E_NOTIMPL;
}

HRESULT K3DScene::Render()
{
	m_Fog.ApplyFog();

	float NowTime = timeGetTime()*1.0f;
	int FPS = (int)(1000.0f/(NowTime - m_TimeLast));
	int x =0;
	g_cGraphicsTool.DrawLine(&D3DXVECTOR3(0,35,0),&D3DXVECTOR3(0,-35,0),0xffffffff,0xffff0000); 
	g_cGraphicsTool.DrawLine(&D3DXVECTOR3(35,0,0),&D3DXVECTOR3(-35,0,0),0xffffffff,0xffff0000); 
	g_cGraphicsTool.DrawLine(&D3DXVECTOR3(0,0,35),&D3DXVECTOR3(0,0,-35),0xffffffff,0xffff0000); 
	
	//g_cGraphicsTool.DrawGroundGrid();

	/*ENTITYLIST::iterator i = m_RenderEntityList.begin();
	while(i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		DrawRenderEntity(&Entity);
		i++;
	}*/

	for_each(m_RenderEntityList.begin(),m_RenderEntityList.end(),mem_fun_ref(KSceneEntity::RenderDirect));

	/*i = m_RenderEntityList.begin();
	while(i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		DrawRenderEntityShadow(&Entity);
		i++;
	} */

	/*for(DWORD i=1;i<=g_cMeshTable.m_dNumXMesh;i++)
	{
		LPMODEL pMesh = NULL;
		if (SUCCEEDED(g_cMeshTable.GetXMesh(&pMesh,i)))
		{
			pMesh->Render();
		}
	}*/
		
	GraphicsStruct::KCamera* pCamera = &g_GraphiceEngine.m_cCamera; 
			
	//TCHAR str[30];
	//wsprintf(str,"FPS %d",FPS);
	//g_cGraphicsTool.DrawText(100,200,1,1,0xFFFF00FF,20,"ËÎÌו",str,0);
	//SetWindowText(g_hBaseWnd,str);
	m_TimeLast = NowTime;
	return S_OK;
}

HRESULT K3DScene::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

HRESULT K3DScene::SetLighting()
{
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, m_Ambient );

	g_pd3dDevice->SetLight(0,&m_SunLight);
	g_pd3dDevice->LightEnable( 0, TRUE );

	return S_OK;
}

HRESULT K3DScene::FrameMove()
{
	m_TimeControl.FrameMove();

	for_each(m_RenderEntityList.begin(),m_RenderEntityList.end(),mem_fun_ref(KSceneEntity::FrameMove));

	//g_cGraphicsTool.GetSunDirection((D3DXVECTOR3)m_SunLight.Direction);
	m_SunLight.Direction.x = sinf(timeGetTime()*0.0001f);
	m_SunLight.Direction.y = - 1.0f;
	m_SunLight.Direction.z = cosf(timeGetTime()*0.0001f);
	g_pd3dDevice->SetLight(0,&m_SunLight);
	
	return S_OK;
}

HRESULT K3DScene::DrawRenderEntity(LPSCENEENTITY pObj)
{
	switch(pObj->Type)
	{
	case SCENEENTITY_OBJECT:
		{
			LPOBJECT pObject = (LPOBJECT) pObj->Pointer;
			pObject->Render();
		}
		break;
	case SCENEENTITY_MODEL:
		{
			KModel* pMesh = (KModel*) pObj->Pointer;
			pMesh->Render();
		}
		break;
	case SCENEENTITY_POSITION:
		{
			g_cGraphicsTool.DrawPosition((D3DXVECTOR3*)pObj->Pointer);
		}
		break;
	}
	return S_OK;
}

HRESULT K3DScene::DrawRenderEntityShadow(LPSCENEENTITY pObj)
{
	switch(pObj->Type)
	{
	case SCENEENTITY_OBJECT:
		{
			LPOBJECT pObject = (LPOBJECT) pObj->Pointer;
			pObject->RenderShadow(1);
		}
		break;
	case SCENEENTITY_MODEL:
		{
			KModel* pMesh = (KModel*) pObj->Pointer;
			pMesh->RenderShadow();
		}
		break;
	}
	return S_OK;
}

HRESULT K3DScene::AddRenderMesh(KModel *pMesh)
{
	KSceneEntity Entity(SCENEENTITY_MODEL,pMesh);
	m_RenderEntityList.push_back(Entity);
	return S_OK;
}

HRESULT K3DScene::AddRenderObject(LPOBJECT pObject)
{
	KSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
	m_RenderEntityList.push_back(Entity);
	return S_OK;
}

HRESULT K3DScene::RemoveRenderObject(LPOBJECT pObject)
{
	KSceneEntity DestEntity(SCENEENTITY_OBJECT,pObject);
	m_RenderEntityList.remove(DestEntity);
	return S_OK;
}

HRESULT K3DScene::RemoveRenderMesh(LPMODEL pMesh)
{
	KSceneEntity DestEntity(SCENEENTITY_MODEL,pMesh);
	m_RenderEntityList.remove(DestEntity);
	return S_OK;
}

BOOL K3DScene::CheckRenderObject(LPOBJECT pObject)
{
	KSceneEntity DestEntity(SCENEENTITY_OBJECT,pObject);

	ENTITYLIST::iterator i = m_RenderEntityList.begin();
	while(i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;

		if (Entity==DestEntity)
		{		
			return TRUE;
		}
		i++;
	}
	return FALSE;
}

HRESULT K3DScene::AddRenderEntity(SCENEENTITYTYPE Type,PVOID pEntity)
{
	KSceneEntity Entity(Type,pEntity);
	m_RenderEntityList.push_back(Entity);
	return E_NOTIMPL;
}

HRESULT K3DScene::RemoveRenderEntity(SCENEENTITYTYPE Type,PVOID pEntity)
{
	KSceneEntity DestEntity(Type,pEntity);
	m_RenderEntityList.remove(DestEntity);
	return E_NOTIMPL;
}

HRESULT K3DScene::RemoveRenderEntity(KSceneEntity Entity)
{
	m_RenderEntityList.remove(Entity);
	return E_NOTIMPL;
}

HRESULT K3DScene::CleanUp(void)
{
	m_RenderEntityList.clear();
	return E_NOTIMPL;
}

HRESULT K3DScene::_FogInfo::ApplyFog()
{
	if(!m_bFogEnable)
	{
		g_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		return S_OK;
	}

	g_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  m_FogColor );

	g_pd3dDevice->SetRenderState( D3DRS_FOGSTART,   FtoDW(m_fFogStartValue) );
	g_pd3dDevice->SetRenderState( D3DRS_FOGEND,     FtoDW(m_fFogEndValue) );
	g_pd3dDevice->SetRenderState( D3DRS_FOGDENSITY, FtoDW(m_fFogDensity) );

	if( m_bUsingTableFog )
	{
		g_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
		g_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   m_nFogMode );
	}
	else
	{
		g_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
		g_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  m_nFogMode );
		g_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, m_bRangeBasedFog );
	}

	return S_OK;
}

D3DXVECTOR3 K3DScene::GetSunDirection()
{
	return m_SunDirection;
}

HRESULT  K3DScene::GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR2* pIN,DWORD Type)
{
	*pOut = D3DXVECTOR3(0,0,0);
	return S_OK;
}
