////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DSkySystem.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-11-15 14:18:22
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DSKYSYSTEM_H_
#define _INCLUDE_IEKG3DSKYSYSTEM_H_

#include "IEKG3DMeshFarMountain.h"
#include "IEKG3DCloud.h"

////////////////////////////////////////////////////////////////////////////////
interface IEKG3DMeshSkyBox;

struct IEKG3DSkySystem
{
	///<天空的管理部分>
	virtual HRESULT GetSkyBox(IEKG3DMeshSkyBox** ppiMeshSkyBox) = 0;
	virtual HRESULT AddSkyBox(IEKG3DMeshSkyBox** ppiMeshSkyBox) = 0;
	virtual HRESULT DelSkyBox() = 0;
	///</天空的管理部分>
    ///<远山的管理部分>
    virtual HRESULT AddFarMount( const KG3DMESHFARMOUNTAIN_DESCSTRUCT* pDescStruct, IEKG3DMeshFarMountain** ppAddedFarMount ) = 0;///如果传入NULL得话，用默认参数创建
    virtual HRESULT DelFarMount( int nIndex ) = 0;
    virtual HRESULT DelFarMount( IEKG3DMeshFarMountain* pFarMount ) = 0;
    virtual HRESULT UpperFarMount( int nIndex ) = 0;
    virtual HRESULT LowerFarMount( int nIndex ) = 0;
    virtual HRESULT ClearFarMountVec() = 0;
    virtual HRESULT GetFarMount( int nIndex, IEKG3DMeshFarMountain** ppFarMount ) = 0;
    virtual HRESULT GetFarMountIndexByPointer( IEKG3DMeshFarMountain* pFarMout, int* pIndex ) = 0;///得到Vector中的Index，如果没有的话返回-1
    virtual HRESULT IsFarMountPointerValid( IEKG3DMeshFarMountain* pFarMout, BOOL* pBool ) = 0;///从返回值和pBool都可以知道是否有效
    virtual HRESULT GetFarMountVecSize( int* pSize ) = 0;
    ///</远山的管理部分>
    ///<云的管理部分>
    virtual HRESULT AddCloud() = 0;
    virtual HRESULT DelCloud( int nIndex ) = 0;
    virtual HRESULT DelCloud( IEKG3DCloud* pCloud ) = 0;
    virtual HRESULT ClearCloudVec() = 0;
    virtual HRESULT GetCloud( int nIndex, IEKG3DCloud** ppCloud ) = 0;
    virtual HRESULT GetCloudIndexByPointer( IEKG3DCloud* pCloud, int* pIndex ) = 0;///得到Vector中的Index，如果没有的话返回-1
    virtual HRESULT IsCloudPointerValid( IEKG3DCloud* pCloud, BOOL* pBool ) = 0;///从返回值和pBool都可以知道是否有效
    virtual HRESULT GetCloudVecSize( int* pSize ) = 0;
    //virtual	HRESULT	SortCloudVec() = 0; ///强制重新排列云的Vector
    ///</云的管理部分>
	///<雾同步>
	//virtual HRESULT EnableSynWithFogColor(BOOL bEnable) = 0;
	//virtual HRESULT IsEnableSynWithFogColor(BOOL* pbEnable) = 0;
};

#endif //_INCLUDE_IEKG3DSKYSYSTEM_H_
