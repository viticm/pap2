//////////////////////////////////////////////////////////////////////////////////////
//	文件名			:	TextPic.h
//	创建人			:	王西贝
//	创建时间		:	2003-6-22 23:01:40
//	文件说明		:	文字嵌入式图片
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __TEXTPIC_H__
#define __TEXTPIC_H__

#include "../../Public/Text.h"
#include "UiImage.h"
#include <vector>
using namespace std;

struct iRepresentShell;
class KInlinePicSink : public IInlinePicEngineSink
{
public:
	KInlinePicSink();
	BOOL Init(iRepresentShell*);
	BOOL UnInit();
	

protected:
	iRepresentShell* m_pRepShell;
	vector<KUiImageRef>	m_Images;
	vector<KImageParam> m_ImageParams;

	vector<KUiImageRef> m_CustomImages;
	vector<KImageParam> m_CustomImageParams;

public:
	//动态加载图片,获取一个WORD,即图片的索引
	long AddCustomInlinePic(unsigned short& wIndex, const char* szSprPathName);
	//动态卸载图片
	long RemoveCustomInlinePic(unsigned short wIndex);
	
	long GetPicSize(unsigned short wIndex, int& cx, int& cy);
	long DrawPic(unsigned short wIndex, int x, int y);
};

#endif //__TEXTPIC_H__
