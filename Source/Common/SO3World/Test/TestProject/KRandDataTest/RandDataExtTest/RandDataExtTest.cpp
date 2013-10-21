// RandDataExtTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KProbability.h"
#include "KRandDataExtTest.h"
#include <math.h>

int KRandDataExtTest::Init()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;

    nRetCode = m_RandExtTest.Init();
    KG_PROCESS_ERROR(nRetCode);
    m_bInitExtFlag = true;

    nResult = true;
Exit0:
    return nResult;
}

void KRandDataExtTest::UnInit()
{
    m_RandExtTest.UnInit();
    m_bInitExtFlag = false;
}

BOOL KRandDataExtTest::TestSaveNothing()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    BYTE byBuffer[MAX_BUFFERLENTGH];

    if (m_bInitExtFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);
    //将变量删除并重新创建，保证数据的干净

    m_RandExtTest.Save(byBuffer, MAX_BUFFERLENTGH);

    UnInit();
    //保存好之后将其消灭干净，再重新创建一个

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);

    m_RandExtTest.Load(byBuffer, MAX_BUFFERLENTGH);

    KG_PROCESS_ERROR(m_RandExtTest.m_RandCache.Size() == 0);

    nResult = true;
Exit0:
    return nResult;

}

BOOL KRandDataExtTest::TestSaveOne()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    BYTE byBuffer[MAX_BUFFERLENTGH];

    if (m_bInitExtFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);
    //将变量删除并重新创建，保证数据的干净
    
    m_RandExtTest.RandomResult(1, 500, MAX_PROBABILITY - 1);

    m_RandExtTest.Save(byBuffer, MAX_BUFFERLENTGH);

    UnInit();
    //保存好之后将其消灭干净，再重新创建一个

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);

    m_RandExtTest.Load(byBuffer, MAX_BUFFERLENTGH);

    KG_PROCESS_ERROR(m_RandExtTest.m_RandCache.Size() == 1);

    nResult = true;
Exit0:
    return nResult;

}

BOOL KRandDataExtTest::TestSaveAll()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    BYTE byBuffer[MAX_BUFFERLENTGH];

    if (m_bInitExtFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);
    //将变量删除并重新创建，保证数据的干净

    for (int i = 1; i <= 6; i++)
    {
        m_RandExtTest.RandomResult(i, 500, MAX_PROBABILITY - 1);
    }

    m_RandExtTest.Save(byBuffer, MAX_BUFFERLENTGH);

    UnInit();
    //保存好之后将其消灭干净，再重新创建一个

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);

    m_RandExtTest.Load(byBuffer, MAX_BUFFERLENTGH);

    KG_PROCESS_ERROR(m_RandExtTest.m_RandCache.Size() == 5);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KRandDataExtTest::TestRandom(DWORD nProbabilityValue)
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    
    int nAccount    =   0;
    
    int nWantResult = (int)ceil(1000 * 2.5 / (float)nProbabilityValue);

    // 当掉率为100%时，预期结果应该是1次
    if (nProbabilityValue == 1000)
        nWantResult = 1;
    

    //将变量删除并重新创建，保证数据的干净
    if (m_bInitExtFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);


    while(nAccount <= 65535)    //如果到这么多次还没成功，那本身就是出错了
    {
        nAccount++;
        // 999是g_Random(1000)的最大值，也是最可能失败的值
        nRetCode = m_RandExtTest.RandomResult(1, nProbabilityValue, MAX_PROBABILITY - 1);
        if (nRetCode)
            break;
    }
    
    KG_PROCESS_ERROR(nAccount == nWantResult);

    nResult = true;
Exit0:
    return nResult;
}



BOOL KRandDataExtTest::Test2Key01()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    int nAccount    =   0;

    //将变量删除并重新创建，保证数据的干净
    if (m_bInitExtFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RandExtTest.RandomResult(1, 500, MAX_PROBABILITY - 1);
    KG_PROCESS_ERROR(!nRetCode);        //稳定失败

    while(nAccount <= 65535)    //这么多次了，一定出了什么问题
    {
        nAccount++;
        nRetCode = m_RandExtTest.RandomResult(2, 500, MAX_PROBABILITY - 1);
        if (nRetCode)
            break;
        
    }

    KG_PROCESS_ERROR(nAccount == 5);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KRandDataExtTest::Test2Key02()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    int nAccount    =   0;


    //将变量删除并重新创建，保证数据的干净
    if (m_bInitExtFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);


    while(nAccount <= 65535)    //这么多次了，一定出了什么问题
    {
        nAccount++;

        nRetCode = m_RandExtTest.RandomResult(1, 200, MAX_PROBABILITY - 1);
        if (nRetCode)
        {
            break;
        }

        nRetCode = m_RandExtTest.RandomResult(2, 500, MAX_PROBABILITY - 1);
        if (nRetCode)
        {
            KG_PROCESS_ERROR(nAccount == 5 || nAccount == 10);
        }

    }

    KG_PROCESS_ERROR(nAccount == 13);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KRandDataExtTest::TestGetItem01()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    int nAccount    =   0;


    //将变量删除并重新创建，保证数据的干净
    if (m_bInitExtFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RandExtTest.RandomResult(1, 500, MAX_PROBABILITY - 1);
    KG_PROCESS_ERROR(!nRetCode);    // 一定失败

    nRetCode = m_RandExtTest.RandomResult(1, 500, 0);
    KG_PROCESS_ERROR(nRetCode);     //一定成功

    while (nAccount < 65535)
    {
        nAccount++;
        nRetCode = m_RandExtTest.RandomResult(1, 500, MAX_PROBABILITY - 1);
        if (nRetCode)
            break;
    }

    KG_PROCESS_ERROR(nAccount == 5);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KRandDataExtTest::TestGetItem02()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    int nAccount    =   0;


    //将变量删除并重新创建，保证数据的干净
    if (m_bInitExtFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RandExtTest.RandomResult(1, 500, 0);
    KG_PROCESS_ERROR(nRetCode);     //一定成功

    while (nAccount < 65535)
    {
        nAccount++;
        nRetCode = m_RandExtTest.RandomResult(1, 500, MAX_PROBABILITY - 1);
        if (nRetCode)
            break;
    }

    KG_PROCESS_ERROR(nAccount == 5);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KRandDataExtTest::TestOverKey01()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    int nAccount    =   0;


    //将变量删除并重新创建，保证数据的干净
    if (m_bInitExtFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);

    for (int nIndex = 0; nIndex < 4; nIndex++)
    {
        nRetCode = m_RandExtTest.RandomResult(1, 500, MAX_PROBABILITY - 1);
        KG_PROCESS_ERROR(!nRetCode);
    }

    for (int nIndex = 2; nIndex <= 6; nIndex++)
    {
        nRetCode = m_RandExtTest.RandomResult(nIndex, 500, MAX_PROBABILITY - 1);
        KG_PROCESS_ERROR(!nRetCode);
    }

    while (nAccount < 65535) //这么多次了，肯定哪里出了什么问题
    {
        nAccount++;
        nRetCode = m_RandExtTest.RandomResult(1, 500, MAX_PROBABILITY - 1);
        if (nRetCode)
            break;
    }

    KG_PROCESS_ERROR(nAccount == 5);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KRandDataExtTest::TestOverKey02()
{

    BOOL nRetCode    =   false;
    int nResult     =   false;
    int nAccount    =   0;


    //将变量删除并重新创建，保证数据的干净
    if (m_bInitExtFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);

    for (int nIndex = 0; nIndex < 4; nIndex++)
    {
        nRetCode = m_RandExtTest.RandomResult(1, 500, MAX_PROBABILITY - 1);
        KG_PROCESS_ERROR(!nRetCode);
    }

    for (int nIndex = 2; nIndex <= 6; nIndex++)
    {
        nRetCode = m_RandExtTest.RandomResult(nIndex, 500, MAX_PROBABILITY - 1);
        KG_PROCESS_ERROR(!nRetCode);
    }

    while (nAccount < 65535) //这么多次了，肯定哪里出了什么问题
    {
        nAccount++;
        nRetCode = m_RandExtTest.RandomResult(1, 500, MAX_PROBABILITY - 1);
        if (nRetCode)
            break;
    }

    KG_PROCESS_ERROR(nAccount == 5);

    nAccount = 0; // 清0计算下一步
    
    while (nAccount < 65535)
    {
        nAccount++;
        nRetCode = m_RandExtTest.RandomResult(3, 500, MAX_PROBABILITY - 1);
        if (nRetCode)
            break;
    }

    KG_PROCESS_ERROR(nAccount == 4);

    nResult = true;
Exit0:
    return nResult;
}

int KRandDataExtTest::DoTest(int nTestCase)
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    FILE *fpResult  =   NULL;

    fpResult = fopen("ExtTestResult.log", "a");
    KG_PROCESS_ERROR(fpResult);

    switch(nTestCase)
    {
    case rdetcSaveNothing:
        {
            nRetCode = TestSaveNothing();
            if (nRetCode)
            {
                fputs("存储0个数Key用例(24443)测试通过\n", fpResult);
            }
            else 
            {
                fputs("存储0个数Key用例(24443)测试失败\n", fpResult);
            }
            break;
        }
    case rdetcSaveOne:
        {
            nRetCode = TestSaveOne();
            if (nRetCode)
            {
                fputs("存储1个数Key用例(24444)测试通过\n", fpResult);
            }
            else 
            {
                fputs("存储1个数Key用例(24444)测试失败\n", fpResult);
            }
            break;
        }
    case rdetcSaveAll:
        {
            nRetCode = TestSaveAll();
            if (nRetCode)
            {
                fputs("存储满数据Key用例(24445)测试通过\n", fpResult);
            }
            else 
            {
                fputs("存储满数据Key用例(24445)测试失败\n", fpResult);
            }
            break;
        }
    case rdetcRandom02:
        {
            nRetCode = TestRandom(2);
            if (nRetCode)
            {
                fputs("掉落率为0.002的最坏情况用例(24453)测试通过\n", fpResult);
            }
            else 
            {
                fputs("掉落率为0.002的最坏情况用例(24453)测试失败\n", fpResult);
            }
            break;
        }
    case rdetcRandom10:
        {
            nRetCode = TestRandom(100);
            if (nRetCode)
            {
                fputs("掉落率为0.1的最坏情况用例(24454)测试通过\n", fpResult);
            }
            else 
            {
                fputs("掉落率为0.1的最坏情况用例(24454)测试失败\n", fpResult);
            }
            break;
        }
    case rdetcRandom20:
        {
            {
                nRetCode = TestRandom(200);
                if (nRetCode)
                {
                    fputs("掉落率为0.2的最坏情况用例(24455)测试通过\n", fpResult);
                }
                else 
                {
                    fputs("掉落率为0.2的最坏情况用例(24455)测试失败\n", fpResult);
                }
                break;
            }
        }
    case rdetcRandom50:
        {
            {
                nRetCode = TestRandom(500);
                if (nRetCode)
                {
                    fputs("掉落率为0.5的最坏情况用例(24456)测试通过\n", fpResult);
                }
                else 
                {
                    fputs("掉落率为0.5的最坏情况用例(24456)测试失败\n", fpResult);
                }
                break;
            }
        }
    case rdetcRandom100:
        {
            {
                nRetCode = TestRandom(1000);
                if (nRetCode)
                {
                    fputs("掉落率为1的最坏情况用例(24457)测试通过\n", fpResult);
                }
                else 
                {
                    fputs("掉落率为1的最坏情况用例(24457)测试失败\n", fpResult);
                }
                break;
            }
        }
    case rdetcGetItem01:
        {
            {
                nRetCode = TestGetItem01();
                if (nRetCode)
                {
                    fputs("成功时记数清空测试用例01(24449)测试通过\n", fpResult);
                }
                else 
                {
                    fputs("成功时记数清空测试用例01(24449)测试失败\n", fpResult);
                }
                break;
            }
        }
    case rdetcGetItem02:
        {
            {
                nRetCode = TestGetItem02();
                if (nRetCode)
                {
                    fputs("成功时记数清空测试用例02(24449)测试通过\n", fpResult);
                }
                else 
                {
                    fputs("成功时记数清空测试用例02(24449)测试失败\n", fpResult);
                }
                break;
            }
        }
    case rdetc2Key01:
        {
            {
                nRetCode = Test2Key01();
                if (nRetCode)
                {
                    fputs("多Key互不干扰测试用例01(24449)测试通过\n", fpResult);
                }
                else 
                {
                    fputs("多Key互不干扰测试用例01(24449)测试失败\n", fpResult);
                }
                break;
            }
        }
    case rdetc2Key02:
        {
            {
                nRetCode = Test2Key02();
                if (nRetCode)
                {
                    fputs("多Key互不干扰测试用例02(24449)测试通过\n", fpResult);
                }
                else 
                {
                    fputs("多Key互不干扰测试用例02(24449)测试失败\n", fpResult);
                }
                break;
            }
        }
    case rdetcOverKey01:
        {
            {
                nRetCode = TestOverKey01();
                if (nRetCode)
                {
                    fputs("满Key覆盖测试用例01(24449)测试通过\n", fpResult);
                }
                else 
                {
                    fputs("满Key覆盖测试用例01(24449)测试失败\n", fpResult);
                }
                break;
            }
        }
    case rdetcOverKey02:
        {
            {
                nRetCode = TestOverKey02();
                if (nRetCode)
                {
                    fputs("满Key覆盖测试用例02(24449)测试通过\n", fpResult);
                }
                else 
                {
                    fputs("满Key覆盖测试用例02(24449)测试失败\n", fpResult);
                }
                break;
            }
        }
    default:
        {
            fputs("输入错误", fpResult);
            break;
        }

    }

    nResult = true;
Exit0:
    if (fpResult)
    {
        fclose(fpResult);
        fpResult = NULL;
    }
    return nResult;
}

int main(int argc, char* argv[])
{   
    BOOL                 nRetCode    =   false;
    int                 nResult     =   false;
    BOOL                bInitFlag   =   false;

    KRandDataExtTest kRandTest;

    nRetCode = kRandTest.Init();
    KG_PROCESS_ERROR(nRetCode);
    bInitFlag = true;

    for (int nIndex = rdetcBegin + 1; nIndex < rdetcTotal; nIndex++)
    {
        nRetCode = kRandTest.DoTest(nIndex);
        KG_PROCESS_ERROR(nRetCode);
    }


    nResult = true;
Exit0:
    if (bInitFlag)
    {
        kRandTest.UnInit();
    }
    return nResult;
}

