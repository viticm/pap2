// MeshStrip.h: interface for the KModelStrip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHSTRIP_H__4AC419FF_D370_4DCB_92C7_CD5A0D6976BF__INCLUDED_)
#define AFX_MESHSTRIP_H__4AC419FF_D370_4DCB_92C7_CD5A0D6976BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"

class KModelStrip : public KModel  
{
public:
	typedef struct _Node 
	{
		D3DXVECTOR3  Position;
		D3DXVECTOR3  R_Position;
		D3DXVECTOR3  Speed;
		D3DXVECTOR3  Plane;
		D3DXVECTOR3  YPR;
		D3DXVECTOR3  R_YPR;
	}* LPNODE,NODE;
public:
	HRESULT UnBind();
	HRESULT Bind(LPMODEL pXMesh,LPSTR SocketName);
	HRESULT PlaneShadowRender();
	HRESULT Render();
	HRESULT UpdateStrip();
	LPNODE  m_lpStripNode;
	DWORD   m_NumNode;
	DWORD   m_fNodeLength;
	
	BOOL    m_bBind;

	HRESULT LoadMesh(LPSTR pFileName);
	KModelStrip();
	virtual ~KModelStrip();

};

#endif // !defined(AFX_MESHSTRIP_H__4AC419FF_D370_4DCB_92C7_CD5A0D6976BF__INCLUDED_)
