/**************************************************************************



**************************************************************************/
	/*
   It simulates a rope with point-like particles binded with springs. The springs have inner friction and normal length. One tip of the rope is stabilized at a point in space called "Vector3D ropeConnectionPos". This point can be moved externally by a method "void setRopeConnectionVel(Vector3D ropeConnectionVel)". RopeSimulation creates air friction and a planer surface (or ground) with a normal in +y direction. RopeSimulation implements the force applied by this surface. In the code, the surface is refered as "ground".
*/

#include <math.h>

namespace Water
{

class Mass
{
public:
	float m;									
	D3DXVECTOR3 pos;								
	D3DXVECTOR3 vel;								
	D3DXVECTOR3 force;								

	Mass()
	{
		pos = D3DXVECTOR3( 0, 0, 0);
		vel = D3DXVECTOR3( 0, 0, 0);
		force = D3DXVECTOR3( 0, 0, 0);
	}
	Mass(float m)								
	{
		this->m = m;
		pos = D3DXVECTOR3( 0, 0, 0);
		vel = D3DXVECTOR3( 0, 0, 0);
		force = D3DXVECTOR3( 0, 0, 0);
	}


	void applyForce(D3DXVECTOR3 force)
	{
		this->force += force;					
	}

	
	void init()
	{
		force.x = 0;
		force.y = 0;
		force.z = 0;
	}

	
	void simulate(float dt)
	{
		vel += (force / m) * dt;														
		pos += vel * dt;						
	}

};


class Simulation
{
public:
	int numOfMasses;								
	Mass** masses;									
	
	Simulation(int numOfMasses, float m)			
	{
		this->numOfMasses = numOfMasses;
		
		masses = new Mass*[numOfMasses];			

		for (int a = 0; a < numOfMasses; ++a)		
			masses[a] = new Mass( m);				
	}

	virtual void release()							
	{
		for (int a = 0; a < numOfMasses; ++a)		
		{
			delete(masses[a]);
			masses[a] = NULL;
		}
			
		delete(masses);
		masses = NULL;
	}

	Mass* getMass(int index)
	{
		if (index < 0 || index >= numOfMasses)		
			return NULL;							

		return masses[index];						
	}

	virtual void init()								
	{
		for (int a = 0; a < numOfMasses; ++a)		
			masses[a]->init();						
	}

	virtual void solve()							
	{
													
	}

	virtual void simulate(float dt,D3DXVECTOR3 Begin)					
	{
		for (int a = 0; a < numOfMasses; ++a)		
			masses[a]->simulate(dt);				
	}

	virtual void operate(float dt, D3DXVECTOR3 Begin)					
	{
		init();										
		solve();									
		simulate( dt, Begin );								
	}

};


class ConstantVelocity : public Simulation
{
public:
	ConstantVelocity() : Simulation(1, 1.0f)				
	{
		masses[0]->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		
		masses[0]->vel = D3DXVECTOR3(1.0f, 0.0f, 0.0f);		
	}

};


class MotionUnderGravitation : public Simulation
{
public:
	D3DXVECTOR3 gravitation;													
	MotionUnderGravitation(D3DXVECTOR3 gravitation) : Simulation(1, 1.0f)		
	{																		
		this->gravitation = gravitation;									
		masses[0]->pos = D3DXVECTOR3(-10.0f, 0.0f, 0.0f);						
		masses[0]->vel = D3DXVECTOR3(10.0f, 15.0f, 0.0f);						
	}

	virtual void solve()													
	{
		for (int a = 0; a < numOfMasses; ++a)								
			masses[a]->applyForce(gravitation * masses[a]->m);				
	}
	
};


class MassConnectedWithSpring : public Simulation
{
public:
	float springConstant;													
	D3DXVECTOR3 connectionPos;													

	MassConnectedWithSpring(float springConstant) : Simulation(1, 1.0f)		
	{
		this->springConstant = springConstant;								

		connectionPos = D3DXVECTOR3(0.0f, -5.0f, 0.0f);						

		masses[0]->pos = connectionPos + D3DXVECTOR3(10.0f, 0.0f, 0.0f);		
		masses[0]->vel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);						
	}

	virtual void solve()													
	{
		for (int a = 0; a < numOfMasses; ++a)								
		{
			D3DXVECTOR3 springVector = masses[a]->pos - connectionPos;			
			masses[a]->applyForce(-springVector * springConstant);			
		}
	}
	
}; 




class Spring											
														
{
public:
	Mass* mass1;										
	Mass* mass2;										

	float springConstant;								
	float springLength;									
	float frictionConstant;								

	Spring(Mass* mass1, Mass* mass2, 
		float springConstant, float springLength, float frictionConstant)		
	{
		this->springConstant = springConstant;									
		this->springLength = springLength;										
		this->frictionConstant = frictionConstant;								

		this->mass1 = mass1;													
		this->mass2 = mass2;													
	}

	void solve()																	
	{
		D3DXVECTOR3 springVector = mass1->pos - mass2->pos;							
		
		float r = D3DXVec3Length( &springVector);											

		D3DXVECTOR3 force = D3DXVECTOR3( 0, 0, 0);																
		
		if (r != 0)																	
			force += (springVector / r) * (r - springLength) * (-springConstant);	
		
		force += -(mass1->vel - mass2->vel) * frictionConstant;						
																					

		mass1->applyForce(force);													
		mass2->applyForce(-force);													
	}

};


class RopeSimulation : public Simulation				
{
public:
	Spring** springs;									

	D3DXVECTOR3 gravitation;								

	D3DXVECTOR3 ropeConnectionPos;							
	
	D3DXVECTOR3 ropeConnectionVel;							

	float groundRepulsionConstant;						
	
	float groundFrictionConstant;						
	
	float groundAbsorptionConstant;						
														
	float groundHeight;									
														

	float airFrictionConstant;							
	RopeSimulation(										
		int numOfMasses,								//1. mass的数量
		vector<D3DXVECTOR3>Position1,
		float m,										//2. 每个mass的重量
		float springConstant,							//3. 弹簧的张量
		vector<float>Len,								//4. 弹簧不变形时的长度
		float springFrictionConstant,					//5. 弹簧的摩擦力
		D3DXVECTOR3 gravitation,						//6. 重力
		float airFrictionConstant,						//7. 空气的摩擦力
		float groundRepulsionConstant,					//8. ground repulsion constant
		float groundFrictionConstant,					//9. 地表摩擦力
		float groundAbsorptionConstant,					//10. ground absorption constant
		float groundHeight								//11. 地面的高度值
		) : Simulation(numOfMasses, m)					
	{
		this->ropeConnectionPos = D3DXVECTOR3( 0, 0, 0);
		this->ropeConnectionVel = D3DXVECTOR3( 0, 0, 0);
		this->gravitation = gravitation;
		
		this->airFrictionConstant = airFrictionConstant;

		this->groundFrictionConstant = groundFrictionConstant;
		this->groundRepulsionConstant = groundRepulsionConstant;
		this->groundAbsorptionConstant = groundAbsorptionConstant;
		this->groundHeight = groundHeight;

		for (int a = 0; a < numOfMasses; ++a)			
		{
			masses[a]->pos.x = Position1[a].x;		
			masses[a]->pos.y = Position1[a].y;						
			masses[a]->pos.z = Position1[a].z;						
		}

		springs = new Spring*[numOfMasses - 1];			
														
		
		for (a = 0; a < numOfMasses - 1; ++a)			
		{
			
			springs[a] = new Spring(masses[a], masses[a + 1], 
				springConstant, Len[a], springFrictionConstant);
		}
	}

	void release()										
	{
		Simulation::release();							

		for (int a = 0; a < numOfMasses - 1; ++a)		
		{
			delete(springs[a]);
			springs[a] = NULL;
		}
		
		delete(springs);
		springs = NULL;
	}

	void solve()										
	{
		for (int a = 0; a < numOfMasses - 1; ++a)		
		{
			springs[a]->solve();						
		}

		for (a = 0; a < numOfMasses; ++a)				
		{
			masses[a]->applyForce(gravitation * masses[a]->m);				
			
			masses[a]->applyForce(-masses[a]->vel * airFrictionConstant);	

			if (masses[a]->pos.y < groundHeight)		
			{
				D3DXVECTOR3 v = D3DXVECTOR3( 0, 0, 0);								

				v = masses[a]->vel;						
				v.y = 0;								

				
				masses[a]->applyForce(-v * groundFrictionConstant);		

				v = masses[a]->vel;						
				v.x = 0;								
				v.z = 0;								
				
				

				if (v.y < 0)							
					masses[a]->applyForce(-v * groundAbsorptionConstant);		
				
				
				D3DXVECTOR3 force = D3DXVECTOR3(0, groundRepulsionConstant, 0) * 
					(groundHeight - masses[a]->pos.y);

				masses[a]->applyForce(force);			
			}
				
		}


	}

	void simulate(float dt,D3DXVECTOR3 Begin )								
													
	{
		Simulation::simulate(dt,Begin);					

		ropeConnectionPos += ropeConnectionVel * dt;	

		if (ropeConnectionPos.y < groundHeight)			
		{
			ropeConnectionPos.y = groundHeight;
			ropeConnectionVel.y = 0;
		}

		masses[0]->pos.x = Begin.x;	
		masses[0]->pos.y = Begin.y;	
		masses[0]->pos.z = Begin.z;
		masses[0]->vel = ropeConnectionVel;				
	}

	void setRopeConnectionVel(D3DXVECTOR3 ropeConnectionVel)	
	{
		this->ropeConnectionVel = ropeConnectionVel;
	}

};
}