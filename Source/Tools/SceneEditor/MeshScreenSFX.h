// MeshScreenSFX.h: interface for the KModelSFXScreen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHSCREENSFX_H__91343032_E6B2_4380_B953_EB24E83AB735__INCLUDED_)
#define AFX_MESHSCREENSFX_H__91343032_E6B2_4380_B953_EB24E83AB735__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"
namespace ModelSFX
{
class KModelSFXScreen : public KModel  
{
public:
	HRESULT Render();
	HRESULT LoadMesh(LPSTR pFileName);
	KModelSFXScreen();
	virtual ~KModelSFXScreen();

};
};

#endif // !defined(AFX_MESHSCREENSFX_H__91343032_E6B2_4380_B953_EB24E83AB735__INCLUDED_)
