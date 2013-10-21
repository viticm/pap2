//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-03 11:00
//      File_base        : plugin
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _GMS_DEF_H_
#define _GMS_DEF_H_

#define MAX_DESC		500
#define MAX_NAME		256
#define MAX_PLUGCOUNT	50
#define MAX_TEMPBUF		1024
#define	MAXCLIENT		50
#define MAXNETBUF		( 200 * 1024 )
#define MAX_PING_TIME	( 10 )
#define MAX_ALIVE_TMIE	( 30 )
#define MAX_BUFFSIZE	( 16 * 1024 )
#define MAX_CONNERR		( 3 )
/*
*	config
*/
#define MAXHOSTNAME 100
#define MAX_IPSIZE	20
#define MAX_INTERFACE 8
#define MAX_MAC		8

#define OPENFILE		20
#define PLUGPATH		"./plug/"
#define PLUGEXT			"*.*"
#define FILEPATH		"./file/"
#define PROFILEEXT		".progress"
#define DCHAR			'\t'

#define OUTFILE			"/file/execmdout.txt"
#define INFILE			"/file/execmdin.txt"
#define EXEFILE			"/file/execmd"
#define EXEHEAD			"#!/bin/bash\n"
#define INVALID_VALUE (-1)

/*
#define NULL 0
#define FALSE (0)
#define TRUE (1)

*/

typedef unsigned short ProtocolHead;

#define PH( Data ) (*((ProtocolHead*)( Data )))
#define PB( Data ) ((ProtocolHead*)( Data ))

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>

#ifdef WIN32
#include "windows.h"
#include "time.h"
#include "io.h"
#include "direct.h"

#define EXPORT_API __declspec(dllexport)

inline int truncatef( FILE* file, long filelen )
{
	HANDLE handle = (HANDLE)_get_osfhandle(_fileno(file));
	if (fseek(file, filelen, SEEK_SET)!= 0)
		return -1;

	if (!SetEndOfFile(handle))
		return -1;

	return 0;
}

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
#include <errno.h>
#include <sys/wait.h>

inline int truncatef( FILE* file, long filelen )
{
	if( fseek( file, filelen, SEEK_SET ) != 0 )
		return -1;

	return ftruncate( fileno( file ), filelen );
}

#endif

#include "fseye_protocol.h"
#include "guardpluginterface.h"

#endif
