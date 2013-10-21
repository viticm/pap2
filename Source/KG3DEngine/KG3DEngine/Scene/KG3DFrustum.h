#pragma once

enum Frustum {NEAR_LEFT_BOTTOM = 0,NEAR_RIGHT_BOTTOM,NEAR_LEFT_TOP,NEAR_RIGHT_TOP,
			  FAR_LEFT_BOTTOM,     FAR_RIGHT_BOTTOM, FAR_LEFT_TOP, FAR_RIGHT_TOP,};

enum FrustumPlane {NEARPLANE = 0,FARPLANE,LEFTPLANE,RIGHTPLANE,TOPPLANE,BOTTOMPLANE};

enum CULLSTATE
{
	CS_UNKNOWN = 0,      // cull state not yet computed
	CS_INSIDE = 1,       // object bounding box is at least partly inside the frustum
	CS_OUTSIDE = 2,      // object bounding box is outside the frustum
	CS_INSIDE_SLOW = 3,  // OBB is inside frustum, but it took extensive testing to determine this
	CS_OUTSIDE_SLOW = 4, // OBB is outside frustum, but it took extensive testing to determine this
};

class KG3DCamera;
class KG3DPvsPortal;

/*
最重要的函数是Create系列方法
所有函数都没有判断Camera的UP，不是很重要，有空的时候加上。
另外由portal创建的时候，没有判断是否能够创建，有空加上
*/

class KG3DFrustum
{
	KG_TEST_DECLARE_CLASS_TEST(KG3DFrustum)
public:
	//<碰撞>
	//可以把碰撞细分一下，如果光要求二元的碰撞结果的话，INTERSECT和INSIDE可以归类为TRUE
	//，如果要更详细的结果就可以判断返回的是下面哪种
	enum INTERSECT_TYPE
	{
		OUTSIDE = 0,
		INTERSECT = 1,
		INSIDE = 2,
	};

    BOOL PortalVisable(const D3DXVECTOR3 vp[], DWORD dwLookMode) const;

	BOOL IsOBBoxVisible(const D3DXVECTOR3 BoxPos[]) const;
	BOOL IsAABBoxVisible(const AABBOX& BBox) const;

    BOOL	IsNegativeSide(const D3DXPLANE& plane, const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vHalf);
	INT		Intersect(const AABBOX& BBox) const;	//返回上面enum中的一种
	//</碰撞>

	HRESULT GetFrustumXZRect(D3DXVECTOR2& A,D3DXVECTOR2& C);

	//<创建>//这些函数，如果失败的话，Frustum不能使用，里面的参数已经错了
	//HRESULT			CalculateViewFrustum(BOOL bGetTransform = TRUE);
	HRESULT			CreateByDevice(LPDIRECT3DDEVICE9 pDevice);
	HRESULT			CreateByCamera(KG3DCamera& Camera);
	HRESULT			CreateBy2Rays(const D3DXVECTOR3& vSrcA, const D3DXVECTOR3& vDirA,
								const D3DXVECTOR3&	vSrcC, const D3DXVECTOR3& vDirC,FLOAT fDistance);	//两个Dir应该已经单位化,两个Src如果太过于接近是无法求出Near的

    //HRESULT         CalcDirection();    
	HRESULT         CreateByObjectPVS(KG3DCamera& Camera, D3DXVECTOR3 ProtalPos[4]);//根据摄像机和视口创建视锥
	HRESULT			CreateByPointGate(KG3DCamera& Camera
									, const D3DXVECTOR3& BottomLeft
									, const D3DXVECTOR3& BottomRight
									, const D3DXVECTOR3& TopLeft
									, const D3DXVECTOR3& TopRight);//如果创建失败，这个frustum是不能用的。这四个点保证是有序的

	BOOL           CullByPortal(const KG3DPvsPortal& Portal, DWORD dwLookMode);//根据摄像机和视口修改视锥
	//</创建>
	
	HRESULT			DrawFrustumByLine();

	const D3DXVECTOR3&	GetOrigin()const {return m_CameraData.m_vOrigin;}
	const D3DXVECTOR3&	GetPoint(INT nIndex)const;
	INT					GetPointCount()const;
	const D3DXVECTOR3&	GetDirection()const{return m_CameraData.m_vDirection;}
	const D3DXPLANE&	GetPlane(INT nIndex)const;
	INT					GetPlaneCount()const;


	BOOL            IsOBBoxVisibleOptimize(D3DXVECTOR3 BoxPos[]);
	BOOL			IsAABBoxInside(AABBOX& BBox);

	KG3DFrustum();
	~KG3DFrustum();
private:
	

	CULLSTATE KG3DFrustum::CullObjectInside(const D3DXVECTOR3 *ViewFrustumPoint,const D3DXPLANE *ViewFrustumPlane,
		const D3DXVECTOR3 *ObjectBoundPoint,const D3DXPLANE *ObjectBoundPlane);

	VOID	ComputePlanesFromPoints();

//下面的东东当是Private的用，DEBUG时写成protected是为了要子类化进行TDD
#if defined(DEBUG) | defined(_DEBUG)
protected:
#else
private:
#endif
	struct CameraDataStruct;
	HRESULT CreateByGatePrivate(CameraDataStruct& CameraData
					, const D3DXVECTOR3& BottomLeft
					, const D3DXVECTOR3& BottomRight 
					, const D3DXVECTOR3& TopLeft
					, const D3DXVECTOR3& TopRight );
	VOID	ComputeFrustum(const D3DXMATRIX& matView, const D3DXMATRIX& matProj);

	enum{
		em_point_count = 8,
		em_plane_count = 6,
		em_edge_count = 12,
	};

	DWORD			m_dwState;	//可以用作一些BOOL值的，保证这个放前面的话，初始化的时候可以用ZeroMemory
	D3DXVECTOR3		m_vPoints[em_point_count];
	static D3DXVECTOR3	m_stVerticesInPPSpace[em_point_count];//视锥体在PostProjection空间，左边都是{1,-1,0}的，所以可以变成共享的静态数据，可以根据矩阵反计算出原来的点。顺序和上面的NEAR_LEFT_BOTTOM等enum的顺序一致
	D3DXPLANE		m_Planes[em_plane_count];//全部向内

	//D3DXMATRIX		m_matWorld;
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matProj;
	D3DXMATRIX		m_matViewProj;
	//D3DXMATRIX      m_matVPInv;	//2009-1-20这个变量根本不需要存在，随时都可以从m_matViewProj计算出来，有些地方就没有维护这个变量，这样直接错掉	<m_matVPInv Refactor>

	//下面这些除非是用带Camera参数的Create函数创建的，否则不保证有用
	struct CameraDataStruct
	{
		D3DXVECTOR3     m_vOrigin;
		FLOAT           m_fZFar;
		FLOAT			m_fZNear;	
		D3DXVECTOR3     m_vDirection;
	};
	CameraDataStruct m_CameraData;

	static HRESULT			ConstructEdges(const D3DXVECTOR3 (&vPoints)[em_point_count], KG3DLine (&edges)[em_edge_count]);//传进来的是一个KG3DLine数组的引用
};


struct KG3DEntityObjectShell;

struct IKG3DSceneEntityContainer
{
	virtual HRESULT RemoveEntityObjectShell(KG3DEntityObjectShell* pShell) = 0;
	virtual HRESULT InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs) = 0;
};
