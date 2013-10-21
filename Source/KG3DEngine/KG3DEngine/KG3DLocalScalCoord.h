////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3ScalCoordinate.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-11-22 14:46:14
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DLOCALSCALCOORD_H_
#define _INCLUDE_KG3DLOCALSCALCOORD_H_

#include "KG3DSceneEntity.h"

#define SCAL_COORD_MESH_PATH TEXT("\\Data\\editor\\缩放\\local_scal_coord.x")
class KG3DScene;
class KG3DScalCoordinate
{
//private :
//    KG3DScalCoordinate();
public :
	KG3DScalCoordinate(KG3DScene& scene);
    virtual ~KG3DScalCoordinate();

    //static KG3DScalCoordinate* GetCoordinate();

    HRESULT FrameMove();
    HRESULT Render();
    //HRESULT AttachEntity(KG3DScene* pScene, std::list<KG3DSceneEntity> listEntitys);
    //HRESULT AttachEntity(KG3DScene* pScene, KG3DSceneEntity Entity);
    //HRESULT ReleaseAttach();
    HRESULT ScalBegin();
    HRESULT ScalEnd();
    HRESULT SetMode(DWORD dwMode);
    HRESULT Zoom(FLOAT fScanl);
    float   GetScalingFactor();

    //const EntityList& GetEntityList() { return m_EntityList; }
#if defined(DEBUG) | defined(_DEBUG)
	VOID	TestSelectCoord(DWORD dwCoord){m_dwCurrSelCoord = dwCoord;}
#endif

private :
    KG3DScene*      m_pAttachScene;
    //EntityList      m_EntityList;
    LPD3DXMESH      m_pCoordMesh;
    DWORD           m_dwNumMatrials;
    KG3DCOORD           m_dwMode;

    DWORD           m_dwCurrSelCoord;

    D3DXPLANE       m_currSelPane;
    D3DXVECTOR3     m_vPrevCross;
    D3DXMATRIX      m_matCoord;    
    D3DXMATRIX      m_matCoordPane;

    FLOAT           m_fZoom;

public :
    INT             m_nMoveFlag;
};

#include "KG3DBaseCoord.h"
////////////////////////////////////////////////////////////////////////////////
//struct KG3DScaleCoordinate : public KG3DBaseCoord
//{
//	virtual	ULONG	GetCurState() = 0;
//	virtual HRESULT SetPosition(const D3DXVECTOR3& Pos) = 0;
//	virtual HRESULT	Zoom(FLOAT zoomFactor) = 0;//zoomFactor会加到Scale系数里面，而不是乘
//	virtual D3DXVECTOR3 GetTranslation() = 0;
//	virtual ~KG3DScaleCoordinate() = 0{}
//};
KG3DBaseCoord* BuildKG3DScaleCoordinateSolid(LPCTSTR, DWORD_PTR pKG3DMessageBroadcasterGroup, DWORD_PTR);

#endif //_INCLUDE_KG3DLOCALSCALCOORD_H_
