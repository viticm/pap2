#pragma once

#include "cppunit/TestCaller.h"
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestSuite.h"
#include "cppunit/ui/text/TestRunner.h"
#include "cppunit/TextOutputter.h"
#include "cppunit/CompilerOutputter.h"
#include <iostream>
#include <fstream>


CPPUNIT_NS_BEGIN
class KG_Cppunit
{
public:
	KG_Cppunit( std::string name = "" )
	{
		m_pSuite = CppUnit::TestFactoryRegistry::getRegistry(name).makeTest();
		m_pRunner.addTest( m_pSuite ) ;
	} ;
	~KG_Cppunit(){} ;
	int RunTest()
	{
		return m_pRunner.run() ;
	} ;
	int Output()
	{
		std::ofstream outfile;
		outfile.open( "./result.txt",std::ios::out ) ;
		m_pRunner.setOutputter( new CppUnit::TextOutputter( &m_pRunner.result(), outfile ) ) ;
		return 1 ;
	} ;
private:
	CppUnit::Test* m_pSuite ;
	CppUnit::TextUi::TestRunner m_pRunner;

};

CPPUNIT_NS_END