/*****************************************************************************************
//	特别功能的一些基础函数
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-7-26
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../../Public/iRepresentShell.h"

struct KOutputTextParam;
//输出包含tab分格的字符串，可以是多行，返回值表示输出内容共几行
//nLineWidth 的单位是屏幕像素点
//KOutputTextParam::nSkipLine不支持
int OutputTabSplitText(const char* pBuffer, int nCount, int nLineWidth,
					   int nFontId, KOutputTextParam* pParam);
//eType:文件类型（bmp,jpg）
//nQuality:图象质量(0-100)，只对jpg有意义
void SaveScreenToFile(ScreenFileType eType = SCRFILETYPE_JPG, unsigned int nQuality = 100);
bool SetScrPicPath(const char* szPath);