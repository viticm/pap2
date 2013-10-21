
#ifndef _FONT_ADJUST_H_
#define _FONT_ADJUST_H_

#include "./font.h"
#include "./fontrenderer.h"

class KFontAdjust
{
public:
	KFontAdjust();
	virtual ~KFontAdjust();

	void Init(LPCTSTR pcszFontFile, unsigned int uFontSize, unsigned int uDpi);
	void Exit();

	void ReLoad();

	Font* GetFont();

	void AddControl1();
	void DecControl1();
	void SetControl1(float fValue);
	float GetControl1(){ return m_fValue1; };

	void AddControl2();
	void DecControl2();
	void SetControl2(float fValue);
	float GetControl2(){ return m_fValue2; };

	void AddDpi();
	void DecDpi();
	void SetDpi(unsigned int uDpi);
	unsigned int GetDpi();

	void AddFontSize();
	void DecFontSize();
	void SetFontSize(unsigned int uSize);
	unsigned int GetFontSize();

	void Sharp();
	void NoSharp();
	int IsSharp(){ return m_nSharp; };

	void Format();

	void Save();
	void SaveText();
	void Load();

	const char *GetFontFile();

	const unsigned char *GetUch(){ return m_uch; };
private:
	unsigned char m_uch[256];
	unsigned int m_uFontSize;
	unsigned int m_uDpi;
	char m_szFontFile[MAX_PATH];

	int m_nSharp;

	float m_fValue1;
	float m_fValue2;

	Font* m_pFont;
};

extern KFontAdjust g_FontAjust;


#endif