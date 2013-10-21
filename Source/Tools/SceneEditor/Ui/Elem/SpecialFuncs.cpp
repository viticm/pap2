/*****************************************************************************************
//	特别功能的一些基础函数
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-7-26
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KEngine.h"
#include "Shlobj.h"
#include <time.h>
#include "../../Public/Text.h"
#include "SpecialFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;

#define	SCREEN_PIC_SAVE_PATH	"\\UserData\\PrintScreen\\"
static char szScrPicPath[MAX_PATH] = "";

//输出包含tab分格的字符串，可以是多行，返回值表示输出内容共几行
//不支持字符串串中的单行字符宽度，超出限定行宽的情况
//nLineWidth 的单位是屏幕像素点
int OutputTabSplitText(const char* pBuffer, int nCount,
			int nLineWidth, int nFontId,
			KOutputTextParam* pParam)
{
	if (pBuffer == NULL || nCount < 0 || g_pRepresentShell == NULL || nFontId <= 0)
		return 0;

	int	nOldX = pParam->nX;
	int nOldY = pParam->nY;
	int nOutputLineCount = 0;
	int	nCurrentPos = 0;

	pParam->nSkipLine = 0;
	int nMaxLineCount = pParam->nNumLine;
	pParam->nNumLine = 1;

	while(nCurrentPos < nCount && nOutputLineCount < nMaxLineCount)
	{
		int	nLineEndPos = TFindSpecialCtrlInEncodedText(pBuffer, nCount, nCurrentPos, KTC_ENTER);
		int	nTabPos = TFindSpecialCtrlInEncodedText(pBuffer, nCount, nCurrentPos, KTC_TAB);

		int nLen;

		//====输出tab前的部分，靠左对齐===
		if (nTabPos >= 0)
			nLen = nTabPos - nCurrentPos;
		else if (nLineEndPos >= 0)
			nLen = nLineEndPos - nCurrentPos;
		else
			nLen = nCount - nCurrentPos;

		g_pRepresentShell->OutputRichText(nFontId, pParam, pBuffer + nCurrentPos, nLen);

		//====输出tab后的部分，靠右对齐===
		nCurrentPos += nLen;
		if (nTabPos >= 0)
		{
			nCurrentPos ++;
			if (nLineEndPos >= 0)
				nLen = nLineEndPos - nCurrentPos;
			else
				nLen = nCount - nCurrentPos;
			if (nLen > 0)
			{
				int nOutputLen;
				TGetEncodedTextLineCount(pBuffer + nCurrentPos, nLen, 0, nOutputLen, nFontId);
				pParam->nX += nLineWidth - nOutputLen * nFontId / 2;
				g_pRepresentShell->OutputRichText(nFontId, pParam, pBuffer + nCurrentPos, nLen);
				pParam->nX = nOldX;
			}
			nCurrentPos += nLen;
		}
		pParam->nY += nFontId + 1;
		nOutputLineCount++;
	}

	pParam->nY = nOldY;
	return nOutputLineCount;
}

bool SetScrPicPath(const char* szPath)
{
	if(!szPath || !szPath[0] || strlen(szPath) >= MAX_PATH)
	{
		if(!SHGetSpecialFolderPath(NULL, szScrPicPath, CSIDL_DESKTOPDIRECTORY , false))
			return false;
		strcat(szScrPicPath,"\\剑侠截图\\");
	}
	else
	{
		int nLen = strlen(szPath);
		memcpy(szScrPicPath, szPath, nLen);
		if (szPath[nLen - 1] == '\\')
			szScrPicPath[nLen] = 0;
		else
		{
			szScrPicPath[nLen] = '\\';
			szScrPicPath[nLen + 1] = 0;
		}
	}

	return true;
}

//根据时间构造截屏文件名
bool GetAPrintScreenFileName(char *buf, int nBufLen)
{
	if(nBufLen < 128)
		return false;

	time_t ltime;
    struct tm *today;
	time( &ltime );
	today = localtime( &ltime );
	strftime( buf, 128, "%Y-%m-%d_%H-%M-%S", today );
	return true;
}

void SaveScreenToFile(ScreenFileType eType, unsigned int nQuality)
{
	if(!szScrPicPath[0] && !SetScrPicPath(NULL))
		return;

	// 字符串长度不能小于128
	char	szFile[256];
	int		nLen;
	nLen = sprintf(szFile, "%s", szScrPicPath);
	if(GetAPrintScreenFileName(&szFile[nLen], sizeof(szFile) - nLen))
	{
		g_CreatePath(szScrPicPath);

		if(eType == SCRFILETYPE_BMP)
			strcat(szFile, ".bmp");
		else
			strcat(szFile, ".jpg");

		g_pRepresentShell->SaveScreenToFile(szFile, eType, nQuality);
	}
}
