#pragma once

int ReadIniFile();

int CheckVersion(int* pnExitFlag);

int GetEngineVersion(char *pszVersionBuffer, unsigned uBufferLen);

int GetFileMD5(char *pszOutputBuffer, unsigned uOutputBufferLen, const char cszFileName[]);

int SendSystemInfo();

int SendDumpFile(const char cszFileName[]);

int SendSampleFile(const char cszSampleFileName[]);

LONG WINAPI CreateDumpFile(_EXCEPTION_POINTERS *pExceptionInfo);
