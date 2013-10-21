//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTabFile.h
// Date:	2002.02.18
// Code:	Spe
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KTabFile_H
#define KTabFile_H

#include "KMemClass.h"
#include "KITabFile.h"
//---------------------------------------------------------------------------
typedef struct tagTabOffset
{
	DWORD		dwOffset;
	DWORD		dwLength;
} TABOFFSET;

class ENGINE_API KTabFile:public KITabFile
{
private:
	int			m_Width;
	int			m_Height;
	KMemClass	m_Memory;
	KMemClass	m_OffsetTable;
private:
	void		CreateTabOffset();
	BOOL		GetValue(int nRow, int nColumn, LPSTR lpRString, DWORD dwSize);
	int			Str2Col(LPSTR szColumn);
public:
	KTabFile();
	~KTabFile();
	BOOL		Load(LPSTR FileName);
	BOOL		Save(LPSTR FileName){return FALSE;}; //无法保存
	BOOL		LoadPack(LPSTR FileName);
	int			FindRow(LPSTR szRow);//返回以1为起点的值
	int			FindColumn(LPSTR szColumn);//返回以1为起点的值
	void		Col2Str(int nCol, LPSTR szColumn);
	int			GetWidth() { return m_Width;};
	int			GetHeight() { return m_Height;};
	BOOL		GetString(int nRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, BOOL bColumnLab = TRUE);
	BOOL		GetString(int nRow, int nColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize);
	BOOL		GetString(LPSTR szRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize);
	BOOL		GetInteger(int nRow, LPSTR szColumn, int nDefault, int *pnValue, BOOL bColumnLab = TRUE);
	BOOL		GetInteger(int nRow, int nColumn, int nDefault, int *pnValue);
	BOOL		GetInteger(LPSTR szRow, LPSTR szColumn, int nDefault, int *pnValue);
	BOOL		GetFloat(int nRow, LPSTR szColumn, float fDefault, float *pfValue, BOOL bColumnLab = TRUE);
	BOOL		GetFloat(int nRow, int nColumn, float fDefault, float *pfValue);
	BOOL		GetFloat(LPSTR szRow, LPSTR szColumn, float fDefault, float *pfValue);
	void		Clear();
};
//---------------------------------------------------------------------------
#endif
