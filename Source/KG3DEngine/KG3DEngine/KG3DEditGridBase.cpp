#include "stdafx.h"
#include ".\kg3deditgridbase.h"
#include ".\kg3denginemanager.h"
#include "kg3dmodel.h"
#include ".\kg3dmodeltable.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DEditGridBase::KG3DEditGridBase(void)
{
}

KG3DEditGridBase::~KG3DEditGridBase(void)
{
}

HRESULT KG3DEditGridBase::UnInit()
{
	for(size_t i =0;i<m_vecGrid.size();i++)
	{
		SAFE_RELEASE(m_vecGrid[i].pModel);
	}
	m_vecGrid.clear();
	return S_OK;
}

HRESULT KG3DEditGridBase::CheckState(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	D3DXMATRIX matSave;
	g_pd3dDevice->GetTransform( D3DTS_WORLD, &matSave );
	UpdateTransformation();
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &m_Matrix );

	D3DXVECTOR3 Inter(0,0,0);
	float Distance = 0;
	m_dwState = 0xFFFFFFFF;
	for(size_t i =0;i<m_vecGrid.size();i++)
	{
		if(m_vecGrid[i].pModel)
		{
			if(m_vecGrid[i].pModel->IsRayIntersect(Inter,Origin,Direction))
			{
				D3DXVECTOR3 V = Inter - Origin;
				float Len = D3DXVec3Length(&V);
				if(Len>Distance)//选取最远的
				{
					Distance =  Len;
					m_dwState = m_vecGrid[i].dwState;
				}
			}
		}
	}

	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matSave );

	if(Distance>0)
		return S_OK;
	else 
		return E_FAIL;
}

HRESULT KG3DEditGridBase::Render()
{
	UpdateTransformation();

	for(size_t i = 0; i < m_vecGrid.size(); i++)
	{
		KG3DModel* pCurModel = NULL;
		pCurModel = m_vecGrid[i].pModel;
		
		if (!pCurModel)
			continue;

		pCurModel->SetParentObjMatrix(&m_Matrix);
		//KG3DEdtirGridBase has no FrameMove(), so call it here
		pCurModel->FrameMove();

		KG3DRuntimeMaterial *pRuntimeMaterial = pCurModel->GetRuntimeMaterial();
		
		if(m_dwState == m_vecGrid[i].dwState)
		{
			
			pCurModel->SetAlpha(1.0f, FALSE);
			pRuntimeMaterial->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			pRuntimeMaterial->Enable(TRUE);
			pCurModel->Render(0, NULL);
		}
		else
		{
			pCurModel->SetAlpha(0.3f, FALSE);
			pRuntimeMaterial->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			pRuntimeMaterial->Enable(TRUE);
			pCurModel->Render(0, NULL);
		}
	}
	return S_OK;
}

HRESULT KG3DEditGridBase::OpenGridModel(LPSTR pFileName, DWORD dwState)
{
	KG3DModel* pModel = NULL;
	if (SUCCEEDED(g_cModelTable.LoadResourceFromFile(pFileName,
		0, 
		0, 
		(IKG3DResourceBase **)&pModel)))
	{
		pModel->SetScaling(&D3DXVECTOR3(3,3,3));
		m_vecGrid.push_back(_Grid(pModel,dwState));
	}	
	else
    {
		m_vecGrid.push_back(_Grid(NULL,dwState));
    }
	return S_OK;
}

