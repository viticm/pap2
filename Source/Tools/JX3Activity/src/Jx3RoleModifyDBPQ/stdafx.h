// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _STDAFX_
#define _STDAFX_  

#pragma warning(disable: 4996)
#pragma warning(disable: 4100)
#pragma warning(disable: 4127)
#pragma warning(disable: 4201)

#include "KGStdAfx.h"

#ifdef WIN32
//#include <winsock2.h>
#include <windows.h>
#endif

#ifdef __GNUC__
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <limits.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h> 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <conio.h>
#include <crtdbg.h>
#endif

#include "Engine/FileType.h"
#include "Engine/File.h"
#include "Engine/inoutmac.h"
#include "Engine/KThread.h"
#include "KGPublic.h"
#include "Engine/KGLog.h"
#endif  // _STDAFX_

// TODO: reference additional headers your program requires here
