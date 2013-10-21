// KRigidBody.h: interface for the KRigidBody class.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//说明:KRigidBody刚体类,只做一些大概的介绍
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//最重要是要注意m_fInertia转动惯量的计算了,如果使用了一个不恰当的值,会影响到物理系统的效果
//
//目前只是由一个浮点数来标识转动惯量,如果想取得更好的效果,可以考虑使用一个矩阵来
//保存各维的转动惯量信息,再对代码做一些修正
///////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KRIGIDBODY_H__7C732D4B_5BFA_482D_ACDC_D65D77C84F88__INCLUDED_)
#define AFX_KRIGIDBODY_H__7C732D4B_5BFA_482D_ACDC_D65D77C84F88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace Physics
{

//#define USING_FRICTION

typedef class KRigidBody  
{
public:

	typedef struct _SPCY
	{
		//半球体半径
		D3DXVECTOR3 CtrPosition;	//中心点坐标
		D3DXVECTOR3 Normal;			//轴向量
		float		Height;			//圆柱体半高度
		float		Radius;			//圆柱体的圆面半径
		
	}*LPSPCY;

	LPSPCY pSp;


	D3DXVECTOR3 m_Position; //质心坐标
	float       m_fMass; //质量
	float		m_fInvMass; //质量的倒数
	float		m_fInertia;//转动惯量
	float		m_fInvInertia;//转动惯量的倒数

	D3DXVECTOR3 m_LineVel;		//质心线速度	
	D3DXVECTOR3 m_AngVel;		//角速度
//	D3DXVECTOR3 m_RotAxis;      
//	D3DXVECTOR3 m_LineAcc;       //线加速度
	D3DXVECTOR3 m_Force;         //合力
	D3DXVECTOR3 m_Torque;        //合力矩

	D3DXMATRIX m_matOrient;      //刚体局部坐标系， 对应于质心
//	D3DXMATRIX m_matInertia;
//	D3DXMATRIX m_matInvInertia;
//	D3DXMATRIX m_matWorldI;
//	D3DXMATRIX m_matWorldInvI;

//#ifdef USING_FRICTION
	float m_fStaticFriction; //静摩擦系数
	float m_fDynamicFriction; //动摩擦系数
//#endif

	//BOOL m_bImmovable; //不可移动物体标记

private:
	D3DXVECTOR3 m_RelPos;  //pSp中心点与的相对坐标
	D3DXVECTOR3 m_RelNormal; //pSp->normal的相对坐标

	vector<int>vecCollision;  //记录与改物体碰撞的刚体
	vector<KRigidBody*>vecNonCollidable; //屏蔽与本刚体碰撞的刚体列表
	
public:
	KRigidBody();
	virtual ~KRigidBody();

//	int Initialise( D3DXVECTOR3 Pos, float fMass, float fInertia, D3DXMATRIX matOrient );
	int Initialise( D3DXVECTOR3 Pos, float fMass, float fInertia );	
	int Initialise( D3DXVECTOR3 Pos, float fMass );	
	int SetSP(D3DXVECTOR3 Pos, D3DXVECTOR3 Normal, float Height, float Radius);
	int SetMass(float mass);
	int SetInertia(float inertia);
//	int SetInertia(float Ixx, float Iyy, float Izz);
	int ApplyWorldImpulse(const D3DXVECTOR3& Impulse);
	int ApplyWorldAngImpulse(const D3DXVECTOR3& Impulse);
	int ApplyWorldImpulse(const D3DXVECTOR3& Impulse, const D3DXVECTOR3& Pos);
	int UpdateVelocity( float TinyTime, float factor = 0.0f );
	int UpdatePostion( float TinyTime );
	int ClearForce(void);

	vector<int>& Collision(){ return vecCollision; };
	vector<KRigidBody*>& NonCollidable(){ return vecNonCollidable; };

}*LPRIGIDBODY;

};
#endif // !defined(AFX_KRIGIDBODY_H__7C732D4B_5BFA_482D_ACDC_D65D77C84F88__INCLUDED_)
