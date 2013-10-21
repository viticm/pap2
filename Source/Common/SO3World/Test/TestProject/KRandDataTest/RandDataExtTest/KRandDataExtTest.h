///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KRandDataTest.h
// Creator  : CaiYuTao
// Date     : 2009-08-18
// Comment  : 河蟹社会发生器--单元测试修改用
//
///////////////////////////////////////////////////////////////

#define MAX_PROBABILITY     1000
#define MAX_KEYNUM          5
#define MAX_BUFFERLENTGH    262155

enum KRandDataExtTestCase 
{
    rdetcInvalid = -1,

    rdetcBegin,

    rdetcSaveNothing,
    rdetcSaveOne,
    rdetcSaveAll,

    rdetcRandom02,
    rdetcRandom10,
    rdetcRandom20,
    rdetcRandom50,
    rdetcRandom100,

    rdetcGetItem01,
    rdetcGetItem02,

    rdetc2Key01,
    rdetc2Key02,

    rdetcOverKey01,
    rdetcOverKey02,


    rdetcTotal
};

class KRandDataExtTest
{
public:
    KRandDataExtTest() {m_bInitExtFlag = false;}
    int Init(void);
    void UnInit(void);
    int DoTest(int nTestCase);

private:
    BOOL TestSaveNothing();
    BOOL TestSaveOne();
    BOOL TestSaveAll();
    BOOL TestRandom(DWORD nProbabilityValue);
    BOOL TestGetItem01();
    BOOL TestGetItem02();
    BOOL Test2Key01();
    BOOL Test2Key02();
    BOOL TestOverKey01();
    BOOL TestOverKey02();


    KProbability<int, MAX_PROBABILITY, MAX_KEYNUM>            m_RandExtTest; 
    BOOL m_bInitExtFlag;
};