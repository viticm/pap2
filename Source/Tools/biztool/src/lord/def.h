//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 11:00
//      File_base        : sysinfo
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _SYSINFO_DEF_H_
#define _SYSINFO_DEF_H_

#define MAX_NAME		256
#define MAX_TEMPBUF		4096
#define MAXNETBUF		( 200 * 1024 )
#define EXESQLMAXVER	(1)
#define EXESQLMINVER	(1)

#ifndef NULL
#define TRUE 1
#define FALSE 0	
#define NULL 0
#define INVALID_VALUE (-1)
#define SUCCESS_VALUE (0)
#endif

typedef unsigned short ProtocolHead;

#define PH( Data ) (*((ProtocolHead*)( Data )))
#define PB( Data ) ((ProtocolHead*)( Data ))
#define WW( Data ) (*((ProtocolHead*)( Data )))

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>

#include "fseye_protocol.h"

#ifdef WIN32
#include "windows.h"
#include "time.h"
#include "io.h"
#include "direct.h"
#include "psapi.h"

#define EXPORT_API __declspec(dllexport)
#define INT64	__int64

#else

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <mntent.h>
#include <sys/vfs.h>
#include <sys/utsname.h>
#include <errno.h>

#define EXPORT_API
//#define INT64	long long

#endif

#endif
