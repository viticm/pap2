// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <tchar.h>

#define KG_PROCESS_ERROR(Condition) do { if (!(Condition)) goto Exit0; } while (false) 
#define KG_PROCESS_SUCCESS(Condition) do { if (Condition) goto Exit1; } while (false) 
#define snprintf _snprintf 

// TODO: reference additional headers your program requires here
