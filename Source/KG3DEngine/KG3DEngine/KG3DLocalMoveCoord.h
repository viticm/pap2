////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMoveCoordinate.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-11-7 19:13:23
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DLOCALMOVECOORD_H_
#define _INCLUDE_KG3DLOCALMOVECOORD_H_

#include "KG3DSceneEntity.h"

#define MOVE_COORD_MESH_PATH TEXT("\\Data\\editor\\ÒÆ¶¯\\local_move_coord.x")
class KG3DScene;
class KG3DMoveCoordinate
{
public :
	KG3DMoveCoordinate(KG3DScene& scene);
    virtual ~KG3DMoveCoordinate();

    //static KG3DMoveCoordinate* GetCoordinate();

    HRESULT FrameMove();
    HRESULT Render();
    //HRESULT AttachEntity(KG3DScene* pScene, std::list<KG3DSceneEntity> listEntitys);
    //HRESULT AttachEntity(KG3DScene* pScene, KG3DSceneEntity Entity);
    //HRESULT ReleaseAttach();
    HRESULT MoveBegin();
    HRESULT MoveEnd();
    HRESULT SetMode(DWORD dwMode);
    float   GetScalingFactor();

    HRESULT Zoom(FLOAT fScanl);

    //const EntityList& GetEntityList() { return m_EntityList; }

#if defined(DEBUG) | defined(_DEBUG)
	VOID	TestSelectCoord(DWORD dwCoord){m_dwCurrSelCoord = dwCoord;}
#endif
private :
    KG3DScene*      m_pAttachScene;
    //EntityList      m_EntityList;
    LPD3DXMESH      m_pCoordMesh;
    DWORD           m_dwNumMatrials;
    DWORD           m_dwMode;

    DWORD           m_dwCurrSelCoord;

    D3DXPLANE       m_currSelPane;
    D3DXVECTOR3     m_vPrevCross;
    D3DXMATRIX      m_matCoord;    
    D3DXMATRIX      m_matCoordPane;

    FLOAT           m_fZoom;

public :
    INT             m_nMoveFlag;
};


#endif //_INCLUDE_KG3DLOCALMOVECOORD_H_
