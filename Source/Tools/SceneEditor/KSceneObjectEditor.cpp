// KSceneObjectEditor.cpp: implementation of the KSceneObjectEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneObjectEditor.h"
#include "KSceneObjectEditorWizard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KSceneObjectEditor::KSceneObjectEditor()
{
	m_bShowObject = TRUE;
	m_bShowHeight_Up = FALSE;
	m_bShowHeight_Down = TRUE;
	m_bShowBarrier = FALSE;

	m_lpObject = NULL;
	m_bShowPart = TRUE;

	m_dwBarrier = 0;
	m_fHeightValue = 10;
}

KSceneObjectEditor::~KSceneObjectEditor()
{

}

HRESULT KSceneObjectEditor::Render()
{
	g_cGraphicsTool.DrawGroundGrid(50);
	
	if(m_lpObject)
	{
		if(m_bShowObject)
			SAFE_RENDER(m_lpObject);

		m_lpObject->DrawBBox();

		//////////////////////////////////////////////////////////////////////////
		KGraphicsEngine* pEng = &g_GraphiceEngine;

		D3DXVECTOR3 Orig,Dir;
		pEng->GetPickRay(&Orig,&Dir);

		m_lpObject->SetPickLine(&Orig,&Dir);
		
		
		//////////////////////////////////////////////////////////////////////////
		if(m_bShowHeight_Down)
			m_lpObject->RenderCellInfoHeightDown();

		if(m_bShowBarrier)
			m_lpObject->RenderCellInfoBarrier(m_dwBarrier);
		//m_lpObject->RenderCellInfoHeightUp();
	}

	KSceneEditBase::Render();

	return S_OK;
}

HRESULT KSceneObjectEditor::OnNewObject()
{
	KSceneObjectEditorWizard Dlg;
	Dlg.DoModal();


	g_cObjectTable.Get1NewObject(&m_lpObject,OBJECT_DEFAULT);

	return S_OK;
}

HRESULT KSceneObjectEditor::FrameMove()
{
	if(m_lpObject)
	{
		m_lpObject->FrameMove();
	}

	return S_OK;
}

HRESULT KSceneObjectEditor::OnEditMoving()
{
	m_lpObject->ComputeBBox();
	return S_OK;
}

HRESULT KSceneObjectEditor::OnEditRotating()
{
	m_lpObject->ComputeBBox();
	return S_OK;
}

HRESULT KSceneObjectEditor::OnEditScaling()
{
	m_lpObject->ComputeBBox();
	return S_OK;
}

void KSceneObjectEditor::ONMOUSEMOVE(WPARAM wParam, LPARAM lParam)
{
	KSceneEditBase::ONMOUSEMOVE(wParam,lParam);

	int fwKeys = wParam; 
	int xPos = LOWORD(lParam); 
	int yPos = HIWORD(lParam);

	if (fwKeys&MK_LBUTTON)
	{
		if(EDITSTATE_OBJECTEDIT == m_dwState)
		{		
			switch(m_dwSubState)
			{
			case BARRIER_OPERATOR_ADD:
			case BARRIER_OPERATOR_REMOVE:
			case BARRIER_OPERATOR_SET:
				{
					if(m_lpObject)
					{
						m_lpObject->SetCellBarrier(m_dwSubState,m_dwBarrier);
					}	
					break;
				}
			case HEIGHTDOWN_UP:
			case HEIGHTDOWN_DOWN:
			case HEIGHTDOWN_SET:
				{
					if(m_lpObject)
					{
						m_lpObject->SetGridHeightDown(m_dwSubState,m_fHeightValue);
					}	
					break;
				}
			}
		}
	}
}

void KSceneObjectEditor::ONLBUTTONDOWN(WPARAM wParam, LPARAM lParam)
{
	KSceneEditBase::ONLBUTTONDOWN(wParam,lParam);

	int fwKeys = wParam; 
	int xPos = LOWORD(lParam); 
	int yPos = HIWORD(lParam);

	if (fwKeys&MK_LBUTTON)
	{
		if(EDITSTATE_OBJECTEDIT == m_dwState)
		{		
			switch(m_dwSubState)
			{
			case BARRIER_OPERATOR_ADD:
			case BARRIER_OPERATOR_REMOVE:
			case BARRIER_OPERATOR_SET:
				{
					if(m_lpObject)
					{
						m_lpObject->SetCellBarrier(m_dwSubState,m_dwBarrier);
					}	
					break;
				}
			case HEIGHTDOWN_UP:
			case HEIGHTDOWN_DOWN:
			case HEIGHTDOWN_SET:
				{
					if(m_lpObject)
					{
						m_lpObject->SetGridHeightDown(m_dwSubState,m_fHeightValue);
					}	
					break;
				}
			}
		}
	}
}

HRESULT KSceneObjectEditor::LoadbjectFromFile(LPSTR pFileName)
{
	FILE* pFile = fopen(pFileName,"rb");
	if(!pFile)
		return E_FAIL;

	KComponentObject::_ObjectFileHeaderBase Header;
	fread(&Header,sizeof(KComponentObject::_ObjectFileHeaderBase),1,pFile);

	if(FAILED(g_cObjectTable.Get1NewObject(&m_lpObject,Header.Type)))
	{
		fclose(pFile);
		return E_FAIL;
	}

	fclose(pFile);

	if(FAILED(m_lpObject->LoadFromFile(pFileName)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void KSceneObjectEditor::ONKEYDOWN(WPARAM wParam, LPARAM lParam)
{
	KSceneEditBase::ONKEYDOWN(wParam,lParam);
	
	switch(wParam)
	{
	case 219://[
		{
			if(m_lpObject)
			{
				if(m_lpObject->m_fBrushSize>10)
					m_lpObject->m_fBrushSize -= 10;
			}

			break;
		}
	case 221://]
		{
			if(m_lpObject)
			{
				if(m_lpObject->m_fBrushSize<500)
					m_lpObject->m_fBrushSize += 10;
			}
			break;
		}
	}
}

