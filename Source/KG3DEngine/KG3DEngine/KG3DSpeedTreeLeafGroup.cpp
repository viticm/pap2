#include "stdafx.h"
#include "KG3DSpeedTreeLeafGroup.h"
#include "KG3DSceneModelEditor.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DSpeedTreeLeafGroup::KG3DSpeedTreeLeafGroup(void)
{
	m_dwScaleMode = KG3DSceneModelEditor::KG3DSpeedTreeModifier::ENU_SCALE_UNIFORM;
}

KG3DSpeedTreeLeafGroup::~KG3DSpeedTreeLeafGroup(void)
{
}

void KG3DSpeedTreeLeafGroup::OnRotation(const D3DXQUATERNION& vec4Rotation)
{
	D3DXMATRIX matRotation;
	D3DXMATRIX matCurRotation;

	D3DXQUATERNION vec4Current;
	GetRotation(&vec4Current);
	D3DXMatrixRotationQuaternion(&matCurRotation, &vec4Current);
	D3DXMatrixInverse(&matCurRotation, NULL, &matCurRotation);

	D3DXMatrixRotationQuaternion(&matRotation, &vec4Rotation);
	matRotation = matCurRotation * matRotation;
	
	for (std::set<Leaf*>::iterator i = m_Leaves.begin();
		i != m_Leaves.end();
		i++)
	{
		D3DXVECTOR3 vec3Offset = (*i)->Position - m_vec3Center;
		D3DXVec3TransformCoord(&vec3Offset, 
			&vec3Offset,
			&matRotation);
		(*i)->Position = m_vec3Center + vec3Offset;
		(*i)->UpdateLeafOriginPosition();
	}

	if (m_Leaves.size())
	{
		(*m_Leaves.begin())->SetDirtyFlag();
	}

    //KG3DTransformation::SetRotation((D3DXQUATERNION*)&vec4Rotation);
}

void KG3DSpeedTreeLeafGroup::OnMove(const D3DXVECTOR3& vec3Offset)
{
	for (std::set<Leaf*>::iterator i = m_Leaves.begin();
		i != m_Leaves.end();
		i++)
	{
		(*i)->Position += vec3Offset;
		(*i)->UpdateLeafOriginPosition();
	}
	if (m_Leaves.size())
	{
		(*m_Leaves.begin())->SetDirtyFlag();
		CalculateCenter(m_vec3Center);
	}

    //KG3DTransformation::SetTranslation((D3DXVECTOR3*)&vec3Offset);
}

void KG3DSpeedTreeLeafGroup::OnScaleUniform(const D3DXVECTOR3& vec3Scale)
{
	for (std::set<Leaf*>::iterator i = m_Leaves.begin();
		i != m_Leaves.end();
		i++)
	{
		D3DXVECTOR3 vec3Offset = (*i)->Position - m_vec3Center;
		vec3Offset.x *= vec3Scale.x;
		vec3Offset.y *= vec3Scale.y;
		vec3Offset.z *= vec3Scale.z;
		(*i)->Position = m_vec3Center + vec3Offset;
		(*i)->Size *= vec3Scale.x;
		(*i)->UpdateLeafOriginPosition();
	}
	if (m_Leaves.size())
	{
		(*m_Leaves.begin())->SetDirtyFlag();
	}
}

void KG3DSpeedTreeLeafGroup::OnScale(const D3DXVECTOR3 &vec3Scale)
{
	switch(m_dwScaleMode)
	{
	case KG3DSceneModelEditor::KG3DSpeedTreeModifier::ENU_SCALE_UNIFORM:
		OnScaleUniform(vec3Scale);
		break;
	case KG3DSceneModelEditor::KG3DSpeedTreeModifier::ENU_SCALE_LEAF:
		OnLeafScale(vec3Scale);
		break;
	}

    //KG3DTransformation::SetScaling((D3DXVECTOR3*)&vec3Scale);
}

void KG3DSpeedTreeLeafGroup::OnLeafScale(const D3DXVECTOR3& vec3Scale)
{
	for (std::set<Leaf*>::iterator i = m_Leaves.begin();
		i != m_Leaves.end();
		i++)
	{
		(*i)->Size *= vec3Scale.x;
	}
	if (m_Leaves.size())
	{
		(*m_Leaves.begin())->SetDirtyFlag();
		CalculateCenter(m_vec3Center);
	}
}

void KG3DSpeedTreeLeafGroup::CalculateCenter(D3DXVECTOR3& vec3Center)
{
	vec3Center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	for(std::set<Leaf*>::iterator i = m_Leaves.begin();
		i != m_Leaves.end();
		i++)
	{
		vec3Center += (*i)->Position;
	}
	vec3Center /= static_cast<float>(m_Leaves.size());

    //KG3DTransformation::SetTranslation((D3DXVECTOR3*)&vec3Center);
}



BOOL KG3DSpeedTreeLeafGroup::AddLeaf(Leaf* pLeaf)
{
	static D3DXQUATERNION vec4Id(1, 0, 0, 0);
	BOOL bReturn = static_cast<BOOL>((m_Leaves.insert(pLeaf)).second);
	pLeaf->bSelected = TRUE;
	if (bReturn)
	{
		CalculateCenter(m_vec3Center);
	}
	
	SetRotation(&vec4Id);
	return bReturn;
}

void KG3DSpeedTreeLeafGroup::RemoveLeaf(Leaf* pLeaf)
{
	static D3DXQUATERNION vec4Id(1, 0, 0, 0);
	std::set<Leaf*>::iterator i = m_Leaves.find(pLeaf);
	pLeaf->bSelected = FALSE;
	if (i != m_Leaves.end())
	{
		m_Leaves.erase(pLeaf);
		CalculateCenter(m_vec3Center);
	}
	SetRotation(&vec4Id);
}

void KG3DSpeedTreeLeafGroup::Clear(KG3DSceneModelEditor* pScene)
{
	KG_PROCESS_ERROR(pScene);

	for (std::set<Leaf*>::iterator i = m_Leaves.begin();
		i != m_Leaves.end(); 
		i++)
	{
		(*i)->bSelected = FALSE;
        //pScene->RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, (*i)));
		pScene->AddRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, (*i)), TRUE, FALSE);
	}
	m_Leaves.clear();
	m_vec3Center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

Exit0:
	return;
}

Leaf* KG3DSpeedTreeLeafGroup::GetFirstLeaf()
{
	if (m_Leaves.size())
		return *(m_Leaves.begin());
	return NULL;
}


HRESULT KG3DSpeedTreeLeafGroup::Render(unsigned uOption, void* pExtInfo)
{
	HRESULT hr = E_FAIL;
	hr = S_OK;
	for(std::set<Leaf*>::iterator i = m_Leaves.begin();
		i != m_Leaves.end();
		i++)
	{
		(*i)->RenderSelectMark();
	}
	//Exit0:
	return hr;
}

void KG3DSpeedTreeLeafGroup::GetCenter(D3DXVECTOR3& vec3Center)
{
	vec3Center = m_vec3Center;
}

DWORD KG3DSpeedTreeLeafGroup::GetScaleMode()
{
	return m_dwScaleMode;
}

void KG3DSpeedTreeLeafGroup::SetScaleMode(DWORD dwScale)
{
	m_dwScaleMode = dwScale;
}


/*KG3DSceneEntityList KG3DSpeedTreeLeafGroup::ConvertLeavesToEntityList()
{
    KG3DSceneEntityList lstReturn;
    for (std::set<Leaf*>::iterator i = m_Leaves.begin(); i != m_Leaves.end(); i++)
    {
        lstReturn.push_back(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, *i));
    }
    return lstReturn;
}*/
HRESULT KG3DSpeedTreeLeafGroup::SetAllLeavesSelectable(KG3DSceneEditorBase& editBase)
{
	 for (std::set<Leaf*>::iterator i = m_Leaves.begin(); i != m_Leaves.end(); i++)
	 {
		 //KG3DSceneEntity entity(SCENEENTITY_SPEEDTREELEAF, *i);
		 //selTool.SetSelectable(entity, TRUE);
		 editBase.IEInsertEntitySelectable(SCENEENTITY_SPEEDTREELEAF, *i, TRUE);//<Selectable Refactor>
	 }
	 return S_OK;
}

void KG3DSpeedTreeLeafGroup::UpdateByMatrix(const D3DXMATRIX& mat)
{
    KG3DTransformation::UpdateByMatrix(mat);
    OnScale(m_Scaling);
    OnRotation(m_Rotation);
    OnMove(m_Translation);
}

D3DXMATRIX KG3DSpeedTreeLeafGroup::GetWorldMatrix()
{
    return GetMatrix();
}

D3DXMATRIX KG3DSpeedTreeLeafGroup::GetMatrix()
{
    D3DXMATRIX matMov;
    D3DXMATRIX matRot;
    D3DXMatrixTranslation(&matMov, m_vec3Center.x, m_vec3Center.y, m_vec3Center.z);
    D3DXMatrixRotationQuaternion(&matRot, &m_Rotation);
    return matRot * matMov;
}