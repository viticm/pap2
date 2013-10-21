////////////////////////////////////////////////////////////////////////////////
// ***************************************************************
//
//  Description: 
//  -------------------------------------------------------------
//	Author: Zeb
//  -------------------------------------------------------------
//	Create Date: 2006-04-25
//	-------------------------------------------------------------
//	History: 
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
//
// ***************************************************************
#pragma once
#ifndef _INCLUDE_KOUTPUTDEBUG_H_
#define _INCLUDE_KOUTPUTDEBUG_H_
#include "KGPublic.h"

#define WM_EDIT_RECEIVE_STRING WM_USER + 100
#define BUFFER_SIZE 4096

void g_OutputDebugInfo(const char* szInfo, size_t size = 0);
void g_OutputDebugInfoFmt(const char* szFormat, ...);

#define KGLOG_PROCESS_ERROR_CONSOLE(Condition) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                KGLOG_DEBUG,    \
                "KGLOG_PROCESS_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
            );                  \
            g_OutputDebugInfoFmt("ERROR: %s at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION);\
            goto Exit0;         \
        }                       \
    } while (false)

#define KGLOG_COM_PROCESS_ERROR_CONSOLE(Condition) \
    do  \
    {   \
        if (FAILED(Condition))  \
        {                       \
            KGLogPrintf(        \
                KGLOG_DEBUG,    \
                "KGLOG_COM_PROCESS_ERROR_CONSOLE(0x%X) at line %d in %s\n", Condition, __LINE__, KG_FUNCTION  \
            );                  \
            g_OutputDebugInfoFmt("ERROR: %d at line %d in %s\n", Condition, __LINE__, KG_FUNCTION);\
            goto Exit0;         \
        }                       \
    } while (false)

#endif //_INCLUDE_KOUTPUTDEBUG_H_
