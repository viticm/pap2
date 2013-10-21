// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif		

#include "KGStdAfx.h"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <conio.h>
#include <WinCon.h>
#include <mbstring.h>
#endif

#ifdef __GNUC__
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <limits.h>
#endif

#ifdef WIN32
#include <conio.h>
#endif

#include "Ksdef.h"

#include <stdio.h>
#include <map>
#include <list>
#include <string>
#include <string.h>

#include "KGPublic.h"
#include "SO3GlobalDef.h"
#include "ProtocolBasic.h"
#include "Common/KG_Socket.h"
#include "Engine/KG_Time.h"
#include "Engine/KThread.h"
#include "Engine/KGLog.h"
#include "Engine/FileType.h"
#include "Engine/File.h"
#include "Engine/KSG_MD5_String.h"
#include "Engine/inoutmac.h"
#include "Engine/Random.h"


// TODO: 在此处引用程序需要的其他头文件
