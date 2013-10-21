#pragma once

//////////////////////////////////////////////////////////////////////////
//力场

class KPhysicsForceField
{
private:
	char   m_szProperty[512];//用来存参数的内存空间
	AABBOX m_Box;//包围合
	DWORD  m_dwType;

public:
	HRESULT GetForce(D3DXVECTOR3* pForce,D3DXVECTOR3* pPosition);

	KPhysicsForceField(void);
	virtual ~KPhysicsForceField(void);
};

class KPhysicsForceFieldManager
{
public:

public:
	virtual HRESULT GetForce(D3DXVECTOR3* pForce,D3DXVECTOR3* pPosition);

	KPhysicsForceFieldManager(void);
	virtual ~KPhysicsForceFieldManager(void);
};
