// MeshParticleSystem.h: interface for the KModelSFXParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHPARTICLESYSTEM_H__70CD76AE_D6CC_43E5_B636_BBBE15EA55B2__INCLUDED_)
#define AFX_MESHPARTICLESYSTEM_H__70CD76AE_D6CC_43E5_B636_BBBE15EA55B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"
#include "types.h"
#include "ParticleSystem.h"
#include "GraphicTool.h"	// Added by ClassView
#include "MeshBillBoard.h"
#include "String.h"

class KDlg_Emitter_Att;
namespace PropertyBase
{
	class KDlg_PS_Main;
}
namespace ModelSFX
{
class KModelSFXParticleSystem : public ModelSFX::KModelSFXBillBoardBase, public ModelSFX::KParticleSystem
{
	friend class KDlg_Emitter_Att;
	friend class PropertyBase::KDlg_PS_Main;
private:
	CString m_strMtlName;
	BOOL    m_bDepthSort;
	ParticleRenderType m_enumRenderType;
	//int m_iParticleCount;
public:
	HRESULT UpdateVertices();
	virtual HRESULT SortSprites();
	void DrawForceField();
	void DrawBarrier();
	void DrawLauncher();
	virtual HRESULT FrameMove();
	virtual HRESULT LoadMesh(LPSTR pFileName);
	virtual HRESULT Render();
	KModelSFXParticleSystem();
	virtual ~KModelSFXParticleSystem();


	virtual HRESULT AddLauncher(ModelSFX::KLauncher* pLan);
	virtual HRESULT Acquire(void);
};
};

#endif // !defined(AFX_MESHPARTICLESYSTEM_H__70CD76AE_D6CC_43E5_B636_BBBE15EA55B2__INCLUDED_)
