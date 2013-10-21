// SceneTable.cpp: implementation of the KSceneTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "SceneTable.h"
#include "SceneSkinEdit.h"
#include ".\scenetable.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KSceneTable::KSceneTable()
{
	m_lpScenes = NULL;
	m_dMaxNumScene = 0;
	m_dNumScene = 0;
	m_lpCurScene = NULL;
}

KSceneTable::~KSceneTable()
{

}

HRESULT KSceneTable::InvalidateDeviceObjects(void)
{
	for(DWORD i=0;i<m_dNumScene;i++)
	{
		LPSCENE pScene = m_lpScenes[i];
		pScene->InvalidateDeviceObjects();
	}
	return S_OK;
}

HRESULT KSceneTable::RestoreDeviceObjects(void)
{
	for(DWORD i=0;i<m_dNumScene;i++)
	{
		LPSCENE pScene = m_lpScenes[i];
		pScene->RestoreDeviceObjects();
	}
	return S_OK;
}

HRESULT KSceneTable::Get1NewScenePointer(LPSCENE** ppScene)
{
	if (m_dMaxNumScene ==0 )
	{
		m_lpScenes = new LPSCENE[5];
		ZeroMemory(m_lpScenes ,sizeof(LPSCENE)*5);
		m_dMaxNumScene = 5;
	}
	if (m_dNumScene >= m_dMaxNumScene)
	{
		LPSCENE* Temp = m_lpScenes;
		m_dMaxNumScene +=5;
		m_lpScenes = new LPSCENE[m_dMaxNumScene];
		ZeroMemory(m_lpScenes ,sizeof(LPSCENE)*m_dMaxNumScene);
		for(DWORD i=0;i<m_dMaxNumScene-5 ;i++) m_lpScenes[i] = Temp[i];
		delete [] Temp;
	}	
	m_dNumScene ++;
	(*ppScene) = &m_lpScenes[ m_dNumScene -1 ];

	return S_OK;
}

HRESULT KSceneTable::Get1NewScene(DWORD Type,LPSCENE* ppScene)
{
	LPSCENE * NewPointer = NULL;
	if (FAILED(Get1NewScenePointer(&NewPointer)))
		return E_FAIL;
	switch(Type)
	{
	case SCENE_DEFAULT:
		{
			(*NewPointer) = new K3DScene;
			if(FAILED((*NewPointer)->Initialize(this)))
				goto Exit_Error;
			break;
		}
	case SCENE_SKINEDITOR:
		{
			(*NewPointer) = new KSceneSceneEdit;
			if(FAILED((*NewPointer)->Initialize(this))) 
				goto Exit_Error;
			break;
		}
	case SCENE_MESHEDITOR:
		{
			(*NewPointer) = new KSceneModelEditor;
			if(FAILED((*NewPointer)->Initialize(this))) 
				goto Exit_Error;
			break;
		}
	case SCENE_OBJECTEDITOR:
		{
			(*NewPointer) = new KSceneObjectEditor;
			if(FAILED((*NewPointer)->Initialize(this))) 
				goto Exit_Error;
		break;
		}
	case SCENE_SWORDTEST:
		{
			(*NewPointer) = new KSceneSwordTest;
			if(FAILED((*NewPointer)->Initialize(this))) 
				goto Exit_Error;
		break;
		}	
	}
	(*NewPointer)->ID = m_dNumScene;
	(*ppScene) = (*NewPointer);
	return S_OK;

Exit_Error:
	(*NewPointer)->ID = m_dNumScene;
	(*ppScene) = (*NewPointer);
	return E_FAIL;
}

HRESULT KSceneTable::GetScene(LPSCENE* ppScene,DWORD ID)
{
	if ((ID>m_dNumScene)||(ID==0)) return E_FAIL;
	(*ppScene) = m_lpScenes[ID-1];
	return S_OK;
}

HRESULT KSceneTable::RefreshCurScene()
{
	LPSCENE pScene = NULL;
	if (SUCCEEDED(GetScene(&pScene ,m_dCurSceneID)))
	{
		m_lpCurScene = pScene;
	}
	else
	{
		m_lpCurScene = NULL;
	}
	return S_OK;
}

HRESULT KSceneTable::SetManager(PVOID pManager)
{
	m_pManager = pManager;
	return S_OK;
}

HRESULT KSceneTable::CleanUp(void)
{
	for(DWORD i=0;i<m_dNumScene;i++)
	{
		LPSCENE pScene = m_lpScenes[i];
		pScene->CleanUp();
		SAFE_DELETE(pScene);
	}
	SAFE_DELETE_ARRAY(m_lpScenes);
	m_dNumScene = 0;
	m_dMaxNumScene = 0;

	g_cObjectTable.CleanUp();
	g_cMeshTable.CleanUp();
	g_cTextureTable.CleanUp();

	m_dCurSceneID = 0;
	return E_NOTIMPL;
}
