// File: TestSTLStringInVC7.cpp
// Defines the entry point for the console application.

#include"..\..\Common\Common.h"
#include<iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

using namespace std;

void func(const Common::String& s)
{
	Common::String sTemp;

	Common::INIFile inif;
	inif.SetFilename("F:\\WorkSpace\\½£Íø3D³ÌÐò¿â\\ENGINE\\Tools\\SceneEditor\\Default.ttd");
	inif.Open();
	inif.GetINI("TreeInfo", "CPX", sTemp);
	float cpx = sTemp.FloatValue();
	inif.GetINI("TreeInfo", "CPY", sTemp);
	float cpy = sTemp.FloatValue();
	inif.GetINI("TreeInfo", "CPZ", sTemp);
	float cpz = sTemp.FloatValue();

	cout<<"The input: "<<s<<endl;
}

void main()
{
	Common::String sTemp = "Hello, world!";

	cout<<sTemp<<endl;

	func("Hello, you stupid!");

	cout<<"Finished!"<<endl;
}