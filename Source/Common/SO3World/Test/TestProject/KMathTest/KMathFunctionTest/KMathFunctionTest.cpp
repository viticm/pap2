///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KMathFunctionTest
// Creator  : ChenShiCai
// Date     : 2008-10-22
// Comment  : 功能测试
//
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "KMathFunctionTest.h"


CPPUNIT_TEST_SUITE_REGISTRATION(KMathFunctionTest); 

void KMathFunctionTest::KMathTest_6718()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "距离是否在指定数值内 两点间距离小于指定数值";

    nRetCode = g_InRange(1, 1, 1, 2, 2, 2, 5);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
}

void KMathFunctionTest::KMathTest_6719()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "距离是否在指定数值内 两点间距离大于指定数值";

    nRetCode = g_InRange(1, 1, 1, 2, 2, 2, 1);


    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(!nRetCode);
}

void KMathFunctionTest::KMathTest_6720()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "距离是否在指定数值内 两点间距离等于指定数值";

    nRetCode = g_InRange(0, 0, 0, 3, 4, 0, 5);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
}

void KMathFunctionTest::KMathTest_6721()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "距离是否在指定数值内 两点间距离超出int上限";

    nRetCode = g_InRange(0, 0, 0, 2147483647, 2147483647, 2147483647, 1000);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
}

void KMathFunctionTest::KMathTest_6722()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "距离是否在指定范围内 两点间距离小于范围的下限";

    nRetCode = g_InRange(0, 0, 0, 1, 1, 1, 5, 10);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == irrTooClose);
}

void KMathFunctionTest::KMathTest_6723()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "距离是否在指定范围内 两点间距离等于范围的下限";

    nRetCode = g_InRange(0, 0, 0, 3, 4, 0, 5, 6);  

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == irrInRange);
}

void KMathFunctionTest::KMathTest_6724()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "距离是否在指定范围内 两点间距离小于范围的上限";

    nRetCode = g_InRange(0, 0, 0, 3, 4, 0, 1, 6);  

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == irrInRange);
}

void KMathFunctionTest::KMathTest_6725()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "距离是否在指定范围内 两点间距离等于范围的上限";

    nRetCode = g_InRange(0, 0, 0, 3, 4, 0, 1, 5);  

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == irrInRange);
}

void KMathFunctionTest::KMathTest_6726()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "距离是否在指定范围内 两点间距离大于范围的上限";

    nRetCode = g_InRange(0, 0, 0, 5, 5, 5, 1, 4);  

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == irrTooFar);
}
inline int TestSinInRange(int nBegin, int nEnd) 
{
	int nResult = false;    
	int nRetCode = false;

    for (int i = nBegin; i < nEnd + 1; ++i)
    {
        int nKSinRet = g_Sin(i);
        float nRadians = 2 * i * SO3WORLD_PI/ 256;
        float fSinRet = sin(nRadians) * 4096;

        nRetCode = (int)(fSinRet > 0 ? (fSinRet + 0.5) : (fSinRet - 0.5)) - nKSinRet;
        KG_PROCESS_ERROR(!nRetCode);
    }
	
	nResult = true;
Exit0:
	return nResult;
}

void KMathFunctionTest::KMathTest_6715()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "正弦 检查[0, 255]间各个角度的正弦值正确 ";

    nRetCode = TestSinInRange(0, 255);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6716()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "正弦 检查[-255, 0]间各个角度的正弦值正确 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestSinInRange(-255, 0);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 


void KMathFunctionTest::KMathTest_6717()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "正弦 检查[256, 511]间各个角度的正弦值正确 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestSinInRange(256, 511);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6694()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点间距离的平方（根据坐标） 三维空间任意两个坐标点距离的平方 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = g_GetDistance3(10, 20, -30, -10, -20, 10);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == 3600);
} 

void KMathFunctionTest::KMathTest_6695()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点间距离的平方（根据坐标） 三维空间任意一点与坐标系原点的距离 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = g_GetDistance3(0, 0, 0, -5, -6, -7);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == 110);
} 

void KMathFunctionTest::KMathTest_6696()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点间距离的平方（根据坐标） 三维空间同一坐标点的距离 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = g_GetDistance3(10, -20, 50, 10, -20, 50);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == 0);
} 


void KMathFunctionTest::KMathTest_6697()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点间距离的平方（根据xy平面距离平方和z坐标） xy平面距离为正整数，z坐标不同的两点距离 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = g_GetDistance3(10, 5, -5);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == 110);
} 

void KMathFunctionTest::KMathTest_6698()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点间距离的平方（根据xy平面距离平方和z坐标） xy平面距离为0,z方向距离非0 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = g_GetDistance3(0, 10, 0);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == 100);
}

void KMathFunctionTest::KMathTest_6699()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点间距离的平方（根据xy平面距离平方和z坐标） 三维空间中同一点的距离 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = g_GetDistance3(0, 10, 10);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == 0);
} 

void KMathFunctionTest::KMathTest_6700()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点间距离的平方（根据xy平面距离平方和z坐标） 三维空间中同一点的距离 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = g_GetDistance2(10, -10, -10, -10);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == 400);
} 

void KMathFunctionTest::KMathTest_6701()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点间距离的平方（根据xy平面距离平方和z坐标） 三维空间中同一点的距离 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = g_GetDistance2(2, 5, 2, 5);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == 0);
} 

inline int TestGetDirection(float fLeft, int nRight) 
{
    int nResult = false;    
    int nRetCode = false;
    int nX = 0;
    int nY = 0;

    while (true)
    {
        if (fLeft >= 256)
        {
            fLeft -= 256;
            continue;
        }

        if (fLeft < 0)
        {
            fLeft += 256;
            continue;
        }
        break;
    }

    nRight %= 256;

    if (fLeft == 0)
    {
        nX = 100;
        nY = 0;
    }
    else if (fLeft == 64)
    {
        nX = 0;
        nY = 100;
    }
    else if (fLeft == 128)
    {
        nX = -100;
        nY = 0;
    }
    else if (fLeft == 192)
    {
        nX = 0;
        nY = -100;
    }
    else if (fLeft < 64)//我知道可以组合起来，但这样可以不增加判断条件
    {
        nX = 100;
        nY = nX * tan(2 * fLeft * SO3WORLD_PI/ 256);
    }
    else if (fLeft < 128)
    {
        nX = -100;
        nY = nX * tan(2 * fLeft * SO3WORLD_PI/ 256);
    }
    else if (fLeft < 192)
    {
        nX = -100;
        nY = nX * tan(2 * fLeft * SO3WORLD_PI/ 256);
    }
    else if (fLeft < 256)
    {
        nX = 100;
        nY = nX * tan(2 * fLeft * SO3WORLD_PI/ 256);
    }
    else
    {
        ASSERT(false);
    }

    nRetCode = g_GetDirection(nX, nY);
    KG_PROCESS_ERROR(nRetCode == nRight);

    nResult = true;
Exit0:
    return nResult;
}

inline int TestGetDirectionInRange(int nBegin, int nEnd) 
{
    int nResult = false;    
    int nRetCode = false;

    for (int i = nBegin; i < nEnd + 1; ++i)
    {        
        nRetCode = TestGetDirection(i, i);
        KG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

void KMathFunctionTest::KMathTest_6702()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 检查[0, 255]间各个角度的数值正确性 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirectionInRange(0, 255);

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6703()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 计算坐标原点的方向 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = g_GetDirection(0, 0) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == 0);
} 

void KMathFunctionTest::KMathTest_6704()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 x、y达到上限 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = g_GetDirection(2147483647, 2147483647) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode == 32);
} 



void KMathFunctionTest::KMathTest_6727()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 检查第一象限内非整数角度（舍去）的数值正确性 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(45.3, 45) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6728()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 检查第一象限内非整数角度（五入）的数值正确性  ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(45.6, 46) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6729()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 检查第二象限内非整数角度（舍去）的数值正确性 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(105.3, 105) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6730()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 检查第二象限内非整数角度（五入）的数值正确性 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(105.8, 106) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6731()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 检查第三象限内非整数角度（舍去）的数值正确性 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(175.3, 175) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 


void KMathFunctionTest::KMathTest_6732()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 检查第三象限内非整数角度（五入）的数值正确性 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(175.8, 176) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6733()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 检查第四象限内非整数角度（舍去）的数值正确性  ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(205.3, 205) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6734()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "与坐标系(0, 0)的方向 检查第四象限内非整数角度（五入）的数值正确性 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(205.8, 206) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

inline int TestGetDirection(int nX1, int nY1, int nX2, int nY2) 
{
	int nResult = false;    
	int nRetCode = false;
    float fRetCode = 0;
    int nDirection = false;
    int nSinValue = 0;

    nDirection = g_GetDirection(nX1, nY1, nX2, nY2);
    fRetCode = ((float)g_Sin(nDirection)) / 4096;
    nSinValue = fRetCode > 0 ? fRetCode + 0.5 : fRetCode - 0.5; 

    fRetCode = (nY2 - nY1) * (nY2 - nY1) + (nX2 - nX1) * (nX2 - nX1);
    fRetCode = (nY2 - nY1) / sqrt(fRetCode);
    nRetCode = fRetCode > 0 ? fRetCode + 0.5 : fRetCode - 0.5; 

    KG_PROCESS_ERROR(nRetCode == nSinValue);

	nResult = true;
Exit0:
	return nResult;
}

void KMathFunctionTest::KMathTest_6705()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点方向（模板） 第一象限内两点方向正确 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(1, 2, 3, 4) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6706()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点方向（模板） 一、二象限内两点方向正确 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(1, 2, -1, 5) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6707()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点方向（模板） 一、三象限内两点方向正确";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(1, 2, -3, -4) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6708()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点方向（模板） 一四象限内两点方向正确 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(1, 2, 3, -4) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6709()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点方向（模板） 第二象限内两点方向正确 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(-1, 2, -3, 4) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6710()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点方向（模板） 二、三象限内两点方向正确 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(-1, 2, -3, -4) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6711()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点方向（模板） 二、四象限内两点方向正确 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(-1, 2, 3, -4) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6712()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点方向（模板） 第三象限内两点方向正确 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(-1, -2, -3, -4) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6713()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点方向（模板） 三、四象限内两点方向正确  ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(-1, -2, 3, -4) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

void KMathFunctionTest::KMathTest_6714()
{
    int nRetCode = false;
    char szTestCaseDscribe[] = "两点方向（模板） 第四象限内两点方向正确 ";
    KG_CASE_INFO(szTestCaseDscribe, "");

    nRetCode = TestGetDirection(1, -2, 3, -4) ;

    KG_CASE_INFO(szTestCaseDscribe, "");
    KG_CPPUNIT_ASSERT(nRetCode);
} 

