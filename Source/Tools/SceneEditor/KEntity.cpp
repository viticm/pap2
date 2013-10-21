// KEntity.cpp: implementation of the KEntity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KEntity.h"
#include "SceneSkinEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KEntity::KEntity()
{

}

KEntity::~KEntity()
{

}

HRESULT KSceneEntity::Render()
{
	if(RenderCount!=g_dwRenderCount)
	{
		switch(Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Pointer;
				pObject->Render();
				break;
			}
		case SCENEENTITY_MODEL:
			{
				LPMODEL pModel = (LPMODEL) Pointer;
				pModel->Render();
				break;
			}
		case SCENEENTITY_POSITION:
			{
				D3DXVECTOR3* pVec3 = (D3DXVECTOR3*) Pointer;
				g_cGraphicsTool.DrawPosition(pVec3);
				break;
			}
		case SCENEENTITY_GROUP:
			{
				KSceneSceneEdit* pEdit = (KSceneSceneEdit*) RefData;
				DWORD Index = (DWORD) Pointer;
				KSceneEntityGroup* pGroup = &pEdit->m_vecEntityGroup[Index] ;
				
				D3DXMATRIX MatSave;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &MatSave );
				g_pd3dDevice->SetTransform( D3DTS_WORLD, &pGroup->m_Matrix );

				for_each(pGroup->m_ListEntity.begin(),
					pGroup->m_ListEntity.end(),
					mem_fun_ref(Render));

				g_pd3dDevice->SetTransform( D3DTS_WORLD, &MatSave );

				break;
			}
		case SCENEENTITY_SOCKET:
			{
				LPMODEL pModel = (LPMODEL) Pointer;
				DWORD SocketIndex = (DWORD) RefData;

				break;
			}
		}

		RenderCount = g_dwRenderCount;
	}
	return S_OK;
}

HRESULT KSceneEntity::RenderWithEffect(DWORD Option,PVOID pData)
{
	if(RenderCount!=g_dwRenderCount)
	{
		switch(Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Pointer;
				pObject->RenderWithEffect(Option,pData);
				break;
			}
		/*case SCENEENTITY_MODEL:
			{
				LPMODEL pModel = (LPMODEL) Pointer;
				pModel->Render();
				break;
			}
		case SCENEENTITY_POSITION:
			{
				D3DXVECTOR3* pVec3 = (D3DXVECTOR3*) Pointer;
				g_cGraphicsTool.DrawPosition(pVec3);
				break;
			}
		case SCENEENTITY_GROUP:
			{
				KSceneSceneEdit* pEdit = (KSceneSceneEdit*) RefData;
				DWORD Index = (DWORD) Pointer;
				KSceneEntityGroup* pGroup = &pEdit->m_vecEntityGroup[Index] ;

				D3DXMATRIX MatSave;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &MatSave );
				g_pd3dDevice->SetTransform( D3DTS_WORLD, &pGroup->m_Matrix );

				for_each(pGroup->m_ListEntity.begin(),
					pGroup->m_ListEntity.end(),
					mem_fun_ref(Render));

				g_pd3dDevice->SetTransform( D3DTS_WORLD, &MatSave );

				break;
			}*/
		}

		RenderCount = g_dwRenderCount;
	}
	return S_OK;
}

KSceneEntityGroup::KSceneEntityGroup()
{
	m_Position = D3DXVECTOR3(0,0,0);
	m_Scaling  = D3DXVECTOR3(1,1,1);
	m_Rotation = D3DXVECTOR3(0,0,0);
	D3DXMatrixIdentity(&m_Matrix);
}

KSceneEntityGroup::~KSceneEntityGroup()
{
	;
}

void KSceneEntityGroup::ComputeBBox()
{
	D3DXVECTOR3 BoxPos[8];

	m_BBox.Clear();
	list<KSceneEntity>::iterator i=m_ListEntity.begin();
	while(i!=m_ListEntity.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				pObject->m_BBox.TransformCoord(BoxPos,&pObject->m_Matrix);

				m_BBox.AddPosition(BoxPos,8);
				break;
			}
		}
		i++;
	}
}

void KSceneEntity::Move(D3DXVECTOR3 Pos) const
{
	switch(Type)
	{
	case SCENEENTITY_OBJECT:
		{
			LPOBJECT pObject = (LPOBJECT) Pointer;
			pObject->Edit_Move(&(pObject->m_Position+Pos));
			break;
		}
	case SCENEENTITY_MODEL:
		{
			LPMODEL pModel = (LPMODEL) Pointer;
			pModel->m_Matrix._41 += Pos.x;
			pModel->m_Matrix._42 += Pos.y;
			pModel->m_Matrix._43 += Pos.z;

			break;
		}
	case SCENEENTITY_POSITION:
		{
			D3DXVECTOR3* pVec3 = (D3DXVECTOR3*) Pointer;
			(*pVec3) += Pos;

			break;
		}
	case SCENEENTITY_OBJECT_PART:
		{
			LPOBJECT pObject = (LPOBJECT) Pointer;
			DWORD    MeshIndex = (DWORD) RefData;
			KComponentObject::LPMESHKEEPER pKeep = NULL;

			if(SUCCEEDED(pObject->GetMeshKeeper(&pKeep,MeshIndex+1)))
			{
				pKeep->m_Matrix._41 += Pos.x;
				pKeep->m_Matrix._42 += Pos.y;
				pKeep->m_Matrix._43 += Pos.z;
			}
			break;
		}
	case SCENEENTITY_GROUP:
		{
			KSceneSceneEdit* pEdit = (KSceneSceneEdit*) RefData;
			DWORD Index = (DWORD) Pointer;
			KSceneEntityGroup* pGroup = &pEdit->m_vecEntityGroup[Index] ;

			///pGroup->m_Matrix._41 += Pos.x;
			//pGroup->m_Matrix._42 += Pos.y;
			//pGroup->m_Matrix._43 += Pos.z;

			//pGroup->m_Position += Pos;
			/*for_each(pGroup->m_ListEntity.begin(),
				pGroup->m_ListEntity.end(),
				bind2nd(mem_fun_ref(&KSceneEntity::Move),Pos));*/


			list<KSceneEntity>::iterator i = pGroup->m_ListEntity.begin();
			
			while(i!=pGroup->m_ListEntity.end())
			{
				KSceneEntity Entity = *i;
				Entity.Move(Pos);
				Entity.FrameMove();

				i++;
			}

			pGroup->ComputeBBox();
			break;
		}
	}
}

