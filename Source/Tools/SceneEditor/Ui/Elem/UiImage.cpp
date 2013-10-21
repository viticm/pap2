/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-25
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "../../Public/KWin32.h"

#include "Windows.h"
#include "UiImage.h"
#include "../../Public/iRepresentShell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


extern unsigned int	l_Time;
//绘图设备
extern iRepresentShell*	g_pRepresentShell;

unsigned int IR_GetCurrentTime();

//--------------------------------------------------------------------------
//	功能：更新图形换帧计算用时钟
//--------------------------------------------------------------------------
void IR_UpdateTime();

//--------------------------------------------------------------------------
//	功能：初始化结构数据
//--------------------------------------------------------------------------
void IR_InitUiImageRef(KUiImageRef& Img)
{
	memset(&Img, 0, sizeof(KUiImageRef));
	Img.Color.Color_b.a = 255;
}

//--------------------------------------------------------------------------
//	功能：初始化结构数据
//--------------------------------------------------------------------------
void IR_InitUiImagePartRef(KUiImagePartRef& Img)
{
	memset(&Img, 0, sizeof(KUiImagePartRef));
	Img.Color.Color_b.a = 255;
}

int	IR_IsTimePassed(unsigned int uInterval, unsigned int& uLastTimer)
{
	if ((l_Time - uLastTimer) >= uInterval)
	{
		uLastTimer += uInterval;
		return 1;
	}
	return 0;
}

//获取剩余时间，如果时间已经到了/过了，返回值都为0
unsigned int IR_GetRemainTime(unsigned int uInterval, unsigned int uLastTimer)
{
	register unsigned int uRemain;
	if ((uRemain = l_Time - uLastTimer) < uInterval)
		return (uInterval - uRemain);
	return 0;
}

//--------------------------------------------------------------------------
//	功能：换帧计算
//--------------------------------------------------------------------------
int IR_NextFrame(KUiImageRef& Img)
{
	if (Img.nNumFrames > 1)
	{
		if ((l_Time - Img.nFlipTime) >= (DWORD)Img.nInterval)
		{
			Img.nFlipTime += Img.nInterval;
			if ((++Img.nFrame) >= Img.nNumFrames)
			{
				Img.nFlipTime = l_Time;
				Img.nFrame = 0;
				return true;
			}
		}
	}
	else if (Img.nNumFrames == 0)
	{
		KImageParam	Param;
		Param.nNumFrames = 0;
		if (g_pRepresentShell)
			g_pRepresentShell->GetImageParam(Img.szImage, &Param, Img.nType);
		if (Param.nNumFrames > 0)
		{
			Img.nFlipTime  = l_Time;
			Img.nNumFrames = Param.nNumFrames;
			Img.nInterval = Param.nInterval;
		}
		else
			Img.nNumFrames = 1;
		return false;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：设置绘制图的局部
//--------------------------------------------------------------------------
void IR_UpdateImagePart(KUiImagePartRef& Img, int nPartValue, int nFullValue)
{
	if (Img.Width == 0 && Img.szImage[0])
	{
		KImageParam	Param;
		Param.nWidth = 0;
		if (g_pRepresentShell)
			g_pRepresentShell->GetImageParam(Img.szImage, &Param, Img.nType);
		Img.Width = Param.nWidth;
		Img.Height = Param.nHeight;
		if (Img.Width == 0 || Img.Height == 0)
			Img.szImage[0] = 0;
	}
	if (nPartValue >= nFullValue)
	{
		Img.oImgLTPos.nX = Img.oImgLTPos.nY = 0;
		Img.oImgRBPos.nX = Img.Width;
		Img.oImgRBPos.nY = Img.Height;
		return;
	}
	if (nPartValue < 0)
	{
		Img.oImgLTPos.nX = Img.oImgLTPos.nY =
			Img.oImgRBPos.nX = Img.oImgRBPos.nY = 0;
		return;
	}

	switch(Img.nDivideFashion)
	{
	case  IDF_RIGHT_TO_LEFT:
		Img.oImgLTPos.nY = 0;
		Img.oImgRBPos.nX = Img.Width;
		Img.oImgRBPos.nY = Img.Height;
		if (nFullValue)
			Img.oImgLTPos.nX = Img.Width - Img.Width * nPartValue / nFullValue;
		else
			Img.oImgLTPos.nX = 0;
		break;
	case IDF_TOP_TO_BOTTOM:
		Img.oImgLTPos.nX = 0;
		Img.oImgLTPos.nY = 0;
		Img.oImgRBPos.nX = Img.Width;
		if (nFullValue)
			Img.oImgRBPos.nY = Img.Height * nPartValue / nFullValue;
		else
			Img.oImgRBPos.nY = Img.Height;
		break;
	case IDF_BOTTOM_TO_TOP:
		Img.oImgLTPos.nX = 0;
		Img.oImgRBPos.nX = Img.Width;
		Img.oImgRBPos.nY = Img.Height;
		if (nFullValue)
			Img.oImgLTPos.nY = Img.Height - Img.Height * nPartValue / nFullValue;
		else
			Img.oImgLTPos.nY = 0;
		break;
	default:	//include IDF_LEFT_TO_RIGHT
		Img.oImgLTPos.nX = 0;
		Img.oImgLTPos.nY = 0;
		Img.oImgRBPos.nY = Img.Height;
		if (nFullValue)
			Img.oImgRBPos.nX = Img.Width * nPartValue / nFullValue;
		else
			Img.oImgRBPos.nX = Img.Width;
		break;
	}
}

//--------------------------------------------------------------------------
//	功能：绘制图形
//--------------------------------------------------------------------------
void IR_DrawImage(KUiImageRef* pImg)
{
	if (g_pRepresentShell)
		g_pRepresentShell->DrawPrimitives(1, pImg, RU_T_IMAGE, true);
}

//--------------------------------------------------------------------------
//	功能：绘制图形的局部
//--------------------------------------------------------------------------
void IR_DrawImagePart(KUiImagePartRef* pImg)
{
	if (g_pRepresentShell)
		g_pRepresentShell->DrawPrimitives(1, pImg, RU_T_IMAGE_PART, true);
}

//--------------------------------------------------------------------------
//	功能：获得图形资源的KSprite对象
//--------------------------------------------------------------------------
int IR_GetSpritePixcelAlpha(KUiImageRef& Img, int h, int v)
{
	if (g_pRepresentShell)
		return g_pRepresentShell->GetImagePixelAlpha(Img.szImage, Img.nFrame, h, v, Img.nType);
	return 0;
}

//--------------------------------------------------------------------------
//	功能：获得图像参考点（一般所说为重心）
//--------------------------------------------------------------------------
void IR_GetReferenceSpot(KUiImageRef& Img, int& h, int& v)
{
	KImageParam	Param;

	if (g_pRepresentShell && g_pRepresentShell->GetImageParam(Img.szImage, &Param, Img.nType))
	{
		h = Param.nReferenceSpotX;
		v = Param.nReferenceSpotY;
	}
	else
	{
		h = 0;
		v = 0;
	}
}