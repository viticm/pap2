#pragma once
//class RecPlayer;

#define ENABLE_OUTPUT_USED_RESOURCE 0 // normally used for benchmark

#if ENABLE_OUTPUT_USED_RESOURCE

#define g_OpenIniFile g_OpenIniFileShell
#define g_OpenTabFile g_OpenTabFileShell
#define g_OpenFile    g_OpenFileShell

IIniFile *g_OpenIniFileShell(const char* FileName, int ForceUnpakFile = false, int ForWrite = false);
ITabFile *g_OpenTabFileShell(const char* FileName, int ForceUnpakFile = false, int ForWrite = false);
IFile    *g_OpenFileShell(const char* FileName, int ForceUnpakFile = false, int ForWrite = false);

#endif
