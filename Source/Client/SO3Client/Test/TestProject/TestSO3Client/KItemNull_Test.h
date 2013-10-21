/***************************************************
Filename:KItemNull_Test.h
CreatedBy: Yan Chunwa
CreatedOn: 2006Äê1ÔÂ21ÈÕ
Test Requirement Reference by Wiki path:
http://sword-test/qcweb/pmwiki.php?n=XSJQC-DEV.JX3-QC-REQ-SO3Client_UIKItemNull
Comment:
**************************************************/
#ifndef KITEMNULL_TEST_H
#define KITEMNULL_TEST_H

#ifdef _UNIT_TEST

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "../ui/elem/Components/item.h"
#include "../ui/elem/Components/itemeventmgr.h"

class KItemNull_Test: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(KItemNull_Test);
	CPPUNIT_TEST(InitItemTest_1);
	CPPUNIT_TEST(InitItemTest_2);
	CPPUNIT_TEST(InitItemTest_3);
	CPPUNIT_TEST(InitItemTest_4);
	CPPUNIT_TEST(InitItemTest_5);
	CPPUNIT_TEST(InitItemTest_6);
	CPPUNIT_TEST(DeleteItemTest);
	CPPUNIT_TEST(ChgItemTypeTest_1);
	CPPUNIT_TEST(ChgItemTypeTest_2);
	CPPUNIT_TEST(ChgItemTypeTest_3);
	CPPUNIT_TEST(ChgItemTypeTest_4);
	CPPUNIT_TEST(ChgItemTypeTest_5);
	CPPUNIT_TEST(ChgItemTypeTest_6);
	CPPUNIT_TEST(PtINItemTest_1);
	CPPUNIT_TEST(PtINItemTest_2);
	CPPUNIT_TEST(PtINItemTest_3);
	CPPUNIT_TEST(PtINItemTest_4);
	CPPUNIT_TEST(PtINItemTest_5);
	CPPUNIT_TEST(PtINItemTest_6);
	CPPUNIT_TEST(PtINItemTest_7);
	CPPUNIT_TEST(PtINItemTest_8);
	CPPUNIT_TEST(PtINItemTest_9);
	CPPUNIT_TEST(PtINItemTest_10);
	CPPUNIT_TEST(SetPosTypeTest_1);
	CPPUNIT_TEST(SetPosTypeTest_2);
	CPPUNIT_TEST(SetPosTypeTest_3);
	CPPUNIT_TEST(SetPosTypeTest_4);
	CPPUNIT_TEST(SetPosTypeTest_5);
	CPPUNIT_TEST(SetNameTest_1);
	CPPUNIT_TEST(SetNameTest_2);
	CPPUNIT_TEST(SetNameTest_3);
	CPPUNIT_TEST(SetNameTest_4);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void InitItemTest_1();
	void InitItemTest_2();
	void InitItemTest_3();
	void InitItemTest_4();
	void InitItemTest_5();
	void InitItemTest_6();
	void DeleteItemTest();
	void ChgItemTypeTest_1();
	void ChgItemTypeTest_2();
	void ChgItemTypeTest_3();
	void ChgItemTypeTest_4();
	void ChgItemTypeTest_5();
	void ChgItemTypeTest_6();
	void PtINItemTest_1();
	void PtINItemTest_2();
	void PtINItemTest_3();
	void PtINItemTest_4();
	void PtINItemTest_5();
	void PtINItemTest_6();
	void PtINItemTest_7();
	void PtINItemTest_8();
	void PtINItemTest_9();
	void PtINItemTest_10();
	void SetPosTypeTest_1();
	void SetPosTypeTest_2();
	void SetPosTypeTest_3();
	void SetPosTypeTest_4();
	void SetPosTypeTest_5();
	void SetNameTest_1();
	void SetNameTest_2();
	void SetNameTest_3();
	void SetNameTest_4();
private:
	UI::KItemNull *m_item;
	UI::KItemNullData m_itemdata;
};
#endif  //#ifdef UNIT_TEST

#endif  //#ifndef KITEMNULL_TEST_H