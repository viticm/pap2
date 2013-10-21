////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneTopSnapTool.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-13 16:47:28
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DSceneTopSnapTool.h"
#include "KG3DSceneOutDoorSpaceMgr.h"

////////////////////////////////////////////////////////////////////////////////


ULONG STDMETHODCALLTYPE KG3DSceneTopSnapTool::GetType()
{
	return KG3DTYPE_SCENE_TOP_SNAP_TOOL;
}

KG3DSceneTopSnapTool::KG3DSceneTopSnapTool( KG3DScene& Scene )
	:m_Scene(Scene)
{
	
}

KG3DCommonObject* KG3DSceneTopSnapTool::Build( LPCTSTR lpFile, DWORD_PTR pScene, DWORD_PTR )
{
	KG3DSceneTopSnapTool* p = NULL;
	KG3DScene* pSceneSolid = reinterpret_cast<KG3DScene*>(pScene);
	_ASSERTE(NULL != pSceneSolid);
	KG_PROCESS_ERROR(NULL != pSceneSolid);
	p = new KG3DSceneTopSnapTool(*pSceneSolid);
	KG_PROCESS_ERROR(NULL != p);
	{
		HRESULT hr = p->Init();
		KG_COM_PROCESS_ERROR(hr);
	}
	return p;
Exit0:
	SAFE_RELEASE(p);
	return NULL;
}

HRESULT STDMETHODCALLTYPE KG3DSceneTopSnapTool::Render()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE KG3DSceneTopSnapTool::FrameMove()
{
#if 0
	HRESULT hr = E_FAIL;
	{
		KG3DCamera	Cam;
		KG3DSceneOutDoorSpaceMgr* pOutDoor = m_Scene.GetOutDoorSpaceMgr();
		KG_PROCESS_ERROR(NULL != pOutDoor);

		struct ObjVisitor : KG3DRepresentObjectVisitor 
		{
			KG3DCuller* m_pCuller;
			
			KG3DRenderData* m_pRenderData;
			virtual HRESULT Accept(KG3DRepresentObject& Obj)
			{
				if(m_pCuller->Cull(Obj))
					Obj.GetRenderSorterStratege().PutIntoRenderData(Obj, *m_pRenderData);
				return S_OK;
			}
		};

		ObjVisitor objVisitor;
		objVisitor.m_pCuller = NULL;
		
		objVisitor.m_pRenderData = NULL;

		struct CtVisitor : KG3DEntityCtVisitor 
		{
			ObjVisitor* m_pObjVisitor;
			virtual HRESULT Accept(KG3DSceneEntityContainer& Ct)
			{
				Ct.Traverse(*m_pObjVisitor);
				return S_OK;
			}
		};

		CtVisitor ctVisitor;
		ctVisitor.m_pObjVisitor = &objVisitor;

		enum{em_draw_region_count_per_time = 4,};
		int nStartX = 0, nStartZ = 0, nXWidth = 0, nZWidth = 0;
		hr = pOutDoor->GetSceneRect(nStartX, nStartZ, nXWidth, nZWidth);

		INT RegionXStart = GetSceneRegionIndex(nStartX * 100 / SCENEREGIONWIDTH);
		INT RegionZStart = GetSceneRegionIndex(nStartZ * 100 / SCENEREGIONWIDTH);
		INT RegionXCount = nXWidth * 100 / SCENEREGIONWIDTH;
		INT RegionZCount = nZWidth * 100 / SCENEREGIONWIDTH;
		for (INT z = RegionZStart; z < RegionZCount; z += em_draw_region_count_per_time)
		{	
			for (INT x = RegionXStart; x < RegionXCount; x += em_draw_region_count_per_time)
			{
				KG3DSceneRegion* p = pOutDoor->GetRegion(x, z);
				if(NULL == p)
					constinue;
				ctVisitor.Accept(*p);
			}

		}
	}
	return S_OK;
Exit0:
#endif
	return E_FAIL;
}