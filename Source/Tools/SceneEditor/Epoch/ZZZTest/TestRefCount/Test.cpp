// File: Test.cpp
// Desc:

#include"..\..\Render\Renderable.h"
#include<iostream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


/*
class A
{
public:
	virtual ~A() {  }

	void asad() { Notify(); }
	virtual void Notify() = 0;  //{ cout<<"A::Notify()"<<endl; }
};

class B: public A
{
public:

	virtual void Notify() { cout<<"B::Notify()"<<endl; }
};
*/

void hello()
{
	Render::Renderable r1;  //, r2;
	//Render::Renderable r3 = r2;

	//r1 = r2;
}

void main()
{
	hello();

	cout<<"\nDone!"<<endl;
}