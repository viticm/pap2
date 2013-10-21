//////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-6-8 13:24
//      File_base        : cfs_db_def
//      File_ext         : h
//      Author           : Cooler(liuyujun@263.net)
//      Description      : mysql database wrap global define
//
//      <Change_list>
//
//      Example:
//      {
//      Change_datetime  : year-month-day hour:minute
//      Change_by        : changed by who
//      Change_purpose   : change reason
//      }
//////////////////////////////////////////////////////////////////////////

#ifndef _CFS__DB___DEF____H_____
#define _CFS__DB___DEF____H_____

//////////////////////////////////////////////////////////////////////////
// Include region
#include "my_global.h"
#include "mysql.h"
#ifndef WIN32
#include <string.h>
#endif
//#include "utilities.h"

//////////////////////////////////////////////////////////////////////////
// cfs_db_def class define region
#ifndef         WIN32
#define BOOL    int
#endif

#define	DBDEFAULT_OPT_AUTORECNN			TRUE
#define	DBDEFAULT_OPT_CNNTIMEOUT		20		/* In seconds */
#define	DBDEFAULT_OPT_CHARSET			"binary"

#define	DBDEFAULT_CNN_HOSTNAME			"localhost"
#define DBDEFAULT_CNN_USER				NULL
#define DBDEFAULT_CNN_PASSWD			NULL
#define DBDEFAULT_CNN_DB				NULL

	// DB default create options
#define DBDEFAULT_CREATE_ENGINE			"MyISAM"
#define DBDEFAULT_CREATE_CHARSET		"binary"
#define DBDEFAULT_CREATE_COLLATE		""

	// Some limits define
#define DBMAXSIZE_SQLBUFF				(1024 * 2)
#define DBMAXSIZE_SQLBIGBUFF			(1024 * 200)

#define NAME_DBMODULE					"FS2DB"

#define DEFAULT_CFG_FILE				"sqlgen.cfg"
#define DEFAULT_PROCE_PATH				"procedure/"
#define MASK							1166787429
#define MAXLEN_BUF						1024 * 100

#define FILENAME_DBCNNINI				"lord.cfg"

//////////////////////////////////////////////////////////////////////////
// Macro define region

#define MAX_CFSDBHOSTNAME_SIZE		32
#define MAX_CFSDBLOGINUSER_SIZE		32
#define MAX_CFSDBLOGINPWD_SIZE		32
#define MAX_CFSDBNAME_SIZE			32

#define NAME_DBSECUREINI_SECTION		"DBSecure"
#define NAME_DBSECUREINI_KEYHOST		"HostName"
#define NAME_DBSECUREINI_KEYDBNAME		"DBName"
#define NAME_DBSECUREINI_KEYNAME		"LoginName"
#define NAME_DBSECUREINI_KEYPWD			"LoginPwd"
#define NAME_DBSECUREINI_KEYPORT		"Port"

#define PROCPAKFILE						"fs2proc.pak"

#define MAXPROC							1000
#define INT_STR_LEN						(64)

#define	MAXLEN_PROC_PASSBYPARAM			256

#define MIN_DB_ALLOC_SIZE				(1024 * 4)
//======================================================================


extern "C"
{
	void *zlib_compress_init(void);
	void zlib_compress_cleanup(void *);
	
	void *zlib_decompress_init(void);
	void zlib_decompress_cleanup(void *);
	
	int zlib_compress_block(void *, unsigned char *block, int len,
		unsigned char **outblock, int *outlen);
	int zlib_decompress_block(void *, unsigned char *block, int len,
		unsigned char **outblock, int *outlen);
}

//======================================================================

struct _ProcSQLHeader
{
	int nOffset;
	int nLen;
};

struct _ProcFileHeader 
{
	unsigned long ulMask;
	unsigned long ulVersion;
	unsigned long ulFileCount;
};

//======================================================================

#endif // _CFS__DB___DEF____H_____
