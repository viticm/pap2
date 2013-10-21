// XMeshTable.cpp: implementation of the KModelTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "XMeshTable.h"
#include "MeshBlade.h"

#include "MeshTerran.h"
#include "MeshStrip.h"
#include "MeshGroup.h"
#include "MeshSkyBox.h"
#include "MeshFlare.h"
#include "MeshMainTool.h"
#include "MeshMoveGrid.h"
#include "MeshRotateGrid.h"
#include "MeshObjectTableTool.h"
#include ".\meshmeshmaterialtool.h"
#include ".\meshcolorpick.h"
#include ".\meshslidebar.h"
#include ".\mesheditctrl.h"
#include ".\meshitem.h"
#include ".\meshlistbox.h"
#include ".\meshmeshanimation.h"
#include "MeshMeshTool.h"
#include "MeshComboBox.h"
#include "MeshMeshSelect.h"
#include "MeshBillBoard.h"
#include "MeshPropertySet.h"
#include "MeshScreenSFX.h"
#include "MeshGlare.h"
#include "MeshLineGrass.h"
#include "MeshWinBase.h"
#include "MeshButton.h"
#include "MeshListCtrl.h"
#include "meshtreectrl.h"
#include "kmodelsfxbillboard.h"
//#include "./KModelTerrainLOD.h"
#include "./KModelSubd.h"
#include "./KModelWater.h"
#include "./KModelBelt.h"
//#include"KVolumeFog.h"
//#include"KLightning.h"
#include "KModelSpeedTree.h"
#include "KModelSceneSFX.h"
#include "KModelParticleSystem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KModelTable::KModelTable()
{
	m_dMaxNumXMesh = 0;
	m_dNumXMesh = 0;
	m_lpXMeshes = NULL;
	m_lpTextureTable = NULL;
}

KModelTable::~KModelTable()
{

}

HRESULT KModelTable::Get1NewXMeshPointer(LPMODEL **pppXMesh)
{
	if (m_dMaxNumXMesh ==0 )
	{
		m_lpXMeshes = new LPMODEL[5];
		ZeroMemory(m_lpXMeshes ,sizeof(LPMODEL)*5);
		m_dMaxNumXMesh = 5;
	}
	if (m_dNumXMesh >= m_dMaxNumXMesh)
	{
		LPMODEL* Temp = m_lpXMeshes;
		m_dMaxNumXMesh +=5;
		m_lpXMeshes = new LPMODEL[m_dMaxNumXMesh];
		ZeroMemory(m_lpXMeshes ,sizeof(LPMODEL)*m_dMaxNumXMesh);
		for(DWORD i=0;i<m_dMaxNumXMesh-5 ;i++) m_lpXMeshes[i] = Temp[i];
		delete [] Temp;
	}	
	m_dNumXMesh ++;
	(*pppXMesh) = &m_lpXMeshes[ m_dNumXMesh -1 ];

	return S_OK;
}

HRESULT KModelTable::Get1NewXMesh(LPMODEL *ppXMesh)
{
	LPMODEL * NewPointer = NULL;
	if (FAILED(Get1NewXMeshPointer(&NewPointer)))
		return E_FAIL;
	(*NewPointer) = new KModel;
	(*NewPointer)->Initialize(this); 
	(*NewPointer)->ID = m_dNumXMesh;
	
	(*ppXMesh) = (*NewPointer);
	return S_OK;
}

HRESULT KModelTable::Get1NewXMesh(LPMODEL *ppXMesh,DWORD Type)
{
	LPMODEL * NewPointer = NULL;
	if (FAILED(Get1NewXMeshPointer(&NewPointer)))
		return E_FAIL;
	switch(Type)
	{
	case MODELTYPE_DEFAULT:
		(*NewPointer) = new KModel;
		break;
	case MODELTYPE_TERRAIN:
		(*NewPointer) = new KModelTerrain;
		break;	
	case MODELTYPE_STRIP:
		(*NewPointer) = new KModelStrip;
		break;
	case MODELTYPE_GROUP:
		(*NewPointer) = new ModelSFX::KModelGroup;
		break;
	case MODELTYPE_SKYBOX:
		(*NewPointer) = new ModelSFX::KModelSFXSkyBox;
		break;
	case MODELTYPE_LIGHTFLARE:
		(*NewPointer) = new ModelSFX::KModelSFXFlare;
		break;
	case MODELTYPE_MAINTOOL:
		(*NewPointer) = NULL;//new ModelTools::KModelToolTerrain;
		break;
	case MODELTYPE_MOVEGRID:
		(*NewPointer) = new KModelGridMove;
		break;
	case MODELTYPE_ROTATEGRID:
		(*NewPointer) = new KModelGridRotate;
		break;
	case MODELTYPE_OBJECTTABLETOOL:
		(*NewPointer) = NULL;//new ModelTools::KModelToolObjectTable;
		break;
	case MODELTYPE_MATERIALTOOL:
		(*NewPointer) = NULL;//new ModelTools::KModelToolMaterial;
		break;
	case MODELTYPE_COLORPICK:
		(*NewPointer) = NULL;//new ModelTools::KModelToolColorPick;
		break;
	case MODELTYPE_SLIDEBAR:
		(*NewPointer) = NULL;//new ModelUI::KModelUISlideBar;
		break;
	case MODELTYPE_EDITCTRL:
		(*NewPointer) = NULL;//new ModelUI::KModelUIEditCtrl;
		break;
	case MODELTYPE_ITEM:
		(*NewPointer) = NULL;//new ModelUI::KModelUIItem;
		break;
	case MODELTYPE_LISTBOX:
		(*NewPointer) = NULL;//new ModelUI::KModelUIListBox;
		break;
	case MODELTYPE_ANIMATIONTOOL:
		(*NewPointer) = NULL;//new ModelTools::KModelToolAnimation;
		break;
	case MODELTYPE_MESHTOOL:
		(*NewPointer) = NULL;//new ModelTools::KModelToolMesh;
		break;
	case MODELTYPE_COMBOBOX:
		(*NewPointer) = NULL;//new ModelUI::KModelUIComboBox;
		break;
	case MODELTYPE_MESHSELECT:
		(*NewPointer) = NULL;//new ModelTools::KModelToolSelect;
		break;
	case MODELTYPE_BILLBOARD:
		(*NewPointer) = new ModelSFX::KModelSFXBillBoard;
		break;
	case MODELTYPE_PROPERTYSET:
		(*NewPointer) = NULL;//new ModelTools::KModelToolSFX;
		break;
	case MODELTYPE_BLADE:
		(*NewPointer) = new ModelSFX::KModelSFXBlade;
		break;
	case MODELTYPE_SCREENSFX:
		(*NewPointer) = new KModelSceneSFX;//new ModelSFX::KModelSFXScreen;
		break;
	case MODELTYPE_HALO:
		(*NewPointer) = new ModelSFX::KModelSFXHalo;
		break;
	case MODELTYPE_LINEGRASS:
		(*NewPointer) = new ModelSFX::KModelSFXLineGrass;
		break;
	case MODELTYPE_PARTICLESYSTEM:
		(*NewPointer) = new KModelParticleSystem;
		break;
	case MODELTYPE_WINBASE:
		(*NewPointer) = NULL;//new ModelUI::KModelUIBase;
		break;
	case MODELTYPE_BUTTON:
		(*NewPointer) = NULL;//new ModelUI::KModelUIButton;
		break;
	case MODELTYPE_LISTCTRL:
		(*NewPointer) = NULL;//new ModelUI::KModelUIListCtrl;
		break;
	case MODELTYPE_TREECTRL:
		(*NewPointer) = NULL;//new ModelUI::KModelUITreeCtrl;
		break;

	/*case MODELTYPE_TERRAINLOD:
		(*NewPointer) = new KModelTerrainLOD;
		break;*/

	case MODELTYPE_SUBDEV:
		(*NewPointer) = new KModelSubd;
		break;
	case MODELTYPE_WATER:
		(*NewPointer) = new Water::KModelWater;
		break;

	/*case MODELTYPE_LAYEREDVOLUMETRICFOG:
		(*NewPointer) = new KVolumeFog;
		break;*/

	/*case MODELTYPE_DYNAMICLIGHTNING:
		(*NewPointer) = new KLightning;
		break;*/
	
	case MODELTYPE_SPEEDTREE:
		(*NewPointer) = new KModelSpeedTree;
		break;
	case MODELTYPE_SCENESFX:
		(*NewPointer) = new KModelSceneSFX;
		break;		
	case MODELTYPE_BELT:
		(*NewPointer) = new ModelSFX::KModelBelt;
		break;

	default:
		(*NewPointer) = new KModel;
		break;
	}
	
	(*NewPointer)->Initialize(this); 
	(*NewPointer)->ID = m_dNumXMesh;
	(*NewPointer)->m_dwType = Type;

	(*ppXMesh) = (*NewPointer);
	return S_OK;
}

HRESULT KModelTable::GetXMesh(LPMODEL *ppXMesh, DWORD ID)
{
	if ((ID>m_dNumXMesh)||(ID==0)) return E_FAIL;
	(*ppXMesh) = m_lpXMeshes[ID-1];
	return S_OK;
}

void KModelTable::SetTextureTable(KTextureTable *pTextureTable)
{
	m_lpTextureTable = pTextureTable;
}

void KModelTable::Initialize(PVOID pObjectTable)
{
	m_lpParentTable = pObjectTable;
}

HRESULT KModelTable::CleanUp()
{
	for(DWORD i=1;i<=m_dNumXMesh;i++)
	{
		LPMODEL pXMesh = NULL;
		if (SUCCEEDED(GetXMesh(&pXMesh,i)))
		{
			pXMesh->CleanUp();
		}
		
		SAFE_DELETE(pXMesh);
	}
	m_dNumXMesh = 0;
	m_dMaxNumXMesh = 0;
	if (m_lpXMeshes!=NULL)
	{
		delete [] m_lpXMeshes;
		m_lpXMeshes = NULL;
	}
	return S_OK;
}

HRESULT KModelTable::LoadFromFile(DWORD *pMeshID, LPSTR pFileName)
{
	TCHAR Name[256];
	HRESULT hr = S_OK;
	LPMODEL pMesh = NULL;	

	if(FAILED(g_cFileFinder.FindFile(Name,pFileName)))
		return E_FAIL;

	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",Name);
	_splitpath( path_buffer, drive, dir, fname, ext );

	if(_strcmpi(ext,".STree")==0)
	{
		if (FAILED(g_cMeshTable.Get1NewXMesh(&pMesh,MODELTYPE_SPEEDTREE)))
			return E_FAIL;

		if (FAILED(hr = (/*(KModelSpeedTree*)*/pMesh)->LoadMesh(Name)))
		{ 
			return hr;
		}
		(*pMeshID) = pMesh->ID;
	}
	else if(_strcmpi(ext,".SSFX")==0)
	{
		if (FAILED(g_cMeshTable.Get1NewXMesh(&pMesh,MODELTYPE_SCENESFX)))
			return E_FAIL;

		if (FAILED(hr = pMesh->LoadMesh(Name)))
		{ 
			return hr;
		}
		(*pMeshID) = pMesh->ID;
	}
	else
	{
		for(DWORD i=0;i<m_dNumXMesh;i++) 
		{
			LPMODEL pMesh = m_lpXMeshes[i] ;

			if (strcmp(pMesh->m_szMeshFileName,pFileName)==0)
			{
				(*pMeshID) = pMesh->ID;
				return S_OK;
			}
			else
			{
				continue;
			}
		}

		if(_strcmpi(ext,".Mesh")==0)
		{
			if (FAILED(g_cMeshTable.Get1NewXMesh(&pMesh)))
				return E_FAIL;
			if (FAILED(hr = pMesh->LoadMesh(Name)))
			{ 
				return hr;
			}
			(*pMeshID) = pMesh->ID;
		}
		else if( !_strcmpi(ext, ".x") && !_strcmpi(ext, ".X") )
		{
			if (FAILED(g_cMeshTable.Get1NewXMesh(&pMesh)))
				return E_FAIL;
			if (FAILED(hr = pMesh->LoadFromX(Name)))
			{ 
				return hr;
			}
			(*pMeshID) = pMesh->ID;
			strcpy(pMesh->m_szMeshFileName, fname);
		}
		else if(_strcmpi(ext,".lvf")==0)
		{
			//if (FAILED(g_cMeshTable.Get1NewXMesh(&pMesh,MODELTYPE_LAYEREDVOLUMETRICFOG)))
			//	return E_FAIL;
			////////////////////////////////////////////////////////////////////////////
			////在这里填入VolumeFog的载入代码
			//pMesh->LoadMesh(Name);
			//(*pMeshID) = pMesh->ID;
		}
		else if(_strcmpi(ext,".dlf")==0)
		{
			//if (FAILED(g_cMeshTable.Get1NewXMesh(&pMesh,MODELTYPE_DYNAMICLIGHTNING)))
			//	return E_FAIL;
			////////////////////////////////////////////////////////////////////////////
			////在这里填入VolumeFog的载入代码
			//pMesh->LoadMesh(Name);
			//(*pMeshID) = pMesh->ID;
		}
	}

	return S_OK;
}

HRESULT KModelTable::InvalidateDeviceObjects(void)
{
	for(DWORD i=1;i<=m_dNumXMesh;i++)
	{
		LPMODEL pXMesh = NULL;
		if (SUCCEEDED(GetXMesh(&pXMesh,i)))
		{
			pXMesh->InvalidateDeviceObjects();
		}
	}
	return S_OK;
}

HRESULT KModelTable::RestoreDeviceObjects(void)
{
	for(DWORD i=1;i<=m_dNumXMesh;i++)
	{
		LPMODEL pXMesh = NULL;
		if (SUCCEEDED(GetXMesh(&pXMesh,i)))
		{
			pXMesh->RestoreDeviceObjects();
		}
	}
	return S_OK;
}

HRESULT KModelTable::GetCopy(DWORD* pNewMeshID,DWORD SrcMeshID)
{
	LPMODEL pXMesh = NULL;
	if (FAILED(GetXMesh(&pXMesh,SrcMeshID)))
		return E_FAIL;
	LPMODEL pNewModel = NULL;

	if(FAILED(pXMesh->Copy(&pNewModel)))
		return E_FAIL;
	(*pNewMeshID) = pNewModel->ID;
	return S_OK;
}