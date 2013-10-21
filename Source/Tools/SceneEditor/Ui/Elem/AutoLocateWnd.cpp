/*****************************************************************************************
//	窗口自动定位
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-2-17
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "Wnds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


#define	CURSOR_HEIGHT		32

void ALW_GetWndPosition(int& nX, int& nY, int nWidth, int nHeight,
						bool bUseInputParamPos/* = false*/,
						bool bIgnoreCursorPos/* = false*/)
{
	int nSWidth, nSHeight;
	if (bUseInputParamPos == false)
		Wnd_GetCursorPos(&nX, &nY);
	Wnd_GetScreenSize(nSWidth, nSHeight);

	nX -= nWidth / 2;
	if (nX + nWidth > nSWidth)
		nX = nSWidth - nWidth;
	if (nX < 0)
		nX = 0;

	int nCentrePos;
	if (bIgnoreCursorPos == false)
		nCentrePos = (nSHeight - CURSOR_HEIGHT) / 2;
	else
		nCentrePos = nSHeight / 2;
	if (nY > nCentrePos)
		nY -= nHeight;
	else if (bIgnoreCursorPos == false)
		nY += CURSOR_HEIGHT;

	if (nY + nHeight > nSHeight)
		nY = nSHeight - nHeight;
	if (nY < 0)
		nY = 0;
}