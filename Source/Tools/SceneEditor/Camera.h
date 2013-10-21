// Camera.h: interface for the GraphicsStruct::KCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERA_H__9DB96BAF_AB42_4ED7_A72E_FC96F82BFE16__INCLUDED_)
#define AFX_CAMERA_H__9DB96BAF_AB42_4ED7_A72E_FC96F82BFE16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
namespace GraphicsStruct
{

class KCamera  
{
public:
	HRESULT SetCamera();
	GraphicsStruct::KCamera();
	virtual ~KCamera();
	
	D3DXVECTOR3             Position;
	D3DXVECTOR3             ViewPoint;
	D3DXVECTOR3             DefUp;
	float                   Field;
	float                   Aspect;
	float                   zNear;
	float                   zFar;

	BOOL                    m_bPerspect;
};
}

#endif // !defined(AFX_CAMERA_H__9DB96BAF_AB42_4ED7_A72E_FC96F82BFE16__INCLUDED_)
