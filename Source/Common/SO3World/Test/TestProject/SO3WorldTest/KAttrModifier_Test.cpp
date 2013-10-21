#include "StdAfx.h"
#ifdef _UNIT_TEST
#include "kattrmodifier_test.h"
#include "KCharacter.h"
#include "KAttribute.h"

CPPUNIT_TEST_SUITE_REGISTRATION(KAttrModifier_Test);

void KAttrModifier_Test::setUp()
{
}
void KAttrModifier_Test::tearDown()
{
}
void KAttrModifier_Test::testInit()
{
	int nRetCode = false;
	nRetCode = KAttrModifier::Init();
	CPPUNIT_ASSERT(nRetCode == TRUE);

	CPPUNIT_ASSERT(KAttrModifier::m_ProcessFuns[atMaxLifePercent]);
	CPPUNIT_ASSERT(KAttrModifier::m_ProcessFuns[atCurrentLife]);
}
void KAttrModifier_Test::testProcessStrength()
{
	//KCharacter* pCharacter = new KCharacter;
	//pCharacter->m_nStrength = INT_MIN;
	//KAttribute myAttribute;
	//myAttribute.m_nValue1 = INT_MAX;
	//int nRetCode = false;
	//nRetCode = KAttrModifier::m_ProcessFuns[atStrength](pCharacter, &myAttribute);
	//CPPUNIT_ASSERT(PCharacter->m_nStrength == -1);
}
void KAttrModifier_Test::testProcessStrengthPercent()
{
	//KAttrModifier::m_ProcessFuns[atStrengthPercent]
}
void KAttrModifier_Test::testProcessMaxLifePercent()
{
	//KAttrModifier::m_ProcessFuns[atMaxLifePercent]
}
void KAttrModifier_Test::testProcessCurrentLife()
{
	//KAttrModifier::m_ProcessFuns[atCurrentLife]
}
#endif