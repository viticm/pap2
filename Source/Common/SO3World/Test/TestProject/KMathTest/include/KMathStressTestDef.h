///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KMathStressTestDef
// Creator  : ChenShiCai
// Date     : 2008-10-27
// Comment  : 
//
///////////////////////////////////////////////////////////////

#ifndef _KMATHSTRESSTESTDEF_
#define _KMATHSTRESSTESTDEF_

#define SERVER_IP       "192.168.24.53"
#define SERVER_PORT     8030

#define KMATH_TEST_ARRAYSIZE (1024 * 1024)

#define NEW_VERSION 120
#define OLD_VERSION 116

enum CommandType
{
    CT_Random = 1,
    CT_GetDistance3,
    CT_GetDistance2,
    CT_GetDirection,
    CT_InRange,
    CT_Sin,
    CT_Result
};

typedef struct tag_COMMAND
{
    int nCommand;
    int nNum;
    char chData[0];
}C2S_COMMAND, S2C_COMMAND;

struct g_GetDistance3_Para
{
    int nSourceX;
    int nSourceY; 
    int nSourceZ; 
    int nDestX; 
    int nDestY;
    int nDestZ;
};

struct g_GetDistance2_Para
{
    int nSourceX;
    int nSourceY;
    int nDestX;
    int nDestY;
};

struct g_GetDirection_Para
{
    int nX; 
    int nY;
};

struct g_InRange_Para
{
    int nXa;
    int nYa;
    int nZa;
    int nXb;
    int nYb;
    int nZb;
    int nMinRange;
    int nMaxRange;
};

struct g_Sin_Para
{
    int nDirection;
};

struct Test_Result
{
    int nVersion;
    unsigned uGetDistance3Time;
    unsigned uGetDistance2Time;
    unsigned uGetDirectionTime;
    unsigned uInRangeTime;
    unsigned uSinTime;
};

#endif//_KMATHSTRESSTESTDEF_

