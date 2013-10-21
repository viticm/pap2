/*****************************************************************************************
//	界面窗口体系结构--显示图形局部的窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-25
------------------------------------------------------------------------------------------
	显示(spr)图形的窗口。
*****************************************************************************************/
#pragma once
#include "WndWindow.h"
#include "UiImage.h"

class KWndImagePart : public KWndWindow
{
public:
	KUiImagePartRef	m_Image;
protected:
	void			Clone(KWndImagePart* pCopy);
public:
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	//初始化
	virtual void	PaintWindow();								//窗体绘制
	void			SetPart(int nPartValue, int nFullValue);	//根据部分值与全值的比例关系设置绘制图的哪部分区域
	KWndImagePart();
};