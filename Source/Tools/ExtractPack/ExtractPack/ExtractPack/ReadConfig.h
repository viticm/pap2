#pragma once

#include <windows.h>

#define COFING_FINE_NAME "config.ini"

class ReadConfig
{
public:
	ReadConfig(void);
	~ReadConfig(void);

	bool Init();
	bool UnInit();

	bool ReadConfigFile(const char cszFileName[]);

	char *GetPakPath(){  return m_szPakPath; }
	char *GetExtractPath() { return m_szExtractPath; }

private:
	char m_szPakPath[MAX_PATH];
	char m_szExtractPath[MAX_PATH];
};

