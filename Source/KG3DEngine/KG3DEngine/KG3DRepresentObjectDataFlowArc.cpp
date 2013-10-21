#include "StdAfx.h"
#include "KG3DRepresentObjectDataFlowArc.h"
#include "KG3DSceneDataFlowEditor.h"
#include "KG3DDataFlowArc.h"
#include "KG3DRepresentObjectDataFlowProcessor.h"
#include "KG3DGraphicsTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DRepresentObjectDataFlowArc::KG3DRepresentObjectDataFlowArc(void)
{
	m_lpArc = NULL;
}

KG3DRepresentObjectDataFlowArc::~KG3DRepresentObjectDataFlowArc(void)
{
	m_lpArc = NULL;
}

HRESULT KG3DRepresentObjectDataFlowArc::Render(DWORD dwOption)
{
	/*for (size_t i=0;i<m_vecInputNode.size();i++)
	{
		D3DXVECTOR3 V = m_vecInputNode[i];

		g_cGraphicsTool.draw
	}*/
	DWORD color = 0xA0FF8080;
	if(m_lpArc->IsDataReady())
		color = 0xFFFFFFFF;

	g_cGraphicsTool.DrawLine(&m_vCenterA,&m_vCenterB,color,color);
	return S_OK;
}

HRESULT KG3DRepresentObjectDataFlowArc::Refresh(KG3DSceneDataFlowEditor* pEditor)
{
	HRESULT hr = S_OK;
	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwNum = 0;
		m_lpArc->GetNumInputConnection(&dwNum);
		m_vecInputNode.resize(dwNum);

		for (DWORD i=0;i<dwNum;i++)
		{
			if(m_lpArc->m_vecInputConnection[i].uHandleProcessor)
			{
				KG3DRepresentObjectDataFlowProcessor* pRepProcessor = NULL;
				hr = pEditor->FindRepProcessor(&pRepProcessor,m_lpArc->m_vecInputConnection[i].uHandleProcessor);
				if(SUCCEEDED(hr))
				{
					D3DXVECTOR3 vPos = m_Translation;
					D3DXVECTOR3 vPosB;
					pRepProcessor->GetOutputConnectionPoint(m_lpArc->m_vecInputConnection[i].dwIndex,vPos,vPosB);
					m_vecInputNode[i] = vPos;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwNum = 0;
		m_lpArc->GetNumOutputConnection(&dwNum);
		m_vecOutputNode.resize(dwNum);

		for (DWORD i=0;i<dwNum;i++)
		{
			if(m_lpArc->m_vecOutputConnection[i].uHandleProcessor)
			{
				KG3DRepresentObjectDataFlowProcessor* pRepProcessor = NULL;
				hr = pEditor->FindRepProcessor(&pRepProcessor,m_lpArc->m_vecOutputConnection[i].uHandleProcessor);
				if(SUCCEEDED(hr))
				{
					D3DXVECTOR3 vPos = m_Translation;
					D3DXVECTOR3 vPosB;

					pRepProcessor->GetInputConnectionPoint(m_lpArc->m_vecOutputConnection[i].dwIndex,vPos,vPosB);
					m_vecOutputNode[i] = vPos;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		D3DXVECTOR3 V(0,0,0);
		for (size_t i=0;i<m_vecInputNode.size();i++)
		{
			V += m_vecInputNode[i];
		}

		if(m_vecInputNode.size()>0)
			V /= m_vecInputNode.size() * 1.0F;
		else
			V = m_Translation;

		m_vCenterA = V;
	}

	{
		D3DXVECTOR3 V(0,0,0);
		for (size_t i=0;i<m_vecOutputNode.size();i++)
		{
			V += m_vecOutputNode[i];
		}

		if(m_vecOutputNode.size()>0)
			V /= m_vecOutputNode.size() * 1.0F;
		else
			V = m_Translation;

		m_vCenterB = V;
	}

	return S_OK;
}


