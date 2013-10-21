// KEntity.h: interface for the KEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KENTITY_H__B946D9D8_A276_4D03_B761_C63020A88AAB__INCLUDED_)
#define AFX_KENTITY_H__B946D9D8_A276_4D03_B761_C63020A88AAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MeshWinBase.h"
#include "DataObject.h"
#include "./KRegion.h"
#include "Kg3DTypes.h"

struct KSceneEntity;

typedef class KSceneEntityGroup
{
public:
	list<KSceneEntity>m_ListEntity;
	
	AABBOX m_BBox;

	D3DXVECTOR3   m_Position;
	D3DXVECTOR3   m_Scaling;
	D3DXVECTOR3   m_Rotation;
	D3DXMATRIX    m_Matrix;

public:
	void operator = (const KSceneEntityGroup& Group)
	{
		m_ListEntity = Group.m_ListEntity;
		m_BBox = Group.m_BBox;
	}
	void ComputeBBox();

	KSceneEntityGroup();
	virtual ~KSceneEntityGroup();

}*LPENTITYGROUP,ENTITYGROUP;

typedef struct KSceneEntity 
{
	SCENEENTITYTYPE Type;
	PVOID           Pointer;
	PVOID           RefData;
	DWORD           RenderCount;//渲染计数器，用来判断是否本桢已经渲染过

	KSceneEntity()
	{
		Type = SCENEENTITY_NONE;
		Pointer = NULL;
		RefData = NULL;
	}
	KSceneEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer)
	{
		Type = EntityType;
		Pointer = EntityPointer;
		RefData = NULL;
	}

	KSceneEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer,PVOID Data)
	{
		Type = EntityType;
		Pointer = EntityPointer;
		RefData = Data;
	}

	virtual ~KSceneEntity()
	{
		Type = SCENEENTITY_NONE;
		Pointer = NULL;
		RefData = NULL;	
		//SAFE_DELETE(RefData);
	}
	bool operator == (const KSceneEntity A) const
	{
		if((Type==A.Type)&&(Pointer==A.Pointer))
			return true;
		else 
			return false;
	}

	void Move(D3DXVECTOR3 Pos) const;
	

	void Rotation(D3DXVECTOR3 Pos) const
	{
		switch(Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Pointer;
				D3DXVECTOR3 K(pObject->m_Rotation.x+Pos.x,
					pObject->m_Rotation.y+Pos.y,
					pObject->m_Rotation.z+Pos.z);

				pObject->Edit_Rotate(&K);
				break;
			}
		/*case SCENEENTITY_MODEL:
			{
				LPMODEL pModel = (LPMODEL) Pointer;
				pModel->m_Matrix._11 *= Pos.x;
				pModel->m_Matrix._22 *= Pos.y;
				pModel->m_Matrix._33 *= Pos.z;

				break;
			}*/
		}
	}

	void Scale(D3DXVECTOR3 Pos) const
	{
		switch(Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Pointer;
				D3DXVECTOR3 K(pObject->m_Scaling.x*Pos.x,
					pObject->m_Scaling.y*Pos.y,
					pObject->m_Scaling.z*Pos.z);

				pObject->Edit_Scaling(&K);
				break;
			}
		case SCENEENTITY_MODEL:
			{
				LPMODEL pModel = (LPMODEL) Pointer;
				pModel->m_Matrix._11 *= Pos.x;
				pModel->m_Matrix._22 *= Pos.y;
				pModel->m_Matrix._33 *= Pos.z;

				break;
			}
		case SCENEENTITY_OBJECT_PART:
			{
				LPOBJECT pObject = (LPOBJECT) Pointer;
				DWORD    MeshIndex = (DWORD) RefData;
				KComponentObject::LPMESHKEEPER pKeep = NULL;

				if(SUCCEEDED(pObject->GetMeshKeeper(&pKeep,MeshIndex+1)))
				{
					pKeep->m_Matrix._41 *= Pos.x;
					pKeep->m_Matrix._42 *= Pos.y;
					pKeep->m_Matrix._43 *= Pos.z;
				}
				break;
			}
		}
	}

	HRESULT FrameMove()
	{
		switch(Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Pointer;
				pObject->FrameMove();
				break;
			}
		case SCENEENTITY_MODEL:
			{
				LPMODEL pModel = (LPMODEL) Pointer;
				pModel->FrameMove();
				break;
			}
		}
		return S_OK;
	}

	HRESULT Render();
	HRESULT RenderWithEffect(DWORD Option,PVOID pData);

	HRESULT RenderDirect()
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
		}

		return S_OK;
	}
	HRESULT RenderShadow(DWORD ShadowType)
	{
		switch(Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Pointer;
				pObject->RenderShadow(ShadowType);
				break;
			}
		}
		return S_OK;
	}

	PropertyBase::KPropertyBase* Acquire()
	{
		switch(Type)
		{
		case SCENEENTITY_OBJECT:
			{
				return NULL;
				break;
			}
		case SCENEENTITY_MODEL:
			{
				LPMODEL pModel = (LPMODEL) Pointer;
				PropertyBase::KPropertyBase* pPro =  dynamic_cast<PropertyBase::KPropertyBase*> (pModel);
				return pPro;
				break;
			}
		}
		return NULL;
	}
}*LPSCENEENTITY;

class KEntity  
{
public:
	KEntity();
	virtual ~KEntity();

};

#endif // !defined(AFX_KENTITY_H__B946D9D8_A276_4D03_B761_C63020A88AAB__INCLUDED_)
