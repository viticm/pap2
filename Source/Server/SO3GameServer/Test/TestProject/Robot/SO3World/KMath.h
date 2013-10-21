#ifndef KMathH
#define	KMathH

#include <math.h>
#include "Global.h"

int g_InitMath();
int g_UnInitMath();

void g_InitSeries();

#ifdef __linux
#define __cdecl
#endif
//---------------------------------------------------------------------------
#define PI 3.14159265358979323846264338327950288
//---------------------------------------------------------------------------
#define DEST_REGION_DISTANCE	8


extern int g_nDirection[REGION_GRID_WIDTH * DEST_REGION_DISTANCE * 2][REGION_GRID_HEIGHT * DEST_REGION_DISTANCE * 2];

template<typename T>
inline T g_GetDistance3(T nDistance2, T nSourceZ, T nDestZ)
{
	T nDistance3 = nDistance2 + (nSourceZ - nDestZ) * (nSourceZ - nDestZ);

	return nDistance3;
}

template<typename T>
inline T g_GetDistance3(T nSourceX, T nSourceY, T nSourceZ, T nDestX, T nDestY, T nDestZ)
{
	T nDistance3 = (nSourceX - nDestX) * (nSourceX - nDestX) + 
		(nSourceY - nDestY) * (nSourceY - nDestY) + (nSourceZ - nDestZ) * (nSourceZ - nDestZ);

	return nDistance3;
}

template<typename T>
inline T g_GetDistance2(T nSourceX, T nSourceY, T nDestX, T nDestY)
{
	T nDistance2 = (nSourceX - nDestX) * (nSourceX - nDestX) + 
					 (nSourceY - nDestY) * (nSourceY - nDestY);

	return nDistance2;
}

template<typename T>
inline T g_GetDirection(T nSourceX, T nSourceY, T nDestX, T nDestY)
{
	T nXDistance = nDestX - nSourceX;
	T nYDistance = nDestY - nSourceY;

	while (nXDistance >= REGION_GRID_WIDTH * DEST_REGION_DISTANCE ||
		nYDistance >= REGION_GRID_WIDTH * DEST_REGION_DISTANCE ||
		nXDistance < -REGION_GRID_WIDTH * DEST_REGION_DISTANCE ||
		nYDistance < -REGION_GRID_WIDTH * DEST_REGION_DISTANCE)
	{
		nXDistance = nXDistance / 2;
		nYDistance = nYDistance / 2;
	}

	nXDistance += REGION_GRID_WIDTH * DEST_REGION_DISTANCE;
	nYDistance += REGION_GRID_WIDTH * DEST_REGION_DISTANCE;

	return g_nDirection[nXDistance][nYDistance];
}



inline DWORD g_Dice(void)
{
	return g_Random(DICE_MAX - DICE_MIN + 1) + DICE_MIN;
}

//---------------------------------------------------------------------------
#define SIN_COS_NUMBER	4096
extern int		*g_nSin;
extern int		*g_nCos;

inline int g_Sin(int nDirection)
{
    nDirection = nDirection % DIRECTION_COUNT;

    if (nDirection < 0)
        nDirection += DIRECTION_COUNT;

    return g_nSin[nDirection];
}

inline int g_Cos(int nDirection)
{
    nDirection = nDirection % DIRECTION_COUNT;

    if (nDirection < 0)
        nDirection += DIRECTION_COUNT;

    return g_nCos[nDirection];
}

// 正弦余弦的查表函数代码缓冲区
extern unsigned char *g_InternalDirSinCosCode;

typedef int	__cdecl g_InternalDirSinCosFunction(int pSinCosTable[], int nDir, int nMaxDir);

inline int g_DirSin(int nDir, int nMaxDir)
{
    return (*(g_InternalDirSinCosFunction *)(&(g_InternalDirSinCosCode[0])))(g_nSin, nDir, nMaxDir);
}


inline int g_DirCos(int nDir, int nMaxDir)
{
    return (*(g_InternalDirSinCosFunction *)(&(g_InternalDirSinCosCode[0])))(g_nCos, nDir, nMaxDir);
}

inline BOOL g_RandPercent(int nPercent)
{
	return ((int)g_Random(100) < nPercent);
}

#endif //KMathH
