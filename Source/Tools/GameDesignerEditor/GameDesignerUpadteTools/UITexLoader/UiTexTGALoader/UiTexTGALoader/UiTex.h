////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiTex.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-17 13:45:25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UITEX_H_
#define _INCLUDE_UITEX_H_

////////////////////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////////////////////

/*
UiTex文件说明:
界面图像信息文件
UiTex文件头
Frame数据区 长度为  nFrameCount * sizeof(UITEXFRAMEDATASTRUCTURE)
Animate数据区 长度不定  nFrameCount FrameIndex...
*/

struct UITEXFILEHEADER           //UiTex文件头
{
    DWORD   dwUiTexFlag;         //UiTex文件标志
    int     nWidth;              //宽
    int     nHeight;             //长
    int     nFrameCount;         //帧数
    int     nAnimateCount;       //动画数
    DWORD   dwFileLen;           //文件长度
    char    szTgaFileName[32];   //Tga文件名
    char    szBuffer[32];        //保留,也可以作为Tga的延长部分
};

struct UITEXFRAMEDATASTRUCTURE   //Frame数据块
{
    DWORD   dwInfo;              //Frame信息
    int     nLeft;               //左坐标
    int     nTop;                //上坐标
    int     nWidth;              //宽度
    int     nHeight;             //高度
};

struct TXTFILEDATA
{
	DWORD   dwInfo;					//Frame信息
	int		nFrameNum;				//UITex文件内动画序号
	int		nTextNum;				//Icon文件内动画序号
	int     nLeft;					//左
	int     nTop;					//上
	int     nWidth;					//宽
	int     nHeight;				//长
	int		nNewLeft;				//icon文件重排后的左
	int		nNewTop;				//icon文件重排后的上
	char	szTxtFile[MAX_PATH];	//路径
};

#define MAX_TGA_BLOCK_NAME_LEN      32
#define KEEP_ORIGINAL_INDEX         0x00000001
#define HAS_AREADY_ANIMATE_GROUP    0x00000002
#define KEEP_POS_WHEN_AUTO_MAKE_UP  0x00000004  
#define HASE_BEEEN_SET_POS          0x00000008


#endif //_INCLUDE_UITEX_H_
