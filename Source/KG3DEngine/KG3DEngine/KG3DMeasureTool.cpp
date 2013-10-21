#include "StdAfx.h"
#include "KG3DMeasureTool.h"
#include "KG3DGraphicsTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DMeasureTool::KG3DMeasureTool(void)
{
	m_eType = MEASEURE_POINT;
	ZeroMemory(m_vPositions,sizeof(D3DXVECTOR3)*8);
}

KG3DMeasureTool::~KG3DMeasureTool(void)
{
}

HRESULT KG3DMeasureTool::CreateTwoPointLine(D3DXVECTOR3& vA,D3DXVECTOR3& vB)
{
	m_vPositions[0] = vA;
	m_vPositions[1] = vB;
	return S_OK;
}

HRESULT KG3DMeasureTool::Render(KG3DCamera* pCamera,D3DVIEWPORT9& viewport)
{
	GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

	D3DXMATRIX matView = pCamera->GetViewMatrix();
	D3DXMATRIX matProj = pCamera->GetProjectionMatrix();
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	D3DXVECTOR3 V = m_vPositions[1] - m_vPositions[0];
	float fLen = D3DXVec3Length(&V);
	D3DXVECTOR3 A_Dir,B_Dir;
	A_Dir.x = matView._11;
	A_Dir.y = matView._21;
	A_Dir.z = matView._31;
	B_Dir = D3DXVECTOR3(0,1,0);

	g_cGraphicsTool.DrawLine(&m_vPositions[0],&m_vPositions[1],0xFF00FF00,0xFF00FF00);

	D3DXVECTOR3 Center = (m_vPositions[0] + m_vPositions[1]) * 0.5F;
	TCHAR Name[256];
	sprintf(Name,"%.2f",fLen);

	D3DXVECTOR3 vScreen;
	D3DXVec3Project(&vScreen,&Center,&viewport,&matProj,&matView,&matWorld);

	RECT destRect1;
	SetRect(&destRect1, (int)vScreen.x, (int)vScreen.y, 0, 80);

	LPD3DXFONT pFont = g_cGraphicsTool.GetFont();
	if (pFont)
	{
		pFont->DrawTextA(NULL, Name, -1, &destRect1, 
				DT_NOCLIP, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	}
	return S_OK;
}
