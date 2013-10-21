///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KRandDataTest.h
// Creator  : CaiYuTao
// Date     : 2009-08-18
// Comment  : 河蟹社会发生器--单元测试
//
///////////////////////////////////////////////////////////////

#define MILLION_STEP    1024
#define TEST_TIME       1000 * 1000

enum KRandDataTestCase 
{
    rdtcInvalid = -1,

    rdtcBegin,

    rdtcWrongZero,
    rdtcWrongHundred,
    rdtcNormal,
    rdtcSmall,

    rdtcTotal
};



class KRandDataAutoTest
{
public:
    KRandDataAutoTest() {m_bInitFlag = false; m_bInitSmallFlag = false;}
    int Init(void);
    void UnInit(void);
    int DoTest(int nTestCase);

private:
    BOOL TestWrongZero();
    BOOL TestWrongHundred();
    BOOL TestNormal();
    BOOL TestSmall();

    KProbability<int, MILLION_NUM, 5>            m_RandTest; 
    KProbability<int, 100, 5>                    m_RandTestSmall;
    BOOL m_bInitFlag;
    BOOL m_bInitSmallFlag;
};