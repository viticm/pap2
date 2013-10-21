#pragma once
#include "scene.h"
#include "./KRigidBodyTable.h"
#include "./KPhysicsSimulator.h"
//#include "KHingeJoint.h"
#include "KSceneSwordTestWizard.h"

class KSceneSwordTest :
	public KSceneEditBase
{
private:
	float FPS;
	float LastTime;

	BOOL  m_bTesting;
	int   m_nFrameCount;
	DWORD m_dwTestTimeStart;

	int m_nFacesCount;

	int m_nTestType;
	KSceneSwordTestWizard Dlg;

	int m_nNowTestingMeshIndex;

	struct _TestInfo
	{
		string Name;
		DWORD  MeshID;
		DWORD  NumFaces;
		float  FPS;
	};
	vector<_TestInfo>m_vecTestModelID;

public:

	HRESULT InitPhysicsTest(int Index);

	void ONKEYDOWN(WPARAM wParam, LPARAM lParam);

	D3DXVECTOR3 GetObjectPosition(int Index);
	HRESULT FrameMove();

	Physics::KRigidBodyTable    m_RigidTable;
	Physics::KPhysicsSimulator  m_Sim;
//	Physics::KConstraintBodyBodyMaxDist* pConstraint;
//	Physics::KConstraintBodyBodyPoint* pPointConstraint;
//	Physics::KConstraintFixedPoint* pFixedConstraint;

//	Physics::KHingeJoint m_Hinge;

	HRESULT Initialize(PVOID pSceneTable);
	KSceneSwordTest(void);

	virtual ~KSceneSwordTest(void);
	HRESULT Render(void);
};
