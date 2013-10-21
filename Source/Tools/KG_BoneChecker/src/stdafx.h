// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif	

#include "KGStdAfx.h"

#pragma warning(disable:4127)

#include <stdio.h>
#include "KGPublic.h"
#include "Engine/KGLog.h"
#include "Engine/File.h"
#include "Engine/FileType.h"

#include <vector>
#include <string>

using namespace std;
