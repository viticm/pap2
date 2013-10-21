/*
UiTex文件说明:
界面图像信息文件
UiTex文件头
Frame数据区 长度为  nFrameCount * sizeof(UITEXFRAMEDATASTRUCTURE)
Animate数据区 长度不定  nFrameCount FrameIndex...
*/


struct UITEXFILEHEADER           //UiTex文件头
{
    DWORD   dwUiTexFlag;          //UiTex文件标志
    int     nWidth;              //宽
    int     nHeight;             //长
    int     nFrameCount;         //帧数
    int     nAnimateCount;       //动画数
    DWORD   dwFileLen;           //文件长度
    char    szTgaFileName[32];   //STex文件名
    char    szBuffer[32];         //保留,也可以作为szTextFile的延长部分
};

struct UITEXFRAMEDATASTRUCTURE   //Frame数据块
{
    DWORD   dwInfo;              //Frame信息
    int     nLeft;               //左坐标
    int     nTop;                //上坐标
    int     nWidth;              //宽度
    int     nHeight;             //高度
};
