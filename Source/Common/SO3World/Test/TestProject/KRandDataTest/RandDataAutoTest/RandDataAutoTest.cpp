// RandDataAutoTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KProbability.h"
#include "KRandDataTest.h"
#include <map>

int KRandDataAutoTest::Init()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    
    nRetCode = m_RandTest.Init();
    KG_PROCESS_ERROR(nRetCode);
    m_bInitFlag = true;

    nRetCode = m_RandTestSmall.Init();
    KG_PROCESS_ERROR(nRetCode);
    m_bInitSmallFlag = true;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (m_bInitSmallFlag)
        {
            m_RandTestSmall.UnInit();
            m_bInitSmallFlag = false;
        }
        if (m_bInitFlag)
        {
            m_RandTest.UnInit();
            m_bInitFlag = false;
        }
    }
    return nResult;
}


void KRandDataAutoTest::UnInit()
{
    m_RandTest.UnInit();
    m_bInitFlag = false;

    m_RandTestSmall.UnInit();
    m_bInitSmallFlag = false;
}

BOOL KRandDataAutoTest::TestWrongZero()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    int nAccount    =   0;

    if (m_bInitFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);
    //将变量删除并重新创建，保证数据的干净
    
    while (nAccount < 65535)        //跑尽量多的次数，避免随机情况的出现。
    {
        nAccount++;
        nRetCode = m_RandTest.RandomResult(1, 0);
        KG_PROCESS_ERROR(!nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

BOOL KRandDataAutoTest::TestWrongHundred()
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    int nAccount    =   0;

    if (m_bInitFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);
    //将变量删除并重新创建，保证数据的干净

    while (nAccount < 65535)        //跑尽量多的次数，避免随机情况的出现。
    {
        nAccount++;
        nRetCode = m_RandTest.RandomResult(1, MILLION_NUM);
        KG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}
BOOL KRandDataAutoTest::TestSmall()
{
    BOOL     nRetCode    =   false;
    int     nResult     =   false;
    int     nAccount    =   0;
    int     nGetItemNum =   10;
    FILE    *pResult   =   NULL;
    char    szFileName[MAX_PATH];

    map< int, int > mapGetItemIndex;
    map< int, int >::iterator iter;


    if (m_bInitFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);

    //使用一个循环来进行各个区间的测试。期望的掉落率从1到99（即1%到99%)，以1024为区间来记录数据。

    for (int nIndex = 1; nIndex < 100; nIndex++)
    {

        mapGetItemIndex.clear();

        snprintf(szFileName, sizeof(szFileName), "DropSmallDistribute%d.tab", nIndex);
        szFileName[sizeof(szFileName) -1] = '\0';

        pResult = fopen(szFileName, "a");
        KG_PROCESS_ERROR(pResult);

        for (int nNum = 0; nNum < TEST_TIME; nNum++)
        {
            nAccount = 0;
            nGetItemNum = 1;
            while(true)
            {
                nAccount++;
                nRetCode = m_RandTestSmall.RandomResult(nIndex, nIndex);
                if (nRetCode)
                {
                    nGetItemNum--;
                    if (nGetItemNum == 0)
                    {
                        break;
                    }
                }
            }
            iter = mapGetItemIndex.find(nAccount);
            if (iter!= mapGetItemIndex.end())
            {
                iter->second = iter->second + 1;
            }
            else
            {
                mapGetItemIndex.insert(pair< int, int >(nAccount, 1));
            }

            if (m_bInitFlag)
            {
                UnInit();
            }

            nRetCode = Init();
            KG_PROCESS_ERROR(nRetCode);
        }

        //结果输出
        for (iter = mapGetItemIndex.begin(); iter != mapGetItemIndex.end(); iter++)
        {
            fprintf(pResult, "%d\t%d\n", iter->first, iter->second);
        }
        fclose(pResult);
        pResult = NULL;
    }

    nResult = true;
Exit0:

    if (pResult)
    {
        fclose(pResult);
        pResult = NULL;
    }
    return nResult;
}

BOOL KRandDataAutoTest::TestNormal()
{
    BOOL     nRetCode    =   false;
    int     nResult     =   false;
    int     nAccount    =   0;
    int     nGetItemNum =   10;
    FILE    *pResult   =   NULL;
    char    szFileName[MAX_PATH];
    
    map< int, int > mapGetItemIndex;
    map< int, int >::iterator iter;
    

    if (m_bInitFlag)
    {
        UnInit();
    }

    nRetCode = Init();
    KG_PROCESS_ERROR(nRetCode);

    //使用一个循环来进行各个区间的测试。期望的掉落率从1024到MILLION_NUM（即0.1%到99.9%)，以1024为区间来记录数据。

    for (int nIndex = MILLION_STEP; nIndex < MILLION_NUM; nIndex= nIndex + MILLION_STEP)
    {
        
        mapGetItemIndex.clear();

        snprintf(szFileName, sizeof(szFileName), "DropDistribute%d.tab", nIndex);
        szFileName[sizeof(szFileName) -1] = '\0';

        pResult = fopen(szFileName, "a");
        KG_PROCESS_ERROR(pResult);

        for (int nNum = 0; nNum < TEST_TIME; nNum++)
        {
            if (!(nNum % 100000))
            {
                printf("TestTime: %d\n", nNum);
            }
            nAccount = 0;
            nGetItemNum = 1;
            while(true)
            {
                nAccount++;
                nRetCode = m_RandTest.RandomResult(nIndex, nIndex);
                if (nRetCode)
                {
                    nGetItemNum--;
                    if (nGetItemNum == 0)
                    {
                        break;
                    }
                }
            }
            iter = mapGetItemIndex.find(nAccount);
            if (iter!= mapGetItemIndex.end())
            {
                iter->second = iter->second + 1;
            }
            else
            {
                mapGetItemIndex.insert(pair< int, int >(nAccount, 1));
            }

            if (m_bInitFlag)
            {
                UnInit();
            }

            nRetCode = Init();
            KG_PROCESS_ERROR(nRetCode);
        }

        //结果输出
        for (iter = mapGetItemIndex.begin(); iter != mapGetItemIndex.end(); iter++)
        {
            fprintf(pResult, "%d\t%d\n", iter->first, iter->second);
        }

        fclose(pResult);
        pResult = NULL;
    }

    nResult = true;
Exit0:

    if (pResult)
    {
        fclose(pResult);
        pResult = NULL;
    }
    return nResult;
}


int KRandDataAutoTest::DoTest(int nTestCase)
{
    BOOL nRetCode    =   false;
    int nResult     =   false;
    FILE *fpResult  =   NULL;
    
    fpResult = fopen("AutoTestResult.log", "a");
    KG_PROCESS_ERROR(fpResult);

    switch(nTestCase)
    {
    case rdtcWrongZero:
        {
            nRetCode = TestWrongZero();
            if (nRetCode)
            {
                fputs("掉落率为0用例(24451)测试通过\n", fpResult);
            }
            else 
            {
                fputs("掉落率为0用例(24451)测试失败\n", fpResult);
            }
            break;
        }
    case rdtcWrongHundred:
        {
            nRetCode = TestWrongHundred();
            if (nRetCode)
            {
                fputs("掉落率超过百分之百用例(24452)测试成功\n", fpResult);
            }
            else 
            {
                fputs("掉落率超过百分之百用例(24452)测试失败\n", fpResult);
            }
            break;
        }
    case rdtcNormal:
        {
            {
                nRetCode = TestNormal();
                if (nRetCode)
                {
                    fputs("常规掉落测试用例(24449)中最大概率100W测试通过\n", fpResult);
                }
                else 
                {
                    fputs("常规掉落测试用例(24449)中最大概率100W测试失败\n", fpResult);
                }
                break;
            }
        }
    case rdtcSmall:
        {
            {
                nRetCode = TestSmall();
                if (nRetCode)
                {
                    fputs("常规掉落测试用例(24449)中最大概率100测试通过\n", fpResult);
                }
                else 
                {
                    fputs("常规掉落测试用例(24449)中最大概率100测试失败\n", fpResult);
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
    BOOL                nRetCode    =   false;
    int                 nResult     =   false;
    BOOL                bInitFlag   =   false;

    KRandDataAutoTest   kTest;
    
    nRetCode = kTest.Init();
    KG_PROCESS_ERROR(nRetCode);
    bInitFlag = true;

    for (int nIndex = rdtcBegin + 1; nIndex < rdtcTotal; nIndex++)
    {
        nRetCode = kTest.DoTest(nIndex);
        KG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    if (bInitFlag)
    {
        kTest.UnInit();
    }
	return nResult;
}

