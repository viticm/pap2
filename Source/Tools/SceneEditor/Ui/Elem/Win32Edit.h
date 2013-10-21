/*****************************************************************************************
//	界面窗口体系结构--Win32输入窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue), Freeway
//	CreateTime:	2002-12-17
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

HWND Win32Edit_Create(HWND hGameMainWnd, unsigned int uFlag, int nLimitLen);
void Win32Edit_UpdateSizePos(HWND hWnd, int nLeft, int nTop, int nWidth, int nHeight);
void Win32Edit_Destroy(HWND hWnd);
void Win32Edit_SetFocus(HWND hWnd, bool bFocus);
void Win32Edit_SetTextColor(HWND hWnd, unsigned int uColor);
bool Win32Edit_InsertString(HWND hWnd, const char* pString, int nLen);
void Win32Edit_SetText(HWND hWnd, const char* pszString);
int  Win32Edit_GetText(HWND hWnd, char* pBuffer, int nSize, bool bExcludeSpace);

int  Win32Edit_ShowWindow(HWND hWnd, BOOL bShow);
