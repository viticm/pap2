////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DLocalRotationCoord.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-11-9 14:14:48
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DLOCALROTATIONCOORD_H_
#define _INCLUDE_KG3DLOCALROTATIONCOORD_H_

#include "KG3DSceneEntity.h"

#define ROTATION_COORD_MESH_PATH TEXT("\\Data\\editor\\旋转\\local_rotation_coord.x")
class KG3DScene;
class KG3DRotationCoordinateOld
{
//private :
//    KG3DRotationCoordinateOld();

public :
	KG3DRotationCoordinateOld(KG3DScene& scene);
    virtual ~KG3DRotationCoordinateOld();

    //static KG3DRotationCoordinateOld* GetCoordinate();

    STDMETHOD(FrameMove)();
    STDMETHOD(Render)();

    //HRESULT AttachEntity(KG3DScene* pScene, std::list<KG3DSceneEntity> listEntitys);
    //HRESULT AttachEntity(KG3DScene* pScene, KG3DSceneEntity Entity);
    //HRESULT ReleaseAttach();
    HRESULT RotateBegin();
    HRESULT RotateEnd();
    HRESULT SetMode(DWORD dwMode);
    HRESULT Zoom(FLOAT fScanl);
    
	FLOAT	GetRotationAngleX(){return m_fAngelX;}
	FLOAT	GetRotationAngleY(){return m_fAngelY;}
	FLOAT	GetRotationAngleZ(){return m_fAngelZ;}

    //const EntityList& GetEntityList() { return m_EntityList; }

#if defined(DEBUG) | defined(_DEBUG)
	VOID	TestSelectCoord(DWORD dwCoord){m_dwCurrSelCoord = dwCoord;}
#endif

private :
	float   GetScalingFactor();
	DWORD   GetSelCoord(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDir);

    KG3DScene*  m_pAttachScene;
    //EntityList  m_EntityList;
    LPD3DXMESH  m_pCoordMesh;
    DWORD       m_dwNumMatrials;
    DWORD       m_dwMode;			//世界坐标还是局部坐标

    DWORD       m_dwCurrSelCoord;	//选择的是哪个圈圈

    D3DXVECTOR3 m_currSelNormal;	//当前选择的圈圈对应的那个Normal
    D3DXPLANE   m_currSelPane;		//当前选中的圈圈对应的那个面
    D3DXVECTOR3 m_vPrevCross;		//前一个交点
    D3DXVECTOR3 m_vBeginCross;		//开始交点
    D3DXMATRIX  m_matCoord;			//核心坐标
    //D3DXMATRIX  m_matCoordPane;
    D3DXMATRIX  m_matEntityWorld;	//EntityList的核心坐标

    FLOAT       m_fZoom;			//放缩整个坐标圈圈的表现




    INT         m_nMoveFlag;		//是否在转动Begin了
    // editer
    FLOAT       m_fAngelX;	
    FLOAT       m_fAngelY;
    FLOAT       m_fAngelZ;
};

#include "KG3DBaseCoord.h"

//struct KG3DRotationCoordinate : public KG3DBaseCoord
//{
//	virtual	ULONG	GetCurState() = 0;
//	virtual HRESULT SetPosition(const D3DXVECTOR3& Pos) = 0;
//	virtual HRESULT	Zoom(FLOAT zoomFactor) = 0;//zoomFactor会加到Scale系数里面，而不是乘
//	virtual D3DXVECTOR3 GetRotationAngle() = 0;//返回XYZ对应轴的旋转
//	virtual ~KG3DRotationCoordinate() = 0{}
//};
KG3DBaseCoord* BuildKG3DRotationCoordinateSolid(LPCTSTR, DWORD_PTR pScene, DWORD_PTR);


#endif //_INCLUDE_KG3DLOCALROTATIONCOORD_H_
