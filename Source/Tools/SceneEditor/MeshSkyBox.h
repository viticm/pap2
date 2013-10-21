// MeshSkyBox.h: interface for the KModelSFXSkyBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHSKYBOX_H__33C89912_B998_4646_BA77_31AB684D7265__INCLUDED_)
#define AFX_MESHSKYBOX_H__33C89912_B998_4646_BA77_31AB684D7265__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"
namespace ModelSFX
{
class KModelSFXSkyBox : public KModel  
{
public:
	HRESULT GetSkyTexture(LPTEXTURE* ppTexture);

	HRESULT Render();
	LPTEXTURE m_Cubemap;
	
	HRESULT LoadMesh(LPSTR pFileName);
	KModelSFXSkyBox();
	virtual ~KModelSFXSkyBox();

};
};

#endif // !defined(AFX_MESHSKYBOX_H__33C89912_B998_4646_BA77_31AB684D7265__INCLUDED_)
