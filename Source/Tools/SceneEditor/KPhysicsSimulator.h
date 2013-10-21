// KPhysicsSimulator.h: interface for the KPhysicsSimulator class.
//

///////////////////////////////////////////////////////////////////////////////////////////////////
//说明:KPhysicsSimulator物理系统类,只做一些大概的介绍
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//	本代码中碰撞检测的算法是Discrete Methods
//	碰撞响应的算法是"Impulse-based Dynamic Simulation"
//
//  总的来说这些算法是效率最高的算法,虽然可能不能处理某些特殊碰撞情况
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpcyIntersect()函数用以实现两刚体之间的相交测试
//	SpcyPlaneIntersect()函数用以实现刚体与某平面之间的相交测试
//
//	PreProcessCollision()函数用以碰撞响应的预处理
//	ProcessCollision()函数用以碰撞响应的处理
//
//	DoTimeStep()函数用以整合碰撞检测和碰撞响应的代码
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	KConstraintFixedPoint类是建立两刚体连接约束的类,只是简单的做点运动跟随,所以还未整合到本物理系统类中,如有需要,可以整合进这里
///////////////////////////////////////////////////////////////////////////////////////////////////
//	本类存在的问题
//	1.类中,刚体还是可能会出项互相嵌入的现象,已做过一些尝试,但效果都不是很好,目前代码都已被屏蔽,
//	  可参考的代码有SeparateObjects(),HandleAllConstraints()函数和由变量m_bNonePenetration引导的代码,目前都已被屏蔽
//
//	2.类中,刚体与地图的碰撞检测需要考虑修正
//    因为目前实现的只是刚体与平面碰撞的效果
//
//	3.相交测试中,因为碰撞点和嵌入深度都是计算出了一个代替值,而不能计算出一个精确值,这会影响到物理系统的效果
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KPHYSICSSIMULATOR_H__AEF21872_9FF4_4BB9_83A8_201EE068F248__INCLUDED_)
#define AFX_KPHYSICSSIMULATOR_H__AEF21872_9FF4_4BB9_83A8_201EE068F248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "./KRigidBody.h"
//#include ".\KConstraintBodyBodyMaxDist.h"
//#include ".\KConstraintBodyBodyPoint.h"
#include ".\KConstraintFixedPoint.h"
#include ".\kcollisioninfo.h"
//#include ".\kcollisionutil.h"

namespace Physics
{
class KPhysicsSimulator  
{
public:
	
	DWORD m_dwLastTime;
public:
	HRESULT Render();
	HRESULT FrameMove();

	HRESULT AddRigidBody(LPRIGIDBODY pRigid);
	HRESULT RemoveRigidBody(LPRIGIDBODY pRigid);

	HRESULT AddConstraint(KConstraint* pConstraint);
	HRESULT RemoveConstraint(KConstraint* pConstraint);

	KPhysicsSimulator();
	virtual ~KPhysicsSimulator();

	BOOL CheckCollision(LPRIGIDBODY pRigid1, LPRIGIDBODY pRigid2);


	list<LPRIGIDBODY>listRigidBody;		//物理环境中的刚体
	list<KConstraint*>listConstraint;	//物理环境中的约束
	list<D3DXPLANE> listPlane;			//物理环境中的平面

	BOOL m_bAddingGravity;		//TRUE为在重力场状态, FALSE为无重力理想状态
	BOOL m_bUsingFriction;		//TRUE为考虑摩擦力作用,FALSE为不考虑摩擦力作用
//	BOOL m_bNonePenetration;
	float m_fEpsilon;			//碰撞恢复系数,0~1之间,1为完全弹性碰撞,0为完全非弹性碰撞
	
private:

	typedef struct _AABB
	{
		D3DXVECTOR3 min;
		D3DXVECTOR3 max;
	}* LPAABB;

	struct StoredData		//存储碰撞信息的结构
	{
		StoredData(LPRIGIDBODY pRigid)
		{ 
			m_Position = pRigid->m_Position;
			m_LineVel = pRigid->m_LineVel;
			m_AngVel = pRigid->m_AngVel;
			m_matOrient = pRigid->m_matOrient;

		}
		StoredData(){};
		D3DXVECTOR3 m_Position;
		D3DXVECTOR3 m_LineVel;
		D3DXVECTOR3 m_AngVel;	
		D3DXMATRIX m_matOrient;
	};

	vector<StoredData> vecStoredData;
	vector<KCollisionInfo>vecCollisionInfo;



private:

	BOOL CheckVetexStatus( const D3DXVECTOR3& normal, const D3DXVECTOR3& point, const D3DXVECTOR3& vec1, const D3DXVECTOR3& vec2);
//	BOOL CheckCollisionWithVect(LPRIGIDBODY pRigid1, LPRIGIDBODY pRigid2, D3DXVECTOR3& vect );
	BOOL CheckSphereIntersect( const D3DXVECTOR3& vec1,  float r1, const D3DXVECTOR3& vec2,  float r2);
	BOOL SphereAABBIntersect( const D3DXVECTOR3& ct,  float r, const LPAABB A);
	BOOL AABBIntersect( const LPAABB A, const LPAABB B);
    BOOL CreateAABB(LPAABB A, KRigidBody::LPSPCY pSp);

	float VertexLineDist(const D3DXVECTOR3& V0, const D3DXVECTOR3& LV1, const D3DXVECTOR3& LV2);
	float LineDist(const D3DXVECTOR3& L1V1, const D3DXVECTOR3& L1V2, const D3DXVECTOR3& L2V1, const D3DXVECTOR3& L2V2, BOOL & paral);
//	BOOL IsZeroVector(const D3DXVECTOR3& vec);
	BOOL IsAcuteAngle(const D3DXVECTOR3& vec1, const D3DXVECTOR3& vec2, const D3DXVECTOR3& vec3);
	BOOL CheckSeparateVertex(const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3, const D3DXVECTOR3& v4);

	BOOL SphereSpcyIntersect(KRigidBody::LPSPCY pSp, const D3DXVECTOR3& vecCenter, float radius);
//	BOOL SphereSpcyIntersectWithVect(KRigidBody::LPSPCY pSp, const D3DXVECTOR3& vecCenter, float radius, D3DXVECTOR3& vect );

//	void CalCollisionVelocity( D3DXVECTOR3& va , float ma , D3DXVECTOR3& vb , float mb, float e);
//	int StepRigidBody(LPRIGIDBODY pRigid, float TinyTime);

//	BOOL SpcyPlaneIntersect(LPRIGIDBODY pRigid, const D3DXVECTOR3& vect, const D3DXVECTOR3& normal);
//	BOOL SpcyPlaneIntersect1(LPRIGIDBODY pRigid, const D3DXVECTOR3& point, const D3DXVECTOR3& normal, D3DXVECTOR3& inter );
	BOOL LineSegIntersect(const D3DXVECTOR3& q1, const D3DXVECTOR3& q2, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2);
//	BOOL LineSegSpcyIntersect(LPRIGIDBODY pRigid, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2);
//	BOOL TriSpcyIntersect(LPRIGIDBODY pRigid, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3);
	BOOL VertexInSpcy(LPRIGIDBODY pRigid, const D3DXVECTOR3& v1);

public:
	int DisableCollision( LPRIGIDBODY pRigid1, LPRIGIDBODY pRigid2 );

private:
	BOOL IsCollidable( LPRIGIDBODY pRigid, vector<LPRIGIDBODY> & nc );
	BOOL SpcyPlaneIntersect( const LPRIGIDBODY pRigid, D3DXPLANE & Plane, D3DXVECTOR3& Inter, float& PenetrationDepth );
	BOOL SpcyIntersect( const LPRIGIDBODY pRigid1, const LPRIGIDBODY pRigid2, D3DXVECTOR3& Inter, D3DXVECTOR3& DirTo1, float& PenetrationDepth );
	BOOL SpcySphereIntersect( const LPRIGIDBODY pRigid1, const LPRIGIDBODY pRigid2, D3DXVECTOR3& Inter, D3DXVECTOR3& DirTo1, float& PenetrationDepth );//Rigid2是Sphere
//  BOOL SphereIntersect( const LPRIGIDBODY pRigid1, const LPRIGIDBODY pRigid2, D3DXVECTOR3& Inter, D3DXVECTOR3& DirTo1, float& PenetrationDepth );//Rigid1,Rigid2是Sphere

	int UpdateAllVelocities(float Tinytime);
	int UpdateAllPositions(float Tinytime);
	int ClearAllForces(void);
	int GetAllForces(void);
	int DetectAllCollisions(void);
	int DetectCollisions( void );
	int DetectHeightMapCollision( LPRIGIDBODY pRigid, D3DXPLANE& plane);
	int DetectBodyCollision( LPRIGIDBODY pRigid1, LPRIGIDBODY pRigid2);
	int PreProcessCollision( KCollisionInfo& collision, float TinyTime );
	int ProcessCollision( KCollisionInfo& collision, float TinyTime, float epsilon );
	int HandleAllCollisions(float TinyTime);
//	int SeparateObjects( KCollisionInfo& collision, float factor );
//	int HandleAllConstraints(float TinyTime);
	int DoTimeStep(float Tinytime);

//private:
//	void CollisionSimulation( float TinyTime );
//	void SimulationTest( float TinyTime);
//	void PlaneCollisionSimulation(float Tinytime);
//	void TestConstraintMaxDist(float Tinytime);
//	void TestFixedPoint(float Tinytime);
//	void TestRotation(float TinyTime);
};

};
#endif // !defined(AFX_KPHYSICSSIMULATOR_H__AEF21872_9FF4_4BB9_83A8_201EE068F248__INCLUDED_)
