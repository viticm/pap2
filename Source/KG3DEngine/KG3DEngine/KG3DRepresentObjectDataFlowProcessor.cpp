#include "StdAfx.h"
#include "KG3DRepresentObjectDataFlowProcessor.h"
#include "KG3DDataFlowProcessor.h"
#include "KG3DGraphicsTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DRepresentObjectDataFlowProcessor::KG3DRepresentObjectDataFlowProcessor(void)
{
	m_lpProcessor = NULL;
	m_eShapeType = SHAPETYPE_ROUNDRECT;
	m_fSize = 100;
}

KG3DRepresentObjectDataFlowProcessor::~KG3DRepresentObjectDataFlowProcessor(void)
{
	SAFE_RELEASE(m_lpProcessor);
}



HRESULT KG3DRepresentObjectDataFlowProcessor::Render(DWORD dwOption)
{
	switch(m_eShapeType)
	{
	case SHAPETYPE_ROUNDRECT:
		RenderShapeRoundRect();
		break;
	case SHAPETYPE_TRIANGLE:
		RenderShapeTriangle();
		break;
	case SHAPETYPE_CIRCLE:
		RenderShapeCircle();
		break;
	case SHAPETYPE_DATAMODIFY:
		RenderShapeDataModify();
		break;
	}
	//////////////////////////////////////////////////////////////////////////
	{
		DWORD nCount = m_lpProcessor->GetNumInputNode();
		for (DWORD i=0;i<nCount;i++)
		{
			D3DXVECTOR3 A,B;
			GetInputConnectionPoint(i,A,B);
			RenderShapeConnectionPoint(A,B,0xFF00FF00);
		}
	
		nCount = m_lpProcessor->GetNumOutPutNode();
		for (DWORD i=0;i<nCount;i++)
		{
			D3DXVECTOR3 A,B;
			GetOutputConnectionPoint(i,A,B);
			RenderShapeConnectionPoint(A,B,0xFFFF0000);
		}
	}

	return S_OK;
}

HRESULT KG3DRepresentObjectDataFlowProcessor::RenderShapeRoundRect()
{
	float R = m_fSize;
	D3DXVECTOR3 H(m_fSize,0,0);
	D3DXVECTOR3 A = m_Translation - H;
	D3DXVECTOR3 B = m_Translation + H;
	D3DXVECTOR3 RR(0,0,R);

	g_cGraphicsTool.DrawArc(A,R,D3DX_PI*0.5F,D3DX_PI*1.5F,0,0xFFFFFFFF,36);
	g_cGraphicsTool.DrawArc(B,R,D3DX_PI*-0.5F,D3DX_PI*0.5F,0,0xFFFFFFFF,36);

	g_cGraphicsTool.DrawLine(&(A+RR),&(B+RR),0xFFFFFFFF,0xFFFFFFFF);
	g_cGraphicsTool.DrawLine(&(A-RR),&(B-RR),0xFFFFFFFF,0xFFFFFFFF);

	return S_OK;
}

HRESULT KG3DRepresentObjectDataFlowProcessor::RenderShapeTriangle()
{
	D3DXVECTOR3 A = m_Translation - D3DXVECTOR3(m_fSize,0,m_fSize*2);
	D3DXVECTOR3 B = m_Translation - D3DXVECTOR3(m_fSize,0,-m_fSize*2);
	D3DXVECTOR3 C = m_Translation + D3DXVECTOR3(m_fSize*2,0,0);
	g_cGraphicsTool.DrawLine(&A,&B,0xFFFFFFFF,0xFFFFFFFF);
	g_cGraphicsTool.DrawLine(&B,&C,0xFFFFFFFF,0xFFFFFFFF);
	g_cGraphicsTool.DrawLine(&C,&A,0xFFFFFFFF,0xFFFFFFFF);

	return S_OK;
}

HRESULT KG3DRepresentObjectDataFlowProcessor::RenderShapeDataModify()
{
	DWORD dwCount = m_lpProcessor->GetNumInputNode();

	float H = m_fSize * 0.2F;
	float K = H * dwCount * 0.5F ;

	for (DWORD i=0;i<dwCount;i++)
	{
		D3DXVECTOR3 A = m_Translation + D3DXVECTOR3(-m_fSize,0,H*i - K);
		D3DXVECTOR3 B = m_Translation + D3DXVECTOR3( m_fSize,0,H*i - K);
		D3DXVECTOR3 C = m_Translation + D3DXVECTOR3( m_fSize,0,H*(i+1) - K);
		D3DXVECTOR3 D = m_Translation + D3DXVECTOR3(-m_fSize,0,H*(i+1) - K);
		g_cGraphicsTool.DrawLine(&A,&B,0xFFFFFFFF,0xFFFFFFFF);
		g_cGraphicsTool.DrawLine(&B,&C,0xFFFFFFFF,0xFFFFFFFF);
		g_cGraphicsTool.DrawLine(&C,&D,0xFFFFFFFF,0xFFFFFFFF);
		g_cGraphicsTool.DrawLine(&D,&A,0xFFFFFFFF,0xFFFFFFFF);
	}
	return S_OK;
}


HRESULT KG3DRepresentObjectDataFlowProcessor::RenderShapeCircle()
{
	float R = m_fSize;
	D3DXVECTOR3 A = m_Translation;

	g_cGraphicsTool.DrawArc(A,R,0,D3DX_PI*2,0,0xFFFFFFFF,36);
	g_cGraphicsTool.DrawArc(A,R*0.8F,0,D3DX_PI*2,0,0xFFFFFFFF,36);

	return S_OK;
}

HRESULT KG3DRepresentObjectDataFlowProcessor::RenderShapeConnectionPoint(D3DXVECTOR3 A,D3DXVECTOR3 B,DWORD color)
{
	float R = m_fSize*0.1F;
	g_cGraphicsTool.DrawArc(A,R,0,D3DX_PI*2,0,color,16);
	g_cGraphicsTool.DrawLine(&A,&B,color,0xFFFFFFFF);
	return S_OK;
}

HRESULT KG3DRepresentObjectDataFlowProcessor::SetProcessor(KG3DDataFlowProcessor* pProcessor)
{
	m_lpProcessor = pProcessor;
	if(m_lpProcessor)
	{
		switch(m_lpProcessor->GetProcessType())
		{
		case DATA_PROCESSOR_NONE:
			break;
		case DATA_PROCESSOR_DATACREATER:
			m_eShapeType = SHAPETYPE_CIRCLE;
			break;
		case DATA_PROCESSOR_DATADESTROYER:
			break;
		case DATA_PROCESSOR_DATAMODIFY:
			m_eShapeType = SHAPETYPE_DATAMODIFY;
			break;
		case DATA_PROCESSOR_ALGORITHM:
			break;
		case DATA_PROCESSOR_SYSTEMINPUT:
			m_eShapeType = SHAPETYPE_CIRCLE;
			break;
		case DATA_PROCESSOR_SYSTEMOUTPUT:
			m_eShapeType = SHAPETYPE_CIRCLE;
			break;
		default:
			break;
		}
	}
	return S_OK;
}

HRESULT KG3DRepresentObjectDataFlowProcessor::GetInputConnectionPoint(DWORD dwIndex,D3DXVECTOR3& vConnectionPos,D3DXVECTOR3& vStartPos)
{
	KGLOG_PROCESS_ERROR( dwIndex < m_lpProcessor->GetNumInputNode());

	switch(m_eShapeType)
	{
	case SHAPETYPE_ROUNDRECT:
		{
			DWORD nCount = m_lpProcessor->GetNumInputNode();

			float R = m_fSize;
			float R2 = R * 1.5F;
			D3DXVECTOR3 H(m_fSize,0,0);
			D3DXVECTOR3 C = m_Translation - H;

			float fA_Start = D3DX_PI*0.5F;
			float fA_End = D3DX_PI*1.5F;

			DWORD nPart = nCount + 1;
			float K = (fA_End - fA_Start) / nPart;

			float fAngle = fA_Start + (dwIndex + 1) * K;

			D3DXVECTOR3 VR = D3DXVECTOR3(cosf(fAngle),0,sinf(fAngle));

			D3DXVECTOR3 A = C + VR * R2;
			vConnectionPos = A;
			vStartPos = C + VR * R;

		}
		break;
	case SHAPETYPE_TRIANGLE:
		{
			DWORD nCount = m_lpProcessor->GetNumInputNode();

			D3DXVECTOR3 TA = m_Translation - D3DXVECTOR3(m_fSize,0,m_fSize*2);
			D3DXVECTOR3 TB = m_Translation - D3DXVECTOR3(m_fSize,0,m_fSize*-2);


			DWORD nPart = nCount + 1;
			D3DXVECTOR3 K = (TB - TA) / (float)nPart;	

			D3DXVECTOR3 B = TA + (float)(dwIndex+1) * K;

			D3DXVECTOR3 A = B + D3DXVECTOR3(m_fSize*-0.5F,0,0);

			vConnectionPos = A;
			vStartPos = B;

		}
		break;
	case SHAPETYPE_CIRCLE:
		{
			DWORD nCount = m_lpProcessor->GetNumInputNode();

			float R = m_fSize;
			float R2 = R * 1.5F;
			D3DXVECTOR3 H(m_fSize,0,0);
			D3DXVECTOR3 C = m_Translation;

			float fA_Start = D3DX_PI*0.5F;
			float fA_End = D3DX_PI*1.5F;

			DWORD nPart = nCount + 1;
			float K = (fA_End - fA_Start) / nPart;
			
			float fAngle = fA_Start + (dwIndex+1) * K;

			D3DXVECTOR3 VR = D3DXVECTOR3(cosf(fAngle),0,sinf(fAngle));

			D3DXVECTOR3 A = C + VR * R2;
			D3DXVECTOR3 B = C + VR * R;

			vConnectionPos = A;
			vStartPos = B;

		}
		break;
	case SHAPETYPE_DATAMODIFY:
		{
			DWORD nCount = m_lpProcessor->GetNumInputNode();

			float H = m_fSize * 0.2F;
			float K = H * nCount * 0.5F ;
			D3DXVECTOR3 C = m_Translation + D3DXVECTOR3(0,0,K - dwIndex * H - H *0.5F);

			vConnectionPos = C - D3DXVECTOR3(m_fSize*2,0,0) ;
			vStartPos = C - D3DXVECTOR3(m_fSize,0,0);

			break;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectDataFlowProcessor::GetOutputConnectionPoint(DWORD dwIndex,D3DXVECTOR3& vConnectionPos,D3DXVECTOR3& vStartPos)
{
	KGLOG_PROCESS_ERROR( dwIndex < m_lpProcessor->GetNumOutPutNode());

	switch(m_eShapeType)
	{
	case SHAPETYPE_ROUNDRECT:
		{
			DWORD nCount = m_lpProcessor->GetNumOutPutNode();

			float R = m_fSize;
			float R2 = R * 1.5F;
			D3DXVECTOR3 H(m_fSize,0,0);
			D3DXVECTOR3 C = m_Translation + H;

			float fA_Start = -D3DX_PI*0.5F;
			float fA_End = D3DX_PI*0.5F;

			DWORD nPart = nCount + 1;
			float K = (fA_End - fA_Start) / nPart;

			float fAngle = fA_Start + (dwIndex+1) * K;
			D3DXVECTOR3 VR = D3DXVECTOR3(cosf(fAngle),0,sinf(fAngle));

			D3DXVECTOR3 A = C + VR * R2;
			D3DXVECTOR3 B = C + VR * R;

			vConnectionPos = A;
			vStartPos = B;
		}
		break;
	case SHAPETYPE_TRIANGLE:
		{
			DWORD nCount = m_lpProcessor->GetNumOutPutNode();

			float R = m_fSize*0.5F;
			D3DXVECTOR3 H(100,0,0);
			D3DXVECTOR3 C = m_Translation + D3DXVECTOR3(m_fSize*2,0,0);

			float fA_Start = -D3DX_PI*0.5F;
			float fA_End = D3DX_PI*0.5F;

			DWORD nPart = nCount + 1;
			float K = (fA_End - fA_Start) / nPart;

				float fAngle = fA_Start + (dwIndex+1) * K;

				D3DXVECTOR3 VR = D3DXVECTOR3(cosf(fAngle),0,sinf(fAngle));

				D3DXVECTOR3 A = C + VR * R;
				D3DXVECTOR3 B = C;

			vConnectionPos = A;
			vStartPos = B;
		}
		break;
	case SHAPETYPE_CIRCLE:
		{
			DWORD nCount = m_lpProcessor->GetNumOutPutNode();

			float R = m_fSize;
			float R2 = R * 1.5F;
			D3DXVECTOR3 H(m_fSize,0,0);
			D3DXVECTOR3 C = m_Translation;

			float fA_Start = -D3DX_PI*0.5F;
			float fA_End = D3DX_PI*0.5F;

			DWORD nPart = nCount + 1;
			float K = (fA_End - fA_Start) / nPart;

			float fAngle = fA_Start + (dwIndex+1) * K;

			D3DXVECTOR3 VR = D3DXVECTOR3(cosf(fAngle),0,sinf(fAngle));

			D3DXVECTOR3 A = C + VR * R2;
			D3DXVECTOR3 B = C + VR * R;

			vConnectionPos = A;
			vStartPos = B;
		}
		break;
	case SHAPETYPE_DATAMODIFY:
		{
			DWORD nCount = m_lpProcessor->GetNumInputNode();

			float H = m_fSize * 0.2F;
			float K = H * nCount * 0.5F ;
			D3DXVECTOR3 C = m_Translation + D3DXVECTOR3(0,0,K - dwIndex * H - H *0.5F);

			vConnectionPos = C + D3DXVECTOR3(m_fSize*2,0,0) ;
			vStartPos = C + D3DXVECTOR3(m_fSize,0,0);

			break;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectDataFlowProcessor::ComputeBBox()
{
	m_BBox.Clear();

	m_BBox.AddPosition(m_Translation - D3DXVECTOR3(m_fSize,m_fSize,m_fSize));
	m_BBox.AddPosition(m_Translation + D3DXVECTOR3(m_fSize,m_fSize,m_fSize));

	DWORD nCount = m_lpProcessor->GetNumInputNode();
	for (DWORD i=0;i<nCount;i++)
	{
		D3DXVECTOR3 A,B;
		GetInputConnectionPoint(i,A,B);
		m_BBox.AddPosition(A);
	}

	nCount = m_lpProcessor->GetNumOutPutNode();
	for (DWORD i=0;i<nCount;i++)
	{
		D3DXVECTOR3 A,B;
		GetOutputConnectionPoint(i,A,B);
		m_BBox.AddPosition(B);
	}
	return S_OK;
}

HRESULT KG3DRepresentObjectDataFlowProcessor::RenderSelectMark(DWORD dwColor /* = 0xFFFF0000 */)
{
	g_cGraphicsTool.DrawAABBox(&m_BBox,0xFFFF0000);
	return S_OK;
}

HRESULT KG3DRepresentObjectDataFlowProcessor::Refresh(KG3DSceneDataFlowEditor* pEditor)
{
	if(m_lpProcessor)
	{
		m_lpProcessor->Refresh(pEditor);
	}
	return S_OK;
}
