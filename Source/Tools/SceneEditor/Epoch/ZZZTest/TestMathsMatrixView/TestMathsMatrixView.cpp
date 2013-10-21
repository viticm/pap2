// File: TestMathsMatrixView.cpp
// Desc:

#include"..\..\Maths\Maths.h"

#include<iostream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


void main()
{
	Maths::Camera camera;
	camera.Init(Maths::Vector3F(0.f, 100.f, 0.f), Maths::Vector3F(0.f, 100.f, 100.f), Maths::Vector3F(0.f, 1.f, 0.f), 5.f, 10000.f, PI/3, 600.f/800.f);

	Maths::Matrix mtx;
	camera.GetViewMatrix(mtx);

	//camera.MoveForward(10.f);
	//camera.GetCachedViewMatrix(mtx);

	camera.LookUp(PI/64);
	camera.GetCachedViewMatrix(mtx);
}
