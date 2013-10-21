////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DIntersectionBase.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-3-9 10:48:46
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DINTERSECTIONBASE_H_
#define _INCLUDE_KG3DINTERSECTIONBASE_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DRepresentObject;
class KG3DModel;
class KG3DModelST;
class KG3DMesh;
class KG3DSceneEntity;
/*
RayIntersection函数都接受一个Filter参数来定值排除某些东西的碰撞
，Filter必须由KG3DIntersectionFilter派生。
用Accept来接受一个结果，如果返回的是E_FAILED，不接受某个对象，
那么那个对象的RayIntersection返回FALSE。
如，摄像机不会和ModelST碰撞。那么这样子

struct CameraInterFil : public KG3DIntersectionFilter
{
virtual HRESULT Accept(KG3DModelST& ModelST){return E_FAILED;}
}

不想碰那个就在那个函数中返回E_FAILED就可以了，要更仔细的检查的话
可以在函数中写出更仔细的检查方案，如查一下Mesh的属性再决定是否返回E_FAILED。

RayIntersection会保证
层层检查，所以只需要在最方便的地方写Accept就好了。
如Mesh如果有排除摄像机属性的
话，Mesh在摄像机碰撞的时候要排除，那么不需要覆盖Model那层，然后从Model判断Mesh
而是直接覆盖Mesh那层就好了。

用Filter是有代价的，如果Filter为NULL那么代价几乎为0，否则必须层层检查，不然
RayIntersection的内层版本可能被调用，然后没有被Filter。反正就是没有必要就
别传个Filter参数进去。
*/

struct KG3DIntersectionFilter
{
	virtual HRESULT Accept(KG3DRepresentObject& Object){return S_OK;}
	virtual HRESULT Accept(KG3DSceneEntity& Entity){return S_OK;}
	virtual HRESULT Accept(KG3DModel& Model){return S_OK;}
	virtual HRESULT Accept(KG3DModelST& ModelST){return S_OK;}
	virtual HRESULT Accept(KG3DMesh& Mesh){return S_OK;}
};

#endif //_INCLUDE_KG3DINTERSECTIONBASE_H_
