#include "stdafx.h"
#include "KMath.h"

#include <math.h>

//#define	_USBKEY

#ifdef _USBKEY
    #include "USBKey/epsJO.h" 

//#ifndef WIN32
//	#include "USBKey/epsJO.cpp"
//	#include "USBKey/des.cpp"
//#endif
    //---------------------------------------------------------------------------
    // 正弦表 (将浮点数 *1024 整型化)
    int		*g_nSin = NULL;

    // 余弦表 (将浮点数 *1024 整型化)
    int		*g_nCos = NULL;

    // 正弦余弦的查表函数代码缓冲区
    unsigned char *g_InternalDirSinCosCode = NULL;


    //---------------------------------------------------------------------------
    // 五行相生相克

    // 五行相生相克函数代码缓冲区
    unsigned char *g_InternalIsAccrueConquerCode = NULL;
    //---------------------------------------------------------------------------

    int g_nEPJInitFlag = false;

    // {982CD92C-027E-4638-8BD4-5855FFB6DCC1}
	const unsigned char g_cExeData[16] = { 
        0x98, 0x2c, 0xd9, 0x2c, 0x02, 0x7e, 0x46, 0x38, 
        0x8b, 0xd4, 0x58, 0x55, 0xff, 0xb6, 0xdc, 0xc1 
    };


    int _ReadDataFromUSBKey(int nSize, unsigned char abyBuffer[], int *pnRetSize, int nIndex)
    {
        int nResult  = false;
        int nRetCode = false;

		int   nReadSize = 0;
		unsigned char *pbyReadBuf = NULL;

        if (pnRetSize)
            *pnRetSize = 0;

        if (nSize <= 0)
            goto Exit0;

        if (!abyBuffer)
            goto Exit0;
        
        if (nIndex < 0)
            goto Exit0;

		nRetCode = epj_ReadKeyCode(
            (const char *)g_cExeData, sizeof(g_cExeData),
            (char **)(&pbyReadBuf), &nReadSize,
            nIndex
        );
        if (!nRetCode)
            goto Exit0;
        
        if (!pbyReadBuf)
            goto Exit0;

        if (nReadSize <= 0)
            goto Exit0;

        if (nReadSize > nSize)
            goto Exit0;

        memcpy(abyBuffer, pbyReadBuf, nReadSize);

        if (pnRetSize)
            *pnRetSize = nReadSize;

        free(pbyReadBuf);
        pbyReadBuf = NULL;
        nReadSize  = 0;

        nResult = true;
    Exit0:
        return nResult;
    }

    int g_InitMath()
    {
        int nResult = false;
        int nRetCode = false;

        if (g_nEPJInitFlag)
            goto Exit1;

        g_nSin = new int [1024 / sizeof(int)];
		ADD_CODEPOINT(g_nSin);
        if (!g_nSin)
            goto Exit0;

        g_nCos = new int [1024 / sizeof(int)];
		ADD_CODEPOINT(g_nCos);
        if (!g_nCos)
            goto Exit0;

        g_InternalDirSinCosCode = new unsigned char [1024];
		ADD_CODEPOINT(g_InternalDirSinCosCode);
        if (!g_InternalDirSinCosCode)
            goto Exit0;

        g_InternalIsAccrueConquerCode = new unsigned char [1024];
		ADD_CODEPOINT(g_InternalIsAccrueConquerCode);
        if (!g_InternalIsAccrueConquerCode)
            goto Exit0;

        // 这个时候开始从USBKey中读入相应的数据
	    nRetCode = epj_Init("rockey");
        if (!nRetCode)
            goto Exit0;

        g_nEPJInitFlag = true;

        nRetCode = _ReadDataFromUSBKey(
            1024, (unsigned char *)g_nSin, NULL, 
            0
        );
        if (!nRetCode)
            goto Exit0;

        nRetCode = _ReadDataFromUSBKey(
            1024, (unsigned char *)g_nCos, NULL, 
            1
        );
        if (!nRetCode)
            goto Exit0;
        
        nRetCode = _ReadDataFromUSBKey(
            1024, (unsigned char *)g_InternalDirSinCosCode, NULL, 
            2
        );
        if (!nRetCode)
            goto Exit0;
        

        nRetCode = _ReadDataFromUSBKey(
            1024, (unsigned char *)g_InternalIsAccrueConquerCode, NULL, 
            3
        );
        if (!nRetCode)
            goto Exit0;

    Exit1:
        nResult = true;
    Exit0:
        if (!nResult)
        {
            if (g_nEPJInitFlag)
            {
                epj_UnInit();
                g_nEPJInitFlag = false;
            }

            if (g_InternalIsAccrueConquerCode)
            {
				DEL_CODEPOINT(g_InternalIsAccrueConquerCode);
                delete[] g_InternalIsAccrueConquerCode;
                g_InternalIsAccrueConquerCode = NULL;
            }

            if (g_InternalDirSinCosCode)
            {
				DEL_CODEPOINT(g_InternalDirSinCosCode);
                delete[] g_InternalDirSinCosCode;
                g_InternalDirSinCosCode = NULL;
            }

            if (g_nCos)
            {
				DEL_CODEPOINT(g_nCos);
                delete[] g_nCos;
                g_nCos = NULL;
            }

            if (g_nSin)
            {
				DEL_CODEPOINT(g_nSin);
                delete[] g_nSin;
                g_nSin = NULL;
            }

			printf(">>> usbkey not found or driver install failed!\n");
        }

        return nResult;
    }

    int g_UnInitMath()
    {
        int nResult = false;

        if (g_nEPJInitFlag)
        {
            epj_UnInit();
            g_nEPJInitFlag = false;
        }


        if (g_InternalIsAccrueConquerCode)
        {
			DEL_CODEPOINT(g_InternalIsAccrueConquerCode);
            delete[] g_InternalIsAccrueConquerCode;
            g_InternalIsAccrueConquerCode = NULL;
        }

        if (g_InternalDirSinCosCode)
        {
			DEL_CODEPOINT(g_InternalDirSinCosCode);
            delete[] g_InternalDirSinCosCode;
            g_InternalDirSinCosCode = NULL;
        }

        if (g_nCos)
        {
			DEL_CODEPOINT(g_nCos);
            delete[] g_nCos;
            g_nCos = NULL;
        }

        if (g_nSin)
        {
			DEL_CODEPOINT(g_nSin);
            delete[] g_nSin;
            g_nSin = NULL;
        }

        nResult = true;
    //Exit0:
        return nResult;
    }



#else
// 不需要加载USBKey

	int g_nDirection[REGION_GRID_WIDTH * DEST_REGION_DISTANCE * 2][REGION_GRID_HEIGHT * DEST_REGION_DISTANCE * 2];
	int g_nDistance[REGION_GRID_WIDTH * DEST_REGION_DISTANCE * 2][REGION_GRID_HEIGHT * DEST_REGION_DISTANCE * 2];

    // 正弦表 (将浮点数 *SIN_COS_NUMBER 整型化)
    int g_nSinBuffer[DIRECTION_COUNT];
    //---------------------------------------------------------------------------
    // 余弦表 (将浮点数 *SIN_COS_NUMBER 整型化)
    int	g_nCosBuffer[DIRECTION_COUNT];

	// 正弦余弦的查表函数代码缓冲区
    unsigned char g_InternalDirSinCosCodeBuffer[] =
    {
        0x8B, 0x44, 0x24, 0x08, 0x85, 0xC0, 0x7C, 0x16, 
        0x8B, 0x4C, 0x24, 0x0C, 0x3B, 0xC1, 0x7D, 0x0E, 
        0xC1, 0xE0, 0x06, 0x99, 0xF7, 0xF9, 0x8B, 0x4C, 
        0x24, 0x04, 0x8B, 0x04, 0x81, 0xC3, 0x83, 0xC8, 
        0xFF, 0xC3
    };

    // 五行相生相克函数代码缓冲区
    unsigned char g_InternalIsAccrueConquerCodeBuffer[] =
    {
        0x8B, 0x44, 0x24, 0x08, 0x85, 0xC0, 0x7C, 0x1E, 
        0x83, 0xF8, 0x05, 0x7D, 0x19, 0x8B, 0x54, 0x24, 
        0x0C, 0x56, 0x8B, 0x74, 0x24, 0x08, 0x57, 0x33, 
        0xC9, 0x8B, 0x3C, 0x86, 0x3B, 0xD7, 0x5F, 0x0F, 
        0x94, 0xC1, 0x8B, 0xC1, 0x5E, 0xC3, 0x33, 0xC0, 
        0xC3
    };

    //---------------------------------------------------------------------------
    // 正弦表 (将浮点数 *1024 整型化)
    int		*g_nSin = g_nSinBuffer;

    // 余弦表 (将浮点数 *1024 整型化)
    int		*g_nCos = g_nCosBuffer;

    // 正弦余弦的查表函数代码缓冲区
    unsigned char *g_InternalDirSinCosCode = g_InternalDirSinCosCodeBuffer;


    //---------------------------------------------------------------------------
    // 五行相生相克

    // 五行相生相克函数代码缓冲区
    unsigned char *g_InternalIsAccrueConquerCode = g_InternalIsAccrueConquerCodeBuffer;
    //---------------------------------------------------------------------------

    int g_InitMath()
    {
		//矢量角度变换表的初始化
//		printf("\n");
		for (int nY = REGION_GRID_HEIGHT * DEST_REGION_DISTANCE * 2 - 1; nY >= 0; nY--)
		{
			for (int nX = 0; nX < REGION_GRID_WIDTH * DEST_REGION_DISTANCE * 2; nX++)
			{
				int nRealX = nX - REGION_GRID_WIDTH * DEST_REGION_DISTANCE;
				int nRealY = nY - REGION_GRID_HEIGHT * DEST_REGION_DISTANCE;

				//第一象限
				if (nRealX >= 0 && nRealY >=0)
				{
					g_nDirection[nX][nY] = (int)(atan((double)nRealY / nRealX) * 180 / PI * DIRECTION_COUNT / 360);
				}
				//第二象限
				else if (nRealX < 0 && nRealY >=0)
				{
					g_nDirection[nX][nY] = (int)((180 + atan((double)nRealY / nRealX) * 180 / PI) * DIRECTION_COUNT / 360);
				}
				//第三象限
				else if (nRealX < 0 && nRealY < 0)
				{
					g_nDirection[nX][nY] = (int)((180 + atan((double)nRealY / nRealX) * 180 / PI) * DIRECTION_COUNT / 360);
				}
				//第四象限
				else if (nRealX >=0 && nRealY < 0)
				{
					g_nDirection[nX][nY] = (int)((360 + atan((double)nRealY / nRealX) * 180 / PI) * DIRECTION_COUNT / 360);
				}

				if (nRealX == 0 && nRealY == 0)
				{
					g_nDirection[nX][nY] = 0;
				}
//				printf("%4d", g_nDirection[nX][nY]);
			}
//			printf("\n");
		}

		//两点距离表的初始化
		for (int nY = REGION_GRID_HEIGHT * DEST_REGION_DISTANCE * 2 - 1; nY >= 0; nY--)
		{
			for (int nX = 0; nX < REGION_GRID_WIDTH * DEST_REGION_DISTANCE * 2; nX++)
			{
				int nRealX = nX - REGION_GRID_WIDTH * DEST_REGION_DISTANCE;
				int nRealY = nY - REGION_GRID_HEIGHT * DEST_REGION_DISTANCE;
				g_nDistance[nX][nY] = (int)sqrt((double)nRealX * nRealX + nRealY * nRealY);
			}
		}

		//三角函数值的初始化
		for (int nIndex = 0; nIndex < DIRECTION_COUNT; nIndex++)
		{
			g_nSinBuffer[nIndex] = (int)floor(sin((double)nIndex * PI / (DIRECTION_COUNT / 2)) * SIN_COS_NUMBER + 0.5);
			g_nCosBuffer[nIndex] = (int)floor(cos((double)nIndex * PI / (DIRECTION_COUNT / 2)) * SIN_COS_NUMBER + 0.5);
//			printf("%10d %10d %10d\n", nIndex, g_nSinBuffer[nIndex], g_nCosBuffer[nIndex]);
		}

		g_nSin = g_nSinBuffer;
		g_nCos = g_nCosBuffer;

        return true;
    }

    int g_UnInitMath()
    {
        return true;
    }

#endif

